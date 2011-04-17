/*
 *  Abuse - dark 2D side - scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include <ctype.h>
#include <setjmp.h>

#ifdef __APPLE__
// SDL for OSX needs to override main()
#   include <SDL.h>
#endif

#include "sdlport/joy.h"

#include "dev.h"
#include "game.h"

#include "id.h"
#include "timing.h"
#include "automap.h"
#include "help.h"
#include "ability.h"
#include "cache.h"
#include "lisp.h"
#include "jrand.h"
#include "configuration.h"
#include "light.h"
#include "scroller.h"
#include "dprint.h"
#include "nfserver.h"
#include "video.h"
#include "transp.h"
#include "clisp.h"
#include "guistat.h"
#include "menu.h"
#include "gamma.h"
#include "lisp_gc.h"
#include "demo.h"
#include "sbar.h"
#include "profile.h"
#include "compiled.h"
#include "lisp_gc.h"
#include "pmenu.h"
#include "timing.h"
#include "chat.h"
#include "demo.h"
#include "netcfg.h"

#define SHIFT_RIGHT_DEFAULT 0
#define SHIFT_DOWN_DEFAULT 30

extern CrcManager *net_crcs;

Game *the_game = NULL;
WindowManager *wm = NULL;
int dev, shift_down = SHIFT_DOWN_DEFAULT, shift_right = SHIFT_RIGHT_DEFAULT;
double sum_diffs = 1, total_diffs = 12;
int total_active = 0;
int32_t map_xoff = 0, map_yoff = 0;
int32_t current_vxadd, current_vyadd;
int frame_panic = 0, massive_frame_panic = 0;
int demo_start = 0, idle_ticks = 0;
int req_end = 0;

extern palette *old_pal;
char **start_argv;
int start_argc;
int has_joystick = 0;
char req_name[100];

extern uint8_t chatting_enabled;

// Enabled TCPIP driver
#include "tcpip.h"
tcpip_protocol tcpip;

FILE *open_FILE(char const *filename, char const *mode)
{
    /* FIXME: potential buffer overflow here */
    char tmp_name[200];
    if(get_filename_prefix() && filename[0] != '/')
        sprintf(tmp_name, "%s %s", get_filename_prefix(), filename);
    else
        strcpy(tmp_name, filename);
    return fopen(tmp_name, mode);
}

void handle_no_space()
{
    static char const *no_space_msg =
        "\nYou are out of disk space or the game\n"
        "was unable to write to disk for some reason\n"
        "The game cannot continue, please check this out\n"
        "and try again.\n";

    if(!wm)
    {
        fprintf(stderr, "%s\n", no_space_msg);
        exit(0);
    }

    info_field *inf = new info_field(0, wm->font()->height() * 2, ID_NULL,
                                     no_space_msg, NULL);
    button *b = new button(0, 0, ID_QUIT_OK, "Quit", inf);
    Jwindow *no_space = wm->new_window(0, 0, -1, -1, b, "ERROR");

    event ev;
    do
    {
        wm->flush_screen();
        wm->get_event(ev);
    } while(ev.type != EV_MESSAGE || ev.message.id != ID_QUIT_OK);
    wm->close_window(no_space);

    close_graphics();
    exit(1);
}

void Game::play_sound(int id, int vol, int32_t x, int32_t y)
{
    if(!(sound_avail & SFX_INITIALIZED))
        return;
    if(vol < 1)
        return;
    if(!player_list)
        return;

    int mindist = 500;
    view *cd = NULL;
    for(view *f = player_list; f; f = f->next)
    {
        if(!f->local_player())
            continue;

        int d, cx = abs(f->x_center() - x), cy = abs(f->y_center() - y);
        if(cx < cy)
            d = cx + cy - (cx >> 1);
        else
            d = cx + cy - (cy >> 1);

        if(d < mindist)
        {
            cd = f;
            mindist = d;
        }
    }
    if(mindist >= 500)
        return;

    if(mindist < 100)
        mindist = 0;
    else
        mindist -= 100;

    // Calculate the position of the sound relative to the player
    int p = (cd->x_center() - x) / 2 + 128;
    if(p < 0)
        p = 0;
    if(p > 255)
        p = 255;

    int v = (400 - mindist) * sfx_volume / 400 - (127 - vol);
    if(v > 0)
        cache.sfx(id)->play(v, 128, p);
}

int get_option(char const *name)
{
    int i;
    for(i = 1; i < start_argc; i++)
    {
        if(!strcmp(start_argv[i], name))
        {
            return i;
        }
    }
    return 0;
}


void make_screen_size(int w, int h)
{
    for(view *f = player_list; f; f = f->next)
    {
        if(!f->local_player())
            continue;

        if(w >= xres - 1)
            w = xres - 2;
        if(h >= yres - 1)
            h = yres - 2;
        f->suggest.cx1 = (xres + 1) / 2 - w / 2;
        f->suggest.cx2 = (xres + 1) / 2 + w / 2;
        f->suggest.cy1 = (yres - 31) / 2 + 5 - h / 2;
        f->suggest.cy2 = (yres - 51) / 2 + 5 + h / 2;
        f->suggest.shift_down = f->shift_down;
        f->suggest.shift_right = f->shift_right;
        f->suggest.pan_x = f->pan_x;
        f->suggest.pan_y = f->pan_y;
        f->suggest.send_view = 1;
    }
}

void Game::grow_views(int amount)
{
    view *f;

    for(f = first_view; f; f = f->next)
    {
        if(!f->local_player())
            continue;

        f->suggest.cx1=(f->cx1 - amount);
        f->suggest.cy1 = f->cy1 - amount / 2;
        f->suggest.cx2=(f->cx2 + amount);
        f->suggest.cy2 = f->cy2 + amount / 2;
        f->suggest.shift_down = f->shift_down;
        f->suggest.shift_right = f->shift_right;
        f->suggest.pan_x = f->pan_x;
        f->suggest.pan_y = f->pan_y;

        f->suggest.send_view = 1;
    }

    for(f = first_view; f; f = f->next)
    {
        if(!f->local_player())
            continue;

        if(f->suggest.cx2 - f->suggest.cx1 < 20
           || f->suggest.cy2 - f->suggest.cy1 < 15
           || f->suggest.cx1 < 0 || f->suggest.cy1 < 0)
            f->suggest.send_view = 0;

        if(f->next && f->next->local_player()
           && f->suggest.cy2 >= f->next->cy1)
            f->suggest.send_view = 0;
    }
}

void Game::pan(int xv, int yv)
{
    first_view->pan_x += xv;
    first_view->pan_y += yv;
}

view *Game::view_in(int mousex, int mousey)
{
    for(view *f = first_view; f; f = f->next)
        if(f->drawable() && mousex >= f->cx1 && mousey >= f->cy1
           && mousex <= f->cx2 && mousey <= f->cy2)
            return f;
    return NULL;
}

int playing_state(int state)
{
    return state == RUN_STATE || state == PAUSE_STATE;
}

void Game::ftile_on(int screenx, int screeny, int32_t &x, int32_t &y)
{
    mouse_to_game(screenx, screeny, x, y);
    x /= ftile_width();
    y /= ftile_height();
}

void Game::btile_on(int screenx, int screeny, int32_t &x, int32_t &y)
{
    view *f = view_in(screenx, screeny);
    if(f)
    {
        x = ((int32_t)screenx - (int32_t)f->cx1
                + f->xoff() * bg_xmul / bg_xdiv) / (int32_t)b_wid;
        y = ((int32_t)screeny - (int32_t)f->cy1
                + f->yoff() * bg_ymul / bg_ydiv) / (int32_t)b_hi;
    }
    else
    {
        x = -1;
        y = -1;
    }
}

void Game::mouse_to_game(int32_t x, int32_t y,
                         int32_t &gamex, int32_t &gamey, view *f)
{
    if(!f)
        f = view_in(x, y);
    if(!f)
        f = player_list;  // if not in a view use the first one

    if(f)
    {
        if(dev & MAP_MODE)
        {
            gamex = (x - (int32_t)f->cx1) * ftile_width() / AUTOTILE_WIDTH + map_xoff * ftile_width();
            gamey = (y - (int32_t)f->cy1) * ftile_height() / AUTOTILE_HEIGHT + map_yoff * ftile_height();
        }
        else
        {
            gamex = x - (int32_t)f->cx1 + f->xoff();
            gamey = y - (int32_t)f->cy1 + f->yoff();
        }
    }
}

void Game::game_to_mouse(int32_t gamex, int32_t gamey, view *which,
                         int32_t &x, int32_t &y)
{
    if(!(dev & MAP_MODE))
    {
        x = gamex - which->xoff() + which->cx1;
        y = gamey - which->yoff() + which->cy1;
        return;
    }

    int32_t x1, y1;

    if(dev & EDIT_MODE)
    {
        x1 = map_xoff;
        y1 = map_yoff;
    }
    else
    {
        if(which->focus)
        {
            x1 = which->focus->x / ftile_width()
                  - (which->cx2 - which->cx1) / AUTOTILE_WIDTH / 2;
            y1 = which->focus->y / ftile_height()
                  - (which->cy2 - which->cy1) / AUTOTILE_HEIGHT / 2;
        }
        else
            x1 = y1 = 0;
    }

    if(x1 < 0)
        x1 = 0;
    if(y1 < 0)
        y1 = 0;

    x = gamex * AUTOTILE_WIDTH / ftile_width()
          - x1 * AUTOTILE_WIDTH + which->cx1;
    if(x1 > 0)
        x -= (which->focus->x * AUTOTILE_WIDTH / ftile_width())
               % AUTOTILE_WIDTH;

    y = gamey * AUTOTILE_HEIGHT / ftile_height()
          - y1 * AUTOTILE_HEIGHT + which->cy1;
    if(y1 > 0)
        y -= (which->focus->y * AUTOTILE_HEIGHT / ftile_height())
               % AUTOTILE_HEIGHT;
}

