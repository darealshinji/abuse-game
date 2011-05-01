#include "netcfg.hpp"
#include "input.hpp"
#include "cache.hpp"
#include "sock.hpp"
#include "timing.hpp"
#include "light.hpp"

extern char *get_login();
net_configuration *main_net_cfg=NULL;
extern char lsf[256];
char *symbol_str(char *name);

extern net_protocol *prot;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
net_configuration::net_configuration()
{
    strcpy(name, get_login());

    strcpy(server_name, "");
    min_players = 2;
    max_players = 8;
    kills = 25;
    port = 20202;
    server_port = 20202;
    state = SINGLE_PLAYER;
}

extern char game_name[64];

enum { NET_OK = 1, NET_CANCEL, NET_SERVER_NAME, NET_NAME, NET_PORT, NET_SERVER_PORT, NET_MAX, NET_MIN, NET_KILLS, CFG_ERR_OK, NET_SERVER, 
       NET_CLIENT, NET_SINGLE, NET_GAME = 400, 
       MIN_1, MIN_2, MIN_3, MIN_4, MIN_5, MIN_6, MIN_7, MIN_8,
       MAX_2, MAX_3, MAX_4, MAX_5, MAX_6, MAX_7, MAX_8,
       LVL_2, LVL_4, LVL_8, LEVEL_BOX};

