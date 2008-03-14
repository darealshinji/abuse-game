#ifndef _TOOLINIT_HPP_
#define _TOOLINIT_HPP_

#include "jwindow.hpp"

class tools
{
    jwindow *tw;
public:
    void remap(palette *to);
    tools(palette *pal);
};

extern tools *tool_box;
#endif