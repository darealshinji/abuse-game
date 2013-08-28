/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __DEVSCROLL_HPP_
#define __DEVSCROLL_HPP_

#include "scroller.h"

class ATilePicker : public AScroller
{
public:
    ATilePicker(ivec2 pos, int id, int spec_type,
                int scale, int scroll_h, int Wid);

    virtual void handle_inside_event(Event &ev, AImage *screen, InputManager *inm);
    virtual void scroll_event(int newx, AImage *screen);
    virtual char const *read() { return (char const *)this; }
    int picw();
    int pich();
    int total();
    int get_current();
    void set_current(int x);
    void recenter(AImage *screen);
    void reverse() { rev = !rev; }

private:
    int th, last_sel, type, scale, wid, rev;
};

extern int cur_bg,cur_fg,cur_char;

#endif

