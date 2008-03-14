#include "Include/paint.h"
#include "macs.hpp"
#include "Include/undo.h"
#include "supmorph.hpp"
#include "timage.hpp"
#include "status.hpp"
#include "Include/loader.h"
#include "image24.hpp"

#define M_PI 3.14159f

void painter::add_history(char *command)
{
    int use;
    if (tot_hist>=HISTORY_SIZE) {
        int i;
        jfree(command_hist[0]);
        for (i=0;i<HISTORY_SIZE;i++)
            command_hist[i]=command_hist[i+1];
        use=tot_hist-1;
    }
    else {
        use=tot_hist;
        tot_hist++;   
    }
    
    command_hist[use]=strcpy((char *)jmalloc(strlen(command)+1,"history"),command);
    hist_on=use+1;
}

void painter::grow(int xa, int ya)
{
    image *n=new image(current_image()->width()+abs(xa),
        current_image()->height()+abs(ya));
    int xp=(xa<0 ? -xa : 0),
        yp=(ya<0 ? -ya : 0);
    n->clear();
    current_image()->put_image(n,xp,yp);
    delete current_item()->im;
    current_item()->im=n;
    
    draw_image(current_image(),1);
}

void painter::morph(char *st)
{  
    char *fname;
    
    if (*st && num_images)
    {
        trans_image *h1=NULL,*h2;
        int steps=get_number(st);
        int frames=get_number(st);
        int blur=get_number(st);
        
        if (steps<=0 || frames<1 || blur<0)
            info("usage : morph steps frames blur im1 im2 [hint1 hint2] [[filename]]");    
        else
        {          
            int i1,i2,hint1,hint2;
            i1=get_number(st);
            i2=get_number(st);
            if (i1<0 || i2<0)
                info("usage : morph steps im1 im2 [hint1 hint2] [[filename]]");    
            else
            {	
                fname=st;	
                hint1=get_number(st);
                if (hint1<0)
                {
                    h1=new trans_image(get_image(i1),"paint tmp");
                    h1->make_color(15);
                    h2=new trans_image(get_image(i2),"paint tmp");
                    h2->make_color(15);
                } else 
                {
                    hint2=get_number(st);	 
                    if (hint1>=0 && hint1<num_images && hint2>=0 && hint2<num_images)
                    {	    
                        h1=new trans_image(get_image(hint1),"paint morph tmp");	
                        h2=new trans_image(get_image(hint2),"paint morph tmp");
                    }	  
                    fname=st;	  
                }
                
                if (h1)
                {
                    char name[140];	  
                    super_morph *m=new super_morph(h1,h2,steps,NULL);
                    if (m->t!=0)
                    {
                        int hi=max(h1->height(),h2->height()),
                            wid=max(h1->width(),h2->width());
                        delete h1;
                        delete h2;
                        smorph_player mort(m,pal,get_image(i1),get_image(i2),frames,1);
                        
                        delete m;
                        
                        color_filter *filt=palc->get_filter();
                        int frame_on;
                        
                        for (frame_on=0;frame_on<frames;frame_on++)
                        {
                            image *morph_pic=new image(wid,hi);
                            morph_pic->clear();	
                            
                            sprintf(name,"%s/%s-%d",get_item(i1)->name(),
                                get_item(i2)->name(),frame_on);
                            mort.show(morph_pic,0,0,filt,palc->current_pal(),blur); 
                            
                            (*add_imitem())=new imitem(morph_pic,name);
                        }
                    } else
                    {
                        delete m;
                        info("Hint colors do not match up");
                    }
                }	
            }                  
        }    
    }   
}


// returns the image number on success, -1 on error
int painter::get_number(char *&st)    // scans off and image number and advences pointer
{
    char name[100],*n,errmsg[100];
    int i,ret=-1;
    
    while (*st && *st==' ') st++;
    if (!st) 
    {    
        info("Image number/name expected");
        return -1;
    }  
    else
    {
        if (*st=='"')    // its a image name, scan it off and find corresponding name
        {
            st++;
            n=name;      
            while (*st && *st!='"')
                *(n++)=*(st++);      
            if (*st==0)
            {	    
                info("Error : missing a \"");
                return -1;
            }      
            else
            {
                st++;	
                *n=0;	      
                for (i=0;i<num_images && ret<0;i++)
                    if (editim[i])    // make sure it hasn't been deleted
                        if (!strcmp(editim[i]->name(),name)) ret=i;
                        if (ret<0)
                        {
                            sprintf(errmsg,"No image named %s",name);	  
                            info(errmsg);
                            return -1;
                        }		  		
            }            
        } else if (!isdigit(*st))
        {
            n=name;
            while (*st && *st!=',' && *st!='-' && *st!=' ' && *st!='"')
                *(n++)=*(st++);
            *n=0;     
            
            if (!strcmp(name,"last"))
                ret=num_images-1;
            else if (!strcmp(name,"this"))
                ret=cur_image;
            else if (!strcmp(name,"last_on"))
            {
                if (last_on<num_images && last_on>0)
                    ret=last_on;
                else ret=-1;
            }
            else
            {
                info("error : Expected a \", a number, this or last");
                return -1;    
            }
        } else                     // it is a number
        {
            n=name; 
            while (*st && isdigit(*st))
                *(n++)=*(st++);
            *n=0;
            if (!sscanf(name,"%d",&ret))
            {
                info("Error reading number");
                return -1;
            }           
        }
    }
    
    return ret;  
    
}


int painter::merge(char *filename, int start, int end)
{
    char sig[100];
    int y;	  
    FILE *fp;
    int t=0;
    char fn[100],*ff;
    
    do
    {
        ff=fn;
        while (*filename && *filename!=' ') 
            *(ff++)=*(filename++);
        *ff=0;
        
        if (*filename)
            filename++;
        
        fp=fopen(fn,"rb");
        if (!fp)
        {
            sprintf(sig,"Error opening file %s, aborted",fn);
            info(sig);
        }
        else 
        {
            sprintf(sig,"Merging %s",fn);
            t++;
            info(sig);
            eh->flush_screen();
            fclose(fp);
            palette *pal2;
            short tot;    
            pal2=palc->current_pal()->copy();   
            imitem **im=load(fn,tot,pal2);
            
            int count=0;
            if (pal2)
            {
                filter f(pal2,pal);
                for (y=0;y<tot;y++)
                {	
                    if ((start==-1 || y>=start) && (end==-1 || y<=end))
                    {
                        f.apply(im[y]->im);
                        (*add_imitem())=im[y];
                        count++;
                    } else delete im[y];
                }
                jfree(im);
                
                
                delete pal2;
                sprintf(sig,"Merged %d images",count);
                info((char *)sig);      
            } else info("Error : file does not have palette\n");  
        } 
    } while (*filename);
    return t;
}


