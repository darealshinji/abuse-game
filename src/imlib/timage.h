/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __TIMAGE_HPP_
#define __TIMAGE_HPP_

#include "image.h"
#include "palette.h"
#include "filter.h"

/* data is stored in the following format

  skip amount, data size, data  // no scan line wraps allowed

*/


class trans_image       // transpernet image
{
  unsigned char *data;
  short w,h;

public :
  short height() { return h; }
  short width() { return w; }
  trans_image(image *im, char const *name);  // name has no meaning if MEM_CHECK is off
  void put_image(image *screen, int x, int y);   // always transparent

  // if screen x & y offset already calculated save a mul
  // and no clipping, but fast use this
  void put_image_offseted(image *screen, uint8_t *s_off);
  void put_image_filled(image *screen, int x, int y,
            uint8_t fill_color);
  void put_fade(image *screen, int x, int y,
               int frame_on, int total_frames,
               color_filter *f, palette *pal);
  void put_fade_tint(image *screen, int x, int y,
             int frame_on, int total_frames,
             uint8_t *tint,
             color_filter *f, palette *pal);
  void put_color(image *screen, int x, int y, int color);

  void put_blend16(image *screen, image *blend, int x, int y,
               int blendx, int blendy, int blend_amount, color_filter *f, palette *pal);
  void put_double_remaped(image *screen, int x, int y, unsigned char *remap, unsigned char *remap2);
  void put_remaped(image *screen, int x, int y, unsigned char *remap);
  void put_predator(image *screen, int x, int y);
  void put_scan_line(image *screen, int x, int y, int line);   // always transparent
  unsigned char *t_data() { return data; }
  int size();
  image *make_image();
  ~trans_image() { free(data); }

private:
  uint8_t *ClipToLine(image *screen, int x1, int y1, int x2, int y2,
                      int x, int &y, int &ysteps);
} ;


#endif





