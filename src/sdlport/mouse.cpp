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

#include <SDL.h>

#include "common.h"

#include "video.h"
#include "sprite.h"
#include "image.h"
#include "filter.h"
#include "mouse.h"

unsigned char def_mouse[]=
{
    0,2,0,0,0,0,0,0,
    2,1,2,0,0,0,0,0,
    2,1,1,2,0,0,0,0,
    2,1,1,1,2,0,0,0,
    2,1,1,1,1,2,0,0,
    2,1,1,1,1,1,2,0,
    0,2,1,1,2,2,0,0,
    0,0,2,1,1,2,0,0,
    0,0,2,1,1,2,0,0,
    0,0,0,2,2,0,0,0
};

//
// Constructor
//
JCMouse::JCMouse(image *screen, palette *pal)
{
    image *im;
    int br, dr;
    Filter f;
    but = 0;
    cx = cy = 0;
    here = 1;

    m_screen = screen;
    br = pal->brightest( 1 );
    dr = pal->darkest( 1 );
    f.Set( 1, br );
    f.Set( 2, dr );
    im = new image(vec2i(8, 10), def_mouse);
    f.Apply( im );
    sp = new sprite(Screen, im, 100, 100);
    mx = Screen->Size().x / 2;
    my = Screen->Size().y / 2;
}

//
// Destructor
//
JCMouse::~JCMouse()
{
    delete sp->visual;
    delete sp;
}

//
// set_shape()
// Set the shape of the mouse cursor
//
void JCMouse::set_shape(image *im, int centerx, int centery)
{
    sp->change_visual(im, 1);
    cx = -centerx;
    cy = -centery;
}

//
// set_position()
// Set the position of the mouse cursor
//
void JCMouse::set_position(int new_mx, int new_my)
{
    // Make sure the values we are given are sensible.
    mx = Min(new_mx, m_screen->Size().x - 1);
    my = Min(new_my, m_screen->Size().y - 1);

    // Set the new position
    SDL_WarpMouse(mx, my);
}

//
// update()
// Update the mouse position and button states
//
void JCMouse::update(int newx, int newy, int new_but)
{
    if (newx < 0)
    {
        lx = mx;
        ly = my;
        lbut = but;

        uint8_t mask = SDL_GetMouseState(&mx, &my);
        but = ((mask & SDL_BUTTON(1)) != 0) |
              ((mask & SDL_BUTTON(2)) != 0) << 2 |
              ((mask & SDL_BUTTON(3)) != 0) << 1;
    }
    else
    {
        mx = newx;
        my = newy;
        but = new_but;
    }
}

