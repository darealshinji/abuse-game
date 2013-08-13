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

#ifdef __APPLE__
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#else
#   include <GL/gl.h>
#   include <GL/glu.h>
#endif    /* __APPLE__ */

#include "common.h"

#include "filter.h"
#include "video.h"
#include "image.h"
#include "setup.h"

#if 0 // STUB
SDL_Surface *window = NULL, *surface = NULL;
#endif
image *main_screen = NULL;
int mouse_xscale, mouse_yscale;
int xres, yres;

extern Palette *lastl;
extern flags_struct flags;
#if 0 // STUB
GLfloat texcoord[4];
GLuint texid;
SDL_Surface *texture = NULL;
#endif

#if 0 // STUB
//
// power_of_two()
// Get the nearest power of two
//
static int power_of_two(int input)
{
    int value;
    for(value = 1 ; value < input ; value <<= 1);
    return value;
}
#endif

//
// CreateScreen()
// Set the video mode
//
void CreateScreen(int argc, char **argv)
{
    printf("Stub for CreateScreen(...)\n");

#if 0 // STUB
    const SDL_VideoInfo *vidInfo;
    int vidFlags = SDL_HWPALETTE;

    // Check for video capabilities
    vidInfo = SDL_GetVideoInfo();
    if (vidInfo->hw_available)
        vidFlags |= SDL_HWSURFACE;
    else
        vidFlags |= SDL_SWSURFACE;

    if (flags.fullscreen)
        vidFlags |= SDL_FULLSCREEN;

    vidFlags |= SDL_DOUBLEBUF;
#endif

    // Calculate the window scale for the mouse
    mouse_xscale = 1 << 16;
    mouse_yscale = 1 << 16;

#if 0 // STUB
    // allow doublebuffering in with gl too
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, GL_TRUE);
    // set video gl capability
    vidFlags |= SDL_OPENGL;

    // Set the icon for this window.  Looks nice on taskbars etc.
    SDL_WM_SetIcon(SDL_LoadBMP("abuse.bmp"), NULL);

    // Create the window with a preference for 8-bit (palette animations!), but accept any depth */
    window = SDL_SetVideoMode(xres, yres, 8, vidFlags | SDL_ANYFORMAT);
    if (window == NULL)
    {
        printf("Video : Unable to set video mode : %s\n", SDL_GetError());
        exit(1);
    }
#endif

    // Create the screen image
    main_screen = new image(ivec2(xres, yres), NULL, 2);
    if (main_screen == NULL)
    {
        // Our screen image is no good, we have to bail.
        printf("Video : Unable to create screen image.\n");
        exit(1);
    }
    main_screen->clear();

#if 0 // STUB
    // texture width/height should be power of 2
    // FIXME: we can use GL_ARB_texture_non_power_of_two or
    // GL_ARB_texture_rectangle to avoid the extra memory allocation
    int w = power_of_two(xres);
    int h = power_of_two(yres);

    // create texture surface
    texture = SDL_CreateRGBSurface(SDL_SWSURFACE, w , h , 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
                0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
#else
                0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
#endif

    // setup 2D gl environment
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    glViewport(0, 0, window->w, window->h);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0.0, (GLdouble)window->w, (GLdouble)window->h, 0.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // texture coordinates
    texcoord[0] = 0.0f;
    texcoord[1] = 0.0f;
    texcoord[2] = (GLfloat)xres / texture->w;
    texcoord[3] = (GLfloat)yres / texture->h;

    // create an RGBA texture for the texture surface
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, flags.antialias);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, flags.antialias);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->w, texture->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->pixels);

    // Create our 8-bit surface
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, window->w, window->h, 8, 0xff, 0xff, 0xff, 0xff);
    if (surface == NULL)
    {
        // Our surface is no good, we have to bail.
        printf("Video : Unable to create 8-bit surface.\n");
        exit(1);
    }

    printf("Video : %dx%d %dbpp\n", window->w, window->h, window->format->BitsPerPixel);

    // Set the window caption
    SDL_WM_SetCaption("Abuse", "Abuse");

    // Grab and hide the mouse cursor
    SDL_ShowCursor(0);
    if (flags.grabmouse)
        SDL_WM_GrabInput(SDL_GRAB_ON);
#endif

    update_dirty(main_screen);
}

//
// DestroyScreen()
// Shutdown the video mode
//
void DestroyScreen()
{
    printf("Stub for DestroyScreen()\n");

    if (lastl)
        delete lastl;
    lastl = NULL;
#if 0 // STUB
    // Free our 8-bit surface
    if (surface)
        SDL_FreeSurface(surface);
    if (texture)
        SDL_FreeSurface(texture);
#endif

    delete main_screen;
}

// put_part_image()
// Draw only dirty parts of the image
//
void put_part_image(image *im, int x, int y, int x1, int y1, int x2, int y2)
{
    printf("Stub for put_part_image(%i,%i %i,%i %i,%i)\n", x, y, x1, y1, x2, y2);

#if 0 // STUB
    int xe, ye;
    int srcx, srcy;
    uint8_t *dpixel;
    uint16_t dinset;

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

    dinset = ((surface->w - w)) * surface->format->BytesPerPixel;

    // Lock the surface if necessary
    if (SDL_MUSTLOCK(surface))
        SDL_LockSurface(surface);

    dpixel = (uint8_t *)surface->pixels;
    dpixel += (x + (y * surface->w)) * surface->format->BytesPerPixel;

    // Update surface part
    dpixel = ((uint8_t *)surface->pixels) + y * surface->w + x;
    for (int i = 0; i < h; i++)
    {
        memcpy(dpixel, im->scan_line(srcy) + srcx , w);
        dpixel += surface->w;
        srcy++;
    }

    // Unlock the surface if we locked it.
    if (SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);
#endif
}

//
// load()
// Set the palette
//
void Palette::load()
{
    printf("Stub for palette::load\n");

    if (lastl)
        delete lastl;
    lastl = Copy();

    // Force to only 256 colours.
    // Shouldn't be needed, but best to be safe.
    if (m_colors.Count() > 256)
        m_colors.Resize(256);

#if 0 // STUB
    SDL_Color *sdlcolors = new SDL_Color[m_colors.Count()];
    for(int i = 0; i < m_colors.Count(); i++)
    {
        sdlcolors[i].r = m_colors[i].r;
        sdlcolors[i].g = m_colors[i].g;
        sdlcolors[i].b = m_colors[i].b;
    }
    SDL_SetColors(surface, sdlcolors, 0, m_colors.Count());

    if (window->format->BitsPerPixel == 8)
        SDL_SetColors(window, sdlcolors, 0, m_colors.Count());
#endif

    // Now redraw the surface
    UpdateScreen();
}

// ---- support functions ----

void UpdateScreen()
{
    printf("Stub for UpdateScreen()\n");

#if 0 // STUB
    // convert color-indexed surface to RGB texture
    SDL_BlitSurface(surface, NULL, texture, NULL);

    // Texturemap complete texture to surface so we have free scaling
    // and antialiasing
    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0, texture->w, texture->h,
                    GL_RGBA, GL_UNSIGNED_BYTE, texture->pixels);
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(texcoord[0], texcoord[1]);
        glVertex3i(0, 0, 0);
        glTexCoord2f(texcoord[2], texcoord[1]);
        glVertex3i(window->w, 0, 0);
        glTexCoord2f(texcoord[0], texcoord[3]);
        glVertex3i(0, window->h, 0);
        glTexCoord2f(texcoord[2], texcoord[3]);
        glVertex3i(window->w, window->h, 0);
    glEnd();

    SDL_GL_SwapBuffers();
#endif
}

