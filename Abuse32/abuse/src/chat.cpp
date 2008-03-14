#include "chat.hpp"

extern char *symbol_str(char *name);
chat_console *chat = NULL;

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
chat_console::chat_console(JCFont *font, int width, int height) :
console(font, width, height < 4 ? 4 : height, symbol_str("CHAT"))
{
    property.hidden = 1;
    clear();
    cx = 0;
    cy = h - 1;
    lastx = xres / 2 - screen_w() / 2;
    lasty = yres - h;
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void chat_console::clear()
{
    memset(text, ' ', text_w * text_h);
    memset(text + text_w * (text_h - 2), '-', text_w);
    redraw();
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void chat_console::put_all(char *st)
{
    memmove(text, text + text_w, text_w * (text_h - 3));
    memset(text + text_w * (text_h - 3), ' ', text_w);  
    memcpy(text + text_w * (text_h - 3), st , strlen(st));
    redraw();
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void chat_console::draw_user(char *st)
{
    memset(text + text_w * (text_h - 1), ' ', text_w);
    memcpy(text + text_w * (text_h - 1), st, strlen(st));
    cx = strlen(st);
    cy = text_h - 1;
    redraw();
}

