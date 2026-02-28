#ifndef _GAME_H_
#define _GAME_H_

#include <genesis.h>

//==============================================================================
// GAME CONSTANTS
//==============================================================================

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   224
#define TILE_SIZE       16

// Maximum values
#define MAX_PARTY_SIZE      4
#define MAX_INVENTORY       32
#define MAX_ENEMIES         4
#define MAX_MAP_WIDTH       64
#define MAX_MAP_HEIGHT      64

//==============================================================================
// GAME STATES
//==============================================================================

typedef enum {
    STATE_TITLE,
    STATE_OVERWORLD,
    STATE_DUNGEON,
    STATE_COMBAT,
    STATE_MENU,
    STATE_CUTSCENE
} GameState;

//==============================================================================
// CHARACTER STATS (Phantasy Star style)
//==============================================================================

typedef struct {
    u8  hp;              // Current HP
    u8  maxHp;           // Maximum HP
    u8  mp;              // Current MP/TP
    u8  maxMp;           // Maximum MP/TP
    u8  strength;        // Physical attack power
    u8  mental;          // Tech/magic power
    u8  agility;         // Speed, dodge, turn order
    u8  defense;         // Damage reduction
    u16 exp;             // Experience points
    u8  level;           // Current level (1-99)
} Stats;

//==============================================================================
// PARTY MEMBER
//==============================================================================

typedef struct {
    char name[12];
    Stats stats;
    u8  equipment[4];    // Weapon, Armor, Shield, Item
    u8  techniques[8];   // Learned techs/spells
    Sprite* sprite;
    u16 x, y;            // Position on map
    u8  direction;       // Facing direction
    bool active;         // In party?
} PartyMember;

//==============================================================================
// QUEENY-SPECIFIC: Heroine/Antagonist Duality
//==============================================================================

typedef enum {
    ALIGN_HEROINE,       // Protagonist mode
    ALIGN_ANTAGONIST,    // Villain mode (story branches?)
    ALIGN_NEUTRAL        // Player choice matters
} QueenyAlignment;

typedef struct {
    PartyMember base;
    QueenyAlignment alignment;
    u8 corruption;       // 0-255, affects story/events
    u8 redemption;       // 0-255, counter to corruption
    bool hasTransformed; // Unlocked final form?
} QueenyCharacter;

//==============================================================================
// ENEMY
//==============================================================================

typedef enum {
    ENEMY_SLIME,
    ENEMY_SKELETON,
    ENEMY_DARK_MAGE,
    ENEMY_QUEENY_SHADOW, // Mirror battle?
    ENEMY_COUNT
} EnemyType;

typedef struct {
    EnemyType type;
    char name[16];
    Stats stats;
    u16 expReward;
    u8  itemDrop;
    u8  dropRate;        // Percentage
    Sprite* sprite;
    bool isBoss;
} Enemy;

//==============================================================================
// COMBAT SYSTEM
//==============================================================================

typedef enum {
    CMD_ATTACK,
    CMD_TECHNIQUE,
    CMD_ITEM,
    CMD_DEFEND,
    CMD_FLEE
} CombatCommand;

typedef struct {
    u8 actorIndex;       // Who's acting
    bool isPlayer;       // Player or enemy?
    CombatCommand command;
    u8 target;           // Target index
    u8 subIndex;         // Which tech/item
} CombatAction;

//==============================================================================
// WORLD/MAP
//==============================================================================

typedef enum {
    TERRAIN_PLAINS,
    TERRAIN_FOREST,
    TERRAIN_DESERT,
    TERRAIN_MOUNTAIN,
    TERRAIN_WATER
} TerrainType;

typedef struct {
    u8 width, height;
    u8 tiles[MAX_MAP_HEIGHT][MAX_MAP_WIDTH];
    u8 terrain[MAX_MAP_HEIGHT][MAX_MAP_WIDTH];
    u8 encounterRate;    // Random battle chance
    EnemyType enemies[4];// Possible encounters
} Map;


// Directions
#define DIR_NORTH   0
#define DIR_SOUTH   1
#define DIR_EAST    2
#define DIR_WEST    3

// Items
#define ITEM_NONE           0
#define ITEM_WOOD_SWORD     1
#define ITEM_LEATHER_ARMOR  2
#define ITEM_CRYSTAL_SHARD  3

// Techniques
#define TECH_NONE       0
#define TECH_HEAL       1  // Heroine path - restore HP
#define TECH_DRAIN      2  // Antagonist path - damage + heal
#define TECH_NEUTRAL    3  // Balanced path - basic attack boost

// Function prototypes to add:
void initOverworld(void);
void updateOverworld(void);
void cleanupOverworld(void);
void spawnPortal(u8 tileX, u8 tileY, EnemyType enemy);
void startCombat(EnemyType type, u8 count);
void initCombat(void);
void updateCombat(void);
void cleanupCombat(void);


//==============================================================================
// GLOBAL VARIABLES (extern - defined in game.c)
//==============================================================================

extern GameState currentState;
extern GameState previousState;
extern u16 joyState;

// Party
extern PartyMember party[MAX_PARTY_SIZE];
extern QueenyCharacter queeny;
extern u8 partyCount;

// Game state
extern u32 playTime;     // Frames (divide by 60 for seconds)
extern u16 gold;
extern u8 inventory[MAX_INVENTORY];
extern u8 inventoryCount;

// Current location
extern Map* currentMap;
extern u16 cameraX, cameraY;

// Combat
extern bool inCombat;
extern Enemy currentEnemies[MAX_ENEMIES];
extern u8 enemyCount;
extern CombatAction actionQueue[8];
extern u8 actionCount;

// Palettes
extern const u16 palette_all_black[16];
extern const u16 palette_ui[16];
extern const u16 palette_overworld[16];

//==============================================================================
// FUNCTION PROTOTYPES
//==============================================================================

// System
void changeState(GameState newState);

// Title screen
void initTitle(void);
void updateTitle(void);
void cleanupTitle(void);

// Overworld
void initOverworld(void);
void updateOverworld(void);
void cleanupOverworld(void);
void loadMap(u8 mapId);
void checkEncounters(void);

// Dungeon (First-person)
void initDungeon(void);
void updateDungeon(void);
void cleanupDungeon(void);
void renderDungeonView(void);
void moveDungeon(u8 direction);

// Combat
void initCombat(void);
void updateCombat(void);
void cleanupCombat(void);
void startCombat(EnemyType type, u8 count);
void executeCombatRound(void);
u8 calculateTurnOrder(void);

// UI
void initMenu(void);
void updateMenu(void);
void cleanupMenu(void);
void drawText(u16 x, u16 y, const char* text);
void drawNumber(u16 x, u16 y, u16 number, u8 digits);
void showMessage(const char* text);
void clearMessage(void);

// Cutscenes
void initCutscene(void);
void updateCutscene(void);
void cleanupCutscene(void);
void startCutscene(u8 cutsceneId);

#endif // _GAME_H_

