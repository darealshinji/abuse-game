/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __FSELECT_HPP_
#define __FSELECT_HPP_

#include "jwindow.hpp"

Jwindow *file_dialog(char const *prompt, char const *def,
             int ok_id, char const *ok_name, int cancel_id,
                     char const *cancel_name, char const *FILENAME_str,
             int filename_id);

#endif




