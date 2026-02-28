#ifndef _GAME_H_
#define _GAME_H_

#include <genesis.h>

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   224

typedef enum {
    STATE_TITLE,
    STATE_OVERWORLD,
    STATE_COMBAT
} GameState;

#endif
