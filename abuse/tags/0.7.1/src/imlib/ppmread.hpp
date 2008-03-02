/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __PPMREAD__
#define __PPMREAD__
#include "image.hpp"
#include "palette.hpp"
#define PPM_R3G3B2 1
#define PPM_BW 2
#define PPM_REG 3
extern image *read_ppm(char *fn,palette *&pal, int pal_type=0);
extern void write_ppm(image *im,palette *pal,char *fn);
#endif

