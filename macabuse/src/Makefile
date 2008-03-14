# {{{ Makefile
#
# Oliver Yu
#
# $id$
# }}}

# {{{ List Macros

EXENAME		= boat

X11_FILES = x11_vid.c x11_evt.c
SVGA_FILES = svga_vid.c svga_evt.c

CSRC =  $(X11_FILES) \
        floater.c \
        boat.c \
        bomb.c \
        image.c \
        linked.c \
        palette.c \
        jmalloc.c \
        specs.c \
        filter.c \
        dprint.c \
        rectlist.c \
        texture.c \
        draw.c \
        timing.c \
        main.c \
        boatcnst.c \
        boatdraw.c \
        bombdraw.c \
        anims.c \
        animate.c \
        wat_part.c \
        boatgfx.c

CCSRC =  \
		
EXTDEP =  \

EXEDEP =	$(CCSRC:%.cc=%.o) $(CSRC:%.c=%.o)
SRCDEP =	$(CCSRC:%.cc=.%.d) $(CSRC:%.c=.%.d)	
DIRTYFILES = \
	*.bak \
	$(EXEDEP) \
	$(SRCDEP) \
	$(EXENAME)

# }}} ################################################################
# {{{ Translator Definitions

CC              = g++
CXX             = g++
SHELL		= /bin/sh

INCLUDE		= -Iinc

LIBS		= -lm -lXext -lX11
DEFS		= -DMEM_CHECK

CDEBUG		= -O2 -g
CPPFLAGS	= $(CDEBUG) $(INCLUDE) $(DEFS)
LDFLAGS		= -O2 -g

# }}} ################################################################
# {{{ Implicit Rules

.SUFFIXES: .c .cc .o .d

.%.d: %.c
	$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $< \
		      | sed '\''s/$*\\.o[ :]*/& $@/g'\'' > $@'
.%.d: %.cc
	$(SHELL) -ec '$(CXX) -MM $(CPPFLAGS) $< \
		      | sed '\''s/$*\\.o[ :]*/& $@/g'\'' > $@'

# }}} ################################################################
# {{{ Explicit Rules

all: $(EXENAME)

$(EXENAME): $(EXEDEP) $(EXTDEP)
	$(CXX)	$(LDFLAGS) $(EXTDEP) $(EXEDEP) $(LIBS) -o $(EXENAME)

clean:
	-rm -f $(DIRTYFILES)

-include $(SRCDEP)

# }}} ################################################################

# {{{ Emacs Locals
# Local Variables:
# folded-file: t
# End:
# }}}

