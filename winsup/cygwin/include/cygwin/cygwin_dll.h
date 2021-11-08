/* cygwin_dll.h

This file is part of Cygwin.

This software is a copyrighted work licensed under the terms of the
Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
details. */

#ifndef __CYGWIN_CYGWIN_DLL_H__
#define __CYGWIN_CYGWIN_DLL_H__

#include <windows.h>

#ifdef __cplusplus
#define CDECL_BEGIN extern "C" {
#define CDECL_END }
#else
#define CDECL_BEGIN
#define CDECL_END
#endif

#define DECLARE_CYGWIN_DLL(Entry)					      \
									      \
CDECL_BEGIN								      \
  int WINAPI Entry (HINSTANCE h, DWORD reason, void *ptr);	              \
  typedef int (*mainfunc) (int, char **, char **);			      \
  extern PVOID msys_attach_dll (HMODULE, mainfunc);			      \
  extern void msys_detach_dll (PVOID);				      \
CDECL_END								      \
									      \
static HINSTANCE storedHandle;						      \
static DWORD storedReason;						      \
static void* storedPtr;							      \
int __dynamically_loaded;						      \
									      \
static int __dllMain (int a __attribute__ ((__unused__)),		      \
		      char **b __attribute__ ((__unused__)),		      \
		      char **c __attribute__ ((__unused__)))		      \
{									      \
  return Entry (storedHandle, storedReason, storedPtr);		              \
}									      \
									      \
static PVOID dll_index;							      \
									      \
int WINAPI _msys_dll_entry (HINSTANCE h, DWORD reason, void *ptr)	      \
{									      \
  int ret;								      \
  ret = 1;								      \
									      \
  switch (reason)							      \
  {									      \
    case DLL_PROCESS_ATTACH:						      \
    {									      \
      storedHandle = h;							      \
      storedReason = reason;						      \
      storedPtr = ptr;							      \
      __dynamically_loaded = (ptr == NULL);				      \
      dll_index = msys_attach_dll (h, &__dllMain);			      \
      if (dll_index == (PVOID) -1)					      \
	ret = 0;							      \
    }									      \
    break;								      \
									      \
    case DLL_PROCESS_DETACH:						      \
    {									      \
      ret = Entry (h, reason, ptr);					      \
      if (ret)								      \
      {									      \
	msys_detach_dll (dll_index);					      \
	dll_index = (PVOID) -1;						      \
      }									      \
    }									      \
    break;								      \
									      \
    case DLL_THREAD_ATTACH:						      \
    {									      \
      ret = Entry (h, reason, ptr);					      \
    }									      \
    break;								      \
									      \
    case DLL_THREAD_DETACH:						      \
    {									      \
      ret = Entry (h, reason, ptr);					      \
    }									      \
    break;								      \
  }									      \
  return ret;								      \
}									      \
									      \
/* OBSOLETE: This is only provided for source level compatibility. */         \
int WINAPI _msys_nonmsys_dll_entry (HINSTANCE h, DWORD reason, void *ptr) \
{									      \
  return _msys_dll_entry (h, reason, ptr);				      \
}									      \

#endif /* __CYGWIN_CYGWIN_DLL_H__ */
