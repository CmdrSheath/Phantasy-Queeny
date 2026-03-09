//==============================================================================
// PHANTASY QUEENY - Pause Menu System (DEBUG VERSION)
//==============================================================================

#include <genesis.h>
#include <string.h>
#include "game.h"

static PauseMenuOption currentSelection = MENU_PARTY;
static u16 prevJoyState = 0;

static const char* menuLabels[MENU_COUNT] = {
    "PARTY", "INV", "ALIGN", "MAP", "OPT", "SAVE"
};

void initPauseMenu(void) {
    // FLASH SCREEN BLUE to prove we got here
    PAL_setColor(0, 0x000E);  // Background = blue
    VDP_waitVSync();
    
    // Small delay to see the flash
    for(u16 i = 0; i < 30; i++) VDP_waitVSync();
    
    // Restore black background
    PAL_setColor(0, 0x0000);
    
    // Clear only text area, not whole plane
    VDP_clearTextAreaBG(BG_B, 0, 0, 40, 28);
    
    currentSelection = MENU_PARTY;
    prevJoyState = 0;
    
    // Draw menu at top-right (small)
    VDP_drawTextBG(BG_B, "+--------+", 28, 2);
    VDP_drawTextBG(BG_B, "|  MENU  |", 28, 3);
    VDP_drawTextBG(BG_B, "+--------+", 28, 4);
    
    // Draw items
    for(u8 i = 0; i < MENU_COUNT; i++) {
        VDP_drawTextBG(BG_B, menuLabels[i], 30, 6 + i);
    }
    
    // Cursor
    VDP_drawTextBG(BG_B, ">", 28, 6);
    
    // Controls at bottom
    VDP_drawTextBG(BG_B, "A=SEL B/START=CLOSE", 2, 26);
}

void updatePauseMenu(void) {
    u16 joyState = JOY_readJoypad(JOY_1);
    u16 pressed = joyState & ~prevJoyState;
    
    // CLOSE MENU - most important!
    if (pressed & (BUTTON_START | BUTTON_B)) {
        // FLASH GREEN to show we're exiting
        PAL_setColor(0, 0x00E0);
        for(u16 i = 0; i < 10; i++) VDP_waitVSync();
        PAL_setColor(0, 0x0000);
        
        changeState(STATE_OVERWORLD);
        return;
    }
    
    if (pressed & BUTTON_UP) {
        if (currentSelection > 0) {
            VDP_drawTextBG(BG_B, " ", 28, 6 + currentSelection);
            currentSelection--;
            VDP_drawTextBG(BG_B, ">", 28, 6 + currentSelection);
        }
    }
    
    if (pressed & BUTTON_DOWN) {
        if (currentSelection < MENU_COUNT - 1) {
            VDP_drawTextBG(BG_B, " ", 28, 6 + currentSelection);
            currentSelection++;
            VDP_drawTextBG(BG_B, ">", 28, 6 + currentSelection);
        }
    }
    
    if (pressed & BUTTON_A) {
        // FLASH RED for selection
        PAL_setColor(0, 0x0E00);
        for(u16 i = 0; i < 10; i++) VDP_waitVSync();
        PAL_setColor(0, 0x0000);
    }
    
    prevJoyState = joyState;
}

void cleanupPauseMenu(void) {
    // Just clear text, don't touch tiles
    VDP_clearTextAreaBG(BG_B, 0, 0, 40, 28);
}
