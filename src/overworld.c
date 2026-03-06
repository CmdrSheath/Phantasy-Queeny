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
    VDP_clearPlane(BG_B, TRUE);
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
            showMessage("Neon lights hum with energy...");
            break;
        default:
            showMessage("Nothing interesting here.");
            break;
    }
}

void checkTileInteraction(void) {
    u8 tile = testMap[playerY][playerX];
    
    switch(tile) {
        case TILE_PORTAL:
            showMessage("A dimensional portal swirls...");
            break;
        case TILE_NEON:
            showMessage("Neon lights flicker...");
            break;
        default:
            break;
    }
}

void initOverworld(void) {
    PAL_setColor(0, 0x0000);
    PAL_setColor(1, 0x0444);
    PAL_setColor(2, 0x0EEE);
    PAL_setColor(3, 0x0088);
    PAL_setColor(4, 0x00EE);
    PAL_setColor(5, 0x0E00);
    PAL_setColor(6, 0x0EE0);
    
    playerX = 20;
    playerY = 14;
    oldPlayerX = 20;
    oldPlayerY = 14;
    moveTimer = 0;
    animTimer = 0;
    prevJoyState = 0;
    
    drawMap();
    VDP_clearPlane(BG_B, TRUE);
    
    showMessage("Falcon City - Downtown");
}

void updateOverworld(void) {
    animTimer++;
    if (animTimer >= 30) {
        animTimer = 0;
        u16 temp = PAL_getColor(3);
        PAL_setColor(3, PAL_getColor(4));
        PAL_setColor(4, temp);
    }
    
    if (moveTimer > 0) {
        moveTimer--;
        clearAndDrawPlayer();
        return;
    }
    
    u16 joyState = JOY_readJoypad(JOY_1);
    s16 dx = 0;
    s16 dy = 0;
    bool moved = false;
    
    if (joyState & BUTTON_UP) { dy = -1; moved = true; }
    else if (joyState & BUTTON_DOWN) { dy = 1; moved = true; }
    else if (joyState & BUTTON_LEFT) { dx = -1; moved = true; }
    else if (joyState & BUTTON_RIGHT) { dx = 1; moved = true; }
    
    if (moved) {
        s16 newX = playerX + dx;
        s16 newY = playerY + dy;
        
        if (isWalkable(newX, newY)) {
            playerX = newX;
            playerY = newY;
            moveTimer = 8;
            checkTileInteraction();
        }
    }
    
    u16 pressedButtons = joyState & ~prevJoyState;
    
    if (pressedButtons & BUTTON_A) {
        forceInteract();
    }
    
    if (pressedButtons & BUTTON_START) {
        showMessage("Menu not implemented yet");
    }
    
    prevJoyState = joyState;
    
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
                case TILE_NEON:
                    tileIndex = 2; // Animated neon
                    palette = PAL2;
                    break;
                case TILE_PORTAL:
                    tileIndex = 3; // Portal
                    palette = PAL3;
                    break;
                case TILE_FLOOR:
                default:
                    tileIndex = 0; // Empty/floor
                    break;
            }
            
            VDP_setTileMapXY(BG_A, 
                TILE_ATTR_FULL(palette, 0, 0, flip, tileIndex), 
                x, y);
        }
    }
}

void drawPlayer(void) {
    // Draw player as sprite (or character on BG_B)
    // For now, use a simple character on BG_B at player position
    // Clear previous position
    VDP_clearText(0, 0, 40);
    
    // Draw player at current position
    // Using 'Q' for Queeny
    char playerStr[2] = "Q";
    VDP_drawText(playerStr, playerX, playerY);
}

bool isWalkable(s16 x, s16 y) {
    // Check bounds
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return false;
    }
    
    // Check tile type
    u8 tile = testMap[y][x];
    return (tile == TILE_FLOOR || tile == TILE_NEON || tile == TILE_PORTAL);
}

void checkTileInteraction(void) {
    u8 tile = testMap[playerY][playerX];
    
    switch(tile) {
        case TILE_PORTAL:
            showMessage("A dimensional portal swirls...");
            // Could trigger combat here
            // changeState(STATE_COMBAT);
            break;
        case TILE_NEON:
            showMessage("Neon lights flicker...");
            break;
        default:
            clearMessage();
            break;
    }
}

void initOverworld(void) {
    // Set up palettes
    PAL_setColor(0, 0x0000); // Black (background)
    PAL_setColor(1, 0x0444); // Dark gray (floor)
    PAL_setColor(2, 0x0EEE); // White (walls)
    PAL_setColor(3, 0x0088); // Blue (neon dim)
    PAL_setColor(4, 0x00EE); // Cyan (neon bright)
    PAL_setColor(5, 0x0E00); // Red (portal)
    PAL_setColor(6, 0x0EE0); // Yellow (portal center)
    
    // Reset player position
    playerX = 20;
    playerY = 14;
    moveTimer = 0;
    
    // Draw the map
    drawMap();
    
    // Show initial message
    showMessage("Falcon City - Downtown");
}

void updateOverworld(void) {
    // Animate neon tiles
    animTimer++;
    if (animTimer >= 30) {
        animTimer = 0;
        // Toggle neon color between palette entries 3 and 4
        u16 temp = PAL_getColor(3);
        PAL_setColor(3, PAL_getColor(4));
        PAL_setColor(4, temp);
    }
    
    // Handle movement with cooldown (prevent too fast movement)
    if (moveTimer > 0) {
        moveTimer--;
        return;
    }
    
    u16 joyState = JOY_readJoypad(JOY_1);
    s16 dx = 0;
    s16 dy = 0;
    
    if (joyState & BUTTON_UP) dy = -1;
    else if (joyState & BUTTON_DOWN) dy = 1;
    else if (joyState & BUTTON_LEFT) dx = -1;
    else if (joyState & BUTTON_RIGHT) dx = 1;
    
    // Try to move
    if (dx != 0 || dy != 0) {
        s16 newX = playerX + dx;
        s16 newY = playerY + dy;
        
        if (isWalkable(newX, newY)) {
            playerX = newX;
            playerY = newY;
            moveTimer = 8; // 8 frame cooldown between moves
            
            // Check what's on the new tile
            checkTileInteraction();
        }
        
        drawPlayer();
    }
    
    // Interact button
    if (joyState & BUTTON_A) {
        checkTileInteraction();
        // Prevent holding A
        while(JOY_readJoypad(JOY_1) & BUTTON_A) {
            SYS_doVBlankProcess();
        }
    }
    
    // Menu button
    if (joyState & BUTTON_START) {
        showMessage("Menu not implemented yet");
        while(JOY_readJoypad(JOY_1) & BUTTON_START) {
            SYS_doVBlankProcess();
        }
    }
}

void cleanupOverworld(void) {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    clearMessage();
}

void spawnPortal(u8 tileX, u8 tileY, int enemyType) {
    // Placeholder for dynamic portal spawning
}

void checkCollision(void) {
    // Handled in updateOverworld
}

void checkEncounters(void) {
    // Random encounter system placeholder
}

void updateCamera(void) {
    // Camera is static for now (full map fits on screen)
}

void renderMap(void) {
    // Handled in drawMap
}

void tryInteract(void) {
    checkTileInteraction();
}

