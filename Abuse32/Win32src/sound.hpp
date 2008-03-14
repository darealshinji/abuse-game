#ifndef __SOUND_HPP_
#define __SOUND_HPP_

#include "dsound.h"
#include "windows.h"
/* options are passed via command line */

#define SFX_INITIALIZED    1
#define MUSIC_INITIALIZED  2

#define MAX_SIMULTANEOUS_SOUNDS 30
#define MAX_IDENTICAL_SIMULTANEOUS_SOUNDS 10

int sound_init(int argc, char **argv);
void sound_uninit();
void print_sound_options();              // print the options avaible for sound
void ClearSoundCache();

// ----------------------------------------------------------------------------
class sound_effect
{
    friend class CSoundServer;
    long size;
    void *data;
    LPDIRECTSOUNDBUFFER pDSBuffer;
    int PlayingCount;
    DWORD LastStarted;
//    int PlayingList[MAX_IDENTICAL_SIMULTANEOUS_SOUNDS];
public :
    sound_effect(char *filename);
    void play(int volume = 127, int pitch = 128, int panpot = 128);
    ~sound_effect();
};

typedef sound_effect *LPsound_effect;

// ----------------------------------------------------------------------------
class song
{
    char *Name;
    unsigned char *data;
    unsigned long song_id;
public :
    char *name() { return Name; }
    song(char *filename);
    void play(unsigned char volume = 127);
    void stop(long fadeout_time = 0);                             // time in ms
    int playing();
    void set_volume(int volume);
    ~song();
};

// ----------------------------------------------------------------------------
class CSoundServer {
    LPsound_effect *PlayingList;
    LPDIRECTSOUNDBUFFER *PlayingSounds;
    unsigned long List_Size, PlayingCount;
public:
    __inline CSoundServer() {
        PlayingSounds = NULL;
        PlayingList = NULL;
        PlayingCount = 0;
        List_Size = 0;
        SetListSize(20);
    }
    ~CSoundServer() {
        EmptySoundCache();
    }
    sound_effect *FindEffect(sound_effect *Sound);
    int AddSound(LPsound_effect Sound, int volume = 0, int pan = 0, int frequency = 11025);
    void DeleteSound(sound_effect *Sound);
    void DeleteSound(unsigned long index);
    void EmptySoundCache();
    void CleanUpSounds();
    void ReduceEffectCount(sound_effect *Sound);
    void SetListSize(unsigned int size);
    void ClearExcessSounds();
};

#endif











