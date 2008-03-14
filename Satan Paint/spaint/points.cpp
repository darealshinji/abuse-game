#include "Include/points.h"
#include "specs.hpp"
#include "macs.hpp"
#include <string.h>

void point_list::resize(int ow, int oh, int nw, int nh)
{
    uchar *d=data;
    for (int i=0;i<tot;i++,d+=2) {
        d[0]=d[0]*nw/ow;
        d[1]=d[1]*nh/oh;
    }
}

point_list::point_list(unsigned char how_many, unsigned char *Data)
{
    tot=how_many;
    if (tot) {
        data=(unsigned char *)jmalloc((int)how_many*2,"point list");
        memcpy(data,Data,(int)tot*2);
    }
    else
        data = NULL;
}

point_list::point_list(jFILE *fp)
{
    fp->read(&tot,1);
    if (tot) {
        data=(unsigned char *)jmalloc((int)tot*2,"point list");
        fp->read(data,(int)tot*2);
    }
    else
        data = NULL;
}

void point_list::save(jFILE *fp)
{
    fp->write(&tot,1);
    if (tot)
        fp->write(data,(int)tot*2);
}


int point_list::size()
{
    if (tot)
        return 1+2*(int)tot;
    else
        return 1;  
}