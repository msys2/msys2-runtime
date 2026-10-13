# Makefile.am for winsup stuff
#
# This file is part of Cygwin.
#
# This software is a copyrighted work licensed under the terms of the
# Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
# details.

# This makefile requires GNU make.

cygdocdir = $(datarootdir)/doc/Cygwin

cygdoc_DATA = \
	CYGWIN_LICENSE \
	COPYING

SUBDIRS = cygwin testsuite

if BUILD_CYGSERVER
SUBDIRS += cygserver
endif

if BUILD_UTILS
SUBDIRS += utils
else
# Enter utils/ for the MinGW host-tool subset even when the cygwin-linked utils
# are disabled (--enable-mingw-utils; utils/Makefile.am descends only into mingw).
if BUILD_MINGW_UTILS
SUBDIRS += utils
endif
endif

if BUILD_DOC
SUBDIRS += doc
endif

testsuite: cygwin

if BUILD_CYGSERVER
cygserver: cygwin
endif

if BUILD_UTILS
utils: cygwin
else
if BUILD_MINGW_UTILS
utils: cygwin
endif
endif
