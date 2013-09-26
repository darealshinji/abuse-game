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

#define HS_ICON_W 10
#define HS_ICON_H 8

uint8_t const hs_left_arrow[10*8]={
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 2, 0, 1, 1, 2, 2, 2, 2, 2, 2, 2,
    0, 0, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    2, 0, 0, 0, 0};


uint8_t const hs_right_arrow[10*8]={
    0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0,
    0, 0, 0, 0, 0, 2, 1, 1, 0, 0, 0, 0, 0, 0, 2,
    1, 1, 0, 0, 0};


uint8_t const vs_up_arrow[8*10]={
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 0,
    0, 0, 0, 1, 1, 1, 1, 2, 0, 0, 1, 2, 1, 1, 2,
    1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 0, 0, 1, 1,
    2, 0, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 0, 0, 1,
    1, 2, 0, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 0, 0,
    0, 2, 2, 0, 0};


uint8_t const vs_down_arrow[8*10]={
    0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 1, 1, 2, 0,
    0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 0, 0, 1, 1, 2,
    0, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 0, 0, 1, 1,
    2, 0, 0, 0, 1, 2, 1, 1, 2, 1, 2, 0, 0, 1, 1,
    1, 1, 2, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0};

static void show_icon(AImage *screen, ivec2 pos, ivec2 size, uint8_t const *buf)
{
    ivec2 caa, cbb;
    screen->GetClip(caa, cbb);

    uint8_t remap[3];
    remap[0] = wm->medium_color();
    remap[1] = wm->bright_color();
    remap[2] = wm->dark_color();

    for (int yc = size.y; yc; yc--, pos.y++)
    {
        if (pos.y >= caa.y && pos.y < cbb.y)
        {
            uint8_t *sl = screen->scan_line(pos.y) + pos.x;
            for (int xc = size.x, xo = pos.x; xc; xc--, xo++, sl++, buf++)
            {
                if (xo >= caa.x && xo < cbb.x)
                    *sl = remap[*buf];
            }
        }
    }
    screen->AddDirty(pos, pos + size);
}

AScroller::AScroller(ivec2 pos, int id, ivec2 size, int Vert, int Total_items)
  : AWidget(pos, id),
    m_size(size)
{
    sx = 0;
    t = Total_items;
    drag = -1;
    vert = Vert;
}

ibox2 AScroller::GetArea()
{
    area_config();
    return ibox2(m_pos - ivec2(1),
                 m_pos + m_size + (vert ? ivec2(bw(), 0) : ivec2(0, bh())));
}

void AScroller::dragger_area(int &x1, int &y1, int &x2, int &y2)
{
  if (vert)
  { x1=m_pos.x+m_size.x; y1=m_pos.y+bh(); x2=m_pos.x+m_size.x+bw()-1; y2=m_pos.y+m_size.y-bh()-1; }
  else { x1=m_pos.x+bw(); y1=m_pos.y+m_size.y; x2=m_pos.x+m_size.x-bw(); y2=m_pos.y+m_size.y+bh()-1; }
}

int AScroller::bh() const { return vert ? 15 : 13; }
int AScroller::bw() const { return vert ? 12 : 14; }

uint8_t const *AScroller::GetIcon(int index)
{
    if (index == 0)
        return vert ? vs_up_arrow : hs_left_arrow;
    else
        return vert ? vs_down_arrow : hs_right_arrow;
}

void AScroller::draw_first(AImage *screen)
{
    sx = lol::min(sx, t - 1);
    Draw(0, screen);
    screen->WidgetBar(b1(), b1() + ivec2(bw() - 1, bh() - 1),
                      wm->bright_color(), wm->medium_color(), wm->dark_color());
    screen->WidgetBar(b2(), b2() + ivec2(bw() - 1, bh() - 1),
                      wm->bright_color(), wm->medium_color(), wm->dark_color());
    show_icon(screen, b1() + ivec2(2), ivec2(bw() - 4, bh() - 4), GetIcon(0));
    show_icon(screen, b2() + ivec2(2), ivec2(bw() - 4, bh() - 4), GetIcon(1));

    int x1, y1, x2, y2;
    dragger_area(x1,y1,x2,y2);
    screen->Bar(ivec2(x1, y1), ivec2(x2, y2), wm->black());
    screen->Bar(ivec2(x1 + 1, y1 + 1), ivec2(x2 - 1, y2 - 1), wm->medium_color());
    draw_widget(screen, 0);
    scroll_event(sx, screen);
}

