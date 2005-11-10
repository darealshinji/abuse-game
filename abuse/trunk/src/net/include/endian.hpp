#ifndef __INDIANS__HPP_
#define __INDIANS__HPP_

#error hi


#ifdef __linux__
#include <asm/byteorder.h>
#endif

#ifdef __sgi
#include <sys/endian.h>
#endif



// these macros swap the "endians" of a word to intel form... this should be done for anything sent
// across the net as the other computer might have a different endianess

#ifdef BIGUNS
#define swap_uint16(x) (((((uint16_t) (x)))<<8)|((((uint16_t) (x)))>>8))
#define swap_uint32(x) \
   ((( ((uint32_t)(x)) )>>24)|((( ((uint32_t)(x)) )&0x00ff0000)>>8)| \
   ((( ((uint32_t)(x)) )&0x0000ff00)<<8)|(( ((uint32_t)(x)) )<<24))
#else
#define swap_uint16(x) (x)
#define swap_uint32(x) (x)

#endif

#endif




