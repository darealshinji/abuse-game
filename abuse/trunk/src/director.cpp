/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "common.h"

#include "director.h"
#include "game.h"
#include "lisp.h"
#include "fonts.h"

director scene_director;

void director::scroll_text(char *Text)
{ text=Text;
  text_y=the_game->first_view->cy2-the_game->first_view->cy1+1;
}

director::director()
{
  tleft=ttop=tright=tbottom=pan_xv=pan_yv=0;
  text_step=-2;
  frame_speed=100;
  scroll_speed=60;
  pan_speed=60;
  scene_abort=0;
}

extern unsigned char *white_light;



int text_draw(int y, int x1, int y1, int x2, int y2, char const *buf, JCFont *font, uint8_t *cmap, char color)
{
  int cx1, cy1, cx2, cy2, word_size, word_len;
  screen->GetClip(cx1, cy1, cx2, cy2);
  screen->InClip(x1,y1,x2+1,y2+1);
  int h=font->height()+2,w=font->width(),x=x1,dist;
  y+=y1;
  char const *word_start;

  while (buf && *buf)
  {
    do
    {
      if (*buf=='\\' && buf[1]=='n')
      {
    x=x1;
    y+=h*2;
    buf+=2;
      }

      // skip space
      if (*buf==' ' || *buf=='\r' || *buf=='\n' || *buf=='\t')
      {
    x+=w;
    while (*buf==' ' || *buf=='\r' || *buf=='\n' || *buf=='\t')   // skip space until next word
          buf++;
      }

      word_start=buf;
      for (word_len=0,word_start=buf,word_size=0; *buf && *buf!=' ' && *buf!='\r' && *buf!='\n' &&
       *buf!='\t' && (*buf!='\\' || buf[1]!='n'); buf++,word_size+=w,word_len++);

      if (word_size<x2-x1) // make sure the word can fit on the screen
      {
    if (word_size+x>x2)    // does word not fit on line?
    {
      y+=h;                // go to next line
      x=x1;
    }
      }


      if (y+h<y1)         // word on screen yet?
    x+=word_size;

    } while (*buf && y+h<y1);     // if not on screen yet, fetch next word

    dist=31;
    if (y-y1<dist)
    {
      if (y-y1<1) dist=0;
      else dist=y-y1;
    }

    if (y2-y<dist)
    {
      if (y2-y<1) dist=0;
      else dist=y2-y;
    }

    int c=cmap[dist];
    if (y>y2) { buf=NULL; }
    else
    {
      while (word_len--)
      {
    font->put_char(screen,x+1,y+1,*word_start,0);
    font->put_char(screen,x,y,*word_start,c);
    word_start++;
    x+=w;
      }
    }

  }
  screen->SetClip(cx1,cy1,cx2,cy2);
  return (y<=y1);
}

void director::wait(void *arg)
{
  if (scene_abort) return ;
  pan_time=frame_time=text_time=NULL;
  int done=0;
  LSymbol *pan_symbol = LSymbol::FindOrCreate("pan"),
             *text_symbol = LSymbol::FindOrCreate("text");

  JCFont *font=wm->font();

  do
  {
    the_game->draw_map(the_game->first_view);
    time_marker cur_time;

    if (pan_steps)
    {
      if (pan_time)
      {
    if ((int)(cur_time.diff_time(pan_time)*1000)>pan_speed)
    {
      the_game->pan(pan_xv,pan_yv);
      pan_steps--;
      delete pan_time;
      if (pan_steps)
          pan_time=new time_marker;
      else pan_time=NULL;
    }
      } else pan_time=new time_marker;
    } else if (arg==pan_symbol) done=1;

    if (text)
    {
      if (text_draw(text_y,
            the_game->first_view->cx1+tleft,
            the_game->first_view->cy1+ttop,
            the_game->first_view->cx2-tright,
            the_game->first_view->cy2-tbottom,text,font,
            white_light+32*256,
            wm->bright_color()

            ))
        text=NULL;
      if (text_time)
      {
    if ((int)(cur_time.diff_time(text_time)*1000)>scroll_speed)
    {
      text_y+=text_step;
      delete text_time;
      if (text)
        text_time=new time_marker;
      else
        text_time=NULL;
    }
      } else text_time=new time_marker;
    } else if (arg==text_symbol) done=1;

    wm->flush_screen();
    while (wm->event_waiting())
    {
      event ev;
      wm->get_event(ev);
      if (ev.type==EV_KEY)
      {
    switch (ev.key)
    {
      case JK_UP :
      case JK_LEFT :
      {
        if (scroll_speed>=20)
        scroll_speed-=20;
          else text_step--;
      }
      break;
      case JK_RIGHT :
      case JK_DOWN :
      {
        if (text_step<-2)
        text_step++;
        else if (scroll_speed<200) scroll_speed+=20;
        break;
        case JK_ESC : set_abort(1); done=1; break;
        case JK_ENTER : done=1; break;
      }
    }
      }
    }
  } while (!done);
  if (pan_time) delete pan_time;
  if (frame_time) delete frame_time;
  if (text_time) delete text_time;
}

