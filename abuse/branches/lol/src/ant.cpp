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

#include <ctype.h>

#include "common.h"

#include "ant.h"
#include "lisp.h"
#include "lisp_gc.h"
#include "compiled.h"
#include "objects.h"
#include "level.h"
#include "game.h"
#include "clisp.h"
#include "dev.h"

enum {  ANT_need_to_dodge,     // ant vars
    ANT_no_see_time,
    ANT_hide_flag };

int can_see(GameObject *o, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
  int32_t nx2=x2,ny2=y2;
  g_current_level->foreground_intersect(x1,y1,x2,y2);
  if (x2!=nx2 || y2!=ny2) return 0;

  g_current_level->boundary_setback(o,x1,y1,x2,y2);
  return (x2==nx2 && y2==ny2);
}


// if we first saw the player or it's been a while since we've seen the player then do a scream
static void scream_check(GameObject *o, GameObject *b)
{
  if (can_see(o,o->m_pos.x, o->m_pos.y, b->m_pos.x, b->m_pos.y))
  {
    if (o->lvars[ANT_no_see_time]==0 || o->lvars[ANT_no_see_time]>20)
      the_game->play_sound(S_ASCREAM_SND, 127, o->m_pos.x, o->m_pos.y);
    o->lvars[ANT_no_see_time]=1;
  } else o->lvars[ANT_no_see_time]++;
}

static int ant_congestion(GameObject *o)
{
  for (GameObject *d=g_current_level->first_active_object(); d; d=d->next_active)
  {
    if (d->otype == o->otype && lol::abs(o->m_pos.x - d->m_pos.x) < 30
         && lol::abs(o->m_pos.x - d->m_pos.y) < 20)
      return 1;
  }
  return 0;
}

static int ant_dodge(GameObject *o)
{
  if (o->lvars[ANT_need_to_dodge]==1)
  {
    o->lvars[ANT_need_to_dodge]=0;
    if (rand(2) == 0)
    {
      o->set_state(stopped);
      o->set_aistate(ANT_JUMP);
      if (!can_see(o, o->m_pos.x, o->m_pos.y, o->m_pos.x, o->m_pos.y - 120))   // is there a roof above?
      {
        o->m_vel = ivec2(0, -17);
        o->set_aistate(ANT_JUMP_ROOF);
        ant_ai();
      }
      else
      {
        o->m_vel = ivec2(o->direction > 0 ? 22 : -22, -12);
        o->set_aistate(ANT_JUMP);
      }
    }
    return 1;
  } else return 0;
}

static int alien_wait_time()
{
  void *v=symbol_value(l_difficulty);
  if (v==l_easy)
    return 6;
  else if (v==l_medium)
   return 4;
  else if (v==l_hard)
    return 2;
  else return 1;
}

static int can_hit_player(GameObject *o, GameObject *b)
{
  return can_see(o, o->m_pos.x + (o->direction > 0 ? 15 : -15), o->m_pos.y - 15, b->m_pos.x, b->m_pos.y - 15);
}

static void fire_at_player(GameObject *o, GameObject *b)
{
  int32_t firex = o->m_pos.x + (o->direction > 0 ? 15 : -15),
          firey = o->m_pos.y - 15,
          playerx = b->m_pos.x + b->m_vel.x * 8,
          playery = b->m_pos.y - 15 + b->m_vel.y * 2;
  if (can_see(o, o->m_pos.x, o->m_pos.y, firex, firey) && can_see(o, firex, firey, playerx, playery))
  {
    int angle=lisp_atan2(firey-playery,playerx-firex);
    void *call_list=NULL;
    PtrRef r1(call_list);
    push_onto_list(LPointer::Create(b),call_list);
    push_onto_list(LNumber::Create(angle),call_list);
    push_onto_list(LNumber::Create(firey),call_list);
    push_onto_list(LNumber::Create(firex),call_list);
    push_onto_list(LNumber::Create(o->aitype()),call_list);
    push_onto_list(LPointer::Create(o),call_list);
    ((LSymbol *)l_fire_object)->EvalUserFunction((LList *)call_list);
    o->set_state((character_state)S_weapon_fire);
  }
}

