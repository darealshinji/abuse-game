dnl Process this file with autoconf to produce a configure script.
AC_INIT(abuse, 0.7.0)
AC_CONFIG_AUX_DIR(.auto)
AM_INIT_AUTOMAKE

dnl Version information
VERSION="0.7.0"

dnl Checks for programs.
AC_PROG_LIBTOOL
AC_ISC_POSIX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_C_BIGENDIAN

dnl Check for X
AC_PATH_X
X_LIBS=-L$x_libraries

dnl Figure out where the datafiles will be
EXPDATADIR="-DEXPDATADIR=\\\"$datadir/games/abuse\\\""
AC_SUBST(EXPDATADIR)

dnl Checks for libraries
dnl Do we need to link against something for X shared memory support?
AC_CHECK_LIB(Xext,XShmAttach,:,[
AC_CHECK_LIB(XextSam,XShmAttach,LIBS="$LIBS -lXextSam",,$X_LIBS -lX11 -lXext)
],$X_LIBS -lX11)

dnl Checks for Solaris compatibility
AC_CHECK_LIB(m,pow,LIBS="$LIBS -lm")
AC_CHECK_LIB(socket,socket,LIBS="$LIBS -lsocket")
AC_CHECK_LIB(nsl,gethostbyname,LIBS="$LIBS -lnsl")

dnl Check for SDL
SDL_VERSION=1.1.6
AM_PATH_SDL($SDL_VERSION, :,
		AC_MSG_ERROR([*** SDL version $SDL_VERSION or above not found!]))
CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS $X_LIBS -L/usr/lib"

# Optimizations
CPPFLAGS="${CPPFLAGS} -g -O2 -fno-strength-reduce -fomit-frame-pointer"
# Code qui fait des warnings == code de porc == deux baffes dans ta gueule
CPPFLAGS="${CPPFLAGS} -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Waggregate-return -Wsign-compare"

dnl Checks for header files
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h malloc.h string.h sys/ioctl.h sys/time.h unistd.h)

dnl Checks for functions
AC_FUNC_MEMCMP
AC_CHECK_FUNCS(atexit on_exit strstr gettimeofday)

dnl Check for OpenGL
dnl Should this be more thorough?
dnl For OpenGL support on OSX it's better to use Project Builder as -lGL
dnl doesn't seem to work this way.
AC_MSG_CHECKING(for OpenGL support)
have_opengl=no
AC_TRY_COMPILE([
		#ifdef WIN32
		#include <windows.h>
		#elif defined(__APPLE__) && defined(__MACH__)
/*		#include <OpenGL/gl.h>*/
		#error	/* Error so the compile fails on OSX */
		#else
		#include <GL/gl.h>
		#endif
],[
],[
have_opengl=yes
])
AC_MSG_RESULT($have_opengl)
if test x$have_opengl = xyes; then
		CFLAGS="$CFLAGS -DHAVE_OPENGL"
		CXXFLAGS="$CXXFLAGS -DHAVE_OPENGL"
		LIBS="$LIBS -lGL -lpthread"
fi

AC_OUTPUT([Makefile
		abuse-sdl.6
		src/Makefile
		src/net/Makefile
		src/imlib/Makefile
		src/sdlport/Makefile])

echo "
------ Configuration for $PACKAGE $VERSION ------
    Compiler:         ${CC} (version `${CC} --version`)
    Install prefix:   $prefix
    SDL version:      `sdl-config --version`
    OpenGL support:   $have_opengl

Now type 'make' to build $PACKAGE.
"
