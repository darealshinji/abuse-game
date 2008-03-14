#include "Include/paint.h"
#include "filter.hpp"

void painter::invert(char *st)
{
    if (num_images) {
        add_undo(cur_image,undo_change);
        image *im=current_image();
        for (int y=0;y<im->height();y++) {
            uchar *sl=im->scan_line(y);
            for (int x=0;x<im->width();x++,sl++) {
                int r=pal->red(*sl),g=pal->green(*sl),b=pal->blue(*sl);
                int i=(unsigned char)((double) r*0.30+(double) g*0.59+(double)b*0.11);
                if (i<30)
                    *sl=(30-i)/2+52;
                else if (i<60)
                    *sl=((60-i)-30)/2+112;
                else if (i<90)
                    *sl=((90-i)-60)/2+160;
                else if (i<120)
                    *sl=((120-i)-90)/2+136;
                else 
                    *sl=((256-i)-120)/4+208;
                
                //	if (*sl)
                //	  r=255-pal->red(*sl)+20;
                //	if (r>255) r=255;
                //	int r=255-pal->red(*sl),g=0,b=0;
                //	 g=255-pal->green(*sl),b=255-pal->blue(*sl);
                //	*sl=pal->find_closest(r,g,b);
            }
        }
        draw_image(current_image(),1);
    }
}

void painter::bevel(char *st)
{
    int v;
    if (!sscanf(st,"%d",&v) || v<1 || v>255)
        info("bevel value should be 1..255");  
    else if (num_images) {
        add_undo(cur_image,undo_change);
        image *im=current_image();
        for (int y=1;y<im->height()-1;y++) {
            uchar *prev=im->scan_line(y-1)+1;
            uchar *sl=im->scan_line(y)+1;
            uchar *next=im->scan_line(y+1)+1;
            for (int x=1;x<im->width()-1;x++,sl++,prev++,next++) {
                if (*sl) {
                    int top=0,bottom=0;
                    if (*prev==0) top++;
                    if (*(prev-1)==0) top++;
                    if (*(sl-1)==0) top++;
                    if (*(next-1)==0) top++;
                    
                    if (*(prev+1)==0) bottom++;
                    if (*(sl+1)==0) bottom++;
                    if (*(next+1)==0) bottom++;
                    if (*(next)==0) bottom++;
                    
                    if (top || bottom) {
                        uchar r,g,b;
                        pal->get(*sl,r,g,b);
                        if (top>bottom) {
                            if (r+v>255) r=255; else r+=v;
                            if (g+v>255) g=255; else g+=v;
                            if (b+v>255) b=255; else b+=v;
                        }
                        else {
                            if (r-v<0) r=0; else r-=v;
                            if (g-v<0) g=0; else g-=v;
                            if (b-v<0) b=0; else b-=v;
                        }
                        *sl=pal->find_closest(r,g,b);
                    }
                }
            }
        }
        draw_image(current_image(),1);
    }
}

void painter::merge_gray(char *st)
{
    char fn[100];
    long w,h;
    if (sscanf(st,"%d%d%s",&w,&h,fn)!=3)
        info("ussage : merge_raw_gray width height filename");
    else {
        jFILE fp(fn,"rb");
        if (fp.open_failure())
            info("could not open file for reading",fn);
        else {
            palette *pal2=new palette(256);
            for (int c=0;c<256;c++)
                pal2->set(c,c,c,c);
            filter f(pal2,pal);
            delete pal2;
            
            image *im = new image((short) w, (short) h);
            for (int y=0;y<im->width();y++)
                fp.read(im->scan_line(y),im->width());
            //      f.apply(im);
            (*add_imitem()) = new imitem(im, fn);
        }
    }
}

void painter::write_color(char *filename)
{
    if (num_images) {
        jFILE fp(filename,"wb");
        if (fp.open_failure())
            info("could not open output file");
        else {
            image *im=current_image();
            for (int y=0;y<im->height();y++) {
                unsigned char *sl=im->scan_line(y);
                for (int x=0;x<im->width();x++,sl++) {
                    fp.write_byte(pal->red(*sl));       
                    fp.write_byte(pal->green(*sl));       
                    fp.write_byte(pal->blue(*sl));       
                }
            }
        }
    }  
}

void painter::write_gray(char *filename)
{
    if (num_images) {
        palette *p2=pal->copy();
        p2->make_black_white();
        jFILE fp(filename,"wb");
        if (fp.open_failure())
            info("could not open output file");
        else {
            image *im=current_image();
            for (int y=0;y<im->height();y++) {
                unsigned char *sl=im->scan_line(y);
                for (int x=0;x<im->width();x++,sl++)
                    fp.write_byte(p2->red(*sl));       
            }
        }
        delete p2;
    }  
}  

void blend_row(image *im, palette *pal, int r1, int r2, int percent)
{
    unsigned char *sl1=im->scan_line(r1);
    unsigned char *sl2=im->scan_line(r2);
    for (int x=0;x<im->width();x++,sl1++,sl2++) {
        unsigned char *p1=(unsigned char *)pal->addr()+(*sl1)*3;
        unsigned char *p2=(unsigned char *)pal->addr()+(*sl2)*3;
        
        int r=(p1[0]*(100-percent)+p2[0]*percent)/100;
        int g=(p1[1]*(100-percent)+p2[1]*percent)/100;
        int b=(p1[2]*(100-percent)+p2[2]*percent)/100;
        *sl1=pal->find_closest(r,g,b);
    }
}

void painter::repeat(char *st)
{
    int r;
    if (num_images) {
        if (sscanf(st,"%d",&r)) {
            add_undo(cur_image,undo_change);
            image *im=current_image();
            int doy=r;
            if (doy>im->height()/2) doy=im->height()/2;
            
            blend_row(im,pal,0,im->height()-1,50);


            blend_row(im,pal,5,4,25);
            blend_row(im,pal,4,3,30);
            blend_row(im,pal,3,2,35);
            blend_row(im,pal,2,1,40);
            blend_row(im,pal,1,0,45);


            blend_row(im,pal,im->height()-5,im->height()-6,25);
            blend_row(im,pal,im->height()-4,im->height()-5,30);
            blend_row(im,pal,im->height()-3,im->height()-4,35);
            blend_row(im,pal,im->height()-2,im->height()-3,40);
            blend_row(im,pal,im->height()-1,im->height()-2,45);


            draw_image(current_image(),1);
        }
        else
            info("ussage : repeat [# of rows]");
    }
}