void painter::clear_unused()
{
    int i,x,y,w,h;
    unsigned char used[256],*sl; 
    memset(used,0,256);
    add_undo(0,undo_palette,palc->current_pal());  
    info("Scanning artwork, hang on");  
    for (i=0;i<num_images;i++)
    {
        w=get_image(i)->width();
        h=get_image(i)->height();
        for (y=0;y<h;y++)
        {
            sl=get_image(i)->scan_line(y);
            for (x=0;x<w;x++,sl++)
                used[*sl]=1;  
        }    
    } 
    for (i=0;i<256;i++)
        if (!used[i])
            palc->change_color(i,0,0,0);
        
}


void painter::load_property(char *filename, char *range)
{
    char buffer[500],*np;  
    int i,found,old_cur;
    
    FILE *fp=fopen(filename,"r");
    if (!fp) 
    {
        sprintf(buffer,"Unable to open %s",filename);
        info(buffer);
    } else
    {
        i=get_next_mark(range);
        if (i==-1) i=cur_image;    
        for (;i!=-1;i=get_next_mark(range,i))
        {      
            fseek(fp,0,SEEK_SET);
            found=0;      
            while (!feof(fp) && !found)
            {	
                fgets(buffer,500,fp);
                buffer[strlen(buffer)-1]=0;      // chop off the newline	
                if (strlen(buffer)>5)
                {	  
                    np=buffer;
                    while (np[4] && (np[0]!='n' || np[1]!='a' || np[2]!='m' || np[3]!='e'))
                        np++;
                    if (np[4])
                    {
                        if (!strcmp(np+5,editim[i]->name()))
                        {
                            old_cur=cur_image;
                            set_current(i);
                            event ev;	      
                            set(buffer);
                            set_current(old_cur);
                            found=1;	      
                        }
                    }	  
                }
            }
            if (!found)
            {
                sprintf(buffer,"property for %s not found",editim[i]->name());
                info(buffer);
            }      
        }
        fclose(fp);        
    }    
}


void painter::write_property(char *filename, char *range)
{
    char buffer[500];
    int i;
    
    FILE *fp=fopen(filename,"a");
    if (!fp) 
    {
        sprintf(buffer,"Unable to open %s",filename);
        info(buffer);
    } else
    {
        i=get_next_mark(range);
        if (i==-1) i=cur_image;    
        for (;i!=-1;i=get_next_mark(range,i))
        {      
            editim[i]->make_property_string(buffer);
            fprintf(fp,"%s\n",buffer);
        }
        fclose(fp);          
    }      
}

/* 
the set command takes a list of things to set and their values, 
setable properties are (type, xcfg, attack, damage)

 examples include
 set type foretile
 set attack 2 4 4 8 2
 set xcfg 8
 you can combine more than one set in the same command, example
 set type character xcfg 4 damage 3 0 0 0 4 5 4 
 the above example reuires the type set to occur first in the list because the other
 things being set are dependant on being a character type.
*/
void painter::set(char *st)
{
    int attack=0,body=0,damage=0,found=1,i;  
    unsigned char data[50];
    char word[50],cmd[50];
    int total,num; 
    if (!num_images) return ;
    do
    {
        while (*st==' ' || *st==',') st++;
        
        if (*st)
        { 
            total=0;     
            if (!sscanf(st,"%s",word))
            {	
                info("error parsing set command");
                return ;
            }
            while (*st!=' ') st++;  // skip past first word
            while (*st==' ') st++;  // and the space between the two     
            if (!strcmp(word,"name"))      
            {	
                editim[cur_image]->set_name(st);
                st+=strlen(st);
            }
            
            else if (strcmp(word,"type")) // if it's not a type command then a number should follow
            {
                if (!sscanf(st,"%d",&total))     
                {
                    info("expected number");	  
                    return ;
                }
                
                while (*st && *st!=' ') st++;  // skip past the number and any trailing spaces.
                while (*st==' ') st++;
                
                if (!strcmp(word,"attack") || !strcmp(word,"damage") ||
                    !strcmp(word,"frame"))
                {
                    if (total<0)
                    {
                        image *i=current_image();
                        int w=i->width()-1,h=i->height()-1;
                        int x1=w,y1=h,x2=0,y2=h,x3=w,y3=0,x4=0,y4=0,x,y;
                        unsigned char *sl;
                        for (y=0;y<i->height();y++)
                        {
                            sl=i->scan_line(y);
                            for (x=0;x<i->width();x++,sl++)	      
                                if (*sl)
                                {		  
                                    if (x*x+y*y<x1*x1+y1*y1)
                                    {
                                        x1=x;
                                        y1=y;
                                    } 
                                    
                                    if ((x-w)*(x-w)+y*y<(x2-w)*(x2-w)+y2*y2)
                                    {
                                        x2=x;
                                        y2=y;
                                    }
                                    
                                    if (x*x+(y-h)*(y-h)<x3*x3+(y3-h)*(y3-h))
                                    {
                                        x3=x;
                                        y3=y;
                                    }
                                    
                                    if ((x-w)*(x-w)+(y-h)*(y-h)<(x4-w)*(x4-w)+(y4-h)*(y4-h))
                                    {
                                        x4=x;
                                        y4=y;
                                    }
                                    
                                }
                        }
                        total=5;
                        data[0]=x1; data[1]=y1;
                        data[2]=x2; data[3]=y2;
                        data[4]=x4; data[5]=y4;
                        data[6]=x3; data[7]=y3;
                        data[8]=x1; data[9]=y1;
                    }
                    else
                    {
                        for (i=0;i<total*2;i++)
                        {
                            if (!sscanf(st,"%d",&num))
                                info("error parsing set command");
                            else
                            {	    
                                data[i]=num;	    
                                while (*st!=' ') st++;
                                while (*st==' ') st++;
                            }
                        }	
                    }
                    switch (editim[cur_image]->spec_type())
                    {
                    case SPEC_CHARACTER2 :
                        if (!strcmp(word,"attack"))
                            ((figure *)editim[cur_image])->set_hit(total,data);
                        else if (!strcmp(word,"damage"))        
                            ((figure *)editim[cur_image])->set_damage(total,data);            
                        else info("character does not have this property");
                        break;
                    case SPEC_FORETILE : 
                        if (!strcmp(word,"frame"))
                            ((foretile *)editim[cur_image])->set_points(total,data);
                        else info("foretile does not have this property");
                        break;
                    }
                } else if (!strcmp(word,"xcfg"))
                {
                    if (editim[cur_image]->spec_type()==SPEC_CHARACTER2)
                    {
                        if (total<0)
                            ((figure *)editim[cur_image])->xcfg=current_image()->width()/2;
                        else
                            ((figure *)editim[cur_image])->xcfg=total;
                    }
                    else info("not character type");
                } else if (!strcmp(word,"cx"))
                {
                    if (editim[cur_image]->spec_type()==SPEC_PARTICLE)	  
                        ((particle *)editim[cur_image])->cx=total;
                    else info("not particle type");
                } else if (!strcmp(word,"cy"))
                {
                    if (editim[cur_image]->spec_type()==SPEC_PARTICLE)	  
                        ((particle *)editim[cur_image])->cy=total;
                    else info("not particle type");
                } 
                else if (!strcmp(word,"hit_damage"))
                {
                    if (editim[cur_image]->spec_type()==SPEC_FORETILE)
                        ((foretile *)editim[cur_image])->damage=total;
                    else if (editim[cur_image]->spec_type()==SPEC_CHARACTER2)
                        ((figure *)editim[cur_image])->hit_damage=total;	  
                    else info("not foretile or character type");
                } else if (!strcmp(word,"advance"))
                {
                    if (editim[cur_image]->spec_type()==SPEC_CHARACTER2)
                        ((figure *)editim[cur_image])->advance=total;	  
                    else info("not character type");
                } else if (!strcmp(word,"next"))
                {
                    if (editim[cur_image]->spec_type()==SPEC_FORETILE)
                        ((foretile *)editim[cur_image])->next_pic=total;
                    else if (editim[cur_image]->spec_type()==SPEC_BACKTILE)
                        ((backtile *)editim[cur_image])->next_pic=total;
                    else info("not foretile or backtile type");
                }
                
      } else
      {  
          if (!sscanf(st,"%s",word))
          {
              info("expected character type");
              return ;
          }
          while (*st && *st!=' ') st++;   // skip over the character type
          while (*st==' ') st++;
          sprintf(cmd,"type %s",word);
          event ev;
          do_command(cmd,ev);        
      }
    }
  } while (*st);
}


