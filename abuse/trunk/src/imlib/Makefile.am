## Process this file with automake to produce Makefile.in
noinst_LIBRARIES = libimlib.a

libimlib_a_SOURCES = \
    filter.cpp filter.hpp \
    globals.cpp \
    image.cpp image.hpp \
    linked.cpp linked.hpp \
    input.cpp input.hpp \
    palette.cpp palette.hpp \
    include.cpp include.hpp \
    fonts.cpp fonts.hpp \
    specs.cpp specs.hpp \
    supmorph.cpp supmorph.hpp \
    pcxread.cpp pcxread.hpp \
    timage.cpp timage.hpp \
    jrand.cpp jrand.hpp \
    keys.cpp keys.hpp \
    dprint.cpp dprint.hpp \
    status.cpp status.hpp \
    readwav.cpp readwav.hpp \
    pmenu.cpp pmenu.hpp \
    scroller.cpp scroller.hpp \
    filesel.cpp filesel.hpp \
    tools.cpp tools.hpp \
    guistat.cpp guistat.hpp \
    sprite.cpp sprite.hpp \
    jwindow.cpp jwindow.hpp \
    main.hpp macs.hpp video.hpp event.hpp mouse.hpp timing.hpp jdir.hpp \
    system.h \
    $(NULL)

INCLUDES = -I$(srcdir) $(SDL_CFLAGS)

CPPFLAGS = @CPPFLAGS@ -DNO_CHECK
