/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __LMENU_HPP_
#define __LMENU_HPP_

class lmenu : public ifield
{
public :
  int id;
  ifield *next;
  virtual void area(int &x1, int &y1, int &x2, int &y2, window_manager *wm);
  virtual void draw_first(image *screen, window_manager *wm);
  virtual void draw(int active, image *screen, window_manager *wm)        = 0;
  virtual void handle_event(event &ev, image *screen, window_manager *wm) = 0;
  virtual char *read() = 0;
} ;
} ;

#endif
