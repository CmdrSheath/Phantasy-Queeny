//==============================================================================
// PHANTASY QUEENY - Pause Menu System
// Simple overlay - no plane clearing
//==============================================================================

#include <genesis.h>
#include <string.h>
#include "game.h"

static PauseMenuOption currentSelection = MENU_PARTY;
static u16 prevJoyState = 0;
static bool menuActive = false;

static const char* menuLabels[MENU_COUNT] = {
    "PARTY", "INV", "ALIGN", "MAP", "OPT", "SAVE"
};

void initPauseMenu(void) {
    menuActive = true;
    currentSelection = MENU_PARTY;
    prevJoyState = 0;
    
    // Just draw text on top of existing screen (BG_A)
    // Small box at top-right
    VDP_drawText("----------", 28, 2);
    VDP_drawText("|  MENU  |", 28, 3);
    VDP_drawText("----------", 28, 4);
    
    // Menu items
    for(u8 i = 0; i < MENU_COUNT; i++) {
        VDP_drawText(menuLabels[i], 30, 6 + i);
    }
    
    // Cursor
    VDP_drawText(">", 28, 6);
    
    // Controls hint
    VDP_drawText("A=SEL B/START=CLOSE", 2, 26);
}

void updatePauseMenu(void) {
    if (!menuActive) return;
    
    u16 joyState = JOY_readJoypad(JOY_1);
    u16 pressed = joyState & ~prevJoyState;
    
    // CLOSE MENU
    if (pressed & (BUTTON_START | BUTTON_B)) {
        menuActive = false;
        changeState(STATE_OVERWORLD);
        return;
    }
    
    // Navigate up
    if (pressed & BUTTON_UP) {
        if (currentSelection > 0) {
            VDP_drawText(" ", 28, 6 + currentSelection);
            currentSelection--;
            VDP_drawText(">", 28, 6 + currentSelection);
        }
    }
    
    // Navigate down
    if (pressed & BUTTON_DOWN) {
        if (currentSelection < MENU_COUNT - 1) {
            VDP_drawText(" ", 28, 6 + currentSelection);
            currentSelection++;
            VDP_drawText(">", 28, 6 + currentSelection);
        }
    }
    
    // Select
    if (pressed & BUTTON_A) {
        // Visual feedback
        VDP_drawText(">>", 26, 6 + currentSelection);
    }
    
    prevJoyState = joyState;
}

void cleanupPauseMenu(void) {
    // Clear only the menu text area, not the whole screen
    // Clear rows 2-20, columns 28-38
    for (u8 y = 2; y <= 26; y++) {
        VDP_clearText(28, y, 12);
    }
    menuActive = false;
}
