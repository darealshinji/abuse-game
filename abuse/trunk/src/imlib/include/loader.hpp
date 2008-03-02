/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __LOADER_HPP_
#define __LOADER_HPP_
#include "image.hpp"
#include "palette.hpp"
#include "image24.hpp"


// supported graphics file formats so far..

enum graphics_type 
{ LOADER_not_supported,
  LOADER_mdl,	              // propritary
  LOADER_spe,		      // propritary
  LOADER_ppm,		      // portable pixmap
  LOADER_bmp8,		      // 8 bit color Microsoft Bitmap	
  LOADER_bmp24,		      // 24 bit color Microsoft Bitmap
  LOADER_pcx8,		      // 8 bit color PC paintbrush
  LOADER_pcx24,		      // 24 bit color PC paintbrush
  LOADER_xwd,		      // X-window dump format
  LOADER_pic,	              // Pic (used in Gl's)
  LOADER_lbm,                 // Delux Paint
  LOADER_targa                // 24 bit, converted to 8 on load though
} ;

graphics_type tell_file_type(char *filename);
int tell_color_size(char *filename);     // returns 8 or 24 
short load_any(char *filename, image **&images, 
	       palette *&pal, short &total_read, palette *ref_pal=0);
image24 *load_any24(char *filename);

#endif










