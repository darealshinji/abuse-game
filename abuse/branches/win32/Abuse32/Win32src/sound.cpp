#include "readwav.hpp"
#include "jmalloc.hpp"
#include <stdlib.h>
#include "sound.hpp"
#include <stdio.h>
#include "dprint.hpp"
#include "specs.hpp"
#include "macs.hpp"
#include "timing.hpp"
#include "dsound.h"
#include "windows.h"
#include "..\abuse32.h"

#include "crtdbg.h"

LPDIRECTSOUND       g_pDS            = NULL;
LPDIRECTSOUNDNOTIFY g_pDSNotify      = NULL;

CSoundServer *SoundServer = NULL;

int SOUND_AVAIL = 0;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void print_sound_options() { ; }    // print the options avaible for sound

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void song::set_volume(int volume)  // 0..127
{

}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void sound_uninit()
{
    SOUND_AVAIL = 0;
    if (!g_pDS)
        return;
    dprintf("Releasing DirectSound interface\n");
    if (SoundServer) {
        SoundServer->EmptySoundCache();
        delete SoundServer;
        SoundServer = NULL;
    }
    g_pDS->Release();
    g_pDS = NULL;
}

extern int ComInitialized;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int sound_init(int argc, char **argv)
{
    if (g_pDS)
        sound_uninit();

    atexit(sound_uninit);

    HRESULT             hRet;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

/*    if (hRet = CoInitialize(NULL))
        return 0; */
    
    if (!ComInitialized) {
        hRet = CoInitialize(NULL);
        if (hRet == S_OK)
            ComInitialized = 1;
        else
            WriteErr(0, "DirectSound: Couldn't initialize COM library.");
    }
    
    if (hRet = DirectSoundCreate(NULL, &g_pDS, NULL)) {
        dprintf("Unable to create DirectSound interface\n");
        return 0;
    }
    if (hRet = g_pDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)) {
        dprintf("Unable to set DirectSound cooperative level\n");
        sound_uninit();
        return 0;
    }

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    hRet = g_pDS->CreateSoundBuffer(&dsbd, &pDSBPrimary, NULL);
    if (hRet != DS_OK) {
        dprintf("Unable to create primary DirectSound buffer\n");
        sound_uninit();
        return 0;
    }

    // Set primary buffer format to 44.1kHz and 16-bit output.
    WAVEFORMATEX wfx;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = 2;
    wfx.nSamplesPerSec  = 44100;
    wfx.wBitsPerSample  = 16;
    wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    hRet = pDSBPrimary->SetFormat(&wfx);
    if (hRet != DS_OK) {
        dprintf("Unable to set DirectSound primary buffer format\n");
        sound_uninit();
        return 0;
    }
    if (pDSBPrimary)
        pDSBPrimary->Release();
    
    SOUND_AVAIL = 1;
    SoundServer = new CSoundServer;
    dprintf("DirectSound interface initialized to 44.1kHz, 16-bit stereo\n\n");
    return SOUND_AVAIL;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
