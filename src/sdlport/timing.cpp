#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "timing.hpp"

#ifdef __APPLE__
// OSX 10.1 has nanosleep but no header for it!
extern "C" {
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);
}
#endif

// Constructor
//
time_marker::time_marker()
{
	get_time();
}

//
// get_time()
// Get the current time
//
void time_marker::get_time()
{
	struct timeval tv = { 0, 0 };
	gettimeofday( &tv, NULL );
	seconds = tv.tv_sec;
	micro_seconds = tv.tv_usec;
}

//
// diff_time()
// Find the time difference
//
double time_marker::diff_time( time_marker *other )
{
	return (double)(seconds - other->seconds) + (double)(micro_seconds - other->micro_seconds) / 1000000;
}

void timer_init()
{
	/* Do Nothing */
}

void timer_uninit()
{
	/* Do Nothing */
}

void milli_wait( unsigned wait_time )
{
	struct timespec ts = { 0, wait_time * 1000000 };
	nanosleep( &ts, NULL );
}

