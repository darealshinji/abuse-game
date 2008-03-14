#ifndef __AREA_HPP_
#define __AREA_HPP_
#include "macs.hpp"

class select_line
{
  public :
  ushort y,x1,x2;
  select_line *next;
  select_line(int Y, int X1, int X2, select_line *Next)
  { y=Y; x1=X1; x2=X2; next=Next; }
} ;

select_line *make_select_mask(ushort *points, int total);




#endif
