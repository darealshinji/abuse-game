#include "Include/paint.h"
#include "Include/edit.h"
#include "Include/id.h"

#include "xwdread.hpp"
#include "pcxread.hpp"
#include "ppmread.hpp"

#include "Include/debug.h"
#include "timing.hpp"
//#include "monoprnt.hpp"
#include "jrand.hpp"
#include "Include/action.h"
#include "Include/toolinit.h"
#include <ctype.h>
//#include <unistd.h>
#include "Include/id.h"
#include "Include/pcommand.h"
#include "Include/logo.h"
#include "Include/sat.h"
#include "Include/scale.h"
#include "guistat.hpp"
#include "Include/loader.h"
#include "Include/include.h"

#include "IO.h"

#ifdef __WATCOMC__
extern char *expand_filename(char *name);
extern void expand_argv(int &argc, char **&argv);
#endif

int empty_start=0;
pm_cmd_handler *pch=NULL;
window_manager *eh=NULL;
paint_pal *palc=NULL;
JCFont *font;
painter *paint;
cursor paint_cursor;
void light(select_line *s, image *im, palette *pal, color_filter *f,char *st);
void gravel(select_line *s, image *im, palette *pal, color_filter *f,char *st);
void rgb_invert(select_line *s, image *im, palette *pal, color_filter *f,char *st);

int tile_state(int state)
{ return state==MODE_TILE; }

int index_state(int state)
{ if (state==INDEX_PICK || state==ANIMATE_PICK) return 1; else return 0; }

int animate_state(int state)
{ if (state==ANIMATING) return 1; else return 0; }

int menu_state(int state)
{
    if (state!=INDEX_PICK && state!=ANIMATING)
        return 1;
    else return 0;
}

int edit_state(int state)
{  
    switch (state)
    {
    case MODE_PIXEL_DRAW :
    case MODE_IMAGE_DRAW :
    case MODE_ZOOM_IN :
    case MODE_ZOOM_OUT :
    case MODE_FILL :
    case MODE_LINE :
    case MODE_CIRCLE :
    case MODE_RECT :
    case MODE_BAR :
    case EDIT_MODE :
    case MODE_AREA_SELECT :
    case MODE_POLY_SELECT :
        return 1;
    default : return 0;
    }
}

void painter::change_state(int new_state)   // does any action nessary to switch to new state
{
    int x,y,nrd=0;
    if (eh)
        eh->mouse_status(x,y,state_enter_button_status);
    
    if (index_state(new_state) && !index_state(state))
    {    
        paint_cursor.reset();
        nrd=1;
    }
    if (!index_state(new_state) && index_state(state))
    {
        if (last_index)
        { 
            set_current(last_index->image_num);
            draw_image(current_image(),1);
            Index *old;
            while (indexes)
            { 
                old=indexes;
                indexes=indexes->next;
                delete old;
            }
            indexes=NULL;	      
        }
        nrd=1;
    }
    
    if (animate_state(new_state) && !animate_state(state))
    {
        if (num_images && max_ani)
        {
            nrd=1;
            int by=WINDOW_FRAME_TOP,bh=eh->font()->height()+10;      
            animate_control_window=eh->new_window
                (-1,0,-1,-1,
                new button(WINDOW_FRAME_LEFT,by,PAINT_ANIMATE_SPEED_UP,"FASTER",
                new button(WINDOW_FRAME_LEFT,by+bh,PAINT_ANIMATE_SLOW_DOWN,"SLOWER",
                new button(WINDOW_FRAME_LEFT,by+bh*2,PAINT_ANIMATE_DONE,"DONE",
                new button(WINDOW_FRAME_LEFT,by+bh*3,PAINT_ANIMATE_FORWARD,"FORWARD",
                new button(WINDOW_FRAME_LEFT,by+bh*4,PAINT_ANIMATE_BACKWARD,"BACKWARD",NULL))))));
            last_animate.x1=-1000;      
            ani_frame=0;      
            ani_dir=1;  
            ani_sleep=100;      
        } else change_state(EDIT_MODE);
    } else if (animate_state(state) && !animate_state(new_state))  
    {
        eh->close_window(animate_control_window);    
        animate_control_window=NULL;
        nrd=1;
    } else if (tile_state(new_state)!=tile_state(state))
        nrd=1;
    
    state=new_state;
    if (nrd) draw();
}

void painter::mouse_change_state(int new_state) 
{ 
    next_state=new_state; 
    change_state(MOUSE_STATE_CHANGE); 
}

void painter::change_cursor()
{
    paint_cursor.restore();
    paint_cursor.draw();
}

void painter::set_current_color(short color) 
{ 
    if (palc) palc->set_color(color); 
}

void painter::clear_animate_frames() 
{ if (max_ani) 
{ jfree(ani_order); max_ani=0; ani_order=NULL; 
} 
if (animate_pick_window)
animate_pick_window->screen->bar(WINDOW_FRAME_LEFT,
                                 animate_pick_window->y2()-15,
                                 animate_pick_window->x2(),
                                 animate_pick_window->y2(),eh->medium_color());  
}

void painter::print_animation_sequence(image *screen, int x, int y)
{
    char msg[200];   
    int c;
    msg[0]=0;	    
    for (c=0;c<max_ani;c++)
    {
        char tmp[8];	      
        sprintf(tmp,"%d ",ani_order[c]);
        strcat(msg,tmp);
    }              
    screen->bar(x,y,screen->width()-4,y+8,eh->medium_color());  
    eh->font()->put_string(screen,x,y,msg);	    
}


void painter::add_animate_frame(int frame)
{ 
    max_ani++; 
    ani_order=(int *)jrealloc(ani_order,max_ani*sizeof(int),"ani order"); 
    ani_order[max_ani-1]=frame;  
}

void painter::add_undo(int image_number, int action, palette *new_pal)
{
    if (undo_levels)
    {
        if (undo_levels==undos.number_nodes())
        {
            undo_marker *m=(undo_marker *)undos.first();
            undos.unlink((linked_node *)m);
            delete m;      
        }
        undos.add_end((linked_node *) new undo_marker(image_number,action,new_pal));    
    }  
}

void painter::set_undo(int levels)                         //  0=off
{
    while (undos.number_nodes()>levels)
    {
        undo_marker *m=(undo_marker *)undos.first();
        undos.unlink((linked_node *)m);
        delete m;          
    }  
    undo_levels=levels;  
}


void painter::set_current(int x)
{
    if (!num_images) return ;
    if (cur_image!=x)
    {    
        edit->close_window();    // edits on the other image are invalid now.. 
        last_on=cur_image;    
        cur_image=x;
    }  
}

int painter::closest_color(int r, int g, int b, palette *pal)
{
    unsigned char R,G,B;
    int c=0,d=0x100000,i,nd;
    for (i=255;i>=0;i--)
    {
        pal->get(i,R,G,B);
        nd=((int)r-(int)R)*((int)r-(int)R)+
            ((int)g-(int)G)*((int)g-(int)G)+
            ((int)b-(int)B)*((int)b-(int)B);
        if (nd<d)
        { c=i; d=nd; }
    }
    return c;
}

void painter::draw_image_pixel(int x, int y, int c)
{
    int x1,y1,x2,y2;
    if (x<zoomx || y<zoomy) return;
    x1=im_area.x1+(x-zoomx)*zoom;
    y1=im_area.y1+(y-zoomy)*zoom;
    x2=x1+zoom-1;
    y2=y1+zoom-1;
    if (x2<=im_area.x2 && y2<=im_area.y2)
    {
        for (int yy=y1;yy<=y2;yy++) 
            memset(screen->scan_line(yy)+x1,c,x2-x1+1);
        if (pix_dirty.x1==-1)
        {
            pix_dirty.x1=x1;
            pix_dirty.y1=y1;
            pix_dirty.x2=x2;
            pix_dirty.y2=y2;
        } else
        {
            int min_x1,max_x2,min_y1,max_y2;
            min_x1=min(pix_dirty.x1,x1);
            max_x2=max(pix_dirty.x2,x2);
            min_y1=min(pix_dirty.y1,y1);
            max_y2=max(pix_dirty.y2,y2);
            if ((x2-x1+1)*(y2-y1+1)+(pix_dirty.x2-pix_dirty.x1+1)*(pix_dirty.y2-pix_dirty.y1+1)+50>
                (max_x2-min_x1+1)*(max_y2-min_y1+1))
            {
                screen->add_dirty(pix_dirty.x1,pix_dirty.y1,pix_dirty.x2,pix_dirty.y2);
                pix_dirty.x1=x1;
                pix_dirty.y1=y1;
                pix_dirty.x2=x2;
                pix_dirty.y2=y2;
            } else
            {
                pix_dirty.x1=min_x1;
                pix_dirty.y1=min_y1;
                pix_dirty.x2=max_x2;
                pix_dirty.y2=max_y2;
            }
        }
        
        //    screen->bar(x1,y1,x2,y2,c);
    }
}

void painter::change_palette(palette *new_pal)
{
    palc->change_palette(new_pal);
}



void painter::paste(event ev, int put_mode)
{
    short rx,ry;  
    if (num_images && select_buffer)
    { 
        add_undo(cur_image,undo_change);
        mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,rx,ry);
        int dx=rx-select_buffer->width()/2;
        int dy=ry-select_buffer->height()/2;
        select_buffer->put_image(current_image(),dx,dy,put_mode);
        draw_area(current_image(),dx,dy,dx+select_buffer->width()-1,dy+select_buffer->height()-1);
    }
}


void painter::show_current()
{
    char msg[100],msg2[100];
    if (num_images)
    {
        sprintf(msg,"(%d of %d), %dx%d, zoom = X %d",cur_image,num_images,
            current_image()->width(),current_image()->height(),zoom);
        sprintf(msg2,"%s : '%s'",spec_types[current_item()->spec_type()],current_item()->name());
    }
    else 
    {
        sprintf(msg,"no images defined (use new command)");
        strcpy(msg2,"");
    }
    info(msg,msg2);
}

void painter::redraw_sel_area()
{
    if (!sel_mask) return ;
    int x1=10000,x2=0,y1=10000,y2=0;
    for (select_line *sp=sel_mask;sp;sp=sp->next)
    {
        if (sp->y>y2) y2=sp->y;
        if (sp->y<y1) y1=sp->y;
        if (sp->x1<x1) x1=sp->x1;
        if (sp->x2>x2) x2=sp->x2;   
    }
    draw_area(current_image(),x1,y1,x2,y2);
}

