//==============================================================================
// PHANTASY QUEENY - Pause Menu System
// Simple floating menu on right side
//==============================================================================

#include <genesis.h>
#include <string.h>
#include "game.h"

// Menu state
static PauseMenuOption currentSelection = MENU_PARTY;
static u16 prevJoyState = 0;

// Menu labels
static const char* menuLabels[MENU_COUNT] = {
    "PARTY",
    "INV",
    "ALIGN",
    "MAP",
    "OPT",
    "SAVE"
};

void initPauseMenu(void) {
    // Ensure BG_B is clear (transparent) except our menu area
    VDP_clearPlane(BG_B, TRUE);
    
    // Reset state
    currentSelection = MENU_PARTY;
    prevJoyState = 0;
    
    // Draw small menu box on right side (columns 28-37, rows 4-14)
    // Top border
    VDP_drawTextBG(BG_B, "----------", 28, 4);
    VDP_drawTextBG(BG_B, "MENU", 30, 5);
    VDP_drawTextBG(BG_B, "----------", 28, 6);
    
    // Menu items
    for(u8 i = 0; i < MENU_COUNT; i++) {
        VDP_drawTextBG(BG_B, menuLabels[i], 30, 8 + i);
    }
    
    // Bottom border
    VDP_drawTextBG(BG_B, "----------", 28, 8 + MENU_COUNT);
    
    // Draw initial cursor
    VDP_drawTextBG(BG_B, ">", 28, 8);
    
    // Controls hint at bottom of screen
    VDP_drawTextBG(BG_B, "A:SEL B:BACK START:CLOSE", 2, 26);
}

void updatePauseMenu(void) {
    u16 joyState = JOY_readJoypad(JOY_1);
    u16 pressed = joyState & ~prevJoyState;
    
    // Close menu
    if (pressed & (BUTTON_START | BUTTON_B)) {
        changeState(STATE_OVERWORLD);
        return;
    }
    
    // Navigate
    if (pressed & BUTTON_UP) {
        if (currentSelection > 0) {
            // Clear old cursor
            VDP_drawTextBG(BG_B, " ", 28, 8 + currentSelection);
            currentSelection--;
            // Draw new cursor
            VDP_drawTextBG(BG_B, ">", 28, 8 + currentSelection);
        }
    }
    
    if (pressed & BUTTON_DOWN) {
        if (currentSelection < MENU_COUNT - 1) {
            // Clear old cursor
            VDP_drawTextBG(BG_B, " ", 28, 8 + currentSelection);
            currentSelection++;
            // Draw new cursor
            VDP_drawTextBG(BG_B, ">", 28, 8 + currentSelection);
        }
    }
    
    // Select (placeholder - just show message for now)
    if (pressed & BUTTON_A) {
        // Show what was selected
        char msg[32];
        sprintf(msg, "Selected: %s", menuLabels[currentSelection]);
        
        // Draw selection indicator
        VDP_drawTextBG(BG_B, ">>", 26, 8 + currentSelection);
        
        // For now, just a visual feedback
        // In future, this would open submenu or perform action
    }
    
    prevJoyState = joyState;
}

void cleanupPauseMenu(void) {
    // Clear BG_B completely to remove menu
    VDP_clearPlane(BG_B, TRUE);
}