void AScroller::wig_area(int &x1, int &y1, int &x2, int &y2)
{
  int sx1,sy1,sx2,sy2;
  dragger_area(sx1,sy1,sx2,sy2);
  if (vert)
  {
    x1=m_pos.x+m_size.x+1;
    if (t<2)
      y1=m_pos.y+bh()+1;
    else
      y1=m_pos.y+bh()+1+sx*(sy2-sy1+1-bh())/(t-1);
  } else
  {
    if (t<2) x1=m_pos.x+bw()+1;
    else x1=m_pos.x+bw()+1+sx*(sx2-sx1+1-bw())/(t-1);
    y1=m_pos.y+m_size.y+1;
  }
  x2=x1+bw()-3;
  y2=y1+bh()-3;

}

void AScroller::draw_widget(AImage *screen, int erase)
{
  int x1,y1,x2,y2;
  wig_area(x1,y1,x2,y2);
  if (erase)
    screen->Bar(ivec2(x1, y1), ivec2(x2, y2), wm->medium_color());
  else
    screen->WidgetBar(ivec2(x1, y1), ivec2(x2, y2), wm->bright_color(),
                      wm->medium_color(), wm->dark_color());
}

void AScroller::Draw(int active, AImage *screen)
{
    ibox2 area = GetArea();
    screen->Rectangle(area.A, area.B,
                      active ? wm->bright_color() : wm->dark_color());
}

void AScroller::handle_event(Event &ev, AImage *screen, InputManager *inm)
{
    ivec2 mouse = ev.mouse_move;

  switch (ev.type)
  {
    case EV_MOUSE_BUTTON:
    {
      if (ev.mouse_button && drag==-1)
      {
        if (mouse.x >= b1().x && mouse.x < b1().x+bw() && mouse.y >= b1().y -2 && mouse.y < b1().y +bh())
        {
          if (sx>0)
          {
            draw_widget(screen,1);
            sx--;
            draw_widget(screen,0);
            scroll_event(sx,screen);
          }
        } else if (mouse.x >= b2().x && mouse.x < b2().x+bw() && mouse.y >= b2().y && mouse.y <= b2().y + bh())
        {
          if (sx<t-1)
          {
            draw_widget(screen,1);
            sx++;
            draw_widget(screen,0);
            scroll_event(sx,screen);
          }
        }
        else
        {
          int dx1,dy1,dx2,dy2;
          dragger_area(dx1,dy1,dx2,dy2);
          if (mouse.x >=dx1 && mouse.x <=dx2 && mouse.y >=dy1 && mouse.y <=dy2)
          {
            int x1,y1,x2,y2;
            wig_area(x1,y1,x2,y2);
            if (mouse.x >=x1 && mouse.x <=x2 && mouse.y >=y1 && mouse.y <=y2)
            {
              drag=sx;
              inm->grab_focus(this);
            }
            else if (t>1)
            {
              int nx = mouse_to_drag(mouse);
              if (nx!=sx && nx>=0 && nx<t)
              {
                draw_widget(screen,1);
                sx=nx;
                draw_widget(screen,0);
                scroll_event(sx,screen);
              }
            }
          } else handle_inside_event(ev,screen,inm);
        }
      } else if (!ev.mouse_button && drag!=-1)
      {
        inm->release_focus();
        drag=-1;
      }
    } break;

    case EV_MOUSE_MOVE :
    {
      if (drag!=-1)
      {
        int nx = mouse_to_drag(mouse);
        if (nx<0) nx=0; else if (nx>=t) nx=t-1;
        if (nx!=sx)
        {
          draw_widget(screen,1);
          sx=nx;
          draw_widget(screen,0);
          scroll_event(sx,screen);
        }
      } else if ( activate_on_mouse_move())
      {
        int x1,y1,x2,y2;
        wig_area(x1,y1,x2,y2);
        if (mouse.x >= m_pos.x && mouse.x <= m_pos.x + m_size.x - 1
             && mouse.y >= m_pos.y && mouse.y <= m_pos.y + m_size.y - 1)
          handle_inside_event(ev, screen, inm);
      }

    } break;
    case EV_KEY :
    {
      switch (ev.key)
      {
        case JK_LEFT :
        { handle_left(screen,inm); } break;
        case JK_RIGHT :
        { handle_right(screen,inm); } break;
        case JK_UP :
        { handle_up(screen,inm); } break;
        case JK_DOWN :
        { handle_down(screen,inm); } break;

        default :
          handle_inside_event(ev,screen,inm);
      }
    } break;
  }
}


