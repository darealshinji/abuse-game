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

#include "game.h"

#include "netcfg.h"
#include "input.h"
#include "cache.h"
#include "light.h"

#include "dev.h"

#include "net/sock.h"

extern char *get_login();
net_configuration *main_net_cfg = NULL;
extern char lsf[256];

extern net_protocol *prot;

net_configuration::net_configuration()
{
  strcpy(name,get_login());

  strcpy(server_name,"My Netgame");

  min_players=2;
  max_players=8;
  kills=25;
  port=20202;
  server_port=20202;
  state=SINGLE_PLAYER;
}


extern char game_name[50];

enum { NET_OK=1, NET_CANCEL, NET_SERVER_NAME, NET_NAME, NET_PORT, NET_SERVER_PORT, NET_MAX, NET_MIN, NET_KILLS, CFG_ERR_OK, NET_SERVER,
       NET_CLIENT, NET_SINGLE, NET_GAME=400,  MIN_1,MIN_2,MIN_3,MIN_4,MIN_5,MIN_6,MIN_7,MIN_8,   MAX_2,MAX_3,MAX_4,MAX_5,MAX_6,MAX_7,MAX_8,
       LVL_2,LVL_4,LVL_8,LEVEL_BOX } ;


void net_configuration::cfg_error(char const *msg)
{
    AWidgetList widgets;
    widgets << new AInfoField(ivec2(0, 0), 0, msg);
    widgets << new AButton(ivec2(0, 30), CFG_ERR_OK, symbol_str("ok_button"));
    AWindow *j = wm->CreateWindow(ivec2(-1, 0), ivec2(-1), symbol_str("input_error"), widgets);

    Event ev;
    do
    {
        wm->flush_screen();
        do { wm->get_event(ev); } while (ev.type==EV_MOUSE_MOVE && wm->IsPending());
    } while (ev.type!=EV_MESSAGE || ev.message.id!=CFG_ERR_OK || ev.type==EV_CLOSE_WINDOW || (ev.type==EV_KEY && ev.key==JK_ESC));
    wm->close_window(j);
    wm->flush_screen();
}

int net_configuration::restart_state()
{
  switch (state)
  {
    case RESTART_SERVER :
    case RESTART_CLIENT :
    case RESTART_SINGLE :
      return 1;
    default:
      break;
  }
  return 0;
}

int net_configuration::notify_reset()
{
  switch (state)
  {
    case RESTART_SERVER : { state=SERVER; } break;
    case RESTART_CLIENT : { state=CLIENT; } break;
    case RESTART_SINGLE : { state=SINGLE_PLAYER; } break;
    default: break;
  }


  return 1;
}


int net_configuration::confirm_inputs(InputManager *i, int server)
{
  if (server)
  {
    int kl;

    if (sscanf(i->get(NET_KILLS)->read(),"%d",&kl)!=1 || kl<1 || kl>99)  {  error(symbol_str("kill_error")); return 0; }

    char *nm=i->get(NET_NAME)->read();
    if (strstr(nm,"\"")) {  error(symbol_str("name_error")); return 0; }
    strcpy(name,nm);

    min_players=((AWidget *)(i->get(NET_MIN)->read()))->m_id - MIN_1 + 1;
    max_players=((AWidget *)(i->get(NET_MAX)->read()))->m_id - MAX_2 + 2;
    if (max_players<min_players)  {  error(symbol_str("max_error")); return 0; }

    char *s_nm=i->get(NET_SERVER_NAME)->read();
    if (strstr(s_nm,"\"")) {  error(symbol_str("game_error")); return 0; }

    strcpy(game_name,s_nm);

    bFILE *fp=open_file("addon/deathmat/gamename.lsp","wb");
    if (!fp->open_failure())
    {
      char str[100];
      sprintf(str,"(setq gamename \"%s\")\n",game_name);
      fp->write(str,strlen(str)+1);
    }
    delete fp;
    strcpy(lsf,"addon/deathmat/deathmat.lsp");


    fp=open_file("addon/deathmat/levelset.lsp","wb");
    if (!fp->open_failure())
    {
      char str[100];
      if (((AWidget *)(i->get(LEVEL_BOX)->read()))->m_id == LVL_2)
        sprintf(str,"(load \"addon/deathmat/small.lsp\")\n");
      else if (((AWidget *)(i->get(LEVEL_BOX)->read()))->m_id == LVL_4)
        sprintf(str,"(load \"addon/deathmat/medium.lsp\")\n");
      else
        sprintf(str,"(load \"addon/deathmat/large.lsp\")\n");
      fp->write(str,strlen(str)+1);
    }
    delete fp;


    kills=kl;

  } else  {
    char *nm=i->get(NET_NAME)->read();
    if (strstr(nm,"\"")) {  error(symbol_str("name_error")); return 0; }
    strcpy(name,nm);
  }

  bFILE *fp=open_file("addon/deathmat/username.lsp","wb");
  if (!fp->open_failure())
  {
    char str[100];
    sprintf(str,"(setq username \"%s\")\n",name);
    fp->write(str,strlen(str)+1);
  }
  delete fp;



  return 1;

}

