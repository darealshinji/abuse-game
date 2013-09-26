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

#include "imlib/scroller.h"

#include "devsel.h"
#include "cache.h"
#include "game.h"

void scale_put(AImage *im, AImage *screen, int x, int y, short new_width, short new_height);
void scale_put_trans(AImage *im, AImage *screen, int x, int y, short new_width, short new_height);
int cur_bg=0,cur_fg=0,cur_char=0;

void ATilePicker::recenter(AImage *screen)
{
  set_x(get_current(), screen);
}

int ATilePicker::picw()
{
  switch (type)
  {
    case SPEC_FORETILE :
    {
      return cache.foret(foretiles[0])->im->Size().x/scale;
    } break;
    case SPEC_BACKTILE :
    {
      return cache.backt(backtiles[0])->im->Size().x/scale;
    } break;
    default :
      return 30/scale;
  }
}

int ATilePicker::pich()
{
  switch (type)
  {
    case SPEC_FORETILE :
    {
      return cache.foret(foretiles[0])->im->Size().y/scale;
    } break;
    case SPEC_BACKTILE :
    {
      return cache.backt(backtiles[0])->im->Size().y/scale;
    } break;
    default :
      return 40/scale;
  }
}

int ATilePicker::total()
{
  switch (type)
  {
    case SPEC_FORETILE :
    { return nforetiles; } break;
    case SPEC_BACKTILE :
    { return nbacktiles; } break;
    case SPEC_CHARACTER :
    { return total_objects; } break;
  }
  return 1;
}

ATilePicker::ATilePicker(ivec2 pos, int id, int spec_type,
             int Scale, int scroll_h, int Wid)
  : AScroller(pos, id, ivec2(2, 2), 1, 0)
{
  wid=Wid;
  type=spec_type;
  th=scroll_h;
  scale=Scale;
  SetSize(ivec2(picw() * wid, pich() * th));
  sx=get_current();
  t=total();
}


void ATilePicker::scroll_event(int newx, AImage *screen)
{
  int ya = pich(), xw = picw(), c = get_current();
  AImage im(ivec2(xw, ya));
  last_sel=newx;

  screen->Bar(m_pos, m_pos + m_size - ivec2(1, 1), wm->black());
  for (int i=newx; i<newx+th*wid; i++)
  {
    ivec2 xyo = m_pos + ivec2(((i - newx) % wid) * xw, ((i - newx) / wid) * ya);

      int blank=0;
      if (i<t)
      {
    switch (type)
    {
      case SPEC_FORETILE :
      {
        if (foretiles[i]<0) blank=1;
        else
        {
          im.clear();
          the_game->get_fg(i)->im->PutImage(&im,ivec2(0,0));

          if (rev)
          {
        screen->Bar(xyo, xyo + ivec2(xw - 1, ya - 1), wm->bright_color());
        scale_put_trans(&im,screen,xyo.x,xyo.y,xw,ya);
          }
          else scale_put(&im,screen,xyo.x,xyo.y,xw,ya);
        }
      } break;
      case SPEC_BACKTILE :
      {
        if (backtiles[i]<0) blank=1;
        else
          scale_put(the_game->get_bg(i)->im,screen,m_pos.x,m_pos.y,xw,ya);

      } break;
      case SPEC_CHARACTER :
      {
        figures[i]->get_sequence(stopped)->get_figure(0)->forward->PutImage(&im,ivec2(0,0));
        scale_put(&im,screen,m_pos.x,m_pos.y,xw,ya);
      } break;
    }
      } else blank=1;

      if (i==c)
        screen->Rectangle(m_pos, m_pos + ivec2(xw - 1, ya - 1),
                          wm->bright_color());


  }
}


void ATilePicker::handle_inside_event(Event &ev, AImage *screen, InputManager *inm)
{
  if (ev.type==EV_MOUSE_BUTTON)
  {
    int sel=((ev.mouse_move.y-m_pos.y)/pich()*wid)+(ev.mouse_move.x-m_pos.x)/picw()+last_sel;
    if (sel<t && sel>=0 && sel!=get_current())
    {
      set_current(sel);
      scroll_event(last_sel, screen);
    }
  }
}




int ATilePicker::get_current()
{
  switch (type)
  {
    case SPEC_FORETILE :
    { return cur_fg; } break;
    case SPEC_BACKTILE :
    { return cur_bg; } break;
    case SPEC_CHARACTER :
    { return cur_char; } break;
  }
  return 0;
}

void ATilePicker::set_current(int x)
{
  switch (type)
  {
    case SPEC_FORETILE :
    { cur_fg=x; } break;
    case SPEC_BACKTILE :
    { cur_bg=x; } break;
    case SPEC_CHARACTER :
    { cur_char=x; } break;
  }
}





