/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "common.h"

#include "timage.h"

void trans_image::make_color(int c)
{
  uint8_t *dp=data;
  int y,x;
  for (y=0; y<h; y++)
  {
    x=0;
    while(x<w)
    {
      x+=*(dp++);
      if (x<w)
      {
    int run=*(dp++);
    memset(dp,c,run);
    x+=run;
    dp+=run;
      }
    }
  }
}

image *trans_image::make_image()
{
  image *im=new image(vec2i(w,h));

  im->Lock();
  uint8_t *d=im->scan_line(0),*dp=data,*dline;
  int y,x;
  for (y=0; y<h; y++)
  {
    x=0;
    dline=d;
    memset(dline,0,w);
    while(x<w)
    {
      int skip=*(dp++);
      dline+=skip;
      x+=skip;
      if (x<w)
      {
    int run=*(dp++);
    memcpy(dline,dp,run);
    x+=run;
    dline+=run;
    dp+=run;
      }
    }
    d=im->next_line(y,d);
  }
  im->Unlock();
  return im;
}

trans_image::trans_image(image *im, char const *name)
{
  int size=0,x,y;
  uint8_t *sl,*datap,*marker;
  w=im->Size().x;
  h=im->Size().y;

  im->Lock();

  // first we must find out how much data to allocate
  for (y=0; y<im->Size().y; y++)
  {
    sl=im->scan_line(y);
    x=0;
    while (x<w)
    {
      size++;
      while (x<w && *sl==0) { sl++; x++; }

      if (x<w)
      {
        size++;  // byte for the size of the run
        while (x<w && (*sl)!=0)
        {
      size++;
      x++;
      sl++;
    }
      }
    }
  }

  data=(uint8_t *)malloc(size);
  int ww=im->Size().x,hh=im->Size().y;
  datap=data;
  if (!datap)
  { printf("size = %d %d (%d)\n",im->Size().x,im->Size().y,size);  }
  CONDITION(datap,"malloc error for trans_image::data");

  for (y=0; y<hh; y++)  // now actually make the runs
  {
    sl=im->scan_line(y);
    x=0;
    while (x<ww)
    {
      *datap=0;  // start the skip at 0
      while (x<im->Size().x && (*sl)==0)
      { sl++; x++; (*datap)++; }
      datap++;

      if (x<ww)
      {
        marker=datap;   // let marker be the run size
    *marker=0;
    datap++;    // skip over this spot
        while (x<im->Size().x && (*sl)!=0)
        {
          (*marker)++;
      (*datap)=*sl;
          datap++;
      x++;
      sl++;
    }
      }
    }
  }
  im->Unlock();
}

void trans_image::put_scan_line(image *screen, int x, int y, int line)   // always transparent
{
  int x1, y1, x2, y2;
  screen->GetClip(x1, y1, x2, y2);
  if (y + line < y1 || y + line >= y2 || x >= x2 || x + w - 1 < x1)
    return; // clipped off completely?

  uint8_t *datap=data;
  int ix;
  while (line)            // skip scan line data until we get to the line of interest
  {
    for (ix=0; ix<w; )
    {
      ix+=*datap;        // skip blank space
      datap++;
      if (ix<w)
      {
    int run_length=*datap;     // skip run
    ix+=run_length;
    datap+=run_length+1;
      }
    }
    line--;
    y++;
  }


  // now slam this list of runs to the screen
  screen->Lock();
  uint8_t *screen_line=screen->scan_line(y)+x;

  for (ix=0; ix<w; )
  {
    int skip=*datap;              // how much space to skip?
    datap++;
    screen_line+=skip;
    ix+=skip;

    if (ix<w)
    {
      int run_length=*datap;
      datap++;

      if (x+ix+run_length-1<x1)      // is this run clipped out totally?
      {
    datap+=run_length;
    ix+=run_length;
    screen_line+=run_length;
      }
      else
      {
    if (x+ix<x1)                 // is the run clipped partially?
    {
      int clip=(x1-(x+ix));
      datap+=clip;
      run_length-=clip;
      screen_line+=clip;
      ix+=clip;
    }

    if (x + ix >= x2)                      // clipped totally on the right?
        {
          screen->Unlock();
          return ;                        // we are done, return!
        }
    else if (x + ix + run_length > x2)    // partially clipped?
    {
      memcpy(screen_line, datap, x + ix + run_length - x2); // slam what we can
      screen->Unlock();
      return ;    // and return 'cause we are done with the line
        } else
        {
      memcpy(screen_line,datap,run_length);
      screen_line+=run_length;
        datap+=run_length;
        ix+=run_length;
        }
      }
    }
  }
  screen->Unlock();
}


