// Win32 DDraw graphics support


#include "globals.hpp"
#include "system.h"
#include "video.h"
#include "dos.h"
#include "macs.hpp"
#include "bitmap.h"
#include "image.hpp"
#include "palette.hpp"
#include "jmalloc.hpp"
#include <conio.h>
#include <windows.h>
#include <ddraw.h>

#include <crtdbg.h>
#include "..\Abuse32.h"

#define WINDOW_A 0
#define WINDOW_B 1
#define WINDOW_DEFINED 1
#define WINDOW_WRITABLE 4

unsigned char current_background;
extern unsigned int xres,yres;
extern palette *lastl;

LPDIRECTDRAW7               g_pDD = NULL;         // DirectDraw object
LPDIRECTDRAWSURFACE7        g_pDDSPrimary = NULL; // DirectDraw primary surface
LPDIRECTDRAWSURFACE7        g_pDDSBack = NULL;    // DirectDraw back surface
LPDIRECTDRAWSURFACE7        g_pDDSWindow = NULL;
LPDIRECTDRAWPALETTE         g_pal = NULL;         // the palette
LPDIRECTDRAWCLIPPER         g_pClip = NULL;
LPDIRECTDRAWCLIPPER         g_pbClip = NULL;      // clipper for back buffer
PALETTEENTRY                color_array[256];

HPEN hPen;

image *screen, *backscreen;
DDSURFACEDESC2 dummy_sd;

int Windowed = 0;

RECT                    g_rcWindow;             // Saves the window size & pos.
RECT                    g_rcViewport;           // Pos. & size to blt from
RECT                    g_rcScreen;             // Screen pos. for blt

