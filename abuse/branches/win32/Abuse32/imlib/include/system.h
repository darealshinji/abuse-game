#ifndef __SYS__
#define __SYS__


#ifdef WORDS_BIGENDIAN
  #define BIG_ENDIANS
#else
  #define LITTLE_ENDIANS
#endif

#define short_swap(x) (((((unsigned short) (x)))<<8)|((((unsigned short) (x)))>>8))
#define long_swap(x) \
   ((( ((unsigned long)(x)) )>>24)|((( ((unsigned long)(x)) )&0x00ff0000)>>8)| \
   ((( ((unsigned long)(x)) )&0x0000ff00)<<8)|(( ((unsigned long)(x)) )<<24))

#if defined BIG_ENDIANS
#define LONG int
#define int_to_intel(x) short_swap(x)
#define int_to_local(x) int_to_intel(x)
#define big_long_to_local(x) (x)
#define big_short_to_local(x) (x)
#define long_to_intel(x) long_swap(x)
#define long_to_local(x) long_to_intel(x)
#else
// Removed by Jeremy Scott 6/18/2000
//#define LONG long
// Removed by Jeremy Scott 6/18/2000
#define int_to_intel(x) (x)
#define int_to_local(x) (x)
#define long_to_local(x) (x)
#define long_to_intel(x) (x)
#define big_long_to_local(x) long_swap(x)
#define big_short_to_local(x) short_swap(x)
#endif

#define bltl(x) big_long_to_local(x)
#define bstl(x) big_short_to_local(x)
#define lltl(x) long_to_intel(x)
#define lstl(x) int_to_intel(x)

#endif




