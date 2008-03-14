#ifndef __UNDO_HPP_
#define __UNDO_HPP_
#include "items.h"
#include "palette.hpp"
#include "linked.hpp"

enum undo_actions 
{ 
    undo_delete,
    undo_change,
    undo_create, 
    undo_palette
};

class undo_marker : public linked_node {
    imitem *saved;
    palette *old_pal;
    int action, num;
public:
    undo_marker(int image_number, int action_to_occur, palette *pal=NULL);  
    void restore();    
    ~undo_marker() { if (saved) delete saved; if (old_pal) delete old_pal; }  	         
};

#endif
