/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "video.hpp"
#include "image.hpp"
#include "input.hpp"
#include "event.hpp"
#include "filter.hpp"
#include "event.hpp"
#include "jwindow.hpp"

static int jw_left = 3, jw_right = 3, jw_top = 2, jw_bottom = 3;

int frame_top() { return jw_top; }
int frame_bottom() { return jw_bottom; }
int frame_left() { return jw_left; }
int frame_right() { return jw_right; }

//
//  Sets the size of the border around each window
//
void set_frame_size(int x)
{  
    if(x < 1)
        x = 1;
    jw_left = x;
    jw_right = x;
    jw_top = 2;
    jw_bottom = x;
}

 // true if a window lies in this area
int window_manager::window_in_area(int x1, int y1, int x2, int y2)
{
  for (jwindow *f=first;f;f=f->next) 
    if (f->x<=x2 && f->y<=y2 && f->x+f->l-1>=x1 && f->y+f->h-1>=y1)
      return 1;
  return 0;
}

void window_manager::grab_focus(jwindow *j)
{ grab=j; }

void window_manager::release_focus()
{ grab=NULL; }


void window_manager::close_window(jwindow *j)
{
    delete j;
}

void window_manager::hide_windows()
{
  jwindow *p;
  for (p=first;p;p=p->next)
  {
    if (!p->is_hidden())
    {
      p->hide();
      screen->add_dirty(p->x,p->y,p->x+p->l-1,p->y+p->h-1);
    }
  }
}

void window_manager::show_windows()
{
  jwindow *p;
  for (p=first;p;p=p->next)
    if (p->is_hidden())
      show_window(p);      
}

