#include "macs.hpp"
#include "Include/id.h"
#include "input.hpp"
#include "Include/pcommand.h"
#include "Include/paint.h"

/*

 Data types :
 
  r - range
  s - string
  d - integer
  c - color number [0-255]
  i - image number (0-max_images)
  t - image type [image,foretile,backtile,character]
  
   
    Prompt format
    
     command [type=Prompt name, ...]
     ex. mouse_shape [i=Image=this, d=X center=0, d=Y center=0]
     
*/

int type_field_length(char type)
{
    switch (type)
    {
    case 'r' : return 20; break;
    case 's' : return 30; break;
    case 'd' : return 6; break;
    case 'c' : return 4; break;
    case 'i' : return 5; break;
    case 't' : return 10; break;     
    default :
        CONDITION(0,"Bad type");
    }
    return 5;   // doesn't happen
}

ifield *make_cfield(int cx,int cy,char *prompt,char *def, int type,int id)
{
    char ifd[200];
    
    int l=type_field_length(type);
    memset(ifd,'*',l);
    ifd[l]=0;
    
    return new text_field(cx,cy,id,prompt,ifd,def,NULL);
}

char *eval_pvalue(char *&c)
{
    static char v[100];
    char *p;
    p=v;
    while (*c && *c!=']' && *c!=',') {
        *p=*c;
        c++;
        p++;
    }
    while (*(p-1)==' ') p--;
    *p=0;
    if (*c && *c!=']') c++;
    return v;
}

jwindow *make_cdialog(char *prompt, window_manager *eh)
{
    char pt[100],*p,cm[100],def[100];
    int max_h=7+eh->font()->height(),max_w=0;
    int cy=max_h+WINDOW_FRAME_TOP,cx=WINDOW_FRAME_LEFT+5;
    ifield *first=NULL,*last;
    char *c=prompt;
    for (p=cm;*c!='[' && *c;c++,p++) { *p=*c; } 
    if (*c) c++;
    if (*(p-1)==' ') p--;  *p=0;
    
    int id=PCMD_DIAG_FIRST;
    while (*c && *c!=']')
    {
        int type=*c;
        int i_size=type_field_length(type); c++;
        CONDITION(*c=='=',"Expecting ="); c++;
        
        int p_size=0;
        p=pt; for (;*c && *c!=',' && *c!=']' && *c!='=';c++,p_size++,p++) { *p=*c; } 
        while (*(p-1)==' ') p--;
        *p=0; 
        if (*c=='=')
        {
            c++;
            strcpy(def,eval_pvalue(c));
        }
        else
        {
            def[0]=0;
            if (*c && *c!=']') c++;
        }
        
        while (*c==' ') c++;
        max_h+=eh->font()->height()+7;   
        if (max_w<(i_size+p_size+1)*eh->font()->width())
            max_w=(i_size+p_size+1)*eh->font()->width();
        
        ifield *np=make_cfield(cx,cy,pt,def,type,id++);
        cy+=eh->font()->height()+7;   
        
        if (first)
            last->next=np;
        else
            first=np;
        last=np;      
    }
    if (!first) return NULL;
    cy+=2; max_h+=2;
    last->next=new button(cx,cy,PCOMMAND_OK," OK  ",
        new button(cx+eh->font()->width()*7+8,cy,PCOMMAND_CANCEL,"CANCEL",NULL)); 
    if (eh->font()->width()*14+20>max_w)
        max_w=eh->font()->width()*14+20;
    cy+=eh->font()->height()+10;
    max_h+=eh->font()->height()+10;
    
    last=last->next->next; 
    last->next=new info_field(cx+max_w/2-strlen(cm)*eh->font()->width()/2,WINDOW_FRAME_TOP+3,
        0,cm,NULL);
    
    max_w+=WINDOW_FRAME_RIGHT+WINDOW_FRAME_TOP+5;
    max_h+=WINDOW_FRAME_TOP+WINDOW_FRAME_BOTTOM;
    
    return eh->new_window(max_w/2-xres/2,max_h/2-yres/2,max_w,max_h,first,"command");  
}

