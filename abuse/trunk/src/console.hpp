/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __CONSOLE_HPP_
#define __CONSOLE_HPP_
#include "jwindow.hpp"

class console
{
  protected :
  int lastx,lasty,w,h,cx,cy;
  JCFont *fnt;
  char *screen;
  jwindow *con_win;
  char *name;
  public :

  console(JCFont *font, int width, int height, char const *Name);
  int showing() { return con_win!=NULL; }
  void show();
  void hide();
  void redraw();
  void put_char(char ch);
  void do_cr();
  int screen_w() { return w*fnt->width(); }
  int screen_h() { return h*fnt->height(); }
  int wx() { return con_win->x1(); }
  int wy() { return con_win->y1(); }
  void draw_cursor();
  void put_string(char const *st);
  void draw_char(int x, int y, char ch);
  void toggle() { if (con_win) hide(); else show(); }
  void print_f(char const *format, ...);
  ~console();
} ;

class shell_term : public console
{
  char shcmd[300];
  public :
  shell_term(JCFont *font, int width, int height, char const *Name);
  virtual ~shell_term() {};
  int handle_event(event &ev);
  virtual void prompt();
  virtual void execute(char const *st);
} ;

#endif
