/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef __WATCOMC__
char const *get_username() { return "DOS user"; }
#elif (defined(__APPLE__) && !defined(__MACH__))
char const *get_username() { return "Mac user"; }
#else
  
char const *get_username()
{
  struct passwd *pw;
  char const *name;

  if (!(name = getlogin()))
  {
    if ((pw = getpwuid (getuid())))
      return pw->pw_name;
    else
      return "UNIX user";
  } else return name; 
}

#endif


