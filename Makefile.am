## Process this file with automake to produce Makefile.in

SUBDIRS = src data doc

EXTRA_DIST = COPYING.GPL COPYING.WTFPL PACKAGERS bootstrap

if HAVE_NONFREE
else
dist-hook:
	@echo ""
	@echo "          ===================================================="
	@echo "            WARNING: This tarball will exclude non-free data"
	@echo "          ===================================================="
	@echo ""
endif

