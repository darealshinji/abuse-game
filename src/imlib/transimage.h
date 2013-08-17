/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
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

class TransImage
{
public:
    TransImage(AImage *im, char const *name);
    ~TransImage();

    inline ivec2 Size() { return m_size; }
    inline uint8_t *Data() { return m_data; }

    AImage *ToImage();

    void PutImage(AImage *screen, ivec2 pos);
    void PutRemap(AImage *screen, ivec2 pos, uint8_t *map);
    void PutDoubleRemap(AImage *screen, ivec2 pos, uint8_t *map, uint8_t *map2);
    void PutFade(AImage *screen, ivec2 pos, int amount, int nframes,
                 ColorFilter *f, Palette *pal);
    void PutFadeTint(AImage *screen, ivec2 pos, int amount, int nframes,
                     uint8_t *tint, ColorFilter *f, Palette *pal);
    void PutColor(AImage *screen, ivec2 pos, uint8_t color);
    void PutFilled(AImage *screen, ivec2 pos, uint8_t color);
    void PutPredator(AImage *screen, ivec2 pos);
    void PutBlend(AImage *screen, ivec2 pos, AImage *blend, ivec2 bpos,
                  int blend_amount, ColorFilter *f, Palette *pal);
    void PutScanLine(AImage *screen, ivec2 pos, int line);

    size_t DiskUsage();

private:
    uint8_t *ClipToLine(AImage *screen, ivec2 pos1, ivec2 pos2,
                        ivec2 &posy, int &ysteps);

    enum PutMode { NORMAL, REMAP, REMAP2, FADE, FADE_TINT, COLOR,
                   FILLED, PREDATOR, BLEND, SCANLINE };
    template<int N>
    void PutImageGeneric(AImage *dest, ivec2 pos, uint8_t color,
                         AImage *blend, ivec2 bpos,
                         uint8_t *map1, uint8_t *map2, int amount,
                         int nframes, uint8_t *tint,
                         ColorFilter *f, Palette *pal);

    ivec2 m_size;
    uint8_t *m_data;
};

#endif

