/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __STACK_HPP_
#define __STACK_HPP_

#include <stdio.h>
#include <stdlib.h>

// A fixed-size stack class
template<class T> class grow_stack
{
public:
    T **sdata;
    long son;

private:
    long smax;

public:
    grow_stack(int max_size)
    {
        smax = max_size;
        son = 0;
        sdata = (T **)malloc(sizeof(T *) * smax);
    }

    void push(T *data)
    {
        if(son >= smax)
        {
            lbreak("error: stack overflow (%ld >= %ld)\n", son, smax);
            exit(1);
        }
        sdata[son] = data;
        son++;
    }

    T *pop(long total)
    {
        if (total > son)
        {
            lbreak("error: stack underflow\n");
            exit(1);
        }
        son -= total;
        return sdata[son];
    }

    void clean_up()
    {
        if(son != 0)
            fprintf(stderr, "warning: cleaning up stack and not empty\n");
        free(sdata);
        sdata = NULL;
        son = 0;
    }
};

#endif
