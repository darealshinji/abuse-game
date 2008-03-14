#include "game.hpp"
#include "dev.hpp"
#include "id.hpp"
#include "joy.hpp"
#include "timing.hpp"
#include "automap.hpp"
#include "help.hpp"
#include "ability.hpp"
#include "cache.hpp"
#include "lisp.hpp"
#include "jrand.hpp"
#include "config.hpp"
#include "light.hpp"
#include "scroller.hpp"
#include "dprint.hpp"
#include "nfserver.hpp"
#include "video.hpp"
#include "transp.hpp"
#include "clisp.hpp"
#include "guistat.hpp"
#include "menu.hpp"
#include "gamma.hpp"
#include "lisp_gc.hpp"
#include "demo.hpp"
#include "sbar.hpp"
#include "profile.hpp"
#include "compiled.hpp"
#include "lisp_gc.hpp"
#include "pmenu.hpp"
#include "timing.hpp"
#include "chat.hpp"
#include "demo.hpp"
#include "netcfg.hpp"
#include "console.hpp"

#include "crtdbg.h"

#define SHIFT_RIGHT_DEFAULT 0
#define SHIFT_DOWN_DEFAULT 30

class game;
extern crc_manager *net_crcs;
extern void show_verinfo(int argc, char **argv);
game *the_game;
window_manager *eh = NULL;
int dev, shift_down = SHIFT_DOWN_DEFAULT, shift_right = SHIFT_RIGHT_DEFAULT;
double sum_diffs = 1, total_diffs = 12;
int total_active = 0;
long map_xoff = 0, map_yoff = 0;
long current_vxadd, current_vyadd;
int frame_panic = 0, massive_frame_panic = 0;
int demo_start = 0, idle_ticks = 0;
int req_end = 0;

extern palette *old_pal;

extern char** _argv;
extern int _argc;

char **start_argv;
int start_argc;
int has_joystick = 0;
char req_name[100];

int registered = 0;
extern uchar chatting_enabled;

extern int confirm_quit();

//------------------ Added by Jeremy Scott -----------------------------------
char *AddonDir = NULL;                               // Path to prefix to load

//----------------------------------------------------------------------------


