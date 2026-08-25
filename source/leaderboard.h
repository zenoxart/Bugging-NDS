#ifndef BUGSWARM_LEADERBOARD_H
#define BUGSWARM_LEADERBOARD_H

#include "globals.h"

#define LEADERBOARD_SIZE 8

typedef struct {
    char initials[4]; // 3 letters + NUL
    unsigned int score;
} LeaderboardEntry;

extern LeaderboardEntry leaderboard[LEADERBOARD_SIZE];
extern int leaderboardCount;
extern bool leaderboardStorageOk; // false if there's nowhere to save (no FAT/SD found)

// Mounts storage (libfat) and loads the saved board, or seeds a few
// placeholder entries if there's no save file yet. Call once at boot.
void leaderboard_init(void);

// true if `score` would land somewhere on the board (board isn't full yet,
// or it beats the current lowest entry).
bool leaderboard_qualifies(unsigned int score);

// Inserts the score at its sorted rank, writes the board to storage
// (if available), and returns the new 0-based rank.
int leaderboard_insert(unsigned int score, const char initials[3]);

// Draws the full board on the sub-screen console. highlightRank < 0 = none.
void leaderboard_render(int highlightRank);

#endif
