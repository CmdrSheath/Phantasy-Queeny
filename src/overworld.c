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
    VDP_clearPlane(BG_A, TRUE);
    
    for (u16 y = 0; y < MAP_HEIGHT; y++) {
        for (u16 x = 0; x < MAP_WIDTH; x++) {
            u8 tile = testMap[y][x];
            u16 palette = PAL0;
            u16 tileIndex;
            
            switch(tile) {
                case TILE_WALL:
                    tileIndex = 1;
                    palette = PAL1;
                    break;
                case TILE_NEON:
                    tileIndex = 2;
                    palette = PAL2;
                    break;
                case TILE_PORTAL:
                    tileIndex = 3;
                    palette = PAL3;
                    break;
                case TILE_FLOOR:
                default:
                    tileIndex = 0;
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
    // 

    // Set up palettes
   
    // PAL0 colors (0-15) - used for floor and general
    PAL_setColor(0, 0x0000);  // Black (transparent/background)
    PAL_setColor(1, 0x0444);  // Dark gray (floor)
    
    // PAL1 colors (16-31) - used for walls
    // Color 16 in VDP is index 0 of PAL1
    PAL_setColor(16, 0x0000); // Black (transparent for PAL1)
    PAL_setColor(17, 0x0AAA); // Bright grey (walls) - tile uses color 1
    
    // PAL2 colors (32-47) - used for neon
    PAL_setColor(32, 0x0000); // Black (transparent for PAL2)
    PAL_setColor(33, 0x00E0); // Neon green dim - tile uses color 1
    PAL_setColor(34, 0x00F0); // Neon green bright - for animation
    
    // PAL3 colors (48-63) - used for portal
    PAL_setColor(48, 0x0000); // Black (transparent for PAL3)
    PAL_setColor(49, 0x000E); // Portal blue dim - tile uses color 1
    PAL_setColor(50, 0x000F); // Portal blue bright - for animation
    
    // Reset player
    playerX = 20;
    playerY = 14;
    oldPlayerX = 20;
    oldPlayerY = 14;
    moveTimer = 0;
    animTimer = 0;
    prevJoyState = 0;
    currentTile = 0;
    
    // Draw map on BG_A
    drawMap();
    
    // Clear BG_B for player and text
    VDP_clearPlane(BG_B, TRUE);
    
    // Show welcome message
    showMessage("Falcon City - Downtown");
}


void updateOverworld(void) {
    // Animate neon
    animTimer++;
    if (animTimer >= 30) {
        animTimer = 0;
        u16 temp = PAL_getColor(3);
        PAL_setColor(3, PAL_getColor(4));
        PAL_setColor(4, temp);
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
        showMessage("Menu not implemented");
    }
    
    prevJoyState = joyState;
    
    // Draw player
    clearAndDrawPlayer();
}

void cleanupOverworld(void) {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    clearMessage();
}

void spawnPortal(u8 tileX, u8 tileY, int enemyType) {}
void checkCollision(void) {}
void checkEncounters(void) {}
void updateCamera(void) {}
void renderMap(void) { drawMap(); }
void tryInteract(void) { forceInteract(); }
