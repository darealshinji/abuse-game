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

image *trans_image::make_image()
{
  image *im = new image(m_size);

  im->Lock();
  uint8_t *d=im->scan_line(0),*dp=m_data,*dline;
  int y,x;
  for (y=0; y<m_size.y; y++)
  {
    x=0;
    dline=d;
    memset(dline,0,m_size.x);
    while(x<m_size.x)
    {
      int skip=*(dp++);
      dline+=skip;
      x+=skip;
      if (x<m_size.x)
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
  m_size = im->Size();

  im->Lock();

  // first we must find out how much data to allocate
  for (y=0; y<im->Size().y; y++)
  {
    sl=im->scan_line(y);
    x=0;
    while (x<m_size.x)
    {
      size++;
      while (x<m_size.x && *sl==0) { sl++; x++; }

      if (x<m_size.x)
      {
        size++;  // byte for the size of the run
        while (x<m_size.x && (*sl)!=0)
        {
      size++;
      x++;
      sl++;
    }
      }
    }
  }

  m_data=(uint8_t *)malloc(size);
  int ww=im->Size().x,hh=im->Size().y;
  datap=m_data;
  if (!datap)
  { printf("size = %d %d (%d)\n",im->Size().x,im->Size().y,size);  }
  CONDITION(datap,"malloc error for trans_image::m_data");

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

trans_image::~trans_image()
{
    free(m_data);
}

void trans_image::put_scan_line(image *screen, int x, int y, int line)   // always transparent
{
  int x1, y1, x2, y2;
  screen->GetClip(x1, y1, x2, y2);
  if (y + line < y1 || y + line >= y2 || x >= x2 || x + m_size.x - 1 < x1)
    return; // clipped off completely?

  uint8_t *datap=m_data;
  int ix;
  while (line)            // skip scan line data until we get to the line of interest
  {
    for (ix=0; ix<m_size.x; )
    {
      ix+=*datap;        // skip blank space
      datap++;
      if (ix<m_size.x)
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

  for (ix=0; ix<m_size.x; )
  {
    int skip=*datap;              // how much space to skip?
    datap++;
    screen_line+=skip;
    ix+=skip;

    if (ix<m_size.x)
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


uint8_t *trans_image::ClipToLine(image *screen, int x1, int y1, int x2, int y2,
                                 int x, int &y, int &ysteps)
{
    // check to see if it is totally clipped out first
    if (y + m_size.y <= y1 || y >= y2 || x >= x2 || x + m_size.x <= x1)
        return NULL;

    uint8_t *parser = m_data;

    int skiplines = Max(y1 - y, 0); // number of lines to skip
    ysteps = Min(y2 - y, m_size.y - skiplines); // number of lines to draw
    y += skiplines; // first line to draw

    while (skiplines--)
    {
        for (int ix = 0; ix < m_size.x; )
        {
            ix += *parser++; // skip over empty space

            if (ix >= m_size.x)
                break;

            ix += *parser;
            parser += *parser + 1; // skip over data
        }
    }

    screen->AddDirty(Max(x, x1), y, Min(x + m_size.x, x2), y + m_size.y);
    return parser;
}

void trans_image::PutFilled(image *screen, int x, int y, uint8_t color)
{
    PutImageGeneric<FILLED>(screen, x, y, color, NULL, 0, 0, NULL, NULL,
                            0, 1, NULL, NULL, NULL);
}

void trans_image::put_image_offseted(image *screen, uint8_t *s_off)   // if screen x & y offset already calculated save a mul
{
  int ix,ysteps=m_size.y;
  int screen_skip;
  uint8_t skip,*datap=m_data;

  screen->Lock();
  screen_skip = screen->Size().x - m_size.x;
  for (; ysteps; ysteps--)
  {
    for (ix=0; ix<m_size.x; )
    {
      skip=*datap;       // skip leading blank space
      datap++;
      ix+=skip;
      s_off+=skip;

      if (s_off<screen->scan_line(0))
          printf("bad write in trans_image::put_image_offseted");


      if (ix<m_size.x)
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

template<int N>
void trans_image::PutImageGeneric(image *screen, int x, int y, uint8_t color,
                                  image *blend, int bx, int by,
                                  uint8_t *remap, uint8_t *remap2,
                                  int amount, int total_frames,
                                  uint8_t *tint, color_filter *f, palette *pal)
{
    int x1, y1, x2, y2;
    int ysteps, mul = 0;

    screen->GetClip(x1, y1, x2, y2);
    uint8_t *datap = ClipToLine(screen, x1, y1, x2, y2, x, y, ysteps),
            *screen_line, *blend_line = NULL, *paddr = NULL;
    if (!datap)
        return; // if ClipToLine says nothing to draw, return

    CONDITION(N == BLEND && y >= by && y + ysteps < by + blend->Size().y + 1,
              "Blend doesn't fit on trans_image");

    if (N == FADE || N == FADE_TINT || N == BLEND)
        paddr = (uint8_t *)pal->addr();

    if (N == FADE || N == FADE_TINT)
        mul = (amount << 16) / total_frames;
    else if (N == BLEND)
        mul = ((16 - amount) << 16 / 16);

    if (N == PREDATOR)
        ysteps = Min(ysteps, y2 - 1 - y - 2);

    screen->Lock();

    screen_line = screen->scan_line(y)+x;
    int sw = screen->Size().x;
    x1 -= x; x2 -= x;

    for (; ysteps > 0; ysteps--, y++)
    {
        if (N == BLEND)
            blend_line = blend->scan_line(y - by);

        for (int ix = 0; ix < m_size.x; )
        {
            // Handle a run of transparent pixels
            int todo = *datap++;

            // FIXME: implement FILLED mode
            ix += todo;
            screen_line += todo;

            if (ix >= m_size.x)
                break;

            // Handle a run of solid pixels
            todo = *datap++;

            // Chop left side if necessary, but no more than todo
            int tochop = Min(todo, Max(x1 - ix, 0));

            ix += tochop;
            screen_line += tochop;
            datap += tochop;
            todo -= tochop;

            // Chop right side if necessary and process the remaining pixels
            int count = Min(todo, Max(x2 - ix, 0));

            if (N == NORMAL)
            {
                memcpy(screen_line, datap, count);
            }
            else if (N == COLOR)
            {
                memset(screen_line, color, count);
            }
            else if (N == PREDATOR)
            {
                memcpy(screen_line, screen_line + 2 * m_size.x, count);
            }
            else if (N == REMAP)
            {
                uint8_t *sl = screen_line, *sl2 = datap;
                while (count--)
                    *sl++ = remap[*sl2++];
            }
            else if (N == DOUBLE_REMAP)
            {
                uint8_t *sl = screen_line, *sl2 = datap;
                while (count--)
                    *sl++ = remap2[remap[*sl2++]];
            }
            else if (N == FADE || N == FADE_TINT || N == BLEND)
            {
                uint8_t *sl = screen_line;
                uint8_t *sl2 = (N == BLEND) ? blend_line + x + ix - bx : sl;
                uint8_t *sl3 = datap;

                while (count--)
                {
                    uint8_t *p1 = paddr + 3 * *sl2++;
                    uint8_t *p2 = paddr + 3 * (N == FADE_TINT ? tint[*sl3++] : *sl3++);

                    uint8_t r = ((((int)p1[0] - p2[0]) * mul) >> 16) + p2[0];
                    uint8_t g = ((((int)p1[1] - p2[1]) * mul) >> 16) + p2[1];
                    uint8_t b = ((((int)p1[2] - p2[2]) * mul) >> 16) + p2[2];

                    *sl++ = f->lookup_color(r >> 3, g >> 3, b >> 3);
                }
            }

            datap += todo;
            ix += todo;
            screen_line += todo;
        }
        screen_line += sw - m_size.x;
    }
    screen->Unlock();
}

void trans_image::PutImage(image *screen, int x, int y)
{
    PutImageGeneric<NORMAL>(screen, x, y, 0, NULL, 0, 0, NULL, NULL,
                            0, 1, NULL, NULL, NULL);
}

void trans_image::PutRemap(image *screen, int x, int y, uint8_t *remap)
{
    PutImageGeneric<REMAP>(screen, x, y, 0, NULL, 0, 0, remap, NULL,
                           0, 1, NULL, NULL, NULL);
}

void trans_image::PutDoubleRemap(image *screen, int x, int y,
                                 uint8_t *remap, uint8_t *remap2)
{
    PutImageGeneric<DOUBLE_REMAP>(screen, x, y, 0, NULL, 0, 0, remap, remap2,
                                  0, 1, NULL, NULL, NULL);
}

// Used when eg. the player teleports, or in rocket trails
void trans_image::PutFade(image *screen, int x, int y,
                          int amount, int total_frames,
                          color_filter *f, palette *pal)
{
    PutImageGeneric<FADE>(screen, x, y, 0, NULL, 0, 0, NULL, NULL,
                          amount, total_frames, NULL, f, pal);
}

void trans_image::PutFadeTint(image *screen, int x, int y,
                              int amount, int total_frames,
                              uint8_t *tint, color_filter *f, palette *pal)
{
    PutImageGeneric<FADE_TINT>(screen, x, y, 0, NULL, 0, 0, NULL, NULL,
                               amount, total_frames, tint, f, pal);
}

void trans_image::PutColor(image *screen, int x, int y, uint8_t color)
{
    PutImageGeneric<COLOR>(screen, x, y, color, NULL, 0, 0, NULL, NULL,
                           0, 1, NULL, NULL, NULL);
}

// This method is unused but is believed to work.
// Assumes that the blend image completely covers the transparent image.
void trans_image::PutBlend(image *screen, int x, int y,
                           image *blend, int bx, int by,
                           int amount, color_filter *f, palette *pal)
{
    PutImageGeneric<BLEND>(screen, x, y, 0, blend, bx, by, NULL, NULL,
                           amount, 1, NULL, f, pal);
}

void trans_image::PutPredator(image *screen, int x, int y)
{
    PutImageGeneric<PREDATOR>(screen, x, y, 0, NULL, 0, 0, NULL, NULL,
                              0, 1, NULL, NULL, NULL);
}

size_t trans_image::MemUsage()
{
    uint8_t *d = m_data;
    size_t t = 0;

    for (int y = 0; y < m_size.y; y++)
    {
        for (int x = 0; x < m_size.x; x++)
        {
            x += *d; d++; t++;

            if (x >= m_size.x)
                break;

            int s = *d; d++; t += s + 1; d += s; x += s;
        }
    }
    return t + 4 + 4;
}

