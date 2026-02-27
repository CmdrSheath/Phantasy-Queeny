//==============================================================================
// PHANTASY QUEENY
// A Sega Genesis RPG based on the comic series
// Main entry point and game state manager
//==============================================================================

#include <genesis.h>
#include "game.h"

// Game state machine
GameState currentState;
GameState previousState;

// System initialization
void initSystem() {
    SYS_init();
    VDP_setScreenWidth320();
    VDP_setPlanSize(64, 32);
    SPR_init();
    JOY_init();
    
    // Initialize random seed
    setRandomSeed(0xC427); // Comic reference? Change if needed
    
    // Load palette
    VDP_setPalette(PAL0, palette_all_black);
}

// State transition
void changeState(GameState newState) {
    previousState = currentState;
    currentState = newState;
    
    // Cleanup previous state
    switch(previousState) {
        case STATE_TITLE:     cleanupTitle(); break;
        case STATE_OVERWORLD: cleanupOverworld(); break;
        case STATE_DUNGEON:   cleanupDungeon(); break;
        case STATE_COMBAT:    cleanupCombat(); break;
        case STATE_MENU:      cleanupMenu(); break;
        case STATE_CUTSCENE:  cleanupCutscene(); break;
    }
    
    // Initialize new state
    switch(newState) {
        case STATE_TITLE:     initTitle(); break;
        case STATE_OVERWORLD: initOverworld(); break;
        case STATE_DUNGEON:   initDungeon(); break;
        case STATE_COMBAT:    initCombat(); break;
        case STATE_MENU:      initMenu(); break;
        case STATE_CUTSCENE:  initCutscene(); break;
    }
}

int main(bool hardReset) {
    initSystem();
    
    // Start with title screen
    changeState(STATE_TITLE);
    
    // Main game loop - 60 FPS
    while(TRUE) {
        // Read input
        joyState = JOY_readJoypad(JOY_1);
        
        // Update current state
        switch(currentState) {
            case STATE_TITLE:     updateTitle(); break;
            case STATE_OVERWORLD: updateOverworld(); break;
            case STATE_DUNGEON:   updateDungeon(); break;
            case STATE_COMBAT:    updateCombat(); break;
            case STATE_MENU:      updateMenu(); break;
            case STATE_CUTSCENE:  updateCutscene(); break;
        }
        
        // Update sprites and VDP
        SPR_update();
        SYS_doVBlankProcess();
    }
    
    return 0;
}

