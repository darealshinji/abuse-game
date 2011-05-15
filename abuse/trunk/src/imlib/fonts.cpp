/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include <ctype.h>

#include "common.h"

#include "fonts.h"

texture_font::texture_font(image *letters, image *font_pattern)
{ fntpat=font_pattern;
  let=letters;
  tl=(let->Size().x+1)/32;
  th=(let->Size().y+1)/8;
}

void texture_font::put_char(image *screen,  int x, int y, char ch)
{ if (fntpat)
    screen->PutPartMasked(fntpat, vec2i(x, y), let,
       vec2i(((int)ch%32)*tl,((int)ch/32)*th), vec2i(0,0), vec2i(tl,th));
  else screen->PutPart(let, vec2i(x, y), vec2i(((int)ch%32)*tl, ((int)ch/32)*th),
     vec2i(((int)ch%32)*tl+tl, ((int)ch/32)*th+th), 1);
}

void texture_font::put_string(image *screen, int x, int y, char const *st)
{ while (*st)
  { put_char(screen,x,y,*st);
    st++;
    x+=tl;
  }
}


void JCFont::put_string(image *screen, int x, int y, char const *st, int color)
{ while (*st)
  { put_char(screen,x,y,*st,color);
    st++;
    x+=tl;
  }
}


void JCFont::put_char(image *screen,  int x, int y, char ch, int color)
{
  if (let[(int)ch])
  {
    if (color>=0)
      let[(int)ch]->PutColor(screen,vec2i(x,y),color);
    else let[(int)ch]->PutImage(screen,vec2i(x,y));
  }
}

JCFont::JCFont(image *letters)
{
  tl=(letters->Size().x+1)/32;
  th=(letters->Size().y+1)/8;

  image tmp(vec2i(tl,th));

  int ch;

  for (ch=0; ch<256; ch++)
  {
    tmp.clear();
    tmp.PutPart(letters, vec2i(0, 0), vec2i(((int)ch%32)*tl, ((int)ch/32)*th),
                vec2i(((int)ch%32)*tl+tl, ((int)ch/32)*th+th), 1);
    let[ch] = new TransImage(&tmp, "JCfont");
  }
}

JCFont::~JCFont()
{
  int ch;
  for (ch=0; ch<256; ch++)
    delete let[ch];
}

