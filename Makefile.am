
include $(top_srcdir)/build/autotools/common.am

SUBDIRS = src data doc

EXTRA_DIST += COPYING.GPL COPYING.WTFPL PACKAGERS bootstrap

dist-hook:
	@echo ""
	@echo "          ===================================================="
if HAVE_NONFREE
	@echo "            WARNING: This tarball will contain non-free data"
else
	@echo "            WARNING: This tarball will exclude non-free data"
endif
	@echo "          ===================================================="
	@echo ""

