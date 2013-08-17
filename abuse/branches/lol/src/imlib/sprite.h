/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __SPRITE_HPP
#define __SPRITE_HPP

#include "image.h"
#include "linked.h"

class ASprite : public linked_node
{
public:
    ASprite(AImage *screen, AImage *visual, ivec2 pos);
    ~ASprite();

    void SetVisual(AImage *visual, int delete_old = 0);

    AImage *m_visual, *m_screen, *m_save;

private:
    ivec2 m_pos;
};

#endif

