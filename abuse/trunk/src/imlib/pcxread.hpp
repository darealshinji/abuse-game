/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __PCX_READ_HPP__
#define __PCX_READ_HPP__

#include "image.hpp"
#include "palette.hpp"

void write_PCX(image *im, palette *pal, char const *filename);

#endif
