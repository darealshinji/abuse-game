/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

/* FIXME: this file is unused */

#ifndef __MORPH_HPP_
#define __MORPH_HPP_

#include <stdio.h>

#include "image.h"
#include "palette.h"
#include "macs.h"
#include "specs.h"
#include "filter.h"


struct morph_point8
{
  unsigned char x1,y1,x2,y2;
  unsigned char start_color,end_color;
} ;

struct morph_point16
{
  unsigned short x1,y1,x2,y2;
  unsigned char start_color,end_color;
} ;

struct morph_patch                // patch a frame of animation
{
  unsigned short patches;
  unsigned char *patch_data;       // x,y,color
} ;

class jmorph
{
protected :
  unsigned char small;
  void *p;                  // points to a 8 or 16 struct
  long total;
  unsigned short w[2],h[2];
public :
  int total_points() { return total; }
  morph_point8 *small_points() { return (morph_point8 *)p; }
  jmorph(spec_entry *e, FILE *fp);
  jmorph(image *i1, image *hint1, image *i2, image *hint2, int aneal_steps);
  void show_frame(image *screen, int x, int y, int frames, int frame_on,
             color_filter *fli, palette *pal);
  void show_8(image *screen, int x, int y, int frame_on, color_filter *fli, palette *pal);
  void show_24frame(unsigned char *screen, int width, int height,
                    int x, int y, int frames, int frame_on, palette *pal);
  void show_step_frame(image *screen, int x, int y, int frame_on, color_filter *fli, palette *pal);
  int bound_x1(int which) { return 0; }
  int bound_y1(int which) { return 0; }
  int bound_x2(int which) { return w[which]; }
  int bound_y2(int which) { return h[which]; }
  int write(FILE *fp);
  void add_filler(int frames);
  int small_morph() { return small; }
  ~jmorph() { free(p); }
} ;



class patched_morph : public jmorph
{
public :
  morph_patch *pats;
  unsigned short patches;
  patched_morph(spec_entry *e, FILE *fp);
  patched_morph(image *i1, image *hint1, image *i2, image *hint2, int aneal_steps,
            color_filter *fli, palette *pal, int frames);
  void show_frame(image *screen, int x, int y, int frame_on, color_filter *fli, palette *pal);
  void show_8(image *screen, int x, int y, int frame_on, color_filter *fli, palette *pal);
  ~patched_morph() { free(pats); }

} ;

struct step_struct
{
  unsigned short x,y,r,g,b;
  short dx,dy,dr,dg,db;
} ;

class step_morph
{
  int total;
  step_struct *points;
  int frame_on,dir,face_dir;
  patched_morph *pm;
public :
  step_morph(patched_morph *mor, palette *pal, int frame_direction, int face_direction);
  void show_frame(image *screen, int x, int y,  color_filter *fli);
  void reverse_direction();
  ~step_morph() { free(points); }
} ;


#endif






