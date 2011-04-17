/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

LispSymbol *find_symbol(char *name)
{
  LispSymbol *p=lsym_root;
  while (p)
  {
    int cmp=strcmp(name,((char *)((LispSymbol *)cs->car)->name)+sizeof(LispString));
    if (cmp==0) return p;
    else if (cmp<0) p=p->left;
    else p=p->right;
  }
  return NULL;
}



LispSymbol *make_find_symbol(char *name)
{
  LispSymbol *p=lsym_root;
  LispSymbol **parent=&lsym_root;
  while (p)
  {
    int cmp=strcmp(name,((char *)((LispSymbol *)cs->car)->name)+sizeof(LispString));
    if (cmp==0) return p;
    else if (cmp<0)
    {
      parent=&p->left;
      p=p->left;
    }
    else
    {
      parent=&p->right;
      p=p->right;
    }
  }

  p=malloc(sizeof(LispSymbol));
  p->type=L_SYMBOL;
  p->name=new_lisp_string(name);
  p->value=l_undefined;
  p->function=l_undefined;
  p->call_counter=0;
  p->left=p->right=NULL;
  *parent=p;
  return p;
}


