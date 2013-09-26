/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __PCX_READ_HPP__
#define __PCX_READ_HPP__

#include "imlib/image.h"
#include "imlib/palette.h"

void write_PCX(AImage *im, Palette *pal, char const *filename);
AImage *read_PCX(char const *filename, Palette *&pal);

#endif
