#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "image.hpp"
#include "palette.hpp"
#include "config.hpp"
#include "crc.hpp"

#define TTINTS 9
extern uchar *tints[TTINTS];
extern unsigned char *white_light,*white_light_initial,*green_light,*trans_table;
extern short ambient_ramp;
#define REVERSE_GREEN_TINT 8

extern short shutdown_lighting_value,shutdown_lighting;

class light_source
{
  public :
  int32_t type,x,xshift,y,yshift;
  int32_t outer_radius,mul_div,inner_radius;

  int32_t x1,y1,x2,y2;
  char known;
  light_source *next;

  void calc_range();
  light_source(char Type, int32_t X, int32_t Y, int32_t Inner_radius, int32_t Outer_radius, 
	       int32_t Xshift, int32_t Yshift,
	       light_source *Next);
  light_source *copy();
} ;

class light_patch
{
  public :
  int32_t total,x1,y1,x2,y2;
  light_source **lights;  
  light_patch *next;
  light_patch(int32_t X1, int32_t Y1, int32_t X2, int32_t Y2, light_patch *Next)
  { 
    x1=X1; y1=Y1; x2=X2; y2=Y2;
    next=Next;
    total=0;
    lights=NULL;
  }
  void add_light(int32_t X1, int32_t Y1, int32_t X2, int32_t Y2, light_source *who);
  light_patch *copy(light_patch *Next);
  ~light_patch() { if (total) jfree(lights); }
} ;

void delete_all_lights();
void delete_light(light_source *which);
light_source *add_light_source(char type, int32_t x, int32_t y, 
			       int32_t inner, int32_t outer, int32_t xshift, int32_t yshift);

void add_light_spec(spec_directory *sd, char *level_name);
void write_lights(bFILE *fp);
void read_lights(spec_directory *sd, bFILE *fp, char *level_name);


void delete_patch_list(light_patch *first);
light_patch *find_patch(int screenx, int screeny, light_patch *list);
int calc_light_value(int32_t x, int32_t y, light_patch *which);			       
void light_screen(image *sc, int32_t screenx, int32_t screeny, uchar *light_lookup, ushort ambient);
void double_light_screen(image *sc, int32_t screenx, int32_t screeny, uchar *light_lookup, ushort ambient,
			 image *out, int32_t out_x, int32_t out_y);

void calc_light_table(palette *pal);
extern light_source *first_light_source;
extern int light_detail;

extern int32_t light_to_number(light_source *l);
extern light_source *number_to_light(int32_t x);

#endif
