/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __EXITPROC_HPP_
#define __EXITPROC_HPP_

#include <stdlib.h>

#ifdef HAVE_ATEXIT
#define exit_proc(x,y) atexit(y)
#elif defined (HAVE_ON_EXIT)
#define exit_proc(x,y) on_exit(x,NULL)
#else
#error no atexit?!?
#endif


#endif
