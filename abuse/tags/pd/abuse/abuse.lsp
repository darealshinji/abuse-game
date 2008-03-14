(perm-space)
(setq load_warn nil)
(setq section 'game_section)
(if (not (load "lisp/english.lsp"))             ;; load language specific stuff
    (progn (print "Please make sure you unzipped the game with the -d option")(quit)))
(setq load_warn T)
(load "lisp/common.lsp")
(load "lisp/userfuns.lsp")
(load "lisp/options.lsp")     
(load "lisp/startup.lsp")


;*************************************
;*************************************
;*************************************
(if (not (local_load "lisp/input.lsp"))   ; get local copy first
    (load "lisp/input.lsp"))
;*************************************
;*************************************
;*************************************
;*************************************
;*************************************
;*************************************
(load "lisp/sfx.lsp")
;*************************************
;*************************************
;*************************************
(load "lisp/gates.lsp")
;*************************************
;*************************************
;*************************************
(load "lisp/duong.lsp")
;*************************************
;*************************************
;*************************************
(load "lisp/ant.lsp")
;*************************************
;*************************************
;*************************************
(load "lisp/people.lsp")
;*************************************
;*************************************
;*************************************
(load "lisp/weapons.lsp")
;*************************************
;*************************************
;*************************************
(load "lisp/explo.lsp")
;*************************************
;*************************************
;*************************************
(load "lisp/platform.lsp")
(load "lisp/guns.lsp")
(load "lisp/jugger.lsp")
(load "lisp/flyer.lsp")
(load "lisp/teleport.lsp")
(load "lisp/general.lsp")
(load "lisp/powerup.lsp")
(load "lisp/doors.lsp")
(load "lisp/light.lsp")(load "lisp/ladder.lsp")(load "lisp/switch.lsp")
(setq bad_guy_list (list DARNEL ANT_ROOF TRACK_GUN SPRAY_GUN JUGGER ROB1 WHO ROCKET FLYER GREEN_FLYER BOSS_ANT))
(gc)(tmp-space)(create_players DARNEL)
