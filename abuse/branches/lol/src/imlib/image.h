/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include <stdlib.h>
#include "linked.h"
#include "palette.h"
#include "specs.h"
#define MAX_DIRTY 200

void image_init();
void image_uninit();
extern linked_list image_list;

class ADirtyRect
{
public :
    inline ADirtyRect() {}

    ADirtyRect(ivec2 aa, ivec2 bb)
    {
        m_aa = aa;
        m_bb = bb;
        if (!(bb >= aa))
            printf("add incorrect dirty\n");
    }
    virtual int compare(void *n1)
    {
        return ((ADirtyRect *)n1)->m_aa.y > m_aa.y;
    }

    ivec2 m_aa, m_bb;
};

class image_descriptor
{
    friend class AImage;

public:
    uint8_t keep_dirt;

    void *extended_descriptor;

    image_descriptor(ivec2 size, int keep_dirties = 1);
    int bound_x1(int x1) { return lol::max(x1, m_aa.x); }
    int bound_y1(int y1) { return lol::max(y1, m_aa.y); }
    int bound_x2(int x2) { return lol::min(x2, m_bb.x); }
    int bound_y2(int y2) { return lol::min(y2, m_bb.y); }
    inline int x1_clip() { return m_aa.x; }
    inline int y1_clip() { return m_aa.y; }
    inline int x2_clip() { return m_bb.x; }
    inline int y2_clip() { return m_bb.y; }
    void GetClip(ivec2 &aa, ivec2 &bb)
    {
        aa = m_aa; bb = m_bb;
    }
    void SetClip(ivec2 aa, ivec2 bb)
    {
        m_aa = lol::max(aa, ivec2(0));
        m_bb = lol::min(lol::max(bb, m_aa + ivec2(1)), m_size);
    }
    void GetClip(int &x1, int &y1, int &x2, int &y2)
    {
        x1 = m_aa.x; y1 = m_aa.y; x2 = m_bb.x; y2 = m_bb.y;
    }
    void SetClip(int x1, int y1, int x2, int y2)
    {
        if(x2 < x1 + 1) x2 = x1 + 1;
        if(y2 < y1 + 1) y2 = y1 + 1;
        m_aa.x = lol::max(x1, 0); m_aa.y = lol::max(y1, 0);
        m_bb.x = lol::min(x2, m_size.x); m_bb.y = lol::min(y2, m_size.y);
    }
    void ReduceDirties();
    void AddDirty(ivec2 aa, ivec2 bb);
    void DeleteDirty(ivec2 aa, ivec2 bb);
    void Resize(ivec2 size)
    {
        m_size = size;
        m_aa = ivec2(0);
        m_bb = size;
    }

    Array<ADirtyRect> m_dirties; /* Is private because of update_dirties() */

private:
    ivec2 m_size, m_aa, m_bb;
};

class AImage : public linked_node
{
public:
    AImage(bFILE *fp, SpecEntry *e = NULL);
    AImage(ivec2 size, int create_descriptor = 0);
    ~AImage();

    uint8_t Pixel(ivec2 pos);
    void PutPixel(ivec2 pos, uint8_t color);

    ivec2 Size() const { return m_size; }

private:
    Array<uint8_t> m_data;
    ivec2 m_size;

public:
    image_descriptor *m_special;

    inline uint8_t *scan_line(int y)
    {
        /* FIXME: use the following construct for runtime checks */
        //return &m_data[y * m_size.x];

        return m_data.Data() + y * m_size.x;
    }
    AImage *copy(); // makes a copy of an image
    void clear(int color = -1); // -1 is background color

    void PutImage(AImage *screen, ivec2 pos, int transparent = 0);
    void PutPart(AImage *screen, ivec2 pos, ivec2 aa, ivec2 bb,
                 int transparent = 0);
    AImage *copy_part_dithered(int x1, int y1, int x2, int y2);
    void Bar(ivec2 p1, ivec2 p2, uint8_t color);
    void xor_bar(int x1, int y1, int x2, int y2, uint8_t color);
    void WidgetBar(ivec2 p1, ivec2 p2,
                   uint8_t light, uint8_t med, uint8_t dark);
    void Line(ivec2 p1, ivec2 p2, uint8_t color);
    void Rectangle(ivec2 p1, ivec2 p2, uint8_t color);
    void burn_led(int x, int y, int32_t num, int color, int scale = 1);
    void SetClip(ivec2 aa, ivec2 bb);
    void GetClip(ivec2 &aa, ivec2 &bb);
    void InClip(ivec2 aa, ivec2 bb);
    void SetClip(int x1, int y1, int x2, int y2);
    void GetClip(int &x1, int &y1, int &x2, int &y2);
    void InClip(int x1, int y1, int x2, int y2);

    void dirt_off()
    {
        if(m_special && m_special->keep_dirt) m_special->keep_dirt = 0;
    }
    void dirt_on()
    {
        if(m_special) m_special->keep_dirt = 1;
    }

    void AddDirty(ivec2 aa, ivec2 bb)
    {
        if (m_special) m_special->AddDirty(aa, bb);
    }
    void DeleteDirty(ivec2 aa, ivec2 bb)
    {
        if (m_special) m_special->DeleteDirty(aa, bb);
    }
    void dither(Palette *pal); // use a b&w palette!
    void Scale(ivec2 size);
    void SetSize(ivec2 size);
    void flood_fill(int x, int y, uint8_t color);
    AImage *create_smooth(int smoothness = 1); // 0 no smoothness
    void unpack_scanline(int line, char bitsperpixel = 1);
    void FlipX();
    void FlipY();
};

class image_controller
{
public:
    image_controller()
    {
        image_init();
    }
    ~image_controller()
    {
        image_uninit();
    }
};

#endif /* _IMAGE_HPP_ */

