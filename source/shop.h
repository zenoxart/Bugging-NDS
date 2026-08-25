#ifndef BUGSWARM_SHOP_H
#define BUGSWARM_SHOP_H

#include "globals.h"

// Post-boss upgrade shop (sub-screen). Entered once per cycle right after
// the Queen is defeated; gameplay is paused while it's open.

void shop_init(void);         // call once at boot: allocates icon/button sprite VRAM
void shop_begin(void);        // call once when entering STATE_SHOP
bool shop_update(void);       // returns true once "Continue" is selected
void shop_render(void);       // text (title, coins, level/cost)
void shop_draw_sprites(void); // icons + tappable buttons (oamSub)

#endif
