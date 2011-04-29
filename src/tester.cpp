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

#include "items.h"

backtile::backtile(spec_entry *e, FILE *fp)
{
  im=new image(e,fp);
  next=read_short(fp);
}

foretile::foretile(spec_entry *e, FILE *fp)
{
  unsigned char d[2],i;
  im=new image(e,fp);
  next=read_short(fp);
  fread(&damage,1,1,fp);
  points=new point_list(fp);
}

figure::figure(spec_entry *e, FILE *fp)
{
  im=new image(e,fp);
  fread(&hit_damage,1,1,fp);
  fread(&xcfg,1,1,fp);

  touch=new point_list(fp);
  damage=new point_list(fp);
  hit=new point_list(fp);
}

