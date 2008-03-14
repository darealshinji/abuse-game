#include <stdio.h>
#include <stdlib.h>
#include "specs.hpp"

void free_up_memory() { ; }

#define BUF_SIZE 32768
char buf[BUF_SIZE];

int main(int argc, char **argv)
{
    if (argc<2) {
        printf("Ussage : %s filename filename1 filename2 ...\n");
        exit(1);
    }
    spec_directory *sd_old;
    FILE *fp=fopen(argv[1],"rb");
    if (!fp)
    {
        fp=fopen(argv[1],"wb");
        if (!fp)
        {
            printf("Unable to open %s for writing\n",argv[1]);
            exit(0);
        }
        sd_old=new spec_directory();
        fp=NULL;
    } else sd_old=new spec_directory(fp);
    
    
    spec_directory sd_new;  
    for (int i=2;i<argc;i++)
    {
        FILE *fp_new=fopen(argv[i],"rb");
        if (!fp_new)    
            printf("Unable to open %s for reading, not added to file\n",argv[i]);         
        else
        {
            fseek(fp_new,0,SEEK_END);
            sd_new.add(new spec_entry(SPEC_NORMAL_FILE,argv[i],NULL,ftell(fp_new),0));
            fclose(fp_new);
        }
    }
    
    
    FILE *tmp2=fopen("sadd.tmp","wb");
    if (!tmp2)
    { printf("unable to open tmp file in currnet directory\n");
    exit(1);
    }
    fclose(tmp2);   // just make sure we have write access
    
    sd_new.calc_offsets();
    jFILE *tmp=sd_new.write("sadd.tmp");
    for (i=0;fp && i<sd_old->total;i++)
    {
        int bread,left=sd_old->entries[i]->size;
        do
        {
            if (left<BUF_SIZE)
                bread=fread(buf,1,left,fp);
            else
                bread=fread(buf,1,BUF_SIZE,fp);
            if (bread) tmp->write(buf,bread); 
            left-=bread;
        } while (left>0);
    }
    if (fp)
        fclose(fp);  
    
    for (i=2;i<argc;i++)
    {
        FILE *fp=fopen(argv[i],"rb");
        int old=sd_old->total;
        if (fp)
        {
            int left=sd_new.entries[old+i-2]->size,bread;
            printf("adding %s (%d bytes)\n",argv[i],left);
            do
            {
                if (left<BUF_SIZE)
                    bread=fread(buf,1,left,fp);
                else
                    bread=fread(buf,1,BUF_SIZE,fp);
                if (bread) tmp->write(buf,bread); 
                left-=bread;
                fprintf(stderr,".");
            } while (left>0);      
            fclose(fp);
        }
    }
    
    delete tmp;
    return 1;
}