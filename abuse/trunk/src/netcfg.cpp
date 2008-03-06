/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "game.hpp"


#include "netcfg.hpp"
#include "input.hpp"
#include "cache.hpp"
#include "sock.hpp"
#include "timing.hpp"
#include "light.hpp"

#include "dev.hpp"

extern char *get_login();
net_configuration *main_net_cfg=NULL;
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
  jwindow *j=eh->new_window(-1,0,-1,-1,new info_field(WINDOW_FRAME_LEFT,WINDOW_FRAME_TOP,0,msg,
      new button(WINDOW_FRAME_LEFT,WINDOW_FRAME_TOP+30,CFG_ERR_OK,symbol_str("ok_button"),NULL)),symbol_str("input_error"));
  event ev;
  do
  {
    eh->flush_screen();
    do { eh->get_event(ev); } while (ev.type==EV_MOUSE_MOVE && eh->event_waiting()); 
  } while (ev.type!=EV_MESSAGE || ev.message.id!=CFG_ERR_OK || ev.type==EV_CLOSE_WINDOW || (ev.type==EV_KEY && ev.key==JK_ESC));
  eh->close_window(j);
  eh->flush_screen();
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


int net_configuration::confirm_inputs(input_manager *i, int server)
{
  if (server)
  {
    int kl;

    if (sscanf(i->get(NET_KILLS)->read(),"%d",&kl)!=1 || kl<1 || kl>99)  {  error(symbol_str("kill_error")); return 0; }

    char *nm=i->get(NET_NAME)->read();
    if (strstr(nm,"\"")) {  error(symbol_str("name_error")); return 0; }    
    strcpy(name,nm);

    min_players=((ifield *)(i->get(NET_MIN)->read()))->id-MIN_1+1;
    max_players=((ifield *)(i->get(NET_MAX)->read()))->id-MAX_2+2;
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
      if (((ifield *)(i->get(LEVEL_BOX)->read()))->id==LVL_2)
        sprintf(str,"(load \"addon/deathmat/small.lsp\")\n"); 
      else if (((ifield *)(i->get(LEVEL_BOX)->read()))->id==LVL_4)
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
  ifield *ilist=NULL;
  int x=WINDOW_FRAME_LEFT,y=WINDOW_FRAME_TOP;
  
  jwindow *sv=eh->new_window(50,80,-1,-1,new button(WINDOW_FRAME_LEFT,WINDOW_FRAME_TOP,NET_SERVER,symbol_str("server"),
				     new button(WINDOW_FRAME_LEFT,WINDOW_FRAME_TOP+eh->font()->height()*2,NET_CLIENT,symbol_str("client"),
				     new button(WINDOW_FRAME_LEFT,WINDOW_FRAME_TOP+eh->font()->height()*4,NET_SINGLE,symbol_str("single_play"),
				     new button(WINDOW_FRAME_LEFT,WINDOW_FRAME_TOP+eh->font()->height()*6,NET_CANCEL,symbol_str("cancel_net"),
						NULL)))),symbol_str("Networking"));

  event ev;
  int done=0;
  do
  {
    eh->flush_screen();
    do { eh->get_event(ev); } while (ev.type==EV_MOUSE_MOVE && eh->event_waiting()); 
    if (ev.type==EV_MESSAGE)
    {
      if (ev.message.id==NET_SERVER) { done=1; state=RESTART_SERVER;  start_edit=0; demo_start=0; start_running=1; }
      else if (ev.message.id==NET_CLIENT) { done=1; state=RESTART_CLIENT;  start_edit=0; demo_start=0; start_running=1; }
      else if (ev.message.id==NET_SINGLE) { done=1; state=RESTART_SINGLE;  start_edit=0; demo_start=0; start_running=0; }
      else if (ev.message.id==NET_CANCEL) { done=1; }
    } else if (ev.type==EV_CLOSE_WINDOW || (ev.type==EV_KEY & ev.key==JK_ESC)) done=1;
  
  } while (!done);

  eh->close_window(sv);
  eh->flush_screen();
	   
  if (state==RESTART_SINGLE) 
  {
    strcpy(lsf,"abuse.lsp");
    return 1;
  }
  if (ev.message.id==NET_CANCEL || state==RESTART_SINGLE) return 0;

  if (state==RESTART_SERVER)
  {
    ilist=new button(x,y,NET_CANCEL,symbol_str("cancel_button"),ilist);
    ilist=new button(x,y,NET_OK,       symbol_str("server"),ilist);
    ilist=new text_field(x,y,NET_KILLS,symbol_str("kills_to_win"),"******",kills,ilist);
    ilist=new text_field(x,y,NET_MAX,symbol_str("max_play"),"******",max_players,ilist);
    ilist=new text_field(x,y,NET_MIN,symbol_str("min_play"),"******",min_players,ilist);
    ilist=new text_field(x,y,NET_PORT,symbol_str("use_port"),"******",port,ilist);
    ilist=new text_field(x,y,NET_NAME,symbol_str("your_name"),"****************",name,ilist);

  } else
  {
    ilist=new button(x,y,NET_CANCEL,symbol_str("cancel_button"),ilist);
    ilist=new button(x,y,NET_OK,symbol_str("client"),ilist);
//    ilist=new text_field(x,y,NET_PORT,symbol_str("use_port"),"******",port,ilist);
    ilist=new text_field(x,y,NET_SERVER_PORT,symbol_str("server_port"),"******",server_port,ilist);
    ilist=new text_field(x,y,NET_SERVER_NAME,symbol_str("server_name"),"*********************************",game_name,ilist);
    ilist=new text_field(x,y,NET_NAME,symbol_str("your_name"),"****************",name,ilist);
  }

  ifield *i=ilist;
  for (;i;i=i->next)
  {
    i->y=y;
    int x1,y1,x2,y2;
    i->area(x1,y1,x2,y2,eh);
    y=y2+2;
  }


  jwindow *nw=eh->new_window(0,0,-1,-1,ilist,symbol_str("Networking"));

  done=0;
  do
  {
    eh->flush_screen();
    do { eh->get_event(ev); } while (ev.type==EV_MOUSE_MOVE && eh->event_waiting()); 
    if (ev.type==EV_MESSAGE && ev.message.id==NET_OK && confirm_inputs(nw,state==RESTART_SERVER))
      done=1;
    if (ev.type==EV_MESSAGE && (ev.message.id==NET_CANCEL || ev.message.id==NET_SINGLE))
       done=1;
    if (ev.type==EV_CLOSE_WINDOW || (ev.type==EV_KEY && ev.key==JK_ESC))
      done=1;

  } while (!done);
  eh->close_window(nw);
  eh->flush_screen();

  return ev.message.id==NET_OK;
}

*/


void net_configuration::error(char const *message)
{
  image *screen_backup=screen->copy();

  image *ns=cash.img(cash.reg("art/frame.spe","net_screen",SPEC_IMAGE,1));
  int ns_w=ns->width(),ns_h=ns->height();
  int x=(xres+1)/2-ns_w/2,y=(yres+1)/2-ns_h/2;
  ns->put_image(screen,x,y);
  JCFont *fnt=eh->font();

  uint8_t *remap=white_light+30*256;

  uint8_t *sl=screen->scan_line(0);
  int xx=screen->width()*screen->height();
  for (;xx;xx--,sl++) *sl=remap[*sl];

  int fx=x+ns_w/2-strlen(message)*fnt->width()/2,
    fy=y+ns_h/2-fnt->height();

  fnt->put_string(screen,fx+1,fy+1,message,eh->black());
  fnt->put_string(screen,fx,fy,message,eh->bright_color());

  
  {   
    char const *ok = symbol_str("ok_button");

    int bx=x+ns_w/2-strlen(ok)*fnt->width()/2-3,
      by=y+ns_h/2+fnt->height()*3;
    
    button *sb=new button(bx,by,NET_SERVER,ok,NULL);

    input_manager inm(screen,eh,sb);
    inm.allow_no_selections();
    inm.clear_current();

    int done=0;
    event ev;
    do
    {
      eh->flush_screen();
      do  { eh->get_event(ev); } while (ev.type==EV_MOUSE_MOVE && eh->event_waiting()); 
      inm.handle_event(ev,NULL,eh);
      if ((ev.type==EV_KEY && (ev.key==JK_ESC || ev.key==JK_ENTER)) ||
	  ev.type==EV_MESSAGE) done=1;
    } while (!done);
  }

  screen_backup->put_image(screen,0,0);
  eh->flush_screen();
  delete screen_backup;
}


ifield *net_configuration::center_ifield(ifield *i,int x1, int x2, ifield *place_below)
{
  int X1,Y1,X2,Y2;
  i->area(X1,Y1,X2,Y2,eh);
  i->x=(x1+x2)/2-(X2-X1)/2;

  if (place_below)
  {
    place_below->area(X1,Y1,X2,Y2,eh);
    i->y=Y2+2;
  }
  return i;
}

int net_configuration::get_options(int server)
{
  image *ns=cash.img(cash.reg("art/frame.spe","net_screen",SPEC_IMAGE,1));
  int ns_w=ns->width(),ns_h=ns->height();
  int x=(xres+1)/2-ns_w/2,y=(yres+1)/2-ns_h/2;
  ns->put_image(screen,x,y);
  JCFont *fnt=eh->font();
  image *ok_image=cash.img(cash.reg("art/frame.spe","dev_ok",SPEC_IMAGE,1))->copy(),
    *cancel_image=cash.img(cash.reg("art/frame.spe","cancel",SPEC_IMAGE,1))->copy();

  ifield *list=NULL;

  if (server)
  {       
    list=center_ifield(new text_field(x,y+30,NET_NAME,symbol_str("your_name"),"************************",name,list),x,x+ns_w,NULL);
    list=center_ifield(new text_field(0,0,NET_SERVER_NAME,symbol_str("server_name"),"************************",game_name,list),x,x+ns_w,list);
    list=center_ifield(new info_field(0,0,0,symbol_str("min_play"),list),x,x+ns_w,list);


    button_box *b=new button_box(0,0,NET_MIN,1,NULL,list);
    b->add_button(new button(0,0,MIN_8,"8",NULL));
    b->add_button(new button(0,0,MIN_7,"7",NULL));
    b->add_button(new button(0,0,MIN_6,"6",NULL));
    b->add_button(new button(0,0,MIN_5,"5",NULL));
    b->add_button(new button(0,0,MIN_4,"4",NULL));
    b->add_button(new button(0,0,MIN_3,"3",NULL));
    button *r=new button(0,0,MIN_2,"2",NULL); r->push();
    b->add_button(r);
    b->add_button(new button(0,0,MIN_1,"1",NULL));



    b->arrange_left_right(eh);
    center_ifield(b,x,x+ns_w,list);
    b->arrange_left_right(eh);
    list=b;

    list=center_ifield(new info_field(0,0,0,symbol_str("max_play"),list),x,x+ns_w,list);

    b=new button_box(0,0,NET_MAX,1,NULL,list);
    button *q=new button(0,0,MAX_8,"8",NULL); q->push();
    b->add_button(q);
    b->add_button(new button(0,0,MAX_7,"7",NULL));
    b->add_button(new button(0,0,MAX_6,"6",NULL));
    b->add_button(new button(0,0,MAX_5,"5",NULL));
    b->add_button(new button(0,0,MAX_4,"4",NULL));
    b->add_button(new button(0,0,MAX_3,"3",NULL));
    b->add_button(new button(0,0,MAX_2,"2",NULL));
    b->arrange_left_right(eh);
    center_ifield(b,x,x+ns_w,list);
    b->arrange_left_right(eh);    
    list=b;


    list=center_ifield(new info_field(0,0,0,symbol_str("level_size"),list),x,x+ns_w,list);

    b=new button_box(0,0,LEVEL_BOX,1,NULL,list);
    b->add_button(new button(0,0,LVL_8,symbol_str("lvl_8"),NULL));
    b->add_button(new button(0,0,LVL_4,symbol_str("lvl_4"),NULL));
    q=new button(0,0,LVL_2,symbol_str("lvl_2"),NULL); q->push();
    b->add_button(q);

    b->arrange_left_right(eh);
    center_ifield(b,x,x+ns_w,list);
    b->arrange_left_right(eh);    
    list=b;

    list=center_ifield(new text_field(0,0,NET_KILLS,symbol_str("kills_to_win"),"***","25",list),x,x+ns_w,list);



				    
  } else
  {
    list=center_ifield(new text_field(x,y+80,NET_NAME,symbol_str("your_name"),"************************",name,list),x,x+ns_w,NULL);
  }


  list=new button(x+80-17,y+ns_h-20-fnt->height(),NET_OK,ok_image,list);
  list=new button(x+80+17,y+ns_h-20-fnt->height(),NET_CANCEL,cancel_image,list);

  int ret=0;

  {
    input_manager inm(screen,eh,list);
    inm.allow_no_selections();
    inm.clear_current();
   
    int done=0;
    event ev;
    do
    {
      eh->flush_screen();
      do { eh->get_event(ev); } while (ev.type==EV_MOUSE_MOVE && eh->event_waiting()); 
      inm.handle_event(ev,NULL,eh);
      if (ev.type==EV_MESSAGE)
      {
	switch (ev.message.id)
	{
	  case NET_OK : { if (confirm_inputs(&inm,server)) 
          { ret=1; done=1; } 			 
          else { ((button *)inm.get(NET_OK))->push(); inm.redraw(); }
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
  screen->clear();
  
  image *ns=cash.img(cash.reg("art/frame.spe","net_screen",SPEC_IMAGE,1));
  int ns_w=ns->width(),ns_h=ns->height();
  int x=(xres+1)/2-ns_w/2,y=(yres+1)/2-ns_h/2;
  ns->put_image(screen,x,y);
  char const *nw_s = symbol_str("Networking");
  JCFont *fnt=eh->font();


  eh->font()->put_string(screen,x+ns_w/2-strlen(nw_s)*fnt->width()/2,y+21/2-fnt->height()/2,
      nw_s,eh->medium_color());
  {

    char const *server_str = symbol_str("server");
    button *sb=new button(x+40,y+ns_h-23-fnt->height(),NET_SERVER,server_str,NULL);

    if (main_net_cfg && (main_net_cfg->state==CLIENT || main_net_cfg->state==SERVER))
      sb=new button(x+40,y+ns_h-9-fnt->height(),NET_SINGLE,symbol_str("single_play"),sb);

    input_manager inm(screen,eh,sb);

    inm.allow_no_selections();
    inm.clear_current();


    event ev;
    int done=0;
    int button_y=25,total_games=0;
    enum { MAX_GAMES=9 };
    net_address *game_addr[MAX_GAMES+1];
    int join_game=-1;
    time_marker start,now;

    do
    {
      if (eh->event_waiting())
      {
        do  { eh->get_event(ev); } while (ev.type==EV_MOUSE_MOVE && eh->event_waiting()); 
        inm.handle_event(ev,NULL,eh);
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
			// No event waiting...  We can't wait for long, because we are pretending to broadcast.
			milli_wait(5); // ECS - Added so waiting in dialog doesn't use 100% of CPU
		}
	}

      eh->flush_screen();
      char name[256];

      now.get_time();
      if (total_games<MAX_GAMES && now.diff_time(&start)>0.5)
      {
        start.get_time();
        net_address *find=prot->find_address(0x9090,name);    // was server_port
        if (find)
        {
          int bw=strlen(name)*fnt->width();
          inm.add(new button(x+ns_w/2-bw/2,y+button_y,NET_GAME+total_games,name,NULL));
          find->set_port(server_port);
          game_addr[total_games]=find;
			
          total_games++;
          button_y+=fnt->height()+10;
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
        time_marker start,now;
        do
        {
          now.get_time();
          char name[256];
          net_address *find=prot->find_address(0x9090,name);  // was server_port
          if (find)
          {	  
            if (find->equal(game_addr[join_game]))
              still_there=1;
            delete find;
          }
			
        } while (now.diff_time(&start)<3 && !still_there);
			
        if (still_there)
        {
          game_addr[join_game]->store_string(server_name,sizeof(server_name));
          state=RESTART_CLIENT;
          ret=1;
			
        } else error(symbol_str("not_there"));
			
			
        prot->reset_find_list();
        int i; 
        for (i=0;i<total_games;i++)        // delete all the addresses we found and stored
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


