/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
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

static inline int Min(int a, int b) { return a < b ? a : b; }
static inline int Max(int a, int b) { return a > b ? a : b; }
static inline float Min(float a, float b) { return a < b ? a : b; }
static inline float Max(float a, float b) { return a > b ? a : b; }

#endif