void painter::do_command(char *st, event &ev)
{
    char cmd[150],filename[150],cmd2[200],*sp;
    short r,x,y,x1,y1,x2,y2, c;
    int p[8];
    image *im;
    imitem **images,*imi;
    
    palette *pal2;
    
    add_history(st);
    
    
    r=sscanf(st,"%s%d%d%d%d%d%d%d%d",cmd,&p[0],&p[1],&p[2],&p[3],&p[4],&p[5],&p[6],&p[7]);
    
    while (*st && *st!=' ')        // skip past the command
        st++; 
    if (*st) st++;    
    
    if (!strcmp(cmd,"with"))
    {
        if (!num_images) return ;    
        if (!sscanf(st,"%s",cmd2))
            info("with expects a range to follow");
        else
        {
            while (*st && *st!=' ') st++; 
            while (*st && *st==' ') st++;
            c=cur_image;                  
            int n=num_images-1;
            int tot=0,on=0;
            for (x=get_next_mark(cmd2);x!=-1;x=get_next_mark(cmd2,x,n),tot++);
            if (!tot) return;
            stat_man->push(cmd,NULL);      
            for (x=get_next_mark(cmd2);x!=-1;x=get_next_mark(cmd2,x,n))
            {
                stat_man->update(on*100/tot); on++;
                if (x>=0 && x<=n)
                {
                    set_current(x);	
                    if (cur_image>=0 && cur_image<num_images)
                        do_command(st,ev);  
                }
            }      
            stat_man->pop();
            set_current(c);      
        }    
    }
    
    
    if (!strcmp(cmd,"zoom"))
        change_zoom(st,ev);
    
    if (!strcmp(cmd,"clear_unused"))
        clear_unused();
    
    if (!strcmp(cmd,"blend"))
        blend(st);
    
    
    if (!strcmp(cmd,"at"))
    {
        if (r<3) info("at expects x & y");
        else
        {
            while (!isdigit(*st)) st++; st++;
            while (!isdigit(*st)) st++; 
            while (*st!=' ') st++;
            while (*st==' ') st++;
            x1=ev.mouse_move.x;
            y1=ev.mouse_move.y;      
            ev.mouse_move.x=p[0];
            ev.mouse_move.y=p[1];      
            do_command(st,ev); 
            ev.mouse_move.x=x1;
            ev.mouse_move.y=y1;      
        }    
    }
    
    if (!strcmp(cmd,"black_white"))
    {
        palette *p=palc->current_pal()->copy();
        p->black_white();
        palc->change_palette(p);
        delete p;
    }
    
    if (!strcmp(cmd,"setkey"))
        setkey(st);  
    if (!strcmp(cmd,"set"))
        set(st);
    
    if (!strcmp(cmd,"calc_filter"))
        palc->get_filter();
    
    if (!strcmp(cmd,"remap"))
    {    
        if (!num_images) return ;
        add_undo(cur_image,undo_change);   
        remap(st);
    }
    if (!strcmp(cmd,"refresh"))
    {
        if (!strcmp(st,"off"))
            no_draw_mode=1;
        else no_draw_mode=0;  
    }  
    
    if (!strcmp(cmd,"undo"))
    {    
        undo();
        if (num_images)
            draw_image(current_image(),1);
    }  
    if (!strcmp(cmd,"undos"))
        set_undo(p[0]);
    
    if (!strcmp(cmd,"name")) {
        if (!num_images)
            return;
        add_undo(cur_image, undo_change);
        current_item()->set_name(st); 
    }
    if (!strcmp(cmd,"read")) {
        if (sscanf(st,"%s",filename)!=1)    
            info("ussage : read filename (processes text file commands)");
        else
            read(filename);
        
    }      
    if (!strcmp(st,"remove singles")) {
        remove_singles(current_image());
        draw_image(current_image(),1);
    }
    
    if (!strcmp(st,"make colorstrip")) {
        num_images++;
        editim=(imitem **)jrealloc(editim,sizeof(imitem *)*num_images,"image array");
        im=((*add_imitem())=new imitem(new image(256,1),"color strip"))->im;
        
        for (x=0;x<256;x++)
            im->putpixel(x, 0, (char) x);
        set_current(num_images-1);
        draw_image(current_image(),1);
        add_undo(cur_image,undo_create);
    }
    if (!strcmp(cmd,"mark"))
        strcpy(marked,st);
    
    if (!strcmp(cmd,"renumber")) {   if (!num_images) return ; renumber(st); }
    if (!strcmp(cmd,"next")) next_image();
    if (!strcmp(cmd,"last")) last_image();
    if (!strcmp(cmd,"copy")) copy_image(); 
    if (!strcmp(cmd,"copy_to")) 
    {
        int x;
        if (sscanf(st,"%d",&x))
            copy_to(x); 
        else
            info("usage : copy_to image_number");
    }
    if (!strcmp(cmd,"crop")) crop();
    if (!strcmp(cmd,"crop_center")) {   if (!num_images) return ; crop_center(st); }
    if (!strcmp(cmd,"copy_prop")) {   if (!num_images) return ; copy_property(st); }
    if (!strcmp(cmd,"rm_dups")) {   if (!num_images) return ; rm_dups(); }
    if (!strcmp(cmd,"rotate90")) {   if (!num_images) return ; rotate90(); }
    if (!strcmp(cmd,"quit")) quit_signal=1;
    if (!strcmp(cmd,"repeat")) repeat(st); 
    if (!strcmp(cmd,"write_raw_gray")) write_gray(st);
    if (!strcmp(cmd,"write_raw_color")) write_color(st);
    if (!strcmp(cmd,"merge_raw_gray")) merge_gray(st);
    if (!strcmp(cmd,"bevel")) bevel(st);
    if (!strcmp(cmd,"tint_pal")) 
    {
        int r,g,b,five;
        int t=sscanf(st,"%d%d%d%d",&r,&g,&b,&five);
        if (t==3) five=0;
        else if (t<3)
            info("ussage : tint_pal r g b");
        else
            palc->tint_pal(r,g,b,five);
    }
    
    if (!strcmp(cmd,"invert_pal"))
    {
        int r,g,b;
        int t=sscanf(st,"%d%d%d",&r,&g,&b);
        if (t!=3)
            info("ussage : invert_pal r g b");
        else
            palc->invert_pal(r,g,b);
    }
    
    if (!strcmp(cmd,"light")) 
    {
        add_undo(cur_image,undo_change);
        light(sel_mask,current_image(),palc->current_pal(),palc->get_filter(),st);
        redraw_sel_area();
    }
    
    if (!strcmp(cmd,"invert")) 
    {
        add_undo(cur_image,undo_change);
        rgb_invert(sel_mask,current_image(),palc->current_pal(),palc->get_filter(),st);
        redraw_sel_area();
    }
    
    if (!strcmp(cmd,"gravel")) 
    {
        add_undo(cur_image,undo_change);
        gravel(sel_mask,current_image(),palc->current_pal(),palc->get_filter(),st);
        redraw_sel_area();
    }
    
    if ((!strcmp(cmd,"sat") || !strcmp(cmd,"saturation"))  && num_images)
        create_sat_window(sel_mask,current_image(),palc->current_pal());  
    
    if (!strcmp(cmd,"color_add")) 
    {
        if (r!=2)
            info("ussage : color_add value");
        else
            palc->add(p[0]);
    }
    
    if (!strcmp(cmd,"add_bs")) 
    {
        if (r!=2)
            info("ussage : add bs value");
        else
            palc->add_bs(p[0]);
    }
    
    
    if (!strcmp(cmd,"upper_name"))
    {   if (!num_images) return ;
    for (sp=current_item()->name();*sp;sp++) { *sp=toupper(*sp); } }
    
    if (!strcmp(cmd,"lower_name"))
    {   if (!num_images) return ;
    for (sp=current_item()->name();*sp;sp++) { *sp=tolower(*sp); } }
    
    if (!strcmp(cmd,"prefix_name"))
    { 
        if (!num_images) return ;
        char s[140];
        strcpy(s,st);
        strcat(s,current_item()->name());
        current_item()->set_name(s);
    }
    
    if (!strcmp(cmd,"suffix_name"))
    { 
        if (!num_images) return ;
        char s[140];
        strcpy(s,current_item()->name());
        strcat(s,st);
        current_item()->set_name(s);
    }
    
    if (!strcmp(cmd,"delete")) 
        delete_image(st);
    
    if (!strcmp(cmd,"reverse"))
        reverse(st);
    
    if (!strcmp(cmd,"grow")) 
    {
        if (r!=3)
            info("ussage : grow xamount yamount");
        else 
        {
            if (!num_images) return ;
            grow(p[0],p[1]);
        }
    }
    
    if (!strcmp(cmd,"type"))
    {
        if (!num_images) return ;
        add_undo(cur_image,undo_change);
        
        sscanf(st,"%s",cmd2);
        r=1;
        if (!strcmp(cmd2,"image"))
            imi=new imitem(current_image()->copy(),current_item()->name()); 
        else if (!strcmp(cmd2,"foretile"))
            imi=(imitem *)new foretile(current_image()->copy(),current_item()->name()); 
        else if (!strcmp(cmd2,"backtile"))
            imi=(imitem *)new backtile(current_image()->copy(),current_item()->name()); 
        else if (!strcmp(cmd2,"character"))
            imi=(imitem *)new figure(current_image()->copy(),current_item()->name()); 
        else if (!strcmp(cmd2,"particle"))
            imi=(imitem *)new particle(current_image()->copy(),current_item()->name()); 
        else r=0;
        if (r)
        {
            delete editim[cur_image];
            editim[cur_image]=imi;
        } else info("Valid types are image, foretile, backtile, character");  
    }
    
    
    
    if (!strcmp(cmd,"animate"))
    {
        int total_images=0,counter,bad=0;
        for (counter=get_next_mark(st);counter!=-1;counter=get_next_mark(st,counter))
        {
            total_images++;
            if (counter<0 || counter>num_images) bad=1;
        }
        if (bad)
            info("Bad image range selected");
        else if (total_images)
        {  
            max_ani=total_images;
            ani_order=(int *)jmalloc(max_ani*(sizeof(int)),"ani order");
            for (x=0,counter=get_next_mark(st);counter!=-1;counter=get_next_mark(st,counter),x++)
                ani_order[x]=counter;
            ani_frame=0;
            change_state(ANIMATING);
        } else info("ussage : animate [image numbers]");
    }
    if (!strcmp(cmd,"morph"))
        morph(st);
    
    if (!strcmp(cmd,"mouse_shape"))
    {
        p[0]=get_number(st);   
        p[1]=get_number(st);
        p[2]=get_number(st);
        if (p[0]<num_images && p[0]>=0)
        {
            if (p[1]<0) p[1]=0;
            if (p[2]<0) p[2]=0;
            eh->set_mouse_shape(editim[p[0]]->im->copy(),p[1],p[2]);
        } else info("ussage : mouse_shape image centerx centery");
    }
    
    if (!strcmp(cmd,"palette"))
    {
        if (!strcmp(st,"on"))
        { palc->show(); } 
        else if (!strcmp(st,"off"))
        { palc->hide(); } 
        else if (!strcmp(st,"toggle"))
        { palc->toggle(); } 
    }
    
    if (!strcmp(cmd,"flip"))
        if (!strcmp(st,"x"))
        {
            if (!num_images) return ;
            add_undo(cur_image,undo_change);
            current_image()->flip_x();
            if (current_item()->spec_type()==SPEC_CHARACTER2)
                ((figure *)current_item())->xcfg=current_image()->width()-
                ((figure *)current_item())->xcfg-1;
            draw_image(current_image(),1);
        }
        else if (!strcmp(st,"y"))
        {
            if (!num_images) return ;
            add_undo(cur_image,undo_change);
            current_image()->flip_y();
            draw_image(current_image(),1);
        }
        
        if (!strcmp(cmd,"chop"))
        { if (r!=3)
        info("usage : chop width height");
        else
        {
            if (!num_images) return ;
            for (y=0;y<(p[1]+current_image()->height()-1)/p[1];y++)
                for (x=0;x<(p[0]+current_image()->width()-1)/p[0];x++)
                {
                    im=new image(p[0],p[1]);
                    im->clear();
                    x2=p[0]*x+p[0]-1;
                    if (x2>=current_image()->width()) x2=current_image()->width()-1;
                    y2=p[1]*y+p[1]-1;
                    if (y2>=current_image()->height()) y2=current_image()->height()-1;
                    
                    sprintf(cmd2,"%s:chopped (%d,%d)",current_name(),x,y);
                    current_image()->put_part(im,0,0,p[0]*x,p[1]*y,x2,y2,0);
                    
                    switch (current_item()->spec_type())
                    {
                    case SPEC_IMAGE : (*add_imitem())=new imitem(im,cmd); break;
                    case SPEC_BACKTILE : (*add_imitem())=new backtile(im,cmd); break;
                    case SPEC_FORETILE : (*add_imitem())=new foretile(im,cmd); break;
                    case SPEC_CHARACTER2 : (*add_imitem())=new figure(im,cmd); break;
                    case SPEC_PARTICLE : (*add_imitem())=new particle(im,cmd); break;
                    }
                }
        }
        }
        
        if (!strcmp(cmd,"news"))
        {
            if (select_buffer)
            {
                (*add_imitem())=new imitem(select_buffer->copy(),"select buffer");       
                set_current(num_images-1);
                draw_image(current_image(),1);
            }    
        }
        
        
        if (!strcmp(cmd,"new") || !strcmp(cmd,"newb") || !strcmp(cmd,"newf") || !strcmp(cmd,"newc"))
        { if(r==1 && !new_window)
        new_window=eh->new_window(-1,-1,0,0,
        new button(4,10,PAINT_NEW_IMAGE,"new image",
        new button(4,30,PAINT_NEW_BACKTILE,"new background tile",
        new button(4,50,PAINT_NEW_FORETILE,"new foreground tile",
        new button(4,70,PAINT_NEW_CHARACTER,"new character",
        
        new text_field(5,90,PAINT_NEW_W,"Width","*****","30",
        new text_field(5,110,PAINT_NEW_H,"Height","*****","30",
        new text_field(5,130,PAINT_NEW_NAME,"Name","*****************","Name me!",NULL
        ))))))));
        else if (r!=3)
            info("usage : new[b|f|c] width height name");
        else
        {
            int t=sscanf(st,"%d%d%s",&p[0],&p[1],cmd2);
            if (t==2) strcpy(cmd2,"noname");
            else if (t<2) 
            {
                info("ussage : width height [name]\n");
                return ;
            }
            if ( (!strcmp(cmd,"newf") || !strcmp(cmd,"newc")) && (p[0]>256 || p[1]>256))
                info("Maximum size is 255x255");
            else
            {
                im=new image(p[0],p[1]);
                im->clear();
                if (cmd[2]==0)
                    strcpy(cmd2,"Name me please!");
                if (!strcmp(cmd,"newb"))
                    (*add_imitem())=new backtile(im,cmd2);
                else if (!strcmp(cmd,"newf"))
                    (*add_imitem())=new foretile(im,cmd2);
                else if (!strcmp(cmd,"newc"))
                    (*add_imitem())=new figure(im,cmd2);
                else if (!strcmp(cmd,"newp"))
                    (*add_imitem())=new particle(im,cmd2);
                else
                    (*add_imitem())=new imitem(im,cmd2);
                set_current(num_images-1);
                draw_image(current_image(),1);
            }
        }
        }
        
        if (!strcmp(cmd,"sample_scale"))
        { if (r!=3)
        info("usage : sample_scale width height");
        else
        {
            if (!num_images) return ;
            add_undo(cur_image,undo_change);
            sample_scale(p[0],p[1]);
            draw_image(current_image(),1);
        }
        }
        
        if (!strcmp(cmd,"ascale"))
        { if (r!=3 || p[0]<=0 || p[1]<=0)
        info("usage : scale width height");
        else
        {
            if (!num_images) return ;
            add_undo(cur_image,undo_change);
            image *new_im=aliased_scale(editim[cur_image]->im,p[0],p[1],palc->current_pal());
            editim[cur_image]->scale(p[0],p[1]);
            delete editim[cur_image]->im;
            editim[cur_image]->im=new_im;
            draw_image(current_image(),1);
        }
        }
        
        
        if (!strcmp(cmd,"ascalex"))
        { if (r!=3 || p[0]<=0 || p[1]<=0)
        info("usage : scale width height");
        else
        {
            if (!num_images) return ;
            int nw=(current_image()->width()*p[0]/p[1]);
            int nh=(current_image()->height()*p[0]/p[1]);
            if (nw<=0 || nh<=0) return ;
            add_undo(cur_image,undo_change);
            
            image *new_im=aliased_scale(editim[cur_image]->im,nw,nh,palc->current_pal());
            editim[cur_image]->scale(nw,nh);
            delete editim[cur_image]->im;
            editim[cur_image]->im=new_im;
            draw_image(current_image(),1);
        }
        }
        
        if (!strcmp(cmd,"scale"))
        { if (r!=3 || p[0]<=0 || p[1]<=0)
        info("usage : scale width height");
        else
        {
            if (!num_images) return ;
            add_undo(cur_image,undo_change);
            
            editim[cur_image]->scale(p[0],p[1]);
            draw_image(current_image(),1);
        }
        }
        
        if (!strcmp(cmd,"scalex"))
        { if (r!=3 || p[0]<=0 || p[1]<=0)
        info("usage : scalex mult_ratio div_ratio");
        else
        {
            if (!num_images) return ;
            int nw=(current_image()->width()*p[0]/p[1]);
            int nh=(current_image()->height()*p[0]/p[1]);
            if (nw<=0 || nh<=0) return ;
            
            add_undo(cur_image,undo_change);
            editim[cur_image]->scale(nw,nh);
            draw_image(current_image(),1);
        }
        }
        
        if (!strcmp(cmd,"where"))
        {
            mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,x,y);
            sprintf(cmd,"Mouse at Image (%d,%d), Screen (%d, %d)\n",x,y,ev.mouse_move.x,ev.mouse_move.y);
            info(cmd);
        }
        if (!strcmp(cmd,"blur_color"))
        {  
            if (last_select.image_number!=cur_image || r>3)
                info("blur_color amount color");
            else
            {
                if (!num_images) return ;
                add_undo(cur_image,undo_change);      
                if (r<2)
                    blur_area(last_select.x1,last_select.y1,
                    last_select.x2,last_select.y2,1,current_color()); 
                else if (r<3)
                    blur_area(last_select.x1,last_select.y1,
                    last_select.x2,last_select.y2,p[0],current_color()); 
                else
                    blur_area(last_select.x1,last_select.y1,
                    last_select.x2,last_select.y2,p[0],p[1]);    
                draw_image(current_image(),1);
            }
            
        }
        
        
        if (!strcmp(cmd,"blur"))
        {
            x1=p[0]; y1=p[1]; x2=p[2]; y2=p[3];
            if (r!=6 || (x2<x1 || y2<y1 || x1<0 || y1<0 || p[4]<1 ||
                x2>=current_image()->width() || y2>=current_image()->height()))
            {
                if (last_select.image_number==cur_image)
                {	
                    if (!num_images) return ;
                    add_undo(cur_image,undo_change);
                    blur_area(last_select.x1,last_select.y1,last_select.x2,last_select.y2,1,-1);
                    draw_image(current_image(),1);
                }      
                else      
                    info("usage : blur x1 y1 x2 y2 blur_amount");
            }
            
            else 
            {
                if (!num_images) return ;
                add_undo(cur_image,undo_change);
                blur_area(x1,y1,x2,y2,p[4],-1);    
                draw_image(current_image(),1);
            }    
        }  
        if (!strcmp(cmd,"anti"))
        {
            x1=p[0]; y1=p[1]; x2=p[2]; y2=p[3];
            if (r!=5 || (x2<x1 || y2<y1 || x1<0 || y1<0 || 
                x2>=current_image()->width() || y2>=current_image()->height()))
            {
                if (last_select.image_number==cur_image)
                {	
                    if (!num_images) return ;
                    add_undo(cur_image,undo_change);
                    anti(last_select.x1,last_select.y1,last_select.x2,last_select.y2);      
                    draw_image(current_image(),1);
                }      
                info("usage : anti x1 y1 x2 y2");
            }    
            else 
            {
                if (!num_images) return ;
                add_undo(cur_image,undo_change);
                anti(x1,y1,x2,y2);    
                draw_image(current_image(),1);
            }    
        }  
        if (!strcmp(cmd,"clip"))
        {
            x1=p[0]; y1=p[1]; x2=p[2]; y2=p[3];
            if (r!=5 || (x2<x1 || y2<y1 || x1<0 || y1<0 ||
                x2>=current_image()->width() || y2>=current_image()->height()))
            {
                if (last_select.image_number==cur_image)
                {	
                    if (!num_images) return ;
                    x1=last_select.x1; y1=last_select.y1;  x2=last_select.x2; y2=last_select.y2;
                } else x1=-1;   
                if (x1<0)
                    info("usage : clip x1 y1 x2 y2");
            }    
            
            if (x1>=0)
            {
                if (!num_images) return ;
                add_undo(cur_image,undo_change);
                im=new image(x2-x1+1,y2-y1+1);
                current_image()->put_part(im,0,0,x1,y1,x2,y2);
                delete current_item()->im;
                current_item()->im=im;
                draw_image(current_image(),1);
            }
        }
        if (!strcmp(cmd,"load"))
        {
            sscanf(st,"%s %s",cmd2,filename);
            if (strcmp(cmd2,"palette"))
                info("usage : load palette filename");
            else
            { 
                jFILE fp(filename,"rb");
                if (fp.open_failure())      
                    info("Unable to open file for reading");
                else
                {
                    add_undo(0,undo_palette,palc->current_pal());  
                    pal2=NULL;
                    images=load(filename,x,pal2);
                    if (x==0)
                        info(imerr_messages[current_error()]);
                    else
                    { for (y=0;y<x;y++)
                    delete images[y];
                    jfree(images);
                    change_palette(pal2);
                    delete pal2;	
                    }
                }
            }
        }
        if (!strcmp(cmd,"solid"))
        {
            if (r!=2)
                info("usage : solid colornumber");
            else
            {
                if (!num_images) return ;
                add_undo(cur_image,undo_change);
                current_image()->make_color(p[0]);
                draw_image(current_image(),0);
            }
        }
        
        if (!strcmp(cmd,"merge")) 
        {
            if (st[0]==0)
                info("usage : merge filename");
            else
                merge(st,-1,-1);
        }
        
        if (!strcmp(cmd,"merge_range")) 
        {
            int sta,end;
            char *s=st;
            while (*s && *s!=' ') s++;
            if (*s)
            {     
                *s=0;
                s++;
            }
            if (*s==0 || sscanf(s,"%d%d",&sta,&end)!=2)
                info("usage : merge filename ");
            else
                merge(st,sta,end);
        }
        
        if (!strcmp(cmd,"add_xcfg"))
        {
            if (r!=2)
                info("usage :  add_xcfg amount");
            else
            {
                add_undo(cur_image,undo_change);
                if (current_item()->spec_type()==SPEC_CHARACTER2)
                    ((figure *)current_item())->xcfg+=p[0];
            }
        }
        
        if (!strcmp(cmd,"select"))
        {
            if (!num_images) return ;
            if (r==3)
            {
                x1=p[0]; y1=p[1]; x2=p[2]; y2=p[3];
                if ((x2<x1 || y2<y1 || x1<0 || y1<0 ||
                    x2>=current_image()->width() || y2>=current_image()->height()))
                    info("usage : select [and mouse selection] | select x1 y1 x2 y2");
                else
                {
                    if (select_buffer) delete select_buffer;
                    select_buffer=new image(x2-x1+1,y2-y1+1);
                    current_image()->put_part(select_buffer,0,0,x1,y1,x2,y2);
                }
            }
            else
            { mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,selx1,sely1);
            
            // move the the selection point inside the image!
            if (selx1<0) selx1=0;
            if (sely1<0) sely1=0;
            if (selx2>=current_image()->width()) selx2=current_image()->width()-1;
            if (sely2>=current_image()->height()) sely2=current_image()->height()-1;
            
            selx2=selx1;
            sely2=sely1;
            if (selx1<0) { selx1=0; selx2=0; }
            if (sely1<0) { sely1=0; sely2=0; }
            change_state(AREA_SELECT);
            info("Please select an area with mouse");
            sel_but=ev.mouse_button;
            }
        }
        if (!strcmp(cmd,"swap"))
        { 
            if (!num_images) return ;
            p[0]=get_number(st);
            p[1]=get_number(st);    
            if (p[0]>num_images || p[1]>num_images || p[0]<=0 || p[1]<=0)
                info("usage : swap X Y  (swaps image number X with Y)");
            else
            {
                imi=editim[p[0]];
                editim[p[0]]=editim[p[1]];
                editim[p[1]]=imi;
                if (p[0]==cur_image || p[1]==cur_image)
                    draw_image(current_image(),1);
            }
        }
        if (!strcmp(cmd,"tile") && num_images)
        {
            change_state(MODE_TILE);
            ev.type=EV_SPURIOUS;
        }
        
        if (!strcmp(cmd,"go"))
        { 
            p[0]=get_number(st);
            if (r!=2 || p[0]<0 || p[0]>=num_images)
                info("usage : go image_number");
            else
            {
                set_current(p[0]);
                draw_image(current_image(),1);
            }
        }
        if (!strcmp(cmd,"index"))
            if (num_images)   
                change_state(INDEX_PICK);
            
            if (!strcmp(cmd,"write_tpal"))
            {
                char fn[100];
                if (!sscanf(st,"%s",fn))
                    info("usage : write_tpal filename");
                else write_tpal(fn);    
            }
            
            if (!strcmp(cmd,"load_tpal"))
            {
                char fn[100];
                if (!sscanf(st,"%s",fn))
                    info("usage : read_tpal filename");
                else load_tpal(fn);    
            }
            
            if (!strcmp(cmd,"write_pcx"))
            {
                while (*st==' ') st++;
                image *im=current_image();
                if (current_item()->spec_type()==SPEC_CHARACTER2)
                {
                    im=new image(320,200);
                    im->clear();
                    current_image()->put_image(im,320/2-((figure *)current_item())->xcfg,200/2);
                }
                if (st[0]==0)
                    write_PCX(im,pal,current_item()->name());
                else write_PCX(im,pal,st);
                if (im!=current_image()) delete im;
            }  
            
            if (!strcmp(cmd,"write_bmp")) {
                while (*st==' ') st++;
                if (st[0]==0)
                    info("Usage : write_bmp filename");
                else write_bmp(current_image(),pal,st);
            }  
            if (!strcmp(cmd,"write_ppm")) {
                while (*st==' ') st++;
                if (st[0]==0)
                    info("Usage : write_ppm filename");
                else write_ppm(current_image(),pal,st);
            }
            
            if (!strcmp(cmd,"write_include")) {
                while (*st==' ') st++;
                if (st[0]==0)
                    info("Usage : writeinclude filename");
                else write_include(current_image(),pal,st,current_item()->name()); 
            }                 
            
            if (!strcmp(cmd,"delete_file"))
                if (st[0]==0)
                    info("Usage : delete_file filename");
                else if (unlink(st))
                    info("delete_file failed");
                
                
                if (!strcmp(cmd,"write_property"))
                    if (!sscanf(st,"%s",filename))
                        info("Usage : write_property filename");
                    else write_property(filename,st+strlen(filename)+1);
                    
                    if (!strcmp(cmd,"load_property"))
                        if (!sscanf(st,"%s",filename))
                            info("Usage : load_property filename");
                        else load_property(filename,st+strlen(filename)+1);  
                        
                        if (!strcmp(cmd,"saveas"))
                        {
                            if (st[0]==0)
                                info("Usage : saveas filename");
                            else 
                            {
                                strcpy(fname,st);   // make this the new filename
                                save_stuff();
                            }
                        }        
}

