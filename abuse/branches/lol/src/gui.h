/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __GUI_HPP_
#define __GUI_HPP_

#include "window.h"

class AIconButton : public AWidget
{
public:
    AIconButton(ivec2 pos, int id, int Up, int down, int upa, int downa, int act_id = -1, char const *help_key = nullptr);

    virtual void area(int &x1, int &y1, int &x2, int &y2);
    virtual void draw_first(AImage *screen) { Draw(0, screen); }
    virtual void Draw(int active, AImage *screen);
    virtual void handle_event(Event &ev, AImage *screen, InputManager *im);

    virtual char *read() { return (char *)&up; }
    int status() { return up; }
    void set_act_id(int id);

private:
    int up, act, u, d, ua, da;  // up, down, up active, down active
    int activate_id;            // sent when if not -1 when object receives a draw active
    char key[16];
};

class AIconSwitchButton : public AWidget
{
public:
    AIconSwitchButton(ivec2 pos, int id, int start_on, AWidgetList const &buttons = AWidgetList());
    ~AIconSwitchButton();

    virtual void area(int &x1, int &y1, int &x2, int &y2);
    virtual void draw_first(AImage *screen) { m_buttons[m_current]->draw_first(screen); }
    virtual void Draw(int active, AImage *screen) { m_buttons[m_current]->Draw(active, screen); act = active; }
    virtual void handle_event(Event &ev, AImage *screen, InputManager *im);
    virtual AWidget *unlink(int id);
    virtual char *read() { return m_buttons[m_current]->read(); }

private:
    AWidgetList m_buttons;
    int m_current, act;
};

#endif


