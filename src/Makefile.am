## Process this file with automake to produce Makefile.in

SUBDIRS = net imlib sdlport

bin_PROGRAMS = abuse

abuse_SOURCES = specache.cpp netcfg.cpp text_gui.cpp innet.cpp \
	chat.cpp endgame.cpp version.cpp loadgame.cpp profile.cpp \
	cop.cpp statbar.cpp compiled.cpp ant.cpp sensor.cpp \
	lisp_opt.cpp demo.cpp lcache.cpp lisp_gc.cpp nfclient.cpp \
	username.cpp clisp.cpp gui.cpp transp.cpp collide.cpp trig.cpp \
	property.cpp lisp.cpp cache.cpp particle.cpp objects.cpp \
	extend.cpp console.cpp ability.cpp items.cpp dev.cpp chars.cpp \
	level.cpp smallfnt.cpp automap.cpp help.cpp intsect.cpp \
	loader2.cpp seq.cpp points.cpp fnt6x13.cpp morpher.cpp \
	menu.cpp director.cpp view.cpp config.cpp game.cpp light.cpp \
	devsel.cpp crc.cpp gamma.cpp language.cpp

abuse_LDADD = ./sdlport/libsdlport.a ./imlib/libimlib.a ./net/libnet.a

abuse_DEPENDENCIES = ./sdlport/libsdlport.a ./imlib/libimlib.a ./net/libnet.a

INCLUDES = -I$(srcdir) -I$(srcdir)/imlib -I$(srcdir)/net $(SDL_CFLAGS)

CPPFLAGS = @CPPFLAGS@ -DMANAGE_MEM -DNO_CHECK