extern int start_running,demo_start,start_edit;

/*int net_configuration::input()   // pulls up dialog box and input fileds
{
  AWidget *ilist=NULL;
  int x=0,y=0;

  AWindow *sv=wm->new_window(50,80,-1,-1,new AButton(0,0,NET_SERVER,symbol_str("server"),
                     new AButton(0,wm->font()->height()*2,NET_CLIENT,symbol_str("client"),
                     new AButton(0,wm->font()->height()*4,NET_SINGLE,symbol_str("single_play"),
                     new AButton(0,wm->font()->height()*6,NET_CANCEL,symbol_str("cancel_net"),
                        NULL)))),symbol_str("Networking"));

  Event ev;
  int done=0;
  do
  {
    wm->flush_screen();
    do { wm->get_event(ev); } while (ev.type==EV_MOUSE_MOVE && wm->IsPending());
    if (ev.type==EV_MESSAGE)
    {
      if (ev.message.id==NET_SERVER) { done=1; state=RESTART_SERVER;  start_edit=0; demo_start=0; start_running=1; }
      else if (ev.message.id==NET_CLIENT) { done=1; state=RESTART_CLIENT;  start_edit=0; demo_start=0; start_running=1; }
      else if (ev.message.id==NET_SINGLE) { done=1; state=RESTART_SINGLE;  start_edit=0; demo_start=0; start_running=0; }
      else if (ev.message.id==NET_CANCEL) { done=1; }
    } else if (ev.type==EV_CLOSE_WINDOW || (ev.type==EV_KEY & ev.key==JK_ESC)) done=1;

  } while (!done);

  wm->close_window(sv);
  wm->flush_screen();

  if (state==RESTART_SINGLE)
  {
    strcpy(lsf,"abuse.lsp");
    return 1;
  }
  if (ev.message.id==NET_CANCEL || state==RESTART_SINGLE) return 0;

  if (state==RESTART_SERVER)
  {
    ilist=new AButton(x,y,NET_CANCEL,symbol_str("cancel_button"),ilist);
    ilist=new AButton(x,y,NET_OK,       symbol_str("server"),ilist);
    ilist=new ATextField(x,y,NET_KILLS,symbol_str("kills_to_win"),"******",kills,ilist);
    ilist=new ATextField(x,y,NET_MAX,symbol_str("max_play"),"******",max_players,ilist);
    ilist=new ATextField(x,y,NET_MIN,symbol_str("min_play"),"******",min_players,ilist);
    ilist=new ATextField(x,y,NET_PORT,symbol_str("use_port"),"******",port,ilist);
    ilist=new ATextField(x,y,NET_NAME,symbol_str("your_name"),"****************",name,ilist);

  } else
  {
    ilist=new AButton(x,y,NET_CANCEL,symbol_str("cancel_button"),ilist);
    ilist=new AButton(x,y,NET_OK,symbol_str("client"),ilist);
//    ilist=new ATextField(x,y,NET_PORT,symbol_str("use_port"),"******",port,ilist);
    ilist=new ATextField(x,y,NET_SERVER_PORT,symbol_str("server_port"),"******",server_port,ilist);
    ilist=new ATextField(x,y,NET_SERVER_NAME,symbol_str("server_name"),"*********************************",game_name,ilist);
    ilist=new ATextField(x,y,NET_NAME,symbol_str("your_name"),"****************",name,ilist);
  }

  AWidget *i=ilist;
  for (; i; i=i->next)
  {
    i->y=y;
    int x1,y1,x2,y2;
    i->area(x1,y1,x2,y2);
    y=y2+2;
  }


  AWindow *nw=wm->new_window(0,0,-1,-1,ilist,symbol_str("Networking"));

  done=0;
  do
  {
    wm->flush_screen();
    do { wm->get_event(ev); } while (ev.type==EV_MOUSE_MOVE && wm->IsPending());
    if (ev.type==EV_MESSAGE && ev.message.id==NET_OK && confirm_inputs(nw,state==RESTART_SERVER))
      done=1;
    if (ev.type==EV_MESSAGE && (ev.message.id==NET_CANCEL || ev.message.id==NET_SINGLE))
       done=1;
    if (ev.type==EV_CLOSE_WINDOW || (ev.type==EV_KEY && ev.key==JK_ESC))
      done=1;

  } while (!done);
  wm->close_window(nw);
  wm->flush_screen();

  return ev.message.id==NET_OK;
}

*/


