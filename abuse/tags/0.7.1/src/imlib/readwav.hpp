/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __READ_WAV_HPP_
#define __READ_WAV_HPP_
#include "specs.hpp"
unsigned char *read_wav(char *filename, long &sample_rate, long &data_size);
void write_wav(char *filename, long sample_rate, long data_size, unsigned char *data);


#endif



