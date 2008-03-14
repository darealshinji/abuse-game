#include "Include/paint.h"
#include "Include/pal.h"
#include "input.hpp"
#include "Include/id.h"
#include "specs.hpp"
#include "Include/toolinit.h"
#include "Include/pcommand.h"
#include <stdlib.h>

extern pm_cmd_handler *pch;
char pal_win_on=0;

static unsigned short calc_crc(unsigned char *buf, long len)
{
    unsigned char c1=0,c2=0;
    while (len)
    {
        len--;
        c1+=*buf; 
        c2+=c1;
        buf++;
    }
    return (c2<<8)|c1;
}

void paint_pal::invert_pal(int r, int g, int b)
{
    paint->add_undo(0,undo_palette,pal);
    int i;
    for (i=0;i<256;i++)
    {
        uchar rv,gv,bv;
        pal->get(i,rv,gv,bv);
        if (r) rv=255-rv;
        if (g) gv=255-gv;
        if (b) bv=255-bv;
        pal->set(i,rv,gv,bv);
    }
    pal->load();
    table_recalc=1;  
}

void paint_pal::tint_pal(int r, int g, int b, int five)
{ 
    paint->add_undo(0,undo_palette,pal);
    if (five)
    {
        color_filter *f=get_filter();
        for (int i=0;i<256;i++)
        {
            uchar rv,gv,bv;
            int nr,ng,nb;
            pal->get(i,rv,gv,bv);
            nr=(int)rv*r/100; if (nr>255) nr=255; else if (nr<0) nr=0;
            ng=(int)gv*g/100; if (ng>255) ng=255; else if (ng<0) ng=0;
            nb=(int)bv*b/100; if (nb>255) nb=255; else if (nb<0) nb=0;
            
            int c=f->lookup_color(nr>>3,ng>>3,nb>>3);      
            pal->set(i,pal->red(c),pal->green(c),pal->blue(c));
        }
    } 
    else
    {
        for (int i=0;i<256;i++)
        {
            uchar rv,gv,bv;
            int nr,ng,nb;
            pal->get(i,rv,gv,bv);
            nr=(int)rv*r/100; if (nr>255) nr=255; else if (nr<0) nr=0;
            ng=(int)gv*g/100; if (ng>255) ng=255; else if (ng<0) ng=0;
            nb=(int)bv*b/100; if (nb>255) nb=255; else if (nb<0) nb=0;
            pal->set(i,nr,ng,nb);
        }
    }
    
    pal->load();
    table_recalc=1;  
    tool_box->remap(pal);
}

void paint_pal::add_bs(int x)
{ 
    bs+=x; 
    if (bs<1) 
        bs=1; 
    if (bs>20) 
        bs=20; 
    if (pw) 
    { hide();
    show();
    }
}

void paint_pal::add(int x)
{
    int c=cur_col+x;
    while (c<0) c+=256;
    while (c>=256) c-=256;
    set_color(c);
}

void paint_pal::change_palette(palette *new_pal)
{  
    delete pal;
    pal=new_pal->copy();
    pal->load();
    table_recalc=1;  
    tool_box->remap(pal);
}


void paint_pal::change_color(int x, int r, int g, int b)
{  
    pal->set(x,r,g,b);
    pal->load();
    table_recalc=1;
    tool_box->remap(pal);
}


void paint_pal::load_stuff(char *filename)
{
    char cfname[100];  
    jFILE *fp=new jFILE(filename,"rb");
    if (fp->open_failure()) 
    {
        delete fp;
        return;
    }
    
    spec_directory sd(fp);
    sprintf(cfname,"%s-color-filter",filename);  
    if (sd.find(cfname))
    {
        table_recalc=0;
        if (color_table) delete color_table;    
        color_table=new color_filter(sd.find(cfname),fp);    
    }
    delete fp;
}

jwindow *filt_stat_win=NULL;

