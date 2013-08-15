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
            m_colors[i] = u8vec3(((i >> 5) & 7) * 255 / 7,
                                 ((i >> 2) & 7) * 255 / 7,
                                 (i & 3) * 255 / 7);
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

void Palette::Load()
{
    // Force to only 256 colours.
    // Shouldn't be needed, but best to be safe.
    if (m_colors.Count() > 256)
        m_colors.Resize(256);

    delete lastl;
    lastl = Copy();
}

Palette *Palette::LastLoaded()
{
    return lastl;
}

void Palette::FadeTo(int total_fades, int fade_on, u8vec3 dest)
{
    for (int i = 0; i < m_colors.Count(); i++)
        m_colors[i] += u8vec3(ivec3(dest - m_colors[i]) * fade_on / total_fades);
}

