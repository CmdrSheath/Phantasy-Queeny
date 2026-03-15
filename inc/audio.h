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

// Music track IDs - add new scenes here
typedef enum {
    MUSIC_TITLE,
    MUSIC_CUTSCENE,
    MUSIC_OVERWORLD,
    MUSIC_COMBAT,
    MUSIC_DUNGEON,
    MUSIC_VICTORY,
    MUSIC_COUNT
} MusicTrack;

// Function prototypes
void initAudio(void);
void playMusic(MusicTrack track);
void stopMusic(void);
void pauseMusic(void);
void resumeMusic(void);
void playSFX(SoundEffect sfx);
void updateAudio(void);

#endif
