/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include <math.h>
#include <string.h>
#include <stdio.h>

#include "gifread.hpp"
#include "palette.hpp"
#include "image.hpp"
#include "video.hpp"
#include "linked.hpp"
#include "gifdecod.hpp"
#include "system.h"
#include "dos.h"
#include "dir.h"
#include "macs.hpp"


struct GifScreen {
	uint16_t	Width;
	uint16_t	Height;
	uint8_t 	ColorMap[3][256];
	uint16_t	BitPixel;
	uint16_t	ColorResolution;
	uint16_t	Background;
	uint16_t	AspectRatio;
} GifScreen;

struct gif_screen {
  uint16_t w,h;
  uint8_t color_info,background,reserved;
} gif_screen;

struct gif_image {
  uint16_t xoff,yoff,w,h;
  uint8_t color_info;
} gif_image;

image *read_gif(char *fn, palette *&pal)
{
  char buf[100],er;
  uint8_t sep;
  unsigned int ncolors;
  FILE *fp;
  image *im;
  clear_errors();
  fp=fopen(fn,"rb");
  er=0;
  im=NULL;
  if (fp==NULL) er=imFILE_NOT_FOUND;
  else
  {
    if (fread(buf,1,6,fp)==6)
    {
      buf[6]=0;
      if (!strcmp("GIF87a",buf))
      {
        fread((uint8_t *)&gif_screen.w,2,1,fp);
        gif_screen.w=uint16_to_local(gif_screen.w);
        fread((uint8_t *)&gif_screen.h,2,1,fp);
        gif_screen.h=uint16_to_local(gif_screen.h);
        fread((uint8_t *)&gif_screen.color_info,1,1,fp);
        fread((uint8_t *)&gif_screen.background,1,1,fp);
        if (fread((uint8_t *)&gif_screen.reserved,1,1,fp)==1)
	{
	  if (gif_screen.color_info&128)
	  {
	    ncolors=2<<(gif_screen.color_info&0x0f);
	    make_block(sizeof(palette));
//	    pal=new palette(ncolors);
	    pal=new palette(256);
	    if (pal)
	    {  
              if (fread((uint8_t *)pal->addr(),1,ncolors*3,fp)!=ncolors*3) er=imREAD_ERROR;
	    } else er=imMEMORY_ERROR;
	  }
	  if (!er)
	  { do
	    {
	      if (fread((uint8_t *)&sep,1,1,fp)!=1)
		er=imREAD_ERROR;
	    } while (!er && sep!=',');
            fread((uint8_t *)&gif_image.xoff,2,1,fp);
            gif_image.xoff=uint16_to_local(gif_image.xoff);
            fread((uint8_t *)&gif_image.yoff,2,1,fp);
            gif_image.yoff=uint16_to_local(gif_image.yoff);
            fread((uint8_t *)&gif_image.w,2,1,fp);
            gif_image.w=uint16_to_local(gif_image.w);
            fread((uint8_t *)&gif_image.h,2,1,fp);
            gif_image.h=uint16_to_local(gif_image.h);
	    if (!er && (fread((uint8_t *)&gif_image.color_info,1,1,fp)==1))
	    {
	      if (gif_image.color_info&128)
	      {
		ncolors=2<<(gif_image.color_info&0x0f);
                CHECK(ncolors<=256);
		make_block(sizeof(palette));
		pal = new palette(ncolors);
		if (pal)
		{ if (fread((uint8_t *)pal->addr(),1,ncolors*3,fp)!=ncolors*3) er=imREAD_ERROR;
		} else er=imMEMORY_ERROR;
	      }

	      if (!er)
	      {
		make_block(sizeof(image));
		im=new image(gif_image.w+1,gif_image.h);
		decode_gif_data(im,fp);
		fclose(fp);
	      }

	    } else er=imREAD_ERROR;
	  }

	} else er=imREAD_ERROR;
      } else er=imINCORRECT_FILETYPE;
    } else er=imREAD_ERROR;
    fclose(fp);
  }
  set_error(er);
  return im;
}
