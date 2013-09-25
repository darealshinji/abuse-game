
include $(top_srcdir)/build/autotools/common.am

noinst_LIBRARIES = libimlib.a

libimlib_a_SOURCES = \
    filter.cpp filter.h \
    image.cpp image.h \
    transimage.cpp transimage.h \
    input.cpp input.h \
    palette.cpp palette.h \
    include.cpp include.h \
    fonts.cpp fonts.h \
    specs.cpp specs.h \
    supmorph.cpp supmorph.h \
    pcxread.cpp pcxread.h \
    keys.cpp keys.h \
    dprint.cpp dprint.h \
    status.cpp status.h \
    pmenu.cpp pmenu.h \
    scroller.cpp scroller.h \
    filesel.cpp filesel.h \
    tools.cpp tools.h \
    guistat.cpp guistat.h \
    sprite.cpp sprite.h \
    window.cpp window.h \
    video.cpp video.h \
    event.cpp event.h \
    jdir.h visobj.h \
    $(NULL)

libimlib_a_CPPFLAGS = -I$(srcdir) -I$(srcdir)/.. $(AM_CPPFLAGS)

