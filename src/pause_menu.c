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
    inSubMenu = false;
    
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

    if (inSubMenu) {
        // In submenu mode
        if (pressed & BUTTON_C) {
            closeSubMenu();
        }
        else if (pressed & BUTTON_A) {
            if (currentSelection == MENU_SAVE) {
                VDP_drawText("SAVED!", 10, 18);
            }
        }
        prevJoyState = joyState;
        return;  // Don't process main menu navigation
 

    
    // Navigate up
    if (pressed & BUTTON_UP) {
        if (currentSelection > 0) {
            // Clear old cursor AND selection marker
            VDP_drawText(" ", 29, 8 + currentSelection);
            VDP_drawText(" ", 28, 8 + currentSelection); // Clear * marker
            currentSelection--;
            VDP_drawText(">", 29, 8 + currentSelection);
        }
    }
    
    // Navigate down
    if (pressed & BUTTON_DOWN) {
        if (currentSelection < MENU_COUNT - 1) {
            // Clear old cursor AND selection marker
            VDP_drawText(" ", 29, 8 + currentSelection);
            VDP_drawText(" ", 28, 8 + currentSelection); // Clear * marker
            currentSelection++;
            VDP_drawText(">", 29, 8 + currentSelection);
        }
    }
    
    // Select option - opens submenu
    if (pressed & BUTTON_A) {
        // Show selection marker
        VDP_drawText("*", 28, 8 + currentSelection);
        
        // Open submenu for this option
        openSubMenu(currentSelection);
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

static bool inSubMenu = false;
static u8 subMenuSelection = 0;

static void clearSubMenuArea(void) {
    // Clear left side area (columns 2-24, rows 8-20)
    for(u8 y = 8; y <= 20; y++) {
        VDP_clearText(2, y, 22);
    }
}

static void openSubMenu(PauseMenuOption option) {
    inSubMenu = true;
    subMenuSelection = 0;
    
    clearSubMenuArea();
    
    // Draw submenu box
    VDP_drawText("+------------------+", 2, 8);
    
    switch(option) {
        case MENU_PARTY:
            VDP_drawText("| QUEENY           |", 2, 9);
            VDP_drawText("| Lv: 1            |", 2, 10);
            VDP_drawText("| HP: 100/100      |", 2, 11);
            VDP_drawText("| MP: 50/50        |", 2, 12);
            VDP_drawText("| ATK: 15 DEF: 10  |", 2, 13);
            break;
            
        case MENU_INVENTORY:
            VDP_drawText("| INVENTORY        |", 2, 9);
            VDP_drawText("|                  |", 2, 10);
            VDP_drawText("| (Empty)          |", 2, 11);
            VDP_drawText("|                  |", 2, 12);
            VDP_drawText("|                  |", 2, 13);
            break;
            
        case MENU_ALIGNMENT:
            VDP_drawText("| ALIGNMENT        |", 2, 9);
            VDP_drawText("|                  |", 2, 10);
            VDP_drawText("| Neutral          |", 2, 11);
            VDP_drawText("| [====|====]      |", 2, 12);
            VDP_drawText("|                  |", 2, 13);
            break;
            
        case MENU_MAP:
            VDP_drawText("| MAP              |", 2, 9);
            VDP_drawText("|                  |", 2, 10);
            VDP_drawText("| Falcon City      |", 2, 11);
            VDP_drawText("| Downtown         |", 2, 12);
            VDP_drawText("|                  |", 2, 13);
            break;
            
        case MENU_OPTIONS:
            VDP_drawText("| OPTIONS          |", 2, 9);
            VDP_drawText("| SFX: ON          |", 2, 10);
            VDP_drawText("| Music: ON        |", 2, 11);
            VDP_drawText("|                  |", 2, 12);
            VDP_drawText("|                  |", 2, 13);
            break;
            
        case MENU_SAVE:
            VDP_drawText("| SAVE GAME?       |", 2, 9);
            VDP_drawText("|                  |", 2, 10);
            VDP_drawText("| > Yes            |", 2, 11);
            VDP_drawText("|   No             |", 2, 12);
            VDP_drawText("|                  |", 2, 13);
            break;
    }
    
    VDP_drawText("+------------------+", 2, 14);
    VDP_drawText("C:BACK", 2, 16);
}

static void closeSubMenu(void) {
    inSubMenu = false;
    clearSubMenuArea();
    // Clear the * marker from main menu
    VDP_drawText(" ", 28, 8 + currentSelection);
}

