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

#include <stdlib.h>

#include "common.h"

#include "light.h"
#include "image.h"
#include "video.h"
#include "palette.h"
#include "timing.h"
#include "specs.h"
#include "dprint.h"
#include "filter.h"
#include "status.h"
#include "dev.h"

LightSource *first_light_source=NULL;
uint8_t *white_light,*white_light_initial,*green_light,*trans_table;
short ambient_ramp=0;
short shutdown_lighting_value,shutdown_lighting=0;
extern char disable_autolight;   // defined in dev.h

int light_detail=MEDIUM_DETAIL;

int32_t light_to_number(LightSource *l)
{
    if (l)
    {
        int x = 1;
        for (LightSource *s = first_light_source; s; s = s->m_next, x++)
            if (s == l)
                return x;
    }
    return 0;
}


LightSource *number_to_light(int32_t x)
{
  if (x==0) return NULL;
  x--;
  LightSource *s=first_light_source;
  for (; x && s; x--,s=s->m_next)
    ;
  return s;
}

LightSource *LightSource::Copy()
{
    m_next = new LightSource(m_type, m_pos, m_inner_radius,
                             m_outer_radius, m_shift, m_next);
    return m_next;
}

void delete_all_lights()
{
  while (first_light_source)
  {
    if (dev_cont)
      dev_cont->notify_deleted_light(first_light_source);

    LightSource *p=first_light_source;
    first_light_source=first_light_source->m_next;
    delete p;
  }
}

void delete_light(LightSource *which)
{
  if (dev_cont)
    dev_cont->notify_deleted_light(which);

  if (which==first_light_source)
  {
    first_light_source=first_light_source->m_next;
    delete which;
  }
  else
  {
    LightSource *f=first_light_source;
    for (; f->m_next!=which && f; f=f->m_next)
      ;
    if (f)
    {
      f->m_next=which->m_next;
      delete which;
    }
  }
}

void LightSource::CalcRange()
{
    ivec2 delta = ivec2(m_outer_radius >> m_shift.x,
                        m_outer_radius >> m_shift.y);
    switch (m_type)
    {
    case 0:
        m_p1 = m_pos - delta;
        m_p2 = m_pos + delta;
        break;
    case 1:
        m_p1 = m_pos - delta;
        m_p2 = m_pos + ivec2(delta.x, 0);
        break;
    case 2:
        m_p1 = m_pos - ivec2(delta.x, 0);
        m_p2 = m_pos + delta;
        break;
    case 3:
        m_p1 = m_pos - ivec2(0, delta.y);
        m_p2 = m_pos + delta;
        break;
    case 4:
        m_p1 = m_pos - delta;
        m_p2 = m_pos + ivec2(0, delta.y);
        break;
    case 5:
        m_p1 = m_pos - ivec2(0, delta.y);
        m_p2 = m_pos + ivec2(delta.x, 0);
        break;
    case 6:
        m_p1 = m_pos - delta;
        m_p2 = m_pos;
        break;
    case 7:
        m_p1 = m_pos - ivec2(delta.x, 0);
        m_p2 = m_pos + ivec2(0, delta.y);
        break;
    case 8:
        m_p1 = m_pos;
        m_p2 = m_pos + delta;
        break;
    case 9:
        m_p1 = m_pos;
        m_p2 = m_pos + m_shift;
        break;
    }
    mul_div = (1 << 16) / (m_outer_radius - m_inner_radius) * 64;
}

LightSource::LightSource(char Type, ivec2 pos, int32_t inner_radius,
               int32_t outer_radius, ivec2 shift, LightSource *next)
{
    m_type = Type;
    m_pos = pos;
    m_inner_radius = inner_radius;
    m_outer_radius = outer_radius;
    m_next = next;
    known = 0;
    m_shift = shift;

    CalcRange();
}


int count_lights()
{
  int t=0;
  for (LightSource *s=first_light_source; s; s=s->m_next)
    t++;
  return t;
}

LightSource *AddLightSource(char type, ivec2 pos,
                            int32_t inner, int32_t outer, ivec2 shift)
{
    first_light_source = new LightSource(type, pos, inner, outer,
                                         shift, first_light_source);
    return first_light_source;
}


#define TTINTS 9
uint8_t *tints[TTINTS];
uint8_t bright_tint[256];

