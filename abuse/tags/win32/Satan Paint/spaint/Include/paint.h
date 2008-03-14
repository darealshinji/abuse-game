#ifndef __PAINT_HPP_
#define __PAINT_HPP_
#include <math.h>
#include <ctype.h>
#include "image.hpp"
#include "event.hpp"
#include "palette.hpp"
#include "fonts.hpp"
#include "filter.hpp"
#include "loader2.hpp"
#include "Include/items.h"
#include "jwindow.hpp"
#include "Include/pal.h"
#include "points.hpp"
#include "Include/undo.h"
#include "timing.hpp"
#include "Include/area.h"

#include "pmenu.hpp"
#define MODE_MODIFY   1 
#define MODE_SHOW     2
#define MODE_CLEAR    3
#define HISTORY_SIZE  20



#define SHOW_PROP 1

class painter;
class editor;

extern unsigned char fnt6x13[192*104];
extern unsigned char fnt5x7[160*56];

extern window_manager *eh;
extern paint_pal *palc;
extern JCFont *font;     // current font used by application
extern painter *paint;

int select_points(int message_id);

class keyed_command : public linked_node {
public:
    int key;
    char *com,*help;
    keyed_command(int Key, char *Command, char *Help)
    { key=Key; com=strcpy((char *)jmalloc(strlen(Command)+1,"key command"),Command);
    help=strcpy((char *)jmalloc(strlen(Help)+1,"key help"),Help); }
    char *command() { return com; }
    ~keyed_command() { jfree(com); jfree(help); } 
};

class command_manager {
    linked_list commands;
public:
    keyed_command *find_key(int key) {
        keyed_command *found=NULL,*p;       
        loopct(keyed_command,p,commands.first(),!found, 
            if (key==p->key) found=p; );
            return found;
    }          
    
    void add_key(int Key, char *Command, char *Help) {
        keyed_command *p=find_key(Key);
        if (p) {
            commands.unlink((linked_node *)p);
            delete p;
        }
        commands.add_end((linked_node *) new keyed_command( Key, Command, Help));
    }
};

class Index {
public:
    Index *next;
    int image_num,x1,y1,x2,y2;
    int in_area(int x,int y) {
        return (x >= x1 && x <= x2 && y >= y1 && y <= y2);
    }
    Index(int num, int X1,int Y1,int X2, int Y2) {
        image_num = num;
        x1 = X1;
        y1 = Y1;
        x2 = X2;
        y2 = Y2;
    }
};

struct view_area {
    short x1, y1, x2, y2;
};

class painter {
public:
    view_area im_area,inf_area,pix_dirty;
    palette *pal;
    char marked[300];
    char *command_hist[HISTORY_SIZE];
    int tot_hist,hist_on;
    
    imitem **editim;
    image **tools,    // a list of bitmaps showing tools on left side
        *select_buffer;
    struct {
        int image_number;
        int x1, y1, x2, y2;    
    } last_select;
    
    struct {
        int x1, y1, x2, y2;
    } last_animate; 
    
    linked_list undos;
    int undo_levels,quit_signal,state_enter_button_status;
    
    jwindow *title_window,*new_window,*points_window,*animate_pick_window,
        *animate_control_window;
    color_filter *col_filt;
    editor *edit; 
    char fname[100];
    char command[200];
    
    unsigned char point_data[4][514];
    int total_points[4],current_points,lastpx,lastpy,last_button;
    command_manager key_list;
    
    Index *indexes,*last_index;
    int max_ani,*ani_order,no_draw_mode,ani_xoff,next_off;
    short zoomx,zoomy,zoom, // current color, zoom position and scale
        ntools, // number of tools loaded from the file
        bright_color,num_images,rcommand,brush_size,next_state,
        selx1,sely1,selx2,sely2,sel_but,state;
    int cur_image;
    short ani_frame,ani_dir;
    long ani_sleep;
    
    /*************************** Selection stuff **************************/
    time_marker last_sel_draw_time;
    ushort *area_sel_list;
    select_line *sel_mask;
    int area_sel_total,last_sel_draw_offset;
    void reset_selection_list();
    void draw_sel_list(int offset);
    void get_last_sel_point(short &lx,short &ly);
    void add_sel_point(short fx,short fy);
    void stagger_line(int x1, int y1, int x2, int y2, int c1, int c2, int start_c1);
    int draw_selection_state(int state);
    void redraw_sel_area();
    
    
    int index_xoffset,index_yoffset,index_flags,last_on;
    int get_next_mark(char *mark, int last_mark=-1, int last_image=-1); 
    // -1 gets the first mark, ret of -1 = no marks
    
