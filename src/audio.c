//==============================================================================
// PHANTASY QUEENY - Audio System
// Organized by scene for easy editing
//==============================================================================

#include "audio.h"
#include "game.h"

//==============================================================================
// INCLUDES, DEFINES, AND STATIC VARIABLES
//==============================================================================

// Sequencer state
static u16 musicTimer = 0;
static u8 musicStep = 0;
static bool musicPlaying = FALSE;
static MusicTrack currentTrack = MUSIC_TITLE;

// Note frequencies (NTSC PSG) - 3579545 / (32 * Hz)
#define NOTE_A2  812
#define NOTE_B2  724
#define NOTE_C3  684
#define NOTE_D3  610
#define NOTE_E3  543
#define NOTE_F3  513
#define NOTE_G3  457
#define NOTE_A3  406
#define NOTE_B3  362
#define NOTE_C4  342
#define NOTE_D4  305
#define NOTE_E4  271
#define NOTE_F4  256
#define NOTE_G4  228
#define NOTE_A4  203
#define NOTE_B4  181
#define NOTE_C5  171
#define NOTE_D5  152
#define NOTE_E5  136
#define NOTE_F5  128
#define NOTE_G5  114
#define NOTE_REST 0


//==============================================================================
// MUSIC DATA - ORGANIZED BY SCENE
// Format: Each scene has MELODY (channel 0) and COUNTER/BASS (channel 1)
// Add new scenes here following the same pattern
//==============================================================================

// -----------------------------------------------------------------------------
// --- TITLE SCREEN MUSIC ---
// Slow, mysterious sci-fi theme
// Melody: d d e f g f e d c d e c d d - -
// Counter: d - - - d - - - d - - - d - - -
// -----------------------------------------------------------------------------

// Title melody: d d e f g f e d c d e c d d - -
static const u16 titleMelody[] = {
    NOTE_D4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
    NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_D4, NOTE_REST, NOTE_REST
};

// Title counter: d - - - d - - - d - - - d - - -
static const u16 titleCounter[] = {
    NOTE_D3, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_D3, NOTE_REST, NOTE_REST, NOTE_REST,
    NOTE_D3, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_D3, NOTE_REST, NOTE_REST, NOTE_REST
};


// -----------------------------------------------------------------------------
// --- CUTSCENE MUSIC ---
// Ambient, storytelling mood
// Melody: c e g c5 g e c e g c5 g e c - - -
// Counter: c - g - c - g - c - g - c - - -
// -----------------------------------------------------------------------------

// Cutscene melody: c e g c5 g e c e g c5 g e c - - -
static const u16 cutsceneMelody[] = {
    NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_E4,
    NOTE_G4, NOTE_C5, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_REST, NOTE_REST, NOTE_REST
};

// Cutscene counter: c - g - c - g - c - g - c - - -
static const u16 cutsceneCounter[] = {
    NOTE_C3, NOTE_REST, NOTE_G3, NOTE_REST, NOTE_C3, NOTE_REST, NOTE_G3, NOTE_REST,
    NOTE_C3, NOTE_REST, NOTE_G3, NOTE_REST, NOTE_C3, NOTE_REST, NOTE_REST, NOTE_REST
};


// -----------------------------------------------------------------------------
// --- OVERWORLD MUSIC ---
// Upbeat exploration theme (your original composition)
// Melody: a a b c d e f g g f e d c b a a
// Counter: a a - - a a - - a a - - a a - -
// -----------------------------------------------------------------------------

// Overworld melody: a a b c d e f g g f e d c b a a
static const u16 overworldMelody[] = {
    NOTE_A4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4,
    NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_B3, NOTE_A3, NOTE_A3
};

// Overworld counter: a a - - a a - - a a - - a a - -
static const u16 overworldCounter[] = {
    NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST, NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST,
    NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST, NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST
};


// -----------------------------------------------------------------------------
// --- COMBAT MUSIC ---
// Intense, fast-paced battle theme
// Melody: e e g e d c d e g a g e d c d e
// Counter: e - e - e - e - e - e - e - e -
// -----------------------------------------------------------------------------

// Combat melody: e e g e d c d e g a g e d c d e
static const u16 combatMelody[] = {
    NOTE_E4, NOTE_E4, NOTE_G4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4,
    NOTE_G4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4
};

// Combat counter: e - e - e - e - e - e - e - e - e -
static const u16 combatCounter[] = {
    NOTE_E3, NOTE_REST, NOTE_E3, NOTE_REST, NOTE_E3, NOTE_REST, NOTE_E3, NOTE_REST,
    NOTE_E3, NOTE_REST, NOTE_E3, NOTE_REST, NOTE_E3, NOTE_REST, NOTE_E3, NOTE_REST
};


// -----------------------------------------------------------------------------
// --- DUNGEON MUSIC ---
// Dark, atmospheric exploration
// Melody: a g f e d c d e f g a - - - - -
// Counter: a - a - a - a - a - a - - - - -
// -----------------------------------------------------------------------------

// Dungeon melody: a g f e d c d e f g a - - - - -
static const u16 dungeonMelody[] = {
    NOTE_A3, NOTE_G3, NOTE_F3, NOTE_E3, NOTE_D3, NOTE_C3, NOTE_D3, NOTE_E3,
    NOTE_F3, NOTE_G3, NOTE_A3, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST
};

// Dungeon counter: a - a - a - a - a - a - a - - - - -
static const u16 dungeonCounter[] = {
    NOTE_A2, NOTE_REST, NOTE_A2, NOTE_REST, NOTE_A2, NOTE_REST, NOTE_A2, NOTE_REST,
    NOTE_A2, NOTE_REST, NOTE_A2, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST
};


