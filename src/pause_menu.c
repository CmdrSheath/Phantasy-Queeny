//==============================================================================
// PHANTASY QUEENY - Pause Menu System
// Simple overlay - no plane clearing
//==============================================================================

        
#include "game.h"

void initPauseMenu(void) {
    // Just show PAUSED text
    VDP_drawText("PAUSED", 15, 10);
    VDP_drawText("Press START to resume", 8, 12);
}

void updatePauseMenu(void) {
    u16 joy = JOY_readJoypad(JOY_1);
    
    if (joy & BUTTON_START) {
        // Wait for release
        while(JOY_readJoypad(JOY_1) & BUTTON_START);
        changeState(STATE_OVERWORLD);
    }
}

void cleanupPauseMenu(void) {
    VDP_clearText(15, 10, 6);
    VDP_clearText(8, 12, 21);
}
