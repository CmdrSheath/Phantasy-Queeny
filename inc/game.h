#ifndef _GAME_H_
#define _GAME_H_

#include <genesis.h>
#include <stdbool.h>

// Type definitions
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

// Game states
typedef enum {
    STATE_TITLE,
    STATE_CUTSCENE,
    STATE_OVERWORLD,
    STATE_COMBAT,
    STATE_DUNGEON
} GameState;

// Party member struct (minimal)
typedef struct {
    char name[12];
} PartyMember;

// Function prototypes for state management
void changeState(GameState newState);
void initGame(void);
void updateGame(void);

// State-specific functions
void initTitle(void);
void updateTitle(void);
void cleanupTitle(void);

void initCutscene(void);
void updateCutscene(void);
void cleanupCutscene(void);

void initOverworld(void);
void updateOverworld(void);
void cleanupOverworld(void);

void initCombat(void);
void updateCombat(void);
void cleanupCombat(void);

// UI functions
void drawText(u16 x, u16 y, const char* text);
void showMessage(const char* text);
void clearMessage(void);

#endif