int window_state(int state)
{
    switch (state)
    {
    case RUN_STATE:
    case PAUSE_STATE:
    case JOY_CALB_STATE:
        return 1;

    case INTRO_START_STATE:
    case HELP_STATE:
    case INTRO_MORPH_STATE:
    case MENU_STATE:
    case SCENE_STATE:
        return 0;
    }

    return 1;
}

void Game::set_state(int new_state)
{
    int d = 0;
    reset_keymap(); // we think all the keys are up right now

    if(playing_state(new_state) && !playing_state(state))
    {
        if(first_view && first_view != player_list)
        {
            while(first_view)
            {
                view *tmp = first_view;
                first_view = first_view->next;
                delete tmp;
            }
            first_view = old_view;
            old_view = NULL;
        }
        first_view = player_list;
        d = 1;
    }
    else if(!playing_state(new_state) && (playing_state(state) || state == START_STATE))
    {
        if(player_list)
        {
            first_view = new view(player_list->focus, NULL, -1);
            first_view->pan_x = player_list->xoff();
            first_view->pan_y = player_list->yoff();
        }
        else
            first_view = new view(NULL, NULL, 0);
        first_view->cx1 = (xres + 1) / 2 - 155;
        first_view->cy1 = (yres + 1) / 2 - 95;
        first_view->cx2 = (xres + 1) / 2 + 155;
        if(total_weapons)
            first_view->cy2 = (yres + 1) / 2 + 68;
        else
            first_view->cy2 = (yres + 1) / 2 + 95;
        d = 1;
    }

    // switching to / from scene mode cause the screen size to change and the border to change
    // so we need to redraw.
    if(window_state(new_state) && !window_state(state))
        wm->show_windows();
    else if(!window_state(new_state) && window_state(state))
        wm->hide_windows();

    int old_state = state;
    state = new_state;

    pal->load();    // restore old palette

    if(playing_state(state) &&  !(dev & EDIT_MODE))
        wm->set_mouse_shape(cache.img(c_target)->copy(), 8, 8);
    else
        wm->set_mouse_shape(cache.img(c_normal)->copy(), 1, 1);

    if(old_state == SCENE_STATE && new_state != SCENE_STATE)
    {
        d = 1;
        scene_director.set_abort(0);   // don't skip any more scene stuff
    }
    else if(new_state == SCENE_STATE && old_state != SCENE_STATE)
        d = 1;

    if(d)
        draw(state == SCENE_STATE);

    dev_cont->set_state(new_state);
}

void Game::joy_calb(event &ev)
{
    if(!joy_win) // make sure the joystick calibration window is open
        return;

    if(ev.type == EV_SPURIOUS) // spurious means we should update our status
    {
        int b1, b2, b3 = 0, x, y;
        joy_status(b1, b2, b2, x, y);
        int but = b1|b2|b3;
        if(x > 0) x = 1; else if(x < 0) x = -1;
        if(y > 0) y = 1; else if(y < 0) y = -1;
        if(but) but = 1;
        int dx = 20, dy = 5;
        image *jim = cache.img(joy_picts[but * 9+(y + 1)*3 + x + 1]);
        joy_win->screen->bar(dx, dy, dx + jim->width()+6, dy + jim->height()+6, wm->black());
        jim->put_image(joy_win->screen, dx + 3, dy + 3);

        if(but)
            joy_calibrate();
    }
    else if(ev.type == EV_MESSAGE && ev.message.id == JOY_OK)
    {
        wm->close_window(joy_win);
        joy_win = NULL;
        set_state(MENU_STATE);
    }
}

void Game::menu_select(event &ev)
{
    state = DEV_MOUSE_RELEASE;
    if(top_menu)
    {
#if 0
        wm->push_event(new event(men_mess[((pick_list *)ev.message.data)->get_selection()], NULL));
        wm->close_window(top_menu);
        top_menu = NULL;
#endif
    }
}


void Game::show_help(char const *st)
{
    strcpy(help_text, st);
    help_text_frames = 0;
    refresh = 1;
}

void Game::draw_value(image *screen, int x, int y, int w, int h,
                      int val, int max)
{
    screen->bar(x, y, x + w - 1, y + h, wm->dark_color());
    screen->bar(x, y + 1, x + w * val / max, y + h - 1, wm->bright_color());
}


void Game::set_level(level *nl)
{
    if(current_level)
        delete current_level;
    current_level = nl;
}

void Game::load_level(char const *name)
{
    if(current_level)
      delete current_level;

    bFILE *fp = open_file(name, "rb");

    if(fp->open_failure())
    {
        delete fp;
        current_level = new level(100, 100, name);
        char msg[100];
        sprintf(msg, symbol_str("no_file"), name);
        show_help(msg);
    }
    else
    {
        spec_directory sd(fp);
        current_level = new level(&sd, fp, name);
        delete fp;
    }

    base->current_tick=(current_level->tick_counter()&0xff);

    current_level->level_loaded_notify();
    the_game->help_text_frames = 0;
}

int Game::done()
{
  return finished || (main_net_cfg && main_net_cfg->restart_state());
}

void Game::end_session()
{
  finished = true;
  if(main_net_cfg)
  {
    delete main_net_cfg;
    main_net_cfg = NULL;
  }
}

void Game::put_block_fg(int x, int y, trans_image *im)
{
  for(view *f = first_view; f; f = f->next)
  {
    if(f->drawable())
    {
      int xoff = f->xoff(), yoff = f->yoff(), viewx1 = f->cx1, viewy1 = f->cy1, viewx2 = f->cx2, viewy2 = f->cy2;
      if(xoff / ftile_width()>x || xoff / ftile_width()+(viewx2 - viewx1)/ftile_width()+1 < x ||
      yoff / ftile_height()>y || yoff / ftile_height()+(viewy2 - viewy1)/ftile_height()+1 < y) return;
      short cx1, cy1, cx2, cy2;
      screen->get_clip(cx1, cy1, cx2, cy2);
      screen->set_clip(viewx1, viewy1, viewx2, viewy2);
      im->put_image(screen, (x - xoff / ftile_width())*ftile_width()+viewx1 - xoff % ftile_width(),
            (y - yoff / ftile_height())*ftile_height()+viewy1 - yoff % ftile_height());
      screen->set_clip(cx1, cy1, cx2, cy2);
    }
  }
}

void Game::put_block_bg(int x, int y, image *im)
{
  for(view *f = first_view; f; f = f->next)
  {
    if(f->drawable())
    {
      int xoff = f->xoff(), yoff = f->yoff(), viewx1 = f->cx1, viewy1 = f->cy1, viewx2 = f->cx2, viewy2 = f->cy2;
      int xo = xoff * bg_xmul / bg_xdiv;
      int yo = yoff * bg_ymul / bg_ydiv;

      if(xo / btile_width()>x || xo / btile_width()+(viewx2 - viewx1)/btile_width()+1 < x ||
      yo / btile_height()>y || yo / btile_height()+(viewy2 - viewy1)/btile_height()+1 < y) return;
      short cx1, cy1, cx2, cy2;
      screen->get_clip(cx1, cy1, cx2, cy2);
      screen->set_clip(viewx1, viewy1, viewx2, viewy2);
      im->put_image(screen, (x - xo / btile_width())*btile_width()+viewx1 - xo % btile_width(),
            (y - yo / btile_height())*btile_height()+viewy1 - yo % btile_height(), 0);
      screen->set_clip(cx1, cy1, cx2, cy2);
    }
  }
}

int need_delay = 1;

void Game::dev_scroll()
{
  need_delay = 0;
  if(dev)
  {
    int xmargin, ymargin;
    if(xres > 400)
    {
      xmargin = 20;
      ymargin = 10;
    }
    else
    {
      xmargin = 10;
      ymargin = 5;
    }

    int xs, ys;
    if(mousex < xmargin &&  dev_cont->ok_to_scroll()) xs = -18;
    else if(mousex>(screen->width()-xmargin) &&  dev_cont->ok_to_scroll()) xs = 18;
    else if(wm->key_pressed(JK_LEFT) && !last_input && !dev_cont->need_arrows())
      xs = -18;
    else if(wm->key_pressed(JK_RIGHT) && !last_input && !dev_cont->need_arrows())
      xs = 18;
    else xs = 0;


    if(mousey < ymargin && dev_cont->ok_to_scroll()) ys = -18;
    else if(mousey>(screen->height()-ymargin) &&  dev_cont->ok_to_scroll()) ys = 18;
    else if(wm->key_pressed(JK_UP) && !last_input)
      ys = -18;
    else if(wm->key_pressed(JK_DOWN) && !last_input)
      ys = 18;
    else ys = 0;


    if(xs || ys)
    {
      need_delay = 1;
      if(dev & MAP_MODE)
      {
    map_xoff += xs / 2;
    map_yoff += ys / 2;
    if(map_xoff < 0) map_xoff = 0;
    if(map_yoff < 0) map_yoff = 0;
      }
      else
      {
    for(view *v = first_view; v; v = v->next)
    {
      if(xs >= 0 || v->xoff()>0)
        v->pan_x += xs;
      if(ys >= 0 || v->yoff()>0)
        v->pan_y += ys;
    }
      }
      refresh = 1;
    }
  }
}