int get_option(char *name);
/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void ClearScreen()
{
    HRESULT hRet;
    DDBLTFX bltFX;
    ZeroMemory(&bltFX, sizeof(bltFX));
    bltFX.dwSize = sizeof(bltFX);
    bltFX.dwFillColor = 0x0;
    hRet = g_pDDSBack->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &bltFX);
    if (Windowed) {
        hRet = g_pDDSPrimary->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &bltFX);
        hRet = g_pDDSWindow->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &bltFX);
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void* Lock_Screen() 
{
    HRESULT hr;
    dummy_sd.dwSize = sizeof(dummy_sd);
    hr = g_pDDSBack->Lock(NULL, &dummy_sd, 0, NULL);
    if (hr == DDERR_SURFACELOST) {
//        g_pDD->RestoreAllSurfaces();
        hr = g_pDDSPrimary->Restore();
        ClearScreen();
        g_pDDSPrimary->SetPalette(g_pal);
    }
    if (hr == DD_OK)
        return dummy_sd.lpSurface;
    else
        return NULL;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void Unlock_Screen()
{
    g_pDDSBack->Unlock(NULL);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void Finish_Frame()
{
    HRESULT hRet;
    
    if (!Windowed) {
        hRet = g_pDDSPrimary->Flip(NULL, DDFLIP_WAIT);
        if (hRet == DDERR_SURFACELOST) {
            g_pDDSPrimary->Restore();
            ClearScreen();
            g_pDDSPrimary->SetPalette(g_pal);
        }
    }
    else {
        static DDBLTFX ddbltfx;
        static DDSURFACEDESC2 ddsdp, ddsdb;

        ZeroMemory(&ddbltfx, sizeof(ddbltfx));
        ddbltfx.dwSize = sizeof(ddbltfx);

        ZeroMemory(&ddsdp, sizeof(ddsdp));
        ddsdp.dwSize = sizeof(ddsdp);
        ZeroMemory(&ddsdb, sizeof(ddsdb));
        ddsdb.dwSize = sizeof(ddsdb);
    
        hRet = g_pDDSWindow->Lock(NULL, &ddsdp, 0, NULL);
        hRet = g_pDDSBack->Lock(NULL, &ddsdb, 0, NULL);
        DWORD *line_addrp;
        unsigned char *line_addrb;
        DWORD i, j;
        for (i = 0; i < ddsdb.dwHeight; i++) {
            line_addrp = (DWORD*) ddsdp.lpSurface + i * (ddsdp.lPitch / (ddsdp.ddpfPixelFormat.dwSize >> 3));
            line_addrb = (unsigned char*) ddsdb.lpSurface + (i * ddsdb.lPitch);
            for (j = 0; j < ddsdb.dwWidth; j++) {
                *line_addrp = *((DWORD*) (&color_array[line_addrb[j]]));
                line_addrp++;
            }
        }
        g_pDDSWindow->Unlock(NULL);
        g_pDDSBack->Unlock(NULL);

        g_pDDSPrimary->Blt(&g_rcScreen, g_pDDSWindow, NULL, 0, &ddbltfx);
//            hRet = g_pDDSBack->Blt(&drect, imSurface, &srect, 0, &ddbltfx);
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void ToggleWindowedMode()
{
    Windowed = !Windowed;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
HRESULT DDrawInit(int xres, int yres, int color_depth, int refresh)
{
    DDSURFACEDESC2              ddsd;
    DDSCAPS2                    ddscaps;
    HRESULT                     hRet;
    BYTE                        rgnDataBuffer[1024];
    
    ///////////////////////////////////////////////////////////////////////////
    // Create the main DirectDraw object
    ///////////////////////////////////////////////////////////////////////////
    hRet = DirectDrawCreateEx(NULL, (VOID**) &g_pDD, IID_IDirectDraw7, NULL);

    WriteErr(!hRet, "DDrawInit -- Couldn't create IDirectDraw7 interface");

    // Create the primary surface with 1 back buffer
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    if (!Windowed) {
        // Get exclusive mode
        hRet = g_pDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
        WriteErr(!hRet, "DDrawInit -- Couldn't Set Cooperative Level");

        // Set the video mode to 640x480x8
        hRet = g_pDD->SetDisplayMode(xres, yres, color_depth, refresh, 0);
        WriteErr(!hRet, "DDrawInit -- Couldn't Set Resolution");
        ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
        ddsd.dwBackBufferCount = 1;
        hRet = g_pDD->CreateSurface(&ddsd, &g_pDDSPrimary, NULL);
        WriteErr(!hRet, "DDrawInit -- Couldn't create primary surface");

        ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
        g_pDDSPrimary->GetPixelFormat(&ddsd.ddpfPixelFormat);

        // Get a pointer to the back buffer
        ZeroMemory(&ddscaps, sizeof(ddscaps));
        ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
        hRet = g_pDDSPrimary->GetAttachedSurface(&ddscaps, &g_pDDSBack);
        WriteErr(!hRet, "DDrawInit -- Couldn't get offscreen buffer");
    }
    else {
        SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_OVERLAPPEDWINDOW);
        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_WINDOWEDGE | WS_EX_APPWINDOW);
        SetWindowPos(hWnd, HWND_TOP, 0, 0, xres, yres, SWP_NOREDRAW | SWP_NOMOVE);
        
        MoveWindow(hWnd, 20, 20, xres, yres, FALSE);

        hRet = g_pDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
        
        ddsd.dwFlags = DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
        hRet = g_pDD->CreateSurface(&ddsd, &g_pDDSPrimary, NULL);
        
        ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
        ddsd.dwWidth        = xres;
        ddsd.dwHeight       = yres;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        hRet = g_pDD->CreateSurface(&ddsd, &g_pDDSWindow, NULL);

        ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
        ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED8;
        ddsd.ddpfPixelFormat.dwRGBBitCount = 8;
        ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
        ddsd.dwWidth        = xres;
        ddsd.dwHeight       = yres;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        hRet = g_pDD->CreateSurface(&ddsd, &g_pDDSBack, NULL);

//                SetWindowPos(hWnd, HWND_TOP, 20, 40, xres, yres, SWP_NOREDRAW | SWP_NOSIZE);
                GetWindowRect(hWnd, &g_rcWindow);
                AdjustWindowRectEx( &g_rcWindow, GetWindowLong(hWnd, GWL_STYLE), GetMenu(hWnd) != NULL,
                                    GetWindowLong(hWnd, GWL_EXSTYLE));

                SetWindowPos( hWnd, NULL, 0, 0, g_rcWindow.right-g_rcWindow.left, g_rcWindow.bottom-g_rcWindow.top,
                              SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );

                SetWindowPos( hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                              SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE );

            	GetClientRect(hWnd, &g_rcViewport);
            	GetClientRect(hWnd, &g_rcScreen);
            	ClientToScreen(hWnd, (POINT*)&g_rcScreen.left);
            	ClientToScreen(hWnd, (POINT*)&g_rcScreen.right);
    }

    ZeroMemory(color_array, sizeof(color_array));
    hRet = g_pDD->CreatePalette(DDPCAPS_8BIT, color_array, &g_pal, NULL);
    WriteErr(!hRet, "DDrawInit -- Couldn't create palette");
    if (!Windowed) {
        hRet = g_pDDSPrimary->SetPalette(g_pal);
        WriteErr(!hRet, "DDrawInit -- Couldn't set palette for primary surface");
    }
    hRet = g_pDDSBack->SetPalette(g_pal);
    WriteErr(!hRet, "DDrawInit -- Couldn't set palette for offscreen surface");

    hRet = g_pDD->CreateClipper(0, &g_pClip, NULL);
    WriteErr(!hRet, "DDrawInit -- Couldn't create clipper");
    
    hRet = g_pClip->SetHWnd(0, hWnd);
    WriteErr(!hRet, "DDrawInit -- Couldn't set clipper's window handle");
    
    hRet = g_pDDSPrimary->SetClipper(g_pClip);
    WriteErr(!hRet, "DDrawInit -- Couldn't set clipper for primary surface");

    if (Windowed) {

        hRet = g_pDD->CreateClipper(0, &g_pbClip, NULL);

        HRGN hrgn = CreateRectRgn(0, 0, ddsd.dwWidth, ddsd.dwHeight);
        GetRegionData(hrgn, sizeof(rgnDataBuffer), (RGNDATA*) rgnDataBuffer);
        DeleteObject(hrgn);

        hRet = g_pbClip->SetClipList((RGNDATA*) rgnDataBuffer, 0);
        hRet = g_pDDSBack->SetClipper(g_pbClip);
        WriteErr(!hRet, "DDrawInit -- Couldn't set clipper for offscreen surface"); /**/
    
        hRet = g_pDDSWindow->SetClipper(g_pbClip);
    }
    else {
        hRet = g_pDDSBack->SetClipper(g_pClip);
        WriteErr(!hRet, "DDrawInit -- Couldn't set clipper for offscreen surface"); /**/
    }

    return DD_OK;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void Free_DDraw()
{
    HRESULT hRet;
    if (g_pDD) {
        if (!Windowed) {
            hRet = g_pDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
            g_pDD->FlipToGDISurface();
        }
        if (Windowed && g_pDDSBack)
            g_pDDSBack->Release();
        if (g_pbClip)
            g_pbClip->Release();

        if (g_pDDSPrimary)
            g_pDDSPrimary->Release();
        if (g_pClip)
            g_pClip->Release();
        g_pDD->Release();
        if (g_pal)
            g_pal->Release();
    }
    g_pDDSPrimary = NULL;
    g_pDD = NULL;
    g_pDDSBack = NULL;
    g_pClip = NULL;
    g_pbClip = NULL;
    g_pal = NULL;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void set_mode(int mode, int argc, char **argv)
{
    int i;
    if (g_pDD)
        return;
    atexit(Free_DDraw);
    xres = 0; yres = 0;
    for (i = 1; i < argc; i++)
        if (!strcmp(argv[i], "-window"))
            Windowed = 1;

    for (i = 1; i < argc; i++)
        if (!strcmp(argv[i], "-size")) {
            xres = atoi(argv[i + 1]);
            yres = atoi(argv[i + 2]);
        }

    if (!xres) {
        xres = 320;
        yres = 200;
    }

    if (xres < 639 || yres < 399)
        for (i = 1; i < argc; i++)
            if (!strcmp(argv[i], "-2"))
                WriteErr(0, "Resolution must be > 640x400 to use -2 option\n");

    if (DDrawInit(xres, yres, 8 /* color depth */, 0 /*refresh rate*/) != DD_OK) {
        WriteErr(0, "Unable to set video mode.\nTry one that your video card supports.\n");
    };

    hPen = CreatePen(PS_SOLID, 1, 0);
    screen = new image(xres, yres, NULL, 0x0a);
    screen->clear();
    backscreen = new image(xres, yres, NULL, 0x0a);
    backscreen->clear();
    image_list.unlink(screen);
    image_list.unlink(backscreen);
    ClearScreen();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int get_vmode()
{ 
    return -1; 
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void close_graphics()
{
    if (lastl)
        delete lastl;
    lastl = NULL;
        
    if (screen)
        delete screen;
    screen = NULL;
    DeleteObject(hPen);
    image_uninit();
    Free_DDraw();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void put_part(image *im, int x, int y, int x1, int y1, int x2, int y2)
{
    HRESULT hRet;
    DDBLTFX ddbltfx;
    RECT drect, srect;
    drect.left = x; drect.right = x + x2 - x1 + 1; drect.top = y; drect.bottom = y + y2 - y1 + 1;
    srect.left = x1; srect.right = x2 + 1; srect.top = y1; srect.bottom = y2 + 1;
    ZeroMemory(&ddbltfx, sizeof(ddbltfx));
    ddbltfx.dwSize = sizeof(ddbltfx);

    if (!im->Surf) {
        DDSURFACEDESC2 ddsd;
        LPDIRECTDRAWSURFACE7 imSurface = NULL;
    
        ZeroMemory(&ddsd, sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd);
        ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
        ddsd.dwHeight = im->height();
        ddsd.dwWidth = im->width();
    
        hRet = g_pDD->CreateSurface(&ddsd, &imSurface, NULL);
        hRet = imSurface->Lock(NULL, &ddsd, 0, NULL);

        unsigned char *line_addr;
        line_addr = (unsigned char*) ddsd.lpSurface;
        if ((DWORD) ddsd.lPitch == ddsd.dwWidth)
            memcpy(line_addr, im->scan_line(0), (im->width()) * (im->height()));
        else {
            unsigned char* im_addr = im->scan_line(0);
            int i;
            for (i = 0; i < im->height(); i++) {
                memcpy(line_addr, im_addr, im->width());
                im_addr += im->width();
                line_addr += ddsd.lPitch;
            }
        }
        imSurface->Unlock(NULL);
        hRet = g_pDDSBack->Blt(&drect, imSurface, &srect, 0, &ddbltfx);
        imSurface->Release();
        imSurface = NULL;
    }
    else {
        hRet = g_pDDSBack->Blt(&drect, im->Surf, &srect, 0, &ddbltfx);
    }
    if (hRet == DD_OK)
        return;
    switch (hRet) {
        case DDERR_SURFACELOST:
            g_pDDSPrimary->Restore();
            break;
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void TransparentBlit(image *im, int x, int y, int color)
{
    HRESULT hRet;
    DDBLTFX ddbltfx;
    RECT drect, srect;
    drect.left = x; drect.right = x + im->width(); drect.top = y; drect.bottom = y + im->height();
    srect.left = 0; srect.right = im->width(); srect.top = 0; srect.bottom = im->height();
    ZeroMemory(&ddbltfx, sizeof(ddbltfx));
    ddbltfx.dwSize = sizeof(ddbltfx);
    ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue = ddbltfx.ddckSrcColorkey.dwColorSpaceHighValue = color;

    if (!im->Surf) {
        DDSURFACEDESC2 ddsd;
        LPDIRECTDRAWSURFACE7 imSurface = NULL;
    
        ZeroMemory(&ddsd, sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd);
        ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
        ddsd.dwHeight = im->height();
        ddsd.dwWidth = im->width();
    
        hRet = g_pDD->CreateSurface(&ddsd, &imSurface, NULL);
        hRet = imSurface->Lock(NULL, &ddsd, 0, NULL);

        unsigned char *line_addr;
        line_addr = (unsigned char*) ddsd.lpSurface;
        if ((DWORD) ddsd.lPitch == ddsd.dwWidth)
            memcpy(line_addr, im->scan_line(0), (im->width()) * (im->height()));
        else {
            unsigned char* im_addr = im->scan_line(0);
            int i;
            for (i = 0; i < im->height(); i++) {
                memcpy(line_addr, im_addr, im->width());
                im_addr += im->width();
                line_addr += ddsd.lPitch;
            }
        }
        imSurface->Unlock(NULL);
        hRet = g_pDDSBack->Blt(&drect, imSurface, &srect, DDBLT_KEYSRCOVERRIDE, &ddbltfx);
        imSurface->Release();
        imSurface = NULL;
    }
    else {
        hRet = g_pDDSBack->Blt(&drect, im->Surf, &srect, DDBLT_KEYSRCOVERRIDE, &ddbltfx);
    }
    if (hRet == DD_OK)
        return;
    switch (hRet) {
        case DDERR_SURFACELOST:
            g_pDDSPrimary->Restore();
            break;
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void put_image(image *im, int x, int y)
{ 
    put_part(im, x, y, 0, 0, im->width() - 1, im->height() - 1);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void update_dirty(image *im, int xoff, int yoff)
{
    put_image(im, xoff, yoff);
/*  int count;
  dirty_rect *dr,*q;
  CHECK(im->special);  // make sure the image has the ablity to contain dirty areas
  if (im->special->keep_dirt==0)
    put_image(im,0,0);
  else
  {
    count=im->special->dirties.number_nodes();
    if (!count) return;  // if nothing to update, return
    dr=(dirty_rect *)im->special->dirties.first();
    while (count>0)
    {
//      if (dr->dx1+xoff<0) dr->dx1=0-xoff;
//      if (dr->dy1+yoff<0) dr->dy1=0-yoff;
//      if (dr->dx2+xoff>xres) dr->dx2=xres-xoff;
//      if (dr->dy2+yres>yres) dr->dy2=yres-yoff;
//      if (dr->dx1<=dr->dx2 && dr->dy1<=dr->dy2)
      put_part(im,dr->dx1+xoff,dr->dy1+yoff,dr->dx1,dr->dy1,dr->dx2,dr->dy2);
      q=dr;
      dr=(dirty_rect *)dr->next();
      im->special->dirties.unlink((linked_node *)q);
      delete q;
      count--;
    }
  } */
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void palette::load()
{
    int i;
    HRESULT hr;
    unsigned char* entry = (unsigned char*) addr();
    for (i = 0; i < 256; i++) {
        color_array[i].peRed = *entry;
        color_array[i].peGreen = *(entry + 1);
        color_array[i].peBlue = *(entry + 2);
        color_array[i].peFlags = 0;
        entry += 3;
    }
    hr = g_pal->SetEntries(0, 0, 256, color_array);
    lastl = copy();
    if (Windowed)
        for (i = 0; i < 256; i++) {
            int t = *((int*) &color_array[i]);
            int c = (t & 0x0000FF00) | ((t & 0xFF) << 16) | ((t & 0xFF0000) >> 16);
            *((int*) &color_array[i]) = c;
        }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void palette::load_nice()
{ 
    load(); 
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void image::make_page(short width, short height, unsigned char *page_buffer)
{
    if (page_buffer)
        data = page_buffer;
    else 
        data = (unsigned char*) jmalloc(width * height, "image::data");
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void image::delete_page()
{
    if (!special || !special->static_mem)
        jfree(data);
}