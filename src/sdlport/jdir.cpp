/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 2001 Anthony Kruize <trandor@labyrinth.net.au>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 */

#include "config.h"

#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jmalloc.hpp"

void get_directory(char *path, char **&files, int &tfiles, char **&dirs, int &tdirs)
{
    struct dirent *de;
    files = NULL;
    dirs = NULL;
    tfiles = 0;
    tdirs = 0;
    DIR *d = opendir( path );

    if( !d )
        return;

    char **tlist = NULL;
    int t = 0;
    char curdir[200];
    getcwd( curdir, 200 );
    chdir( path );

    do
    {
        de = readdir( d );
        if( de )
        {
            t++;
            tlist = (char **)jrealloc(tlist,sizeof(char *)*t,"tmp file list");
            tlist[t-1] = strcpy((char *)jmalloc(strlen(de->d_name)+1,"tmp file name"),de->d_name);
        }
    } while( de );
    closedir( d );

    for( int i=0; i < t; i++ )
    {
        d = opendir( tlist[i] );
        if( d )
        {
            tdirs++;
            dirs = (char **)jrealloc(dirs,sizeof(char *)*tdirs,"dir list");
            dirs[tdirs-1] = strcpy((char *)jmalloc(strlen(tlist[i])+1,"tmp file name"),tlist[i]);
            closedir( d );
        }
        else
        {
            tfiles++;
            files = (char **)jrealloc(files,sizeof(char *)*tfiles,"dir list");
            files[tfiles-1] = strcpy((char *)jmalloc(strlen(tlist[i])+1,"tmp file name"),tlist[i]);
        }
        jfree( tlist[i] );
    }
    if( t )
        jfree( tlist );
    chdir( curdir );
}
