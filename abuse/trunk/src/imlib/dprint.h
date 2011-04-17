/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef DPRINT_HPP_
#define DPRINT_HPP_

void set_dprinter(void (*stat_fun)(char *));       // called with debug info
void set_dgetter(void (*stat_fun)(char *, int));   // called mainly by lisp breaker
void dprintf(const char *format, ...);
void dgets(char *buf, int size);


#endif
