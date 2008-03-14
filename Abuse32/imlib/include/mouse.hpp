#ifndef __JCMOUSE_HPP_INCLUDED_
#define __JCMOUSE_HPP_INCLUDED_
#include "image.hpp"
#include "sprite.hpp"

class JCMouse
{
    int here, but;
    sprite *sp;
    image *screen;
    int lx, ly, lz, lbut, mx, my, mz;
    int cx, cy;                       // center of mouse cursor
public :
    JCMouse(image *Screen, palette *pal);
    void set_shape(image *im, int centerx = 0, int centery = 0);
    void update(int newx = -1, int newy = -1, int new_but = -1);
    void set_shape(image *im) { if (here) sp->change_visual(im); sp->visual->SaveSurface();}
    void get_shape(int &centerx, int &centery);
    
    int x() { if (here) return mx; else return 0; }
    int y() { if (here) return my; else return 0; }
    int z() { if (here) return mz; else return 0; }
    int drawx() { return mx - cx; }
    int drawy() { return my - cy; }
    int lastx() {
        return lx;
    }
    int lasty() { 
        return ly;
    }
    int lastz() {
        return lz;
    }
    int last_button() { 
        return lbut; 
    }
    int button() { return but; }
    int exsist() { return here; }
    sprite *mouse_sprite() { return sp; }
    void set_position(int new_mx, int new_my);
    ~JCMouse();
    
#ifdef __POWERPC__
    int set_button(int b) { return (but = b); }
#endif
};

#endif