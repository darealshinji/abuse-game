/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __JWIN__
#define __JWIN__

#include <stdarg.h>

#include "imlib/video.h"
#include "imlib/image.h"
#include "imlib/event.h"
#include "imlib/filter.h"
#include "imlib/fonts.h"

class WindowManager;
class InputManager;
class AWindow;

extern int frame_top();
extern int frame_bottom();
extern int frame_left();
extern int frame_right();

void set_frame_size(int x);

class AWidget
{
    friend class AWindow;
    friend class InputManager;

public:
    AWidget();
    AWidget(ivec2 pos, int id);
    virtual ~AWidget();

    virtual void set_owner(AWindow *owner);
    virtual void Move(ivec2 pos) { m_pos = pos; }
    virtual ibox2 GetArea() = 0;
    virtual void draw_first(AImage *screen) = 0;
    virtual void Draw(int active, AImage *screen) = 0;
    virtual void handle_event(Event &ev, AImage *screen, InputManager *im) = 0;
    virtual int selectable() { return 1; }
    virtual void remap(Filter *f) { (void)f; }
    virtual char const *read() = 0;
    virtual AWidget *find(int search_id) { if (m_id == search_id) return this; else return nullptr; }
    virtual AWidget *unlink(int id) { (void)id; return nullptr; }

    ivec2 m_pos;
    int m_id;

protected:
    AWindow *m_owner;
};

class AWidgetList : public array<AWidget *>
{
};

class InputManager
{
    friend class AWindow;

public:
    InputManager(AImage *screen, AWidgetList const &fields = AWidgetList());
    InputManager(AWindow *owner, AWidgetList const &fields = AWidgetList());
    ~InputManager();

    void handle_event(Event &ev, AWindow *j);
    AWidget *get(int id);
    void redraw();
    void Add(AWidgetList const &fields);
    void remap(Filter *f);
    AWidget *unlink(int id); // unlink ID from list and return field pointer
    void clear_current();
    void grab_focus(AWidget *i);
    void release_focus();
    void allow_no_selections();

private:
    AImage *m_surf;
    AWidgetList m_fields;
    int m_active, m_grab;
    AWindow *m_cur, *m_owner;
    int no_selections_allowed;
};

class AWindow
{
    friend class InputManager;

public:
    AWindow *next;
    int backg;
    InputManager *inm;
    void *local_info;  // pointer to info block for local system (may support windows)

    AWindow(String const &name = "");
    AWindow(ivec2 pos, ivec2 size,
            String const &name = "",
            AWidgetList const &widgets = AWidgetList());
    ~AWindow();

    virtual void redraw();
    void Resize(ivec2 size);
    void clear(int color = 0) { m_surf->Bar(ivec2(x1(), y1()),
                                            ivec2(x2(), y2()), color); }
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
    char const *read(int id) { return inm->get(id)->read(); }
    void local_close();

    static int left_border();
    static int right_border();
    static int top_border();
    static int bottom_border();

    ivec2 m_pos, m_size;
    AImage *m_surf;

protected:
    AWindow *m_owner;
    int _x1, _y1, _x2, _y2;

private:
    char *_name;
    bool _hidden;
    bool _moveable;

    void reconfigure();
};

class WindowManager : public EventHandler
{
    friend class AWindow;

protected:
    void add_window(AWindow *);
    void remove_window(AWindow *);

public:
    WindowManager(AImage *, Palette *, int hi, int med, int low, JCFont *);
    ~WindowManager();

    AWindow *m_first, *m_grab;
    AImage *mouse_pic, *mouse_save;
    int hi, med, low, bk; // bright, medium, dark and black colors
    int key_state[512];
    enum { inputing, dragging } state;
    int drag_mousex, drag_mousey, frame_suppress;
    AWindow *drag_window;
    JCFont *fnt, *wframe_fnt;

    AWindow *CreateWindow(ivec2 pos, ivec2 size,
                          String const &name = "",
                          AWidgetList const &widgets = AWidgetList());

    JCFont *frame_font() { return wframe_fnt; }
    void close_window(AWindow *j);
    void resize_window(AWindow *j, int l, int h);
    void move_window(AWindow *j, int x, int y);
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
    void hide_window(AWindow *j);
    void show_window(AWindow *j);
    void grab_focus(AWindow *j);
    void release_focus();

private:
    Palette *m_pal;
    AImage *m_surf;
};

#endif


