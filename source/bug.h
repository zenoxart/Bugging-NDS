#ifndef BUGSWARM_BUG_H
#define BUGSWARM_BUG_H

#include "globals.h"

typedef enum {
    BUG_CRAWLER,
    BUG_SPLITTER,
    BUG_QUEEN
} BugType;

typedef struct {
    bool active;
    BugType type;
    bool mini;      // true for splitter offspring
    int x, y;
    int dir;        // -1 or +1, horizontal direction
    int hp;
    int scoreValue;
    int coinValue;
    int moveEvery;  // frames between 1px horizontal steps (speed)
    int descendEvery;
    int moveTimer;
    int descendTimer;
} Bug;

extern Bug bugs[MAX_BUGS];
extern bool queenActive;

void bug_init(void);
void bug_spawn(BugType type, int phase, int cycle); // picks a free slot at the top of the field
void bug_spawn_mini(int x, int y, int dir);          // splitter offspring, spawned in pairs
void bug_update(void);   // move + steer around obstacles + reach-player detection
void bug_render(void);
void bug_kill(int index); // handles score/combo/splitting for a bullet kill
int bug_size(Bug *b);     // sprite/AABB dimension (8, 16 or 32) for the given bug

#endif