void filt_stat(int r_on)
{
    if (filt_stat_win)
    {
        char stat[100];
        sprintf(stat,"%d of 32",r_on);    
        filt_stat_win->clear();
        eh->font()->put_string(filt_stat_win->screen,WINDOW_FRAME_LEFT,WINDOW_FRAME_TOP,stat);
        eh->flush_screen();
    }
}

color_filter *paint_pal::get_filter(int recompute)
{
    if (!recompute) return color_table;  // if they are just checking to see if we have one,
    // don't recompute
    
    if (table_recalc)  // do a recalc if the user changed any of the colors
    {
        if (color_table)        // if there is an old copy of the filter, delete it
            delete color_table;
        
        ushort crc=calc_crc((uchar *)pal->addr(),768);
        char name[200];
        sprintf(name,"%s/%d.spt",getenv("HOME"),crc);
        jFILE fp(name,"rb");
        if (fp.open_failure())
        {
            // this will take a while, let the user know what is going on.   
            filt_stat_win=eh->new_window(10,10,130,eh->font()->height()+2,NULL,"Recalculating");
            eh->flush_screen();  
            color_table=new color_filter(pal,5,filt_stat);
            table_recalc=0;  
            eh->close_window(filt_stat_win);
            filt_stat_win=NULL;
            spec_directory sd;
            sd.add_by_hand(new spec_entry(SPEC_COLOR_TABLE,"color filter",NULL,32*32*32,0));
            sd.calc_offsets();
            jFILE *fp=sd.write(name);
            if (fp && !fp->open_failure())      
                fp->write(color_table->table(),32*32*32);
            if (fp) delete fp;
            sd.delete_entries();
        } else
        {
            spec_directory sd(&fp);
            color_table=new color_filter(sd.find(SPEC_COLOR_TABLE),&fp);
            table_recalc=0;  
        }
    }   
    return color_table;   
}


int paint_pal::set_color(int color)
{
    if (color!=cur_col)
    {
        if (cedit)
        { wm->close_window(cedit);
        cedit=NULL;
        }
        if (pw)
            draw_color(cur_col,0);
        cur_col=color;
        if (pw)
            draw_color(cur_col,1);
        paint->change_cursor();
    }
    return cur_col;
}

void paint_pal::draw_wigets(int clear)
{
    char st[50];
    int i,x=cedit->x1(),y=cedit->y1(),c[3];
    c[0]=pal->red(cur_col);
    c[1]=pal->green(cur_col);
    c[2]=pal->blue(cur_col);
    for (i=0;i<3;i++)
    {
        if (clear)
        {
            cedit->screen->bar(x+2+c[i],y+3,x+2+c[i]+6,y+8,wm->medium_color());
            cedit->screen->bar(x,cedit->y2()-wm->font()->height()-1,cedit->x2(),cedit->y2(),cur_col);
        }
        else
        {
            cedit->screen->wiget_bar(x+2+c[i],y+3,x+2+c[i]+6,y+8,
                wm->bright_color(),wm->medium_color(),wm->dark_color());
            sprintf(st,"%d %d %d (#%d)",c[0],c[1],c[2],cur_col);
            wm->font()->put_string(cedit->screen,100,
                cedit->y2()-wm->font()->height()-1,st);
        }
        y+=14+wm->font()->height();
    }
}

