#include "Include/undo.h"
#include "Include/paint.h"

undo_marker::undo_marker(int image_number, int action_to_occur, palette *pal)
{
    action=action_to_occur;
    num=image_number;  
    saved=NULL;
    old_pal=NULL;
    
    if (action==undo_palette)
        old_pal=pal->copy();  
    else if (action!=undo_create)
        saved=paint->get_item(image_number)->copy(paint->get_item(image_number)->name());     
}


void undo_marker::restore()
{
    char com[100];
    
    switch (action)
    {
    case undo_delete :
        paint->insert_imitem(num,saved->copy(saved->name())); break;
    case undo_create :
        sprintf(com,"%d",num);   
        paint->delete_image(com); break;
    case undo_change :
        delete paint->editim[num];
        paint->editim[num]=saved->copy(saved->name());  break;
    case undo_palette :
        palc->change_palette(old_pal); break;              
    }  
}