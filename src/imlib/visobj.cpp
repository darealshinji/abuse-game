/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "input.hpp"
#include "visobj.hpp"

void image_visual::draw(image *screen, int x, int y, 
		    filter *f)
{ 
  if (f)
    f->put_image(screen,im,x,y,1);
  else
    im->put_image(screen,x,y); 
}


string_visual::string_visual(char *string, int Color)
{
  st=strcpy((char *)jmalloc(strlen(string)+1,"string visual"),string);
  color=Color;
  w=-1; 
}


int string_visual::width()
{
  if (w==-1)  // not calculated yet
  {
    int fw=wm->font()->width(),fh=wm->font()->height(),maxw=0;
    char *info=st;
    for (w=fw,h=fh+1;*info;info++)
    {
      if (w>maxw) maxw=w;
      if (*info=='\n')
      {
	h+=fh+1;
	w=1;
      }
      else w+=fw;      
    }
    w=maxw;
  }
  return w;
}

int string_visual::height()
{ 
  if (w==-1) width();
  return h;
}


static void put_para(image *screen, char *st, int dx, int dy, 
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
      font->put_char(screen,dx,dy,*st,color);
      dx+=xspace;
    }
    st++;
  }
}

void string_visual::draw(image *screen, int x, int y, filter *f)

{ 
  put_para(screen,st,x+1,y+1,wm->font()->width(),wm->font()->height(),
	   wm->font(),f ? f->get_mapping(color) : color);
}

