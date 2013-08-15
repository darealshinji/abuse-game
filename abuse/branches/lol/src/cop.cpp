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
#include "ant.h"
#include "dev.h"

enum { point_angle, fire_delay1 };

#define SHOTGUN  10
#define GRENADE  2
#define ROCKET   3
#define PLASMA   4
#define FIREBOMB 5
#define DFRIS    6
#define LSABER   7

signed char small_fire_off[24*2]=  // x & y offset from character to end of gun.
  { 17,20,     // 1
    17,23,     // 2
    17,28,
    15,33,
    11,39,     // 5
    7,43,
    -3,44,     // 7
    -10,42,
    -16,39,
    -20,34,
    -20,28,
    -20,25,
    -19,20,
    -19,16,
    -16,14,
    -14,11,
    -11,9,
    -7,8,
    -3,8,
    2,8,
    6,9,
    10,10,
    14,13,
    16,15 };

signed char large_fire_off[24*2]=
  { 18,25,
    17,30,
    15,34,
    14,36,
    10,39,
    7,41,
    4,42,
    -3,41,
    -8,39,
    -11,37,
    -14,33,
    -16,30,
    -18,25,
    -17,21,
    -14,17,
    -11,15,
    -7,13,
    -4,12,
    3,12,
    9,12,
    12,15,
    14,16,
    15,18,
    16,21 };


enum { in_climbing_area,
    disable_top_draw,
    just_hit,
    ship_pan_x,
    special_power,
    used_special_power,
    last1_x, last1_y,
    last2_x, last2_y,
    has_saved_this_level,
    r_ramp, g_ramp, b_ramp,
        is_teleporting,
       just_fired};


enum { sgb_speed,
       sgb_angle,
       sgb_lastx,
       sgb_lasty,
       sgb_bright_color,
       sgb_medium_color,
       sgb_lifetime };

enum { NO_POWER,
       FAST_POWER,
       FLY_POWER,
       SNEAKY_POWER,
       HEALTH_POWER } ;

enum { top_point_angle,
       top_fire_delay1,
       top_just_fired };

inline int angle_diff(int a1, int a2)
{
  if (a1<a2)
  { if ((a2-a1)<180)
      return a2-a1;
    else return 360-a2+a1;
  } else
  {
    if ((a1-a2)<180)
      return a1-a2;
    else return 360-a1+a2;
  }
  return 0;
}

void *top_ai()
{
  GameObject *o=current_object;
  if (o->total_objects())            // make sure we are linked to the main character
  {
    GameObject *q=o->get_object(0);

    view *v = q->m_controller;
    if (v)
    {
      if (!v->freeze_time)
      {
    o->direction=1;                 // always face right

    if (q->direction<0)
          q->m_pos.x += 4;
    int i;
    signed char *fire_off=o->otype==S_DFRIS_TOP ? large_fire_off :
                                    (o->otype==S_ROCKET_TOP ? large_fire_off :
                     (o->otype==S_BFG_TOP ? large_fire_off : small_fire_off));
    signed char *f=fire_off,*fb=NULL;
    int best_diff=200,best_num=0;
    int iy=f[1],ix=f[6*2];

    int best_angle = lisp_atan2(q->m_pos.y - iy - v->pointer_y, v->pointer_x - q->m_pos.x - ix);
    for (i=0; i<24; i++,f+=2)             // check all the angles to see which would best fit animation wise
    {
      int this_angle=lisp_atan2(f[1]-iy,f[0]-ix);
      int this_diff=angle_diff(this_angle,best_angle);
      if (this_diff<best_diff)
      {
        best_diff=this_diff;
        best_num=i;
        fb=f;
      }
    }


    // if the pointer is too close to the player go with the angle shown, not the angle through the pointer
    if (lol::abs(q->m_pos.y - fb[1] - v->pointer_y) < 45
         && lol::abs(v->pointer_x - q->m_pos.x + fb[0]) < 40)
      o->lvars[point_angle]=lisp_atan2(fb[1]-iy,fb[0]-ix);
    else
      o->lvars[point_angle] = lisp_atan2(q->m_pos.y - fb[1] - v->pointer_y,
                                         v->pointer_x - (q->m_pos.x + fb[0]));


    if (q->direction<0)
          q->m_pos.x -=4 ;

    o->m_pos = q->m_pos + ivec2(0, 29 - q->picture()->Size().y);

    o->current_frame=best_num;


    if (o->lvars[fire_delay1])
      o->lvars[fire_delay1]--;

    o->otype=weapon_types[v->current_weapon];  // switch to correct top part
      }
    }
  }
  return true_symbol;
}


