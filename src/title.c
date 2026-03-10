//==============================================================================
// PHANTASY QUEENY - Title Screen
//==============================================================================

#include <genesis.h>
#include <string.h>
#include "game.h"

static u16 blinkTimer = 0;
static bool showPressStart = true;

void initTitle(void)
{
    // Clear screen
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    
    // Set colors
    PAL_setColor(1, 0x0EEE); // White
    PAL_setColor(2, 0x00EE); // Cyan
    PAL_setColor(3, 0x0E0E); // Magenta
    
    // Draw title
    VDP_drawText("PHANTASY QUEENY", 12, 8);
    VDP_drawText("================", 12, 9);
    
    // Draw subtitle
    VDP_drawText("A Sci-Fi RPG Adventure", 9, 12);
    
    // Draw controls hint
    VDP_drawText("D-PAD: Move", 13, 18);
    VDP_drawText("A: Interact", 13, 19);
    VDP_drawText("START: Menu", 13, 20);

    playMusic(MUSIC_TITLE);
    
    blinkTimer = 0;
    showPressStart = true;
}

void updateTitle(void)
{
    // Blink "PRESS START" text
    blinkTimer++;
    if (blinkTimer >= 30) { // Toggle every half second at 60fps
        blinkTimer = 0;
        showPressStart = !showPressStart;
        
        if (showPressStart) {
            VDP_drawText("PRESS START BUTTON", 11, 15);
        } else {
            VDP_clearText(11, 15, 18);
        }
    }
    
    // Check for start button
    u16 joyState = JOY_readJoypad(JOY_1);
    if (joyState & BUTTON_START) {
        // Small delay to prevent accidental double-press
        waitMs(200);
        changeState(STATE_CUTSCENE);
    }
}

void cleanupTitle(void)
{
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
}