void painter::copy_property(char *st)
{
    char word[50],buf[300],sub_buf[300]; 
    int num=get_number(st);
    if (!num_images) return ;
    if (num<0 || num>=num_images)
        info("ussage : copy_prop image_num properties");
    else
    {
        editim[num]->make_property_string(buf);
        while (*st)
        {
            while (*st==' ') st++;
            if (*st)
            {
                sscanf(st,"%s",word);
                sprintf(sub_buf," %s",word);	
                while (*st && *st!=' ') st++;
                char *find=strstr(buf,sub_buf);
                if (find)
                {
                    char *o=sub_buf;
                    while (*find!=',' && *find)
                        *(o++)=*(find++);
                    *o=0;
                    set(sub_buf);
                } else 
                {
                    sprintf(buf,"image %d has no property %s\n",num,word);
                    info(buf);
                }
            }
        }
    }
    
}

void painter::remap(char *st)
{
    if (num_images)
    {
        filter f;
        image *im=current_image();  
        palette *pal=palc->current_pal(); 
        int first=get_number(st);
        if (first==-1)
            info("Remap ussage : [color numbers] (map to current color)\n");
        f.set(first,current_color());
        do
        {
            first=get_number(st);
            if (first!=-1)
                f.set(first,current_color());
        } while (first!=-1);
        f.apply(im);
        draw_image(im,1);
    }  
}


void painter::undo()
{
    if (undos.first())
    {
        undo_marker *m=(undo_marker *)undos.first()->last();
        undos.unlink((linked_node *)m);     
        m->restore();
        delete m;    
    }  
}


int painter::get_next_mark(char *mark, int last_mark, int last_image)   
// -1 gets the first mark, -1 ret means no marks
{
    char *st=mark;
    char number[100];
    
    int num,return_next,end_sequence,counter,l;  
    if (last_image>=0) l=last_image; else l=num_images-1;
    
    if (last_mark==-1) 
        return_next=1;
    else return_next=0;
    sscanf(mark,"%s",number);
    if (!strcmp(number,"all"))  
        if (last_mark<l)
            return last_mark+1;
        else return -1;
        
        while (1)
        {
            num=get_number(st);    
            if (num==-1) return -1;
            if (return_next) 
                return num;
            else if (num==last_mark)    
                return_next=1;
            
            if (*st=='-')   // we have a sequence delimeter
            {
                st++;      
                end_sequence=get_next_mark(st,-1);   
                if (end_sequence==-1) return -1;
                
                if (*st=='"') 
                {
                    st++;	
                    while (*st!='"') st++;
                    st++;
                }
                else if (isdigit(*st))
                {	
                    while (isdigit(*st))
                        st++;
                } else 
                    while (*st && *st!=' ' && *st!='-' && *st!=',') st++;
                    
                    
                    if (end_sequence<num) counter=-1;
                    else if (end_sequence>num) counter=1;
                    else counter=0;
                    if (counter)
                    {
                        while (num!=end_sequence)
                        {
                            num+=counter;
                            if (return_next) return num;
                            else if (num==last_mark) return_next=1;           
                        }	 
                    }     
            }       
        }    
}


void painter::renumber (char *command)
{
    char *st=command;
    char pnum[200],*n,new_name[200];
    int num,count;
    
    
    sscanf(command,"%d",&count);
    
    while (*st!='"' && *st) st++;
    if (!*st) return ; else st++;
    
    for (n=pnum;*st && *st!='"';st++,n++)
        *n=*st;
    *n=0;
    st++;  
    
    for (num=get_next_mark(st);num!=-1;num=get_next_mark(st,num),count++)
    {
        sprintf(new_name,pnum,count,count,count,count,count);
        get_item(num)->set_name(new_name);    
    }       
}



