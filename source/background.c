#include "background.h"
#include "globals.h"
#include "forest_floor_bg_bin.h" // generated at build time by bin2s from data/forest_floor.bg.bin
#include "gameover_bg_bin.h"     // generated at build time by bin2s from data/gameover.bg.bin
#include "titlescreen_bg_bin.h"  // generated at build time by bin2s from data/titlescreen.bg.bin

static int bgId;

void background_init(void) {
    vramSetBankB(VRAM_B_MAIN_BG_0x06000000);

    bgId = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    bgSetPriority(bgId, 3); // behind every sprite (which use priorities 0-2)

    background_show_title();
}

// the bitmap background is 256x256; the art only covers the visible
// 256x192 rows (the top screen's actual height), which lines up with the
// start of the buffer, so a straight copy is enough.
void background_show_forest(void) {
    dmaCopy(forest_floor_bg_bin, bgGetGfxPtr(bgId), forest_floor_bg_bin_size);
}

void background_show_gameover(void) {
    dmaCopy(gameover_bg_bin, bgGetGfxPtr(bgId), gameover_bg_bin_size);
}

void background_show_title(void) {
    dmaCopy(titlescreen_bg_bin, bgGetGfxPtr(bgId), titlescreen_bg_bin_size);
}
