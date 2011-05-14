/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __JWIN__
#define __JWIN__

#include "video.h"
#include "image.h"
#include "event.h"
#include "filter.h"
#include "fonts.h"

class ifield;
class WindowManager;
class Jwindow;

extern int frame_top();
extern int frame_bottom();
extern int frame_left();
extern int frame_right();

void set_frame_size(int x);

class InputManager
{
    friend class Jwindow;

public:
    InputManager(image *screen, ifield *first);
    InputManager(Jwindow *owner, ifield *first);
    ~InputManager();

    void handle_event(Event &ev, Jwindow *j);
    ifield *get(int id);
    void redraw();
    void add(ifield *i);
    void remap(Filter *f);
    ifield *unlink(int id); // unlink ID from list and return field pointer
    void clear_current();
    void grab_focus(ifield *i);
    void release_focus();
    void allow_no_selections();

private:
    image *m_surf;
    ifield *m_first, *m_active, *m_grab;
    Jwindow *m_cur, *m_owner;
    int no_selections_allowed;
};

class ifield
{
    friend class Jwindow;
    friend class InputManager;

protected:
    Jwindow *owner;

public :
    ifield();
    int x, y;

    int id;
    ifield *next;
    virtual void set_owner(Jwindow *owner);
    virtual void move(int newx, int newy) { x = newx; y = newy; }
    virtual void area(int &x1, int &y1, int &x2, int &y2) = 0;
    virtual void draw_first(image *screen) = 0;
    virtual void draw(int active, image *screen) = 0;
    virtual void handle_event(Event &ev, image *screen, InputManager *im) = 0;
    virtual int selectable() { return 1; }
    virtual void remap(Filter *f) { ; }
    virtual char *read() = 0;
    virtual ifield *find(int search_id) { if (id==search_id) return this; else return NULL; }
    virtual ifield *unlink(int id) { return NULL; }
    virtual ~ifield();
} ;

class Jwindow
{
    friend class InputManager;

public:
    Jwindow *next;
    int x, y, l, h, backg;
    InputManager *inm;
    void *local_info;  // pointer to info block for local system (may support windows)

    Jwindow(char const *name = NULL);
    Jwindow(int X, int Y, int L, int H, ifield *f, char const *name = NULL);
    ~Jwindow();

    virtual void redraw();
    void resize(int L, int H);
    void clear(int color = 0) { m_surf->bar(x1(), y1(), x2(), y2(), color); }
    void show() { _hidden = false; }
    void hide() { _hidden = true; }
    bool is_hidden() { return _hidden; }
    void freeze() { _moveable = false; }
    void thaw() { _moveable = true; }
    bool is_moveable() { return _moveable; }
    int x1() { return _x1; }
    int y1() { return _y1; }
    int x2() { return _x2; }
    int y2() { return _y2; }
    void clip_in() { m_surf->SetClip(x1(), y1(), x2() + 1, y2() + 1); }
    void clip_out() { m_surf->SetClip(0, 0, l, h); }
    char *read(int id) { return inm->get(id)->read(); }
    void local_close();

    static int left_border();
    static int right_border();
    static int top_border();
    static int bottom_border();

    image *m_surf;

protected:
    Jwindow *owner;
    int _x1, _y1, _x2, _y2;

private:
    char *_name;
    bool _hidden;
    bool _moveable;

    void reconfigure();
};

class WindowManager : public EventHandler
{
    friend class Jwindow;

protected:
    void add_window(Jwindow *);
    void remove_window(Jwindow *);

public:
    WindowManager(image *, palette *, int hi, int med, int low, JCFont *);
    ~WindowManager();

    Jwindow *m_first, *m_grab;
    image *mouse_pic, *mouse_save;
    int hi, med, low, bk; // bright, medium, dark and black colors
    int key_state[512];
    enum { inputing, dragging } state;
    int drag_mousex, drag_mousey, frame_suppress;
    Jwindow *drag_window;
    JCFont *fnt, *wframe_fnt;

    Jwindow *new_window(int x, int y, int l, int h,
                        ifield *fields = NULL, char const *Name = NULL);

    void set_frame_font(JCFont *fnt) { wframe_fnt = fnt; }
    JCFont *frame_font() { return wframe_fnt; }
    void close_window(Jwindow *j);
    void resize_window(Jwindow *j, int l, int h);
    void move_window(Jwindow *j, int x, int y);
    void get_event(Event &ev);
    void flush_screen();
    int bright_color() { return hi; }
    int medium_color() { return med; }
    int dark_color() { return low; }
    int black() { return bk; }
    void set_colors(int Hi, int Med, int Low) { hi=Hi; med=Med; low=Low; }
    JCFont *font() { return fnt; }

    int key_pressed(int x) { return key_state[x]; }
    void hide_windows();
    void show_windows();
    void hide_window(Jwindow *j);
    void show_window(Jwindow *j);
    void set_frame_suppress(int x) { frame_suppress=x; }
    void grab_focus(Jwindow *j);
    void release_focus();
    int window_in_area(int x1, int y1, int x2, int y2); // true if a window lies in this area

private:
    palette *m_pal;
    image *m_surf;
};

#endif


