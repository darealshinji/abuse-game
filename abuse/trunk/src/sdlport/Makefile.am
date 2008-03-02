## Process this file with automake to produce Makefile.in
DEFS = @DEFS@ @EXPDATADIR@

noinst_LIBRARIES = libsdlport.a

libsdlport_a_SOURCES = video.cpp mouse.cpp event.cpp sound.cpp timing.cpp jdir.cpp joystick.cpp setup.cpp
libsdlport_a_LIBADD =

INCLUDES = -I$(srcdir)/../ -I$(srcdir)/../imlib $(SDL_CFLAGS)

CPPFLAGS = @CPPFLAGS@ -DMANAGE_MEM -DNO_CHECK
