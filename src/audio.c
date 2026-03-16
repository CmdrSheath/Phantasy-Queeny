//==============================================================================
// PHANTASY QUEENY - Audio System
// Channels: 0=Music Melody, 1=Music Bass, 2=Music Drum, 3=SFX Only
//==============================================================================

#include "audio.h"
#include "game.h"

static u16 musicTimer = 0;
static u8 musicStep = 0;
static bool musicPlaying = FALSE;
static MusicTrack currentTrack = MUSIC_TITLE;
static u8 sfxTimer = 0;

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
#define NOTE_REST 0

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

// PSG register addresses
#define PSG_PORT 0xC00011

void initAudio(void) {
    // Reset PSG - silence all channels
    volatile u16* psg = (volatile u16*)PSG_PORT;
    *psg = 0x9F;  // Channel 0 silence
    *psg = 0xBF;  // Channel 1 silence  
    *psg = 0xDF;  // Channel 2 silence
    *psg = 0xFF;  // Channel 3 silence
    
    musicTimer = 0;
    musicStep = 0;
    musicPlaying = FALSE;
    currentTrack = MUSIC_TITLE;
    sfxTimer = 0;
}

void playMusic(MusicTrack track) {
    // Silence music channels only (0,1,2), keep SFX channel (3)
    volatile u16* psg = (volatile u16*)PSG_PORT;
    *psg = 0x9F;
    *psg = 0xBF;
    *psg = 0xDF;
    
    currentTrack = track;
    musicStep = 0;
    musicTimer = 0;
    musicPlaying = TRUE;
}

void stopMusic(void) {
    musicPlaying = FALSE;
    volatile u16* psg = (volatile u16*)PSG_PORT;
    *psg = 0x9F;
    *psg = 0xBF;
    *psg = 0xDF;
    *psg = 0xFF;
}

void pauseMusic(void) {
    musicPlaying = FALSE;
}

void resumeMusic(void) {
    musicPlaying = TRUE;
}

void playSFX(SoundEffect sfx) {
    volatile u16* psg = (volatile u16*)PSG_PORT;
    
    // Channel 3 only - never touched by music
    // Format: 1 cc 0 tttttttt = tone data, 1 cc 1 llll = volume
    
    sfxTimer = 20;  // SFX lasts 20 frames
    
    switch(sfx) {
        case SFX_MENU_MOVE:
            *psg = 0xE4;   // Channel 3 tone low (E4)
            *psg = 0xF5;   // Channel 3 volume 5
            break;
            
        case SFX_MENU_SELECT:
            *psg = 0xCB;   // Channel 3 tone low (A4)
            *psg = 0xF3;   // Channel 3 volume 3
            break;
            
        case SFX_MENU_CANCEL:
            *psg = 0xD5;   // Channel 3 tone low (C4)
            *psg = 0xF6;   // Channel 3 volume 6
            break;
            
        case SFX_PORTAL_HUM:
            *psg = 0xE5;   // Channel 3 tone low (G3)
            *psg = 0xF4;   // Channel 3 volume 4
            break;
            
        case SFX_COMBAT_START:
            *psg = 0xE4;   // Channel 3 tone
            *psg = 0xF2;   // Channel 3 volume 2
            break;
            
        case SFX_ATTACK:
            // Use noise for attack
            *psg = 0xF4;   // Channel 3 noise, volume 4
            break;
            
        case SFX_MAGIC:
            *psg = 0xAB;   // Channel 3 tone high (C5)
            *psg = 0xF3;   // Channel 3 volume 3
            break;
            
        case SFX_DAMAGE:
            *psg = 0xD4;   // Channel 3 tone (E3)
            *psg = 0xF7;   // Channel 3 volume 7 (loud)
            break;
            
        case SFX_VICTORY:
            *psg = 0xAB;   // Channel 3 tone (C5)
            *psg = 0xF2;   // Channel 3 volume 2
            break;
            
        default:
            *psg = 0xC0;   // Channel 3 tone (A4)
            *psg = 0xF4;   // Channel 3 volume 4
            break;
    }
}

void updateAudio(void) {
    volatile u16* psg = (volatile u16*)PSG_PORT;
    
    // Handle SFX timeout - cut off SFX after timer expires
    if (sfxTimer > 0) {
        sfxTimer--;
        if (sfxTimer == 0) {
            *psg = 0xFF;  // Silence channel 3
        }
    }
    
    // Music sequencer - channels 0, 1, 2 only
    if (!musicPlaying) return;
    if (currentTrack >= 3) return;
    
    const MusicTrackData* track = &musicTracks[currentTrack];
    
    musicTimer++;
    if (musicTimer < track->tempo) return;
    musicTimer = 0;
    
    // Channel 0 = Melody (0x80 = channel 0 tone, 0x90 = channel 0 volume)
    u16 note = track->melody[musicStep];
    if (note != NOTE_REST) {
        *psg = 0x80 | (note & 0x0F);  // Tone low 4 bits
        *psg = 0x90 | ((note >> 4) & 0x3F);  // Tone high 6 bits + volume 0
    } else {
        *psg = 0x9F;  // Silence channel 0
    }
    
    // Channel 1 = Bass
    u16 bassNote = track->counter[musicStep];
    if (bassNote != NOTE_REST) {
        *psg = 0xA0 | (bassNote & 0x0F);
        *psg = 0xB0 | ((bassNote >> 4) & 0x3F);
    } else {
        *psg = 0xBF;  // Silence channel 1
    }
    
    // Channel 2 = Simple drum beat on every 4th step
    if ((musicStep % 4) == 0) {
        *psg = 0xC0;  // Low tone
        *psg = 0xD8;  // Volume 8
    } else {
        *psg = 0xDF;  // Silence channel 2
    }
    
    musicStep++;
    if (musicStep >= 16) musicStep = 0;
}
