#ifndef lltl

#ifdef __sgi
#define BIGUNS
#endif


// these macros swap the "endians" of a word to intel form... this should be done for anything sent
// across the net as the other computer might have a different endianess

#ifdef BIGUNS
#define lstl(x) (((((uint16_t) (x)))<<8)|((((uint16_t) (x)))>>8))
#define lltl(x) \
   ((( ((uint32_t)(x)) )>>24)|((( ((uint32_t)(x)) )&0x00ff0000)>>8)| \
   ((( ((uint32_t)(x)) )&0x0000ff00)<<8)|(( ((uint32_t)(x)) )<<24))
#else
#define lstl(x) (x)
#define lltl(x) (x)

#endif

#endif





