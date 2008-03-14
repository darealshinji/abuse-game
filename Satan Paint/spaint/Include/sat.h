#ifndef _SAT_HPP_
#define _SAT_HPP_

extern int sat_r,sat_g,sat_b;
void sat_render(image *im, color_filter *f);
void create_sat_window(select_line *mask, image *im, palette *pal);
int sat_handle_event(event &ev);


#endif
