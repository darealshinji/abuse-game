/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef MACS__
#define MACS__
#include "system.h"
#include <stdio.h>
#define ERROR(x,st) { if (!(x)) \
   { printf("Error on line %d of %s : %s\n", \
     __LINE__,__FILE__,st); exit(1); } }

// These macros should be removed for the non-debugging version
#ifdef NO_CHECK
#define CONDITION(x,st) 
#define CHECK(x) 
#else
#define CONDITION(x,st) ERROR(x,st)
#define CHECK(x) CONDITION(x,"Check stop");
#endif


#ifndef min
#define min(x,y) (x<y ? x:y)
#endif
#ifndef max
#define max(x,y) (x>y ? x:y)
#endif

//#define uchar  unsigned char
//#define schar  signed char
//#define ushort unsigned short
//typedef unsigned short int ushort;
//#define sshort signed short
//#define ulong  unsigned long
//typedef unsigned long int ulong;

#endif
