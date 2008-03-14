#include "tools.hpp"
#include "Include/id.h"
#include "Include/paint.h"


tool_picker::~tool_picker() {
    delete old_pal;
    delete map;
}

void tool_picker::remap(palette *pal, image *screen)
{
    delete map;
    map=new filter(old_pal,pal);
    draw_first(screen);
}

tool_picker::tool_picker(int X, int Y, int ID, 
                         int show_h, visual_object **Icons, int *Ids, int total_ic, 
                         palette *icon_palette, palette *pal, ifield *Next) :
spicker(X, Y, ID, show_h, 1, 1, 0, Next)
{
    iw=ih=0;
    icons=Icons;
    ids=Ids;
    total_icons=total_ic;
    for (int i=0;i<total_ic;i++) {
        if (icons[i]->width()>iw)
            iw=icons[i]->width();
        if (icons[i]->height()>ih)
            ih=icons[i]->height();
    }
    map=new filter(icon_palette,pal);
    old_pal=icon_palette->copy();
    reconfigure();
}

void tool_picker::draw_item(image *screen, int x, int y, int num, int active)
{
    if (!active)
        screen->bar(x,y,x+iw-1,y+ih-1,eh->black());
    else
        screen->bar(x,y,x+iw-1,y+ih-1,eh->bright_color());    
    icons[num]->draw(screen, x, y, map); //+js
    // js map->put_image(screen, icons[num], x, y, 1);
}