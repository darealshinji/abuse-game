/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __FONTS_HPP_
#define __FONTS_HPP_

#include "image.h"
#include "transimage.h"

class JCFont
{
public:
    JCFont(image *letters);
    ~JCFont();

    void PutChar(image *screen, vec2i pos, char ch, int color = -1);
    void PutString(image *screen, vec2i pos, char const *st, int color = -1);
    vec2i Size() const { return m_size; }

private:
    vec2i m_size;
    TransImage *m_data[256];
};

#endif

