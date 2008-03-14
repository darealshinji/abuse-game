#ifndef __SYS__
#define __SYS__


#ifdef WORDS_BIGENDIAN
  #define BIG_ENDIANS
#else
  #define LITTLE_ENDIANS
#endif


#if defined( __WATCOMC__ )
  // they didn't include this def in their math include
  #ifndef M_PI
    #define M_PI 3.141592654
  #endif
  // so stuff can find proper file ops
  #include <io.h>
#elif defined( __POWERPC__ )
  // they didn't include this def in their math include
  #ifndef M_PI
    #define M_PI 3.141592654
  #endif
	#include <unistd.h>
#else
  // so apps can find unlink
  #include <unistd.h>
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
#define LONG long
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




