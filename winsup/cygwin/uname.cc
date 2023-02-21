/* uname.cc
   Written by Steve Chamberlain of Cygnus Support, sac@cygnus.com
   Rewritten by Geoffrey Noer of Cygnus Solutions, noer@cygnus.com

This file is part of Cygwin.

This software is a copyrighted work licensed under the terms of the
Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
details. */

#include "winsup.h"
#include <stdio.h>
#include <sys/utsname.h>
#include <netdb.h>
#include "cygwin_version.h"
#include "cygtls.h"

extern "C" int cygwin_gethostname (char *__name, size_t __len);
extern "C" int getdomainname (char *__name, size_t __len);

#if __GNUC__ >= 8
#define ATTRIBUTE_NONSTRING __attribute__ ((nonstring))
#else
#define ATTRIBUTE_NONSTRING
#endif

static const char*
get_sysname()
{
#ifdef __MSYS__
  char* msystem = getenv("MSYSTEM");
  if (!msystem || strcmp(msystem, "MSYS") == 0)
    return "MSYS";
  else if (strcmp(msystem, "CYGWIN") == 0)
    return "CYGWIN";
  else if (strstr(msystem, "32") != NULL)
    return "MINGW32";
  else
    return "MINGW64";
#else
  return "CYGWIN";
#endif
}

/* uname: POSIX 4.4.1.1 */

/* New entrypoint for applications since API 335 */
extern "C" int
uname_x (struct utsname *name)
{
  __try
    {
      char buf[NI_MAXHOST + 1] ATTRIBUTE_NONSTRING;

      memset (name, 0, sizeof (*name));
      /* sysname */
      const char* sysname = get_sysname();
      __small_sprintf (name->sysname, "%s_%s-%u",
		       sysname,
		       wincap.osname (), wincap.build_number ());
      /* nodename */
      memset (buf, 0, sizeof buf);
      cygwin_gethostname (buf, sizeof buf - 1);
      strncat (name->nodename, buf, sizeof (name->nodename) - 1);
      /* machine */
      switch (wincap.cpu_arch ())
	{
	  case PROCESSOR_ARCHITECTURE_AMD64:
	    strcpy (name->machine, "x86_64");
	    break;
	  default:
	    strcpy (name->machine, "unknown");
	    break;
	}
      /* release */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation="
#ifdef CYGPORT_RELEASE_INFO
      snprintf (name->release, _UTSNAME_LENGTH, "%s-%s.%s",
		__XSTRING (CYGPORT_RELEASE_INFO), MSYS2_RUNTIME_COMMIT_SHORT, name->machine);
#else
      extern const char *uname_dev_version;
      if (uname_dev_version && uname_dev_version[0])
	snprintf (name->release, _UTSNAME_LENGTH, "%s-%s.%s",
		  uname_dev_version, MSYS2_RUNTIME_COMMIT_SHORT, name->machine);
      else
	__small_sprintf (name->release, "%d.%d.%d-%s-api-%d.%s",
			 cygwin_version.dll_major / 1000,
			 cygwin_version.dll_major % 1000,
			 cygwin_version.dll_minor,
			 MSYS2_RUNTIME_COMMIT_SHORT,
			 cygwin_version.api_minor,
			 name->machine);
#endif
#pragma GCC diagnostic pop
      /* version */
      stpcpy (name->version, cygwin_version.dll_build_date);
      strcat (name->version, " UTC");
      /* domainame */
      memset (buf, 0, sizeof buf);
      getdomainname (buf, sizeof buf - 1);
      strncat (name->domainname, buf, sizeof (name->domainname) - 1);
    }
  __except (EFAULT) { return -1; }
  __endtry
  return 0;
}

/* Old entrypoint for applications up to API 334 */
struct old_utsname
{
  char sysname[21];
  char nodename[20];
  char release[20];
  char version[20];
  char machine[20];
};

extern "C" int
uname (struct utsname *in_name)
{
  struct old_utsname *name = (struct old_utsname *) in_name;
  __try
    {
      memset (name, 0, sizeof (*name));
      const char* sysname = get_sysname();
      __small_sprintf (name->sysname, "%s_%s", sysname, wincap.osname ());

      /* Computer name */
      cygwin_gethostname (name->nodename, sizeof (name->nodename) - 1);

      /* Cygwin dll release */
      __small_sprintf (name->release, "%d.%d.%d(%d.%d/%d/%d/%s)",
		       cygwin_version.dll_major / 1000,
		       cygwin_version.dll_major % 1000,
		       cygwin_version.dll_minor,
		       cygwin_version.api_major,
		       cygwin_version.api_minor,
		       cygwin_version.shared_data,
		       cygwin_version.mount_registry,
		       MSYS2_RUNTIME_COMMIT_SHORT);

      /* Cygwin "version" aka build date */
      strcpy (name->version, cygwin_version.dll_build_date);

      /* CPU type */
      switch (wincap.cpu_arch ())
	{
	  case PROCESSOR_ARCHITECTURE_AMD64:
	    strcpy (name->machine, "x86_64");
	    break;
	  default:
	    strcpy (name->machine, "unknown");
	    break;
	}
    }
  __except (EFAULT)
    {
      return -1;
    }
  __endtry
  return 0;
}
