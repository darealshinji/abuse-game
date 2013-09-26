/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef VIS_OBJECT_HPP
#define VIS_OBJECT_HPP

#include "imlib/window.h"
#include "imlib/filter.h"

class AVisualObject
{
public:
    virtual void Draw(AImage *screen, ivec2 pos, Filter *f) = 0;
    virtual ivec2 Size() const = 0;
    virtual ~AVisualObject() { }
};

#endif

