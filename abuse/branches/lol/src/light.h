/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "imlib/image.h"
#include "imlib/palette.h"

#include "configuration.h"
#include "crc.h"

#define TTINTS 9
extern uint8_t *tints[TTINTS];
extern uint8_t *white_light,*white_light_initial,*green_light,*trans_table;
extern int16_t ambient_ramp;
#define REVERSE_GREEN_TINT 8

extern int16_t shutdown_lighting_value,shutdown_lighting;

class LightSource
{
public:
    LightSource(char Type, ivec2 pos, int32_t inner_r, int32_t outer_r,
                ivec2 shift, LightSource *next);
    LightSource *Copy();
    void CalcRange();

    ivec2 m_pos, m_shift;
    int32_t m_type;
    int32_t m_outer_radius, mul_div, m_inner_radius;

    ivec2 m_p1, m_p2;
    char known;
    LightSource *m_next;
};

class LightPatch
{
public:
    LightPatch(ivec2 p1, ivec2 p2, LightPatch *next)
      : m_p1(p1),
        m_p2(p2),
        m_next(next)
    {
    }

    void AddLight(ivec2 p1, ivec2 p2, LightSource *who);
    LightPatch *Copy(LightPatch *next);

    ivec2 m_p1, m_p2;
    Array<LightSource *> m_lights;

    /* FIXME: this should disappear */
    LightPatch *m_next;
};

void delete_all_lights();
void delete_light(LightSource *which);
LightSource *AddLightSource(char type, ivec2 pos, int32_t inner,
                            int32_t outer, ivec2 shift);

void add_light_spec(SpecDir *sd, char const *level_name);
void write_lights(bFILE *fp);
void read_lights(SpecDir *sd, bFILE *fp, char const *level_name);

void light_screen(AImage *sc, int32_t screenx, int32_t screeny, uint8_t *light_lookup, uint16_t ambient);

void calc_light_table(Palette *pal);
extern LightSource *first_light_source;
extern int light_detail;

extern int32_t light_to_number(LightSource *l);
extern LightSource *number_to_light(int32_t x);

#endif
