/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 2001 Anthony Kruize <trandor@labyrinth.net.au>
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 */

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include "common.h"

#include "imlib/filter.h"
#include "imlib/video.h"
#include "imlib/image.h"

#include "setup.h"

array<uint8_t> g_screen;

AImage *main_screen = NULL;
int mouse_xscale, mouse_yscale;
int xres, yres;

extern Palette *lastl;
extern flags_struct flags;

//
// CreateScreen()
// Set the video mode
//
void CreateScreen(int argc, char **argv)
{
    mouse_xscale = 1 << 16;
    mouse_yscale = 1 << 16;

    /* Temporary screen buffer */
    g_screen.Resize(xres * yres);

    // Create the screen image
    main_screen = new AImage(ivec2(xres, yres), 2);
    if (main_screen == NULL)
    {
        // Our screen image is no good, we have to bail.
        printf("Video : Unable to create screen image.\n");
        exit(1);
    }
    main_screen->clear();

    update_dirty(main_screen);
}

//
// DestroyScreen()
// Shutdown the video mode
//
void DestroyScreen()
{
    if (lastl)
        delete lastl;
    lastl = NULL;

    delete main_screen;
}

// put_part_image()
// Draw only dirty parts of the image to screen
//
void put_part_image(AImage *im, int x, int y, int x1, int y1, int x2, int y2)
{
    int xe, ye;
    int srcx, srcy;
    uint8_t *dpixel;

    if (y > yres || x > xres)
        return;

    ASSERT(x1 >= 0);
    ASSERT(x2 >= x1);
    ASSERT(y1 >= 0);
    ASSERT(y2 >= y1);

    // Adjust if we are trying to draw off the screen
    if (x < 0)
    {
        x1 += -x;
        x = 0;
    }
    srcx = x1;
    if (x + (x2 - x1) >= xres)
        xe = xres - x + x1 - 1;
    else
        xe = x2;

    if (y < 0)
    {
        y1 += -y;
        y = 0;
    }
    srcy = y1;
    if (y + (y2 - y1) >= yres)
        ye = yres - y + y1 - 1;
    else
        ye = y2;

    if (srcx >= xe || srcy >= ye)
        return;

    int w = xe - srcx;
    int h = ye - srcy;

    dpixel = g_screen.Data() + x + y * xres;

    // Update surface part
    for (int i = 0; i < h; i++)
    {
        memcpy(dpixel, im->scan_line(srcy) + srcx , w);
        dpixel += xres;
        srcy++;
    }
}

// ---- support functions ----

void UpdateScreen()
{
    /* FIXME: synchronisation */
}

