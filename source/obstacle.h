#ifndef BUGSWARM_OBSTACLE_H
#define BUGSWARM_OBSTACLE_H

#include "globals.h"

typedef struct {
    bool active;
    int x, y;
    int hp;
    int shakeTimer; // counts down after a hit; obstacle_render() jitters its position while > 0
} Obstacle;

extern Obstacle obstacles[MAX_OBSTACLES];

void obstacle_init(void);
void obstacle_spawn_wave(int count); // scatters `count` fresh mushrooms into free slots
void obstacle_update(void);
void obstacle_render(void);

// true if an active obstacle overlaps the given box (used by bugs to steer around them)
bool obstacle_blocks(int x, int y, int w, int h);

// damages the obstacle overlapping the box, if any; returns true if a hit was absorbed
bool obstacle_hit(int x, int y, int w, int h);

#endif
