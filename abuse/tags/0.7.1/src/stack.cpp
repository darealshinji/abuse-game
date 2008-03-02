/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#ifndef __STACK_HPP_
#define __STACK_HPP_

template <class T> grow_stack        // stack does not shrink
{ 
  public :
  T **sdata;
  long ssize,son;

  grow_stack() { sdata=NULL; ssize=0; son=0; }
  void *push(T *data) 
  { if (son>=ssize) 
    { ssize+=0x100;
      sdata=(T **)jrealloc(sdata,sizeof(T *)*ssize,"stack");
    }
    sdata[son++]=data;
  }
   
  T *pop(long total) 
  { if (son>total) { lbreak("stack underflow\n"); exit(0); }
    son-=total;
    return sdata[son];
  }
} ;

#endif