static int player_fire_weapon(GameObject *o, int type, GameObject *target, int angle, signed char *fire_off)
{

  if (!o->total_objects()) return 0;
  GameObject *other=o->get_object(0);
  ivec2 otherpos = other->m_pos;
  if (other->direction < 0)
    other->m_pos.x += 4;

  int firex=other->m_pos.x + fire_off[o->current_frame * 2];
  int firey=other->m_pos.y - fire_off[o->current_frame * 2 + 1];

  // fire try to move up to gun level

  int32_t x2 = o->m_pos.x, y2 = firey;
//  g_current_level->foreground_intersect(other->m_pos.x, other->m_pos.y, x2, y2);      // find first location we can actuall "see"
//  g_current_level->all_boundary_setback(o, other->m_pos.x, other->m_pos.y, x2, y2);       // to make we don't fire through walls
  other->m_pos.y = y2;

  if (other->m_pos.y == firey)             // now try to move out to end of gun if we were not blocked above
  {
    x2=firex;
    g_current_level->foreground_intersect(other->m_pos.x, other->m_pos.y, x2, y2);      // find first location we can actuall "see"
    g_current_level->all_boundary_setback(other, other->m_pos.x, other->m_pos.y, x2, y2);       // to make we don't fire through walls
    o->m_pos.x = x2;
  }

  void *list=NULL;
  PtrRef r1(list);
  push_onto_list(LPointer::Create(target),list);
  push_onto_list(LNumber::Create(angle),list);
  push_onto_list(LNumber::Create(y2),list);
  push_onto_list(LNumber::Create(x2),list);
  push_onto_list(LNumber::Create(type),list);
  push_onto_list(LPointer::Create(o->get_object(0)),list);
  ((LSymbol *)l_fire_object)->EvalFunction(list);
  o->lvars[top_just_fired]=1;
  other->lvars[just_fired]=1;
  other->m_pos = otherpos;

  return 1;
}



void *laser_ufun(void *args)
{
  GameObject *o=current_object;
  PtrRef r1(args);
  void *signal=CAR(args);  args=CDR(args);
  void *ret=NULL;

  if (signal==l_FIRE)
  {
    if (!o->lvars[fire_delay1])                   // make sur we are not waiting of previous fire
    {
      int32_t value=lnumber_value(CAR(args)->Eval());
      if (value)                                   // do we have ammo ?
      {
    o->lvars[fire_delay1]=3;
    if (player_fire_weapon(o,SHOTGUN,NULL,o->lvars[point_angle],small_fire_off))
          ret=LNumber::Create(-1);
    else ret=LNumber::Create(0);
      } else
      {
    o->lvars[fire_delay1]=5;                  // no ammo, set large fire delay for next shot
    player_fire_weapon(o,SHOTGUN,NULL,o->lvars[point_angle],small_fire_off);
    ret=LNumber::Create(0);
      }
    } else ret=LNumber::Create(0);                // can't fire yet, return 0 ammo subtract
  }
  return ret;
}


static int ammo_type(int otype)
{
  if (otype==S_GRENADE_TOP)
    return GRENADE;
  else if (otype==S_FIREBOMB_TOP)
    return FIREBOMB;
  else if (otype==S_DFRIS_TOP)
    return DFRIS;
  else return SHOTGUN;
}


void *top_ufun(void *args)                       // generic top character ai GRENADE && FIREBOMB
{
  GameObject *o=current_object;
  PtrRef r1(args);
  void *signal=CAR(args);  args=CDR(args);
  void *ret=NULL;

  if (signal==l_FIRE)
  {
    if (!o->lvars[fire_delay1])                   // make sur we are not waiting of previous fire
    {
      int32_t value=lnumber_value(CAR(args)->Eval());
      if (value)                                   // do we have ammo ?
      {
    o->lvars[fire_delay1]=6;
    if (player_fire_weapon(o,ammo_type(o->otype),NULL,o->lvars[point_angle],
                   o->otype==DFRIS ? large_fire_off : small_fire_off ))
          ret=LNumber::Create(-1);
    else ret=LNumber::Create(0);
      } else ret=LNumber::Create(0);
    } else ret=LNumber::Create(0);                // can't fire yet, return 0 ammo subtract
  }
  return ret;
}

