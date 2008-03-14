#ifndef __CURSOR_HPP_
#define __CURSOR_HPP_

class cursor
{
  int cs_x1,cs_y1,cs_x2,cs_y2,mousex,mousey,drawn_mode;
public :
  cursor();
  void draw(int x, int y);
  void draw();
  void restore();
  void reset() { cs_x1=-10000; }
} ;


#endif
