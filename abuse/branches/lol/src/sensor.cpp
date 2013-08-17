/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include "common.h"

#include "lisp.h"
#include "lisp_gc.h"
#include "compiled.h"
#include "objects.h"
#include "level.h"
#include "game.h"
#include "clisp.h"

enum { un_offable };     // vars

void *sensor_ai()
{
  GameObject *o=current_object,*b;
  if (o->aistate()==0)                     // turned off, what for player to enter
  {
    if (player_list->next)                 // find closest player
      b=g_current_level->attacker(current_object);
    else b=player_list->m_focus;
    if (lol::abs(b->m_pos - o->m_pos) < o->m_vel) // inside area?
    {
      if (!o->hp())
        o->set_aistate(1);
      else
        o->set_aistate(o->hp());
      o->set_state((character_state)S_blocking);
    } else if (o->state!=stopped)
      o->set_state(stopped);
  } else if (!o->lvars[un_offable])
  {
    if (!o->hp())
    {
      if (player_list->next)
        b=g_current_level->attacker(current_object);
      else b=player_list->m_focus;
      if (!(lol::abs(o->m_pos - b->m_pos) <= o->m_accel))
        o->set_aistate(0);
    } else o->set_aistate(o->aistate()-1);
  }
  return true_symbol;
}