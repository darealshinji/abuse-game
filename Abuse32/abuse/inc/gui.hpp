#ifndef __GUI_HPP_
#define __GUI_HPP_
#include "jwindow.hpp"


class ico_button : public ifield {
    int up, act, u, d, ua, da;  // up, down, up active, down active
    int activate_id;            // if not -1, this is the event sent when object receives a draw active
    char key[16];
public:
    ico_button(int X, int Y, int ID, int Up, int down, int upa, int downa, ifield *Next, int act_id=-1, char *help_key=NULL);
    
    virtual void    area(int &x1, int &y1, int &x2, int &y2);
    virtual void    draw_first(image *screen) { draw(0, screen); }
    virtual void    draw(int active, image *screen); 
    virtual void    handle_event(event &ev, image *screen, input_manager *im);
    virtual char    *read() { return (char*) &up; }
//    virtual int     selectable() { return 0; }
    void            set_xy(int X, int Y) { x = X; y = Y; }
    int             X() { return x; }
    int             Y() { return y; }
    int             status() { return up; }
    void            set_act_id(int id);
};

class ico_switch_button : public ifield {
    ifield *blist, *cur_but;
    int act;
public:
    ico_switch_button(int X, int Y, int ID, int start_on, ifield *butts, ifield *Next);
    ~ico_switch_button();
    virtual void    area(int &x1, int &y1, int &x2, int &y2);
    virtual void    draw_first(image *screen) { cur_but->draw_first(screen); }
    virtual void    draw(int active, image *screen) { cur_but->draw(active, screen); act = active; }
    virtual void    handle_event(event &ev, image *screen, input_manager *im);
    virtual         ifield *unlink(int id);
    virtual char    *read() { return cur_but->read(); }
};

#endif