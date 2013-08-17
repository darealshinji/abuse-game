/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __TOOLS_HPP_
#define __TOOLS_HPP_

#include "jwindow.h"
#include "input.h"
#include "specs.h"
#include "scroller.h"
#include "visobj.h"

class tool_picker : public spicker
{
  Filter *map;
  visual_object **icons;
  int *ids;
  int total_icons;
  int iw,ih;
  Palette *old_pal;

  public :

  // you are expected keep image and id list in memory, tool_picker does not copy them
  tool_picker(int X, int Y, int ID,
          int show_h, visual_object **Icons, int *Ids, int total_ic,
          Palette *icon_palette, Palette *pal, ifield *Next);

  virtual void draw_item(AImage *screen, int x, int y, int num, int active);
  virtual int total() { return total_icons; }
  virtual int item_width() { return iw; }
  virtual int item_height() { return ih; }
  virtual void note_new_current(AImage *screen, InputManager *inm, int x)
  { wm->Push(Event(ids[x], NULL)); }

  void remap(Palette *pal, AImage *screen);
  ~tool_picker();
} ;


#endif







