/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __INPUT_HPP_
#define __INPUT_HPP_

#include "window.h"
#include "filter.h"

extern WindowManager *wm; /* FIXME: get rid of this if possible */

class AButton : public AWidget
{
public:
    AButton(ivec2 pos, int ID, char const *Text);
    AButton(ivec2 pos, int ID, AImage *vis);
    AButton(ivec2 pos, int ID, AImage *Depressed, AImage *Pressed, AImage *active);
    virtual ~AButton() { }

    virtual ibox2 GetArea();
    virtual void draw_first(AImage *screen);
    virtual void Draw(int active, AImage *screen);
    virtual void handle_event(Event &ev, AImage *screen, InputManager *im);
    void change_visual(AImage *new_visual);
    virtual void remap(Filter *f);
    void push();
    virtual char const *read() { return (char const *)&up; }
    int status() { return up; }
    void set_act_id(int id) { act_id=id; }

private:
    int up, act;
    String m_text;
    AImage *visual, *pressed, *act_pict;
    int act_id;
};

class AButtonBox : public AWidget
{
public:
    AButtonBox(ivec2 pos, int ID, int MaxDown, Array<AButton *> const &buttons = Array<AButton *>());
    virtual ~AButtonBox();

    void add_button(AButton *b);
    void press_button(int id);      // if button box doesn't contain id, nothing happens
    virtual void remap(Filter *f);
    virtual void Move(ivec2 pos);
    virtual ibox2 GetArea();
    virtual void draw_first(AImage *screen);
    virtual void Draw(int active, AImage *screen);
    virtual void handle_event(Event &ev, AImage *screen, InputManager *im);
    virtual char const *read();   // return pointer to first button which is depressed
    virtual AWidget *find(int search_id);  // should return pointer to item you control with this id
    void arrange_left_right();
    void arrange_up_down();

private:
    Array<AButton *> m_buttons;
    int maxdown;
};

class ATextField : public AWidget
{
public:
    ATextField(ivec2 pos, int ID, char const *Prompt, char const *Format, char const *Data);
    ATextField(ivec2 pos, int ID, char const *Prompt, char const *Format, double Data);
    virtual ~ATextField() { free(prompt); free(format); free(data); }

    virtual ibox2 GetArea();
    virtual void draw_first(AImage *screen);
    virtual void Draw(int active, AImage *screen);
    virtual void handle_event(Event &ev, AImage *screen, InputManager *im);

    virtual char const *read();
    void change_data(char const *new_data, int new_cursor,       // cursor==-1, does not change it.
                     int active, AImage *screen);

private:
    int cur;
    char *prompt, *data, *format;
    int xstart() { return m_pos.x + wm->font()->Size().x * (strlen(prompt)+1) + 3; }
    int xend() { return m_pos.x + wm->font()->Size().x * (strlen(prompt) + 1 + strlen(format)) + 7; }
    int yend() { return m_pos.y + wm->font()->Size().y + 5; }
    void draw_cur(int color, AImage *screen);
    int last_spot() { int x=strlen(data); while (x && data[x-1]==' ') x--; return x; }
    void draw_text(AImage *screen)
    {
        screen->Bar(ivec2(xstart() + 1, m_pos.y + 1), ivec2(xend() - 1, yend() - 1),
                    wm->dark_color());
        wm->font()->PutString(screen, ivec2(xstart() + 1, m_pos.y + 3), data);
    }
};


class AInfoField : public AWidget
{
public:
    AInfoField(ivec2 pos, int ID, char const *info);
    virtual ~AInfoField() { }

    virtual ibox2 GetArea();
    virtual void draw_first(AImage *screen);
    virtual void Draw(int active, AImage *screen) { UNUSED(active, screen); }
    virtual void handle_event(Event &ev, AImage *screen, InputManager *im) { UNUSED(ev, screen, im); }
    virtual char const *read() { return m_text.C(); }
    virtual int selectable() { return 0; }

private:
    String m_text;
    int w, h;
    void put_para(AImage *screen, char const *st, int dx, int dy,
                  int xspace, int yspace, JCFont *font, int color);
};

#endif

