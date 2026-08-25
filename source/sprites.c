#include <string.h>
#include "sprites.h"
#include "spritesheet_data.h"
#include "weapon_sprites_data.h"

u16 *gfxPlayer;
u16 *gfxBullet;
u16 *gfxCrawler;
u16 *gfxSplitter;
u16 *gfxMini;
u16 *gfxQueen;
u16 *gfxObstacle;
u16 *gfxPowerup;
u16 *gfxFlameBullet;
u16 *gfxPowerupFlame;

void sprites_init(void) {
    vramSetBankA(VRAM_A_MAIN_SPRITE);
    oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);

    gfxPlayer   = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_Bmp);
    gfxBullet   = oamAllocateGfx(&oamMain, SpriteSize_8x8,   SpriteColorFormat_Bmp);
    gfxCrawler  = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_Bmp);
    gfxSplitter = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_Bmp);
    gfxMini     = oamAllocateGfx(&oamMain, SpriteSize_8x8,   SpriteColorFormat_Bmp);
    gfxQueen    = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_Bmp);
    gfxObstacle = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_Bmp);
    gfxPowerup  = oamAllocateGfx(&oamMain, SpriteSize_8x8,   SpriteColorFormat_Bmp);
    gfxFlameBullet  = oamAllocateGfx(&oamMain, SpriteSize_8x8, SpriteColorFormat_Bmp);
    gfxPowerupFlame = oamAllocateGfx(&oamMain, SpriteSize_8x8, SpriteColorFormat_Bmp);

    // pixel art baked in by tools/gen_sprites.py (see tools/spritesheet_preview.png)
    memcpy(gfxPlayer,   SPR_PLAYER,   sizeof(SPR_PLAYER));
    memcpy(gfxBullet,   SPR_BULLET,   sizeof(SPR_BULLET));
    memcpy(gfxCrawler,  SPR_CRAWLER,  sizeof(SPR_CRAWLER));
    memcpy(gfxSplitter, SPR_SPLITTER, sizeof(SPR_SPLITTER));
    memcpy(gfxMini,     SPR_MINI,     sizeof(SPR_MINI));
    memcpy(gfxQueen,    SPR_QUEEN,    sizeof(SPR_QUEEN));
    memcpy(gfxObstacle, SPR_OBSTACLE, sizeof(SPR_OBSTACLE));
    memcpy(gfxPowerup,  SPR_POWERUP,  sizeof(SPR_POWERUP));

    // flamethrower weapon art (tools/gen_weapon_sprites.py)
    memcpy(gfxFlameBullet,  SPR_FLAME_BULLET,   sizeof(SPR_FLAME_BULLET));
    memcpy(gfxPowerupFlame, SPR_POWERUP_FLAME,  sizeof(SPR_POWERUP_FLAME));
}
