#include <stdio.h>
#include <string.h>
#include "shop.h"
#include "shop_sprites_data.h"
#include "game.h"
#include "player.h"

#define ROW_RAPID    0
#define ROW_HEAT     1
#define ROW_LIFE     2
#define ROW_CONTINUE 3
#define SHOP_ROW_COUNT 4

static u16 *gfxIconRapid;
static u16 *gfxIconHeat;
static u16 *gfxIconLife;
static u16 *gfxBtnBuy;
static u16 *gfxBtnMax;
static u16 *gfxBtnContinue;

static int cursor;

void shop_init(void) {
    // oamSub/VRAM bank D are already initialized by hud_init(); this just
    // claims more graphics slots in the same allocator.
    gfxIconRapid   = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_Bmp);
    gfxIconHeat    = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_Bmp);
    gfxIconLife    = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_Bmp);
    gfxBtnBuy      = oamAllocateGfx(&oamSub, SpriteSize_32x16, SpriteColorFormat_Bmp);
    gfxBtnMax      = oamAllocateGfx(&oamSub, SpriteSize_32x16, SpriteColorFormat_Bmp);
    gfxBtnContinue = oamAllocateGfx(&oamSub, SpriteSize_64x32, SpriteColorFormat_Bmp);

    memcpy(gfxIconRapid, SPR_ICON_RAPID, sizeof(SPR_ICON_RAPID));
    memcpy(gfxIconHeat, SPR_ICON_HEAT, sizeof(SPR_ICON_HEAT));
    memcpy(gfxIconLife, SPR_ICON_LIFE, sizeof(SPR_ICON_LIFE));
    memcpy(gfxBtnBuy, SPR_BTN_BUY, sizeof(SPR_BTN_BUY));
    memcpy(gfxBtnMax, SPR_BTN_MAX, sizeof(SPR_BTN_MAX));
    memcpy(gfxBtnContinue, SPR_BTN_CONTINUE, sizeof(SPR_BTN_CONTINUE));
}

void shop_begin(void) {
    cursor = 0;
}

static int rapidCost(void) { return UPGRADE_BASE_COST * (player.rapidFireLevel + 1); }
static int heatCost(void) { return UPGRADE_BASE_COST * (player.heatResistLevel + 1); }

static bool buyRapid(void) {
    if (player.rapidFireLevel >= UPGRADE_MAX_LEVEL) return false;
    int cost = rapidCost();
    if (game.currency < (unsigned int)cost) return false;
    game.currency -= (unsigned int)cost;
    player.rapidFireLevel++;
    return true;
}

static bool buyHeat(void) {
    if (player.heatResistLevel >= UPGRADE_MAX_LEVEL) return false;
    int cost = heatCost();
    if (game.currency < (unsigned int)cost) return false;
    game.currency -= (unsigned int)cost;
    player.heatResistLevel++;
    return true;
}

static bool buyLife(void) {
    if (game.currency < (unsigned int)EXTRA_LIFE_COST) return false;
    game.currency -= (unsigned int)EXTRA_LIFE_COST;
    player.lives++;
    player.extraLivesBought++;
    return true;
}

static void activate(int row) {
    switch (row) {
        case ROW_RAPID: buyRapid(); break;
        case ROW_HEAT:  buyHeat(); break;
        case ROW_LIFE:  buyLife(); break;
        default: break; // ROW_CONTINUE is handled by the caller
    }
}

// button hitboxes, in the same order as the ROW_* constants
static bool hitTestButton(int px, int py, int *outRow) {
    if (aabbOverlap(px, py, 1, 1, SHOP_BTN_X, SHOP_ROW_RAPID_Y, 32, 16)) { *outRow = ROW_RAPID; return true; }
    if (aabbOverlap(px, py, 1, 1, SHOP_BTN_X, SHOP_ROW_HEAT_Y, 32, 16))  { *outRow = ROW_HEAT;  return true; }
    if (aabbOverlap(px, py, 1, 1, SHOP_BTN_X, SHOP_ROW_LIFE_Y, 32, 16))  { *outRow = ROW_LIFE;  return true; }
    if (aabbOverlap(px, py, 1, 1, SHOP_CONTINUE_X, SHOP_CONTINUE_Y, 64, 32)) { *outRow = ROW_CONTINUE; return true; }
    return false;
}

bool shop_update(void) {
    touchPosition touch;
    touchRead(&touch);
    int down = keysDown();

    if (down & KEY_TOUCH) {
        int r;
        if (hitTestButton(touch.px, touch.py, &r)) {
            cursor = r;
            if (r == ROW_CONTINUE) return true;
            activate(r);
        }
    }
    if (down & KEY_DOWN) cursor = (cursor + 1) % SHOP_ROW_COUNT;
    if (down & KEY_UP) cursor = (cursor + SHOP_ROW_COUNT - 1) % SHOP_ROW_COUNT;
    if (down & (KEY_A | KEY_START)) {
        if (cursor == ROW_CONTINUE) return true;
        activate(cursor);
    }
    return false;
}