inline uint8_t *trans_image::clip_y(image *screen, int x1, int y1, int x2, int y2,
                   int x, int &y, int &ysteps)
{
  // check to see if it is total clipped out first
  if (y+h<=y1 || y>y2 || x>x2 || x+w<=x1)
    return NULL;

  uint8_t *datap=data;


  ysteps=height();

  if (y<y1)  // check to see if the image gets clipped at the top
  {


    // because data is stored in runs, we need to skip over the top clipped portion
    int skips=(y1-y);     // how many lines do we need to skip?
    ysteps-=skips;        // reduce h (number of lines to draw)
    y=y1;                // start drawing here now
    while (skips--)
    {
      int ix=0;
      while (ix<w)
      {
        ix+=(*datap);       // skip over empty space
        datap++;
        if (ix<w)
        { ix+=*datap;
        datap+=(*datap)+1;   // skip over data
        }
      }
    }
  }

  if (y+ysteps>y2)  // check to see if it gets clipped at the bottom
    ysteps-=(y+ysteps-y2-1);

  screen->AddDirty(Max(x, x1), y, Min(x + width(), x2 + 1), y + h);
  return datap;
}

void trans_image::put_image_filled(image *screen, int x, int y,
                   uint8_t fill_color)
{
  int x1, y1, x2, y2;
  int chop_length,ysteps;

  screen->GetClip(x1, y1, x2, y2);
  uint8_t *datap = clip_y(screen, x1, y1, x2 - 1, y2 - 1, x, y, ysteps),
          *screen_line;
  if (!datap) return ;     // if clip_y says nothing to draw, return

  screen->Lock();

  screen_line=screen->scan_line(y)+x;
  int sw=screen->Size().x-w;
  x1 -= x; x2 -= x;
  for (; ysteps>0; ysteps--)
  {
    int ix,slam_length;
    for (ix=0; ix<w; )
    {
      int blank=(*datap);
      memset(screen_line,fill_color,blank);
      ix+=blank;       // skip over empty space
      screen_line+=blank;

      datap++;
      if (ix<w)
      {
    slam_length=*datap;     // find the length of this run
    datap++;
    if (ix + slam_length < x1 || ix >= x2)  // see if this run is totally clipped
    {
      datap+=slam_length;
      ix+=slam_length;
      screen_line+=slam_length;
    }
    else
    {
      if (ix<x1)                // the left side needs to be chopped ?
      {
        chop_length=(x1-ix);

        if (chop_length>=slam_length)  // see if we chopped it all off
        {                              // yes, we did
          ix+=slam_length;             // advance everything to the end of run
          screen_line+=slam_length;
          datap+=slam_length;
          slam_length=0;
        } else
        {
          slam_length-=chop_length;   // else advance everything to begining of slam
          ix+=chop_length;
          screen_line+=chop_length;
          datap+=chop_length;
        }
      }

      if (slam_length)   // see if there is anything left to slam
      {
        if (ix + slam_length >= x2) // see if right side needs to be chopped off
          memcpy(screen_line, datap, x2 - ix);
        else
          memcpy(screen_line,datap,slam_length);
        datap+=slam_length;
        ix+=slam_length;
        screen_line+=slam_length;
      }
    }
      }
    }
    screen_line+=sw;
  }
  screen->Unlock();
}

