// Bug Swarm - a small Centipede/Galaxian-inspired fixed shooter for the
// Nintendo DS, built on libnds. See README.md for controls and the roadmap.

#include <nds.h>
#include "globals.h"
#include "background.h"
#include "sprites.h"
#include "hud.h"
#include "shop.h"
#include "game.h"

int main(void) {
    // top screen: BG3 (forest-floor bitmap) behind hardware sprites (the playfield)
    videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D | DISPLAY_SPR_1D_BMP);
    // bottom screen: text console (HUD) with graphical gauge sprites (source/hud.c) on top
    videoSetModeSub(MODE_0_2D | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D | DISPLAY_SPR_1D_BMP);
    consoleDemoInit();

    background_init();
    sprites_init();
    hud_init();
    shop_init();
    game_init();

    while (pmMainLoop()) {
        scanKeys();

        game_update();
        game_render();

        swiWaitForVBlank();
    }

    return 0;
}
