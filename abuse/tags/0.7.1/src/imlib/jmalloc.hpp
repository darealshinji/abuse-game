/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __jmalloc_hpp_
#define __jmalloc_hpp_


#include <stdlib.h>

#ifdef MANAGE_MEM
enum {ALLOC_SPACE_STATIC,ALLOC_SPACE_CACHE};
extern int alloc_space;
void *jmalloc(int32_t size, char const *what_for);
void *jrealloc(void *ptr, int32_t size, char const *what_for);
void jfree(void *ptr);
void mem_report(char const *filename);
void jmalloc_init(int32_t min_size);
void jmalloc_uninit();
int32_t j_allocated();
int32_t j_available();
extern void free_up_memory();
#else
#define jmalloc(x,y) malloc(x)
#define jrealloc(x,y,z) realloc(x,y)
#define jfree(x) free(x)
#endif




#endif





