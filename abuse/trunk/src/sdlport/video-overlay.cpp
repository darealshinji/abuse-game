/************************
 * video.c
 * SDL port for Abuse
 * by Anthony Kruize
 ************************/

#include <SDL.h>

#include "filter.hpp"
#include "globals.hpp"
#include "system.h"
#include "video.hpp"
#include "macs.hpp"
#include "image.hpp"
#include "jmalloc.hpp"
#include "setup.h"

SDL_Surface *window = NULL, *surface = NULL;
SDL_Overlay *overlay;
image *screen = NULL;
unsigned char current_background;

extern unsigned int xres, yres;
extern palette *lastl;
extern flags_struct flags;

unsigned short win_xres, win_yres;
int win_xscale, win_yscale;


// Forward declarations
void blit_surface( SDL_Rect *srcrect, SDL_Rect *dstrect );

//
// set_mode()
// Set the video mode
//
void set_mode( int mode, int argc, char **argv )
{
	const SDL_VideoInfo *vidInfo;
	int vidFlags = SDL_HWPALETTE;

	// Check for video capabilities
	vidInfo = SDL_GetVideoInfo();
	if( vidInfo->hw_available )
	{
		vidFlags |= SDL_HWSURFACE;
	}
	else
	{
		vidFlags |= SDL_SWSURFACE;
	}
	if( flags.fullscreen )
	{
		vidFlags |= SDL_FULLSCREEN;
	}
	if( flags.doublebuf )
	{
		vidFlags |= SDL_DOUBLEBUF;
	}

	// Calculate the window scale
	win_xscale = (flags.xres << 16) / xres;
	win_yscale = (flags.yres << 16) / yres;

	// Set the icon for this window.  Looks nice on taskbars etc.
	SDL_WM_SetIcon( SDL_LoadBMP("abuse.bmp"), NULL );

	// Create the window
	window = SDL_SetVideoMode( flags.xres, flags.yres, 0, vidFlags );
	if( window == NULL )
	{
		printf( "Video : Unable to set video mode : %s\n", SDL_GetError() );
		exit( 1 );
	}

	// Use overlays because they are faster for scaling
	if( flags.overlay )
	{
		overlay = SDL_CreateYUVOverlay( xres, yres, SDL_UYVY_OVERLAY, window );
	}

	// Create the screen image
	screen = new image( xres, yres, NULL, 2 );
	if( screen == NULL )
	{
		// Our screen image is no good, we have to bail.
		printf( "Video : Unable to create screen image.\n" );
		exit( 1 );
	}
	screen->clear();

	// Create our 8-bit surface
	surface = SDL_CreateRGBSurface( SDL_SWSURFACE, window->w, window->h, 8, 0xff, 0xff, 0xff, 0xff );
	if( surface == NULL )
	{
		// Our surface is no good, we have to bail.
		printf( "Video : Unable to create 8-bit surface.\n" );
		exit( 1 );
	}

	printf( "Video : Using a window of %dx%d %dbpp\n", window->w, window->h, window->format->BitsPerPixel );

	// Set the window caption
	SDL_WM_SetCaption( "Abuse-SDL", "Abuse-SDL" );

	// Grab and hide the mouse cursor
	SDL_ShowCursor( 0 );
	if( flags.grabmouse )
	{
		SDL_WM_GrabInput( SDL_GRAB_ON );
	}

	update_dirty( screen );
}

//
// close_graphics()
// Shutdown the video mode
//
void close_graphics()
{
	if( lastl )
	{
		delete lastl;
	}
	lastl = NULL;
	// Free our 8-bit surface
	if( surface )
	{
		SDL_FreeSurface( surface );
	}
	// Free our overlay
	if( overlay )
	{
		SDL_FreeYUVOverlay( overlay );
	}
	delete screen;
}

