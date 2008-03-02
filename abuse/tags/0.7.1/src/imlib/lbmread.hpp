/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef _LBMREAD_HPP_
#define _LBMREAD_HPP_

#include "image.hpp"
#include "palette.hpp"

image *read_lbm(char *filename, palette *&pal);

#endif
