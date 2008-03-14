#ifndef __PCOMMAND_HPP_
#define __PCOMMAND_HPP_

#include "jwindow.hpp"
#include "id.hpp"
#include "pmenu.hpp"

class pm_command;

class pm_cmd_handler
{
  int id_on;
  pm_command *first,*current;
  pmenu *pm;
  jwindow *pm_diag;
  void parse(char *pc, jwindow *j);
  int refresh;
  public :
  pm_cmd_handler(); 
  void need_refresh() { refresh=1; }
  int add_id(char *command);
  int handle_event(event &ev, int ok_to_pull);
  void draw(image *screen, window_manager *wm);
  void notify_image_click(int x);
  void notify_palette_click(int x);
  void note_resize();
} ;

#endif