void AScroller::handle_right(AImage *screen, InputManager *inm)
{
  UNUSED(inm);

  if (!vert && sx<t-1)
  {
    draw_widget(screen,1);
    sx++;
    draw_widget(screen,0);
    scroll_event(sx,screen);
  }
}

void AScroller::handle_left(AImage *screen, InputManager *inm)
{
  UNUSED(inm);

  if (!vert && sx>1)
  {
    draw_widget(screen,1);
    sx--;
    draw_widget(screen,0);
    scroll_event(sx,screen);
  }
}

void AScroller::handle_up(AImage *screen, InputManager *inm)
{
  UNUSED(inm);

  if (vert && sx>1)
  {
    draw_widget(screen,1);
    sx--;
    draw_widget(screen,0);
    scroll_event(sx,screen);
  }
}

void AScroller::handle_down(AImage *screen, InputManager *inm)
{
  UNUSED(inm);

  if (vert && sx<t-1)
  {
    draw_widget(screen,1);
    sx++;
    draw_widget(screen,0);
    scroll_event(sx,screen);
  }
}

void AScroller::set_x (int x, AImage *screen)
{
  if (x<0) x=0;
  if (x>=t) x=t-1;
  if (x!=sx)
  {
    draw_widget(screen,1);
    sx=x;
    draw_widget(screen,0);
    scroll_event(sx,screen);
  }
}

int AScroller::mouse_to_drag(ivec2 pos)
{
  int x1, y1, x2, y2;
  dragger_area(x1, y1, x2, y2);

  if (vert)
  {
    int h=(y2-y1+1-bh());
    if (h)
      return (pos.y - m_pos.y - bh() - bh() / 2) * (t - 1) / h;
    return 0;
  }
  else
  {
    int w=(x2-x1+1-bw());
    if (w)
      return (pos.x - m_pos.x - bw() - bw()/2)*(t-1)/w;
    return 0;
  }
}


void AScroller::scroll_event(int newx, AImage *screen)
{
  screen->Bar(m_pos, m_pos + m_size - ivec2(1), wm->black());
  int xa,ya,xo=0,yo;
  if (vert) { xa=0; ya=30; yo=m_pos.x+5; yo=m_pos.y+5; } else { xa=30; ya=0; xo=m_pos.x+5; yo=m_pos.y+5; }
  for (int i=newx,c=0; c<30 && i<100; i++,c++)
  {
    char st[10];
    sprintf(st,"%d",i);
    wm->font()->PutString(screen, ivec2(xo, yo), st, wm->bright_color());
    xo+=xa; yo+=ya;
  }
}

void APickList::area_config()
{
    m_size.x = wid * wm->font()->Size().x;
    m_size.y = th * (wm->font()->Size().y + 1);
}

int lis_sort(void const *a, void const *b)
{
    APickListItem const *a1 = (APickListItem const *)a;
    APickListItem const *a2 = (APickListItem const *)b;
    return strcmp(a1->name, a2->name);
}

APickList::APickList(ivec2 pos, int id, int height, char **List, int num_entries,
                     int start_yoffset, AImage *texture)
     : AScroller(pos, id, ivec2(2, 2), 1, 0)
{
  th=height;
  tex=texture;
  t=num_entries;
  wid=0;
  key_hist_total=0;
  lis=(APickListItem *)malloc(sizeof(APickListItem)*num_entries);
  int i=0;
  for (; i<num_entries; i++)
  {
    lis[i].name=List[i];
    lis[i].number=i;
  }
  qsort((void *)lis,num_entries,sizeof(APickListItem),lis_sort);

  for (i=0; i<t; i++)
    if ((int)strlen(List[i])>wid)
      wid=strlen(List[i]);
  cur_sel=sx=start_yoffset;
}