void calc_tint(uint8_t *tint, u8vec3 s, u8vec3 a, Palette *pal)
{
  Palette npal;
  for (int i = 0; i < 256; i++)
  {
    npal.SetColor(i, s);
    s = (u8vec3)clamp((ivec3)s + (ivec3)a, 0, 255);
  }
  Filter f(pal, &npal);
  Filter f2(&npal, pal);

  for (int i = 0; i < 256; i++, tint++)
    *tint = f2.GetMapping(f.GetMapping(i));
}


void calc_light_table(Palette *pal)
{
    white_light_initial=(uint8_t *)malloc(256*64);
    white_light=white_light_initial;

//    green_light=(uint8_t *)malloc(256*64);
    int i = 0;
    for( ; i < TTINTS; i++ )
    {
        tints[i] = (uint8_t *)malloc( 256 );
    }

    char *lightpath;
    lightpath = (char *)malloc( strlen( get_save_filename_prefix() ) + 9 + 1 );
    sprintf( lightpath, "%slight.tbl", get_save_filename_prefix() );

    bFILE *fp=open_file( lightpath, "rb" );
    int recalc = 0;
    if( fp->open_failure() )
    {
        recalc = 1;
    }
    else
    {
        if (fp->read_uint16() != Crc::FromData((uint8_t *)pal->Data(), 768))
            recalc=1;
        else
        {
            fp->read(white_light,256*64);
//            fp->read(green_light,256*64);
            for (i=0; i<TTINTS; i++)
                fp->read(tints[i],256);
            fp->read(bright_tint,256);
//            trans_table=(uint8_t *)malloc(256*256);
//            fp.read(trans_table,256*256);
        }
    }
    delete fp;
    fp = NULL;

    if( recalc )
    {
        dprintf("Palette has changed, recalculating light table...\n");
        stat_man->push("white light",NULL);
        for (int color = 0; color < 256; color++)
        {
            u8vec3 rgb = pal->GetColor(color);
            stat_man->update(color * 100 / 256);
            for (int intensity = 63; intensity >= 0; intensity--)
            {
                if (rgb != u8vec3(0))
                    white_light[intensity*256+color] = pal->FindClosest(rgb);
                else
                    white_light[intensity*256+color]=0;
                rgb = max(rgb, u8vec3(1)) - u8vec3(1);
            }
        }
        stat_man->pop();

/*    stat_man->push("green light",NULL);
    for (color=0; color<256; color++)
    {
      stat_man->update(color*100/256);
      uint8_t r,g,b;
      pal->get(color,b,r,g);
      r=r*3/5; b=b*3/5; g+=7; if (g>255) g=255;

      for (int intensity=63; intensity>=0; intensity--)
      {
    if (r>0 || g>0 || b>0)
          green_light[intensity*256+color]=pal->find_closest(r,g,b);
    else
          green_light[intensity*256+color]=0;
    if (r) r--;
    if ((intensity&1)==1)
      if (g) g--;
    if (b) b--;
      }
    }
    stat_man->pop(); */

    stat_man->push("tints",NULL);
    uint8_t t[TTINTS*6]={ 0,0,0,0,0,0, // normal
                   0,0,0,1,0,0,     // red
           0,0,0,1,1,0,     // yellow
           0,0,0,1,0,1,     // purple
           0,0,0,1,1,1,     // gray
           0,0,0,0,1,0,     // green
           0,0,0,0,0,1,     // blue
           0,0,0,0,1,1,     // cyan






           0,0,0,0,0,0   // reverse green  (night vision effect)
         } ;
    uint8_t *ti=t+6;
    uint8_t *c;
    for (i=0,c=tints[0]; i<256; i++,c++) *c=i;  // make the normal tint (maps everthing to itself)
    for (i=0,c=tints[TTINTS-1]; i<256; i++,c++)  // reverse green
    {
      ivec3 rgb = (ivec3)pal->GetColor(i);
      rgb = clamp(ivec3(rgb.r / 2, 255 - rgb.g - 30, rgb.b * 3 / 5 + 50), 0, 255);
      *c = pal->FindClosest(rgb);
    }
    for (i=0; i<256; i++)
    {
      u8vec3 rgb = pal->GetColor(i) + (u8vec3(255) - pal->GetColor(i)) / (uint8_t)2;
      bright_tint[i] = pal->FindClosest(rgb);
    }

    // make the colored tints
    for (i=1; i<TTINTS-1; i++)
    {
      stat_man->update(i*100/(TTINTS-1));
      calc_tint(tints[i], u8vec3(ti[0], ti[1], ti[2]),
                          u8vec3(ti[3], ti[4], ti[5]), pal);
      ti += 6;
    }
    stat_man->pop();
/*    fprintf(stderr,"calculating transparency tables (256 total)\n");
    trans_table=(uint8_t *)malloc(256*256);

    uint8_t *tp=trans_table;
    for (i=0; i<256; i++)
    {
      uint8_t r1,g1,b1,r2,g2,b2;
      pal->get(i,r1,g1,b1);
      if ((i%16)==0)
        fprintf(stderr,"%d ",i);
      for (int j=0; j<256; j++,tp++)
      {
    if (r1==0 && r2==0 && b2==0)
      *tp=j;
    else
    {
      pal->get(j,r2,g2,b2);
      *tp=pal->find_closest((r2-r1)*3/7+r1,(g2-g1)*3/7+g1,(b2-b1)*3/7+b1);
    }
      }
    }*/


        bFILE *f = open_file( lightpath, "wb" );
        if( f->open_failure() )
            dprintf( "Unable to open file light.tbl for writing\n" );
        else
        {
            f->write_uint16(Crc::FromData(pal->Data(), 768));
            f->write(white_light, 256 * 64);
//      f->write(green_light,256*64);
            for (int i=0; i<TTINTS; i++)
                f->write(tints[i],256);
            f->write(bright_tint,256);
//    f.write(trans_table,256*256);
        }
        delete f;
    }
    free( lightpath );
}