void window_manager::hide_window(jwindow *j)
{
  jwindow *k;
  if (j==first)
    first=first->next;
  else
  {
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

void window_manager::show_window(jwindow *j)
{
  if (j->is_hidden())
  {
    j->show();
    j->screen->add_dirty(0,0,j->l-1,j->h-1);
  }
}

void window_manager::get_event(event &ev)
{
  jwindow *j;
  eh->get_event(ev);
  if (ev.type==EV_KEY)
    key_state[ev.key]=1;
  else if (ev.type==EV_KEYRELEASE)
    key_state[ev.key]=0;

  if (state==inputing)
  {
    for (ev.window=NULL,j=first;j;j=j->next)
      if (!j->is_hidden() && ev.mouse_move.x>=j->x && ev.mouse_move.y>=j->y &&
          ev.mouse_move.x<j->x+j->l && ev.mouse_move.y<j->y+j->h)
        ev.window=j;

    if (!ev.window && grab) ev.window=grab;

    if (ev.window)
    {
      int closew=0,movew=0;

      if ((ev.type==EV_MOUSE_BUTTON && ev.mouse_button==1 && ev.window &&
	   ev.mouse_move.x>=ev.window->x && ev.mouse_move.y>=ev.window->y &&
	   ev.mouse_move.x<ev.window->x+ev.window->l && ev.mouse_move.y<ev.window->y+ev.window->y1()))
      {
	if (ev.mouse_move.x-ev.window->x<11) closew=1;
	else if (ev.window->is_moveable()) movew=1;
      } else if (grab)
        ev.window=grab;

      if (ev.type==EV_KEY && ev.key==JK_ESC)
        closew=1;

      
    
      if (closew)
        ev.type=EV_CLOSE_WINDOW;
      else if (movew)
      {	
	int red=0;
	if (ev.window==first)       // see if we need to raise the window
	{
	  first=first->next;
	  if (first)
	    red=1;
	}
	else
	{
	  jwindow *last=first;
	  for (;last->next!=ev.window;last=last->next);
	  if (ev.window->next)
	    red=1;
	  last->next=ev.window->next;
	}
	if (!first)
	  first=ev.window;
	else
	{
	  jwindow *last=first;
	  for (;last->next;last=last->next);
	  last->next=ev.window;
	}
	ev.window->next=NULL;
	if (red)
	{
	  jwindow *j=ev.window;
/*	  screen->add_dirty(j->x,j->y,j->x+j->l-1,j->y+j->h-1);
	  for (p=first;p!=j;p=p->next)
	    p->screen->add_dirty(j->x-p->x,j->y-p->y,j->x+j->l-1-p->x,j->y+j->h-1-p->y);*/
	  j->screen->add_dirty(0,0,j->l-1,j->h-1);
	  flush_screen();
	}

        state=dragging;
        drag_window=ev.window;
        drag_mousex=ev.window->x-ev.mouse_move.x;
        drag_mousey=ev.window->y-ev.mouse_move.y;
        ev.type=EV_SPURIOUS;
      } else if (ev.window)
        ev.window->inm->handle_event(ev,ev.window);
    }
  } else if (state==dragging)
  {
    ev.window=drag_window;
    if (ev.type==EV_MOUSE_BUTTON && ev.mouse_button==0)  // user released the mouse
    {
      state=inputing;
      ev.type=EV_SPURIOUS;
    } else if (ev.type==EV_MOUSE_MOVE)
    {
       move_window(drag_window,ev.mouse_move.x+drag_mousex,ev.mouse_move.y+drag_mousey);
       flush_screen();
       ev.type=EV_DRAG_WINDOW;
       ev.window_position.x=ev.mouse_move.x+drag_mousex;
       ev.window_position.y=ev.mouse_move.y+drag_mousey;
    }
  } 
  if (ev.type==EV_REDRAW)
  {
    for (j=first;j;j=j->next) 
       j->screen->add_dirty(ev.redraw.x1-j->x,ev.redraw.y1-j->y,
		     ev.redraw.x2-j->x,ev.redraw.y2-j->y);
    screen->add_dirty(ev.redraw.x1,ev.redraw.y1,ev.redraw.x2,ev.redraw.y2);
    flush_screen();
    ev.type=EV_SPURIOUS;   // we took care of this one by ourselves.
  }
}

void jwindow::resize(int L, int H)
{
  screen->change_size(L,H);
  l=L; h=H;
}

void window_manager::resize_window(jwindow *j, int l, int h)
{
  jwindow *p;
  screen->add_dirty(j->x,j->y,j->x+j->l-1,j->y+j->h-1);
  for (p=first;p!=j;p=p->next)
    p->screen->add_dirty(j->x-p->x,j->y-p->y,j->x+j->l-1-p->x,j->y+j->h-1-p->y);
  j->resize(l,h);
  if (!frame_suppress)
  j->redraw();
}

void window_manager::move_window(jwindow *j, int x, int y)
{
    screen->add_dirty(j->x, j->y, j->x + j->l - 1, j->y + j->h - 1);
    for(jwindow *p = first; p != j; p = p->next)
        p->screen->add_dirty(j->x - p->x, j->y - p->y, j->x + j->l - 1 - p->x,
                             j->y + j->h - 1 - p->y);
    j->x = x;
    j->y = y;
    j->screen->add_dirty(0, 0, j->l - 1, j->h - 1);
}

window_manager::window_manager(image *Screen, palette *Pal, int Hi, 
                               int Med, int Low, JCFont *Font)
{
    wm = this;
    screen = Screen;
    hi = Hi; low = Low; med = Med; first = NULL; pal = Pal; grab = NULL;
    bk = pal->find_closest(0, 0, 0);
    state = inputing; fnt = Font;  wframe_fnt = Font;
    memset(key_state, 0, sizeof(key_state));
    eh = new event_handler(screen, pal);
    frame_suppress = 0;
}

window_manager::~window_manager()
{
    delete eh;
    while(first)
        close_window(first);
    wm = NULL;
}

void window_manager::add_window(jwindow *win)
{
    if(!first)
        first = win;
    else
    {
        jwindow *tmp = first;
        while(tmp->next)
            tmp = tmp->next;
        tmp->next = win;
        win->next = NULL;
    }
}

void window_manager::remove_window(jwindow *win)
{
    if(grab == win)
        grab = NULL;

    // close the window we were dragging
    if(state == dragging && win == drag_window)
        state = inputing;

    if(first == win)
        first = first->next;
    else
    {
        jwindow * search;
        for(search = first; search->next != win; search = search->next)
            search->screen->add_dirty(win->x - search->x,
                                      win->y - search->y,
                                      win->x + win->l - 1 - search->x,
                                      win->y + win->h - 1 - search->y);
        search->screen->add_dirty(win->x - search->x, win->y - search->y,
                                  win->x + win->l - 1 - search->x,
                                  win->y + win->h - 1 - search->y);
        search->next = win->next;
    }

    screen->add_dirty(win->x, win->y, win->x + win->l - 1,
                      win->y + win->h - 1);
}

jwindow * window_manager::new_window(int x, int y, int l, int h,
                                     ifield * fields, char const *name)
{
    if(x > screen->width () - 4)
        x = screen->width () - 25;
    if(y > screen->height () - 4)
        y = screen->height () - 10;
 
    jwindow * j = new jwindow (x, y, l, h, fields, name);
    j->show();

    return j;
}

void window_manager::flush_screen()
{
  jwindow *p,*q;

  int mx=0,my=0;
  image *mouse_pic=NULL,*mouse_save=NULL;
  
  if (has_mouse())
  {    
    mouse_pic=eh->mouse_sprite()->visual;
    mouse_save=eh->mouse_sprite()->save;
    mx=eh->mouse->drawx();
    my=eh->mouse->drawy();

    screen->put_part(mouse_save,0,0,mx,my,mx+mouse_pic->width()-1,my+mouse_pic->height()-1);
    mouse_pic->put_image(screen,mx,my,1);
  }
  
  for (p=first;p;p=p->next)
    if (!p->is_hidden())
       screen->delete_dirty(p->x,p->y,p->x+p->l-1,p->y+p->h-1);
  update_dirty(screen);

  if (has_mouse())
    mouse_save->put_image(screen,mx,my);


  for (p=first;p;p=p->next)
  {
    if (!p->is_hidden())
    {
      if (has_mouse())
      {      
	p->screen->put_part(mouse_save,0,0,mx-p->x,my-p->y,
			    mx-p->x+mouse_pic->width()-1,
			    my-p->y+mouse_pic->height()-1);
	if (has_mouse())
        mouse_pic->put_image(p->screen,mx-p->x,my-p->y,1);
      }
      

//      screen->delete_dirty(p->x,p->y,p->x+p->l-1,p->y+p->h-1);
      for (q=p->next;q;q=q->next)
        if (!q->is_hidden())
          p->screen->delete_dirty(q->x-p->x,
                              q->y-p->y,
                              q->x+q->l-1-p->x,
                              q->y+q->h-1-p->y);
      update_dirty(p->screen,p->x,p->y);
      if (has_mouse())
         mouse_save->put_image(p->screen,mx-p->x,my-p->y,0);
    }
  }
}

jwindow::jwindow()
{
    _x1 = _y1 = _x2 = _y2 = 0;
    _hidden = true;
    _moveable = true;
    // property.flags = JWINDOW_NOAUTOHIDE_FLAG;
    inm = new input_manager (this, NULL);
    screen = NULL;
    _name = NULL;
    wm->add_window(this);
}

jwindow::jwindow(int X, int Y, int L, int H, ifield *f, char const *name)
{
    l = 0;
    h = 0; 
    _hidden = false;
    _moveable = true;

    _x1 = left_border();
    _y1 = name ? top_border() : jw_top + 5;

    screen = NULL;
    inm = new input_manager(screen, f);
    reconfigure(); /* FIXME: TODO */

    l = L >= 0 ? L + left_border() : l - L;
    h = H >= 0 ? H + top_border() : h - H;
    y = Y >= 0 ? Y : yres - h + Y - top_border() - bottom_border() - 1;
    x = X >= 0 ? X : xres - l + X - left_border() - right_border() - 1;

    backg = wm->medium_color();

    _x2 = l - 1;
    _y2 = h - 1;
    l += right_border();
    h += bottom_border();

    if(L == -1)
        if(l < 15 + left_border() + right_border())
            l = 15 + left_border() + right_border();
    if(H == -1)
        if(h < top_border() + bottom_border())
            h = top_border() + bottom_border();
    screen = new image(l, h, NULL, 2);
    screen->clear(backg);
    // Keep this from getting destroyed when image list is cleared
    image_list.unlink(screen);

    inm->screen = screen;
    next = NULL;

    _name = NULL;
    if(name)
        _name = strcpy((char *)jmalloc(strlen(name) + 1,
                                       "jwindow::window name"), name);

    wm->add_window(this);
    if(!wm->frame_suppress)
        redraw();
}

jwindow::~jwindow()
{
    wm->remove_window(this);
    local_close();
    delete screen;
    delete inm;
    if(_name)
        jfree(_name);
}

void jwindow::reconfigure()
{
    int x1, y1, x2, y2;
    ifield *i;
    l = 2;
    h = 2;
    for(i = inm->first; i; i = i->next)
    {
        i->set_owner(this);
        i->area(x1, y1, x2, y2);
        if ((int)y2 > (int)h) 
            h = y2;
        if ((int)x2 > (int)l) 
            l = x2;
    }
}

void jwindow::local_close()
{
    ;
}

void jwindow::redraw()
{
    int hi = wm->bright_color ();
    int med = wm->medium_color ();
    int low = wm->dark_color ();
    JCFont * fnt = wm->frame_font ();

    if(_name)
    {
        if (right_border() >= 1)
        {
            screen->widget_bar (0, 0, l - 1, h - 1, hi, med, low);
            if (right_border() >= 3)
                screen->widget_bar (right_border() - 1, top_border() - 1,
                                l - left_border(), h - bottom_border(), low,
                                med, hi);
          
          else
            screen->line (left_border() - 1, top_border() - 1,
                           right_border() - 1, top_border() - 1, low);
        }
      screen->rectangle (2, 2, top_border() - 2, top_border() - 3,
                           wm->black ());
      screen->widget_bar (3, 3, top_border() - 3, top_border() - 4, hi, med, low);     // draws the 'close' button
    }
  
  else
    {
      if (right_border() >= 1)
        {
          screen->widget_bar (0, 0, l - 1, h - 1, hi, med, low);
          if (right_border() >= 3)
            screen->widget_bar (right_border() - 1, jw_top + 4,
                                l - left_border(), h - bottom_border(), low,
                                med, hi);
          
          else
            screen->line (left_border() - 1, jw_top + 4, right_border() - 1,
                           jw_top + 4, low);
        }
      screen->rectangle (1, 1, 4, 4, wm->black ());
      screen->widget_bar (2, 2, 3, 3, hi, med, low);   // draws the 'close' button
    }
  if (_name && _name[0])
    {
      screen->bar (top_border(), 1,
                    top_border() + fnt->width () * strlen (_name) + 1,
                    top_border() - 2, med);
      fnt->put_string (screen, top_border() + 1, 1, _name, low);
    }
  screen->bar (x1 (), y1 (), x2 (), y2 (), backg);  // clear 'client' region
  inm->redraw ();
}

int jwindow::left_border()
{
    return frame_left();
}

int jwindow::right_border()
{
    return frame_right();
}

int jwindow::top_border()
{
    return wm->font()->height() + frame_top();
}

int jwindow::bottom_border()
{
    return frame_bottom();
}


ifield *input_manager::unlink(int id)     // unlinks ID from fields list and return the pointer to it
{ 
  for (ifield *i=first,*last=NULL;i;i=i->next)
  {
    if (i->id==id) 
    {
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

input_manager::~input_manager() 
{ ifield *i; 
  while (first) 
  { i=first; 
    first=first->next; 
    delete i; 
  } 
} 

void input_manager::clear_current()
{
    if(owner)
        screen = owner->screen;
    if(active)
        active->draw(0, screen);
    active = NULL;
}

void input_manager::handle_event(event &ev, jwindow *j)
{
  ifield *i,*in_area=NULL;
  int x1,y1,x2,y2;

  if(owner)
      screen = owner->screen;

  if (j)
  {
    ev.mouse_move.x-=j->x;
    ev.mouse_move.y-=j->y;
    cur=j;
  }

  if (!grab)
  {
    if ((ev.type==EV_MOUSE_BUTTON && ev.mouse_button==1) || ev.type==EV_MOUSE_MOVE)
    {
      for (i=first;i;i=i->next)
      {
	i->area(x1,y1,x2,y2);
	if (ev.mouse_move.x>=x1 && ev.mouse_move.y>=y1 &&
	    ev.mouse_move.x<=x2 && ev.mouse_move.y<=y2)
        in_area=i;
      }
      if (in_area!=active && (no_selections_allowed || (in_area && in_area->selectable())))
      {
	if (active)
          active->draw(0,screen);

	active=in_area; 

	if (active)
	  active->draw(1,screen);
      }
    } 
    if (ev.type==EV_KEY && ev.key==JK_TAB && active)
    { 
      active->draw(0,screen);
      do
      {
	active=active->next;
	if (!active) active=first;
      } while (active && !active->selectable());
      active->draw(1,screen);
    }
  } else active=grab;

  if (active)
  {
    if (ev.type!=EV_MOUSE_MOVE && ev.type!=EV_MOUSE_BUTTON)
      active->handle_event(ev,screen,this);
    else
    {
      active->area(x1,y1,x2,y2);
      if (grab || (ev.mouse_move.x>=x1 && ev.mouse_move.y>=y1 &&
          ev.mouse_move.x<=x2 && ev.mouse_move.y<=y2))
      {
	if (j)
	  active->handle_event(ev,screen,j->inm);
	else active->handle_event(ev,screen,this);
      }
    }
  }

  if (j)
  {
    ev.mouse_move.x+=j->x;
    ev.mouse_move.y+=j->y;
  }
}

void input_manager::allow_no_selections()
{
  no_selections_allowed=1;
}

void input_manager::redraw()
{
    ifield *i;
    if(owner)
        screen = owner->screen;
    for(i = first; i; i = i->next)
        i->draw_first(screen);
    if(active)
        active->draw(1, screen);
}

input_manager::input_manager(image *Screen, ifield *First)
{
    no_selections_allowed = 0;
    cur = NULL;
    grab = NULL;
    owner = NULL;
    screen = Screen;
    active = first = First;
    while(active && !active->selectable())
        active = active->next;
    if(screen)
        redraw();
}

input_manager::input_manager(jwindow *Owner, ifield *First)
{
    no_selections_allowed = 0;
    cur = NULL;
    grab = NULL;
    owner = Owner;
    screen = NULL;
    active = first = First;
    while(active && !active->selectable())
        active = active->next;
}

void input_manager::grab_focus(ifield *i)
{ grab=i; 
  if (cur)
    wm->grab_focus(cur);
}

void input_manager::release_focus()
{ grab=NULL; 
  if (cur)
    wm->release_focus();
}

void input_manager::remap(filter *f)
{
  for (ifield *i=first;i;i=i->next)
   i->remap(f);
  redraw();
}

void input_manager::add(ifield *i) 
{ ifield *f=first;
  if (i->selectable())
  {
    if (!f)
      first=i;
    else
    {
      while (f->next) f=f->next;
      f->next=i; 
    }
  }
}

ifield *input_manager::get(int id)
{
  ifield *f;
  for (f=first;f;f=f->next)
  {
    ifield *ret=f->find(id);
    if (ret) return ret;
  }
  return NULL;
}

ifield::ifield()
{
    owner = NULL;
    x = 0;
    y = 0;
    next = NULL;
    id = 0;
}

ifield::~ifield()
{
    ;
}

/* re-position the control with respect to the "client" area of the window */
void ifield::set_owner(jwindow * newowner)
{
    if(owner)
        move(x - owner->x1(), y - owner->y1());
    owner = newowner;
    if(owner)
        move(x + owner->x1(), y + owner->y1());
}

