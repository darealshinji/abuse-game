/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */


#ifndef __LEVEL_HPP_
#define __LEVEL_HPP_

#include "specs.h"
#include "objects.h"
#include "view.h"
#include "id.h"

#include <stdlib.h>
#define ASPECT 4             // foreground scrolls 4 times faster than background


// the following defines the area of activity for objects
// when they are out of this are no processing occurs on them
// region is specified from upper left corner of screen
#define ACTIVE_LEFT 500
#define ACTIVE_RIGHT (280+500)
#define ACTIVE_TOP 200
#define ACTIVE_BOTTOM (180+200)
#define fgvalue(y) ((y) & 0x3fff)
#define above_tile(y) ((y) & 0x4000)
#define bgvalue(y) (y)

class area_controller
{
  public :
  int32_t x,y,w,h,active;
  int32_t ambient,view_xoff,view_yoff;
  int32_t ambient_speed, view_xoff_speed,view_yoff_speed;
  area_controller *next;
  area_controller(int32_t X, int32_t Y, int32_t W, int32_t H, area_controller *Next);
} ;

extern int32_t last_tile_hit_x,last_tile_hit_y;
extern int dev;
class Level        // contain map info and objects
{
  uint16_t *map_fg,        // just big 2d arrays
           *map_bg,
       bg_width,bg_height,
       fg_width,fg_height;
  String m_name, m_first_name;
  int32_t total_objs;
  GameObject *first,*first_active,*last;

  GameObject **attack_list;                // list of characters for tick which can attack someone
  int attack_list_size,attack_total;
  void add_attacker(GameObject *who);

  GameObject **target_list;                // list of characters for tick which can be attacked
  int target_list_size,target_total;
  void add_target(GameObject *who);

  GameObject **block_list;                // list of characters who can block a character
  int block_list_size,block_total;
  void add_block(GameObject *who);

  void remove_block(GameObject *who);
  void remove_all_block(GameObject *who);

  GameObject **all_block_list;            // list of characters who can block a character or can be hurt
  int all_block_list_size,all_block_total;
  void add_all_block(GameObject *who);
  uint32_t ctick;

public :
    String const & GetOriginalName() const
    {
        if (m_first_name.Count())
            return m_first_name;
        return m_name;
    }

    String const &GetName() const
    {
        return m_name;
    }

    void SetName(String const &name)
    {
        m_name = name;
    }

  uint32_t tick_counter() { return ctick; }
  void set_tick_counter(uint32_t x);
  area_controller *area_list;

  void clear_active_list() { first_active=NULL; }
  GameObject *attacker(GameObject *who);
  int is_attacker(GameObject *who);
  GameObject *main_character();

  GameObject *first_object() { return first; }
  GameObject *first_active_object() { return first_active; }
  uint16_t foreground_width() { return fg_width; }
  uint16_t foreground_height() { return fg_height; }
  uint16_t background_width() { return bg_width; }
  uint16_t background_height() { return bg_height; }
  int load_failed() { return map_fg==NULL; }
  Level(SpecDir *sd, bFILE *fp, char const *lev_name);
  void load_fail();
  Level(int width, int height, char const *name);
  int save(char const *filename, int save_all);  // save_all includes player and view information (1 = success)
  void set_size(int w, int h);
  void remove_light(LightSource *which);
  void try_pushback(GameObject *subject,GameObject *target);
  ~Level();

  int fg_raised(int x, int y) { ASSERT(x>=0 && y>=0 && x<fg_width && y<fg_height);
                 return (*(map_fg+x+y*fg_width))&0x4000; }
  void fg_set_raised(int x, int y, int r) { ASSERT(x>=0 && y>=0 && x<fg_width && y<fg_height);
                        uint16_t v=(*(map_fg+x+y*fg_width))&(0xffff-0x4000);
                        if (r) (*(map_fg+x+y*fg_width))=v|0x4000;
                        else (*(map_fg+x+y*fg_width))=v;
                      }
  void mark_seen(int x, int y) { ASSERT(x>=0 && y>=0 && x<fg_width && y<fg_height);
                      (*(map_fg+x+y*fg_width))|=0x8000; }
  void clear_fg(int32_t x, int32_t y) { *(map_fg+x+y*fg_width)&=0x7fff; }

