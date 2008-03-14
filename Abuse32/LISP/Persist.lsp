(setq PersistVars '(0 0 0 0 0 0 0 0 0 0 0 0 0 0 0))

(defun persist_ufun (value)
	(if (atom value)
		(progn
			(setq (car (nth Persist.PersistIndex PersistVars)) value)
			(print value Persist.PersistIndex)
		)
		(car (nth Persist.PersistIndex PersistVars))
	)
)


(defun persist_ai () 
	T
)

(def_char PERSIST_VAR
	(vars
		Persist.PersistIndex
	)
	
	(funs
		(draw_fun dev_draw)
		(ai_fun persist_ai)
		(user_fun persist_ufun)
	)
	(fields
		("Persist.PersistIndex" "Variable Index")
	)
	(range 0 0)
	(states "art/misc.spe"
		(stopped '("switch_off1" "switch_off2"))
	)
)

(defun persist_switch_ai () 
	(next_picture)
	(select (aistate)
		(0      ; waiting for player to press, then turn to on
			(if (and (< (distx) 20) (< (disty) 30) (with_object (bg) (pressing_action_key)))
				(progn
					(play_sound SWITCH_SND 127 (x) (y))
					(set_state running)		 
					(set_aistate 1)
					(with_obj0 (persist_ufun 1))
				)
			)
		)
		(1     ; wait for player to let go of button	  
			(if (not (with_object (bg) (pressing_action_key)))
				(set_aistate 2)
			)
		)
		(2     ; wait for player to press, then turn to off
			(if (and (< (distx) 20) (< (disty) 30) (with_object (bg) (pressing_action_key)))
				(progn
					(play_sound SWITCH_SND 127 (x) (y))
					(set_state stopped)
					(set_aistate 4)
				)
			)
		)
		(4     ; wait for player to let go of button
			(if (not (with_object (bg) (pressing_action_key)))
				(set_aistate 0)
			)
		)
	)
	T
)

(def_char PERSIST_SWITCH
	(funs 
		(ai_fun persist_switch_ai)
		(reload_fun lower_reload)
	)
	(range 0 0)  
	(states "art/misc.spe"
		(stopped '("switch_off1" "switch_off2"))
		(running '("switch_on1"  "switch_on2"))
	)
)