LightPatch *LightPatch::Copy(LightPatch *next)
{
    LightPatch *p = new LightPatch(m_p1, m_p2, next);
    p->m_lights = m_lights;
    return p;
}

#define MAX_LP 6

// insert light into list make sure the are sorted by y1
void InsertLight(LightPatch *&first, LightPatch *l)
{
    if (!first)
        first = l;
    else if (l->m_p1.y < first->m_p1.y)
    {
        l->m_next = first;
        first = l;
    }
    else
    {
        LightPatch *p = first;
        for (; p->m_next && p->m_next->m_p1.y < l->m_p1.y; p = p->m_next)
            ;
        l->m_next = p->m_next;
        p->m_next = l;
    }
}

void AddLight(LightPatch *&first, ivec2 p1, ivec2 p2, LightSource *who)
{
    for (LightPatch *p = first, *next; p; p = next)
    {
        next = p->m_next;

        // first see if light patch we are adding is enclosed entirely
        // by another patch
        if (p1.x >= p->m_p1.x && p1.y >= p->m_p1.y
             && p2.x <= p->m_p2.x && p2.y <= p->m_p2.y)
        {
            if (p->m_lights.Count() == MAX_LP)
                return;

            if (p1.x > p->m_p1.x)
            {
                LightPatch *l = p->Copy(nullptr);
                l->m_p2.x = p1.x - 1;
                InsertLight(first, l);
            }
            if (p2.x < p->m_p2.x)
            {
                LightPatch *l = p->Copy(nullptr);
                l->m_p1.x = p2.x + 1;
                InsertLight(first, l);
            }
            if (p1.y>p->m_p1.y)
            {
                LightPatch *l = p->Copy(nullptr);
                l->m_p1.x = p1.x;
                l->m_p2.x = p2.x;
                l->m_p2.y = p1.y - 1;
                InsertLight(first, l);
            }
            if (p2.y<p->m_p2.y)
            {
                LightPatch *l = p->Copy(nullptr);
                l->m_p1.x = p1.x;
                l->m_p2.x = p2.x;
                l->m_p1.y = p2.y + 1;
                InsertLight(first, l);
            }
            p->m_p1 = p1;
            p->m_p2 = p2;
            // p has possibly changed its p1.y, so we need to move it to
            // its correct sorted spot in the list
            if (first == p)
                first = first->m_next;
            else
            {
                LightPatch *q = first;
                for (; q->m_next!=p; q=q->m_next)
                    ;
                q->m_next=p->m_next;
            }
            InsertLight(first, p);

            p->m_lights.Push(who);
            return;
        }

        // see if the patch completly covers another patch.
        if (p1 <= p->m_p1 && p2 >= p->m_p2)
        {
            if (p1.x < p->m_p1.x)
                AddLight(first, p1, ivec2(p->m_p1.x - 1, p2.y), who);
            if (p2.x > p->m_p2.x)
                AddLight(first, ivec2(p->m_p2.x + 1, p1.y), p2, who);
            if (p1.y < p->m_p1.y)
                AddLight(first, ivec2(p->m_p1.x, p1.y), ivec2(p->m_p2.x, p->m_p1.y - 1), who);
            if (p2.y > p->m_p2.y)
                AddLight(first, ivec2(p->m_p1.x, p->m_p2.y + 1), ivec2(p->m_p2.x, p2.y), who);

            if (p->m_lights.Count() < MAX_LP)
                p->m_lights.Push(who);

            return;
        }

        // see if we intersect another rect
        if (p2 >= p->m_p1 && p1 <= p->m_p2)
        {
            ivec2 ap1 = lol::max(p1, p->m_p1),
                  ap2 = lol::min(p2, p->m_p2);

            if (p1.x < p->m_p1.x)
                AddLight(first, ivec2(p1.x, ap1.y), ivec2(p->m_p1.x - 1, ap2.y), who);

            if (p2.x > p->m_p2.x)
                AddLight(first, ivec2(p->m_p2.x + 1, ap1.y), ivec2(p2.x, ap2.y), who);

            if (p1.y < p->m_p1.y)
                AddLight(first, p1, ivec2(p2.x, p->m_p1.y - 1), who);

            if (p2.y > p->m_p2.y)
                AddLight(first, ivec2(p1.x, p->m_p2.y + 1), p2, who);

            AddLight(first, ap1, ap2, who);
            return;
        }
    }
}

