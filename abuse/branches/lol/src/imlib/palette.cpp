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

#include <math.h>

#include "common.h"

#include "palette.h"
#include "image.h"
#include "video.h"
#include "filter.h"

Palette *lastl = NULL;

Palette::Palette(bFILE *fp)
{
    int count = fp->read_uint16();
    m_colors.Resize(count);
    m_used.Resize(count, 0);
    set_all_unused();
    fp->read(m_colors.Data(), m_colors.Bytes());
    bg = 0;
}

Palette::Palette(SpecEntry *e, bFILE *fp)
{
    fp->seek(e->offset,0);
    int count = fp->read_uint16();
    m_colors.Resize(count);
    m_used.Resize(count, 0);
    set_all_unused();
    fp->read(m_colors.Data(), m_colors.Bytes());
    bg = 0;
}

int Palette::write(bFILE *fp)
{
    fp->write_uint16(m_colors.Count());
    return fp->write(m_colors.Data(), m_colors.Bytes()) == m_colors.Bytes();
}

int Palette::FindClosest(u8vec3 color) const
{
     int c = 0, d = 0x100000;
     for (int i = 0; i < m_colors.Count(); ++i)
     {
         int nd = sqlength((ivec3)(color - m_colors[i]));
         if (nd < d)
         {
             c = i;
             d = nd;
         }
     }
     return c;
}

int Palette::FindColor(u8vec3 color) const
{
    for (int i = 0; i < m_colors.Count(); ++i)
        if (m_used[i] && m_colors[i] == color)
            return i;
    return -1;
}

uint32_t Palette::getquad(int x)
{
    union { char entry[4]; uint32_t ret; };
    entry[3] = 0;
    entry[2] = m_colors[x].r;
    entry[1] = m_colors[x].g;
    entry[0] = m_colors[x].b;
    return ret;
}


void Palette::black_white()
{
    for (int i = 0; i < 256; i++)
        m_colors[i] = u8vec3(GetGray(i));
}

void Palette::make_black_white()
{
    for (int i = 0; i < m_colors.Count(); i++)
    {
        uint8_t c = (uint8_t)((double)i / m_colors.Count() * 255);
        m_colors[i] = u8vec3(c);
    }
}

void Palette::set_rgbs()
{
    ASSERT(m_colors.Count() == 256);

    for (int i = 0; i < 64; i++)
    {
        int v = 0;
        if (i > 0)
        {
            v = (int)((double)i + (double)(lol::sqrt(63.0 - i)));
            v <<= 2;
        }

        m_colors[i] = u8vec3(i, 0, 0);       // reds 0-63
        m_colors[i + 64] = u8vec3(0, i, 0);
        m_colors[i + 128] = u8vec3(0, 0, i); // blues 128-191
        m_colors[i + 192] = u8vec3(v, v, v); // whites .. rest
    }
    set_all_used();
}

void Palette::set_all_used()
{
    memset(m_used.Data(), 1, m_used.Bytes());
}

void Palette::set_all_unused()
{
    memset(m_used.Data(), 0, m_used.Bytes());
}

Palette *Palette::Copy() const
{
    Palette *p = new Palette(m_colors.Count());
    p->m_colors = m_colors;
    p->m_used = m_used;
    return p;
}

int Palette::used(int color_num)
{
    return m_used[color_num];
}

void Palette::defaults()
{
    m_colors[0] = u8vec3(0);
    m_used[0] = 1;

    for (int i = 1; i < m_colors.Count(); i++)
        m_used[i] = 0;

    if (m_colors.Count() == 256)
        for (int i = 0; i < m_colors.Count(); i++)
            m_colors[i] = u8vec3(RED3(i), GREEN3(i), BLUE2(i));
    else if (m_colors.Count() == 16)
        for (int i = 0; i < m_colors.Count(); i++)
            m_colors[i] = u8vec3(255 - (i & 3),
                                 255 - ((i & 4) >> 2),
                                 255 - ((i & 8) >> 3));
    else
        for (int i = 0; i < m_colors.Count(); i++)
            m_colors[i] = u8vec3(255 - (i % 3),
                                 255 - ((i + 1) % 3),
                                 255 - ((i + 2) % 3));
}

void Palette::shift(int amount)
{
    if (amount < 0)
    {
        int m = -amount;
        for (int i = 0; i < m_colors.Count(); i++)
        {
            u8vec3 c = m_colors[i];
            m_colors[i] = u8vec3(c.r >> m, c.g >> m, c.b >> m);
        }
    }
    else if (amount > 0)
    {
        int m = amount;
        for (int i = 0; i < m_colors.Count(); i++)
        {
            u8vec3 c = m_colors[i];
            m_colors[i] = u8vec3(c.r << m, c.g << m, c.b << m);
        }
    }
}



