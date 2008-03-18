/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "lisp.hpp"

char const *lang_string(char const *symbol)
{
  void *v=find_symbol(symbol);
  if (!v || !DEFINEDP(symbol_value(v))) return "Language symbol missing!";
  else return lstring_value(symbol_value(v));
}