void add_error(int x, int y, int w, int h, unsigned char *buffer,
               int r_error, int g_error, int b_error, int error_mult)
{
    if (x>=w || x<0 || y>=h || y<0)           // check to make sure this pixel is in the image
        return ;
    unsigned char *pix=buffer+(y*w*3+x*3);    
    int result;
    
    result=(int)(*pix)+r_error*error_mult/32;
    if (result>255)
        *pix=255;
    else if (result<0) *pix=0;
    else *pix=result;  
    pix++;
    
    result=(int)(*pix)+g_error*error_mult/32;
    if (result>255)
        *pix=255;
    else if (result<0) *pix=0;
    else *pix=result;
    pix++;
    
    result=(int)(*pix)+b_error*error_mult/32;
    if (result>255)
        *pix=255;
    else if (result<0) *pix=0;
    else *pix=result;
}


void painter::quant8(int x, int y,           // place to put onto the destination image
                     int w, int h,           // width & hieght of 24 bit buffer
                     unsigned char *buffer,  // 24 bit buffer to place onto 8 bit image
                     image *dest)            // destination image to place quant buffer
{  
    int i,j,closest;
    unsigned char r,g,b,*cur_pixel=buffer,ar,ag,ab;  
    palette *pal=palc->current_pal(); 
    if (!h) return ;
    stat_man->push("Quantizing",NULL);
    for (j=0;j<h;j++)
    {    
        stat_man->update(j*100/h);
        for (i=0;i<w;i++)
        {
            r=(*cur_pixel);  cur_pixel++;
            g=(*cur_pixel);  cur_pixel++;
            b=(*cur_pixel);  cur_pixel++;      
            closest=palc->find_rgb(r,g,b);                   // find the closest match in palette
            dest->putpixel(x+i,y+j,closest);                // set the pixel to this color
            
            pal->get(closest,ar,ag,ab);                     // see what the actual color we used was
            
            add_error(i+1,j,w,h,buffer,ar-r,ag-g,ab-b,8);  
            add_error(i+2,j,w,h,buffer,ar-r,ag-g,ab-b,4);  
            
            add_error(i-2,j+1,w,h,buffer,ar-r,ag-g,ab-b,2);
            add_error(i-1,j+1,w,h,buffer,ar-r,ag-g,ab-b,4);
            add_error(i,j+1,w,h,buffer,ar-r,ag-g,ab-b,8);
            add_error(i+1,j+1,w,h,buffer,ar-r,ag-g,ab-b,4);
            add_error(i+2,j+1,w,h,buffer,ar-r,ag-g,ab-b,2);
        }
    }   
    stat_man->pop();
}




void painter::blur_area(int x1, int y1, int x2, int y2, int blur_amount, 
                        int blur_color)
{  
    if (blur_amount<1) 
        return ;
    
    image *im=current_image();
    int x,y,wid=im->width(),hi=im->height();
    
    // make sure the rectangle is good
    CHECK(x1>=0 && x1<=x2 && x2<wid && y1>=0 && y1<=y2 && y2<hi);
    
    unsigned char *ibuf=(unsigned char *)jmalloc((x2-x1+1)*3*(y2-y1+1),"blur temp"),
        *cstart;
    
    palette *pal=palc->current_pal(); 
    if (y1>y2) return;
    stat_man->push("Blurring",NULL);
    for (y=y1;y<=y2;y++)    
    {  
        stat_man->update( (y-y1)*100/(y2-y1+1));
        for (x=x1;x<=x2;x++)
        {
            int l,k,t=0,r=0,g=0,b=0;
            unsigned char rv,gv,bv;	
            cstart=ibuf+((x-x1)*3+(y-y1)*(x2-x1+1)*3);
            if (blur_color<0 || blur_color==im->pixel(x,y))
            {
                for (l=-blur_amount;l<=blur_amount;l++)
                {	  
                    for (k=-blur_amount;k<=blur_amount;k++)
                    {
                        int dx=x+l,dy=y+k;
                        if (dx>=0 && dx<wid && dy>=0 && dy<hi)
                        {
                            t++;
                            pal->get(im->pixel(dx,dy),rv,gv,bv);
                            r+=rv;
                            g+=gv;
                            b+=bv;	                    
                        }	                  
                    }      	  	  		
                }                    
                *cstart=r/t;  cstart++;	
                *cstart=g/t;  cstart++;	
                *cstart=b/t;                           
            } else
            {
                pal->get(im->pixel(x,y),cstart[0],cstart[1],cstart[2]);
            }           
        }
        
    }
    stat_man->pop();
    quant8(x1,y1,(x2-x1+1),(y2-y1+1),ibuf,im);  
    jfree(ibuf);  
}

void painter::blend(char *st)
{
    int num,amount;  
    num=get_number(st);
    amount=get_number(st);
    if (!num_images) return ;
    if (amount<=0 || num<0 || amount>99)  
        info("ussage : blend image# percentage");
    else {
        if (num>=num_images) info("image # out of bounds");
        else {
            int w1=get_image(num)->width(),w2=get_image(cur_image)->width();
            int h1=get_image(num)->height(),h2=get_image(cur_image)->height();
            
            image24 *n=new image24(w2,h2);
            int x,y;
            unsigned char r1,g1,b1,r2,g2,b2,*s1,*s2,*s3;
            
            for (y = 0; y < h2; y++) {
                if (y<h1)
                    s1=get_image(num)->scan_line(y);
                
                s2=get_image(cur_image)->scan_line(y);	
                s3=n->scan_line(y);
                
                for (x=0;x<w2;x++,s1++,s2++) {
                    if (x<w1 && y<h1)
                        pal->get(*s1,r1,g1,b1);
                    else r1=g1=b1=0;
                    
                    
                    pal->get(*s2,r2,g2,b2);
                    *(s3++)=(((int)r1*(100-amount)+(int)r2*amount)/100);
                    *(s3++)=(((int)g1*(100-amount)+(int)g2*amount)/100);
                    *(s3++)=(((int)b1*(100-amount)+(int)b2*amount)/100);	  
                }
            }
            delete editim[cur_image]->im;
            editim[cur_image]->im=n->dither(palc->current_pal());
            delete n;     
        }
    }
}

