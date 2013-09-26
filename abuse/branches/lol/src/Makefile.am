
include $(top_srcdir)/build/autotools/common.am

bin_PROGRAMS = abuse abuse-tool

noinst_LIBRARIES = libabuse.a

abuse_SOURCES = \
    common.h \
    \
    blit.lolfx \
    \
    specache.cpp specache.h \
    netcfg.cpp netcfg.h \
    innet.cpp \
    chat.cpp chat.h \
    endgame.cpp \
    loadgame.cpp loadgame.h \
    profile.cpp profile.h \
    cop.cpp cop.h \
    statbar.cpp statbar.h \
    compiled.cpp compiled.h \
    ant.cpp ant.h \
    sensor.cpp \
    demo.cpp demo.h \
    lcache.cpp lcache.h \
    nfclient.cpp nfclient.h \
    clisp.cpp clisp.h \
    gui.cpp gui.h \
    transp.cpp transp.h \
    collide.cpp \
    property.cpp property.h \
    cache.cpp cache.h \
    particle.cpp particle.h \
    objects.cpp objects.h \
    extend.cpp extend.h \
    console.cpp console.h \
    ability.cpp ability.h \
    items.cpp items.h \
    dev.cpp dev.h \
    chars.cpp chars.h \
    level.cpp level.h \
    smallfnt.cpp \
    automap.cpp automap.h \
    help.cpp help.h \
    intsect.cpp intsect.h \
    loader2.cpp loader2.h \
    seq.cpp seq.h \
    points.cpp points.h \
    fnt6x13.cpp \
    morpher.cpp morpher.h \
    menu.cpp menu.h \
    director.cpp director.h \
    view.cpp view.h \
    configuration.cpp configuration.h \
    game.cpp game.h \
    light.cpp light.h \
    devsel.cpp devsel.h \
    crc.cpp crc.h \
    gamma.cpp gamma.h \
    id.h netface.h \
    nfserver.h \
    \
    ui/volumewindow.cpp ui/volumewindow.h \
    $(NULL)
abuse_CPPFLAGS = $(AM_CPPFLAGS)
abuse_LDFLAGS = libabuse.a $(AM_LDFLAGS)
abuse_LDADD = @LOL_LIBS@
abuse_DEPENDENCIES = libabuse.a @LOL_DEPS@

abuse_tool_SOURCES = \
    tool/abuse-tool.cpp \
    crc.cpp crc.h
abuse_tool_CPPFLAGS = $(AM_CPPFLAGS)
abuse_tool_LDFLAGS = libabuse.a $(AM_LDFLAGS)
abuse_tool_LDADD = @LOL_LIBS@
abuse_tool_DEPENDENCIES = libabuse.a @LOL_DEPS@

libabuse_a_SOURCES = \
    imlib/filter.cpp imlib/filter.h \
    imlib/image.cpp imlib/image.h \
    imlib/transimage.cpp imlib/transimage.h \
    imlib/input.cpp imlib/input.h \
    imlib/palette.cpp imlib/palette.h \
    imlib/include.cpp imlib/include.h \
    imlib/fonts.cpp imlib/fonts.h \
    imlib/specs.cpp imlib/specs.h \
    imlib/supmorph.cpp imlib/supmorph.h \
    imlib/pcxread.cpp imlib/pcxread.h \
    imlib/keys.cpp imlib/keys.h \
    imlib/dprint.cpp imlib/dprint.h \
    imlib/status.cpp imlib/status.h \
    imlib/pmenu.cpp imlib/pmenu.h \
    imlib/scroller.cpp imlib/scroller.h \
    imlib/filesel.cpp imlib/filesel.h \
    imlib/tools.cpp imlib/tools.h \
    imlib/guistat.cpp imlib/guistat.h \
    imlib/sprite.cpp imlib/sprite.h \
    imlib/window.cpp imlib/window.h \
    imlib/video.cpp imlib/video.h \
    imlib/event.cpp imlib/event.h \
    imlib/jdir.h imlib/visobj.h \
    \
    lisp/lisp.cpp lisp/lisp.h \
    lisp/lisp_opt.cpp lisp/lisp_opt.h \
    lisp/lisp_gc.cpp lisp/lisp_gc.h \
    lisp/trig.cpp \
    lisp/stack.h lisp/symbols.h \
    \
    net/gserver.cpp net/gserver.h \
    net/gclient.cpp net/gclient.h \
    net/fileman.cpp net/fileman.h \
    net/sock.cpp net/sock.h \
    net/tcpip.cpp net/tcpip.h \
    net/ghandler.h net/undrv.h \
    \
    sdlport/video.cpp \
    sdlport/event.cpp \
    sdlport/sound.cpp sdlport/sound.h \
    sdlport/jdir.cpp \
    sdlport/joystick.cpp sdlport/joy.h \
    sdlport/setup.cpp sdlport/setup.h \
    sdlport/hmi.cpp sdlport/hmi.h \
    $(NULL)
libabuse_a_CPPFLAGS = -I$(srcdir) -I$(srcdir)/.. $(AM_CPPFLAGS) -DASSETDIR=\"$(assetdir)\"

