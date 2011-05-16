/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include "common.h"

#include "automap.h"
#include "game.h"

automap *current_automap=0;

void automap::draw()
{
  if (!automap_window) return ;
  image *screen=automap_window->m_surf;

  long sx,ex,sy,ey,x,y,window_xstart,window_ystart,
                       window_xend,window_yend,centerx,centery,
                       draw_xstart,draw_ystart,
                       i,j;

  x=the_game->first_view->x_center();
  y=the_game->first_view->y_center();


  window_xstart=automap_window->x1();
  window_ystart=automap_window->y1();
  window_xend=automap_window->x2();
  window_yend=automap_window->y2();
  centerx=(window_xstart+window_xend)/2;
  centery=(window_ystart+window_yend)/2;

  sx=x/f_wid-w/2;                // start drawing with this foretile
  sy=y/f_hi-h/2;
  ex=sx+w;
  ey=sy+h;

  if (sx<0)                       // does the map scroll past the left side ?
  { sx=0;                         // yes, start drawing at 0
    draw_xstart=centerx-(x*AUTOTILE_WIDTH/f_wid);
  }
  else
    draw_xstart=centerx-(x*AUTOTILE_WIDTH/f_wid-sx*AUTOTILE_WIDTH);

  if (sy<0)
  {
    sy=0;
    draw_ystart=centery-(y*AUTOTILE_HEIGHT/f_hi);
  }
  else
    draw_ystart=centery-(y*AUTOTILE_HEIGHT/f_hi-sy*AUTOTILE_HEIGHT);

  // if view position hasn't changed, only update the blinking dot and return
  if (draw_xstart==old_dx && draw_ystart==old_dy)
  {
   automap_window->m_surf->Lock();
   automap_window->m_surf->AddDirty(centerx,centery,centerx + 1,centery + 1);
    if ((tick++)&4)
      automap_window->m_surf->PutPixel(vec2i(centerx,centery),255);
    else
      automap_window->m_surf->PutPixel(vec2i(centerx,centery),27);
   automap_window->m_surf->Unlock();
    return ;
  }

  old_dx=draw_xstart;
  old_dy=draw_ystart;


  if (ex>=cur_lev->foreground_width())
    ex=cur_lev->foreground_width()-1;
  if (ey>=cur_lev->foreground_height())
    ey=cur_lev->foreground_height()-1;


  screen->Bar(vec2i(window_xstart, window_ystart),
              vec2i(draw_xstart, window_yend), 0);
  screen->Bar(vec2i(window_xstart, window_ystart),
              vec2i(window_xend, draw_ystart), 0);


/*  if (ex>=cur_lev->foreground_width())
  {
    draw_xend=center
    ex=foreground_width()-1; */




  // we are going to redraw the whole map, so make the dirty rect work easier by marking
  // everything dirty
  screen->AddDirty(window_xstart,window_ystart,window_xend+1,window_yend+1);




  // draw the tiles that will be around the border of the automap with PutImage
  // because it handles clipping, but for ths reason is slower, the rest
  // we will slam on as fast as possible

  screen->SetClip(vec2i(window_xstart, window_ystart),
                  vec2i(window_xend + 1, window_yend + 1));
#if 0
  for (i=draw_xstart,j=draw_ystart,x=sx,y=sy; y<=ey; j+=AUTOTILE_HEIGHT,y++)
    screen->PutImage(foretiles[cur_lev->get_fg(x, y)]->micro_image, vec2i(i, j), 0);

  for (i=draw_xstart+ex*AUTOTILE_WIDTH,j=draw_ystart,y=sy,x=ex; y<=ey; j+=AUTOTILE_HEIGHT,y++)
    screen->PutImage(foretiles[cur_lev->get_fg(x, y)]->micro_image, vec2i(i, j), 0);

  for (i=draw_xstart,j=draw_ystart,x=sx,y=sy; x<=ex; i+=AUTOTILE_WIDTH,x++)
    screen->PutImage(foretiles[cur_lev->get_fg(x, y)]->micro_image, vec2i(i, j), 0);

  for (i=draw_xstart,j=draw_ystart+ey*AUTOTILE_HEIGHT,x=sx,y=ex; x<=ex; i+=AUTOTILE_WIDTH,x++)
    screen->PutImage(foretiles[cur_lev->get_fg(x, y)]->micro_image, vec2i(i, j), 0);
#endif

  unsigned short *fgline;
  for (j=draw_ystart,y=sy; y<=ey; j+=AUTOTILE_HEIGHT,y++)
  {
    fgline=cur_lev->get_fgline(y)+sx;
    for (i=draw_xstart,x=sx; x<=ex; i+=AUTOTILE_WIDTH,x++,fgline++)
    {
      if ((*fgline)&0x8000)
      {
    int id=foretiles[ (*fgline)&0x7fff];
    if (id>=0)
          screen->PutImage(cache.foret(id)->micro_image, vec2i(i, j), 0);
    else
          screen->PutImage(cache.foret(foretiles[0])->micro_image, vec2i(i, j), 0);
      }
      else
        screen->Bar(vec2i(i, j),
                    vec2i(i + AUTOTILE_WIDTH - 1, j + AUTOTILE_HEIGHT - 1), 0);
    }
  }

  // draw the person as a dot, no need to add a dirty because we marked the
  // whole screen already
  automap_window->m_surf->Lock();
  if ((tick++)&4)
    automap_window->m_surf->PutPixel(vec2i(centerx,centery),255);
  else
    automap_window->m_surf->PutPixel(vec2i(centerx,centery),27);
  automap_window->m_surf->Unlock();

  // set the clip back to full window size because soemthing else could mess with the area
  automap_window->m_surf->SetClip(vec2i(0), screen->Size());
}

void automap::toggle_window()
{
    if (automap_window)
    {
        wm->close_window(automap_window);
        automap_window = NULL;
    }
    else
    {
        old_dx = -1000; // make sure the map gets drawn the first time
        old_dy = -1000;

        automap_window = wm->new_window(0, 0, w * AUTOTILE_WIDTH,
                                        h * AUTOTILE_HEIGHT, NULL, "Map");
        automap_window->m_surf->Bar(vec2i(17, 1), vec2i(17 + 8 * 6 + 3, 6),
                                    wm->medium_color());
        wm->font()->PutString(automap_window->m_surf, vec2i(20, 2), "Automap",
                              wm->dark_color());
        draw();
    }
}


automap::automap(level *l, int width, int height)
{
  w=width;
  h=height;

  tick=0;
  cur_lev=l;
  automap_window=NULL;
  toggle_window();
}

void automap::handle_event(Event &ev)
{

  //only respond to stuff in our window or on the main screen
  if (ev.window==NULL || ev.window==automap_window)
  {
    switch (ev.type)
    {
      case EV_KEY :
        switch(ev.key)
    {
      case 'A' :
      case 'a' :
          toggle_window();
        break;
    }
    break;
      case EV_CLOSE_WINDOW :
        wm->close_window(automap_window);
    automap_window=NULL;
    break;
    }
  }
}







