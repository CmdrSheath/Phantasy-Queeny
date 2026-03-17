//==============================================================================
// PHANTASY QUEENY - Audio System
// 4 PSG channels: 0=Melody, 1=Bass, 2=Music Drum, 3=SFX Only
// All channels play simultaneously at different volumes
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
#define NOTE_D5  152
#define NOTE_E5  136
#define NOTE_F5  128
#define NOTE_G5  114
#define NOTE_REST 0

// Volume levels (0=full, 15=silence)
#define VOL_MUSIC_MELODY  4    // Quiet melody
#define VOL_MUSIC_BASS    6    // Quieter bass
#define VOL_MUSIC_DRUM    8    // Quiet drums
#define VOL_SFX           2    // Loud SFX (lower number = louder)

// Music data
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

void initAudio(void) {
    PSG_reset();
    musicTimer = 0;
    musicStep = 0;
    musicPlaying = FALSE;
    currentTrack = MUSIC_TITLE;
    sfxTimer = 0;
}

void playMusic(MusicTrack track) {
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
    sfxTimer = 20;  // SFX duration (longer for audibility)
    
    switch(sfx) {
        case SFX_MENU_MOVE:
            PSG_setTone(3, NOTE_E4);
            PSG_setEnvelope(3, VOL_SFX);
            break;
        case SFX_MENU_SELECT:
            PSG_setTone(3, NOTE_A4);
            PSG_setEnvelope(3, VOL_SFX);
            break;
        case SFX_MENU_CANCEL:
            PSG_setTone(3, NOTE_C4);
            PSG_setEnvelope(3, VOL_SFX);
            break;
        case SFX_PORTAL_HUM:
            PSG_setTone(3, NOTE_G3);
            PSG_setEnvelope(3, VOL_SFX);
            break;
        case SFX_COMBAT_START:
            PSG_setTone(3, NOTE_E4);
            PSG_setEnvelope(3, VOL_SFX);
            break;
        case SFX_ATTACK:
            PSG_setTone(3, NOTE_A3);
            PSG_setEnvelope(3, VOL_SFX);
            break;
        case SFX_MAGIC:
            PSG_setTone(3, NOTE_C5);
            PSG_setEnvelope(3, VOL_SFX);
            break;
        case SFX_DAMAGE:
            PSG_setTone(3, NOTE_E3);
            PSG_setEnvelope(3, VOL_SFX);
            break;
        case SFX_VICTORY:
            PSG_setTone(3, NOTE_C5);
            PSG_setEnvelope(3, VOL_SFX);
            break;
        default:
            PSG_setTone(3, NOTE_A4);
            PSG_setEnvelope(3, VOL_SFX);
            break;
    }
}

void updateAudio(void) {
    // Handle SFX timeout
    if (sfxTimer > 0) {
        sfxTimer--;
        if (sfxTimer == 0) {
            PSG_setTone(3, NOTE_REST);
        }
    }
    
    // Music sequencer - channels 0, 1, 2
    if (!musicPlaying) return;
    if (currentTrack > MUSIC_COMBAT) return;
    
    const MusicTrackData* track = &musicTracks[currentTrack];
    
    musicTimer++;
    if (musicTimer < track->tempo) return;
    musicTimer = 0;
    
    // Channel 0: Melody (quiet)
    u16 note = track->melody[musicStep];
    PSG_setTone(0, note);
    PSG_setEnvelope(0, (note == NOTE_REST) ? 15 : VOL_MUSIC_MELODY);
    
    // Channel 1: Bass (quieter)
    u16 bass = track->counter[musicStep];
    PSG_setTone(1, bass);
    PSG_setEnvelope(1, (bass == NOTE_REST) ? 15 : VOL_MUSIC_BASS);
    
    // Channel 2: Simple drum on beat
    if ((musicStep % 4) == 0) {
        PSG_setTone(2, NOTE_A2);
        PSG_setEnvelope(2, VOL_MUSIC_DRUM);
    } else {
        PSG_setEnvelope(2, 15);  // Silence
    }
    
    musicStep++;
    if (musicStep >= 16) musicStep = 0;
}
