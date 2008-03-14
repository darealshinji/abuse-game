#ifndef __INPUT_HPP_
#define __INPUT_HPP_
#include "jwindow.hpp"
#include "filter.hpp"

// --------------------------------------------------------------------------- 
class button : public ifield {
    int up,act;
    char *text;
    image *visual,*pressed,*act_pict;
    int act_id;
public:
    button(int X, int Y, int ID, char *Text, ifield *Next);
    button(int X, int Y, int ID, image *vis, ifield *Next);
    button(int X, int Y, int ID, image *Depressed, image *Pressed, image *active, ifield *Next);
    
    virtual void area(int &x1, int &y1, int &x2, int &y2);
    virtual void draw_first(image *screen);
    virtual void draw(int active, image *screen); 
    virtual void handle_event(event &ev, image *screen, input_manager *im);
    void change_visual(image *new_visual);
    virtual void remap(filter *f);
    virtual ~button() {
        if (text)
            jfree(text);
    }
    void push();
    virtual char *read() { return (char *)&up; }
    int status() { return up; }
    void set_act_id(int id) { act_id=id; }
};

// --------------------------------------------------------------------------- 
class button_box : public ifield {
    button *buttons;
    int maxdown;
public:
    button_box(int X, int Y, int ID, int MaxDown, button *Buttons, ifield *Next);
    void add_button(button *b);
    void press_button(int id);      // if button box doesn't contain id, nothing happens
    virtual void remap(filter *f);
    virtual void area(int &x1, int &y1, int &x2, int &y2);
    virtual void draw_first(image *screen);
    virtual void draw(int active, image *screen); 
    virtual void handle_event(event &ev, image *screen, input_manager *im);  
    virtual ~button_box();
    virtual char *read();   // return pointer to first button which is depressed
    virtual ifield *find(int search_id);  // should return pointer to item you control with this id
    void arrange_left_right();
    void arrange_up_down();
    virtual void Move(int NewX, int NewY);
};

// --------------------------------------------------------------------------- 
class text_field : public ifield {
    int Number;
    unsigned int slen;
    unsigned short cur;
    char *prompt,*data,*format;
    int xstart() { return x+wm->font()->width()*(strlen(prompt)+1)+3; }
    int xend() { return x+wm->font()->width()*(strlen(prompt)+1+strlen(format))+7; }
    int yend() { return y+wm->font()->height()+5; }
    void draw_cur(int color, image *screen);
    int last_spot() {
        return strlen(data);
    }
    void draw_text(image *screen);
public: 
    text_field(int X, int Y, int ID, char *Prompt, char *Format, char *Data, ifield *Next);
    text_field(int X, int Y, int ID, char *Prompt, char *Format, double Data, ifield *Next);
    
    virtual void area(int &x1, int &y1, int &x2, int &y2);
    virtual void draw_first(image *screen);
    virtual void draw(int active, image *screen); 
    virtual void handle_event(event &ev, image *screen, input_manager *im);
    
    virtual ~text_field() {
        jfree(prompt);
        jfree(format);
        jfree(data);
    }
    virtual char *read();
    virtual int ReadInt() {
        return atoi(data);
    }
    virtual double ReadDouble() {
        return atof(data);
    }
    void change_data(char *new_data, int new_cursor,       // cursor==-1, does not change it.
        int active, image *screen);
};

// --------------------------------------------------------------------------- 
class info_field : public ifield {
    char *text;
    int w,h;
    void put_para(image *screen, char *st, int dx, int dy, int xspace, 
        int yspace, JCFont *font, int color);
public: 
    info_field(int X, int Y, int ID, char *info, ifield *Next);
    virtual void area(int &x1, int &y1, int &x2, int &y2);
    virtual void draw_first(image *screen);
    virtual void draw(int active, image *screen) { ; }
    virtual void handle_event(event &ev, image *screen, input_manager *im) { ; }
    virtual char *read() { return text; }
    virtual int selectable() { return 0; } 
    virtual ~info_field() { jfree(text); }
};

ifield* center_ifield(ifield *i, int x1, int x2, ifield *place_below);

#endif