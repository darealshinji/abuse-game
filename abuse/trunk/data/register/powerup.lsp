;; Copyright 1995 Crack dot Com,  All Rights reserved
;; See licencing information for more details on usage rights

(defun sneaky_power_ai () 
  (next_picture)
  (if (touching_bg) 
      (progn 
	(with_object (bg) (setq special_power SNEAKY_POWER))
	nil)
    T))

(defun fly_power_ai () 
  (next_picture)
  (if (touching_bg)
      (progn 
	(with_object (bg) (setq special_power FLY_POWER))
	nil)
    T))


(defun health_power_ai () 
  (next_picture)
  (if (touching_bg) 
      (progn 
	(with_object (bg) 
		     (progn
		       (setq special_power HEALTH_POWER)
		       (give_player_health 100)))
	nil)
    T))

(defun power_fly_cache (type)
  (list (list CLOUD) (list fly_image)))

(def_char POWER_FLY
  (funs (ai_fun fly_power_ai)
	(get_cache_list_fun power_fly_cache))
  (flags (add_front T))
  (range 20 20)
  (states "art/misc.spe" (stopped "fly")))

(def_char POWER_SNEAKY
  (funs (ai_fun sneaky_power_ai))
  (flags (add_front T))
  (range 20 20)
  (states "art/misc.spe" (stopped "sneaky")))


(def_char POWER_HEALTH
  (funs (ai_fun health_power_ai))
  (flags (add_front T))
  (range 20 20)
  (states "art/misc.spe" (stopped "b_check")))
