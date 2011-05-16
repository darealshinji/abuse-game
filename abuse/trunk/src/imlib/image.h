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

class dirty_rect : public linked_node
{
public :
    int16_t dx1, dy1, dx2, dy2;
    dirty_rect(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
    {
        dx1 = x1; dy1 = y1; dx2 = x2; dy2 = y2;
        if (x2 < x1 || y2 < y1)
            printf("add incorrect dirty\n");
    }
    virtual int compare(void *n1) { return ((dirty_rect *)n1)->dy1 > dy1; }
};

class image_descriptor
{
private:
    vec2i m_size, m_aa, m_bb;

public:
    uint8_t keep_dirt,
            static_mem; // if set, don't free memory on exit

    linked_list dirties;
    void *extended_descriptor;

    image_descriptor(vec2i size, int keep_dirties = 1, int static_memory = 0);
    int bound_x1(int x1) { return Max(x1, m_aa.x); }
    int bound_y1(int y1) { return Max(y1, m_aa.y); }
    int bound_x2(int x2) { return Min(x2, m_bb.x); }
    int bound_y2(int y2) { return Min(y2, m_bb.y); }
    inline int x1_clip() { return m_aa.x; }
    inline int y1_clip() { return m_aa.y; }
    inline int x2_clip() { return m_bb.x; }
    inline int y2_clip() { return m_bb.y; }
    void ClearDirties();
    void GetClip(vec2i &aa, vec2i &bb)
    {
        aa = m_aa; bb = m_bb;
    }
    void SetClip(vec2i aa, vec2i bb)
    {
        m_aa = Max(aa, vec2i(0));
        m_bb = Min(Max(bb, m_aa + vec2i(1)), m_size);
    }
    void GetClip(int &x1, int &y1, int &x2, int &y2)
    {
        x1 = m_aa.x; y1 = m_aa.y; x2 = m_bb.x; y2 = m_bb.y;
    }
    void SetClip(int x1, int y1, int x2, int y2)
    {
        if(x2 < x1 + 1) x2 = x1 + 1;
        if(y2 < y1 + 1) y2 = y1 + 1;
        m_aa.x = Max(x1, 0); m_aa.y = Max(y1, 0);
        m_bb.x = Min(x2, m_size.x); m_bb.y = Min(y2, m_size.y);
    }
    void ReduceDirties();
    void AddDirty(int x1, int y1, int x2, int y2);
    void delete_dirty(int x1, int y1, int x2, int y2);
    void Resize(vec2i size)
    {
        m_size = size;
        m_aa = vec2i(0);
        m_bb = size;
    }
};

class image : public linked_node
{
private:
    uint8_t *m_data;
    vec2i m_size;
    bool m_locked;

    void MakePage(vec2i size, uint8_t *page_buffer);
    void DeletePage();

public:
    image_descriptor *m_special;

    image(bFILE *fp, spec_entry *e = NULL);
    image(vec2i size, uint8_t *page_buffer = NULL, int create_descriptor = 0);
    ~image();

    void Lock();
    void Unlock();

    uint8_t Pixel(vec2i pos);
    void PutPixel(vec2i pos, uint8_t color);

    inline uint8_t *scan_line(int16_t y)
    {
        return m_data + y * m_size.x;
    }
    image *copy(); // makes a copy of an image
    void clear(int16_t color = -1); // -1 is background color

    vec2i Size() const { return m_size; }

    void scroll(int16_t x1, int16_t y1, int16_t x2, int16_t y2,
                int16_t xd, int16_t yd);
    void PutImage(image *screen, vec2i pos, int transparent = 0);
    void PutPart(image *screen, vec2i pos, vec2i aa, vec2i bb,
                 int transparent = 0);
    image *copy_part_dithered(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void Bar(vec2i p1, vec2i p2, uint8_t color);
    void xor_bar(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
    void WidgetBar(vec2i p1, vec2i p2,
                   uint8_t light, uint8_t med, uint8_t dark);
    void Line(vec2i p1, vec2i p2, uint8_t color);
    void Rectangle(vec2i p1, vec2i p2, uint8_t color);
    void burn_led(int16_t x, int16_t y, int32_t num, int16_t color,
                  int16_t scale = 1);
    void SetClip(vec2i aa, vec2i bb);
    void GetClip(vec2i &aa, vec2i &bb);
    void InClip(vec2i aa, vec2i bb);
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

    void AddDirty(int x1, int y1, int x2, int y2)
    {
        if (m_special) m_special->AddDirty(x1, y1, x2, y2);
    }
    void delete_dirty(int x1, int y1, int x2, int y2)
    {
        if(m_special) m_special->delete_dirty(x1, y1, x2, y2);
    }
    void ClearDirties()
    {
        if (m_special) m_special->ClearDirties();
    }
    void dither(palette *pal); // use a b&w palette!
    void Scale(vec2i size);
    void SetSize(vec2i size, uint8_t *page = NULL);
    void flood_fill(int16_t x, int16_t y, uint8_t color);
    image *create_smooth(int16_t smoothness = 1); // 0 no smoothness
    void unpack_scanline(int16_t line, char bitsperpixel = 1);
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