void *ant_ai()
{
  GameObject *o=current_object,*b;

  if (o->hp()==0)    // if we are dead return NULL and get deleted
  {
    if (o->state==dead)
      return NULL;
    else o->set_state(dead);
    return true_symbol;
  }


  if (o->state==flinch_up || o->state==flinch_down)
  {
    o->next_picture();
    return true_symbol;
  }


  switch (o->aistate())
  {
    case ANT_START :
    {
      o->set_state((character_state)S_hanging);
      if (o->lvars[ANT_hide_flag])
        o->set_aistate(ANT_HIDING);

      else o->set_aistate(ANT_HANGING);
    } break;
    case ANT_HIDING :
    {
      if (rand(128) == 0) the_game->play_sound(S_SCARE_SND, 127, o->m_pos.x, o->m_pos.y);
      if (o->otype!=S_HIDDEN_ANT)
      {
    o->change_type(S_HIDDEN_ANT);      // switch types so noone hurts us.
    o->set_state(stopped);
    o->set_aistate(ANT_HIDING);
      }

      int fall=0;
      if (o->total_objects()==0)
      {
    if (player_list->next)
      b=g_current_level->attacker(current_object);
    else b=player_list->m_focus;
    if (lol::abs(b->m_pos.x - o->m_pos.x) < 130 && (o->m_pos.y < b->m_pos.y))
      fall=1;
      }
      else if (o->get_object(0)->aistate()!=0)
        fall=1;

      if (fall)
      {
    o->change_type(S_ANT_ROOF);
    o->set_state((character_state)S_falling);
    o->set_aistate(ANT_FALL_DOWN);
    o->set_targetable(1);
      } else o->set_targetable(0);
    } break;
    case ANT_HANGING :
    {
      int fall=0;
      if (rand(128) == 0) the_game->play_sound(S_SCARE_SND, 127, o->m_pos.x, o->m_pos.y);
      if (o->lvars[ANT_hide_flag])
        o->set_aistate(ANT_HIDING);
      else
      {
    o->set_state((character_state)S_hanging);
    if (o->total_objects())
    {
      if (o->get_object(0)->aistate()!=0)
      fall=1;
    } else
    {
      if (player_list->next)
        b=g_current_level->attacker(current_object);
      else b=player_list->m_focus;
      if (lol::abs(b->m_pos.x - o->m_pos.x) < 130 && (o->m_pos.y < b->m_pos.y))
      fall=1;
    }
    if (fall)
    {
      o->set_state((character_state)S_fall_start);
      o->set_aistate(ANT_FALL_DOWN);
      o->set_targetable(1);
    } else o->set_targetable(0);
      }

    } break;
    case ANT_FALL_DOWN :
    {
      o->set_state((character_state)S_falling);

      if (player_list->next)
      b=g_current_level->attacker(current_object);
      else b=player_list->m_focus;

      scream_check(o,b);
      int ret=o->mover(0,0,0);
      if ((ret&BLOCKED_DOWN) || !can_see(o, o->m_pos.x, o->m_pos.y, o->m_pos.x, o->m_pos.y + 1))
      {
    o->set_state((character_state)S_landing);
    the_game->play_sound(S_ALAND_SND, 127, o->m_pos.x, o->m_pos.y);
    o->set_aistate(ANT_LANDING);
      }
    } break;
    case ANT_LANDING :
    {
      if (!o->next_picture())
      {
    int32_t xv=0,yv=2;
    o->try_move(o->m_pos.x, o->m_pos.y, xv, yv, 1);
    if (yv!=0)
    {
      o->set_gravity(1);
      o->set_aistate(ANT_FALL_DOWN);
    }
    else
    {
      o->set_state(stopped);
      o->set_aistate(ANT_RUNNING);
      return (void *)ant_ai;
    }
      }
    } break;
    case ANT_RUNNING :
    {
      if (player_list->next)
      b=g_current_level->attacker(current_object);
      else b=player_list->m_focus;
      scream_check(o,b);


      if (rand(16) == 0)
      o->lvars[ANT_need_to_dodge]=1;
      if (!ant_dodge(o))
      {
    if ((o->m_pos.x > b->m_pos.x && o->direction == -1) || (o->m_pos.x < b->m_pos.x && o->direction == 1))
    {
      o->next_picture();
      if (rand(4) == 0 && lol::abs(o->m_pos.x-b->m_pos.x)<180 && lol::abs(o->m_pos.y-b->m_pos.y)<100 && can_hit_player(o,b))
      {
        o->set_state((character_state)S_weapon_fire);
        o->set_aistate(ANT_FIRE);
      } else if (lol::abs(o->m_pos.x-b->m_pos.x)<100 && lol::abs(o->m_pos.y-b->m_pos.y)<10 && rand(4) == 0)
        o->set_aistate(ANT_POUNCE_WAIT);
      else if (lol::abs(o->m_pos.x-b->m_pos.x)>140 && !ant_congestion(o))
        o->set_aistate(ANT_JUMP);
      else
      {
        int32_t xm=o->direction>0 ? get_ability(o->otype,run_top_speed) : -get_ability(o->otype,run_top_speed);
        int32_t ym=0,new_xm=xm;
        if (o->state!=running) o->set_state(running);

        o->try_move(o->m_pos.x,o->m_pos.y,new_xm,ym,3);
        if (new_xm!=xm)    // blocked, see if we can climb ramp
        {
          new_xm=xm;
          ym=-lol::abs(xm);
          o->try_move(o->m_pos.x,o->m_pos.y,new_xm,ym,3);
          if (new_xm==xm)
          {
        o->m_pos.x+=new_xm;
        o->m_pos.y+=ym;
        new_xm=0;
        ym=lol::abs(xm);      // now get back on the ground
        o->try_move(o->m_pos.x,o->m_pos.y,new_xm,ym,3);
        o->m_pos.x+=new_xm;
        o->m_pos.y+=ym;
          } else
          {
        o->direction=0-o->direction;
        o->set_aistate(ANT_JUMP);
          }
        } else
          o->m_pos.x+=new_xm;
        new_xm=0;
        ym=10;       // see if we should fall
        o->try_move(o->m_pos.x,o->m_pos.y,new_xm,ym,3);
        if (ym==10)
          o->set_aistate(ANT_FALL_DOWN);
        else o->m_pos.y+=ym;
      }
    } else
    {
      o->direction=o->m_pos.x>b->m_pos.x ? -1 : 1;
      o->set_aistate(ANT_LANDING);
    }
      }
    } break;

    case ANT_POUNCE_WAIT :
    {
      if (!ant_dodge(o))
      {
    o->set_state((character_state)S_pounce_wait);
    if (o->aistate_time()>alien_wait_time())
    {
      the_game->play_sound(S_ASLASH_SND,127,o->m_pos.x,o->m_pos.y);
      o->set_state(stopped);
      o->set_aistate(ANT_JUMP);
    }
      }
    } break;

    case ANT_JUMP :
    {
      o->lvars[ANT_need_to_dodge]=0;
      if (o->move(o->direction,-1,0)&BLOCKED_DOWN)
      o->set_aistate(ANT_RUNNING);
    } break;
    case ANT_FIRE :
    {
      if (!ant_dodge(o))
      {
    if (o->state==S_fire_wait)
    {
      if (!o->next_picture() || symbol_value(l_difficulty)==l_extreme)
      {
        if (player_list->next)
        b=g_current_level->attacker(current_object);
        else b=player_list->m_focus;
        fire_at_player(o,b);
        o->set_state(stopped);
        o->set_aistate(ANT_RUNNING);
      }
    } else o->set_state((character_state)S_fire_wait);
      }
    } break;
    case ANT_JUMP_ROOF :
    {
      o->lvars[ANT_need_to_dodge]=0;
      o->set_state((character_state)S_jump_up);
//      o->m_vel.y += 1;
      o->m_accel.x = 0;
      int32_t xv = 0, yv = o->m_vel.y;
      o->m_pos.y-=31;
      o->try_move(o->m_pos.x,o->m_pos.y,xv,yv,1);
      o->m_pos.y+=31+yv;
      if (yv != o->m_vel.y)
      {
        if (o->m_vel.y > 0)
        {
          o->set_state(stopped);
          o->set_aistate(ANT_RUNNING);
        }
        else
        {
          o->set_state((character_state)S_top_walk);
          o->set_aistate(ANT_ROOF_WALK);
        }
        o->m_vel.y = 0;
      }
    } break;
    case ANT_ROOF_WALK :
    {
      if (player_list->next)
      b=g_current_level->attacker(current_object);
      else b=player_list->m_focus;
      scream_check(o,b);
      if ((rand(8) == 0 && lol::abs(o->m_pos.x-b->m_pos.x)<10 && o->m_pos.y<b->m_pos.y) ||
      o->lvars[ANT_need_to_dodge]==1)
      {
    o->set_gravity(1);
    o->set_state(run_jump);
    o->set_aistate(ANT_JUMP);
    ant_ai();
      }
      else
      {
    if ((o->m_pos.x>b->m_pos.x && o->direction>0) || (o->m_pos.x<b->m_pos.x && o->direction<0))
    o->direction=-o->direction;
    else if (lol::abs(o->m_pos.x-b->m_pos.x)<120 && rand(4) == 0)
    {
      o->set_state((character_state)S_ceil_fire);
      o->set_aistate(ANT_CEIL_SHOOT);
      ant_ai();
    } else
    {
      int speed=o->direction>0 ? get_ability(o->otype,run_top_speed) :
                -get_ability(o->otype,run_top_speed);
      if (can_see(o,o->m_pos.x,o->m_pos.y-31,o->m_pos.x+speed,o->m_pos.y-31) &&
          !can_see(o,o->m_pos.x+speed,o->m_pos.y-31,o->m_pos.x+speed,o->m_pos.y-32))
      {
        o->m_pos.x+=speed;
        if (!o->next_picture()) o->set_state((character_state)S_top_walk);

      } else o->set_aistate(ANT_FALL_DOWN);
    }
      }
    } break;
    case ANT_CEIL_SHOOT :
    {
      if (!o->next_picture())
      {
    if (player_list->next)
      b=g_current_level->attacker(current_object);
    else b=player_list->m_focus;
    fire_at_player(o,b);
    o->set_state((character_state)S_top_walk);
    o->set_aistate(ANT_ROOF_WALK);
      }
    } break;
  }



  return true_symbol;
}



