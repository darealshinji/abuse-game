
include $(top_srcdir)/build/autotools/common.am

noinst_LIBRARIES = libnet.a

libnet_a_SOURCES = \
    gserver.cpp gserver.h \
    gclient.cpp gclient.h \
    fileman.cpp fileman.h \
    sock.cpp sock.h \
    tcpip.cpp tcpip.h \
    ghandler.h undrv.h \
    $(NULL)

libnet_a_CPPFLAGS = -I$(srcdir) -I$(srcdir)/../ -I$(srcdir)/../imlib $(AM_CPPFLAGS)

