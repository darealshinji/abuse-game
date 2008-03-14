#ifndef __JWIN__
#define __JWIN__
#include "video.hpp"
#include "image.hpp"
#include "event.hpp"
#include "filter.hpp"
#include "event.hpp"
#include "fonts.hpp"

class ifield;
class window_manager;
class jwindow;

int frame_top();
int frame_bottom();
int frame_left();
int frame_right();

void set_frame_size(int x);

#define WINDOW_FRAME_TOP    0
#define WINDOW_FRAME_BOTTOM 0
#define WINDOW_FRAME_LEFT   0
#define WINDOW_FRAME_RIGHT  0

#define DRAW_FLAG_ACTIVE 1
#define DRAW_FLAG_HOVER  2

#define JWINDOW_TRANSPARENT_FLAG 0x00000001
#define JWINDOW_MOVEABLE_FLAG    0x00000002
#define JWINDOW_NOAUTOHIDE_FLAG  0x00000004
#define JWINDOW_IGNOREMOUSE_FLAG 0x00000008
extern window_manager *wm;

typedef int* (*JWINDOW_EVENT_FUNC)(event *ev);

// --------------------------------------------------------------------------- 
class input_manager {
friend jwindow;
protected:
    image *screen;
    ifield *first, *active, *grab, *LastOver;
    jwindow *cur;
    jwindow *owner;
    int no_selections_allowed;
public:
    input_manager(jwindow *Owner, ifield *First);
    input_manager(image *Screen, ifield *First);
    void handle_event(event &ev, jwindow *j);
    ifield *get(int id);
    void redraw();
    void add(ifield *i);
    void remap(filter *f);
    ifield *unlink(int id);     // unlinks ID from fields list and return the pointer to it
    ifield *current() { return active; }
    ifield *SelectNextField(int reverse = 0);
    void clear_current();
    void grab_focus(ifield *i);
    void release_focus();
    void allow_no_selections();
    ~input_manager();
};

// --------------------------------------------------------------------------- 
class ifield {
friend jwindow;
friend input_manager;
protected:
    jwindow *owner;
public:
    int x, y;
    int id;
    ifield *next;
    ifield();
    virtual void area(int &x1, int &y1, int &x2, int &y2) = 0;
    virtual void draw_first(image *screen)                = 0;
    virtual void draw(int active, image *screen)          = 0;
    virtual void handle_event(event &ev, image *screen, input_manager *im) = 0;
    virtual int selectable() { return 1; }
    virtual void remap(filter *f) { ; }
    virtual char *read() = 0;
    virtual ifield *find(int search_id) {
        if (id == search_id)
            return this;
        else if (next)
            return next->find(search_id);
        else
            return NULL; }
    virtual ifield *unlink(int id) { return NULL; } 
    virtual ~ifield();
    virtual void SetOwner(jwindow *Owner);
    virtual void Move(int NewX, int NewY) {
        x = NewX; y = NewY;
    }
};

// --------------------------------------------------------------------------- 
struct jwindow_properties
{
    int moveable,
        hidden,
        flags;
};

// --------------------------------------------------------------------------- 
class jwindow {
friend input_manager;
protected:
    jwindow_properties property;
    jwindow       *owner;
    int           ClientX1, ClientX2, ClientY1, ClientY2;
    JWINDOW_EVENT_FUNC OnEvent;
public:
    jwindow       *next;
    char          *name;
    int           x, y, l, h, backg;
    image         *screen;
    input_manager *inm;
    void          *local_info;     // pointer to info block for local system (may support windows)
    __inline static int LeftBorder() { return frame_left(); }
    __inline static int RightBorder() { return frame_right(); }
    __inline static int TopBorder();
    __inline static int BottomBorder() { return frame_bottom(); }
    __inline void show() { property.hidden = 0; }
    __inline void hide() { property.hidden = 1; }
    __inline void toggle() { property.hidden = !property.hidden; }
    __inline int  hidden() { return property.hidden; }
    __inline int  flags() { return property.flags; }
    __inline int  moveable() { return property.moveable; }

    jwindow(int X, int Y, int L, int H, ifield *fields, char *Name=NULL);
    jwindow();
    ~jwindow();
    virtual void  Reconfigure();
    virtual void  redraw();
    void          resize(int L, int H);
    void          clear(int color = 0) { screen->bar(x1(),y1(),x2(),y2(),color); }
    __inline int  x1() { return ClientX1; }
    __inline int  y1() { return ClientY1; } 
    __inline int  x2() { return ClientX2; }
    __inline int  y2() { return ClientY2; } 
    void          clip_in() { screen->set_clip(x1(),y1(),x2(),y2()); }
    void          clip_out() { screen->set_clip(0,0,l-1,h-1); }
    char          *read(int id) { return inm->get(id)->read(); }  
    void          local_close();
    void          set_moveability(int x);
    void          SetBorder(int x);
    virtual int   handle_event(event &ev);
};

// --------------------------------------------------------------------------- 
class window_manager {
protected:
    void AddWindow(jwindow *NewWin);
    void DeleteWindow(jwindow *Which);
public:
friend jwindow;
    event_handler *eh;
    jwindow       *first, *grab;
    image         *screen, *mouse_pic, *mouse_save;
    palette       *pal;
    int           hi, med, low, bk;                     // bright, medium, dark and black colors
    int           key_state[512];
    enum { inputing, dragging } state;
    int           drag_mousex, drag_mousey, frame_suppress;
    jwindow       *drag_window;
    JCFont        *fnt, *wframe_fnt;
    
    window_manager(image *Screen, palette *Pal, int Hi, int Med, int Low, JCFont *Font); 
    ~window_manager();
    
    jwindow       *new_window(int x, int y, int l, int h, ifield *fields=NULL, char *Name=NULL);
    void          set_frame_font(JCFont *fnt) { wframe_fnt=fnt; }
    JCFont        *frame_font() { return wframe_fnt; }
    void          close_window(jwindow *j);
    void          resize_window(jwindow *j, int l, int h);
    void          move_window(jwindow *j, int x, int y);
    void          get_event(event &ev);
    void          push_event(event *ev) { eh->push_event(ev); }
    int           event_waiting() { return eh->event_waiting(); }
    void          flush_screen();
    int           bright_color() { return hi; }
    int           medium_color() { return med; }
    int           dark_color() { return low; }
    int           black() { return bk; }
    void          set_colors(int Hi, int Med, int Low) { hi=Hi; med=Med; low=Low; }
    JCFont        *font() { return fnt; }
    int           has_mouse() { return eh->has_mouse(); }
    void          mouse_status(int &x, int &y, int &button) {eh->mouse_status(x,y,button); }	       
    void          set_mouse_shape(image *im, int centerx, int centery) {
                      eh->set_mouse_shape(im, centerx, centery);
                  }
    void          set_mouse_position(int mx, int my) {
                      eh->set_mouse_position(mx,my);
                  }
    int           key_pressed(int x) {
                      return key_state[x];
                  }
    void          hide_windows();
    void          show_windows();
    void          hide_window(jwindow *j);
    void          show_window(jwindow *j);
    void          set_frame_suppress(int x) {
                      frame_suppress = x;
                  }
    void          grab_focus(jwindow *j);
    void          release_focus();
    int           window_in_area(int x1, int y1, int x2, int y2); // true if a window lies in this area
};

// --------------------------------------------------------------------------- 
// --------------------------------------------------------------------------- 

__inline int jwindow::TopBorder() {
    return wm->font()->height() + frame_top();
}

#endif