int paint_pal::handle_event(event &ev)
{
    int i,r,g,b,x,y;
    if (cedit) {
        if (ev.window!=cedit)
            return 0;
        if (ev.type==EV_CLOSE_WINDOW) {
            wm->close_window(cedit); 
            cedit=NULL;
        } 
        else if (ev.mouse_button) {
            x=ev.mouse_move.x-cedit->x-cedit->x1();
            y=ev.mouse_move.y-cedit->y-cedit->y1();
        
            r=pal->red(cur_col);  g=pal->green(cur_col);
            b=pal->blue(cur_col);
            if (x>=0 && y>=0 && x<=256+6 && y<=10) {
                if (x<3)
                    r=0;
                else
                    r=x-3;
                if (r>255)
                    r=255;
            }
            else {
                y-=14+wm->font()->height();
                if (x>=0 && y>=0 && x<=256+6 && y<=10)
                { if (x<3) g=0;
                else g=x-3;
                if (g>255) g=255;
                } else
                {
                    y-=14+wm->font()->height();
                    if (x>=0 && y>=0 && x<=256+6 && y<=10)
                    { if (x<3) b=0;
                    else b=x-3;
                    if (b>255) b=255;
                    }
                }
            }
            if ((unsigned) r != pal->red(cur_col) || (unsigned) g != pal->green(cur_col) || (unsigned) b != pal->blue(cur_col)) {
                paint->add_undo(0,undo_palette,pal);
            
                draw_wigets(1);
                pal->set(cur_col,r,g,b);
                pal->load();
                table_recalc=1;	
                draw_wigets(0);
            }
        }
    }
    else if (ev.window!=pw || !pw)
        return 0;
    else
    {
        if (ev.type==EV_KEY)
        {
            switch (ev.key)
            {
            case '-' : 
                { add_bs(-1);} break;
            case '+' :
                { add_bs(1); } break;
            case '[' :
                { add(-1); } break;
            case ']' :
                { add(1); } break;
            }		   
        }
        
        if (ev.type==EV_CLOSE_WINDOW) 
        { 
            wm->close_window(pw);
            pw=NULL;
            pal_win_on=0;
        }
        else {
            int x=ev.mouse_move.x-(pw->x+pw->x1()),
                y=ev.mouse_move.y-(pw->y+pw->y1());
            if (x>=0 && y>=0 && x<=pw->x2()-pw->x1() && y<=pw->y2()-pw->y1()) {
                if ((ev.mouse_button==1 || ev.mouse_button==2) && (y<=pw->y2()-pw->y1())) {
                    draw_color(cur_col,0);
                    cur_col=x/bs*PAL_H+y/bs;
                    if (cur_col<0) cur_col=0;
                    if (cur_col>255) cur_col=255;
                    draw_color(cur_col,1);
                    pch->notify_palette_click(cur_col);
                } 
                if (ev.mouse_button == 2 || ev.type==EV_MESSAGE && ev.message.id == PALETTE_EDIT_COLOR) {
                    if (ev.type==EV_MESSAGE && ev.message.id==PALETTE_EDIT_COLOR) {
                        ((button *)ev.message.data)->push();
                        pw->inm->redraw();
                    }
                    
                    x = ev.mouse_move.x + 2; 
                    if ((unsigned) x + 272 > xres)
                        x = xres-272;
                    y = ev.mouse_move.y + 2;
                    if ((unsigned) y + 90 > yres)
                        y = yres - 90;
                    
                    cedit=wm->new_window(x,y,256+4+6,80);
                    x=cedit->x1(); y=cedit->y1();
                    char *cs[]={"Red","Green","Blue"};
                    for (i=0;i<3;i++) {
                        cedit->screen->wiget_bar(x+1,y+1,x+(cedit->x2()-cedit->x1())-2,y+10,
                            wm->bright_color(),wm->medium_color(),wm->dark_color());
                        wm->font()->put_string(cedit->screen,x+120,y+12,cs[i]);
                        y+=14+wm->font()->height();
                    }
                    draw_wigets(0);
                }
            }    
        }
    }
    return 1;
}

