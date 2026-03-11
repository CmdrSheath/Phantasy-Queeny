//==============================================================================
// PHANTASY QUEENY - Combat System
//==============================================================================

#include <genesis.h>
#include <string.h>
#include <stdbool.h>
#include "game.h"
#include "audio.h"

void initCombat(void) {
    // Clear screen
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    
    // Set colors
    PAL_setColor(1, 0x0EEE); // White
    PAL_setColor(2, 0x0E00); // Red
    
    // Draw combat UI
    VDP_drawText("COMBAT MODE", 14, 2);
    VDP_drawText("================", 14, 3);
    
    // Enemy placeholder
    VDP_drawText("[ENEMY]", 16, 8);
    VDP_drawText("Crystal Slasher", 13, 10);
    
    // Player stats
    VDP_drawText("Queeny: HP 100/100", 2, 20);
    VDP_drawText("MP 50/50", 2, 21);
    
    // Commands
    VDP_drawText("A: Attack", 25, 18);
    VDP_drawText("B: Magic", 25, 19);
    VDP_drawText("C: Item", 25, 20);
    VDP_drawText("START: Flee", 25, 21);

    playMusic(MUSIC_COMBAT);
    playSFX(SFX_COMBAT_START);
    
    showMessage("A wild Slasher appears!");
}

void updateCombat(void) {
    u16 joyState = JOY_readJoypad(JOY_1);
    
    // Return to overworld with A (for testing)
    if (joyState & BUTTON_A) {
        showMessage("You escaped!");
        waitMs(500);
        changeState(STATE_OVERWORLD);
        return;
    }
    
    // Start button also returns to overworld
    if (joyState & BUTTON_START) {
        changeState(STATE_OVERWORLD);
        return;
    }
}

void cleanupCombat(void) {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    clearMessage();
}

void startCombat(int enemyType, int count) {
    changeState(STATE_COMBAT);
}
