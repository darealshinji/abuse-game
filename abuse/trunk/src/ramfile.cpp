/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

ram_file::ram_file()
{
  buf_size=2048;
  buf=(uchar *)jmalloc(buf_size,"ram file buffer");
  buf_end=0;
}

int ram_file::open_failure() { return 0; }

int ram_file::unbuffered_read(void *buf, size_t count)       // returns number of bytes read
{
}

  virtual int unbuffered_write(void *buf, size_t count);      // returns number of bytes written
  virtual int unbuffered_seek(long offset, int whence);  // whence=SEEK_SET, SEEK_CUR, SEEK_END, ret=0=success
  virtual int unbuffered_tell();
  virtual int file_size();
  virtual ~ram_file();