void Palette::SetColor(int n, u8vec3 color)
{
    m_colors[n] = color;
}

u8vec3 Palette::GetColor(int n) const
{
    return m_colors[n];
}

uint8_t Palette::GetGray(int n) const
{
    u8vec3 c = m_colors[n];
    return (uint8_t)(c.r * 0.30 + c.g * 0.59 + c.b * 0.11);
}

Palette::Palette(int number_colors)
{
    ASSERT(number_colors > 0, "Palette::constructor - need at least one color!");

    m_colors.Resize(number_colors);
    m_used.Resize(number_colors);
    defaults();
}



quant_node::~quant_node()
{
/*  if (!is_leaf())
  { for (i=0; i<8; i++)
      if (children[i])
      {    delete children[i];
    children[i]=NULL;
      }
  } */
}


/*void quant_node::prune()
{
  int t,r,g,b;
  ASSERT(!is_leaf(),"Cannot prune a leaf!");
  total(t,r,g,b);
  red=r/t;
  green=g/t;
  blue=b/t;
  be_childish();
} */

void quant_node::total(int &tnodes, int &tr, int &tg, int &tb)
{
  int i;
  if (is_leaf())
  { tnodes+=tot;
    tr+=red*tot;
    tg+=green*tot;
    tb+=blue*tot;
  }
  else
  { for (i=0; i<8; i++)
      if (children[i])
    children[i]->total(tnodes,tr,tg,tb);
  }
}

quant_node::quant_node(int level, quant_node *dad,
    unsigned char r, unsigned char g, unsigned char b)
{
    int i;
    ASSERT(level <= 8, "Tree cannot have more than eight levels");

    if (level == 8)
        be_childish();
    else
        for (i=0; i<8; i++) children[i]=NULL;
    padre = dad;
    red = r; green = g; blue = b;
    tot = 0;
}

quant_palette::quant_palette(int max_colors)
{ root=NULL; nc=0; mx=max_colors; }

void quant_palette::re_delete(quant_node *who, int lev)  // removes all children from memory
{ int x;                                  // and recurses down
  if (who)
  {
    if (!who->is_leaf())
    { for (x=0; x<8; x++)
    if (who->children[x])
    {
      ASSERT(lev < 8, "Levl > 7");
      re_delete(who->children[x],lev+1);
      level[lev].unlink(who->children[x]);
      delete who->children[x];
    }
    }
  }
}

void quant_palette::prune()
{
  int pruned,lev,x,r,g,b,t;
  quant_node *p=NULL,*f=NULL;
  for (pruned=0,lev=8; lev>1 && !pruned; lev--)
  {
    p=(quant_node *)level[lev-1].first();
    if (p)
    { do
      {
    f=p->father();
    for (x=0; x<8 && !pruned; x++)
      if (f->children[x])
        if (f->children[x]->Next()!=p->Next())        // if this son is not me!
        pruned=1;                   //  I have a brother! stop
       p=(quant_node *)p->Next();
      } while (p != level[lev-1].first() && !pruned);
    }
  }
  ASSERT(lev > 0, "could not prune!");
  t=0; r=0; g=0; b=0;
  f->total(t,r,g,b);
  if (t<=1)
  {
    t=0; r=0; g=0; b=0;
    f->total(t,r,g,b);
  }
  ASSERT(t > 1, "Should be more colors");
  printf("%d Pruned at level %d, r=%d, g=%d, b=%d, total nodes off = %d\n",nc,
    lev,r/t,g/t,b/t,t);
  f->set(r/t,g/t,b/t);
  nc-=t;
  nc++;
  re_delete(f,lev);
  f->be_childish();
}

quant_palette::~quant_palette()
{
  if (root)
  {
    re_delete(root,1);
    delete root;
  }
}

int Palette::FindBrightest(int all) const
{
    int brv = 0, bri = 0;

    for (int i = 0; i < m_colors.Count(); ++i)
    {
        if (all || m_used[i])
        {
            int gray = GetGray(i);
            if (gray > brv)
            {
                brv = gray;
                bri = i;
            }
        }
    }

    return bri;
}

int Palette::FindDarkest(int all, int noblack) const
{
    int brv = 258, bri = 0;

    for (int i = 0; i < m_colors.Count(); ++i)
    {
        if (all || m_used[i])
        {
            int gray = GetGray(i);
            if (gray < brv && (gray || !noblack))
            {
                brv = gray;
                bri = i;
            }
        }
    }

    return bri;
}

Palette *last_loaded()
{
    return lastl;
}

void Palette::FadeTo(int total_fades, int fade_on, u8vec3 dest)
{
    for (int i = 0; i < m_colors.Count(); i++)
        m_colors[i] += u8vec3(ivec3(dest - m_colors[i]) * fade_on / total_fades);
}