/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
FILE *open_FILE(char *filename, char *mode)
{
    char tmp_name[0xff];
    if (get_filename_prefix() && filename[0] != '/')
        sprintf(tmp_name, "%s%s", get_filename_prefix(), filename);
    else
        strcpy(tmp_name, filename);
    return fopen(tmp_name, mode);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void handle_no_space()
{
    char *no_space_msg = "\nYou are out of disk space or the game\n"
                         "was unable to write to disk for some reason\n"
                         "The game cannot continue, please check this out\n"
                         "and try again.\n";
    if (eh) {
        jwindow *no_space = eh->new_window(0, 0, -1, -1,
            new button(0, 0, ID_QUIT_OK, "Quit",
            new info_field(0, eh->font()->height() * 2, ID_NULL,
            no_space_msg, NULL)), "ERROR");
        event ev;
        do {
            eh->flush_screen();
            eh->get_event(ev);
        } while (ev.type != EV_MESSAGE || ev.message.id != ID_QUIT_OK);
        eh->close_window(no_space);

        close_graphics();
        Shutdown(0, NULL, 0);
    }
    else {
        WriteErr(0, no_space_msg);
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::play_sound(int id, int vol, long x, long y)
{
    if (sound_avail & SFX_INITIALIZED) {
        if (vol < 1)
            return;
        if (!player_list)
            return;

        int panpot;
        unsigned long MinDist = 0xffffffff;
        view *cd = NULL;
        for (view *f = player_list; f; f = f->next) {
            if (f->local_player()) {
                long cx = abs(f->x_center() - x),
                     cy = abs(f->y_center() - y), d;
	            if (cx < cy)
                    d = cx + cy - (cx >> 1);
	            else
                    d = cx + cy - (cy >> 1);
                if ((unsigned) d < MinDist) {
	                cd = f;
	                MinDist = d;
	                panpot = ((x - f->x_center()) * 10000) / 500;
                }
/*                long w = f->cx1 - f->cx2,
                     h = f->cy1 - f->cy2; */
            }
        }
//        long MaxDist = (xres << 2);
        if (MinDist > 500 || !cd)
            return;
        if (MinDist < 100)
            MinDist = 0;
        else
            MinDist -= 100;
        int v = (400 - MinDist) * sfx_volume / 400 - (127 - vol);
        if (v > 0)
            cash.sfx(id)->play(v, 128, panpot);
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int get_option(char *name)
{
    int i;
    for (i = 1; i < _argc; i++)
        if (!strcmp(_argv[i], name))
            return i;
    return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void make_screen_size(unsigned int w, unsigned int h)
{
    for (view *f = player_list; f; f = f->next) {
        if (f->local_player()) {
            if (w >= xres - 1)
                w = xres - 2;
            if (h >= yres - 1)
                h = yres - 2;
            f->suggest.cx1 = (xres + 1) / 2 - w / 2;
            f->suggest.cx2 = (xres + 1) / 2 + w / 2;
            f->suggest.cy1 = (yres - 31) / 2 +5 - h / 2;
            f->suggest.cy2 = (yres - 51) / 2 +5 + h / 2;
            f->suggest.shift_down = f->shift_down;
            f->suggest.shift_right = f->shift_right;
            f->suggest.pan_x = f->pan_x;
            f->suggest.pan_y = f->pan_y;
            
            f->suggest.send_view = 1;
        }
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::grow_views(int amount)
{
    int undo = 0;
    view *f = first_view;
    for (; f; f = f->next) {
        if (f->local_player()) {
            f->suggest.cx1 = (f->cx1 - amount);
            f->suggest.cy1 = f->cy1 - amount / 2;
            f->suggest.cx2 = (f->cx2 + amount);
            f->suggest.cy2 = f->cy2 + amount / 2;
            f->suggest.shift_down = f->shift_down;
            f->suggest.shift_right = f->shift_right;
            f->suggest.pan_x = f->pan_x;
            f->suggest.pan_y = f->pan_y;
            
            f->suggest.send_view = 1;
        }
    }
    for (f = first_view; f; f = f->next) {
        if (f->local_player()) {
            if (f->suggest.cx2 - f->suggest.cx1 < 20 || f->suggest.cy2 - f->suggest.cy1 < 15 || 
                f->suggest.cx1 < 0 || f->suggest.cy1 < 0) f->suggest.send_view = 0;
            if (f->next && f->next->local_player() && f->suggest.cy2 >= f->next->cy1)
                f->suggest.send_view = 0;
        }
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::pan(int xv, int yv) 
{ 
    first_view->pan_x += xv; 
    first_view->pan_y += yv; 
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
view *game::view_in(int mousex, int mousey)
{
    for (view *f = first_view; f; f = f->next)
        if (f->drawable() && mousex >= f->cx1 && mousey >= f->cy1 && mousex <= f->cx2 && mousey <= f->cy2)
            return f;
    return NULL;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int playing_state(int state)
{ 
    if (state == RUN_STATE || state == PAUSE_STATE)
        return 1;
    else
        return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::ftile_on(int screenx, int screeny, long &x, long &y)
{
    mouse_to_game(screenx, screeny, x, y);
    x /= ftile_width();
    y /= ftile_height();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::btile_on(int screenx, int screeny, long &x, long &y)
{
  view *f=view_in(screenx,screeny);
  if (f)
  {
    x=((long)(screenx)-(long)f->cx1+f->xoff()*bg_xmul/bg_xdiv)/(long)b_wid; 
    y=((long)(screeny)-(long)f->cy1+f->yoff()*bg_ymul/bg_ydiv)/(long)b_hi;
  }
  else
  {
    x=-1; 
    y=-1;
  }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::mouse_to_game(long x, long y, long &gamex, long &gamey, view *f)
{
    if (!f) {
        f=view_in(x,y);
        if (!f) 
            f=player_list;  // if not in a view use the first on
    }
    if (f) {
        if (dev&MAP_MODE) {
	        gamex=((x-(long)f->cx1)*ftile_width()/AUTOTILE_WIDTH+map_xoff*ftile_width());
	        gamey=((y-(long)f->cy1)*ftile_height()/AUTOTILE_HEIGHT+map_yoff*ftile_height());
        } 
        else {
	        gamex=(x-(long)f->cx1+f->xoff());
	        gamey=(y-(long)f->cy1+f->yoff());
        }
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::game_to_mouse(long gamex, long gamey, view *which, long &x, long &y)
{
    if (dev&MAP_MODE)
    {
        long x1,y1;
        if (dev&EDIT_MODE)
        {
            x1=map_xoff;
            y1=map_yoff;
        } else
        {
            if (which->focus)
            {
                x1=which->focus->x/ftile_width()-(which->cx2-which->cx1)/AUTOTILE_WIDTH/2;
                y1=which->focus->y/ftile_height()-(which->cy2-which->cy1)/AUTOTILE_HEIGHT/2;
            } else x1=y1=0;
        }
        if (x1<0) x1=0;
        if (y1<0) y1=0;
        
        x=gamex*AUTOTILE_WIDTH/ftile_width()-x1*AUTOTILE_WIDTH+which->cx1;
        if (x1>0)
            x-=((which->focus->x*AUTOTILE_WIDTH/ftile_width()) %AUTOTILE_WIDTH);
        
        y=gamey*AUTOTILE_HEIGHT/ftile_height()-y1*AUTOTILE_HEIGHT+which->cy1;
        if (y1>0)
            y-=((which->focus->y*AUTOTILE_HEIGHT/ftile_height()) %AUTOTILE_HEIGHT);
    }
    else
    {
        x=gamex-which->xoff()+which->cx1;
        y=gamey-which->yoff()+which->cy1;
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int window_state(int state)
{
    switch (state) {
        case RUN_STATE : 
        case PAUSE_STATE : 
        case JOY_CALB_STATE :
            return 1;
            break;
        case INTRO_START_STATE :
        case HELP_STATE : 
        case INTRO_MORPH_STATE :
        case MENU_STATE :
        case SCENE_STATE :
            return 0;
            break;
    }
    return 1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::set_state(int new_state) 
{
    int d=0;
    reset_keymap();                             // we think all the keys are up right now
    if (playing_state(new_state) && !playing_state(state)) {
        if (first_view && first_view!=player_list) {
            while (first_view) {
                view *tmp=first_view;
                first_view=first_view->next;
                delete tmp;
            }
            first_view=old_view;
            old_view=NULL;
        }
        first_view=player_list;
        d=1;
    }
    else if (!playing_state(new_state) && (playing_state(state) || state==START_STATE)) {
        if (player_list) {
            first_view=new view(player_list->focus,NULL,-1);
            first_view->pan_x=player_list->xoff();
            first_view->pan_y=player_list->yoff();
        }
        else    
            first_view=new view(NULL,NULL,0);
        first_view->cx1=(xres+1)/2-155;
        first_view->cy1=(yres+1)/2-95;
        first_view->cx2=(xres+1)/2+155;
        if (total_weapons)
            first_view->cy2=(yres+1)/2+68;
        else
            first_view->cy2=(yres+1)/2+95;
        d=1;
    }
    
    // switching to/from scene mode cause the screen size to change and the border to change
    // so we need to redraw.
    if (window_state(new_state) && !window_state(state))
        eh->show_windows();
    else if (!window_state(new_state) && window_state(state))
        eh->hide_windows();

    int old_state=state;
    state=new_state;
    
    pal->load();    // restore old palette
    
    if (playing_state(state) &&  !(dev&EDIT_MODE))
        eh->set_mouse_shape(cash.img(c_target)->copy(),8,8);
    else
        eh->set_mouse_shape(cash.img(c_normal)->copy(),1,1);
    
    if (old_state==SCENE_STATE && new_state!=SCENE_STATE) {
        d=1;
        scene_director.set_abort(0);   // don't skip any more scene stuff
    }
    else if (new_state==SCENE_STATE && old_state!=SCENE_STATE)
        d=1;
    
    if (d)
        draw(state==SCENE_STATE);
    
    dev_cont->set_state(new_state);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::joy_calb(event &ev)
{
    if (joy_win) {  // make sure the joy stick calibration window is open
        if (ev.type == EV_SPURIOUS) {  // spurious means we should update your status
            int b1, b2, b3 = 0, x, y;
            joy_status(b1, b2, b2, x, y);
            int but = b1 | b2 | b3;
            if (x > 0)
                x = 1;
            else if (x < 0)
                x = -1;
            if (y>0)
                y = 1;
            else if (y<0)
                y = -1;
            if (but) but=1;
            int dx = 20, dy = 5;
            image *jim = cash.img(joy_picts[but * 9 + (y + 1) * 3 + x + 1]);
            joy_win->screen->bar(dx, dy, dx + jim->width() + 6, dy + jim->height() + 6, eh->black());
            jim->put_image(joy_win->screen, dx + 3, dy + 3);
            if (but)
                joy_calibrate();
        }
        else if (ev.type == EV_MESSAGE && ev.message.id == JOY_OK) {
            eh->close_window(joy_win);
            joy_win = NULL;
            set_state(MENU_STATE);
        }        
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::menu_select(event &ev)
{
  state=DEV_MOUSE_RELEASE;
  if (top_menu)
  {
/*    eh->push_event(new event(men_mess[((pick_list *)ev.message.data)->get_selection()],NULL));
    eh->close_window(top_menu);
    top_menu=NULL;*/
  }
}


/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::show_help(char *st)
{
  strcpy(help_text,st);
  help_text_frames=0;  
  refresh=1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::draw_value(image *screen, int x, int y, int w, int h, int val, int max)
{
  screen->bar(x,y,x+w-1,y+h,eh->dark_color());
  screen->bar(x,y+1,x+w*val/max,y+h-1,eh->bright_color());  
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::set_level(level *nl)
{
    if (current_level) 
        delete current_level;  
    current_level=nl;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::load_level(char *name)
{
    GameConsole->hide();
    if (dev_console)
        dev_console->hide();
    char lev_name[1024];
    if (current_automap)
        delete current_automap;
    current_automap = NULL;
    if (current_level) 
        delete current_level;

     if (AddonDir)
        sprintf(lev_name, "%s%s", AddonDir, name);
    else
        strcpy(lev_name, name);

    bFILE *fp = open_file(lev_name, "rb");
    
    if (fp->open_failure()) {
        delete fp;
        current_level = new level(100, 100, name);
        char msg[100];
        sprintf(msg, symbol_str("no_file"), name);
        show_help(msg);
    }
    else {
        spec_directory sd(fp);  
        
        spec_entry *e = sd.find("Copyright 1995 Crack dot Com, All Rights reserved"); 
        if (!e) {
            the_game->show_help(symbol_str("missing_c"));
            current_level = new level(100, 100, "untitled");
            the_game->need_refresh();
        }
        else 
            current_level = new level(&sd, fp, name);
        delete fp;
    }
    
    current_automap = new automap(current_level, 10, 10);
    base->current_tick = (short) (current_level->tick_counter() & 0xff); 
    
    current_level->level_loaded_notify();
    the_game->help_text_frames = 0;  
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int game::done() 
{ 
  return finished || (main_net_cfg && main_net_cfg->restart_state());

}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::end_session()
{
  finished=1;
  if (main_net_cfg)
  {
    delete main_net_cfg;
    main_net_cfg=NULL;
  }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::put_block_fg(int x, int y, trans_image *im)
{
  for (view *f=first_view;f;f=f->next)
  {
    if (f->drawable())
    {
      int xoff=f->xoff(),yoff=f->yoff(),viewx1=f->cx1,viewy1=f->cy1,viewx2=f->cx2,viewy2=f->cy2;
      if (xoff/ftile_width()>x || xoff/ftile_width()+(viewx2-viewx1)/ftile_width()+1<x ||
	  yoff/ftile_height()>y || yoff/ftile_height()+(viewy2-viewy1)/ftile_height()+1<y) return;
      short cx1,cy1,cx2,cy2;
      screen->get_clip(cx1,cy1,cx2,cy2);
      screen->set_clip(viewx1,viewy1,viewx2,viewy2);
      im->put_image(screen,(x-xoff/ftile_width())*ftile_width()+viewx1-xoff%ftile_width(),
		    (y-yoff/ftile_height())*ftile_height()+viewy1-yoff%ftile_height());
      screen->set_clip(cx1,cy1,cx2,cy2);
    }
  }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::put_block_bg(int x, int y, image *im)
{
  for (view *f=first_view;f;f=f->next)
  {
    if (f->drawable())
    {
      int xoff=f->xoff(),yoff=f->yoff(),viewx1=f->cx1,viewy1=f->cy1,viewx2=f->cx2,viewy2=f->cy2;
      int xo=xoff*bg_xmul/bg_xdiv;
      int yo=yoff*bg_ymul/bg_ydiv;
      
      if (xo/btile_width()>x || xo/btile_width()+(viewx2-viewx1)/btile_width()+1<x ||
	  yo/btile_height()>y || yo/btile_height()+(viewy2-viewy1)/btile_height()+1<y) return;
      short cx1,cy1,cx2,cy2;
      screen->get_clip(cx1,cy1,cx2,cy2);
      screen->set_clip(viewx1,viewy1,viewx2,viewy2);
      im->put_image(screen,(x-xo/btile_width())*btile_width()+viewx1-xo%btile_width(),
		    (y-yo/btile_height())*btile_height()+viewy1-yo%btile_height(),0);
      screen->set_clip(cx1,cy1,cx2,cy2);
    }
  }
}

int need_delay=1;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::dev_scroll()
{
  need_delay=0;
  if (dev)
  {
    int xmargin,ymargin;
    if (xres>400)
    {
      xmargin=20;
      ymargin=10;
    }
    else 
    {
      xmargin=10;
      ymargin=5;
    }

    int xs,ys;
    if (mousex<xmargin &&  dev_cont->ok_to_scroll()) xs=-18;
    else if (mousex>(screen->width()-xmargin) &&  dev_cont->ok_to_scroll()) xs=18;
    else if (eh->key_pressed(JK_LEFT) && !last_input && !dev_cont->need_arrows())
      xs=-18;
    else if (eh->key_pressed(JK_RIGHT) && !last_input && !dev_cont->need_arrows())
      xs=18;
    else xs=0;
	     

    if (mousey<ymargin && dev_cont->ok_to_scroll()) ys=-18;
    else if (mousey>(screen->height()-ymargin) &&  dev_cont->ok_to_scroll()) ys=18;
    else if (eh->key_pressed(JK_UP) && !last_input)
      ys=-18;
    else if (eh->key_pressed(JK_DOWN) && !last_input)
      ys=18;
    else ys=0;

    
    if (xs || ys)
    {
      need_delay=1;
      if (dev&MAP_MODE)
      {
	map_xoff+=xs/2;
	map_yoff+=ys/2;
	if (map_xoff<0) map_xoff=0;
	if (map_yoff<0) map_yoff=0;
      } 
      else
      { 
	for (view *v=first_view;v;v=v->next)
	{
	  if (xs>=0 || v->xoff()>0)
	    v->pan_x+=xs;
	  if (ys>=0 || v->yoff()>0)
	    v->pan_y+=ys;
	}
      }
      refresh=1;
    }
  }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void remap_area(image *screen, int x1, int y1, int x2, int y2, uchar *remap)
{
    uchar *sl = (uchar*) screen->scan_line(y1) + x1;
    int x, y, a = screen->width() - (x2 - x1 + 1);
    uchar c;
    for (y = y1; y <= y2; y++) {
        for (x = x1; x <= x2; x++) {
            c = *sl;
            *(sl++) = remap[c];
        }
        sl += a;
    }
    screen->UnLock();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void post_render()
{
    if (DEFINEDP(symbol_function(l_post_render))) {
        screen->dirt_off();
        clear_tmp();
        eval_function((lisp_symbol*) l_post_render,NULL);
        clear_tmp();
        screen->dirt_on();
    }
}
  
/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::draw_map(view *v, int interpolate)
{
    backtile *bt;
    int x1, y1, x2, y2, x, y, xo, yo, nxoff, nyoff;
    short cx1, cy1, cx2, cy2;
    screen->get_clip(cx1, cy1, cx2, cy2);

    if (!current_level || state==MENU_STATE) {
        if (title_screen >= 0) {
            if (state == SCENE_STATE)
                screen->set_clip((short) v->cx1, (short) v->cy1, (short) v->cx2, (short) v->cy2);
            image *tit = cash.img(title_screen);
            tit->put_image(screen, screen->width() / 2 - tit->width() / 2,
		                    screen->height() / 2 - tit->height() / 2);
            if (state == SCENE_STATE)
                screen->set_clip(cx1, cy1, cx2, cy2);
            eh->flush_screen();
        }
        return;
    }

    refresh = 0;

  // save the dirty rect routines some work by markinging evrything in the 
  // view area dirty alreadt

    if (small_render)
        screen->add_dirty(v->cx1, v->cy1, (v->cx2 - v->cx1 + 1) * 2 + v->cx1, v->cy1 + (v->cy2 - v->cy1 + 1) * 2);
    else
        screen->add_dirty(v->cx1, v->cy1, v->cx2, v->cy2);

    if (v->draw_solid != -1) {     // fill the screen and exit..
        int c = v->draw_solid;
        for (int y = v->cy1; y <= v->cy2; y++)
            memset(screen->scan_line(y) + v->cx1, c, v->cx2 - v->cx1 + 1);
        v->draw_solid = -1;
        screen->UnLock();
        return;
    }

    long old_cx1, old_cy1, old_cx2, old_cy2;   // if we do a small render, we need to restore these
    image *old_screen = NULL;
    if (small_render && (dev & DRAW_LIGHTS)) { // cannot do this if we skip lighting
        old_cx1 = v->cx1;
        old_cy1 = v->cy1;
        old_cx2 = v->cx2;
        old_cy2 = v->cy2;

        v->cx1 = 0;
        v->cy1 = 0;
        v->cx2 = small_render->width() - 1;
        v->cy2 = small_render->height() - 1;

        old_screen = screen;
        screen = small_render;
    }
    else
        screen->dirt_off();

    long xoff, yoff;
    if (interpolate) {
        xoff = v->interpolated_xoff();
        yoff = v->interpolated_yoff();
    }
    else {
        xoff = v->xoff();
        yoff = v->yoff();
    }

    current_vxadd=xoff-v->cx1;
    current_vyadd=yoff-v->cy1;

    screen->set_clip((short) v->cx1, (short) v->cy1, (short) v->cx2, (short) v->cy2);

    nxoff=xoff*bg_xmul/bg_xdiv;
    nyoff=yoff*bg_ymul/bg_ydiv;

    x1=nxoff/btile_width(); y1=nyoff/btile_height();
    x2=x1+(v->cx2-v->cx1+btile_width())/btile_width();
    y2=y1+(v->cy2-v->cy1+btile_height())/btile_height();

    xo=v->cx1-nxoff%btile_width();
    yo=v->cy1-nyoff%btile_height();
  
    int xinc, yinc, draw_x, draw_y;


    if (!(dev & MAP_MODE) && (dev & DRAW_BG_LAYER)) {
        xinc = btile_width();  
        yinc = btile_height();  
    
        int bh = current_level->background_height(), bw = current_level->background_width();
        ushort *bl;
        for (draw_y = yo, y = y1; y <= y2; y++, draw_y += yinc) {
            if (y >= bh)
                bl = NULL;
            else
                bl = current_level->get_bgline(y) + x1;

            for (x = x1, draw_x = xo; x <= x2; x++, draw_x += xinc) {
                if (x < bw && y < bh) {
                    bt = get_bg(*bl);
	                bl++;
                }
	            else bt = get_bg(0);

                bt->im->put_image(screen, draw_x, draw_y); 
//        if (!(dev & EDIT_MODE) && bt->next)
//	  current_level->put_bg(x,y,bt->next);
            }
        }
    }

//  if (!(dev&EDIT_MODE))
//    server_check();

    uchar rescan = 0;  

    int fw, fh;

    if (dev & MAP_MODE) {
        fw = AUTOTILE_WIDTH;
        fh = AUTOTILE_HEIGHT;
        if (dev&EDIT_MODE) {
	        x1 = map_xoff;
	        y1 = map_yoff;
        }
        else {
            if (v->focus) {
	            x1 = v->focus->x / ftile_width() - (v->cx2 - v->cx1) / fw / 2;
	            y1 = v->focus->y / ftile_height() - (v->cy2 - v->cy1) / fh / 2;
            }
	        else 
                x1 = y1 = 0;
        }
        if (x1 > 0)
            xo = v->cx1 - ((v->focus->x * fw / ftile_width()) % fw);
        else 
            xo = v->cx1;
        if (y1 > 0)
            yo = v->cy1 - ((v->focus->y * fh / ftile_height()) % fh);
        else 
            yo = v->cy1;
    }
    else {
        fw = ftile_width();
        fh = ftile_height();
        x1 = (xoff) / fw; 
        y1 = (yoff) / fh;
        xo = v->cx1 - xoff % fw;
        yo = v->cy1 - yoff % fh;
    }
    if (x1 < 0) 
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    
    x2 = x1 + (v->cx2 - v->cx1 + fw) / fw;
    y2 = y1 + (v->cy2 - v->cy1 + fh) / fh;
    if (x2 >= current_level->foreground_width())
        x2 = current_level->foreground_width() - 1;
    if (y2 >= current_level->foreground_height())
        y2 = current_level->foreground_height() - 1;

    xinc = fw;
    yinc = fh;

    if (dev & DRAW_FG_LAYER) {
        short ncx1,ncy1,ncx2,ncy2;
        screen->get_clip(ncx1,ncy1,ncx2,ncy2);

        int scr_w=screen->width();
        if (dev & MAP_MODE) {
            if (dev & EDIT_MODE)
                screen->clear(eh->bright_color());
            else
                screen->clear(eh->black());
            screen->Lock();
            for (y = y1, draw_y = yo; y <= y2; y++, draw_y += yinc) {
                if (!(draw_y < ncy1 || draw_y + yinc >= ncy2)) {
	                ushort *cl = current_level->get_fgline(y) + x1;
	                uchar *sl1 = screen->scan_line(draw_y) + xo;
                    for (x = x1, draw_x = xo; x <= x2; x++, cl++, sl1 += xinc, draw_x += xinc) {
                        if (!(draw_x < ncx1 || draw_x + xinc >= ncx2)) {
	                        int fort_num;
//	      if (*cl&0x8000 || (dev&EDIT_MODE))
	                        fort_num = fgvalue(*cl);
//	      else fort_num=0;
	                        uchar *sl2 = get_fg(fort_num)->micro_image->scan_line(0);
	                        uchar *sl3 = sl1;
	                        memcpy(sl3, sl2, AUTOTILE_WIDTH); sl2 += AUTOTILE_WIDTH; sl3 += screen->Pitch();
	                        memcpy(sl3, sl2, AUTOTILE_WIDTH); sl2 += AUTOTILE_WIDTH; sl3 += screen->Pitch();
	                        memcpy(sl3, sl2, AUTOTILE_WIDTH);
                        }
                    }
                }
            }
            screen->UnLock();
            if (dev & EDIT_MODE)
                current_level->draw_areas(v);
        }
        else {
            int fg_h=current_level->foreground_height(),fg_w=current_level->foreground_width();
      
            for (y=y1,draw_y=yo;y<=y2;y++,draw_y+=yinc) {
	            ushort *cl;
	            if (y<fg_h)
	                cl = current_level->get_fgline(y)+x1;
	            else 
                    cl = NULL;
	            uchar *sl1 = draw_y < ncy1 ? 0 : screen->scan_line(draw_y) + xo;

                for (x = x1, draw_x = xo; x <= x2; x++, draw_x += xinc, cl++, sl1 += xinc) {
                    if (x<fg_w && y<fg_h) {
	                    if (above_tile(*cl))
	                        rescan = 1;
                        else {
	                        int fort_num=fgvalue(*cl);	  
                            if (fort_num!=BLACK) {
//		                        if (draw_y<ncy1 || draw_y+yinc>=ncy2 || draw_x<ncx1 || draw_x+xinc>=ncx2)
	                                get_fg(fort_num)->im->put_image(screen,draw_x,draw_y);
/*		                        else
	                                get_fg(fort_num)->im->put_image_offseted(screen,sl1); */

		                        if (!(dev & EDIT_MODE))
	                                *cl |= 0x8000;      // mark as has-been-seen
                            }
                        }
                    }
                }
            }
        }
    }
    unsigned short ro = rand_on;
    if (dev & DRAW_PEOPLE_LAYER) {
        if (interpolate)
            current_level->interpolate_draw_objects(v);
        else
            current_level->draw_objects(v);
    }

//  if (!(dev&EDIT_MODE))
//    server_check();

    if (!(dev & MAP_MODE)) {
        draw_panims(v);
        int fg_h = current_level->foreground_height(),
            fg_w = current_level->foreground_width();

        if (dev & DRAW_FG_LAYER && rescan) {
            for (y = y1, draw_y = yo; y <= y2; y++, draw_y += yinc) {
	            ushort *cl = current_level->get_fgline(y)+x1;
                for (x = x1, draw_x = xo; x <= x2; x++, draw_x += xinc, cl++) {
                    if (above_tile(*cl)) {
	                    int fort_num = fgvalue(*cl);
                        if (fort_num != BLACK) {
                            if (dev & DRAW_BG_LAYER)
	                            get_fg(fort_num)->im->put_image(screen, draw_x, draw_y);
	                        else
	                            get_fg(fort_num)->im->put_image_filled(screen, draw_x, draw_y, 0);

	                        if (!(dev & EDIT_MODE))
	                            current_level->mark_seen(x, y);
                            else {
		                        screen->line(draw_x, draw_y, draw_x + xinc, draw_y + yinc, eh->bright_color());
		                        screen->line(draw_x + xinc, draw_y, draw_x, draw_y + yinc, eh->bright_color());
                            }
                        }
                    }
                }
            }
        }
        if (dev & DRAW_FG_BOUND_LAYER) {
            int b = eh->bright_color();
            int fg_h = current_level->foreground_height(),fg_w=current_level->foreground_width();

            for (y=y1,draw_y=yo;y<=y2;y++,draw_y+=yinc) {
	            ushort *cl;
	            if (y<fg_h)
	                cl=current_level->get_fgline(y)+x1;
	            else 
                    cl=NULL;
                for (x=x1,draw_x=xo;x<=x2;x++,draw_x+=xinc,cl++) {
                    if (x<fg_w && y<fg_h) {
	                    int fort_num=fgvalue(*cl);	  
                        if (fort_num!=BLACK) {
	                        point_list *p=get_fg(fort_num)->points;
	                        uchar *d=p->data;	
                            if (p->tot) {
                                for (int i=1;i<p->tot;i++) {
		                            d+=2;
		                            screen->line(draw_x+*(d-2),draw_y+*(d-1),draw_x+*d,draw_y+*(d+1),b);
                                }
		                        screen->line(draw_x+*d,draw_y+*(d-1),draw_x+p->data[0],draw_y+p->data[1],b);
                            }
                        }
                    }
                }
            }
        }

//    if (!(dev&EDIT_MODE))
//      server_check();
        if (dev & DRAW_HELP_LAYER) {
            if (help_text_frames>=0) {
	            int color;
	
	            if (help_text_frames < 10)
                    color = 2;
	            else
                    color = 2 + (help_text_frames - 10);
	
	            int x1=v->cx1,y1=v->cy1,x2=v->cx2,y2=v->cy1+eh->font()->height()+10;

	            remap_area(screen,x1,y1,x2,y2,white_light+40*256);
	            screen->bar(x1,y1,x2,y1,color);
	            screen->bar(x1,y2,x2,y2,color);

	            eh->font()->put_string(screen, x1 + 5, y1 + 5, help_text, color);
	            if (color>30)
                    help_text_frames=-1;      
	            else 
                    help_text_frames++;
            }
        }
        if (dev_cont)
            dev_cont->dev_draw(v);  
        if (cash.in_use())
            cash.img(vmm_image)->put_image(screen, (short) v->cx1, v->cy2 - cash.img(vmm_image)->height() + 1);  

        if (dev & DRAW_LIGHTS) {
            if (small_render) {
	            double_light_screen(screen, xoff, yoff, white_light, v->ambient, old_screen, old_cx1, old_cy1);

	            v->cx1 = old_cx1;
	            v->cy1 = old_cy1;
	            v->cx2 = old_cx2;
	            v->cy2 = old_cy2;
	            screen = old_screen;
            }
            else {
	            screen->dirt_on();
//	if (xres*yres<=64000) 
                light_screen(screen, xoff, yoff, white_light, v->ambient);
//	else light_screen(screen,xoff,yoff,white_light,63);            // no lighting for hi-rez
            }
        }
        else 
            screen->dirt_on();
    }
    else
        screen->dirt_on();

    rand_on = ro;              // restore random start in case in draw funs moved it
                               // ... not every machine will draw the same thing

    post_render();

    screen->set_clip(cx1, cy1, cx2, cy2);
    
    if (playing_state(state))        // draw stuff outside the clipping region
        v->draw_character_damage();

    if (profiling())
        profile_update();

    sbar.draw_update();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::put_fg(int x, int y, int type)
{ 
  if (current_level->get_fg(x,y)!=type)
  {
    current_level->put_fg(x,y,type);
    for (view *f=first_view;f;f=f->next)
      if (f->drawable())
        draw_map(f);    
  }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::put_bg(int x, int y, int type)
{
  if (current_level->get_bg(x,y)!=type)
  {
    current_level->put_bg(x,y,type);
    for (view *f=first_view;f;f=f->next)
      if (f->drawable())
        draw_map(f);    
  }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int game::in_area(event &ev, int x1, int y1, int x2, int y2)
{
    return (last_demo_mx >= x1 && last_demo_mx <= x2 && last_demo_my >= y1 && last_demo_my <= y2);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::request_level_load(char *name)
{
    strcpy(req_name, name);
}

extern int start_doubled;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void fade_in(image *im, int steps)
{
    palette *old_pal = pal->copy();
    int i;
    if (im) {
        screen->clear();
        im->put_image(screen,(xres+1)/2-im->width()/2,(yres+1)/2-im->height()/2);
    }
    
    for (i = 0; i < steps; i++) {
        uchar *sl1 = (uchar*) pal->addr();    
        uchar *sl2 = (uchar*) old_pal->addr();    
        int j;
        int v = (i + 1) * 256 / steps;
        for (j = 0; j < 256; j++) {
            *(sl1) = ((int) *(sl2)) * v / 256;  sl1++; sl2++;
            *(sl1) = ((int) *(sl2)) * v / 256;  sl1++; sl2++;
            *(sl1) = ((int) *(sl2)) * v / 256;  sl1++; sl2++;
        }
        pal->load();
        eh->flush_screen();
        milli_wait(25);
    }
    delete pal;
    pal=old_pal;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void fade_out(int steps)
{
    palette *old_pal=pal->copy();
    int i;
    for (i=0;i<steps;i++) {
        uchar *sl1=(uchar *)pal->addr();    
        uchar *sl2=(uchar *)old_pal->addr();    
        int j;
        int v=(steps-i)*256/steps;
        for (j=0;j<256;j++) {
            *(sl1)=((int)*(sl2))*v/256;  sl1++; sl2++;
            *(sl1)=((int)*(sl2))*v/256;  sl1++; sl2++;
            *(sl1)=((int)*(sl2))*v/256;  sl1++; sl2++;
        }
        pal->load();
        eh->flush_screen();
        milli_wait(25);
    }
    screen->clear();
    eh->flush_screen();
    delete pal;
    pal=old_pal;
    
    pal->load();
}

int text_draw(int y, int x1, int y1, int x2, int y2, char *buf, JCFont *font, uchar *cmap, char color);

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void do_title()
{
#ifdef _DEBUG
    return;
#endif
    if (cdc_logo != -1) {
        if (sound_avail & MUSIC_INITIALIZED) {
            if (current_song) {
                current_song->stop(); 
                delete current_song;
            }
            current_song = new song("music/intro.hmi");
            current_song->play(music_volume);
        }
        void *logo_snd = symbol_value(make_find_symbol("LOGO_SND"));

        if (DEFINEDP(logo_snd) && (sound_avail & SFX_INITIALIZED))
            cash.sfx(lnumber_value(logo_snd))->play(sfx_volume);

        image blank(2, 2);
        blank.clear();
        eh->set_mouse_shape(blank.copy(), 0, 0);      // don't show mouse
        fade_in(cash.img(cdc_logo), 32);
    
        milli_wait(400);

        void *space_snd = symbol_value(make_find_symbol("SPACE_SND"));

        fade_out(32);
        milli_wait(100);

        int i, abort = 0;
        char *str = lstring_value(eval(make_find_symbol("plot_start")));
    
        bFILE *fp = open_file("art/smoke.spe","rb");
        if (!fp->open_failure()) {
            spec_directory sd(fp);
            palette *old_pal=pal;
            pal = new palette(sd.find(SPEC_PALETTE),fp);
            pal->shift(1);

            image *gray=new image(sd.find("gray_pict"),fp);
            image *smoke[5];

            char nm[20];
            for (i = 0; i < 5; i++) {
	            sprintf(nm, "smoke%04d.pcx", i + 1);
	            smoke[i] = new image(sd.find(nm), fp);
            }

            screen->clear();
            pal->load();

            int dx = (xres + 1) / 2 - gray->width() / 2, 
                dy = (yres + 1) / 2 - gray->height() / 2;
            gray->put_image(screen, dx, dy);
            smoke[0]->put_image(screen, dx + 24, dy + 5);

            fade_in(NULL, 16);
            uchar cmap[32];
            for (i = 0; i < 32; i++)
                cmap[i] = pal->find_closest(i * 256 / 32, i * 256 / 32, i * 256 / 32);

            event ev; ev.type = EV_SPURIOUS;
            time_marker start;
            time_marker SpaceSndWait;
            int Done = 0;
            
            while (!Done && ev.type != EV_KEY && ev.type != EV_MOUSE_BUTTON) {
                time_marker now;
                i = (int) (now.diff_time(&start) * 100 / 14);
	            gray->put_image(screen, dx, dy);
	            smoke[i % 5]->put_image(screen, dx + 24, dy + 5);
	            Done = text_draw(205 - i, dx + 15, dy, dx + 320 - 15, dy + 199, str, eh->font(), cmap, eh->bright_color());
	            eh->flush_screen();

	            while (eh->event_waiting() && ev.type != EV_KEY)
                    eh->get_event(ev);
                if (now.diff_time(&SpaceSndWait) > 1.0) {
                    if (DEFINEDP(space_snd) && (sound_avail & SFX_INITIALIZED))
	                    cash.sfx(lnumber_value(space_snd))->play(sfx_volume * 90 / 127);
                    SpaceSndWait.get_time();
                }
            }

            the_game->reset_keymap();

            fade_out(16);

            for (i = 0; i < 5; i++) 
                delete smoke[i];
            delete gray;

            delete pal;
            pal = old_pal;
        }
        delete fp;

        for (i = 0; i < 100 && !abort; i++) {}

        if (title_screen >= 0)
            fade_in(cash.img(title_screen), 32);
    }
}

extern int start_edit;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::request_end()
{
  req_end = 1;
}

extern void fast_load_start_recording(char *name);
extern void fast_load_stop_recording();
extern void fast_load_start_reloading(char *name);
extern void fast_load_stop_reloading();

extern void PreloadData();

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
game::game(int argc, char **argv)
{
    int i;
    
    req_name[0] = 0;
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
    
    for (i = 1; i < argc; i++)
        if (!strcmp(argv[i],"-no_delay")) {
            no_delay = 1;
            dprintf("Frame delay off (-nodelay)\n");
        }
        
        
    image_init();  
    zoom = 15;  
    no_delay = 0;
    
    if (get_option("-use_joy")) {
        has_joystick = joy_init(argc, argv);
        dprintf("Joystick : ");
        if (has_joystick)
            dprintf("detected\n");
        else
            dprintf("not detected\n");
    }
    else
        has_joystick = 0;
    
    fast_load_start_recording("fastload.dat");
    load_data(argc, argv);  
    fast_load_stop_recording();
    
    get_key_bindings();
    
    reset_keymap();                   // we think all the keys are up right now
    finished = 0;
    
    calc_light_table(pal);
    
    if (current_level == NULL && net_start()) { // if we joined a net game get level from server
        if (!request_server_entry()) {
            WriteErr(0, "Error requesting entry to server!")
        }
        net_reload();
    }
    
    recalc_local_view_space();   // now that we know what size the screen is...
    
    morph_dark_color = get_color(cash.img(window_colors)->pixel(2, 1));
    morph_bright_color = get_color(cash.img(window_colors)->pixel(0, 1));
    morph_med_color = get_color(cash.img(window_colors)->pixel(1, 1));
    morph_sel_frame_color = pal->find_closest(255, 255, 0);
    light_connection_color = morph_sel_frame_color;
    
    dark_color = get_color(cash.img(window_colors)->pixel(2, 0));
    bright_color = get_color(cash.img(window_colors)->pixel(0, 0));
    med_color = get_color(cash.img(window_colors)->pixel(1, 0));
    
    if (NILP(symbol_value(l_default_font))) {
        WriteErr(0, "No font defined, set symbol default-font to an image name\n");
    }
    int font_pict;
    if (big_font_pict != -1) {
        if (small_font_pict != -1) {
            if (xres / (start_doubled ? 2 : 1) > 400) {
                font_pict = big_font_pict;
            } 
            else
                font_pict = small_font_pict;
        }
        else
            font_pict = big_font_pict;
    }
    else
        font_pict = small_font_pict;
    
    if (console_font_pict == -1)
        console_font_pict = font_pict;
    game_font = new JCFont(cash.img(font_pict));
    
    chat = new chat_console(console_font, 50, 6);
    chat->hide();

    WriteErr(eh->has_mouse(), "No mouse driver detected, please rectify.\n");

    eh->set_mouse_shape(cash.img(c_normal)->copy(), 1, 1);
    gamma_correct(pal);

    GameConsole->hide();

    if (main_net_cfg == NULL || (main_net_cfg->state != net_configuration::SERVER && main_net_cfg->state != net_configuration::CLIENT)) {
        if (!start_edit && !net_start())
            do_title();
    }
    else if (main_net_cfg && main_net_cfg->state == net_configuration::SERVER) {
        the_game->load_level(level_file);
        start_running = 1;
    }
    
    dev |= DRAW_FG_LAYER | DRAW_BG_LAYER | DRAW_PEOPLE_LAYER | DRAW_HELP_LAYER | DRAW_LIGHTS | DRAW_LINKS;
    
    if (dev & EDIT_MODE)
        set_frame_size(0);
    // do_intro();
    state = START_STATE;         // first set the state to one that has windows

    if (start_running)
        set_state(RUN_STATE);
    else {
        screen->clear();  
        if (title_screen >= 0) {
            image *tit = cash.img(title_screen);
            tit->put_image(screen, screen->width() / 2 - tit->width() / 2, screen->height() / 2 - tit->height() / 2);
        }
        set_state(MENU_STATE);   // then go to menu state so windows will turn off
    }
}


time_marker *led_last_time = NULL, *fps_mark_start = NULL;
double desired_fps = 15.0;
double avg_fps = 15.0, possible_fps = 15.0;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::toggle_delay()
{
    no_delay = !no_delay;
    if (no_delay)
        show_help(symbol_str("delay_off"));
    else
        show_help(symbol_str("delay_on"));
    avg_fps = possible_fps = 15.0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::show_time()
{
    if (first_view && fps_on) {
        char str[10];
        sprintf(str, "%d", (long) (avg_fps * 10.0));
        console_font->put_string(screen, first_view->cx1, first_view->cy1, str);
        
        sprintf(str, "%d", total_active);
        console_font->put_string(screen, first_view->cx1, first_view->cy1 + 10, str);
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::update_screen()
{
    if (state == HELP_STATE)
        draw_help();
    else if (current_level) {
        if (!(dev & EDIT_MODE) || refresh) {
            view *f = first_view;
            current_level->clear_active_list();
            for (; f; f = f->next) {
                if (f->focus) {
	                int w, h;

	                w = (f->cx2 - f->cx1 + 1);
	                h = (f->cy2 - f->cy1 + 1);

	                total_active += current_level->add_drawables(f->xoff() - w / 4, f->yoff() - h / 4,
						                                         f->xoff() + w + w / 4, f->yoff() + h + h / 4);
                }
            }

            for (f = first_view; f; f = f->next) {
                if (f->drawable()) {
                    if (interpolate_draw) {
                        draw_map(f, 1);
	                    eh->flush_screen();
                    }
                    draw_map(f, 0);
                }
            }
            if (current_automap)
                current_automap->draw();
        }
        if (state == PAUSE_STATE) {
            for (view *f = first_view; f; f = f->next)
                cash.img(pause_image)->put_image(screen, (f->cx1 + f->cx2) / 2 - cash.img(pause_image)->width() / 2, f->cy1 + 5, 1);
        }
        show_time();
    }

    if (state == RUN_STATE && cash.prof_is_on())
        cash.prof_poll_end();

    eh->flush_screen();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::do_intro()
{

}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int game::calc_speed()
{
    static time_marker FrameMark;
    int ret = 0;
    if (dev & EDIT_MODE && !need_delay)
        return 1;
    if (fps_mark_start) {
        time_marker t;
        
        // find average fps for last 10 frames
        double td = t.diff_time(fps_mark_start);
        if (td < 0.001)     // something is rotten in the state of demark
            td = 0.001;
        
        avg_fps = avg_fps * 9.0 / 10.0 + 1.0 / (td * 10.0);  
        possible_fps = possible_fps * 9.0 / 10.0 + 1.0 / (td * 10.0);  
        
        if (avg_fps > 14) {
            if (massive_frame_panic > 20)
                massive_frame_panic = 20;
            else if (massive_frame_panic)
                massive_frame_panic--;
        }
        
        if (avg_fps > 15 && ((dev & EDIT_MODE) == 0 || need_delay)) {
            frame_panic = 0;
            double stime = 1.0 / desired_fps;
//            long stime = (long) ((1 / 15.0 - 1.0 / possible_fps) * 1000.0); 
            if (!no_delay) {
//                milli_wait(stime);
                avg_fps -= 1.0 / (td * 10.0);        // subtract out old estimate

                time_marker t; 
                if (t.diff_time(&FrameMark) >= stime) {
                    FrameMark.get_time();
                    ret = 1;
                }
                
                // find average fps for last 10 frames
                double td = t.diff_time(fps_mark_start);
                if (td < 0.00001)     // something is rotten in the state of demark
                    td = 0.00001;
                
                avg_fps += 1.0 / (td * 10.0);       // add in new estimate
            }
            else
                ret = 1;
        }
        else if (avg_fps < 14) {
            if (avg_fps < 10)
                massive_frame_panic++;
            frame_panic++;
            ret = 1;
        }
        delete fps_mark_start;    
    }
    fps_mark_start = new time_marker;
    return ret;
}

extern int start_edit;

void single_render();
void double_render();

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::get_input()
{
    event ev;  
    idle_ticks++;
    while (event_waiting()) {
        get_event(ev);
        
        if (ev.type == EV_MOUSE_MOVE)
            last_input = ev.window;
        // don't process repeated keys in the main window, it will slow down the game handle such
        // useless events. However in other windows it might be useful, such as in input windows
        // where you want to repeatedly scroll down..
        if (ev.type != EV_KEY || !key_down(ev.key) || ev.window || (dev & EDIT_MODE)) {
            if (ev.type == EV_KEY) {
                set_key_down(ev.key, 1);
                if (playing_state(state)) {
                    if (ev.key < 256) {
                        if (chat && chat->chat_event(ev))
                            base->packet.write_byte(SCMD_CHAT_KEYPRESS);
                        else
                            base->packet.write_byte(SCMD_KEYPRESS);
                    }
                    else
                        base->packet.write_byte(SCMD_EXT_KEYPRESS);
                    base->packet.write_byte(client_number());
                    if (ev.key > 256)
                        base->packet.write_byte(ev.key - 256);
                    else
                        base->packet.write_byte(ev.key);	  
                }
            }
            else if (ev.type == EV_KEYRELEASE) {
                set_key_down(ev.key, 0);
                if (playing_state(state)) {
                    if (ev.key < 256)
                        base->packet.write_byte(SCMD_KEYRELEASE);
                    else
                        base->packet.write_byte(SCMD_EXT_KEYRELEASE);
                    base->packet.write_byte(client_number());
                    if (ev.key > 255)
                        base->packet.write_byte(ev.key - 256);
                    else
                        base->packet.write_byte(ev.key);	  
                }	
            }
            
            if ((dev & EDIT_MODE) || start_edit || ev.type == EV_MESSAGE)
                dev_cont->handle_event(ev);
            
            view *v = first_view;
            for (; v; v = v->next)
                if (v->local_player() && v->handle_event(ev))
                    ev.type = EV_SPURIOUS;       // if the event was used by the view, gobble it up
                
            if (current_automap)
                current_automap->handle_event(ev);
            
            help_handle_event(ev);    
            mousex = last_demo_mx;
            mousey = last_demo_my;
            
            if (ev.type == EV_MESSAGE) {
                switch (ev.message.id) {
                case CALB_JOY : {
                        if (!joy_win) {
                            joy_win = eh->new_window(80, 50, -1, -1,
                                new button(70, 9, JOY_OK, "OK",
                                new info_field(0, 30, DEV_NULL,
                                " Center joystick and\n"
                                "press the fire button", NULL)),
                                "Joystick");
                            set_state(JOY_CALB_STATE); 
                        }
                    }
                case TOP_MENU : {
                        menu_select(ev);
                    } break;
                case DEV_QUIT : {
                        finished = 1;
                    } break;
                }
            }	    
            else if (ev.type == EV_CLOSE_WINDOW && ev.window == top_menu) {
                eh->close_window(top_menu);
                top_menu = NULL;
            }
            if (ev.type == EV_KEY && (ev.key == '~' || ev.key == '`'))
                GameConsole->toggle();
            switch (state) {
            case JOY_CALB_STATE :
                { joy_calb(ev); } break;
            case INTRO_START_STATE :
                {
                    do_intro(); 
                    if (dev & EDIT_MODE)
                        set_state(RUN_STATE);
                    else
                        set_state(MENU_STATE); 
                } break;     
            case PAUSE_STATE : if (ev.type == EV_KEY && (ev.key == JK_SPACE || ev.key == JK_ENTER)) 
                               { set_state(RUN_STATE); } break;
            case RUN_STATE : {
                    if (ev.window == NULL) {
                        switch (ev.type) {
                        case EV_KEY:
                            {
                                switch (ev.key) {
                                case 'm' : {
                                    if (dev & MAP_MODE)
                                        dev -= MAP_MODE;
                                    else if ((player_list && player_list->next) || dev&EDIT_MODE)
                                        dev |= MAP_MODE;
                                
                                    if (!(dev & MAP_MODE)) {
                                        if (dev_cont->tbw) dev_cont->toggle_toolbar();
                                        edit_mode=ID_DMODE_DRAW;
                                    }
                                    need_refresh();	 
                                } break;
                                case 'v': 
                                    { eh->push_event(new event(DO_VOLUME,NULL)); } break;
                                case 'p': 
                                    { if (!(dev&EDIT_MODE) && (!main_net_cfg || 
                                        (main_net_cfg->state!=net_configuration::SERVER &&
                                        main_net_cfg->state!=net_configuration::CLIENT)))  
                                        set_state(PAUSE_STATE); 
                                    } break;
                                case 'S': 
                                    if (start_edit)
                                    { eh->push_event(new event(ID_LEVEL_SAVE,NULL)); } break;
                                case JK_TAB : 
                                    { if (start_edit) toggle_edit_mode(); need_refresh(); } break;
                                case 'c':
                                case 'C':
                                    if (chatting_enabled && (!(dev & EDIT_MODE) && chat))
                                        chat->toggle();
                                    break;
                                case '9': 
                                    dev = dev ^ PERFORMANCE_TEST_MODE;
                                    need_refresh();
                                    break;
                                }
                            } break;	  	  
                        case EV_RESIZE : {
                                view *v;
                                for (v = first_view; v; v = v->next) { // see if any views need to change size
                                    if (v->local_player()) {
                                        int w = (xres - 10) / (small_render ? 2 : 1);
                                        int h = (yres - 10) / (small_render ? 2 : 1);
                                        
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
                        case EV_REDRAW : screen->add_dirty(ev.redraw.x1, ev.redraw.y1,
                                             ev.redraw.x2, ev.redraw.y2); break;
                        case EV_MESSAGE :	    
                            switch (ev.message.id)
                            {
                            case RAISE_SFX :
                                if (sfx_volume != 127)
                                    sfx_volume = min(127, sfx_volume + 16);
                                break;
                            case LOWER_SFX :
                                if (sfx_volume != 0)
                                    sfx_volume = max(sfx_volume - 16, 0);
                                break;
                            case RAISE_MUSIC :
                                if (music_volume != 126) {
                                    music_volume = min(music_volume + 16, 127);
                                    if (current_song)
                                        current_song->set_volume(music_volume);
                                }
                                break;
                            case LOWER_MUSIC :
                                if (music_volume != 0) {
                                    music_volume = max(music_volume - 16, 0);
                                    if (current_song)
                                        current_song->set_volume(music_volume);
                                }
                                ((button*) ev.message.data)->push();	      
                                break;
                            }
                        }
                    }
                } break;
            }
    }
  }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void net_send(int force = 0)
{
    if ((!(dev&EDIT_MODE)) || force) {
        if (demo_man.state == demo_manager::PLAYING) {
            base->input_state = INPUT_PROCESSING;
        } 
        else {
            if (!player_list->focus) {
                WriteErr(0, "Players have not been created\ncall create_players");
            }
            
            view *p = player_list;
            for (; p; p = p->next)
                if (p->local_player())
                    p->get_input();

            base->packet.write_byte(SCMD_SYNC);
            base->packet.write_short(make_sync());

            if (base->join_list)
                base->packet.write_byte(SCMD_RELOAD);
            
            //      printf("save tick %d, pk size=%d, rand_on=%d, sync=%d\n",current_level->tick_counter(),
            //	     base->packet.packet_size(),rand_on,make_sync());
            send_local_request();
        }
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void net_receive()
{
    if (!(dev&EDIT_MODE) && current_level) {
        uchar buf[PACKET_MAX_SIZE + 1];
        int size;
        
        if (demo_man.state == demo_manager::PLAYING) {
            if (!demo_man.get_packet(buf, size))
                size = 0;
            base->packet.packet_reset();
            base->mem_lock = 0;
        }
        else {
            size = get_inputs_from_server(buf);
            if (demo_man.state == demo_manager::RECORDING)
                demo_man.save_packet(buf, size);
        }
        process_packet_commands(buf, size);
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void game::step()
{
    clear_tmp();
    if (current_level) {
        current_level->unactivate_all();
        total_active=0;
        for (view *f=first_view;f;f=f->next) {
            if (f->focus) {
	            f->update_scroll();
	            int w,h;

	            w=(f->cx2-f->cx1+1);
	            h=(f->cy2-f->cy1+1);
                total_active += current_level->add_actives(f->xoff()-w/4,f->yoff()-h/4,
						       f->xoff()+w+w/4,f->yoff()+h+h/4);
            }
        }
    }

    if (state == RUN_STATE) {
        if ((dev & EDIT_MODE) || (main_net_cfg && (main_net_cfg->state == net_configuration::CLIENT ||
					        main_net_cfg->state == net_configuration::SERVER)))
            idle_ticks = 0;

        if (demo_man.current_state() == demo_manager::NORMAL && idle_ticks > 420 && demo_start) {
            idle_ticks = 0;
            set_state(MENU_STATE);
        }
        else if (!(dev & EDIT_MODE)) {              // if edit mode, then don't step anything
            if (key_down(JK_ESC)) {
	            set_state(MENU_STATE);
	            set_key_down(JK_ESC, 0);
            }
            ambient_ramp = 0;
            view *v;
            for (v = first_view; v; v = v->next)
                v->update_scroll();

            cash.prof_poll_start();
            current_level->tick();
            sbar.step();
        } 
        else
            dev_scroll();  
    }
    else if (state == JOY_CALB_STATE) {
        event ev;
        joy_calb(ev);
    }
    else if (state == MENU_STATE)
        main_menu();  
    
    if ((key_down('x') || key_down(JK_F4)) && (key_down(JK_ALT_L) || key_down(JK_ALT_R)) && confirm_quit())
        finished = 1;
}

extern void *current_demo;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
game::~game()
{
    GameConsole->show();
    current_demo = NULL;
    if (first_view == player_list)
        first_view = NULL;
    while (player_list) {
        view *p = player_list;
        game_object *o = p->focus;
        player_list = player_list->next;
        delete p;
        o->set_controller(NULL);
        if (current_level && o)
            current_level->delete_object(o);
        else
            delete o;
    }
    
    if (current_level)
        delete current_level;
    current_level = NULL;
    
    if (first_view!=player_list) {
        while (player_list) {
            view *p = player_list;
            player_list = player_list->next;
            delete p;
        }
    }
    
    while (first_view) {
        view *p = first_view;
        first_view = first_view->next;
        delete p;
    }
    
    player_list=NULL;  
    
    if (old_view) {
        first_view = old_view;
        while (first_view) {
            view *p = first_view;
            first_view = first_view->next;
            delete p;
        }
    }
    old_view = NULL;
    
    int i = 0;
    for (; i < total_objects; i++) {
        jfree(object_names[i]);
        delete figures[i];
    }
    free_pframes();
    if (fps_mark_start)
        delete fps_mark_start;
    fps_mark_start = NULL;

    delete pal;
    jfree(object_names);
    object_names = NULL;
    jfree(figures);
    figures = NULL;
    
    jfree(backtiles);
    backtiles = NULL;
    jfree(foretiles);
    foretiles = NULL;
    if (total_weapons)
        jfree(weapon_types);
    weapon_types = NULL;
    
    config_cleanup();
    //  delete color_table;
    //  delete eh;
    delete game_font;
    //  delete big_font;
    //  delete console_font;
    if (total_help_screens)
        jfree(help_screens);
    help_screens = NULL;
    
    image_uninit();
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void game::draw(int scene_mode)
{
    screen->add_dirty(0, 0, xres, yres);
    image *bt=cash.img(border_tile);
    int tw = bt->width(), th = bt->height(), dy = 0;
    int xt = screen->width() / tw + 1, yt = screen->height() / th + 1;
    screen->clear();
    
    if (scene_mode) {
        char *helpstr="ARROW KEYS CHANGE TEXT SPEED";
        eh->font()->put_string(screen, screen->width() / 2 - (eh->font()->width() * strlen(helpstr)) / 2 + 1,
            screen->height() - eh->font()->height() - 5 + 1, helpstr, eh->dark_color());
        eh->font()->put_string(screen, screen->width() / 2 - (eh->font()->width() * strlen(helpstr)) / 2,
            screen->height() - eh->font()->height() - 5, helpstr, eh->bright_color());
    }
    for (view *f = first_view; f; f = f->next)
        draw_map(f, 0);
    
    sbar.redraw(screen);
}

int external_print = 0;

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void start_sound(int argc, char **argv)
{
    sfx_volume = music_volume = 127;
    
    for (int i = 1; i < argc; i++)
        if (!strcmp(argv[i], "-sfx_volume")) {
            i++;
            if (atoi(argv[i]) >= 0 && atoi(argv[i]) < 127)
                sfx_volume = atoi(argv[i]);
            else 
                dprintf("Bad sound effects volume level, use 0..127\n");      
        }
        else if (!strcmp(argv[i], "-music_volume")) {
            i++;
            if (atoi(argv[i]) >= 0 && atoi(argv[i]) < 127)
                music_volume = atoi(argv[i]);
            else 
                dprintf("Bad music volume level, use 0..127\n");      
        }
    sound_avail = sound_init(argc, argv);
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void game_printer(char *st)
{
    if (GameConsole)
        GameConsole->put_string(st);
/*    if (dev_console && !external_print) {
        dev_console->put_string(st);
    }
    else { 
        fprintf(stderr, "%s", st);
    } */
    _RPT0(_CRT_WARN, st);
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void game_getter(char *st, int max)
{
    if (!max)
        return;
    max--;
    *st = 0;
    if (GameConsole && !external_print) {
        GameConsole->show();
        int t = 0;
        event ev;
        do {
            get_event(ev);
            if (ev.type == EV_KEY) {
                if (ev.key==JK_BACKSPACE) {
                    if (t) {
                        GameConsole->printf("%c", ev.key);
                        t--;
                        st--;
                        *st = 0;
                        max++;
                    }
                }
                else if (ev.key >= ' ' && ev.key <= '~') {
                    GameConsole->printf("%c",ev.key);
                    *st = ev.key;
                    t++;
                    max--;
                    st++;
                    *st = 0;
                }			  
            }
            eh->flush_screen();
        } while (ev.type != EV_KEY || ev.key != JK_ENTER);    
        dprintf("\n");
    }
    else {
        if (fgets(st, max, stdin)) {
            if (*st)
                st[strlen(st) - 1] = 0;
        }
    }
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void show_startup()
{
    show_verinfo(start_argc, start_argv);
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
char *get_line(int open_braces)
{
    char *line = (char*) jmalloc(1024, "lstring");
    game_getter(line, 1024);
    fgets(line, 1024, stdin);
    
    char prev=' ';
    for (char *s = line; *s && (prev!=' ' || *s!=';'); s++) {
        prev = *s;
        if (*s=='(') open_braces++;
        else if (*s==')') open_braces--;
    }
    if (open_braces < 0)
        dprintf("\nToo many )'s\n");
    else if (open_braces > 0) {
        char *s2 = get_line(open_braces);
        line = (char*) jrealloc(line, strlen(line) + strlen(s2) + 1, "lstring");
        strcat(line, s2);
        jfree(s2);
    }
    return line;
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void check_for_upgrade(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
        if (!strcmp(argv[i], "-upgrade")) {
            lisp_init(0xf000, 0x30000);
            char *prog = "(load \"lisp/upgrade.lsp\")", *cs;
            cs = prog;
            if (!eval(compile(cs)))
                dprintf("file does not exist : lisp/upgrade.lsp, cannot upgrade\n");
        }        
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void check_for_lisp(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i],"-lisp")) {
            lisp_init(0xf000, 0x30000);
            dprintf(" CLIVE (C) 1995 Jonathan Clark, all rights reserved\n"
                    "   (C LISP interpreter and various extentions)\n"
                    "Type (CTRL-Z) to exit\n");
            
            while (!feof(stdin)) {
                dprintf("Lisp> ");
                char *l = get_line(0);
                char *s = l;
                while (*s) {
                    void *prog = compile(s);
                    l_user_stack.push(prog);
                    while (*s==' ' || *s=='\t' || *s=='\r' || *s=='\n') s++;
                    lprint(eval(prog));
                    l_user_stack.pop(1);
                }
                jfree(l);
            }      
            WriteErr(0, "End of input : bye\n");
        }
    }
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void music_check()
{
    if (sound_avail & MUSIC_INITIALIZED) {
        if (current_song && !current_song->playing()) {
            current_song->play();
            dprintf("song finished\n");
        }
        if (!current_song) {
            current_song=new song("music/intro.hmi");
            current_song->play(music_volume);
            
            /*      if (DEFINEDP(symbol_function(l_next_song)))  // if user function installed, call it to load up next song
            {
            int sp=current_space;
            current_space=PERM_SPACE;
            eval_function((lisp_symbol *)l_next_song,NULL);
            current_space=sp;
        } */
        } 
    }
}

//void setup(int argc, char **argv);

void share_end();
void show_end();

void show_sell(int abortable);

extern pmenu *dev_menu;


extern int jmalloc_max_size;
extern int jmalloc_min_low_size;

extern int (*verify_file_fun)(char *,char *);

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
int registered_ok(char *filename, char *mode)
{
    if (registered) return 1;
    
    char name[256],*c;
    c=name;
    while (*filename) { *c=*(filename++); *c=toupper(*c); c++; } *c=0;
    if (strstr(name,"REGISTER"))
        return 0;
    else return 1;
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void game_net_init(int argc, char **argv)
{
    int nonet = !net_init(argc, argv);
    if (nonet)
        dprintf("No network driver, or network driver returned failure\n");
    else  {
        set_file_opener(open_nfs_file);
        if (main_net_cfg && main_net_cfg->state == net_configuration::CLIENT) {
            if (set_file_server(net_server))
                start_running = 1;
            else {
                dprintf("Unable to attach to server, quitting\n");
            }
        } 
        else {
            int i;
            for (i = 1; i < argc - 1; i++)
                if (!strcmp(argv[i], "-fs"))
                    if (!set_file_server(argv[i + 1]))
                        dprintf("Couldn't set default file server to %s\n", argv[i + 1]);
        }
    }	
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
int main(int argc, char *argv[]) {
    return start_abuse(argc, argv);
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void JWindowStartup()
{
    int dark_color, bright_color, med_color;
    PreloadData();
    dark_color = cash.img(window_colors)->pixel(2, 0);
    bright_color = cash.img(window_colors)->pixel(0, 0);
    med_color = cash.img(window_colors)->pixel(1, 0);
    
    if (NILP(symbol_value(l_default_font))) {
        WriteErr(0, "No font defined, set symbol default-font to an image name\n");
    }
    int font_pict;
    if (big_font_pict != -1) {
        if (small_font_pict != -1) {
            if (xres / (start_doubled ? 2 : 1) > 400) {
                font_pict = big_font_pict;
            } 
            else
                font_pict = small_font_pict;
        }
        else
            font_pict = big_font_pict;
    }
    else
        font_pict = small_font_pict;
    
    if (console_font_pict == -1)
        console_font_pict = font_pict;
    
    game_font = new JCFont(cash.img(font_pict));
    
    console_font = new JCFont(cash.img(console_font_pict));
    
    eh = new window_manager(screen, pal, bright_color, med_color, dark_color, game_font);  
    
    image blank(2, 2); 
    blank.clear();
    eh->set_mouse_shape(blank.copy(), 0, 0);      // don't show mouse
    
//    delete stat_man;  // move to a graphical status manager
    gui_status_manager *gstat = new gui_status_manager();
    gstat->set_window_title("status");
    stat_man = gstat;
    if (!get_option("-lisp"))
        GameConsole = new console(console_font, (xres / console_font->width()), (yres >> 1) / console_font->height(), "Console");
    else
        GameConsole = new console(console_font, (xres / console_font->width()), yres / console_font->height(), "Console");
    GameConsole->show();
    pal->load();
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void GameLoop(int argc, char **argv)
{
    do {
        if (main_net_cfg) {
            if (!main_net_cfg->notify_reset()) {
                if (!get_option("-no_timer"))
                    timer_uninit();
                sound_uninit();
                WriteErr(0, "Couldn't reset");
            }
        }
        
        game_net_init(argc, argv);
        lisp_init(0x16000, 0x94000);
        
        dev_init(argc, argv);
        
        game *g = new game(argc, argv);
        
        dev_cont = new dev_controll();
        dev_cont->load_stuff();
        
        g->get_input();      // prime the net
        int xx;
        for (xx = 1; xx < argc; xx++)
            if (!strcmp(argv[xx], "-server")) {
                xx++;
                if (!become_server(argv[xx]))
                    WriteErr(0, "Unable to start server\n");
                xx = argc + 1;
            }
            
        if (main_net_cfg) 
            wait_min_players();
        
        net_send(1);
        if (net_start()) {
            g->step();                    // process all the objects in the 
            while (!g->calc_speed()) {};
            g->update_screen();               // redraw the screen with any changes
        }
        
        while (!g->done()) {
            music_check();
            if (req_end) {
                eh->hide_windows();
                if (current_automap)
                    delete current_automap;
                current_automap = NULL;
				delete current_level;
				current_level = NULL;
				if (!registered)
				    share_end();
				else 
                    show_end();
				the_game->set_state(MENU_STATE);
				req_end = 0;
            }

            if (demo_man.current_state() == demo_manager::NORMAL) {
				net_receive();
            }
            
            if (req_name[0]) {           // see if a request for a level load was made during the last tick
                eh->hide_windows();
                g->load_level(req_name);
				req_name[0] = 0;
				g->draw(g->state == SCENE_STATE);
            }

      //    if (demo_man.current_state()!=demo_manager::PLAYING)
            g->get_input();
            if (demo_man.current_state() == demo_manager::NORMAL)
                net_send();
            else 
                demo_man.do_inputs();
            service_net_request();
            g->step();                        // process all the objects in the 
            server_check();
            while (!g->calc_speed()) {};
            if (!req_name[0])                // see if a request for a level load was made during the last tick
                g->update_screen();               // redraw the screen with any changes
        }
        
        net_uninit();
        
        if (net_crcs) {
            net_crcs->clean_up();
            delete net_crcs;
            net_crcs = NULL;
        }
        
        delete chat;
        
        if (!registered)
            show_sell(0);

        if (small_render) { 
            delete small_render; 
            small_render = NULL; 
        }
        
        if (current_song) {
            current_song->stop();
            delete current_song; 
            current_song = NULL; 
        }
        
        cash.empty();
        
        if (dev_console) {
            delete dev_console;
            dev_console = NULL;
        }
        
        if (dev_menu) {
            delete dev_menu;
            dev_menu = NULL;
        }
        
        if (g)
            delete g;
        g = NULL;
        if (old_pal) 
            delete old_pal; 
        old_pal = NULL;
        compiled_uninit();
        delete_all_lights();
        jfree(white_light_initial);
        
        for (int i = 0; i < TTINTS; i++) 
            jfree(tints[i]);
        
        dev_cleanup();
        if (dev_cont)
            delete dev_cont;
        dev_cont = NULL;
        
        if (!(main_net_cfg && main_net_cfg->restart_state())) {
            void *end_msg=make_find_symbol("end_msg");
            if (DEFINEDP(symbol_value(end_msg)))
                printf("%s\n",lstring_value(symbol_value(end_msg)));
        }
        
        lisp_uninit();
        
        base->packet.packet_reset();
#ifdef _DEBUG
        mem_report("end.mem");
#endif
    } while (main_net_cfg && main_net_cfg->restart_state());
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void ShutdownEngine()
{
    delete stat_man;
    if (current_automap)
        delete current_automap;
    current_automap = NULL;

    if (GameConsole) {
        delete GameConsole;
        GameConsole = NULL;
    }
    
    if (main_net_cfg) { 
        delete main_net_cfg; 
        main_net_cfg = NULL; 
    }

    delete game_font;
    
    delete console_font;
    
    delete eh;
    
    set_filename_prefix(NULL);  // dealloc this mem if there was any
    set_save_filename_prefix(NULL);
    if (!get_option("-no_timer"))
        timer_uninit();
#ifdef _DEBUG    
    mem_report("end.mem");
#endif
    //  jmalloc_uninit();
    l_user_stack.clean_up();
    l_ptr_stack.clean_up();
    
    sound_uninit();
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
int start_abuse(int argc, char **argv)
{
    start_argc = argc;
    start_argv = argv;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-cprint"))
            external_print = 1;
        if (!strcmp(argv[i], "-min_low")) {
            i++;
            jmalloc_min_low_size = atoi(argv[i]);
        }
    }
    jmalloc_init(0x150000); 
    
    set_dprinter(game_printer);
    set_dgetter(game_getter);
    set_no_space_handler(handle_no_space);
    
    lisp_init(0x16000, 0x94000);
    JWindowStartup();
    lisp_uninit();
    
    show_startup();
    
    dprintf("DirectDraw successfully initialized to %dx%dx8\n\n", xres, yres);
    
    start_sound(argc, argv);
    
    if (!get_option("-no_timer"))
        timer_init();
    
    if (getenv("ABUSE_PATH"))       // look to see if we are supposed to fetch the data elsewhere
        set_filename_prefix(getenv("ABUSE_PATH"));   
    
    if (getenv("ABUSE_SAVE_PATH"))       // look to see if we are supposed to fetch the data elsewhere
        set_save_filename_prefix(getenv("ABUSE_SAVE_PATH"));   
    
    if (!get_option("-share")) {
        jFILE *fp = new jFILE("register/micron.vcd", "rb");
        if (!fp->open_failure()) {
            spec_directory sd(fp);
            if (sd.find("Copyright (C) 1995 Crack dot Com, All Rights reserved"))
                registered = 1;
        }
        delete fp;
    }
    
    verify_file_fun = registered_ok;
    
    jrand_init();
    jrand();  // so compiler doesn't complain
    
    set_spec_main_file("abuse.spe");
    
    check_for_lisp(argc, argv);
    check_for_upgrade(argc, argv);
    

    GameLoop(argc, argv);
    ShutdownEngine();

    return 0;
}