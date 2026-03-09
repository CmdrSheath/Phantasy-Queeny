//==============================================================================
// PHANTASY QUEENY - Pause Menu System
// Simple overlay using BG_B
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

void initPauseMenu(void) {
    // Set up BG_B palette for menu
    // PAL0 color 15 = white text, color 0 = black bg
    PAL_setColor(15, 0x0EEE);  // White for text
    
    // Clear BG_B completely
    VDP_clearPlane(BG_B, TRUE);
    
    // Fill BG_B with solid black tiles (tile 1 in PAL0)
    for (u16 y = 0; y < 28; y++) {
        for (u16 x = 0; x < 40; x++) {
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, 1), x, y);
        }
    }
    
    // Reset state
    currentSelection = MENU_PARTY;
    inSubMenu = false;
    prevJoyState = 0;
    
    // Draw menu border
    VDP_drawTextBG(BG_B, "PAUSE MENU", 15, 2);
    VDP_drawTextBG(BG_B, "==========", 15, 3);
    
    // Draw initial menu items
    for(u8 i = 0; i < MENU_COUNT; i++) {
        if (i == 0) {
            VDP_drawTextBG(BG_B, ">", 10, 6 + i);
            VDP_drawTextBG(BG_B, menuLabels[i], 12, 6 + i);
        } else {
            VDP_drawTextBG(BG_B, " ", 10, 6 + i);
            VDP_drawTextBG(BG_B, menuLabels[i], 12, 6 + i);
        }
    }
    
    // Draw controls at bottom
    VDP_drawTextBG(BG_B, "A:SELECT B:BACK START:CLOSE", 5, 25);
}

static void drawMenu(void) {
    // Redraw just the menu items (not the whole screen)
    for(u8 i = 0; i < MENU_COUNT; i++) {
        if (i == currentSelection) {
            VDP_drawTextBG(BG_B, ">", 10, 6 + i);
        } else {
            VDP_drawTextBG(BG_B, " ", 10, 6 + i);
        }
    }
}

static void drawSubMenu(void) {
    // Clear center area
    for (u8 y = 6; y < 20; y++) {
        for (u8 x = 20; x < 38; x++) {
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, 1), x, y);
        }
    }
    
    // Draw submenu content based on selection
    switch(currentSelection) {
        case MENU_PARTY:
            VDP_drawTextBG(BG_B, "PARTY", 22, 8);
            VDP_drawTextBG(BG_B, "Queeny Lv.1", 22, 10);
            VDP_drawTextBG(BG_B, "HP:100 MP:50", 22, 11);
            break;
        case MENU_INVENTORY:
            VDP_drawTextBG(BG_B, "ITEMS", 22, 8);
            VDP_drawTextBG(BG_B, "No items", 22, 10);
            break;
        case MENU_ALIGNMENT:
            VDP_drawTextBG(BG_B, "ALIGN", 22, 8);
            VDP_drawTextBG(BG_B, "Neutral", 22, 10);
            break;
        case MENU_MAP:
            VDP_drawTextBG(BG_B, "MAP", 22, 8);
            VDP_drawTextBG(BG_B, "Falcon City", 22, 10);
            break;
        case MENU_OPTIONS:
            VDP_drawTextBG(BG_B, "OPTIONS", 22, 8);
            VDP_drawTextBG(BG_B, "SFX:ON", 22, 10);
            break;
        case MENU_SAVE:
            VDP_drawTextBG(BG_B, "SAVE?", 22, 8);
            VDP_drawTextBG(BG_B, "A:Yes B:No", 22, 10);
            break;
    }
}

void updatePauseMenu(void) {
    u16 joyState = JOY_readJoypad(JOY_1);
    u16 pressed = joyState & ~prevJoyState;
    
    if (inSubMenu) {
        if (pressed & (BUTTON_B | BUTTON_C)) {
            // Back to main menu
            inSubMenu = false;
            // Clear submenu area
            for (u8 y = 6; y < 20; y++) {
                for (u8 x = 20; x < 38; x++) {
                    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, 1), x, y);
                }
            }
        }
        else if (pressed & BUTTON_A && currentSelection == MENU_SAVE) {
            VDP_drawTextBG(BG_B, "SAVED!", 22, 15);
        }
    } else {
        // Main menu mode
        if (pressed & (BUTTON_START | BUTTON_B)) {
            // Close pause menu
            changeState(STATE_OVERWORLD);
            return;
        }
        
        if (pressed & BUTTON_UP) {
            if (currentSelection > 0) {
                currentSelection--;
                drawMenu();
            }
        }
        
        if (pressed & BUTTON_DOWN) {
            if (currentSelection < MENU_COUNT - 1) {
                currentSelection++;
                drawMenu();
            }
        }
        
        if (pressed & BUTTON_A) {
            inSubMenu = true;
            drawSubMenu();
        }
    }
    
    prevJoyState = joyState;
}

void cleanupPauseMenu(void) {
    // Clear BG_B so overworld shows through
    VDP_clearPlane(BG_B, TRUE);
}
