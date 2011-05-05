;; Copyright 1995 Crack dot Com,  All Rights reserved
;; See licensing information for more details on usage rights

(setq keep_backup T)                  ;; determines if Save

(setq load_warn nil)

(if (local_load "config/username.lsp")  
  (set_login username))

(if (local_load "config/gamename.lsp")  
  (set_game_name gamename)
  (set_game_name "Abuse Net Game"))

(load "demo.lsp")

(local_load "gamma.lsp")              ;; load gamma correction values if they have been saved

(if (not (load "hardness.lsp"))       ;; load hardness, if no file set to hard
    (setf difficulty        'hard))
(setq load_warn T)

; *********** Defaults **************************

(setf sfx_directory     "sfx/")

;(load_big_font     "art/letters.spe" "letters")
(load_big_font     "art/fonts.spe" "screen11")
(load_small_font   "art/fonts.spe" "small_font")
(load_console_font "art/fonts.spe" "fnt5x7")
(load_color_filter "art/back/backgrnd.spe")
(load_palette      "art/back/backgrnd.spe")
(setq normal_tint (def_tint "art/back/backgrnd.spe"))


(load_tiles "art/fore/foregrnd.spe"  ;; 0
	    "art/fore/techno.spe"    ;; 1-99
	    "art/fore/techno2.spe"   ;; 100-167
	    "art/fore/techno3.spe"   ;; 200-236
	    "art/fore/techno4.spe"   ;; 300-460
	    "art/fore/cave.spe"      ;; 500-634

	    "art/back/backgrnd.spe"  ;; 0
	    "art/back/intro.spe"     ;; 5-37
	    "art/back/city.spe"      ;; 40-70
	    "art/back/cave.spe"      ;; 84-103
	    "art/back/tech.spe"      ;; 110-139
	    )

(setq load_warn nil)
(load "register/tiles.lsp")       ;; load up registered artwork if it's there
(setq load_warn T)

(setf title_screen      '("art/title.spe" . "title_screen"))
(setf logo              '("art/title.spe" . "cdc_logo"))

(setf demos        '("levels/demo1.dat" "levels/demo3.dat" "levels/demo4.dat" "levels/demo5.dat"))
(setq help_screens '("art/help.spe" "sell1"))

(if (not (get_option "-f"))
    (progn
      (if skip_trainer_level
	  (set_first_level "levels/level01.spe")
	(set_first_level "levels/level00.spe"))))


