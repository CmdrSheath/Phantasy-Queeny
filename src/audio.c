//==============================================================================
// PHANTASY QUEENY - Audio System
// XGM Driver with FM Synth Chiptune Sounds
//==============================================================================

#include "audio.h"
#include "game.h"

// XGM music data (generated from your composition)
// This is the 4-track, 8-measure loop you specified

// FM Instrument Patches (Early 90s Phantasy Star style)
// Patch 0: Bass/Drum Kick (FM Channel 1)
// Patch 1: Square Lead Main (FM Channel 2) 
// Patch 2: Saw Lead Melody (FM Channel 3)
// Patch 3: Bass Counter (FM Channel 4)
// PSG Noise: Hi-hats and snare

// Your composition converted to XGM pattern format
// Track 1: Drums (Kick on beats 1,2,3,4 - whole notes)
// Track 2: Main (16 quarter notes: a a b c d e f g g f e d c b a a)
// Track 3: Melody (Same as main, different patch)
// Track 4: Counter (a a - - a a - - a a - - a a - -, lower pitch)

// XGM header structure
typedef struct {
    u8 id[4];           // "XGM "
    u8 version;         // 1
    u8 pal;             // 0=NTSC, 1=PAL
    u16 sampleRate;     // Usually 14000+ for PCM, 0 for FM only
    u32 musicDataSize;  // Size of music data
    u32 musicDataOffset;// Offset to music data
} XGMHeader;

// Simple FM note definition
typedef struct {
    u8 note;      // 0-127 (MIDI note numbers)
    u8 duration;  // In frames (60fps)
    u8 velocity;  // 0-127
} FMNote;

// Pattern data for one channel (16 steps = 1 measure at 4/4)
typedef struct {
    FMNote notes[16];  // 16 quarter notes per measure
    u8 patch;          // Which FM patch to use
} ChannelPattern;

// Complete pattern for all 4 tracks
typedef struct {
    ChannelPattern drums;      // Track 1
    ChannelPattern main;       // Track 2
    ChannelPattern melody;     // Track 3
    ChannelPattern counter;    // Track 4
} MusicPattern;

// Your specific composition data
// Key: A minor (A3 = MIDI 57, A4 = 69, etc.)
// Tempo: 120 BPM = 30 frames per quarter note

static const MusicPattern overworldPattern = {
    // Track 1: Drums (FM Channel 6 + PSG Noise)
    // Pattern: Kick on 1,2,3,4 (whole notes), Hi-hats on offbeats
    {
        {
            {57, 30, 100}, {0, 30, 0}, {0, 30, 0}, {0, 30, 0},  // Measure 1: Kick + 3 rests
            {57, 30, 100}, {0, 30, 0}, {0, 30, 0}, {0, 30, 0},  // (simplified for demo)
            {57, 30, 100}, {0, 30, 0}, {0, 30, 0}, {0, 30, 0},
            {57, 30, 100}, {0, 30, 0}, {0, 30, 0}, {0, 30, 0}
        },
        0  // Drum patch
    },
    
    // Track 2: Main (FM Channel 1) - Square Lead
    // Pattern: a a b c d e f g g f e d c b a a (16 quarter notes)
    {
        {
            {69, 30, 100}, {69, 30, 100}, {71, 30, 100}, {72, 30, 100},  // A A B C
            {74, 30, 100}, {76, 30, 100}, {77, 30, 100}, {79, 30, 100},  // D E F G
            {79, 30, 100}, {77, 30, 100}, {76, 30, 100}, {74, 30, 100},  // G F E D
            {72, 30, 100}, {71, 30, 100}, {69, 30, 100}, {69, 30, 100}   // C B A A
        },
        1  // Square lead patch
    },
    
    // Track 3: Melody (FM Channel 2) - Saw Lead  
    // Same notes as main, different timbre
    {
        {
            {69, 30, 100}, {69, 30, 100}, {71, 30, 100}, {72, 30, 100},
            {74, 30, 100}, {76, 30, 100}, {77, 30, 100}, {79, 30, 100},
            {79, 30, 100}, {77, 30, 100}, {76, 30, 100}, {74, 30, 100},
            {72, 30, 100}, {71, 30, 100}, {69, 30, 100}, {69, 30, 100}
        },
        2  // Saw lead patch
    },
    
    // Track 4: Counter Melody (FM Channel 3) - Bass
    // Pattern: a a - - a a - - a a - - a a - - (lower pitch)
    // A3 = MIDI 57 (one octave down)
    {
        {
            {57, 30, 100}, {57, 30, 100}, {0, 30, 0}, {0, 30, 0},    // A A - -
            {57, 30, 100}, {57, 30, 100}, {0, 30, 0}, {0, 30, 0},    // A A - -
            {57, 30, 100}, {57, 30, 100}, {0, 30, 0}, {0, 30, 0},    // A A - -
            {57, 30, 100}, {57, 30, 100}, {0, 30, 0}, {0, 30, 0}     // A A - -
        },
        3  // Bass patch
    }
};

