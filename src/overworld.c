//==============================================================================
// PHANTASY QUEENY - Overworld / Metropolis
// Top-down tile-based city exploration
//==============================================================================

#include <genesis.h>
#include <string.h>
#include <stdbool.h>
#include "game.h"

// Map dimensions (in tiles)
#define MAP_WIDTH 40
#define MAP_HEIGHT 28

// Simple tile types
#define TILE_FLOOR 0
#define TILE_WALL 1
#define TILE_NEON 2
#define TILE_PORTAL 3

// Player position (in tiles)
static s16 playerX = 20;
static s16 playerY = 14;
static s16 oldPlayerX = 20;
static s16 oldPlayerY = 14;
static u16 moveTimer = 0;
static u16 animTimer = 0;
static u16 prevJoyState = 0;
static u8 currentTile = 0;  // Track current tile for interaction

// Simple test map (1 = wall, 0 = floor, 2 = neon, 3 = portal)
static const u8 testMap[28][40] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,1,1,1,0,1},
    {1,0,1,2,1,0,0,0,1,2,2,1,0,0,0,0,1,2,2,2,2,2,2,1,0,0,0,0,1,2,2,1,0,0,0,1,2,1,0,1},
    {1,0,1,1,1,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,1,0,1},
    {1,0,1,2,1,0,0,0,0,0,0,0,0,1,2,2,1,0,0,0,0,0,0,0,0,1,2,2,1,0,0,0,0,0,0,0,1,2,0,1},
    {1,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1},
    {1,0,1,2,0,0,1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,1,0,0,2,2,1},
    {1,0,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1},
    {1,0,1,2,2,1,0,0,0,0,1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,1,0,0,0,0,1,2,2,1},
    {1,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,1},
    {1,0,1,2,0,0,0,0,1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,1,0,0,0,0,2,1},
    {1,0,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

void drawMap(void) {
    for (u16 y = 0; y < MAP_HEIGHT; y++) {
        for (u16 x = 0; x < MAP_WIDTH; x++) {
            u8 tile = testMap[y][x];
            u16 palette = PAL0;
            u16 tileIndex = 0;
            
            switch(tile) {
                case TILE_WALL:
                    tileIndex = 1;
                    palette = PAL1;  // Uses color 17
                    break;
                case TILE_NEON:
                    tileIndex = 1;
                    palette = PAL2;  // Uses color 33
                    break;
                case TILE_PORTAL:
                    tileIndex = 1;
                    palette = PAL3;  // Uses color 49
                    break;
                default:
                    tileIndex = 0;
                    palette = PAL0;
                    break;
            }
            
            VDP_setTileMapXY(BG_A, 
                TILE_ATTR_FULL(palette, 0, 0, 0, tileIndex), 
                x, y);
        }
    }
}


void clearAndDrawPlayer(void) {
    // Clear only the text area where player was, not entire plane
    // Actually, let's just clear a small region around player
    VDP_clearTextAreaBG(BG_B, oldPlayerX, oldPlayerY, 1, 1);
    VDP_drawTextBG(BG_B, "Q", playerX, playerY);
}

bool isWalkable(s16 x, s16 y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return false;
    }
    
    u8 tile = testMap[y][x];
    return (tile == TILE_FLOOR || tile == TILE_NEON || tile == TILE_PORTAL);
}

void forceInteract(void) {
    u8 tile = testMap[playerY][playerX];
    
    switch(tile) {
        case TILE_PORTAL:
            showMessage("Portal energy surges!");
            break;
        case TILE_NEON:
            showMessage("Neon lights hum...");
            break;
        default:
            showMessage("Nothing here.");
            break;
    }
}

void checkTileInteraction(void) {
    u8 tile = testMap[playerY][playerX];
    
    // Only show message if tile changed or it's a special tile
    if (tile != currentTile) {
        currentTile = tile;
        
        switch(tile) {
            case TILE_PORTAL:
                showMessage("A dimensional portal swirls...");
                break;
            case TILE_NEON:
                showMessage("Neon lights flicker...");
                break;
        }
    }
}