LightPatch *MakePatchList(ivec2 size, ivec2 screen)
{
    LightPatch *first = new LightPatch(ivec2(0), size - ivec2(1), nullptr);

    // determine which lights will have effect
    for (LightSource *f = first_light_source; f; f = f->m_next)
    {
        ivec2 p1 = lol::max(f->m_p1 - screen, ivec2(0));
        ivec2 p2 = lol::min(f->m_p2 - screen, size - ivec2(1));

        if (p1.x <= p2.x && p1.y <= p2.y)
            AddLight(first, p1, p2, f);
    }

    return first;
}

inline void MAP_PUT(uint8_t * screen_addr, uint8_t * remap, int w)
{
  register int cx=w;
  register uint8_t * di=screen_addr;
  register uint8_t * si=remap;
  while (cx--)
  {
    uint8_t x=*((uint8_t *)si+*((uint8_t *)di));
    *((uint8_t *)(di++))=x;
  }
}

inline void MAP_2PUT(uint8_t * in_addr, uint8_t * out_addr, uint8_t * remap, int w)
{
  while (w--)
  {
    uint8_t x=*(((uint8_t *)remap)+(*(uint8_t *)(in_addr++)));
    *((uint8_t *)(out_addr++))=x;
    *((uint8_t *)(out_addr++))=x;
  }
}

uint16_t min_light_level;

// calculate the light value for this block.  sum up all contritors
static inline int CalcLightValue(LightPatch *lp, ivec2 screen)
{
    int lv = min_light_level;

    for (int i = 0; i < lp->m_lights.Count(); ++i)
    {
        LightSource *l = lp->m_lights[i];

        if (l->m_type == 9) // if light is a Solid rectangle, return its value
            return l->m_inner_radius;

        // xdist between light and this block; shift makes distance further, making light skinner.
        int dx = lol::abs(l->m_pos.x - screen.x) << l->m_shift.x;
        int dy = lol::abs(l->m_pos.y - screen.y) << l->m_shift.y;

        // calculate approximate distance
        int r2 = dx + dy - (lol::min(dx, dy) >> 1);

        if (r2 < l->m_outer_radius) // if this within the light's outer radius?
        {
            int v = l->m_outer_radius - r2;
            lv += v * l->mul_div >> 16;
        }
    }

    return lol::min(lv, 63); // lighting table only has 64 (256 bytes) entries
}


