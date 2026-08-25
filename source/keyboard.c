#include <stdio.h>
#include <string.h>
#include "keyboard.h"
#include "leaderboard.h"

typedef struct {
    const char *letters;
    int len;
    int startCol; // text column of the first letter's center
    int y;        // text row
} KbRow;

// QWERTY, split across 3 rows so it reads like a real keyboard.
static const KbRow KB_ROWS[3] = {
    {"QWERTYUIOP", 10, 1, 7},
    {"ASDFGHJKL",   9, 2, 9},
    {"ZXCVBNM",     7, 5, 11},
};
#define KB_ROW_COUNT 3
#define KB_COL_STEP 3 // each letter owns a 3-column cell: [bracket][letter][bracket]

#define KB_ACTION_Y   14
#define KB_DEL_COL    7  // "[DEL]" spans columns 7..11
#define KB_OK_COL     21 // "[OK]"  spans columns 21..24

typedef struct {
    unsigned int pendingScore;
    char letters[3];
    int count;
    int cursorRow; // 0..2 = letter rows, 3 = action row
    int cursorCol; // column index within the current row (0 = DEL, 1 = OK on the action row)
    int committedRank;
} KbState;

static KbState kb;

void keyboard_begin(unsigned int score) {
    kb.pendingScore = score;
    kb.count = 0;
    kb.cursorRow = 0;
    kb.cursorCol = 0;
    kb.committedRank = -1;
}

static void addLetter(char c) {
    if (kb.count >= 3) return;
    kb.letters[kb.count++] = c;
}

static void doDelete(void) {
    if (kb.count > 0) kb.count--;
}

static bool doCommit(void) {
    if (kb.count < 1) return false; // need at least one letter
    char initials[3] = {' ', ' ', ' '};
    for (int i = 0; i < kb.count; i++) initials[i] = kb.letters[i];
    kb.committedRank = leaderboard_insert(kb.pendingScore, initials);
    return true;
}

// Whole 3-column cell (bracket/letter/bracket) counts as the tap target.
static bool hitTestLetterRow(const KbRow *row, int tcol, int *outIdx) {
    int rel = tcol - (row->startCol - 1);
    if (rel < 0) return false;
    int idx = rel / KB_COL_STEP;
    if (idx >= row->len) return false;
    *outIdx = idx;
    return true;
}

static bool hitTest(int px, int py, int *outRow, int *outCol) {
    int tcol = px / 8;
    int trow = py / 8;

    for (int r = 0; r < KB_ROW_COUNT; r++) {
        if (trow != KB_ROWS[r].y) continue;
        int idx;
        if (hitTestLetterRow(&KB_ROWS[r], tcol, &idx)) {
            *outRow = r;
            *outCol = idx;
            return true;
        }
    }
    if (trow == KB_ACTION_Y) {
        if (tcol >= KB_DEL_COL && tcol <= KB_DEL_COL + 4) {
            *outRow = KB_ROW_COUNT;
            *outCol = 0;
            return true;
        }
        if (tcol >= KB_OK_COL && tcol <= KB_OK_COL + 3) {
            *outRow = KB_ROW_COUNT;
            *outCol = 1;
            return true;
        }
    }
    return false;
}

// true if selecting the cursor's current cell should end entry this frame
static bool activateCursor(void) {
    if (kb.cursorRow < KB_ROW_COUNT) {
        addLetter(KB_ROWS[kb.cursorRow].letters[kb.cursorCol]);
        if (kb.count == 3) return doCommit();
        return false;
    }
    if (kb.cursorCol == 0) {
        doDelete();
        return false;
    }
    return doCommit();
}

static void clampCursorToRow(void) {
    int len = (kb.cursorRow < KB_ROW_COUNT) ? KB_ROWS[kb.cursorRow].len : 2;
    if (kb.cursorCol >= len) kb.cursorCol = len - 1;
    if (kb.cursorCol < 0) kb.cursorCol = 0;
}

bool keyboard_update(void) {
    touchPosition touch;
    touchRead(&touch);
    int held = keysHeld();
    int down = keysDown();

    if (held & KEY_TOUCH) {
        int r, c;
        if (hitTest(touch.px, touch.py, &r, &c)) {
            kb.cursorRow = r;
            kb.cursorCol = c;
        }
    }
    if (down & KEY_TOUCH) {
        int r, c;
        if (hitTest(touch.px, touch.py, &r, &c)) {
            kb.cursorRow = r;
            kb.cursorCol = c;
            if (activateCursor()) return true;
        }
    }

    if (down & KEY_RIGHT) {
        int len = (kb.cursorRow < KB_ROW_COUNT) ? KB_ROWS[kb.cursorRow].len : 2;
        kb.cursorCol = (kb.cursorCol + 1) % len;
    }
    if (down & KEY_LEFT) {
        int len = (kb.cursorRow < KB_ROW_COUNT) ? KB_ROWS[kb.cursorRow].len : 2;
        kb.cursorCol = (kb.cursorCol + len - 1) % len;
    }
    if (down & KEY_DOWN) {
        kb.cursorRow = (kb.cursorRow + 1) % (KB_ROW_COUNT + 1);
        clampCursorToRow();
    }
    if (down & KEY_UP) {
        kb.cursorRow = (kb.cursorRow + KB_ROW_COUNT) % (KB_ROW_COUNT + 1);
        clampCursorToRow();
    }
    if (down & (KEY_A | KEY_START)) {
        if (activateCursor()) return true;
    }
    if (down & KEY_B) {
        doDelete();
    }

    return false;
}

int keyboard_last_rank(void) {
    return kb.committedRank;
}

void keyboard_render(void) {
    iprintf("\x1b[1;9HENTER INITIALS\n");

    char disp[4] = {'_', '_', '_', '\0'};
    for (int i = 0; i < kb.count; i++) disp[i] = kb.letters[i];
    iprintf("\x1b[3;14H%s\n", disp);

    for (int r = 0; r < KB_ROW_COUNT; r++) {
        const KbRow *row = &KB_ROWS[r];
        for (int c = 0; c < row->len; c++) {
            int col = row->startCol + c * KB_COL_STEP;
            bool sel = (kb.cursorRow == r && kb.cursorCol == c);
            iprintf("\x1b[%d;%dH%c%c%c", row->y, col - 1,
                    sel ? '[' : ' ', row->letters[c], sel ? ']' : ' ');
        }
    }

    bool selDel = (kb.cursorRow == KB_ROW_COUNT && kb.cursorCol == 0);
    bool selOk  = (kb.cursorRow == KB_ROW_COUNT && kb.cursorCol == 1);
    iprintf("\x1b[%d;%dH%cDEL%c\n", KB_ACTION_Y, KB_DEL_COL, selDel ? '[' : ' ', selDel ? ']' : ' ');
    iprintf("\x1b[%d;%dH%cOK%c\n", KB_ACTION_Y, KB_OK_COL, selOk ? '[' : ' ', selOk ? ']' : ' ');

    iprintf("\x1b[17;0HTouch a letter, or\n");
    iprintf("\x1b[18;0HPad+A (B deletes)\n");
}
