/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __HUMAN_HPP_
#define __HUMAN_HPP_
#include "objects.hpp"
#include "ability.hpp"

class human : public game_object
{

public :  
  human(long X, long Y) { defaults(); x=X; y=Y; }
  human(FILE *fp, unsigned char *state_remap) { load(fp,state_remap); }
  virtual int size() { return game_object::size(); }
  virtual game_objects type() { return O_human; }
  virtual void do_damage(int amount, game_object *from);
  virtual int move(int cx, int cy, int button);  // return false if the route is blocked
} ;

#endif





