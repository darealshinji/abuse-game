/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 2001 Anthony Kruize <trandor@labyrinth.net.au>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 */

#include "config.h"

#include <string.h>

#include <SDL.h>
#ifdef USE_SDL_MIXER
#include <SDL/SDL_mixer.h>
#include "hmi.h"
#endif

#include "sound.h"
#include "readwav.h"
#include "specs.h"
#include "setup.h"

class effect_handle
{
public:
    effect_handle    *prev;        // Handle to the previous effect
    effect_handle    *next;        // Handle to the next effect
    Uint8            *data;        // Audio data
    Uint8            *pos;        // current playing position in the data
    Uint32            length;        // Length of the data
    Uint32            volume;        // Volume for this effect.

    effect_handle( effect_handle *Next )
    {
        next = Next;
        if( next )
        {
            next->prev = this;
        }
        prev = NULL;
        data = NULL;
        pos = NULL;
    }

    ~effect_handle()
    {
        if( next )
        {
            next->prev = prev;
        }
        if( prev )
        {
            prev->next = next;
        }
    }
};

static effect_handle * fx_list = NULL;
extern flags_struct flags;
static int sound_enabled = 0;
static SDL_AudioSpec audioObtained;

//
// mix_audio
//
// Do the actual playing of the audio by looping through our effects list
// and mixing each sample.
//
void mix_audio(void *udata, Uint8 *stream, int len)
{
#ifndef USE_SDL_MIXER
    effect_handle *handle = fx_list;

    while( handle )
    {
        if( handle->length > 0 && handle->pos )
        {
            len = ( len > (int)handle->length ? handle->length : len );
            SDL_MixAudio( stream, handle->pos, len, handle->volume );
            handle->pos += len;
            handle->length -= len;
            handle = handle->next;
        }
        else
        {
            // update the list pointer if we're deleting the first node
            if( fx_list == handle )
            {
                fx_list = handle->next;
            }
            effect_handle *tmp = handle->next;
            if( !flags.mono )
            {
                // delete the audio buffer
                free( handle->data );
            }
            delete handle;
            handle = tmp;
        }
    }
#endif
}

//
// sound_init()
// Initialise audio
//
int sound_init( int argc, char **argv )
{
    SDL_AudioSpec audioWanted;
    char *sfxdir, *datadir;
    FILE *fd = NULL;

    // Disable sound if requested.
    if( flags.nosound )
    {
        // User requested that sound be disabled
        printf( "Sound : Disabled (-nosound)\n" );
        return 0;
    }

    // Check for the sfx directory, disable sound if we can't find it.
    datadir = get_filename_prefix();
    sfxdir = (char *)malloc( strlen( datadir ) + 5 + 1 );
    sprintf( sfxdir, "%s/sfx/", datadir );
    if( (fd = fopen( sfxdir,"r" )) == NULL )
    {
        // Didn't find the directory, so disable sound.
        printf( "Sound : Disabled (couldn't find the sfx directory)\n" );
        return 0;
    }
    free( sfxdir );

#ifdef USE_SDL_MIXER
    if (Mix_OpenAudio(11025, AUDIO_U8, 2, 128) < 0)
    {
        printf( "Sound : Unable to open audio - %s\nSound : Disabled (error)\n", SDL_GetError() );
        return 0;
    }

    Mix_AllocateChannels(50);

    int tempChannels = 0;
    Mix_QuerySpec(&audioObtained.freq, &audioObtained.format, &tempChannels);
    audioObtained.channels = tempChannels & 0xFF;

    sound_enabled = SFX_INITIALIZED | MUSIC_INITIALIZED;
#else
    audioWanted.freq = 11025;
    audioWanted.format = AUDIO_U8;
    audioWanted.channels = 2 - flags.mono;
    audioWanted.samples = 128;
    audioWanted.callback = mix_audio;
    audioWanted.userdata = NULL;

    // Now open the audio device
    if( SDL_OpenAudio( &audioWanted, &audioObtained ) < 0 )
    {
        printf( "Sound : Unable to open audio - %s\nSound : Disabled (error)\n", SDL_GetError() );
        return 0;
    }

    SDL_PauseAudio( 0 );

    sound_enabled = SFX_INITIALIZED;
#endif

    printf( "Sound : Enabled\n" );

    // It's all good
    return sound_enabled;
}

//
// sound_uninit
//
// Shutdown audio and release any memory left over.
//
void sound_uninit()
{
    if( sound_enabled )
    {
#ifdef USE_SDL_MIXER
        Mix_CloseAudio();
#else
        SDL_PauseAudio( 1 );
        while( fx_list )
        {
            effect_handle *last = fx_list;
            fx_list = fx_list->next;
            free( last );
        }
        SDL_CloseAudio();
#endif
    }
}

//
// sound_effect constructor
//
// Read in the requested .wav file.
//
sound_effect::sound_effect( char * filename )
{
    if( sound_enabled )
    {
        long sample_speed;

#ifdef USE_SDL_MIXER
        void* temp_data = (void *)read_wav( filename, sample_speed, size );

        SDL_AudioCVT audiocvt;
    
        SDL_BuildAudioCVT( &audiocvt, AUDIO_U8, 1, 11025, audioObtained.format, audioObtained.channels, audioObtained.freq );
        data = malloc( size * audiocvt.len_mult );

        memcpy( data, temp_data, size );
        audiocvt.buf = (Uint8*)data;
        audiocvt.len = size;
        SDL_ConvertAudio( &audiocvt );
        size = (Uint32)((double)size * audiocvt.len_ratio);

        free(temp_data);

        this->chunk = Mix_QuickLoad_RAW((Uint8*)data, size);
#else
        data = (void *)read_wav( filename, sample_speed, size );
#endif
    }
}

