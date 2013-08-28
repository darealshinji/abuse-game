/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include <string.h>

#include "common.h"

#include "video.h"
#include "image.h"
#include "input.h"
#include "event.h"
#include "filter.h"
#include "window.h"

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

void WindowManager::grab_focus(AWindow *j)
{
    m_grab = j;
}

void WindowManager::release_focus()
{
    m_grab = nullptr;
}

void WindowManager::close_window(AWindow *j)
{
    delete j;
}

void WindowManager::hide_windows()
{
    for (AWindow *p = m_first; p; p = p->next)
    {
        if (!p->is_hidden())
        {
            p->hide();
            m_surf->AddDirty(p->m_pos, p->m_pos + p->m_size);
        }
    }
}

void WindowManager::show_windows()
{
    for (AWindow *p = m_first; p; p = p->next)
        if (p->is_hidden())
            show_window(p);
}

void WindowManager::hide_window(AWindow *j)
{
    if (j == m_first)
        m_first = m_first->next;
    else
    {
        AWindow *k;
        for (k = m_first; k->next != j; k = k->next)
            k->m_surf->AddDirty(j->m_pos - k->m_pos,
                                j->m_pos - k->m_pos + j->m_size);
        k->m_surf->AddDirty(j->m_pos - k->m_pos,
                            j->m_pos - k->m_pos + j->m_size);
        k->next = j->next;
    }
    m_surf->AddDirty(j->m_pos, j->m_pos + j->m_size);
    j->hide();
}

void WindowManager::show_window(AWindow *j)
{
    if (j->is_hidden())
    {
        j->show();
        j->m_surf->AddDirty(ivec2(0), j->m_size);
    }
}

void WindowManager::get_event(Event &ev)
{
    Get(ev);

    if (ev.type == EV_KEY)
        key_state[ev.key] = 1;
    else if (ev.type == EV_KEYRELEASE)
        key_state[ev.key] = 0;

    if (state == inputing)
    {
        AWindow *j;
        for (ev.window = NULL, j = m_first; j; j = j->next)
            if (!j->is_hidden() && ev.mouse_move >= j->m_pos
                                && ev.mouse_move < j->m_pos + j->m_size)
                ev.window = j;

        if (!ev.window && m_grab)
            ev.window = m_grab;

        if (ev.window)
        {
            int closew = 0, movew = 0;

            if ((ev.type == EV_MOUSE_BUTTON && ev.mouse_button == 1 && ev.window &&
                ev.mouse_move >= ev.window->m_pos &&
                ev.mouse_move < ev.window->m_pos + ivec2(ev.window->m_size.x, ev.window->y1())))
            {
                if (ev.mouse_move.x - ev.window->m_pos.x < 11)
                    closew = 1;
                else if (ev.window->is_moveable())
                    movew = 1;
            }
            else if (m_grab)
                ev.window = m_grab;

            if (ev.type == EV_KEY && ev.key == JK_ESC)
                closew = 1;

            if (closew)
                ev.type = EV_CLOSE_WINDOW;
            else if (movew)
            {
                int red = 0;
                if (ev.window == m_first)       // see if we need to raise the window
                {
                    m_first = m_first->next;
                    if (m_first)
                        red = 1;
                }
                else
                {
                    AWindow *last = m_first;
                    for (; last->next != ev.window; last = last->next)
                        ;
                    if (ev.window->next)
                        red = 1;
                    last->next = ev.window->next;
                }
                if (!m_first)
                    m_first = ev.window;
                else
                {
                    AWindow *last = m_first;
                    for (; last->next; last=last->next)
                        ;
                    last->next = ev.window;
                }
                ev.window->next = nullptr;
                if (red)
                {
                    AWindow *j = ev.window;
/*      m_surf->AddDirty(j->x,j->y,j->x+j->l,j->y+j->h);
      for (p=m_first; p!=j; p=p->next)
        p->m_surf->AddDirty(j->x-p->x,j->y-p->y,j->x+j->l-p->x,j->y+j->h-p->y); */
                    j->m_surf->AddDirty(ivec2(0), j->m_size);
                    flush_screen();
                }

                state = dragging;
                drag_window = ev.window;
                drag_mousex = ev.window->m_pos.x - ev.mouse_move.x;
                drag_mousey = ev.window->m_pos.y - ev.mouse_move.y;
                ev.type = EV_SPURIOUS;
            }
            else if (ev.window)
                ev.window->inm->handle_event(ev, ev.window);
        }
    }
    else if (state == dragging)
    {
        ev.window = drag_window;
        if (ev.type == EV_MOUSE_BUTTON && ev.mouse_button == 0)  // user released the mouse
        {
          state = inputing;
          ev.type = EV_SPURIOUS;
        }
        else if (ev.type == EV_MOUSE_MOVE)
        {
           move_window(drag_window,
                       ev.mouse_move.x + drag_mousex,
                       ev.mouse_move.y + drag_mousey);
           flush_screen();
           ev.type = EV_DRAG_WINDOW;
           ev.window_position.x = ev.mouse_move.x + drag_mousex;
           ev.window_position.y = ev.mouse_move.y + drag_mousey;
        }
    }
}

