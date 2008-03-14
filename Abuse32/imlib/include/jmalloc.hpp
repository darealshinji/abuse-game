#ifndef __jmalloc_hpp_
#define __jmalloc_hpp_


#include <stdlib.h>
#define MANAGE_MEM

#ifdef MANAGE_MEM
enum { ALLOC_SPACE_STATIC,ALLOC_SPACE_CACHE };
extern int alloc_space;

#ifndef __NM //_WIN32
void *jmalloc(long size, char *what_for);
void *jrealloc(void *ptr, long size, char *what_for);
void jfree(void *ptr);
#else
#define jmalloc(x,y) malloc(x)
#define jrealloc(x,y,z) realloc(x,y)
#define jfree(x) free(x)
#endif
void mem_report(char *filename);
void jmalloc_init(long min_size);
void jmalloc_uninit();
long j_allocated();
long j_available();
//extern void free_up_memory();
#else
enum { ALLOC_SPACE_STATIC,ALLOC_SPACE_CACHE };
#define jmalloc(x,y) malloc(x)
#define jrealloc(x,y,z) realloc(x,y)
#define jfree(x) free(x)
#define jmalloc_init(x)
#define mem_report(x)
#define small_ptr_size(x) 128
#define small_static_allocation_summary(x, y, z)
#define jmalloc_min_low_size 0
extern int alloc_space;
#define j_allocated() 0
#define j_available() 0
#endif

#endif