void trans_image::put_image_offseted(image *screen, uint8_t *s_off)   // if screen x & y offset already calculated save a mul
{
  int ix,ysteps=height();
  int screen_skip;
  uint8_t skip,*datap=data;

  screen->Lock();
  screen_skip = screen->Size().x - w;
  for (; ysteps; ysteps--)
  {
    for (ix=0; ix<w; )
    {
      skip=*datap;       // skip leading blank space
      datap++;
      ix+=skip;
      s_off+=skip;

      if (s_off<screen->scan_line(0))
          printf("bad write in trans_image::put_image_offseted");


      if (ix<w)
      {
    skip=*datap;
    datap++;
    memcpy(s_off,datap,skip);
    datap+=skip;
    s_off+=skip;
    ix+=skip;

    if (s_off>=screen->scan_line(screen->Size().y+1))
            printf("bad write in trans_image::put_image_offseted");
      }
    }
    s_off+=screen_skip;
  }
  screen->Unlock();
}

void trans_image::put_image(image *screen, int x, int y)
{
  int x1, y1, x2, y2;
  int chop_length,ysteps;

  screen->GetClip(x1, y1, x2, y2);
  uint8_t *datap = clip_y(screen, x1, y1, x2 - 1, y2 - 1, x, y, ysteps),
          *screen_line;
  if (!datap) return ;     // if clip_y says nothing to draw, return

  screen->Lock();
  screen_line=screen->scan_line(y)+x;
  int sw=screen->Size().x;
  x1 -= x; x2 -= x;
  for (; ysteps>0; ysteps--)
  {
    int ix,slam_length;
    for (ix=0; ix<w; )
    {
      ix+=(*datap);       // skip over empty space
      datap++;
      if (ix<w)
      {
    slam_length=*datap;     // find the length of this run
    datap++;
    if (ix + slam_length < x1 || ix >= x2)  // see if this run is totally clipped
    {
      datap+=slam_length;
      ix+=slam_length;
    }
    else
    {
      if (ix<x1)                // the left side needs to be chopped ?
      {
        chop_length=(x1-ix);

        if (chop_length>=slam_length)  // see if we chopped it all off
        {                              // yes, we did
          ix+=slam_length;             // advance everything to the end of run
          datap+=slam_length;
          slam_length=0;
        } else
        {
          slam_length-=chop_length;   // else advance everything to begining of slam
          ix+=chop_length;
          datap+=chop_length;
        }
      }

      if (slam_length)   // see if there is anything left to slam
      {
        if (ix + slam_length >= x2) // see if right side needs to be chopped off
          memcpy(screen_line + ix, datap, x2 - ix);
        else
          memcpy(screen_line+ix,datap,slam_length);
        datap+=slam_length;
        ix+=slam_length;
      }
    }
      }
    }
    screen_line+=sw;
  }
  screen->Unlock();
}

void trans_image::put_remaped(image *screen, int x, int y, uint8_t *remap)
{
  int x1, y1, x2, y2;
  int chop_length, ysteps;

  screen->GetClip(x1, y1, x2, y2);
  uint8_t *datap = clip_y(screen, x1, y1, x2 - 1, y2 - 1, x, y, ysteps),
          *screen_line;
  if (!datap) return ;     // if clip_y says nothing to draw, return

  screen->Lock();
  screen_line=screen->scan_line(y)+x;
  int sw=screen->Size().x;
  x1 -= x; x2 -= x;
  for (; ysteps>0; ysteps--)
  {
    int ix,slam_length;
    for (ix=0; ix<w; )
    {
      ix+=(*datap);       // skip over empty space
      datap++;
      if (ix<w)
      {
    slam_length=*datap;     // find the length of this run
    datap++;
    if (ix + slam_length < x1 || ix >= x2)  // see if this run is totally clipped
    {
      datap+=slam_length;
      ix+=slam_length;
    }
    else
    {
      if (ix<x1)                // the left side needs to be chopped ?
      {
        chop_length=(x1-ix);

        if (chop_length>=slam_length)  // see if we chopped it all off
        {                              // yes, we did
          ix+=slam_length;             // advance everything to the end of run
          datap+=slam_length;
          slam_length=0;
        } else
        {
          slam_length-=chop_length;   // else advance everything to begining of slam
          ix+=chop_length;
          datap+=chop_length;
        }
      }


      if (slam_length)   // see if there is anything left to slam
      {
        int counter;
        if (ix + slam_length >= x2) // see if right side needs to be chopped off
          counter = x2 - ix;
        else
          counter = slam_length;

        uint8_t *sl=screen_line+ix,*sl2=datap;
        ix+=slam_length;
        datap+=slam_length;
        while (counter)
        {
          counter--;
          *(sl)=remap[*(sl2)];
          sl++;
          sl2++;
        }
      }
    }
      }
    }
    screen_line+=sw;
  }
  screen->Unlock();
}



