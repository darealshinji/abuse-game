;; Copyright 1995 Crack dot Com,  All Rights reserved
;; See licensing information for more details on usage rights

;; all messages that need translation here
;; Latest version of this file is "V-E"  (version E=1.46)


(select section
	('game_section

/********** New for Version E (1.51)   **************/
	 (setq level_name         "Levelname")
	 (setq FILENAME           "DATEINAME")
	 (setq CHAT               "REDEN")
	 (setq resync             "Klienten werden synchronisiert, bitte warten...")
	 (setq slack              "Faulenzer rausschmei�en ")
	 (setq hold!              "Bitte warten!")
	 (setq waiting            "Wartet auf Daten...")
	 (setq Error              "Fehler")
	 (setq locating           "Versucht, Server %s zu finden, bitte warten\n")
	 (setq unable_locate      "nicht auffindbar")
	 (setq located            "Server gefunden! L�dt Daten, bitte warten....\n")
	 (setq no_prot            "Kein Netzwerkprotokoll installiert \n")
	 (setq Installed          "Installiert")
	 (setq Not_Installed      "Nicht installiert ")
	 (setq calc_crc           "Netz : berechnet Datei-CRCs")
	 (setq edit_saved         "Ver�nderungen wurden gespeichert")
	 (setq saveas_name        "Speicher Name")
	 (setq l_light            "Licht")
	 (setq l_fg               "VG")
	 (setq l_bg               "HG")
	 (setq New?               "Neu")
	 (setq l_EDIT             "Bearbeiten")

	 /********** New for Version D (1.46)   **************/

         (setq ant_hide           "verborgen. (1=ja,0=nein)")
	 (setq ant_total          "total")
         (setq ant_type           "Typ (0..7)")
         (setq obj_frames         "Frames bis Ankunft")
         (setq obj_frame          "jetziger Frame")
         (setq respawn_reg        "Frames bis zur Regener.")
         (setq conc_flash         "Blitz (1=an)")
         (setq bomb_blink         "Blinkzeit")
         (setq lightin_speed      "Blitztempo 0..9")
         (setq gate_delay_time    "Verz�gerung")
         (setq gate_pulse_speed   "Pulstempo")
         (setq pusher_speed       "Schubtempo")
         (setq spring_yvel        "Wert yvel?")
         (setq train_msg_num      "Nachr. Nr.")
         (setq obj_holder_xoff    "x versetzt")
         (setq obj_holder_yoff    "y versetzt")
         (setq obj_holder_del     "l�schen wenn aus (1=ja)")
         (setq spray_delay        "Verz�g.")
         (setq spray_start        "Startwinkel")
         (setq spray_end          "Endwinkel")
         (setq spray_speed        "Winkeltempo")
         (setq spray_cangle       "Jetztwinkel")
         (setq d_track_speed      "Peiltempo")
         (setq track_fspeed       "Feuertempo")
         (setq track_burst        "Feuersto�")
         (setq track_cont         "Verz�g. weiter")
         (setq track_sangle       "Startwinkel")
         (setq track_eangle       "Endwinkel")
         (setq track_cangle       "Jetztwinkel")
         (setq jug_throw_spd      "Wurftempo (0..255)")
         (setq jug_throw_xv       "Wurf xvel")
         (setq jug_throw_yv       "Wurf yvel")
         (setq jug_stat           "statisch? (1=ja,0=nein)")
         (setq rob_noturn         "nicht umdrehen (1=an)")
         (setq rob_hide           "verborg. Start (1=verborgen,0=sichtbar)")
         (setq who_fdelay         "Feuerverz�g.")
         (setq who_bdelay         "F-Sto�verz�g.")
         (setq who_btotal         "F-Sto� total")
         (setq who_mxv            "max. xvel")
         (setq who_myv            "max. yvel")
         (setq wrob_fdelay        "Feuerverz�g.")
         (setq wrob_bdelay        "F-Sto�verz�g.")
         (setq wrob_btotal        "F-Sto� total")
         (setq wrob_mxv           "max. xvel")
         (setq wrob_myv           "max. yvel")
         (setq dimmer_step_amount "Stufen Menge")
         (setq dimmer_steps       "Stufen dunkel")
         (setq dimmer_dist        "Distanz aktiv")
         (setq dimmer_dedist      "Distanz inaktiv")
         (setq dimmer_silent      "Stiller Modus (1=ja,0=nein)")
         (setq restart_station    "Stationsnummer")
         (setq next_level         "n�chst. Level")
         (setq plat_speed         "Tempo")
         (setq plat_2speed        "2. Tempo")
         (setq plat_pos           "Jetztposition (0..tempo)")
         (setq plat_wait          "max. Wartezeit")
         (setq amb_num            "Sound Nr. (0-15)")
         (setq amb_vol            "Lautst�rke (0-127)")
         (setq amb_rep            "Wiederholverz�g. (0=keine)")
         (setq amb_rand           "Freie Verz�g. (0=keine)")
         (setq switch_reset       "Reset-Tempo")
         (setq sens_onxd          "(an) x Dist")
         (setq sens_onyd          "(an) y Dist")
         (setq sens_offxd         "(aus) x Dist")
         (setq sens_offyd         "(aus) y Dist")
         (setq sens_reset         "(aus) Reset-Tempo")
         (setq sens_unoff         "unausschaltbar (1=ja)")
         (setq sens_cs            "Jetztstatus")
         (setq tp_amb             "Umgebung")



	 (setq ai_xvel            "Xvel    ")
	 (setq ai_yvel            "Yvel    ")
	 (setq ai_xacel           "Xacel   ")
	 (setq ai_yacel           "Yacel   ")
         (setq ai_stime           "ST Zeit ")
         (setq ai_gravity         "Schwerkraft ")
         (setq ai_health          "Gesundheit  ")
         (setq ai_morph           "Morph Power ")
         (setq ai_type            "AI-Typ ")
         (setq ai_state           "AI-Status")
         (setq ai_fade            "Transparenz 0-15")

         (setq a_ambient          "Umgebung      ")
         (setq a_aspeed           "Umgeb.-Tempo")
         (setq a_view_xoff        "Sicht xaus    ")
         (setq a_view_yoff        "Sicht yaus    ")
         (setq a_view_xspd        "Sicht x Tempo ")
         (setq a_view_yspd        "Sicht y Tempo ")
         (setq saved_game         "Spiel gespeichert als savegame.spe")
         (setq saved_level        "Level gespeichert als  '%s'..\n")
         (setq _scroll            "Scrollen")    ; as in left-right, up-down
         (setq ap_width           "Breite ")
         (setq ap_height          "H�he")
         (setq ap_name            "Name")
         (setq ap_pal             "Palette hinzu")
         (setq mouse_at           "Maus an Punkt %d, %d\n")


         (setq l_links            "Link")
         (setq l_light            "Licht")
	 (setq l_char             "Char")
         (setq l_back             "Hintergr.")
         (setq l_bound            "Grenz")
         (setq l_fore             "Vorn")


         (setq SHOW?              "ZEIGE?")
         (setq back_loss (concatenate 'string "Dieses Scrolltempo verringert die Gr��e der Hintergrundkarte.\n"
                                      "Dabei k�nnen Platten verloren gehen. Trotzdem weitermachen?\n"))
         (setq WARNING            "WARNUNG")
	 (setq x_mul              "X mul")    ; X multiple
	 (setq y_mul              "Y mul")
	 (setq x_div              "X div")    ; X divisor
	 (setq y_div              "Y div")

	 /*********** New for Version 1.45 ***********************/


;; Weitere Einzelheiten zu den Benutzerrechten unter Lizenzrechte.


	 (setq file_top           "Datei")
	 (setq edit_top           "Bearbeiten")
	 (setq window_top         "Fenster")
	 (setq menu1_load         "Level laden")
	 (setq menu1_save         "Level speichern (S)")
	 (setq menu1_saveas       "Level speichern als")
	 (setq menu1_savegame     "Spiel speichern")
	 (setq menu1_new          "Neues Level")
	 (setq menu1_resize       "Kartengr��e �ndern")
	 (setq menu1_suspend      "Nicht-Spieler L�hmung an/aus")
	 (setq menu1_toggle       "Spiel Modus an/aus (TAB)")
	 (setq menu1_savepal      "Paletten speichern         ")
	 (setq menu1_startc       "Cache-Profil starten   ")
	 (setq menu1_endc         "Cache-Profil beenden     ")
	 (setq menu1_quit         "Abbrechen      (ALT-X)")

	 (setq menu2_light        "Licht an/aus")
	 (setq menu2_scroll       "Scrollrate festlegen")
	 (setq menu2_center       "Spieler zentrieren   (c)")
	 (setq menu2_addpal       "Palette hinzuf�gen")
	 (setq menu2_delay        "Verz�gerung an/aus (D)")
	 (setq menu2_god          "Unverwundbar")
	 (setq menu2_clear        "Waffen an/aus (Umschalt+Y)")
	 (setq menu2_mscroll      "Mit Maus scrollen")
	 (setq menu2_lock         "Palettenfenster festsetzen")
	 (setq menu2_raise        "Vordergrund hervorheben ")
	 (setq menu2_names        "Objektnamen an/aus")
	 (setq menu2_map          "Karte an/aus (m)")
	 (setq menu2_view         "Blickwinkel ausschalten ")
	 (setq menu2_light        "Autolicht an/aus")
	 (setq menu2_fps          "Bildrate/Objektzahl zeigen ")

	 (setq menu3_fore         "Vordergrund  (f)")
	 (setq menu3_back         "Hintergrund  (b)")
	 (setq menu3_layers       "Schichten zeigen (L)")
	 (setq menu3_light        "Belichtung    (l)")
	 (setq menu3_pal          "Paletten    (p)")
	 (setq menu3_objs         "Objekte     (o)")
	 (setq menu3_console      "Konsole     (-)")
	 (setq menu3_toolbar      "Werkzeugleiste    (a)")
	 (setq menu3_prof         "Profil     (P)")
	 (setq menu3_save         "Positionen speichern")




	 (setq level_size "Levelgr��e")
                           ; 012345678901234567 (please keep same allignment of Name level & total)
	 (setq score_header "Name              Level gesamt")   ; V-E
	 (setq space_cont "LEERTASTE, um fortzufahren ")        ; V-E
	 (setq no_saved "Kein gespeichertes Spiel")

	 (setq lvl_2   "Klein") ; V-C added
	 (setq lvl_4   "Mittel") ; V-C added
	 (setq lvl_8   "Gro�") ; V-C added

	 (setq status  "Status...")    ; V-A added
	 (setq Station "Station Nr.")   ; V-A added
	 (setq secured " gesichert!")   ; V-A added
	 (setq loading "L�dt %s")  ; V-A added

         (setq gamma_msg "Klicken Sie die dunkelste Farbe \nauf Ihrem Monitor an, und klicken Sie OK.")
         (setq telep_msg "Pfeiltaste  (unten) dr�cken, um zu teleportieren.")

         (defun get_train_msg (message_num)
           (select message_num
                   (0 "Mit der Maus zielen und mit Linksklick feuern.")
                   (1 "Munition sammeln, um Schu�rate zu erh�hen.")
                   (2 "Mit Pfeiltaste (unten) aktivieren - hier ist ein Schalter.")
                   (3 "Hier Spiel speichern, Pfeiltaste (unten) dr�cken.")
                   (4 "Pfeiltaste (unten) dr�cken, um Plattform zu aktivieren.")
                   (5 "Rechten Mausknopf festhalten, um Spezialkr�fte zu aktivieren.")
                   (6 "Mit STRG oder EINFG w�hlen Sie Waffen aus.")
                   (7 "Pfeiltaste (oben) dr�cken, um Leiter hochzuklettern.")
                   (8 "F�r n�chsten Level Pfeiltaste (unten) dr�cken.")
                   (9 "Auf versteckte W�nde schie�en, um sie zu zerst�ren.")
                   (10 "Kugel anschie�en, um zu aktivieren.")
                   (11 "Pfeiltaste (unten) dr�cken, um zu teleportieren.")))

 (setq not_there       "Spiel l�uft nicht mehr ")
 (setq max_error       "Max Spielerzahl sollte gleich oder mehr als Min Spielerzahl sein ") ; V-C changed


	 ;(setq min_error       "Min Spielerzahl 1-8\nMin Spielerzahl sollte weniger oder gleich Max Spielerzahl sein ")  ; V-A changed, V-B deleted

	 (setq port_error      "Spielzahl mu� zwischen 1-9 sein.")       ; V-A changed
	 (setq kill_error      "Absch�sse m�ssen zwischen 1-99 sein.")      ; V-A changed
	 (setq name_error      "Ung�ltiger Name")         ; V-B changed
	 (setq game_error      "Ung�ltiger Spielname ")         ; V-B added
	 (setq input_error     "Eingabefehler")
	 (setq ok_button       "OK")
	 (setq cancel_button   "ABBRECHEN")
	 (setq kills_to_win    "Absch�sse, um zu gewinnen:")
	 (setq max_play        "Maximale Spielerzahl")
	 (setq min_play        "\nMinimale Spielerzahl")          ; V-B (added \n)
	 (setq use_port        "Spielzahl")
	 (setq your_name       "Ihr Name")

	 (setq max_error       "Max Spielerzahl mu� gr��er als Min Spielerzahl sein.")
         (setq min_error       "Min. Spielerzahl 1-8")
         (setq use_port        "Spielzahl")
         (setq your_name       " Ihr Name:")
         (setq max_players     "Maximale Spielerzahl erreicht, bitte sp�ter versuchen.\n ")
         (setq net_not_reg     "Demoversion auf diesem Server nicht spielbar.\n")
         (setq min_wait        "Wartet auf %d zus�tzliche Spieler!")
         (setq lev_complete    "Level abgeschlossen")
         (setq no_low_mem         (concatenate 'string "Nicht gen�gend Grundspeicher\n"
                                           "  Vorschl�ge...\n"
                                           "    - Startdiskette erstellen (Info im Handbuch)\n"
                                           "    - TSR's u. f�r ABUSE nicht ben�tigte Treiber beseitigen\n"
                                           "    - Machen Sie mehr Speicher frei\n"))

         (setq no_mem    (concatenate 'string "Nicht gen�gend Speicher verf�gbar\n"
                                           "  Vorschl�ge...\n"
					   "    - Startdiskette erstellen (Info im Handbuch)\n"
                                           "    - TSR's u. f�r ABUSE nicht ben�tigte Treiber beseitigen\n"
                                           "    - Machen Sie mehr Speicher frei\n"))



         ; this is not used right now...
         ;(setq server_not_reg  (concatenate 'string "Server betreibt gerade Demoversion von ABUSE,\n"
         ;                                           "Kompatibilit�tsprobleme k�nnen mit der Share- Option vermieden werden\n"))

         (setq server_port     "Server-Port")
         (setq server_name     "Spielname:")
         (setq Networking      "Netzwerk")
         (setq server          " Neues Netzwerkspiel")
         (setq client          " Spiel beitreten ")
         (setq single_play     "   Allein spielen     ")
	 (setq single_play     "    Netzspiel abbrechen ")  ; V-A
         (setq cancel_net      "      Abbrechen         ")

         (setq ic_return       "Zur�ck ins Spiel")
         (setq ic_quit         "Abuse abbrechen")
         (setq ic_volume       "Lautst�rkeregler")
         (setq ic_gamma        "Kontrastregler")
         (setq ic_easy         "Schwierigkeitsgrad: Niete")
         (setq ic_medium   "Schwierigkeitsgrad: Leicht")
         (setq ic_hard         "Schwierigkeitsgrad: Normal")
         (setq ic_extreme   "Schwierigkeitsgrad: Extrem")  ; don't make any strings longer than this!
         (setq ic_load         "Gespeichertes Spiel laden")
         (setq ic_start        "Neues Spiel")
         (setq ic_sell         "Beteiligte")
	 (setq ic_networking   "Netzwerk")
         (setq no_file         "Datei '%s' nicht auffindbar")
         (setq SFXv            "SFX")
         (setq MUSICv          "Musik")

         (setq to_be_continued "Fortsetzung folgt.....")
         (setq no_edit         "Diese Abuse-Version hat keine Edit-Funktionen")
         (setq no_hirez        "High-Res. gibt es nur im Edit-Modus (-edit)")
         (setq no2             "-2 kann nicht mit -edit zusammen benutzt werden")
         (setq no_pals         "Paletten sind nicht definiert")
         (setq unchop1         "ussage : unchop xsize ysize\n")
         (setq size1           "ussage : size width height\n")
         (setq name_now        "Level-Name ist jetzt '%s'\n")
         (setq esave           "edit save : writing edit.lsp\n")
         (setq nd_player       "Spieler kann nicht gel�scht werden!\n")
         (setq d_nosel         "Kein Objekt oder Licht zum L�schen ausgew�hlt.")
         (setq forward?        "Welches Objekt mitnehmen?")
         (setq back?           "Welches Objekt zur�cklegen?")
         (setq aitype?         "AI �ndern wof�r?")
         (setq prof_off        "Cache-Profiling ist jetzt aus.")
         (setq prof?           "Cache-Profiling ist nicht an!")
         (setq sure?           "Sind Sie sicher?")
         (setq width_          "Weite")
         (setq height_         "H�he")
         (setq suspend_on      "L�hmungs-Modus aktiviert. Objekte werden nicht reagieren.")
         (setq suspend_off     "L�hmungs-Modus deaktiviert. Objekte werden reagieren.")
         (setq quit_title      "Abbrechen")
         (setq YES             "JA")
         (setq NO              "NEIN")
         (setq seqs_off        "Kontinuierliche Bildsequenzen aus\n")
         (setq seqs_on         " Kontinuierliche Bildsequenzen an (1 Bild alle 5 Sek)\n")
         (setq ms_on           "Maus-Scrolling eingeschaltet\n")
         (setq ms_off          "Maus-Scrolling ausgeschaltet\n")
         (setq pal_lock        "Paletten sind gesichert")
         (setq pal_unlock      "Paletten sind ungesichert")
         (setq vs_dis          "Sichtschwenken aus")
         (setq vs_en           "Sichtschwenken an")
         (setq fg_r            "Die Platten im Vordergrund werden h�her gesetzt")
         (setq fg_l            "Die Platten im Vordergrund werden tiefer gesetzt")
         (setq no_clone        "Kann Spieler nicht klonen!\n")
         (setq no_edit.lsp     "Kann edit.lsp zum Schreiben nicht �ffnen")
         (setq missing_sym     "Fehlendes Sprach-Symbol!")
         (setq delay_off       "Verz�gerung aus")
         (setq delay_on        "Verz�gerung an")
         (setq too_big         "Zu gro�, bitte benutzen Sie kleinere Zahlen!")
         (setq LOAD            "LADEN")   ; don't let this get too long
         (setq SAVE            "SPEICHERN")   ; don't let this get too long

	 (setq net_not_reg
               (concatenate 'string "Dieser Server betreibt die REGISTRIERTE ABUSE Version, Sie aber nicht.\n"
                            "Bitten Sie den Betreiber des Servers, mit der Shareware-Version zu spielen,\n"
                            "oder kaufen Sie Abuse. Im registrierten Netzwerk-Spiel k�nnen Sie fliegen,\n"
                            "unsichtbar sein, und Sie haben mehr Waffen\n"))
         (setq server_not_reg
               (concatenate 'string
                            "Dieser Server betreibt die Shareware-Version, Sie jedoch\n"
                            "die registrierte Version (danke!). Damit keine Konflikte zwischen den beiden Versionen entstehen,\n"
                            "betreiben Sie auf diesem Server bitte die Shareware-Version\n"
                            "Beispiel : abuse -net irgendwie.irgendwo.net -share\n"))

	 (setq thank_you "Danke, da� Sie Abuse spielten!\n\n")     ; V-A
         (setq load_warn nil)
         (if (not (load "register/english.lsp"))
             (setq end_msg
                   (concatenate 'string
                                "* �ndern Sie LISP/OPTIONS.LSP, um einige Optionen f�r ABUSE zu\n"
                                "    konfigurieren.\n\n"
                                "* Halten Sie beim Einkaufsbummel die Augen auf, denn Ende M�rz '96 erscheint die "
				"  kommerzielle Version von ABUSE f�r acht Spieler, IPX-Netzwerk unterst�tzt, \n"
                                "  mit einem eingebauten Level-Editor, 17 Einzelspieler-Level plus 12 Exklusiv-\n"
                                "  Netzwerk- \n Level, todbringenderen Waffen, extra Powerups, \n"
                                "  st�rkeren Feinden, besseren Soundeffekten und noch mehr k�nstlerischen\n"
                                "  Effekten.\n\n"


                                "* Um eine volle ABUSE-Version von Electronic Arts zu bestellen, rufen Sie bitte an:\n"
                                "  USA       1-800-245-4525\n"
                                "  CANADA    604-451-3600\n"
                                "  ENGLAND   0753-549442\n"
                                "  FRANKREICH    72 17 07 83\n"
                                "  DEUTSCHLAND   05241 / 24307\n"
                                "  AUSTRALIEN +61(75)911 388\n"
                                "  JAPAN     03-5410-3100\n\n")))

         (setq load_warn T)

         (setq plot_start
               (concatenate 'string
                            "Ihr Name ist Nick Vrenna. Wir schreiben das Jahr 2009. Sie werden zu Unrecht in \n"
			    "einem streng bewachten unterirdischen Gef�ngnis festgehalten, "
                            "wo illegale genetische Experimente durchgef�hrt werden.\\n"
                            "Alan Blake, der leitende Wissenschaftler der Forschungsabteilung, hat das Gen isoliert, "
                            "das in Menschen Gewalt und Aggressionen hervorruft. Diese genetische Sequenz, die "
                            '(#\") "Abuse" '(#\") " hei�t, ist hochansteckend und verursacht schreckliche "
			    "Transformationen und groteske Nebenwirkungen. "
                            "Sie sind die einzige Person, die dagegen immun ist. \\n"
                            "Im Gef�ngnis bricht ein Aufstand aus, und w�hrend dieses wilden Durcheinanders werden "
			    "alle Zellent�ren ge�ffnet. Bald sind alle, sowohl W�rter als auch Str�flinge, infiziert "
                            "und werden in Mutanten transformiert, "
                            "die das Geb�ude in ihre Gewalt bringen.\\n"
                            "Ihre einzige Chance zu entkommen ist es, den Kampfanzug anzuziehen und m�glichst "
                            "schnell zum Kontroll-Raum zu gelangen, der im untersten Gescho� des Geb�udes liegt. "
			    "Erst m�ssen Sie jedoch die Wasserversorgung des Gef�ngnisses unterbrechen, damit "
                            "das Abuse-infizierte Wasser nicht auch noch die Au�enwelt vergiften kann. Freiheit und "
                            "das Schicksal der Welt liegen nun in Ihren H�nden. "))



         (setq plot_middle
               (concatenate 'string
                            "Sie haben den anf�nglichen Aufstand �berlebt, haben sich aber im Gef�ngnis komplett "
			    "verirrt. Bis jetzt war es verd�chtig einfach. \\n"
			    "Wenn Sie ausbrechen wollen - liegt das wirkliche ABUSE noch vor Ihnen. "))


         (setq plot_end
               (concatenate 'string
                            "Gl�ckwunsch! Sie haben das Undenkbare �berlebt  und sind bis in den Kontroll-Raum "
                            "vorgedrungen. Sie haben den Schalter umgelegt, somit die Wasserversorgung "
			    "umgeleitet und der Verbreitung von Abuse Einhalt geboten! ")))
)

