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

#include <unistd.h>

#include "common.h"

#include "filesel.h"
#include "input.h"
#include "scroller.h"
#include "jdir.h"

class AFilePicker : public AScrollPicker
{
public:
    AFilePicker(ivec2 pos, int id, int rows);
    ~AFilePicker() { free_up(); }

    virtual int total() { return tf+td; }
    virtual int item_width() { return wm->font()->Size().x * wid; }
    virtual int item_height() { return wm->font()->Size().y + 1; }
    virtual void DrawItem(AImage *screen, ivec2 pos, int num, int active);
    virtual void note_selection(AImage *screen, InputManager *inm, int x);
    void free_up();

private:
    char **f,**d;
    int tf,td,wid,sid;
    char cd[300];
};

void AFilePicker::free_up()
{
  int i=0;
  for (; i<tf; i++)
    free(f[i]);
  for (i=0; i<td; i++)
    free(d[i]);
  if (tf) free(f);
  if (td) free(d);
}

void AFilePicker::note_selection(AImage *screen, InputManager *inm, int x)
{
  if (x<td)
  {
#if !defined __CELLOS_LV2__
    if (strcmp(d[x],"."))
    {
      ibox2 area = GetArea();
      screen->Bar(area.A, area.B, wm->medium_color());

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
      for (; i<tf; i++)
        if ((int)strlen(f[i])>wid)
          wid=strlen(f[i]);
      for (i=0; i<td; i++)
        if ((int)strlen(d[i])+2>wid)
          wid=strlen(d[i])+2;
      sx=0;

      reconfigure();
      draw_first(screen);
    }
#endif
  } else
  {
    char nm[200];
    sprintf(nm,"%s/%s",cd,f[x-td]);
    ATextField *link=(ATextField *)inm->get(sid);
    link->change_data(nm,strlen(nm),1,screen);
  }

}

void AFilePicker::DrawItem(AImage *screen, ivec2 pos, int num, int active)
{
    if (active)
        screen->Bar(pos,
                    pos + ivec2(item_width() - 1, item_height() - 1),
                    wm->black());

    char st[100], *dest;
    if (num >= td)
        dest = f[num - td];
    else
        sprintf(dest = st, "<%s>", d[num]);

    wm->font()->PutString(screen, pos, dest, wm->bright_color());
}

AFilePicker::AFilePicker(ivec2 pos, int id, int Rows)
  : AScrollPicker(pos, 0, Rows, 1, 1, 0)
{
    sid = id;
    strcpy(cd, ".");

    get_directory(cd,f,tf,d,td);
    wid=0;
    int i=0;
    for (; i<tf; i++)
        if ((int)strlen(f[i])>wid) wid=strlen(f[i]);
    for (i=0; i<td; i++)
        if ((int)strlen(d[i])+2>wid) wid=strlen(d[i])+2;
    reconfigure();
}

AWindow *file_dialog(char const *prompt, char const *def,
                     int ok_id, char const *ok_name, int cancel_id,
                     char const *cancel_name, char const *FILENAME_str,
                     int filename_id)
{
    int wh2 = 5 + wm->font()->Size().y + 5;
    int wh3 = wh2 + wm->font()->Size().y + 12;

    AWidgetList widgets;
    widgets << new AInfoField(ivec2(5, 5), 0, prompt);
    widgets << new ATextField(ivec2(0, wh2), filename_id, ">", "****************************************", def);
    widgets << new AButton(ivec2(50, wh3), ok_id, ok_name);
    widgets << new AButton(ivec2(100, wh3), cancel_id, cancel_name);
    widgets << new AFilePicker(ivec2(15, wh3 + wm->font()->Size().y + 10), filename_id, 8);
    return wm->CreateWindow(ivec2(0), ivec2(-1), FILENAME_str, widgets);
}