static int climb_handler(GameObject *, int xm, int ym, int but);

void *plaser_ufun(void *args)
{
  GameObject *o=current_object;
  PtrRef r1(args);
  void *signal=CAR(args);  args=CDR(args);
  void *ret=NULL;

  if (signal==l_FIRE)
  {
    if (!o->lvars[fire_delay1])                   // make sur we are not waiting of previous fire
    {
      int32_t value=lnumber_value(CAR(args)->Eval());
      if (value)                                   // do we have ammo ?
      {
    o->lvars[fire_delay1]=2;
    if (player_fire_weapon(o,PLASMA,NULL,o->lvars[point_angle],small_fire_off))
          ret=LNumber::Create(-1);
    else ret=LNumber::Create(0);
      } else ret=LNumber::Create(0);
    } else ret=LNumber::Create(0);                // can't fire yet, return 0 ammo subtract
  }
  return ret;
}

void *lsaber_ufun(void *args)
{
  GameObject *o=current_object;
  PtrRef r1(args);
  void *signal=CAR(args);  args=CDR(args);
  void *ret=NULL;

  if (signal==l_FIRE)
  {
    if (!o->lvars[fire_delay1])                   // make sur we are not waiting of previous fire
    {
      int32_t value=lnumber_value(CAR(args)->Eval());
      if (value)                                   // do we have ammo ?
      {
    o->lvars[fire_delay1]=1;
    if (player_fire_weapon(o,LSABER,NULL,o->lvars[point_angle]+(g_current_level->tick_counter()&7)-8,
                   small_fire_off))
          ret=LNumber::Create(-1);
    else ret=LNumber::Create(0);
      } else ret=LNumber::Create(0);
    } else ret=LNumber::Create(0);                // can't fire yet, return 0 ammo subtract
  }
  return ret;
}



void *player_rocket_ufun(void *args)
{
  GameObject *o=current_object;
  PtrRef r1(args);
  void *signal=CAR(args);  args=CDR(args);
  void *ret=NULL;
  int xd,yd,cl=0xfffffff,d;
  if (signal==l_FIRE)
  {
    if (!o->lvars[fire_delay1])                   // make sur we are not waiting of previous fire
    {
      int32_t value=lnumber_value(CAR(args)->Eval());
      if (value)                                   // do we have ammo ?
      {
    o->lvars[fire_delay1]=6;
    GameObject *target=NULL,*p,*bot=o->total_objects() ? o->get_object(0) : 0;
    if (bad_guy_array)
    {
      GameObject *other=current_object->total_objects() ? current_object->get_object(0) : 0;
      for (p=g_current_level->first_active_object(); p; p=p->next_active)
      {
        xd = lol::abs(p->m_pos.x - o->m_pos.x);
        yd = lol::abs(p->m_pos.y - o->m_pos.y);
        if (xd<160 && yd<130 && bad_guy_array[p->otype] && p!=other)
        {
          if (p->targetable() &&
          !(p->otype==S_ROCKET && p->total_objects() && p->get_object(0)==bot))  // don't track onto own missles
          {
        d=xd*xd+yd*yd;
        if (d<cl)
        {
          cl=d;
          target=p;
        }
          }
        }
      }
    }
    if (player_fire_weapon(o,ROCKET,target,o->lvars[point_angle],large_fire_off))
          ret=LNumber::Create(-1);
    else ret=LNumber::Create(0);

      } else ret=LNumber::Create(0);
    } else ret=LNumber::Create(0);                // can't fire yet, return 0 ammo subtract
  }
  return ret;
}

static int player_move(GameObject *o, int xm, int ym, int but)
{
  if (!o->lvars[in_climbing_area])
  {
    if (o->state==S_climbing)
    {
      o->set_gravity(1);
      o->set_state(run_jump_fall);
    }
    o->next_picture();
    return o->mover(xm,ym,but);
  } else return climb_handler(o,xm,ym,but);

}