void remap_line_asm2(uint8_t *addr,uint8_t *light_lookup,uint8_t *remap_line,int count)
//inline void remap_line_asm2(uint8_t *addr,uint8_t *light_lookup,uint8_t *remap_line,int count)
{
  while (count--)
  {
    uint8_t *off=light_lookup+(((int32_t)*remap_line)<<8);
    remap_line++;

    *addr=off[*addr];
    addr[1]=off[addr[1]];
    addr[2]=off[addr[2]];
    addr[3]=off[addr[3]];
    addr[4]=off[addr[4]];
    addr[5]=off[addr[5]];
    addr[6]=off[addr[6]];
    addr[7]=off[addr[7]];
    addr+=8;

  }
}

inline void put_8line(uint8_t *in_line, uint8_t *out_line, uint8_t *remap, uint8_t *light_lookup, int count)
{
  uint8_t v;
  int x;
  for (x=0; x<count; x++)
  {
    uint8_t *off=light_lookup+(((int32_t)*remap)<<8);

    v=off[*(in_line++)];
    *(out_line++)=v;
    *(out_line++)=v;

    v=off[*(in_line++)];
    *(out_line++)=v;
    *(out_line++)=v;

    v=off[*(in_line++)];
    *(out_line++)=v;
    *(out_line++)=v;

    v=off[*(in_line++)];
    *(out_line++)=v;
    *(out_line++)=v;

    v=off[*(in_line++)];
    *(out_line++)=v;
    *(out_line++)=v;

    v=off[*(in_line++)];
    *(out_line++)=v;
    *(out_line++)=v;

    v=off[*(in_line++)];
    *(out_line++)=v;
    *(out_line++)=v;

    v=off[*(in_line++)];
    *(out_line++)=v;
    *(out_line++)=v;

    remap++;
  }
}


