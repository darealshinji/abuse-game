#include "linked.hpp"
#include "console.hpp"
#include "jmalloc.hpp"
#include <ctype.h>
#include <stdarg.h>

console *GameConsole = NULL;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void console::put_string(char *st)
{
    while (*st) {
        put_char(*st);
        st++;
    }
    wm->flush_screen();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void console::redraw()
{
    clear();
    char *s = text;
    int dx, dy, xa = fnt->width(), ya = fnt->height(), i, j;
    for (j = 0, dy = wy(); j < text_h; j++, dy += ya) {
        for (i = 0, dx = wx(); i < text_w; i++, s++, dx += xa) {
            if (*s)
                fnt->put_char(screen, dx, dy, *s);
        }
    }
    fnt->put_char(screen, wx() + cx * xa, wy() + cy * ya, '_');
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
console::~console()
{
    jfree(text);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
console::console(JCFont *font, int width, int height, char *Name)
{
    next = NULL;
    name = NULL;
    inm = NULL;

    property.hidden = 1;
    property.moveable = 0;
    property.flags = property.flags | JWINDOW_TRANSPARENT_FLAG;
    backg = wm->black();

    if (font)
        fnt = font;
    else
        fnt = wm->font();
    
    l = fnt->width() * width + 2;
    h = fnt->height() * height + 2;

    ClientX1 = 0;
    ClientX2 = l - 1;
    ClientY1 = 0;
    ClientY2 = h - 1;

    screen = new image(l, h, NULL, 2);
    image_list.unlink(screen);

    screen->clear(backg);

    text_w = width;
    text_h = height;
    text = (char*) jmalloc(text_w * text_h, "console screen");
    memset(text, ' ', text_w * text_h);
    x = y = 0;
    cx = cy = 0;
    lastx = 0;
    lasty = 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void console::draw_cursor()
{
    fnt->put_char(screen, cx * fnt->width() + wx(), cy * fnt->height() + wy(), '_');
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void console::draw_char(int x, int y, char ch)
{
    int fw = fnt->width(), fh = fnt->height();
    int dx = wx() + x * fw, dy = wy() + y * fh;
    screen->bar(dx, dy, dx + fw - 1, dy + fh - 1, wm->black());
    fnt->put_char(screen, dx, dy, ch); 
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void console::do_cr()
{
    if (cx < text_w && cy < text_h)
        draw_char(cx, cy, text[cy * text_w + cx]);
    cx = 0;
    cy++;  
    if (cy >= text_h) {
        cy = text_h - 1;
        memmove(text, text + text_w, text_w * (text_h - 1));
        memset(text + text_w * (text_h - 1), ' ', text_w);
        redraw();
    }
    else
        draw_cursor();    
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void console::put_char(char ch)
{
    switch (ch) {
        case JK_BACKSPACE: {
            if (cx) {
                draw_char(cx, cy, text[cy * text_w + cx]);
                cx--;
                draw_cursor();
            }
        } 
        break;
        case '\n':
        case JK_ENTER: {
            do_cr();
        } 
        break;
        default: {
            text[cy * text_w + cx] = ch;
            draw_char(cx, cy, ch);
            cx++;
            if (cx >= text_w)
                do_cr();
            else 
                draw_cursor();
        }
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void console::printf(const char *format, ...)
{
    char st[300];
    va_list ap;
    va_start(ap, format);
    vsprintf(st, format, ap);
    va_end(ap);
    put_string(st);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
shell_term::shell_term(JCFont *font, int width, int height, char *Name) :
          console(font, width, height, Name)
{
    shcmd[0] = 0;
    prompt();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void shell_term::prompt()
{
    put_string("(?=help)>");
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void shell_term::execute(char *st)
{
    put_string(st);
    put_string(" : unhandled\n");
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int shell_term::handle_event(event &ev)
{
    if (ev.window == this) {
        switch (ev.type) {
            case EV_KEY : {
                switch (ev.key) {
                case JK_BACKSPACE: {
                        if (shcmd[0] != 0) {
                                shcmd[strlen(shcmd) - 1] = 0;
                                put_char(ev.key);
                        }
                    } break;
                case JK_ENTER : {
                        put_char(ev.key);
                        execute(shcmd);
                        prompt();
                        shcmd[0] = 0;
                    } break;
                default : {
                        if (ev.key < 256 && isprint(ev.key)) {
                            int x = strlen(shcmd);
                            shcmd[x + 1] = 0;
                            shcmd[x] = ev.key;
                            put_char(ev.key);
                        }
                    } break;
                } break;
            }
        }
        return 1;
    } 
    return 0;
}