void draw_point_list(point_list *p, int x, int y, int color, int mess_color)
{
    int i;
    unsigned char *s=p->data;  
    if (p->tot)
    {
        if (s[0]!=s[p->tot*2-2] || s[1]!=s[p->tot*2-1]) color=mess_color;
        for (i=0;i<p->tot-1;i++,s+=2)    
            screen->line(x+s[0],y+s[1],x+s[2],y+s[3],color);    
    }
}


void painter::draw_indexes(int xoffset, int yoffset)
{
    Index *ind;
    int x,y,red=palc->current_pal()->find_closest(255,0,0),
        blue=palc->current_pal()->find_closest(0,0,255),
        yellow=palc->current_pal()->find_closest(255,255,0),
        green=palc->current_pal()->find_closest(0,255,0),
        white=eh->bright_color();
    
    
    screen->clear();
    for(ind=indexes;ind;ind=ind->next)
    {
        imitem *itm=editim[ind->image_num];     
        x=ind->x1+1+xoffset;
        y=ind->y1+1+yoffset;    
        itm->im->put_image(screen,x,y);  
        if (index_flags)
        {
            switch (itm->spec_type())
            {
            case SPEC_FORETILE : draw_point_list( ((foretile *)itm)->p,x,y,white,green); break;
            case SPEC_CHARACTER2 : 
                draw_point_list( ((figure *)itm)->damage,x,y,red,green); 
                draw_point_list( ((figure *)itm)->hit,x,y,yellow,green); 
                screen->line(x+((figure *)itm)->xcfg,y,
                    x+((figure *)itm)->xcfg,y+itm->im->height()-1,white); 
                screen->line(x+((figure *)itm)->xcfg,y+itm->im->height()-1,
                    x+((figure *)itm)->xcfg+((figure *)itm)->advance,y+itm->im->height()-1,white);
                break;
            }      
        }    
    }
}

