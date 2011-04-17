/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "chat.h"
#include "dev.h"

chat_console *chat = NULL;

chat_console::chat_console(JCFont *font, int width, int height) :
  console(font,width,height<4 ? 4 : height,symbol_str("CHAT"))
{
    hide();
    clear();
    cx = 0;
    cy = h - 1;
    lastx = xres / 2 - screen_w() / 2;
    lasty = yres - h;
}

void chat_console::clear()
{
  memset(screen,' ',w*h);
  memset(screen+w*(h-2),'-',w);
  redraw();
}

void chat_console::put_all(char *st)
{
  memmove(screen,screen+w,w*(h-3));
  memset(screen+w*(h-3),' ',w);
  memcpy(screen+w*(h-3),st,strlen(st));
  redraw();
}


void chat_console::draw_user(char *st)
{
  memset(screen+w*(h-1),' ',w);
  memcpy(screen+w*(h-1),st,strlen(st));
  cx=strlen(st);
  cy=h-1;
  redraw();
}