void light_screen(image *sc, int32_t screenx, int32_t screeny, uint8_t *light_lookup, uint16_t ambient)
{
  int lx_run=0,ly_run;                     // light block x & y run size in pixels ==  (1<<lx_run)

  if (shutdown_lighting && !disable_autolight)
    ambient=shutdown_lighting_value;

  switch (light_detail)
  {
    case HIGH_DETAIL :
    { lx_run=2; ly_run=1; } break;       // 4 x 2 patches
    case MEDIUM_DETAIL :
    { lx_run=3; ly_run=2; } break;       // 8 x 4 patches  (default)
    case LOW_DETAIL :
    { lx_run=4; ly_run=3; } break;       // 16 x 8 patches
    case POOR_DETAIL :                   // poor detail is no lighting
    return ;
  }
  if ((int)ambient+ambient_ramp<0)
    min_light_level=0;
  else if ((int)ambient+ambient_ramp>63)
    min_light_level=63;
  else min_light_level=(int)ambient+ambient_ramp;

  if (ambient==63) return ;
  ivec2 caa, cbb;
  sc->GetClip(caa, cbb);

  LightPatch *first = MakePatchList(cbb - caa, ivec2(screenx, screeny));

  int prefix_x=(screenx&7);
  int prefix=screenx&7;
  if (prefix)
    prefix=8-prefix;
  int suffix_x = cbb.x - 1 - caa.x - (screenx & 7);

  int suffix = (cbb.x - caa.x - prefix) & 7;

  int32_t remap_size=((cbb.x - caa.x - prefix - suffix)>>lx_run);

  uint8_t *remap_line=(uint8_t *)malloc(remap_size);

  LightPatch *f=first;

  main_screen->Lock();

  int scr_w=main_screen->Size().x;
  uint8_t *screen_line=main_screen->scan_line(caa.y)+caa.x;

  for (int y = caa.y; y < cbb.y; )
  {
    int x,count;
//    while (f->m_next && f->m_p2.y<y)
//      f=f->m_next;
    uint8_t *rem=remap_line;

    int todoy=4-((screeny+y)&3);
    if (y + todoy >= cbb.y)
      todoy = cbb.y - y;

    int calcy=((y+screeny)&(~3))-caa.y;


    if (suffix)
    {
      LightPatch *lp=f;
      for (; (lp->m_p1.y>y-caa.y || lp->m_p2.y<y-caa.y ||
                  lp->m_p1.x>suffix_x || lp->m_p2.x<suffix_x); lp=lp->m_next)
          ;
      uint8_t * caddr=(uint8_t *)screen_line + cbb.x - caa.x - suffix;
      uint8_t *r=light_lookup+(((int32_t)CalcLightValue(lp, ivec2(suffix_x + screenx, calcy)) << 8));
      switch (todoy)
      {
    case 4 :
    {
      MAP_PUT(caddr,r,suffix); caddr+=scr_w;
    }
    case 3 :
    { MAP_PUT(caddr,r,suffix); caddr+=scr_w; }
    case 2 :
    { MAP_PUT(caddr,r,suffix); caddr+=scr_w; }
    case 1 :
    {
      MAP_PUT(caddr,r,suffix);
    }
      }
    }

    if (prefix)
    {
      LightPatch *lp=f;
      for (; (lp->m_p1.y>y-caa.y || lp->m_p2.y<y-caa.y ||
                  lp->m_p1.x>prefix_x || lp->m_p2.x<prefix_x); lp=lp->m_next);

      uint8_t *r=light_lookup+(((int32_t)CalcLightValue(lp, ivec2(prefix_x + screenx, calcy)) << 8));
      uint8_t * caddr=(uint8_t *)screen_line;
      switch (todoy)
      {
    case 4 :
    {
      MAP_PUT(caddr,r,prefix);
      caddr+=scr_w;
    }
    case 3 :
    { MAP_PUT(caddr,r,prefix); caddr+=scr_w; }
    case 2 :
    { MAP_PUT(caddr,r,prefix); caddr+=scr_w; }
    case 1 :
    { MAP_PUT(caddr,r,prefix); }
      }
      screen_line+=prefix;
    }




    for (x=prefix,count=0; count<remap_size; count++,x+=8,rem++)
    {
      LightPatch *lp=f;
      for (; (lp->m_p1.y>y-caa.y || lp->m_p2.y<y-caa.y || lp->m_p1.x>x || lp->m_p2.x<x); lp=lp->m_next)
        ;
      *rem = CalcLightValue(lp, ivec2(x + screenx, calcy));
    }

    switch (todoy)
    {
      case 4 :
      remap_line_asm2(screen_line,light_lookup,remap_line,count);  y++; todoy--;  screen_line+=scr_w;
      case 3 :
      remap_line_asm2(screen_line,light_lookup,remap_line,count);  y++; todoy--;  screen_line+=scr_w;
      case 2 :
      remap_line_asm2(screen_line,light_lookup,remap_line,count);  y++; todoy--;  screen_line+=scr_w;
      case 1 :
      remap_line_asm2(screen_line,light_lookup,remap_line,count);  y++; todoy--;  screen_line+=scr_w;
    }


    screen_line-=prefix;
  }
  main_screen->Unlock();

  while (first)
  {
    LightPatch *p=first;
    first=first->m_next;
    delete p;
  }
  free(remap_line);
}


