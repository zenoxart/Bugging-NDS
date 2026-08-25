#ifndef BUGSWARM_SPRITES_H
#define BUGSWARM_SPRITES_H

#include "globals.h"

// Direct-bitmap (RGB15 + alpha) sprite graphics, generated procedurally at
// startup so the project needs no external art pipeline (grit/png).
extern u16 *gfxPlayer;      // 16x16
extern u16 *gfxBullet;      // 8x8
extern u16 *gfxCrawler;     // 16x16
extern u16 *gfxSplitter;    // 16x16
extern u16 *gfxMini;        // 8x8
extern u16 *gfxQueen;       // 32x32
extern u16 *gfxObstacle;    // 16x16
extern u16 *gfxPowerup;     // 8x8
extern u16 *gfxFlameBullet;  // 8x8 - flamethrower particle
extern u16 *gfxPowerupFlame; // 8x8 - flamethrower pickup

void sprites_init(void);

#endif
