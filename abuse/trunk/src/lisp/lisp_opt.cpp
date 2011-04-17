/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#ifdef NO_LIBS
#include "fakelib.h"
#else
#include "macs.h"
#endif

#include "lisp.h"
#include "lisp_gc.h"

void *true_symbol=NULL,*l_undefined,*list_symbol,*string_symbol,     // in lisp_init()
     *quote_symbol,*backquote_symbol,*comma_symbol,*do_symbol,*in_symbol,*aref_symbol,
     *colon_initial_contents,*colon_initial_element,*if_symbol,
     *progn_symbol,*eq_symbol,*zero_symbol,*eq0_symbol,*car_symbol,*cdr_symbol,
     *load_warning;


void *if_1progn,*if_2progn,*if_12progn,*not_symbol;

void *comp_optimize(void *list)
{
  void *return_val=list;
  p_ref r1(list);
  if (list)
  {
    if (CAR(list)==if_symbol)
    {
      void *eval1=lcar(lcdr(lcdr(list)));
      p_ref r2(eval1);
      void *eval2=lcar(lcdr(lcdr(lcdr(list))));
      p_ref r3(eval2);

      void *ret=NULL;
      p_ref r1(ret);
      if (lcar(list)==eq_symbol && (lcar(lcdr(list))==zero_symbol))  //  simplify (eq 0 x) -> (eq0 x)
      {
    push_onto_list(lcar(lcdr(lcdr(list))),ret);
    push_onto_list(eq0_symbol,ret);
    return_val=comp_optimize(ret);
      } else if (lcar(list)==eq_symbol &&
         (lcar(lcdr(lcdr(list)))==zero_symbol)) //simplify (eq x 0)-> (eq0 x)
      {
    push_onto_list(lcar(lcdr(list)),ret);
    push_onto_list(eq0_symbol,ret);
    return_val=comp_optimize(ret);
      } else if (lcar(lcar(lcdr(list)))==not_symbol)  // simplify (if (not y) x z) -> (if y z x)
      {
    push_onto_list(lcar(lcdr(lcdr(list))),ret);
    push_onto_list(lcar(lcdr(lcdr(lcdr(list)))),ret);
    push_onto_list(lcar(lcdr(lcar(lcdr(list)))),ret);
    push_onto_list(if_symbol,ret);
    return_val=comp_optimize(ret);
      }
      else if (lcar(eval1)==progn_symbol && (eval2==NULL ||
                         item_type(eval2)!=L_CONS_CELL))
      {
    push_onto_list(eval2,ret);
    push_onto_list(lcdr(eval1),ret);
    push_onto_list(lcar(lcdr(list)),ret);
    push_onto_list(if_1progn,ret);
    return_val=comp_optimize(ret);
      } else if (lcar(eval1)==progn_symbol && lcar(eval2)==progn_symbol)
      {
    push_onto_list(lcdr(eval2),ret);
    push_onto_list(lcdr(eval1),ret);
    push_onto_list(lcar(lcdr(list)),ret);
    push_onto_list(if_12progn,ret);
    return_val=comp_optimize(ret);
      } else if (lcar(eval2)==progn_symbol)
      {
    push_onto_list(lcdr(eval2),ret);
    push_onto_list(eval1,ret);
    push_onto_list(lcar(lcdr(list)),ret);
    push_onto_list(if_2progn,ret);
    return_val=comp_optimize(ret);
      }

    }
  }
  return return_val;
}

void l_comp_init()
{
    // This needs to be defined first
    l_undefined = LispSymbol::FindOrCreate(":UNDEFINED");

    // Collection problems result if we don't do this
    ((LispSymbol *)l_undefined)->function = NULL;
    ((LispSymbol *)l_undefined)->value = NULL;

    true_symbol = LispSymbol::FindOrCreate("T");

    list_symbol = LispSymbol::FindOrCreate("list");
    string_symbol = LispSymbol::FindOrCreate("string");
    quote_symbol = LispSymbol::FindOrCreate("quote");
    backquote_symbol = LispSymbol::FindOrCreate("backquote");
    comma_symbol = LispSymbol::FindOrCreate("comma");
    in_symbol = LispSymbol::FindOrCreate("in");
    do_symbol = LispSymbol::FindOrCreate("do");
    aref_symbol = LispSymbol::FindOrCreate("aref");
    colon_initial_contents = LispSymbol::FindOrCreate(":initial-contents");
    colon_initial_element = LispSymbol::FindOrCreate(":initial-element");

    if_1progn = LispSymbol::FindOrCreate("if-1progn");
    if_2progn = LispSymbol::FindOrCreate("if-2progn");
    if_12progn = LispSymbol::FindOrCreate("if-12progn");
    if_symbol = LispSymbol::FindOrCreate("if");
    progn_symbol = LispSymbol::FindOrCreate("progn");
    not_symbol = LispSymbol::FindOrCreate("not");
    eq_symbol = LispSymbol::FindOrCreate("eq");
    zero_symbol = LispSymbol::FindOrCreate("0");
    eq0_symbol = LispSymbol::FindOrCreate("eq0");
    car_symbol = LispSymbol::FindOrCreate("car");
    cdr_symbol = LispSymbol::FindOrCreate("cdr");
    load_warning = LispSymbol::FindOrCreate("load_warning");
}

