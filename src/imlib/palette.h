/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef _PALETTE_H_
#define _PALETTE_H_

#include "linked.h"
#include "specs.h"

class Palette : public linked_node
{
public:
    Palette(int number_colors=256);
    Palette(SpecEntry *e, bFILE *fp);
    Palette(bFILE *fp);

    /* FIXME: this should actually be "Active()" or "SetActive()" */
    void Load();
    static Palette *LastLoaded();

    Palette *Copy() const;

    int Count() const { return m_colors.Count(); }
    u8vec3 *Data() { return m_colors.Data(); }
    u8vec3 const *Data() const { return m_colors.Data(); }

    u8vec3 GetColor(int x) const;
    uint8_t GetGray(int x) const;
    void SetColor(int x, u8vec3 color);

    int FindColor(u8vec3 color) const;
    int FindClosest(u8vec3 color) const;
    int FindBrightest(int all = 0) const;
    int FindDarkest(int all = 0, int noblack = 1) const;

    void FadeTo(int total_fades, int fade_on, u8vec3 dest);

private:
    Array<u8vec3> m_colors;
    Array<uint8_t> m_used;
    int bg;

public :
    uint32_t getquad(int x);
    void shift(int amount);

    void defaults();
    void set_rgbs();
    void make_black_white();
    void black_white();

    int used(int color_num);
    void set_all_used();
    void set_all_unused();
    int write(bFILE *fp);
    int size();
};

#endif

