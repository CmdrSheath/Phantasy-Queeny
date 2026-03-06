//==============================================================================
// PHANTASY QUEENY - UI System
//==============================================================================

#include <genesis.h>
#include <string.h>
#include "game.h"

// Message buffer
static char currentMessage[40] = "";
static u16 messageTimer = 0;

void initMenu() {}

void updateMenu() {}

void cleanupMenu() {}

void drawText(u16 x, u16 y, const char* text) {
    VDP_drawText(text, x, y);
}

void drawNumber(u16 x, u16 y, u16 number, u8 digits) {
    char buffer[6];
    // Simple number to string
    switch(digits) {
        case 1: sprintf(buffer, "%1d", number); break;
        case 2: sprintf(buffer, "%2d", number); break;
        case 3: sprintf(buffer, "%3d", number); break;
        case 4: sprintf(buffer, "%4d", number); break;
        case 5: sprintf(buffer, "%5d", number); break;
        default: sprintf(buffer, "%d", number); break;
    }
    VDP_drawText(buffer, x, y);
}

void showMessage(const char* text) {
    // Clear previous message area
    VDP_clearTextArea(2, 24, 36, 2);
    
    // Copy and display new message
    strncpy(currentMessage, text, 39);
    currentMessage[39] = '\0';
    VDP_drawText(currentMessage, 2, 24);
    
    // Set timer (messages auto-clear after 3 seconds)
    messageTimer = 180; // 3 seconds at 60fps
}

void clearMessage() {
    VDP_clearTextArea(2, 24, 36, 2);
    currentMessage[0] = '\0';
    messageTimer = 0;
}

// Call this in game loop to auto-clear messages
void updateMessageTimer(void) {
    if (messageTimer > 0) {
        messageTimer--;
        if (messageTimer == 0) {
            clearMessage();
        }
    }
}
