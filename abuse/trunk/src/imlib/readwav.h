/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __READ_WAV_H__
#define __READ_WAV_H__

#include "specs.h"
uint8_t *read_wav(char const *filename, int &sample_rate, int &data_size);

#endif
