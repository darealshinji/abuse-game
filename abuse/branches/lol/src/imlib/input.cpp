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

#include "input.h"

void AButton::remap(Filter *f)
{
    if (visual)
    {
        f->Apply(visual);
        if (pressed)
            f->Apply(pressed);
    }
}

void AButtonBox::press_button(int id)      // if button box doesn't contain id, nothing happens
{
}

void AButtonBox::remap(Filter *f)
{
    for (int i = 0; i < m_buttons.Count(); ++i)
        m_buttons[i]->remap(f);
}

AWidget *AButtonBox::find(int id)
{
    if (id == m_id)
        return this;

    for (int i = 0; i < m_buttons.Count(); ++i)
        if (id == m_buttons[i]->m_id)
            return m_buttons[i];

    return nullptr;
}

AButtonBox::AButtonBox(ivec2 pos, int id, int MaxDown, Array<AButton *> const &buttons)
{
    m_pos = pos;
    m_id = id;
    m_buttons = buttons;
    maxdown = MaxDown;
    if (m_buttons.Count() && maxdown)
        m_buttons[0]->push();  // the first button is automatically selected!
}

AButtonBox::~AButtonBox()
{
    for (int i = 0; i < m_buttons.Count(); ++i)
        delete m_buttons[i];
}

ibox2 AButtonBox::GetArea()
{
    ibox2 ret;
    for (int i = 0; i < m_buttons.Count(); ++i)
    {
        ibox2 area = m_buttons[i]->GetArea();
        if (i)
        {
            ret.A = lol::min(ret.A, area.A);
            ret.B = lol::max(ret.B, area.B);
        }
        else
            ret = area;
    }
    return ret;
}

void AButtonBox::draw_first(AImage *screen)
{
    for (int i = 0; i < m_buttons.Count(); ++i)
        m_buttons[i]->draw_first(screen);
}

void AButtonBox::Draw(int active, AImage *screen)
{
    return;
}

void AButtonBox::Move(ivec2 pos)
{
    for (int i = 0; i < m_buttons.Count(); ++i)
        m_buttons[i]->Move(pos + m_buttons[i]->m_pos);
    m_pos = pos;
}

char const *AButtonBox::read()
{
    for (int i = 0; i < m_buttons.Count(); ++i)
        if (*((int const *)m_buttons[i]->read()) == 0)
            return (char const *)m_buttons[i];

    return nullptr;
}

void AButtonBox::handle_event(Event &ev, AImage *screen, InputManager *im)
{
    switch (ev.type)
    {
    case EV_MOUSE_BUTTON:
        // see if the user clicked on a button
        for (int i = 0, found = 0; i < m_buttons.Count() && !found; ++i)
        {
            ibox2 area = m_buttons[i]->GetArea();
            if (ev.mouse_move >= area.A && ev.mouse_move <= area.B)
            {
                m_buttons[i]->handle_event(ev, screen, im);

                int total = 0;
                for (int j = 0; j < m_buttons.Count(); ++j)
                    if (*((int const *)m_buttons[j]->read()) == 0)
                        total++;

                if (*((int const *)m_buttons[i]->read()) == 0)  // did the user press or release the button
                {
                    for (int j = 0; j < m_buttons.Count() && total > maxdown; ++j)
                        if ((i != j || maxdown == 0) && *((int const *)m_buttons[j]->read()) == 0)
                        {
                            total--;
                            m_buttons[j]->push();
                            m_buttons[j]->draw_first(screen);
                        }
                    m_buttons[i]->draw_first(screen);
                }
                else if (total == 0 && maxdown)
                {
                    // don't let the user de-press a button if non others are selected.
                    m_buttons[i]->push();
                }

                found = 1; // don't look at anymore buttons

            }
        }
        break;
    }
}

void AButtonBox::add_button(AButton *b)
{
    m_buttons.Push(b);
}

void AButtonBox::arrange_left_right()
{
    ivec2 on = m_pos;
    for (int i = 0; i < m_buttons.Count(); ++i)
    {
        ibox2 area = m_buttons[i]->GetArea();
        m_buttons[i]->m_pos = on;
        on.x += (area.B.x - area.A.x + 1) + 1;
    }
}

void AButtonBox::arrange_up_down()
{
    ivec2 on = m_pos;
    for (int i = 0; i < m_buttons.Count(); ++i)
    {
        ibox2 area = m_buttons[i]->GetArea();
        m_buttons[i]->m_pos = on;
        on.y += (area.B.y - area.A.y + 1) + 1;
    }
}

void AButton::change_visual(AImage *new_visual)
{
    ASSERT(visual);
    visual = new_visual;
}

ibox2 AButton::GetArea()
{
    if (pressed)
        return ibox2(m_pos, m_pos + pressed->Size() - ivec2(1));

    if (m_text.Count())
        return ibox2(m_pos, m_pos + wm->font()->Size() * ivec2(m_text.Count(), 1) + ivec2(6));

    return ibox2(m_pos, m_pos + visual->Size() + ivec2(6));
}


