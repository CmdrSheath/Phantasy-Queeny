//==============================================================================
// PHANTASY QUEENY - Game System Initialization
//==============================================================================

#include "game.h"
#include <genesis.h>

void initGameSystem(void) {
    // Global initialization
    // Seed random number generator
    setRandomSeed(getTick());
}
