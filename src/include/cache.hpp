#ifndef __CASHE_HPP_
#define __CASHE_HPP_

#include <stdlib.h>
#include "specs.hpp"
#include "items.hpp"
#include "sound.hpp"
#include "particle.hpp"
#include "lisp.hpp"

class level;
/* Cache item types :

      foretile,backtile,character,
      sound,
      image,trans_image

*/



struct cache_item
{
  void *data;
  int32_t last_access;   
  uint8_t type;
  int16_t file_number;
  int32_t offset;
} ;



class crced_file
{
  public :
  int crc_calculated;
  uint32_t crc;
  char *filename;
  crced_file(char const *name);
  ~crced_file();
} ;

class crc_manager  // stores crc for each file open so redundant calculations are not done
{
  int total_files;
  crced_file **files;
  public :
  crc_manager();
  int get_filenumber(char const *filename);
  uint32_t get_crc(int32_t filenumber, int &failed);
  void set_crc(int32_t filenumber, uint32_t crc);
  char *get_filename(int32_t filenumber);
  void clean_up();
  int total_filenames() { return total_files; }
  int write_crc_file(char const *filename);
  int load_crc_file(char const *filename);
} ;



class cache_list
{
  cache_item *list;
  int32_t total,last_registered,last_access,poll_start_access;
  int16_t last_file;           // for speed leave the last file accessed open

  bFILE *fp,*cache_file,*cache_read_file;
  spec_directory *last_dir;
  int32_t last_offset;          // store the last offset so we don't have to seek if
                             // we don't need to
  

  int16_t lcache_number;
  int32_t alloc_id();
  void locate(cache_item *i, int local_only=0);    // set up file and offset for this item
  void normalize();
  void unmalloc(cache_item *i);
  int used,                                 // flag set when disk is accessed
      ful;                                  // set when stuff has to be thrown out
  int *prof_data;                           // holds counts for each id
  void preload_cache_object(int type);
  void preload_cache(level *lev);
public :
  void create_lcache();
  cache_list();
  void free_oldest();
  int in_use() { if (used) { used=0; return 1; } else return 0; }
  int full() { if (ful) { ful=0; return 1; } else return 0; }
  int32_t reg_object(char const *filename, void *object, int type, int rm_dups);      // lisp object
  int32_t reg(char const *filename, char const *name, int type=-1, int rm_dups=0);          // returns id to item
  int32_t reg_lisp_block(Cell *block);
  int loaded(int id);
  void unreg(int id);
  void note_need(int id);

  backtile       *backt(int id);
  foretile       *foret(int id);
  figure          *fig(int id);
  image           *img(int id);
  part_frame     *part(int id);
  sound_effect    *sfx(int id);
  Cell         *lblock(int id);
  char_tint     *ctint(int id);

  void prof_init();
  void prof_write(bFILE *fp);
  void prof_uninit();
  int  prof_size();                   // sizeof of spec entry that will be saved
  void prof_poll_start();
  void prof_poll_end();
  int  prof_is_on() { return prof_data!=NULL; }   // so level knows weither to save prof info or not
  int compare(int a, int b);          // compares the ussage counts of 2 entries (used by qsort)
  int offset_compare(int a, int b);

  void load_cache_prof_info(char *filename, level *lev);
  int search(int *sarray, uint16_t filenum, int32_t offset);  // sarray is a index table sorted by offset/filenum

  void show_accessed();
  void empty();
  ~cache_list();
} ;

extern cache_list cash;
extern crc_manager crc_man;


#endif






