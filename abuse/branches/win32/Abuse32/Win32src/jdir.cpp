/*

 Watcom C,  filename expansion stuff, by JC

  
    ported to Win32 by Jeremy Scott;  February 2001
*/

#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>
#include <dos.h>
#include <string.h>
#include "jmalloc.hpp"
#include "windows.h"

int iswild(char *name)
{
    while (*name && !(*name == '*' || *name == '?'))
        name++;
    return *name;
}

char *expand_filename(char *name)
{  
    char *ret = NULL;
    char drv2[_MAX_DRIVE], drive[_MAX_DRIVE];
    char dir2[_MAX_DIR], directory[_MAX_DIR];
    char fname[_MAX_FNAME];
    char extin[_MAX_FNAME];
    char pathin[_MAX_PATH];
    
    long ret_length = 0;
    if (iswild(name)) {
        
        WIN32_FIND_DATA FindData;
        ZeroMemory(&FindData, sizeof(FindData));
        FindData.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
        HANDLE FoundDir = FindFirstFile(name, &FindData);
        FindClose(FoundDir);
        if (FoundDir != INVALID_HANDLE_VALUE)
            return name;
        else {
            _splitpath(name, drive, directory, fname, extin);
            ZeroMemory(&FindData, sizeof(FindData));
            HANDLE FoundFile;
            while ((FoundFile = FindFirstFile(name, &FindData)) != NULL) {
                _splitpath(FindData.cFileName, drv2, dir2, fname, extin);
                _makepath(pathin, drive, directory, fname, extin);
                
                if (ret_length) {
                    ret=(char *)jrealloc(ret,ret_length+strlen(pathin)+2,"expanded filename");
                    strcat(ret," ");
                    strcat(ret,pathin);
                }
                else {
                    ret=(char *)jmalloc(strlen(pathin)+1,"expanded filename");
                    strcpy(ret,pathin);
                }
                ret_length = strlen(ret);
            }
            FindClose(FoundFile);
        }
    }
    else
        return strcpy((char *)jmalloc(strlen(name)+1,"entry name"),name);  
    return ret;
}

void expand_argv(int &argc, char **&argv)
{
    int i, tot_argc = 1; 
    
    // first count up the number of args
    for (i=1;i<argc;i++) {
        char *expanded_name=expand_filename(argv[i]);
        char *s;
        if (expanded_name) {
            tot_argc++;
            for (s=expanded_name;*s;s++)
                if (*s==' ') tot_argc++;
                jfree(expanded_name);
        }
    }
    
    // now allocate new space for the argv array, don't free old memory because who knows
    // where it came from..
    
    char **new_argv=(char **)jmalloc((tot_argc+1)*sizeof(char *),"expanded argv pointers");
    
    
    // now save all the strings to the appropriate pointer entries
    new_argv[0]=argv[0];
    char nname[_MAX_PATH],*nnamep;
    int len,arg_on=1;;
    for (i=1;i<argc;i++) {
        char *expanded_name = expand_filename(argv[i]);
        char *s;
        if (expanded_name) {
            nnamep = nname;
            len = 0;
            for (s = expanded_name; *s; s++) {
                if (*s == ' ') {
                    *nnamep = 0;
                    new_argv[arg_on] = strcpy((char *)jmalloc(len+1,"argv name"),nname);
                    arg_on++;
                    len = 0;
                    nnamep = nname;
                }
                else {
                    *(nnamep++) = *s;
                    len++;
                }
            }
            *nnamep = 0;
            new_argv[arg_on] = strcpy((char*) jmalloc(len + 1, "argv name"), nname);
            arg_on++;
            jfree(expanded_name);
        }    
    }
    argc = tot_argc;
    argv = new_argv;
}

void get_directory(char *path, char **&files, int &tfiles, char **&dirs, int &tdirs)
{
    WIN32_FIND_DATA FindData;
    files = NULL;
    dirs = NULL;
    tfiles = 0;
    tdirs = 0;

    HANDLE FindResult;
    int Next = 1;
    char *DirName = (char*) jmalloc(strlen(path) + strlen("*.spe") + 1, "get_directory--Directory Name");
    sprintf(DirName, "%s*.*", path);

    FindResult = FindFirstFile((LPCSTR) DirName, &FindData);
    while (FindResult != INVALID_HANDLE_VALUE && Next) {
        if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && strcmp(FindData.cFileName, ".")) {
	        tdirs++;
	        dirs = (char**) jrealloc(dirs, sizeof(char*) * tdirs, "dir list");
	        dirs[tdirs - 1] = strcpy((char*) jmalloc(strlen(FindData.cFileName)+1, "tmp file name"), FindData.cFileName);
        }
        Next = FindNextFile(FindResult, &FindData);
    }
    FindClose(FindResult);
    FindResult = 0;
    Next = 1;
    strcpy(DirName, path);
    strcpy(DirName + strlen(path), "*.spe");
    FindResult = FindFirstFile((LPCSTR) DirName, &FindData);
    while (FindResult != INVALID_HANDLE_VALUE && Next) {
	    tfiles++;
	    files = (char**) jrealloc(files, sizeof(char*) * tfiles, "dir list");
	    files[tfiles - 1] = strcpy((char*) jmalloc(strlen(FindData.cFileName)+1, "tmp file name"),FindData.cFileName);
        Next = FindNextFile(FindResult, &FindData);
    }
    delete DirName;
}
