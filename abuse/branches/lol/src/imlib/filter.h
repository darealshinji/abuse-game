/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef _FILTER_HPP
#define _FILTER_HPP

#include "imlib/image.h"
#include "imlib/palette.h"
#include "imlib/specs.h"

class Filter
{
public :
    Filter(int colors = 256);
    Filter(Palette *from, Palette *to);
    ~Filter();

    void Set(int color_num, int change_to);
    int GetMapping(int color_num) { return m_table[color_num]; }
    void Apply(AImage *im);
    void PutImage(AImage *screen, AImage *im, ivec2 pos);

private:
    Array<uint8_t> m_table;
};

class ColorFilter
{
public:
    ColorFilter(SpecEntry *e, bFILE *fp);
    ColorFilter(Palette *pal, int color_bits);
    ~ColorFilter();

    size_t DiskUsage();
    int Write(bFILE *fp);
    int Lookup(u8vec3 c)
    {
        return m_table[(c.r * m_size + c.g) * m_size + c.b];
    }

private:
    Array<uint8_t> m_table;
    int m_size;
};

#endif