// FM Patch definitions (YM2612 register settings)
// Early 90s chiptune style

static const u8 fmPatchSquare[8] = {
    // Algorithm 4 (simple carrier/modulator)
    // Feedback 0
    // TL: 0, 32 (carrier quieter)
    // AR: 31, 31 (fast attack)
    // DR: 0, 0 (no decay)
    // SR: 0, 0  
    // RR: 15, 15 (fast release)
    // SL: 0, 0
    // Multiple: 1, 1
    // DT: 0, 0
    0x04, 0x00,  // Algorithm, Feedback
    0x00, 0x20,  // TL operators 1, 2
    0x1F, 0x1F,  // AR
    0x00, 0x00   // DR
};

static const u8 fmPatchSaw[8] = {
    // Algorithm 5 (more complex)
    // Slightly brighter than square
    0x05, 0x01,
    0x00, 0x18,
    0x1F, 0x1F,
    0x05, 0x05   // Slight decay for saw character
};

static const u8 fmPatchBass[8] = {
    // Algorithm 2 (bass punch)
    0x02, 0x02,
    0x00, 0x28,
    0x1F, 0x18,
    0x08, 0x08
};

static const u8 fmPatchDrum[8] = {
    // Algorithm 7 (noise-like)
    0x07, 0x00,
    0x00, 0x10,
    0x1F, 0x1F,
    0x0A, 0x0A
};

// Sequencer state
static u8 currentStep = 0;
static u8 currentMeasure = 0;
static u8 tempoCounter = 0;
static bool musicPlaying = false;
static MusicPattern currentPattern;
static u8 activeTracks = 0x0F; // All 4 tracks active

// Sound effect priority system
// code no longer used
// static u8 sfxPriority[SFX_COUNT] = { 1, 1, 1, 2, 3, 3, 3, 3, 4  // Higher = more important };
////

static void loadFMPatch(u8 channel, const u8* patch) {
    // Set YM2612 registers for FM channel
    // This is simplified - full implementation would set all 22 registers per channel
    
    u8 baseReg = channel * 0x10;  // Channel register base
    
    // Set algorithm and feedback
    YM2612_writeReg(0, 0xB0 + channel, patch[0]);
    YM2612_writeReg(0, 0xB4 + channel, patch[1]);
    
    // Set operator TL (Total Level)
    YM2612_writeReg(0, 0x40 + baseReg, patch[2]);
    YM2612_writeReg(0, 0x44 + baseReg, patch[3]);
    
    // Set attack rate
    YM2612_writeReg(0, 0x50 + baseReg, patch[4]);
    YM2612_writeReg(0, 0x54 + baseReg, patch[5]);
    
    // Set decay rate
    YM2612_writeReg(0, 0x60 + baseReg, patch[6]);
    YM2612_writeReg(0, 0x64 + baseReg, patch[7]);
}

static void playFMNote(u8 channel, u8 note, u8 velocity) {
    if (note == 0) {
        // Note off
        YM2612_writeReg(0, 0x28, 0x00 + channel);  // Key off
        return;
    }
    
    // Convert MIDI note to F-number and block
    // Formula: F-number = 144 * (2^20) * (2^((note-69)/12)) / (YM2612 clock / 144)
    // Simplified lookup for common octaves
    
    u8 octave = (note / 12) - 1;
    u8 semitone = note % 12;
    
    // F-number table for A4=440Hz
    static const u16 fNumbers[12] = {
        617, 653, 692, 733, 776, 822,  // C, C#, D, D#, E, F
        871, 923, 978, 1036, 1097, 1163 // F#, G, G#, A, A#, B
    };
    
    u16 fNum = fNumbers[semitone];
    
    // Set frequency
    u8 reg = channel * 4;  // Channel register offset
    YM2612_writeReg(0, 0xA4 + reg, ((octave & 0x07) << 3) | ((fNum >> 8) & 0x07));
    YM2612_writeReg(0, 0xA0 + reg, fNum & 0xFF);
    
    // Key on with velocity (using TL adjustment)
    YM2612_writeReg(0, 0x28, 0xF0 + channel);  // Key on all operators
}

