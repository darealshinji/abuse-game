## Process this file with automake to produce Makefile.in
noinst_LIBRARIES = libnet.a

libnet_a_SOURCES = \
    gserver.cpp gserver.hpp \
    gclient.cpp gclient.hpp \
    fileman.cpp fileman.hpp \
    sock.cpp sock.hpp \
    tcpip.cpp tcpip.hpp \
    indian.hpp ghandler.hpp undrv.hpp \
    $(NULL)

INCLUDES = -I$(srcdir) -I$(srcdir)/../ -I$(srcdir)/../imlib $(SDL_CFLAGS)

CPPFLAGS = @CPPFLAGS@ -DMANAGE_MEM -DNO_CHECK