void trans_image::put_double_remaped(image *screen, int x, int y, uint8_t *remap, uint8_t *remap2)
{
  int x1, y1, x2, y2;
  int chop_length, ysteps;

  screen->GetClip(x1, y1, x2, y2);
  uint8_t *datap = clip_y(screen, x1, y1, x2 - 1, y2 - 1, x, y, ysteps),
          *screen_line;
  if (!datap) return ;     // if clip_y says nothing to draw, return

  screen->Lock();
  screen_line=screen->scan_line(y)+x;
  int sw=screen->Size().x;
  x1 -= x; x2 -= x;
  for (; ysteps>0; ysteps--)
  {
    int ix,slam_length;
    for (ix=0; ix<w; )
    {
      ix+=(*datap);       // skip over empty space
      datap++;
      if (ix<w)
      {
    slam_length=*datap;     // find the length of this run
    datap++;
    if (ix + slam_length < x1 || ix >= x2)  // see if this run is totally clipped
    {
      datap+=slam_length;
      ix+=slam_length;
    }
    else
    {
      if (ix<x1)                // the left side needs to be chopped ?
      {
        chop_length=(x1-ix);

        if (chop_length>=slam_length)  // see if we chopped it all off
        {                              // yes, we did
          ix+=slam_length;             // advance everything to the end of run
          datap+=slam_length;
          slam_length=0;
        } else
        {
          slam_length-=chop_length;   // else advance everything to begining of slam
          ix+=chop_length;
          datap+=chop_length;
        }
      }


      if (slam_length)   // see if there is anything left to slam
      {
        int counter;
        if (ix + slam_length >= x2) // see if right side needs to be chopped off
          counter = x2 - ix;
        else
          counter = slam_length;

        uint8_t *sl=screen_line+ix,*sl2=datap;
        ix+=slam_length;
        datap+=slam_length;
        while (counter)
        {
          counter--;
          *(sl)=remap2[remap[*(sl2)]];
          sl++;
          sl2++;
        }
      }
    }
      }
    }
    screen_line+=sw;
  }
  screen->Unlock();
}



