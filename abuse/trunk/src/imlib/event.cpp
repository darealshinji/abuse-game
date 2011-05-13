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

//
// Destructor
//
EventHandler::~EventHandler()
{
    delete mouse;
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

