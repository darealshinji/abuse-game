/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __EVENT_HPP_
#define __EVENT_HPP_
#define EV_MOUSE_MOVE     1
#define EV_MOUSE_BUTTON   2
#define EV_KEY            4
#define EV_KEY_SPECIAL    8
#define EV_REDRAW        16
#define EV_SPURIOUS      32
#define EV_RESIZE        64
#define EV_KEYRELEASE   128
#define EV_CLOSE_WINDOW 256
#define EV_DRAG_WINDOW  512
#define EV_MESSAGE     1024

#define LEFT_BUTTON    1
#define RIGHT_BUTTON   2
#define MIDDLE_BUTTON  4
#include "keys.h"
#include "sprite.h"

class Jwindow;

class Event : public linked_node
{
public:
    Event()
    {
        type = EV_SPURIOUS;
    }

    Event(int id, char *data)
    {
        type = EV_MESSAGE;
        message.id = id;
        message.data = data;
    }

    int type;
    vec2i mouse_move;
    int mouse_button, key;

    struct { char alt, ctrl, shift; } key_special;
    struct { int x1, y1, x2, y2; void *start; } redraw;

    Jwindow *window;      // NULL is root
    vec2i window_position;
    struct { int id; char *data; } message;
};

class EventHandler
{
public:
    EventHandler(image *screen, palette *pal);
    ~EventHandler();

    void Push(Event *ev)
    {
        m_events.add_end(ev);
    }

    void SysInit();
    void SysWarpMouse(vec2i pos);

  int IsPending();
  void Get(Event &ev);
  void add_redraw(int X1, int Y1, int X2, int Y2, void *Start);
  void flush_screen();

  int has_mouse() { return 1; }
  void set_mouse_shape(image *im, int centerx, int centery)
  {
    m_sprite->change_visual(im);
    m_center = vec2i(-centerx, -centery);
  }
  void set_mouse_position(int mx, int my)
  {
    m_pos = vec2i(Min(mx, m_screen->Size().x - 1),
                  Min(my, m_screen->Size().y - 1));
    SysWarpMouse(m_pos);
  }

private:
    int get_key_flags();

    linked_list m_events;
    int m_pending, last_keystat, last_key;
    sprite_controller sc;

    image *m_screen;

protected:
    /* Mouse information */
    sprite *m_sprite;
    vec2i m_pos, m_lastpos, m_center;
    int m_button, m_lastbutton;
};

#endif