void APickList::handle_inside_event(Event &ev, AImage *screen, InputManager *inm)
{
  if (ev.type==EV_MOUSE_MOVE && activate_on_mouse_move())
  {
    int sel=last_sel+(ev.mouse_move.y-m_pos.y)/(wm->font()->Size().y+1);
    if (sel!=cur_sel && sel<t && sel>=0)
    {
      cur_sel=sel;
      scroll_event(last_sel,screen);
    }
  }
  else if (ev.type==EV_MOUSE_BUTTON)
  {
    int sel=last_sel+(ev.mouse_move.y-m_pos.y)/(wm->font()->Size().y+1);
    if (sel<t && sel>=0)
    {
      if (sel==cur_sel)
      wm->Push(Event(m_id, (char *)this));
      else
      {
    cur_sel=sel;
    scroll_event(last_sel,screen);
      }
    }
  } else if (ev.type==EV_KEY && ev.key==JK_ENTER)
    wm->Push(Event(m_id, (char *)this));
  else if (ev.type==EV_KEY)
  {
    int found=-1;
    if (key_hist_total<20)
      key_hist[(int)(key_hist_total++)]=ev.key;

    for (int i=0; i<t && found==-1; i++)
    {
      if ((int)strlen(lis[i].name)>=key_hist_total && memcmp(lis[i].name,key_hist,key_hist_total)==0)
    found=i;
    }
    if (found!=-1)
    {
      sx=found;
      cur_sel=found;
      scroll_event(sx,screen);
    } else key_hist_total=0;
  }
}

void APickList::handle_up(AImage *screen, InputManager *inm)
{
  if (cur_sel>0)
    cur_sel--;
  else return ;
  if (cur_sel<sx)
  {
    draw_widget(screen,1);
    sx=cur_sel;
    draw_widget(screen,0);
  }
  scroll_event(sx,screen);
}

void APickList::handle_down(AImage *screen, InputManager *inm)
{
  if (cur_sel<t-1)
    cur_sel++;
  else return ;
  if (cur_sel>sx+th-1)
  {
    draw_widget(screen,1);
    sx=cur_sel-th+1;
    draw_widget(screen,0);
  }
  scroll_event(sx,screen);
}

void APickList::scroll_event(int newx, AImage *screen)
{
  last_sel=newx;
  if (tex)
  {
    ivec2 caa, cbb;
    screen->GetClip(caa, cbb);
    screen->SetClip(m_pos, m_pos + m_size);
    int tw=(m_size.x+tex->Size().x-1)/tex->Size().x;
    int th=(m_size.y+tex->Size().y-1)/tex->Size().y;
    int dy=m_pos.y;
    for (int j=0; j<th; j++,dy+=tex->Size().y)
      for (int i=0,dx=m_pos.x; i<tw; i++,dx+=tex->Size().x)
        screen->PutImage(tex, ivec2(dx, dy));

    screen->SetClip(caa, cbb);
  } else screen->Bar(m_pos, m_pos + m_size - ivec2(1), wm->black());

  int dy=m_pos.y;
  for (int i=0; i<th; i++,dy+=wm->font()->Size().y+1)
  {
    if (i+newx==cur_sel)
      screen->Bar(ivec2(m_pos.x, dy), ivec2(m_pos.x + wid * wm->font()->Size().x - 1,
                                      dy + wm->font()->Size().y),
                  wm->dark_color());
    if (i+newx<t)
      wm->font()->PutString(screen, ivec2(m_pos.x, dy), lis[i+newx].name,
                            wm->bright_color());
  }
}


AScrollPicker::AScrollPicker(ivec2 pos, int id, int rows, int cols, int Vert, int MultiSelect)
  : AScroller(pos, id, ivec2(2, 2), Vert, 0),
    m_rows(rows),
    m_cols(cols)
{
  m_size.x = -1; /* XXX: we override AScroller */
  last_click = -1;
  m = MultiSelect;
  select = NULL;
}

void AScrollPicker::set_select(int x, int on)
{
  if (m)
  {
    if (on)
      select[x/8]|=1<<(x&7);
    else
      select[x/8]&=(0xff-(1<<(x&7)));
  } else cur_sel=x;
}

