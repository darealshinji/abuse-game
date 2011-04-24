//
// Abuse Tool - package manager for Abuse format
//
// Copyright: (c) 2011 Sam Hocevar <sam@hocevar.net>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://sam.zoy.org/projects/COPYING.WTFPL for more details.
//

#include "config.h"

#include <cstring>
#include <cstdio>

#include "common.h"
#include "specs.h"
#include "image.h"

static void Usage();

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        Usage();
        return EXIT_FAILURE;
    }

    char *file = argv[1];
    char *cmd = argv[2];

    if (!strcmp(cmd, "list"))
    {
        jFILE fp(file, "rb");
        if (fp.open_failure())
        {
            fprintf(stderr, "ERROR - could not open %s\n", file);
            return EXIT_FAILURE;
        }

        printf("   id  type     size  name & information\n");
        printf(" ----  ----  -------  ----------------------------\n");

        spec_directory dir(&fp);
        for (int i = 0; i < dir.total; i++)
        {
            spec_entry *se = dir.entries[i];

            /* Print basic information */
            printf("% 5i   % 3i % 8i  %s",
                   i, se->type, (int)se->size, se->name);

            /* Is there anything special to say? */
            switch (se->type)
            {
            case SPEC_IMAGE:
            case SPEC_FORETILE:
            case SPEC_BACKTILE:
            case SPEC_CHARACTER:
            case SPEC_CHARACTER2:
              {
                image *im = new image(&fp, se);
                printf(" (%i x %i pixels)", im->Size().x, im->Size().y);
                delete im;
                break;
              }
            case SPEC_PALETTE:
              {
                palette *pal = new palette(se, &fp);
                printf(" (%i colors)", pal->pal_size());
                delete pal;
                break;
              }
            }

            /* Finish line */
            putchar('\n');
        }
    }
    /* else if (...) */
    else
    {
        fprintf(stderr, "abuse-tool: unknown command `%s'\n", cmd);
    }

    return EXIT_SUCCESS;
}

static void Usage()
{
    fprintf(stderr, "%s",
            "Usage: abuse-tool <spec_file> <command> [args...]\n"
            "List of available commands:\n"
            "  list -- list the contents of a SPEC file\n");
}

