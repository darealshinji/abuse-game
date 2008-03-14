#include "scroller.hpp"
#include "Include/24bit.h"
#include "Include/area.h"
#include "Include/id.h"

int sat_r=100,sat_g=100,sat_b=100;

class num_picker : public spicker 
{
    int f,l;
public :
    num_picker (int X, int Y, int ID, int start, int first, int last, ifield *Next) :
       spicker(X,Y,ID,1,10,0,0,Next)
       {
           f=first;
           l=last;
           reconfigure();
           sx=start;
       }
       virtual int total() { return l-f+1; }
       virtual void draw_item(image *screen, int x, int y, int num, int active);
       int dig_len(int x) { int y=1; while (x) { x/=10; y++; } return y; }
       virtual int item_width() { return wm->font()->width()*(dig_len(l)+1); }
       virtual int item_height() { return wm->font()->height(); }
       virtual void note_new_current(image *screen, input_manager *inm, int x)
       { wm->push_event(new event(id,(char *)x));}
};

void num_picker::draw_item(image *screen, int x, int y, int num, int active)
{
    char name[10];
    sprintf(name,"%d",num+f);
    int w=item_width(),h=item_height();
    screen->bar(x,y,x+w-1,y+h-1,wm->black());
    wm->font()->put_string(screen,x,y,name,wm->medium_color());
}

static jwindow *sat_win=NULL;
extern window_manager *eh;
static rgb_lighting *lighting;

void create_sat_window(select_line *mask, image *im, palette *pal)
{
    int xo=WINDOW_FRAME_LEFT,
        yo=WINDOW_FRAME_TOP,
        ya=eh->font()->height()*2+5;
    
    sat_win=eh->new_window(0,-1,-1,-1,
        new num_picker(xo,yo,SAT_R,sat_r,0,200,
        new num_picker(xo,yo+ya,SAT_G,sat_g,0,200,
        new num_picker(xo,yo+ya+ya,SAT_B,sat_b,0,200,NULL))));
    eh->grab_focus(sat_win);
    lighting=new rgb_lighting(mask,im,pal,sat_r,sat_g,sat_b);  
}



int sat_handle_event(event &ev)
{
    if (!sat_win) return 0;
    if (ev.type==EV_CLOSE_WINDOW)
    {
        eh->close_window(sat_win);
        sat_win=NULL;
        delete lighting;
    }  
    return 1;
}


void sat_render(image *im, color_filter *f)
{
    if (sat_win)
    {
        rgb_lighting *nl=(rgb_lighting *)lighting->copy();
        nl->r=sat_r;
        nl->g=sat_g;
        nl->b=sat_b;
        nl->apply();
        nl->put(im,f);
        delete nl;
    }
}