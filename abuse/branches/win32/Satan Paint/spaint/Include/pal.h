#ifndef __PAL__
#define __PAL__
#include "jwindow.hpp"
#include "palette.hpp"
#include "video.hpp"
#include "input.hpp"
#include "filter.hpp"
#include "otree.h"

#define PAL_W 10 
#define PAL_H 26 

extern unsigned char default_palette[256*3];
extern char pal_win_on;
class paint_pal
{
    jwindow *pw,*cedit;
    palette *pal;
    window_manager *wm;
    int bs,cur_col,table_recalc,palx,paly;
    color_filter *color_table;
    void create_window();
    void draw_color(int i, int selected);
    void draw_wigets(int clear);
public :
    unsigned char find_rgb(unsigned char r, unsigned char g, unsigned char b) 
    { return pal->find_closest(r,g,b); }
    color_filter *get_filter(int recompute=1);
    palette *current_pal() { return pal; }
    paint_pal(window_manager *WM,  palette *Pal, int block_size);
    int current_color() { return cur_col; }
    int set_color(int color);
    int handle_event(event &ev);
    void show() { if (!pw && !cedit) create_window(); }
    void hide() { if (pw) { palx=pw->x; paly=pw->y; wm->close_window(pw); pw=NULL; } }
    void add_bs(int x);
    void toggle() { if (pw) hide(); else show(); }
    void load_stuff(char *filename);
    void tint_pal(int r, int g, int b, int five);
    void invert_pal(int r, int g, int b);  
    
    void change_color(int x, int r, int g, int b);
    void change_palette(palette *new_pal);
    void add(int x);
    int kill_table() { if (color_table) { delete color_table; table_recalc=1; return 1; } return 0; }
};

#endif