static void undo_special_power(GameObject *o)
{
  switch (o->lvars[special_power])
  {
    case SNEAKY_POWER :
    {
      if (o->lvars[used_special_power]>0)
        o->lvars[used_special_power]--;
    } break;
    case FAST_POWER :
    {
      o->lvars[used_special_power]=0;
    } break;
  }
}

static void do_special_power(GameObject *o, int xm, int ym, int but, GameObject *top)
{
  switch (o->lvars[special_power])
  {
    case FLY_POWER :
    {
      GameObject *cloud=create(S_CLOUD, o->m_pos.x + o->direction * -10, o->m_pos.y + rand(5));
      if (g_current_level)
        g_current_level->add_object(cloud);
      o->set_state(run_jump);
      o->set_gravity(1);
      o->m_accel.y = 0;
      if (o->m_vel.y > 0)
        o->m_vel.y /= 2;
      if (ym < 0)
        o->m_vel.y -= 3;
      else
        o->m_vel.y -= 2;
      the_game->play_sound(S_FLY_SND, 32, o->m_pos.x, o->m_pos.y);
    } break;
    case FAST_POWER :
    {
      if ((g_current_level->tick_counter()%16)==0)
      the_game->play_sound(S_SPEED_SND, 100, o->m_pos.x, o->m_pos.y);

      o->lvars[used_special_power]=1;
      o->lvars[last1_x] = o->m_pos.x;
      o->lvars[last1_y] = o->m_pos.y;
      int32_t oyvel = o->m_vel.y;
      int in=o->lvars[in_climbing_area];

      player_move(o,xm,ym,but);
      if (ym < 0 && !oyvel && o->m_vel.y < 0) // if they just jumped, make them go higher
          o->m_vel.y = o->m_vel.y * 4 / 3;
      o->lvars[in_climbing_area]=in;

      o->lvars[last2_x] = o->m_pos.x;
      o->lvars[last2_y] = o->m_pos.y;
    } break;
    case SNEAKY_POWER:
    {
      if (o->lvars[used_special_power]<15)
        o->lvars[used_special_power]++;
    } break;
  }
}

static int climb_off_handler(GameObject *o)
{
  if (o->next_picture())
    o->m_controller->pan_y -= 4;
  else
  {
    o->m_pos.y -= 28;
    o->m_controller->pan_y += 28;
    o->m_controller->m_lastpos.y -= 28;
    o->set_state(stopped);
  }
  return 0;
}


static int climb_on_handler(GameObject *o)
{
  if (o->next_picture())
    o->m_controller->pan_y += 4;
  else
    o->set_state((character_state)S_climbing);
  return 0;
}

static int climb_handler(GameObject *o, int xm, int ym, int but)
{
  int yd=o->lvars[in_climbing_area];  // see how from the top we are
  o->lvars[in_climbing_area]=0;          // set 0, ladders will set back to proper if still in area
  if (o->state==S_climb_off)
    climb_off_handler(o);
  else if (o->state==S_climb_on)
    climb_on_handler(o);
  else
  {
    if (o->state==S_climbing)
    {
      if (ym>0)                       // going down
      {

    if (o->current_frame==0) o->current_frame=9;
      o->current_frame--;

/*    if (o->lvars[special_power]==FAST_POWER)
    {
      int32_t xv=0,yv=4;
      o->try_move(o->m_pos.x, o->m_pos.y, xv, yv, 1);
      if (yv==4)
        o->m_pos.y += 3;
      else
      {
        o->set_gravity(1);
          o->set_state(run_jump_fall);
      }
    }
    else */ o->m_pos.y += 3;


      } else if (ym<0)
      {
    if (yd<32)
      o->set_state((character_state)S_climb_off);
    else
    {
      if (!o->next_picture()) o->set_state((character_state)S_climbing);
      o->m_pos.y -= 3;
    }
      }
      if (xm)                     // trying to get off the ladder, check to see if that's ok
      {
    int32_t x2=0,y2=-20;
    o->try_move(o->m_pos.x, o->m_pos.y, x2, y2, 3);
    if (y2==-20)
    {
      o->set_gravity(1);
      if (ym>=0)
        o->set_state(run_jump_fall);
      else
      {
        o->set_state(run_jump);
        o->m_vel.y = get_ability(o->otype, jump_yvel);
      }
    }
      }
    }  else if (ym>0 && yd<10)
    {
      o->m_pos.y += 28;
      o->m_controller->pan_y-=28;
      o->m_controller->m_lastpos.y += 28;
      o->set_state((character_state)S_climb_on);
    }
    else if (o->m_vel.y >= 0 && (ym > 0 || (ym < 0 && yd > 8)))
    {
      o->set_state((character_state)S_climbing);
      o->set_gravity(0);
      o->m_vel = ivec2(0);
      o->m_accel = ivec2(0);
      return 0;
    } else
    {
      o->next_picture();
      return o->mover(xm,ym,but);
    }
  }
  return 0;
}