void remap_area(image *screen, int x1, int y1, int x2, int y2, uint8_t *remap)
{
    screen->lock();

    uint8_t *sl = (uint8_t *)screen->scan_line(y1) + x1;
    int step = screen->width() - (x2 - x1 + 1);

    for(int y = y1; y <= y2; y++)
    {
        for(int x = x1; x <= x2; x++)
        {
            uint8_t c = *sl;
            *(sl++) = remap[c];
        }
        sl += step;
    }
    screen->unlock();
}

static void post_render()
{
  if(DEFINEDP(symbol_function(l_post_render)))
  {
    screen->dirt_off();
    clear_tmp();
    eval_function((lisp_symbol *)l_post_render, NULL);
    clear_tmp();
    screen->dirt_on();
  }
}

void Game::draw_map(view *v, int interpolate)
{
  backtile *bt;
  int x1, y1, x2, y2, x, y, xo, yo, nxoff, nyoff;
  short cx1, cy1, cx2, cy2;
  screen->get_clip(cx1, cy1, cx2, cy2);

  if(!current_level || state == MENU_STATE)
  {
    if(title_screen >= 0)
    {
      if(state == SCENE_STATE)
        screen->set_clip(v->cx1, v->cy1, v->cx2, v->cy2);
      image *tit = cache.img(title_screen);
      tit->put_image(screen, screen->width()/2 - tit->width()/2,
                    screen->height()/2 - tit->height()/2);
      if(state == SCENE_STATE)
        screen->set_clip(cx1, cy1, cx2, cy2);
      wm->flush_screen();
    }
    return;
  }

  refresh = 0;


  // save the dirty rect routines some work by markinging evrything in the
  // view area dirty alreadt

  if(small_render)
    screen->add_dirty(v->cx1, v->cy1, (v->cx2 - v->cx1 + 1)*2 + v->cx1, v->cy1+(v->cy2 - v->cy1 + 1)*2);
  else
    screen->add_dirty(v->cx1, v->cy1, v->cx2, v->cy2);

  if(v->draw_solid != -1)      // fill the screen and exit..
  {
    int c = v->draw_solid;
    screen->lock();
    for(int y = v->cy1; y <= v->cy2; y++)
      memset(screen->scan_line(y)+v->cx1, c, v->cx2 - v->cx1 + 1);
    screen->unlock();
    v->draw_solid = -1;
    return;
  }

  int32_t old_cx1 = 0, old_cy1 = 0, old_cx2 = 0, old_cy2 = 0;   // if we do a small render, we need to restore these
  image *old_screen = NULL;
  if(small_render && (dev & DRAW_LIGHTS))  // cannot do this if we skip lighting
  {
    old_cx1 = v->cx1;
    old_cy1 = v->cy1;
    old_cx2 = v->cx2;
    old_cy2 = v->cy2;

    v->cx1 = 0;
    v->cy1 = 0;
    v->cx2 = small_render->width()-1;
    v->cy2 = small_render->height()-1;

    old_screen = screen;
    screen = small_render;
  } else
    screen->dirt_off();


  int32_t xoff, yoff;
  if(interpolate)
  {
    xoff = v->interpolated_xoff();
    yoff = v->interpolated_yoff();
  } else
  {
    xoff = v->xoff();
    yoff = v->yoff();
  }

//  if(xoff > max_xoff) xoff = max_xoff;
//  if(yoff > max_yoff) yoff = max_yoff;

  current_vxadd = xoff - v->cx1;
  current_vyadd = yoff - v->cy1;

  screen->set_clip(v->cx1, v->cy1, v->cx2, v->cy2);

  nxoff = xoff * bg_xmul / bg_xdiv;
  nyoff = yoff * bg_ymul / bg_ydiv;


  x1 = nxoff / btile_width(); y1 = nyoff / btile_height();
  x2 = x1+(v->cx2 - v->cx1 + btile_width())/btile_width();
  y2 = y1+(v->cy2 - v->cy1 + btile_height())/btile_height();


  xo = v->cx1 - nxoff % btile_width();
  yo = v->cy1 - nyoff % btile_height();

  int xinc, yinc, draw_x, draw_y;


  if(!(dev & MAP_MODE) && (dev & DRAW_BG_LAYER))
  {
    xinc = btile_width();
    yinc = btile_height();

    int bh = current_level->background_height(), bw = current_level->background_width();
    uint16_t *bl;
    for(draw_y = yo, y = y1; y <= y2; y++, draw_y += yinc)
    {
      if(y >= bh)
        bl = NULL;
      else
        bl = current_level->get_bgline(y)+x1;

      for(x = x1, draw_x = xo; x <= x2; x++, draw_x += xinc)
      {
    if(x < bw && y < bh)
    {
          bt = get_bg(*bl);
      bl++;
    }
    else bt = get_bg(0);

        bt->im->put_image(screen, draw_x, draw_y);
//        if(!(dev & EDIT_MODE) && bt->next)
//      current_level->put_bg(x, y, bt->next);
      }
    }
  }

//  if(!(dev & EDIT_MODE))
//    server_check();

  uint8_t rescan = 0;

    int fw, fh;

    if(dev & MAP_MODE)
    {
      fw = AUTOTILE_WIDTH;
      fh = AUTOTILE_HEIGHT;
      if(dev & EDIT_MODE)
      {
    x1 = map_xoff;
    y1 = map_yoff;
      } else
      {
    if(v->focus)
    {
      x1 = v->focus->x / ftile_width()-(v->cx2 - v->cx1)/fw / 2;
      y1 = v->focus->y / ftile_height()-(v->cy2 - v->cy1)/fh / 2;
    } else x1 = y1 = 0;
      }
      if(x1 > 0)
        xo = v->cx1-((v->focus->x * fw / ftile_width()) %fw);
      else xo = v->cx1;
      if(y1 > 0)
        yo = v->cy1-((v->focus->y * fh / ftile_height()) %fh);
      else yo = v->cy1;
    } else
    {
      fw = ftile_width();
      fh = ftile_height();
      x1=(xoff)/fw; y1=(yoff)/fh;
      xo = v->cx1 - xoff % fw;
      yo = v->cy1 - yoff % fh;

    }
    if(x1 < 0) x1 = 0;
    if(y1 < 0) y1 = 0;

    x2 = x1+(v->cx2 - v->cx1 + fw)/fw;
    y2 = y1+(v->cy2 - v->cy1 + fh)/fh;
    if(x2 >= current_level->foreground_width())
      x2 = current_level->foreground_width()-1;
    if(y2 >= current_level->foreground_height())
      y2 = current_level->foreground_height()-1;


    xinc = fw;
    yinc = fh;

  if(dev & DRAW_FG_LAYER)
  {
    short ncx1, ncy1, ncx2, ncy2;
    screen->get_clip(ncx1, ncy1, ncx2, ncy2);

    int scr_w = screen->width();
    if(dev & MAP_MODE)
    {
      if(dev & EDIT_MODE)
        screen->clear(wm->bright_color());
      else
        screen->clear(wm->black());
      screen->lock();
      for(y = y1, draw_y = yo; y <= y2; y++, draw_y += yinc)
      {
    if(!(draw_y < ncy1 ||draw_y + yinc >= ncy2))
    {
      uint16_t *cl = current_level->get_fgline(y)+x1;
      uint8_t *sl1 = screen->scan_line(draw_y)+xo;
      for(x = x1, draw_x = xo; x <= x2; x++, cl++, sl1 += xinc, draw_x += xinc)
      {
        if(!(draw_x < ncx1 || draw_x + xinc >= ncx2))
        {
          int fort_num;
//          if(*cl & 0x8000 || (dev & EDIT_MODE))
            fort_num = fgvalue(*cl);
//          else fort_num = 0;

          uint8_t *sl2 = get_fg(fort_num)->micro_image->scan_line(0);
          uint8_t *sl3 = sl1;
          memcpy(sl3, sl2, AUTOTILE_WIDTH); sl2 += AUTOTILE_WIDTH; sl3 += scr_w;
          memcpy(sl3, sl2, AUTOTILE_WIDTH); sl2 += AUTOTILE_WIDTH; sl3 += scr_w;
          memcpy(sl3, sl2, AUTOTILE_WIDTH);
        }
      }
    }
      }
      screen->unlock();

      if(dev & EDIT_MODE)
        current_level->draw_areas(v);
    } else
    {

      int fg_h = current_level->foreground_height(), fg_w = current_level->foreground_width();

      for(y = y1, draw_y = yo; y <= y2; y++, draw_y += yinc)
      {

    uint16_t *cl;
    if(y < fg_h)
      cl = current_level->get_fgline(y)+x1;
    else cl = NULL;
    uint8_t *sl1 = draw_y < ncy1 ? 0 : screen->scan_line(draw_y)+xo;

    for(x = x1, draw_x = xo; x <= x2; x++, draw_x += xinc, cl++, sl1 += xinc)
    {
      if(x < fg_w && y < fg_h)
      {
        if(above_tile(*cl))
        rescan = 1;
        else
        {
          int fort_num = fgvalue(*cl);
          if(fort_num != BLACK)
          {
        if(draw_y < ncy1 || draw_y + yinc >= ncy2 || draw_x < ncx1 || draw_x + xinc >= ncx2)
            get_fg(fort_num)->im->put_image(screen, draw_x, draw_y);
        else
            get_fg(fort_num)->im->put_image_offseted(screen, sl1);

        if(!(dev & EDIT_MODE))
            *cl|=0x8000;      // mark as has - been - seen
          }
        }
      }
    }
      }
    }
  }

  int32_t ro = rand_on;
  if(dev & DRAW_PEOPLE_LAYER)
  {
    if(interpolate)
      current_level->interpolate_draw_objects(v);
    else
      current_level->draw_objects(v);
  }

//  if(!(dev & EDIT_MODE))
//    server_check();

  if(!(dev & MAP_MODE))
  {

    draw_panims(v);

    if(dev & DRAW_FG_LAYER && rescan)
    {
      for(y = y1, draw_y = yo; y <= y2; y++, draw_y += yinc)
      {
    uint16_t *cl = current_level->get_fgline(y)+x1;
    for(x = x1, draw_x = xo; x <= x2; x++, draw_x += xinc, cl++)
    {
      if(above_tile(*cl))
      {
        int fort_num = fgvalue(*cl);
        if(fort_num != BLACK)
        {
          if(dev & DRAW_BG_LAYER)
          get_fg(fort_num)->im->put_image(screen, draw_x, draw_y);
          else
          get_fg(fort_num)->im->put_image_filled(screen, draw_x, draw_y, 0);

          if(!(dev & EDIT_MODE))
          current_level->mark_seen(x, y);
          else
          {
        screen->line(draw_x, draw_y, draw_x + xinc, draw_y + yinc, wm->bright_color());
        screen->line(draw_x + xinc, draw_y, draw_x, draw_y + yinc, wm->bright_color());
          }
        }
      }
    }
      }
    }


    if(dev & DRAW_FG_BOUND_LAYER)
    {
      int b = wm->bright_color();
      int fg_h = current_level->foreground_height(), fg_w = current_level->foreground_width();

      for(y = y1, draw_y = yo; y <= y2; y++, draw_y += yinc)
      {
    uint16_t *cl;
    if(y < fg_h)
      cl = current_level->get_fgline(y)+x1;
    else cl = NULL;
    for(x = x1, draw_x = xo; x <= x2; x++, draw_x += xinc, cl++)
    {
      if(x < fg_w && y < fg_h)
      {
        int fort_num = fgvalue(*cl);
        if(fort_num != BLACK)
        {
          point_list *p = get_fg(fort_num)->points;
          uint8_t *d = p->data;
          if(p->tot)
          {
        for(int i = 1; i < p->tot; i++)
        {
          d += 2;
          screen->line(draw_x+*(d - 2), draw_y+*(d - 1), draw_x+*d, draw_y+*(d + 1), b);
        }
        screen->line(draw_x+*d, draw_y+*(d - 1), draw_x + p->data[0], draw_y + p->data[1], b);
          }
        }
      }
    }
      }
    }

//    if(!(dev & EDIT_MODE))
//      server_check();

    if(dev & DRAW_HELP_LAYER)
    {
      if(help_text_frames >= 0)
      {
    int color;

    if(help_text_frames < 10)
        color = 2;
    else
        color = 2+(help_text_frames - 10);

    int x1 = v->cx1, y1 = v->cy1, x2 = v->cx2, y2 = v->cy1 + wm->font()->height()+10;

    remap_area(screen, x1, y1, x2, y2, white_light + 40 * 256);
    screen->bar(x1, y1, x2, y1, color);
    screen->bar(x1, y2, x2, y2, color);

    wm->font()->put_string(screen, x1 + 5, y1 + 5,
                   help_text, color);
    if(color > 30)
        help_text_frames = -1;
    else help_text_frames++;

      }
    }

    if(dev_cont)
    dev_cont->dev_draw(v);
    if(cache.in_use())
    cache.img(vmm_image)->put_image(screen, v->cx1, v->cy2 - cache.img(vmm_image)->height()+1);

    if(dev & DRAW_LIGHTS)
    {
      if(small_render)
      {
    double_light_screen(screen, xoff, yoff, white_light, v->ambient, old_screen, old_cx1, old_cy1);

    v->cx1 = old_cx1;
    v->cy1 = old_cy1;
    v->cx2 = old_cx2;
    v->cy2 = old_cy2;
    screen = old_screen;
      } else
      {
    screen->dirt_on();
    if(xres * yres <= 64000)
          light_screen(screen, xoff, yoff, white_light, v->ambient);
    else light_screen(screen, xoff, yoff, white_light, 63);            // no lighting for hi - rez
      }

    } else
      screen->dirt_on();



  }  else
    screen->dirt_on();

  rand_on = ro;                // restore random start in case in draw funs moved it
                               // ... not every machine will draw the same thing

  post_render();

  screen->set_clip(cx1, cy1, cx2, cy2);




  if(playing_state(state))        // draw stuff outside the clipping region
    v->draw_character_damage();

  if(profiling())
    profile_update();

  sbar.draw_update();
}

