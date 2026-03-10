//==============================================================================
// PHANTASY QUEENY - Game System Initialization
//==============================================================================

#include "game.h"
#include <genesis.h>
#include "audio.h"

void initGameSystem(void) {
    // Global initialization
    // Seed random number generator
    setRandomSeed(getTick());
    initAudio();
    playMusic(MUSIC_TITLE);
}
