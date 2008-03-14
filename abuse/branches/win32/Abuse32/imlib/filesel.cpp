#include "filesel.hpp"
#include "input.hpp"
#include "scroller.hpp"
#include "jdir.hpp"

#ifdef __WATCOMC__
#include <direct.h>
#endif

class file_picker : public spicker
{
    char **f,**d;
    int tf,td,wid,sid;
    char *cd;
public:
    file_picker(int X, int Y, int ID, int Rows, ifield *Next);
    virtual int total() { return tf+td; }
    virtual int item_width() { return wm->font()->width()*wid; }
    virtual int item_height() { return wm->font()->height()+1; }
    virtual void draw_item(image *screen, int x, int y, int num, int active);
    virtual void note_selection(image *screen, input_manager *inm, int x);
    void free_up();
    ~file_picker() { free_up(); }
} ; 

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void file_picker::free_up()
{
    int i=0;
    for (;i<tf;i++)
        jfree(f[i]);
    for (i=0;i<td;i++)
        jfree(d[i]);
    if (tf)
        jfree(f);
    if (td)
        jfree(d);
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void file_picker::note_selection(image *screen, input_manager *inm, int x)
{
    if (x<td) {
        if (strcmp(d[x], ".")) {
            int x1,y1,x2,y2;
            area(x1,y1,x2,y2);
            screen->bar(x1,y1,x2,y2,wm->medium_color());
            
            if (strcmp(d[x], "..")) {
                cd = (char*) jrealloc(cd, strlen(cd) + strlen(d[x]) + 16, "file_picker::note_selection-cd");
                sprintf(cd,"%s%s\\",cd,d[x]);
            }
            else {
                char *curr = cd + strlen(cd) - 1;
                do {
                    curr--;
                } while (curr[0] != '\\' && curr != cd);
                if (curr != cd)
                    curr[1] = 0;
                else
                    curr[0] = 0;
                cd = (char*) jrealloc(cd, strlen(cd) + 1, "file_picker::note_selection-cd");
            }
            free_up();
            get_directory(cd,f,tf,d,td);
            wid = 0;
            int i;
            for (i = 0; i < tf; i++)
                if (strlen(f[i]) > (unsigned) wid) 
                    wid = strlen(f[i]);
            for (i = 0; i < td; i++)
                if (strlen(d[i]) + 2 > (unsigned) wid) 
                    wid = strlen(d[i]) + 2;
            sx = 0;
            
            reconfigure();  
            draw_first(screen);
        }
    }
    else {
        char *nm = (char*) jmalloc(strlen(cd) + strlen(f[x-td]) + 16, "file_picker::note_selection::nm");
        sprintf(nm, "%s%s", cd, f[x - td]);
        text_field *link = (text_field*) inm->get(sid);
        link->change_data(nm, strlen(nm), 1, screen);
        jfree(nm);
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void file_picker::draw_item(image *screen, int x, int y, int num, int active)
{
    if (active)
        screen->bar(x,y,x+item_width()-1,y+item_height()-1,wm->black());
    
    if (num<td)
    {
        char st[100];
        sprintf(st,"<%s>",d[num]);
        wm->font()->put_string(screen,x,y,st,wm->bright_color());
    } else
        wm->font()->put_string(screen,x,y,f[num-td],wm->bright_color());
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
file_picker::file_picker(int X, int Y, int ID, int Rows, ifield *Next)
: spicker(X,Y,0,Rows,1,1,0,Next)
{
    sid = ID;
    
    cd = (char*) jmalloc(strlen("") + 1, "file_picker::file_picker-cd");
    strcpy(cd,"");
    
    get_directory(cd,f,tf,d,td);
    wid=0;
    int i;
    for (i = 0; i < tf; i++)
        if (strlen(f[i]) > (unsigned) wid) 
            wid = strlen(f[i]);
        for (i = 0; i < td; i++)
            if (strlen(d[i]) + 2 > (unsigned) wid) 
                wid = strlen(d[i]) + 2;
            reconfigure();  
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
jwindow *file_dialog(char *prompt, char *def,
                     int ok_id, char *ok_name, int cancel_id, char *cancel_name, char *FILENAME_str, int filename_id)
{
    int wl=WINDOW_FRAME_LEFT,wh=WINDOW_FRAME_TOP;
    int wh2=wh+5+wm->font()->height()+5;
    int wh3=wh2+wm->font()->height()+12;
    jwindow *j=wm->new_window(0,0,-1,-1,
                              new info_field(wl + 5, wh + 5, 0, prompt,
                              new text_field(wl, wh2, filename_id, ">", "****************************************", def,
                              new button(wl + 50, wh3, ok_id, ok_name,
                              new button(wl + 100, wh3, cancel_id, cancel_name,
                              new file_picker(wl + 15, wh3 + wm->font()->height() + 10, filename_id, 8,
                              NULL))))), FILENAME_str);
    return j;
}