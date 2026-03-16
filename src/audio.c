//==============================================================================
// PHANTASY QUEENY - Audio System with Visual Debug
//==============================================================================

#include "audio.h"
#include "game.h"

// Sequencer state
static u16 musicTimer = 0;
static u8 musicStep = 0;
static bool musicPlaying = FALSE;
static MusicTrack currentTrack = MUSIC_TITLE;

// Note frequencies
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

// Debug visual
static u8 debugSFXCount = 0;
static u8 debugTimer = 0;

// --- MUSIC DATA ---
static const u16 titleMelody[] = {
    NOTE_D4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
    NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_D4, NOTE_REST, NOTE_REST
};
static const u16 titleCounter[] = {
    NOTE_D3, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_D3, NOTE_REST, NOTE_REST, NOTE_REST,
    NOTE_D3, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_D3, NOTE_REST, NOTE_REST, NOTE_REST
};
static const u16 overworldMelody[] = {
    NOTE_A4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4,
    NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_B3, NOTE_A3, NOTE_A3
};
static const u16 overworldCounter[] = {
    NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST, NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST,
    NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST, NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST
};
static const u16 combatMelody[] = {
    NOTE_E4, NOTE_E4, NOTE_G4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4,
    NOTE_G4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4
};
static const u16 combatCounter[] = {
    NOTE_E3, NOTE_REST, NOTE_E3, NOTE_REST, NOTE_E3, NOTE_REST, NOTE_E3, NOTE_REST,
    NOTE_E3, NOTE_REST, NOTE_E3, NOTE_REST, NOTE_E3, NOTE_REST, NOTE_E3, NOTE_REST
};

typedef struct {
    const u16* melody;
    const u16* counter;
    u8 tempo;
} MusicTrackData;

static const MusicTrackData musicTracks[] = {
    [MUSIC_TITLE]     = {titleMelody,     titleCounter,     40},
    [MUSIC_OVERWORLD]= {overworldMelody, overworldCounter, 30},
    [MUSIC_COMBAT]   = {combatMelody,    combatCounter,    20}
};

void initAudio(void) {
    PSG_reset();
    musicTimer = 0;
    musicStep = 0;
    musicPlaying = FALSE;
    currentTrack = MUSIC_TITLE;
    debugSFXCount = 0;
    debugTimer = 0;
}

void playMusic(MusicTrack track) {
    stopMusic();
    currentTrack = track;
    musicStep = 0;
    musicTimer = 0;
    musicPlaying = TRUE;
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
    // Visual debug: flash bottom left corner
    debugSFXCount++;
    debugTimer = 30;  // Show for 30 frames (0.5 seconds)
    
    // Draw debug indicator
    char debugStr[4];
    debugStr[0] = 'S';
    debugStr[1] = '0' + (debugSFXCount % 10);
    debugStr[2] = '\0';
    VDP_drawTextBG(BG_B, debugStr, 0, 26);  // Bottom left
    
    // Flash color
    PAL_setColor(15, 0x0EEE);  // White
    
    // Try multiple methods to play sound
    
    // Method 1: Direct PSG register write (bypass SGDK)
    volatile u16* psg = (volatile u16*)0xC00011;
    
    switch(sfx) {
        case SFX_MENU_MOVE:
            *psg = 0x9E4;  // Channel 2, tone E4
            *psg = 0xB08;  // Volume 8
            break;
        case SFX_MENU_SELECT:
            *psg = 0x9CB;  // Channel 2, tone A4
            *psg = 0xB06;  // Volume 6
            break;
        case SFX_MENU_CANCEL:
            *psg = 0x9D5;  // Channel 2, tone C4
            *psg = 0xB08;
            break;
        case SFX_PORTAL_HUM:
            *psg = 0x9E5;  // Channel 2, tone G3
            *psg = 0xB04;
            break;
        default:
            *psg = 0x9C0;  // Channel 2, tone A4
            *psg = 0xB08;
            break;
    }
}

void updateAudio(void) {
    // Handle debug display
    if (debugTimer > 0) {
        debugTimer--;
        if (debugTimer == 0) {
            VDP_clearText(0, 26, 2);  // Clear debug
            PAL_setColor(15, 0x0EEE);  // Reset white
        } else if (debugTimer == 15) {
            PAL_setColor(15, 0x0E00);  // Flash red mid-way
        }
    }
    
    // Music sequencer
    if (!musicPlaying) return;
    if (currentTrack >= MUSIC_COUNT) return;
    
    const MusicTrackData* track = &musicTracks[currentTrack];
    
    musicTimer++;
    if (musicTimer < track->tempo) return;
    musicTimer = 0;
    
    u16 note = track->melody[musicStep];
    if (note != NOTE_REST) {
        PSG_setTone(0, note);
        PSG_setEnvelope(0, PSG_ENVELOPE_MAX);
    } else {
        PSG_setTone(0, NOTE_REST);
    }
    
    u16 bassNote = track->counter[musicStep];
    if (bassNote != NOTE_REST) {
        PSG_setTone(1, bassNote);
        PSG_setEnvelope(1, PSG_ENVELOPE_MAX);
    } else {
        PSG_setTone(1, NOTE_REST);
    }
    
    musicStep++;
    if (musicStep >= 16) musicStep = 0;
}