class pm_command
{
public :
    pm_command *next;
    char *cmd;
    int id;
    pm_command(char *command, int Id, pm_command *Next)
    {
        id=Id;
        next=Next;
        cmd=strcpy((char *)jmalloc(strlen(command)+1,"pm_command"),command);
    } ;
    ~pm_command() { jfree(cmd); }
} ;


int pm_cmd_handler::add_id(char *command)
{
    first=new pm_command(command,id_on,first);
    return id_on++;
}

/*char *p_cmds[]= {

 
  
   
    
     
      
       
        
         
          
           
            "solid [c=color]",
            "select [d=x1, d=y1, d=x2, d=y2]",
            "swap [i=image 1, i=image 2]",
            "clip [d=left X, d=top Y, d=right X, d=bottom Y]",
            "tile",
            "go [i=image #]",
            "blend [d=with image #, d=amount (0..100)]",
            "blur [d=x1, d=y1, d=x2, d=y2, d=blur_amount]",
            "blur_color [d=amount, d=color #]",
            "anti [d=x1, d=y1, d=x2, d=y2]",
            "morph [d=aneal_steps=200, d=frames=10, d=blur=10000, i=start image #,"
            "i=end image #, i=start hint (optional), i=end hint (optional)]",
            "undo",
            "undos [d=# of undos to keep=3]",
            "renumber [d=starting_number=0, s=format string=%04d, r=range=all]",
            "crop",
            "crop_center",
            "upper_name",
            "lower_name",
            "prefix_name [s=prefix]",
            "suffix_name [s=suffix]",
            "remap [c=remap to, s=colors to remap]",
            "setkey [s=keyname, s=command]",
            "delete [r=range=this]",
            "rm_dups",
            "clear_unsed",
            "grow [d=xamount=1, d=yamount=1]",
"mouse_shape [i=image #=this, d=X center=0, d=Y center=0]" };*/

void pm_cmd_handler::parse(char *pc, jwindow *j)
{
    int i=PCMD_DIAG_FIRST;
    char cmd[300],*p,tmp[100];
    for (p=cmd;*pc && *pc!='[';pc++,p++) *p=*pc;
    while (*(p-1)==' ') p--;
    
    if (*pc)
    {
        while (*pc && *pc!=']')
        {
            *p=' '; p++;   
            pc++;
            while (*pc==' ') pc++;
            strcpy(tmp,j->read(i++));
            for (char *c=tmp;*c;c++,p++) { *p=*c; }
            while (*(p-1)==' ') p--;
            while (*pc!=',' && *pc && *pc!=']') pc++;
        }    
    } 
    *p=0;
    event ev;
    paint->do_command(cmd,ev);
}

int pm_cmd_handler::handle_event(event &ev, int ok_to_pull)
{
    if (ok_to_pull && pm->handle_event(ev, screen)) {
        if (refresh)
            draw(screen,eh);
        return 1;
    }
    else if (ev.type==EV_MESSAGE) {
        if (pm_diag && ev.message.id==PCOMMAND_OK) {
            parse(current->cmd,pm_diag);
            eh->close_window(pm_diag);
            pm_diag=NULL;
        }
        else if (pm_diag && ev.message.id==PCOMMAND_CANCEL) {
            eh->close_window(pm_diag);
            pm_diag=NULL;
        }
        else if (ev.message.id>=PCOMMAND_FIRST && ev.message.id<id_on && !pm_diag) {
            pm_command *p=first;
            for (;p && p->id!=ev.message.id;p=p->next);
            if (p) {
                pm_diag=make_cdialog(p->cmd,eh);
                if (!pm_diag)   // if command doesn't need any arguments, do it now
                    paint->do_command(p->cmd,ev);
                current=p;
            }
            return 1;
        }
    }
    return 0;
}

