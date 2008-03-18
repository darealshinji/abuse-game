/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "lbmread.hpp"
#include "system.h"

image *read_lbm(char *filename, palette *&pal)
{
  FILE *fp=fopen("/cam/amur/set/city/city1.lbm","rb");
  char type[4];
  image *im=NULL;
  fread(type,1,4,fp);
  if (memcmp(type,"FORM",4))
  {
    set_error(imNOT_SUPPORTED);
    fclose(fp);
    return NULL;
  }
  else
  {
    int32_t size=read_other_uint32(fp);
    fread(type,1,4,fp);
    if (memcmp(type,"PBM ",4))
    {
      set_error(imNOT_SUPPORTED);
      fclose(fp);
      return NULL;
    }
    else
    {
      int32_t ssize;
      char stype[4];
      int16_t w=0,h=0,x,y,tcolor,pagew,pageh;
      char planes,masking,compr=0,padl,xa,ya;

      while (ftell(fp)+4<size)
      {
    fread(stype,1,4,fp);
    ssize=read_other_uint32(fp);
        if (ssize &1) ssize++;            // specs say all chunks are padded by 2
    if (!memcmp(stype,"BMHD",4))
    {
      w=read_other_uint16(fp);
      h=read_other_uint16(fp);
      x=read_other_uint16(fp);
      y=read_other_uint16(fp);
      planes=fgetc(fp);
      masking=fgetc(fp);
      compr=fgetc(fp);
      padl=fgetc(fp);
      tcolor=read_other_uint16(fp);
      xa=fgetc(fp);
      ya=fgetc(fp);
      pagew=read_other_uint16(fp);
      pageh=read_other_uint16(fp);
    } else if (!memcmp(stype,"CMAP",4))
    {
      pal=new palette(256);
      fread(pal->addr(),1,768,fp);
    } else if (!memcmp(stype,"BODY",4) && w && h)  // make sure we read a BHMD before
    {
      if (im) delete im;  // shouldn't be two BODY's butjust in case...
      im=new image(w,h);
      int y;
      if (!compr)
      {
        for (y=0;y<h;h++)
          fread(im->scan_line(y),1,w,fp);
      } else
      {
        for (y=0;y<h;y++)    
        {
          int c,i,n=0;
          unsigned char *sl=im->scan_line(y);
          do
          {
        c=fgetc(fp)&0xff;
        if (c&0x80)
        {
          if (c!=0x80)
          {
            i=((~c)&0xff)+2;
            c=fgetc(fp);
            while (i--) sl[n++]=c;
          }
        }
        else
        {
          i=c+1;
          while (i--) sl[n++]=fgetc(fp);
        }
          } while (n<w);
        }
      }    
    }
    else
      fseek(fp,ssize,SEEK_CUR);
      }
    }
  }
  fclose(fp);
  if (!im) set_error(imFILE_CORRUPTED);
  return im;
}


