/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include <ctype.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#define make_dir(dir) mkdir(dir,S_IRWXU | S_IRWXG | S_IRWXO)

#include "lisp.hpp"
#include "lisp_gc.hpp"

int change_dir(char *path)
{
    int ret = chdir(path); // weird
    ret = chdir(path);
    return ret == 0;
}

long K_avail(char *path);

int nice_copy(char *title, char *source, char *dest) { return 0; }

long K_avail(char *path)
{
#if 0 // ugh
  char cmd[100];
  sprintf(cmd,"du %s",path);
  FILE *fp=popen(cmd,"rb");
  if (!fp)
  {
    pclose(fp);
    return 20000;
  }
  else
  {
    fgets(cmd,100,fp);
    if (feof(fp))
    {
      pclose(fp);
      return 20000;
    }
    fgets(cmd,100,fp);
    char *s=cmd+strlen(cmd)-1;
    while (*s==' ' || *s=='\t') s--;
    while (*s!=' ' && *s!='\t') s--;  // skip last field

    while (*s==' ' || *s=='\t') s--;
    while (*s!=' ' && *s!='\t') s--;  // skip last field

    while (*s==' ' || *s=='\t') s--;
    while (*s!=' ' && *s!='\t') s--; s++;  // skip last field

    long a;
    sscanf(s,"%d",&a);
    
    pclose(fp);
    return a;
  }
#endif
	return 0;
}

void *show_yes_no(void *t, void *msg, void *y, void *n)
{
  p_ref r1(t),r2(msg),r3(y),r4(n);
  t=eval(t); msg=eval(msg); y=eval(y); n=eval(n);
  int c;
  char const *yes = lstring_value(y);
  char const *no = lstring_value(n);
  do
  {
    printf("\n\n\n\n\n%s\n\n",lstring_value(t));
    void *v=msg;
    if (item_type(v)==L_CONS_CELL)
      while (v) { printf("** %s\n",lstring_value(CAR(v))); v=CDR(v); }
    else printf("** %s\n",lstring_value(v));
    char msg[100];
    fgets(msg,100,stdin);
    c=msg[0];
  } while (toupper(c)!=toupper(yes[0]) && toupper(c)!=toupper(no[0]));
  if (toupper(c)==toupper(yes[0]))
    return true_symbol;
  else return NULL;
}

void *nice_menu(void *main_title, void *menu_title, void *list)
{
  p_ref r1(main_title),r2(menu_title),r3(list);
  main_title=eval(main_title);  menu_title=eval(menu_title);  list=eval(list);
  char msg[100];
  int i=0,d;
  do
  {
    printf("\n\n\n\n\n%s\n\n%s\n-----------------------------------\n",
	   lstring_value(main_title),lstring_value(menu_title));

    void *v=list;
    for (;v;v=CDR(v),i++)
    {
      printf("%d) %s\n",i+1,lstring_value(CAR(v)));
    }
    fprintf(stderr,"> ");
    fgets(msg,100,stdin);
    sscanf(msg,"%d",&d);
  } while (d-1>=i && d<=0);
  return new_lisp_number(d-1);
}

void *nice_input(char *t, char *p, char *d)
{
	unsigned int x;
	for( x = 0; x < ( 40 - strlen( t ) / 2 ); x++ ) printf(" ");
	printf("%s\n",t);
	for( x = 0; x < 78; x++ ) printf("-"); printf("\n");
	fprintf( stderr, "%s (ENTER=%s) > ", p, d );
	char ln[100];
	fgets( ln, 100, stdin ); ln[strlen(ln) - 1] = 0;
	if( ln[0] == 0 )
	{
		strcpy(ln,d);                  // d might get collect in next new
		return new_lisp_string(ln);
	}
	else
		return new_lisp_string(ln);
}