void Game::put_fg(int x, int y, int type)
{
  if(current_level->get_fg(x, y)!=type)
  {
    current_level->put_fg(x, y, type);
    for(view *f = first_view; f; f = f->next)
      if(f->drawable())
        draw_map(f);
  }
}

void Game::put_bg(int x, int y, int type)
{
  if(current_level->get_bg(x, y)!=type)
  {
    current_level->put_bg(x, y, type);
    for(view *f = first_view; f; f = f->next)
      if(f->drawable())
        draw_map(f);
  }
}

int Game::in_area(event &ev, int x1, int y1, int x2, int y2)
{
  return (last_demo_mx >= x1 && last_demo_mx <= x2 &&
      last_demo_my >= y1 && last_demo_my <= y2);
}

void Game::request_level_load(char *name)
{
  strcpy(req_name, name);
}

extern int start_doubled;

void fade_in(image *im, int steps)
{
  palette *old_pal = pal->copy();
  int i;
  if(im)
  {
    screen->clear();
    im->put_image(screen, (xres + 1)/2 - im->width()/2, (yres + 1)/2 - im->height()/2);
  }

  for(i = 0; i < steps; i++)
  {
    uint8_t *sl1=(uint8_t *)pal->addr();
    uint8_t *sl2=(uint8_t *)old_pal->addr();
    int j;
    int v=(i + 1)*256 / steps;
    for(j = 0; j < 256; j++)
    {
      *(sl1)=((int)*(sl2))*v / 256;  sl1++; sl2++;
      *(sl1)=((int)*(sl2))*v / 256;  sl1++; sl2++;
      *(sl1)=((int)*(sl2))*v / 256;  sl1++; sl2++;
    }
    pal->load();
    wm->flush_screen();
    milli_wait(25);
  }
  delete pal;
  pal = old_pal;
}

void fade_out(int steps)
{
  palette *old_pal = pal->copy();
  int i;
  for(i = 0; i < steps; i++)
  {
    uint8_t *sl1=(uint8_t *)pal->addr();
    uint8_t *sl2=(uint8_t *)old_pal->addr();
    int j;
    int v=(steps - i)*256 / steps;
    for(j = 0; j < 256; j++)
    {
      *(sl1)=((int)*(sl2))*v / 256;  sl1++; sl2++;
      *(sl1)=((int)*(sl2))*v / 256;  sl1++; sl2++;
      *(sl1)=((int)*(sl2))*v / 256;  sl1++; sl2++;
    }
    pal->load();
    wm->flush_screen();
    milli_wait(25);
  }
  screen->clear();
  wm->flush_screen();
  delete pal;
  pal = old_pal;

  pal->load();
}

int text_draw(int y, int x1, int y1, int x2, int y2, char const *buf, JCFont *font, uint8_t *cmap, char color);

