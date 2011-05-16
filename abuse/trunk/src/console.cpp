/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "common.h"

#include "game.h"

#include "linked.h"
#include "console.h"

void console::put_string(char const *st)
{
  while (*st)
  {
    put_char(*st);
    st++;
  }
}

void console::redraw()
{
    if (!con_win)
        return;

    con_win->clear();
    char *s = screen;
    int xa = fnt->Size().x, ya = fnt->Size().y;
    for (int j = 0, dy = wy(); j < h; j++, dy += ya)
        for (int i = 0, dx = wx(); i < w; i++, s++, dx += xa)
            if (*s)
                fnt->PutChar(con_win->m_surf, vec2i(dx, dy), *s);
    fnt->PutChar(con_win->m_surf, vec2i(wx() + cx * xa, wy() + cy * ya), '_');
}

void console::show()
{
  if (!con_win)
  {
    con_win=wm->CreateWindow(vec2i(lastx, lasty),
                             vec2i(screen_w(), screen_h()), NULL, name);
    redraw();
    con_win->m_surf->SetClip(vec2i(con_win->x1(), con_win->y1()),
                             vec2i(con_win->x2() + 1, con_win->y2() + 1));
  }
}

void console::hide()
{
  if (con_win)
  {
    lastx=con_win->m_pos.x;
    lasty=con_win->m_pos.y;
    wm->close_window(con_win);
    con_win=NULL;
  }
}

console::~console()
{
  hide();
  free(screen);
  free(name);
}

console::console(JCFont *font, int width, int height, char const *Name)
{
  con_win=NULL;
  w=width;
  h=height;
  screen=(char *)malloc(w*h);
  memset(screen,' ',w*h);
  cx=cy=0;
  fnt=font;
  lastx=xres/2-screen_w()/2;
  lasty=yres/2-screen_h()/2;
  name = strdup(Name);
}


void console::draw_cursor()
{
    if (!con_win)
        return;

    fnt->PutChar(con_win->m_surf,
                 vec2i(cx, cy) * fnt->Size() + vec2i(wx(), wy()), '_');
}


void console::draw_char(int x, int y, char ch)
{
    if (!con_win)
        return;

    vec2i fs = fnt->Size();
    vec2i pos = vec2i(wx(), wy()) + vec2i(x, y) * fs;
    con_win->m_surf->Bar(pos, pos + fs - vec2i(1), wm->black());
    fnt->PutChar(con_win->m_surf, pos, ch);
}

void console::do_cr()
{
  if (cx<w && cy<h)  draw_char(cx,cy,screen[cy*w+cx]);
  cx=0;
  cy++;
  if (cy>=h)
  {
    cy=h-1;
    if (con_win)
    {
      memmove(screen,screen+w,w*(h-1));
      memset(screen+w*(h-1),' ',w);
      redraw();
      wm->flush_screen();
    }
  } else draw_cursor();
}

void console::put_char(char ch)
{


  switch (ch)
  {
    case JK_BACKSPACE :
    {
      if (cx)
      {
    if (con_win)
      draw_char(cx,cy,screen[cy*w+cx]);
    cx--;
    if (con_win)
      draw_cursor();
      }
    } break;
    case '\n' :
    case JK_ENTER :
    {
      do_cr();
    } break;
    default :
    {
      screen[cy*w+cx]=ch;
      if (con_win)
        draw_char(cx,cy,ch);
      cx++;
      if (cx>=w) do_cr(); else
      if (con_win) draw_cursor();
    }
  }
}


void console::print_f( const char *format, ...)
{
  char st[300];
  va_list ap;
  va_start(ap, format);
  vsprintf(st,format,ap);
  va_end(ap);
  put_string(st);
}


shell_term::shell_term(JCFont *font, int width, int height, char const *Name) :
  console(font,width,height,Name)
{
  shcmd[0]=0;
  prompt();
}

void shell_term::prompt()
{
  put_string("(?=help)>");
}

void shell_term::execute(char const *st)
{
  put_string(st);
  put_string(" : unhandled\n");
}

int shell_term::handle_event(Event &ev)
{
  if (ev.window==con_win && con_win)
  {
    switch (ev.type)
    {
      case EV_KEY :
      {
    switch (ev.key)
    {
      case JK_BACKSPACE:
      {
        if (shcmd[0]!=0)
        {
          shcmd[strlen(shcmd)-1]=0;
          put_char(ev.key);
        }
      } break;
      case JK_ENTER :
      {
        put_char(ev.key);
        execute(shcmd);
        prompt();
        shcmd[0]=0;
      } break;
      default :
      {
        if (ev.key<256 && isprint(ev.key))
        {
          int x=strlen(shcmd);
          shcmd[x+1]=0;
          shcmd[x]=ev.key;
          put_char(ev.key);
        }
      } break;
    } break;
      }
    }
    return 1;
  }
  return 0;
}