int AScrollPicker::get_select(int x)
{
  if (m)
    return select[x/8]&(1<<(x&7));
  else return (x==cur_sel);
}

int AScrollPicker::first_selected()
{
  if (m)
  {
    for (int i=0; i<t; i++)
      if (get_select(i)) return i;
    return -1;
  } else return cur_sel;
}

void AScrollPicker::reconfigure()
{
  if (select)
    free(select);
  select=NULL;


  t=total();
  if (sx>t)
    sx=t-1;
  if (m)
  {
    select=(uint8_t *)malloc((t+7)/8);
    memset(select,0,(t+7)/8);
  } else cur_sel=0;
}

void AScrollPicker::draw_background(AImage *screen)
{
    screen->Bar(m_pos, m_pos + m_size - ivec2(1), wm->dark_color());
}


void AScrollPicker::area_config()
{
    m_size.x = item_width() * (vert ? 1 : m_cols) + 4;
    m_size.y = item_height() * (vert ? m_rows : 1) + 4;
}

void AScrollPicker::set_x(int x, AImage *screen)
{
    cur_sel = x;
    sx = x;
    scroll_event(x, screen);
}


void AScrollPicker::scroll_event(int newx, AImage *screen)
{
    ivec2 pos = m_pos + ivec2(2);
    ivec2 step = vert ? ivec2(0, item_height())
                      : ivec2(item_width(), 0);

    last_sel = newx;
    draw_background(screen);

    for (int i = newx; i < newx + vis(); i++)
    {
        if (i < t)
        {
            if (m)
                DrawItem(screen, pos, i, get_select(i));
            else
                DrawItem(screen, pos, i, i == cur_sel);
        }
        pos += step;
    }
}


void AScrollPicker::handle_inside_event(Event &ev, AImage *screen, InputManager *inm)
{
  switch (ev.type)
  {
    case EV_MOUSE_MOVE :
    {
      if (activate_on_mouse_move())
      {
    int me;
    if (vert)
      me=last_sel+(ev.mouse_move.y-m_pos.y)/item_height();
    else
      me=last_sel+(ev.mouse_move.x-m_pos.x)/item_width();
    if (me<t && me>=0)
    {
      if (cur_sel!=me)
      {
        cur_sel=me;
        scroll_event(last_sel,screen);
        note_new_current(screen,inm,me);
      }
    }
      }
    } break;
    case EV_MOUSE_BUTTON :
    {
      int me;
      if (vert)
    me=last_sel+(ev.mouse_move.y-m_pos.y)/item_height();
      else
    me=last_sel+(ev.mouse_move.x-m_pos.x)/item_width();
      if (me<t && me>=0)
      {
    if (m)
    {
      if (ev.mouse_button)
      {
        if (ok_to_select(me))
        {
          set_select(me,!get_select(me));
          scroll_event(last_sel,screen);
          inm->grab_focus(this);
        }
      } else last_click=-1;

    } else if (ok_to_select(me))
    {
      if (cur_sel==me)
        note_selection(screen,inm,me);
      else
      {
        cur_sel=me;
        scroll_event(last_sel,screen);
        note_new_current(screen,inm,me);
      }
    }
      }
    } break;
  }
}



void AScrollPicker::handle_up(AImage *screen, InputManager *inm)
{
  if (vert && cur_sel>0)
  {
    cur_sel--;

    if (cur_sel<sx)
    {
      draw_widget(screen,1);
      last_sel=sx=cur_sel;
      draw_widget(screen,0);
    }
    scroll_event(last_sel,screen);
    note_new_current(screen,inm,cur_sel);
  }
}

void AScrollPicker::handle_down(AImage *screen, InputManager *inm)
{
  if (vert && cur_sel<t-1)
    cur_sel++;
  else
    return;

  if (cur_sel>sx+m_rows-1)
  {
    draw_widget(screen,1);
    last_sel=sx=cur_sel-m_rows+1;
    draw_widget(screen,0);
  }
  scroll_event(sx,screen);
  note_new_current(screen,inm,cur_sel);
}

void AScrollPicker::handle_left(AImage *screen, InputManager *inm)
{
}

void AScrollPicker::handle_right(AImage *screen, InputManager *inm)
{
}




