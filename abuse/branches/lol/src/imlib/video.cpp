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

#include <stdlib.h>

#include "common.h"

#include "image.h"
#include "video.h"

void update_dirty(AImage *im, int xoff, int yoff)
{
    // make sure the image has the ability to contain dirty areas
    ASSERT(im->m_special);

    if (im->m_special->keep_dirt == 0)
    {
        put_image(im, xoff, yoff);
    }
    else
    {
        for (int i = 0; i < im->m_special->m_dirties.Count(); ++i)
        {
            ADirtyRect &rect = im->m_special->m_dirties[i];

            put_part_image(im, xoff + rect.m_aa.x, yoff + rect.m_aa.y,
                           rect.m_aa.x, rect.m_aa.y,
                           rect.m_bb.x + 1, rect.m_bb.y + 1);
        }

        im->m_special->m_dirties.Resize(0);
    }

    UpdateScreen();
}

void put_image(AImage * im, int x, int y)
{
    put_part_image(im, x, y, 0, 0, im->Size().x - 1, im->Size().y - 1);
}