void net_configuration::error(char const *message)
{
  AImage *screen_backup = main_screen->copy();

  AImage *ns=cache.img(cache.reg("art/frame.spe","net_screen",SPEC_IMAGE,1));
  int ns_w=ns->Size().x,ns_h=ns->Size().y;
  int x=(xres+1)/2-ns_w/2,y=(yres+1)/2-ns_h/2;
  main_screen->PutImage(ns, ivec2(x, y));
  JCFont *fnt=wm->font();

  uint8_t *remap=white_light+30*256;

  uint8_t *sl = main_screen->scan_line(0);
  int xx = main_screen->Size().x * main_screen->Size().y;
  for (; xx; xx--,sl++) *sl=remap[*sl];

  int fx=x+ns_w/2-strlen(message)*fnt->Size().x/2,
      fy=y+ns_h/2-fnt->Size().y;

  fnt->PutString(main_screen, ivec2(fx + 1, fy + 1), message, wm->black());
  fnt->PutString(main_screen, ivec2(fx, fy), message, wm->bright_color());


  {
    char const *ok = symbol_str("ok_button");

    int bx=x+ns_w/2-strlen(ok)*fnt->Size().x/2-3,
        by=y+ns_h/2+fnt->Size().y*3;

    AWidgetList sb;
    sb << new AButton(vec2(bx, by), NET_SERVER, ok);

    InputManager inm(main_screen, sb);
    inm.allow_no_selections();
    inm.clear_current();

    int done=0;
    Event ev;
    do
    {
      wm->flush_screen();
      do  { wm->get_event(ev); } while (ev.type==EV_MOUSE_MOVE && wm->IsPending());
      inm.handle_event(ev,NULL);
      if ((ev.type==EV_KEY && (ev.key==JK_ESC || ev.key==JK_ENTER)) ||
      ev.type==EV_MESSAGE) done=1;
    } while (!done);
  }

  main_screen->PutImage(screen_backup, ivec2(0, 0));
  wm->flush_screen();
  delete screen_backup;
}


AWidget *net_configuration::center_ifield(AWidget *f, int x1, int x2, AWidget *place_below)
{
  int X1,Y1,X2,Y2;
  f->area(X1,Y1,X2,Y2);
  f->m_pos.x=(x1+x2)/2-(X2-X1)/2;

  if (place_below)
  {
    place_below->area(X1,Y1,X2,Y2);
    f->m_pos.y=Y2+2;
  }
  return f;
}

