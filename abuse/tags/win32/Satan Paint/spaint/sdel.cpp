#include "image.hpp"
#include "specs.hpp"
#include <stdio.h>
#include <stdlib.h>

void free_up_memory() { ; }

main(int argc, char **argv)
{
    jFILE *fp;
    spec_entry *e;
    int i;
    image_init();
    spec_directory *sd,*old_dir;
    if (argc<3)
    {
        printf("ussage : %s spe_file [object names | object offest]\n",argv[0]);
        exit(0);
    }
    
    fp=new jFILE(argv[1],"rb");
    if (fp->open_failure()) 
    { 
        delete fp;
        printf("Unable to open %s\n",argv[1]); 
        exit(1); 
    }
    
    old_dir=new spec_directory(argv[1]);
    sd=new spec_directory(fp);    
    int del,j;
    for (i=2;i<argc;i++)
    {
        do
        {
            del=0;
            for (j=0;j<sd->total && !del;j++)
            {
                if (!strcmp(argv[i],sd->entries[j]->name) || 
                    atoi(argv[i])==sd->entries[j]->offset)
                {
                    del=1;
                    sd->remove(sd->entries[j]);
                    old_dir->remove(old_dir->entries[j]);
                }
            }      
        } while (del);
    }
    
    sd->calc_offsets();
    jFILE *fo=sd->write("new0XYZ.spe");
    if (!fo)
    {
        printf("unable to open tmp file 'new0XYZ.spe'\n");
        exit(0);
    }
    
    for (i=0;i<sd->total;i++)  
    {
        fp->seek(old_dir->entries[i]->offset,SEEK_SET);
        unsigned char *buf=(unsigned char *)mjalloc(old_dir->entries[i]->size,"read buf");
        fp->read(buf,old_dir->entries[i]->size);
        fo->write(buf,old_dir->entries[i]->size);
        jfree(buf);
    }
    delete fp;
    delete fo;
}