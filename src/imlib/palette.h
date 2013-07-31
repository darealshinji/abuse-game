/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef _PALETTE_H_
#define _PALETTE_H_

#include "linked.h"
#include "specs.h"
#define COLOR_BITS 6    // On PC-6, most others -8
#define COLOR_SHIFT (8-COLOR_BITS)
#define MAX_COLOR ((1<<COLOR_BITS)-1)
#define RED3(x)  (unsigned char) ((((int)x&(7<<5))>>5)*(int)255/(int)7)
#define GREEN3(x) (unsigned char) (((x&(7<<2))>>2)*(int)255/(int)7)
#define BLUE2(x) (unsigned char) ((x&3)*(int)255/(int)3)


class Palette : public linked_node
{
public:
  Palette(int number_colors=256);
  Palette(SpecEntry *e, bFILE *fp);
  Palette(bFILE *fp);

  Palette *Copy() const;

  int Count() const { return m_colors.Count(); }
  u8vec3 *Data() { return m_colors.Data(); }
  u8vec3 const *Data() const { return m_colors.Data(); }

  u8vec3 GetColor(int x) const;
  uint8_t GetGray(int x) const;
  void SetColor(int x, u8vec3 color);

  int FindColor(u8vec3 color) const;
  int FindClosest(u8vec3 color) const;
  int FindBrightest(int all = 0) const;
  int FindDarkest(int all = 0, int noblack = 1) const;

  void FadeTo(int total_fades, int fade_on, u8vec3 dest);

private:
  Array<u8vec3> m_colors;
  Array<uint8_t> m_used;
  int bg;

public :
  uint32_t getquad(int x);
  void shift(int amount);
  void load();
  void load_nice();

  void defaults();
  void set_rgbs();
  void make_black_white();
  void black_white();

  int used(int color_num);
  void set_all_used();
  void set_all_unused();
  int write(bFILE *fp);
  int size();
};

class quant_node : public linked_node
{
  quant_node *padre;
public :
  unsigned tot;
  quant_node *children[8];
  int is_leaf() { return children[0]==this; }
  void be_childish() { children[0]=this; }
  quant_node *father() { return padre; }
  quant_node(int level, quant_node *dad,
    unsigned char r=0, unsigned char g=0, unsigned char b=0);
  void total(int &tnodes, int &tr, int &tg, int &tb);
//  void prune();
  void set(int r,int g,int b) { red=r; green=g; blue=b; }
  unsigned char red,green,blue;
  ~quant_node();
} ;


class quant_palette
{
  linked_list level[8];
  quant_node *root;
  int nc,mx;
  void prune();
  void re_delete(quant_node *who, int lev);
public :
  quant_palette(int max_colors=256);
  ~quant_palette();
} ;

Palette *last_loaded();
#endif

