#ifndef __W32_VIDEO_HPP_
#define __W32_VIDEO_HPP_
#include "system.h"

#include "image.hpp"

extern unsigned char current_background;
extern unsigned int xres, yres;
extern int xoff, yoff;
extern image *screen;

extern LPDIRECTDRAW7 g_pDD;
extern PALETTEENTRY color_array[256];
extern HPEN hPen;

void set_mode(int mode, int argc=0, char **argv=NULL);
void close_graphics();
void fill_image(image *im, int x1, int y1, int x2, int y2);
void update_dirty(image *im, int xoff=0, int yoff=0);
void Finish_Frame();
void put_image(image *im, int x, int y);
void TransparentBlit(image *im, int x, int y, int color);

void clear_put_image(image *im, int x, int y);
int get_vmode();
void ToggleWindowedMode();

#endif
