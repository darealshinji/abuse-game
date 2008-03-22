/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>

#include "packet.hpp"

int packet::advance(int32_t offset)
{
  ro+=offset;
  return ro<=rend;
}

void packet::write_uint32(uint32_t x)
{
  x=lltl(x);
  write((uint8_t *)&x,4);
}

void packet::write_uint16(uint16_t x)
{
  x=lstl(x);
  write((uint8_t *)&x,2);
}

void packet::write_uint8(uint8_t x)
{
  write(&x,1);
}

packet::~packet()
{ free(buf); }

packet::packet(int prefix_size)
{
  pre_size=prefix_size;

  buf_size=1000;
  buf=(uint8_t *)malloc(buf_size);
  reset();
}

void packet::get_string(char *st, int len)
{
  char *b=(char *)buf+ro;
  while (len>1 && !eop() && *b)
  {
    *st=*b;
    st++;
    ro++;
    b++;
    len--;
  }
  if (*b==0) ro++;
  *st=0;
}

void packet::reset()
{ ro=wo=rend=pre_size; }

void packet::make_bigger(int max)
{
  if (buf_size<max)
  {
    buf_size=max;
    buf=(uint8_t *)realloc(buf,max);
  }
}

int packet::read(uint8_t *buffer, int size)
{
  if (size>rend-ro)
    size=rend-ro;

  if (size>0)
  {
    memcpy(buffer,buf+ro,size);
    ro+=size;
    return size;
  } else return 0;
}


int packet::write(uint8_t *buffer, int size)
{
  if (size>buf_size-wo)
    make_bigger(wo+size);
  if (size>0)
  {
    memcpy(buf+wo,buffer,size);
    wo+=size;
    rend=wo;
    return size;
  }
  return 0;
}



void packet::insert_into(packet &pk)
{
  pk.write(buf+pre_size,rend-pre_size);
}



