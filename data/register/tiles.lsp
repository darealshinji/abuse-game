
(load_tiles 
	    "register/alien.spe"    ;; 700-774
	    "register/trees.spe"    ;; 800-931
	    "art/endgame.spe"       ;; 950-1014
	    "register/trees2.spe"   ;; 1100-1134

	    "register/alienb.spe"   ;; 150-179
	    "register/green2.spe"   ;; 200-268
	    "register/galien.spe"   ;; 300-320
	    )

(defun end_game_ai ()
  (if (activated)
      (if (eq (aistate) 8)
	  (if (not (next_picture))
	      (request_end_game))
	(set_aistate (+ (aistate) 1))))
  T)
      


(def_char END_GAME
  (funs (ai_fun end_game_ai))
  (range 0 0)  
  (states "art/endgame.spe"
	  (stopped (app (seq "pipe" 1 9)
			(seq "pipe" 1 9)
			(seq "pipe" 1 9)
			(seq "pipe" 1 9)
			(seq "pipe" 1 9)
			(seq "pipe" 1 9)
			(seq "pipe" 1 9)
			(seq "pipe" 1 9)
			(seq "pipe" 1 9)))))





