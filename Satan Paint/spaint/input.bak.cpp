#include "jwindow.hpp"
#include "image.hpp"
#include "specs.hpp"
#include "fonts.hpp"
#include <ctype.h>

class input_manager;

class ifield
{
protected :
  int x,y;
public :
  ifield *next;
  virtual void area(int &x1, int &y1, int &x2, int &y2, window_manager *wm) = 0;
  virtual void draw_first(image *screen, window_manager *wm)              = 0;
  virtual void draw(int active, image *screen, window_manager *wm)        = 0;
  virtual void handle_event(event &ev, image *screen, window_manager *wm) = 0;
} ;

class input_manager
{
  image *screen;
  ifield *first,*active;
  window_manager *wm;
public :
  input_manager(image *Screen, window_manager *WM, ifield *First);
  void handle_event(event &ev, int xoff, int yoff);
  void redraw();
  void add(ifield *i) { i->next=first; first=i; }
  ~input_manager(); 
} ;

class button : public ifield
{
  int x,y,id,up,act;
  char *text;
public : 
  button(int X, int Y, int ID, char *Text, ifield *Next)
  { x=X; y=Y; id=ID; text=strcpy((char *)malloc(strlen(Text)+1),Text); up=1; next=Next; act=0; }
  virtual void area(int &x1, int &y1, int &x2, int &y2, window_manager *wm)
  { x1=x; y1=y; x2=x+wm->font()->width()*strlen(text)+6; y2=y+wm->font()->height()+7; }
  virtual void draw_first(image *screen, window_manager *wm);
  virtual void draw(int active, image *screen, window_manager *wm); 
  virtual void handle_event(event &ev, image *screen, window_manager *wm);
  virtual ~button() { free(text); }
 
} ;

class text_field : public ifield
{
  int x,y,id,cur;
  char *prompt,*data,*format;
  int xstart(window_manager *wm) { return x+wm->font()->width()*(strlen(prompt)+1)+3; }
  int xend(window_manager *wm) { return x+wm->font()->width()*(strlen(prompt)+1+strlen(format))+7; }
  int yend(window_manager *wm) { return y+wm->font()->height()+5; }
  void draw_cur(int color, image *screen, window_manager *wm);
  int last_spot() { int x=strlen(data); while (x && data[x-1]==' ') x--; return x; }
  void draw_text(image *screen, window_manager *wm)
  {
    screen->bar(xstart(wm)+1,y+1,xend(wm)-1,yend(wm)-1,wm->dark_color());
    wm->font()->put_string(screen,xstart(wm)+1,y+3,data);
  }
public : 
  text_field(int X, int Y, int ID, char *Prompt, char *Format, 
                               char *Data, ifield *Next);
  virtual void area(int &x1, int &y1, int &x2, int &y2, window_manager *wm);
  virtual void draw_first(image *screen, window_manager *wm);
  virtual void draw(int active, image *screen, window_manager *wm); 
  virtual void handle_event(event &ev, image *screen, window_manager *wm);

  virtual ~text_field() { free(prompt); free(format); }
 
} ;

input_manager::~input_manager() 
{ ifield *i; 
  while (first) 
  { i=first; 
    first=first->next; 
    delete i; 
  } 
} 

