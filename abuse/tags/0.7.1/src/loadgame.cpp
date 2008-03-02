/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include <string.h>

#include "specs.hpp"
#include "jwindow.hpp"
#include "id.hpp"
#include "input.hpp"
#include "fonts.hpp"
#include "lisp.hpp"
#include "dprint.hpp"
#include "cache.hpp"
#include "gui.hpp"
#include "dev.hpp"
#include "id.hpp"
#include "demo.hpp"
#include "game.hpp"

extern void *save_order;         // load from "saveordr.lsp", contains a list ordering the save games

extern JCFont *console_font;

extern window_manager *eh;

#define MAX_SAVE_GAMES 5
int last_save_game_number=0;

int save_buts[MAX_SAVE_GAMES*3];


void load_number_icons()
{
  char name[100];
  int i;
  for (i=0;i<MAX_SAVE_GAMES*3;i++)
  {
    sprintf(name,"nums%04d.pcx",i+1);
    save_buts[i]=cash.reg("art/icons.spe",name,SPEC_IMAGE,1); 
  }
}


void last_savegame_name(char *buf)
{
	printf( "last_savegame_name()\n" );
	sprintf(buf,"%ssave%04d.spe",get_save_filename_prefix(), (last_save_game_number+MAX_SAVE_GAMES-1)%MAX_SAVE_GAMES+1);
}

jwindow *create_num_window(int mx, int total_saved, image **thumb_nails)
{
  ico_button *buts[MAX_SAVE_GAMES];
  int y=WINDOW_FRAME_TOP,i;
  int ih=cash.img(save_buts[0])->height();
  int x=0;
  for (i=0;i<total_saved;i++,y+=ih)
  {
    if (thumb_nails) { while (!thumb_nails[x]) x++; }
    buts[i]=new ico_button(WINDOW_FRAME_LEFT,y,ID_LOAD_GAME_NUMBER+x,
			   save_buts[x*3+0],save_buts[x*3+0],save_buts[x*3+1],save_buts[x*3+2],NULL);
    buts[i]->set_act_id(ID_LOAD_GAME_PREVIEW+x);
    x++;
  }

  for (i=0;i<total_saved-1;i++)
    buts[i]->next=buts[i+1];

  return eh->new_window(mx,yres/2-(WINDOW_FRAME_TOP+ih*5)/2,-1,-1,buts[0]);
}

int get_save_spot()
{
  int i=MAX_SAVE_GAMES,last_free=0;
  for (;i>0;)
  {
    char name[20];
    sprintf(name,"%ssave%04d.spe", get_save_filename_prefix(),i);
    FILE *fp=open_FILE(name,"rb");  
    if (fp)
      i=0;
    else { last_free=i; i--; }
    fclose(fp);
  }

  if (last_free) return last_free;    // if there are any slots not created yet...
        
  int w=cash.img(save_buts[0])->width();
  int mx=last_demo_mx-w/2;
  if((unsigned)(mx + w + 10) > xres) mx = xres - w - 10;
  if( mx < 0) mx = 0;

  jwindow *l_win=create_num_window(mx,MAX_SAVE_GAMES,NULL);
  event ev;
  int got_level=0;
  int quit=0;
  do
  {
    eh->flush_screen();
    eh->get_event(ev);
    if (ev.type==EV_MESSAGE && ev.message.id>=ID_LOAD_GAME_NUMBER && ev.message.id<ID_LOAD_GAME_PREVIEW)
      got_level=ev.message.id-ID_LOAD_GAME_NUMBER+1;


    if (ev.type==EV_CLOSE_WINDOW && ev.window==l_win)
      quit=1;
  } while (!got_level && !quit);

  eh->close_window(l_win);
  the_game->reset_keymap();
  return got_level;
}

void get_savegame_name(char *buf)  // buf should be at least 50 bytes
{
	sprintf(buf,"save%04d.spe",(last_save_game_number++)%MAX_SAVE_GAMES+1);
/*  FILE *fp=open_FILE("lastsave.lsp","wb");
  if (fp)
  {
    fprintf(fp,"(setq last_save_game %d)\n",last_save_game_number%MAX_SAVE_GAMES);
    fclose(fp);
  } else dprintf("Warning unable to open lastsave.lsp for writing\n"); */
}