// put_part_image()
// Draw only dirty parts of the image
//
void put_part_image( image *im, int x, int y, int x1, int y1, int x2, int y2 )
{
	int xs, xe, ys, ye;
	SDL_Rect srcrect, dstrect;
	int ii, jj;
	int srcx, srcy, xstep, ystep;
	Uint8 *dpixel;
	Uint16 dinset;
	int dest_addr, line_width;

	if( y > (int)yres || x > (int)xres )
	{
		return;
	}
	CHECK( x1 >= 0 && x2 >= x1 && y1 >= 0 && y2 >= y1 );

	// Adjust if we are trying to draw off the screen
	if( x < 0 )
	{
		x1 += -x;
		x = 0;
	}
	srcrect.x = x1;
	if( x + ( x2 - x1 ) >= xres )
	{
		xe = xres - x + x1 - 1;
	}
	else
	{
		xe = x2;
	}
	if( y < 0 )
	{
		y1 += -y;
		y = 0;
	}
	srcrect.y = y1;
	if( y + ( y2 - y1 ) >= yres )
	{
		ye = yres - y + y1 - 1;
	}
	else
	{
		ye = y2;
	}
	if( srcrect.x >= xe || srcrect.y >= ye )
		return;

	if( SDL_LockYUVOverlay( overlay ) == -1 )
	{
		// we failed to lock
		printf( "ERROR - Failed to lock overlay.\n" );
		return;
	}

	// This is our old copying code, left here for now just in case I still nee it
	// Set up our blit rectangle
	dstrect.w = xe - xs;
	dstrect.h = ye - ys;
	dstrect.x = x;
	dstrect.y = y;
	line_width = xe - xs + 1;

	if( SDL_LockYUVOverlay( overlay ) == -1 )
	{
		// we failed to lock
		printf( "ERROR - Failed to lock overlay.\n" );
		return;
	}

	// fill buffer with current surface
/*	for( int ii = ys; ii <= ye; ii++ )
	{
//		dest_addr = ( (dstrect.y + ii - ys) * screen_width ) + dstrect.x;
		dest_addr = ( (dstrect.y + ii - ys) * overlay->pitches[0] ) + dstrect.x;
//		memcpy( &((char *)(surface->pixels))[dest_addr], im->scan_line( ii ) + xs, line_width );
//		memcpy( &((char *)(overlay->pixels))[dest_addr], im->scan_line( ii ) + xs, line_width );
	}*/
	char blah = 0xff;
	for( int ii = 0; ii < overlay->pitches[0] * 2; ii++ )
	memcpy( &((char *)(overlay->pixels))[ii], ((char *)(&blah)), 1 );

	SDL_UnlockYUVOverlay( overlay );

/*	// Scale the image onto the surface
	srcrect.w = xe - srcrect.x;
	srcrect.h = ye - srcrect.y;
	dstrect.x = ((x * win_xscale) >> 16);
	dstrect.y = ((y * win_yscale) >> 16);
	dstrect.w = ((srcrect.w * win_xscale) >> 16);
	dstrect.h = ((srcrect.h * win_yscale) >> 16);

	xstep = (srcrect.w << 16) / dstrect.w;
	ystep = (srcrect.h << 16) / dstrect.h;

	srcy = ((srcrect.y) << 16);
	dinset = ((surface->w - dstrect.w)) * surface->format->BytesPerPixel;

	// Lock the surface if necessary
	if( SDL_MUSTLOCK( surface ) )
	{
		SDL_LockSurface( surface );
	}
	dpixel = (Uint8 *)surface->pixels;
	dpixel += (dstrect.x + ((dstrect.y) * surface->w)) * surface->format->BytesPerPixel;

	for( ii = 0; ii < dstrect.h; ii++ )
	{
		srcx = (srcrect.x << 16);
		for( jj = 0; jj < dstrect.w; jj++ )
		{
			memcpy( dpixel, im->scan_line( (srcy >> 16) ) + ((srcx >> 16) * surface->format->BytesPerPixel), surface->format->BytesPerPixel );
			dpixel += surface->format->BytesPerPixel;
			srcx += xstep;
		}
		dpixel += dinset;
		srcy += ystep;
	}

	// Now unlock the surface if we locked it.
	if( SDL_MUSTLOCK( surface ) )
	{
		SDL_UnlockSurface( surface );
	} */

	// Now blit the surface
	blit_surface( &dstrect, &dstrect );
}

