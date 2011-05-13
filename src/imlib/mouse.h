/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __MOUSE_HPP_
#define __MOUSE_HPP_

#include "image.h"
#include "sprite.h"

class JCMouse
{
public:
    JCMouse(image *screen, palette *pal);
    ~JCMouse();

    void set_shape(image *im, int centerx = 0, int centery = 0);
    void update(int newx = -1, int newy = -1, int new_but = -1);
    void set_shape(image *im) { if (here) sp->change_visual(im); }
    sprite *mouse_sprite() { return sp; }
    void set_position(int new_mx, int new_my);

    int x() { return here ? mx : 0; }
    int y() { return here ? my : 0; }
    int drawx() { return mx - cx; }
    int drawy() { return my - cy; }
    int lastx() { return here ? lx : 0; }
    int lasty() { return here ? ly : 0; }
    int last_button() { return here ? lbut : 0; }
    int button() { return but; }
    int exists() { return here; }

#ifdef __POWERPC__
    int set_button(int b) { return (but = b); }
#endif

private:
    int here, but;
    sprite *sp;
    image *m_screen;
    int lx, ly, lbut, mx, my;
    int cx, cy; // center of mouse cursor
};

#endif