void painter::copy_to(int num)
{
    char name[100];
    if (num>=0 && num<num_images && num!=cur_image)
    {
        sprintf(name,"Copied %s->%s\n",current_item()->name(),editim[num]->name());
        info(name);
        
        add_undo(num,undo_change);
        delete editim[num];    
        sprintf(name,"%s+",current_item()->name());  
        editim[num]=current_item()->copy(name);
        
    }
    
}

void painter::copy_image()
{
    char name[100];
    if (num_images)
    {
        add_undo(cur_image,undo_create);
        switch (current_item()->spec_type())
        {
        case SPEC_IMAGE :
        case SPEC_BACKTILE : 
        case SPEC_FORETILE : 
        case SPEC_CHARACTER2 : 
        case SPEC_PARTICLE : 
            sprintf(name,"%s+",current_item()->name());
            (*add_imitem())=current_item()->copy(name);
            show_current();
            break;
            
        }
    }
}

void painter::pan(short key)
{
    short x1,y1,x2,y2,sc;
    if (zoom==1)
        sc=20;
    else
    {
        sc=5-zoom; 
        if (sc<1) sc=1; 
    }
    switch (key)
    { case JK_DOWN : zoomy+=sc; draw_image(current_image(),1); break;
    case JK_UP : zoomy-=sc; if (zoomy<0) zoomy=0; draw_image(current_image(),1);  break;
    case JK_RIGHT : zoomx+=sc; draw_image(current_image(),1); break;
    case JK_LEFT : zoomx-=sc; if (zoomx<0) zoomx=0; draw_image(current_image(),1);  break;
    }
    image_area(x1,y1,x2,y2);
    if (((((x2-x1)/zoom)>current_image()->width()-zoomx) && (key==JK_LEFT || key==JK_RIGHT))
        || ((((y2-y1)/zoom)>current_image()->height()-zoomy) && (key==JK_DOWN || key==JK_UP)))
        draw_image(current_image(),1);
    else
        draw_image(current_image(),0);
    
}

void painter::info(char *st, char *st2, int cursor)
{
    short x1,y1,x2,y2;
    info_area(x1,y1,x2,y2);
    //  if (st2)
    screen->bar(x1,y1,x2,y2,eh->dark_color());
    //  else
    //    screen->bar(x1,y1,x2,y1+font->height()+1,eh->dark_color());
    font->put_string(screen,x1+1,y1+1,st);
    if (st2)
        font->put_string(screen,x1+1,y1+font->height()+2,st2);
    if (cursor>=0)
        screen->bar(x1+1+font->width()*strlen(st),y1+font->height()-2,
        x1+3+font->width()*strlen(st),y1+font->height(),eh->bright_color());
    pch->need_refresh();
    
}

void painter::mouse_image_position(short mousex, short mousey, short &x, short &y)
{
    short x1, y1,x2,y2;
    image_area(x1,y1,x2,y2);
    if (mousex<x1) mousex=x1;
    if (mousey<y1) mousey=y1;
    if (mousex>x2) mousex=x2;
    if (mousey>y2) mousey=y2;
    x=(mousex-x1)/zoom+zoomx;
    y=(mousey-y1)/zoom+zoomy;
}

void painter::pixel_area(short x, short y, short &x1, short &y1, short &x2, short &y2)
{
    short ix1,iy1,iy2,ix2;
    image_area(ix1,iy1,ix2,iy2);
    x1=ix1+(x-zoomx)*zoom;
    y1=iy1+(y-zoomy)*zoom;
    x2=x1+zoom-1;
    y2=y1+zoom-1;
}

int painter::draw_selection_state(int state)
{
    if (state==MODE_AREA_SELECT || state==MODE_POLY_SELECT) return 1;
    else return 0;
}

void painter::step()
{
    imitem *cur_ani;  
    if (state==ANIMATING)
    {
        if (last_animate.x1>-1000)  // erase the last draw if there was one
            screen->bar(last_animate.x1,last_animate.y1,last_animate.x2,last_animate.y2,0);
        
        int but;        
        eh->mouse_status(last_animate.x1,last_animate.y1,but);    
        cur_ani=editim[ani_order[ani_frame]];
        
        last_animate.y1-=cur_ani->im->height();  // draw ABOVE the mouse cursor     
        
        if (ani_dir>0)
            ani_xoff+=next_off;
        else ani_xoff-=next_off;
        
        
        if (cur_ani->spec_type()==SPEC_CHARACTER2)  // move to CFG if character type
        {
            last_animate.x1-=((figure *)cur_ani)->xcfg;
            next_off=((figure *)cur_ani)->advance;
            last_animate.x1+=ani_xoff;
        } else next_off=0;
        
        
        last_animate.x2=last_animate.x1+cur_ani->im->width();
        last_animate.y2=last_animate.y1+cur_ani->im->height();
        
        
        
        
        
        int forw=*((int *)animate_control_window->read(PAINT_ANIMATE_FORWARD)),
            back=*((int *)animate_control_window->read(PAINT_ANIMATE_BACKWARD));
        
        cur_ani->im->put_image(screen,last_animate.x1,last_animate.y1,0);    
        
        // go to the next animation frame
        ani_frame+=ani_dir;    
        if (ani_frame>=max_ani)
            if (!forw && !back)
            {	
                ani_dir=-ani_dir;
                ani_frame+=ani_dir;
            }
            else
            {
                ani_frame=0;
                ani_xoff=0;
            }
            if (ani_frame<0)
                if (!forw && !back)
                {	
                    ani_dir=-ani_dir;
                    ani_frame+=ani_dir;
                }
                else 
                {
                    ani_frame=max_ani-1;
                    ani_xoff=0;
                }
                
                milli_wait(ani_sleep);    
    } else if (draw_selection_state(state))
    {
        time_marker now;
        if (now.diff_time(&last_sel_draw_time)>2)
        {
            draw_sel_list( (last_sel_draw_offset++)&1);
            last_sel_draw_time.get_time();
        }
    } else
        if (!eh->event_waiting())
            milli_wait(10);     // if nothing is going on, wait 1/100 of a second,
        // so we don't hog the CPU
}

void painter::calc_areas()
{
    inf_area.x1=0; inf_area.y1=0;  
    inf_area.x2=screen->width()-1;
    inf_area.y1=0;
    if (font)
        inf_area.y2=font->height()*2+3;
    else inf_area.y2=12;
    
    im_area.x1=0;
    im_area.x2=inf_area.x2;
    im_area.y1=inf_area.y2+5;
    im_area.y2=screen->height()-2;
    if (pch)
        pch->note_resize();
}

void painter::info_area(short &x1, short &y1, short &x2, short &y2)
{
    x1=inf_area.x1;
    y1=inf_area.y1;
    x2=inf_area.x2;
    y2=inf_area.y2;
}

void painter::image_area(short &x1, short &y1, short &x2, short &y2)
{
    x1=im_area.x1;
    y1=im_area.y1;
    x2=im_area.x2;
    y2=im_area.y2;
}


void painter::draw_image(image *im, short erase)
{
    short ix1, iy1, ix2, iy2,x,y;
    short i,j,k,mx,my, dy=0,dx=0,x2,y2;
    unsigned char *sl,*sl2;
    if (no_draw_mode) return ;
    
    paint_cursor.restore();
    image_area(ix1,iy1,ix2,iy2);
    screen->add_dirty(ix1,iy1,ix2,iy2);
    if (erase || im==NULL)
        screen->bar(ix1,iy1,ix2,iy2,0);
    screen->rectangle(ix1-1,iy1-1,ix2+1,iy2+1,15);
    screen->rectangle(ix1-2,iy1-2,ix2+2,iy2+2,16);
    if (num_images && im)
    {
        if (zoomx>im->width())
            zoomx=im->width()-1;
        if (zoomy>im->height())
            zoomy=im->height()-1;
        
        mx=zoomx+(ix2-ix1)/zoom; 
        if (mx>im->width())
        { dx=1;  mx=im->width(); }
        
        
        my=zoomy+(iy2-iy1)/zoom;
        if (my>im->height())  
        { dy=1;  my=im->height(); }
        
        if (zoom==1)
        { screen->set_clip(ix1,iy1,ix2,iy2);
        im->put_part(screen,ix1,iy1,zoomx,zoomy,im->width()-1,im->height()-1); 
        screen->set_clip(0,0,screen->width()-1,screen->height()-1);
        }
        else
        {
            for (j=zoomy,y=iy1+(j-zoomy)*zoom;j<my;j++,y+=zoom)
            { sl=im->scan_line(j);
            for (k=0;k<zoom;k++)
            {
                sl2=screen->scan_line(y+k);
                for (i=zoomx,x=ix1+(i-zoomx)*zoom;i<mx;i++,x+=zoom)
                    memset(&sl2[x],sl[i],zoom);
            }
            }
        }
        if (dx)
            x2=ix1+(mx-zoomx)*zoom+1; else x2=ix2;
        if (dy)
            y2=iy1+(my-zoomy)*zoom+1; else y2=iy2;
        
        if (dx)
            screen->line(x2, iy1, x2, y2, (unsigned char) bright_color);
        if (dy)
            screen->line(ix1, y2, x2, y2, (unsigned char) bright_color);
    }
    show_current();
    
    if (draw_selection_state(state))
    {
        draw_sel_list( (last_sel_draw_offset++)&1);
        last_sel_draw_time.get_time();
    }
    
    paint_cursor.draw();
}

