#include "jwindow.hpp"
#include "image.hpp"
#include "specs.hpp"
#include "fonts.hpp"
#include "input.hpp"

int main(int argc, char **argv)
{
    image_init();
    set_mode(19,argc,argv);
    event *eh;
    palette p;
    p.defaults();
    p.load();
    spec_directory spe("letters.spe");
    FILE *fp = fopen("letters.spe", "rb");
    image *fontim = new image(spe.find("letters"), fp);
    fclose(fp);
    fontim->make_color(p.find_closest(255,255,255));
    JCFont fnt(fontim);
    
    window_manager *wm=new window_manager(screen,&p,
        p.find_closest(255,0,0),
        p.find_closest(128,0,0),
        p.find_closest(64,0,0),&fnt);
    event ev;
    int i; 
    for (i=0;i<5;i++)
        wm->new_window(50+i*5,50+i*5,40,40,NULL);
    
    char name[]="Jonathan          ";
    
    jwindow *j=wm->new_window(10,10,200,80,
        new button(20,20,1001,"Push Me!",
        new button(20,40,1002,"Don't push",
        new text_field(20,60,1004,"Name","**************",name,NULL)))
        );
    
    do
    {
        wm->flush_screen();
        wm->get_event(ev);
        if (ev.type==EV_MESSAGE && ev.message.id==1001)
            if ( *((int *)ev.message.data)==0)
                wm->new_window(10,10,100,50,new button(20,20,1005,"Ok!",NULL));
            
    } while (ev.type!=EV_KEY || ev.key!='q');
    delete wm; 
    close_graphics();
    image_uninit();
    return 1;
}