void do_title()
{
    if(cdc_logo == -1)
        return;

    if(sound_avail & MUSIC_INITIALIZED)
    {
        if(current_song)
        {
            current_song->stop();
            delete current_song;
        }
        current_song = new song("music/intro.hmi");
        current_song->play(music_volume);
    }

    void *logo_snd = symbol_value(make_find_symbol("LOGO_SND"));

    if(DEFINEDP(logo_snd) && (sound_avail & SFX_INITIALIZED))
        cache.sfx(lnumber_value(logo_snd))->play(sfx_volume);

    // This must be a dynamic allocated image because if it
    // is not and the window gets closed during do_title, then
    // exit() will try to delete (through the desctructor of
    // image_list in image.cpp) the image on the stack -> boom.
    image *blank = new image(2, 2);
    blank->clear();
    wm->set_mouse_shape(blank->copy(), 0, 0); // hide mouse
    delete blank;
    fade_in(cache.img(cdc_logo), 32);

    milli_wait(400);

    void *space_snd = symbol_value(make_find_symbol("SPACE_SND"));

    fade_out(32);
    milli_wait(100);

    int i;
    char *str = lstring_value(eval(make_find_symbol("plot_start")));

    bFILE *fp = open_file("art/smoke.spe", "rb");
    if(!fp->open_failure())
    {
        spec_directory sd(fp);
        palette *old_pal = pal;
        pal = new palette(sd.find(SPEC_PALETTE), fp);
        pal->shift(1);

        image *gray = new image(sd.find("gray_pict"), fp);
        image *smoke[5];

        char nm[20];
        for(i = 0; i < 5; i++)
        {
            sprintf(nm, "smoke%04d.pcx", i + 1);
            smoke[i] = new image(sd.find(nm), fp);
        }

        screen->clear();
        pal->load();

        int dx = (xres + 1) / 2 - gray->width() / 2, dy = (yres + 1) / 2 - gray->height() / 2;
        gray->put_image(screen, dx, dy);
        smoke[0]->put_image(screen, dx + 24, dy + 5);

        fade_in(NULL, 16);
        uint8_t cmap[32];
        for(i = 0; i < 32; i++)
        cmap[i] = pal->find_closest(i * 256 / 32, i * 256 / 32, i * 256 / 32);

        event ev;
        ev.type = EV_SPURIOUS;
        time_marker start;

        for(i = 0; i < 320 && (ev.type != EV_KEY && ev.type != EV_MOUSE_BUTTON); i++)
        {
            gray->put_image(screen, dx, dy);
            smoke[i % 5]->put_image(screen, dx + 24, dy + 5);
            text_draw(205 - i, dx + 15, dy, dx + 320 - 15, dy + 199, str, wm->font(), cmap, wm->bright_color());
            wm->flush_screen();
            time_marker now;

            while(now.diff_time(&start) < 0.18)
            {
                milli_wait(20); // ECS - Added the wait, so CPU utilization stays low during the story
                now.get_time();
            }

            start.get_time();

            while(wm->event_waiting() && ev.type != EV_KEY)
            {
                wm->get_event(ev);
            }
            if((i % 5) == 0 && DEFINEDP(space_snd) && (sound_avail & SFX_INITIALIZED))
            {
                cache.sfx(lnumber_value(space_snd))->play(sfx_volume * 90 / 127);
            }
        }

        the_game->reset_keymap();

        fade_out(16);

        for(i = 0; i < 5; i++)
            delete smoke[i];
        delete gray;
        delete pal;
        pal = old_pal;
    }
    delete fp;

    if(title_screen >= 0)
        fade_in(cache.img(title_screen), 32);
}

extern int start_edit;

void Game::request_end()
{
  req_end = 1;
}

extern void fast_load_start_recording(char *name);
extern void fast_load_stop_recording();
extern void fast_load_start_reloading(char *name);
extern void fast_load_stop_reloading();

Game::Game(int argc, char **argv)
{
  int i;
  req_name[0]=0;
  bg_xmul = bg_ymul = 1;
  bg_xdiv = bg_ydiv = 8;
  last_input = NULL;
  current_automap = NULL;
  current_level = NULL;
  refresh = 1;
  the_game = this;
  top_menu = joy_win = NULL;
  old_view = first_view = NULL;
  nplayers = 1;

  help_text_frames = 0;
  strcpy(help_text, "");


  for(i = 1; i < argc; i++)
    if(!strcmp(argv[i], "-no_delay"))
    {
      no_delay = 1;
      dprintf("Frame delay off (-nodelay)\n");
    }


  image_init();
  zoom = 15;
  no_delay = 0;

  if(get_option("-use_joy"))
  {
    has_joystick = joy_init(argc, argv);
    dprintf("Joystick : ");
    if(has_joystick) dprintf("detected\n");
    else dprintf("not detected\n");
  }
  else has_joystick = 0;

//    ProfilerInit(collectDetailed, bestTimeBase, 2000, 200); //prof
    char *fastpath;
    fastpath = (char *)malloc(strlen(get_save_filename_prefix()) + 12 + 1);
    sprintf(fastpath, "%sfastload.dat", get_save_filename_prefix());
    fast_load_start_recording(fastpath);
    load_data(argc, argv);
    fast_load_stop_recording();
    free(fastpath);
//    ProfilerDump("\pabuse.prof");  //prof
//    ProfilerTerm();

  get_key_bindings();

  reset_keymap();                   // we think all the keys are up right now
  finished = false;

  calc_light_table(pal);

  if(current_level == NULL && net_start())  // if we joined a net game get level from server
  {
    if(!request_server_entry())
    {
      exit(0);
    }
    net_reload();
//    load_level(NET_STARTFILE);
  }


  set_mode(19, argc, argv);
  if(get_option("-2") && (xres < 639 || yres < 399))
  {
    close_graphics();
    fprintf(stderr, "Resolution must be > 640x400 to use -2 option\n");
    exit(0);
  }
  pal->load();

  recalc_local_view_space();   // now that we know what size the screen is...

  dark_color = get_color(cache.img(window_colors)->pixel(2, 0));
  bright_color = get_color(cache.img(window_colors)->pixel(0, 0));
  med_color = get_color(cache.img(window_colors)->pixel(1, 0));

  morph_dark_color = get_color(cache.img(window_colors)->pixel(2, 1));
  morph_bright_color = get_color(cache.img(window_colors)->pixel(0, 1));
  morph_med_color = get_color(cache.img(window_colors)->pixel(1, 1));
  morph_sel_frame_color = pal->find_closest(255, 255, 0);
  light_connection_color = morph_sel_frame_color;

  if(NILP(symbol_value(l_default_font)))
  {
    printf("No font defined, set symbol default-font to an image name\n");
    exit(0);
  }
  int font_pict;
  if(big_font_pict != -1)
  {
    if(small_font_pict != -1)
    {
      if(xres/(start_doubled ? 2 : 1)>400)
      {
    font_pict = big_font_pict;
      }
      else font_pict = small_font_pict;
    } else font_pict = big_font_pict;
  } else font_pict = small_font_pict;

  if(console_font_pict == -1) console_font_pict = font_pict;
  game_font = new JCFont(cache.img(font_pict));

  console_font = new JCFont(cache.img(console_font_pict));

  wm = new WindowManager(screen, pal, bright_color,
                         med_color, dark_color, game_font);

  delete stat_man;  // move to a graphical status manager
  gui_status_manager *gstat = new gui_status_manager();
  gstat->set_window_title("status");
  stat_man = gstat;


  chat = new chat_console( console_font, 50, 6);

  if(!wm->has_mouse())
  {
    close_graphics();
    image_uninit();
    printf("No mouse driver detected, please rectify.\n");
    exit(0);
  }


  gamma_correct(pal);

  if(main_net_cfg == NULL || (main_net_cfg->state != net_configuration::SERVER &&
                 main_net_cfg->state != net_configuration::CLIENT))
  {
    if(!start_edit && !net_start())
      do_title();
  } else if(main_net_cfg && main_net_cfg->state == net_configuration::SERVER)
  {
    the_game->load_level(level_file);
    start_running = 1;
  }


  dev|= DRAW_FG_LAYER | DRAW_BG_LAYER | DRAW_PEOPLE_LAYER | DRAW_HELP_LAYER | DRAW_LIGHTS | DRAW_LINKS;

  if(dev & EDIT_MODE)
    set_frame_size(0);
//  do_intro();
  state = START_STATE;         // first set the state to one that has windows


  if(start_running)
    set_state(RUN_STATE);
  else
  {
    screen->clear();
    if(title_screen >= 0)
    {
      image *tit = cache.img(title_screen);
      tit->put_image(screen, screen->width()/2 - tit->width()/2,
                    screen->height()/2 - tit->height()/2);
    }
    set_state(MENU_STATE);   // then go to menu state so windows will turn off
  }
}



time_marker *led_last_time = NULL, *fps_mark_start = NULL;
double avg_fps = 15.0, possible_fps = 15.0;

void Game::toggle_delay()
{
  no_delay=!no_delay;
  if(no_delay)
    show_help(symbol_str("delay_off"));
  else show_help(symbol_str("delay_on"));
  avg_fps = possible_fps = 15.0;
}

void Game::show_time()
{
  if(first_view && fps_on)
  {
    char str[10];
    sprintf(str, "%ld", (long)(avg_fps * 10.0));
    console_font->put_string(screen, first_view->cx1, first_view->cy1, str);

    sprintf(str, "%d", total_active);
    console_font->put_string(screen, first_view->cx1, first_view->cy1 + 10, str);
  }
}

void Game::update_screen()
{
  if(state == HELP_STATE)
    draw_help();
  else if(current_level)
  {
    if(!(dev & EDIT_MODE) || refresh)
    {
      view *f = first_view;
      current_level->clear_active_list();
      for(; f; f = f->next)
      {
    if(f->focus)
    {
      int w, h;

      w=(f->cx2 - f->cx1 + 1);
      h=(f->cy2 - f->cy1 + 1);

      total_active += current_level->add_drawables(f->xoff()-w / 4, f->yoff()-h / 4,
                             f->xoff()+w + w / 4, f->yoff()+h + h / 4);

    }
      }

      for(f = first_view; f; f = f->next)
      {
        if(f->drawable())
    {
      if(interpolate_draw)
      {
            draw_map(f, 1);
        wm->flush_screen();
      }
          draw_map(f, 0);
    }
      }
      if(current_automap)
      current_automap->draw();
    }
    if(state == PAUSE_STATE)
    {
      for(view *f = first_view; f; f = f->next)
        cache.img(pause_image)->put_image(screen, (f->cx1 + f->cx2)/2 - cache.img(pause_image)->width()/2,
                   f->cy1 + 5, 1);
    }

    show_time();
  }

  if(state == RUN_STATE && cache.prof_is_on())
    cache.prof_poll_end();

  wm->flush_screen();

}

