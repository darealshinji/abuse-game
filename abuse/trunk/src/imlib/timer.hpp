/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __TIMER_HPP_
#define __TIMER_HPP_
typedef void (*int_handler)();
void init_timer(int_handler int_proc, long utime);
#endif

