(def_char START_RED
	(funs (ai_fun   do_nothing)
		(get_cache_list_fun start_cache)
		(draw_fun dev_draw))
	(range 0 0)
	(states "addon/team/team.spe" (stopped "start_red"))
)

;----------------------------------------------------------------------
(def_char START_GREEN
  (funs (ai_fun   do_nothing)
	(get_cache_list_fun start_cache)
	(draw_fun dev_draw))
  (range 0 0)
  (states "addon/team/team.spe" (stopped "start_green")))

;----------------------------------------------------------------------
(def_char START_YELLOW
  (funs (ai_fun   do_nothing)
	(get_cache_list_fun start_cache)
	(draw_fun dev_draw))
  (range 0 0)
  (states "addon/team/team.spe" (stopped "start_yellow")))

;----------------------------------------------------------------------
(def_char START_BLUE
  (funs (ai_fun   do_nothing)
	(get_cache_list_fun start_cache)
	(draw_fun dev_draw))
  (range 0 0)
  (states "addon/team/team.spe" (stopped "start_blue")))

;----------------------------------------------------------------------
(def_char START_TEAM
	(vars
		start.team
	)
	(funs
		(ai_fun   do_nothing)
		(get_cache_list_fun start_cache)
		(draw_fun dev_draw)
	)
	(range 0 0)
	(states "addon/team/team.spe" (stopped "start_blue"))
)


;----------------------------------------------------------------------
;-=-=-=- CTF Flags -=-=-=-;
;----------------------------------------------------------------------

(defun base_draw ()
	(draw_tint (aref player_tints base.team))
	(if (edit_mode) 
		(progn
			(let (
				(x1 (- (x) (xvel)))
				(y1 (- (y) (yvel)))
				(x2 (+ (x) (xvel)))
				(y2 (+ (y) (yvel)))
				(c  (find_rgb 0 255 0))
				)
				(draw_line x1 y1 x2 y1 c)
				(draw_line x2 y1 x2 y2 c)
				(draw_line x2 y2 x1 y2 c)
				(draw_line x1 y2 x1 y1 c)
			)
			(let (
				(x1 (- (x) (xacel)))
				(y1 (- (y) (yacel)))
				(x2 (+ (x) (xacel)))
				(y2 (+ (y) (yacel)))
				(c  (find_rgb 255 0 0))
				)
				(draw_line x1 y1 x2 y1 c)
				(draw_line x2 y1 x2 y2 c)
				(draw_line x2 y2 x1 y2 c)
				(draw_line x1 y2 x1 y1 c)
			)
		)
	)
)

;----------------------------------------------------------------------
(defun base_flag_check (i)
	(if (>= i 0)
		(progn
			(let (
				(dx (- (x) (with_object (get_object i) (x))))
				(dy (- (y) (with_object (get_object i) (y))))
				)
					(let (
						(dx (if (< dx 0) (- 0 dx) dx))
						(dy (if (< dy 0) (- 0 dy) dy))
					)
					(if (and (< dx (xvel)) (< dy (yvel)))
						(with_object (get_object i) ; The flag
							(progn
								(with_object (get_object 1) ; The flag's owner
									(progn
										(remove_object (me))
									)
								)
								(remove_object (get_object 1))
								(set_x (with_object (get_object 0) (x)))
								(set_y (- (with_object (get_object 0) (y)) (picture_height)))
							)
						)
					)
				)
			)
			(base_flag_check (- i 1))
		)
	)
)
 
;----------------------------------------------------------------------
(defun base_ai ()
	(base_flag_check (- (total_objects) 1))
	T
)

;----------------------------------------------------------------------
(defun flag_draw ()
   (draw_tint (aref player_tints flag.team))
)

;----------------------------------------------------------------------
(defun flag_ai ()
	(if (and (touching_bg) (eq (total_objects) 1) (not (eq flag.team (with_object (bg) (get_object_team)))))
		(progn
			(link_object (bg))
			(with_object (bg) (link_object (me)))
		)
	)
	(if (> (total_objects) 1)
		(progn
			(if (>= 0 (with_object (get_object 1) (hp)))
				(progn
					(setq flag.delay 0)
					(remove_object (get_object 1))
				)
				(progn
					(set_x (with_object (get_object 1) (x)))
					(set_y (with_object (get_object 1) (- (y) (/ (picture_height) 2))))
				)
			)
		)
		(progn
			(setq flag.delay (+ flag.delay 1))
			(if (eq flag.delay 75) ; 5 second delay
				(progn
					(set_x (with_object (get_object 0) (x)))
					(set_y (with_object (get_object 0) (y)))
				)
			)
		)
   	)
	(if (eq (total_objects) 0)
		(progn
			(print "Flag: I have no return point!  Committing suicide...")
			nil
		)
		T
	)
)

;----------------------------------------------------------------------
(def_char CTF_FLAG
	(vars
		flag.team
		flag.delay
		flag.tint
	)
	(funs (ai_fun   flag_ai)
		(get_cache_list_fun start_cache)
		(draw_fun flag_draw))
	(fields
		("flag.team" "red/blue (0/1)")
	)
	(range 0 0)
	(states "addon/team/team.spe" (stopped "flag0001"))
)

;----------------------------------------------------------------------
(def_char CTF_BASE
	(vars
		base.team
		base.tint
	)
	(funs (ai_fun   base_ai)
;;    (get_cache_list_fun start_cache)
		(draw_fun base_draw))

	(fields
		("base.team" "red/blue (0/1)")
		("xvel" "X Range")
		("yvel" "Y Range")
	)
	(range 0 0)
	(states "addon/team/team.spe" (stopped "flag0002"))
)