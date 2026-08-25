#include <string.h>
#include "hud.h"
#include "hud_sprites_data.h"
#include "sprites.h" // gfxPowerup, reused as the combo pip icon
#include "player.h"
#include "game.h"

#define COL(r, g, b) ARGB16(1, (r), (g), (b))

// --- sub-screen (oamSub): position indicator, coin, hearts, multi-shot bar ---
static u16 *gfxIndicator;
static u16 *gfxCoin;
static u16 *gfxHeart;
static u16 *gfxMultiSeg;

// --- top-screen overlay (oamMain): heat gauge dial/needle, combo pips ---
static u16 *gfxGaugeDialMain;
static u16 *gfxGaugeNeedleMain;

static inline void putpx(u16 *buf, int w, int x, int y, u16 col) {
    if (x < 0 || y < 0) return;
    buf[y * w + x] = col;
}

static void fillRect(u16 *buf, int w, int x0, int y0, int rw, int rh, u16 col) {
    for (int y = y0; y < y0 + rh; y++)
        for (int x = x0; x < x0 + rw; x++)
            putpx(buf, w, x, y, col);
}

// a small downward-pointing pin/marker, 8x8
static void drawIndicator(u16 *buf) {
    static const int halfw[8] = {3, 3, 3, 3, 2, 2, 1, 0};
    for (int y = 0; y < 8; y++)
        for (int x = 4 - halfw[y]; x <= 4 + halfw[y]; x++)
            putpx(buf, 8, x, y, COL(31, 31, 31));
    putpx(buf, 8, 3, 1, COL(10, 28, 31));
    putpx(buf, 8, 4, 1, COL(10, 28, 31));
}

// a bordered 8x8 gauge segment block (used by the multi-shot bar)
static void drawSegment(u16 *buf, u16 fill, u16 border) {
    fillRect(buf, 8, 0, 0, 8, 8, border);
    fillRect(buf, 8, 1, 1, 6, 6, fill);
}

void hud_init(void) {
    vramSetBankD(VRAM_D_SUB_SPRITE);
    oamInit(&oamSub, SpriteMapping_Bmp_1D_128, false);

    gfxIndicator = oamAllocateGfx(&oamSub, SpriteSize_8x8, SpriteColorFormat_Bmp);
    gfxCoin      = oamAllocateGfx(&oamSub, SpriteSize_8x8, SpriteColorFormat_Bmp);
    gfxHeart     = oamAllocateGfx(&oamSub, SpriteSize_8x8, SpriteColorFormat_Bmp);
    gfxMultiSeg  = oamAllocateGfx(&oamSub, SpriteSize_8x8, SpriteColorFormat_Bmp);

    drawIndicator(gfxIndicator);
    drawSegment(gfxMultiSeg, COL(8, 27, 29), COL(3, 12, 13));

    memcpy(gfxCoin, SPR_COIN, sizeof(SPR_COIN));
    memcpy(gfxHeart, SPR_HEART, sizeof(SPR_HEART));

    // the heat gauge art is generated once (tools/gen_hud_sprites.py) but
    // gets its own VRAM allocation here on the MAIN engine, since oamMain
    // and oamSub each manage their own separate graphics memory.
    gfxGaugeDialMain   = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_Bmp);
    gfxGaugeNeedleMain = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_Bmp);
    memcpy(gfxGaugeDialMain, SPR_GAUGE_DIAL, sizeof(SPR_GAUGE_DIAL));
    memcpy(gfxGaugeNeedleMain, SPR_GAUGE_NEEDLE, sizeof(SPR_GAUGE_NEEDLE));
}

void hud_hide(void) {
    oamClear(&oamSub, 0, 128);
    oamUpdate(&oamSub);
}

void hud_update_playing(void) {
    // --- cannon position indicator: tracks player.x at full pixel precision ---
    int ix = player.x + PLAYER_SIZE / 2 - 4;
    oamSet(&oamSub, HUD_OAM_INDICATOR, ix, HUD_INDICATOR_Y, 0, 15, SpriteSize_8x8, SpriteColorFormat_Bmp,
           gfxIndicator, -1, false, false, false, false, false);

    // --- coin icon, next to the COINS text ---
    oamSet(&oamSub, HUD_OAM_COIN, 0, 16, 0, 15, SpriteSize_8x8, SpriteColorFormat_Bmp,
           gfxCoin, -1, false, false, false, false, false);

    // --- hearts for lives (capped; game.c prints a "+N" overflow next to them) ---
    int hearts = clampi(player.lives, 0, HUD_MAX_HEARTS);
    for (int i = 0; i < HUD_MAX_HEARTS; i++) {
        int id = HUD_OAM_HEART_BASE + i;
        oamSet(&oamSub, id, i * 9, 24, 0, 15, SpriteSize_8x8, SpriteColorFormat_Bmp,
               gfxHeart, -1, false, i >= hearts, false, false, false);
    }

    // --- multi-shot bar: depletes as the power-up runs out ---
    int multiFilled = clampi(
        (player.multiTimer * HUD_MULTI_SEGMENTS + PLAYER_MULTI_FRAMES - 1) / PLAYER_MULTI_FRAMES,
        0, HUD_MULTI_SEGMENTS);
    for (int i = 0; i < HUD_MULTI_SEGMENTS; i++) {
        int id = HUD_OAM_MULTI_BASE + i;
        bool lit = player.multiTimer > 0 && i < multiFilled;
        oamSet(&oamSub, id, 56 + i * 8, 72, 0, 15, SpriteSize_8x8, SpriteColorFormat_Bmp,
               gfxMultiSeg, -1, false, !lit, false, false, false);
    }

    oamUpdate(&oamSub);
}

void hud_render_overlay(void) {
    // --- heat gauge: analog dial (static) + a needle rotated with the DS's
    //     affine sprite hardware - top screen, bottom-right ---
    oamSet(&oamMain, OAM_GAUGE_DIAL_MAIN, MAIN_GAUGE_X, MAIN_GAUGE_Y, 0, 15, SpriteSize_32x32,
           SpriteColorFormat_Bmp, gfxGaugeDialMain, -1, false, false, false, false, false);

    // libnds' full circle is DEGREES_IN_CIRCLE = 32768 (NOT 65536), so a
    // 180-degree sweep is -16384, not -32768.
    int heat = clampi(player.heat, 0, PLAYER_HEAT_MAX);
    int angle = -(heat * degreesToAngle(180)) / PLAYER_HEAT_MAX; // 0 = points left, -16384 = points right
    oamSet(&oamMain, OAM_GAUGE_NEEDLE_MAIN, MAIN_GAUGE_X, MAIN_GAUGE_Y, 0, 15, SpriteSize_16x16,
           SpriteColorFormat_Bmp, gfxGaugeNeedleMain, MAIN_GAUGE_AFFINE_ID, true, false, false, false, false);
    oamRotateScale(&oamMain, MAIN_GAUGE_AFFINE_ID, angle, 1 << 8, 1 << 8);

    // --- combo pips: top screen, bottom-left, one lit per combo level ---
    for (int i = 0; i < COMBO_MAX; i++) {
        int id = OAM_COMBO_BASE + i;
        oamSet(&oamMain, id, MAIN_COMBO_X + i * MAIN_COMBO_STEP, MAIN_COMBO_Y, 0, 15,
               SpriteSize_8x8, SpriteColorFormat_Bmp, gfxPowerup, -1, false,
               i >= game.combo, false, false, false);
    }
}
