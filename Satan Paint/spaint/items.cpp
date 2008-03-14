#include "Include/items.h"
#include "Include/paint.h"
#include "Include/id.h"
#include "specs.hpp"

int particle::size()
{
    int t=0;
    for (int y=0;y<im->height();y++)
    {
        uchar *sl=im->scan_line(y);
        for (int x=0;x<im->width();x++,sl++)
            if (*sl) t++;
    }
    return 4+t*(4+1);
}

struct part
{
    short x,y;
    uchar color;
};

int part_comp(void *p1, void *p2)
{
    if ( ((part *)p1)->y<((part *)p2)->y) return -1;
    else if ( ((part *)p1)->y>((part *)p2)->y) return 1;
    else if ( ((part *)p1)->x<((part *)p2)->x) return -1;
    else if ( ((part *)p1)->x>((part *)p2)->x) return 1;
    else return 0;
}

int particle::write(jFILE *fp)
{
    int t=0;
    int y=0;
    for (;y<im->height();y++)
    {
        uchar *sl=im->scan_line(y);
        for (int x=0;x<im->width();x++,sl++)
            if (*sl) t++;
    }
    part *plist=(part *)jmalloc(sizeof(part)*t,"tmp particle list");
    
    int on=0;
    for (y=0;y<im->height();y++)
    {
        uchar *sl=im->scan_line(y);
        for (int x=0;x<im->width();x++,sl++)    
        {
            if (*sl) 
            {
                plist[on].x=x-cx;
                plist[on].y=y-cy;
                plist[on].color=*sl;
                on++;
            }
        }
    }
    
    fp->write_long(t);
    for (int i=0;i<t;i++)
    {
        fp->write_short(plist[i].x);
        fp->write_short(plist[i].y);
        fp->write_byte(plist[i].color);
    }
    
    jfree(plist);
    return 1;
}

particle::particle(spec_entry *e, jFILE *fp)
{
    fp->seek(e->offset,0);
    long t=fp->read_long();
    part *plist=(part *)jmalloc(sizeof(part)*t,"tmp particle list");
    long x1=100000,y1=100000,x2=-100000,y2=-100000;
    int i=0;
    for (;i<t;i++)
    {
        short x=fp->read_short();
        short y=fp->read_short();
        plist[i].x=x;
        plist[i].y=y;
        plist[i].color=fp->read_byte();
        if (x<x1) x1=x;
        if (y<y1) y1=y;
        if (x>x2) x2=x;
        if (y>y2) y2=y;
    }
    im=new image((x2-x1+1),(y2-y1+1));
    im->clear();
    int xa=-x1,ya=-y1;
    for (i=0;i<t;i++)
        *(im->scan_line(plist[i].y+ya)+plist[i].x+xa)=plist[i].color;
    jfree(plist);
    cx = (short) x1;
    cy = (short) y1;
    set_name(e->name);
}

int imitem::write(jFILE *fp)
{
    int y;
    
    fp->write_short(im->width());
    fp->write_short(im->height());
    for (y=0;y<im->height();y++)
        if (fp->write(im->scan_line(y),im->width())!=im->width())
            return 0;  
        return 1;  
}


void figure::scale(int nw, int nh)
{
    damage->resize(im->width(),im->height(),nw,nh);
    hit->resize(im->width(),im->height(),nw,nh);
    xcfg=xcfg*nw/im->width();
    advance=advance*nw/im->width();
    im->resize(nw,nh);
}


void foretile::scale(int nw, int nh)
{
    p->resize(im->width(),im->height(),nw,nh);
    im->resize(nw,nh);
}


void figure::get_fields(input_manager *im)
{
    int n,i;
    set_name(im->get(ITEM_NAME)->read());
    if (sscanf(im->get(FIGURE_DAMAGE)->read(),"%d",&n) && n>=0 && n<=255)
        hit_damage=n; else paint->info("Hit damage should be a number 0..255");
    
    if (sscanf(im->get(FIGURE_ADVANCE)->read(),"%d",&n) && n>=-128 && n<=127)
        advance=n; else paint->info("Advace should be a number -128..127");
    
    i=select_points(FIGURE_EDIT_XCENTER);
    xcfg=(unsigned char)paint->point_data[i][0];
    
    
    i=select_points(FIGURE_EDIT_HIT);
    set_hit(paint->total_points[i],paint->point_data[i]);
    
    
    i=select_points(FIGURE_EDIT_DAMAGE);
    set_damage(paint->total_points[i],paint->point_data[i]);
    
}