void text_field::handle_event(event &ev, image *screen, window_manager *wm)
{
  int xx;
  if (ev.type==EV_KEY)
  {
    switch (ev.key)
    {
      case JK_LEFT : if (cur) { draw_cur(wm->dark_color(),screen,wm); cur--;
                           draw_cur(wm->bright_color(),screen,wm); } break; 
      case JK_RIGHT : if (cur<strlen(format)-1) { draw_cur(wm->dark_color(),screen,wm); cur++;
                           draw_cur(wm->bright_color(),screen,wm); } break; 
      case JK_END : if (cur!=last_spot()) 
                          { draw_cur(wm->dark_color(),screen,wm); cur=last_spot(); 
                            if (cur==strlen(format)-1) cur--; 
                           draw_cur(wm->bright_color(),screen,wm); } break; 
      case JK_HOME : if (cur) 
                          { draw_cur(wm->dark_color(),screen,wm); cur=0;
                           draw_cur(wm->bright_color(),screen,wm); } break; 
      case JK_BACKSPACE : if (cur)
         { draw_cur(wm->dark_color(),screen,wm); cur--;
           for (xx=cur;xx<strlen(format)-1;xx++)
             data[xx]=data[xx+1];
           data[strlen(format)-1]=' ';
           draw_text(screen,wm);
           draw_cur(wm->bright_color(),screen,wm); 
         } break; 
      default : if (ev.key>=' ' && ev.key<='~')
         { 
           draw_cur(wm->dark_color(),screen,wm); 
           for (xx=strlen(format)-1;xx>cur && xx>0;xx--)
             data[xx]=data[xx-1];
           data[cur]=ev.key;
           if (cur<strlen(format)-1)
             cur++;
           draw_text(screen,wm);
           draw_cur(wm->bright_color(),screen,wm); 
         } break;
    }
  } 
}

void text_field::draw_first(image *screen, window_manager *wm)
{
  wm->font()->put_string(screen,x,y+3,prompt);
  screen->bar(xstart(wm),y,xend(wm),yend(wm),wm->dark_color());
  wm->font()->put_string(screen,xstart(wm)+1,y+3,data);
}

void text_field::draw_cur(int color, image *screen, window_manager *wm)
{
  screen->bar(xstart(wm)+cur*wm->font()->width()+1,
                      yend(wm)-2,
                      xstart(wm)+(cur+1)*wm->font()->width(),
                      yend(wm)-1,color);
}

void text_field::draw(int active, image *screen, window_manager *wm)
{
  if (active)
  {
    screen->rectangle(xstart(wm),y,xend(wm),yend(wm),wm->bright_color());
    draw_cur(wm->bright_color(),screen,wm);
  }
  else
  {
    screen->rectangle(xstart(wm),y,xend(wm),yend(wm),wm->dark_color());
    draw_cur(wm->dark_color(),screen,wm);
  }
}

void text_field::area(int &x1, int &y1, int &x2, int &y2, window_manager *wm)
{
  x1=x; y1=y; 
  x2=xend(wm);
  y2=yend(wm);
}

text_field::text_field(int X, int Y, int ID, char *Prompt, char *Format, 
                                                     char *Data, ifield *Next)
{
  x=X; y=Y; id=ID; 
  prompt=strcpy((char *)malloc(strlen(Prompt)+1),Prompt); 
  format=strcpy((char *)malloc(strlen(Format)+1),Format); 
  data=Data; 
  cur=strlen(data); 
  while (cur && data[cur-1]==' ') cur--;
  next=Next; 
}


void button::handle_event(event &ev, image *screen, window_manager *wm)
{
  if ((ev.type==EV_KEY && ev.key==13) || (ev.type==EV_MOUSE_BUTTON &&
                                         ev.mouse_button))
  {
    int x1=x,y1=y,x2=x+wm->font()->width()*strlen(text)+6,
      y2=y+wm->font()->height()+7; 
    up=!up;
    if (up) draw_first(screen,wm);
    else
    {
      screen->line(x1,y1,x2,y1,wm->dark_color());
      screen->line(x1,y1,x1,y2,wm->dark_color());
      screen->line(x2,y1+1,x2,y2,wm->bright_color());
      screen->line(x1+1,y2,x2,y2,wm->bright_color());
      screen->bar(x1+1,y1+1,x2-1,y2-1,wm->medium_color());
      wm->font()->put_string(screen,x+4,y+4,text);
    }
    draw(act,screen,wm);
    wm->push_event(new event(id,(char *)&up));
  }
}

