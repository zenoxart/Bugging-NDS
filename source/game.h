#ifndef BUGSWARM_GAME_H
#define BUGSWARM_GAME_H

#include "globals.h"

typedef enum {
    STATE_TITLE,
    STATE_PLAYING,
    STATE_GAMEOVER,
    STATE_ENTER_INITIALS,
    STATE_LEADERBOARD,
    STATE_SHOP
} GameStateId;

typedef struct {
    GameStateId state;
    unsigned int score;
    unsigned int highScore;
    unsigned int currency; // spent in the post-boss shop; resets each run
    int combo;
    int comboTimer;
    unsigned int phaseTimer; // frames since the current difficulty cycle started
    int cycle;                // increments every time a Queen is defeated
    int spawnTimer;
    unsigned int titleSeedTicker;
    int lastRank; // leaderboard rank to highlight (-1 = none), set on entering STATE_LEADERBOARD
} Game;

extern Game game;

void game_init(void);          // one-time setup at boot
void game_reset(void);         // (re)starts a run
void game_update(void);        // advances the current state by one frame
void game_render(void);        // draws every entity plus the sub-screen HUD

GamePhase game_phase(void);
void game_add_score(unsigned int base);
void game_add_currency(unsigned int amount);
void game_break_combo(void);
void game_on_player_hit(void);
void game_on_queen_killed(void);

int rng(int maxExclusive);

#endif
