#ifndef __POINTS_HPP_
#define __POINTS_HPP_
#include <stdio.h>
#include <stdlib.h>
#include "specs.hpp"

class point_list
{
public :
  unsigned char tot;
  unsigned char *data;
  point_list(unsigned char how_many, unsigned char *Data);
  point_list() { tot=0; data=NULL; }
  point_list(jFILE *fp);
  void save(jFILE *fp);
  point_list *copy() { return new point_list(tot,data); }
  void resize(int ow, int oh, int nw, int nh);
  int size();
  ~point_list() { if (tot) jfree(data); }
} ;

#endif