//
// put_image()
// Draw the entire image
//
void put_image( image * im, int x, int y )
{
	put_part_image( im, x, y, 0, 0, im->width() - 1, im->height() - 1 );
}

//
// update_dirty_window()
// Update the dirty parts of the window
//
void update_dirty_window( image *im, int xoff, int yoff )
{
	int count;
	dirty_rect *dr, *q;
	CHECK( im->special ); // make sure the image has the ability to contain dirty areas
	if( im->special->keep_dirt == 0 )
	{
		put_image( im, xoff, yoff );
	}
	else
	{
		count = im->special->dirties.number_nodes();
		if( !count )
			return;  // if nothing to update, return
		dr = (dirty_rect *)( im->special->dirties.first() );
		while( count > 0 )
		{
			put_part_image( im, xoff + dr->dx1, yoff + dr->dy1, dr->dx1, dr->dy1, dr->dx2 + 1, dr->dy2 + 1 );
			q = dr;
			dr = (dirty_rect *)( dr->next() );
			im->special->dirties.unlink( ( linked_node *)q );
			delete q;
			count--;
		}
	}
}

//
// update_dirty()
// Update the dirty parts of the image
//
void update_dirty( image *im, int xoff, int yoff )
{
	update_dirty_window( im, xoff, yoff );
}

//
// make_page()
//
void image::make_page( short width, short height, unsigned char *page_buffer )
{
	if( page_buffer )
	{
		data = page_buffer;
	}
	else
	{
		data = (unsigned char *)jmalloc( width * height, "image::data" );
	}
}

//
// delete_page()
//
void image::delete_page()
{
	if( !special || !special->static_mem )
	{
		jfree( data );
	}
}

//
// load()
// Set the palette
//
void palette::load()
{
	if( lastl )
	{
		delete lastl;
	}
	lastl = copy();

	// Force to only 256 colours.
	// Shouldn't be needed, but best to be safe.
	if( ncolors > 256 )
	{
		ncolors = 256;
	}

	SDL_Color colors[ncolors];
	for( int ii = 0; ii < ncolors; ii++ )
	{
		colors[ii].r = red(ii);
		colors[ii].g = green(ii);
		colors[ii].b = blue(ii);
	}
	SDL_SetColors( surface, colors, 0, ncolors );
	if( window->format->BitsPerPixel == 8 )
	{
		SDL_SetColors( window, colors, 0, ncolors );
	}

	// Now redraw the surface
	blit_surface( NULL, NULL );
}

//
// load_nice()
//
void palette::load_nice()
{
	load();
}

// ---- support functions ----

void blit_surface( SDL_Rect *srcrect, SDL_Rect *dstrect)
{
	SDL_Surface *cvtsfc = SDL_ConvertSurface( surface, window->format, SDL_HWSURFACE );
	if( cvtsfc == NULL )
	{
		// Our convert surface died...bad :(
		return;
	}

	if( flags.overlay )
	{
		SDL_Rect or;
		or.x = 0;
		or.y = 0;
		or.w = window->w;
		or.h = window->h;
		SDL_DisplayYUVOverlay( overlay, &or );
	}

//	SDL_BlitSurface( cvtsfc, srcrect, window, dstrect );
	SDL_FreeSurface( cvtsfc );

	// Flip the backbuffer to the front
	if( flags.doublebuf )
	{
		SDL_Flip( window );
	}
	else
	{
		if( dstrect == NULL )
		{
			SDL_UpdateRect( window, 0, 0, 0, 0 );
		}
		else
		{
			SDL_UpdateRect( window, dstrect->x, dstrect->y, dstrect->w, dstrect->h );
		}
	}
}
