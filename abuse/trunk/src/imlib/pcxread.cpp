/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "pcxread.h"
#include "specs.h"

struct PCX_header_type
{
  char manufactururer,version,encoding,bits_per_pixel;
  short xmin,ymin,xmax,ymax,hres,vres;
  char palette[48];
  char reserved,color_planes;
  short bytes_per_line,palette_type;
  char filter[58];
} PCX_header;

int write_PCX_header(FILE *fp)
{
  if (!fwrite(&PCX_header.manufactururer,1,1,fp)) return 0;
  if (!fwrite(&PCX_header.version,1,1,fp)) return 0;
  if (!fwrite(&PCX_header.encoding,1,1,fp)) return 0;
  if (!fwrite(&PCX_header.bits_per_pixel,1,1,fp)) return 0;
  write_uint16(fp,PCX_header.xmin);
  write_uint16(fp,PCX_header.ymin);
  write_uint16(fp,PCX_header.xmax);
  write_uint16(fp,PCX_header.ymax);
  write_uint16(fp,PCX_header.hres);
  write_uint16(fp,PCX_header.vres);
  if (!fwrite(PCX_header.palette,1,48,fp)) return 0;
  if (!fwrite(&PCX_header.reserved,1,1,fp)) return 0;
  if (!fwrite(&PCX_header.color_planes,1,1,fp)) return 0;
  write_uint16(fp,PCX_header.bytes_per_line);
  write_uint16(fp,PCX_header.palette_type);
  if (!fwrite(PCX_header.filter,1,58,fp)) return 0;
  return 1;
}

void write_PCX(image *im, palette *pal, char const *filename)
{
  FILE *fp=fopen(filename,"wb");
  if (!fp)
  {
    set_error(imWRITE_ERROR);
    return ;
  }

  PCX_header.manufactururer=10;
  PCX_header.version=5;
  PCX_header.encoding=1;
  PCX_header.bits_per_pixel=8;
  PCX_header.xmin=0;
  PCX_header.ymin=0;
  PCX_header.xmax=im->width()-1;
  PCX_header.ymax=im->height()-1;
  PCX_header.hres=320;
  PCX_header.vres=200;
  PCX_header.reserved=0;
  PCX_header.color_planes=1;
  PCX_header.bytes_per_line=im->width();
  PCX_header.palette_type=0;
  memset(PCX_header.filter,0,58);

  if (!write_PCX_header(fp))
  {
    set_error( imWRITE_ERROR);
    return ;
  }

  int y,run_length,x;
  unsigned char *sl,code;
  for (y=0;y<im->height();y++)
  {
    sl=im->scan_line(y);
    for (x=0;x<im->width();)
    {
      run_length=1;
      while (x+run_length<im->width() && sl[x]==sl[x+run_length])
        run_length++;
      if (run_length==1 && sl[x]<64)
        fputc(sl[x],fp);
      else
      {
        if (run_length>=64)
      run_length=63;
    code=0xc0 | run_length;
    fputc(code,fp);
    fputc(sl[x],fp);
    
      }
      x+=run_length;

    }
  }
  fputc(12,fp);  // note that there is a palette attached
  fwrite(pal->addr(),1,256*3,fp);
  fclose(fp);
}