sound_effect::sound_effect(char *filename)
{  
    data = NULL;
    PlayingCount = 0;
    pDSBuffer = NULL;
    if (!SOUND_AVAIL)
        return;
    long sample_speed;
    
    data = (void*) read_wav(filename, sample_speed, size);  
    DSBUFFERDESC dsbd;
    VOID*   pbData  = NULL;
    VOID*   pbData2 = NULL;
    DWORD   dwLength;
    DWORD   dwLength2;
    HRESULT hr;

    WAVEFORMATEX wformat;
    ZeroMemory(&wformat, sizeof(WAVEFORMATEX));
    wformat.nChannels = 1;
    wformat.nAvgBytesPerSec = 11025;
    wformat.wFormatTag = WAVE_FORMAT_PCM;
    wformat.nSamplesPerSec = 11025;
    wformat.wBitsPerSample = 8;
    wformat.cbSize = 0;
    wformat.nBlockAlign = 1;

    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_CTRLPAN       | DSBCAPS_CTRLVOLUME |
                         DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC | DSBCAPS_GETCURRENTPOSITION2;
    dsbd.dwBufferBytes = size;
    dsbd.lpwfxFormat   = &wformat;

    // Create and lock the static DirectSound buffer 
    hr = g_pDS->CreateSoundBuffer(&dsbd, &pDSBuffer, NULL);
    if (hr == DS_OK) {
        hr = pDSBuffer->Lock(0, size, &pbData, &dwLength, NULL, &dwLength2, DSBLOCK_ENTIREBUFFER);

        // Copy the memory to it.
        memcpy(pbData, data, size);

        // Done copying sound data, unlock the buffer
        pDSBuffer->Unlock(pbData, size, NULL, 0);
    }
};

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void sound_effect::play(int volume, int pitch, int panpot)
{
    if (!SOUND_AVAIL)
        return;
    if (PlayingCount >= MAX_IDENTICAL_SIMULTANEOUS_SOUNDS) {
        DWORD Now = GetTickCount();
        if (Now <= LastStarted)
            return;
        SoundServer->ReduceEffectCount(this);
    }
    SoundServer->AddSound(this, ((volume - 127) * 2000) / 127, panpot, pitch);
    LastStarted = GetTickCount() + 25;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
sound_effect::~sound_effect() 
{ 
    SoundServer->DeleteSound(this);
    if (data)
        jfree(data);
    if (pDSBuffer)
        pDSBuffer->Release();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
song::song(char *filename) 
{
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void song::play(unsigned char volume) 
{
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void song::stop(long fadeout_time) 
{ 
}                                        // time in ms

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int song::playing() 
{
    return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
song::~song() 
{
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CSoundServer::DeleteSound(sound_effect *Sound)
{
    unsigned int i;
    for (i = 0; i < List_Size; i++)
        if (PlayingList[i] == Sound)
            DeleteSound(i);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
sound_effect *CSoundServer::FindEffect(sound_effect *Sound)
{
    unsigned int i;
    for (i = 0; i < List_Size; i++)
        if (PlayingList[i] == Sound)
            return PlayingList[i];
    return NULL;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CSoundServer::SetListSize(unsigned int size)
{
    _RPT2(_CRT_WARN, "CSoundServer::SetListSize -- changing list size from %d to %d\n", List_Size, size);
    if (size == List_Size)
        return;
    int i = List_Size;
    int NewElements = size - List_Size;
    List_Size = size;
    PlayingList = (LPsound_effect*) jrealloc(PlayingList, sizeof(*PlayingList) * List_Size, "CSoundServer::AddSound-PlayingList");
    PlayingSounds = (LPDIRECTSOUNDBUFFER*) jrealloc(PlayingSounds, sizeof(*PlayingSounds) * List_Size, "CSoundServer::AddSound-PlayingSounds");
    if (NewElements > 0) {
        ZeroMemory(&PlayingList[i], sizeof(PlayingList[i]) * NewElements);
        ZeroMemory(&PlayingSounds[i], sizeof(PlayingSounds[i]) * NewElements);
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int CSoundServer::AddSound(LPsound_effect Sound, int volume, int pan, int frequency)
{
    HRESULT hr;
    unsigned int i = 0;
    CleanUpSounds();
    while (i < List_Size && PlayingList[i])
        i++;
    if (i >= List_Size) {
        SetListSize(List_Size + 8);
    }
    Sound->PlayingCount++;
    PlayingList[i] = Sound;
    g_pDS->DuplicateSoundBuffer(Sound->pDSBuffer, &PlayingSounds[i]);
    hr = PlayingSounds[i]->SetVolume(volume);
    hr = PlayingSounds[i]->SetPan(pan);
//    hr = PlayingSounds[i]->SetFrequency(frequency);
    hr = PlayingSounds[i]->Play(0, 0, 0);
    return i;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CSoundServer::DeleteSound(unsigned long index)
{
    sound_effect *sound = PlayingList[index];
    if (PlayingSounds[index]) {
        PlayingSounds[index]->Release();
        PlayingSounds[index] = NULL;
        
//        memmove(&sound->PlayingList[1], sound->PlayingList, sizeof(sound->PlayingList[0]) * MAX_IDENTICAL_SIMULTANEOUS_SOUNDS);
//        sound->PlayingList[MAX_IDENTICAL_SIMULTANEOUS_SOUNDS - 1] = -1;
        PlayingList[index]->PlayingCount--;
        PlayingList[index] = NULL;
    }
}
 
/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CSoundServer::ReduceEffectCount(sound_effect *Sound)
{
    unsigned int i = 0;
    unsigned int leastindex = 0xFFFFFFFF, least = 0;
    for (i = 0; i < List_Size; i++)
        if (PlayingSounds[i])
            if (PlayingList[i] == Sound) {
                DWORD curr;
                PlayingSounds[i]->GetCurrentPosition(&curr, NULL);
                if (curr >= least) {
                    leastindex = i;
                    least = curr;
                }
            }
//    _RPT2(_CRT_WARN, "CSoundServer::ReduceEffectCount -- Too many 0x%x's playing (%d), deleting one\n", PlayingList[Num], PlayingList[Num]->PlayingCount);
    if (leastindex != 0xFFFFFFFF)
        DeleteSound(leastindex);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CSoundServer::EmptySoundCache()
{
    _RPT0(_CRT_WARN, "CSoundServer::EmptySoundCache -- Releasing sound effects\n");
    unsigned int i;
    for (i = 0; i < List_Size; i++) {
        if (PlayingSounds[i]) {
            PlayingSounds[i]->Release();
            PlayingSounds[i] = NULL;
            PlayingList[i]->PlayingCount = 0;
            PlayingList[i] = NULL;
        }
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CSoundServer::CleanUpSounds()
{
    unsigned int i;
    DWORD SoundStatus = 0;
    for (i = 0; i < List_Size; i++) {
        if (PlayingSounds[i]) {
            PlayingSounds[i]->GetStatus(&SoundStatus);
            if (SoundStatus & DSBSTATUS_BUFFERLOST || !(SoundStatus & DSBSTATUS_PLAYING) || SoundStatus & DSBSTATUS_TERMINATED) {
                DeleteSound(i);
            }
        }
    }
}
