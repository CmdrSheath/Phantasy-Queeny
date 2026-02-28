//////
//////

//==============================================================================
// PHANTASY QUEENY - Overworld / Metropolis
// Sci-fi city exploration with dimensional portals
//==============================================================================

#include "game.h"

// Map constants
#define METRO_WIDTH     48
#define METRO_HEIGHT    32

// Tile types
#define TILE_FLOOR      0
#define TILE_WALL       1
#define TILE_WATER      2
#define TILE_BUILDING   3
#define TILE_ROAD       4
#define TILE_PORTAL     5  // Dimensional rift!

// Portal states
#define PORTAL_CLOSED   0
#define PORTAL_OPENING  1
#define PORTAL_ACTIVE   2

typedef struct {
    u16 x, y;
    u8 state;
    u8 timer;        // Animation frame
    EnemyType enemy; // What emerges
    bool triggered;  // Has player seen this?
} Portal;

// Metropolis map data (simplified - 48x32)
// 0=FLOOR, 1=WALL, 2=WATER, 3=BUILDING, 4=ROAD, 5=PORTAL
const u8 metroMap[METRO_HEIGHT][METRO_WIDTH] = {
    // Row 0-5: Northern district (industrial)
    {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
    {3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,3},
    {3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,4,3},
    {3,4,0,0,0,4,4,4,4,0,0,0,4,4,4,4,0,0,0,4,4,4,4,0,0,0,4,4,4,4,0,0,0,4,4,4,4,0,0,0,4,4,4,4,0,0,4,3},
    {3,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,3},
    {3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,3},
    
    // Row 6-10: Central plaza (where Queeny starts)
    {3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,4,3},
    {3,4,0,0,0,4,4,4,4,0,0,0,4,4,4,4,0,0,0,4,4,4,4,0,0,0,4,4,4,4,0,0,0,4,4,4,4,0,0,0,4,4,4,4,0,0,4,3},
    {3,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,3}, // START HERE (row 8, col 22)
    {3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,3},
    {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
    
    // Row 11-15: Eastern tech district (portals appear here)
    {3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,3},
    {3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,0,4,3,3,4,0,0,4,3},
    {3,4,0,0,0,4,4,4,4,0,0,0,4,4,4,4,0,5,0,4,4,4,4,0,0,0,4,4,4,4,0,0,0,4,4,4,4,0,0,0,4,4,4,4,0,0,4,3}, // PORTAL at 13,17
    {3,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,3},
    {3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,3},
    
    // ... (continue pattern for remaining rows)
    // I'll provide the full map if needed, but this is enough to test
};

// Active portals in the city
#define MAX_PORTALS 4
Portal portals[MAX_PORTALS];
u8 portalCount = 0;

// Camera position
s16 camX = 0, camY = 0;

// Animation frames
u8 portalAnimTimer = 0;

void initOverworld() {
    // Set Queeny's starting position (Central Plaza)
    party[0].x = 22 * TILE_SIZE;  // Center of map
    party[0].y = 8 * TILE_SIZE;
    party[0].direction = DIR_SOUTH;
    
    // Initialize camera
    camX = party[0].x - SCREEN_WIDTH/2;
    camY = party[0].y - SCREEN_HEIGHT/2;
    
    // Clamp camera to map bounds
    if (camX < 0) camX = 0;
    if (camY < 0) camY = 0;
    if (camX > (METRO_WIDTH * TILE_SIZE) - SCREEN_WIDTH) 
        camX = (METRO_WIDTH * TILE_SIZE) - SCREEN_WIDTH;
    if (camY > (METRO_HEIGHT * TILE_SIZE) - SCREEN_HEIGHT) 
        camY = (METRO_HEIGHT * TILE_SIZE) - SCREEN_HEIGHT;
    
    // Setup VDP
    VDP_setPalette(PAL0, palette_overworld);
    VDP_setPalette(PAL1, palette_ui);
    
    // Load tileset (placeholder - you'll replace with actual art)
    // VDP_loadTileSet(metro_tileset, 1, DMA);
    
    // Draw the visible portion of map
    updateCamera();
    renderMap();
    
    // Initialize portals
    memset(portals, 0, sizeof(portals));
    portalCount = 0;
    
    // Spawn first portal (tutorial encounter)
    spawnPortal(17, 13, ENEMY_SLASHER); // x=17, y=13
    
    // Setup Queeny sprite
    party[0].sprite = SPR_addSprite(&spr_queeny, 
        party[0].x - camX, party[0].y - camY, 
        TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
    
    // Display area name
    showMessage("SECTOR 7 - CENTRAL PLAZA");
}

void spawnPortal(u8 tileX, u8 tileY, EnemyType enemy) {
    if (portalCount >= MAX_PORTALS) return;
    
    Portal* p = &portals[portalCount++];
    p->x = tileX * TILE_SIZE + 8;  // Center in tile
    p->y = tileY * TILE_SIZE + 8;
    p->state = PORTAL_OPENING;
    p->timer = 0;
    p->enemy = enemy;
    p->triggered = FALSE;
    
    // Visual effect: portal appears
    // We'll add sprite/animation later
}

void updateOverworld() {
    u8 speed = 2;  // Walking speed
    bool moved = FALSE;
    
    // D-pad movement
    if (joyState & BUTTON_UP) {
        party[0].y -= speed;
        party[0].direction = DIR_NORTH;
        moved = TRUE;
    }
    else if (joyState & BUTTON_DOWN) {
        party[0].y += speed;
        party[0].direction = DIR_SOUTH;
        moved = TRUE;
    }
    
    if (joyState & BUTTON_LEFT) {
        party[0].x -= speed;
        party[0].direction = DIR_WEST;
        moved = TRUE;
    }
    else if (joyState & BUTTON_RIGHT) {
        party[0].x += speed;
        party[0].direction = DIR_EAST;
        moved = TRUE;
    }
    
    // Collision detection with walls/buildings
    if (moved) {
        checkCollision();
        checkEncounters();
        updateCamera();
    }
    
    // Update portal animations
    updatePortals();
    
    // Update sprite position
    SPR_setPosition(party[0].sprite, 
        party[0].x - camX, party[0].y - camY);
    
    // Button actions
    if (joyState & BUTTON_A) {
        // Interact/talk
        tryInteract();
    }
    if (joyState & BUTTON_B) {
        // Open menu
        changeState(STATE_MENU);
    }
    if (joyState & BUTTON_C) {
        // Run (if we implement that)
    }
    
    // Animate Queeny if moving
    if (moved) {
        // Cycle through walk frames
        // SPR_setAnim(party[0].sprite, (party[0].x / 8) % 4);
    }
}

void checkCollision() {
    // Get tile at Queeny's position
    u8 tileX = party[0].x / TILE_SIZE;
    u8 tileY = party[0].y / TILE_SIZE;
    
    // Bounds check
    if (tileX >= METRO_WIDTH) tileX = METRO_WIDTH - 1;
    if (tileY >= METRO_HEIGHT) tileY = METRO_HEIGHT - 1;
    
    u8 tile = metroMap[tileY][tileX];
    
    // Solid tiles: walls, buildings, water
    if (tile == TILE_WALL || tile == TILE_BUILDING || tile == TILE_WATER) {
        // Push back based on direction
        switch(party[0].direction) {
            case DIR_NORTH: party[0].y += 2; break;
            case DIR_SOUTH: party[0].y -= 2; break;
            case DIR_WEST:  party[0].x += 2; break;
            case DIR_EAST:  party[0].x -= 2; break;
        }
    }
}

void checkEncounters() {
    // Check portal collisions
    for (u8 i = 0; i < portalCount; i++) {
        Portal* p = &portals[i];
        if (p->state != PORTAL_ACTIVE) continue;
        
        // Distance check (16px radius)
        s16 dx = party[0].x - p->x;
        s16 dy = party[0].y - p->y;
        u16 dist = (dx * dx) + (dy * dy);  // Squared distance
        
        if (dist < 256) {  // 16^2
            // Portal collision! Start combat
            p->triggered = TRUE;
            startCombat(p->enemy, 1);  // 1 Slasher
            return;
        }
    }
}

void updatePortals() {
    portalAnimTimer++;
    
    for (u8 i = 0; i < portalCount; i++) {
        Portal* p = &portals[i];
        
        switch(p->state) {
            case PORTAL_OPENING:
                // 60 frame animation (1 second)
                p->timer++;
                if (p->timer > 60) {
                    p->state = PORTAL_ACTIVE;
                    // Visual: portal fully open, pulsing
                }
                break;
                
            case PORTAL_ACTIVE:
                // Pulsing effect
                // Change palette or sprite frame based on timer
                break;
        }
    }
}

void updateCamera() {
    // Center camera on Queeny
    camX = party[0].x - SCREEN_WIDTH/2;
    camY = party[0].y - SCREEN_HEIGHT/2;
    
    // Clamp to map bounds
    s16 maxCamX = (METRO_WIDTH * TILE_SIZE) - SCREEN_WIDTH;
    s16 maxCamY = (METRO_HEIGHT * TILE_SIZE) - SCREEN_HEIGHT;
    
    if (camX < 0) camX = 0;
    if (camY < 0) camY = 0;
    if (camX > maxCamX) camX = maxCamX;
    if (camY > maxCamY) camY = maxCamY;
    
    // Update VDP scroll
    VDP_setHorizontalScroll(BG_A, -camX);
    VDP_setVerticalScroll(BG_A, camY);
}

void renderMap() {
    // Draw visible tiles to background
    // This is simplified - real implementation would use tilemaps
    u16 startCol = camX / TILE_SIZE;
    u16 startRow = camY / TILE_SIZE;
    
    for (u16 row = 0; row < (SCREEN_HEIGHT/TILE_SIZE) + 2; row++) {
        for (u16 col = 0; col < (SCREEN_WIDTH/TILE_SIZE) + 2; col++) {
            u16 mapX = startCol + col;
            u16 mapY = startRow + row;
            
            if (mapX >= METRO_WIDTH || mapY >= METRO_HEIGHT) continue;
            
            u8 tile = metroMap[mapY][mapX];
            // VDP_setTileMapXY(BG_A, tile, col, row);
        }
    }
}

void tryInteract() {
    // Check NPCs, objects in front of Queeny
    // For now, just a debug message
    showMessage("Nothing here...");
}

void cleanupOverworld() {
    SPR_releaseSprite(party[0].sprite);
    VDP_clearPlan(BG_A, TRUE);
    clearMessage();
}
