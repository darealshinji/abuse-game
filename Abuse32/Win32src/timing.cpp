#include <time.h>
#include "timing.hpp"
#include <dos.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "dprint.hpp"
#include <string.h>
#include "macs.hpp"

volatile unsigned long tick_counter, start_tick_counter; // note : if timer handler is installed for more than 497 days then
                                                        // tick_counter will overflow.  ** don't use this in OS code **

void (*install_timer_handler) (void (*fun)()) = NULL;   // function to install timer
void (*uninstall_timer_handler)() = NULL;
unsigned char timer_installed = 0;

static chain_on = 0, chain_counter = 0;
int total_timer_calls = 0;

#define MAX_TIMER_CALLS 0x32

#define TICKS_PER_SEC 1000

struct
{
    int and_call_mask;
    void ( *fun)();  
} timer_calls[MAX_TIMER_CALLS];

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void add_timer_call(void ( *fun)(), int and_call_mask)
{
    if (total_timer_calls >= MAX_TIMER_CALLS) {
        WriteErr(0, "add_timer_call -- Too may timer calls installed");
    }
    timer_calls[total_timer_calls].fun = fun;
    timer_calls[total_timer_calls].and_call_mask = and_call_mask;
    total_timer_calls++;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void remove_timer_call(void ( *fun)())
{
    for (int i = 0; i < total_timer_calls; i++) {
        if (timer_calls[i].fun == fun) {
            for (int j = i; j < total_timer_calls - 1; j++)        
                timer_calls[j] = timer_calls[j + 1];
            total_timer_calls--;
            return;
        }
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void new_timer_int () 
{ 
    tick_counter++;
    for (int i = 0; i < total_timer_calls; i++)
        if ((tick_counter & timer_calls[i].and_call_mask) == 0)
            timer_calls[i].fun();
} 

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void timer_stub()  // this should be called int timer_init has been called and install_timer_handler!=NULL
{

}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void timer_init()
{
    timer_installed = 1;
}

static char dim[12] = {31,  // Jan
		               29,  // Feb
		               31,  // March
		               30,  // Apr
		               31,  // May
		               30,  // June
		               31,  // July
		               30,  // Aug
		               31,  // Sept
		               31,  // October
		               30,  // Nov
		               31}; // Dec

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
static int days_in_month(long month, long year)
{
    if (month == 1 && (((year - 1) % 4) == 0))   // check for leap-year
        return 28;
    else 
        return dim[month];
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void timer_uninit()
{
    timer_installed = 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void time_marker::get_time()
{
    seconds = 0;
    micro_seconds = GetTickCount();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
time_marker::time_marker()
{
    seconds = 0;
    micro_seconds = GetTickCount();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
double time_marker::diff_time(time_marker *other)
{
    return (double) (micro_seconds-other->micro_seconds) / 1000.0;
}