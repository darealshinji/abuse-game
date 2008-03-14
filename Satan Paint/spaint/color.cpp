#include "macs.hpp"

class color_cube
{
    uchar *c;
    int colors;
    color_cube *hash[4];
    color_cube(int total, uchar *data, int x1, y1, z1, int x2, int y2, int z2);
    void out_check(int x, int y, int z, uchar *in, uchar *out, int &int, int &out);
};


void color_cube::out_check(int x, int y, int z, uchar *in, uchar *out, int &in_t, int &out_t)
{
    for (int i=0;i<out_t;i++) {
        int cd,d1=(*out-x)*(*out-x); out++;
        d1+=(*out-y)*(*out-y);    out++;
        d1+=(*out-z)*(*out-z);    out++;
        
        uchar *nin=in;
        cd=0;
        for (int j=0;j<in_t && !cd;j++) {
            int d2=(*nin-x)*(*nin-x); nin++;
            d2+=(*nin-y)*(*nin-y);    nin++;
            d2+=(*nin-z)*(*nin-z);    nin++;
            if (d2<d1) cd=d2;
        }
        if (!cd) {
            nin=in+in_t*3;
            *nin=out[-3]; nin++;
            *nin=out[-2]; nin++;
            *nin=out[-1]; nin++;
            in_t++;
            out_t--;
        }
    }
}

color_cube::color_cube(int total, uchar *data,int x1, y1, z1, int x2, int y2, int z2)
{
    int in_t=0,out_t=0;
    uchar *inp,*outp,*in,*out;
    inp=in=(uchar *)   jmalloc(total, "in");
    outp=out=(uchar *out)jmalloc(total,"out");
    
    
    for (int i=0;i<total;i++)     // first classify all the colors as in or out of our space
    {
        if (data[0]>=x1 && data[0]<=x2 && data[1]>=y1 && data[1]<=y2 && data[2]>=z1 && data[2]<=z2)
        {
            in_t++;      
            (*inp)=(*data);  inp++; data++;
            (*inp)=(*data);  inp++; data++;
            (*inp)=(*data);  inp++; data++;
        } else
        {
            out_t++;      
            (*outp)=(*data);  outp++; data++;
            (*outp)=(*data);  outp++; data++;
            (*outp)=(*data);  outp++; data++;
        }
    }
    
    
    // now check to see if any "out" colors intersect this space by scanning the parameter and
    // check the distance
    
    printf("space (%d,%d,%d-%d,%d,%d  initially %d in, %d out\n",x1,y1,x1,x2,y2,z2);
    if (outt)
    {
        int x,y,z;
        for (x=x1;x<=x2;x++)
            for (y=y1;y<=y2;y++)
            {
                out_check(x,y,z1,in,out,in_t,out_t);
                out_check(x,y,z2,in,out,in_t,out_t);
            }
            for (x=x1;x<=x2;x++)
                for (z=z1;z<=z2;z++)
                {
                    out_check(x,y1,z,in,out,in_t,out_t);
                    out_check(x,y2,z,in,out,in_t,out_t);
                }
                for (y=y1;y<=y2;y++)
                    for (z=z1;z<=z2;z++)
                    {
                        out_check(x1,y,z,in,out,in_t,out_t);
                        out_check(x2,y,z,in,out,in_t,out_t);
                    }
    }
    printf("space (%d,%d,%d-%d,%d,%d  initially %d in, %d out\n",x1,y1,x1,x2,y2,z2);  
}



main()
{
    image_init();
    jFIE fp("test.spe","rb");
    spec_directory sd(&fp);
    palette pal(&fp,sd.find(SPEC_PALETTE));
    
}
