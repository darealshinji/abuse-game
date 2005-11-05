/************************
 * mouse.c
 * SDL port for Abuse
 * by Anthony Kruize
 ************************/

#include <SDL.h>

#include "video.hpp"
#include "sprite.hpp"
#include "image.hpp"
#include "filter.hpp"
#include "mdlread.hpp"
#include "mouse.hpp"

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
JCMouse::JCMouse( image *Screen, palette *pal )
{
	image *im;
	int br, dr;
	filter f;
	but = 0;
	cx = cy = 0;
	here = 1;
	sp = NULL;

	screen = Screen;
	br = pal->brightest( 1 );
	dr = pal->darkest( 1 );
	f.set( 1, br );
	f.set( 2, dr );
	im = new image( 8, 10, def_mouse );
	f.apply( im );
	sp = new sprite( Screen, im, 100, 100 );
	mx = Screen->width() / 2;
	my = Screen->height() / 2;
}

//
// Destructor
//
JCMouse::~JCMouse()
{
	if( sp )
	{
		delete sp->visual;
		delete sp;
	}
}

//
// set_shape()
// Set the shape of the mouse cursor
//
void JCMouse::set_shape( image *im, int centerx, int centery )
{
	sp->change_visual( im, 1 );
	cx = -centerx;
	cy = -centery;
}

//
// set_position()
// Set the position of the mouse cursor
//
void JCMouse::set_position( int new_mx, int new_my )
{
	// Make sure the values we are given are sensible.
	if( new_mx > 319 )
	{
		new_mx = 319;
	}
	if( new_my > 199 )
	{
		new_my = 199;
	}

	// Set the new position
	mx = new_mx;
	my = new_my;
	SDL_WarpMouse( new_mx, new_my );
}

//
// update()
// Update the mouses position and buttons states
//
void JCMouse::update( int newx, int newy, int new_but )
{
	if( newx < 0 )
	{
		Uint8 mask;

		lx = mx;
		ly = my;
		lbut = but;
		mask = SDL_GetMouseState( &mx, &my );
		but = ( ( mask & SDL_BUTTON(1) ) != 0 ) |
			  ( ( mask & SDL_BUTTON(2) ) != 0 ) << 2 |
			  ( ( mask & SDL_BUTTON(3) ) != 0 ) << 1;
	}
	else
	{
		mx = newx;
		my = newy;
		but = new_but;
	}
}
