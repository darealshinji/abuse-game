#ifndef __OTREE_HPP_
#define __OTREE_HPP_

#include "palette.hpp"

class onode {
public:
    int value;               // value <0 indicates the node is not 
    // a real color and further searching is required
    onode *children[8];
    onode() { int i; for (i=0;i<8;i++) children[i]=NULL; value=-1; }  // no children to start  
    onode *right_most();
    onode *left_most();
};

class otree {
    onode *root;
public:
    otree(palette *pal);
    unsigned char lookup_color(unsigned char red, unsigned char green, unsigned char blue);	
    void make_links(onode *root, palette *pal);
};

#endif