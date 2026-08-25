#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "game.h"
#include "player.h"
#include "bullet.h"
#include "bug.h"
#include "obstacle.h"
#include "powerup.h"
#include "leaderboard.h"
#include "keyboard.h"
#include "hud.h"
#include "shop.h"
#include "background.h"

Game game;

int rng(int maxExclusive) {
    if (maxExclusive <= 0) return 0;
    return rand() % maxExclusive;
}

GamePhase game_phase(void) {
    if (game.phaseTimer < PHASE1_END) return PHASE_INVASION;
    if (game.phaseTimer < PHASE2_END) return PHASE_SWARM;
    if (game.phaseTimer < PHASE3_END) return PHASE_PANIC;
    if (game.phaseTimer < PHASE4_END) return PHASE_OVERRUN;
    return PHASE_QUEEN;
}

static int spawnIntervalFor(GamePhase phase, int cycle) {
    return clampi(100 - (int)phase * 14 - cycle * 10, 20, 100);
}

void game_add_score(unsigned int base) {
    game.score += base * (unsigned int)game.combo;
    if (game.combo < COMBO_MAX) {
        game.combo++;
        if (game.combo == COMBO_MAX) player_grant_combo_multishot();
    }
    game.comboTimer = COMBO_WINDOW;
}

void game_add_currency(unsigned int amount) {
    game.currency += amount;
}

void game_break_combo(void) {
    game.combo = 1;
    game.comboTimer = 0;
}

void game_on_player_hit(void) {
    game_break_combo();
}

void game_on_queen_killed(void) {
    game.cycle++;
    game.phaseTimer = 0;
    game.spawnTimer = 0;
    obstacle_spawn_wave(2);
    shop_begin();
    game.state = STATE_SHOP;
}

void game_init(void) {
    game.state = STATE_TITLE;
    game.score = 0;
    game.highScore = 0;
    game.currency = 0;
    game.titleSeedTicker = 0;
    game.lastRank = -1;
    player_init();
    bullet_init();
    bug_init();
    obstacle_init();
    powerup_init();
    leaderboard_init();
}

void game_reset(void) {
    srand(game.titleSeedTicker + 1);
    game.score = 0;
    game.currency = 0;
    game.combo = 1;
    game.comboTimer = 0;
    game.phaseTimer = 0;
    game.cycle = 0;
    game.spawnTimer = 60;
    player_init();
    bullet_init();
    bug_init();
    obstacle_init();
    powerup_init();
    obstacle_spawn_wave(4);
    background_show_forest();
    game.state = STATE_PLAYING;
}

static void resolveCollisions(void) {
    // bullets vs bugs
    for (int bi = 0; bi < MAX_BULLETS; bi++) {
        if (!bullets[bi].active) continue;
        for (int gi = 0; gi < MAX_BUGS; gi++) {
            Bug *g = &bugs[gi];
            if (!g->active) continue;
            int size = bug_size(g);
            if (!aabbOverlap(bullets[bi].x, bullets[bi].y, BULLET_SIZE, BULLET_SIZE,
                              g->x, g->y, size, size))
                continue;
            bullets[bi].active = false;
            g->hp--;
            if (g->hp <= 0) bug_kill(gi);
            break;
        }
    }

    // remaining bullets vs obstacles (a bullet that already hit a bug this frame is gone)
    for (int bi = 0; bi < MAX_BULLETS; bi++) {
        if (!bullets[bi].active) continue;
        if (obstacle_hit(bullets[bi].x, bullets[bi].y, BULLET_SIZE, BULLET_SIZE))
            bullets[bi].active = false;
    }

    // bugs vs player
    for (int gi = 0; gi < MAX_BUGS; gi++) {
        Bug *g = &bugs[gi];
        if (!g->active) continue;
        int size = bug_size(g);
        if (!aabbOverlap(g->x, g->y, size, size, player.x, PLAYER_Y, PLAYER_SIZE, PLAYER_SIZE))
            continue;
        player_hit();
        if (g->type == BUG_QUEEN) queenActive = false;
        g->active = false;
    }
}

