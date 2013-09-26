/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __CACHE_HPP_
#define __CACHE_HPP_

#include <stdlib.h>

#include "lisp/lisp.h"

#include "imlib/specs.h"

#include "sdlport/sound.h"

#include "items.h"
#include "particle.h"

class Level;

class CrcedFile
{
    friend class CrcManager;

protected:
    CrcedFile(char const *name);
    ~CrcedFile();

    String m_name;
    uint32_t m_crc;
    int m_calculated;
};

// stores crc for each file open to avoid redundant calculations
class CrcManager
{
public:
    CrcManager();

    int GetFileNumber(String const &filename);
    String const &GetFileName(int filenumber);

    uint32_t get_crc(int filenumber, int &failed);
    void set_crc(int filenumber, uint32_t crc);
    void clean_up();
    int total_filenames() { return m_files.Count(); }
    int write_crc_file(char const *filename);
    int load_crc_file(char const *filename);

private:
    Array<CrcedFile *> m_files;
};

/* Cache item types:
 *  - foretile
 *  - backtile
 *  - character
 *  - sound
 *  - image
 *  - TransImage
 */

struct CacheItem
{
    friend class CacheList;

protected:
    void *data;
    int32_t last_access;
    uint8_t type;
    int16_t file_number;
    int32_t offset;
};

class CacheList
{
private:
    CacheItem *list;
    int32_t total, last_registered, last_access, poll_start_access;
    int16_t last_file; // for speed leave the last file accessed open

    bFILE *fp;
    SpecDir *last_dir;
    int32_t last_offset; // store the last offset so we don't have to seek if
                         // we don't need to

    int AllocId();
    void locate(CacheItem *i, int local_only = 0); // set up file and offset for this item
    void normalize();
    void unmalloc(CacheItem *i);
    int used, // flag set when disk is accessed
        ful;  // set when stuff has to be thrown out
    int *prof_data; // holds counts for each id
    void preload_cache_object(int type);
    void preload_cache(Level *lev);

public:
    CacheList();
    ~CacheList();

    void free_oldest();
    int in_use() { if (used) { used = 0; return 1; } else return 0; }
    int full() { if (ful) { ful = 0; return 1; } else return 0; }
    int reg_object(char const *filename, LObject *object, int type,
                   int rm_dups); // lisp object
    int reg(char const *filename, char const *name, int type = -1,
                int rm_dups = 0); // returns id to item
    int loaded(int id);
    void unreg(int id);
    void note_need(int id);

    backtile *backt(int id);
    foretile *foret(int id);
    figure *fig(int id);
    AImage *img(int id);
    part_frame *part(int id);
    sound_effect *sfx(int id);
    LObject *lblock(int id);
    char_tint *ctint(int id);

    void prof_init();
    void prof_write(bFILE *fp);
    void prof_uninit();
    int  prof_size(); // sizeof of spec entry that will be saved
    void prof_poll_start();
    void prof_poll_end();
    int  prof_is_on() { return prof_data != NULL; }   // so Level knows weither to save prof info or not
    int compare(int a, int b); // compares usage count (used by qsort)
    int offset_compare(int a, int b);

    void load_cache_prof_info(String const &filename, Level *lev);
    // sarray is a index table sorted by offset/filenum
    int search(int *sarray, uint16_t filenum, int32_t offset);

    void show_accessed();
    void empty();
};

extern CacheList cache;
extern CrcManager crc_manager;

#endif

