#include <stdio.h>
#include <string.h>
#include <fat.h>
#include "leaderboard.h"

#define SAVE_PATH "/bugswarm_scores.dat"
#define SAVE_MAGIC 0x53575342u // arbitrary, just validates the file is ours

LeaderboardEntry leaderboard[LEADERBOARD_SIZE];
int leaderboardCount = 0;
bool leaderboardStorageOk = false;

typedef struct {
    unsigned int magic;
    int count;
    LeaderboardEntry entries[LEADERBOARD_SIZE];
} SaveBlob;

static void seedDefaults(void) {
    static const LeaderboardEntry defaults[] = {
        {"BUG", 2000},
        {"ARC", 1200},
        {"CLD", 600},
    };
    leaderboardCount = sizeof(defaults) / sizeof(defaults[0]);
    for (int i = 0; i < leaderboardCount; i++) leaderboard[i] = defaults[i];
}

static void load(void) {
    FILE *f = fopen(SAVE_PATH, "rb");
    if (!f) {
        seedDefaults();
        return;
    }
    SaveBlob blob;
    size_t n = fread(&blob, sizeof(blob), 1, f);
    fclose(f);
    if (n != 1 || blob.magic != SAVE_MAGIC || blob.count < 0 || blob.count > LEADERBOARD_SIZE) {
        seedDefaults();
        return;
    }
    leaderboardCount = blob.count;
    memcpy(leaderboard, blob.entries, sizeof(LeaderboardEntry) * leaderboardCount);
}

static void save(void) {
    if (!leaderboardStorageOk) return;
    FILE *f = fopen(SAVE_PATH, "wb");
    if (!f) return;
    SaveBlob blob;
    blob.magic = SAVE_MAGIC;
    blob.count = leaderboardCount;
    memcpy(blob.entries, leaderboard, sizeof(LeaderboardEntry) * leaderboardCount);
    fwrite(&blob, sizeof(blob), 1, f);
    fclose(f);
}

void leaderboard_init(void) {
    leaderboardStorageOk = fatInitDefault();
    load(); // still gives a populated (session-only) board if storage is unavailable
}

bool leaderboard_qualifies(unsigned int score) {
    if (leaderboardCount < LEADERBOARD_SIZE) return true;
    return score > leaderboard[LEADERBOARD_SIZE - 1].score;
}

int leaderboard_insert(unsigned int score, const char initials[3]) {
    int rank = leaderboardCount < LEADERBOARD_SIZE ? leaderboardCount : LEADERBOARD_SIZE - 1;
    while (rank > 0 && leaderboard[rank - 1].score < score) {
        leaderboard[rank] = leaderboard[rank - 1];
        rank--;
    }
    leaderboard[rank].initials[0] = initials[0];
    leaderboard[rank].initials[1] = initials[1];
    leaderboard[rank].initials[2] = initials[2];
    leaderboard[rank].initials[3] = '\0';
    leaderboard[rank].score = score;
    if (leaderboardCount < LEADERBOARD_SIZE) leaderboardCount++;

    save();
    return rank;
}

void leaderboard_render(int highlightRank) {
    iprintf("\x1b[1;10HLEADERBOARD\n");
    for (int i = 0; i < leaderboardCount; i++) {
        char marker = (i == highlightRank) ? '>' : ' ';
        iprintf("\x1b[%d;3H%c%d. %s  %06lu\n", 4 + i, marker, i + 1,
                leaderboard[i].initials, (unsigned long)leaderboard[i].score);
    }
    if (!leaderboardStorageOk)
        iprintf("\x1b[18;1H(not saved: no storage found)\n");
    iprintf("\x1b[21;7HPress START\n");
}
