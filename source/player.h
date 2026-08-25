#ifndef BUGSWARM_PLAYER_H
#define BUGSWARM_PLAYER_H

#include "globals.h"

typedef struct {
    int x;
    int lives;
    int heat;
    bool overheated;
    int fireCooldown;
    int invuln;
    int multiTimer;
    int flameTimer;       // >0 while the flamethrower powerup is active
    int comboMultiShots;  // free triple-shots left, granted by hitting a 5-combo

    // shop upgrades (source/shop.c), persist for the run, reset by player_init()
    int rapidFireLevel;   // each level shortens the fire cooldown
    int heatResistLevel;  // each level reduces heat gained per shot
    int extraLivesBought; // display-only purchase counter
} Player;

extern Player player;

void player_init(void);
void player_update(void);
void player_render(void);
void player_hit(void); // called when a bug reaches the player

int player_fire_cooldown(void); // effective cooldown after rapidFireLevel
int player_heat_per_shot(void); // effective heat/shot after heatResistLevel

// called by game.c when a combo first reaches COMBO_MAX
void player_grant_combo_multishot(void);

#endif
