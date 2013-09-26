/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef _CRC_HPP_
#define _CRC_HPP_

#include "imlib/specs.h"

class Crc
{
public:
    static uint16_t FromData(void const *buf, size_t len);
    static uint32_t FromFile(bFILE *fp);
};

#endif

