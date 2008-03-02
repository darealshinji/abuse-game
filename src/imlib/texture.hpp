/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __TEXTURE_HPP_
#define __TEXTURE_HPP_
#include "image.hpp"

void texture_map(image *screen, image *tx, short *points);
void clear_texture_map(image *screen, image *tx, short *points);
#endif