void Game::do_intro()
{

}

int Game::calc_speed()
{
    int ret = 0;
    if(fps_mark_start)
    {
        time_marker t;

        // find average fps for last 10 frames
        double td = t.diff_time(fps_mark_start);
        if(td < 0.001)     // something is rotten in the state of demark
            td = 0.001;

        avg_fps = avg_fps * 9.0 / 10.0 + 1.0/(td * 10.0);
        possible_fps = possible_fps * 9.0 / 10.0 + 1.0/(td * 10.0);

        if(avg_fps > 14)
        {
            if(massive_frame_panic > 20)
                massive_frame_panic = 20;
            else if(massive_frame_panic)
                massive_frame_panic--;
        }

        if(avg_fps > 15 && ((dev & EDIT_MODE)==0 || need_delay))
        {
            frame_panic = 0;
            int32_t stime=(int32_t)((1 / 15.0 - 1.0 / possible_fps)*1000.0);
            if(stime > 0 && !no_delay)
            {
                milli_wait(stime);
                avg_fps -= 1.0/(td * 10.0);   // subtract out old estimate

                time_marker t;

                // find average fps for last 10 frames
                double td = t.diff_time(fps_mark_start);
                if(td < 0.00001)     // something is rotten in the state of demark
                    td = 0.00001;

                avg_fps += 1.0/(td * 10.0);       // add in new estimate
            }
        }
        else if(avg_fps < 14)
        {
            if(avg_fps < 10)
                massive_frame_panic++;
            frame_panic++;
            ret = 1;
        }
        else if(dev & EDIT_MODE)
        {
            // ECS - Added this case and the wait.  It's a cheap hack to assure that
            // we don't exceed 30FPS in edit mode and hog the CPU.
            milli_wait(33);
        }

        delete fps_mark_start;
    }
    fps_mark_start = new time_marker;
    return ret;
}

extern int start_edit;

void Game::get_input()
{
    event ev;
    idle_ticks++;
    while(event_waiting())
    {
        get_event(ev);

        if(ev.type == EV_MOUSE_MOVE)
        {
            last_input = ev.window;
        }
        // don't process repeated keys in the main window, it will slow down the game to handle such
        // useless events. However in other windows it might be useful, such as in input windows
        // where you want to repeatedly scroll down...
        if(ev.type != EV_KEY || !key_down(ev.key) || ev.window || (dev & EDIT_MODE))
        {
            if(ev.type == EV_KEY)
            {
                set_key_down(ev.key, 1);
                if(playing_state(state))
                {
                    if(ev.key < 256)
                    {
                        if(chat && chat->chat_event(ev))
                            base->packet.write_uint8(SCMD_CHAT_KEYPRESS);
                        else
                            base->packet.write_uint8(SCMD_KEYPRESS);
                    }
                    else
                        base->packet.write_uint8(SCMD_EXT_KEYPRESS);
                    base->packet.write_uint8(client_number());
                    if(ev.key > 256)
                        base->packet.write_uint8(ev.key - 256);
                    else
                        base->packet.write_uint8(ev.key);
                }
            }
            else if(ev.type == EV_KEYRELEASE)
            {
                set_key_down(ev.key, 0);
                if(playing_state(state))
                {
                    if(ev.key < 256)
                        base->packet.write_uint8(SCMD_KEYRELEASE);
                    else
                        base->packet.write_uint8(SCMD_EXT_KEYRELEASE);
                    base->packet.write_uint8(client_number());
                    if(ev.key > 255)
                        base->packet.write_uint8(ev.key - 256);
                    else
                        base->packet.write_uint8(ev.key);
                }
            }

            if((dev & EDIT_MODE) || start_edit || ev.type == EV_MESSAGE)
            {
                dev_cont->handle_event(ev);
            }

            view *v = first_view;
            for(; v; v = v->next)
            {
                if(v->local_player() && v->handle_event(ev))
                    ev.type = EV_SPURIOUS;       // if the event was used by the view, gobble it up
            }

            if(current_automap)
            {
                current_automap->handle_event(ev);
            }

            help_handle_event(ev);
            mousex = last_demo_mx;
            mousey = last_demo_my;

            if(ev.type == EV_MESSAGE)
            {
                switch (ev.message.id)
                {
                    case CALB_JOY:
                    {
                        if(!joy_win)
                        {
                            joy_win = wm->new_window(80, 50, -1, -1,
                                    new button(70, 9, JOY_OK, "OK",
                                    new info_field(0, 30, DEV_NULL,
                                    " Center joystick and\n"
                                    "press the fire button", NULL)),
                                    "Joystick");
                            set_state(JOY_CALB_STATE);
                        }
                    }
                    case TOP_MENU:
                    {
                        menu_select(ev);
                    } break;
                    case DEV_QUIT:
                    {
                        finished = true;
                    } break;
                }
            }
            else if(ev.type == EV_CLOSE_WINDOW && ev.window == top_menu)
            {
                wm->close_window(top_menu);
                top_menu = NULL;
            }

            switch(state)
            {
                case JOY_CALB_STATE:
                {
                    joy_calb(ev);
                } break;
                case INTRO_START_STATE:
                {
                    do_intro();
                    if(dev & EDIT_MODE)
                        set_state(RUN_STATE);
                    else
                        set_state(MENU_STATE);
                } break;
                case PAUSE_STATE:
                {
                    if(ev.type == EV_KEY && (ev.key == JK_SPACE || ev.key == JK_ENTER))
                    {
                        set_state(RUN_STATE);
                    }
                } break;
                case RUN_STATE:
                {
                    if(ev.window == NULL)
                    {
                        switch (ev.type)
                        {
                            case EV_KEY:
                            {
                                switch (ev.key)
                                {
                                    case 'm':
                                    {
                                        if(dev & MAP_MODE)
                                            dev -= MAP_MODE;
                                        else if((player_list && player_list->next) || dev & EDIT_MODE)
                                            dev |= MAP_MODE;

                                        if(!(dev & MAP_MODE))
                                        {
                                            if(dev_cont->tbw)
                                                dev_cont->toggle_toolbar();
                                            edit_mode = ID_DMODE_DRAW;
                                        }
                                        need_refresh();
                                    } break;
                                    case 'v':
                                    {
                                        wm->push_event(new event(DO_VOLUME, NULL));
                                    } break;
                                    case 'p':
                                    {
                                        if(!(dev & EDIT_MODE) && (!main_net_cfg ||
                                            (main_net_cfg->state != net_configuration::SERVER &&
                                            main_net_cfg->state != net_configuration::CLIENT)))
                                        {
                                            set_state(PAUSE_STATE);
                                        }
                                    } break;
                                    case 'S':
                                    {
                                        if(start_edit)
                                        {
                                            wm->push_event(new event(ID_LEVEL_SAVE, NULL));
                                        }
                                    } break;
                                    case JK_TAB:
                                        if(start_edit)
                                            toggle_edit_mode();
                                        need_refresh();
                                        break;
                                    case 'c':
                                    case 'C':
                                        if(chatting_enabled && (!(dev & EDIT_MODE) && chat))
                                            chat->toggle();
                                        break;
                                    case '9':
                                        dev = dev ^ PERFORMANCE_TEST_MODE;
                                        need_refresh();
                                        break;
                                }
                            } break;
                            case EV_RESIZE:
                            {
                                view *v;
                                for(v = first_view; v; v = v->next)  // see if any views need to change size
                                {
                                    if(v->local_player())
                                    {
                                        int w = (xres - 10)/(small_render ? 2 : 1);
                                        int h = (yres - 10)/(small_render ? 2 : 1);

                                        v->suggest.send_view = 1;
                                        v->suggest.cx1 = 5;
                                        v->suggest.cx2 = 5 + w;
                                        v->suggest.cy1 = 5;
                                        v->suggest.cy2 = 5 + h;
                                        v->suggest.pan_x = v->pan_x;
                                        v->suggest.pan_y = v->pan_y;
                                        v->suggest.shift_down = v->shift_down;
                                        v->suggest.shift_right = v->shift_right;
                                    }
                                }
                                draw();
                            } break;
                            case EV_REDRAW:
                            {
                                screen->add_dirty(ev.redraw.x1, ev.redraw.y1,
                                    ev.redraw.x2, ev.redraw.y2);
                            } break;
                            case EV_MESSAGE:
                            {
                                switch (ev.message.id)
                                {
                                    case RAISE_SFX:
                                    case LOWER_SFX:
                                    case RAISE_MUSIC:
                                    case LOWER_MUSIC:
                                    {
                                        if(ev.message.id == RAISE_SFX && sfx_volume != 127)
                                            sfx_volume = min(127, sfx_volume + 16);
                                        if(ev.message.id == LOWER_SFX && sfx_volume != 0)
                                            sfx_volume = max(sfx_volume - 16, 0);
                                        if(ev.message.id == RAISE_MUSIC && music_volume != 126)
                                        {
                                            music_volume = min(music_volume + 16, 127);
                                            if(current_song && (sound_avail & MUSIC_INITIALIZED))
                                                current_song->set_volume(music_volume);
                                        }

                                        if(ev.message.id == LOWER_MUSIC && music_volume != 0)
                                        {
                                            music_volume = max(music_volume - 16, 0);
                                            if(current_song && (sound_avail & MUSIC_INITIALIZED))
                                                current_song->set_volume(music_volume);
                                        }

                                        ((button *)ev.message.data)->push();
/*                                        volume_window->inm->redraw();
                                        draw_value(volume_window->screen, 2, 43,
                                                (volume_window->x2()-volume_window->x1()-1), 8, sfx_volume, 127);
                                        draw_value(volume_window->screen, 2, 94,
                                                (volume_window->x2()-volume_window->x1()-1), 8, music_volume, 127);
*/
                                        break;
                                    }
                                }
                            }
                        }
                    }
                } break;
            }
        }
    }
}


