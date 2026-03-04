//==============================================================================
// PHANTASY QUEENY - Overworld / Metropolis
// Sci-fi city exploration with dimensional portals
//==============================================================================

#include <genesis.h>
#include <string.h>
#include <stdbool.h>
#include "game.h"

void changeState(GameState newState);

void initOverworld(void) {
    VDP_drawText("OVERWORLD", 10, 10);
}

void updateOverworld(void) {
    if (JOY_readJoypad(JOY_1) & BUTTON_A) {
        changeState(STATE_COMBAT);
    }
}

void cleanupOverworld(void) {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
}

void spawnPortal(u8 tileX, u8 tileY, int enemyType) {
    // Placeholder
}

void checkCollision(void) {
    // Placeholder
}

void checkEncounters(void) {
    // Placeholder
}

void updateCamera(void) {
    // Placeholder
}

void renderMap(void) {
    // Placeholder
}

void tryInteract(void) {
    VDP_drawText("Nothing here...", 10, 20);
}