void paint_pal::draw_color(int i, int selected)
{
    int red=0;
    pw->clip_in();
    if (selected)
    {
        pw->screen->bar(pw->x1()+(i/PAL_H)*bs-1,
            pw->y1()+(i%PAL_H)*bs-1,
            pw->x1()+(i/PAL_H)*bs+bs-1,
            pw->y1()+(i%PAL_H)*bs+bs-1,wm->bright_color());
        pw->screen->bar(pw->x1()+(i/PAL_H)*bs,
            pw->y1()+(i%PAL_H)*bs,
            pw->x1()+(i/PAL_H)*bs+bs-2,
            pw->y1()+(i%PAL_H)*bs+bs-2,wm->dark_color());  
        red=1;
    }
    else 
        pw->screen->bar(pw->x1()+(i/PAL_H)*bs+red-1,
        pw->y1()+(i%PAL_H)*bs+red-1,
        pw->x1()+(i/PAL_H)*bs+bs-1-red,
        pw->y1()+(i%PAL_H)*bs+bs-1-red,0);
    pw->screen->bar(pw->x1()+(i/PAL_H)*bs+red,
        pw->y1()+(i%PAL_H)*bs+red,
        pw->x1()+(i/PAL_H)*bs+bs-2-red,
        pw->y1()+(i%PAL_H)*bs+bs-2-red,i);
    pw->clip_out();
}



void paint_pal::create_window()
{
    int l=PAL_W*bs,h=PAL_H*bs,i;
    if (palx - 12 > (signed) xres || paly - 12 > (signed) yres)
        pw = wm->new_window(xres-l-8,-1,l,h,NULL);
    else
        pw=wm->new_window(palx,paly,l,h,NULL);    
    
    pal_win_on=1;
    pw->clip_in();
    pw->screen->bar(pw->x1(),pw->y1(),pw->x2(),pw->y2(),0);
    for (i=0;i<256;i++)
        pw->screen->bar(pw->x1()+(i/PAL_H)*bs,
        pw->y1()+(i%PAL_H)*bs,
        pw->x1()+(i/PAL_H)*bs+bs-2,
        pw->y1()+(i%PAL_H)*bs+bs-2,i);
    pw->inm->redraw();
    pw->clip_out();
}

paint_pal::paint_pal(window_manager *WM,  palette *Pal, int block_size)
{
    pal=Pal;
    cedit=NULL;
    wm=WM;
    bs=block_size;
    cur_col=0;
    table_recalc=1;
    color_table=NULL;  
    create_window();
    draw_color(0,1);
    palx=10000;
}