void *cop_mover(int xm, int ym, int but)
{

  int ret=0;
  GameObject *o=current_object,*top;
  if (o->m_controller && o->m_controller->freeze_time)
  {
    o->m_controller->freeze_time--;
    if (but || o->m_controller->key_down(JK_SPACE) || o->m_controller->key_down(JK_ENTER))
      o->m_controller->freeze_time = 0;
  }
  else
  {
    if (!o->total_objects())                  // if no top create one
    {
      top=create(S_MGUN_TOP, o->m_pos.x, o->m_pos.y, 0, 0);
      g_current_level->add_object_after(top,o);
      o->add_object(top);
      top->add_object(o);
    } else top=o->get_object(0);

    if (o->m_vel.y > 10)
    {
      o->m_accel.y = 0;
      o->m_vel.y -= 1; // terminal velocity
    }

    if (o->aistate()==0)  // just started, wait for button
    {
      o->set_aistate(1);
    } else if (o->aistate()==1)         // normal play
    {
      if (o->hp()==0)
      {
    o->set_aistate(2);                // go to deing state
    o->set_state(dead);
      }
      else
      {
    if (o->hp()<40 && (g_current_level->tick_counter()%16)==0) // if low on health play heart beat
      the_game->play_sound(S_LOW_HEALTH_SND, 127, o->m_pos.x, o->m_pos.y);
    else if (o->hp()<15 && (g_current_level->tick_counter()%8)==0) // if low on health play heart beat
      the_game->play_sound(S_LOW_HEALTH_SND, 127, o->m_pos.x, o->m_pos.y);
    else if (o->hp()<7 && (g_current_level->tick_counter()%4)==0) // if low on health play heart beat
      the_game->play_sound(S_LOW_HEALTH_SND, 127, o->m_pos.x, o->m_pos.y);

    if (but&1)
        do_special_power(o,xm,ym,but,top);
    else
    undo_special_power(o);
    ret=player_move(o,xm,ym,but);
    top->m_pos = o->m_pos + ivec2(0, 29 - top->picture()->Size().y);

    if ((but&2) && !o->lvars[is_teleporting] && o->state!=S_climbing && o->state!=S_climb_off)
    {
      void *args=NULL;
      PtrRef r1(args);
      view *v = o->m_controller;

      push_onto_list(LNumber::Create(v->weapon_total(v->current_weapon)),args);
      push_onto_list(l_FIRE,args);

      current_object=top;
      void *ret = ((LSymbol *)figures[top->otype]->get_fun(OFUN_USER_FUN))->EvalFunction(args);
      current_object=o;
      v->add_ammo(v->current_weapon,lnumber_value(ret));
    }
      }
    } else if (o->aistate()==3)
    {
      if (!o->m_controller || o->m_controller->key_down(JK_SPACE))
      {
        // call the user function to reset the player
    ((LSymbol *)l_restart_player)->EvalFunction(NULL);
    o->m_controller->reset_player();
    o->set_aistate(0);
      } else if (o->m_controller && o->m_controller->local_player())
        the_game->show_help(symbol_str("space_cont"));

    } else o->set_aistate(o->aistate()+1);
  }

  return LNumber::Create(ret);
}



void *ladder_ai()
{
  view *f=player_list;
  GameObject *o=current_object;
  if (o->total_objects())
  {
    GameObject *other=o->get_object(0);
    for (; f; f=f->next)
    {
      int mex = f->m_focus->m_pos.x;
      int mey = f->m_focus->m_pos.y;

      if (o->m_pos.x <= mex && o->m_pos.y <= mey
           && other->m_pos.x >= mex && other->m_pos.y >= mey)
      {
    if (f->m_focus->state==S_climbing)
      f->m_focus->m_pos.x = (o->m_pos.x + other->m_pos.x) / 2;
        f->m_focus->lvars[in_climbing_area] = mey - o->m_pos.y;
      }
    }
  }
  return true_symbol;
}



