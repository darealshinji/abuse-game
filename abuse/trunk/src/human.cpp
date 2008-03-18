/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "human.hpp"
#include "game.hpp"


void human::do_damage(int amount, game_object *from)
{
  game_object::do_damage(amount,from);
  the_game->show_help("Ouch, that really hurt!");

//  if (!gravity())
//    set_state(flinch);
}


int human::move(int cx, int cy, int button)
{
/*  if (state==hanging)
  {
    int y_moveup[15]
    if (end_of_sequence())
    {
      int xv=direction,yv


    }*/
  return game_object::move(cx,cy,button);



}

