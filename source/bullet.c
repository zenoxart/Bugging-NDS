#include "bullet.h"
#include "sprites.h"
#include "game.h"

Bullet bullets[MAX_BULLETS];

void bullet_init(void) {
    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
}

static bool spawn(int x, int y, bool flame) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].active = true;
            bullets[i].flame = flame;
            bullets[i].x = x;
            bullets[i].y = y;
            bullets[i].life = flame ? FLAME_LIFETIME : 0;
            return true;
        }
    }
    return false;
}

bool bullet_spawn(int x, int y) {
    return spawn(x, y, false);
}

bool bullet_spawn_flame(int x, int y) {
    return spawn(x, y, true);
}

void bullet_update(void) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        bullets[i].y -= BULLET_SPEED;

        if (bullets[i].flame) {
            if (bullets[i].life > 0) bullets[i].life--;
            if (bullets[i].y < -BULLET_SIZE || bullets[i].life <= 0) {
                // a spray weapon fizzling out mid-air isn't a "miss" -
                // only a normal shot leaving the field breaks the combo
                bullets[i].active = false;
            }
            continue;
        }

        if (bullets[i].y < -BULLET_SIZE) {
            bullets[i].active = false;
            game_break_combo(); // a shot that left the field without hitting anything is a miss
        }
    }
}

void bullet_render(void) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        int id = OAM_BULLET_BASE + i;
        Bullet *b = &bullets[i];
        u16 *gfx = b->flame ? gfxFlameBullet : gfxBullet;
        oamSet(&oamMain, id, b->x, b->y, 1, 15, SpriteSize_8x8, SpriteColorFormat_Bmp,
               gfx, -1, false, !b->active, false, false, false);
    }
}
