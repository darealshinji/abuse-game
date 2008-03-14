#ifndef VIS_OBJECT_HPP
#define VIS_OBJECT_HPP

#include "jwindow.hpp"
#include "filter.hpp"

class visual_object {
public:
    virtual void draw(image *screen, int x, int y, filter *f) = 0;
    virtual int width() = 0;
    virtual int height() = 0;
};

class image_visual : public visual_object {
public:
    image *im;
    
    image_visual(image *img) { im=img; }
    virtual void draw(image *screen, int x, int y, filter *f);
    virtual int width() { return im->width(); }
    virtual int height() { return im->height(); }
};

class string_visual : public visual_object {
    char *st;
    int color;
    int w,h;
public :
    string_visual(char *string, int Color);
    virtual void draw(image *screen, int x, int y, filter *f);
    virtual int width(); 
    virtual int height();
};


#endif