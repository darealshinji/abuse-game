#ifndef _IMGAE_HPP_
#define _IMGAE_HPP_
#include <stdlib.h>
#include "linked.hpp"
#include "palette.hpp"
#include "system.h"
#include "specs.hpp"
#define MAX_DIRTY 200
#define Inew(pointer,type); { make_block(sizeof(type)); pointer=new type; }

extern char const *imerr_messages[];  // correspond to imERRORS
#define imREAD_ERROR 	       1
#define imINCORRECT_FILETYPE   2
#define imFILE_CORRUPTED       3
#define imFILE_NOT_FOUND       4
#define imMEMORY_ERROR         5
#define imNOT_SUPPORTED        6
#define imWRITE_ERROR          7
#define imMAX_ERROR 	       7

int16_t current_error();
void clear_errors();
void set_error(int16_t x);
int16_t last_error();
void make_block(size_t size);
void image_init();
void image_uninit();
extern linked_list image_list;

typedef struct image_color_t
{
	uint16_t r;
	uint16_t g;
	uint16_t b;
} image_color;

class filter;


class dirty_rect : public linked_node
{
public :
  int16_t dx1,dy1,dx2,dy2;
  dirty_rect(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
  { dx1=x1; dy1=y1; dx2=x2; dy2=y2; 
    if (x2<x1 || y2<y1) 
      printf("add inccorect dirty\n");
  }
  virtual int16_t compare(void *n1, int16_t field)
  { return ((dirty_rect *)n1)->dy1>dy1; }
} ;

class image_descriptor
{
  int16_t l,h;
  int16_t clipx1, clipy1, clipx2, clipy2;
public :  
  uint8_t keep_dirt,
	  static_mem;      // if this flag is set then don't free memory on exit
  
  linked_list dirties;
  void *extended_descriptor;              // type depends on current system

