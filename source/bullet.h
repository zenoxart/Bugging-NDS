#ifndef BUGSWARM_BULLET_H
#define BUGSWARM_BULLET_H

#include "globals.h"

typedef struct {
    bool active;
    bool flame; // true for flamethrower particles: shorter range, different sprite
    int x, y;
    int life;   // frames left before it fizzles out; only meaningful when flame is true
} Bullet;

extern Bullet bullets[MAX_BULLETS];

void bullet_init(void);
bool bullet_spawn(int x, int y);       // a normal shot; returns false if no free slot
bool bullet_spawn_flame(int x, int y); // a short-range flame particle
void bullet_update(void);              // move, expire off-screen or out of life (breaks combo on miss)
void bullet_render(void);

#endif
