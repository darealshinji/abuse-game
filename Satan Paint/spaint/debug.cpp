#include "Include/debug.h"


#ifdef __linux__
#include <malloc.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
long mem_avail() 
{
    return 0;
}
#else
#ifdef __WATCOMC__
#include <malloc.h>
long mem_avail() 
{
    return _memmax();    
}
#else
long mem_avial() { return 0; } 
#endif
#endif

jwindow *memory_window;
window_manager *dwm;
int mem_tick=0;


void debug_init(window_manager *wm)
{
    memory_window=wm->new_window(0,-1,280,25);  
    dwm=wm;  
    memory_window=NULL;  
}


void debug_handle_event(event &ev)
{
#ifdef __linux__
    if (memory_window && ev.window==memory_window)
    {
        if (ev.type==EV_CLOSE_WINDOW)
        {      
            dwm->close_window(memory_window);
            memory_window=NULL;
        }
        else 
        {
            char mem[100];            
            sprintf(mem,"mem avail : %d",mem_tick++,mem_avail());      
            memory_window->screen->bar(memory_window->x1(),memory_window->y1(),memory_window->x2(),
                memory_window->y2(),0);      
            dwm->font()->put_string(memory_window->screen,5,12,mem);           
        }
        
    }
#endif  
}