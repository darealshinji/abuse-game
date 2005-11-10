#include "packet.hpp"
#include "jmalloc.hpp"
#include <stdlib.h>
#include <string.h>


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
{ jfree(buf); }

packet::packet(int prefix_size) 
{ 
  pre_size=prefix_size;

#ifdef MANAGE_MEM
  int sp=alloc_space;
  alloc_space=ALLOC_SPACE_STATIC;
#endif

  buf_size=1000;
  buf=(uint8_t *)jmalloc(buf_size,"packet buffer");
  reset(); 

#ifdef MANAGE_MEM
  alloc_space=sp;
#endif
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
    buf=(uint8_t *)jrealloc(buf,max,"packet buffer"); 
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