    void calc_areas();
    void clear_animate_frames();
    void add_animate_frame(int frame);
    void print_animation_sequence(image *screen, int x, int y);
    
    void title_screen();
    int closest_color(int r, int g, int b, palette *pal);
    int current_color() { if (palc) return palc->current_color(); else return 0; }
    void info_area(short &x1, short &y1, short &x2, short &y2); 
    // returns the area where image is drawn
    void image_area(short &x1, short &y1, short &x2, short &y2);  
    // draws a specified image in the image location of the screen
    void draw_image(image *im, short erase=1);  
    void draw_area(image *im, int x1, int y1, int x2, int y2, int color=-1);
    void draw_image_area(image *main, image *sub, 
        int x, int y, int x1, int y1, int x2, int y2, int transparent);
    void draw_tools();
    short  colors_in_row() { if (xres<639) return 52; else return 53; }
    void mouse_click(event &ev);  // called if the mouse is clicked
    void next_image();
    void last_image();
    void change_zoom(char *st, event &ev);
    void step();
    void handle_event(event &ev);
    void mouse_image_position(short mousex, short mousey, short &x, short &y);
    void pixel_area(short x, short y, short &x1, short &y1, short &x2, short &y2);
    void set_current_color(short color);
    void info(char *st, char *st2=NULL, int cursor=-1);
    imitem **load(char *filename, short &total, palette *&pal);
    void load_images(char *filename);
    void pan(short key);
    void delete_image(char *mark);
    void copy_image();
    void do_command(char *st, event &ev);
    void show_current();      // display the current image, totals and such
    void fill_image(short x, short y);
    void frame(short x1, short y1, short x2, short y2, int );
    void paste(event ev, int put_mode);
    void change_palette(palette *new_pal);
    void draw_image_pixel(int x, int y, int c);
    void line(int x1, int y1, int x2, int y2, int color, int mode); 
    void circle(int x, int y, int r, int color, int mode);
    void blur_area(int x1, int y1, int x2, int y2, 
        int blur_amount=1, int blur_color=-1);
    void anti(int x1, int y1, int x2, int y2);
    void draw();
    void save_stuff();
    void remove_singles(image *im);
    void draw_indexes(int xoffset, int yoffest);
    void set_current(int x);
    image *current_image() { if (num_images) 
        return editim[cur_image]->im; 
    else printf("arg\n"); return NULL; }
    image *get_image(int x) { return editim[x]->im; }
    imitem *get_item(int x) { return editim[x]; }
    char *current_name() { return editim[cur_image]->name(); }
    imitem *current_item() { return editim[cur_image]; }
    imitem **add_imitem();
    void insert_imitem(int insert_point, imitem *im);
    void rotate90();
    painter(char *filename);
    
    void change_cursor();
    void read(char *filename);
    void setkey(char *st);
    void crop();
    void crop_center(char *command);
    void renumber (char *command);
    void rm_dups();
    void add_undo(int image_number, int action, palette *new_pal=NULL);
    void set_undo(int levels);                         //  0=off
    void undo();
    void remap(char *st);
    void set(char *st);
    void reverse(char *st);
    void load_sequence(char *filebase, int set_palette, int still);
    void write_property(char *filename, char *range);
    void load_property(char *filename, char *range);
    void clear_unused();
    void grow(int xa, int ya);
    int merge(char *filename, int start=-1, int end=-1);
    int get_number(char *&st);
    void morph(char *st);
    void blend(char *st);
    void bevel(char *st);
    void copy_property(char *st);
    void copy_to(int num);
    void add_history(char *command);
    void sample_scale(int width, int height);
    void set_font(JCFont *new_font);
    void flush_screen();
    void mouse_change_state(int new_state);
    void repeat(char *st);
    void write_color(char *filename);
    void write_gray(char *filename);
    void merge_gray(char *st);
    void invert(char *st);
    void load_tpal(char *st);
    void write_tpal(char *st);
    
    void change_state(int new_state);   // does any action nessary to switch to new state
    void quant8(int x, int y,           // place to put onto the destination image
        int w, int h,           // width & hieght of 24 bit buffer
        unsigned char *buffer,  // 24 bit buffer to place onto 8 bit image
        image *dest);           // destination image to place quant buffer
};


#endif