int net_configuration::get_options(int server)
{
  AImage *ns=cache.img(cache.reg("art/frame.spe","net_screen",SPEC_IMAGE,1));
  int ns_w=ns->Size().x,ns_h=ns->Size().y;
  int x=(xres+1)/2-ns_w/2,y=(yres+1)/2-ns_h/2;
  main_screen->PutImage(ns, ivec2(x, y));
  JCFont *fnt=wm->font();
  AImage *ok_image=cache.img(cache.reg("art/frame.spe","dev_ok",SPEC_IMAGE,1))->copy(),
    *cancel_image=cache.img(cache.reg("art/frame.spe","cancel",SPEC_IMAGE,1))->copy();

  AWidgetList list;

  if (server)
  {
    list << center_ifield(new ATextField(ivec2(x, y + 30), NET_NAME, symbol_str("your_name"), "************************", name), x, x + ns_w, nullptr);
    list << center_ifield(new ATextField(ivec2(0, 0), NET_SERVER_NAME, symbol_str("server_name"), "************************", game_name), x, x + ns_w, list.Last());
    list << center_ifield(new AInfoField(ivec2(0, 0), 0, symbol_str("min_play")), x, x + ns_w, list.Last());

    AButtonBox *b = new AButtonBox(ivec2(0, 0), NET_MIN, 1);
    b->add_button(new AButton(ivec2(0, 0), MIN_8, "8"));
    b->add_button(new AButton(ivec2(0, 0), MIN_7, "7"));
    b->add_button(new AButton(ivec2(0, 0), MIN_6, "6"));
    b->add_button(new AButton(ivec2(0, 0), MIN_5, "5"));
    b->add_button(new AButton(ivec2(0, 0), MIN_4, "4"));
    b->add_button(new AButton(ivec2(0, 0), MIN_3, "3"));
    AButton *r = new AButton(ivec2(0, 0), MIN_2, "2");
    r->push();
    b->add_button(r);
    b->add_button(new AButton(ivec2(0, 0), MIN_1,"1"));
    b->arrange_left_right();
    center_ifield(b, x, x + ns_w, list.Last());
    b->arrange_left_right();
    list << b;

    list << center_ifield(new AInfoField(ivec2(0, 0), 0, symbol_str("max_play")), x, x + ns_w, list.Last());

    b = new AButtonBox(ivec2(0, 0), NET_MAX, 1);
    AButton *q = new AButton(ivec2(0, 0), MAX_8, "8");
    q->push();
    b->add_button(q);
    b->add_button(new AButton(ivec2(0, 0), MAX_7, "7"));
    b->add_button(new AButton(ivec2(0, 0), MAX_6, "6"));
    b->add_button(new AButton(ivec2(0, 0), MAX_5, "5"));
    b->add_button(new AButton(ivec2(0, 0), MAX_4, "4"));
    b->add_button(new AButton(ivec2(0, 0), MAX_3, "3"));
    b->add_button(new AButton(ivec2(0, 0), MAX_2, "2"));
    b->arrange_left_right();
    center_ifield(b, x, x + ns_w, list.Last());
    b->arrange_left_right();
    list << b;

    list << center_ifield(new AInfoField(ivec2(0, 0), 0, symbol_str("level_size")), x, x + ns_w, list.Last());

    b = new AButtonBox(ivec2(0, 0), LEVEL_BOX, 1);
    b->add_button(new AButton(ivec2(0, 0), LVL_8, symbol_str("lvl_8")));
    b->add_button(new AButton(ivec2(0, 0), LVL_4, symbol_str("lvl_4")));
    q = new AButton(ivec2(0, 0), LVL_2, symbol_str("lvl_2"));
    q->push();
    b->add_button(q);
    b->arrange_left_right();
    center_ifield(b, x, x + ns_w, list.Last());
    b->arrange_left_right();
    list << b;

    list << center_ifield(new ATextField(ivec2(0, 0), NET_KILLS, symbol_str("kills_to_win"), "***", "25"), x, x + ns_w, list.Last());
  }
  else
  {
    list << center_ifield(new ATextField(ivec2(x, y + 80), NET_NAME, symbol_str("your_name"), "************************", name), x, x + ns_w, nullptr);
  }

  list << new AButton(ivec2(x + 80 - 17, y + ns_h - 20 - fnt->Size().y), NET_OK, ok_image);
  list << new AButton(ivec2(x + 80 + 17, y + ns_h - 20 - fnt->Size().y), NET_CANCEL, cancel_image);

  int ret = 0;

  {
    InputManager inm(main_screen, list);
    inm.allow_no_selections();
    inm.clear_current();

    int done=0;
    Event ev;
    do
    {
      wm->flush_screen();
      do { wm->get_event(ev); } while (ev.type==EV_MOUSE_MOVE && wm->IsPending());
      inm.handle_event(ev,NULL);
      if (ev.type==EV_MESSAGE)
      {
    switch (ev.message.id)
    {
      case NET_OK : { if (confirm_inputs(&inm,server))
          { ret=1; done=1; }
          else { ((AButton *)inm.get(NET_OK))->push(); inm.redraw(); }
          } break;
      case NET_CANCEL : done=1;
    }
      } if (ev.type==EV_KEY && ev.key==JK_ESC) done=1;

    } while (!done);

  }
  delete ok_image;
  delete cancel_image;

  return ret;
}

