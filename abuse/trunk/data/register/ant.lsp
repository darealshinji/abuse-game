;; Copyright 1995 Crack dot Com,  All Rights reserved
;; See licencing information for more details on usage rights


(defun boss_damage (amount from hitx hity push_xvel push_yvel)
  (if (and (eq (fade_count) 0) (not (eq (aistate) 0)) (< (aitype) 6))
      (progn
	(damage_fun amount from hitx hity push_xvel push_yvel)
	(if (eq (hp) 0)
	    (progn
	      (set_hp 1)
	      (set_aitype (+ (aitype) 1))
	      (if (eq (aitype) 6)         ;; go to next alien type
		  (set_aistate 10)        ;; end game
		(set_aistate 5)))))))         ;; fade out
    
(defun boss_cons () 
  (set_hp 1)
  (setq taunt_time 20))

(defun boss_fire ()
  (let ((firex (+ (x) (* (direction) 17)) )
	(firey (- (y) 25))
	(playerx (+ (with_object (bg) (x)) (with_object (bg) (* (xvel) 8))))
	(playery (+ (- (with_object (bg) (y)) 15) (with_object (bg) (* (yvel) 2)))))
    
    (if (and (can_see (x) (y) firex firey nil) (can_see firex firey playerx playery nil))
	(progn
	  (let ((angle (atan2 (- firey playery)
			      (- playerx firex))))
	    (fire_object (me) (aitype) firex firey angle (bg))
	    (set_state weapon_fire))

	  ))))


(defun boss_ai ()
  (if (total_objects)

      (if (eq (aistate) 11)
	  nil
	(progn
	  (select (aistate)
		  (0;; wait for turn on
		   (set_state hiding);; can't see us
		   (set_targetable nil)
		   (if (activated)
		       (set_aistate 1)))
		  (1;; taunt for a while
		   (set_targetable nil)
		   (if (eq taunt_time 0)
		       (progn
			 (set_fade_count 14)
			 (set_state stopped)
			 (play_sound APPEAR_SND 127 (x) (y))
			 (set_aistate 2)));; fade in
		   (progn
		     (setq taunt_time (- taunt_time 1))
		     (if (eq (mod taunt_time 25) 0)
			 (play_sound TAUNT_SND 127 (x) (y)))))

		  (2;; fade in
		   (set_direction (toward))
		   (if (eq (fade_count) 0)
		       (progn
			 (set_state weapon_fire)
			 (go_state 3))
		     (set_fade_count (- (fade_count) 2))))

		  (3;; wait to fire
		   (set_targetable T)
		   (if (next_picture) T
		     (go_state 4)))

		  (4;; fire1
		   (boss_fire)
		   (set_aistate 5)
		   (set_state weapon_fire))

		  (5;; wait to fire
		   (set_targetable T)
		   (if (next_picture) T
		     (go_state 6)))

		  (6;; fire1
		   (boss_fire)
		   (set_aistate 7)
		   (set_state stopped))

		  (7;; fade out
		   (set_targetable nil)
		   (set_fade_count (+ (fade_count) 2))	   
		   (if (eq (fade_count) 14)
		       (progn
			 (set_state hiding)
			 (let ((to_object (get_object (random (total_objects)))))
			   (set_x (with_object to_object (x)))
			   (set_y (with_object to_object (y))))
			 (setq taunt_time (- 30 (* (aitype) 2)))
			 (go_state 0))))
	  
		  (10;; game over	  
		   (set_state hiding)
		   (set_targetable nil)
		   (if (eq (state_time) 60)
		       (go_state 11))
		   (if (not (eq (state_time) 0))
		       (progn
			 (if (eq (mod (state_time) 8) 0)
			     (play_sound GRENADE_SND 127 (x) (y)))
			 (add_object EXPLODE1 (+ (x) (random (* (state_time) 2))) (+ (random (state_time)) (y)))
			 (add_object EXPLODE1 (- (x) (random (* (state_time) 2))) (- (y) (random (state_time)))))))
		  )

	  T))))
	   

	   
	     
(def_char BOSS_ANT
  (funs (ai_fun boss_ai)
	(draw_fun ant_draw)
	(damage_fun boss_damage))
  (vars taunt_time)
  (flags (hurtable T)
	 (unlistable T))
  (fields ("taunt_time" "taunt_tint")
	 ("aistate" "aistate"))
  (states  "register/boss.spe" 
   (stopped "awlk0001.pcx")
   (hiding  "hidden")
   (weapon_fire  (seq "asht" 2 5))))






