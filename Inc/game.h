here
#ifndef _GAME_H_
#define _GAME_H_

#include <genesis.h>

// Basic definitions for future expansion
#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   224

// Game states
typedef enum {
    STATE_TITLE,
    STATE_OVERWORLD,
    STATE_COMBAT
} GameState;

#endif


