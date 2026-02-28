#include <genesis.h>
#include <string.h>
#include <stdbool.h>
#include "game.h"

void initCombat(void) {
    VDP_drawText("COMBAT MODE", 10, 10);
}

void updateCombat(void) {
    if (JOY_readJoypad(JOY_1) & BUTTON_A) {
        changeState(STATE_OVERWORLD);
    }
}

void cleanupCombat(void) {
    VDP_clearPlan(PLAN_A, TRUE);
    VDP_clearPlan(PLAN_B, TRUE);
}

void startCombat(int enemyType, int count) {
    changeState(STATE_COMBAT);
}
