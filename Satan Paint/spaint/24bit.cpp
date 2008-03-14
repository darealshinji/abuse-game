#include "Include/24bit.h"

rgb_scan::rgb_scan(rgb_scan *copy)
{
    x1=copy->x1;
    x2=copy->x2;
    data=(uchar *)jmalloc((x2-x1+1)*3,"rgb_scan");
    memcpy(data,copy->data,(x2-x1+1)*3);
    next=NULL;
}

rgb_area::~rgb_area()
{
    for (int Y=y1;Y<y2;Y++)
    {
        while (y[Y-y1])
        {
            rgb_scan *p=y[Y-y1];
            y[Y-y1]=y[Y-y1]->next;
            delete p;
        }
    }
    jfree(y);
}

void rgb_scan::render(uchar *sl, color_filter *f)
{
    int x=x1;
    sl+=x;
    uchar *d=data;
    while (x<=x2)
    {
        *sl=f->lookup_color(d[0]>>3,d[1]>>3,d[2]>>3);
        d+=3;
        x++;
        sl++;
    }
}

rgb_scan::rgb_scan(image *im, palette *pal, int y, int X1, int X2, rgb_scan *Next)
{
    uchar *p=(uchar *)pal->addr(),*pp;
    
    x1=X1;
    x2=X2;
    uchar *sl=im->scan_line(y)+x1;
    
    data=(uchar *)jmalloc((x2-x1+1)*3,"rgb_line");
    uchar *d=data;
    while (X1<=X2)
    {
        pp=p+((*sl)*3); sl++;
        *d=*pp;  d++; pp++;
        *d=*pp;  d++; pp++;
        *d=*pp;  d++; pp++;
        X1++;
    }
    next=Next;
}

void rgb_area::make_lines(select_line *first, image *im, palette *pal)
{
    y1=10000,y2=0;
    select_line *p=first;
    for (;p;p=p->next)
    {
        if (p->y<y1) y1=p->y;
        if (p->y>y2) y2=p->y;
    }
    y=(rgb_scan **)jmalloc(sizeof(rgb_scan *)*(y2-y1+1),"rbg_scans");
    memset(y,0,sizeof(rgb_scan *)*(y2-y1+1));
    
    for (p=first;p;p=p->next) 
        y[p->y-y1]=new rgb_scan(im,pal,p->y,p->x1,p->x2,y[p->y-y1]);  
}

void rgb_area::put(image *im, color_filter *f)
{
    uchar *sl;
    for (int Y=y1;Y<=y2;Y++)
    {
        if (y[Y-y1])
        {
            sl=im->scan_line(Y);
            for (rgb_scan *rs=y[Y-y1];rs;rs=rs->next)      
                rs->render(sl,f);      
        }
    }
}

void rgb_area::apply()
{
    for (int z=y1;z<=y2;z++)
    {
        for (rgb_scan *p=y[z-y1];p;p=p->next)
            apply_scan(p);
    }
}


rgb_area::rgb_area(rgb_area *copy)
{
    y1=copy->y1;
    y2=copy->y2;
    y=(rgb_scan **)jmalloc(sizeof(rgb_scan *)*(y2-y1+1),"rbg_scans");  
    for (int i=y1;i<=y2;i++)
    {
        if (copy->y[i-y1])
        {
            rgb_scan *last=NULL;
            for (rgb_scan *f=copy->y[i-y1];f;f=f->next)
            {
                rgb_scan *p=f->copy();
                if (!last)
                    y[i-y1]=p;
                else
                    last->next=p;
                last=p;
            }
        } else y[i-y1]=NULL;
    }  
}


void rgb_lighting::apply_scan(rgb_scan *rs)
{
    uchar *d=rs->data;
    uchar rmap[256],gmap[256],bmap[256];
    for (int i=0;i<256;i++)
    {
        if (r>100)
        {
            int nr=(i)+(r-100)*255/100;
            rmap[i]=nr>255 ? 255 : nr;
        } else
        {
            int nr=(i)-(100-r)*255/100;
            rmap[i]=nr<0 ? 0 : nr; 
        }
        if (g>100)
        {
            int ng=(i)+(g-100)*255/100;
            gmap[i]=ng>255 ? 255 : ng;
        } else
        {
            int ng=(i)-(100-g)*255/100;
            gmap[i]=ng<0 ? 0 : ng; 
        }
        
        if (b>100)
        {
            int nb=(i)+(b-100)*255/100;
            bmap[i]=nb>255 ? 255 : nb;
        } else
        {
            int nb=(i)-(100-b)*255/100;
            bmap[i]=nb<0 ? 0 : nb; 
        }
    }
    
    for (int x=rs->x1;x<=rs->x2;x++)
    {
        *d=rmap[*d]; d++;
        *d=gmap[*d]; d++;
        *d=bmap[*d]; d++;
    }
}


void rgb_gravel::apply_scan(rgb_scan *rs)
{
    uchar *d=rs->data;
    if (dif==0) return ;
    int v,r;
    for (int x=rs->x1;x<=rs->x2;x++)
    {
        v=rand()%dif;
        if ((rand()&1)==1) v=-v;
        r=*d+v;  if (r<0) r=0; else if (r>255) r=255;  *d=r; d++;
        r=*d+v;  if (r<0) r=0; else if (r>255) r=255;  *d=r; d++;
        r=*d+v;  if (r<0) r=0; else if (r>255) r=255;  *d=r; d++;    
    }
}




void rgb_invert_cl::apply_scan(rgb_scan *rs)
{
    uchar *d=rs->data;
    for (int x=rs->x1;x<=rs->x2;x++)
    {
        if (r) *d=255-*d; d++;
        if (g) *d=255-*d; d++;
        if (b) *d=255-*d; d++;
    }
}


void light(select_line *s, image *im, palette *pal, color_filter *f,char *st)
{
    int r,g,b;
    if (sscanf(st,"%d %d %d",&r,&g,&b)==3)
    {
        rgb_lighting l(s,im,pal,r,g,b);
        l.apply();
        l.put(im,f);
    }
}



void rgb_invert(select_line *s, image *im, palette *pal, color_filter *f,char *st)
{
    int r,g,b;
    if (sscanf(st,"%d %d %d",&r,&g,&b)==3)
    {
        rgb_invert_cl l(s,im,pal,r,g,b);
        l.apply();
        l.put(im,f);
    }
}


void gravel(select_line *s, image *im, palette *pal, color_filter *f,char *st)
{
    int d;
    if (sscanf(st,"%d",&d)==1)
    {
        rgb_gravel l(s,im,pal,d);
        l.apply();
        l.put(im,f);
    }
}