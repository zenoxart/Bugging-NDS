#ifndef BUGSWARM_BACKGROUND_H
#define BUGSWARM_BACKGROUND_H

// Sets up BG3 on the main engine as a static 16-bit bitmap, initially
// showing the title art (tools/titlescreen.png). Call once at startup,
// after videoSetMode() has enabled DISPLAY_BG3_ACTIVE.
void background_init(void);

// Swaps BG3's pixels to the plain forest-floor art (tools/background_preview.png,
// via tools/import_background.py). Call once when leaving STATE_TITLE or
// STATE_GAMEOVER.
void background_show_forest(void);

// Swaps BG3's pixels to the game-over art (tools/gameoverscreen.png, via
// tools/import_gameover.py). Call once when entering STATE_GAMEOVER.
void background_show_gameover(void);

// Swaps BG3's pixels to the title art (tools/titlescreen.png, via
// tools/import_titlescreen.py). Call once when entering/returning to
// STATE_TITLE.
void background_show_title(void);

#endif