void trans_image::put_fade(image *screen, int x, int y,
               int frame_on, int total_frames,
               color_filter *f, palette *pal)
{
  int x1, y1, x2, y2;
  int ix,slam_length,chop_length,ysteps;

  screen->GetClip(x1, y1, x2, y2);
  uint8_t *datap = clip_y(screen, x1, y1, x2 - 1, y2 - 1, x, y, ysteps),
          *screen_line;
  if (!datap) return ;

  uint8_t *screen_run,*paddr=(uint8_t *)pal->addr(),
                *caddr1,*caddr2,r_dest,g_dest,b_dest;

  long fixmul=(frame_on<<16)/total_frames;
  screen->Lock();
  for (; ysteps>0; ysteps--,y++)
  {
    screen_line=screen->scan_line(y);

    for (ix=0; ix<w; )
    {
      ix+=(*datap);       // skip over empty space
      datap++;
      if (ix<w)
      {
    slam_length=*datap;     // find the length of this run
    datap++;
    if (x + ix + slam_length < x1 || x + ix >= x2)  // see if this run is totally clipped
    {
      datap+=slam_length;
      ix+=slam_length;
    }
    else
    {
      if (x+ix<x1)                // the left side needs to be chopped ?
      {
        chop_length=(x1-x-ix);

        if (chop_length>=slam_length)  // see if we chopped it all off
        {                              // yes, we did
          ix+=slam_length;             // advance everything to the end of run
          datap+=slam_length;
          slam_length=0;
        } else
        {
          slam_length-=chop_length;   // else advance everything to begining of slam
          ix+=chop_length;
          datap+=chop_length;
        }
      }

      if (slam_length)   // see if there is anything left to slam
      {
        if (x + ix + slam_length >= x2) // see if right side needs to be chopped off
          chop_length = x2 - 1 - x - ix;
        else
          chop_length = slam_length;
        screen_run=screen_line+x+ix;

        slam_length-=chop_length;
        ix+=chop_length;

        while (chop_length--)
        {
          caddr1=paddr+(int)(*screen_run)*3;
          caddr2=paddr+(int)(*datap)*3;

          r_dest=((((int)(*caddr1)-(int)(*caddr2))*fixmul)>>16)+(int)(*caddr2);
          caddr1++; caddr2++;

          g_dest=((((int)(*caddr1)-(int)(*caddr2))*fixmul)>>16)+(int)(*caddr2);
          caddr1++; caddr2++;

          b_dest=((((int)(*caddr1)-(int)(*caddr2))*fixmul)>>16)+(int)(*caddr2);
          *screen_run=f->lookup_color(r_dest>>3,g_dest>>3,b_dest>>3);

          screen_run++;
          datap++;
        }
        datap+=slam_length;
        ix+=slam_length;
      }
    }
      }
    }
  }
  screen->Unlock();
}




void trans_image::put_fade_tint(image *screen, int x, int y,
                int frame_on, int total_frames,
                uint8_t *tint,
                color_filter *f, palette *pal)
{
  int x1, y1, x2, y2;
  int ix,slam_length,chop_length,ysteps;

  screen->GetClip(x1, y1, x2, y2);
  uint8_t *datap = clip_y(screen, x1, y1, x2 - 1, y2 - 1, x, y, ysteps),
          *screen_line;
  if (!datap) return ;

  screen->Lock();
  uint8_t *screen_run,*paddr=(uint8_t *)pal->addr(),
                *caddr1,*caddr2,r_dest,g_dest,b_dest;

  long fixmul=(frame_on<<16)/total_frames;
  for (; ysteps>0; ysteps--,y++)
  {
    screen_line=screen->scan_line(y);

    for (ix=0; ix<w; )
    {
      ix+=(*datap);       // skip over empty space
      datap++;
      if (ix<w)
      {
    slam_length=*datap;     // find the length of this run
    datap++;
    if (x + ix + slam_length < x1 || x + ix >= x2)  // see if this run is totally clipped
    {
      datap+=slam_length;
      ix+=slam_length;
    }
    else
    {
      if (x+ix<x1)                // the left side needs to be chopped ?
      {
        chop_length=(x1-x-ix);

        if (chop_length>=slam_length)  // see if we chopped it all off
        {                              // yes, we did
          ix+=slam_length;             // advance everything to the end of run
          datap+=slam_length;
          slam_length=0;
        } else
        {
          slam_length-=chop_length;   // else advance everything to begining of slam
          ix+=chop_length;
          datap+=chop_length;
        }
      }

      if (slam_length)   // see if there is anything left to slam
      {
        if (x + ix + slam_length >= x2) // see if right side needs to be chopped off
          chop_length = x2 - 1 - x - ix;
        else chop_length=slam_length;
        screen_run=screen_line+x+ix;

        slam_length-=chop_length;
        ix+=chop_length;

        while (chop_length--)
        {
          caddr1=paddr+(int)(*screen_run)*3;
          caddr2=paddr+(int)(tint[*datap])*3;

          r_dest=((((int)(*caddr1)-(int)(*caddr2))*fixmul)>>16)+(int)(*caddr2);
          caddr1++; caddr2++;

          g_dest=((((int)(*caddr1)-(int)(*caddr2))*fixmul)>>16)+(int)(*caddr2);
          caddr1++; caddr2++;

          b_dest=((((int)(*caddr1)-(int)(*caddr2))*fixmul)>>16)+(int)(*caddr2);
          *screen_run=f->lookup_color(r_dest>>3,g_dest>>3,b_dest>>3);

          screen_run++;
          datap++;
        }
        datap+=slam_length;
        ix+=slam_length;
      }
    }
      }
    }
  }
  screen->Unlock();
}

