//==============================================================================
// PHANTASY QUEENY - Core Game Data
//==============================================================================

#include "game.h"

// Input state
u16 joyState = 0;

// Party data
PartyMember party[MAX_PARTY_SIZE];
QueenyCharacter queeny;
u8 partyCount = 0;

// Game progress
u32 playTime = 0;
u16 gold = 0;
u8 inventory[MAX_INVENTORY];
u8 inventoryCount = 0;

// Map data
Map* currentMap = NULL;
u16 cameraX = 0, cameraY = 0;

// Combat state
bool inCombat = FALSE;
Enemy currentEnemies[MAX_ENEMIES];
u8 enemyCount = 0;
CombatAction actionQueue[8];
u8 actionCount = 0;

// Palettes (placeholder - replace with your colors)
const u16 palette_all_black[16] = {
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000
};

const u16 palette_ui[16] = {
    0x0000, // 0: Transparent/Black
    0x0EEE, // 1: White
    0x00E0, // 2: Green (HP)
    0x000E, // 3: Blue (MP)
    0x0E00, // 4: Red (Damage)
    0x00EE, // 5: Cyan
    0x0E0E, // 6: Magenta
    0x0EE0, // 7: Yellow
    0x0444, // 8: Dark gray
    0x0888, // 9: Light gray
    0x0E80, // 10: Orange
    0x0800, // 11: Dark red
    0x0008, // 12: Dark blue
    0x0080, // 13: Dark green
    0x0EA0, // 14: Gold
    0x0666  // 15: Medium gray
};

const u16 palette_overworld[16] = {
    0x0000, // Transparent
    0x0EEE, // White (clouds)
    0x00E0, // Green (grass)
    0x0080, // Dark green (forest)
    0x0E00, // Red (lava/damage)
    0x000E, // Blue (water)
    0x0008, // Dark blue (deep water)
    0x0EE0, // Yellow (sand)
    0x0E80, // Orange (desert)
    0x0444, // Gray (mountain)
    0x0888, // Light gray (snow)
    0x0E0E, // Purple (magic areas)
    0x00EE, // Cyan (ice)
    0x0800, // Brown (dirt)
    0x0EA0, // Gold (treasure)
    0x0666  // Dark gray (rocks)
};

// Initialize Queeny as the main character
void initQueeny() {
    // Copy name
    strcpy(queeny.base.name, "QUEENY");
    
    // Starting stats (Level 1)
    queeny.base.stats.hp = 25;
    queeny.base.stats.maxHp = 25;
    queeny.base.stats.mp = 10;
    queeny.base.stats.maxMp = 10;
    queeny.base.stats.strength = 8;
    queeny.base.stats.mental = 6;
    queeny.base.stats.agility = 7;
    queeny.base.stats.defense = 5;
    queeny.base.stats.exp = 0;
    queeny.base.stats.level = 1;
    
    // Starting equipment
    queeny.base.equipment[0] = ITEM_WOOD_SWORD;
    queeny.base.equipment[1] = ITEM_LEATHER_ARMOR;
    queeny.base.equipment[2] = ITEM_NONE;
    queeny.base.equipment[3] = ITEM_NONE;
    
    // No techniques yet (learn at level 2+)
    memset(queeny.base.techniques, 0, sizeof(queeny.base.techniques));
    
    // Position
    queeny.base.x = 10;
    queeny.base.y = 10;
    queeny.base.direction = DIR_SOUTH;
    queeny.base.active = TRUE;
    
    // Story state
    queeny.alignment = ALIGN_HEROINE; // Starts as heroine
    queeny.corruption = 0;
    queeny.redemption = 0;
    queeny.hasTransformed = FALSE;
    
    // Add to party
    party[0] = queeny.base;
    partyCount = 1;
}