static void printCostOrMax(int row, int textRow, int level, int cost) {
    char mark = (cursor == row) ? '>' : ' ';
    if (level >= UPGRADE_MAX_LEVEL)
        iprintf("\x1b[%d;4H%c Lv%d/%d  MAX\n", textRow, mark, level, UPGRADE_MAX_LEVEL);
    else
        iprintf("\x1b[%d;4H%c Lv%d/%d  %dc\n", textRow, mark, level, UPGRADE_MAX_LEVEL, cost);
}

void shop_render(void) {
    iprintf("\x1b[1;5HBOSS DEFEATED!\n");
    iprintf("\x1b[2;7HCOINS: %04lu\n", (unsigned long)game.currency);

    // icon + button sprites are drawn by shop_draw_sprites(); this text
    // sits between them (x=32..176), clear of both.
    iprintf("\x1b[4;4HRAPID FIRE\n");
    printCostOrMax(ROW_RAPID, 5, player.rapidFireLevel, rapidCost());

    iprintf("\x1b[7;4HHEAT RESIST\n");
    printCostOrMax(ROW_HEAT, 8, player.heatResistLevel, heatCost());

    char mark = (cursor == ROW_LIFE) ? '>' : ' ';
    iprintf("\x1b[10;4HEXTRA LIFE\n");
    iprintf("\x1b[11;4H%c x%d      %dc\n", mark, player.extraLivesBought, EXTRA_LIFE_COST);

    // the continue button (oamSub) covers rows 14-17 (y=112..144); this
    // label sits a full row above it (row 12 ends at y=104) so it's never
    // under the sprite.
    iprintf("\x1b[12;11H%sCONTINUE%s\n", cursor == ROW_CONTINUE ? "[" : " ", cursor == ROW_CONTINUE ? "]" : " ");
    iprintf("\x1b[19;1HTap an icon's button, or Pad+A\n");
}

void shop_draw_sprites(void) {
    oamClear(&oamSub, 0, 128); // hud.c owns these same slot numbers while playing

    oamSet(&oamSub, SHOP_OAM_ICON_RAPID, SHOP_ICON_X, SHOP_ROW_RAPID_Y, 0, 15, SpriteSize_16x16,
           SpriteColorFormat_Bmp, gfxIconRapid, -1, false, false, false, false, false);
    u16 *btnRapid = (player.rapidFireLevel >= UPGRADE_MAX_LEVEL) ? gfxBtnMax : gfxBtnBuy;
    oamSet(&oamSub, SHOP_OAM_BTN_RAPID, SHOP_BTN_X, SHOP_ROW_RAPID_Y, 0, 15, SpriteSize_32x16,
           SpriteColorFormat_Bmp, btnRapid, -1, false, false, false, false, false);

    oamSet(&oamSub, SHOP_OAM_ICON_HEAT, SHOP_ICON_X, SHOP_ROW_HEAT_Y, 0, 15, SpriteSize_16x16,
           SpriteColorFormat_Bmp, gfxIconHeat, -1, false, false, false, false, false);
    u16 *btnHeat = (player.heatResistLevel >= UPGRADE_MAX_LEVEL) ? gfxBtnMax : gfxBtnBuy;
    oamSet(&oamSub, SHOP_OAM_BTN_HEAT, SHOP_BTN_X, SHOP_ROW_HEAT_Y, 0, 15, SpriteSize_32x16,
           SpriteColorFormat_Bmp, btnHeat, -1, false, false, false, false, false);

    oamSet(&oamSub, SHOP_OAM_ICON_LIFE, SHOP_ICON_X, SHOP_ROW_LIFE_Y, 0, 15, SpriteSize_16x16,
           SpriteColorFormat_Bmp, gfxIconLife, -1, false, false, false, false, false);
    oamSet(&oamSub, SHOP_OAM_BTN_LIFE, SHOP_BTN_X, SHOP_ROW_LIFE_Y, 0, 15, SpriteSize_32x16,
           SpriteColorFormat_Bmp, gfxBtnBuy, -1, false, false, false, false, false);

    oamSet(&oamSub, SHOP_OAM_BTN_CONTINUE, SHOP_CONTINUE_X, SHOP_CONTINUE_Y, 0, 15, SpriteSize_64x32,
           SpriteColorFormat_Bmp, gfxBtnContinue, -1, false, false, false, false, false);

    oamUpdate(&oamSub);
}