void trans_image::put_color(image *screen, int x, int y, int color)
{
  int x1, y1, x2, y2;
  int ix,slam_length,chop_length,ysteps;

  screen->GetClip(x1, y1, x2, y2);
  uint8_t *datap = clip_y(screen, x1, y1, x2, y2, x, y, ysteps),
          *screen_line;
  if (!datap) return ;

  screen->Lock();
  for (; ysteps>0; ysteps--,y++)
  {
    screen_line=screen->scan_line(y);

    for (ix=0; ix<w; )
    {
      ix+=(*datap);       // skip over empty space
      datap++;
      if (ix<w)
      {
    slam_length=*datap;     // find the length of this run
    datap++;
    if (x + ix + slam_length < x1 || x + ix >= x2)  // see if this run is totally clipped
    {
      datap+=slam_length;
      ix+=slam_length;
    }
    else
    {
      if (x+ix<x1)                // the left side needs to be chopped ?
      {
        chop_length=(x1-x-ix);

        if (chop_length>=slam_length)  // see if we chopped it all off
        {                              // yes, we did
          ix+=slam_length;             // advance everything to the end of run
          datap+=slam_length;
          slam_length=0;
        } else
        {
          slam_length-=chop_length;   // else advance everything to begining of slam
          ix+=chop_length;
          datap+=chop_length;
        }
      }

      if (slam_length)   // see if there is anything left to slam
      {
        if (x + ix + slam_length >= x2) // see if right side needs to be chopped off
          memset(screen_line + x + ix, color, x2 - x - ix);
        else
          memset(screen_line + x + ix, color, slam_length);
        datap+=slam_length;
        ix+=slam_length;
      }
    }
      }
    }
  }
  screen->Unlock();
}


// ASSUMES that the blend image completly covers this image
void trans_image::put_blend16(image *screen, image *blend, int x, int y,
                  int blendx, int blendy, int blend_amount, color_filter *f, palette *pal)

{
  int x1, y1, x2, y2;
  int ix,slam_length,chop_length,ysteps;
  uint8_t *paddr=(uint8_t *)pal->addr();

  screen->GetClip(x1, y1, x2, y2);
  uint8_t *datap=clip_y(screen, x1, y1, x2 - 1, y2 - 1, x, y, ysteps),
          *blend_line, *screen_line;
  if (!datap) return ;
  CONDITION(y>=blendy && y+ysteps<blendy+blend->Size().y+1,"Blend doesn't fit on trans_image");

  blend_amount=16-blend_amount;

  screen->Lock();
  for (; ysteps>0; ysteps--,y++)
  {
    screen_line=screen->scan_line(y);
    blend_line=blend->scan_line(y-blendy);


    for (ix=0; ix<w; )
    {
      ix+=(*datap);       // skip over empty space
      datap++;
      if (ix<w)
      {
    slam_length=*datap;     // find the length of this run
    datap++;
    if (x + ix + slam_length < x1 || x + ix >= x2)  // see if this run is totally clipped
    {
      datap+=slam_length;
      ix+=slam_length;
    }
    else
    {
      if (x+ix<x1)                // the left side needs to be chopped ?
      {
        chop_length=(x1-x-ix);

        if (chop_length>=slam_length)  // see if we chopped it all off
        {                              // yes, we did
          ix+=slam_length;             // advance everything to the end of run
          datap+=slam_length;
          slam_length=0;
        } else
        {
          slam_length-=chop_length;   // else advance everything to begining of slam
          ix+=chop_length;
          datap+=chop_length;
        }
      }

      if (slam_length)   // see if there is anything left to slam
      {

        if (x + ix + slam_length >= x2) // see if right side needs to be chopped off
          chop_length = x2 - 1 - x - ix;
        else
          chop_length = slam_length;

        uint8_t *screen_run=screen_line+x+ix,
                      *blend_run=blend_line+x+ix-blendx,
                      *caddr1,*caddr2,r_dest,g_dest,b_dest;

        slam_length-=chop_length;
        ix+=chop_length;


        while (chop_length--)
        {
          caddr1=paddr+(int)(*blend_run)*3;
          caddr2=paddr+(int)(*datap)*3;

          r_dest=((int)(*caddr1)-(int)(*caddr2))*blend_amount/16+(int)(*caddr2);
          caddr1++; caddr2++;

          g_dest=((int)(*caddr1)-(int)(*caddr2))*blend_amount/16+(int)(*caddr2);
          caddr1++; caddr2++;

          b_dest=((int)(*caddr1)-(int)(*caddr2))*blend_amount/16+(int)(*caddr2);

          *screen_run=f->lookup_color(r_dest>>3,g_dest>>3,b_dest>>3);


          screen_run++;
          blend_run++;
          datap++;
        }
        datap+=slam_length;
        ix+=slam_length;
      }

    }
      }
    }
  }

  screen->Unlock();
}

