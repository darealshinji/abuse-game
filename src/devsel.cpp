/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "devsel.hpp"
#include "scroller.hpp"
#include "cache.hpp"
#include "game.hpp"

void scale_put(image *im, image *screen, int x, int y, short new_width, short new_height);
void scale_put_trans(image *im, image *screen, int x, int y, short new_width, short new_height);
int cur_bg=0,cur_fg=0,cur_char=0;

void tile_picker::recenter(image *screen, window_manager *wm)
{
  set_x(get_current(),screen,wm);
}

int tile_picker::picw()
{
  switch (type)
  {
    case SPEC_FORETILE :
    {
      return cash.foret(foretiles[0])->im->width()/scale;
    } break;
    case SPEC_BACKTILE :
    {
      return cash.backt(backtiles[0])->im->width()/scale;
    } break;
    default :
      return 30/scale; 
  }
}

int tile_picker::pich()
{
  switch (type)
  {
    case SPEC_FORETILE :
    {
      return cash.foret(foretiles[0])->im->height()/scale;
    } break;
    case SPEC_BACKTILE :
    {
      return cash.backt(backtiles[0])->im->height()/scale;
    } break;
    default :
      return 40/scale; 
  }
}

int tile_picker::total()
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

tile_picker::tile_picker(int X, int Y, int ID, int spec_type, window_manager *wm, 
			 int Scale, int scroll_h, int Wid, ifield *Next)
     : scroller(X,Y,ID,2,2,1,0,Next)
{ 
  wid=Wid;
  type=spec_type;
  th=scroll_h;
  scale=Scale;
  set_size(picw()*wid,pich()*th);
  sx=get_current();
  t=total();
}


void tile_picker::scroll_event(int newx, image *screen, window_manager *wm)
{
  int yo=y,ya=pich(),xw=picw(),c=get_current(),xo;
  image im(xw,ya);
  last_sel=newx;

  screen->bar(x,y,x+l-1,y+h-1,eh->black());
  for (int i=newx;i<newx+th*wid;i++)
  {
    xo=x+((i-newx)%wid)*xw;
    yo=y+((i-newx)/wid)*ya;

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
	      the_game->get_fg(i)->im->put_image(&im,0,0);

	      if (rev)
	      {
		screen->bar(xo,yo,xo+xw-1,yo+ya-1,wm->bright_color());
		scale_put_trans(&im,screen,xo,yo,xw,ya);
	      }
	      else scale_put(&im,screen,xo,yo,xw,ya);
	    }
	  } break;
	  case SPEC_BACKTILE : 
	  {
	    if (backtiles[i]<0) blank=1;
	    else
	      scale_put(the_game->get_bg(i)->im,screen,xo,yo,xw,ya);
	    
	  } break;
	  case SPEC_CHARACTER : 
	  {
	    figures[i]->get_sequence(stopped)->get_figure(0)->forward->put_image(&im,0,0);
	    scale_put(&im,screen,xo,yo,xw,ya);
	  } break;
	}
      } else blank=1;

      if (i==c)
        screen->rectangle(xo,yo,xo+xw-1,yo+ya-1,wm->bright_color());


  }
}


void tile_picker::handle_inside_event(event &ev, image *screen, window_manager *wm, input_manager *inm)
{
  if (ev.type==EV_MOUSE_BUTTON)
  {
    int sel=((ev.mouse_move.y-y)/pich()*wid)+(ev.mouse_move.x-x)/picw()+last_sel;
    if (sel<t && sel>=0 && sel!=get_current())
    {
      set_current(sel);
      scroll_event(last_sel,screen,wm);
    }
  }
}




int tile_picker::get_current()
{
  switch (type)
  { 
    case SPEC_FORETILE : 
    {return cur_fg;} break;
    case SPEC_BACKTILE : 
    {return cur_bg;} break;
    case SPEC_CHARACTER : 
    {return cur_char;} break;
  }
  return 0;
}

void tile_picker::set_current(int x)
{
  switch (type)
  {
    case SPEC_FORETILE : 
    {cur_fg=x;} break;
    case SPEC_BACKTILE : 
    {cur_bg=x;} break;
    case SPEC_CHARACTER : 
    {cur_char=x;} break;
  }
}