int net_configuration::input()   // pulls up dialog box and input fileds
{
  int ret=0;
  main_screen->clear();

  AImage *ns=cache.img(cache.reg("art/frame.spe","net_screen",SPEC_IMAGE,1));
  int ns_w=ns->Size().x,ns_h=ns->Size().y;
  int x=(xres+1)/2-ns_w/2,y=(yres+1)/2-ns_h/2;
  main_screen->PutImage(ns, ivec2(x, y));
  char const *nw_s = symbol_str("Networking");
  JCFont *fnt=wm->font();


  wm->font()->PutString(main_screen, ivec2(x + ns_w / 2 - strlen(nw_s) * fnt->Size().x / 2, y + 21 / 2 - fnt->Size().y / 2),
      nw_s,wm->medium_color());
  {
    AWidgetList sb;
    char const *server_str = symbol_str("server");
    sb << new AButton(ivec2(x + 40, y + ns_h - 23 - fnt->Size().y), NET_SERVER, server_str);

    if (main_net_cfg && (main_net_cfg->state==CLIENT || main_net_cfg->state==SERVER))
      sb << new AButton(ivec2(x + 40, y + ns_h - 9 - fnt->Size().y), NET_SINGLE, symbol_str("single_play"));

    InputManager inm(main_screen, sb);

    inm.allow_no_selections();
    inm.clear_current();


    Event ev;
    int done=0;
    int button_y=25,total_games=0;
    enum { MAX_GAMES=9 };
    net_address *game_addr[MAX_GAMES+1];
    int join_game=-1;

    Timer t;

    do
    {
      if (wm->IsPending())
      {
        do  { wm->get_event(ev); } while (ev.type==EV_MOUSE_MOVE && wm->IsPending());
        inm.handle_event(ev,NULL);
        if (ev.type==EV_MESSAGE)
        {
          switch (ev.message.id)
          {
            case NET_CANCEL : done=1; break;
            case NET_SERVER : done=1; break;
            case NET_SINGLE : done=1; break;
            default :
              if (ev.message.id>=NET_GAME && ev.message.id<NET_GAME+MAX_GAMES)
              {
                join_game=ev.message.id-NET_GAME;
                done=1;
              }
          }
        }
        else if (ev.type==EV_KEY && ev.key==JK_ESC )
        {
            done=1;
        }
        else
        {
            // No event waiting...  We can't wait for long, because we are
            // pretending to broadcast.
            // ECS - Added so waiting in dialog doesn't use 100% of CPU
            Timer tmp; tmp.Wait(0.005f);
        }
    }

      wm->flush_screen();
      char name[256];

      if (total_games<MAX_GAMES && t.Poll() > 0.5)
      {
        t.Get();
        net_address *find=prot->find_address(0x9090,name);    // was server_port
        if (find)
        {
          int bw=strlen(name)*fnt->Size().x;
          AWidgetList widget;
          widget << new AButton(ivec2(x + ns_w / 2 - bw / 2, y + button_y), NET_GAME + total_games, name);
          inm.Add(widget);
          find->set_port(server_port);
          game_addr[total_games] = find;

          total_games++;
          button_y += fnt->Size().y + 10;
          inm.redraw();
        }
      }

    } while (!done);

    prot->reset_find_list();

    if (join_game>=0)
    {
      if (get_options(0))
      {
        int still_there=1;  // change this back to 0, to check if games are stil alive
        Timer t2;
        do
        {
          char name[256];
          net_address *find=prot->find_address(0x9090,name);  // was server_port
          if (find)
          {
            if (find->equal(game_addr[join_game]))
              still_there=1;
            delete find;
          }

        } while (t.Poll() < 3.0 && !still_there);

        if (still_there)
        {
          game_addr[join_game]->store_string(server_name,sizeof(server_name));
          state=RESTART_CLIENT;
          ret=1;

        } else error(symbol_str("not_there"));


        prot->reset_find_list();
        int i;
        for (i=0; i<total_games; i++)        // delete all the addresses we found and stored
          delete game_addr[join_game];
      }
    } else if (ev.type==EV_MESSAGE && ev.message.id==NET_SERVER)
    {
      if (get_options(1))
      {
        state=RESTART_SERVER;
        return 1;
      }
      else return 0;
    } else if (ev.type==EV_MESSAGE && ev.message.id==NET_SINGLE)
    {
      state=RESTART_SINGLE;
      start_running=0;

      strcpy(lsf,"abuse.lsp");
      return 1;
    }
  }

  return ret;
}

