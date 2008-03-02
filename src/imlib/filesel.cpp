/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#ifdef __WATCOMC__
#   include <direct.h>
#endif

#include "filesel.hpp"
#include "input.hpp"
#include "scroller.hpp"
#include "jdir.hpp"

class file_picker : public spicker
{
  char **f,**d;
  int tf,td,wid,sid;
  char cd[300];
  public:
  file_picker(int X, int Y, int ID, int Rows, ifield *Next);
  virtual int total() { return tf+td; }
  virtual int item_width(window_manager *wm) { return wm->font()->width()*wid; }
  virtual int item_height(window_manager *wm) { return wm->font()->height()+1; }
  virtual void draw_item(window_manager *wm, image *screen, int x, int y, int num, int active);
  virtual void note_selection(window_manager *wm, image *screen, input_manager *inm, int x);
  void free_up();
  ~file_picker() { free_up(); }
} ; 

void file_picker::free_up()
{
  int i=0;
  for (;i<tf;i++)
    jfree(f[i]);
  for (i=0;i<td;i++)
    jfree(d[i]);
  if (tf) jfree(f);
  if (td) jfree(d);
}

void file_picker::note_selection(window_manager *wm, image *screen, input_manager *inm, int x)
{
  if (x<td)
  {
    if (strcmp(d[x],"."))
    {
      int x1,y1,x2,y2;
      area(x1,y1,x2,y2,wm);
      screen->bar(x1,y1,x2,y2,wm->medium_color());

      char st[200],curdir[200];
      sprintf(st,"%s/%s",cd,d[x]);
      getcwd(curdir,200);
      chdir(st);
      getcwd(cd,200);
      chdir(curdir);

      free_up();
      get_directory(cd,f,tf,d,td);
      wid=0;
      int i=0;
      for (;i<tf;i++)
      if ((int)strlen(f[i])>wid) wid=strlen(f[i]);
      for (i=0;i<td;i++)
      if ((int)strlen(d[i])+2>wid) wid=strlen(d[i])+2;
      sx=0;

      

      reconfigure();  
      draw_first(screen,wm);
    }
  } else
  {
    char nm[200];
    sprintf(nm,"%s/%s",cd,f[x-td]);
    text_field *link=(text_field *)inm->get(sid);
    link->change_data(nm,strlen(nm),1,screen,wm);
  }

}

void file_picker::draw_item(window_manager *wm, image *screen, int x, int y, int num, int active)
{
  if (active)
    screen->bar(x,y,x+item_width(wm)-1,y+item_height(wm)-1,wm->black());

  if (num<td)
  {
    char st[100];
    sprintf(st,"<%s>",d[num]);
    wm->font()->put_string(screen,x,y,st,wm->bright_color());
  } else
    wm->font()->put_string(screen,x,y,f[num-td],wm->bright_color());
}

file_picker::file_picker(int X, int Y, int ID, int Rows, ifield *Next)
  : spicker(X,Y,0,Rows,1,1,0,Next)
{
  
  sid=ID;

  strcpy(cd,".");
 
  get_directory(cd,f,tf,d,td);
  wid=0;
  int i=0;
  for (;i<tf;i++)
    if ((int)strlen(f[i])>wid) wid=strlen(f[i]);
  for (i=0;i<td;i++)
    if ((int)strlen(d[i])+2>wid) wid=strlen(d[i])+2;
  reconfigure();  
}

jwindow *file_dialog(window_manager *wm, char const *prompt, char const *def,
		     int ok_id, char const *ok_name, int cancel_id,
                     char const *cancel_name, char const *FILENAME_str,
                     int filename_id)
{
  int wl=WINDOW_FRAME_LEFT,wh=WINDOW_FRAME_TOP;
  int wh2=wh+5+wm->font()->height()+5;
  int wh3=wh2+wm->font()->height()+12;
  jwindow *j=wm->new_window(0,0,-1,-1,
			    new info_field(wl+5,wh+5,0,prompt,
                            new text_field(wl,wh2,filename_id,
					   ">","****************************************",def,
			    new button(wl+50,wh3,ok_id,     ok_name,
			    new button(wl+100,wh3,cancel_id,cancel_name,
			    new file_picker(wl+15,wh3+wm->font()->height()+10,filename_id,8,
					  NULL))))),

			    FILENAME_str);
  return j;
}





