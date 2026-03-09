//==============================================================================
// PHANTASY QUEENY - Pause Menu System
// Right-side menu with left-side submenus
//==============================================================================

#include <genesis.h>
#include <string.h>
#include "game.h"

// Menu state
static PauseMenuOption currentSelection = MENU_PARTY;
static bool inSubMenu = false;
static u16 prevJoyState = 0;

// Menu labels
static const char* menuLabels[MENU_COUNT] = {
    "PARTY",
    "INVENTORY", 
    "ALIGNMENT",
    "MAP",
    "OPTIONS",
    "SAVE"
};

// Submenu content
static void drawPartySubMenu(void) {
    VDP_drawTextBG(BG_B, "PARTY INFO", 2, 2);
    VDP_drawTextBG(BG_B, "==========", 2, 3);
    VDP_drawTextBG(BG_B, "Queeny", 2, 5);
    VDP_drawTextBG(BG_B, "HP: 100/100", 2, 6);
    VDP_drawTextBG(BG_B, "MP: 50/50", 2, 7);
    VDP_drawTextBG(BG_B, "Level: 1", 2, 8);
}

static void drawInventorySubMenu(void) {
    VDP_drawTextBG(BG_B, "INVENTORY", 2, 2);
    VDP_drawTextBG(BG_B, "=========", 2, 3);
    VDP_drawTextBG(BG_B, "Empty...", 2, 5);
}

static void drawAlignmentSubMenu(void) {
    VDP_drawTextBG(BG_B, "ALIGNMENT", 2, 2);
    VDP_drawTextBG(BG_B, "=========", 2, 3);
    VDP_drawTextBG(BG_B, "Neutral", 2, 5);
    VDP_drawTextBG(BG_B, "[====|====]", 2, 6);
}

static void drawMapSubMenu(void) {
    VDP_drawTextBG(BG_B, "MAP", 2, 2);
    VDP_drawTextBG(BG_B, "===", 2, 3);
    VDP_drawTextBG(BG_B, "Falcon City", 2, 5);
    VDP_drawTextBG(BG_B, "Downtown", 2, 6);
}

static void drawOptionsSubMenu(void) {
    VDP_drawTextBG(BG_B, "OPTIONS", 2, 2);
    VDP_drawTextBG(BG_B, "=======", 2, 3);
    VDP_drawTextBG(BG_B, "SFX: ON", 2, 5);
    VDP_drawTextBG(BG_B, "Music: ON", 2, 6);
}

static void drawSaveSubMenu(void) {
    VDP_drawTextBG(BG_B, "SAVE", 2, 2);
    VDP_drawTextBG(BG_B, "====", 2, 3);
    VDP_drawTextBG(BG_B, "Save game?", 2, 5);
    VDP_drawTextBG(BG_B, "A: Yes  B: No", 2, 7);
}

static void drawSubMenu(void) {
    // Clear left side area (columns 0-19, rows 0-27)
    VDP_clearTextAreaBG(BG_B, 0, 0, 20, 28);
    
    switch(currentSelection) {
        case MENU_PARTY:
            drawPartySubMenu();
            break;
        case MENU_INVENTORY:
            drawInventorySubMenu();
            break;
        case MENU_ALIGNMENT:
            drawAlignmentSubMenu();
            break;
        case MENU_MAP:
            drawMapSubMenu();
            break;
        case MENU_OPTIONS:
            drawOptionsSubMenu();
            break;
        case MENU_SAVE:
            drawSaveSubMenu();
            break;
        default:
            break;
    }
}

static void drawMainMenu(void) {
    // Draw right-side menu box (columns 25-38)
    // Top border
    VDP_drawTextBG(BG_B, "============", 25, 2);
    
    // Menu items
    for(u8 i = 0; i < MENU_COUNT; i++) {
        if (i == currentSelection) {
            // Draw cursor
            VDP_drawTextBG(BG_B, ">", 25, 4 + i);
            VDP_drawTextBG(BG_B, menuLabels[i], 27, 4 + i);
        } else {
            VDP_drawTextBG(BG_B, " ", 25, 4 + i);
            VDP_drawTextBG(BG_B, menuLabels[i], 27, 4 + i);
        }
    }
    
    // Bottom border
    VDP_drawTextBG(BG_B, "============", 25, 4 + MENU_COUNT + 1);
}

void initPauseMenu(void) {
    // Clear BG_B for menu
    VDP_clearPlane(BG_B, TRUE);
    
    // Reset state
    currentSelection = MENU_PARTY;
    inSubMenu = false;
    prevJoyState = 0;
    
    // Draw initial menu
    drawMainMenu();
    
    // Show hint
    showMessage("A:Select C:Back START/B:Close");
}

void updatePauseMenu(void) {
    u16 joyState = JOY_readJoypad(JOY_1);
    u16 pressed = joyState & ~prevJoyState;
    
    if (inSubMenu) {
        // In submenu mode
        if (pressed & (BUTTON_C | BUTTON_B)) {
            // Back to main menu
            inSubMenu = false;
            // Clear left side
            VDP_clearTextAreaBG(BG_B, 0, 0, 24, 28);
            // Redraw main menu
            drawMainMenu();
        }
        // A button handles submenu action
        else if (pressed & BUTTON_A) {
            // Handle submenu selection (placeholder)
            // For save menu, this would trigger save, etc.
        }
    } else {
        // In main menu mode
        if (pressed & (BUTTON_START | BUTTON_B)) {
            // Close pause menu
            changeState(STATE_OVERWORLD);
            return;
        }
        
        if (pressed & BUTTON_UP) {
            if (currentSelection > 0) {
                currentSelection--;
                drawMainMenu();
            }
        }
        
        if (pressed & BUTTON_DOWN) {
            if (currentSelection < MENU_COUNT - 1) {
                currentSelection++;
                drawMainMenu();
            }
        }
        
        if (pressed & BUTTON_A) {
            // Open submenu
            inSubMenu = true;
            drawSubMenu();
            drawMainMenu(); // Keep menu visible on right
        }
    }
    
    prevJoyState = joyState;
}

void cleanupPauseMenu(void) {
    VDP_clearPlane(BG_B, TRUE);
    clearMessage();
}