void painter::draw_image_area(image *main, image *sub, 
                              int x, int y, int x1, int y1, int x2, int y2, int transparent)
{
    if (no_draw_mode) return ;  
    short ix1,iy1,ix2,iy2;
    image_area(ix1,iy1,ix2,iy2); 
    
    if (num_images && main && sub)
    {
        // clip in x1,y1-x2,y2 to fit sub
        if (x1<0) x1=0;
        if (y1<0) y1=0;
        if (x2>=sub->width()) x2=sub->width()-1;
        if (y2>=sub->height()) y2=sub->height()-1;
        if (x1>x2 || y1>y2) return ; // total clipped out
        
        
        long cx1=zoomx,
            cy1=zoomy,
            cx2=zoomx+(ix2-ix1)/zoom-1,
            cy2=zoomy+(iy2-iy1)/zoom-1;
        if (cx2>=main->width()) cx2=main->width()-1;
        if (cy2>=main->height()) cy2=main->height()-1;
        
        // see if it going to be clipped out of the image area
        if (x>cx2 || y>cy2 || x+(x2-x1)<cx1 || y+(y2-y1)<cy1) return ;
        
        
        // clip to fit the image area
        if (x<cx1)
        { x1+=(cx1-x); x=cx1; }
        if (y<cy1)
        { y1+=(cy1-y); y=cy1; }
        
        if (x+x2-x1+1>cx2)
        { x2=cx2-x+x1; }
        
        if (y+y2-y1+1>cy2)
        { y2=cy2-y+y1; }
        if (x1>x2 || y1>y2) return ;    
        
        
        
        
        if (zoom==1)
        {
            short cx1,cy1,cx2,cy2;
            screen->get_clip(cx1,cy1,cx2,cy2);
            screen->set_clip(ix1,iy1,ix1+main->width()-1-zoomx,iy2+main->height()-1-zoomy);
            sub->put_part(screen,ix1+(x-zoomx),iy1+(y-zoomy),x1,y1,x2,y2,transparent);      
            screen->set_clip(cx1,cy1,cx2,cy2);      
        }
        else
        {
            int dx1=ix1+(x-zoomx)*zoom,
                dx2=ix1+(x+x2-x1-zoomx)*zoom+zoom-1,
                dy1=iy1+(y-zoomy)*zoom,
                dy2=iy1+(y+y2-y1-zoomy)*zoom+zoom-1;
            screen->add_dirty(dx1,dy1,dx2,dy2);
            
            y=dy1;
            x=dx1;
            
            for (int j=y1;j<=y2;j++)
            {
                unsigned char *sl=sub->scan_line(j);
                for (int k=0;k<zoom;k++,y++)
                {
                    unsigned char *sl2=screen->scan_line(y)+x;
                    for (int i=x1;i<=x2;i++,sl2+=zoom)
                        if (!transparent || sl[i])
                            memset(sl2,sl[i],zoom);
                }
            }
            
        }
    }
}

void painter::draw_area(image *im, int x1, int y1, int x2, int y2, int color)
{
    if (no_draw_mode) return ;  
    short ix1,iy1,ix2,iy2;
    image_area(ix1,iy1,ix2,iy2); 
    
    if (num_images && im)
    {
        if (zoomx>im->width())
            zoomx=im->width()-1;
        if (x1<zoomx) x1=zoomx;
        
        if (zoomy>im->height())
            zoomy=im->height()-1;
        if (y1<zoomy) y1=zoomy;
        
        int mx=zoomx+(ix2-ix1)/zoom; 
        if (mx>im->width())
            mx=im->width();
        if (x2>=mx) x2=mx-1;
        
        int my=zoomy+(iy2-iy1)/zoom;
        if (my>im->height())  
            my=im->height();
        if (y2>=my) y2=my-1;
        
        if (x2<x1 || y2<y1) return;   // if cliped off screen return
        
        
        if (zoom==1)
        {
            if (color>=0)
                screen->bar((x1-zoomx)*zoom+ix1,(y1-zoomy)*zoom+iy1,
                (x2-zoomx)*zoom+ix1,(y2-zoomy)*zoom+iy1,color);
            else
                im->put_part(screen,(x1-zoomx)*zoom+ix1,(y1-zoomy)*zoom+iy1,x1,y1,x2,y2);       
        }
        else
        {
            int dx1=ix1+(x1-zoomx)*zoom,
                dx2=ix1+(x2-zoomx)*zoom+zoom-1,
                dy1=iy1+(y1-zoomy)*zoom,
                dy2=iy1+(y2-zoomy)*zoom+zoom-1;
            screen->add_dirty(dx1,dy1,dx2,dy2);
            
            
            if (color>=0)
                screen->bar(dx1,dy1,dx2,dy2,color);
            else
            {
                for (int j=y1,y=dy1;j<=y2;j++,y+=zoom)
                { unsigned char *sl=im->scan_line(j);
                for (int k=0;k<zoom;k++)
                {
                    unsigned char *sl2=screen->scan_line(y+k);
                    for (int i=x1,x=dx1;i<=x2;i++,x+=zoom)
                        memset(&sl2[x],sl[i],zoom);
                }
                }
            }
        }
    }
}

void painter::mouse_click(event &ev)
{
    short x, y, row, col;
    if (ev.mouse_button && ev.window==NULL) {
        x=ev.mouse_move.x; y=ev.mouse_move.y;
        if (num_images) {
            mouse_image_position(x,y,row,col);
            if (row>=0 && col>=0 && row<current_image()->width() && col<current_image()->height()) {
                if (ev.mouse_button==1) {
                    if (last_button==0)
                        add_undo(cur_image,undo_change);
                
                    current_image()->bar(row-brush_size+1,col-brush_size+1,
                        row+brush_size-1,col+brush_size-1,current_color());
                    draw_area(current_image(),row-brush_size+1,col-brush_size+1,
                        row+brush_size-1,col+brush_size-1);
                }
                else 
                    set_current_color(current_image()->pixel(row,col)); 
            }
        }
    }
}


void painter::next_image()
{
    short last=cur_image,e;
    if (!num_images) return ;
    cur_image++;
    if (cur_image>=num_images) 
        set_current(0);
    
    if (last==-1 || (!num_images)) e=0;
    else if (editim[last]->im->height()==current_image()->height() 
        && editim[last]->im->width()==current_image()->width()) 
        e=0;
    else e=1;
    if (num_images)
        draw_image(current_image(),e);
    else
        draw_image(NULL);
}

void painter::last_image()
{
    if (!num_images) return ;
    short last=cur_image,e;
    cur_image--;
    if (cur_image<0) set_current(num_images-1);
    
    if (last==-1 || (!num_images)) e=0;
    else if (editim[last]->im->height()==current_image()->height() 
        && editim[last]->im->width()==current_image()->width()) 
        e=0;
    else e=1;
    if (num_images)
        draw_image(current_image(),e);
    else
        draw_image(NULL);
}

void painter::draw()
{
    screen->clear();
    if (index_state(state))
    {  
        Index *old;    
        while (indexes)        // if we already had in index in memory, delete it
        { 
            old=indexes;
            indexes=indexes->next;
            delete old;
        }
        if (num_images)
        { 
            int x1,y1,y2,r;
            x1=0; y1=0;
            y2=0; last_index=NULL;
            for (r=0;r<num_images;r++)
            {
                if (x1+editim[r]->im->width()+2>=screen->width())
                { x1=0; y1=y2; }
                Index *ind=new Index(r,x1,y1,x1+editim[r]->im->width()+2,
                    y1+editim[r]->im->height()+2);
                
                ind->next=indexes;
                indexes=ind;
                x1+=editim[r]->im->width()+2;
                if (y1+editim[r]->im->height()+2>y2)
                    y2=y1+editim[r]->im->height()+2;
            }
            draw_indexes(index_xoffset,index_yoffset);
        }    
    } else if (animate_state(state))
        eh->flush_screen();
    else if (tile_state(state))
    {
        screen->clear();
        image *im=current_image();
        char f[100];
        sprintf(f,"tiled image #%d, size = %dx%d",cur_image+1,
            current_image()->width(),current_image()->height());
        info(f);
        short x1,y1,x2,y2,cx1,cy1,cx2,cy2;
        image_area(x1,y1,x2,y2);
        screen->get_clip(cx1,cy1,cx2,cy2);
        screen->set_clip(x1,y1,x2,y2);
        int l=(x2-x1+1)/im->width()+1;
        int h=(y2-y1+1)/im->height()+1;
        for (int y=0;y<h;y++)
            for (int x=0;x<l;x++)
                im->put_image(screen,x1+x*im->width(),y1+y*im->height());
            screen->set_clip(cx1,cy1,cx2,cy2);
    }
    else
    {
        if (num_images)
            draw_image(current_image(),1);
        else title_screen();
    }
}

void painter::flush_screen()
{
    if (pix_dirty.x1>=0)
    {
        screen->add_dirty(pix_dirty.x1,pix_dirty.y1,pix_dirty.x2,pix_dirty.y2);
        pix_dirty.x1=-1;
    }
    eh->flush_screen();
}

painter::painter(char *filename)
{
    current_points=0;
    sel_mask=NULL;
    area_sel_list=NULL;
    area_sel_total=0;
    next_off=ani_xoff=0;
    filter f;
    image *fnt_pic;
    short i;
    ani_frame=-1;
    selx1=-1;
    last_select.image_number=-1;
    index_flags=0;
    last_on=-1; 
    rcommand=0; 
    tot_hist=0;
    hist_on=0;
    pix_dirty.x1=-1;        // no dirty pixels right now.
    
    undo_levels=4;
    last_button=0;  
    no_draw_mode=0;  
    max_ani=0;
    ani_order=NULL;
    animate_pick_window=NULL;
    quit_signal=0;
    calc_areas();
    
    num_images=0; editim=NULL; pal=new palette; font=NULL; brush_size=1;
    pal->defaults();  
    col_filt=NULL;
    indexes=NULL;
    index_xoffset=index_yoffset=0;
    select_buffer=NULL;
    
    if (filename)  
        editim=load(filename,num_images,pal);
    else
    {
        pal=new palette;
        memcpy(pal->addr(),default_palette,768);
        strcpy(fname,"noname.spe");
    }
    
    if (current_error())
        return ;  
    
    if (!filename)
    {
        empty_start=1;
        strcpy(fname,"noname.spe");
    }
    else
        strcpy(fname,filename);
    bright_color=pal->brightest(1);
    strcmp(command,"");
    change_state(EDIT_MODE);
    f.set(0, 0);
    for (i = 1; i < 256; i++)
        f.set(i, (char) bright_color);
    
/*    if (xres < 400)
        fnt_pic=new image(160, 56, (unsigned char*) fnt5x7); 
    else */
    fnt_pic=new image(192, 104, (unsigned char*) fnt6x13);
    
    f.apply(fnt_pic); 
    set_font(new JCFont(fnt_pic));
    
    edit=new editor();
    cur_image=0;
    zoom=1; zoomx=0; zoomy=0;
    pal->load();
    set_current_color(0);
    
    points_window=new_window=NULL;
}

