/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 2001 Anthony Kruize <trandor@labyrinth.net.au>
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
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

#include "event.h"
#include "video.h"
#include "filter.h"

//
// Constructor
//
EventHandler::EventHandler(image *screen, palette *pal)
{
    CHECK(screen && pal);
    last_keystat = get_key_flags();
    m_pending = 0;

    m_screen = screen;

    // Mouse stuff
    uint8_t mouse_sprite[]=
    {
        0, 2, 0, 0, 0, 0, 0, 0,
        2, 1, 2, 0, 0, 0, 0, 0,
        2, 1, 1, 2, 0, 0, 0, 0,
        2, 1, 1, 1, 2, 0, 0, 0,
        2, 1, 1, 1, 1, 2, 0, 0,
        2, 1, 1, 1, 1, 1, 2, 0,
        0, 2, 1, 1, 2, 2, 0, 0,
        0, 0, 2, 1, 1, 2, 0, 0,
        0, 0, 2, 1, 1, 2, 0, 0,
        0, 0, 0, 2, 2, 0, 0, 0
    };

    Filter f;
    f.Set(1, pal->brightest(1));
    f.Set(2, pal->darkest(1));
    image *im = new image(vec2i(8, 10), mouse_sprite);
    f.Apply(im);

    m_sprite = new sprite(screen, im, 100, 100);
    m_pos = m_lastpos = screen->Size() / 2;
    m_button = m_lastbutton = 0;
    m_center = vec2i(0, 0);

    // Platform-specific stuff
    SysInit();
}

//
// Destructor
//
EventHandler::~EventHandler()
{
    ;
}

//
// flush_screen()
// Redraw the screen
//
void EventHandler::flush_screen()
{
    update_dirty(main_screen);
}

//
// add_redraw()
// Add a redraw rectangle.
//
void EventHandler::add_redraw(int X1, int Y1, int X2, int Y2, void *start)
{
    Event *ev;
    ev = new Event;
    ev->type = EV_REDRAW;
    ev->redraw.x1 = X1;
    ev->redraw.x2 = X2;
    ev->redraw.y1 = Y1;
    ev->redraw.y2 = Y2;
    ev->redraw.start = start;
    Push(ev);
}

