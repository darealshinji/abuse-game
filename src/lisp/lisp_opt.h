/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __LISP_OPT_HPP_
#define __LISP_OPT_HPP_

#include "lisp.h"

extern LispObject *l_undefined;
extern LispSymbol *true_symbol;

extern void *list_symbol,*string_symbol,     // in lisp_init()
           *quote_symbol,*backquote_symbol,*comma_symbol,*do_symbol,*in_symbol,*aref_symbol,
       *colon_initial_contents,*colon_initial_element,*if_symbol,
       *progn_symbol,*car_symbol,*cdr_symbol,*load_warning;

#endif
