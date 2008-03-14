#include "video.h"
#include "image.hpp"
#include "event.hpp"
#include "filter.hpp"
#include "jwindow.hpp"

window_manager *wm;

int jw_left   = 3,
    jw_right  = 3,
    jw_top    = 2,
    jw_bottom = 3;

int frame_top() { return jw_top; }
int frame_bottom() { return jw_bottom; }
int frame_left() { return jw_left; }
int frame_right() { return jw_right; }

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
ifield::ifield()
{
    owner = NULL;
    x = 0;
    y = 0;
    next = NULL;
    id = 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
ifield::~ifield()
{

}

/* --------------------------------------------------------------------------- 
Sets the size of the border around each window
 ------------------------------------------------------------------------- /**/
void set_frame_size(int x)
{  
    if (x < 1)
        x = 1;
    jw_left = x;
    jw_right = x;
    jw_top = 2;
    jw_bottom = x;
}

/* --------------------------------------------------------------------------- 
// true if a window lies in this area
 ------------------------------------------------------------------------- /**/
int window_manager::window_in_area(int x1, int y1, int x2, int y2)
{
    for (jwindow *f = first; f; f = f->next) 
        if (f->x <= x2 && f->y <= y2 && f->x + f->l - 1 >= x1 && f->y + f->h - 1 >= y1)
            return 1;
        return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void window_manager::grab_focus(jwindow *j)
{ 
    grab = j;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void window_manager::release_focus()
{ 
    grab = NULL;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void window_manager::close_window(jwindow *j)
{
    delete j;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void window_manager::hide_windows()
{
    jwindow *p;
    for (p=first;p;p=p->next) {
        if (!(p->flags() & JWINDOW_NOAUTOHIDE_FLAG)) {
            p->hide();
            screen->add_dirty(p->x,p->y,p->x+p->l-1,p->y+p->h-1);
        }
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void window_manager::show_windows()
{
    jwindow *p;
    for (p=first;p;p=p->next)
        if (!(p->flags() & JWINDOW_NOAUTOHIDE_FLAG))
            show_window(p);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void window_manager::hide_window(jwindow *j)
{
    jwindow *k;
    if (j==first)
        first=first->next;
    else {
        for (k=first;k->next!=j;k=k->next)
            k->screen->add_dirty(j->x-k->x,j->y-k->y, 
            j->x+j->l-1-k->x,j->y+j->h-1-k->y);
        k->screen->add_dirty(j->x-k->x,j->y-k->y, 
            j->x+j->l-1-k->x,j->y+j->h-1-k->y);
        k->next=j->next;
    }
    screen->add_dirty(j->x,j->y,j->x+j->l-1,j->y+j->h-1);
    j->hide();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void window_manager::show_window(jwindow *j)
{
    if (j->hidden()) {
        j->show();
        j->screen->add_dirty(0, 0, j->l - 1, j->h - 1);
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void window_manager::get_event(event &ev)
{
    jwindow *j;
    eh->get_event(ev);
    if (ev.type == EV_KEY)
        key_state[ev.key] = 1;
    else if (ev.type == EV_KEYRELEASE)
        key_state[ev.key] = 0;
    
    if (state == inputing) {
        for (ev.window = NULL, j = first; j; j = j->next)
            if (!j->hidden() && ev.mouse_move.x >= j->x && ev.mouse_move.y >= j->y && ev.mouse_move.x < j->x + j->l && ev.mouse_move.y < j->y + j->h)
                ev.window = j;

        if (!ev.window && grab)
            ev.window = grab;

        if (ev.window) {
            int closew = 0, movew = 0;
            int WinClientX = 0, WinClientY = 0;
            WinClientX = ev.mouse_move.x - ev.window->x;
            WinClientY = ev.mouse_move.y - ev.window->y;
            if (ev.type == EV_MOUSE_BUTTON && (ev.mouse_button & 1) && WinClientX >= 0 && WinClientX < ev.window->l && 
                                                                      WinClientY >= 0 && WinClientY < ev.window->TopBorder()) {
                if (WinClientX >= 1 && WinClientX < font()->height() + 1 && WinClientY > 1 && WinClientY < font()->height() + 1)
                    closew = 1;
                else if (ev.window->moveable())
                    movew = 1;
            }
            else if (grab)
                ev.window = grab;
            
            if (ev.type == EV_KEY && ev.key == JK_ESC)
                closew = 1;
            
            if (closew)
                ev.type = EV_CLOSE_WINDOW;
            else if (movew) {
                int red = 0;
                if (ev.window == first) {      // see if we need to raise the window
                    first = first->next;
                    if (first)
                        red = 1;
                }
                else {
                    jwindow *last = first;
                    for (; last->next != ev.window; last = last->next);
                    if (ev.window->next)
                        red = 1;
                    last->next = ev.window->next;
                }
                if (!first)
                    first = ev.window;
                else {
                    jwindow *last = first;
                    for (; last->next; last = last->next);
                    last->next = ev.window;
                }
                ev.window->next = NULL;
                if (red) {
                    jwindow *j = ev.window;
                    j->screen->add_dirty(0, 0, j->l - 1, j->h - 1);
                    flush_screen();
                }
                state = dragging;
                drag_window = ev.window;
                drag_mousex = WinClientX;
                drag_mousey = WinClientY;
                ev.type = EV_SPURIOUS;
            }
            else if (ev.window)
                ev.window->handle_event(ev);
        }
    }
    else if (state == dragging) {
        ev.window = drag_window;
        if (ev.type == EV_MOUSE_BUTTON && !(ev.mouse_button & 1)) { // user released left mouse button
            state = inputing;
            ev.type = EV_SPURIOUS;
        }
        else if (ev.type == EV_MOUSE_MOVE) {
            move_window(drag_window, ev.mouse_move.x - drag_mousex, ev.mouse_move.y - drag_mousey);
            flush_screen();
            ev.type = EV_DRAG_WINDOW;
            ev.window_position.x = ev.mouse_move.x + drag_mousex;
            ev.window_position.y = ev.mouse_move.y + drag_mousey;
        }
    }
    if (ev.type == EV_REDRAW) {
        for (j = first; j; j = j->next) 
            j->screen->add_dirty(ev.redraw.x1 - j->x, ev.redraw.y1 - j->y, ev.redraw.x2 - j->x, ev.redraw.y2 - j->y);
        screen->add_dirty(ev.redraw.x1, ev.redraw.y1, ev.redraw.x2, ev.redraw.y2);
        flush_screen();
        ev.type = EV_SPURIOUS;   // we took care of this one by ourselves.
    }
    if (ev.type == EV_SCREEN_REFRESH) {
        eh->mouse_sprite()->visual->RestoreSurface();
        if (pal)
            pal->load();
        for (j = first; j; j = j->next) 
            j->redraw();
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void window_manager::resize_window(jwindow *j, int l, int h)
{
    jwindow *p;
    screen->add_dirty(j->x, j->y, j->x + j->l - 1, j->y + j->h - 1);
    for (p = first; p != j; p = p->next)
        p->screen->add_dirty(j->x - p->x, j->y - p->y, j->x + j->l - 1 - p->x, j->y + j->h - 1 - p->y);
    j->resize(l, h);
    if (!frame_suppress)
        j->redraw();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void window_manager::move_window(jwindow *j, int x, int y)
{
    jwindow *p;
    screen->add_dirty(j->x,j->y,j->x+j->l-1,j->y+j->h-1);
    for (p=first;p!=j;p=p->next)
        p->screen->add_dirty(j->x-p->x,j->y-p->y,j->x+j->l-1-p->x,j->y+j->h-1-p->y);
    j->x=x;
    j->y=y;
    j->screen->add_dirty(0,0,j->l-1,j->h-1);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
window_manager::window_manager(image *Screen, palette *Pal, int Hi, int Med, int Low, JCFont *Font)
{
    wm = this;
    screen = Screen;
    hi = Hi;
    low = Low;
    med = Med;
    first = NULL;
    pal = Pal;
    grab = NULL;
    bk = pal->find_closest(0, 0, 0);
    state = inputing;
    fnt = Font;
    wframe_fnt = Font;
    memset(key_state, 0, sizeof(key_state));
    eh = new event_handler(screen, pal);
    frame_suppress = 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void window_manager::DeleteWindow(jwindow *Which)
{
    if (grab == Which)
        grab = NULL;
    if (state == dragging && Which == drag_window)  // close the window we were dragging
        state = inputing;

    if (first == Which)
        first = first->next;
    else {
        jwindow *search;
        for (search = first; search->next != Which; search = search->next)
            search->screen->add_dirty(Which->x - search->x, Which->y-search->y, Which->x + Which->l - 1 - search->x, Which->y + Which->h - 1 - search->y);
        search->screen->add_dirty(Which->x - search->x, Which->y-search->y, Which->x + Which->l - 1 - search->x, Which->y + Which->h - 1 - search->y);
        search->next = Which->next;
    }
    screen->add_dirty(Which->x, Which->y, Which->x + Which->l - 1, Which->y + Which->h - 1);

}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void window_manager::AddWindow(jwindow *NewWin)
{
    jwindow *k;
    if (!first)
        first = NewWin;
    else {
        k = first;
        while (k->next) 
            k = k->next;
        k->next = NewWin;
        NewWin->next = NULL;
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
jwindow *window_manager::new_window(int x, int y, int l, int h, ifield *fields, char *Name)
{
    if (x > screen->width() - 4) 
        x = screen->width() - 25;
    if (y > screen->height() - 4) 
        y = screen->height() - 10;
    
    jwindow *j = new jwindow(x, y, l, h, fields, Name);
    j->show();
    return j;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void window_manager::flush_screen()
{
    jwindow *p;
    
    int mx,my;
    image *mouse_pic, *mouse_save;
    
    if (has_mouse()) {
        mouse_pic = eh->mouse_sprite()->visual;
        mouse_save = eh->mouse_sprite()->save;
        mx = eh->mouse->drawx();
        my = eh->mouse->drawy();
        
        screen->put_part(mouse_save, 0, 0, mx, my, mx + mouse_pic->width() - 1, my + mouse_pic->height() - 1);
        mouse_pic->put_image(screen, mx, my, 1);
    }
    
/*    for (p = first; p; p = p->next)
        if (!p->hidden())
            screen->delete_dirty(p->x, p->y, p->x + p->l - 1, p->y + p->h - 1); */
    update_dirty(screen);
        
    if (has_mouse())
        mouse_save->put_image(screen, mx, my);
    
    for (p = first; p; p = p->next) {
        if (!p->hidden()) {
            if (has_mouse()) {
                p->screen->put_part(mouse_save, 0, 0, mx - p->x, my - p->y, mx - p->x + mouse_pic->width() - 1, my - p->y + mouse_pic->height() - 1);
                if (has_mouse())
                    mouse_pic->put_image(p->screen, mx-p->x, my-p->y, 1);
            }
/* js
            for (q = p->next; q; q = q->next)
                if (!q->hidden())
                    p->screen->delete_dirty(q->x-p->x, q->y - p->y, q->x + q->l - 1 - p->x, q->y + q->h - 1 - p->y); js/**/
            if ((p->flags() & JWINDOW_TRANSPARENT_FLAG) != JWINDOW_TRANSPARENT_FLAG)
                put_image(p->screen, p->x, p->y); // Put the window on the screen
            else
                TransparentBlit(p->screen, p->x, p->y, p->backg);
            if (has_mouse())
                mouse_save->put_image(p->screen, mx - p->x, my - p->y, 0);
        }
    }
    Finish_Frame();
}

/* ---------------------------------------------------------------------------
 ------------------------------------------------------------------------- /**/
window_manager::~window_manager()
{
    delete eh;
    while (first)
        close_window(first);
    wm = NULL;
}

/* ---------------------------------------------------------------------------
 ------------------------------------------------------------------------- /**/
void jwindow::set_moveability(int x)
{
    property.moveable = x;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
jwindow::jwindow()
{
    ClientX1 = 0; ClientY1 = 0; ClientX2 = 0; ClientY2 = 0;
    hide();
    set_moveability(1);
    property.flags = JWINDOW_NOAUTOHIDE_FLAG;
    inm = new input_manager(this, NULL);
    screen = NULL;
    name = NULL;
    wm->AddWindow(this);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
jwindow::jwindow(int X, int Y, int L, int H, ifield *fields, char *Name)
{
    l = 0; h = 0; 
    set_moveability(1);
    property.flags = 0;
    ClientX1 = LeftBorder();
    if (Name)
        ClientY1 = TopBorder();
    else
        ClientY1 = jw_top + 5;
    screen = NULL;
    inm = new input_manager(screen, fields);
    Reconfigure();
    if (L < 0)
        l = l - L; 
    else 
        l = L + LeftBorder();
    if (H < 0)
        h = h - H;
    else 
        h = H + TopBorder();
    
    if (Y < 0)
        y = yres - h + Y - TopBorder() - BottomBorder() - 1; 
    else 
        y = Y;
    if (X < 0)
        x = xres - l + X - LeftBorder() - RightBorder() - 1; 
    else x = X;
    
    backg = wm->medium_color();
    ClientX2 = l - 1;
    ClientY2 = h - 1;
    l += RightBorder();
    h += BottomBorder();
    
    if (L == -1)
        if (l < 15 + LeftBorder() + RightBorder())
            l = 15 + LeftBorder() + RightBorder();
    if (H == -1)
        if (h < TopBorder() + BottomBorder())
            h = TopBorder() + BottomBorder();
    screen = new image(l, h, NULL, 2);
    screen->clear(backg);
    image_list.unlink(screen); // Keep this from getting destroyed when image list is cleared
    inm->screen = screen;
    next = NULL;
    if (Name == NULL)
        name = NULL;
    else
        name = strcpy((char*) jmalloc(strlen(Name) + 1, "jwindow::window name"), Name);
    wm->AddWindow(this);
    if (!wm->frame_suppress)
        redraw();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void jwindow::SetBorder(int x)
{


}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
jwindow::~jwindow()
{
    wm->DeleteWindow(this);
    local_close();
    delete screen;
    delete inm;
    jfree(name);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void jwindow::resize(int L, int H)
{
    screen->change_size(L, H);
    l = L; h = H;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void jwindow::local_close() { ; }

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void jwindow::redraw()
{
    int hi = wm->bright_color();
    int med = wm->medium_color();
    int low = wm->dark_color();
    JCFont *fnt = wm->frame_font();

    if (name) {
        if (RightBorder() >= 1) {
            screen->wiget_bar(0, 0, l - 1, h - 1, hi, med, low);
            if (RightBorder() >= 3)
                screen->wiget_bar(RightBorder() - 1, TopBorder() - 1, l - LeftBorder(), h - BottomBorder(), low, med, hi);
            else
                screen->line(LeftBorder() - 1, TopBorder() - 1, RightBorder() - 1, TopBorder() - 1, low);
        }

        screen->rectangle(2, 2, TopBorder() - 2, TopBorder() - 3, wm->black());
        screen->wiget_bar(3, 3, TopBorder() - 3, TopBorder() - 4, hi, med, low); // draws the 'close' button
    }
    else {
        if (RightBorder() >= 1) {
            screen->wiget_bar(0, 0, l - 1, h - 1, hi, med, low);
            if (RightBorder() >= 3)
                screen->wiget_bar(RightBorder() - 1, jw_top + 4, l - LeftBorder(), h - BottomBorder(), low, med, hi);
            else
                screen->line(LeftBorder() - 1, jw_top + 4, RightBorder() - 1, jw_top + 4, low);
        }

        screen->rectangle(1, 1, 4, 4, wm->black());
        screen->wiget_bar(2, 2, 3, 3, hi, med, low); // draws the 'close' button
    }
    if (name && name[0]) {
        screen->bar(TopBorder(), 1, TopBorder() + fnt->width() * strlen(name) + 1, TopBorder() - 2, med);
        fnt->put_string(screen, TopBorder() + 1, 1, name, low);
    }

    screen->bar(x1(), y1(), x2(), y2(), backg); // clear 'client' region
    inm->redraw();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int jwindow::handle_event(event &ev)
{
    if (ev.type == EV_SCREEN_REFRESH) {
        redraw();
    }
    else if (inm)
        inm->handle_event(ev, this);
    return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void jwindow::Reconfigure()
{
    int x1, y1, x2, y2;
    ifield *i;
    l = 2; 
    h = 2; 
    for (i = inm->first; i; i = i->next) {
        i->SetOwner(this);
        i->area(x1, y1, x2, y2);
        if ((int) y2 > (int) h) 
            h = y2;
        if ((int) x2 > (int) l) 
            l = x2;
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
ifield *input_manager::unlink(int id)     // unlinks ID from fields list and return the pointer to it
{ 
    for (ifield *i=first,*last;i;i=i->next) {
        if (i->id==id) {
            if (i==first)
                first=first->next;
            else
                last->next=i->next;
            if (active==i)
                active=first;
            return i;
        }
        ifield *x=i->unlink(id);
        if (x) return x;
        last=i;
    }
    return NULL;   // no such id
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
input_manager::~input_manager() 
{ 
    ifield *i; 
    while (first) {
        i = first; 
        first = first->next; 
        delete i;
    }
} 

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void input_manager::clear_current()
{
    if (owner)
        screen = owner->screen;
    if (active)
        active->draw(0, screen);
    active = NULL;
}

extern unsigned char shift_flag;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
ifield* input_manager::SelectNextField(int reverse)
{
    ifield *orig = active;
    if (active) {
        if (active != LastOver)
            active->draw(0, screen);
        active = active->next;
    }
    else
        active = first;
    if (!reverse) { // just a tab
        if (!active && orig)
            active = first;
        while (active && !active->selectable() && orig != active) {
            active = active->next;
            if (orig && !active)
                active = first;
        }
    }
    else { // Shift-tab
        ifield *found = first, *selected = orig;
        if (orig == first)
            orig = NULL;
        while (found && found->next != orig) {
            found = found->next;
            if (found->selectable())
                selected = found;
        }
        if (found->selectable())
            active = found;
        else
            active = selected;
    }
    return active;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void input_manager::handle_event(event &ev, jwindow *j)
{
    if (owner)
        screen = owner->screen;
    ifield *i, *in_area = NULL;
    int x1, y1, x2, y2;
    if (j) {
        ev.mouse_move.x -= j->x;
        ev.mouse_move.y -= j->y;
        cur = j;
    }

    if (!grab) {
        switch (ev.type) {
            case EV_MOUSE_BUTTON: // OnMouseButton
                if (ev.mouse_button & 1) { // OnLeftButton
                    for (i = first; i; i = i->next) {
                        i->area(x1, y1, x2, y2);
                        if (ev.mouse_move.x >= x1 && ev.mouse_move.y >= y1 && ev.mouse_move.x <= x2 && ev.mouse_move.y <= y2)
                            in_area = i;
                    }
                    if (in_area != active && (no_selections_allowed || (in_area && in_area->selectable()))) {
                        if (active)
                            if (active != LastOver)
                                active->draw(0, screen);
                            else
                                active->draw(2, screen);
                        active = in_area;
                        if (active)
                            active->draw(1, screen);
                    }
                }
                break;
            case EV_MOUSE_MOVE:
                for (i = first; i; i = i->next) {
                    i->area(x1, y1, x2, y2);
                    if (ev.mouse_move.x >= x1 && ev.mouse_move.y >= y1 && ev.mouse_move.x <= x2 && ev.mouse_move.y <= y2)
                        in_area = i;
                }
                if (in_area != LastOver && (no_selections_allowed || (in_area && in_area->selectable()))) {
                    if (LastOver && LastOver != active)
                        LastOver->draw(0, screen);
                    LastOver = in_area;
                    if (LastOver)
                        if (LastOver != active)
                            LastOver->draw(2, screen);
                        else
                            LastOver->draw(3, screen);
                }
                break;
            case EV_KEY:
                switch (ev.key) {
                    case JK_TAB:
                        SelectNextField(shift_flag);
                        if (active)
                            active->draw(1, screen);
                        break;
                }
                break;
            case EV_SCREEN_REFRESH:
                redraw();
                break;
        }
    }
    else
        active = grab;
    
    if (active) {
        if (ev.type != EV_MOUSE_MOVE && ev.type != EV_MOUSE_BUTTON)
            active->handle_event(ev, screen, this);
        else {
            active->area(x1, y1, x2, y2);
            if (grab || (ev.mouse_move.x >= x1 && ev.mouse_move.y >= y1 && ev.mouse_move.x <= x2 && ev.mouse_move.y <= y2)) {
                if (j)
                    active->handle_event(ev, screen, j->inm);
                else
                    active->handle_event(ev, screen, this);
            }
        }
    }

    if (j) {
        ev.mouse_move.x += j->x;
        ev.mouse_move.y += j->y;
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void input_manager::allow_no_selections()
{
    no_selections_allowed=1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void input_manager::redraw()
{
    if (owner)
        screen = owner->screen;
    ifield *i;
    for (i = first; i; i = i->next)
        i->draw_first(screen);
    if (active)
        active->draw(1, screen);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
input_manager::input_manager(image *Screen, ifield *First)
{
    no_selections_allowed = 0;
    cur = NULL;
    grab = NULL;
    LastOver = NULL;
    screen = Screen;
    owner = NULL;
    active = first = First;
    while (active && !active->selectable())
        active = active->next;
    if (screen)
        redraw();
}
 
/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
input_manager::input_manager(jwindow *Owner, ifield *First)
{
    no_selections_allowed = 0;
    cur = NULL;
    grab = NULL;
    LastOver = NULL;
    active = first = First;
    screen = NULL;
    owner = Owner;
    while (active && !active->selectable())
        active = active->next;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void input_manager::grab_focus(ifield *i)
{
    grab = i;
    if (cur)
        wm->grab_focus(cur);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void input_manager::release_focus()
{
    grab = NULL;
    if (cur)
        wm->release_focus();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void input_manager::remap(filter *f)
{
    for (ifield *i=first;i;i=i->next)
        i->remap(f);
    redraw();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void input_manager::add(ifield *i) 
{
    ifield *f = first;
    if (i->selectable()) {
        if (!f)
            first=i;
        else {
            while (f->next)
                f = f->next;
            f->next = i; 
        }
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
ifield *input_manager::get(int id)
{
    ifield *f;
    for (f = first; f; f = f->next) {
        ifield *ret = f->find(id);
        if (ret)
            return ret;
    }
    return NULL;
}

/* --------------------------------------------------------------------------- 
Function to re-position the control with respect to the "client" area of the
window
 ------------------------------------------------------------------------- /**/
void ifield::SetOwner(jwindow *Owner)
{
    if (owner)
        Move(x - owner->x1(), y - owner->y1());
    owner = Owner;
    if (owner)
        Move(x + owner->x1(), y + owner->y1());
}