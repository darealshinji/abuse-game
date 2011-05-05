(select (random 4)
	(0 (setq net_levels
        '("netlevel/limeston.spe"
	    "netlevel/bugs.spe"
          "netlevel/jdm3.spe")))
      (1 (setq net_levels
        '("netlevel/bugs.spe"
	    "netlevel/limeston.spe"
          "netlevel/jdm3.spe")))
      (2 (setq net_levels
        '("netlevel/limeston.spe"
	    "netlevel/jdm3.spe"
          "netlevel/bugs.spe")))
      (3 (setq net_levels
        '("netlevel/jdm3.spe"
	    "netlevel/bugs.spe"
	    "netlevel/limeston.spe")))
      )
