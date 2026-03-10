//==============================================================================
// PHANTASY QUEENY - Pause Menu System
// Simple overlay - no plane clearing
//==============================================================================

  

#include <genesis.h>
#include <string.h>
#include "game.h"

static PauseMenuOption currentSelection = MENU_PARTY;
static u16 prevJoyState = 0;

static const char* menuLabels[MENU_COUNT] = {
    "PARTY",
    "INV", 
    "ALIGN",
    "MAP",
    "OPT",
    "SAVE"
};

void initPauseMenu(void) {
    currentSelection = MENU_PARTY;
    prevJoyState = 0;
    
    // Draw menu box on right side (columns 28-38, rows 4-16)
    // Top border
    VDP_drawText("+--------+", 28, 4);
    VDP_drawText("|  MENU  |", 28, 5);
    VDP_drawText("+--------+", 28, 6);
    
    // Menu items
    for(u8 i = 0; i < MENU_COUNT; i++) {
        VDP_drawText(menuLabels[i], 30, 8 + i);
    }
    
    // Bottom border
    VDP_drawText("+--------+", 28, 8 + MENU_COUNT);
    
    // Initial cursor
    VDP_drawText(">", 29, 8);
    
    // Controls hint at bottom
    VDP_drawText("A:SELECT  B/START:CLOSE", 2, 26);
}

void updatePauseMenu(void) {
    u16 joyState = JOY_readJoypad(JOY_1);
    u16 pressed = joyState & ~prevJoyState;
    
    // Close menu
    if (pressed & (BUTTON_START | BUTTON_B)) {
        changeState(STATE_OVERWORLD);
        return;
    }
    
    // Navigate up
    if (pressed & BUTTON_UP) {
        if (currentSelection > 0) {
            // Clear old cursor
            VDP_drawText(" ", 29, 8 + currentSelection);
            currentSelection--;
            // Draw new cursor
            VDP_drawText(">", 29, 8 + currentSelection);
        }
    }
    
    // Navigate down
    if (pressed & BUTTON_DOWN) {
        if (currentSelection < MENU_COUNT - 1) {
            // Clear old cursor
            VDP_drawText(" ", 29, 8 + currentSelection);
            currentSelection++;
            // Draw new cursor
            VDP_drawText(">", 29, 8 + currentSelection);
        }
    }
    
    // Select option
    if (pressed & BUTTON_A) {
        // Show selection feedback
        VDP_drawText("*", 28, 8 + currentSelection);
        
        // Handle selection (placeholder for now)
        switch(currentSelection) {
            case MENU_PARTY:
                // Future: Show party info
                break;
            case MENU_INVENTORY:
                // Future: Show items
                break;
            case MENU_ALIGNMENT:
                // Future: Show alignment meter
                break;
            case MENU_MAP:
                // Future: Show map
                break;
            case MENU_OPTIONS:
                // Future: Show options
                break;
            case MENU_SAVE:
                // Future: Save game
                VDP_drawText("SAVED!", 15, 15);
                break;
        }
    }
    
    prevJoyState = joyState;
}

void cleanupPauseMenu(void) {
    // Clear menu box area (columns 28-38, rows 4-16)
    for(u8 y = 4; y <= 16; y++) {
        VDP_clearText(28, y, 10);
    }
    // Clear bottom hint
    VDP_clearText(2, 26, 25);
    // Clear any selection feedback
    VDP_clearText(15, 15, 6);
}