AButton::AButton(ivec2 pos, int id, char const *text)
  : AWidget(pos, id),
    m_text(text)
{
    act_id = -1;
    up = 1;
    act = 0;
    visual = NULL;
    pressed = NULL;
}


AButton::AButton(ivec2 pos, int id, AImage *vis)
  : AWidget(pos, id)
{
    act_id = -1;
    visual = vis; up = 1; act = 0;
    pressed = NULL;
}

AButton::AButton(ivec2 pos, int id, AImage *Depressed, AImage *Pressed, AImage *active)
  : AWidget(pos, id)
{
    act_id = -1;
    visual = Depressed; up = 1; act = 0;
    pressed = Pressed;
    act_pict = active;
}


void ATextField::change_data(char const *new_data, int new_cursor, // cursor==-1, does not change it.
                 int active, AImage *screen)
{
  if (strlen(format)<strlen(new_data))
    data=(char *)realloc(data,strlen(new_data));

  strcpy(data,new_data);
  if (new_cursor!=-1)
    cur=new_cursor;
  draw_first(screen);
  Draw(active, screen);
}

char const *ATextField::read()
{
    while (*data && data[strlen(data) - 1] == ' ')
        data[strlen(data) - 1] = 0; /* Strip trailing spaces */
    return data;
}

void ATextField::handle_event(Event &ev, AImage *screen, InputManager *im)
{
  int xx;
  if (ev.type==EV_KEY)
  {
    switch (ev.key)
    {
      case JK_LEFT : if (cur) { draw_cur(wm->dark_color(),screen); cur--;
                           draw_cur(wm->bright_color(),screen); } break;
      case JK_RIGHT : if (cur<(int)strlen(format)-1) { draw_cur(wm->dark_color(),screen); cur++;
                           draw_cur(wm->bright_color(),screen); } break;
      case JK_END : if (cur!=last_spot())
                          { draw_cur(wm->dark_color(),screen); cur=last_spot();
                            if (cur==(int)strlen(format)-1) cur--;
                           draw_cur(wm->bright_color(),screen); } break;
      case JK_HOME : if (cur)
                          { draw_cur(wm->dark_color(),screen); cur=0;
                           draw_cur(wm->bright_color(),screen); } break;
      case JK_BACKSPACE : if (cur)
         { draw_cur(wm->dark_color(),screen); cur--;
           for (xx=cur; xx<(int)strlen(format)-1; xx++)
             data[xx]=data[xx+1];
           data[strlen(format)-1]=' ';
           draw_text(screen);
           draw_cur(wm->bright_color(),screen);
           wm->Push(Event(m_id, (char *)this));
         } break;
      default : if (ev.key>=' ' && ev.key<='~')
         {
           draw_cur(wm->dark_color(),screen);
           for (xx=strlen(format)-1; xx>cur && xx>0; xx--)
             data[xx]=data[xx-1];
           data[cur]=ev.key;
           if (cur<(int)strlen(format)-1)
             cur++;
       data[strlen(format)]=0;
           draw_text(screen);
           draw_cur(wm->bright_color(),screen);
           wm->Push(Event(m_id, (char *)this));
         } break;
    }
  }
}

void ATextField::Draw(int active, AImage *screen)
{
  if (active)
  {
    screen->Rectangle(ivec2(xstart(), m_pos.y), ivec2(xend(), yend()),
                      wm->bright_color());
    draw_cur(wm->bright_color(),screen);
  }
  else
  {
    screen->Rectangle(ivec2(xstart(), m_pos.y), ivec2(xend(), yend()),
                      wm->dark_color());
    draw_cur(wm->dark_color(),screen);
  }
}

ibox2 ATextField::GetArea()
{
    return ibox2(m_pos, ivec2(xend(), yend()));
}

ATextField::ATextField(ivec2 pos, int id, char const *Prompt,
                       char const *Format, char const *Data)
  : AWidget(pos, id)
{
    int slen=(strlen(Format)>strlen(Data) ? strlen(Format) : strlen(Data));

    prompt = strdup(Prompt);
    format = strcpy((char *)malloc(slen+1),Format);
    data = strcpy((char *)malloc(slen+1),Data);
    cur = strlen(data);
    while (cur && data[cur-1]==' ') cur--;
}

ATextField::ATextField(ivec2 pos, int id, char const *Prompt,
                       char const *Format, double Data)
  : AWidget(pos, id)
{
  char num[20];
  sprintf(num,"%g",Data);
  int slen=(strlen(Format)>strlen(num) ? strlen(Format) : strlen(num));
  prompt = strdup(Prompt);
  format=strcpy((char *)malloc(slen+1),Format);
  data=strcpy((char *)malloc(slen+1),num);
  cur=strlen(num);
  while (cur && data[cur-1]==' ') cur--;
}