void AWindow::Resize(ivec2 size)
{
    m_surf->SetSize(size);
    m_size = size;
}

void WindowManager::resize_window(AWindow *j, int l, int h)
{
    m_surf->AddDirty(j->m_pos, j->m_pos + j->m_size);
    for (AWindow *p = m_first; p != j; p = p->next)
        p->m_surf->AddDirty(j->m_pos - p->m_pos,
                            j->m_pos - p->m_pos + j->m_size);
    j->Resize(ivec2(l, h));
    if (!frame_suppress)
        j->redraw();
}

void WindowManager::move_window(AWindow *j, int x, int y)
{
    m_surf->AddDirty(j->m_pos, j->m_pos + j->m_size);
    for(AWindow *p = m_first; p != j; p = p->next)
        p->m_surf->AddDirty(j->m_pos - p->m_pos,
                            j->m_pos - p->m_pos + j->m_size);
    j->m_pos.x = x;
    j->m_pos.y = y;
    j->m_surf->AddDirty(ivec2(0), j->m_size);
}

WindowManager::WindowManager(AImage *screen, Palette *pal, int Hi,
                             int Med, int Low, JCFont *Font)
  : EventHandler(screen, pal)
{
    wm = this;
    m_surf = screen;
    hi = Hi; low = Low; med = Med; m_first = NULL; m_pal = pal; m_grab = NULL;
    bk = pal->FindClosest(u8vec3(0));
    state = inputing; fnt = Font;  wframe_fnt = Font;
    memset(key_state, 0, sizeof(key_state));
    frame_suppress = 0;
}

WindowManager::~WindowManager()
{
    while(m_first)
        close_window(m_first);
    wm = NULL;
}

void WindowManager::add_window(AWindow *win)
{
    if (!m_first)
        m_first = win;
    else
    {
        AWindow *tmp = m_first;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = win;
        win->next = nullptr;
    }
}

void WindowManager::remove_window(AWindow *win)
{
    if (m_grab == win)
        m_grab = nullptr;

    // close the window we were dragging
    if(state == dragging && win == drag_window)
        state = inputing;

    if(m_first == win)
        m_first = m_first->next;
    else
    {
        AWindow * search;
        for(search = m_first; search->next != win; search = search->next)
            search->m_surf->AddDirty(win->m_pos - search->m_pos,
                                     win->m_pos - search->m_pos + win->m_size);
        search->m_surf->AddDirty(win->m_pos - search->m_pos,
                                 win->m_pos - search->m_pos + win->m_size);
        search->next = win->next;
    }

    m_surf->AddDirty(win->m_pos, win->m_pos + win->m_size);
}

AWindow * WindowManager::CreateWindow(ivec2 pos, ivec2 size,
                                      String const &name,
                                      AWidgetList const &widgets)
{
    if(pos.x > m_surf->Size().x - 4)
        pos.x = m_surf->Size().x - 25;
    if(pos.y > m_surf->Size().y - 4)
        pos.y = m_surf->Size().y - 10;

    AWindow * j = new AWindow(pos, size, name, widgets);
    j->show();

    return j;
}

