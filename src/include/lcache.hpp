/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __LCACHE_HPP_
#define __LCACHE_HPP_
#include "lisp.hpp"

#ifdef SCADALISP
#define can_cache_lisp() 0
#else
#define can_cache_lisp() 0 /* XXX */
#endif

long block_size(Cell *level);              // return number of bytes to save this block of code
void write_level(bFILE *fp, Cell *level);
Cell *load_block(bFILE *fp);

#endif
