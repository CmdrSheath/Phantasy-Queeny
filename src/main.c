//==============================================================================
// PHANTASY QUEENY - Sega Genesis RPG
// Main entry point with state management
//==============================================================================

#include <genesis.h>
#include <string.h>
#include "game.h"

// Current game state
GameState currentState = STATE_TITLE;
GameState nextState = STATE_TITLE;
bool stateInitialized = false;

// External function from game.c
extern void initGameSystem(void);

int main(bool hardReset)
{
    // Initialize system
    VDP_setScreenWidth320();
    VDP_setBackgroundColor(0);
    PAL_setColor(0, 0x0000); // Black background
    
    // Initialize joypad
    JOY_init();
    
    // Initialize game (calls the one in game.c)
    initGameSystem();
    
    // Start with title screen
    changeState(STATE_TITLE);
    
    // Main game loop
    while(1)
    {
        // Update current state
        updateGame();
        
        // Wait for vertical blank (60 FPS)
        SYS_doVBlankProcess();
    }
    
    return 0;
}

void changeState(GameState newState)
{
    // Clean up current state if needed
    if (stateInitialized) {
        switch(currentState) {
            case STATE_TITLE:
                cleanupTitle();
                break;
            case STATE_CUTSCENE:
                cleanupCutscene();
                break;
            case STATE_OVERWORLD:
                cleanupOverworld();
                break;
            case STATE_COMBAT:
                cleanupCombat();
                break;
            case STATE_PAUSE_MENU:
                cleanupPauseMenu();
                break;

            default:
                break;
        }
    }
    
    // Switch to new state
    currentState = newState;
    stateInitialized = false;
}

void updateGame(void)
{
    // Initialize state if first time
    if (!stateInitialized) {
        switch(currentState) {
            case STATE_TITLE:
                initTitle();
                break;
            case STATE_CUTSCENE:
                initCutscene();
                break;
            case STATE_OVERWORLD:
                initOverworld();
                break;
            case STATE_COMBAT:
                initCombat();
                break;
            case STATE_PAUSE_MENU:
                initPauseMenu();
                 break;
            default:
                break;
        }
        stateInitialized = true;
    }
    

    updateAudio();

    // Update current state
    switch(currentState) {
        case STATE_TITLE:
            updateTitle();
            break;
        case STATE_CUTSCENE:
            updateCutscene();
            break;
        case STATE_OVERWORLD:
            updateOverworld();
            break;
        case STATE_COMBAT:
            updateCombat();
            break;
        case STATE_PAUSE_MENU:
            updatePauseMenu();
            break;
        default:
            break;
    }

    updateMessageTimer();
}
