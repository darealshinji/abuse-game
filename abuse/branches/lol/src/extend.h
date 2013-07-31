/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef EXTEND_HPP_
#define EXTEND_HPP_

#define NNET_HISTSIZE 10
#define FLAG_JUST_HIT     1
#define FLAG_JUST_BLOCKED 2
#define FLOATING_FLAG     4
#define KNOWN_FLAG        8


#include "lisp/lisp.h"

#include "morpher.h"
#include "chars.h"


class View;
class GameObject;
class LightSource;

class SimpleObject
{
public:
    SimpleObject();

protected:
    int8_t Fade_dir;
    uint8_t Fade_count,Fade_max;
    uint8_t Flags,grav_on,targetable_on;
    uint8_t Fx,Fy,Fxvel,Fyvel,Fxacel,Fyacel;
    uint8_t Aitype;
    uint16_t Aistate,Aistate_time;
    uint16_t Hp,Mp,Fmp;
    int8_t Frame_dir;
    int _tint, _team;

    uint8_t tobjs,tlights;
    GameObject **objs,*link;
    LightSource **lights;

    morph_char *mc;

public:
    // leave these public, so I don't have monster code changes.
    ivec2 m_pos;
    ivec2 m_last_pos; // used for frame interpolation on fast machines
    ivec2 m_vel, m_accel;

    view *m_controller;

    int total_vars();
    char const *var_name(int x);
    int var_type(int x);
    void set_var(int x, uint32_t v);
    int32_t get_var(int x);

    int8_t direction,active;
    uint16_t otype;
    character_state state;
    short current_frame;

    int targetable()           { return targetable_on; }
    int gravity()              { return grav_on; }
    int floating()             { return flags()&FLOATING_FLAG; }

    int keep_ai_info()         { return 1; }
    uint8_t flags()            { return Flags; }

    uint8_t fx()               { return Fx; }
    uint8_t fy()               { return Fy; }
    uint8_t fxvel()            { return Fxvel; }
    uint8_t fyvel()            { return Fyvel; }
    uint8_t fxacel()           { return Fxacel; }
    uint8_t fyacel()           { return Fyacel; }

    uint8_t sfx()              { return Fx; }  // x & y should always be positive
    uint8_t sfy()              { return Fy; }
    uint8_t sfxvel()           { if (m_vel.x>=0) return Fxvel; else return -Fxvel; }
    uint8_t sfyvel()           { if (m_vel.y>=0) return Fyvel; else return -Fyvel; }
    uint8_t sfxacel()          { if (m_accel.x>=0) return Fxacel; else return -Fxacel; }
    uint8_t sfyacel()          { if (m_accel.y>=0) return Fyacel; else return -Fyacel; }

    uint8_t aitype()           { return Aitype; }
    uint16_t aistate()         { return Aistate; }
    uint16_t aistate_time()    { return Aistate_time; }
    uint16_t hp()              { return Hp;         }
    uint16_t mp()              { return Mp;         }
    uint16_t fmp()             { return Fmp;        }
    int8_t fade_dir()          { return Fade_dir;   }
    int8_t frame_dir()         { return Frame_dir;  }
    uint8_t fade_count()       { return Fade_count; }
    uint8_t fade_max()         { return Fade_max;   }
    uint8_t total_objects()    { return tobjs;      }
    uint8_t total_lights()     { return tlights;    }

    morph_char *morph_status()     { return mc; }
    LightSource *get_light(int x)
    { if (x>=tlights) { lbreak("bad x for light\n"); exit(0); } return lights[x]; }
    GameObject *get_object(int x)
    { if (x>=tobjs) { lbreak("bad x for object\n"); exit(0); } return objs[x]; }

    void set_targetable(uint8_t x)  { targetable_on=x; }
    void set_flags(uint8_t f)       { Flags=f; }
    void set_fx(uint8_t x)          { Fx=x; }
    void set_fy(uint8_t y)          { Fy=y; }
    void set_fxvel(uint8_t xv)      { Fxvel=abs(xv); }
    void set_fyvel(uint8_t yv)      { Fyvel=abs(yv); }
    void set_fxacel(uint8_t xa)     { Fxacel=abs(xa); }
    void set_fyacel(uint8_t ya)     { Fyacel=abs(ya); }
    void set_aitype(uint8_t t)      { Aitype=t; }
    void set_aistate(uint16_t s)      { Aistate=s; }
    void set_aistate_time(uint16_t t) { Aistate_time=t; }
    void set_hp(uint16_t h)           { Hp=h; }
    void set_mp(uint16_t m)           { Mp=m; }
    void set_fmp(uint16_t m)          { Fmp=m; }



    void set_fade_count(uint8_t f)        { Fade_count=f; }
    void set_fade_max(uint8_t m)          { Fade_max=m;  }
    void set_fade_dir(int8_t d)            { Fade_dir=d; }

    void set_frame_dir(int8_t d)           { Frame_dir=d; }
    void add_light(LightSource *ls);
    void add_object(GameObject *o);

    void remove_object(GameObject *o);
    void remove_light(LightSource *ls);
    void set_morph_status(morph_char *mc);

    void set_gravity(int x)               { grav_on=x; }
    void set_floating(int x)
    { if (x)
        set_flags(flags()|FLOATING_FLAG);
      else
        set_flags(flags()&(0xff-FLOATING_FLAG));
    }

    void clean_up();
};

extern SimpleObject g_default_simple;

#endif







