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
#include "pcxread.h"

static void Usage();

enum
{
    CMD_INVALID,
    CMD_LIST,
    CMD_GET,
    CMD_MOVE,
    CMD_DEL,
    CMD_PUT,
    CMD_RENAME,
    CMD_GETPCX,
    CMD_PUTPCX,
};

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        Usage();
        return EXIT_FAILURE;
    }

    int cmd = !strcmp(argv[2], "list") ? CMD_LIST
            : !strcmp(argv[2], "get") ? CMD_GET
            : !strcmp(argv[2], "del") ? CMD_DEL
            : !strcmp(argv[2], "put") ? CMD_PUT
            : !strcmp(argv[2], "move") ? CMD_MOVE
            : !strcmp(argv[2], "rename") ? CMD_RENAME
            : !strcmp(argv[2], "getpcx") ? CMD_GETPCX
            : !strcmp(argv[2], "putpcx") ? CMD_PUTPCX
            : CMD_INVALID;

    if (cmd == CMD_INVALID)
    {
        fprintf(stderr, "abuse-tool: unknown command `%s'\n", argv[2]);
        return EXIT_FAILURE;
    }

    /* Check argument count and file access mode */
    char const *mode = "rwb";
    int minargc = 3;

    switch (cmd)
    {
    case CMD_LIST:
        mode = "rb"; // Read-only access
        break;
    case CMD_GET:
        minargc = 4;
        mode = "rb"; // Read-only access
        break;
    case CMD_PUT:
        minargc = 5;
        break;
    case CMD_MOVE:
        minargc = 5;
        break;
    case CMD_RENAME:
        minargc = 5;
        break;
    case CMD_DEL:
        minargc = 4;
        break;
    case CMD_GETPCX:
        minargc = 4;
        mode = "rb"; // Read-only access
        break;
    case CMD_PUTPCX:
        minargc = 5;
        break;
    }

    if (argc < minargc)
    {
        fprintf(stderr, "abuse-tool: too few arguments to command `%s'\n",
                         argv[2]);
        return EXIT_FAILURE;
    }

    /* Open the SPEC file */
    char tmpfile[4096];
    char const *file = argv[1];
    snprintf(tmpfile, 4096, "%s.tmp", file);

    jFILE fp(file, mode);
    if (fp.open_failure())
    {
        fprintf(stderr, "ERROR - could not open %s\n", file);
        return EXIT_FAILURE;
    }

    spec_directory dir(&fp);

    /* Now really execute commands */
    if (cmd == CMD_LIST)
    {
        printf("   id  type     size  name & information\n");
        printf(" ----  ----  -------  ----------------------------\n");

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

        return EXIT_SUCCESS;
    }
    else if (cmd == CMD_GET)
    {
        int id = atoi(argv[3]);

        if (id < 0 || id >= dir.total)
        {
            fprintf(stderr, "abuse-tool: id %i not found in %s\n", id, file);
            return EXIT_FAILURE;
        }

        spec_entry *se = dir.entries[id];
        fp.seek(se->offset, SEEK_SET);

        for (size_t todo = se->size; todo > 0; )
        {
            uint8_t buf[1024];
            int step = Min(todo, 1024);
            fp.read(buf, step);
            fwrite(buf, step, 1, stdout);
            todo -= step;
        }
        return EXIT_SUCCESS;
    }
    else if (cmd == CMD_GETPCX)
    {
        palette *pal;
        int imgid = atoi(argv[3]);
        int palid = argc > 4 ? atoi(argv[4]) : -1;

        for (int i = 0; palid == -1 && i < dir.total; i++)
            if (dir.entries[i]->type == SPEC_PALETTE)
                palid = i;

        if (palid == -1)
            pal = new palette(256);
        else
            pal = new palette(dir.entries[palid], &fp);

        image *im = new image(&fp, dir.entries[imgid]);
        write_PCX(im, pal, "/dev/stdout");
        delete im;
        delete pal;
        return EXIT_SUCCESS;
    }
    else if (cmd == CMD_MOVE)
    {
        int src = atoi(argv[3]);
        int dst = atoi(argv[4]);

        if (src < 0 || dst < 0 || src >= dir.total || dst >= dir.total)
        {
            fprintf(stderr, "abuse-tool: ids %i/%i out of range\n", src, dst);
            return EXIT_FAILURE;
        }

        dir.FullyLoad(&fp);

        spec_entry *tmp = dir.entries[src];
        for (int d = src < dst ? 1 : -1; src != dst; src += d)
            dir.entries[src] = dir.entries[src + d];
        dir.entries[dst] = tmp;
    }
    else if (cmd == CMD_RENAME)
    {
        int id = atoi(argv[3]);

        if (id < 0 || id >= dir.total)
        {
            fprintf(stderr, "abuse-tool: id %i out of range\n", id);
            return EXIT_FAILURE;
        }

        dir.FullyLoad(&fp);
        dir.entries[id]->name = argv[4];
    }
    else if (cmd == CMD_DEL)
    {
        int id = atoi(argv[3]);

        if (id < 0 || id >= dir.total)
        {
            fprintf(stderr, "abuse-tool: id %i out of range\n", id);
            return EXIT_FAILURE;
        }

        dir.total--;
        for (int i = id; i < dir.total; i++)
            dir.entries[i] = dir.entries[i + 1];

        dir.FullyLoad(&fp);
    }
    else
    {
        /* Not implemented yet */
        return EXIT_FAILURE;
    }

    /* If we get here, we need to write the directory back */
    dir.calc_offsets();
    fp.seek(0, SEEK_SET); // FIXME: create a new file
    dir.write(&fp);
    for (int i = 0; i < dir.total; i++)
        fp.write(dir.entries[i]->data, dir.entries[i]->size);

    return EXIT_SUCCESS;
}

static void Usage()
{
    fprintf(stderr, "%s",
            "Usage: abuse-tool <spec_file> <command> [args...]\n"
            "List of available commands:\n"
            "  list                  list the contents of a SPEC file\n"
            "  get <id>              dump entry <id> to stdout\n"
            "  getpcx <id>           dump PCX image <id> to stdout\n"
            "  del <id>              delete entry <id>\n"
            "  rename <id> <name>    rename entry <id> to <name>\n"
            "  move <id1> <id2>      move entry <id1> to <id2>\n");
}

