#ifndef __CONSOLE_HPP_
#define __CONSOLE_HPP_
#include "jwindow.hpp"

class console : public jwindow {
protected:
    int lastx, lasty, text_w, text_h, cx, cy;
    JCFont *fnt;
    char *text;
    char *name;
    char *input;
public:
    __inline static int LeftBorder() { return 0; }
    __inline static int RightBorder() { return 0; }
    __inline static int TopBorder() { return 0; }
    __inline static int BottomBorder() { return 0; }
    console(JCFont *font, int width, int height, char *Name);
    virtual void redraw();
    void put_char(char ch);
    void do_cr();
    int screen_w() {
        return text_w * fnt->width();
    }
    int screen_h() {
        return text_h * fnt->height();
    }
    int wx() {
        return x1();
    }
    int wy() {
        return y1();
    }
    void draw_cursor();
    void put_string(char *st);
    void draw_char(int x, int y, char ch);
    void printf(const char *format, ...);
    ~console();
};

class shell_term : public console {
    char shcmd[300];
public:
    shell_term(JCFont *font, int width, int height, char *Name);
    virtual int handle_event(event &ev);
    virtual void prompt();
    virtual void execute(char *st);
};

extern console *GameConsole;

#endif