  image_descriptor(int16_t length, int16_t height,
		  int keep_dirties=1, int static_memory=0);
  int16_t bound_x1(int16_t x1)  { return x1<clipx1 ? clipx1 : x1; }
  int16_t bound_y1(int16_t y1)  { return y1<clipy1 ? clipy1 : y1; }
  int16_t bound_x2(int16_t x2)  { return x2>clipx2 ? clipx2 : x2; }
  int16_t bound_y2(int16_t y2)  { return y2>clipy2 ? clipy2 : y2; }
  int16_t x1_clip() { return clipx1; }
  int16_t y1_clip() { return clipy1; }
  int16_t x2_clip() { return clipx2; }
  int16_t y2_clip() { return clipy2; }
  void dirty_area(int16_t x1, int16_t y1, int16_t x2, int16_t y2) { ;}
  void clean_area(int16_t x1, int16_t y1, int16_t x2, int16_t y2) { ; }
  void clear_dirties();
  int16_t get_dirty_area(int16_t &x1, int16_t &y1, int16_t &x2, int16_t &y2) { return 0; }
  void get_clip(int16_t &x1, int16_t &y1, int16_t &x2, int16_t &y2)
    { x1=clipx1; y1=clipy1; x2=clipx2; y2=clipy2; }
  void set_clip(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
    { if (x2<x1) x2=x1;
      if (y2<y1) y2=y1;
      if (x1<0) clipx1=0; else clipx1=x1;
      if (y1<0) clipy1=0; else clipy1=y1;
      if (x2>=l) clipx2=l-1; else clipx2=x2;
      if (y2>=h) clipy2=h-1; else clipy2=y2;
    }
  void reduce_dirties();
  void add_dirty(int x1, int y1, int x2, int y2);
  void delete_dirty(int x1, int y1, int x2, int y2);
  void resize(int16_t length, int16_t height)
   { l=length; h=height; clipx1=0; clipy1=0; clipx2=l-1; clipy2=h-1; }
} ;

class image : public linked_node
{ 
  uint8_t *data;
  int16_t w,h;
  void make_page(int16_t width, int16_t height, uint8_t *page_buffer);
  void delete_page();
public :
  image_descriptor *special;
  image(spec_entry *e, bFILE *fp);
  image(bFILE *fp);
  image(int16_t width, int16_t height,                 // required
	uint8_t *page_buffer=NULL,
	int16_t create_descriptor=0);        // 0=no, 1=yes, 2=yes & keep dirties
  uint8_t  pixel              (int16_t x, int16_t y);
  void     putpixel           (int16_t x, int16_t y, char color);
  uint8_t *scan_line          (int16_t y) { return data+y*w; }
  uint8_t *next_line          (int16_t lasty, uint8_t *last_scan) 
                                    { return last_scan+w; }          
  int32_t  total_pixels       (uint8_t background=0);
  image    *copy               ();    // makes a copy of an image
  void     clear              (int16_t color=-1);  // -1 is background color
  void     to_24bit           (palette &pal);
  int16_t  width              () { return (int16_t)w; }
  int16_t  height             () { return (int16_t)h; }
  void     scroll             (int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t xd, int16_t yd);
  void     fill_image         (image *screen, int16_t x1, int16_t y1, int16_t x2, int16_t y2, 
			      int16_t align=1);
  void     put_image          (image *screen, int16_t x, int16_t y, char transparent=0);
  void     put_part           (image *screen, int16_t x, int16_t y, int16_t x1, int16_t y1, 
				     int16_t x2, int16_t y2, char transparent=0);
  void     put_part_xrev      (image *screen, int16_t x, int16_t y, int16_t x1, int16_t y1, 
				     int16_t x2, int16_t y2, char transparent=0);
  void     put_part_masked    (image *screen, image *mask, int16_t x, int16_t y, 
				     int16_t maskx, int16_t masky, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
  image    *copy_part_dithered (int16_t x1, int16_t y1, int16_t x2, int16_t y2);
  void     bar                (int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
  void     xor_bar            (int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
  void 	   wiget_bar          (int16_t x1, int16_t y1, int16_t x2, int16_t y2, 
				     uint8_t light, uint8_t med, uint8_t dark);
  void     line               (int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
  void     rectangle          (int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
  void     burn_led           (int16_t x, int16_t y, int32_t num, int16_t color, int16_t scale=1);
  void     set_clip           (int16_t x1, int16_t y1, int16_t x2, int16_t y2);
  void     get_clip           (int16_t &x1,int16_t &y1,int16_t &x2,int16_t &y2);
  void     in_clip            (int16_t x1, int16_t y1, int16_t x2, int16_t y2);

  void     dirt_off           () { if (special && special->keep_dirt) special->keep_dirt=0; }
  void     dirt_on            () { if (special) special->keep_dirt=1; }

  void     add_dirty          (int x1, int y1, int x2, int y2) 
                                    { if (special) special->add_dirty(x1,y1,x2,y2); }
  void     delete_dirty       (int x1, int y1, int x2, int y2) 
                                    { if (special) special->delete_dirty(x1,y1,x2,y2); }
  void     clear_dirties      () { if (special) special->clear_dirties(); }
  void     dither             (palette *pal); // use a b&w palette!
  void     resize             (int16_t new_width, int16_t new_height);
  void     change_size        (int16_t new_width, int16_t new_height, uint8_t *page=NULL);
  void     flood_fill         (int16_t x, int16_t y, uint8_t color);
  image    *create_smooth     (int16_t smoothness=1); // 0 no smoothness
  void     unpack_scanline    (int16_t line, char bitsperpixel=1);
  uint8_t  brightest_color    (palette *pal);
  void     flip_x	      ();
  void     flip_y             ();
  void     make_color         (uint8_t color);
  uint8_t  darkest_color      (palette *pal, int16_t noblack=0);

  ~image();
} ;


class image_controller
{
public :
  image_controller() { image_init(); }
  ~image_controller()
  { 
     image_uninit(); 
  }
} ;



#endif








