/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include <stdlib.h>

#include "common.h"

#include "image.h"
#include "video.h"

void update_dirty(image *im, int xoff, int yoff)
{
    // make sure the image has the ability to contain dirty areas
    CHECK(im->m_special);

    if(im->m_special->keep_dirt == 0)
    {
        put_image(im, xoff, yoff);
    }
    else
    {
        int count = im->m_special->dirties.number_nodes();
        dirty_rect *dr = (dirty_rect *)(im->m_special->dirties.first());
        while(count > 0)
        {
            put_part_image(im, xoff + dr->dx1, yoff + dr->dy1,
                           dr->dx1, dr->dy1, dr->dx2 + 1, dr->dy2 + 1);
            dirty_rect *tmp = dr;
            dr = (dirty_rect *)(dr->next());
            im->m_special->dirties.unlink(tmp);
            delete tmp;
            count--;
        }
    }

    update_window_done();
}

void put_image(image * im, int x, int y)
{
    put_part_image(im, x, y, 0, 0, im->Size().x - 1, im->Size().y - 1);
}

