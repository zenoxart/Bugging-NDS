#ifndef BUGSWARM_GLOBALS_H
#define BUGSWARM_GLOBALS_H

#include <nds.h>

// ---- screen ----
#define SCREEN_W 256
#define SCREEN_H 192

// ---- entity limits ----
#define MAX_BULLETS   12
#define MAX_BUGS      32
#define MAX_OBSTACLES 8
#define MAX_POWERUPS  3

// ---- OAM (main engine) layout ----
// One fixed OAM id per entity slot so we can update every id every frame
// (hidden when the slot is inactive) instead of tracking allocation.
#define OAM_PLAYER         0
#define OAM_BULLET_BASE    1
#define OAM_BUG_BASE       (OAM_BULLET_BASE + MAX_BULLETS)
#define OAM_OBSTACLE_BASE  (OAM_BUG_BASE + MAX_BUGS)
#define OAM_POWERUP_BASE   (OAM_OBSTACLE_BASE + MAX_OBSTACLES)

// top-screen HUD overlay (source/hud.c): heat gauge bottom-right, combo
// pips bottom-left, both above the player's row so they never sit under it
#define OAM_GAUGE_DIAL_MAIN   (OAM_POWERUP_BASE + MAX_POWERUPS)
#define OAM_GAUGE_NEEDLE_MAIN (OAM_GAUGE_DIAL_MAIN + 1)
#define OAM_COMBO_BASE        (OAM_GAUGE_NEEDLE_MAIN + 1) // ..+COMBO_MAX-1
#define MAIN_GAUGE_AFFINE_ID  0 // oamMain affine slot used by the needle
#define MAIN_GAUGE_X  (SCREEN_W - 32 - 4)
#define MAIN_GAUGE_Y  156
#define MAIN_COMBO_X  4
#define MAIN_COMBO_Y  156
#define MAIN_COMBO_STEP 10

// ---- player ----
#define PLAYER_Y            168
#define PLAYER_SIZE         16
#define PLAYER_DPAD_SPEED   3
#define PLAYER_HEAT_MAX     100
#define PLAYER_HEAT_PER_SHOT 16
#define PLAYER_HEAT_COOL     1
#define PLAYER_HEAT_RESUME   20   // heat must drop back below this after overheating
#define PLAYER_FIRE_COOLDOWN 8    // frames between shots
#define PLAYER_INVULN_FRAMES 90
#define PLAYER_MULTI_FRAMES  480  // MULTI powerup duration
#define PLAYER_FLAME_FRAMES  360  // flamethrower powerup duration
#define FLAME_FIRE_COOLDOWN  2    // frames between flame particles (rapid stream)
#define FLAME_HEAT_PER_SHOT  3    // heat added per flame particle (ignores rapid-fire/heat-resist upgrades)
#define FLAME_LIFETIME       16   // frames a flame particle travels before fizzling out (short range)
#define COMBO_MULTISHOT_BONUS 2   // free triple-shots granted when a combo first reaches COMBO_MAX
#define COMBO_MULTISHOT_CAP   6   // clamp so repeated max-combos can't stack forever

// ---- bullets ----
#define BULLET_SIZE   8
#define BULLET_SPEED  6

// ---- bugs ----
#define BUG_SIZE       16
#define BUG_MINI_SIZE  8
#define QUEEN_SIZE     32
#define BUG_PLAYFIELD_MIN_X 4
#define BUG_PLAYFIELD_MAX_X (SCREEN_W - BUG_SIZE - 4)
#define BUG_SPAWN_Y 20

// ---- obstacles ----
#define OBSTACLE_SIZE 16
#define OBSTACLE_HP   3
#define OBSTACLE_MIN_Y 60
#define OBSTACLE_MAX_Y 140
#define OBSTACLE_SHAKE_FRAMES 10 // how long a mushroom vibrates after being hit

// ---- powerups ----
#define POWERUP_SIZE  8
#define POWERUP_LIFETIME (60 * 6)
#define POWERUP_FALL_EVERY 3
#define FLAME_DROP_CHANCE 20 // 1-in-20 = 5% chance per (non-mini) Crawler kill

// ---- scoring ----
#define SCORE_CRAWLER   100
#define SCORE_MINI      250
#define SCORE_SPLITTER  150
#define SCORE_QUEEN     5000
#define SCORE_OBSTACLE  50
#define COMBO_MAX       5
#define COMBO_WINDOW    (60 * 2) // frames to keep a combo alive between kills

// ---- currency ("coins"), spent in the post-boss shop ----
#define COINS_CRAWLER   1
#define COINS_MINI      1
#define COINS_SPLITTER  2
#define COINS_OBSTACLE  1
#define COINS_QUEEN     50

// ---- shop upgrades (source/shop.c, source/player.c) ----
#define UPGRADE_MAX_LEVEL       5
#define UPGRADE_BASE_COST       15  // cost of the next level = UPGRADE_BASE_COST * (level + 1)
#define RAPID_FIRE_STEP         1   // frames shaved off the fire cooldown per level
#define HEAT_RESIST_STEP        2   // heat/shot removed per level
#define MIN_FIRE_COOLDOWN       2   // clamp so rapid fire can't reach 0
#define MIN_HEAT_PER_SHOT       4   // clamp so heat resist can't reach 0
#define EXTRA_LIFE_COST         25

// ---- sub-screen HUD widgets (oamSub) ----
// the heat gauge and combo pips live on the main screen now (see the
// OAM_GAUGE_*/OAM_COMBO_BASE block above) - only the position indicator,
// coin, hearts and multi-shot bar remain on the sub screen.
#define HUD_INDICATOR_Y      8 // top of the sub-screen, row 1
#define HUD_OAM_INDICATOR    0
#define HUD_OAM_COIN         1
#define HUD_MAX_HEARTS       6
#define HUD_OAM_HEART_BASE   2                          // 2..7
#define HUD_MULTI_SEGMENTS   8
#define HUD_OAM_MULTI_BASE   (HUD_OAM_HEART_BASE + HUD_MAX_HEARTS) // 8..15

// ---- shop screen widgets (oamSub) - reuses the same hardware slots as the
// HUD widgets above since only one of {hud, shop} is ever drawn at a time ----
#define SHOP_OAM_ICON_RAPID   0
#define SHOP_OAM_BTN_RAPID    1
#define SHOP_OAM_ICON_HEAT    2
#define SHOP_OAM_BTN_HEAT     3
#define SHOP_OAM_ICON_LIFE    4
#define SHOP_OAM_BTN_LIFE     5
#define SHOP_OAM_BTN_CONTINUE 6

#define SHOP_ICON_X   8
#define SHOP_BTN_X    190
#define SHOP_ROW_RAPID_Y 32
#define SHOP_ROW_HEAT_Y  56
#define SHOP_ROW_LIFE_Y  80
#define SHOP_CONTINUE_X  96
#define SHOP_CONTINUE_Y  112

// ---- difficulty phases (in frames, ~60fps) ----
#define PHASE1_END (15 * 60)
#define PHASE2_END (40 * 60)
#define PHASE3_END (70 * 60)
#define PHASE4_END (100 * 60)

typedef enum {
    PHASE_INVASION = 1,
    PHASE_SWARM,
    PHASE_PANIC,
    PHASE_OVERRUN,
    PHASE_QUEEN
} GamePhase;

static inline int clampi(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static inline bool aabbOverlap(int ax, int ay, int aw, int ah,
                                int bx, int by, int bw, int bh) {
    return ax < bx + bw && ax + aw > bx &&
           ay < by + bh && ay + ah > by;
}

#endif
