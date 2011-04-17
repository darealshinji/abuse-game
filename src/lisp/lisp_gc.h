/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "stack.h"
#ifndef __LISP_GC_HPP_
#define __LISP_GC_HPP_

extern grow_stack<void> l_user_stack;       // stack user progs can push data and have it GCed
extern grow_stack<void *>l_ptr_stack;      // stack of user pointers, user pointers get remapped on GC

void collect_space(int which_space); // should be tmp or permenant

void register_pointer(void *&addr);
void unregister_pointer(void *&addr);

class p_ref {
  public :
  p_ref(void *&ref) { l_ptr_stack.push(&ref); }
  p_ref(LispObject *&ref) { l_ptr_stack.push((void **)&ref); }
  p_ref(LispArray *&ref) { l_ptr_stack.push((void **)&ref); }
  ~p_ref() { l_ptr_stack.pop(1); }
} ;


#endif