void initOverworld(void) {
    // Set up palettes
    // White.   0x0EEE
    // Blue.    0x0088
    // Cyan.    0x00EE
    // Red.     0x0E00
    // Yellow.  0x0EE0
    // L Grey.  0x0AAA
    // portal blue. 0x000E
    // 
    // PAL0 (colors 0-15)
    PAL_setColor(0, 0x0000);  // Black transparent
    PAL_setColor(1, 0x0444);  // Dark grey floor
    PAL_setColor(2, 0x0AAA);  // Light grey walls
    PAL_setColor(3, 0x00E0);  // Neon green dim
    PAL_setColor(4, 0x00F0);  // Neon green bright
    PAL_setColor(5, 0x00EE);  // Portal blue dim
    PAL_setColor(6, 0x000F);  // Portal blue bright
    PAL_setColor(15, 0x0EEE); // White text
    
    // PAL1 (colors 16-31) - walls use color 17
    PAL_setColor(16, 0x0000); // Transparent
    PAL_setColor(17, 0x0AAA); // Wall color
    
    // PAL2 (colors 32-47) - neon uses color 33
    PAL_setColor(32, 0x0000); // Transparent
    PAL_setColor(33, 0x00E0); // Neon
    PAL_setColor(34, 0x00F0); // Neon bright
    
    // PAL3 (colors 48-63) - portal uses color 49
    PAL_setColor(48, 0x0000); // Transparent
    PAL_setColor(49, 0x00EE); // Portal
    PAL_setColor(50, 0x000F); // Portal bright
    
    // NOW clear planes
    VDP_clearPlane(BG_B, TRUE);
    
    // Reset player
    playerX = 20;
    playerY = 14;
    oldPlayerX = 20;
    oldPlayerY = 14;
    moveTimer = 0;
    animTimer = 0;
    prevJoyState = 0;
    currentTile = 0;
    
    // Draw the map
    drawMap();
    
    // Show message
    showMessage("Falcon City - Downtown");

    playMusic (MUSIC_OVERWORLD);
}




void updateOverworld(void) {
    // Animate neon
    animTimer++;
    if (animTimer >= 30) {
        animTimer = 0;
        // Swap colors 33 and 34 (PAL2 indices 1 and 2)
        u16 temp = PAL_getColor(33);
        PAL_setColor(33, PAL_getColor(34));
        PAL_setColor(34, temp);
    }

    
    // Handle movement cooldown
    if (moveTimer > 0) {
        moveTimer--;
        // Still draw player every frame
        clearAndDrawPlayer();
        return;
    }
    
    // Read input
    u16 joyState = JOY_readJoypad(JOY_1);
    s16 dx = 0;
    s16 dy = 0;
    bool moved = false;
    
    if (joyState & BUTTON_UP) { dy = -1; moved = true; }
    else if (joyState & BUTTON_DOWN) { dy = 1; moved = true; }
    else if (joyState & BUTTON_LEFT) { dx = -1; moved = true; }
    else if (joyState & BUTTON_RIGHT) { dx = 1; moved = true; }
    
    // Move player
    if (moved) {
        s16 newX = playerX + dx;
        s16 newY = playerY + dy;
        
        if (isWalkable(newX, newY)) {
            oldPlayerX = playerX;
            oldPlayerY = playerY;
            playerX = newX;
            playerY = newY;
            moveTimer = 8;
            
            // Check what we stepped on
            checkTileInteraction();
        }
    }
    
    // Handle button presses (detect rising edge)
    u16 pressedButtons = joyState & ~prevJoyState;
    
    if (pressedButtons & BUTTON_A) {
        forceInteract();
    }
    
    if (pressedButtons & BUTTON_START) {
        // Open pause menu instead of showing message
        changeState(STATE_PAUSE_MENU);
        return;  // Exit update early since state changed
    }
    
    prevJoyState = joyState;
    
    // Draw player
    clearAndDrawPlayer();
}

void cleanupOverworld(void) {
    //VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    clearMessage();
}

void spawnPortal(u8 tileX, u8 tileY, int enemyType) {}
void checkCollision(void) {}
void checkEncounters(void) {}
void updateCamera(void) {}
void renderMap(void) { drawMap(); }
void tryInteract(void) { forceInteract(); }
