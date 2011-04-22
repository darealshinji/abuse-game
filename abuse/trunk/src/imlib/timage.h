/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __TIMAGE_HPP__
#define __TIMAGE_HPP__

#include "image.h"
#include "palette.h"
#include "filter.h"

/*  Data is stored in the following format:
 *
 *   uint8_t skip;       // transparent pixel count
 *   uint8_t size;       // solid pixel count
 *   uint8_t data[size]; // solid pixel values
 *   ...
 *   (no scan line wraps allowed, there can be a last skip value)
 */

class trans_image // transparent image
{
public:
    trans_image(image *im, char const *name);
    ~trans_image();

    inline vec2i Size() { return m_size; }
    inline uint8_t *Data() { return m_data; }

    image *ToImage();

    void PutImage(image *screen, int x, int y); // always transparent
    void PutRemap(image *screen, int x, int y, uint8_t *remap);
    void PutDoubleRemap(image *screen, int x, int y,
                        uint8_t *remap, uint8_t *remap2);
    void PutFade(image *screen, int x, int y, int amount, int total_frames,
                 color_filter *f, palette *pal);
    void PutFadeTint(image *screen, int x, int y, int amount, int total_frames,
                     uint8_t *tint, color_filter *f, palette *pal);
    void PutColor(image *screen, int x, int y, uint8_t color);
    void PutFilled(image *screen, int x, int y, uint8_t color);
    void PutPredator(image *screen, int x, int y);
    void PutBlend(image *screen, int x, int y, image *blend, int bx, int by,
                  int blend_amount, color_filter *f, palette *pal);
    void PutScanLine(image *screen, int x, int y, int line);

    size_t MemUsage();

private:
    uint8_t *ClipToLine(image *screen, int x1, int y1, int x2, int y2,
                        int x, int &y, int &ysteps);

    enum PutMode { NORMAL, REMAP, REMAP2, FADE, FADE_TINT, COLOR,
                   FILLED, PREDATOR, BLEND, SCANLINE };
    template<int N>
    void PutImageGeneric(image *dest, int x, int y, uint8_t color,
                         image *blend, int bx, int by,
                         uint8_t *map1, uint8_t *map2, int amount,
                         int total_frames, uint8_t *tint,
                         color_filter *f, palette *pal);

    vec2i m_size;
    uint8_t *m_data;
};

#endif