//
// sound_effect destructor
//
// Release the audio data.
//
sound_effect::~sound_effect()
{
    if( sound_enabled )
    {
#ifdef USE_SDL_MIXER
        // Sound effect deletion only happens on level load, so there
        // is no problem in stopping everything. But the original playing
        // code handles the sound effects and the "playlist" differently.
        // Therefore with SDL_mixer, a sound that has not finished playing
        // on a level load will cut off in the middle. This is most noticable
        // for the button sound of the load savegame dialog.
        Mix_FadeOutGroup(-1, 100);
        while (Mix_Playing(-1))
            SDL_Delay(10);
        Mix_FreeChunk(this->chunk);
#endif

        if( data )
        {
            free( data );
        }
    }
}

//
// sound_effect::play
//
// Insert a new effect_handle into the list and modify the audio
// if we're doing stereo.
// panpot defines the pan position for the sound effect.
//   0   - Completely to the right.
//   128 - Centered.
//   255 - Completely to the left.
//
void sound_effect::play( int volume, int pitch, int panpot )
{
    if( sound_enabled )
    {
#ifdef USE_SDL_MIXER
        int channel = Mix_PlayChannel(-1, this->chunk, 0);
        if (channel > -1)
        {
            Mix_Volume(channel, volume);
            Mix_SetPanning(channel, panpot, 255 - panpot);
        }
#else
        SDL_LockAudio();

        fx_list = new effect_handle( fx_list );
        if( fx_list == NULL )
        {
            printf( "Sound : ERROR - Failed to create new effect.\n" );
            SDL_UnlockAudio();
            return;
        }

        if( !flags.mono )
        {
            unsigned int i;
            Uint32 cvtBufferSize;
            SDL_AudioCVT audiocvt;

            // Do some audio conversion
            SDL_BuildAudioCVT( &audiocvt, AUDIO_U8, 1, 11025, audioObtained.format, audioObtained.channels, audioObtained.freq );
            audiocvt.buf = (Uint8 *)malloc( size * audiocvt.len_mult );
            audiocvt.len = size;
            memcpy( audiocvt.buf, data, size );
            SDL_ConvertAudio( &audiocvt );
            cvtBufferSize = (Uint32)((double)size * audiocvt.len_ratio);

            // Adjust for requested pan position
            if( panpot != 128 )
            {
                if( panpot > 128 )
                {
                    // Pan to the left
                    panpot = (panpot - 255) * -1;
                    for( i = 1 ; i <= cvtBufferSize; i += 2 )
                    {
                        audiocvt.buf[i] = (((audiocvt.buf[i] - 128) * panpot) / 128) + 128;
                    }
                }
                else
                {
                    // Pan to the right
                    for( i = 0 ; i < cvtBufferSize; i += 2 )
                    {
                        audiocvt.buf[i] = (((audiocvt.buf[i] - 128) * panpot) / 128) + 128;
                    }
                }
            }

            fx_list->data = audiocvt.buf;
            fx_list->pos = audiocvt.buf;
            fx_list->length = cvtBufferSize;
            fx_list->volume = volume;
        }
        else
        {
            // Only doing mono so don't mess with the audio data.
            fx_list->data = (Uint8 *)data;
            fx_list->pos = (Uint8 *)data;
            fx_list->length = size;
            fx_list->volume = volume;
        }
        SDL_UnlockAudio();
#endif
    }
}


// Play music using SDL_Mixer

song::song( char const * filename )
{
    data = NULL;
    Name = strdup(filename);
    song_id = 0;
    
#ifdef USE_SDL_MIXER
    rw = NULL;
    music = NULL;

    char realname[255];
    strcpy(realname, get_filename_prefix());
    strcat(realname, filename);

    unsigned int data_size;
    data = load_hmi(realname, data_size);

    if (data)
    {
        rw = SDL_RWFromMem(data, data_size);
        music = Mix_LoadMUS_RW(rw);
    }
#endif
}

song::~song()
{
    if( playing() )
    {
        stop();
    }
    if( data )
    {
        free( data );
    }
    free( Name );

#ifdef USE_SDL_MIXER
    Mix_FreeMusic(this->music);
    SDL_FreeRW(this->rw);
#endif
}

void song::play( unsigned char volume )
{
    song_id = 1;

#ifdef USE_SDL_MIXER
    Mix_PlayMusic(this->music, 0);
    Mix_VolumeMusic(volume);
#endif
}

void song::stop( long fadeout_time )
{
    song_id = 0;

#ifdef USE_SDL_MIXER
    Mix_FadeOutMusic(100);
#endif
}

int song::playing()
{
#ifdef USE_SDL_MIXER
    return Mix_PlayingMusic();
#else
    return song_id;
#endif
}

void song::set_volume( int volume )
{
#ifdef USE_SDL_MIXER
    Mix_VolumeMusic(volume);
#else
    // do nothing...
#endif
}
