## Process this file with automake to produce Makefile.in

SUBDIRS = lisp net imlib sdlport

bin_PROGRAMS = abuse

abuse_SOURCES = \
    specache.cpp specache.hpp \
    netcfg.cpp netcfg.hpp \
    text_gui.cpp text_gui.hpp \
    innet.cpp \
    chat.cpp chat.hpp \
    endgame.cpp \
    version.cpp \
    loadgame.cpp loadgame.hpp \
    profile.cpp profile.hpp \
    cop.cpp cop.hpp \
    statbar.cpp \
    compiled.cpp compiled.hpp \
    ant.cpp ant.hpp \
    sensor.cpp \
    demo.cpp demo.hpp \
    lcache.cpp lcache.hpp \
    nfclient.cpp nfclient.hpp \
    username.cpp \
    clisp.cpp clisp.hpp \
    gui.cpp gui.hpp \
    transp.cpp transp.hpp \
    collide.cpp \
    trig.cpp \
    property.cpp property.hpp \
    cache.cpp cache.hpp \
    particle.cpp particle.hpp \
    objects.cpp objects.hpp \
    extend.cpp extend.hpp \
    console.cpp console.hpp \
    ability.cpp ability.hpp \
    items.cpp items.hpp \
    dev.cpp dev.hpp \
    chars.cpp chars.hpp \
    level.cpp level.hpp \
    smallfnt.cpp \
    automap.cpp automap.hpp \
    help.cpp help.hpp \
    intsect.cpp intsect.hpp \
    loader2.cpp loader2.hpp \
    seq.cpp seq.hpp \
    points.cpp points.hpp \
    fnt6x13.cpp \
    morpher.cpp morpher.hpp \
    menu.cpp menu.hpp \
    director.cpp director.hpp \
    view.cpp view.hpp \
    config.cpp config.hpp \
    game.cpp game.hpp \
    light.cpp light.hpp \
    devsel.cpp devsel.hpp \
    crc.cpp crc.hpp \
    gamma.cpp gamma.hpp \
    language.cpp language.hpp \
    bus_type.hpp stack.hpp id.hpp netface.hpp isllist.hpp sbar.hpp \
    nfserver.hpp exitproc.hpp \
    $(NULL)

EXTRALIBS = lisp/liblisp.a sdlport/libsdlport.a imlib/libimlib.a net/libnet.a
abuse_LDADD = $(EXTRALIBS)
abuse_DEPENDENCIES = $(EXTRALIBS)

INCLUDES = -I$(srcdir) -I $(srcdir)/lisp -I$(srcdir)/imlib \
           -I$(srcdir)/net $(SDL_CFLAGS)

CPPFLAGS = @CPPFLAGS@ -DMANAGE_MEM -DNO_CHECK
