/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef _SCROLLER_HPP_
#define _SCROLLER_HPP_

#include "imlib/input.h"

class AScroller : public AWidget
{
public:
    AScroller(ivec2 pos, int id, ivec2 size, int Vert, int Total_items);

    void SetSize(ivec2 size) { m_size = size; }

    int t, sx;
    virtual ibox2 GetArea();
    virtual void draw_first(AImage *screen);
    virtual void Draw(int active, AImage *screen);
    virtual void handle_event(Event &ev, AImage *screen, InputManager *im);
    virtual char const *read() { return (char const *)&sx; }

    virtual int activate_on_mouse_move() { return 1; }
    virtual void handle_inside_event(Event &ev, AImage *screen, InputManager *inm) { UNUSED(ev, screen, inm); }
    virtual void scroll_event(int newx, AImage *screen);
    virtual void handle_up(AImage *screen, InputManager *inm);
    virtual void handle_down(AImage *screen, InputManager *inm);
    virtual void handle_left(AImage *screen, InputManager *inm);
    virtual void handle_right(AImage *screen, InputManager *inm);
    virtual void area_config() { ; }
    virtual void set_x(int x, AImage *screen);

protected:
    ivec2 m_size;
    int drag, vert, last_click;

    int bh() const;
    int bw() const;
    void drag_area(int &x1, int &y1, int &x2, int &y2);
    void dragger_area(int &x1, int &y1, int &x2, int &y2);
    ivec2 b1() { return m_pos + (vert ? ivec2(m_size.x, 0) : ivec2(0, m_size.y)); }
    ivec2 b2() { return m_pos + m_size - (vert ? ivec2(0, bw()) : ivec2(bh(), 0)); }
    uint8_t const *GetIcon(int index);
    void wig_area(int &x1, int &y1, int &x2, int &y2);

    int wig_x();
    int darea() { return (m_size.x - bw() - 2) - bw() - bw(); }
    void draw_widget(AImage *screen, int erase);
    int mouse_to_drag(ivec2 pos);
};

class AScrollPicker : public AScroller
{
public:
    AScrollPicker(ivec2 pos, int id, int rows, int cols, int Vert, int MultiSelect);
    ~AScrollPicker() { free(select); }

    int vis() { return (vert ? m_size.y : m_size.x); }
    virtual void area_config();
    void set_select(int x, int on);
    int get_select(int x);
    int first_selected();
    virtual void scroll_event(int newx, AImage *screen);
    virtual void handle_inside_event(Event &ev, AImage *screen, InputManager *inm);

    // you should define \/
    virtual void DrawItem(AImage *screen, ivec2 pos, int num, int active) = 0;
    virtual int total() = 0;
    virtual int item_width() = 0;
    virtual int item_height() = 0;

    virtual void draw_background(AImage *screen);
    virtual void note_selection(AImage *screen, InputManager *inm, int x) { UNUSED(screen, inm, x); }
    virtual void note_new_current(AImage *screen, InputManager *inm, int x) { UNUSED(screen, inm, x); }
    virtual int ok_to_select(int num) { UNUSED(num); return 1; }
    virtual void handle_up(AImage *screen, InputManager *inm);
    virtual void handle_down(AImage *screen, InputManager *inm);
    virtual void handle_left(AImage *screen, InputManager *inm);
    virtual void handle_right(AImage *screen, InputManager *inm);
    virtual void set_x(int x, AImage *screen);
    void reconfigure();   // should be called by constructor after class is ready to take virtual calls

protected:
    int m_rows, m_cols;
    int m, last_sel, cur_sel;
    uint8_t *select;
};

struct APickListItem
{
    char *name;
    int number;
};

class APickList : public AScroller
{
public:
    APickList(ivec2 pos, int id, int height, char **List, int num_entries,
              int start_yoffset, AImage *texture = nullptr);
    ~APickList() { free(lis); }

    virtual void handle_inside_event(Event &ev, AImage *screen, InputManager *inm);
    virtual void scroll_event(int newx, AImage *screen);
    virtual char const *read() { return (char const *)this; }
    virtual void area_config();
    virtual void handle_up(AImage *screen, InputManager *inm);
    virtual void handle_down(AImage *screen, InputManager *inm);
    int get_selection() { return lis[cur_sel].number; }

private:
    int last_sel, cur_sel, th, wid;
    APickListItem *lis;
    char key_hist[20], key_hist_total;
    AImage *tex;
};

#endif





