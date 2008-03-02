/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "system.h"

main()
{
#if BYTE_ORDER==LITTLE_ENDIAN
  printf("Hello little\n");
#else 
  printf("hi big\n");
#endif
}

