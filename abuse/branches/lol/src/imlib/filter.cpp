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

#include "imlib/image.h"
#include "imlib/filter.h"

Filter::Filter(int colors)
{
    ASSERT(colors >= 0 && colors <= 256, "bad color count");

    m_table.Resize(colors, 0);
}

// Creates a conversion filter from one palette to another
Filter::Filter(Palette *from, Palette *to)
{
    int size = lol::max(from->Count(), to->Count());
    m_table.Resize(size);

    int dk = to->FindDarkest(1);

    for (int i = 0; i < size; i++)
    {
        u8vec3 color = from->GetColor(i);
        int n = to->FindClosest(color);

        // Make sure non-blacks don't get remapped to the transparency
        if (color != u8vec3::zero && to->GetColor(n) == u8vec3::zero)
            n = dk;

        m_table[i] = n;
    }
}

Filter::~Filter()
{
}

void Filter::Set(int color_num, int change_to)
{
    ASSERT(color_num >= 0 && color_num < m_table.Count(), "Bad colors_num");

    m_table[color_num] = change_to;
}

void Filter::Apply(AImage *im)
{
    uint8_t *dst = im->scan_line(0);
    int npixels = im->Size().x * im->Size().y;
    while (npixels--)
    {
        *dst = m_table[*dst];
        ++dst;
    }
}

/* This is only ever used in the editor, when showing the toolbar. It
 * does not look like it's very useful. */
void Filter::PutImage(AImage *screen, AImage *im, ivec2 pos)
{
    ivec2 aa = ivec2::zero, bb = im->Size(), caa, cbb;
    screen->GetClip(caa, cbb);

    // See if the image gets clipped off the screen
    if (!(pos < cbb && pos + (bb - aa) > caa))
        return;

    aa += lol::max(caa - pos, ivec2::zero);
    pos = lol::max(pos, caa);
    bb = lol::min(bb, cbb - pos + aa);

    if (!(aa < bb))
        return;

    ivec2 span = bb - aa;

    screen->AddDirty(pos, pos + span);

    for (int j = 0; j < span.y; j++)
    {
        uint8_t *src = im->scan_line(aa.y + j) + aa.x;
        uint8_t *dst = screen->scan_line(pos.y + j) + pos.x;

        for (int i = 0; i < span.x; i++, src++, dst++)
            if (*src)
                *dst = m_table[*src];
    }
}

ColorFilter::ColorFilter(Palette *pal, int color_bits)
{
    int max = pal->Count();
    int mul = 1 << (8 - color_bits);
    m_size = 1 << color_bits;
    m_table.Resize(m_size * m_size * m_size);

    /* For each colour in the RGB cube, find the nearest palette element. */
    for (int r = 0; r < m_size; r++)
    for (int g = 0; g < m_size; g++)
    for (int b = 0; b < m_size; b++)
    {
        ivec3 rgb = ivec3(r, g, b) * mul;
        int best = 256 * 256 * 3;
        int n = 0;

        for (int i = 0; i < max; i++)
        {
            int dist = sqlength(rgb - (ivec3)pal->GetColor(i));
            if (dist < best)
            {
                best = dist;
                n = i;
            }
        }
        m_table[(r * m_size + g) * m_size + b] = n;
    }
}

ColorFilter::ColorFilter(SpecEntry *e, bFILE *fp)
{
    fp->seek(e->offset, 0);
    m_size = fp->read_uint16();
    m_table.Resize(m_size * m_size * m_size);
    fp->read(m_table.Data(), m_table.Bytes());
}

ColorFilter::~ColorFilter()
{
}

size_t ColorFilter::DiskUsage()
{
    return sizeof(uint16_t) + m_table.Bytes();
}

int ColorFilter::Write(bFILE *fp)
{
    fp->write_uint16(m_size);
    return fp->write(m_table.Data(), m_table.Bytes()) == m_table.Bytes();
}

