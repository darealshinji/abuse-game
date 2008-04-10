## Process this file with automake to produce Makefile.in
noinst_LIBRARIES = liblisp.a

liblisp_a_SOURCES = \
    lisp.cpp lisp.hpp \
    lisp_opt.cpp lisp_opt.hpp \
    lisp_gc.cpp lisp_gc.hpp \
    trig.cpp \
    stack.hpp symbols.hpp \
    $(NULL)

INCLUDES = -I$(srcdir) -I$(srcdir)/../ -I$(srcdir)/../imlib $(SDL_CFLAGS)

CPPFLAGS = @CPPFLAGS@ -DNO_CHECK