ifield *figure::fields(ifield *next)
{
    char dam[5];
    int x;
    
    x=select_points(FIGURE_EDIT_HIT);
    paint->total_points[x]=hit->tot;
    memcpy(paint->point_data[x],hit->data,(int)hit->tot*2);
    
    x=select_points(FIGURE_EDIT_DAMAGE);
    paint->total_points[x]=damage->tot;
    memcpy(paint->point_data[x],damage->data,(int)damage->tot*2);
    
    x=select_points(FIGURE_EDIT_XCENTER);
    paint->total_points[x]=1;
    paint->point_data[x][0]=xcfg;
    paint->point_data[x][1]=0;
    
    
    sprintf(dam,"%d",(int)hit_damage);
    return imitem::fields(new text_field(5,60,FIGURE_DAMAGE,"atk damage","****",dam,
        new text_field(5,80,FIGURE_ADVANCE,"frame advance","****",advance,
        new button(5,100,FIGURE_EDIT_HIT,"attack",
        new button(64,100,FIGURE_SHOW_HIT,"show",
        new button(120,100,FIGURE_HIDE_HIT,"hide",
        
        new button(5,120,FIGURE_EDIT_DAMAGE,"damage",
        new button(64,120,FIGURE_SHOW_DAMAGE,"show",
        new button(120,120,FIGURE_HIDE_DAMAGE,"hide",
        new button(5,140,FIGURE_EDIT_XCENTER,"X-CFG",	      
        new button(64,140,FIGURE_SHOW_XCENTER,"show",	      
        new button(120,140,FIGURE_HIDE_XCENTER,"hide",	      
        
        next))))))))))));
}

figure::figure(spec_entry *e, jFILE *fp)
{
    
    im=new image(e,fp);
    fp->read(&hit_damage,1);
    fp->read(&xcfg,1);
    
    if (e->type==SPEC_CHARACTER)
    {
        point_list tmp(fp);
        advance=0;
    }
    else  
        advance=fp->read_byte();
    
    damage=new point_list(fp); 
    hit=new point_list(fp);
    set_name(e->name);
}


int figure::size()
{
    return imitem::size()+
        1+1+1+       // hit, xcfg, advance
        hit->size()+
        damage->size(); 
}

int figure::write(jFILE *fp)
{
    if (!imitem::write(fp))
        return 0;  
    if (fp->write(&hit_damage,1)!=1)
        return 0;  
    if (fp->write(&xcfg,1)!=1)
        return 0;
    if (fp->write(&advance,1)!=1)
        return 0; 
    
    damage->save(fp);
    hit->save(fp);
    
    return 1;  
}


int foretile::size()
{
    return imitem::size()+2+1+
        p->size();
}

int foretile::write(jFILE *fp)
{
    if (!imitem::write(fp))
        return 0;
    
    fp->write_short(next_pic);       // 2
    fp->write(&damage,1);            // 1
    p->save(fp);                     // 1+tot*2
    return 1;
    
}

foretile::foretile(spec_entry *e, jFILE *fp)
{
    im=new image(e,fp);
    next_pic=fp->read_short();  
    fp->read(&damage,1);
    p=new point_list(fp);
    
    set_name(e->name);
}

void foretile::make_property_string(char *buffer)
{
    int i;
    char num[100];  
    sprintf(buffer,"type foretile damage %d, next %d, frame %d ",damage,next_pic,p->tot);  
    for (i=0;i<p->tot*2;i++) {
        sprintf(num,"%d",(int)p->data[i]);
        strcat(buffer,num);    
        if (i!=p->tot*2-1)
            strcat(buffer," ");
    }
    sprintf(num,", name %s",name());
    strcat(buffer,num);  
}

