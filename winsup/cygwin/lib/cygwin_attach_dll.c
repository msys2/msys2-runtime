/* attach_dll.cc: crt0 for attaching cygwin DLL from a non-cygwin app.

This file is part of Cygwin.

This software is a copyrighted work licensed under the terms of the
Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
details. */

#undef __INSIDE_CYGWIN__
#include "winlean.h"
#include <time.h>	/* Needed since call to sys/time.h via sys/cygwin.h
			   complains otherwise */
#include <sys/cygwin.h>
#include "crt0.h"

/* for a loaded dll */
PVOID
#ifdef __MSYS__
msys_attach_dll (HMODULE h, MainFunc f)
#else
cygwin_attach_dll (HMODULE h, MainFunc f)
#endif
{
  static struct per_process u;
#ifdef __MSYS__
  (void) _msys_crt0_common (f, &u);
#else
  (void) _cygwin_crt0_common (f, &u);
#endif

  /* jump into the dll. */
  return dll_dllcrt0 (h, &u);
}
