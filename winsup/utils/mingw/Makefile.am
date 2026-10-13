# Makefile for Cygwin utilities

# This file is part of Cygwin.

# This software is a copyrighted work licensed under the terms of the
# Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
# details.

# We put utilities built with a MinGW compiler in a separate Makefile here
# because it's tricky with Automake to use different compilers for the same
# language in the same Makefile.

override CXX = @MINGW_CXX@
AM_CPPFLAGS =

include $(top_srcdir)/Makefile.am.common

CFLAGS_COMMON=-Wimplicit-fallthrough=4 -Werror -D_WIN32_WINNT=0x0a00 -DNTDDI_VERSION=WDK_NTDDI_VERSION
AM_CXXFLAGS=-fno-exceptions -fno-rtti -fno-use-cxa-atexit $(flags_common) $(CFLAGS_COMMON)

AM_LDFLAGS = -static

if BUILD_MINGW_UTILS
# --enable-mingw-utils: build only the MinGW host-tool subset that has been
# ported/validated for the ARM64 cross build (cygwin-console-helper, ldh).
# cygcheck and strace are added here as they migrate onto this path; until then
# they are built by their own recipe.  The cygwin-linked utils in ../ stay
# disabled.
bin_PROGRAMS = \
	cygwin-console-helper \
	ldh
else
bin_PROGRAMS = \
	cygcheck \
	cygwin-console-helper \
	ldh \
	strace
endif

# The x86 getprocaddr helpers require the i686/x86_64 MinGW compilers; the ARM64
# helper requires the aarch64 one.  Build each only when its compiler is present
# so a host lacking some of them (e.g. the ARM64 cross build) still configures.
libexec_PROGRAMS =
if HAVE_MINGW32_CC
libexec_PROGRAMS += getprocaddr32
endif
if HAVE_MINGW64_CC
libexec_PROGRAMS += getprocaddr64
endif
if HAVE_MINGWARM64_CC
libexec_PROGRAMS += getprocaddrarm64
endif

# Must *not* use -O2 here, as it screws up the stack backtrace
getprocaddr32.o: %32.o: %.c
	$(MINGW32_CC) -c -o $@ $<

getprocaddr32.exe: %.exe: %.o
	$(MINGW32_CC) -o $@ $^ -static -ldbghelp

getprocaddr64.o: %64.o: %.c
	$(MINGW64_CC) -c -o $@ $<

getprocaddr64.exe: %.exe: %.o
	$(MINGW64_CC) -o $@ $^ -static -ldbghelp

# Native ARM64 helper, used to gently kill native ARM64 (PID_NOTCYGWIN)
# processes on Windows-on-ARM.  Only built when the aarch64 MinGW compiler is
# available (HAVE_MINGWARM64_CC, see configure.ac).
getprocaddrarm64.o: %arm64.o: %.c
	$(MINGWARM64_CC) -c -o $@ $<

getprocaddrarm64.exe: %.exe: %.o
	$(MINGWARM64_CC) -o $@ $^ -static -ldbghelp

# cygwin-console-helper and ldh are in the mingw-utils subset, so their
# sources/flags are defined unconditionally (built in both modes).
cygwin_console_helper_SOURCES = cygwin-console-helper.cc

ldh_SOURCES = ldh.cc
ldh_LDFLAGS = ${AM_LDFLAGS} -Wl,--disable-high-entropy-va

# The following targets and their per-target flags are only defined when the
# full MinGW host-tool set is built (not the --enable-mingw-utils subset), so
# automake does not warn about per-target flags for programs that are not built.
if !BUILD_MINGW_UTILS
cygcheck_SOURCES = \
	bloda.cc \
	cygcheck.cc \
	dump_setup.cc \
	path.cc
cygcheck_CPPFLAGS=-I$(srcdir)/.. -idirafter ${top_srcdir}/cygwin/include
cygcheck_LDFLAGS = ${AM_LDFLAGS} -Wl,--disable-high-entropy-va
cygcheck_LDADD = -lz -lwininet -lshlwapi -lpsapi -lntdll

strace_SOURCES = \
	path.cc \
	strace.cc
strace_CPPFLAGS=-I$(srcdir)/.. -idirafter ${top_srcdir}/cygwin/local_includes -idirafter ${top_srcdir}/cygwin/include
strace_LDFLAGS = ${AM_LDFLAGS} -Wl,--disable-high-entropy-va
strace_LDADD = -lntdll

noinst_PROGRAMS = path-testsuite

path_testsuite_SOURCES = \
	path.cc \
	testsuite.cc
path_testsuite_CPPFLAGS=-I$(srcdir)/.. -idirafter ${top_srcdir}/cygwin/include
path_testsuite_CXXFLAGS = -DTESTSUITE

TESTS = path-testsuite
endif
