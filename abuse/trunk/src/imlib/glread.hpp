/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __GLREAD_HPP_
#define __GLREAD_HPP_
#include "palette.hpp"
#include "image.hpp"
image *read_glfont(char *fn);
image *read_pic(char *fn, palette *&pal);
image *read_clp(char *fn);

#endif

