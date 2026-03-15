//==============================================================================
// PHANTASY QUEENY - Simplified Audio System
// Using SGDK PSG for chiptune sounds
//==============================================================================

#include "audio.h"
#include "game.h"

// Simple tone sequences for music
static u16 musicTimer = 0;
static u8 musicStep = 0;
static bool musicPlaying = FALSE;
static MusicTrack currentTrack = MUSIC_TITLE;

// Note frequencies (NTSC) for PSG
// A4 = 440Hz, PSG frequency = 3579545 / (32 * Hz)
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
#define NOTE_REST 0

// Your melody: a a b c d e f g g f e d c b a a
static const u16 overworldMelody[] = {
    NOTE_A4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4,
    NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_B3, NOTE_A3, NOTE_A3
};

// Counter: a a - - a a - - a a - - a a - -
static const u16 counterMelody[] = {
    NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST, NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST,
    NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST, NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST
};

void initAudio(void) {

void initAudio(void) {
    // Visual debug - flash screen white briefly
    PAL_setColor(0, 0x0EEE);  // White
    waitMs(100);
    PAL_setColor(0, 0x0000);  // Back to black
    
    YM2612_reset();
    PSG_reset();
    musicTimer = 0;
    musicStep = 0;
    musicPlaying = FALSE;
    
    // Debug beep on startup
    PSG_setTone(0, NOTE_A4);
    PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
    waitMs(100);
    PSG_setTone(0, NOTE_REST);
}

void playMusic(MusicTrack track) {
    currentTrack = track;
    musicStep = 0;
    musicTimer = 0;
    musicPlaying = TRUE;
    
    // Debug: play a test tone when music starts
    PSG_setTone(1, NOTE_C5);
    PSG_setEnvelope(1, PSG_ENVELOPE_MIN);
}

void stopMusic(void) {
    musicPlaying = FALSE;
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
            
        case SFX_COMBAT_START:
            PSG_setTone(2, NOTE_G4);
            PSG_setEnvelope(2, PSG_ENVELOPE_MIN);
            break;
            
        default:
            break;
    }
}

void updateAudio(void) {
    if (!musicPlaying) return;
    
    musicTimer++;
    if (musicTimer < 30) return;  // 30 frames = 0.5s per note (120 BPM)
    musicTimer = 0;
    
    // Play main melody on channel 0
    u16 note = overworldMelody[musicStep];
    if (note != NOTE_REST) {
        PSG_setTone(0, note);
        PSG_setEnvelope(0, PSG_ENVELOPE_ALTERNATE);  // Sustained tone
    } else {
        PSG_setTone(0, NOTE_REST);
    }
    
    // Play counter melody on channel 1 (one octave lower, same rhythm)
    u16 bassNote = counterMelody[musicStep];
    if (bassNote != NOTE_REST) {
        PSG_setTone(1, bassNote);
        PSG_setEnvelope(1, PSG_ENVELOPE_ALTERNATE);
    } else {
        PSG_setTone(1, NOTE_REST);
    }
    
    musicStep++;
    if (musicStep >= 16) musicStep = 0;  // Loop 16-step pattern
}
