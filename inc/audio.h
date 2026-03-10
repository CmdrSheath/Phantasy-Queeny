#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <genesis.h>

// Sound effect IDs
typedef enum {
    SFX_MENU_MOVE,
    SFX_MENU_SELECT,
    SFX_MENU_CANCEL,
    SFX_PORTAL_HUM,
    SFX_COMBAT_START,
    SFX_ATTACK,
    SFX_MAGIC,
    SFX_DAMAGE,
    SFX_VICTORY,
    SFX_COUNT
} SoundEffect;

// Music track IDs
typedef enum {
    MUSIC_TITLE,
    MUSIC_OVERWORLD,
    MUSIC_COMBAT,
    MUSIC_DUNGEON,
    MUSIC_COUNT
} MusicTrack;

// Initialize sound system
void initAudio(void);

// Music control
void playMusic(MusicTrack track);
void stopMusic(void);
void pauseMusic(void);
void resumeMusic(void);

// Sound effects
void playSFX(SoundEffect sfx);

// Update audio system (call in game loop)
void updateAudio(void);

#endif