  uint16_t *get_fgline(int y) { ASSERT(y>=0 && y<fg_height); return map_fg+y*fg_width; }
  uint16_t *get_bgline(int y) { ASSERT(y>=0 && y<bg_height); return map_bg+y*bg_width; }
  uint16_t GetFg(ivec2 pos) { if (pos >= ivec2(0) && pos < ivec2(fg_width, fg_height))
                              return fgvalue(*(map_fg+pos.x+pos.y*fg_width));
                                    else return 0;
                      }
  uint16_t GetBg(ivec2 pos) { if (pos >= ivec2(0) && pos < ivec2(bg_width, bg_height))
                      return *(map_bg+pos.x+pos.y*bg_width);
                                     else return 0;
                    }
  void PutFg(ivec2 pos, uint16_t tile) { *(map_fg+pos.x+pos.y*fg_width)=tile; }
  void PutBg(ivec2 pos, uint16_t tile) { *(map_bg+pos.x+pos.y*bg_width)=tile; }
  void draw_objects(view *v);
  void interpolate_draw_objects(view *v);
  void draw_areas(view *v);
  int tick();                                // returns false if character is dead
  void check_collisions();
  void wall_push();
  void add_object(GameObject *new_guy);
  void add_object_after(GameObject *new_guy, GameObject *who);
  void delete_object(GameObject *who);
  void remove_object(GameObject *who);      // unlinks the object from level, but doesn't delete it
  void load_objects(SpecDir *sd, bFILE *fp);
  void load_cache_info(SpecDir *sd, bFILE *fp);
  void old_load_objects(SpecDir *sd, bFILE *fp);
  void load_options(SpecDir *sd, bFILE *fp);
  void write_objects(bFILE *fp, object_node *save_list);
  void write_options(bFILE *fp);
  void write_thumb_nail(bFILE *fp, AImage *im);
  void write_cache_prof_info();
  void restart();


  void unactivate_all();
  // forms all the objects in processing range into a linked list
  int add_actives(int32_t x1, int32_t y1, int32_t x2, int32_t y2);  //returns total added
  void pull_actives(GameObject *o, GameObject *&last_active, int &t);
  int add_drawables(int32_t x1, int32_t y1, int32_t x2, int32_t y2);  //returns total added

  GameObject *find_object(int32_t x, int32_t y);

  GameObject *damage_intersect(int32_t x1, int32_t y1, int32_t &x2, int32_t &y2, GameObject *exclude);
  GameObject *boundary_setback(GameObject *subject, int32_t x1, int32_t y1, int32_t &x2, int32_t &y2);
  GameObject *all_boundary_setback(GameObject *subject, int32_t x1, int32_t y1, int32_t &x2, int32_t &y2);
  int crush(GameObject *by_who, int xamount, int yamount);
  int push_characters(GameObject *by_who, int xamount, int yamount);  // return 0 if fail on any.
  int platform_push(GameObject *by_who, int xamount, int yamount);
  void foreground_intersect(int32_t x1, int32_t y1, int32_t &x2, int32_t &y2);
  void vforeground_intersect(int32_t x1, int32_t y1, int32_t &y2);

  void hurt_radius(int32_t x, int32_t y,int32_t r, int32_t m, GameObject *from, GameObject *exclude,
           int max_push);
  void send_signal(int32_t signal);
  void next_focus();
  void to_front(GameObject *o);
  void to_back(GameObject *o);
  GameObject *find_closest(int x, int y, int type, GameObject *who);
  GameObject *find_xclosest(int x, int y, int type, GameObject *who);
  GameObject *find_xrange(int x, int y, int type, int xd);
  GameObject *find_self(GameObject *me);


  void write_links(bFILE *fp, object_node *save_list, object_node *exclude_list);
  void load_links(bFILE *fp, SpecDir *sd, object_node *save_list, object_node *exclude_list);


  GameObject *find_type(int type, int skip);
  void insert_players();   // inserts the players into the level


  GameObject *get_random_start(int min_player_dist, view *exclude);
//  GameObject *find_enemy(GameObject *exclude1, GameObject *exclude2);

  bFILE *create_dir(char *filename, int save_all,
            object_node *save_list, object_node *exclude_list);
  view *make_view_list(int nplayers);
  int32_t total_light_links(object_node *list);
  int32_t total_object_links(object_node *save_list);
  GameObject *find_object_in_area(int32_t x, int32_t y, int32_t x1, int32_t y1,
                   int32_t x2, int32_t y2, Cell *list, GameObject *exclude);
  GameObject *find_object_in_angle(int32_t x, int32_t y, int32_t start_angle, int32_t end_angle,
                    void *list, GameObject *exclude);
  object_node *make_not_list(object_node *list);
  int load_player_info(bFILE *fp, SpecDir *sd, object_node *save_list);
  void write_player_info(bFILE *fp, object_node *save_list);
  void write_object_info(char *filename);
  void level_loaded_notify();
} ;

extern Level *g_current_level;
void pull_actives(GameObject *o, GameObject *&last_active, int &t);



#endif

