void pm_cmd_handler::note_resize()
{
    pm->move(0,yres-eh->font()->height()-5);
}

pm_cmd_handler::pm_cmd_handler() 
{ 
    first=NULL;  
    id_on=PCOMMAND_FIRST; 
    pm_diag=NULL;
    refresh=1;
    pmenu_item *cm=new pmenu_item("Commands",new psub_menu(
        new pmenu_item(add_id("with [r=range=this, s=command]"),"Do command",NULL,-1,
        new pmenu_item(add_id("at [d=x, d=y, s=command]"),"At..command",NULL,-1,
        new pmenu_item(add_id("select [d=x1, d=y1, d=x2, d=y2]"),"Set select",NULL,-1,
        new pmenu_item(add_id("swap [i=item 1 #, i=item 2 #]"),"Swap 2 items",NULL,-1,
        new pmenu_item(add_id("reverse [r=range]"),"Reverse seqeunce",NULL,-1,
        new pmenu_item(add_id("go [i=image #]"),"Go to..",NULL,-1,
        new pmenu_item(add_id("tile"),"Tile",NULL,-1,
        new pmenu_item(add_id("morph [d=aneal_steps=200, d=frames=10, d=blur=10000, i=start image #,"
        "i=end image #, i=start hint (optional), i=end hint (optional)]"),"Morph",NULL,-1,
        new pmenu_item(0,       NULL, NULL,       -1,
        new pmenu_item(add_id("palette toggle"),"Palette window",&pal_win_on,-1,
        new pmenu_item(0,       NULL, NULL,       -1,
        new pmenu_item(add_id("undo"),"Undo",NULL,-1,
        new pmenu_item(add_id("undos [d=# of undos to keep=3]"),"Set # undo's",NULL,-1,
        new pmenu_item(add_id("renumber [d=starting_number=0, s=format string=%04d, r=range=all]"),
        "renumber",NULL,-1,
        new pmenu_item(add_id("setkey [s=keyname, s=command]"),"Set key",NULL,-1,
        new pmenu_item(add_id("rm_dups"),"Remove duplicates",NULL,-1,
        new pmenu_item(add_id("clear_unused"),"Minimize Palette",NULL,-1,
        new pmenu_item(add_id("mouse_shape [i=image #=this, d=X center=0, d=Y center=0]"),
        "Set mouse shape",NULL,-1,
        NULL)))))))))))))))))),NULL),NULL);
    
    
    pmenu_item *em=new pmenu_item("Effects",new psub_menu(
        new pmenu_item(add_id("remove singles"),"Remove single pixels",NULL,-1,
        new pmenu_item(add_id("make color strip"),"Make color strip",NULL,-1,
        new pmenu_item(add_id("solid [c=color]"),"Turn to mask",NULL,-1,
        new pmenu_item(add_id("repeat [d=# of rows to blend?]"),"Make repeatable",NULL,-1,
        new pmenu_item(add_id("animate [r=range]"),"Animate",NULL,-1,
        new pmenu_item(0,       NULL,        NULL,-1,
        new pmenu_item(add_id("blend [d=with image #, d=amount (0..100)]"),"Blend",NULL,-1,
        new pmenu_item(add_id("blur [d=x1, d=y1, d=x2, d=y2, d=blur_amount]"),"Blur",NULL,-1,
        new pmenu_item(add_id("blur_color [d=amount, d=color #]"),"Blur color",NULL,-1,
        new pmenu_item(add_id("anti [d=x1, d=y1, d=x2, d=y2]"),"Antialase",NULL,-1,
        new pmenu_item(add_id("light [d=red (0..200), d=green (0..200), d=blue (0..200)]"),
        "Saturation",NULL,-1,
        new pmenu_item(add_id("gravel [d=diff max=5]"),"Gravel",NULL,-1,
        new pmenu_item(add_id("invert [d=red?=1, d=green?=1, d=blue?=1]"),"Color Invert",NULL,-1,
        new pmenu_item(add_id("remap [c=remap to, s=colors to remap]"),"Remap colors",NULL,-1,
        new pmenu_item(add_id("bevel [d=color difffernce=5]"),"Bevel object",NULL,-1,
        new pmenu_item(add_id("invert_pal [d=red (1 is yes/0 is no)=1, d=green (1 is yes/0 is no)=1, d=blue (1 is yes/0 is no)=1"),
        "Invert Palette",NULL,-1,
        new pmenu_item(add_id("tint_pal [d=red (0..200%), d=green (0..200%), d=blue (0..200%),"
        " d=5 bit? (1 is yes)=0]"),
        "Tint Palette",NULL,-1,
        new pmenu_item(add_id("black_white"),"Black & White Palette",NULL,-1,
        NULL)))))))))))))))))),NULL),cm);
    
    psub_menu *scale_menu=new psub_menu(
        new pmenu_item(add_id("scale [d=new width, d=new height]"),"scale (abs size)",NULL,-1,
        new pmenu_item(add_id("scalex [d=multiplyer=1, d=divisor=1]"),"scale (factor)",NULL,-1,
        new pmenu_item(add_id("ascale [d=new width, d=new height]"),
        "aliased scale (abs size)",NULL,-1,
        new pmenu_item(add_id("ascalex [d=multiplyer=1, d=divisor=1]"),
        "aliased scale (factor)",NULL,-1,NULL)))),NULL);
    
    psub_menu *new_menu=new psub_menu(
        new pmenu_item(add_id("new [d=width, d=height, s=name]"),"New Image",NULL,-1,
        new pmenu_item(add_id("newf [d=width, d=height, s=name]"),"New foretile",NULL,-1,
        new pmenu_item(add_id("newb [d=width, d=height, s=name]"),"New backtile",NULL,-1,
        new pmenu_item(add_id("newc [d=width, d=height, s=name]"),"New character",NULL,-1,
        new pmenu_item(add_id("news"),"New (selected)",NULL,-1,NULL))))),NULL);
    
    
    psub_menu *name_menu=new psub_menu(
        new pmenu_item(add_id("name [s=name]"),"Name",NULL,-1,
        new pmenu_item(add_id("upper_name"),"Upper case name",NULL,-1,
        new pmenu_item(add_id("lower_name"),"Lower case name",NULL,-1,
        new pmenu_item(add_id("prefix_name [s=prefix]"),"Prefix name",NULL,-1,
        new pmenu_item(add_id("suffix_name [s=suffix]"),"Suffix name",NULL,-1,
        NULL))))),NULL);
    
    psub_menu *reduce_menu=new psub_menu(
        new pmenu_item(add_id("chop [d=width, d=height]"),"Chop up",NULL,-1,
        new pmenu_item(add_id("clip [d=left X, d=top Y, d=right X, d=bottom Y]"),"Clip",NULL,-1,
        new pmenu_item(add_id("crop"),"Crop",NULL,-1,
        new pmenu_item(add_id("crop_center"),"Crop centered",NULL,-1,NULL)))),NULL);
    
    pmenu_item *im=new pmenu_item("Image",new psub_menu(
        new pmenu_item(add_id("copy"),"Copy",NULL,-1,
        new pmenu_item(add_id("copy_to [i=copy_to]"),"Copy to",NULL,-1,
        new pmenu_item(add_id("flip x"),"Flip X (horz)",NULL,-1,
        new pmenu_item(add_id("flip y"),"Flip Y (vert)",NULL,-1,
        new pmenu_item(add_id("rotate90"),"Rotate 90 deg",NULL,-1,
        new pmenu_item("Scale",scale_menu,	     
        new pmenu_item(add_id("delete [r=range=this]"),"Delete",NULL,-1,
        new pmenu_item("Reduce",reduce_menu,
        new pmenu_item(add_id("grow [d=xamount=1, d=yamount=1]"),"Grow",NULL,-1,
        new pmenu_item(0,       NULL,        NULL,-1,
        new pmenu_item("Name",name_menu,
        new pmenu_item("New",new_menu,
        NULL)))))))))))),NULL),em);
    
    
    
    pmenu_item *fm=new pmenu_item("File",new psub_menu(
        new pmenu_item(add_id("load palette [s=filename]"),"Load Palette",NULL,-1,
        new pmenu_item(add_id("merge [s=filename]"),"Load graphics..",NULL,-1,
        new pmenu_item("Import weird images",new psub_menu(
        new pmenu_item(add_id("merge_raw_gray [d=width, d=height, s=filename=.gry]"),     
        "Raw 8 bit gray scale",NULL,-1,NULL),NULL),
        
        new pmenu_item(add_id("read [s=filename]"),"Execute script file",NULL,-1,
        new pmenu_item(add_id("write_tpal [s=filename]"),"Write palette textfile",NULL,-1,
        new pmenu_item(add_id("load_tpal [s=filename]"),"Load palette textfile",NULL,-1,
        
        new pmenu_item(add_id("save"),"save (s)",NULL,-1,
        new pmenu_item(add_id("saveas [s=filename]"),"Save as",NULL,-1,
        new pmenu_item("Export",new psub_menu(
        new pmenu_item(add_id("write_ppm [s=filename=.ppm]"),     "(PPM) Portable Pixmap",NULL,-1,
        new pmenu_item(add_id("write_pcx [s=filename=.pcx]"),     "(PCX) PC Paintbrush",NULL,-1,
        new pmenu_item(add_id("write_bmp [s=filename=.bmp]"),     "(BMP) Microsoft Bitmap",NULL,-1,
        new pmenu_item(add_id("write_raw_gray [s=filename=.gry]"),"Raw 8 bit gray scale",NULL,-1,
        new pmenu_item(add_id("write_raw_color [s=filename=.clr]"),"Raw 24 bit color",NULL,-1,
        new pmenu_item(add_id("write_include [s=filename=.c]"),   "(.C) C language definition ",
        NULL,-1,
        new pmenu_item(add_id("write_property [s=filename=.prp, r=range=all]"),
        "(PRP) Property text file",
        NULL,-1,NULL))))))),NULL),
        new pmenu_item(add_id("quit"),"Quit",NULL,-1,NULL)))))))))),
        NULL),im);
    
    
    pm = new pmenu(0, yres - wm->font()->height()-5, fm, screen);
}