void painter::anti(int x1, int y1, int x2, int y2)
{    
    image *im=current_image();
    int x,y,wid=im->width(),hi=im->height();
    
    // make sure the rectangle is good
    CHECK(x1>=0 && x1<=x2 && x2<wid && y1>=0 && y1<=y2 && y2<hi);
    
    unsigned char *ibuf=(unsigned char *)jmalloc((x2-x1+1)*3*(y2-y1+1),"anti temp"),
        *cstart;
    
    palette *pal=palc->current_pal(); 
    
    stat_man->push("Anti-aliasing",NULL);
    
    for (y=y1;y<=y2;y++) {
        stat_man->update( (y-y1)*100/(y2-y1+1));
        for (x=x1;x<=x2;x++) {
            int l,k,t=0,r=0,g=0,b=0;
            unsigned char rv,gv,bv;	
            for (l=-1;l<=1;l++) {
                for (k=-1;k<=1;k++) {
                    if (!l || !k) {
                        int dx=x+l,dy=y+k;
                        if (dx>=0 && dx<wid && dy>=0 && dy<hi) {
                            t++;
                            pal->get(im->pixel(dx,dy),rv,gv,bv);
                            r+=rv;
                            g+=gv;
                            b+=bv;	                    
                        }	                  
                    }	  
                }      	  	  		
            }    
            t+=5;
            pal->get(im->pixel(x,y),rv,gv,bv);
            r+=(int)rv*5;
            g+=(int)gv*5;
            b+=(int)bv*5;	           
            
            cstart=ibuf+((x-x1)*3+(y-y1)*(x2-x1+1)*3);
            *cstart=r/t;  cstart++;	
            *cstart=g/t;  cstart++;	
            *cstart=b/t;                           
        }
    }
    stat_man->pop();
    
    quant8(x1,y1,(x2-x1+1),(y2-y1+1),ibuf,im);  
    jfree(ibuf);  
}

void  painter::read(char *filename)
{
    char st[200],*s;
    event ev;
    int no_refresh=0;
    
    FILE *fp;
    fp=fopen(filename,"r");
    if (!fp)
        info("file not foud");
    else {
        while (!feof(fp)) {
            fgets(st,200,fp);
            st[strlen(st)-1]=0;
            
            for (s=st;(*s)==' ';s++);
            
            if ((*s) && (*s)!=';' && (*s)!='#') {
                do_command(st,ev);      
                if (!strcmp(st,"index"))
                    no_refresh=1;
                else no_refresh=0;	
            }
        }   	    
    }  
    fclose(fp);  
    if (!no_refresh) {
        if (num_images)
            draw_image(current_image(),1);
    }
    
}


int key_value(char *s)
{
    int v;
    
    if ((s[0]=='F' || s[0]=='f') && isdigit(s[1])) {
        sscanf(s + 1, "%d", &v);
        return v + JK_F1-1;    
    }
    else if (strlen(s)>1)
        return 0;
    else
        return s[0];      
}

void painter::setkey(char *st)
{
    char key[20], *s;
    if (sscanf(st, "%s", key) != 1)
        info("usage : setkey key command");
    else if (key_value(key) == 0)
        info("Bad key name. ex (a, b, A, B, F1-F10)");
    else {
        for (s = st; *s != ' '; s++);
        s++;    
        key_list.add_key(key_value(key), s, NULL);
    }         
}

void painter::crop()
{
    int x1=100000,y1=100000,x2=0,y2=0;
    char st[100];  
    event ev;
    if (num_images) {
        image *im=current_image();
        int y,x;
        unsigned char *sl;
        for (y=0;y<im->height();y++) {
            sl=im->scan_line(y);      
            for (x=0;x<im->width();x++,sl++)
                if (*sl) {
                    if (x<x1) x1=x;
                    if (x>x2) x2=x;
                    if (y<y1) y1=y;
                    if (y>y2) y2=y;	 
                }  
        }    
        if (x1==100000)
            info("Nothing in image to crop");
        else {
            sprintf(st,"clip %d %d %d %d",x1,y1,x2,y2);
            do_command(st,ev);      
        }    
    }
    else
        info("no image to crop");  
}

void find_min(image *im, long &x1, long &y1, long &x2, long &y2, int skip_first, long &fx, long &fy)
{
    x1=0xfffffff;
    y1=0xfffffff;
    x2=y2=0;
    
    for (int y=0;y<im->height();y++) {
        unsigned char *sl=im->scan_line(y);      
        for (int x=0;x<im->width();x++,sl++) {
            if (*sl) {
                if (skip_first) {
                    fx=x;
                    fy=y;
                    skip_first=0;
                }
                else {
                    if (x<x1) x1=x;
                    if (x>x2) x2=x;
                    if (y<y1) y1=y;
                    if (y>y2) y2=y;	 
                }
            }  
        }
    }    
}

void painter::rotate90()
{
    if (!num_images) return ;
    add_undo(cur_image,undo_change);
    image *o=editim[cur_image]->im;
    image *n=new image(o->height(),o->width());
    for (int y=0;y<o->height();y++) {
        unsigned char *sl=o->scan_line(y);
        for (int x=0;x<o->width();x++,sl++)
            n->putpixel(y,x,*sl);
    }
    delete o;
    editim[cur_image]->im=n;
    draw_image(current_image(),1);  
}

void painter::crop_center(char *command)
{
    char st[100];  
    event ev;
    
    if (!command[0])
        command="this";
    
    
    long x1,y1,x2,y2,mx1=0xfffffff,my1=0xfffffff,mx2=0,my2=0,fx,fy;
    int i=get_next_mark(command);
    for (;i!=-1;i=get_next_mark(command,i)) {
        find_min(editim[i]->im,x1,y1,x2,y2,1,fx,fy);
        if (x1<mx1) mx1=x1;
        if (y1<my1) my1=y1;
        if (x2>mx2) mx2=x2;
        if (y2>my2) my2=y2;      
    }
    
    int ocur = cur_image;
    for (i=get_next_mark(command);i!=-1;i=get_next_mark(command,i)) {
        cur_image=i;
        find_min(editim[i]->im,x1,y1,x2,y2,1,fx,fy);
        sprintf(st,"clip %d %d %d %d",x1,y1,x2,my2);
        do_command(st,ev);      
        if (current_item()->spec_type()==SPEC_CHARACTER2)
            ((figure*) current_item())->xcfg = (unsigned char) (fx - x1);
    }
    cur_image = ocur;
}

imitem **painter::add_imitem()
{
    num_images++;
    editim=(imitem **)jrealloc(editim,sizeof(imitem *)*num_images,"image array");
    return &editim[num_images-1];
}

void painter::insert_imitem(int insert_point, imitem *im)
{
    int i;  
    if (insert_point>num_images) insert_point=num_images;
    
    num_images++;
    editim=(imitem **)jrealloc(editim,sizeof(imitem *)*num_images,"image array");
    for (i=num_images-1;i>insert_point;i--)
        editim[i]=editim[i-1];
    editim[insert_point]=im;  
}


