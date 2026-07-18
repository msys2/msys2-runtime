/* exception.h

This software is a copyrighted work licensed under the terms of the
Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
details. */

#pragma once

#define exception_list void
#ifndef __aarch64__
typedef struct _DISPATCHER_CONTEXT *PDISPATCHER_CONTEXT;
#endif

class exception
{
  static EXCEPTION_DISPOSITION myfault (EXCEPTION_RECORD *, exception_list *,
					CONTEXT *, PDISPATCHER_CONTEXT);
  static EXCEPTION_DISPOSITION handle (EXCEPTION_RECORD *, exception_list *,
				       CONTEXT *, PDISPATCHER_CONTEXT);
#ifndef __aarch64__
public:
  exception () __attribute__ ((always_inline))
  {
    /* Install SEH handler. */
    asm volatile ("\n\
    1:									\n\
      .seh_handler							  \
	_ZN9exception6handleEP17_EXCEPTION_RECORDPvP8_CONTEXTP19_DISPATCHER_CONTEXT,	  \
	@except								\n\
      .seh_handlerdata							\n\
      .long 1								\n\
      .rva 1b, 2f, 2f, 2f						\n\
      .seh_code								\n");
  };
  ~exception () __attribute__ ((always_inline))
  {
    asm volatile ("\n\
      nop								\n\
    2:									\n\
      nop								\n");
  }
#else
public:
  /* Clang emits native ARM64 SEH for the __try macros below.  The handful of
     legacy RAII guard sites need no inline scope-table fragment on ARM64. */
  exception () = default;
  ~exception () = default;
#endif
};

LONG CALLBACK myfault_altstack_handler (EXCEPTION_POINTERS *);

class cygwin_exception
{
  PUINT_PTR framep;
  PCONTEXT ctx;
  EXCEPTION_RECORD *e;
  HANDLE h;
  void dump_exception ();
  void open_stackdumpfile ();
public:
  cygwin_exception (PUINT_PTR in_framep, PCONTEXT in_ctx = NULL, EXCEPTION_RECORD *in_e = NULL):
    framep (in_framep), ctx (in_ctx), e (in_e), h (NULL) {}
  void dumpstack ();
  PCONTEXT context () const {return ctx;}
  EXCEPTION_RECORD *exception_record () const {return e;}
};
