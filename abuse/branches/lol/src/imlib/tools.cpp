/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include "common.h"

#include "imlib/tools.h"

AToolPicker::AToolPicker(ivec2 pos, int id,
          int show_h, AVisualObject **Icons, int *Ids, int total_ic,
          Palette *icon_palette, Palette *pal)
  : AScrollPicker(pos, id, show_h, 1, 1, 0)
{
    iw = ih = 0;
    icons = Icons;
    ids = Ids;
    total_icons = total_ic;
    for (int i = 0; i < total_ic; i++)
    {
        iw = lol::max(iw, icons[i]->Size().x);
        ih = lol::max(ih, icons[i]->Size().y);
    }
    map = new Filter(icon_palette, pal);
    old_pal = icon_palette->Copy();
    reconfigure();
}

AToolPicker::~AToolPicker()
{
    delete old_pal;
    delete map;
    for (int i = 0; i < total_icons; i++)
        delete icons[i];                   // delete visual object, which should be a "shell"
}

void AToolPicker::remap(Palette *pal, AImage *screen)
{
    delete map;
    map = new Filter(old_pal, pal);
    draw_first(screen);
}

void AToolPicker::DrawItem(AImage *screen, ivec2 pos, int num, int active)
{
    screen->Bar(pos, pos + ivec2(iw - 1, ih - 1),
                active ? wm->bright_color() : wm->black());
    icons[num]->Draw(screen, pos, map);
}

