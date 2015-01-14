/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
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
/* #define EV_REDRAW        16 UNUSED */
#define EV_SPURIOUS      32
#define EV_RESIZE        64
#define EV_KEYRELEASE   128
#define EV_CLOSE_WINDOW 256
#define EV_DRAG_WINDOW  512
#define EV_MESSAGE     1024

#define LEFT_BUTTON    1
#define RIGHT_BUTTON   2
#define MIDDLE_BUTTON  4

#include "imlib/keys.h"

#include "sprite.h"

class AWindow;

class Event
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
    ivec2 mouse_move;
    int mouse_button, key;

    struct { char alt, ctrl, shift; } key_special;

    AWindow *window;      // NULL is root
    ivec2 window_position;
    struct { int id; char *data; } message;
};

class EventHandler
{
public:
    EventHandler(AImage *screen, Palette *pal);
    ~EventHandler();

    void Push(Event const &ev)
    {
        /* Don’t really care about losing events */
        m_events.try_push(ev);
    }

    void SysInit();
    void SysWarpMouse(ivec2 pos);
    void SysEvent(Event &ev);

    int IsPending();
    void Get(Event &ev);
  void flush_screen();

  int has_mouse() { return 1; }
    void SetMouseShape(AImage *im, ivec2 center)
    {
        m_sprite->SetVisual(im, 1);
        m_center = center;
    }
    void SetMousePos(ivec2 pos)
    {
        m_pos = ivec2(lol::min(lol::max(pos.x, 0), m_screen->Size().x - 1),
                      lol::min(lol::max(pos.y, 0), m_screen->Size().y - 1));
        SysWarpMouse(m_pos);
    }

private:
    queue<Event, 256> m_events;
    int m_pending, last_key;

    AImage *m_screen;

protected:
    /* Mouse information */
    ASprite *m_sprite;
    ivec2 m_pos, m_center;
    int m_button;
};

#endif

