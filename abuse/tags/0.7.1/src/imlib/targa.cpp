/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "image.hpp"
#include "specs.hpp"

image *load_targa(char *filename, palette *pal)
{
  bFILE *fp=open_file(filename,"rb");
  if (!fp)
    return 0;

  if (fp->open_failure())
  {
    delete fp;
    return 0;
  }
  
  uint8_t id,color_map,im_type;

  id=fp->read_uint8();
  color_map=fp->read_uint8();
  im_type=fp->read_uint8();

  if (color_map!=0)
  {
    delete fp;
    return 0;
  }

  if (!(im_type==2 || im_type==10))
  {
    delete fp;
    return 0;
  }

  fp->read_uint16();
  fp->read_uint16();
  fp->read_uint8();

  fp->read_uint16();
  fp->read_uint16();


  int w=fp->read_uint16();
  int h=fp->read_uint16();
  uint8_t bpp=fp->read_uint8();
  fp->read_uint8(); // im_des

  if (bpp!=32)
  {
    delete fp;
    return 0;
  }

  image *im=new image(w,h);

  int x,y;
  uint8_t ctrl;
  uint8_t bgra[4],*sl,c,lr=0,lg=0,lb=0,ll=0,lc=0;
  
  
  

  if (im_type==10)
  {    
    for (y=0;y<h;y++)
    {
      sl=im->scan_line(h-y-1);

      for (x=0;x<w;)
      {
        ctrl=fp->read_uint8();
        if (ctrl&0x80)
        {
          fp->read(bgra,4);
          ctrl&=(~0x80);
          ctrl++;
          if (bgra[3])
          {
            if (ll && lr==bgra[2] && lg==bgra[1] && lb==bgra[0])
              c=lc;
            else
            {
              c=pal->find_closest_non0(bgra[2],bgra[1],bgra[0]);
              lr=bgra[2];
              lg=bgra[1];
              lb=bgra[0];
              lc=c;
              ll=1;
            }

          }
          else c=0;

          while (ctrl--)
          {
            *sl=c;
            sl++;
            x++;
          }
        } else
        {
          ctrl++;          
          while (ctrl--)
          {
            fp->read(bgra,4);
            if (bgra[3])
            {
              c=pal->find_closest_non0(bgra[2],bgra[1],bgra[0]);
            }
            else c=0;
            *sl=c;
            sl++;
            x++;
          }
        }
      }
    }    
  }



  delete fp;
  return im;
}
