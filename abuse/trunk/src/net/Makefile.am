## Process this file with automake to produce Makefile.in
noinst_LIBRARIES = libnet.a

libnet_a_SOURCES = gserver.cpp gclient.cpp fileman.cpp sock.cpp tcpip.cpp

INCLUDES = -I$(srcdir) -I$(srcdir)/../ -I$(srcdir)/../imlib

CPPFLAGS = @CPPFLAGS@ -DMANAGE_MEM -DNO_CHECK