void double_light_screen(image *sc, int32_t screenx, int32_t screeny, uint8_t *light_lookup, uint16_t ambient,
             image *out, int32_t out_x, int32_t out_y)
{
  if (sc->Size().x*2+out_x>out->Size().x ||
      sc->Size().y*2+out_y>out->Size().y)
    return ;   // screen was resized and small_render has not changed size yet


  int lx_run=0,ly_run;                     // light block x & y run size in pixels ==  (1<<lx_run)
  switch (light_detail)
  {
    case HIGH_DETAIL :
    { lx_run=2; ly_run=1; } break;       // 4 x 2 patches
    case MEDIUM_DETAIL :
    { lx_run=3; ly_run=2; } break;       // 8 x 4 patches  (default)
    case LOW_DETAIL :
    { lx_run=4; ly_run=3; } break;       // 16 x 8 patches
    case POOR_DETAIL :                   // poor detail is no lighting
    return ;
  }
  if ((int)ambient+ambient_ramp<0)
    min_light_level=0;
  else if ((int)ambient+ambient_ramp>63)
    min_light_level=63;
  else min_light_level=(int)ambient+ambient_ramp;

  ivec2 caa, cbb;
  sc->GetClip(caa, cbb);


  if (ambient==63)      // lights off, just double the pixels
  {
    uint8_t *src=sc->scan_line(0);
    uint8_t *dst=out->scan_line(out_y+caa.y*2)+caa.x*2+out_x;
    int d_skip=out->Size().x-sc->Size().x*2;
    int x,y;
    uint16_t v;
    for (y=sc->Size().y; y; y--)
    {
      for (x=sc->Size().x; x; x--)
      {
    v=*(src++);
    *(dst++)=v;
    *(dst++)=v;
      }
      dst=dst+d_skip;
      memcpy(dst,dst-out->Size().x,sc->Size().x*2);
      dst+=out->Size().x;
    }

    return ;
  }

  LightPatch *first = MakePatchList(cbb - caa, ivec2(screenx, screeny));

  int scr_w=sc->Size().x;
  int dscr_w=out->Size().x;

  int prefix_x=(screenx&7);
  int prefix=screenx&7;
  if (prefix)
    prefix=8-prefix;
  int suffix_x = cbb.x - 1 - caa.x - (screenx & 7);

  int suffix = (cbb.x - caa.x - prefix) & 7;

  int32_t remap_size = ((cbb.x - caa.x - prefix - suffix)>>lx_run);

  uint8_t *remap_line=(uint8_t *)malloc(remap_size);

  LightPatch *f=first;
  uint8_t *in_line=sc->scan_line(caa.y)+caa.x;
  uint8_t *out_line=out->scan_line(caa.y*2+out_y)+caa.x*2+out_x;


  for (int y = caa.y; y < cbb.y; )
  {
    int x,count;
//    while (f->m_next && f->m_p2.y<y)
//      f=f->m_next;
    uint8_t *rem=remap_line;

    int todoy=4-((screeny+y)&3);
    if (y + todoy >= cbb.y)
      todoy = cbb.y - y;

    int calcy=((y+screeny)&(~3))-caa.y;


    if (suffix)
    {
      LightPatch *lp=f;
      for (; (lp->m_p1.y>y-caa.y || lp->m_p2.y<y-caa.y ||
                  lp->m_p1.x>suffix_x || lp->m_p2.x<suffix_x); lp=lp->m_next);
      uint8_t * caddr=(uint8_t *)in_line + cbb.x - caa.x - suffix;
      uint8_t * daddr=(uint8_t *)out_line+(cbb.x - caa.x - suffix)*2;

      uint8_t *r = light_lookup + (((int32_t)CalcLightValue(lp, ivec2(suffix_x + screenx, calcy)) << 8));
      switch (todoy)
      {
    case 4 :
    {
      MAP_2PUT(caddr,daddr,r,suffix); daddr+=dscr_w;
      MAP_2PUT(caddr,daddr,r,suffix); daddr+=dscr_w; caddr+=scr_w;
    }
    case 3 :
    {
      MAP_2PUT(caddr,daddr,r,suffix); daddr+=dscr_w;
      MAP_2PUT(caddr,daddr,r,suffix); daddr+=dscr_w; caddr+=scr_w;
    }
    case 2 :
    {
      MAP_2PUT(caddr,daddr,r,suffix); daddr+=dscr_w;
      MAP_2PUT(caddr,daddr,r,suffix); daddr+=dscr_w; caddr+=scr_w;
    }
    case 1 :
    {
      MAP_2PUT(caddr,daddr,r,suffix); daddr+=dscr_w;
      MAP_2PUT(caddr,daddr,r,suffix); daddr+=dscr_w; caddr+=scr_w;
    } break;
      }
    }

    if (prefix)
    {
      LightPatch *lp=f;
      for (; (lp->m_p1.y > y - caa.y || lp->m_p2.y < y - caa.y ||
               lp->m_p1.x > prefix_x || lp->m_p2.x < prefix_x); lp = lp->m_next)
          ;

      uint8_t *r = light_lookup + (((int32_t)CalcLightValue(lp, ivec2(prefix_x + screenx, calcy)) << 8));
      uint8_t * caddr=(uint8_t *)in_line;
      uint8_t * daddr=(uint8_t *)out_line;
      switch (todoy)
      {
    case 4 :
    {
      MAP_2PUT(caddr,daddr,r,prefix); daddr+=dscr_w;
      MAP_2PUT(caddr,daddr,r,prefix); daddr+=dscr_w; caddr+=scr_w;
    }
    case 3 :
    {
      MAP_2PUT(caddr,daddr,r,prefix); daddr+=dscr_w;
      MAP_2PUT(caddr,daddr,r,prefix); daddr+=dscr_w; caddr+=scr_w;
    }
    case 2 :
    {
      MAP_2PUT(caddr,daddr,r,prefix); daddr+=dscr_w;
      MAP_2PUT(caddr,daddr,r,prefix); daddr+=dscr_w; caddr+=scr_w;
    }
    case 1 :
    {
      MAP_2PUT(caddr,daddr,r,prefix); daddr+=dscr_w;
      MAP_2PUT(caddr,daddr,r,prefix); daddr+=dscr_w; caddr+=scr_w;
    } break;
      }
      in_line+=prefix;
      out_line+=prefix*2;
    }




    for (x=prefix,count=0; count<remap_size; count++,x+=8,rem++)
    {
      LightPatch *lp=f;
      for (; (lp->m_p1.y > y - caa.y || lp->m_p2.y < y - caa.y
               || lp->m_p1.x > x || lp->m_p2.x < x); lp = lp->m_next)
        ;
      *rem = CalcLightValue(lp, ivec2(x + screenx, calcy));
    }

    rem=remap_line;

    put_8line(in_line,out_line,rem,light_lookup,count);
    memcpy(out_line+dscr_w,out_line,count*16);
    out_line+=dscr_w;
    in_line+=scr_w; out_line+=dscr_w; y++; todoy--;
    if (todoy)
    {
      put_8line(in_line,out_line,rem,light_lookup,count);
      memcpy(out_line+dscr_w,out_line,count*16);
      out_line+=dscr_w;
      in_line+=scr_w; out_line+=dscr_w; y++; todoy--;
      if (todoy)
      {
    put_8line(in_line,out_line,rem,light_lookup,count);
    memcpy(out_line+dscr_w,out_line,count*16);
    out_line+=dscr_w;
    in_line+=scr_w; out_line+=dscr_w; y++; todoy--;
    if (todoy)
    {
      put_8line(in_line,out_line,rem,light_lookup,count);
      memcpy(out_line+dscr_w,out_line,count*16);
      out_line+=dscr_w;
      in_line+=scr_w; out_line+=dscr_w; y++;
    }
      }
    }
    in_line-=prefix;
    out_line-=prefix*2;
  }


  while (first)
  {
    LightPatch *p=first;
    first=first->m_next;
    delete p;
  }
  free(remap_line);
}