void painter::remove_singles(image *im)
{
    unsigned char *u,*d,*sl;
    int y,x;
    if (num_images) {
        add_undo(cur_image,undo_change);
        for (y=1;y<im->height()-1;y++) {
            u=im->scan_line(y-1);    
            sl=im->scan_line(y);    
            d=im->scan_line(y+1);    
            for (x=1;x<im->width()-1;x++) 
                if (u[x-1]==u[x] && u[x]==u[x+1] && u[x]==sl[x-1] && u[x]==sl[x+1] &&
                    u[x]==d[x-1] && u[x]==d[x] && u[x]==d[x+1] && u[x]!=sl[x])
                    sl[x]=u[x]; 
        }
    }  
}

void painter::circle(int x, int y, int r, int color, int mode)
{
    int p=(r+1)*8,i;
    float ang,nang;
    if (num_images) {
        add_undo(cur_image,undo_change);
        for (i=0,ang=0;i<p;i++) {
            nang=ang+(2*M_PI)/(float)p;
            line((int)(x+cos(ang)*r),(int)(y+sin(ang)*r),(int)(x+cos(nang)*r),
                (int)(y+sin(nang)*r),color,mode);
            ang=nang;
        }
    }  
}

void painter::stagger_line(int x1, int y1, int x2, int y2, int c1, int c2, int start_c1)
{
    int t=abs(x2-x1)>abs(y2-y1) ? abs(x2-x1)+1 : abs(y2-y1)+1;
    long xo=x1<<16,yo=y1<<16,dx=((x2-x1)<<16)/t,dy=((y2-y1)<<16)/t,count=0,x,y;
    while (t--) {
        x=xo>>16;
        y=yo>>16;
        if (((x>>2)+(y>>2)+start_c1)&1)
            draw_image_pixel(x,y,c1);
        else draw_image_pixel(x,y,c2);
        xo+=dx; yo+=dy;
    }
}

void painter::line(int x1, int y1, int x2, int y2, int color, int mode)
{
    if (!num_images) return ;
    image *im=current_image();
    int iw=im->width(),ih=im->height();
    int d,x,y,ax,ay,sx,sy,dx,dy;
    dx=x2-x1;  ax=abs(dx)<<1;  sx=dx<0 ? -1 : dx>0 ? 1 : 0;
    dy=y2-y1;  ay=abs(dy)<<1;  sy=dy<0 ? -1 : dy>0 ? 1 : 0;
    x=x1; y=y1;
    if (ax>ay)
    {
        d=ay-(ax>>1);
        for (;;) {
            if (x<iw && y<ih)
            {
                if (mode==MODE_CLEAR)
                    draw_image_pixel(x,y,im->pixel(x,y));
                else draw_image_pixel(x,y,color);
                if (mode==MODE_MODIFY) 
                    im->putpixel(x,y,color);
            }
            if (x==x2) return ;
            if (d>=0) { y+=sy; d-=ax; }  x+=sx; d+=ay;
        }
    } else
    {
        d=ax-(ay>>1);
        for (;;) {
            if (x<iw && y<ih)
            {
                if (mode==MODE_CLEAR)
                    draw_image_pixel(x,y,im->pixel(x,y));
                else draw_image_pixel(x,y,color);
                if (mode==MODE_MODIFY) 
                    im->putpixel(x,y,color);
            }
            if (y==y2) return ;
            if (d>=0) { x+=sx; d-=ay; }  y+=sy; d+=ax;
        }
    }
}


imitem **painter::load(char *filename, short &total, palette *&new_pal)
{
    jFILE fp(filename,"rb");
    char sig[8],name[70];
    spec_entry *e;
    imitem **ima;
    image **imp;
    
    total=0;
    int i;
    if (fp.open_failure()) {
        set_error(imFILE_NOT_FOUND);
        return NULL;
    }
    else {
        fp.read(sig,8);
        sig[7]=0;
        if (!strcmp(sig,SPEC_SIGNATURE)) {
            fp.seek(0,0);
            spec_directory d(&fp);
            int j;
            for (j=0;j<d.total;j++) {
                e = d.entries[j];
                if (e->type==SPEC_IMAGE || e->type==SPEC_FORETILE || 
                    e->type==SPEC_BACKTILE || e->type==SPEC_CHARACTER2 || e->type==SPEC_CHARACTER ||
                    e->type==SPEC_PARTICLE)
                    total++;
            }
            ima = (imitem**) jmalloc(total * (sizeof(imitem*)), "image array");
            i = 0;
            for (j = 0; j < d.total; j++) {
                e=d.entries[j];	
                if (e->type==SPEC_IMAGE || e->type==SPEC_FORETILE || 
                    e->type==SPEC_BACKTILE || e->type==SPEC_CHARACTER2 || e->type==SPEC_CHARACTER ||
                    e->type==SPEC_PARTICLE) {
                    switch (e->type) {
                        case SPEC_IMAGE : ima[i++]=new imitem(e,&fp); break;
                        case SPEC_BACKTILE : ima[i++]=new backtile(e,&fp); break;
                        case SPEC_FORETILE : ima[i++]=new foretile(e,&fp); break;
                        case SPEC_CHARACTER :
                        case SPEC_CHARACTER2 : ima[i++]=new figure(e,&fp); break;
                        case SPEC_PARTICLE : ima[i++]=new particle(e,&fp); break;
                    }
                }
                else if (e->type==SPEC_PALETTE) {
                    if (new_pal)
                        delete new_pal;
                    new_pal = new palette(e, &fp);
                }
            }
        }
        else {
            switch (tell_color_size(filename)) {
            case 0 :
            case 8 : {
                    if (new_pal) {
                        delete new_pal;
                        new_pal=NULL;
                    }
                    
                    load_any(filename,imp,new_pal,total,pal);
                    if (total<1) return NULL;
                    ima=(imitem **)jmalloc(sizeof(imitem *)*total,"image array");
                    if (total==1) {
                        strcpy(name,filename);
                        ima[0]=new imitem(imp[0],name);
                    }
                    else {
                        for (i=0;i<total;i++) {
                            sprintf(name,"%s-%d",filename,i);
                            ima[i]=new imitem(imp[i],name);
                        } 
                    }
                } break;
            case 24 : {
                    image24 *im24 = load_any24(filename);
                    if (im24) {
                        ima=(imitem **)jmalloc(sizeof(imitem **),"image array");	     
                        ima[0]=new imitem(im24->dither(pal),filename);
                        total = 1;
                        delete im24;
                    } break;  
                }
            }     
        }
    }
    return ima;
    
    
}



