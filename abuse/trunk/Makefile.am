## Process this file with automake to produce Makefile.in

SUBDIRS = src data

man_MANS = abuse.6 abuse-tool.6

datafiledir = $(datadir)/games/abuse
datafile_DATA = abuse.bmp abuse.png

EXTRA_DIST = PACKAGERS bootstrap abuse.bmp abuse.png

if HAVE_NONFREE
else
dist-hook:
	@echo ""
	@echo "          ===================================================="
	@echo "            WARNING: This tarball will exclude non-free data"
	@echo "          ===================================================="
	@echo ""
endif

