#include "Include/area.h"
#include "jmalloc.hpp"
#include <string.h>

int find_sel_above(select_line *first, int x, int y)
{
    for (select_line *f=first;f;f=f->next) {
        if (f->y==y-1 && f->x1<=x && f->x2>=x) return 1;
    }
    return 0;
}

select_line *add_select_line(select_line *first, int y, int x1, int x2)  // add an area to the list, 
//     combine if possible
{  
    if (x1>x2) { int swap=x1; x1=x2; x2=swap; }
    select_line *last=NULL;
    for (select_line *f=first;f;f=f->next) {
        if (f->y==y && ((f->x1<=x1 && f->x2>=x2) || (f->x2==x1) || (f->x1==x2))) {
            f->x1=min(x1,f->x1);
            f->x2=max(x2,f->x2);
            return first;                 // add into this area, don't add new one
        }
        last=f;
    }
    select_line *p=new select_line(y,x1,x2,NULL);
    if (last)
        last->next=p;
    else return p;
    return first;
}

int intsect_x(int y, int x1, int y1, int x2, int y2)
{
    if ((y>=y1 && y<=y2) || (y<=y1 && y>=y2)) {
        if (y==y1) return x1;
        else if (y==y2) return x2;
        else {
            int w=(x2-x1),h=(y2-y1);
            return (int)(w*(y-y1)/h+x1);
        }
    } else return -1;
}

select_line *make_select_mask(ushort *points, int total)
{
    if (total<2) return NULL;
    int i=0,y1=0xffff,y2=0,x2=0;
    ushort *son=points;
    for (i=0;i<total;i++)  // scan for the top and bottom points in selection list
    {
        if (*son>x2) x2=*son;
        son++;  
        if (*son>y2) y2=*son;
        if (*son<y1) y1=*son;
        son++;
    }
    
    uchar *mask_line=(uchar *)jmalloc(x2+1,"select mask tmp");
    select_line *first=NULL;
    // now make area list between y1 && y2
    for (int y=y1;y<=y2;y++) {
        memset(mask_line,0,x2+1);
        for (i=1,son=points;i<total;i++,son+=2)   // see if any segments pass through this y.
        {
            int lx=son[0],ly=son[1],cx=son[2],cy=son[3];
            if (ly==y2) ly++;
            if (cy==y2) cy++;
            // this one does, find the point of intersection
            if ((ly<=y+0.5 && cy>=y+0.5) || (ly>=y+0.5 && cy<=y+0.5)) {
                int tx=intsect_x(y,lx,ly,cx,cy);
                ushort *s2=points;
                mask_line[tx]+=2;
                for (int j=1;j<total;j++,s2+=2) {
                    int ox=intsect_x(y,s2[0],s2[1],s2[2],s2[3]);
                    if (ox>tx) {
                        mask_line[tx]++;
                        if ((ox==s2[0] && y==s2[1]) || (ox==s2[2] && y==s2[3])) {
                            s2+=2; 
                            j++;
                        }
                    }
                }
                if ((tx==cx && y==cy) || (tx==lx && y==ly)) {
                    son+=2;
                    i++;
                }	    
            }
        }
        i=0;
        while (i<=x2) {
            while (i<=x2 && !mask_line[i]) i++;
            if (i<=x2)  // did we find a point?
            {
                int tx=mask_line[i];
                int lx=i;
                if ((tx&1)) {
                    i++;
                    while (i<=x2 && !mask_line[i]) 
                        i++;
                    if (i<=x2) {
                        first=add_select_line(first,y,lx,i); 
                    } else first=add_select_line(first,y,lx,lx); 
                }
                else {
                    first=add_select_line(first,y,i,i); 	  
                    i++;
                }
            }
        }
    }
    jfree(mask_line);
    return first;
}

/*
static ushort *cur_plist;
int compare_ind(void const *a, void *const *b)
{
int p1=*((int *)a),p2=*((int *)b);
return cur_plist[p1*2+1]<=cur_plist[p2*2+1] ? -1 : 1;
}

 typedef struct { double x,dx; int i; } Edge;
 
  select_line *make_select_mask(ushort *points, int total)
  {
  if (total<=0) return NULL;
  int *ind=(int *)jmalloc(sizeof(int *)*total,"indecies");
  select_line *first=NULL;
  for (int k=0;k<total;k++) ind[k]=k;
  cur_plist=points;
  qsort(ind,total,sizeof(ind[0]),comapre,ind);
  
}*/
