/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __SYS__
#define __SYS__


#ifdef WORDS_BIGENDIAN
  #define BIG_ENDIANS
#else
  #define LITTLE_ENDIANS
#endif


#include <unistd.h>
#include <stdint.h>

#define uint16_swap(x) (((((uint16_t) (x)))<<8)|((((uint16_t) (x)))>>8))
#define uint32_swap(x) \
   ((( ((uint32_t)(x)) )>>24)|((( ((uint32_t)(x)) )&0x00ff0000)>>8)| \
   ((( ((uint32_t)(x)) )&0x0000ff00)<<8)|(( ((uint32_t)(x)) )<<24))

#if defined BIG_ENDIANS
#define LONG int32_t
#define uint16_to_intel(x) uint16_swap(x)
#define uint16_to_local(x) uint16_to_intel(x)
#define big_uint32_to_local(x) (x)
#define big_uint16_to_local(x) (x)
#define uint32_to_intel(x) uint32_swap(x)
#define uint32_to_local(x) uint32_to_intel(x)
#else
#define LONG int32_t
#define uint16_to_intel(x) (x)
#define uint16_to_local(x) (x)
#define uint32_to_local(x) (x)
#define uint32_to_intel(x) (x)
#define big_uint32_to_local(x) uint32_swap(x)
#define big_uint16_to_local(x) uint16_swap(x)
#endif

#define bltl(x) big_uint32_to_local(x)
#define bstl(x) big_uint16_to_local(x)
#define lltl(x) uint32_to_intel(x)
#define lstl(x) uint16_to_intel(x)

#endif




