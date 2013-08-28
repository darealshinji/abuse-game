/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __TOOLS_HPP_
#define __TOOLS_HPP_

#include "window.h"
#include "input.h"
#include "specs.h"
#include "scroller.h"
#include "visobj.h"

class AToolPicker : public AScrollPicker
{
public:
    // you are expected keep image and id list in memory, AToolPicker does not copy them
    AToolPicker(ivec2 pos, int id,
                int show_h, AVisualObject **Icons, int *Ids, int total_ic,
                Palette *icon_palette, Palette *pal);
    ~AToolPicker();

    virtual void DrawItem(AImage *screen, ivec2 pos, int num, int active);
    virtual int total() { return total_icons; }
    virtual int item_width() { return iw; }
    virtual int item_height() { return ih; }

    virtual void note_new_current(AImage *screen, InputManager *inm, int x)
    {
        wm->Push(Event(ids[x], nullptr));
    }

    void remap(Palette *pal, AImage *screen);

private:
    Filter *map;
    AVisualObject **icons;
    int *ids;
    int total_icons;
    int iw,ih;
    Palette *old_pal;
};

#endif







