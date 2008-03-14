#include "dprint.hpp"
#include <stdio.h>
#include "macs.hpp"
#include <string.h>

#include "windows.h"

uchar major_version = 2;
uchar minor_version = 00;

extern int get_option(char *name);

static void setup()
{
    dprintf("-------- Abuse (Version %d.%02d) --------\n", major_version, minor_version);
#ifdef WIN32
    dprintf("Win32/DirectX port by Jeremy Scott (scot4875@uidaho.edu)\n");
    dprintf("Built %s\n", __DATE__);
    dprintf("Check www.abuse2.com for updates\n\n");
    Sleep(750);
#endif
}

void show_verinfo(int argc, char **argv)
{
    setup();
    if (major_version < 1) {
    fprintf(stderr, "*******************************************************\n"
                    "This is the final beta before we ship.\n"
                    "Please report any bugs to abuse-bugs@crack.com.  Include\n"
                    "game version number and your system specifications.\n"
                    "*** Finger abuse-bugs@crack.com or check\n"
                    "http://www.crack.com for the latest version number\n"
                    "before submitting any bug reports.\n"
                    "*******************************************************\n\n");
  }
}