void figure::make_property_string(char *buffer)
{
    int i;
    char num[300];  
    sprintf(buffer,"type character xcfg %d , hit_damage %d , advance %d , ",xcfg,hit_damage,advance);  
    
    sprintf(num,"damage %d ",damage->tot);  
    strcat(buffer,num);  
    for (i=0;i<damage->tot*2;i++)
    {
        sprintf(num,"%d ",(int)damage->data[i]);
        strcat(buffer,num);    
    }
    
    
    sprintf(num,", attack %d ",hit->tot);  
    strcat(buffer,num);  
    for (i=0;i<hit->tot*2;i++)
    {
        sprintf(num,"%d ",(int)hit->data[i]);
        strcat(buffer,num);    
    }
    sprintf(num,", name %s",name());
    strcat(buffer,num);  
}


backtile::backtile(spec_entry *e, jFILE *fp)
{
    im=new image(e,fp);
    next_pic=fp->read_short();  
    set_name(e->name);
}

void foretile::get_fields(input_manager *im)
{
    int n,i;
    set_name(im->get(ITEM_NAME)->read());
    if (sscanf(im->get(ITEM_NEXT)->read(),"%d",&n))
    {    
        next_pic=n;
        if (n>=paint->num_images)
            paint->info("Warning : next_pic does not exsist!");
    } else
    {
        for (n=0,i=-1;n<paint->num_images && i<0;n++)
            if (!strcmp(paint->editim[n]->name(),im->get(ITEM_NEXT)->read())) 
                i=n;
            if (i>=0)
                next_pic=i;
            else  
                paint->info("Warning : next_pic does not exsist!");
    }
    if (sscanf(im->get(ITEM_DAMAGE)->read(),"%d",&n))
        damage=n; else paint->info("Damage should be a number 0..255"); 
    set_points(paint->total_points[paint->current_points],
        paint->point_data[paint->current_points]);
}

void backtile::get_fields(input_manager *im)
{
    int n,i;
    set_name(im->get(ITEM_NAME)->read());
    if (sscanf(im->get(ITEM_NEXT)->read(),"%d",&n))
    {    
        next_pic=n;
        if (n>=paint->num_images)
            paint->info("Warning : next_pic does not exsist!");
    } else
    {
        for (n=0,i=-1;n<paint->num_images && i<0;n++)
            if (!strcmp(paint->editim[n]->name(),im->get(ITEM_NEXT)->read())) 
                i=n;
            if (i>=0)
                next_pic=i;
            else  
                paint->info("Warning : next_pic does not exsist!");
    }
}

void imitem::get_fields(input_manager *im)
{
    set_name(im->get(ITEM_NAME)->read());
}

ifield *imitem::fields(ifield *next)
{
    char st1[10],st2[10];
    sprintf(st1,"%d",im->width());
    sprintf(st2,"%d",im->height());
    return new text_field(5,20,ITEM_NAME,"Name","*******************",name(),
        new button(5,40,ITEM_OK,"OK",
        new button(50,40,ITEM_CANCEL,"CANCEL",next)));
}

ifield *backtile::fields(ifield *next)
{
    char n[10];
    sprintf(n,"%d",(int)next_pic);
    return imitem::fields(new text_field(5,60,ITEM_NEXT,"next_pic","*****",n,
        next));
}

ifield *foretile::fields(ifield *next)
{
    char n[10],dam[5];
    paint->total_points[paint->current_points]=p->tot;
    memcpy(paint->point_data,p->data,(int)p->tot*2);
    
    sprintf(n,"%d",(int)next_pic);
    sprintf(dam,"%d",(int)damage);
    return imitem::fields(new text_field(5,60,ITEM_NEXT,"next_pic","*****",n,
        new text_field(5,80,ITEM_DAMAGE,"damage","****",dam,
        new button(5,100,FORETILE_EDIT_POINTS,"make frame",
        new button(150,80,FORETILE_SHOW_POINTS,"show",
        new button(150,100,FORETILE_HIDE_POINTS,"hide",
        next))))));
}

int backtile::size()
{
    return imitem::size()+2; 
}


int backtile::write(jFILE *fp)
{
    if (!imitem::write(fp))
        return 0;
    fp->write_short(next_pic);   
    return 1;  
}