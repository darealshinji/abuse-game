#include "joy.hpp"
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <stdlib.h>


int use_joy = 1,
    joy_centx = 50,
    joy_centy = 50;    // which joystick do you want to use?
               
int joy_init(int argc, char **argv)
{
    int i;  
    for (i = 1; i < argc; i++)
    if (!strcmp(argv[i], "-joy"))
        use_joy = 1;
    else 
        if (!strcmp(argv[i], "-joy2"))
            use_joy = 2;
        else 
            if (!strcmp(argv[i], "-nojoy"))
                use_joy = 0;
  return use_joy;
}             

void joy_status(int &b1, int &b2, int &b3, int &xv, int &yv)
{
    if (use_joy) {  // make sure that we detected one before
// Insert Windows-Specific Code
// Jeremy Scott 6/25/2000
    }
    else 
        xv = yv = b1 = b2 = b3 = 0;
}


void joy_calibrate()
{
// Don't need under Win32
}





