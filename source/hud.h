#ifndef BUGSWARM_HUD_H
#define BUGSWARM_HUD_H

#include "globals.h"

// Graphical HUD widgets drawn as hardware sprites, independent of the text
// HUD in game.c: sub-screen (position indicator, coin, hearts, multi-shot
// bar) plus a top-screen overlay (heat gauge, combo pips).

void hud_init(void); // call once at boot, after consoleDemoInit()

// Draws the sub-screen widgets from the current player state. Call every
// frame while STATE_PLAYING.
void hud_update_playing(void);

// Hides every sub-screen HUD sprite. Call every frame while NOT STATE_PLAYING.
void hud_hide(void);

// Draws the top-screen overlay (heat gauge bottom-right, combo pips
// bottom-left) into oamMain. Call during STATE_PLAYING's sprite pass,
// alongside player_render()/bullet_render()/etc, before oamUpdate(&oamMain).
void hud_render_overlay(void);

#endif