void painter::delete_image(char *mark)
{
    imitem **new_editim;
    short i, total_deleted=0,old_cur=cur_image,j;
    if (!num_images) return ;
    
    while (*mark==' ') mark++;
    
    if (!strcmp(mark,""))
    {
        add_undo(cur_image,undo_delete);
        delete editim[cur_image];    
        editim[cur_image]=NULL;    
        total_deleted=1;        
    }
    else
    {    
        int num;
        for (num=get_next_mark(mark);num!=-1;num=get_next_mark(mark,num))
        {
            if (num<num_images && editim[num])
            {	
                add_undo(num,undo_delete);
                delete editim[num];  
                editim[num]=NULL;
                total_deleted++;	
            }
        }    
    }
    if ((num_images-total_deleted)==0)
    {
        jfree(editim);
        editim=NULL;
        old_cur=-1;    
    }
    else
    {    
        new_editim=(imitem **) jmalloc(sizeof(imitem *)*(num_images-total_deleted),"new im array");
        for (i=0,j=0;j<num_images;j++)
        {      
            if (editim[j])
            {
                new_editim[i]=editim[j];
                i++;
            }
            else if (j<=cur_image)
            {
                cur_image--;	
                if (cur_image<0) 
                { set_current(0); 
                old_cur=-1; 
                }	
            }    
        }  
        jfree(editim);
        editim=new_editim;  
    }
    num_images-=total_deleted;
    if (cur_image>=num_images)
        set_current(num_images-1);
    
    if (old_cur!=cur_image)
    {    
        if (num_images)
            draw_image(current_image());
        else 
            draw_image(NULL);
    }  
}

void painter::change_zoom(char *st, event &ev)
{
    short x1,y1,x2,y2,zx,zy,d=0,m,old_zoom=zoom,rel=0;
    int x;
    char word[50];
    
    if (sscanf(st,"%s",word))
        if (!strcmp(word,"relative"))
        {
            rel=zoom;
            st+=9;
        }
        
        if (sscanf(st,"%d",&x))
        { 
            mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,zx,zy);
            image_area(x1,y1,x2,y2);
            zoom=rel+x;
            if (zoom<=0) { zoom=1; info("Cannot zoom out any more"); }
            if (zoom>old_zoom)
            {
                zoomx=zx-((x2-x1+1)/2)/zoom;
                zoomy=zy-((y2-y1+1)/2)/zoom; 
            }
            else
            {
                if (zoomx+(x2-x1)/zoom>current_image()->width())
                    zoomx=current_image()->width()-(x2-x1)/zoom;
                if (zoomy+(y2-y1)/zoom>current_image()->height())
                    zoomy=current_image()->height()-(y2-y1)/zoom;
            }
            d=1;
        }
        else if (!strcmp(st,"min") && (zoom>1 || zoomx!=0 || zoomy!=0)) 
        { zoomx=0; zoomy=0; zoom=1; d=1; }
        else if (!strcmp(st,"max"))
        { 
            m=zoom;
            image_area(x1,y1,x2,y2);
            if (zoomx!=0) d=1;
            if (zoomy!=0) d=1;
            zoomx=zoomy=0;
            
            zoom=(y2-y1)/current_image()->height();
            if ((x2-x1)/current_image()->width()<zoom)
                zoom=(x2-x1)/current_image()->width();
            if (zoom<1) zoom=1;
            if (m!=zoom) d=1;
        }
        if (zoomx<0) zoomx=0; 
        if (zoomy<0) zoomy=0;
        if (zoom<=0) 
        {
            zoom=1;
            info("Cannot zoom out anymore");
        }
        if (d)
            draw_image(current_image(),1); 
}


void painter::rm_dups()
{  
    image *i1,*i2;
    if (num_images)
    {
        int i,del,j;
        do
        {
            del=-1;
            for (i=0;del<0 && i<num_images;i++)
            {
                i1=editim[i]->im;
                for (j=i+1;del<0 && j<num_images;j++)
                {
                    i2=editim[j]->im;
                    int h=i2->height(),w=i2->width();
                    int equal=1,y;
                    
                    if ((i1->width()==w) && (i1->height()==h))
                    {
                        unsigned char *s1,*s2;
                        for (y=0;equal && y<h;y++)
                        {
                            s1=i1->scan_line(y);
                            s2=i2->scan_line(y);
                            
                            if (memcmp(s1,s2,w))
                                equal=0;
                        }
                    } else equal=0;
                    
                    if (equal) del=j;
                }
                if (del>=0)
                {
                    char st[100];
                    sprintf(st,"delete %d",del);
                    event ev;
                    do_command(st,ev);
                }      
            }
        } while (del>=0);    
    }
}


void painter::sample_scale(int width, int height)
{
    image *im=current_image(),*n;
    n=new image(width,height);
    int xc,yc,w=im->width(),h=im->height();
    float x,y;
    
    for (yc=0;yc<height;yc++) {
        unsigned char *sl=n->scan_line(yc);
        for (xc=0;xc<width;xc++,sl++) {
            x=xc*w/(float)width;
            y=yc*h/(float)height;
        }
    }
}


class fill_rect
{
public :
    short x,y;
    fill_rect *last;
    fill_rect(short X, short Y, fill_rect *Last)
    { x=X; y=Y; last=Last; }
} ;

