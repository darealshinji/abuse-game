/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include "specache.h"

SpecDirCache g_sd_cache;

void SpecDirCache::Load(bFILE *fp)
{
    uint16_t tfn = fp->read_uint16();
    m_size = 0;
    for (int i = 0; i < tfn; i++)
    {
        char fn[256];
        uint8_t len;
        fp->read(&len, 1);
        fp->read(fn, len);
        GetSpecDir(fn, fp);
    }
}

void SpecDirCache::Save(bFILE *fp)
{
    uint16_t total = 0;
    for (FileNode *f = m_list; f; f = f->m_next)
        total++;
    fp->write_uint16(total);
    for (FileNode *f = m_list; f; f = f->m_next)
    {
        uint8_t len = f->m_name.Count() + 1;
        fp->write(&len, 1);
        fp->write(f->m_name.C(), len);
        f->m_sd->write(fp);
    }
}

SpecDir *SpecDirCache::GetSpecDir(String const filename, bFILE *fp)
{
    FileNode **parent = 0, *p = m_root;
    while (p)
    {
        int cmp = strcmp(p->m_name.C(), filename.C());
        if (cmp == 0)
            return p->m_sd;
        parent = cmp < 0 ? &p->m_left : &p->m_right;
        p = *parent;
    }

    bool need_close = false;
    if (!fp)
    {
        fp = open_file(filename.C(), "rb");
        if (fp->open_failure())
        {
            delete fp;
            return 0;
        }
        need_close = true;
    }

    FileNode *f = new FileNode(filename, new SpecDir(fp));
    f->m_next = m_list;
    m_list = f;

    m_size += f->m_sd->m_data.Count();
    if (parent)
        *parent = f;
    else
        m_root = f;

    if (need_close)
        delete fp;
    return f->m_sd;
}

SpecDirCache::~SpecDirCache()
{
    ClearNode(m_root);
}

void SpecDirCache::Clear()
{
    ClearNode(m_root);
    m_root = nullptr;
    m_size = 0;
}

void SpecDirCache::ClearNode(FileNode *f)
{
    if (f)
    {
        if (f->m_left)
        {
            ClearNode(f->m_left);
            delete f->m_left;
        }
        if (f->m_right)
        {
            ClearNode(f->m_right);
            delete f->m_right;
        }
    }
}

