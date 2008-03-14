#include "macs.hpp"
//#include "mdlread.hpp"
#include "video.hpp"
#include "image.hpp"
#include "palette.hpp"
#include "linked.hpp"
#include <math.h>
#include "glread.hpp"
//#include "gifread.hpp"
//#include "ppmread.hpp"
//#include "emm.hpp"
#include "sprite.hpp"


/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void sprite::restore_background()
{ 
    if (x + save->width() >= 0 && y+save->height() >= 0 && x <= (signed) xres && y <= (signed) yres)
        save->put_image(screen, x, y); 
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void sprite::get_background() 
{ 
    if (x+visual->width()>=0 && y+visual->height()>=0 && x <= (signed) xres && y <= (signed) yres)
        screen->put_part(save,0,0,x,y,x+save->width()-1,y+save->height()-1); 
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void sprite::draw()
{
    if (x+visual->width()>=0 && y+visual->height()>=0 && x<= (signed) xres && y<= (signed) yres)
        visual->put_image(screen,x,y,1);
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
sprite::sprite(image *Screen, image *Visual, int X, int Y)
{
    CHECK(Visual && Screen);
    x=X; y=Y; visual=Visual; screen=Screen;
    save=new image(visual->width(),visual->height());
    get_background();
};

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
sprite::~sprite()
{
    if (visual)
        delete visual;
    if (save)
        delete save;
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void sprite_controller::add_sprite(sprite *sp)
{ sprites.add_end((linked_node *)sp); }

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void sprite_controller::remove_sprites()
{ sprite *sp; loopt(sprite,sp,sprites.first(),sp->restore_background();); }

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void sprite_controller::put_sprites()
{ sprite *sp; loopt(sprite,sp,sprites.first(),sp->draw();); }

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void sprite_controller::get_backgrounds()
{ sprite *sp; loopt(sprite,sp,sprites.first(),sp->get_background();); }

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void sprite::change_visual(image *Visual, int delete_old)
{ 
    if (visual)
        delete visual;
    visual = Visual->copy();
    image_list.unlink(visual);
    if (save->width()!=Visual->width() || save->height()!=Visual->height()) {
        delete save;
        save = new image(visual->width(),visual->height());
        image_list.unlink(save);
    }
    get_background();
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void sprite_controller::bring_front(sprite *sp)
{
    WriteErr(sprites.unlink((linked_node *)sp),"unlink failure");
    sprites.add_end((linked_node *)sp);
}

/* --------------------------------------------------------------------------- 
------------------------------------------------------------------------- /**/
void sprite_controller::delete_sprite(sprite *sp)
{
    WriteErr(sprites.unlink((linked_node *)sp),"unlink failure");
    delete sp;
}
