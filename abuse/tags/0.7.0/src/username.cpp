#ifdef __WATCOMC__
char *get_username() { return "DOS user"; }
#elif (defined(__APPLE__) && !defined(__MACH__))
char *get_username() { return "Mac user"; }
#else
  
#include	<stdio.h>
#include	<pwd.h>
#include	<sys/types.h>
#include        <unistd.h>

char *get_username()
{
  struct passwd		*pw;
  char			*name;

  if (!(name = getlogin()))
  {
    if ((pw = getpwuid (getuid())))
      return pw->pw_name;
    else
      return "UNIX user";
  } else return name; 
}

#endif


