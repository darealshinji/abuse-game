/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __STACK_HPP_
#define __STACK_HPP_
#ifndef NO_LIBS
#include "jmalloc.hpp"
#else
#include "fakelib.hpp"
#endif

#include <stdio.h>
struct cons_cell;

template<class T> class grow_stack        // stack does not shrink
{ 
  public :
  T **sdata;
  long son;
  long smax;

  grow_stack(int max_size) {
    smax=max_size;
    son=0;
    sdata=(T **)jmalloc(sizeof(T *)*smax,"pointer stack");
  }
  void push(T *data) 
  {
    if (son>=smax) { lbreak("stack overflow (%ld)\n",smax); exit(1); }
    sdata[son]=data;
    son++;
  }
   
  T *pop(long total) 
  { if (total>son) { lbreak("stack underflow\n"); exit(1); }
    son-=total;
    return sdata[son];
  }
  void clean_up() 
  { 
    if (son!=0) fprintf(stderr,"Warning cleaning up stack and not empty\n");
    jfree(sdata); 
    sdata=NULL;  son=0; 
  }
} ;

#endif
