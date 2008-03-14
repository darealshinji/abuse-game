#ifndef __MACS__INCLUDED
#define __MACS__INCLUDED
#include "system.h"
#include <stdio.h>
#include <windows.h>

void close_graphics();
void Shutdown(int Line, char* SrcFile, int code);

extern char ErrMsg[1024];

#ifdef _WIN32
void WINAPI KillDInput();
void set_dprinter(void (*stat_fun)(char *));       // called with debug info
#define WriteErr(x, st) { if (!(x)) { \
    if (st) \
        sprintf(ErrMsg, st); \
    Shutdown(__LINE__, __FILE__, -1); } }

#endif

#ifdef _DEBUG
#undef NO_CHECK
#else
#define NO_CHECK
#endif
// These macros should be removed for the non-debugging version
#ifdef NO_CHECK
    #define CONDITION(x,st) 
    #define CHECK(x) 
#else
    #define CONDITION(x,st) WriteErr(x,st)
    #define CHECK(x) CONDITION(x,"Check stop");
#endif

#ifndef min
#define min(x,y) (x<y ? x:y)
#endif
#ifndef max
#define max(x,y) (x>y ? x:y)
#endif

#define uchar  unsigned char
#define schar  signed char
#define ushort unsigned short
#define sshort signed short
#define ulong  unsigned long

#endif
