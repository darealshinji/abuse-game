#ifndef __EXITPROC_HPP_
#define __EXITPROC_HPP_

#include <stdlib.h>

#ifdef HAVE_ATEXIT
#define exit_proc(x,y) atexit(y)
#elif defined (HAVE_ON_EXIT)
#define exit_proc(x,y) on_exit(x,NULL)
// COMMENTED OUT BY JEREMY SCOTT 6/17/2000
//#else
// #error no atexit?!?
// COMMENTED OUT BY JEREMY SCOTT 6/17/2000
#endif


#endif
