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

#include "common.h"

#include "cache.h"
#include "gui.h"
#include "dev.h"
#include "loader2.h"

void AIconButton::set_act_id(int id)
{
  activate_id=id;
}

AIconSwitchButton::AIconSwitchButton(ivec2 pos, int id, int start_on, AWidgetList const &buttons)
  : AWidget(pos, id)
{
  m_buttons = buttons;
  m_current = start_on < m_buttons.Count() ? start_on : -1;
  act = 0;
  for (int i = 0; i < m_buttons.Count(); ++i)
      m_buttons[i]->m_pos = m_pos;
}

void AIconSwitchButton::area(int &x1, int &y1, int &x2, int &y2)
{
  x1 = 10000;
  y1 = 10000;
  x2 = -10000;
  y2 = -10000;
  for (int i = 0; i < m_buttons.Count(); ++i)
  {
    int X1, Y1, X2, Y2;
    m_buttons[i]->area(X1, Y1, X2, Y2);
    x1 = lol::min(x1, X1);
    y1 = lol::min(y1, Y1);
    x2 = lol::max(x2, X2);
    y2 = lol::max(y2, Y2);
  }
  if (!m_buttons.Count())
  {
    x1 = x2 = m_pos.x;
    y1 = y2 = m_pos.y;
  }
}

AWidget *AIconSwitchButton::unlink(int id)
{
  for (int i = 0; i < m_buttons.Count(); ++i)
  {
    if (m_buttons[i]->m_id == id)
    {
      AWidget *ret = m_buttons[i];
      m_buttons.Remove(i);
      if (m_current >= m_buttons.Count())
        m_current = 0;
      return ret;
    }
    AWidget *x = m_buttons[i]->unlink(id);
    if (x)
      return x;
  }
  return nullptr;
}

void AIconSwitchButton::handle_event(Event &ev, AImage *screen, InputManager *im)
{
  if ((ev.type == EV_KEY && ev.key == 13)
       || (ev.type == EV_MOUSE_BUTTON && ev.mouse_button))
  {
    ++m_current;
    if (m_current >= m_buttons.Count())
      m_current = 0;
    m_buttons[m_current]->Draw(act, screen);
    m_buttons[m_current]->handle_event(ev, screen, im);
  }
}

void AIconButton::Draw(int active, AImage *screen)
{
    int x1, y1, x2, y2;
    area(x1, y1, x2, y2);

    if (active != act && activate_id != -1 && active)
        wm->Push(Event(activate_id, NULL));

    screen->PutImage(cache.img((up && !active) ? u :
                               (up && active) ? ua :
                               (!up && !active) ? d : da), ivec2(x1, y1));

    if (act != active && active && activate_id != -1)
        wm->Push(Event(activate_id, NULL));
    act = active;

    if (active && key[0])
    {
        int g = 80;
        screen->Bar(ivec2(0, 0), ivec2(144, 20), 0);
        wm->font()->PutString(screen, ivec2(0), symbol_str(key),
                              color_table->Lookup(u8vec3(g >> 3, g >> 3, g >> 3)));
    }
    else if (!active && key[0])
    {
        screen->Bar(ivec2(0, 0), ivec2(144, 20), 0);
    }
}

extern long S_BUTTON_PRESS_SND;
extern int sfx_volume;

void AIconButton::handle_event(Event &ev, AImage *screen, InputManager *im)
{
  if ((ev.type==EV_KEY && ev.key==13) || (ev.type==EV_MOUSE_BUTTON &&
                                         ev.mouse_button))
  {
    int  x1,y1,x2,y2;
    area(x1,y1,x2,y2);
    up=!up;
    Draw(act, screen);
    wm->Push(Event(m_id, (char *)this));
    if (S_BUTTON_PRESS_SND)
      cache.sfx(S_BUTTON_PRESS_SND)->play(sfx_volume);
  }
}

void AIconButton::area(int &x1, int &y1, int &x2, int &y2)
{
  x1=m_pos.x; y1=m_pos.y;
  x2=m_pos.x+cache.img(u)->Size().x-1;
  y2=m_pos.y+cache.img(u)->Size().y-1;
}

AIconButton::AIconButton(ivec2 pos, int id, int Up, int down, int upa, int downa, int act_id, char const *help_key)
  : AWidget(pos, id)
{
  if (help_key)
  {
    strncpy(key,help_key,15);
    key[15]=0;
  }
  else key[0]=0;

  up=1;
  u=Up; d=down;
  ua=upa; da=downa;
  activate_id=act_id;
  act = 0;
}

AIconSwitchButton::~AIconSwitchButton()
{
  for (int i = 0; i < m_buttons.Count(); ++i)
    delete m_buttons[i];
  m_buttons.Empty();
}