int show_load_icon() 
{
	int i;
	for( i = 0; i < MAX_SAVE_GAMES; i++ )
	{
		char nm[255];
		sprintf( nm, "%ssave%04d.spe", get_save_filename_prefix(), i + 1 );
		bFILE *fp = open_file( nm, "rb" );
		if( fp->open_failure() )
		{
			delete fp;
		}
		else
		{
			delete fp;
			return 1;
		}
	}
	return 0;
}

int load_game(int show_all, char const *title)   // return 0 if the player escapes, else return the number of the game to load
{
	int total_saved=0;
	image *thumb_nails[MAX_SAVE_GAMES];
	int start_num=0;
	int max_w=160,max_h=100;
	memset(thumb_nails,0,sizeof(thumb_nails));

	image *first=NULL;

	for (start_num=0;start_num<MAX_SAVE_GAMES;start_num++)
	{
		char name[255];
		int fail=0;

		sprintf(name,"%ssave%04d.spe", get_save_filename_prefix(), start_num+1);
		bFILE *fp=open_file(name,"rb");
		if (fp->open_failure())
		{
			fail=1;
		}
		else
		{
			spec_directory sd(fp);
			spec_entry *se=sd.find("thumb nail");
			if (se && se->type==SPEC_IMAGE)
			{
				thumb_nails[start_num]=new image(se,fp);
				if (thumb_nails[start_num]->width()>max_w) max_w=thumb_nails[start_num]->width();
				if (thumb_nails[start_num]->height()>max_h) max_h=thumb_nails[start_num]->height();
				if (!first) first=thumb_nails[start_num];
				total_saved++;
			}
			else
				fail=1;
		}
		if (fail && show_all)
		{
			thumb_nails[start_num]=new image(160,100);	
			thumb_nails[start_num]->clear();
			console_font->put_string(thumb_nails[start_num],0,0,symbol_str("no_saved"));
			total_saved++;
			if (!first) first=thumb_nails[start_num];
		}
		delete fp;
	}

	if (!total_saved) return 0; 
	if (total_saved>MAX_SAVE_GAMES)
		total_saved=MAX_SAVE_GAMES;

	int i;
/*  int ih=cash.img(save_buts[0])->height();
  ico_button *buts[MAX_SAVE_GAMES];
  int y=WINDOW_FRAME_TOP;


  for (i=0;i<total_saved;i++,y+=ih)
  {
    buts[i]=new ico_button(WINDOW_FRAME_LEFT,y,ID_LOAD_GAME_NUMBER+i,		       
			   save_buts[i*3+1],save_buts[i*3+1],save_buts[i*3+0],save_buts[i*3+2],NULL);
    buts[i]->set_act_id(ID_LOAD_GAME_PREVIEW+i);
  }

  for (i=0;i<total_saved-1;i++)
    buts[i]->next=buts[i+1];
*/


	jwindow *l_win=create_num_window(0,total_saved,thumb_nails);
	jwindow *preview=eh->new_window(l_win->x+l_win->l+5,l_win->y,max_w,max_h,NULL,title);

	first->put_image(preview->screen,preview->x1(),preview->y1());

	event ev;
	int got_level=0;
	int quit=0;
	do
	{
		eh->flush_screen();
		eh->get_event(ev);
		if (ev.type==EV_MESSAGE && ev.message.id>=ID_LOAD_GAME_NUMBER && ev.message.id<ID_LOAD_GAME_PREVIEW)
			got_level=ev.message.id-ID_LOAD_GAME_NUMBER+1;

		if (ev.type==EV_MESSAGE && ev.message.id>=ID_LOAD_GAME_PREVIEW && ev.message.id<ID_LOAD_PLAYER_GAME)
		{
			int draw_num=ev.message.id-ID_LOAD_GAME_PREVIEW;
			preview->clear();
			thumb_nails[draw_num]->put_image(preview->screen,preview->x1(),preview->y1());
		}

		if ((ev.type==EV_CLOSE_WINDOW) || (ev.type==EV_KEY && ev.key==JK_ESC))
			quit=1;
	} while (!got_level && !quit);

	eh->close_window(l_win);
	eh->close_window(preview);

	for (i=0;i<total_saved;i++)
		if (thumb_nails[i])
			delete thumb_nails[i];

	return got_level;
}
