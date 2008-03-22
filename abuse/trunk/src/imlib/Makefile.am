## Process this file with automake to produce Makefile.in
noinst_LIBRARIES = libimlib.a

libimlib_a_SOURCES = \
    filter.cpp filter.hpp \
    gifread.cpp gifread.hpp \
    globals.cpp \
    image.cpp image.hpp \
    linked.cpp linked.hpp \
    input.cpp input.hpp \
    mdlread.cpp mdlread.hpp \
    palette.cpp palette.hpp \
    ppmread.cpp ppmread.hpp \
    include.cpp include.hpp \
    xwdread.cpp xwdread.hpp \
    fonts.cpp fonts.hpp \
    decoder.cpp \
    loader.cpp loader.hpp \
    glread.cpp glread.hpp \
    texture.cpp texture.hpp \
    specs.cpp specs.hpp \
    supmorph.cpp supmorph.hpp \
    image24.cpp image24.hpp \
    pcxread.cpp pcxread.hpp \
    timage.cpp timage.hpp \
    jrand.cpp jrand.hpp \
    lbmread.cpp lbmread.hpp \
    targa.cpp targa.hpp \
    keys.cpp keys.hpp \
    dprint.cpp dprint.hpp \
    status.cpp status.hpp \
    visobj.cpp visobj.hpp \
    packet.cpp packet.hpp \
    readwav.cpp readwav.hpp \
    pmenu.cpp pmenu.hpp \
    scroller.cpp scroller.hpp \
    filesel.cpp filesel.hpp \
    tools.cpp tools.hpp \
    guistat.cpp guistat.hpp \
    sprite.cpp sprite.hpp \
    jwindow.cpp jwindow.hpp \
    main.hpp macs.hpp video.hpp gifdecod.hpp event.hpp mouse.hpp timing.hpp \
    jdir.hpp emm.hpp system.h std.h dos.h dir.h errs.h \
    $(NULL)

INCLUDES = -I$(srcdir) $(SDL_CFLAGS)

CPPFLAGS = @CPPFLAGS@ -DMANAGE_MEM -DNO_CHECK