void WindowManager::flush_screen()
{
    ivec2 m1(0, 0);

    if (has_mouse())
    {
        m1 = m_pos - m_center;
        ivec2 m2 = m1 + m_sprite->m_visual->Size();

        m_sprite->m_save->PutPart(m_surf, ivec2(0, 0), m1, m2);
        m_surf->PutImage(m_sprite->m_visual, m1, 1);
    }

    for (AWindow *p = m_first; p; p = p->next)
        if (!p->is_hidden())
            m_surf->DeleteDirty(p->m_pos, p->m_pos + p->m_size);
    update_dirty(m_surf);

    if (has_mouse())
        m_surf->PutImage(m_sprite->m_save, m1);

    for (AWindow *p = m_first; p; p = p->next)
    {
        if (p->is_hidden())
            continue;

        if (has_mouse())
        {
            m_sprite->m_save->PutPart(p->m_surf, ivec2(0, 0), m1 - p->m_pos,
                                      m1 - p->m_pos + m_sprite->m_visual->Size());
            p->m_surf->PutImage(m_sprite->m_visual, m1 - p->m_pos, 1);
        }

//      m_surf->DeleteDirty(p->m_pos, p->m_pos + p->m_size);
        for (AWindow *q = p->next; q; q = q->next)
            if (!q->is_hidden())
                p->m_surf->DeleteDirty(q->m_pos - p->m_pos,
                                       q->m_pos - p->m_pos + q->m_size);
        update_dirty(p->m_surf, p->m_pos.x, p->m_pos.y);
        if (has_mouse())
            p->m_surf->PutImage(m_sprite->m_save, m1 - p->m_pos, 0);
    }
}

AWindow::AWindow(String const &name)
{
    _x1 = left_border();
    _y1 = jw_top + 5;
    _x2 = _y2 = 0;

    _hidden = true;
    _moveable = true;
    // property.flags = JWINDOW_NOAUTOHIDE_FLAG;

    inm = new InputManager(this);
    reconfigure();

    m_surf = nullptr;
    next = nullptr;

    _name = strdup(name.C());
    wm->add_window(this);
}

AWindow::AWindow(ivec2 pos, ivec2 size, String const &name, AWidgetList const &widgets)
{
    m_size = ivec2(0);
    _hidden = false;
    _moveable = true;

    _x1 = left_border();
    _y1 = name.Count() ? top_border() : jw_top + 5;

    m_surf = nullptr;
    inm = new InputManager(m_surf, widgets);
    reconfigure(); /* FIXME: TODO */

    m_size.x = size.x >= 0 ? size.x + left_border() : m_size.x - size.x;
    m_size.y = size.y >= 0 ? size.y + top_border() : m_size.y - size.y;
    m_pos.x = pos.x >= 0 ? pos.x : xres - m_size.x + pos.x - left_border() - right_border() - 1;
    m_pos.y = pos.y >= 0 ? pos.y : yres - m_size.y + pos.y - top_border() - bottom_border() - 1;

    backg = wm->medium_color();

    _x2 = m_size.x - 1;
    _y2 = m_size.y - 1;
    m_size += ivec2(right_border(), bottom_border());

    if(size.x == -1)
        m_size.x = lol::max(m_size.x, 15 + left_border() + right_border());
    if(size.y == -1)
        m_size.y = lol::max(m_size.y, top_border() + bottom_border());
    m_surf = new AImage(m_size, 2);
    // Keep this from getting destroyed when image list is cleared
    image_list.Pop();
    m_surf->clear(backg);
    inm->m_surf = m_surf;

    next = nullptr;

    _name = strdup(name.C());

    wm->add_window(this);
    if(!wm->frame_suppress)
        redraw();
}

AWindow::~AWindow()
{
    wm->remove_window(this);
    local_close();
    delete m_surf;
    delete inm;
    if(_name)
        free(_name);
}

void AWindow::reconfigure()
{
    m_size = ivec2(2);

    for (int i = 0; i < inm->m_fields.Count(); ++i)
    {
        int x1, y1, x2, y2;
        inm->m_fields[i]->set_owner(this);
        inm->m_fields[i]->area(x1, y1, x2, y2);
        m_size = lol::max(m_size, ivec2(x2, y2));
    }
}

void AWindow::local_close()
{
    ;
}

