/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include <stdlib.h>

#include "pcxread.hpp"
#include "palette.hpp"
#include "image.hpp"

main(int argc, char **argv)
{
  image_init();
  
  if (argc<3)
  {    
    printf("Usage : %s xsize ysize filename [filename]\n",argv[0]);
    exit(0);    
  }
  
  while (argc>3)
  {
    argc--;
    
     palette *pal;
     image *im=read_PCX(argv[argc],pal);
     
     im->resize(atoi(argv[1]),atoi(argv[2])); 
     write_PCX(im,pal,argv[argc]);
      delete pal;    
     printf("  %s\n",argv[argc]);
          
     clear_errors();
  }  
  image_uninit();
  
}

