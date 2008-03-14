#include "image.hpp"
#include <math.h>
#include "macs.hpp"
#include "status.hpp"


int get_aliased_pixel(double x, double y, image *im, palette *pal)
{
  int cx1=(int)floor(x),cy1=(int)floor(y);
  uchar c5r,c5g,c5b,c6r,c6g,c6b;

  if (cx1==x)
  {
    if (cy1==y)
      return im->pixel(cx1,cy1);
    pal->get(im->pixel(cx1,cy1),c5r,c5g,c5b);
    pal->get(im->pixel(cx1,cy1+1),c6r,c6g,c6b);
  } else
  {
    uchar c1r,c1g,c1b,
          c2r,c2g,c2b,
          c3r,c3g,c3b,
          c4r,c4g,c4b;
    pal->get(im->pixel(cx1,cy1),c1r,c1g,c1b);   
    pal->get(im->pixel(cx1+1,cy1),c2r,c2g,c2b);    
    c5r=(uchar)((x-cx1)*(c2r-c1r)+c1r);
    c5g=(uchar)((x-cx1)*(c2g-c1g)+c1g);
    c5b=(uchar)((x-cx1)*(c2b-c1b)+c1b);   
    if (cy1==y)
      return pal->find_closest(c5r,c5g,c5b);

    pal->get(im->pixel(cx1,cy1+1),c3r,c3g,c3b);    
    pal->get(im->pixel(cx1+1,cy1+1),c4r,c4g,c4b);    
    c6r=(uchar)((x-cx1)*(c4r-c3r)+c3r);
    c6g=(uchar)((x-cx1)*(c4g-c3g)+c3g);
    c6b=(uchar)((x-cx1)*(c4b-c3b)+c3b);
  }


  uchar c7r,c7g,c7b;
  c7r=(uchar)((y-cy1)*(c6r-c5r)+c5r);
  c7g=(uchar)((y-cy1)*(c6g-c5g)+c5g);
  c7b=(uchar)((y-cy1)*(c6b-c5b)+c5b);
  
  return pal->find_closest(c7r,c7g,c7b);
}



image *aliased_scale(image *im, int nw, int nh, palette *pal)
{
  if (nw<=0 || nh<=0) return im->copy();
  image *dest=new image(nw,nh);  
  double source_x,source_y,
         source_xstep=(im->width()/(double)nw),
         source_ystep=(im->height()/(double)nh);
  int dest_x,dest_y;
  
  int w=im->width(),h=im->height();
  stat_man->push("Aliased scale",NULL);

  for (dest_y=0,source_y=0;dest_y<nh;dest_y++,source_y+=source_ystep)
  {
    stat_man->update(dest_y*100/nh);
    uchar *dest_line=dest->scan_line(dest_y);
    for (dest_x=0,source_x=0;dest_x<nw;dest_x++,source_x+=source_xstep,dest_line++)
      *dest_line=get_aliased_pixel(source_x+1 >= w ? w-1 : source_x ,
				   source_y+1 >= h ? h-1 : source_y,im,pal);
      
  }
  stat_man->pop();
  return dest;
}


