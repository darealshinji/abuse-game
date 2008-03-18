/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 2001 Anthony Kruize <trandor@labyrinth.net.au>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 */

#include "config.h"

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

