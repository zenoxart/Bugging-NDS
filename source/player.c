#include "player.h"
#include "sprites.h"
#include "bullet.h"
#include "game.h"

Player player;

void player_init(void) {
    player.x = (SCREEN_W - PLAYER_SIZE) / 2;
    player.lives = 3;
    player.heat = 0;
    player.overheated = false;
    player.fireCooldown = 0;
    player.invuln = 0;
    player.multiTimer = 0;
    player.flameTimer = 0;
    player.comboMultiShots = 0;
    player.rapidFireLevel = 0;
    player.heatResistLevel = 0;
    player.extraLivesBought = 0;
}

int player_fire_cooldown(void) {
    return clampi(PLAYER_FIRE_COOLDOWN - player.rapidFireLevel * RAPID_FIRE_STEP,
                  MIN_FIRE_COOLDOWN, PLAYER_FIRE_COOLDOWN);
}

int player_heat_per_shot(void) {
    return clampi(PLAYER_HEAT_PER_SHOT - player.heatResistLevel * HEAT_RESIST_STEP,
                  MIN_HEAT_PER_SHOT, PLAYER_HEAT_PER_SHOT);
}

void player_grant_combo_multishot(void) {
    player.comboMultiShots = clampi(player.comboMultiShots + COMBO_MULTISHOT_BONUS, 0, COMBO_MULTISHOT_CAP);
}

static void fireShot(void) {
    int cx = player.x + PLAYER_SIZE / 2;
    int ty = PLAYER_Y - 2;

    if (player.flameTimer > 0) {
        // a short-range rapid spray instead of one aimed bullet
        int jitter = rng(5) - 2; // -2..2 px
        bullet_spawn_flame(cx - BULLET_SIZE / 2 + jitter, ty);
        player.heat += FLAME_HEAT_PER_SHOT;
        player.fireCooldown = FLAME_FIRE_COOLDOWN;
    } else {
        bool multi = player.multiTimer > 0 || player.comboMultiShots > 0;
        if (multi) {
            bullet_spawn(cx - 6 - BULLET_SIZE / 2, ty);
            bullet_spawn(cx - BULLET_SIZE / 2, ty);
            bullet_spawn(cx + 6 - BULLET_SIZE / 2, ty);
            if (player.comboMultiShots > 0) player.comboMultiShots--;
        } else {
            bullet_spawn(cx - BULLET_SIZE / 2, ty);
        }
        player.heat += player_heat_per_shot();
        player.fireCooldown = player_fire_cooldown();
    }

    if (player.heat >= PLAYER_HEAT_MAX) {
        player.heat = PLAYER_HEAT_MAX;
        player.overheated = true;
    }
}

void player_update(void) {
    // scanKeys() is called once per frame in main()
    touchPosition touch;
    touchRead(&touch);
    int held = keysHeld();

    if (held & KEY_TOUCH) {
        player.x = clampi(touch.px - PLAYER_SIZE / 2, 0, SCREEN_W - PLAYER_SIZE);
    } else {
        if (held & KEY_LEFT)  player.x -= PLAYER_DPAD_SPEED;
        if (held & KEY_RIGHT) player.x += PLAYER_DPAD_SPEED;
        player.x = clampi(player.x, 0, SCREEN_W - PLAYER_SIZE);
    }

    // heat: cools passively every frame, re-enables firing once it drops far enough
    if (player.heat > 0) player.heat -= PLAYER_HEAT_COOL;
    if (player.heat < 0) player.heat = 0;
    if (player.overheated && player.heat <= PLAYER_HEAT_RESUME) player.overheated = false;

    if (player.fireCooldown > 0) player.fireCooldown--;
    if ((held & KEY_A) && !player.overheated && player.fireCooldown == 0) {
        fireShot();
    }

    if (player.multiTimer > 0) player.multiTimer--;
    if (player.flameTimer > 0) player.flameTimer--;
    if (player.invuln > 0) player.invuln--;
}

void player_hit(void) {
    if (player.invuln > 0) return; // still flashing from the last hit
    player.lives--;
    player.invuln = PLAYER_INVULN_FRAMES;
    game_on_player_hit();
}

void player_render(void) {
    // blink while invulnerable so a hit is readable
    bool hide = player.invuln > 0 && (player.invuln / 6) % 2 == 0;
    oamSet(&oamMain, OAM_PLAYER, player.x, PLAYER_Y, 0, 15, SpriteSize_16x16,
           SpriteColorFormat_Bmp, gfxPlayer, -1, false, hide, false, false, false);
}
