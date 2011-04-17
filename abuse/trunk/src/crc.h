/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef _CRC_HPP_
#define _CRC_HPP_
#include "specs.h"
#include "macs.h"

uint16_t calc_crc(uint8_t *buf, int len);
uint32_t crc_file(bFILE *fp);


#endif
