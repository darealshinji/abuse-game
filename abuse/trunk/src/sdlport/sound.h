/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __SOUND_HPP_
#define __SOUND_HPP_

#ifdef USE_SDL_MIXER
#include <SDL/SDL_mixer.h>
#endif

/* options are passed via command line */

#define SFX_INITIALIZED    1
#define MUSIC_INITIALIZED  2

int sound_init(int argc, char **argv);
void sound_uninit();
void print_sound_options();                         // print the options avaible for sound

class sound_effect
{
  long size;
  void *data;
#ifdef USE_SDL_MIXER
  Mix_Chunk* chunk;
#endif
public :
  sound_effect(char *filename);
  void play(int volume=127, int pitch=128, int panpot=128);
  ~sound_effect();
} ;


class song
{
  char *Name;
  unsigned char *data;
  unsigned long song_id;
#ifdef USE_SDL_MIXER
  Mix_Music* music;
  SDL_RWops* rw;
#endif
public :
  char const *name() { return Name; }
  song(char const *filename);
  void play(unsigned char volume=127);
  void stop(long fadeout_time=0);                                        // time in ms
  int playing();
  void set_volume(int volume);
  ~song();
} ;
#endif











