#include "bug.h"
#include "sprites.h"
#include "obstacle.h"
#include "powerup.h"
#include "game.h"

Bug bugs[MAX_BUGS];
bool queenActive = false;

void bug_init(void) {
    for (int i = 0; i < MAX_BUGS; i++) bugs[i].active = false;
    queenActive = false;
}

int bug_size(Bug *b) {
    if (b->type == BUG_QUEEN) return QUEEN_SIZE;
    if (b->mini) return BUG_MINI_SIZE;
    return BUG_SIZE;
}

static int findFreeSlot(void) {
    for (int i = 0; i < MAX_BUGS; i++)
        if (!bugs[i].active) return i;
    return -1;
}

void bug_spawn(BugType type, int phase, int cycle) {
    int i = findFreeSlot();
    if (i < 0) return;

    Bug *b = &bugs[i];
    b->active = true;
    b->type = type;
    b->mini = false;
    b->dir = rng(2) ? 1 : -1;
    b->moveTimer = 0;
    b->descendTimer = 0;

    int speedBoost = phase + cycle;

    switch (type) {
        case BUG_CRAWLER:
            b->x = BUG_PLAYFIELD_MIN_X + rng(BUG_PLAYFIELD_MAX_X - BUG_PLAYFIELD_MIN_X);
            b->y = BUG_SPAWN_Y;
            b->hp = 1;
            b->scoreValue = SCORE_CRAWLER;
            b->coinValue = COINS_CRAWLER;
            b->moveEvery = clampi(6 - speedBoost, 2, 6);
            b->descendEvery = clampi(90 - speedBoost * 8, 24, 90);
            break;
        case BUG_SPLITTER:
            b->x = BUG_PLAYFIELD_MIN_X + rng(BUG_PLAYFIELD_MAX_X - BUG_PLAYFIELD_MIN_X);
            b->y = BUG_SPAWN_Y;
            b->hp = 2;
            b->scoreValue = SCORE_SPLITTER;
            b->coinValue = COINS_SPLITTER;
            b->moveEvery = clampi(5 - speedBoost, 2, 5);
            b->descendEvery = clampi(80 - speedBoost * 8, 24, 80);
            break;
        case BUG_QUEEN:
            b->x = (SCREEN_W - QUEEN_SIZE) / 2;
            b->y = BUG_SPAWN_Y;
            b->hp = 8;
            b->scoreValue = SCORE_QUEEN;
            b->coinValue = COINS_QUEEN;
            b->moveEvery = 3;
            b->descendEvery = 0; // queen holds her row
            queenActive = true;
            break;
    }
}

void bug_spawn_mini(int x, int y, int dir) {
    int i = findFreeSlot();
    if (i < 0) return;
    Bug *b = &bugs[i];
    b->active = true;
    b->type = BUG_CRAWLER;
    b->mini = true;
    b->x = clampi(x, 0, SCREEN_W - BUG_MINI_SIZE);
    b->y = y;
    b->dir = dir;
    b->hp = 1;
    b->scoreValue = SCORE_MINI;
    b->coinValue = COINS_MINI;
    b->moveEvery = 3;
    b->descendEvery = 40;
    b->moveTimer = 0;
    b->descendTimer = 0;
}

void bug_kill(int index) {
    Bug *b = &bugs[index];
    if (!b->active) return;

    game_add_score(b->scoreValue);
    game_add_currency(b->coinValue);

    if (b->type == BUG_SPLITTER && !b->mini) {
        bug_spawn_mini(b->x - 4, b->y, -1);
        bug_spawn_mini(b->x + 4, b->y, 1);
    }
    // red Crawlers have a small chance to drop the flamethrower powerup
    if (b->type == BUG_CRAWLER && !b->mini && rng(FLAME_DROP_CHANCE) == 0) {
        powerup_spawn(POWERUP_FLAME, b->x, b->y);
    }
    if (b->type == BUG_QUEEN) {
        queenActive = false;
        game_on_queen_killed();
    }

    b->active = false;
}

void bug_update(void) {
    for (int i = 0; i < MAX_BUGS; i++) {
        Bug *b = &bugs[i];
        if (!b->active) continue;

        int size = bug_size(b);

        b->moveTimer++;
        if (b->moveTimer >= b->moveEvery) {
            b->moveTimer = 0;
            int nextX = b->x + b->dir;
            bool outOfBounds = nextX < 0 || nextX > SCREEN_W - size;
            bool blocked = obstacle_blocks(nextX, b->y, size, size);
            if (outOfBounds || blocked) {
                b->dir = -b->dir;
            } else {
                b->x = nextX;
            }
        }

        if (b->descendEvery > 0) {
            b->descendTimer++;
            if (b->descendTimer >= b->descendEvery) {
                b->descendTimer = 0;
                b->y += 6;
            }
        }

        // reached the bottom of the field without being shot down: it escapes
        if (b->y + size >= SCREEN_H) {
            if (b->type == BUG_QUEEN) queenActive = false;
            b->active = false;
            game_break_combo();
        }
    }
}

void bug_render(void) {
    for (int i = 0; i < MAX_BUGS; i++) {
        int id = OAM_BUG_BASE + i;
        Bug *b = &bugs[i];
        if (!b->active) {
            oamSet(&oamMain, id, 0, 0, 1, 15, SpriteSize_16x16, SpriteColorFormat_Bmp,
                   gfxCrawler, -1, false, true, false, false, false);
            continue;
        }

        u16 *gfx;
        SpriteSize size;
        if (b->type == BUG_QUEEN) {
            gfx = gfxQueen;
            size = SpriteSize_32x32;
        } else if (b->mini) {
            gfx = gfxMini;
            size = SpriteSize_8x8;
        } else if (b->type == BUG_SPLITTER) {
            gfx = gfxSplitter;
            size = SpriteSize_16x16;
        } else {
            gfx = gfxCrawler;
            size = SpriteSize_16x16;
        }

        oamSet(&oamMain, id, b->x, b->y, 1, 15, size, SpriteColorFormat_Bmp,
               gfx, -1, false, false, b->dir < 0, false, false);
    }
}