void add_light_spec(SpecDir *sd, char const *level_name)
{
  int32_t size=4+4;  // number of lights and minimum light levels
  for (LightSource *f=first_light_source; f; f=f->m_next)
    size+=6*4+1;
  sd->add_by_hand(new SpecEntry(SPEC_LIGHT_LIST,"lights",NULL,size,0));
}

void write_lights(bFILE *fp)
{
  int t=0;
  LightSource *f=first_light_source;
  for (; f; f=f->m_next) t++;
  fp->write_uint32(t);
  fp->write_uint32(min_light_level);
  for (f = first_light_source; f; f = f->m_next)
  {
    fp->write_uint32(f->m_pos.x);
    fp->write_uint32(f->m_pos.y);
    fp->write_uint32(f->m_shift.x);
    fp->write_uint32(f->m_shift.y);
    fp->write_uint32(f->m_inner_radius);
    fp->write_uint32(f->m_outer_radius);
    fp->write_uint8(f->m_type);
  }
}


void read_lights(SpecDir *sd, bFILE *fp, char const *level_name)
{
  delete_all_lights();
  SpecEntry *se=sd->find("lights");
  if (se)
  {
    fp->seek(se->offset,SEEK_SET);
    int32_t t=fp->read_uint32();
    min_light_level=fp->read_uint32();
    LightSource *last=NULL;
    while (t)
    {
      t--;
      ivec2 pos, shift;
      pos.x = fp->read_uint32();
      pos.y = fp->read_uint32();
      shift.x = fp->read_uint32();
      shift.y = fp->read_uint32();
      int32_t ir=fp->read_uint32();
      int32_t ora=fp->read_uint32();
      int32_t ty=fp->read_uint8();

      LightSource *p = new LightSource(ty, pos, ir, ora, shift, nullptr);

      if (first_light_source)
        last->m_next = p;
      else
        first_light_source = p;
      last = p;
    }
  }
}
