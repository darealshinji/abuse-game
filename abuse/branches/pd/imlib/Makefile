CC=g++
CFLAGS_D=-I../imlib/include -g -DMEM_CHECK 
CFLAGS_O=-I../imlib/include -O2 -DNO_CHECK 
IMLIB_OBJS = IMLIB_OBJS_O = PROG_OBJS = PROG_OBJS_O = all : noname nonamex

LINUX_SVGA_FILES = ../imlib/port/svga/LINUX/debug/event.o \
	../imlib/port/svga/LINUX/debug/monoprnt.o \
	../imlib/port/svga/LINUX/debug/video.o \
	../imlib/port/linux/LINUX/debug/joystick.o \
	../imlib/port/linux/LINUX/debug/sound.o \
	../imlib/port/unix/LINUX/debug/timing.o \
	../imlib/port/svga/LINUX/debug/mouse.o \
	../imlib/LINUX/debug/jwindow.o \
	../imlib/port/unix/LINUX/debug/jdir.o \
	../imlib/port/unix/LINUX/debug/jnet.o

noname : $(LINUX_SVGA_FILES) $(IMLIB_OBJS) $(PROG_OBJS)
	$(CC) -o noname $(LINUX_SVGA_FILES) $(IMLIB_OBJS) $(PROG_OBJS) -lvga -lm -static

LINUX_X_FILES = ../imlib/port/x11/LINUX/debug/event.o \
	../imlib/port/x11/LINUX/debug/monoprnt.o \
	../imlib/port/x11/LINUX/debug/video.o \
	../imlib/port/linux/LINUX/debug/joystick.o \
	../imlib/port/linux/LINUX/debug/sound.o \
	../imlib/port/unix/LINUX/debug/timing.o \
	../imlib/port/x11/LINUX/debug/mouse.o \
	../imlib/LINUX/debug/jwindow.o \
	../imlib/port/unix/LINUX/debug/jdir.o \
	../imlib/port/unix/LINUX/debug/jnet.o

nonamex : $(LINUX_X_FILES) $(IMLIB_OBJS) $(PROG_OBJS)
	$(CC) -o nonamex $(LINUX_X_FILES) $(IMLIB_OBJS) $(PROG_OBJS) -lX11 -lXext -lm

opt : nonameo nonamexo

LINUX_SVGA_FILES_O = ../imlib/port/svga/LINUX/opt/event.o \
	../imlib/port/svga/LINUX/opt/monoprnt.o \
	../imlib/port/svga/LINUX/opt/video.o \
	../imlib/port/linux/LINUX/opt/joystick.o \
	../imlib/port/linux/LINUX/opt/sound.o \
	../imlib/port/unix/LINUX/opt/timing.o \
	../imlib/port/svga/LINUX/opt/mouse.o \
	../imlib/LINUX/opt/jwindow.o \
	../imlib/port/unix/LINUX/opt/jdir.o \
	../imlib/port/unix/LINUX/opt/jnet.o

nonameo : $(LINUX_SVGA_FILES_O) $(IMLIB_OBJS_O) $(PROG_OBJS_O)
	$(CC) -o nonameo $(LINUX_SVGA_FILES_O) $(IMLIB_OBJS_O) $(PROG_OBJS_O) -lvga -lm -static

LINUX_X_FILES_O = ../imlib/port/x11/LINUX/opt/event.o \
	../imlib/port/x11/LINUX/opt/monoprnt.o \
	../imlib/port/x11/LINUX/opt/video.o \
	../imlib/port/linux/LINUX/opt/joystick.o \
	../imlib/port/linux/LINUX/opt/sound.o \
	../imlib/port/unix/LINUX/opt/timing.o \
	../imlib/port/x11/LINUX/opt/mouse.o \
	../imlib/LINUX/opt/jwindow.o \
	../imlib/port/unix/LINUX/opt/jdir.o \
	../imlib/port/unix/LINUX/opt/jnet.o

nonamexo : $(LINUX_X_FILES_O) $(IMLIB_OBJS_O) $(PROG_OBJS_O)
	$(CC) -o nonamexo $(LINUX_X_FILES_O) $(IMLIB_OBJS_O) $(PROG_OBJS_O) -lX11 -lXext -lm

../imlib/port/x11/LINUX/debug/event.o : ../imlib/port/x11/event.c
	$(CC) ../imlib/port/x11/event.c $(CFLAGS_D) -c -o ../imlib/port/x11/LINUX/debug/event.o
../imlib/port/x11/LINUX/debug/event.o : ../imlib/include/keys.hpp
../imlib/port/x11/LINUX/debug/event.o : ../imlib/include/event.hpp
../imlib/port/x11/LINUX/debug/event.o : ../imlib/include/sprite.hpp
../imlib/port/x11/LINUX/debug/event.o : ../imlib/include/mouse.hpp
../imlib/port/x11/LINUX/debug/event.o : ../imlib/include/macs.hpp
../imlib/port/x11/LINUX/debug/event.o : ../imlib/include/gifread.hpp
../imlib/port/x11/LINUX/debug/event.o : ../imlib/in