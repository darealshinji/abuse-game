/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "chmorph.hpp"

jmorph *normal_to_pacman;
color_filter *color_table;

void load_morphs()
{
  FILE *fp=fopen("n2pac.mph","rb");
  CONDITION(fp,"unable to open morph file n2pac.mph");
  
  spec_directory sd(fp);
  sd.find(" 

}

  







