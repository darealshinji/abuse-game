/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __SOUND_H__
#define __SOUND_H__

#if defined USE_SDL_MIXER
#   if defined HAVE_SDL_SDL_H
#      include <SDL/SDL.h>
#   else
#      include <SDL.h>
#   endif
#   if defined HAVE_SDL_SDL_MIXER_H
#      include <SDL/SDL_mixer.h>
#   else
#      include <SDL_mixer.h>
#   endif
#endif

/* options are passed via command line */

#define SFX_INITIALIZED    1
#define MUSIC_INITIALIZED  2

int sound_init(int argc, char **argv);
void sound_uninit();
void print_sound_options(); // print the options avaible for sound

class sound_effect
{
public:
    sound_effect(char const *filename);
    ~sound_effect();

    void play(int volume = 127, int pitch = 128, int panpot = 128);

private:
#if defined USE_SDL_MIXER
    Mix_Chunk* m_chunk;
#endif
};

class song
{
public:
    String const &name() { return m_name; }
    song(String const &filename);
    void play(unsigned char volume=127);
    void stop(long fadeout_time=0); // time in ms
    int playing();
    void set_volume(int volume);
    ~song();

private:
    String m_name;
#if defined USE_SDL_MIXER
    uint8_t *data;
    unsigned long song_id;
    Mix_Music* music;
    SDL_RWops* rw;
#endif
};

#endif

