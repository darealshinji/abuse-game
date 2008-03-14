/*
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *%	  Copyright (C) 1989, by WATCOM Systems Inc. All rights     %
 *%	  reserved. No part of this software may be reproduced	    %
 *%	  in any form or by any means - graphic, electronic or	    %
 *%	  mechanical, including photocopying, recording, taping     %
 *%	  or information storage and retrieval systems - except     %
 *%	  with the written permission of WATCOM Systems Inc.	    %
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  WILDARGV - split DOS command line into individual arguments expanding
	     those that contain ? or *.
  This module is a substitute for the "initargv" module contained in the
  library.

  Modified:	By:		Reason:
  ---------	---		-------
  23-aug-89	John Dahms	was ignoring files with Archive or
				read only attributes turned on. (Bug fix)
  15-sep-91	F.W.Crigger	Use _LpCmdLine, _LpPgmName, _argc, _argv,
  				___Argc, ___Argv
  02-nov-93	A.F.Scian	updated so that source will compile as C++
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <direct.h>
#include <malloc.h>

char	*_LpCmdLine;
char	*_LpPgmName;
int	_argc;			/* argument count  */
char  **_argv;			/* argument vector */
int	___Argc;		/* argument count */
char  **___Argv;		/* argument vector */

static void *_allocate( unsigned amount )
{
    void *p;

    p = malloc(amount);
    return(p);
}

int _make_argv(char *p, char ***argv)
{
    int			argc;
    char		*start;
    char		*new_arg;
    char		wildcard;
    char		lastchar;
//    DIR *		dir;
    argc = 1;
    for (;;) {
	    while (*p == ' ')
            ++p;	/* skip over blanks */
        if (*p == '\0')
            break;
	/* we are at the start of a parm */
	    wildcard = 0;
	    if (*p == '\"') {
	        p++;
	        new_arg = start = p;
	        for (;;) {
		/* end of parm: NULLCHAR or quote */
		        if (*p == '\"')
                    break;
		        if (*p == '\0')
                    break;
		        if (*p == '\\') {
		            if (p[1] == '\"'  ||  p[1] == '\\')
                        ++p;
                }
		        *new_arg++ = *p++;
            }
        }
        else {
	        new_arg = start = p;
	        for (;;) {
		/* end of parm: NULLCHAR or blank */
		        if (*p == '\0')
                    break;
		        if (*p == ' ')
                    break;
		        if ((*p == '\\') && (p[1] == '\"')) {
		            ++p;
                }
                else if (*p == '?' || *p == '*') {
		            wildcard = 1;
                }
		        *new_arg++ = *p++;
            }
        }
	    *argv = (char**) realloc(*argv, (argc + 2) * sizeof(char*));
	    (*argv)[argc] = start;
	    ++argc;
	    lastchar = *p;
	    *new_arg = '\0';
	    ++p;
	    if (lastchar == '\0') 
            break;
    }
    return argc;
}

void __Init_Argv()
{
	_argv = (char **) _allocate( 2 * sizeof( char * ) );
	_argv[0] = _LpPgmName;	/* fill in program name */
	_argc = _make_argv( _LpCmdLine, &_argv );
	_argv[_argc] = NULL;
	___Argc = _argc;
	___Argv = _argv;
}