void pm_cmd_handler::draw(image *screen, window_manager *wm)
{
    pm->draw(screen, 1);
    refresh=0;
}


char pm_field_type(char *s, int x)
{
    while (*s && *s!='[') s++;
    if (*s==0) return 0;
    s++;
    while (x)
    {
        while (*s && *s!=',') s++;
        if (*s) s++;
        while (*s && *s==' ') s++;
        x--;
        if (*s==0) return 0;
    }
    return *s;
}

void pm_cmd_handler::notify_image_click(int x)
{
    char tmp[20];
    if (pm_diag && current)
    {
        ifield *cur=pm_diag->inm->current();
        int type=pm_field_type(current->cmd,cur->id-PCMD_DIAG_FIRST);
        if (type=='i')
        {
            sprintf(tmp,"%d",x);
            ((text_field *)cur)->change_data(tmp,strlen(tmp),1,pm_diag->screen);
        }
    }
}

void pm_cmd_handler::notify_palette_click(int x)
{
    char tmp[20];
    if (pm_diag && current)
    {
        ifield *cur=pm_diag->inm->current();
        int type=pm_field_type(current->cmd,cur->id-PCMD_DIAG_FIRST);
        if (type=='c')
        {
            sprintf(tmp,"%d",x);
            ((text_field *)cur)->change_data(tmp,strlen(tmp),1,pm_diag->screen);
        }
    }
}