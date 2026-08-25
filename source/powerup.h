#ifndef BUGSWARM_POWERUP_H
#define BUGSWARM_POWERUP_H

#include "globals.h"

typedef enum {
    POWERUP_MULTI,
    POWERUP_FLAME,
} PowerupType;

typedef struct {
    bool active;
    PowerupType type;
    int x, y;
    int timer;
} Powerup;

extern Powerup powerups[MAX_POWERUPS];

void powerup_init(void);
void powerup_spawn(PowerupType type, int x, int y);
void powerup_update(void); // falls, times out, and checks pickup against the player
void powerup_render(void);

#endif
