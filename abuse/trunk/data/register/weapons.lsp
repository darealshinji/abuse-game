;; Copyright 1995 Crack dot Com,  All Rights reserved
;; See licencing information for more details on usage rights

(make_ammo_icon 'PLASMA_ICON20 "plasma_small"     20)
(make_ammo_icon 'PLASMA_ICON50 "plasma_large"     50)

(make_ammo_icon 'LSABER_ICON50  "lsaber_small"    50)
(make_ammo_icon 'LSABER_ICON100 "lsaber_large"   100)

(make_ammo_icon 'DFRIS_ICON4  "dfris_small"      4)
(make_ammo_icon 'DFRIS_ICON10 "dfris_large"     10)


(defun pgun_draw ()
  (let ((c (- 255 (* (state_time) 40))))
    (scatter_line sgb_lastx sgb_lasty (x) (y) (find_rgb c (/ c 2) c) (state_time))
    (scatter_line sgb_lastx sgb_lasty (x) (y) (find_rgb c (/ c 2) c) 0)
    ))



(defun pgun_ai ()
  (select (state_time)
	  (0 T)
	  (1 T)
	  (2 T)
	  (3 T)
	  (4 T)
	  (5 nil)))

(def_char PLASMAGUN_BULLET
  (vars sgb_angle sgb_lastx sgb_lasty)
  (funs (ai_fun   pgun_ai)
	(draw_fun pgun_draw))
  (range 10000 10000)
  (flags (unlistable T)
	 (add_front T))
  (states "art/misc.spe" (stopped  "sgun_bullet")))


(defun lsaber_ai ()
  (shift_rand_table (random 80))
  nil)


(defun lsaber_draw () 
  (let ((c1 (find_rgb 255 255 255))
	(c2 (find_rgb 70 59 67))
	(c3 (find_rgb 147 155 195)))


    (scatter_line sgb_lastx sgb_lasty (x) (y) c1 0)
    (scatter_line sgb_lastx sgb_lasty (x) (y) c3 2)
    (ascatter_line sgb_lastx sgb_lasty (x) (y) c1 c2 1)
  ))



(def_char LSABER_BULLET
  (vars sgb_angle sgb_lastx sgb_lasty)
  (funs (ai_fun   lsaber_ai)
	(draw_fun lsaber_draw))
  (range 10000 10000)
  (flags (unlistable T)
	 (add_front T))
  (states "art/misc.spe" (stopped  "sgun_bullet")))


(defun angle_diff (a1 a2)
  (if (< (abs (- a2 a1)) 180)
      (- a2 a1)
    (if (< a1 a2)
	(+ (- a1 a2) 180)
      (- (- a1 a2) 180))))


(defun get_fris_angle ()
  (let ((px (with_object (get_object 0) (player_pointer_x)))
	(py (with_object (get_object 0) (player_pointer_y))))
    (atan2 (- (y) py 4)
	   (- px (x)))))

(defun dfris_ai ()
  (if (and (eq 0 (mod (game_tick) 2)) (not (frame_panic)))
      (let ((rand (rand_on)))
	(with_object (add_object SMALL_LIGHT_CLOUD (+ (x) (random 3)) 
				 (- (y) (random 3) (/ (picture_height) 2)))
		     (set_fade_count 11))
	(set_rand_on rand)))
  (set_course (aistate) 12)
  (if (or (not (eq (bmove (if (> (total_objects) 0) (get_object 0) nil)) T))
	  (< (total_objects) 1)
	  (let ((mex (x))
		(mey (y)))
	    (with_object (get_object 0) (find_object_in_area (- mex 7) 
							     (- mey 7) 
							     (+ mex 7) 
							     (+ mey 7) bad_guy_list))))
      (progn	
	(do_white_explo 40 45)
	nil)
    (progn
      (next_picture)
      (if (> (with_object (get_object 0) (total_objects)) 0)
	  (let ((player_angle (get_fris_angle)))
	    (let ((angle_change (angle_diff (aistate) player_angle)))
	      (if (< (abs angle_change) 35)
		  (set_aistate (mod (+ (aistate) angle_change 360) 360))
		(if (>= angle_change 0)
		    (set_aistate (mod (+ (aistate) 35) 360))
		  (set_aistate (mod (+ (aistate) 325 ) 360)))))))
      T))    
)



(defun dfris_cache (type)
  (list (list EXPLODE8 EXP_LIGHT) 
	(list GRENADE_SND)))

(def_char DFRIS_BULLET
  (funs (ai_fun   dfris_ai)
	(get_cache_list_fun dfris_cache))
  (range 10000 10000)
  (flags (unlistable T)
	 (add_front T))
  (states "art/misc.spe" (stopped  "dfris_bullet")))

