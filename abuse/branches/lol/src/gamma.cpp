/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#if HAVE_CONFIG_H
#   include "config.h"
#endif

#include <math.h>

#include "common.h"

#include "lisp/lisp.h"

#include "imlib/window.h"
#include "imlib/scroller.h"
#include "imlib/dprint.h"

#include "game.h"
#include "id.h"
#include "cache.h"
#include "loader2.h"

extern int dev_ok;
Palette *old_pal = NULL;

class AGrayPicker : public AScrollPicker
{
public:
    AGrayPicker(ivec2 pos, int id, int start, int current)
      : AScrollPicker(pos, id, 1, 20, 0, 0)
    {
        cur_sel = current;
        sc = start;
        reconfigure();
        cur_sel = current;
    }

    int sc;
    virtual void DrawItem(AImage *screen, ivec2 pos, int num, int active)
    {
        ivec2 corner = pos + ivec2(item_width() - 1, item_height() - 1);
        screen->Bar(pos, corner, 0);
        screen->Bar(pos, corner - ivec2(3, 0), sc + num);
        if(active)
        {
            screen->Rectangle(pos, corner, 255);
        }
    }
    void set_pos(int x) { cur_sel = x; }
    virtual int total() { return 32; }
    virtual int item_width() { return 12; }
    virtual int item_height() { return 20; }
    virtual int activate_on_mouse_move() { return 0; }
};

static char const *lang_string(char const *symbol)
{
    LSymbol *v = LSymbol::Find(symbol);
    if (!v || !DEFINEDP(v->GetValue()))
        return "Language symbol missing!";
    return lstring_value(v->GetValue());
}

void gamma_correct(Palette *&pal, int force_menu)
{
    long dg=0,old_dg=0;
    int abort=0;

    // see if user has already done this routine
    LSymbol *gs = LSymbol::Find("darkest_gray");

    if(old_pal)
    {
        delete pal;
        pal = old_pal;
        old_pal = nullptr;
    }

    if(gs && DEFINEDP(gs->GetValue()) && !force_menu)
    {
        dg = lnumber_value(gs->GetValue());
    }
    else
    {
        if(gs && DEFINEDP(gs->GetValue()))
        {
            dg = old_dg = lnumber_value(gs->GetValue());
        }
        // load in a fine gray palette they can chose from
        Palette *gray_pal = pal->Copy();
        int tc = 32;

        for(int i = 0; i < tc; i++)
            gray_pal->SetColor(i, u8vec3(i * 4));

        gray_pal->Load();

        int wm_bc = wm->bright_color(), wm_mc = wm->medium_color(), wm_dc = wm->dark_color();

        u8vec3 br = (u8vec3)min((ivec3)pal->GetColor(wm_bc), ivec3(255 - 20)) + u8vec3(20);
        u8vec3 md = (u8vec3)max((ivec3)pal->GetColor(wm_mc), ivec3(20)) - u8vec3(20);
        u8vec3 dr = (u8vec3)max((ivec3)pal->GetColor(wm_dc), ivec3(40)) - u8vec3(40);

        wm->set_colors(gray_pal->FindClosest(br),
                       gray_pal->FindClosest(md),
                       gray_pal->FindClosest(dr));

        int sh = wm->font()->Size().y + 35;

        AWidgetList widgets;
        widgets << new AButton(ivec2(5, 5 + sh * 3), ID_GAMMA_OK, cache.img(ok_button));
        widgets << new AInfoField(ivec2(35, 10 + sh * 3), ID_NULL, lang_string("gamma_msg"));

        AGrayPicker *gp = new AGrayPicker(ivec2(2, 5 + sh), ID_GREEN_PICKER, 0, dg / 4);
        gp->set_pos(dg / 4);
        widgets << gp;

        AWindow *gw = wm->CreateWindow(ivec2(xres / 2 - 190,
                                             yres / 2 - 90), ivec2(-1), "", widgets);

        Event ev;
        wm->flush_screen();
        do
        {
            do
            {
                wm->get_event(ev);
            } while(ev.type == EV_MOUSE_MOVE && wm->IsPending());
            wm->flush_screen();
            if(ev.type == EV_CLOSE_WINDOW)
                abort = 1;
            if(ev.type == EV_KEY && ev.key == JK_ESC)
                abort = 1;
        } while(!abort && (ev.type != EV_MESSAGE || ev.message.id != ID_GAMMA_OK));

        dg = ((AScrollPicker *)gw->inm->get(ID_GREEN_PICKER))->first_selected() * 4;

        wm->close_window(gw);
        wm->flush_screen();

        wm->set_colors(wm_bc, wm_mc, wm_dc);
        delete gray_pal;

        if(!abort)
        {
            char *gammapath;
            FILE *fp;

            gammapath = (char *)malloc(strlen(get_save_filename_prefix()) + 10);
            sprintf(gammapath, "%sgamma.lsp", get_save_filename_prefix());
            fp = open_FILE(gammapath, "wb");
            if(fp)
            {
                fprintf(fp, "(setq darkest_gray %ld)\n", dg);
                fclose(fp);
                LSpace *sp = LSpace::Current;
                LSpace::Current = &LSpace::Perm;
                LSymbol::FindOrCreate("darkest_gray")->SetNumber(dg);

                LSpace::Current = sp;
            }
            else
            {
                dprintf("Unable to write to file gamma.lsp\n");
            }
            free(gammapath);
        }
    }

    if(abort)
        dg = old_dg;

    if(dg < 1)
        dg = 1;
    else if(dg > 128)
        dg = 128;

    double gamma = log(dg / 255.0) / log(16.0 / 255.0);

    old_pal = pal;
    pal = new Palette;
    for(int i = 0; i < 256; i++)
    {
        u8vec3 old = old_pal->GetColor(i);
        pal->SetColor(i, u8vec3(lol::pow(old.r / 255.0, gamma) * 255,
                                lol::pow(old.g / 255.0, gamma) * 255,
                                lol::pow(old.b / 255.0, gamma) * 255));
    }

    pal->Load();
}
