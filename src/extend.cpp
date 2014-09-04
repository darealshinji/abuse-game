/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#if HAVE_CONFIG_H
#   include "config.h"
#endif

#include "common.h"

#include "lisp/lisp.h"

#include "extend.h"
#include "view.h"
#include "objects.h"

/*



  Simple object             (power ups, non-moving objects)
    int32_t x,y;
    int8_t direction;
    uint16_t otype,state
    uint16_t current_frame;
    extension *


  Moving object             (simple lisp controlled characters)
     uint8_t flags;
     int32_t xvel,yvel,xacel,yacel;
     uint8_t fx,fy,fxvel,fyvel,fxacel,fyacel,aitype;
     uint16_t aistate,aistate_time;
     uint16_t hp,mp,
     extension *


  Complex objects          (can controll lights, other characters, and have a neural net ai)
    uint8_t tobjs,tlights;
    object_list *
    light_list *
    nnet_info *
    int8_t fade_dir, frame_dir;
    uint8_t fade_count,fade_max;
    morph_char *morph_status;


*/

void SimpleObject::add_light(LightSource *ls)
{
  if (!ls) return ;
  ls->known=1;
  for (int i=0; i<tlights; i++) if (lights[i]==ls) return;
  tlights++;
  lights=(LightSource **)realloc(lights,sizeof(LightSource *)*tlights);
  lights[tlights-1]=ls;
}

void SimpleObject::add_object(GameObject *o)
{
  if (!o) return ;
  for (int i=0; i<tobjs; i++) if (objs[i]==o) return;
  o->set_flags(o->flags()|KNOWN_FLAG);
  if(_team != -1)
    o->set_team(_team);
  if(_tint != -1)
    o->set_tint(_tint);
  tobjs++;
  objs=(GameObject **)realloc(objs,sizeof(GameObject *)*tobjs);
  objs[tobjs-1]=o;
}


void SimpleObject::remove_light(LightSource *ls)
{
  for (int i=0; i<tlights; i++)
  {
    if (lights[i]==ls)
    {
      tlights--;
      for (int j=i; j<tlights; j++)     // don't even think about it :)
        lights[j]=lights[j+1];
      lights=(LightSource **)realloc(lights,sizeof(LightSource *)*tlights);
      return ;
    }
  }
}

void SimpleObject::remove_object(GameObject *o)
{
  for (int i=0; i<tobjs; i++)
  {
    if (objs[i]==o)
    {
      tobjs--;
      for (int j=i; j<tobjs; j++)     // don't even think about it :)
        objs[j]=objs[j+1];
      objs=(GameObject **)realloc(objs,sizeof(GameObject *)*tobjs);
      return ;
    }
  }
}


SimpleObject::SimpleObject()
  : m_pos(0),
    m_last_pos(0),
    m_vel(0),
    m_accel(0),
    m_controller(nullptr)
{
  direction=1;
  otype=0;
  state=stopped;
  current_frame=0;

  Fade_dir=0;
  Fade_count=0;
  Fade_max=16;


  tobjs=tlights=0;
  objs=NULL;
  lights=NULL;
  Frame_dir=1;
  mc=NULL;

  Flags=0;
  Fx=Fy=Fxvel=Fyvel=Fxacel=Fyacel=Aitype=0;
  Aistate=Aistate_time=0;
  Hp=Mp=Fmp=0;
  _tint = -1;
  _team = -1;
  grav_on=1;
  targetable_on=1;
}



void SimpleObject::set_morph_status(morph_char *Mc)
{
  mc=Mc;
}

void SimpleObject::clean_up()
{
  if (tlights) free(lights);
  if (tobjs)   free(objs);
  if (m_controller)
    m_controller->m_focus = nullptr;
}


SimpleObject g_default_simple;


