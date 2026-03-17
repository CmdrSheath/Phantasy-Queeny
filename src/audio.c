//==============================================================================
// PHANTASY QUEENY - Audio System
// Channels: 0=Melody, 1=Bass, 2=Drum, 3=SFX Only
//==============================================================================

#include "audio.h"
#include "game.h"

static u16 musicTimer = 0;
static u8 musicStep = 0;
static bool musicPlaying = FALSE;
static MusicTrack currentTrack = MUSIC_TITLE;
static u8 sfxTimer = 0;

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

// Music data
static const u16 overworldMelody[] = {
    NOTE_A4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4,
    NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_B3, NOTE_A3, NOTE_A3
};
static const u16 overworldCounter[] = {
    NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST, NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST,
    NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST, NOTE_A3, NOTE_A3, NOTE_REST, NOTE_REST
};

static const u16 titleMelody[] = {
    NOTE_D4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
    NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_D4, NOTE_REST, NOTE_REST
};
static const u16 titleCounter[] = {
    NOTE_D3, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_D3, NOTE_REST, NOTE_REST, NOTE_REST,
    NOTE_D3, NOTE_REST, NOTE_REST, NOTE_REST, NOTE_D3, NOTE_REST, NOTE_REST, NOTE_REST
};

typedef struct {
    const u16* melody;
    const u16* counter;
    u8 tempo;
} MusicTrackData;

static const MusicTrackData musicTracks[] = {
    [MUSIC_TITLE]     = {titleMelody,     titleCounter,     40},
    [MUSIC_OVERWORLD]= {overworldMelody, overworldCounter, 30},
    [MUSIC_COMBAT]   = {overworldMelody, overworldCounter, 20}
};

// PSG write helper - writes to 0xC00011
static void psgWrite(u16 data) {
    volatile u8* psg = (volatile u8*)0xC00011;
    *psg = (u8)data;
}

void initAudio(void) {
    // Silence all PSG channels properly
    // Format: 1 cc 1 1111 = silence (volume 15 = off)
    psgWrite(0x9F);  // Channel 0 off
    psgWrite(0xBF);  // Channel 1 off  
    psgWrite(0xDF);  // Channel 2 off
    psgWrite(0xFF);  // Channel 3 off
    
    musicTimer = 0;
    musicStep = 0;
    musicPlaying = FALSE;
    currentTrack = MUSIC_TITLE;
    sfxTimer = 0;
}

void playMusic(MusicTrack track) {
    // Silence music channels only
    psgWrite(0x9F);
    psgWrite(0xBF);
    psgWrite(0xDF);
    // Leave channel 3 alone (SFX)
    
    currentTrack = track;
    musicStep = 0;
    musicTimer = 0;
    musicPlaying = TRUE;
}

void stopMusic(void) {
    musicPlaying = FALSE;
    psgWrite(0x9F);
    psgWrite(0xBF);
    psgWrite(0xDF);
    psgWrite(0xFF);
}

void pauseMusic(void) {
    musicPlaying = FALSE;
}

void resumeMusic(void) {
    musicPlaying = TRUE;
}

void playSFX(SoundEffect sfx) {
    // Use SGDK functions instead of raw registers
    // Channel 3 = index 3 in SGDK
    
    sfxTimer = 15;  // SFX duration
    
    switch(sfx) {
        case SFX_MENU_MOVE:
            PSG_setTone(3, NOTE_E4);
            PSG_setEnvelope(3, 8);
            break;
        case SFX_MENU_SELECT:
            PSG_setTone(3, NOTE_A4);
            PSG_setEnvelope(3, 6);
            break;
        case SFX_MENU_CANCEL:
            PSG_setTone(3, NOTE_C4);
            PSG_setEnvelope(3, 8);
            break;
        case SFX_PORTAL_HUM:
            PSG_setTone(3, NOTE_G4);
            PSG_setEnvelope(3, 10);
            break;
        case SFX_COMBAT_START:
            PSG_setTone(3, NOTE_E4);
            PSG_setEnvelope(3, 6);
            break;
        case SFX_ATTACK:
            PSG_setTone(3, NOTE_A3);
            PSG_setEnvelope(3, 8);
            break;
        case SFX_MAGIC:
            PSG_setTone(3, NOTE_C5);
            PSG_setEnvelope(3, 6);
            break;
        case SFX_DAMAGE:
            PSG_setTone(3, NOTE_E3);
            PSG_setEnvelope(3, 10);
            break;
        case SFX_VICTORY:
            PSG_setTone(3, NOTE_C5);
            PSG_setEnvelope(3, 6);
            break;
        default:
            PSG_setTone(3, NOTE_A4);
            PSG_setEnvelope(3, 8);
            break;
    }
}

void updateAudio(void) {
    // SFX timeout - manually silence channel 3
    if (sfxTimer > 0) {
        sfxTimer--;
        if (sfxTimer == 0) {
            // Silence channel 3 using SGDK
            PSG_setTone(3, NOTE_REST);
        }
    }
    
    // Music on channels 0 and 1
    if (!musicPlaying) return;
    if (currentTrack > MUSIC_COMBAT) return;
    
    const MusicTrackData* track = &musicTracks[currentTrack];
    
    musicTimer++;
    if (musicTimer < track->tempo) return;
    musicTimer = 0;
    
    // Channel 0: Melody
    u16 note = track->melody[musicStep];
    PSG_setTone(0, note);
    if (note != NOTE_REST) {
        PSG_setEnvelope(0, 0);  // Full volume
    } else {
        PSG_setEnvelope(0, 15); // Silence
    }
    
    // Channel 1: Bass
    u16 bass = track->counter[musicStep];
    PSG_setTone(1, bass);
    if (bass != NOTE_REST) {
        PSG_setEnvelope(1, 2);  // Slightly quieter
    } else {
        PSG_setEnvelope(1, 15); // Silence
    }
    
    musicStep++;
    if (musicStep >= 16) musicStep = 0;
}
