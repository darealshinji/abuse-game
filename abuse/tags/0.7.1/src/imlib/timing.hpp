/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __TIMING_HPP_
#define __TIMING_HPP_

class time_marker
{
public :
  long seconds;
  long micro_seconds;  
  void get_time();
  time_marker();  
  double diff_time(time_marker *other);    // returns time diff in seconds                   
} ;

void milli_wait(unsigned wait_time);  // sleeps for a milli second 1000 millies=1 sec
void timer_init();
void timer_uninit();

#endif

