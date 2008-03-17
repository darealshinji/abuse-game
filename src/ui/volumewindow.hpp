/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "dev.hpp"

class VolumeWindow : public jwindow
{
private:
    int slider, u_u, u_d, u_ua, u_da, d_u, d_d, d_ua, d_da;
    int bg;

public:
    VolumeWindow();

    virtual void redraw();

    void draw_music_vol();
    void draw_sfx_vol();
    void draw_vol(int x1, int y1, int x2, int y2, int t,
                  int max, int c1, int c2);
};