void net_send(int force = 0)
{
  if((!(dev & EDIT_MODE)) || force)
  {
    if(demo_man.state == demo_manager::PLAYING)
    {
      base->input_state = INPUT_PROCESSING;
    } else
    {



      if(!player_list->focus)
      {
    dprintf("Players have not been created\ncall create_players");
    exit(0);
      }


      view *p = player_list;
      for(; p; p = p->next)
        if(p->local_player())
      p->get_input();


      base->packet.write_uint8(SCMD_SYNC);
      base->packet.write_uint16(make_sync());

      if(base->join_list)
      base->packet.write_uint8(SCMD_RELOAD);

      //      printf("save tick %d, pk size=%d, rand_on=%d, sync=%d\n", current_level->tick_counter(),
      //         base->packet.packet_size(), rand_on, make_sync());
      send_local_request();
    }
  }
}

void net_receive()
{
  if(!(dev & EDIT_MODE) && current_level)
  {
    uint8_t buf[PACKET_MAX_SIZE + 1];
    int size;

    if(demo_man.state == demo_manager::PLAYING)
    {
      if(!demo_man.get_packet(buf, size))
        size = 0;
      base->packet.packet_reset();
      base->mem_lock = 0;
    } else
    {
      size = get_inputs_from_server(buf);
      if(demo_man.state == demo_manager::RECORDING)
    demo_man.save_packet(buf, size);
    }

    process_packet_commands(buf, size);
  }
}

void Game::step()
{
  clear_tmp();
  if(current_level)
  {
    current_level->unactivate_all();
    total_active = 0;
    for(view *f = first_view; f; f = f->next)
    {
      if(f->focus)
      {
    f->update_scroll();
    int w, h;

    w=(f->cx2 - f->cx1 + 1);
    h=(f->cy2 - f->cy1 + 1);
        total_active += current_level->add_actives(f->xoff()-w / 4, f->yoff()-h / 4,
                         f->xoff()+w + w / 4, f->yoff()+h + h / 4);
      }
    }
  }

  if(state == RUN_STATE)
  {
    if((dev & EDIT_MODE) || (main_net_cfg && (main_net_cfg->state == net_configuration::CLIENT ||
                         main_net_cfg->state == net_configuration::SERVER)))
      idle_ticks = 0;

    if(demo_man.current_state()==demo_manager::NORMAL && idle_ticks > 420 && demo_start)
    {
      idle_ticks = 0;
      set_state(MENU_STATE);
    }
    else if(!(dev & EDIT_MODE))               // if edit mode, then don't step anything
    {
      if(key_down(JK_ESC))
      {
    set_state(MENU_STATE);
    set_key_down(JK_ESC, 0);
      }
      ambient_ramp = 0;
      view *v;
      for(v = first_view; v; v = v->next)
        v->update_scroll();

      cache.prof_poll_start();
      current_level->tick();
      sbar.step();
    } else
      dev_scroll();
  } else if(state == JOY_CALB_STATE)
  {
    event ev;
    joy_calb(ev);
  } else if(state == MENU_STATE)
    main_menu();

  if((key_down('x') || key_down(JK_F4))
      && (key_down(JK_ALT_L) || key_down(JK_ALT_R))
      && confirm_quit())
    finished = true;
}

extern void *current_demo;

Game::~Game()
{
  current_demo = NULL;
  if(first_view == player_list) first_view = NULL;
  while(player_list)
  {
    view *p = player_list;
    game_object *o = p->focus;
    player_list = player_list->next;
    delete p;
    o->set_controller(NULL);
    if(current_level && o)
      current_level->delete_object(o);
    else delete o;
  }

  if(current_level) { delete current_level; current_level = NULL; }

  if(first_view != player_list)
  {
    while(player_list)
    {
      view *p = player_list;
      player_list = player_list->next;
      delete p;
    }
  }

  while(first_view)
  {
    view *p = first_view;
    first_view = first_view->next;
    delete p;
  }

  player_list = NULL;

  if(old_view)
  {
    first_view = old_view;
    while(first_view)
    {
      view *p = first_view;
      first_view = first_view->next;
      delete p;
    }
  }
  old_view = NULL;

  int i = 0;
  for(; i < total_objects; i++)
  {
    free(object_names[i]);
    delete figures[i];
  }
  free_pframes();
  if(fps_mark_start) delete fps_mark_start; fps_mark_start = NULL;
  delete pal;
  free(object_names);
  free(figures);

  free(backtiles);
  free(foretiles);
  if(total_weapons)
    free(weapon_types);

  config_cleanup();
  delete color_table;
  delete wm;
  delete game_font;
  delete big_font;
  delete console_font;
  if(total_help_screens)
    free(help_screens);

  close_graphics();
  image_uninit();
}



void Game::draw(int scene_mode)
{
    screen->add_dirty(0, 0, xres, yres);

    screen->clear();

    if(scene_mode)
    {
        char const *helpstr = "ARROW KEYS CHANGE TEXT SPEED";
        wm->font()->put_string(screen, screen->width()/2-(wm->font()->width()*strlen(helpstr))/2 + 1,
            screen->height()-wm->font()->height()-5 + 1, helpstr, wm->dark_color());
        wm->font()->put_string(screen, screen->width()/2-(wm->font()->width()*strlen(helpstr))/2,
            screen->height()-wm->font()->height()-5, helpstr, wm->bright_color());
    }
/*    else
    {
        char *helpstr="PRESS h FOR HELP";
        wm->font()->put_string(screen, screen->width()-wm->font()->width()*strlen(helpstr)-5,
            screen->height()-wm->font()->height()-5, helpstr);
    }*/
/*    int dc = cache.img(window_colors)->pixel(0, 2);
    int mc = cache.img(window_colors)->pixel(1, 2);
    int bc = cache.img(window_colors)->pixel(2, 2);
    screen->line(0, 0, screen->width()-1, 0, dc);
    screen->line(0, 0, 0, screen->height()-1, dc);
    screen->line(0, screen->height()-1, screen->width()-1, screen->height()-1, bc);
    screen->line(screen->width()-1, 0, screen->width()-1, screen->height()-1, bc); */

    for(view *f = first_view; f; f = f->next)
        draw_map(f, 0);

    sbar.redraw(screen);
}

int external_print = 0;

void start_sound(int argc, char **argv)
{
  sfx_volume = music_volume = 127;

  for(int i = 1; i < argc; i++)
    if(!strcmp(argv[i], "-sfx_volume"))
    {
      i++;
      if(atoi(argv[i])>=0 && atoi(argv[i])<127)
        sfx_volume = atoi(argv[i]);
      else printf("Bad sound effects volume level, use 0..127\n");
    }
    else if(!strcmp(argv[i], "-music_volume"))
    {
      i++;
      if(atoi(argv[i])>=0 && atoi(argv[i])<127)
        music_volume = atoi(argv[i]);
      else printf("Bad music volume level, use 0..127\n");
    }

  sound_avail = sound_init(argc, argv);
}

void game_printer(char *st)
{
  if(dev_console && !external_print)
  {
    dev_console->put_string(st);
  }
  else fprintf(stderr, "%s", st);
}


void game_getter(char *st, int max)
{
  if(!max) return;
  max--;
  *st = 0;
  if(dev_console && !external_print)
  {
    dev_console->show();
    int t = 0;
    event ev;
    do
    {
      get_event(ev);
      if(ev.type == EV_KEY)
      {
    if(ev.key == JK_BACKSPACE)
    {
      if(t)
      {
        dev_console->print_f("%c", ev.key);
        t--;
        st--;
        *st = 0;
        max++;
      }
    } else if(ev.key>=' ' && ev.key<='~')
    {
      dev_console->print_f("%c", ev.key);
      *st = ev.key;
      t++;
      max--;
      st++;
      *st = 0;
    }
      }
      wm->flush_screen();
    } while(ev.type != EV_KEY || ev.key != JK_ENTER);
    dprintf("\n");
  }
  else
  {
    if(fgets(st, max, stdin))
    {
      if(*st)
        st[strlen(st)-1]=0;
    }
  }
}