void AWindow::redraw()
{
    int hi = wm->bright_color();
    int med = wm->medium_color();
    int low = wm->dark_color();
    JCFont * fnt = wm->frame_font();

    if(_name)
    {
        if (right_border() >= 1)
        {
            m_surf->WidgetBar(ivec2(0, 0), m_size - ivec2(1), hi, med, low);
            if (right_border() >= 3)
                m_surf->WidgetBar(ivec2(right_border() - 1, top_border() - 1),
                                  m_size - ivec2(left_border(), bottom_border()),
                                  low, med, hi);

          else
            m_surf->Line(ivec2(left_border() - 1, top_border() - 1),
                         ivec2(right_border() - 1, top_border() - 1), low);
        }
      m_surf->Rectangle(ivec2(2, 2), ivec2(top_border() - 2, top_border() - 3),
                        wm->black());
      m_surf->WidgetBar(ivec2(3, 3), ivec2(top_border() - 3, top_border() - 4),
                        hi, med, low); // draws the 'close' button
    }

  else
    {
      if (right_border() >= 1)
        {
          m_surf->WidgetBar(ivec2(0, 0), m_size - ivec2(1), hi, med, low);
          if (right_border() >= 3)
            m_surf->WidgetBar(ivec2(right_border() - 1, jw_top + 4),
                              m_size - ivec2(left_border(), bottom_border()),
                              low, med, hi);
          else
            m_surf->Line(ivec2(left_border() - 1, jw_top + 4),
                         ivec2(right_border() - 1, jw_top + 4), low);
        }
      // Draw the 'close' button
      m_surf->Rectangle(ivec2(1, 1), ivec2(4, 4), wm->black ());
      m_surf->WidgetBar(ivec2(2, 2), ivec2(3, 3), hi, med, low);
    }
  if (_name && _name[0])
    {
      m_surf->Bar(ivec2(top_border(), 1),
                  ivec2(top_border() + fnt->Size().x * strlen (_name) + 1,
                        top_border() - 2),
                  med);
      fnt->PutString(m_surf, ivec2(top_border() + 1, 1), _name, low);
    }
  // clear 'client' region
  m_surf->Bar(ivec2(x1(), y1()), ivec2(x2(), y2()), backg);
  inm->redraw ();
}

int AWindow::left_border()
{
    return frame_left();
}

int AWindow::right_border()
{
    return frame_right();
}

int AWindow::top_border()
{
    return wm->font()->Size().y + frame_top();
}

int AWindow::bottom_border()
{
    return frame_bottom();
}

// unlinks ID from fields list and return the pointer to it
AWidget *InputManager::unlink(int id)
{
    for (int i = 0; i < m_fields.Count(); ++i)
    {
        /* Does this item match the ID? */
        if (m_fields[i]->m_id == id)
        {
            AWidget *ret = m_fields[i];
            m_fields.Remove(i);

            if (m_active == i)
                m_active = m_fields.Count() ? 0 : -1;

            return ret;
        }

        /* Maybe the item's children match the ID */
        AWidget *child = m_fields[i]->unlink(id);
        if (child)
            return child;
    }

    return nullptr; // no such id
}

InputManager::~InputManager()
{
    for (int i = 0; i < m_fields.Count(); ++i)
        delete m_fields[i];
    m_fields.Empty();
}

void InputManager::clear_current()
{
    if (m_owner)
        m_surf = m_owner->m_surf;
    if (m_active >= 0)
        m_fields[m_active]->Draw(0, m_surf);
    m_active = -1;
}

