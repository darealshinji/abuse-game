/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

//
// Globally required headers
//
#include <stdint.h>
#include <stdio.h>

//
// Lol Engine
//
#include <lol/engine.h>
using namespace lol;

//
// Byte swapping
//
static inline int BigEndian()
{
    union { uint32_t const x; uint8_t t[4]; } const u = { 0x01ffff00 };
    return u.t[0];
}

static inline uint16_t lstl(uint16_t x)
{
    if (BigEndian())
        return ((uint16_t)x << 8 ) | ((uint16_t)x >> 8);
    return x;
}

static inline uint32_t lltl(uint32_t x)
{
    if (BigEndian())
        return ((uint32_t)x >> 24) | (((uint32_t)x & 0x00ff0000) >> 8)
             | (((uint32_t)x & 0x0000ff00) << 8) | ((uint32_t)x << 24);
    return x;
}

#endif // __COMMON_H__