void *player_draw(int just_fired_var, int num)
{
  GameObject *o=current_object;
  if (num==0)
  {
    if (o->lvars[just_fired_var])
    {
      o->draw_tint(S_bright_tint);
      o->lvars[just_fired_var]=0;
    } else
      o->drawer();
  }
  else
  {
    if (o->lvars[just_fired_var])
    {
      o->draw_double_tint(lnumber_value(((LArray *)((LSymbol *)l_player_tints)->GetValue())->Get(num)), S_bright_tint);
      o->lvars[just_fired_var]=0;
    } else
      o->draw_tint(lnumber_value(((LArray *)((LSymbol *)l_player_tints)->GetValue())->Get(num)));
  }
  return NULL;
}


void *top_draw()
{
  GameObject *o=current_object;
  if (o->total_objects())
  {
    GameObject *bot=o->get_object(0);
    if (bot->state==stopped  || bot->state==running ||
    bot->state==run_jump || bot->state==run_jump_fall ||
    bot->state==end_run_jump)
    {
      int oldy = o->m_pos.y;
      o->m_pos.x = bot->m_pos.x;
      if (bot->direction<0)
        o->m_pos.x += 4;
      o->m_pos.y = bot->m_pos.y + 29 - bot->picture()->Size().y;

      void *ret=NULL;
      PtrRef r1(ret);

      push_onto_list(LNumber::Create(bot->get_tint()),ret);

      if (bot->lvars[special_power]==SNEAKY_POWER)
      {
    if (bot->lvars[used_special_power]==0)
      player_draw(top_just_fired,bot->get_tint());
    else if (bot->lvars[used_special_power]<15)
      o->draw_trans(bot->lvars[used_special_power],16);
    else
      o->draw_predator();
      } else
        ((LSymbol *)l_player_draw)->EvalFunction(ret);

      o->m_pos.y = oldy;
      if (bot->direction < 0)
        o->m_pos.x -= 4;
    }
  }
  return NULL;
}



void *bottom_draw()
{
  GameObject *o=current_object;

  if (o->lvars[r_ramp] || o->lvars[g_ramp] || o->lvars[b_ramp])
  {
    o->lvars[r_ramp] = lol::max(o->lvars[r_ramp] - 7, 0);
    o->lvars[g_ramp] = lol::max(o->lvars[g_ramp] - 7, 0);
    o->lvars[b_ramp] = lol::max(o->lvars[b_ramp] - 7, 0);
    ivec3 delta(o->lvars[r_ramp], o->lvars[g_ramp], o->lvars[b_ramp]);

    Palette *p = g_palette->Copy();
    for (int i = 0; i < 256; i++)
      p->SetColor(i, (u8vec3)clamp((ivec3)p->GetColor(i) + delta, 0, 255));
    p->Load();
    delete p;
  }

  if (o->aistate()>0)
  {
    switch (o->lvars[special_power])
    {
      case NO_POWER :
      { player_draw(just_fired,o->get_tint()); } break;

      case HEALTH_POWER :
      {
    player_draw(just_fired,o->get_tint());
    if (o->m_controller && o->m_controller->local_player())
      main_screen->PutImage(cache.img(S_health_image),
                            ivec2(o->m_controller->m_bb.x - 20,
                                  o->m_controller->m_aa.y + 5), 1);
      } break;
      case FAST_POWER :
      {
    ((LSymbol *)l_draw_fast)->EvalFunction(NULL);
    int old_state=o->state;
    switch (o->state)
    {
      case stopped : o->state=(character_state)S_fast_stopped; break;
      case running : o->state=(character_state)S_fast_running; break;
      case start_run_jump : o->state=(character_state)S_fast_start_run_jump; break;
      case run_jump : o->state=(character_state)S_fast_run_jump; break;
      case run_jump_fall : o->state=(character_state)S_fast_run_jump_fall; break;
      case end_run_jump : o->state=(character_state)S_fast_end_run_jump; break;
      default: break;
    }

    player_draw(just_fired,o->get_tint());
    o->state=(character_state)old_state;
    if (o->m_controller && o->m_controller->local_player())
      main_screen->PutImage(cache.img(S_fast_image),
                            ivec2(o->m_controller->m_bb.x - 20,
                                  o->m_controller->m_aa.y + 5), 1);
      } break;
      case FLY_POWER :
      {
    int old_state=o->state;
    switch (o->state)
    {
      case stopped : o->state=(character_state)S_fly_stopped; break;
      case running : o->state=(character_state)S_fly_running; break;
      case start_run_jump : o->state=(character_state)S_fly_start_run_jump; break;
      case run_jump : o->state=(character_state)S_fly_run_jump; break;
      case run_jump_fall : o->state=(character_state)S_fly_run_jump_fall; break;
      case end_run_jump : o->state=(character_state)S_fly_end_run_jump; break;
      default: break;
    }

    player_draw(just_fired,o->get_tint());
    o->state=(character_state)old_state;

    if (o->m_controller && o->m_controller->local_player())
      main_screen->PutImage(cache.img(S_fly_image),
                            ivec2(o->m_controller->m_bb.x - 20,
                                  o->m_controller->m_aa.y + 5), 1);
      } break;
      case SNEAKY_POWER :
      {
    if (o->lvars[used_special_power]==0)
      player_draw(just_fired,o->get_tint());
    else if (o->lvars[used_special_power]<15)
      o->draw_trans(o->lvars[used_special_power],16);
    else
      o->draw_predator();

    if (o->m_controller && o->m_controller->local_player())
      main_screen->PutImage(cache.img(S_sneaky_image),
                            ivec2(o->m_controller->m_bb.x - 20,
                                  o->m_controller->m_aa.y + 5), 1);
      } break;
    }
  }
  return NULL;
}