void InputManager::handle_event(Event &ev, AWindow *j)
{
  int in_area = -1;
  int x1,y1,x2,y2;

  if(m_owner)
      m_surf = m_owner->m_surf;

  if (j)
  {
    ev.mouse_move -= j->m_pos;
    m_cur = j;
  }

  if (m_grab == -1)
  {
    if ((ev.type==EV_MOUSE_BUTTON && ev.mouse_button==1) || ev.type==EV_MOUSE_MOVE)
    {
      for (int i = 0; i < m_fields.Count(); ++i)
      {
        m_fields[i]->area(x1,y1,x2,y2);
        if (ev.mouse_move.x >= x1 && ev.mouse_move.y >= y1 &&
            ev.mouse_move.x <= x2 && ev.mouse_move.y <= y2)
            in_area = i;
      }
      if (in_area != m_active && (no_selections_allowed || (in_area >= 0 && m_fields[in_area]->selectable())))
      {
        if (m_active >= 0)
          m_fields[m_active]->Draw(0, m_surf);

        m_active = in_area;

        if (m_active >= 0)
          m_fields[m_active]->Draw(1, m_surf);
      }
    }
    if (ev.type == EV_KEY && ev.key == JK_TAB && m_active >= 0)
    {
      m_fields[m_active]->Draw(0, m_surf);
      do
      {
        ++m_active;
      } while (m_active < m_fields.Count() && !m_fields[m_active]->selectable());

      if (m_active >= m_fields.Count())
        m_active = -1;
      if (m_active >= 0)
        m_fields[m_active]->Draw(1, m_surf);
    }
  }
  else
    m_active = m_grab;

  if (m_active >= 0)
  {
    if (ev.type!=EV_MOUSE_MOVE && ev.type!=EV_MOUSE_BUTTON)
      m_fields[m_active]->handle_event(ev, m_surf, this);
    else
    {
      m_fields[m_active]->area(x1,y1,x2,y2);
      if (m_grab >= 0 || (ev.mouse_move.x>=x1 && ev.mouse_move.y>=y1 &&
          ev.mouse_move.x<=x2 && ev.mouse_move.y<=y2))
      {
        if (j)
          m_fields[m_active]->handle_event(ev,m_surf,j->inm);
        else
          m_fields[m_active]->handle_event(ev,m_surf,this);
      }
    }
  }

  if (j)
    ev.mouse_move += j->m_pos;
}

void InputManager::allow_no_selections()
{
    no_selections_allowed=1;
}

void InputManager::redraw()
{
    if (m_owner)
        m_surf = m_owner->m_surf;
    for (int i = 0; i < m_fields.Count(); ++i)
        m_fields[i]->draw_first(m_surf);
    if (m_active >= 0)
        m_fields[m_active]->Draw(1, m_surf);
}

InputManager::InputManager(AImage *screen, AWidgetList const &fields)
{
    no_selections_allowed = 0;
    m_cur = nullptr;
    m_grab = -1;
    m_owner = nullptr;
    m_surf = screen;
    m_fields = fields;
    m_active = -1;
    for (int i = 0; i < m_fields.Count(); ++i)
        if (m_fields[i]->selectable())
        {
            m_active = i;
            break;
        }

    if (m_surf)
        redraw();
}

InputManager::InputManager(AWindow *owner, AWidgetList const &fields)
{
    no_selections_allowed = 0;
    m_cur = nullptr;
    m_grab = -1;
    m_owner = owner;
    m_surf = nullptr;
    m_fields = fields;
    m_active = -1;
    for (int i = 0; i < m_fields.Count(); ++i)
        if (m_fields[i]->selectable())
        {
            m_active = i;
            break;
        }
}

void InputManager::grab_focus(AWidget *f)
{
    for (int i = 0; i < m_fields.Count(); ++i)
        if (f == m_fields[i])
            m_grab = i;
    if (m_cur)
        wm->grab_focus(m_cur);
}

void InputManager::release_focus()
{
    m_grab = -1;
    if (m_cur)
        wm->release_focus();
}

void InputManager::remap(Filter *f)
{
    for (int i = 0; i < m_fields.Count(); ++i)
        m_fields[i]->remap(f);
    redraw();
}

void InputManager::Add(AWidgetList const &fields)
{
    if (fields[0]->selectable())
        m_fields += fields;
}

AWidget *InputManager::get(int id)
{
    for (int i = 0; i < m_fields.Count(); ++i)
    {
        AWidget *ret = m_fields[i]->find(id);
        if (ret)
            return ret;
    }
    return nullptr;
}

AWidget::AWidget()
  : m_pos(0, 0),
    m_id(0),
    m_owner(nullptr)
{
}

AWidget::AWidget(ivec2 pos, int id)
  : m_pos(pos),
    m_id(id),
    m_owner(nullptr)
{
}

AWidget::~AWidget()
{
    ;
}

/* re-position the control with respect to the "client" area of the window */
void AWidget::set_owner(AWindow * owner)
{
    if (m_owner)
        Move(m_pos - ivec2(m_owner->x1(), m_owner->y1()));
    m_owner = owner;
    if (m_owner)
        Move(m_pos + ivec2(m_owner->x1(), m_owner->y1()));
}