void button::draw(int active, image *screen, window_manager *wm)
{
  if (active)
    screen->rectangle(x+3,y+3,x+wm->font()->width()*strlen(text)+4,
                        y+wm->font()->height()+4,wm->bright_color()); 
  else
    screen->rectangle(x+3,y+3,x+wm->font()->width()*strlen(text)+4,
                        y+wm->font()->height()+4,wm->medium_color()); 
  act=active;
}

void button::draw_first(image *screen, window_manager *wm)
{
  screen->wiget_bar(x,y,x+wm->font()->width()*strlen(text)+6,
                        y+wm->font()->height()+7,wm->bright_color(),
                        wm->medium_color(),wm->dark_color()); 
 screen->wiget_bar(x+1,y,x+wm->font()->width()*strlen(text)+5,
                        y+wm->font()->height()+6,wm->bright_color(),
                        wm->medium_color(),wm->dark_color()); 
  wm->font()->put_string(screen,x+4,y+4,text);
}




void input_manager::handle_event(event &ev, int xoff, int yoff)
{
  ifield *i,*in_area=NULL;
  int x1,y1,x2,y2;
  ev.mouse_move.x-=xoff;
  ev.mouse_move.y-=yoff;

  if (ev.type==EV_MOUSE_BUTTON && ev.mouse_button==1)
  {
    for (i=first;i;i=i->next)
    {
      i->area(x1,y1,x2,y2,wm);
      if (ev.mouse_move.x>=x1 && ev.mouse_move.y>=y1 &&
          ev.mouse_move.x<=x2 && ev.mouse_move.y<=y2)
        in_area=i;
    }
    if (in_area!=active && in_area)
    {
      active->draw(0,screen,wm);
      active=in_area; 
      active->draw(1,screen,wm);
    }
  } 
  if (ev.type==EV_KEY && ev.key==JK_TAB)
  { 
    active->draw(0,screen,wm);
    active=active->next;
    if (!active) active=first;
    active->draw(1,screen,wm);
  }
  if (active)
    active->handle_event(ev,screen,wm);
  
  ev.mouse_move.x+=xoff;
  ev.mouse_move.y+=yoff;
}

void input_manager::redraw()
{
  ifield *i;
  for (i=first;i;i=i->next)
    i->draw_first(screen,wm);
  active->draw(1,screen,wm);
}

input_manager::input_manager(image *Screen, window_manager *WM, ifield *First)
{
  screen=Screen;
  wm=WM;
  first=First;
  active=first;
  redraw();
}

main(int argc, char **argv)
{
  
  image_init();
  set_mode(19,argc,argv);
  event *eh;
  palette p;
  p.defaults();
  p.load();
  spec_directory spe("letters.spe");
  FILE *fp=fopen("letters.spe","rb");
  image *fontim=new image(spe.find("letters"),fp);
  fclose(fp);
  fontim->make_color(p.find_closest(255,255,255));
  JCFont fnt(fontim);
   
  window_manager *wm=new window_manager(screen,&p,
    p.find_closest(255,0,0),
    p.find_closest(128,0,0),
    p.find_closest(64,0,0),&fnt);
  event ev;
  int i; 
  for (i=0;i<5;i++)
    wm->new_window(50+i*5,50+i*5,40,40);
  jwindow *j=wm->new_window(10,10,200,80);
  j->screen->bar(j->x1(),j->y1(),j->x2(),j->y2(),wm->medium_color());
  
  char name[]="Jonathan          ";
  input_manager *inm=new input_manager(j->screen,wm,
    new button(20,20,1001,"Push Me!",
    new button(20,40,1002,"Don't push",
    new text_field(20,60,1004,"Name","**************",name,NULL)))
  );

  

  do
  {
    wm->get_event(ev);
    inm->handle_event(ev,j->x,j->y);
    if (ev.type==EV_MESSAGE && ev.message.id==1001)
       if ( *((int *)ev.message.data)==0)
         wm->new_window(10,10,100,50);
     
    wm->flush_screen();
  } while (ev.type!=EV_KEY || ev.key!='q');
  delete wm; 
  close_graphics();
  image_uninit();
}

