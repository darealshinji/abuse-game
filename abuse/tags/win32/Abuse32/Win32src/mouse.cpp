#include "video.hpp"
#include "sprite.hpp"
#include "image.hpp"
#include "filter.hpp"
#include "mouse.hpp"
#include "dprint.hpp"

unsigned char def_mouse[]=
    { 0,2,0,0,0,0,0,0,
      2,1,2,0,0,0,0,0,
      2,1,1,2,0,0,0,0,
      2,1,1,1,2,0,0,0,
      2,1,1,1,1,2,0,0,
      2,1,1,1,1,1,2,0,
      0,2,1,1,2,2,0,0,
      0,0,2,1,1,2,0,0,
      0,0,2,1,1,2,0,0,
      0,0,0,2,2,0,0,0 };    // 8x10

extern int Win_Mouse_Z;
extern int Win_Mouse_X;
extern int Win_Mouse_Y;
extern int Win_Mouse_Buttons;

class mouse_detector
{
public:
    int detected;
    mouse_detector() {
        detected = 1;
    }
} mouse_detect;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void JCMouse::get_shape(int &centerx, int &centery)
{
    centerx = -cx;
    centery = -cy;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void JCMouse::set_shape(image *im, int centerx, int centery)
{
    sp->change_visual(im, 1);
    cx = -centerx;
    cy = -centery;
    sp->visual->SaveSurface();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
JCMouse::JCMouse(image *Screen, palette *pal)
{
    image *im;
    int br, dr;
    filter f;
    but = 0;
    cx = cy = 0;
    sp = NULL;

    here = mouse_detect.detected;

    if (here) {                    // is it here?
        screen = Screen;
        br = pal->brightest(1);
        dr = pal->darkest(1);
        f.set(1, br);
        f.set(2, dr);
        im = new image(8, 10, def_mouse);
        f.apply(im);
        sp = new sprite(Screen, im, 100, 100);
    }
    else 
        dprintf("mouse not detected\n");

    mx = Screen->width() / 2;
    my = Screen->height() / 2;
    mz = lz = Win_Mouse_Z;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void JCMouse::update(int newx, int newy, int new_but)
{
    if (newx < 0) {
        if (here) {
            lx = mx;
            ly = my;
            lz = mz;
            lbut = but;
            mx = Win_Mouse_X;
            my = Win_Mouse_Y;
            mz = Win_Mouse_Z;
            but = Win_Mouse_Buttons;
        }
    }
    else {
        mx = newx; 
        my = newy; 
        but = new_but; 
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
JCMouse::~JCMouse()
{
    if (sp) {
        delete sp;
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void JCMouse::set_position(int new_mx, int new_my)
{
    mx = new_mx;
    my = new_my;
}
