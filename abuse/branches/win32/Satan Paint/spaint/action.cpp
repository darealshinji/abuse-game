#include "Include/action.h"
#include "Include/paint.h"
#include "Include/id.h"

cursor::cursor()
{ 
    reset();
}

void cursor::draw()
{
    reset();
    if (paint->num_images)
    {
        drawn_mode=paint->state;
        switch (paint->state)
        {
        case EDIT_MODE :
            {
                cs_x1=mousex-paint->brush_size+1;
                if (cs_x1<0) cs_x1=0;
                cs_x2=mousex+paint->brush_size-1;
                if (cs_x2>=paint->current_image()->width()) cs_x2=paint->current_image()->width()-1;
                cs_y1=mousey-paint->brush_size+1;
                if (cs_y1<0) cs_y1=0;
                cs_y2=mousey+paint->brush_size-1;
                if (cs_y2>=paint->current_image()->height()) cs_y2=paint->current_image()->height()-1;
                if (cs_x2<cs_x1 || cs_y2<cs_y1) 
                {
                    reset();            // mark the cursor as undrawn!
                    return ;
                }
                
                for (int y=cs_y1;y<=cs_y2;y++)
                    for (int x=cs_x1;x<=cs_x2;x++)
                        paint->draw_image_pixel(x,y,paint->current_color());
                    
            } break;
        case MODE_ZOOM_IN :
            {
                short ix1,iy1,ix2,iy2;
                paint->image_area(ix1,iy1,ix2,iy2);	
                cs_x1=mousex-((ix2-ix1)/(paint->zoom+2))/2;
                cs_x2=mousex+((ix2-ix1)/(paint->zoom+2))/2;
                cs_y1=mousey-((iy2-iy1)/(paint->zoom+2))/2;
                cs_y2=mousey+((iy2-iy1)/(paint->zoom+2))/2;
                int c=paint->bright_color;
                image *im=paint->current_image();
                paint->draw_area(im,cs_x1,cs_y1,cs_x1+5,cs_y1,c);
                paint->draw_area(im,cs_x1,cs_y1+1,cs_x1,cs_y1+5,c);
                paint->draw_area(im,cs_x2-5,cs_y1,cs_x2,cs_y1,c);
                paint->draw_area(im,cs_x2,cs_y1+1,cs_x2,cs_y1+5,c);
                paint->draw_area(im,cs_x2-5,cs_y2,cs_x2,cs_y2,c);
                paint->draw_area(im,cs_x2,cs_y2-5,cs_x2,cs_y2-1,c);
                paint->draw_area(im,cs_x1,cs_y2,cs_x1+5,cs_y2,c);
                paint->draw_area(im,cs_x1,cs_y2-5,cs_x1,cs_y2-1,c);
                
            } break;
        case MODE_IMAGE_DRAW :
            {
                image *i=paint->select_buffer;
                if (i)
                {	  
                    int w=i->width(),h=i->height();
                    cs_x1=mousex-w/2;
                    cs_x2=cs_x1+w-1;
                    cs_y1=mousey-h/2;
                    cs_y2=cs_y1+h-1;
                    
                    paint->draw_image_area(paint->current_image(),i,cs_x1,cs_y1,0,0,i->width()-1,i->height()-1,1);
                }
            } break;
        }
    }  
}

void cursor::draw(int x, int y)
{
    mousex=x;
    mousey=y;
    draw();
}


void cursor::restore()
{
    if (cs_x1!=-10000)
    {
        switch (drawn_mode)
        {
        case MODE_ZOOM_IN :
            {
                int c=-1;
                image *im=paint->current_image();
                
                
                paint->draw_area(im,cs_x1,cs_y1,cs_x1+5,cs_y1,c);
                paint->draw_area(im,cs_x1,cs_y1+1,cs_x1,cs_y1+5,c);
                paint->draw_area(im,cs_x2-5,cs_y1,cs_x2,cs_y1,c);
                paint->draw_area(im,cs_x2,cs_y1+1,cs_x2,cs_y1+5,c);
                paint->draw_area(im,cs_x2-5,cs_y2,cs_x2,cs_y2,c);
                paint->draw_area(im,cs_x2,cs_y2-5,cs_x2,cs_y2-1,c);
                paint->draw_area(im,cs_x1,cs_y2,cs_x1+5,cs_y2,c);
                paint->draw_area(im,cs_x1,cs_y2-5,cs_x1,cs_y2-1,c);
                
            } break;
        default :
            paint->draw_area(paint->current_image(),cs_x1,cs_y1,cs_x2,cs_y2);
        }
    }
}