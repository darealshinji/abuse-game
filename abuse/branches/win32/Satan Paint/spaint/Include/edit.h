#ifndef __EDIT_HPP
#define __EDIT_HPP

#include "paint.h"

class editor
{
    jwindow *w; 
public :
    editor();
    void open_window();
    void close_window() { if (w) { eh->close_window(w); w=NULL;} }
    void toggle_window();
    void redraw_input() { if (w) { w->inm->redraw(); } }
    int handle_event(event &ev);
} ;


#endif