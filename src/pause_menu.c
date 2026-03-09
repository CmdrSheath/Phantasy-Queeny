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
    VDP_drawTextBG(BG_A, "PARTY INFO", 2, 2);
    VDP_drawTextBG(BG_A, "==========", 2, 3);
    VDP_drawTextBG(BG_A, "Queeny", 2, 5);
    VDP_drawTextBG(BG_A, "HP: 100/100", 2, 6);
    VDP_drawTextBG(BG_A, "MP: 50/50", 2, 7);
    VDP_drawTextBG(BG_A, "Level: 1", 2, 8);
}

static void drawInventorySubMenu(void) {
    VDP_drawTextBG(BG_A, "INVENTORY", 2, 2);
    VDP_drawTextBG(BG_A, "=========", 2, 3);
    VDP_drawTextBG(BG_A, "Empty...", 2, 5);
}

static void drawAlignmentSubMenu(void) {
    VDP_drawTextBG(BG_A, "ALIGNMENT", 2, 2);
    VDP_drawTextBG(BG_A, "=========", 2, 3);
    VDP_drawTextBG(BG_A, "Neutral", 2, 5);
    VDP_drawTextBG(BG_A, "[====|====]", 2, 6);
}

static void drawMapSubMenu(void) {
    VDP_drawTextBG(BG_A, "MAP", 2, 2);
    VDP_drawTextBG(BG_A, "===", 2, 3);
    VDP_drawTextBG(BG_A, "Falcon City", 2, 5);
    VDP_drawTextBG(BG_A, "Downtown", 2, 6);
}

static void drawOptionsSubMenu(void) {
    VDP_drawTextBG(BG_A, "OPTIONS", 2, 2);
    VDP_drawTextBG(BG_A, "=======", 2, 3);
    VDP_drawTextBG(BG_A, "SFX: ON", 2, 5);
    VDP_drawTextBG(BG_A, "Music: ON", 2, 6);
}

static void drawSaveSubMenu(void) {
    VDP_drawTextBG(BG_A, "SAVE", 2, 2);
    VDP_drawTextBG(BG_A, "====", 2, 3);
    VDP_drawTextBG(BG_A, "Save game?", 2, 5);
    VDP_drawTextBG(BG_A, "A: Yes  B: No", 2, 7);
}

static void drawSubMenu(void) {
    // Clear left side area on BG_A (columns 0-23)
    for (u8 y = 0; y < 28; y++) {
        for (u8 x = 0; x < 24; x++) {
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 0, 0, 0, 1), x, y);
        }
    }
    
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
    // Draw right-side menu on BG_A (columns 25-38)
    // Use tile 1 (solid) for background, PAL1 for grey color
    
    for (u8 y = 2; y < 4 + MENU_COUNT + 2; y++) {
        for (u8 x = 25; x < 38; x++) {
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, 1), x, y);
        }
    }
    
    // Draw border
    VDP_drawTextBG(BG_A, "============", 25, 2);
    
    // Menu items
    for(u8 i = 0; i < MENU_COUNT; i++) {
        if (i == currentSelection) {
            VDP_drawTextBG(BG_A, ">", 25, 4 + i);
            VDP_drawTextBG(BG_A, menuLabels[i], 27, 4 + i);
        } else {
            VDP_drawTextBG(BG_A, " ", 25, 4 + i);
            VDP_drawTextBG(BG_A, menuLabels[i], 27, 4 + i);
        }
    }
    
    // Bottom border
    VDP_drawTextBG(BG_A, "============", 25, 4 + MENU_COUNT + 1);
}

void initPauseMenu(void) {
    // Store current game screen (it's on BG_A)
    // We'll draw menu over it on BG_A
    
    // Reset state
    currentSelection = MENU_PARTY;
    inSubMenu = false;
    prevJoyState = 0;
    
    // Draw initial menu
    drawMainMenu();
    
    // Show hint at bottom
    VDP_drawTextBG(BG_A, "A:SELECT C:BACK START/B:CLOSE", 2, 26);
}

void updatePauseMenu(void) {
    u16 joyState = JOY_readJoypad(JOY_1);
    u16 pressed = joyState & ~prevJoyState;
    
    if (inSubMenu) {
        // In submenu mode
        if (pressed & (BUTTON_C | BUTTON_B)) {
            // Back to main menu
            inSubMenu = false;
            // Redraw just the menu (restore map underneath)
            initOverworld();  // Restore map
            drawMainMenu();   // Draw menu on top
        }
        else if (pressed & BUTTON_A) {
            // Handle submenu action
            if (currentSelection == MENU_SAVE) {
                // Save game placeholder
                VDP_drawTextBG(BG_A, "SAVED!", 10, 15);
                waitMs(500);
            }
        }
    } else {
        // In main menu mode
        if (pressed & (BUTTON_START | BUTTON_B)) {
            // Close pause menu - restore overworld
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
    // Just re-init overworld to restore everything
    initOverworld();
    clearMessage();
}
