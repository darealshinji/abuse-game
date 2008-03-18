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

#include "macs.hpp"
#include "mdlread.hpp"
#include "video.hpp"
#include "image.hpp"
#include "palette.hpp"
#include "linked.hpp"

#include "glread.hpp"
#include "gifread.hpp"
#include "ppmread.hpp"


main(int argc, char **argv)
{
  int i,j,k,t;
  image **im,*pic;
  palette *pal;
  image_init();
  set_mode(0x13,argc,argv);
  screen->clear();
  i=argc;
  im=(image **)jmalloc(sizeof(image *)*80,"gl_view image");
  i=1; t=1;
  while (i<argc)
  {
    pal=NULL;
    printf("reading %s",argv[i]);
    im[t]=read_pic(argv[i],pal);
    printf(" %d %d\n",im[t]->width(),im[t]->height());
    if (pal)
    { pal->load();
    delete pal;
    }
    if (*(argv[i]+strlen(argv[i])-3)=='c')
      t++;
    i++;
  }
  for (j=0;j<100;j++)
  {
    for (k=1;k<t;k++)
    {
      im[k]->put_image(screen,0,0);
    printf("put image\n");
      update_dirty(screen);
      screen->bar(0,0,im[k]->width(),im[k]->height(),0);
    }
    for (k=t-1;k>0;k--)
    {
      im[k]->put_image(screen,0,0);
        printf("put image\n");
      update_dirty(screen);
      screen->bar(0,0,im[k]->width(),im[k]->height(),0);
    }
  }


  close_graphics();
  return 0;
}
