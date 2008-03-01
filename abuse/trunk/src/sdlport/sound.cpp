/************************
 * sound.c
 * SDL port for Abuse
 * by Anthony Kruize
 ************************/

#include <SDL.h>

#include "sound.hpp"
#include "jmalloc.hpp"
#include "readwav.hpp"
#include "specs.hpp"
#include "setup.h"

class effect_handle
{
public:
	effect_handle	*prev;		// Handle to the previous effect
	effect_handle	*next;		// Handle to the next effect
	Uint8			*data;		// Audio data
	Uint8			*pos;		// current playing position in the data
	Uint32			length;		// Length of the data
	Uint32			volume;		// Volume for this effect.

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
	sfxdir = (char *)jmalloc( strlen( datadir ) + 5 + 1, "sfxdir" );
	sprintf( sfxdir, "%s/sfx/", datadir );
	if( (fd = fopen( sfxdir,"r" )) == NULL )
	{
		// Didn't find the directory, so disable sound.
		printf( "Sound : Disabled (couldn't find the sfx directory)\n" );
		return 0;
	}
	jfree( sfxdir );

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

	sound_enabled = 1;
	printf( "Sound : Enabled\n" );

	SDL_PauseAudio( 0 );

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
		SDL_PauseAudio( 1 );
		while( fx_list )
		{
			effect_handle *last = fx_list;
			fx_list = fx_list->next;
			jfree( last );
		}
		SDL_CloseAudio();
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
		data = (void *)read_wav( filename, sample_speed, size );
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
		if( data )
		{
			jfree( data );
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
	}
}


//
// We don't handle songs.  These are just stubs to basically do nothing.
// I tried using SDL_mixer to do this, but with no success.
//

song::song( char const * filename )
{
	data = NULL;
	Name = strcpy((char *)jmalloc( strlen( filename ) + 1, "song name" ), filename );
	song_id = 0;
}

song::~song()
{
	if( playing() )
	{
		stop();
	}
	if( data )
	{
		jfree( data );
	}
	jfree( Name );
}

void song::play( unsigned char volume )
{
	song_id = 1;
}

void song::stop( long fadeout_time )
{
	song_id = 0;
}

int song::playing()
{
	return song_id;
}

void song::set_volume( int volume )
{
//	do nothing...
}
