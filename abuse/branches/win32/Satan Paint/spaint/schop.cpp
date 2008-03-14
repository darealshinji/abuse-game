#include "image.hpp"
#include "specs.hpp"
#include <stdio.h>
#include <stdlib.h>

void free_up_memory() { ; }

main(int argc, char **argv)
{
    jFILE *fp;
    spec_entry *e;
    int i,j;
    image_init();
    spec_directory *sd,*old_dir;
    if (argc<2)
    {
        printf("ussage : %s spe_file\n",argv[0]);
        exit(0);
    }
    
    fp=new jFILE(argv[1],"rb");
    if (fp->open_failure()) 
    { printf("Unable to open %s\n",argv[1]); 
    delete fp;
    exit(1); 
    }
    
    sd=new spec_directory(fp);    
    for (i=0;i<sd->total;i++)
    {
        spec_directory tmp;
        spec_entry *e=sd->entries[i];
        tmp.add(new spec_entry(e->type,e->name,e->link,e->size,0));
        tmp.calc_offsets(); 
        char n[100];
        strcpy(n,e->name);
        for (j=0;j<strlen(e->name);j++)
        {
            if (n[j]==' ') 
                n[j]='_';
        }
        jFILE *f=tmp.write(n);
        fp->seek(e->offset,SEEK_SET);
        char *buf=(char *)jmalloc(0xffff,"tmp buffer");
        if (!buf)
        { printf("mallocation error\n");
        exit(0);
        }
        long size=e->size,bytes_read=1;
        while (size && bytes_read)
        {
            long to_read=0xffff;
            if (to_read>size) to_read=size;
            bytes_read=fp->read(buf,to_read);
            f->write(buf,bytes_read);
            size-=bytes_read;
        }
        jfree(buf);
        delete f;
    }
    delete fp;
}