void show_startup()
{
    dprintf("Abuse version %s\n", PACKAGE_VERSION);
}

char *get_line(int open_braces)
{
  char *line=(char *)malloc(1000);
  fgets(line, 1000, stdin);

  char prev=' ';
  for(char *s = line; *s && (prev!=' ' || *s!=';'); s++)
  {
    prev=*s;
    if(*s=='(') open_braces++;
    else if(*s==')') open_braces--;
  }
  if(open_braces < 0)
    fprintf(stderr, "\nToo many)'s\n");
  else if(open_braces > 0)
  {
    char *s2 = get_line(open_braces);
    line=(char *)realloc(line, strlen(line)+strlen(s2)+1);
    strcat(line, s2);
    free(s2);
  }
  return line;
}

void check_for_lisp(int argc, char **argv)
{
    for(int i = 1; i < argc; i++)
    {
        if(!strcmp(argv[i], "-lisp"))
        {
            lisp_init(0xf000, 0x30000);
            char const *eof_char = "Ctrl-D";
            fprintf(stderr,
                    " CLIVE (C) 1995 Jonathan Clark, all rights reserved\n"
                    "   (C LISP interpreter and various extentions)\n"
                    "Type (%s) to exit\n", eof_char);

            while(!feof(stdin))
            {
                fprintf(stderr, "Lisp> ");
                char *l = get_line(0);
                char const *s = l;
                while(*s)
                {
                    void *prog = compile(s);
                    l_user_stack.push(prog);
                    while(*s==' ' || *s=='\t' || *s=='\r' || *s=='\n') s++;
                    lprint(eval(prog));
                    l_user_stack.pop(1);
                }
                free(l);
            }
            fprintf(stderr, "End of input : bye\n");
            exit(0);
        }
    }
}


void music_check()
{
  if(sound_avail & MUSIC_INITIALIZED)
  {
    if(current_song && !current_song->playing())
    {
      current_song->play();
      dprintf("song finished\n");
    }
    if(!current_song)
    {

      current_song = new song("music/intro.hmi");
      current_song->play(music_volume);

/*      if(DEFINEDP(symbol_function(l_next_song)))  // if user function installed, call it to load up next song
      {
    int sp = current_space;
    current_space = PERM_SPACE;
    eval_function((lisp_symbol *)l_next_song, NULL);
    current_space = sp;
      } */
    }
  }
}

void setup(int argc, char **argv);

void share_end();
void show_end();

void show_sell(int abortable);

extern pmenu *dev_menu;

void game_net_init(int argc, char **argv)
{
  int nonet=!net_init(argc, argv);
  if(nonet)
    dprintf("No network driver, or network driver returned failure\n");
  else
  {
    set_file_opener(open_nfs_file);
    if(main_net_cfg && main_net_cfg->state == net_configuration::CLIENT)
    {
      if(set_file_server(net_server))
      start_running = 1;
      else
      {
                dprintf("Unable to attach to server, quitting\n");
                exit(0);
      }
    } else
    {
      int i;
      for(i = 1; i < argc - 1; i++)
      if(!strcmp(argv[i], "-fs"))
      if(!set_file_server(argv[i + 1]))
      dprintf("could not set default file server to %s\n", argv[i + 1]);
    }
  }

}

int main(int argc, char *argv[])
{
    start_argc = argc;
    start_argv = argv;

    for(int i = 0; i < argc; i++)
    {
        if(!strcmp(argv[i], "-cprint"))
        {
            external_print = 1;
        }
    }

#if(defined(__APPLE__) && !defined(__MACH__))
    unsigned char km[16];

    fprintf(stderr, "Mac Options: ");
    xres = 320; yres = 200;
    GetKeys((uint32_t*)&km);
    if((km[ 0x3a >>3] >> (0x3a & 7)) &1 != 0)
    {
        dev|=EDIT_MODE;
        start_edit = 1;
        start_running = 1;
        disable_autolight = 1;
        fprintf(stderr, "Edit Mode...");
    }
    if((km[ 0x3b >>3] >> (0x3b & 7)) &1 != 0)
    {
        PixMult = 1;
        fprintf(stderr, "Single Pixel...");
    }
    else
    {
        PixMult = 2;
        fprintf(stderr, "Double Pixel...");
    }
    if((km[ 0x38 >>3] >> (0x38 & 7)) &1 != 0)
    {
        xres *= 2;  yres *= 2;
        fprintf(stderr, "Double Size...");
    }
    fprintf(stderr, "\n");

    if(tcpip.installed())
        fprintf(stderr, "Using %s\n", tcpip.name());
#endif

    set_dprinter(game_printer);
    set_dgetter(game_getter);
    set_no_space_handler(handle_no_space);

    setup(argc, argv);

    show_startup();

    start_sound(argc, argv);

    stat_man = new text_status_manager();
    if(!get_option("-no_timer"))
    {
        timer_init();
    }

    // look to see if we are supposed to fetch the data elsewhere
    if(getenv("ABUSE_PATH"))
    {
        set_filename_prefix(getenv("ABUSE_PATH"));
    }

    // look to see if we are supposed to save the data elsewhere
    if(getenv("ABUSE_SAVE_PATH"))
    {
        set_save_filename_prefix(getenv("ABUSE_SAVE_PATH"));
    }

    jrand_init();
    jrand();        // so compiler doesn't complain

    set_spec_main_file("abuse.spe");

    check_for_lisp(argc, argv);

    do
    {
        if(main_net_cfg)
        {
            if(!main_net_cfg->notify_reset())
            {
                if(!get_option("-no_timer"))
                    timer_uninit();
                sound_uninit();
                exit(0);
            }
        }

        game_net_init(argc, argv);
//        lisp_init(0x100000, 0x94000);
        lisp_init(0x80000, 0x94000);
//        lisp_init(0x20000, 0x94000);
//        lisp_init(0x100000, 0x10000);

        dev_init(argc, argv);

        Game *g = new Game(argc, argv);

        dev_cont = new dev_controll();
        dev_cont->load_stuff();

        g->get_input();        // prime the net

        int xx;
        for(xx = 1; xx < argc; xx++)
        {
            if(!strcmp(argv[xx], "-server"))
            {
                xx++;
                if(!become_server(argv[xx]))
                {
                    dprintf("unable to become a server\n");
                    exit(0);
                }
                xx = argc + 1;
            }
        }

        if(main_net_cfg)
        {
            wait_min_players();
        }

        net_send(1);
        if(net_start())
        {
            g->step();                // process all the objects in the
            g->calc_speed();
            g->update_screen();        // redraw the screen with any changes
        }

    while(!g->done())
    {
      music_check();

      if(req_end)
      {
                delete current_level;
                current_level = NULL;

                show_end();

                the_game->set_state(MENU_STATE);
                req_end = 0;
      }

      if(demo_man.current_state()==demo_manager::NORMAL)
      {
                net_receive();
      }

      if(req_name[0])            // see if a request for a level load was made during the last tick
      {
        g->load_level(req_name);
        req_name[0]=0;
        g->draw(g->state == SCENE_STATE);
      }

      //    if(demo_man.current_state()!=demo_manager::PLAYING)
      g->get_input();

      if(demo_man.current_state()==demo_manager::NORMAL)
      net_send();
      else demo_man.do_inputs();

      service_net_request();

      g->step();                        // process all the objects in the

      server_check();

      g->calc_speed();
      if(!req_name[0])                // see if a request for a level load was made during the last tick
        g->update_screen();               // redraw the screen with any changes


    }

    net_uninit();

    if(net_crcs)
    {
      net_crcs->clean_up();
      delete net_crcs;
      net_crcs = NULL;
    }

    delete chat;

    milli_wait(500);

    if(small_render) { delete small_render; small_render = NULL; }

    if(current_song)
    { current_song->stop();
      delete current_song;
      current_song = NULL;
    }


    cache.empty();


    if(dev_console)
    {
      delete dev_console;
      dev_console = NULL;
    }

    if(dev_menu)
    {
      delete dev_menu;
      dev_menu = NULL;
    }

    if(g)
    {
        delete g;
        g = NULL;
    }

    if(old_pal) delete old_pal; old_pal = NULL;
    compiled_uninit();
    delete_all_lights();
    free(white_light_initial);

    for(int i = 0; i < TTINTS; i++) free(tints[i]);


    dev_cleanup();
    if(dev_cont)
    {
        delete dev_cont;
        dev_cont = NULL;
    }
    delete stat_man;
    stat_man = new text_status_manager();

    if(!(main_net_cfg && main_net_cfg->restart_state()))
    {
      void *end_msg = make_find_symbol("end_msg");
      if(DEFINEDP(symbol_value(end_msg)))
      printf("%s\n", lstring_value(symbol_value(end_msg)));
    }

    lisp_uninit();

    base->packet.packet_reset();
  } while(main_net_cfg && main_net_cfg->restart_state());

    delete stat_man;

    if(main_net_cfg)
    {
        delete main_net_cfg;
        main_net_cfg = NULL;
    }
    set_filename_prefix(NULL);  // dealloc this mem if there was any
    set_save_filename_prefix(NULL);

    if(!get_option("-no_timer"))
    {
        timer_uninit();
    }

    l_user_stack.clean_up();
    l_ptr_stack.clean_up();

    sound_uninit();

    exit(0);

    return 0;
}

