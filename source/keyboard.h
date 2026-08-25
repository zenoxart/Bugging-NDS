#ifndef BUGSWARM_KEYBOARD_H
#define BUGSWARM_KEYBOARD_H

#include "globals.h"

// On-screen QWERTY keyboard (sub-screen) for entering a 3-letter leaderboard
// initial. Usable by touch (tap a letter) or D-Pad + A (move a cursor).

void keyboard_begin(unsigned int score); // call once when entering the entry state
bool keyboard_update(void);              // returns true once OK/3rd letter commits the entry
void keyboard_render(void);
int keyboard_last_rank(void);            // rank the committed score landed at (valid after commit)

#endif
