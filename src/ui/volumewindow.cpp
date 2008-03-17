/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "volumewindow.hpp" // class VolumeWindow
#include "property.hpp"     // class property_manager
#include "gui.hpp"          // ico_button

VolumeWindow::VolumeWindow()
{
    char const *ff = "art/frame.spe";
    u_u = cash.reg(ff, "u_u", SPEC_IMAGE, 1),
    u_d = cash.reg(ff, "u_u", SPEC_IMAGE, 1),
    u_ua = cash.reg(ff, "u_ua", SPEC_IMAGE, 1),
    u_da = cash.reg(ff, "u_da", SPEC_IMAGE, 1),
    d_u = cash.reg(ff, "d_u", SPEC_IMAGE, 1),
    d_d = cash.reg(ff, "d_u", SPEC_IMAGE, 1),
    d_ua = cash.reg(ff, "d_ua", SPEC_IMAGE, 1),
    d_da = cash.reg(ff, "d_da", SPEC_IMAGE, 1),
    slider = cash.reg(ff, "volume_slide", SPEC_IMAGE, 1);
    x = prop->getd("volume_x", xres / 2 - 20);
    y = prop->getd("volume_y", yres / 2 - 50);
    inm->add(new ico_button(10, 27, ID_SFX_DOWN, d_u, d_d, d_ua, d_da,
                  new ico_button(21, 27, ID_SFX_UP, u_u, u_d, u_ua, u_da,
                      new info_field(15, 42, 0, symbol_str("SFXv"),
                          new ico_button(10, 72, ID_MUSIC_DOWN, d_u, d_d, d_ua, d_da,
                              new ico_button(21, 72, ID_MUSIC_UP, u_u, u_d, u_ua, u_da,
                                  new info_field(10, 86, 0, symbol_str("MUSICv"), NULL)))))));
    
    //reconfigure();
    bg = cash.reg(ff, "vcontrol", SPEC_IMAGE, 1);
    l = cash.img(bg)->width();
    h = cash.img(bg)->height();
    screen = new image(l, h, NULL, 2);
    redraw();
}

void VolumeWindow::redraw()
{
    cash.img(bg)->put_image(screen, 0, 0);
    draw_music_vol();
    draw_sfx_vol();
    inm->redraw();
}

void VolumeWindow::draw_vol(int x1, int y1, int x2, int y2, int t,
                            int max, int c1, int c2)
{
    int dx = x1 + t * (x2 - x1) / max;
    if(t != 0)
    {
        cash.img(slider)->put_image(screen, x1, y1);    
//      screen->bar(x1,y1,dx,y2,c1);
    }
    else
        dx--;

    if(dx < x2)
        screen->bar(dx + 1, y1, x2, y2, c2);
}

void VolumeWindow::draw_sfx_vol()
{
    draw_vol(6, 16, 34, 22, sfx_volume, 127,
             pal->find_closest(200, 75, 19), pal->find_closest(40, 0, 0));
}

void VolumeWindow::draw_music_vol()
{
    draw_vol(6, 61, 34, 67, music_volume, 127,
             pal->find_closest(255, 0, 0), pal->find_closest(40, 0, 0));
}