void *sgun_ai()
{
  GameObject *o=current_object;

  if (o->lvars[sgb_lifetime]==0)
    return NULL;
  o->lvars[sgb_lifetime]--;

  o->lvars[sgb_lastx] = o->m_pos.x;
  o->lvars[sgb_lasty] = o->m_pos.y;
  o->lvars[sgb_speed] = o->lvars[sgb_speed] * 6 / 5;

  int32_t ang=o->lvars[sgb_angle];
  int32_t mag=o->lvars[sgb_speed];

  int32_t xvel = lisp_cos(ang) * mag;
  int32_t yvel = -lisp_sin(ang) * mag;
  current_object->m_vel = ivec2(xvel >> 16, yvel >> 16);
  current_object->set_fxvel((xvel&0xffff)>>8);
  current_object->set_fyvel((yvel&0xffff)>>8);


  int whit=0;
  GameObject *who=o->bmove(whit, o->total_objects() ? o->get_object(0) : 0);

  if (whit || (who && figures[who->otype]->get_cflag(CFLAG_UNACTIVE_SHIELD) && who->total_objects() &&
           who->get_object(0)->aistate()==0))
  {
    o->lvars[sgb_lifetime]=0;
    GameObject *n=create(S_EXPLODE5, o->m_pos.x + rand(4), o->m_pos.y + rand(4));
    g_current_level->add_object(n);
  } else if (who && figures[who->otype]->get_cflag(CFLAG_HURTABLE))
  {
    o->lvars[sgb_lifetime]=0;
    GameObject *n=create(S_EXPLODE3, o->m_pos.x + rand(4), o->m_pos.y + rand(4));
    g_current_level->add_object(n);
     who->do_damage(5, o, o->m_pos.x, o->m_pos.y, (lisp_cos(ang) * 10) >> 16, (lisp_sin(ang) * 10) >> 16);
  }
  return true_symbol;
}



void *mover_ai()
{
  GameObject *o=current_object;
  if (o->total_objects()==2)
  {
    if (o->aistate()<2)
    {
      GameObject *obj=o->get_object(1);
      o->remove_object(obj);
      GameObject *d=o->get_object(0);
      d->add_object(obj);
      d->set_aistate(d->aitype());
    } else
    {
      o->set_aistate(o->aistate()-1);
      GameObject *d=o->get_object(0);
      GameObject *obj=o->get_object(1);

      obj->m_pos = d->m_pos - (d->m_pos - o->m_pos) * (int)o->aistate() / (int)o->aitype();
    }
  }
  return true_symbol;
}


