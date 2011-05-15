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

#include "help.h"
#include "game.h"
#include "netcfg.h"

int total_help_screens;
int *help_screens;
static int help_page=0;

void fade_in(image *im, int steps);
void fade_out(int steps);

void draw_help()
{
  image *im=cache.img(help_screens[help_page]);
  int x1=xres/2-im->Size().x/2,y1=yres/2-im->Size().y/2;
  int x2=x1+im->Size().x,y2=y1+im->Size().y;
  main_screen->PutImage(im, vec2i(x1, y1));
  main_screen->Bar(vec2i(0, 0), vec2i(x1 - 1, yres), 0);
  main_screen->Bar(vec2i(0, 0), vec2i(xres, y1 - 1), 0);
  main_screen->Bar(vec2i(x2, y1), vec2i(xres, yres), 0);
  main_screen->Bar(vec2i(x1, y2), vec2i(x2, yres), 0);
}

void help_handle_event(Event &ev)
{
  if (ev.window!=NULL) return ;

  if (the_game->state!=HELP_STATE)
  {
    if (ev.type==EV_KEY && (ev.key=='h' || ev.key=='?' || ev.key==JK_F1) && help_screens)
    {
      if (!main_net_cfg || (main_net_cfg->state!=net_configuration::SERVER && main_net_cfg->state!=net_configuration::CLIENT))
      {
    the_game->state=HELP_STATE;
    help_page=0;
      }
    }
  } else if (ev.type==EV_KEY)
  {
    if (ev.key==JK_ESC || help_page>=total_help_screens-1)
    {
      the_game->state=RUN_STATE;
      the_game->draw(0);
    }
    else
      help_page++;
  }
}