// -----------------------------------------------------------------------------
// --- VICTORY MUSIC ---
// Short fanfare when winning combat
// Melody: c e g c5 - g e c - - - - - - - -
// Counter: c - - - - - - - - - - - - - - -
// -----------------------------------------------------------------------------

// Victory melody: c e g c5 - g e c - - - - - - - -
static const u16 victoryMelody[] = {
    NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_REST, NOTE_G4, NOTE_E4, NOTE_C4,
    NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST
};

// Victory counter: c - - - - - - - - - - - - - - -
static const u16 victoryCounter[] = {
    NOTE_C3, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST,
    NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST
};


//==============================================================================
// MUSIC TRACK STRUCTURE
// Groups melody and counter for each scene
//==============================================================================

typedef struct {
    const u16* melody;
    const u16* counter;
    u8 tempo;  // Frames per step (lower = faster)
} MusicTrackData;

// Track lookup table - add new tracks here
static const MusicTrackData musicTracks[] = {
    [MUSIC_TITLE]    = {titleMelody,    titleCounter,    40},  // Slower, mysterious
    [MUSIC_CUTSCENE] = {cutsceneMelody, cutsceneCounter, 35},  // Medium, ambient
    [MUSIC_OVERWORLD]= {overworldMelody,overworldCounter,30},  // Faster, upbeat
    [MUSIC_COMBAT]   = {combatMelody,   combatCounter,   20},  // Fast, intense
    [MUSIC_DUNGEON]  = {dungeonMelody,  dungeonCounter,  45},  // Slow, atmospheric
    [MUSIC_VICTORY]  = {victoryMelody,  victoryCounter,  25}   // Medium, triumphant
};


//==============================================================================
// FUNCTION IMPLEMENTATIONS
//==============================================================================

void initAudio(void) {
    PSG_reset();
    musicTimer = 0;
    musicStep = 0;
    musicPlaying = FALSE;
    currentTrack = MUSIC_TITLE;
    
    // Startup beep to confirm audio working
    PSG_setTone(0, NOTE_A4);
    PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
    waitMs(100);
    PSG_setTone(0, NOTE_REST);
}

void playMusic(MusicTrack track) {
    // Stop current music first
    stopMusic();
    
    currentTrack = track;
    musicStep = 0;
    musicTimer = 0;
    musicPlaying = TRUE;
}

void stopMusic(void) {
    musicPlaying = FALSE;
    // Silence all PSG channels
    PSG_setTone(0, NOTE_REST);
    PSG_setTone(1, NOTE_REST);
    PSG_setTone(2, NOTE_REST);
    PSG_setTone(3, NOTE_REST);
}

void pauseMusic(void) {
    musicPlaying = FALSE;
}

void resumeMusic(void) {
    musicPlaying = TRUE;
}

void playSFX(SoundEffect sfx) {
    switch(sfx) {
        case SFX_MENU_MOVE:
            PSG_setTone(2, NOTE_E4);
            PSG_setEnvelope(2, PSG_ENVELOPE_MIN);
            break;
            
        case SFX_MENU_SELECT:
            PSG_setTone(2, NOTE_A4);
            PSG_setEnvelope(2, PSG_ENVELOPE_MIN);
            break;
            
        case SFX_MENU_CANCEL:
            PSG_setTone(2, NOTE_C4);
            PSG_setEnvelope(2, PSG_ENVELOPE_MIN);
            break;
            
        case SFX_PORTAL_HUM:
            PSG_setTone(2, NOTE_G3);
            PSG_setEnvelope(2, PSG_ENVELOPE_MIN);
            break;
            
        case SFX_COMBAT_START:
            PSG_setTone(2, NOTE_E4);
            PSG_setEnvelope(2, PSG_ENVELOPE_MIN);
            break;
            
        case SFX_ATTACK:
            PSG_setTone(3, NOTE_A3);
            PSG_setEnvelope(3, PSG_ENVELOPE_MIN);
            break;
            
        case SFX_MAGIC:
            PSG_setTone(2, NOTE_C5);
            PSG_setEnvelope(2, PSG_ENVELOPE_MIN);
            break;
            
        case SFX_DAMAGE:
            PSG_setTone(3, NOTE_E3);
            PSG_setEnvelope(3, PSG_ENVELOPE_MIN);
            break;
            
        case SFX_VICTORY:
            PSG_setTone(2, NOTE_C5);
            PSG_setEnvelope(2, PSG_ENVELOPE_MIN);
            break;
            
        default:
            break;
    }
}

void updateAudio(void) {
    if (!musicPlaying) return;
    if (currentTrack >= MUSIC_COUNT) return;
    
    const MusicTrackData* track = &musicTracks[currentTrack];
    
    musicTimer++;
    if (musicTimer < track->tempo) return;
    musicTimer = 0;
    
    // Play melody on channel 0
    u16 note = track->melody[musicStep];
    if (note != NOTE_REST) {
        PSG_setTone(0, note);
        PSG_setEnvelope(0, PSG_ENVELOPE_MAX);
    } else {
        PSG_setTone(0, NOTE_REST);
    }
    
    // Play counter/bass on channel 1
    u16 bassNote = track->counter[musicStep];
    if (bassNote != NOTE_REST) {
        PSG_setTone(1, bassNote);
        PSG_setEnvelope(1, PSG_ENVELOPE_MAX);
    } else {
        PSG_setTone(1, NOTE_REST);
    }
    
    musicStep++;
    if (musicStep >= 16) musicStep = 0;  // Loop 16-step pattern
}