void painter::title_screen()
{
    image satan(80,60,spaint);
    image copyr(264,28,copyright);
    image sn(133,21,spaint_name);
    
    int iy=yres/2-satan.height()/2-(copyr.height()+sn.height())/2;
    
    sn.put_image(screen,xres/2-sn.width()/2,iy);
    
    int lx=xres/2-satan.width()/2,ly=iy+sn.height()+4;
    screen->rectangle(lx-1,ly-1,lx+satan.width(),ly+satan.height(),eh->bright_color());
    satan.put_image(screen,lx,ly);
    copyr.put_image(screen,xres/2-copyr.width()/2,iy+satan.height()+4+sn.height()+1);
    
    
    flush_screen();
}


void painter::frame(short x1, short y1, short x2, short y2, int c)
{
    if (y1>y2)
    {
        int tmp=y1;
        y1=y2;
        y2=tmp;
    }  
    if (x1>x2)
    {
        int tmp=x1;
        x1=x2;
        x2=tmp;
    }  
    
    image *im=current_image();
    if (x2!=x1)
    {
        draw_area(im,x1,y1,x2,y1,c);
        draw_area(im,x1,y2,x2-1,y2,c);
    }
    if (y2!=y1)
    {
        draw_area(im,x2,y1+1,x2,y2,c);
        draw_area(im,x1,y1+1,x1,y2-1,c);
    }
}