void fade_in(AImage *im, int steps);
void fade_out(int steps);

void show_stats()
{
  if (g_current_level)
  {
    fade_out(8);
    wm->SetMousePos(ivec2(0, 0));
    main_screen->clear();
    AImage *im=cache.img(cache.reg("art/frame.spe","end_level_screen",SPEC_IMAGE,1));
    main_screen->PutImage(im, ivec2(0, 0));


    int x1=im->Size().x+1,y1=0,x2=xres,y2=main_screen->Size().y;
    fade_in(NULL,16);

    char name[50];
    strcpy(name,g_current_level->original_name());
    char dig1=name[strlen(name)-strlen(".spe")-2];
    char dig2=name[strlen(name)-strlen(".spe")-1];


    char msg[50];

    if (isdigit(dig1) && isdigit(dig2))
    {
      if (dig1!='0')
        sprintf(msg,"%s : %c%c",symbol_str("lev_complete"),dig1,dig2);
      else
        sprintf(msg,"%s : %c",symbol_str("lev_complete"),dig2);
    } else sprintf(msg,"%s : %s",symbol_str("lev_complete"),g_current_level->original_name());

    int w = wm->font()->Size().x * strlen(msg),
        h = wm->font()->Size().y;
    int x=(x1+x2)/2-w/2,y=(y1+y2)/2-h/2;
    main_screen->Bar(ivec2(x - 10, y - 10), ivec2(x + w + 10, y + h + 10),
                     wm->bright_color());
    main_screen->Bar(ivec2(x - 9, y - 9), ivec2(x + w + 9, y + h + 9),
                     wm->medium_color());

    wm->font()->PutString(main_screen, ivec2(x + 1, y + 1), msg, wm->dark_color());
    wm->font()->PutString(main_screen, ivec2(x, y), msg, wm->bright_color());
    wm->flush_screen();
    Timer now; now.Wait(0.5);
  }
}
