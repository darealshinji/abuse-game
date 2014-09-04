/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#if HAVE_CONFIG_H
#   include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include "common.h"

#include "imlib/dprint.h"

#include "property.h"
#include "game.h"

class AProperty
{
public:
    String m_name;
    String m_string;
    int m_num;

    AProperty(char const *name, int def)
      : m_name(name),
        m_num(def)
    {
    }

    AProperty(char const *name, char const *def)
      : m_name(name),
        m_string(def)
    {
    }

    void Set(int x)
    {
        if (m_string.Count())
            m_string.Resize(0);
        m_num = x;
    }

    void Set(char const *x)
    {
        m_string = x;
    }
};


APropertyManager::APropertyManager()
{
}

APropertyManager::~APropertyManager()
{
    for (int i = 0; i < m_props.Count(); ++i)
        delete m_props[i];
}

AProperty *APropertyManager::find(char const *name)
{
    for (int i = 0; i < m_props.Count(); ++i)
        if (m_props[i]->m_name == name)
            return m_props[i];
    return nullptr;
}

int APropertyManager::get(char const *name, int def)
{
    AProperty *f = find(name);
    if (!f || f->m_string.Count())
        return def;
    return f->m_num;
}

char const *APropertyManager::get(char const *name, char const *def)
{
    AProperty *f = find(name);
    if (!f || !f->m_string.Count())
        return def;
    return f->m_string.C();
}

void APropertyManager::set(char const *name, double def)
{
    AProperty *f = find(name);
    if (f)
        f->Set((int)def);
    else
        m_props.Push(new AProperty(name, (int)def));
}

void APropertyManager::set(char const *name, char const *def)
{
    AProperty *f = find(name);
    if (f)
        f->Set(def);
    else
        m_props.Push(new AProperty(name, def));
}

void APropertyManager::save(char const *filename)
{
    FILE *fp = open_FILE(filename, "wb");
    if (!fp)
    {
        dprintf("Error opening %s to save properties\n",filename);
        return;
    }

    for (int i = 0; i < m_props.Count(); ++i)
    {
        fprintf(fp, "%s = ", m_props[i]->m_name.C());
        if (m_props[i]->m_string.Count())
            fprintf(fp, "\"%s\"\n", m_props[i]->m_string.C());
        else
            fprintf(fp, "%d\n", m_props[i]->m_num);
    }
    fclose(fp);
}

/* FIXME: this is putrid. */
void APropertyManager::load(char const *filename)
{
    char buf[100], *c1, *c2, name[100], str[100];
    FILE *fp = open_FILE(filename, "rb");

    if (!fp)
        return;

    while (!feof(fp) && fgets(buf, 100, fp))
    {
        for (c1=buf,c2=name; *c1 && *c1!='='; c1++,c2++)
            *c2=*c1;
        if (*c1==0)
        {
            fprintf(stderr,"Missing = for property line %s in file %s\n",buf,filename);
            exit(1);
        }
        *c2 = ' ';
        while (*c2==' ') { *c2=0; c2--; }
        c1++;
        while (*c1==' ')
            c1++;
        if (*c1=='"')
        {
            c1++;
            for (c2=str; *c1 && *c1!='"'; c1++,c2++)
                *c2=*c1;
            *c2=0;
            if (*c1!='"')
            {
                fprintf(stderr,"Missing \" for property name %s in file %s\n",name,filename);
                exit(1);
            }
            set(name,str);
        }
        else
        {
            double x;
            if (sscanf(c1,"%lg",&x))
                set(name, x);
            else
            {
                fprintf(stderr, "Bad number/string for property name %s in file %s\n",name,filename);
                exit(1);
            }
        }
    }
}