const int Min_But_IDs[] = { MIN_1, MIN_2, MIN_3, MIN_4, MIN_5, MIN_6, MIN_7, MIN_8 };
const int Max_But_IDs[] = { MAX_2, MAX_3, MAX_4, MAX_5, MAX_6, MAX_7, MAX_8 };

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void net_configuration::cfg_error(char *msg)
{
    jwindow *j = wm->new_window(-1,0,-1,-1,new info_field(WINDOW_FRAME_LEFT,WINDOW_FRAME_TOP,0,msg,
        new button(WINDOW_FRAME_LEFT,WINDOW_FRAME_TOP+30,CFG_ERR_OK,symbol_str("ok_button"),NULL)),symbol_str("input_error"));
    event ev;
    do {
        wm->flush_screen();
        do {
            wm->get_event(ev);
        } while (ev.type==EV_MOUSE_MOVE && wm->event_waiting()); 
    } while (ev.type!=EV_MESSAGE || ev.message.id!=CFG_ERR_OK || ev.type==EV_CLOSE_WINDOW || (ev.type==EV_KEY && ev.key==JK_ESC));
    wm->close_window(j);
    wm->flush_screen();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int net_configuration::restart_state()
{
    switch (state) {
        case RESTART_SERVER : 
        case RESTART_CLIENT :
        case RESTART_SINGLE :
            return 1;
    }
    return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int net_configuration::notify_reset()
{
    switch (state) {
        case RESTART_SERVER : { state = SERVER; } break;
        case RESTART_CLIENT : { state = CLIENT; } break;
        case RESTART_SINGLE : { state = SINGLE_PLAYER; } break;
    }
    return 1;
}

int get_option(char* name);
extern char** start_argv;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int net_configuration::save()
{
    bFILE *fp;
    char str[256];
    if (game_name[0]) {
        fp = open_file("config/gamename.lsp", "wb");
        if (!fp->open_failure()) {
            sprintf(str, "(setq gamename \"%s\")\n", game_name);
            fp->write(str, strlen(str) + 1);
        }
        delete fp;
        fp = open_file("config/srvcfg.lsp", "wb");
        if (!fp->open_failure()) {
            sprintf(str, "(perm-space)\n"
                         "(setq min_players %d)\n"
                         "(setq max_players %d)\n"
                         "(setq max_kills %d)\n"
                         "(tmp-space)", min_players, max_players, kills);
            fp->write(str, strlen(str) + 1);
        }
        delete fp;
    }

    fp = open_file("config/username.lsp", "wb");
    if (!fp->open_failure()) {
        sprintf(str, "(setq username \"%s\")\n", name);
        fp->write(str, strlen(str) + 1);
    }
    delete fp;
    return 1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int net_configuration::load()
{
    bFILE *fp;
    if (game_name[0]) {
        fp = open_file("config/srvcfg.lsp", "rb");
        if (!fp->open_failure()) {
            char *cs = "(load \"config/srvcfg.lsp\")";
            eval(compile(cs));
        }
        delete fp;
    }
        
    if (DEFINEDP(symbol_value(make_find_symbol("min_players"))))
        min_players = (char) lnumber_value(symbol_value(make_find_symbol("min_players")));
    else
        min_players = 2;
    if (min_players < 1)
        min_players = 1;
    else if (min_players > 8)
        min_players = 8;
        
    if (DEFINEDP(symbol_value(make_find_symbol("max_players"))))
        max_players = (char) lnumber_value(symbol_value(make_find_symbol("max_players")));
    else
        max_players = 8;
    if (max_players < min_players)
        max_players = min_players;
    else if (max_players > 8)
        max_players = 8;
    
    
    if (DEFINEDP(symbol_value(make_find_symbol("max_kills"))))
        kills = (short) lnumber_value(symbol_value(make_find_symbol("max_kills")));
    else
        kills = 10;

    if (kills < 1)
        kills = 1;
    else if (kills > 99)
        kills = 99;

    return 1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int net_configuration::confirm_inputs(input_manager *i, int server)
{
    game_name[0] = 0; // 'flag' to tell if we're a server or not
    if (server) {
        int kl;

        if (sscanf(i->get(NET_KILLS)->read(), "%d", &kl) != 1 || kl < 1 || kl > 99) {
            error(symbol_str("kill_error"));
            return 0;
        }
        kills = kl;

        char *nm = i->get(NET_NAME)->read();
        if (strstr(nm, "\"") || !strlen(nm)) {
            error(symbol_str("name_error"));
            return 0;
        }    
        strcpy(name, nm);

        min_players = ((ifield*) (i->get(NET_MIN)->read()))->id - MIN_1 + 1;
        max_players = ((ifield*) (i->get(NET_MAX)->read()))->id - MAX_2 + 2;
        if (max_players < min_players) {
            error(symbol_str("max_error")); 
            return 0;
        }

        char *s_nm = i->get(NET_SERVER_NAME)->read();
        if (strstr(s_nm, "\"") || !strlen(nm)) {
            error(symbol_str("game_error"));
            return 0;
        }    

        strcpy(game_name, s_nm);
        
        char RootDir[1024];
        int AddonIndex;
        bFILE *fp;
        sprintf(RootDir, "addon/");
        strcpy(lsf, "addon/deathmat/deathmat.lsp");
        if (AddonIndex = get_option("-a")) {
            AddonIndex++;
            char TestLsf[1024];
            sprintf(TestLsf, "addon/%s/netstart.lsp", start_argv[AddonIndex]);
            fp = open_file(TestLsf, "rb");
            if (!fp->open_failure()) {
                sprintf(RootDir, "addon/%s/", start_argv[AddonIndex]);
                strcpy(lsf, TestLsf);
            }
            delete fp;
        }
        
        fp = open_file("config/levelset.lsp", "wb");
        if (!fp->open_failure()) {
            char str[256];
            if (((ifield*) (i->get(LEVEL_BOX)->read()))->id == LVL_2)
                sprintf(str, "(load \"%sdeathmat/small.lsp\")\n", RootDir);
            else if (((ifield*) (i->get(LEVEL_BOX)->read()))->id == LVL_4)
                sprintf(str, "(load \"%sdeathmat/medium.lsp\")\n", RootDir);
            else
                sprintf(str, "(load \"%sdeathmat/large.lsp\")\n", RootDir);
            fp->write(str, strlen(str) + 1);
        }
        delete fp;
    }
    else {
        char *nm = i->get(NET_NAME)->read();
        if (strstr(nm, "\"")) {
            error(symbol_str("name_error"));
            return 0;
        }    
        strcpy(name, nm);
    }
    return 1;
}

extern int start_running, demo_start, start_edit;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void net_configuration::error(char *message)
{
    image *screen_backup=screen->copy();
    
    image *ns=cash.img(cash.reg("art/frame.spe","net_screen",SPEC_IMAGE,1));
    int ns_w=ns->width(),ns_h=ns->height();
    int x=(xres+1)/2-ns_w/2,y=(yres+1)/2-ns_h/2;
    ns->put_image(screen,x,y);
    JCFont *fnt=wm->font();
    
    uchar *remap=white_light+30*256;
    
    uchar *sl=screen->scan_line(0);
    int xx=screen->width()*screen->height();
    for (;xx;xx--,sl++) *sl=remap[*sl];
    
    int fx=x+ns_w/2-strlen(message)*fnt->width()/2,
        fy=y+ns_h/2-fnt->height();
    
    fnt->put_string(screen,fx+1,fy+1,message,wm->black());
    fnt->put_string(screen,fx,fy,message,wm->bright_color());
    
    
    {   
        char *ok=symbol_str("ok_button");
        
        int bx=x+ns_w/2-strlen(ok)*fnt->width()/2-3,
            by=y+ns_h/2+fnt->height()*3;
        
        button *sb=new button(bx,by,NET_SERVER,ok,NULL);
        
        input_manager inm(screen, sb);
        inm.allow_no_selections();
        inm.clear_current();
        
        int done=0;
        event ev;
        do {
            wm->flush_screen();
            do  { wm->get_event(ev); } while (ev.type==EV_MOUSE_MOVE && wm->event_waiting()); 
            inm.handle_event(ev,NULL);
            if ((ev.type==EV_KEY && (ev.key==JK_ESC || ev.key==JK_ENTER)) ||
                ev.type==EV_MESSAGE) done=1;
        } while (!done);
    }
    
    screen_backup->put_image(screen,0,0);
    wm->flush_screen();
    delete screen_backup;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int net_configuration::get_options(int server)
{
    image *ns = cash.img(cash.reg("art/frame.spe", "net_screen", SPEC_IMAGE, 1));
    int ns_w = ns->width(), ns_h = ns->height();
    int x = (xres + 1) / 2 - ns_w / 2,
        y = (yres + 1) / 2 - ns_h / 2;
    ns->put_image(screen, x, y);
    JCFont *fnt = wm->font();
    image *ok_image = cash.img(cash.reg("art/frame.spe", "dev_ok", SPEC_IMAGE, 1))->copy(),
          *cancel_image = cash.img(cash.reg("art/frame.spe", "cancel", SPEC_IMAGE, 1))->copy();

    ifield *list = NULL;

    if (server) {
        load();
        
        list = center_ifield(new text_field(x, y + 30, NET_NAME, symbol_str("your_name"), "************************", name, list), x, x + ns_w, NULL);
        list = center_ifield(new text_field(0, 0, NET_SERVER_NAME, symbol_str("server_name"), "************************", game_name, list), x, x + ns_w, list);
        list = center_ifield(new info_field(0, 0, 0, symbol_str("min_play"), list), x, x + ns_w, list);

        button_box *b=new button_box(0, 0, NET_MIN, 1, NULL, list);
        b->add_button(new button(0, 0, MIN_8, "8", NULL));
        b->add_button(new button(0, 0, MIN_7, "7", NULL));
        b->add_button(new button(0, 0, MIN_6, "6", NULL));
        b->add_button(new button(0, 0, MIN_5, "5", NULL));
        b->add_button(new button(0, 0, MIN_4, "4", NULL));
        b->add_button(new button(0, 0, MIN_3, "3", NULL));
        b->add_button(new button(0, 0, MIN_2, "2", NULL));
        b->add_button(new button(0, 0, MIN_1, "1", NULL));
        b->press_button(Min_But_IDs[min_players - 1]);

        b->arrange_left_right();
        center_ifield(b, x, x + ns_w, list);
        b->arrange_left_right();
        list = b;

        list = center_ifield(new info_field(0, 0, 0, symbol_str("max_play"), list), x, x + ns_w, list);

        b = new button_box(0, 0, NET_MAX, 1, NULL, list);
        b->add_button(new button(0, 0, MAX_8, "8", NULL));
        b->add_button(new button(0, 0, MAX_7, "7", NULL));
        b->add_button(new button(0, 0, MAX_6, "6", NULL));
        b->add_button(new button(0, 0, MAX_5, "5", NULL));
        b->add_button(new button(0, 0, MAX_4, "4", NULL));
        b->add_button(new button(0, 0, MAX_3, "3", NULL));
        b->add_button(new button(0, 0, MAX_2, "2", NULL));
        b->press_button(Max_But_IDs[max_players - 2]);
        
        b->arrange_left_right();
        center_ifield(b, x, x + ns_w, list);
        b->arrange_left_right();    
        list = b;

        list = center_ifield(new info_field(0, 0, 0, symbol_str("level_size"), list), x, x + ns_w, list);

        b = new button_box(0, 0, LEVEL_BOX, 1, NULL, list);
        b->add_button(new button(0, 0, LVL_8, symbol_str("lvl_8"), NULL));
        b->add_button(new button(0, 0, LVL_4, symbol_str("lvl_4"), NULL));
        b->add_button(new button(0, 0, LVL_2, symbol_str("lvl_2"), NULL));
        b->press_button(LVL_2);

        b->arrange_left_right();
        center_ifield(b, x, x + ns_w, list);
        b->arrange_left_right();    
        list = b;

        list = center_ifield(new text_field(0, 0, NET_KILLS, symbol_str("kills_to_win"), "***", kills, list), x, x + ns_w, list);
    }				    
    else {
        list = center_ifield(new text_field(x, y + 80, NET_NAME, symbol_str("your_name"), "************************", name, list), x, x + ns_w, NULL);
    }

    list = new button(x + 80 - 17, y + ns_h - 20 - fnt->height(), NET_OK, ok_image, list);
    list = new button(x + 80 + 17, y + ns_h - 20 - fnt->height(), NET_CANCEL, cancel_image, list);

    int ret = 0;

    input_manager inm(screen, list);
    inm.allow_no_selections();
    inm.clear_current();

    int done = 0;
    event ev;
    while (!done) {
        wm->flush_screen();
        do {
            wm->get_event(ev);
        } while (ev.type == EV_MOUSE_MOVE && wm->event_waiting()); 
        inm.handle_event(ev, NULL);
        if (ev.type == EV_MESSAGE) {
            switch (ev.message.id) {
	            case NET_OK:
                    if (confirm_inputs(&inm, server)) {
                        ret = 1;
                        done = 1;
                        save();
                    }
                    else {
                        ((button*) inm.get(NET_OK))->push();
                        inm.redraw();
                    }
                    break;
	            case NET_CANCEL:
                    done = 1;	    
            }
        }
        if (ev.type == EV_KEY && ev.key == JK_ESC)
            done = 1;
    } // Until done != 0
    delete ok_image;
    delete cancel_image;
    return ret;
}

/* --------------------------------------------------------------------------- 
    // pulls up dialog box and input fileds
 ------------------------------------------------------------------------- /**/
int net_configuration::input()
{
    // Need to fix
    if (!prot->DoConfig())
        return 0;
    
    int ret = 0;
    screen->clear();
  
    image *ns = cash.img(cash.reg("art/frame.spe", "net_screen", SPEC_IMAGE, 1));
    int ns_w = ns->width(), ns_h = ns->height();
    int x = (xres + 1) / 2 - ns_w / 2,
        y = (yres + 1) / 2 - ns_h / 2;
    ns->put_image(screen, x, y);
    char *nw_s = symbol_str("Networking");
    JCFont *fnt = wm->font();

    wm->font()->put_string(screen, x + ns_w / 2 - strlen(nw_s) * fnt->width() / 2, y + 21 / 2 - fnt->height() / 2, nw_s, wm->medium_color());
    {
        char *server_str = symbol_str("server");
        button *sb = new button(x + 40, y + ns_h - 23 -fnt->height(), NET_SERVER, server_str, NULL);

        if (main_net_cfg && (main_net_cfg->state == CLIENT || main_net_cfg->state == SERVER))
            sb = new button(x + 40, y + ns_h - 9 - fnt->height(), NET_SINGLE, symbol_str("single_play"), sb);

        input_manager inm(screen, sb);

        inm.allow_no_selections();
        inm.clear_current();

        event ev;
        int done = 0;
        int button_y = 25,total_games = 0;
        enum {
            MAX_GAMES = 9
        };
        net_address *game_addr[MAX_GAMES + 1];
        int join_game=-1;
        time_marker start,now;

        do {
            if (wm->event_waiting()) {
                do  { 
                    wm->get_event(ev);
                } while (ev.type == EV_MOUSE_MOVE && wm->event_waiting()); 
                inm.handle_event(ev,NULL);
                if (ev.type == EV_MESSAGE) {
                    switch (ev.message.id) {
                        case NET_CANCEL : done = 1; break;
                        case NET_SERVER : done = 1; break;
                        case NET_SINGLE : done = 1; break;
                        default : 
                            if (ev.message.id >= NET_GAME && ev.message.id < NET_GAME + MAX_GAMES) {
                                join_game = ev.message.id - NET_GAME;
                                done = 1;
                            }
                    }
                }
                else if (ev.type == EV_KEY && ev.key == JK_ESC)
                    done = 1;
            }

            wm->flush_screen();
            char name[256];

            now.get_time();
            if (total_games < MAX_GAMES && now.diff_time(&start) > 1.0) {
                net_address *find = prot->find_address(0x9090, name);    // was server_port
                start.get_time();
                if (find) {
                    int bw = strlen(name) * fnt->width();
                    inm.add(new button(x + ns_w / 2 - bw / 2, y + button_y, NET_GAME + total_games, name, NULL));
                    find->set_port(server_port);
                    game_addr[total_games] = find;
			
                    total_games++;
                    button_y += fnt->height() + 10;
                    inm.redraw();
                }
            }
        } while (!done);

        prot->reset_find_list();
        if (join_game >= 0) {
            if (get_options(0)) {
                int still_there = 1;
                game_addr[join_game]->store_string(server_name, sizeof(server_name));
                state = RESTART_CLIENT;
                ret = 1;
			
                prot->reset_find_list();
                int i; 
                for (i = 0; i < total_games; i++) {      // delete all the addresses we found and stored
                    delete game_addr[i];
                    game_addr[i] = NULL;
                }
                total_games = 0;
            }
        }
        else if (ev.type == EV_MESSAGE && ev.message.id == NET_SERVER) {
            if (get_options(1)) {
                state = RESTART_SERVER;
                return 1;
            }
            else 
                return 0;
        }
        else if (ev.type == EV_MESSAGE && ev.message.id == NET_SINGLE) {
            state = RESTART_SINGLE;
            start_running = 0;

            strcpy(lsf, "abuse.lsp"); // Should be unnecessary...
            return 1;      
        }
    }
    return ret;
}

