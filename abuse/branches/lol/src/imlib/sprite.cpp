/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#if HAVE_CONFIG_H
#   include "config.h"
#endif

#include <math.h>

#include "common.h"

#include "imlib/video.h"
#include "imlib/image.h"
#include "imlib/palette.h"

#include "sprite.h"

ASprite::ASprite(AImage *screen, AImage *visual, ivec2 pos)
{
    ASSERT(visual);
    ASSERT(screen);

    m_pos = pos;
    m_visual = visual;
    m_screen = screen;
    m_save = new AImage(visual->Size());

    if (m_pos + visual->Size() >= ivec2::zero && m_pos < ivec2(xres, yres))
        m_save->PutPart(m_screen, ivec2::zero, m_pos, m_pos + m_save->Size());
}

ASprite::~ASprite()
{
    delete m_save;
}

void ASprite::SetVisual(AImage *visual, int delete_old)
{
    if (delete_old)
        delete m_visual;
    m_visual = visual;
    if (m_save->Size() != visual->Size())
    {
        delete m_save;
        m_save = new AImage(visual->Size());
    }

    if (m_pos + visual->Size() >= ivec2::zero && m_pos < ivec2(xres, yres))
        m_save->PutPart(m_screen, ivec2::zero, m_pos, m_pos + m_save->Size());
}

