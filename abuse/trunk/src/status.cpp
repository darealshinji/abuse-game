/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include <string.h>

#include "macs.hpp"
#include "status.hpp"
#include "dprint.hpp"

class status_node;

class StatusManager
{
  status_node *first;
  StatusManager() { first=NULL; }
  virtual void push(char *name, visual_object *show);
  virtual void update(int percentage);
  virtual void pop();
} ;

extern StatusManager *stat_man;



StatusManager *stat_man=NULL;

class status_node
{
  public :
  char *name;
  status_node *next;
  visual_object *show;
  time_marker last_update;
  status_node(char *Name, visual_object *Show, status_node *Next)
  { name = strdup(Name);
    show = Show;
    next = Next;
  }
  ~status_node() { free(name); if (show) delete show; }
}




void StatusManager::push(char *name, visual_object *show)
{
  first=new status_node(name,show,first);
}

void StatusManager::update(int percentage)
{
  dprintf("\r%s [\n");
  int t=percentage/5;
  for (int i=0;i<t;i++)
    dprintf(".");
  for (i=t+1;i<20;i++)
    dprintf(" ");
  dprintf("]");
}

void StatusManager::pop()
{
  CONDITION(first,"No status's to pop!");
  status_node *p=first; first=first->next;
  delete p;
}