int select_points(int message_id)
{
    switch(message_id)
    {
    case FORETILE_SHOW_POINTS : return 0; break;
    case FORETILE_HIDE_POINTS : return 0; break;
    case FORETILE_EDIT_POINTS : return 0; break;
    case FIGURE_SHOW_HIT : return 0; break;
    case FIGURE_HIDE_HIT : return 0; break;
    case FIGURE_EDIT_HIT : return 0; break;
    case FIGURE_SHOW_DAMAGE : return 1; break;
    case FIGURE_HIDE_DAMAGE : return 1; break;
    case FIGURE_EDIT_DAMAGE : return 1; break;
    case FIGURE_SHOW_TOUCH : return 2; break;
    case FIGURE_HIDE_TOUCH : return 2; break;
    case FIGURE_EDIT_TOUCH : return 2; break;
    case FIGURE_SHOW_XCENTER : return 3; break;
    case FIGURE_HIDE_XCENTER : return 3; break;
    case FIGURE_EDIT_XCENTER : return 3; break;  
    }
    return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void painter::handle_event(event &ev)
{
    char msg[100];
    short mx,my;
    sat_handle_event(ev);
    if (ev.type == EV_MESSAGE) { // first look to see if we got a state change event
        if (ev.message.id>=FIRST_MODE && ev.message.id<=LAST_MODE)
            change_state(ev.message.id);  
    }
    else if (ev.type==EV_RESIZE) {
        calc_areas(); 
        draw();
        
    }
    else if (ev.type==EV_REDRAW)
        screen->add_dirty(ev.redraw.x1,ev.redraw.y1,ev.redraw.x2,ev.redraw.y2);
    else if (ev.type == EV_KEY && ev.window == NULL) {
        if (ev.key=='1' && !rcommand)
            do_command("palette toggle",ev);
        else if (edit_state(state)) {
            if (!rcommand) {
                keyed_command *k=key_list.find_key(ev.key);
                if (k)            
                    do_command(k->command(),ev);
                else {
                    switch(ev.key) {
                    case '>' : 
                    case 'n' : 
                        { next_image(); }  break;
                    case '<' : { last_image(); } break;
                    case '/' : { rcommand=1; strcpy(command,""); } break;
                    case 'a' : do_command("animate",ev); break;
                    case 'A' : do_command("anti",ev); break;	     
                    case 'b' : do_command("blur",ev); break;	      
                    case 'B' : do_command("blur_color",ev); break;	     		
                    case 'u' : do_command("undo",ev); break;
                    case 'e' : if (num_images) edit->toggle_window(); break;
                    case 's' : save_stuff(); break;
                    case 'D' : if (num_images) delete_image(""); break;
                    case 'c' : copy_image(); break;
                    case 'f' : {
                            short fx,fy;  
                            mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,fx,fy);
                            fill_image(fx,fy); 
                        }
                        break;
                    case 'p' : paste(ev,1); break;
                    case 'P' : paste(ev,0); break;
                    case '+' : brush_size++;  change_cursor(); break;
                    case '-' : if (brush_size>1) { brush_size--; change_cursor(); } break;
                    case ' ' : do_command("select",ev); break;
                    case 'w' : do_command("where",ev); break;
                    case 't' : do_command("tile",ev); break;
                    case 'i' : do_command("index",ev); break;
                    case 'm' : do_command("zoom min",ev); break;
                    case 'M' : do_command("zoom max",ev); break;
                    case 'z' : do_command("zoom relative 1",ev); break;
                    case '[' : do_command("color_add -1",ev); break;
                    case ']' : do_command("color_add 1",ev); break;
                    case 'Z' : do_command("zoom relative -1",ev); break;
                    case 'l' : {
                            mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,selx1,sely1);
                            selx2=selx1; sely2=sely1;
                            next_state=state;
                            change_state(LINE_SELECT);
                        } break;
                    case 'C' : {
                            mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,selx1,sely1);
                            selx2 = selx1; sely2 = sely1;
                            next_state = state;
                            change_state(CIRCLE_SELECT); 
                        } break;
                    case JK_DOWN :
                    case JK_UP :
                    case JK_LEFT :
                    case JK_RIGHT : pan(ev.key); break;
                    }
                }
            }

            if (rcommand) {
                if (ev.key==13 || ev.key==27)
                    rcommand=0;
                if (ev.key==13)
                    do_command(command,ev);
                else if (ev.key==8) {
                    if (strlen(command) > 0)
                        command[strlen(command)-1] = 0;
                }
                else if (ev.key == JK_UP) {
                    if (hist_on > 0) {
                        hist_on--;
                        strcpy(command, command_hist[hist_on]);		  
                    }
                }
                else if (ev.key == JK_DOWN) {
                    if (hist_on<tot_hist) {
                        hist_on++;
                        strcpy(command, command_hist[hist_on]);		  
                    }
                }
                else {
                    if ((ev.key!='/' || strlen(command)>0) && rcommand) {
                        if (isprint(((char) ev.key))) {
                            command[strlen(command) + 1] = 0;
                            command[strlen(command)] = ev.key;
                        }
                    }
                } 
                sprintf(msg,"Cmd>%s",command,1);
                info(msg);
            }
        }
    }
    
    switch (state) {
    case MODE_ZOOM_IN : {
            if (ev.type==EV_MOUSE_BUTTON && ev.mouse_button && ev.window==NULL)
                do_command("zoom relative 2",ev);
        } break;
    case MODE_ZOOM_OUT : {
            if (ev.type==EV_MOUSE_BUTTON && ev.mouse_button && ev.window==NULL)
                do_command("zoom relative -2",ev);
        } break;
    case MODE_IMAGE_DRAW : {
            if ((ev.type==EV_MOUSE_BUTTON || ev.type==EV_MOUSE_MOVE) && ev.mouse_button && ev.window==NULL) {
                if (ev.mouse_button==2)
                    paste(ev,0);
                else
                    paste(ev,1);
            }
            else if (ev.type==EV_KEY) {
                switch (ev.key) {
                case 'x' : 
                    { if (select_buffer) select_buffer->flip_x(); } break;
                case 'y' :
                    { if (select_buffer) select_buffer->flip_y(); } break;	  
                } 
            }
        } break;
    case MODE_FILL : {
            if (ev.type==EV_MOUSE_BUTTON && ev.mouse_button && ev.window==NULL) {
                short fx,fy;
                mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,fx,fy);      	
                fill_image(fx,fy);
            }
        } break;
    case MODE_POLY_SELECT : {
            if (ev.type==EV_MOUSE_BUTTON) {
                if (ev.mouse_button)       	  
                    reset_selection_list();
                else
                {
                    if (area_sel_total)
                        add_sel_point(area_sel_list[0],area_sel_list[1]);
                    sel_mask=make_select_mask(area_sel_list,area_sel_total);
                }
            } else if (ev.type==EV_MOUSE_MOVE && ev.mouse_button)
            {
                short fx,fy,lx,ly;
                mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,fx,fy);
                get_last_sel_point(lx,ly);
                if (fx!=lx || fy!=ly)
                    add_sel_point(fx,fy);
            }
            
            
        } break;
    case MODE_AREA_SELECT :
        {
            if (ev.type==EV_MOUSE_BUTTON && ev.window==NULL && num_images)
            {
                if (ev.mouse_button) 
                {
                    reset_selection_list();	  
                    short fx,fy;
                    mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,fx,fy);
                    selx1=selx2=fx;
                    sely1=sely2=fy;
                } else
                {	  
                    frame(selx1,sely1,selx2,sely2,-1);
                    add_sel_point(selx1,sely1);
                    add_sel_point(selx2,sely1);
                    add_sel_point(selx2,sely2);
                    add_sel_point(selx1,sely2);
                    add_sel_point(selx1,sely1);
                    sel_mask=make_select_mask(area_sel_list,area_sel_total);
                }
            } else if (ev.type==EV_MOUSE_MOVE && ev.mouse_button && ev.window==NULL)
            {
                short fx,fy;
                mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,fx,fy);
                frame(selx1,sely1,selx2,sely2,-1);
                sely2=fy; selx2=fx;
                frame(selx1,sely1,selx2,sely2,current_color());
            }
        } break;
        
    case MODE_LINE :
        {
            if (ev.type==EV_MOUSE_BUTTON && ev.mouse_button && ev.window==NULL)
            {
                mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,selx1,sely1);
                selx2=selx1; sely2=sely1;
                next_state=MODE_LINE;
                change_state(LINE_SELECT); break;
            }
        } break;
    case MODE_CIRCLE :
        {
            if (ev.type==EV_MOUSE_BUTTON && ev.mouse_button && ev.window==NULL && num_images)
            {
                mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,selx1,sely1);
                selx2=selx1; sely2=sely1;
                next_state=state;
                change_state(CIRCLE_SELECT);
            }
        } break;
    case MODE_RECT :
        {
            if (ev.type==EV_MOUSE_BUTTON && ev.mouse_button && ev.window==NULL)
            {
                mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,selx1,sely1);
                selx2=selx1; sely2=sely1;
                next_state=state;
                change_state(RECT_SELECT);
            }
        } break;
    case MODE_BAR :
        {
            if (ev.type==EV_MOUSE_BUTTON && ev.mouse_button && ev.window==NULL)
            {
                mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,selx1,sely1);
                selx2=selx1; sely2=sely1;
                next_state=state;
                change_state(RECT_SELECT);
            }
        } break;
        
    case MOUSE_STATE_CHANGE :  // wait for mouse button is released before going to state
        {
            if (ev.type==EV_MOUSE_BUTTON && ev.mouse_button==0)
                change_state(next_state);
        } break;
    case ANIMATING :
        {      
            switch (ev.type)
            {
            case EV_MESSAGE :     
                {	  
                    switch (ev.message.id)
                    {	        
                    case PAINT_ANIMATE_DONE : change_state(EDIT_MODE); 
                        ((button *)ev.message.data)->push(); break;
                    case PAINT_ANIMATE_SPEED_UP : ani_sleep-=10; if (ani_sleep<10) ani_sleep=1; break;
                    case PAINT_ANIMATE_SLOW_DOWN : ani_sleep+=10; break;
                    case PAINT_ANIMATE_BACKWARD :
                    case PAINT_ANIMATE_FORWARD : ani_dir=-ani_dir; break;
                    }  
                }		   
            }  break;
        } break;
        
        
        
        
        
        
        
        // this state sets up the config for doing a X_CENTER 
    case POINT_SELECT_XCENTER_START :
        current_points=select_points(FIGURE_EDIT_XCENTER);
        total_points[current_points]=1;
        lastpx=-1;
        change_state(POINT_SELECT_XCENTER);      // now go to the meat of the handler
        break;
        
    case POINT_SELECT_XCENTER : {
        current_points=select_points(FIGURE_EDIT_XCENTER);
        switch (ev.type) {
        case EV_MOUSE_MOVE : {
            mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,mx,my);
            if (mx>=0 && my>=0) { // make sure the mx & my are withon the image
                point_data[current_points][0] = (unsigned char) mx;
                if (lastpx>=0)      // see if we have to erase the old one
                    line(lastpx,0,lastpx,current_image()->height()-1,bright_color,MODE_CLEAR);
                
                line(mx,0,mx,current_image()->height()-1,bright_color,MODE_SHOW);
                lastpx=mx;       
            } 
                             } break;
            
        case EV_MOUSE_BUTTON : {
            if (ev.mouse_button==1) 
                eh->push_event(new event(PAINT_OK_POINTS,NULL)); 
                               } break;
        }
                                } break;  
        
    case POINT_SELECT_START :   
        if (!ev.window) {
            total_points[current_points]=0;
            change_state(POINT_SELECT);
        }
        break;    
    case POINT_SELECT :
        if (!ev.window) {
            mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,mx,my);
            if (mx<0) mx=0;
            if (my<0) my=0;
            if (mx>=current_image()->width()) mx=current_image()->width()-1;
            if (my>=current_image()->height()) my=current_image()->height()-1;
            
            
            point_data[current_points][total_points[current_points]*2] = (unsigned char) mx;
            point_data[current_points][total_points[current_points]*2+1] = (unsigned char) my;
            if (ev.type==EV_MOUSE_BUTTON && ev.mouse_button==1) {
                if (total_points[current_points]==256) {
                    info("Maximum number of line segments=256");
                    eh->push_event(new event(PAINT_OK_POINTS,NULL)); 
                }
                else {
                    if (total_points[current_points])
                        if (point_data[current_points][total_points[current_points]*2]==
                            point_data[current_points][0] &&
                            point_data[current_points][total_points[current_points]*2+1]==
                            point_data[current_points][1])
                            eh->push_event(new event(PAINT_OK_POINTS,NULL)); 
                        total_points[current_points]++;
                        lastpx=mx; lastpy=my;
                }
            } 
            else if (ev.type==EV_MOUSE_MOVE) {
                int i;
                if (total_points) {
                    line(point_data[current_points][(total_points[current_points]-1)*2],
                        point_data[current_points][(total_points[current_points]-1)*2+1],
                        lastpx,lastpy,bright_color,MODE_CLEAR);
                    for (i=1;i<=total_points[current_points];i++)
                        line(point_data[current_points][(i-1)*2],
                        point_data[current_points][(i-1)*2+1],
                        point_data[current_points][i*2],
                        point_data[current_points][i*2+1],bright_color,MODE_SHOW);
                    lastpx=mx; lastpy=my;
                }
            }
        }
        break;
        
    case LINE_SELECT :
        if (!ev.window)
        {
            mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,mx,my);
            if (mx!=selx2 || my!=sely2)
            {
                line(selx1,sely1,selx2,sely2,current_color(),MODE_CLEAR);
                sely2=my; selx2=mx;
                line(selx1,sely1,selx2,sely2,current_color(),MODE_SHOW);
            } 
            if (ev.type==EV_MOUSE_BUTTON && ev.mouse_button)
            {
                line(selx1,sely1,selx2,sely2,current_color(),MODE_MODIFY);
                change_state(next_state);
            }
        } break;
    case CIRCLE_SELECT :
        if (!ev.window)
        {
            if (!num_images) return ;
            mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,mx,my);
            if (mx!=selx2 || my!=sely2)
            {
                circle(selx1,sely1,max(abs(selx1-selx2),abs(sely1-sely2)),current_color(),MODE_CLEAR);
                sely2=my; selx2=mx;
                circle(selx1,sely1,max(abs(selx1-selx2),abs(sely1-sely2)),current_color(),MODE_SHOW);
            } 
            if (ev.type==EV_MOUSE_BUTTON && ev.mouse_button)
            {
                add_undo(cur_image,undo_change);	
                circle(selx1,sely1,max(abs(selx1-selx2),abs(sely1-sely2)),current_color(),MODE_MODIFY);
                change_state(next_state);
            }
        } break; 
    case RECT_SELECT :
        if (!ev.window)
        {
            if (!num_images) return ;
            mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,mx,my);
            if (mx!=selx2 || my!=sely2)
            {
                image *im=current_image();
                frame(selx1,sely1,selx2,sely2,-1);
                sely2=my; selx2=mx;
                frame(selx1,sely1,selx2,sely2,current_color());
            }
            
            if (ev.type==EV_MOUSE_BUTTON && ev.mouse_button!=state_enter_button_status)
            {
                add_undo(cur_image,undo_change);	
                if (next_state==MODE_BAR)
                {
                    if (selx1>selx2) { int tmp=selx2; selx2=selx1; selx1=tmp; }
                    if (sely1>sely2) { int tmp=sely2; sely2=sely1; sely1=tmp; }
                    
                    current_image()->bar(selx1,sely1,selx2,sely2,current_color());
                    draw_area(current_image(),selx1,sely1,selx2,sely2);
                }
                else 
                    current_image()->rectangle(selx1,sely1,selx2,sely2,current_color());
                if (ev.mouse_button)
                    mouse_change_state(next_state);
                else change_state(next_state);
            }      
        } break;
    case MODE_TILE :
        {
            if (ev.type==EV_KEY) change_state(EDIT_MODE);
        } break;
    case ANIMATE_PICK :
    case INDEX_PICK :
        if (!ev.window)
        { CHECK(indexes);
        Index *i,*found=NULL;
        for (i=indexes;i && !found;i=i->next)
            if (i->in_area(ev.mouse_move.x-index_xoffset,(int)ev.mouse_move.y-index_yoffset)) found=i; 
            i=found;
            if (last_index && found!=last_index)
                screen->rectangle(last_index->x1+index_xoffset,last_index->y1+index_yoffset,
                last_index->x2+index_xoffset,last_index->y2+index_yoffset,0); 
            if (found) {
                screen->rectangle(i->x1+index_xoffset,i->y1+index_yoffset,
                    i->x2+index_xoffset,i->y2+index_yoffset, (unsigned char) bright_color); 
                last_index=i;
            }
            if (ev.type==EV_CLOSE_WINDOW && ev.window==animate_pick_window) {
                eh->close_window(animate_pick_window);
                animate_pick_window=NULL;
                change_state(INDEX_PICK);
            }
            
            if (ev.type==EV_MOUSE_BUTTON)
                if (ev.mouse_button==1) {
                    if (state==ANIMATE_PICK) {
                        if (last_index) {
                            add_animate_frame(last_index->image_num);
                            print_animation_sequence(animate_pick_window->screen,WINDOW_FRAME_LEFT+1,
                                animate_pick_window->y2()-eh->font()->height());
                        }
                    }	  
                    else          
                        mouse_change_state(EDIT_MODE);	  
                }
                else if (ev.mouse_button==2) {
                    if (last_index)
                        pch->notify_image_click(last_index->image_num);
                }
                
                int redraw=0;
                
                if (ev.type==EV_KEY)
                    switch  (ev.key) {
    case JK_DOWN : index_yoffset-=yres/4; redraw=1; break;
    case JK_UP : index_yoffset+=yres/4; redraw=1; break;
    case JK_LEFT : index_xoffset+=xres/4; redraw=1; break;
    case JK_RIGHT : index_xoffset-=xres/4; redraw=1; break;
    case 'f' : index_flags^=SHOW_PROP; redraw=1; break;	   	   
    case ' ' : if (last_index) set_current(last_index->image_num); break;
    case 'a' : if (!animate_pick_window)
                   change_state(ANIMATE_PICK);
        animate_pick_window=eh->new_window(0,-1,300,40,
            new button(WINDOW_FRAME_LEFT,WINDOW_FRAME_TOP+1,PAINT_ANIMATE_CLEAR,"CLEAR",
            new button(WINDOW_FRAME_LEFT+60,WINDOW_FRAME_TOP+1,PAINT_ANIMATE,"ANIMATE",NULL))); 
        eh->font()->put_string(animate_pick_window->screen,80,0,"Animation sequence");	
        break;
                }
                if (redraw) {
                    draw_indexes(index_xoffset,index_yoffset);
                    last_index=0;
                }
                if (ev.type==EV_CLOSE_WINDOW && ev.window==animate_pick_window) {
                    eh->close_window(animate_pick_window);
                    animate_pick_window=NULL;	
                    change_state(INDEX_PICK);
                }      
        } break;
    case AREA_SELECT :
        if (!ev.window)
        {
            if (!num_images) return ;
            if (selx1!=-1)
            {
                if (selx1==-2)
                { if (ev.mouse_button==0)
                selx1=-1;
                }
                else
                {
                    mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,mx,my);
                    if (mx<0) mx=selx1;
                    if (my<0) my=sely1;
                    if (mx>=current_image()->width())
                        mx=current_image()->width()-1;
                    if (my>=current_image()->height())
                        my=current_image()->height()-1;
                    if (mx!=selx2 || my!=sely2)
                    {
                        frame(selx1,sely1,selx2,sely2,-1);
                        selx2=mx; sely2=my;
                        frame(selx1,sely1,selx2,sely2,bright_color);
                    } 
                    if (ev.mouse_button!=sel_but)
                    {
                        if (selx1>selx2)
                        {
                            int tmp=selx1;
                            selx1=selx2;
                            selx2=tmp;	    
                        }
                        if (sely1>sely2)
                        {
                            int tmp=sely1;
                            sely1=sely2;
                            sely2=tmp;	    
                        }	  
                        
                        frame(selx1,sely1,selx2,sely2,-1);
                        if (select_buffer) delete select_buffer;
                        select_buffer=new image(selx2-selx1+1,sely2-sely1+1);
                        sprintf(msg,"Area (%d,%d-%d,%d) copy to select buffer (%d,%d)",selx1,sely1,selx2,sely2,
                            selx2-selx1+1,sely2-sely1+1);
                        info(msg);
                        current_image()->put_part(select_buffer,0,0,selx1,sely1,selx2,sely2);
                        last_select.image_number=cur_image;
                        last_select.x1=selx1;
                        last_select.y1=sely1;
                        last_select.x2=selx2;
                        last_select.y2=sely2;	  
                        
                        if (ev.mouse_button!=0)  // if the user selected this area by pressing a mouse
                            selx1=-2; ///button, then set the flag to wait for the release
                        change_state(EDIT_MODE);
                    }
                }
            } break;
        }
    case WAIT_KEY :
        if (ev.type==EV_KEY) 
        {
            screen->clear();
            draw();	
            ani_frame=-1;
            change_state(EDIT_MODE);
        }  break;
    case EDIT_MODE :
        {
            switch (ev.type)
            { 
            case EV_MOUSE_MOVE :
            case EV_MOUSE_BUTTON : mouse_click(ev); break;
            }
        } break; 
  }
  if (ev.type==EV_MESSAGE)
      switch (ev.message.id)
  {
      case SAT_R :
          {
              sat_r=(int)ev.message.data;
              sat_render(current_image(),palc->get_filter());
              redraw_sel_area();
          } break;
      case SAT_G :
          {
              sat_g=(int)ev.message.data;
              sat_render(current_image(),palc->get_filter());
              redraw_sel_area();
          } break;
      case SAT_B :
          {
              sat_b=(int)ev.message.data;
              sat_render(current_image(),palc->get_filter());
              redraw_sel_area();
          } break;
          
      case PAINT_ANIMATE_CLEAR : clear_animate_frames(); break;
      case PAINT_ANIMATE : 
          {	
              if (animate_pick_window)
              {
                  eh->close_window(animate_pick_window);
                  animate_pick_window=NULL;	  
              }	
              change_state(ANIMATING);  ani_frame=0;  
          }	 break;
          
          
      case EV_CLOSE_WINDOW :       
          if (ev.window==new_window)
          { eh->close_window(new_window); new_window=NULL; } 
          else if (ev.window==points_window)
              eh->push_event(new event(PAINT_OK_POINTS,NULL)); 
          else if (ev.window==animate_pick_window)
          {	  
              eh->close_window(animate_pick_window);
              animate_pick_window=NULL;	  
          }	
          break;
      case PAINT_NEW_IMAGE : sprintf(msg,"new %s %s %s",
                                 new_window->inm->get(PAINT_NEW_W)->read(), 
                                 new_window->inm->get(PAINT_NEW_H)->read(), 
                                 new_window->inm->get(PAINT_NEW_NAME)->read()); do_command(msg,ev); 
          eh->close_window(new_window); new_window=NULL; break; 
      case PAINT_NEW_BACKTILE : sprintf(msg,"newb %s %s %s",
                                    new_window->inm->get(PAINT_NEW_W)->read(), 
                                    new_window->inm->get(PAINT_NEW_H)->read(), 
                                    new_window->inm->get(PAINT_NEW_NAME)->read()); do_command(msg,ev); 
          eh->close_window(new_window); new_window=NULL; break; 
      case PAINT_NEW_FORETILE : sprintf(msg,"newf %s %s %s",
                                    new_window->inm->get(PAINT_NEW_W)->read(), 
                                    new_window->inm->get(PAINT_NEW_H)->read(), 
                                    new_window->inm->get(PAINT_NEW_NAME)->read()); do_command(msg,ev); 
          eh->close_window(new_window); new_window=NULL; break; 
      case PAINT_NEW_CHARACTER : sprintf(msg,"newc %s %s %s", 
                                     new_window->inm->get(PAINT_NEW_W)->read(), 
                                     new_window->inm->get(PAINT_NEW_H)->read(), 
                                     new_window->inm->get(PAINT_NEW_NAME)->read()); do_command(msg,ev); 
          eh->close_window(new_window); new_window=NULL; break; 
          
      case FIGURE_EDIT_XCENTER :
          points_window=eh->new_window(0,0,200,10,NULL);
          eh->font()->put_string(points_window->screen,
              points_window->x1()+2,points_window->y1()+2,"Click on x center");
          ((button *)ev.message.data)->push();
          edit->redraw_input();
          change_state(POINT_SELECT_XCENTER_START);
          break;      
      case FIGURE_HIDE_XCENTER :  
      case FIGURE_SHOW_XCENTER :
          {      
              current_points=select_points(ev.message.id);
              mx=point_data[current_points][0];      
              line(mx,0,mx,current_image()->height()-1,bright_color,
                  (ev.message.id==FIGURE_HIDE_XCENTER) ? MODE_CLEAR : MODE_SHOW);   
              ((button *)ev.message.data)->push();
              edit->redraw_input();
          } break;    	
          
      case FORETILE_EDIT_POINTS : 
      case FIGURE_EDIT_DAMAGE : 
      case FIGURE_EDIT_TOUCH : 
      case FIGURE_EDIT_HIT :           
          if (!points_window)
          {
              current_points=select_points(ev.message.id);        
              total_points[current_points]=0;
              points_window=eh->new_window(-1,-1,0,0, 
                  new button(WINDOW_FRAME_LEFT,WINDOW_FRAME_TOP,PAINT_OK_POINTS,"Done",
                  new button(WINDOW_FRAME_LEFT,WINDOW_FRAME_TOP+17,PAINT_FINISH_POINTS,"Finish",
                  NULL)));
              change_state(POINT_SELECT_START);
              ((button *)ev.message.data)->push();
              edit->redraw_input();
          } break;
          
      case PAINT_OK_POINTS : if (points_window)
                             { eh->close_window(points_window); points_window=NULL; 
          change_state(EDIT_MODE);
          draw_image(current_image(),1);
                             } break;
      case PAINT_FINISH_POINTS : if (points_window)
                                 { eh->close_window(points_window); points_window=NULL; 
          
          if (total_points[current_points])
          {	  
              point_data[current_points][total_points[current_points]*2]=point_data[current_points][0];
              point_data[current_points][total_points[current_points]*2+1]=point_data[current_points][1];
              total_points[current_points]++;
          }
          
          change_state(EDIT_MODE);
          draw_image(current_image(),1);
                                 } break;
          
      case FORETILE_SHOW_POINTS : 
      case FIGURE_SHOW_TOUCH : 
      case FIGURE_SHOW_DAMAGE : 
      case FIGURE_SHOW_HIT :
          {
              current_points=select_points(ev.message.id);
              int i; for (i=1;i<total_points[current_points];i++)
                  line(point_data[current_points][(i-1)*2],
                  point_data[current_points][(i-1)*2+1],
                  point_data[current_points][i*2],
                  point_data[current_points][i*2+1],bright_color,MODE_SHOW);
              ((button *)ev.message.data)->push();
              edit->redraw_input();
          } break;
          
      case FORETILE_HIDE_POINTS :
      case FIGURE_HIDE_TOUCH : 
      case FIGURE_HIDE_DAMAGE : 
      case FIGURE_HIDE_HIT :
          {
              current_points=select_points(ev.message.id);
              int i; for (i=1;i<total_points[current_points];i++)
                  line(point_data[current_points][(i-1)*2],point_data[current_points][(i-1)*2+1],
                  point_data[current_points][i*2],point_data[current_points][i*2+1],bright_color,MODE_CLEAR);
              ((button *)ev.message.data)->push();
              edit->redraw_input();
          } break;
          
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void painter::reset_selection_list()
{
    int d=0;
    while (sel_mask) {
        select_line *p=sel_mask;
        sel_mask=sel_mask->next;
        delete p;
    }
    
    if (area_sel_list) {
        jfree(area_sel_list);
        d=1;
    }
    area_sel_list=NULL;
    area_sel_total=0;
    if (num_images && d && draw_selection_state(state))
        draw_image(current_image(),1);  
    
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void painter::draw_sel_list(int offset)
{
    if (area_sel_total<2 || !area_sel_list) return ;
    int lx=area_sel_list[0],ly=area_sel_list[1];
    ushort *l=area_sel_list+2;
    for (int i=1;i<area_sel_total;i++) {
        stagger_line(lx,ly,l[0],l[1],eh->medium_color(),eh->black(),(i+offset)&1);
        lx=l[0]; ly=l[1];
        l+=2;
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void painter::get_last_sel_point(short &lx,short &ly)
{
    if (area_sel_total>0)
    {
        lx=area_sel_list[area_sel_total*2-2];
        ly=area_sel_list[area_sel_total*2-1];
    }
    else
    {
        lx=-1;
        ly=-1;
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void painter::add_sel_point(short fx,short fy)
{
    if (fx<0) fx=0;
    if (fy<0) fy=0;
    if (!num_images) return ;
    if (fx>=current_image()->width()) fx=current_image()->width()-1;
    if (fy>=current_image()->height()) fy=current_image()->height()-1;
    area_sel_total++;
    area_sel_list=(ushort *)jrealloc(area_sel_list,area_sel_total*sizeof(short)*2,"selection list");
    area_sel_list[area_sel_total*2-2]=fx;
    area_sel_list[area_sel_total*2-1]=fy;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void painter::save_stuff()
{
    char st[100];
    int i;
    sprintf(st,"satan.bak");
    unlink(st);
    rename(fname,st);
    
    jFILE *fp;
    
    
    fp=new jFILE(fname,"wb");  // checkt o see if we have write permission to this file
    if (fp->open_failure())
    { 
        delete fp;
        info("could not open file to save, check permissions");
        return ;
    }
    delete fp;
    unlink(fname);   // delete the file we created so that errors don't get prtined in spec_directory
    
    
    sprintf(st,"Saving to %s...",fname);
    info(st);
    eh->flush_screen();
    
    // create the directory of items and calculate the offsets for the items.
    
    spec_directory sd;  
    for (i=0;i<num_images;i++)
        sd.add_by_hand(new spec_entry(editim[i]->spec_type(),editim[i]->name(),NULL,editim[i]->size(),0));
    sd.add_by_hand(new spec_entry(SPEC_PALETTE,"palette",NULL,pal->size(),0));
    if (palc->get_filter(0))
        sd.add_by_hand(new spec_entry(SPEC_COLOR_TABLE,"color_filter",NULL,palc->get_filter()->size(),0));
    
    sd.calc_offsets();
    
    fp=sd.write(fname);  // write the directory to the file
    
    for (i=0;i<num_images;i++)    
        editim[i]->write(fp);       // now save all the items to the file too
    
    pal->write(fp);
    if (palc->get_filter(0))
        palc->get_filter()->write(fp);
    delete fp;
    sd.delete_entries();
    
    info("done.");
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void free_up_memory()
{
    if (!palc->kill_table()) {
        printf("Out of memory\n");
        exit(0);
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void painter::set_font(JCFont *new_font)
{
    if (font) delete font;
    font=new_font;
    calc_areas();        // different font sizes cause the info area to change.
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
start_spaint(int argc, char **argv)
{
    short i,pal_loaded=0;
    event ev;
    jrand_init();
    image_init();
    set_frame_size(0);
    
    if (argc==1 || (*argv[1])=='-')
        paint=new painter(NULL);
    else {
        pal_loaded=1;
        paint=new painter(argv[1]);
    }
    
    if (current_error()) {
        image_uninit();
        clear_errors();    
    }
    
    eh = new window_manager(screen,paint->pal,
        paint->closest_color(255,255,255,paint->pal),
        paint->closest_color(90,90,90,paint->pal),
        paint->closest_color(40,40,40,paint->pal),font);
    pch = new pm_cmd_handler;
    
    if (!eh->has_mouse()) {
        close_graphics();
        image_uninit();
        printf("No mouse driver detected, please rectify.\n");
        return 1;
    }
    stat_man = new gui_status_manager();
    
    if (yres > 200)
        palc = new paint_pal(eh, paint->pal, yres / 40);
    else
        palc = new paint_pal(eh, paint->pal, yres / 36);

    palc->load_stuff(paint->fname);
    
    tool_box=new tools(palc->current_pal());
    
    char st[150],file_end=0;  
    int start = 2;
    if (!pal_loaded)
        start = 1;
    for (i = start; i < argc; i++) {
        if (!file_end && *argv[i] != '-') {
            sprintf(st, "merge %s", argv[i]);    
            paint->do_command(st, ev);    
        }
        else {
            file_end = 1;
            if (!strcmp(argv[i],"-undo")) {
                i++;
                paint->set_undo(atoi(argv[i]));
            }
            else if (!strcmp(argv[i],"-seq")) {
                i++;
                paint->load_sequence(argv[i],!pal_loaded,0);
            }
            else if (!strcmp(argv[i],"-sseq")) {
                i++;
                paint->load_sequence(argv[i],!pal_loaded,1);
            }
        }
    }

    char fullname[100], tmp[120];
    sprintf(fullname, "%s/satan.rc", getenv("HOME"));
#define R_OK 04

    if (access(fullname, R_OK) == 0) {
        sprintf(tmp, "read %s", fullname);
        paint->do_command(tmp, ev);
    }
    
    else if (access("satan.rc", R_OK) == 0)
        paint->do_command("read satan.rc", ev);

    int last_cursor_x = -1, last_cursor_y;
    
    pch->draw(screen, eh);
    paint->draw();
    do {
        if (eh->event_waiting()) {
            paint->last_button=ev.mouse_button;
            
            eh->get_event(ev);
            
            while (ev.type==EV_MOUSE_MOVE && eh->event_waiting())
                eh->get_event(ev);       
            if (!pch->handle_event(ev,menu_state(paint->state))) {
                if (!palc->handle_event(ev))
                    if (!paint->edit->handle_event(ev)) 
                        if ((ev.window==paint->title_window && ev.type==EV_CLOSE_WINDOW) ||
                            (ev.type==EV_MESSAGE && ev.message.id==PAINT_CLOSE_TITLE)) {
                            eh->close_window(paint->title_window);
                            paint->title_window=NULL;
                        }
                        paint->handle_event(ev);
                        if (ev.type==EV_MOUSE_MOVE) {
                            short mx,my;
                            paint->mouse_image_position(ev.mouse_move.x,ev.mouse_move.y,mx,my);
                            if (mx != last_cursor_x || my != last_cursor_y) {
                                paint_cursor.restore();
                                last_cursor_x = mx;
                                last_cursor_y = my;
                                paint_cursor.draw(mx, my);
                            }
                        }
            }
        }
        else {
            paint->step();
        }
        paint->flush_screen();
    } while (!(ev.type == EV_KEY && ev.key == 'q' && ev.mouse_button == 1) && !paint->quit_signal);
    delete paint;
    delete eh;
    return 0;  
}