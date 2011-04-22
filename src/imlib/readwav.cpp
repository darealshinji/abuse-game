/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include "common.h"

#include "readwav.h"
#include "specs.h"
#include "dprint.h"

struct wav_chunk
{
    char id[4];
    uint32_t size;
    char type[4];
};

struct wav_tag
{
    char id[4];
    uint32_t size;
};


struct wav_format
{
    uint16_t fmt_tag, channels;
    uint32_t samplesps, avg_bytesps;
    uint16_t align;
};

struct pcm_wave
{
    wav_format wf;
    uint16_t bitsps;
};

static void read_chunk(wav_chunk &chunk, bFILE *fp)
{
    fp->read(&chunk.id, 4);
    chunk.size = fp->read_uint32();
    fp->read(&chunk.type, 4);
}

static void read_tag(wav_tag &tag, bFILE *fp)
{
    fp->read(&tag.id, 4);
    tag.size = fp->read_uint32();
}

static void read_pcm(pcm_wave &pcm, bFILE *fp)
{
    pcm.wf.fmt_tag = fp->read_uint16();
    pcm.wf.channels = fp->read_uint16();
    pcm.wf.samplesps = fp->read_uint32();
    pcm.wf.avg_bytesps = fp->read_uint32();
    pcm.wf.align = fp->read_uint16();
    pcm.bitsps = fp->read_uint16();
}

uint8_t *read_wav(char const *filename, int &sample_rate, int &data_size)
{
    bFILE *fp = open_file(filename, "rb");
    if (fp->open_failure())
    {
        delete fp;
        return NULL;
    }

    wav_chunk chunk;
    read_chunk(chunk, fp);
    if (memcmp(chunk.type, "WAVE", 4) != 0)
    {
        printf("Bad WAV file (chunk) %s\n", filename);
        delete fp;
        return NULL;
    }

    wav_tag tag;
    read_tag(tag, fp);
    if (memcmp(tag.id, "fmt ", 4) != 0)
    {
        printf( "fmt tag missing, bad file (%s)\n", filename);
        delete fp;
        return NULL;
    }

    pcm_wave pcm;
    read_pcm(pcm, fp);

    fp->seek(tag.size - 16, SEEK_CUR);  // seek to offset of sample

    read_tag(tag, fp);

    if (memcmp(tag.id, "data", 4) != 0)
    {
        printf("Bad Wav file (tag), %s\n", filename);
        delete fp;
        return NULL;
    }

    data_size = tag.size;
    uint8_t *ret = (uint8_t *)malloc(tag.size);
    ERROR(ret, "Malloc error");

    sample_rate = pcm.wf.samplesps;
    ERROR((unsigned int)fp->read(ret, tag.size) == tag.size, "Premature end of file");
    ERROR(pcm.bitsps == 8, "Only 8-bit samples supported");
    ERROR(pcm.wf.channels == 1, "Only mono samples supported");
    ERROR(pcm.wf.align == 1, "Bad block alignment");
    delete fp;
    return ret;
}