void AButton::push()
{
    up = !up;
}

void AButton::handle_event(Event &ev, AImage *screen, InputManager *im)
{
    if ((ev.type == EV_KEY && ev.key == 13)
         || (ev.type == EV_MOUSE_BUTTON && ev.mouse_button))
    {
        GetArea(); /* FIXME: was this always a no-op? */
        up = !up;
        draw_first(screen);
        Draw(act, screen);
        wm->Push(Event(m_id, (char *)this));
    }
}

void AButton::Draw(int active, AImage *screen)
{
    int color = active ? wm->bright_color() : wm->medium_color();
    ibox2 area = GetArea();

    if (active != act && act_id != -1 && active)
        wm->Push(Event(act_id, nullptr));

    if (!pressed)
    {
        screen->Rectangle(area.A + ivec2(2), area.B - ivec2(2), color);
        act = active;
    }
    else if (!up)
        screen->PutImage(act_pict, m_pos);
    else if (!active)
        screen->PutImage(visual, m_pos);
    else
        screen->PutImage(pressed, m_pos);
}

void AButton::draw_first(AImage *screen)
{
    if (pressed)
    {
        Draw(0, screen);
        return;
    }

    ibox2 area = GetArea();

    if (up)
    {
        screen->Rectangle(area.A, area.B, wm->black());
//      screen->widget_bar(,wm->bright_color(),wm->medium_color(),wm->dark_color());
        screen->WidgetBar(area.A + ivec2(1), area.B - ivec2(1),
                          wm->bright_color(), wm->medium_color(), wm->dark_color());
    }
    else
    {
        screen->Line(area.A, ivec2(area.B.x, area.A.y), wm->dark_color());
        screen->Line(area.A, ivec2(area.A.x, area.B.y), wm->dark_color());
        screen->Line(ivec2(area.B.x, area.A.y + 1), area.B, wm->bright_color());
        screen->Line(ivec2(area.A.x + 1, area.B.y), area.B, wm->bright_color());
        screen->Bar(area.A + ivec2(1), area.B - ivec2(1), wm->medium_color());
    }

    if ((up && m_text.Count()) || (!up && !visual))
    {
        wm->font()->PutString(screen, m_pos + ivec2(4, 5), m_text, wm->black());
        wm->font()->PutString(screen, m_pos + ivec2(3, 4), m_text);
    }
    else if (up)
        screen->PutImage(visual, m_pos + ivec2(3), 1);
    else
        screen->PutImage(visual, area.A + ivec2(3), 1);
}

void ATextField::draw_first(AImage *screen)
{
  wm->font()->PutString(screen, m_pos + ivec2(0, 3), prompt);
  screen->Bar(ivec2(xstart(), m_pos.y), ivec2(xend(), yend()), wm->dark_color());
  wm->font()->PutString(screen, ivec2(xstart() + 1, m_pos.y + 3), data);
}


void ATextField::draw_cur(int color, AImage *screen)
{
    screen->Bar(ivec2(xstart() + cur * wm->font()->Size().x + 1, yend() - 2),
                ivec2(xstart() + (cur + 1) * wm->font()->Size().x, yend() - 1),
                color);
}



AInfoField::AInfoField(ivec2 pos, int id, char const *info)
{
    m_pos = pos;
    m_text = info;
    m_id = id;
    w = -1;
}


ibox2 AInfoField::GetArea()
{
    if (w == -1)     // if we haven't calculated this yet
    {
        int fw = wm->font()->Size().x, fh = wm->font()->Size().y, maxw = 0;
        int i = 0;
        for (w = fw, h = fh + 1; m_text[i]; ++i)
        {
            maxw = lol::max(maxw, w);
            if (m_text[i] == '\n')
            {
                h += fh + 1;
                w = 1;
            }
            else
                w += fw;
        }
        w = maxw;
    }

    return ibox2(m_pos, m_pos + ivec2(w, h));
}

void AInfoField::put_para(AImage *screen, char const *st, int dx, int dy,
                          int xspace, int yspace, JCFont *font, int color)
{
  int ox=dx;
  while (*st)
  {
    if (*st=='\n')
    {
      dx=ox;
      dy+=yspace;
    }
    else
    {
      font->PutChar(screen, ivec2(dx, dy), *st, color);
      dx+=xspace;
    }
    st++;
  }
}

void AInfoField::draw_first(AImage *screen)
{
    put_para(screen, m_text.C(), m_pos.x+1, m_pos.y+1, wm->font()->Size().x,
             wm->font()->Size().y, wm->font(), wm->black());
    put_para(screen, m_text.C(), m_pos.x, m_pos.y, wm->font()->Size().x,
             wm->font()->Size().y, wm->font(), wm->bright_color());
}