void game_update(void) {
    switch (game.state) {
        case STATE_TITLE: {
            game.titleSeedTicker++;
            int down = keysDown();
            if (down & KEY_START) game_reset();
            if (down & KEY_SELECT) {
                game.lastRank = -1;
                background_show_forest();
                game.state = STATE_LEADERBOARD;
            }
            break;
        }

        case STATE_PLAYING: {
            game.phaseTimer++;
            if (game.comboTimer > 0) {
                game.comboTimer--;
                if (game.comboTimer == 0) game.combo = 1;
            }

            GamePhase phase = game_phase();
            if (phase == PHASE_QUEEN) {
                if (!queenActive) bug_spawn(BUG_QUEEN, phase, game.cycle);
            } else {
                game.spawnTimer--;
                if (game.spawnTimer <= 0) {
                    BugType t = (rng(4) == 0) ? BUG_SPLITTER : BUG_CRAWLER;
                    bug_spawn(t, phase, game.cycle);
                    game.spawnTimer = spawnIntervalFor(phase, game.cycle);
                }
            }

            if (game.phaseTimer == PHASE1_END || game.phaseTimer == PHASE2_END ||
                game.phaseTimer == PHASE3_END || game.phaseTimer == PHASE4_END) {
                obstacle_spawn_wave(2);
            }

            player_update();
            bullet_update();
            bug_update();
            obstacle_update();
            powerup_update();
            resolveCollisions();

            if (player.lives <= 0) {
                if (game.score > game.highScore) game.highScore = game.score;
                background_show_gameover();
                game.state = STATE_GAMEOVER;
            }
            break;
        }

        case STATE_GAMEOVER:
            if (keysDown() & KEY_START) {
                background_show_forest();
                if (leaderboard_qualifies(game.score)) {
                    keyboard_begin(game.score);
                    game.state = STATE_ENTER_INITIALS;
                } else {
                    game.lastRank = -1;
                    game.state = STATE_LEADERBOARD;
                }
            }
            break;

        case STATE_ENTER_INITIALS:
            if (keyboard_update()) {
                game.lastRank = keyboard_last_rank();
                game.state = STATE_LEADERBOARD;
            }
            break;

        case STATE_LEADERBOARD:
            if (keysDown() & KEY_START) {
                game.titleSeedTicker = 0;
                background_show_title();
                game.state = STATE_TITLE;
            }
            break;

        case STATE_SHOP:
            if (shop_update()) game.state = STATE_PLAYING;
            break;
    }
}

// prints one line horizontally centered on the 32-column sub-screen console
static void printCentered(int row, const char *fmt, ...) {
    char buf[33];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    int col = (32 - (int)strlen(buf)) / 2;
    if (col < 0) col = 0;
    iprintf("\x1b[%d;%dH%s\n", row, col, buf);
}

static void renderHud(void) {
    consoleClear();
    switch (game.state) {
        case STATE_TITLE:
            printCentered(9, "Touch or Pad: move");
            printCentered(10, "A: fire (hold = rapid)");
            printCentered(11, "Watch the heat gauge!");
            printCentered(16, "Press START");
            printCentered(17, "SELECT: Leaderboard");
            if (game.highScore > 0)
                printCentered(19, "High Score: %06lu", (unsigned long)game.highScore);
            break;

        case STATE_PLAYING: {
            // row 1: cannon-position track/indicator (hud.c draws the
            // sprite at y=8) - the heat gauge moved to the top screen, so
            // the track no longer needs to dodge it and can use the full width.
            char track[33];
            for (int i = 0; i < 32; i++) track[i] = '.';
            track[32] = '\0';
            iprintf("\x1b[1;0H%s\n", track);

            iprintf("\x1b[0;0HSCORE %06lu\n", (unsigned long)game.score);

            // coin icon (hud.c) sits at x=0,y=16 (row 2), right against the count
            iprintf("\x1b[2;1H%04lu\n", (unsigned long)game.currency);

            // heart icons (hud.c) sit at x=0..53,y=24 (row 3); "+N" covers any
            // lives beyond HUD_MAX_HEARTS
            if (player.lives > HUD_MAX_HEARTS)
                iprintf("\x1b[3;8H+%d\n", player.lives - HUD_MAX_HEARTS);
            else
                iprintf("\x1b[3;8H  \n");

            // the multi-shot bar (hud.c) is at x=56..120,y=72 (row 9); status
            // text starts at column 18, clear of the bar. The heat gauge and
            // combo counter are now sprites on the top screen (source/hud.c).
            iprintf("\x1b[9;0HMULTI\n");
            iprintf("\x1b[9;18H%s\n", player.multiTimer > 0 ? "ON" : "  ");
            break;
        }

        case STATE_GAMEOVER:
            printCentered(8, "GAME OVER");
            printCentered(10, "Score: %06lu", (unsigned long)game.score);
            printCentered(11, "High:  %06lu", (unsigned long)game.highScore);
            if (leaderboard_qualifies(game.score))
                printCentered(13, "NEW HIGH SCORE!");
            printCentered(15, "Press START");
            break;

        case STATE_ENTER_INITIALS:
            keyboard_render();
            break;

        case STATE_LEADERBOARD:
            leaderboard_render(game.lastRank);
            break;

        case STATE_SHOP:
            shop_render();
            break;
    }
}

void game_render(void) {
    if (game.state == STATE_PLAYING) {
        player_render();
        bullet_render();
        bug_render();
        obstacle_render();
        powerup_render();
        hud_render_overlay();
        hud_update_playing();
    } else if (game.state == STATE_SHOP) {
        oamClear(&oamMain, 0, 128);
        shop_draw_sprites();
    } else {
        // keep every OAM slot hidden while off the play-state
        oamClear(&oamMain, 0, 128);
        hud_hide();
    }
    oamUpdate(&oamMain);
    renderHud();
}
