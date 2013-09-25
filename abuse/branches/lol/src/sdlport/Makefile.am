
include $(top_srcdir)/build/autotools/common.am

noinst_LIBRARIES = libsdlport.a

libsdlport_a_SOURCES = \
    video.cpp \
    event.cpp \
    sound.cpp sound.h \
    jdir.cpp \
    joystick.cpp joy.h \
    setup.cpp setup.h \
    hmi.cpp hmi.h \
    $(NULL)
libsdlport_a_LIBADD =

libsdlport_a_CPPFLAGS = -I$(srcdir)/../ -I$(srcdir)/../imlib $(AM_CPPFLAGS) -DASSETDIR=\"$(assetdir)\"