unsigned char default_palette[256*3] = {
    0, 0, 0, 159, 159, 159, 155, 155, 155, 151, 151, 151, 147, 147, 147,
    143, 143, 143, 139, 139, 139, 135, 135, 135, 131, 131, 131, 127, 127, 127,
    123, 123, 123, 119, 119, 119, 115, 115, 115, 111, 111, 111, 107, 107, 107,
    103, 103, 103, 99, 99, 99, 95, 95, 95, 91, 91, 91, 87, 87, 87,
    83, 83, 83, 79, 79, 79, 75, 75, 75, 71, 71, 71, 67, 67, 67,
    63, 63, 63, 59, 59, 59, 55, 55, 55, 51, 51, 51, 47, 47, 47,
    43, 43, 43, 39, 39, 39, 35, 35, 35, 31, 31, 31, 27, 27, 27,
    23, 23, 23, 19, 19, 19, 15, 15, 15, 11, 11, 11, 0, 0, 7,
    215, 187, 147, 199, 175, 139, 183, 163, 131, 171, 151, 123, 155, 139, 115,
    143, 127, 107, 131, 115, 99, 123, 107, 95, 115, 103, 91, 111, 99, 87,
    107, 95, 79, 103, 91, 75, 99, 87, 71, 95, 79, 67, 91, 75, 59,
    87, 71, 55, 83, 67, 51, 79, 63, 47, 71, 59, 43, 67, 55, 39,
    63, 51, 35, 59, 47, 35, 55, 39, 31, 51, 35, 27, 47, 35, 23,
    43, 31, 19, 39, 27, 19, 35, 23, 15, 31, 19, 11, 27, 15, 11,
    23, 11, 7, 19, 11, 7, 247, 255, 203, 243, 255, 163, 243, 255, 127,
    247, 255, 91, 243, 227, 75, 231, 195, 59, 219, 159, 47, 207, 123, 31,
    199, 91, 23, 195, 75, 19, 191, 63, 15, 187, 51, 7, 183, 35, 7,
    179, 23, 0, 179, 11, 0, 159, 7, 0, 143, 7, 0, 127, 7, 0,
    107, 0, 0, 91, 0, 0, 75, 0, 0, 55, 0, 0, 39, 0, 0,
    23, 0, 0, 135, 103, 103, 123, 83, 83, 111, 67, 67, 99, 55, 55,
    91, 43, 43, 83, 35, 35, 79, 27, 27, 71, 23, 23, 63, 19, 19,
    59, 15, 15, 51, 11, 11, 43, 7, 7, 35, 79, 171, 27, 63, 143,
    19, 51, 119, 15, 39, 95, 231, 167, 47, 203, 147, 43, 179, 131, 39,
    167, 123, 35, 159, 115, 35, 151, 107, 31, 139, 99, 27, 131, 91, 23,
    123, 87, 23, 115, 79, 19, 107, 71, 15, 99, 67, 15, 91, 59, 11,
    83, 55, 11, 75, 47, 7, 67, 43, 15, 171, 99, 55, 155, 87, 43,
    139, 79, 35, 123, 71, 27, 107, 59, 19, 91, 51, 15, 75, 43, 11,
    63, 35, 7, 131, 219, 63, 115, 195, 55, 103, 171, 47, 91, 151, 43,
    83, 143, 35, 79, 135, 31, 75, 131, 27, 71, 123, 23, 67, 115, 19,
    63, 111, 15, 59, 103, 11, 55, 95, 7, 51, 91, 7, 43, 83, 0,
    43, 75, 0, 39, 71, 0, 39, 71, 7, 35, 67, 7, 35, 59, 0,
    31, 55, 0, 27, 51, 0, 23, 43, 0, 19, 39, 0, 19, 35, 0,
    131, 151, 103, 115, 135, 91, 103, 119, 79, 91, 103, 71, 87, 99, 67,
    83, 95, 63, 75, 87, 55, 71, 83, 51, 71, 79, 51, 67, 75, 47,
    63, 71, 43, 55, 67, 39, 55, 63, 35, 51, 59, 31, 47, 55, 27,
    43, 51, 27, 39, 47, 23, 35, 43, 19, 31, 39, 19, 27, 35, 15,
    23, 31, 11, 19, 27, 11, 15, 23, 7, 15, 19, 7, 255, 0, 115,
    255, 0, 115, 255, 0, 115, 255, 235, 255, 219, 203, 219, 187, 171, 187,
    151, 139, 151, 119, 111, 119, 99, 91, 99, 87, 79, 87, 75, 67, 75,
    67, 59, 67, 63, 55, 63, 55, 47, 55, 51, 43, 51, 47, 39, 47,
    43, 35, 43, 39, 31, 39, 35, 27, 35, 31, 23, 31, 27, 19, 27,
    23, 15, 23, 19, 11, 19, 15, 11, 15, 215, 215, 231, 203, 203, 223,
    195, 195, 219, 183, 187, 215, 175, 179, 211, 163, 171, 207, 155, 163, 199,
    147, 155, 195, 139, 147, 191, 131, 143, 187, 123, 131, 171, 111, 119, 155,
    95, 107, 139, 83, 95, 123, 71, 83, 107, 55, 75, 95, 47, 67, 83,
    43, 59, 79, 39, 55, 71, 35, 51, 67, 31, 47, 63, 31, 43, 59,
    27, 35, 55, 23, 31, 51, 23, 27, 47, 19, 23, 43, 15, 19, 39,
    15, 19, 35, 11, 15, 31, 11, 11, 27, 7, 7, 23, 255, 0, 115,
    255, 0, 115, 255, 0, 115, 255, 0, 115, 255, 0, 115, 255, 0, 115,
    255, 0, 115, 255, 0, 115, 255, 0, 115, 255, 0, 115, 255, 0, 115,
    255, 0, 115, 255, 0, 115, 255, 0, 115, 255, 0, 115, 255, 0, 115,
    87, 87, 87};
