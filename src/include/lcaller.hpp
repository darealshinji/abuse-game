/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __LCALLER_HPP_
#define __LCALLER_HPP_
#include "objects.hpp"
#include "chars.hpp"
#include "game.hpp"

extern voi lcaller_init
extern long lcaller(long number, void *arg);   // exten c function switches on
                                               // number and is given lisp object arg as parameter

#endif
