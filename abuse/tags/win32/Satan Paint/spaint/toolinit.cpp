#include "Include/id.h"
#include "Include/toolinit.h"
#include "tools.hpp"
#include "icon.cpp"
extern palette *pal;
extern window_manager *eh;

#define TCONS 11

uchar *ic_ptrs[TCONS] = {
    pixel_mode, area_select, image_mode,
    fill_mode, line_mode, rect_mode, bar_mode,
    zoom_in, zoom_out, circle_mode, index_mode
};

int ic_ids[TCONS] = {
    EDIT_MODE,
    MODE_AREA_SELECT,
    MODE_IMAGE_DRAW,
    MODE_FILL,
    MODE_LINE,
    MODE_RECT,
    MODE_BAR,
    MODE_ZOOM_IN,
    MODE_ZOOM_OUT,
    MODE_CIRCLE,
    INDEX_PICK,
};


image_visual *ticons[TCONS];

tools::tools(palette *pal)
{
    for (int i=0;i<TCONS;i++)
        ticons[i]=new image_visual(new image(16,16,ic_ptrs[i]));
    
    
    
    palette *ic_pal=new palette();
    memcpy(ic_pal->addr(),image_mode_palette,768);  
    tw=eh->new_window(-1,-1,-1,-1,new tool_picker(WINDOW_FRAME_LEFT,WINDOW_FRAME_TOP, 
                        MODE_BUTTON_BOX, 5, (visual_object**) ticons, ic_ids, TCONS,
                        ic_pal, pal, NULL));
    delete ic_pal;
}


void tools::remap(palette *to)
{
    if (tw)
        ((tool_picker*) tw->inm->get(MODE_BUTTON_BOX))->remap(to, tw->screen);
}

tools *tool_box;