void *respawn_ai()
{
 GameObject *o=current_object;
 int x=o->total_objects();
 if (x)
 {
   GameObject *last=o->get_object(x-1);
   if (last->m_pos == o->m_pos)
   {
     if (last->fade_count())
       last->set_fade_count(last->fade_count()-1);
     o->set_aistate_time(0);
   }
   else if (o->aistate_time() > o->m_vel.x)
   {
     int type=o->get_object(rand(x))->otype;
     GameObject *n=create(type, o->m_pos.x, o->m_pos.y);
     g_current_level->add_object(n);
     o->add_object(n);
     n->set_fade_count(15);
     o->set_aistate_time(0);
   }
 }
 return true_symbol;
}

static int compare_players(const void *a, const void *b)
{
  if  ( ((view **)a)[0]->kills > ((view **)b)[0]->kills)
    return -1;
  else if  ( ((view **)a)[0]->kills < ((view **)b)[0]->kills)
    return 1;
  else if (((view **)a)[0]->player_number > ((view **)b)[0]->player_number)
    return -1;
  else if (((view **)a)[0]->player_number < ((view **)b)[0]->player_number)
    return 1;
  else return 0;
}

void *score_draw()
{
  view *sorted_players[16],*local=NULL;
  int tp=0;
  view *f=player_list;
  for (; f; f=f->next)
  {
    sorted_players[tp]=f;
    tp++;
    if (f->local_player()) local=f;
  }

  JCFont *fnt=wm->font();
  if (local)
  {
    qsort(sorted_players,tp,sizeof(view *),compare_players);

    ivec2 pos = local->m_aa;
    char msg[100];

    int i;
    for (i=0; i<tp; i++)
    {
      int color=lnumber_value(((LArray *)((LSymbol *)l_player_text_color)->GetValue())->Get(sorted_players[i]->get_tint()));
      sprintf(msg,"%3ld %s",(long)sorted_players[i]->kills,sorted_players[i]->name);
      if (sorted_players[i]==local)
        strcat(msg," <<");

      fnt->PutString(main_screen, pos, msg, color);
      pos.y += fnt->Size().y;
    }
  }
  return NULL;
}


extern void fade_in(image *im, int steps);
extern void fade_out(int steps);

void *show_kills()
{
  fade_out(8);
  wm->SetMousePos(ivec2(0, 0));
  main_screen->clear();
  image *im=cache.img(cache.reg("art/frame.spe","end_level_screen",SPEC_IMAGE,1));
  main_screen->PutImage(im, ivec2(0, 0));
  int x1=im->Size().x+1,y1=0,y2=main_screen->Size().y;
  JCFont *fnt=wm->font();

  view *v=player_list; int tp=0,i;
  for (v=player_list; v; v=v->next) tp++;

  int y=(y1+y2)/2-(tp+2)*fnt->Size().y/2,x=x1+10;
  char const *header_str = symbol_str("score_header");
  fnt->PutString(main_screen, ivec2(x, y), header_str, wm->bright_color());
  y += fnt->Size().y;

  main_screen->WidgetBar(ivec2(x, y + 2),
                         ivec2(x + strlen(header_str) * fnt->Size().x,
                               y + fnt->Size().y - 3),
                         wm->bright_color(), wm->medium_color(),
                         wm->dark_color());
  y += fnt->Size().y;
  v = player_list;
  for (i=0; i<tp; i++)
  {
    enum { NAME_LEN=18 } ;
    int color=lnumber_value(((LArray *)((LSymbol *)l_player_text_color)->GetValue())->Get(v->get_tint()));
    char max_name[NAME_LEN];
    strncpy(max_name,v->name,NAME_LEN-1);
    max_name[NAME_LEN-1]=0;
    char msg[100];


    sprintf(msg,"%-17s %3ld  %3ld",max_name,(long)v->kills,(long)(v->tkills+v->kills));
    fnt->PutString(main_screen, ivec2(x, y), msg, color);

    y += fnt->Size().y;
    v = v->next;
  }

  wm->flush_screen();
  Timer now; now.Wait(4.f);   // wait 4 seconds

  return NULL;
}


