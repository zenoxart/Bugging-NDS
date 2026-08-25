#include "powerup.h"
#include "sprites.h"
#include "player.h"

Powerup powerups[MAX_POWERUPS];

void powerup_init(void) {
    for (int i = 0; i < MAX_POWERUPS; i++) powerups[i].active = false;
}

void powerup_spawn(PowerupType type, int x, int y) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) continue;
        powerups[i].active = true;
        powerups[i].type = type;
        powerups[i].x = clampi(x, 0, SCREEN_W - POWERUP_SIZE);
        powerups[i].y = y;
        powerups[i].timer = POWERUP_LIFETIME;
        return;
    }
}

void powerup_update(void) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) continue;

        powerups[i].timer--;
        if (powerups[i].timer <= 0) {
            powerups[i].active = false;
            continue;
        }
        if (powerups[i].timer % POWERUP_FALL_EVERY == 0) powerups[i].y += 1;

        if (aabbOverlap(powerups[i].x, powerups[i].y, POWERUP_SIZE, POWERUP_SIZE,
                         player.x, PLAYER_Y, PLAYER_SIZE, PLAYER_SIZE)) {
            switch (powerups[i].type) {
                case POWERUP_MULTI: player.multiTimer = PLAYER_MULTI_FRAMES; break;
                case POWERUP_FLAME: player.flameTimer = PLAYER_FLAME_FRAMES; break;
            }
            powerups[i].active = false;
        }
    }
}

void powerup_render(void) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        int id = OAM_POWERUP_BASE + i;
        Powerup *p = &powerups[i];
        u16 *gfx = (p->type == POWERUP_FLAME) ? gfxPowerupFlame : gfxPowerup;
        oamSet(&oamMain, id, p->x, p->y, 1, 15, SpriteSize_8x8, SpriteColorFormat_Bmp,
               gfx, -1, false, !p->active, false, false, false);
    }
}
