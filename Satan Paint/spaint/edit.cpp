#include "Include/edit.h"
#include "Include/id.h"

editor::editor()
{
    w=NULL;
}

void editor::toggle_window()
{
    if (w) close_window(); else open_window();
}

#include "Include/pcommand.h"
void editor::open_window()
{
    if (!w)
        w=eh->new_window(-1,-1,0,0,paint->current_item()->fields(NULL));
}

int editor::handle_event(event &ev)
{
    if (w) {
        if (ev.type==EV_CLOSE_WINDOW && ev.window==w) {
            eh->close_window(w);
            w=NULL;
        }
        else if (ev.type==EV_MESSAGE && ev.message.id==ITEM_OK) {
            paint->current_item()->get_fields(w->inm);
            close_window();
        }
        else if (ev.type==EV_MESSAGE && ev.message.id==ITEM_CANCEL)
            close_window();
        else
            return 0;
    } else
        return 0;
    return 1;
}