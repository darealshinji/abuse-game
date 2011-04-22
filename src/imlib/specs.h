/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __SPECS_HPP_
#define __SPECS_HPP_

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "linked.h"

extern char const *spec_types[];

enum
{
    SPEC_INVALID_TYPE = 0,
    SPEC_COLOR_TABLE,
    SPEC_PALETTE,
    SPEC_, /* Unused */
    SPEC_IMAGE,
    SPEC_FORETILE,
    SPEC_BACKTILE,
    SPEC_CHARACTER,
    SPEC_MORPH_POINTS_8,
    SPEC_MORPH_POINTS_16,
    SPEC_GRUE_OBJS,
    SPEC_EXTERN_SFX,
    SPEC_DMX_MUS,
    SPEC_PATCHED_MORPH,
    SPEC_NORMAL_FILE,
    SPEC_COMPRESS1_FILE,
    SPEC_VECTOR_IMAGE,
    SPEC_LIGHT_LIST,
    SPEC_GRUE_FGMAP,
    SPEC_GRUE_BGMAP,
    SPEC_DATA_ARRAY,
    SPEC_CHARACTER2,
    SPEC_PARTICLE,
    SPEC_EXTERNAL_LCACHE,
};

#define SPEC_SIGNATURE    "SPEC1.0"
#define SPEC_SIG_SIZE     8

#define SPEC_FLAG_LINK    1

#define SPEC_SEARCH_INSIDE_OUTSIDE 1
#define SPEC_SEARCH_OUTSIDE_INSIDE 2
#define SPEC_SEARCH_INSIDE_ONLY    3

/*  struct spec_header
 *  {
 *      char signature[8];
 *      uint16_t entries_count;
 *      struct entry
 *      {
 *          uint8_t type;
 *          uint8_t name_length;
 *          char name[name_length];
 *          uint8_t flags;
 *          if (flags & LINK)
 *          {
 *              uint8_t filename_length;
 *              char filename[filename_length];
 *          }
 *          else
 *          {
 *              uint32_t data_size;
 *              uint32_t offset;
 *          }
 *      } entries[entries_count];
 *  }
 */

void set_spec_main_file(char const *filename, int search_order=SPEC_SEARCH_OUTSIDE_INSIDE);

void set_filename_prefix(char const *prefix);
char *get_filename_prefix();
void set_save_filename_prefix(char const *prefix);
char *get_save_filename_prefix();
#define JFILE_CLONED 1

class bFILE     // base file type which other files should be derived from (jFILE & NFS for now)
{
  protected :
  unsigned char *rbuf,*wbuf;
  unsigned long rbuf_start,rbuf_end,rbuf_size,
                wbuf_end,wbuf_size;                // can't seek while writing!
  int flush_writes();                             // returns 0 on failure, else # of bytes written

  virtual int unbuffered_read(void *buf, size_t count)  = 0;
  virtual int unbuffered_write(void const *buf, size_t count) = 0;
  virtual int unbuffered_tell()                         = 0;
  virtual int unbuffered_seek(long offset, int whence)  = 0;   // whence=SEEK_SET, SEEK_CUR,
                                                               // SEEK_END, ret=0=success
  virtual int allow_read_buffering();
  virtual int allow_write_buffering();
  public :
  bFILE();
  virtual int open_failure() = 0;
  int read(void *buf, size_t count);        // returns number of bytes read, calls unbuffer_read
  int write(void const *buf, size_t count); // returns number of bytes written
  int seek(long offset, int whence);        // whence=SEEK_SET, SEEK_CUR, SEEK_END, ret=0=success
  int tell();
  virtual int file_size() = 0;

  virtual ~bFILE();

    // read and write using little-endianness
    uint16_t read_uint16();
    uint32_t read_uint32();
    uint8_t read_uint8();
    double read_double();
    void write_uint16(uint16_t x);
    void write_uint32(uint32_t x);
    void write_uint8(uint8_t x);
    void write_double(double x);
};

class jFILE : public bFILE     // this file type will use virtual opens inside of a spe
{
  char *fname;
  char *tmp_write_name;
  int access;
  int fd,flags;
  long start_offset,file_length;    // offset of file from actual file begining

  long current_offset;  // current offset

public :
    int get_fd() const { return fd; }

  void open_internal(char const *filename, char const *mode, int flags);
  void open_external(char const *filename, char const *mode, int flags);

  jFILE(char const *filename, char const *access_string);      // same as fopen parameters
  jFILE(FILE *file_pointer);                      // assumes fp is at begining of file
  virtual int open_failure() { return fd<0; }
  virtual int unbuffered_read(void *buf, size_t count);       // returns number of bytes read
  virtual int unbuffered_write(void const *buf, size_t count);     // returns number of bytes written
  virtual int unbuffered_seek(long offset, int whence);      // whence=SEEK_SET, SEEK_CUR,
                                                             // SEEK_END, ret=0=success
  virtual int unbuffered_tell();
  virtual int file_size() { return file_length; }
  virtual ~jFILE();
} ;

class spec_entry
{
public :
  char *name;
  unsigned long size,offset;
  unsigned char type;

  spec_entry(unsigned char spec_type,
             char const *object_name,
             char const *link_filename,
             unsigned long data_size,
             unsigned long data_offset)
  { type = spec_type;
    name = strdup(object_name);
    size = data_size;
    offset = data_offset;
  }
  void print();
  ~spec_entry() { if (name) free(name); }
} ;


class spec_directory
{
public :
  void startup(bFILE *fp);

  int total;
  spec_entry **entries;
  void *data;
  long size;
//  spec_directory(char *filename);  ; ; not allowed anymore, user must construct file first!
  spec_directory(FILE *fp);
  spec_directory(bFILE *fp);
  spec_directory();
  spec_entry *find(char const *name);
  spec_entry *find(char const *name, int type);
  spec_entry *find(int type);
  long find_number(char const *name);
  long find_number(int type);
  void remove(spec_entry *e);
  void add_by_hand(spec_entry *e);
  void calc_offsets();
  long data_start_offset();  // returns the first offset past directory items
  long data_end_offset();    // this should be the end of the file
  long type_total(int type);
  jFILE *write(char const *filename);
  int    write(bFILE *fp);
  void print();
  void delete_entries();   // if the directory was created by hand instead of by file
  ~spec_directory();

  static void extract(char const *name);
} ;

/*jFILE *add_directory_entry(char *filename,
                         unsigned short data_type,
                         char *data_name,
                         unsigned long data_size,
                         char *link_filename=NULL); */

uint16_t read_uint16(FILE *fp);
uint32_t read_uint32(FILE *fp);
uint8_t read_uint8(FILE *fp);

void write_uint16(FILE *fp, uint16_t x);
void write_uint32(FILE *fp, uint32_t x);
void write_uint8(FILE *fp, uint8_t x);

void set_spec_main_file(char *filename, int Search_order);
void set_file_opener(bFILE *(*open_fun)(char const *, char const *));
void set_no_space_handler(void (*handle_fun)());
bFILE *open_file(char const *filename, char const *mode);
#endif

