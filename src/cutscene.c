//==============================================================================
// PHANTASY QUEENY - Opening Cutscene
// Simple still image with text (240 frames = 4 seconds)
//==============================================================================

#include <genesis.h>
#include <string.h>
#include "game.h"

static u16 frameCounter = 0;
static bool skipped = false;

void initCutscene(void)
{
    // Clear screen
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    
    // Set colors for cutscene
    PAL_setColor(1, 0x0EEE); // White text
    PAL_setColor(2, 0x0088); // Dark blue background accent
    PAL_setColor(3, 0x0E00); // Red accent
    
    // Draw "still image" representation (border box)
    // Top border
    for (u16 x = 4; x < 36; x++) {
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 0, 0, 0, 1), x, 4);
    }
    // Bottom border
    for (u16 x = 4; x < 36; x++) {
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 0, 0, 0, 1), x, 14);
    }
    // Side borders
    for (u16 y = 4; y <= 14; y++) {
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 0, 0, 0, 1), 4, y);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 0, 0, 0, 1), 35, y);
    }
    
    // Fill "image" area with pattern (simulating city view)
    for (u16 y = 5; y < 14; y++) {
        for (u16 x = 5; x < 35; x++) {
            // Create simple pattern (checkerboard for "city lights")
            u16 tile = ((x + y) % 2 == 0) ? 2 : 3;
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 0, 0, 0, tile), x, y);
        }
    }
    
    // Draw text at bottom
    VDP_drawText("Welcome to", 14, 16);
    VDP_drawText("FALCON CITY", 14, 17);
    
    // Reset counter
    frameCounter = 0;
    skipped = false;
}

void updateCutscene(void)
{
    // Check for skip (press START or A)
    u16 joyState = JOY_readJoypad(JOY_1);
    if ((joyState & (BUTTON_START | BUTTON_A)) && !skipped) {
        skipped = true;
        changeState(STATE_OVERWORLD);
        return;
    }
    
    // Count frames (240 frames = 4 seconds at 60fps)
    frameCounter++;
    
    // Optional: Add "skip" hint after 1 second
    if (frameCounter == 60) {
        VDP_drawText("Press START to skip", 10, 22);
    }
    
    // Auto-advance after 240 frames
    if (frameCounter >= 240) {
        changeState(STATE_OVERWORLD);
    }
}

void cleanupCutscene(void)
{
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    frameCounter = 0;
}

