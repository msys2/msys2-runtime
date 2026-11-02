/* tls_pbuf.cc

This software is a copyrighted work licensed under the terms of the
Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
details. */

#include <winsup.h>
#include <malloc.h>
#include "tls_pbuf.h"

#define tls_pbuf	_my_tls.locals.pathbufs

void
tls_pathbuf::create ()
{
  /* Keep these buffers outside the user heap copied by fork.  Allocating
     them high also prevents thread teardown from opening a low DLL mapping
     hole while another thread is loading a library. */
  SIZE_T size = NT_MAX_PATH * (TP_NUM_C_BUFS
			     + TP_NUM_W_BUFS * sizeof (WCHAR));
  buffer_base = (char *) VirtualAlloc (NULL, size, MEM_RESERVE | MEM_TOP_DOWN,
				      PAGE_READWRITE);
  if (!buffer_base)
    api_fatal ("Internal error: Can't reserve temporary path buffers, %E");
  if ((uintptr_t) buffer_base < 0x100000000UL)
    api_fatal ("Internal error: Temporary path buffers below 4 GiB (%p)",
	       buffer_base);
}

void
tls_pathbuf::destroy ()
{
  if (buffer_base && !VirtualFree (buffer_base, 0, MEM_RELEASE))
    system_printf ("Can't release temporary path buffers at %p, %E",
		   buffer_base);
  else
    clear ();
}

char *
tmp_pathbuf::c_get ()
{
  if (!tls_pbuf.buffer_base)
    tls_pbuf.create ();
  if (tls_pbuf.c_cnt >= TP_NUM_C_BUFS)
    api_fatal ("Internal error: TP_NUM_C_BUFS too small: %u", TP_NUM_C_BUFS);
  if (!tls_pbuf.c_buf[tls_pbuf.c_cnt])
    {
      char *buf = tls_pbuf.buffer_base + tls_pbuf.c_cnt * NT_MAX_PATH;
      if (!VirtualAlloc (buf, NT_MAX_PATH, MEM_COMMIT, PAGE_READWRITE))
	api_fatal ("Internal error: Can't commit new path buf, %E");
      tls_pbuf.c_buf[tls_pbuf.c_cnt] = buf;
    }
  return tls_pbuf.c_buf[tls_pbuf.c_cnt++];
}

PWCHAR
tmp_pathbuf::w_get ()
{
  if (!tls_pbuf.buffer_base)
    tls_pbuf.create ();
  if (tls_pbuf.w_cnt >= TP_NUM_W_BUFS)
    api_fatal ("Internal error: TP_NUM_W_BUFS too small: %u.", TP_NUM_W_BUFS);
  if (!tls_pbuf.w_buf[tls_pbuf.w_cnt])
    {
      char *buf = tls_pbuf.buffer_base
		  + NT_MAX_PATH * (TP_NUM_C_BUFS
				   + tls_pbuf.w_cnt * sizeof (WCHAR));
      if (!VirtualAlloc (buf, NT_MAX_PATH * sizeof (WCHAR), MEM_COMMIT,
			 PAGE_READWRITE))
	api_fatal ("Internal error: Can't commit new wide path buf, %E");
      tls_pbuf.w_buf[tls_pbuf.w_cnt] = (PWCHAR) buf;
    }
  return tls_pbuf.w_buf[tls_pbuf.w_cnt++];
}
