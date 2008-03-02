/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __READXWD_HPP
#define __READXWD_HPP
#include "image.hpp"
#include "palette.hpp"
#include "image24.hpp"

image *read_bmp(palette *&pal, char *filename);
image24 *read_bmp24(char *filename);
int bmp_bits(char *filname);
image *readxwd(char *input_file,palette *&pal);
void write_bmp(image *im, palette *pal, char *filename);
void deskjet_print(int argc, char **argv);


#endif

