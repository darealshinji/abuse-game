/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "stack.h"
#ifndef __LISP_GC_HPP_
#define __LISP_GC_HPP_

extern grow_stack<void> l_user_stack;       // stack user progs can push data and have it GCed

void collect_space(int which_space, int grow); // should be tmp or permenant

void register_pointer(void *&addr);
void unregister_pointer(void *&addr);

// This pointer reference stack lists all pointers to temporary lisp
// objects. This allows the pointers to be automatically modified if an
// object allocation triggers a garbage collection.
class PtrRef
{
public:
    template<typename T> inline PtrRef(T *&ref)
    {
        stack.push((void **)&ref);
    }

    template<typename T> inline PtrRef(T * const &ref)
    {
        stack.push((void **)&ref);
    }

    inline ~PtrRef()
    {
        stack.pop(1);
    }

    // stack of user pointers, user pointers get remapped on GC
    static grow_stack<void *>stack;
};

#endif

