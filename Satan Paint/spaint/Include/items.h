#ifndef __ITEMS_H_INCLUDED_
#define __ITEMS_H_INCLUDED_
#include "image.hpp"
#include "specs.hpp"
#include "fonts.hpp"
#include "jwindow.hpp"
#include "input.hpp"
#include "points.h"
#include <stdio.h>

class imitem {
protected:
    char *Name;
public:
    image *im;
    char *st_width() { static char st[10]; sprintf(st,"%d",im->width()); return st; }
    char *st_height() { static char st[10]; sprintf(st,"%d",im->height()); return st; }
    
    virtual int size() { return 4+im->width()*im->height(); }
    virtual int write(jFILE *fp);
    virtual int spec_type() { return SPEC_IMAGE; }
    virtual void make_property_string(char *buffer) { sprintf(buffer,"type image, name %s",name()); }
    virtual void scale(int nw, int nh) { im->resize(nw,nh); }
    
    virtual ~imitem() { jfree(Name); delete im; }
    char *name() { return Name; }
    
    virtual ifield *fields(ifield *next);
    virtual void get_fields(input_manager *im);
    virtual imitem *copy(char *name) 
    { return new imitem(im->copy(),name); }
    
    
    void set_name(char *n) { Name=strcpy((char *)jrealloc(Name,strlen(n)+1,"item name"),n); }
    imitem(spec_entry *e, jFILE *fp) { im=new image(e,fp); Name=NULL; set_name(e->name); }
    imitem(image *Image, char *name) {im=Image; Name=NULL; set_name(name); }
    imitem() { Name=NULL; im=NULL; }
};

class particle : public imitem {
public:
    signed short cx,cy;
    virtual int size();
    virtual int write(jFILE *fp);
    virtual int spec_type() { return SPEC_PARTICLE; }
    virtual void make_property_string(char *buffer)
    { sprintf(buffer,"type particle, name %s, cx %d, cy %d",name(),cx,cy); }
    
    particle(spec_entry *e, jFILE *fp);
    particle(image *Image, char *name) {im=Image; Name=NULL; set_name(name); 
    cx=im->width()/2; cy=im->height()/2; }
    virtual imitem *copy(char *name) 
    { particle *p=new particle(im->copy(),Name); p->cx=cx; p->cy=cy; return p; }
    
};

class backtile : public imitem {
public:
    unsigned short next_pic;
    virtual int size();
    virtual int write(jFILE *fp);
    virtual int spec_type() { return SPEC_BACKTILE; }
    virtual void make_property_string(char *buffer) 
    { sprintf(buffer,"type backtile, next %d, name %s",next_pic,name()); }
    
    virtual ifield *fields(ifield *next);
    virtual void get_fields(input_manager *im);
    backtile(image *Image, char *name) : imitem(Image,name) { next_pic=0; }
    backtile(spec_entry *e, jFILE *fp);
    virtual imitem *copy(char *name) 
    { backtile *b=new backtile(im->copy(),name); b->next_pic=next_pic; 
    return (imitem *) b; }
};

class figure : public imitem {
public:
    point_list *damage,*hit;
    unsigned char hit_damage,xcfg;
    signed char advance;
    
    virtual ~figure() { delete damage; delete hit; }
    virtual void make_property_string(char *buffer);
    void set_damage(unsigned char how_many, unsigned char *data)
    { delete damage; damage=new point_list(how_many,data); }
    void set_hit(unsigned char how_many, unsigned char *data)
    { delete hit; hit=new point_list(how_many,data); }
    virtual void scale(int nw, int nh);
    
    figure(spec_entry *e, jFILE *fp);
    figure(image *Image, char *name) : imitem(Image,name) 
    { hit_damage=0; damage=new point_list; hit=new point_list; 
    xcfg=Image->width()/2;
    advance=0;
    }
    virtual int size();
    virtual int write(jFILE *fp);  
    virtual int spec_type() { return SPEC_CHARACTER2; }
    virtual ifield *fields(ifield *next);
    virtual void get_fields(input_manager *im);
    virtual imitem *copy(char *name) 
    { figure *f=new figure(im->copy(),name); f->hit_damage=hit_damage;
    f->set_hit(hit->tot,hit->data);
    f->set_damage(damage->tot,damage->data);
    f->xcfg=xcfg;    
    f->advance=advance;
    return (imitem *)f;
    }
};

class foretile : public imitem {
public:
    unsigned short next_pic;
    unsigned char damage;
    point_list *p;
    virtual ~foretile() { delete p; }
    virtual int size();
    virtual int write(jFILE *fp);
    virtual int spec_type() { return SPEC_FORETILE; }
    virtual void make_property_string(char *buffer);
    virtual ifield *fields(ifield *next);
    virtual void get_fields(input_manager *im);
    virtual void scale(int nw, int nh);
    
    void set_points(unsigned char how_many, unsigned char *data)
    { delete p; p=new point_list(how_many,data); }
    
    foretile(image *Image, char *name) : imitem(Image,name) 
    { next_pic=0; damage=0; p=new point_list; }
    
    foretile(spec_entry *e, jFILE *fp);
    
    virtual imitem *copy(char *name) 
    { foretile *f=new foretile(im->copy(),name);
    f->set_points(p->tot,p->data);
    return (imitem *)f;
    } 
};

#endif
