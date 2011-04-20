/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

//
// Lol Engine
//
#include "lol/matrix.h"
using namespace lol;

//
// Custom utility functions
//
static inline int Min(int a, int b) { return a < b ? a : b; }
static inline int Max(int a, int b) { return a > b ? a : b; }
static inline float Min(float a, float b) { return a < b ? a : b; }
static inline float Max(float a, float b) { return a > b ? a : b; }

#endif // __COMMON_H__

