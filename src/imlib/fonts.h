/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __FONTS_HPP_
#define __FONTS_HPP_

#include "image.h"
#include "transimage.h"

class JCFont
{
  int tl,th;
  TransImage *let[256];
public:
  JCFont(image *letters);
  void put_char(image *screen,  int x, int y, char ch, int color=-1);
  void put_string(image *screen, int x, int y, char const *st, int color=-1);
  int height() { return th; }
  int length() { return tl; }
  int width() { return tl; }
  ~JCFont();
} ;

#endif