void painter::fill_image(short x, short y)
{
    if (!num_images) return ;
    int color=current_color();
    unsigned char *sl,*above,*below;
    fill_rect *recs=NULL,*r;
    unsigned char fcolor;
    image *im=current_image();
    sl=im->scan_line(y);
    fcolor=sl[x];
    if (fcolor==color) return ;
    int x1=x,y1=y,x2=x,y2=y;
    add_undo(cur_image,undo_change);
    do
    {
        if (recs)
        { r=recs;
        recs=recs->last;
        x=r->x; y=r->y;
        delete r;
        if (y<y1) y1=y;
        if (y>y2) y2=y;
        }
        sl=im->scan_line(y);
        if (sl[x]==fcolor)
        {
            while (sl[x]==fcolor && x>0) x--;
            if (sl[x]!=fcolor) x++;
            if (x<x1) x1=x;
            if (y>0)
            {
                above=im->scan_line(y-1);
                if (above[x]==fcolor)
                { r=new fill_rect(x,y-1,recs);
                recs=r;  
                }
            }
            if (y<im->height()-1)
            {
                above=im->scan_line(y+1);
                if (above[x]==fcolor)
                { r=new fill_rect(x,y+1,recs);
                recs=r;
                }
            }
            
            
            
            do
            {
                sl[x]=color; 
                if (y>0)
                { above=im->scan_line(y-1);
                if (x>0 && above[x-1]!=fcolor && above[x]==fcolor)
                { r=new fill_rect(x,y-1,recs);
                recs=r;  
                }
                }
                if (y<im->height()-1)
                { below=im->scan_line(y+1);
                if (x>0 && below[x-1]!=fcolor && below[x]==fcolor)
                { r=new fill_rect(x,y+1,recs);
                recs=r;  
                }
                }
                x++;
            } while (sl[x]==fcolor && x<im->width());
            x--;
            if (x>x2) x2=x;
            
            if (y>0)
            {
                above=im->scan_line(y-1);
                if (above[x]==fcolor)
                { r=new fill_rect(x,y-1,recs);
                recs=r;
                }
            }
            if (y<im->height()-1)
            {
                above=im->scan_line(y+1);
                if (above[x]==fcolor)
                { r=new fill_rect(x,y+1,recs);
                recs=r;
                }
            }
        }
    } while (recs);
    draw_area(im,x1,y1,x2,y2);
}

class rev_node
{ public :
int x;
rev_node *next;
rev_node(int X, rev_node *Next) { x=X; next=Next; }
} ;

void painter::reverse(char *st)
{
    rev_node *s=NULL;
    int t=0;
    int i=get_next_mark(st);
    for (;i!=-1;i=get_next_mark(st,i)) 
        if (i>=0 && i<num_images) 
        {
            t++;
            s=new rev_node(i,s);
        }
        
        if (t)
        {
            int c=0;
            for (i=get_next_mark(st);c<t/2;i=get_next_mark(st,i),c++) 
            {
                if (i>=0 && i<num_images) 
                {
                    imitem *it=editim[i];
                    editim[i]=editim[s->x];
                    editim[s->x]=it;
                    rev_node *p=s;
                    s=s->next;
                    delete p;
                }
            }
            while (s)
            {
                rev_node *p=s;
                s=s->next;
                delete p;
            }
        } else info("no images in range");
}

int xcom(image *im)
{
    long c=0,t=0;
    for (int y=0;y<im->height();y++)
    {
        unsigned char *sl=im->scan_line(y);
        for (int x=0;x<im->width();x++,sl++)
        {
            if (*sl) 
            { 
                c+=x;
                t++;
            }
        }
    }
    if (t)
        return c/t;
    else return 0;
}

void painter::load_sequence(char *filebase, int set_palette, int still)
{
    jFILE *fp=NULL;
    int on=1,total=0,maxy=0,er=0,quit=0,first=-1;
    char nm[100];  
    
    for (on=1;on<10 && first==-1;on++)
    {
        sprintf(nm,"%s%04d.pcx",filebase,on);
        fp=new jFILE(nm,"rb");    
        if (!fp->open_failure()) first=on;
        delete fp; 
    }
    if (first==-1) return ;
    
    on=first;
    do
    {
        
        sprintf(nm,"%s%04d.pcx",filebase,on); on++;
        fp=new jFILE(nm,"rb");
        quit=fp->open_failure();
        delete fp;
        
        if (!quit)
        {
            int old_end=num_images;
            char msg[200];
            sprintf(msg,"Scanning %s\n",nm);
            info(msg);
            eh->flush_screen();
            palette *p=NULL;
            short t=0;
            image **im;
            er=load_any(nm,im,p,t);     
            if (t!=0) 
            {
                er=0;
                long x1,y1,x2,y2,fx,fy;
                find_min(im[0],x1,y1,x2,y2,0,fx,fy);
                if (y2>maxy) maxy=y2;
                for (int i=0;i<t;i++)
                    delete im[i];
                if (p) delete p;
                jfree(im);
                total++;
            }
            else quit=1;
        }
    } while (!quit);
    
    if (er)
    {
        info(imerr_messages[er]);
    } else
    {
        int oc=cur_image,lastx;
        int center;
        for (int i=first;i<=total+first-1;i++) {
            char nm[100];
            sprintf(nm,"%s%04d.pcx",filebase,i);
            merge(nm);
            long x1,y1,x2,y2,fx,fy;
            cur_image=num_images-1;
            find_min(editim[cur_image]->im,x1,y1,x2,y2,0,fx,fy);
            sprintf(nm,"clip %d %d %d %d",x1,y1,x2,maxy);
            event ev;
            do_command(nm,ev);
            do_command("type character",ev);
            do_command("set damage -1",ev);
            if (still) {
                if (i==1)	
                    center=xcom(editim[cur_image]->im)+x1;
                ((figure*)editim[cur_image])->xcfg = (unsigned char) (center - x1);
            }
            else {
                int xc=xcom(editim[cur_image]->im);
                ((figure *)editim[cur_image])->xcfg=xc;
                if (i!=1)
                    ((figure *)editim[cur_image-1])->advance=(x1+xc)-lastx;      
                lastx=x1+xc;
            }
        }
        cur_image=oc;
        
    }
}



void painter::load_tpal(char *st)
{
    FILE *fp=fopen(st,"rb");
    if (!fp)
    { info("Unable to open file for reading"); return ; }
    
    int r,g,b,i,x,fail=0;
    palette pal;
    for (i=0;!fail && i<256;i++)
    {
        if (fscanf(fp,"%d%d%d%d",&x,&r,&g,&b)!=4 || x!=i)
            fail=1;
        else
        {
            if (r<0) r=0; else if (r>255) r=255;
            if (g<0) g=0; else if (g>255) g=255;
            if (b<0) b=0; else if (b>255) b=255;
            pal.set(i,r,g,b);
        }
    }
    
    
    fclose(fp);
    palc->change_palette(&pal);
    
}

void painter::write_tpal(char *st)
{
    FILE *fp=fopen(st,"wb");
    if (!fp)
        info("Unable to open file for writing");
    else
    {
        int i;
        palette *p=palc->current_pal();
        uchar r,g,b;
        for (i=0;i<256;i++)
        {
            p->get(i,r,g,b);
            fprintf(fp,"%03d %03d %03d %03d\n",i,r,g,b);      
        }
        fclose(fp);
    }
}
