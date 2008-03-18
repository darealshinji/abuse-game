/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "transp.hpp"

void transp_put(image *im, image *screen, uint8_t *table, int x, int y)
{
  short cx1,cy1,cx2,cy2;
  screen->get_clip(cx1,cy1,cx2,cy2);
  int xs=0,ys=0,xl=im->width(),yl=im->height();
  if (x<cx1)
  {
    int chop=cx1-x;
    xs+=chop;
    xl-=chop;
    x+=chop;
  }
  if (y<cy1)
  {
    int chop=cy1-y;
    ys+=chop;
    yl-=chop;
    y+=chop;
  }
  if (x+xl>cx2)
    xl=cx2-x;
  if (y+yl>cy2)
    yl=cy2-y;

  if (xl<0 || yl<0) return ;
  screen->add_dirty(x,y,x+xl-1,y+yl-1);

  int ye=ys+yl;
  int xe=xs+xl;

  uint8_t *isl=im->scan_line(ys)+xs;
  uint8_t *ssl=screen->scan_line(y)+x;
  int iw=im->width(),sw=screen->width();

  for (int iy=ys;iy<ye;iy++,y++,isl+=iw,ssl+=sw)
  {
    uint8_t *s=ssl,*i=isl;
    for (int ix=xs;ix<xe;ix++,s++,i++)
    {
      if (*i)
        *s=*i;
      else *s=table[*s];
    }
  }
}


/*
void transp_put(image *im, image *screen, uint8_t *table, int x, int y)
{
  short cx1,cy1,cx2,cy2;
  screen->get_clip(cx1,cy1,cx2,cy2);
  int xs=0,ys=0,xl=im->width(),yl=im->height();
  if (x<cx1)
  {
    int chop=cx1-x;
    xs+=chop;
    xl-=chop;
    x+=chop;
  }
  if (y<cy1)
  {
    int chop=cy1-y;
    ys+=chop;
    yl-=chop;
    y+=chop;
  }
  if (x+xl>cx2)
    xl=cx2-x;
  if (y+yl>cy2)
    yl=cy2-y;

  if (xl<0 || yl<0) return ;
  screen->add_dirty(x,y,x+xl-1,y+yl-1);

  int ye=ys+yl;
  int xe=xs+xl;

  uint8_t *isl=im->scan_line(ys)+xs;
  uint8_t *ssl=screen->scan_line(y)+x;
  int iw=im->width(),sw=screen->width();

  for (int iy=ys;iy<ye;iy++,y++,isl+=iw,ssl+=sw)
  {
    uint8_t *s=ssl,*i=isl;
    for (int ix=xs;ix<xe;ix++,s++,i++)
      *s=table[((*i)<<8)|(*s)];
  }
}


*/
