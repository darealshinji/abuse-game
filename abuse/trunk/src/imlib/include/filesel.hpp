#ifndef __FSELECT_HPP_
#define __FSELECT_HPP_

#include "jwindow.hpp"

jwindow *file_dialog(window_manager *wm, char const *prompt, char const *def,
		     int ok_id, char const *ok_name, int cancel_id,
                     char const *cancel_name, char const *FILENAME_str,
		     int filename_id);

#endif