void trans_image::put_predator(image *screen, int x, int y)
{
  int x1, y1, x2, y2;
  int chop_length,ysteps;

  screen->GetClip(x1, y1, x2, y2);
  uint8_t *datap = clip_y(screen, x1, y1, x2 - 1, y2 - 1, x, y, ysteps),
          *screen_line;
  if (!datap) return ;     // if clip_y says nothing to draw, return

  // see if the last scanline is clipped off
  if (y + ysteps == y2 - 1) ysteps -= 2;
  else if (y + ysteps == y2 - 2) ysteps--;
/*  {
    for (int x=0; x<w; )
    {
      int skip=*datap; datap++;
      x+=skip;
      if (x<w)
      {
    int run_size=*datap;
    datap+=run_size+1;
    x+=run_size;
      }
    }
    if (y==y2 - 1)
      return;
    else
      y++;
  }*/

  screen->Lock();
  screen_line=screen->scan_line(y)+x;
  int sw=screen->Size().x;
  x1 -= x; x2 -= x;
  for (; ysteps>0; ysteps--)
  {
    int ix,slam_length;
    for (ix=0; ix<w; )
    {
      ix+=(*datap);       // skip over empty space
      datap++;
      if (ix<w)
      {
    slam_length=*datap;     // find the length of this run
    datap++;
    if (ix + slam_length < x1 || ix >= x2)  // see if this run is totally clipped
    {
      datap+=slam_length;
      ix+=slam_length;
    }
    else
    {
      if (ix<x1)                // the left side needs to be chopped ?
      {
        chop_length=(x1-ix);

        if (chop_length>=slam_length)  // see if we chopped it all off
        {                              // yes, we did
          ix+=slam_length;             // advance everything to the end of run
          datap+=slam_length;
          slam_length=0;
        } else
        {
          slam_length-=chop_length;   // else advance everything to begining of slam
          ix+=chop_length;
          datap+=chop_length;
        }
      }

      if (slam_length)   // see if there is anything left to slam
      {
        if (ix + slam_length >= x2) // see if right side needs to be chopped off
          memcpy(screen_line + ix, screen_line + sw + sw + ix, x2 - ix);
        else
          memcpy(screen_line + ix, screen_line + sw + sw + ix, slam_length);
        datap+=slam_length;
        ix+=slam_length;
      }
    }
      }
    }
    screen_line+=sw;
  }
  screen->Unlock();
}

int trans_image::size()
{
  uint8_t *d=data;
  int t=0;
  for (int y=0; y<h; y++)
  {
    int x=0;
    while (x<w)
    {
      x+=*d; d++; t++;
      if (x<w)
      {
    int s=*d; d++; t+=s+1;
    d+=s;
    x+=s;
      }
    }
  }
  return t+4+4;
}
