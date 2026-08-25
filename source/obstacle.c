#include "obstacle.h"
#include "sprites.h"
#include "game.h"
#include "powerup.h"

Obstacle obstacles[MAX_OBSTACLES];

void obstacle_init(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) obstacles[i].active = false;
}

void obstacle_spawn_wave(int count) {
    for (int n = 0; n < count; n++) {
        for (int i = 0; i < MAX_OBSTACLES; i++) {
            if (obstacles[i].active) continue;
            obstacles[i].active = true;
            obstacles[i].hp = OBSTACLE_HP;
            obstacles[i].shakeTimer = 0;
            obstacles[i].x = BUG_PLAYFIELD_MIN_X + rng(BUG_PLAYFIELD_MAX_X - BUG_PLAYFIELD_MIN_X);
            obstacles[i].y = OBSTACLE_MIN_Y + rng(OBSTACLE_MAX_Y - OBSTACLE_MIN_Y);
            break;
        }
    }
}

void obstacle_update(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active && obstacles[i].shakeTimer > 0) obstacles[i].shakeTimer--;
    }
}

bool obstacle_blocks(int x, int y, int w, int h) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacles[i].active) continue;
        if (aabbOverlap(x, y, w, h, obstacles[i].x, obstacles[i].y, OBSTACLE_SIZE, OBSTACLE_SIZE))
            return true;
    }
    return false;
}

bool obstacle_hit(int x, int y, int w, int h) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacles[i].active) continue;
        if (!aabbOverlap(x, y, w, h, obstacles[i].x, obstacles[i].y, OBSTACLE_SIZE, OBSTACLE_SIZE))
            continue;
        obstacles[i].hp--;
        obstacles[i].shakeTimer = OBSTACLE_SHAKE_FRAMES;
        if (obstacles[i].hp <= 0) {
            obstacles[i].active = false;
            game_add_score(SCORE_OBSTACLE);
            game_add_currency(COINS_OBSTACLE);
            if (rng(4) == 0) powerup_spawn(POWERUP_MULTI, obstacles[i].x, obstacles[i].y);
        }
        return true;
    }
    return false;
}

void obstacle_render(void) {
    static const int SHAKE_PATTERN[4] = {-2, 2, -1, 1};
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        int id = OAM_OBSTACLE_BASE + i;
        Obstacle *o = &obstacles[i];
        int dx = (o->shakeTimer > 0) ? SHAKE_PATTERN[o->shakeTimer % 4] : 0;
        oamSet(&oamMain, id, o->x + dx, o->y, 2, 15, SpriteSize_16x16, SpriteColorFormat_Bmp,
               gfxObstacle, -1, false, !o->active, false, false, false);
    }
}
