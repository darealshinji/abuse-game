/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef _VIDEO_HPP_
#define _VIDEO_HPP_

#define TRI_1024x768x256 0x62
#define TRI_800x600x256  0x5e
#define TRI_640x480x256  0x5c
#define VGA_320x200x256  0x13
#define CGA_640x200x2    6
#define XWINDOWS_256     256
#define XWINDOWS_2       2

#include "image.h"

class AbuseVideo
{

};

extern int xres,yres;
extern int xoff,yoff;
extern AImage *main_screen;

void CreateScreen(int argc, char **argv);
void DestroyScreen();
void UpdateScreen();

void update_dirty(AImage *im, int xoff=0, int yoff=0);
void put_part_image(AImage *im, int x, int y, int x1, int y1, int x2, int y2);
void put_image(AImage * im, int x, int y);

void clear_put_image(AImage *im, int x, int y);
int get_vmode();

#endif
