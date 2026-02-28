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
    STATE_OVERWORLD,
    STATE_COMBAT
} GameState;

// Party member struct (minimal)
typedef struct {
    char name[12];
} PartyMember;

#endif
