#ifndef __24BIT_HPP_
#define __24BIT_HPP_
#include "filter.hpp"
#include "macs.hpp"
#include "Include/area.h"


class rgb_scan
{
public :
    rgb_scan *next;
    int x1,x2;
    uchar *data;
    rgb_scan(image *im, palette *pal, int y, int x1, int y2, rgb_scan *next);
    void render(uchar *sl, color_filter *f);
    rgb_scan(rgb_scan *copy);
    rgb_scan *copy() { return new rgb_scan(this); }
    ~rgb_scan() { jfree(data); }
} ;

class rgb_area
{
public :
    rgb_scan **y;
    int y1,y2;
    void make_lines(select_line *first, image *im, palette *pal);
    rgb_area(select_line *first, image *im, palette *pal) { make_lines(first,im,pal); }
    rgb_area(rgb_area *copy);
    void put(image *im, color_filter *f);
    virtual void apply_scan(rgb_scan *rs) { ; }
    void apply();
    virtual rgb_area *copy() { return new rgb_area(this); }
    virtual ~rgb_area();
} ;

class rgb_lighting : public rgb_area
{
public :
    int r,g,b;
    rgb_lighting(rgb_lighting *copy) :
    rgb_area((rgb_area *)copy)
    {
        r=copy->r;
        g=copy->g;
        b=copy->b;
    }
    rgb_lighting(select_line *first, image *im, palette *pal, int R, int G, int B) :  
    rgb_area(first,im,pal)
    { make_lines(first,im,pal); 
    r=R; if (r<0) r=0; else if (r>200) r=200; 
    g=G; if (g<0) g=0; else if (g>200) g=200; 
    b=B; if (b<0) b=0; else if (b>200) b=200; 
    }
    virtual void apply_scan(rgb_scan *rs);
    virtual rgb_area *copy() { return new rgb_lighting(this); }
} ;

class rgb_gravel : public rgb_area
{
public :
    int dif;
    rgb_gravel(rgb_gravel *copy) :
    rgb_area((rgb_area *)copy)
    {
        dif=copy->dif;
    }
    rgb_gravel(select_line *first, image *im, palette *pal, int Diff) :  
    rgb_area(first,im,pal)
    { make_lines(first,im,pal); 
    dif=Diff;
    }
    virtual void apply_scan(rgb_scan *rs);
    virtual rgb_area *copy() { return new rgb_gravel(this); }
} ;

class rgb_invert_cl : public rgb_area
{
public :
    int r,g,b;
    rgb_invert_cl(rgb_invert_cl *copy) : rgb_area((rgb_area *)copy) {  }
    
    rgb_invert_cl(select_line *first, image *im, palette *pal, int R, int G, int B) :  
    rgb_area(first,im,pal)
    { make_lines(first,im,pal); 
    r=R; g=G; b=B;
    }
    virtual void apply_scan(rgb_scan *rs);
    virtual rgb_area *copy() { return new rgb_invert_cl(this); }
} ;

void light(select_line *s, image *im, palette *pal, color_filter *f,char *st);


#endif