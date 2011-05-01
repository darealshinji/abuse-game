;; Copyright 1995 Crack dot Com,  All Rights reserved
;; See licencing information for more details on usage rights

(def_char FLYER
  (funs (ai_fun flyer_ai)
	(damage_fun  flyer_damage)
	(constructor flyer_cons))

  (flags (hurtable T))
  (abilities (start_hp 20))
  (vars fire_delay burst_delay burst_total burst_wait burst_left 
	max_xvel   max_yvel    smoke_time fire_time)
  (fields ("fire_delay"  who_fdelay)
	  ("burst_delay" who_bdelay)
	  ("burst_total" who_btotal)
	  ("max_xvel"    who_mxv)
	  ("max_yvel"    who_myv)
	  ("hp"          ai_health)
	  ("aitype"      ai_type)
	  ("aistate"     ai_state))
  
  (range 200 200)
  (states "art/flyer.spe"
	  (running (seq "ffly" 1 12))
	  (stopped (seq "unhurtable" 1 7))
	  (flinch_up  '("flinch" "flinch" "flinch"))
	  (turn_around (seq "ftrn" 1 6))))




(def_char GREEN_FLYER
  (funs (ai_fun flyer_ai)
	(damage_fun  flyer_damage)
	(constructor flyer_cons))

  (flags (hurtable T))
  (abilities (start_hp 20))
  (vars fire_delay burst_delay burst_total burst_wait burst_left 
	max_xvel   max_yvel    smoke_time fire_time)
  (fields ("fire_delay"   who_fdelay)
	  ("burst_delay"  who_bdelay)
	  ("burst_total"  who_btotal)
	  ("max_xvel"     who_mxv)
	  ("max_yvel"     who_myv)
	  ("hp"           ai_health)
	  ("aitype"       ai_type)
	  ("aistate"      ai_state))
  
  (range 200 200)
  (states "art/flyer.spe"
	  (running (seq "gspe" 1 7))
	  (stopped (seq "gdrp" 1 12))
	  (flinch_up  '("ghurt" "ghurt" "ghurt"))
	  (turn_around (seq "gspn" 1 7))))