static void updateSequencer(void) {
    if (!musicPlaying) return;
    
    tempoCounter++;
    if (tempoCounter < 30) return;  // 120 BPM, 60fps = 30 frames per quarter note
    tempoCounter = 0;
    
    // Play notes for current step on all active tracks
    if (activeTracks & 0x01) {
        FMNote* note = &currentPattern.drums.notes[currentStep];
        playFMNote(5, note->note, note->velocity);  // Channel 6 for drums
    }
    
    if (activeTracks & 0x02) {
        FMNote* note = &currentPattern.main.notes[currentStep];
        playFMNote(0, note->note, note->velocity);  // Channel 1 for main
    }
    
    if (activeTracks & 0x04) {
        FMNote* note = &currentPattern.melody.notes[currentStep];
        playFMNote(1, note->note, note->velocity);  // Channel 2 for melody
    }
    
    if (activeTracks & 0x08) {
        FMNote* note = &currentPattern.counter.notes[currentStep];
        playFMNote(2, note->note, note->velocity);  // Channel 3 for counter
    }
    
    // Advance sequencer
    currentStep++;
    if (currentStep >= 16) {
        currentStep = 0;
        currentMeasure++;
        if (currentMeasure >= 8) {
            currentMeasure = 0;  // Loop 8 measures
        }
    }
}

void initAudio(void) {
    // Initialize YM2612 FM chip
    YM2612_reset();
    
    // Initialize PSG (SN76489)
    PSG_reset();
    
    // Load instrument patches
    loadFMPatch(0, fmPatchSquare);   // Main
    loadFMPatch(1, fmPatchSaw);      // Melody  
    loadFMPatch(2, fmPatchBass);     // Counter
    loadFMPatch(5, fmPatchDrum);     // Drums (channel 6)
    
    // Initialize sequencer
    currentPattern = overworldPattern;
    currentStep = 0;
    currentMeasure = 0;
    tempoCounter = 0;
    musicPlaying = false;
    
    // Set default PSG noise for hi-hats
    PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_TONE3);
}

void playMusic(MusicTrack track) {
    switch(track) {
        case MUSIC_OVERWORLD:
            currentPattern = overworldPattern;
            activeTracks = 0x0F;  // All tracks
            break;
            
        case MUSIC_TITLE:
            // Simpler version - just main and counter
            activeTracks = 0x0A;  // Tracks 2 and 4
            break;
            
        case MUSIC_COMBAT:
            // Faster tempo, more intense
            activeTracks = 0x0F;
            // Could load different pattern here
            break;
            
        case MUSIC_DUNGEON:
            // Ambient, no drums
            activeTracks = 0x0E;  // No drums
            break;
            
        default:
            break;
    }
    
    currentStep = 0;
    currentMeasure = 0;
    musicPlaying = true;
}

void stopMusic(void) {
    musicPlaying = false;
    
    // Key off all channels
    for(u8 i = 0; i < 6; i++) {
        YM2612_writeReg(0, 0x28, i);
    }
 
}

void pauseMusic(void) {
    musicPlaying = false;
}

void resumeMusic(void) {
    musicPlaying = true;
}

void playSFX(SoundEffect sfx) {
    switch(sfx) {
        case SFX_MENU_MOVE:
            // Short blip
            PSG_setTone(0, 800);
            PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
            break;
            
        case SFX_MENU_SELECT:
            // Higher confirmation tone
            PSG_setTone(0, 1200);
            PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
            break;
            
        case SFX_MENU_CANCEL:
            // Lower cancel tone
            PSG_setTone(0, 600);
            PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
            break;
            
        case SFX_PORTAL_HUM:
            // FM glissando
            playFMNote(4, 50, 80);  // Channel 5 for SFX
            break;
            
        case SFX_COMBAT_START:
            // Orchestral hit simulation
            for(u8 i = 0; i < 3; i++) {
                playFMNote(i, 60 + (i*12), 100);
            }
            break;
            
        case SFX_ATTACK:
            // Noise burst
            PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_TONE3);
            PSG_setEnvelope(3, PSG_ENVELOPE_MIN);
            break;
            
        case SFX_MAGIC:
            // Rising arpeggio
            playFMNote(4, 72, 90);
            break;
            
        case SFX_DAMAGE:
            // Dissonant crash
            playFMNote(4, 30, 127);
            break;
            
        case SFX_VICTORY:
            // Major chord
            playFMNote(0, 72, 100);  // C5
            playFMNote(1, 76, 100);  // E5
            playFMNote(2, 79, 100);  // G5
            break;
            
        default:
            break;
    }
}

void updateAudio(void) {
    updateSequencer();
    
   
}
