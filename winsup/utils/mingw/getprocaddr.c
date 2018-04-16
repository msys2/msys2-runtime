/* getprocaddr.c

This program is a helper for getting the pointers for the
functions in kernel32 module, and optionally injects a remote
thread that runs those functions given a pid and exit code.

We use dbghelp.dll to get the pointer to kernel32!CtrlRoutine
because it isn't exported. For that, we try to generate console
event (Ctrl+Break) ourselves, to find the pointer, and it is
printed if asked to, or a remote thread is injected to run the
given function.

This software is a copyrighted work licensed under the terms of the
Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
details. */

#include <stdio.h>
#include <windows.h>

/* Include dbghelp.h after windows.h */
#include <dbghelp.h>

static DWORD pid;
static uintptr_t exit_code;
static HANDLE CtrlEvent;

static int
inject_remote_thread_into_process (HANDLE process,
                                   LPTHREAD_START_ROUTINE address,
                                   uintptr_t exit_code,
                                   DWORD *thread_return)
{
  int res = -1;

  if (!address)
    return res;
  DWORD thread_id;
  HANDLE thread = CreateRemoteThread (process, NULL, 1024 * 1024, address,
                                      (PVOID)exit_code, 0, &thread_id);
  if (thread)
    {
      /*
       * Wait up to 10 seconds (arbitrary constant) for the thread to finish;
       * Maybe we should wait forever? I have seen Cmd does so, but well...
       */
      if (WaitForSingleObject (thread, 10000) == WAIT_OBJECT_0)
        res = 0;
      /*
      According to the docs at MSDN for GetExitCodeThread, it will
      get the return value from the function, here CtrlRoutine. So, this
      checks if the Ctrl Event is handled correctly by the process.

      By some testing I could see CtrlRoutine returns 0 in case where
      CtrlEvent set by SetConsoleCtrlHandler is handled correctly, in all
      other cases it returns something non-zero(not sure what it that).
      */
      if (thread_return != NULL)
        GetExitCodeThread (thread, thread_return);

      CloseHandle (thread);
    }

  return res;
}

/* Here, we send a CtrlEvent to the current process for the
 * sole purpose of capturing the address of the CtrlRoutine
 * function, by looking the stack trace.
 *
 * This hack is needed because we cannot use GetProcAddress()
 * as we do for ExitProcess(), because CtrlRoutine is not
 * exported (although the .pdb files ensure that we can see
 * it in a debugger).
 */
static WINAPI BOOL
ctrl_handler (DWORD ctrl_type)
{
  unsigned short count;
  void *address;
  HANDLE process;
  PSYMBOL_INFOW info;
  DWORD64 displacement;
  DWORD thread_return = 0;

  count = CaptureStackBackTrace (1l /* skip this function */,
                                 1l /* return only one trace item */, &address,
                                 NULL);
  if (count != 1)
    {
      fprintf (stderr, "Could not capture backtrace\n");
      return FALSE;
    }

  process = GetCurrentProcess ();
  if (!SymInitialize (process, NULL, TRUE))
    {
      fprintf (stderr, "Could not initialize symbols\n");
      return FALSE;
    }

  info = (PSYMBOL_INFOW)malloc (sizeof (*info)
                                + MAX_SYM_NAME * sizeof (wchar_t));
  if (!info)
    {
      fprintf (stderr, "Could not allocate symbol info structure\n");
      return FALSE;
    }
  info->SizeOfStruct = sizeof (*info);
  info->MaxNameLen = MAX_SYM_NAME;

  if (!SymFromAddrW (process, (DWORD64) (intptr_t)address, &displacement,
                     info))
    {
      fprintf (stderr, "Could not get symbol info\n");
      SymCleanup (process);
      return FALSE;
    }

  if (pid == 0)
    {
      printf ("%p\n", (void *)(intptr_t)info->Address);
    }
  else
    {
      LPTHREAD_START_ROUTINE address =
        (LPTHREAD_START_ROUTINE) (intptr_t)info->Address;
      HANDLE h = OpenProcess (PROCESS_CREATE_THREAD |
                              PROCESS_QUERY_INFORMATION |
                              PROCESS_VM_OPERATION |
                              PROCESS_VM_WRITE |
                              PROCESS_VM_READ, FALSE, pid);
      if (h == NULL)
        {
          fprintf (stderr, "OpenProcess failed: %ld\n", GetLastError ());
          return 1;
        }
      /* Inject the remote thread only when asked to */
      if (inject_remote_thread_into_process (h, address, exit_code,
                                             &thread_return) < 0)
        {
          fprintf (stderr,
                   "Error while injecting remote thread for pid(%lu)\n", pid);
          exit (1); /*We should exit immediately or else there will a 10s hang
                       waiting for the event to happen.*/
        }
      if (thread_return)
        fprintf (stderr,
                 "Injected remote thread for pid(%lu) returned %lu\n", pid,
                 thread_return);
    }
  SymCleanup (process);
  if (!SetEvent (CtrlEvent))
    {
      fprintf (stderr, "SetEvent failed (%ld)\n", GetLastError ());
      return 1;
    }
  exit (thread_return != 0);
}

/* The easy route for finding the address of CtrlRoutine
 * would be use GetProcAddress() but this isn't viable
 * here because that symbol isn't exported.
 */
static int
find_ctrl_routine_the_hard_way ()
{
  /*
   * Avoid terminating all processes attached to the current console;
   * This would happen if we used the same console as the caller, though,
   * because we are sending a CtrlEvent on purpose (which _is_ sent to
   * all processes connected to the same console, and the other processes
   * are most likely unprepared for that CTRL_BREAK_EVENT and would be
   * terminated as a consequence, _including the caller_).
   *
   * In case we get only one result from GetConsoleProcessList(), we don't
   * need to create and allocate a new console, and it could avoid a console
   * window popping up.
   */
  DWORD proc_lists;
  if (GetConsoleProcessList (&proc_lists, 5) > 1)
    {
      if (!FreeConsole () && GetLastError () != ERROR_INVALID_PARAMETER)
        {
          fprintf (stderr, "Could not detach from current Console: %ld\n",
                   GetLastError ());
          return 1;
        }
      if (!AllocConsole ())
        {
          fprintf (stderr, "Could not allocate a new Console\n");
          return 1;
        }
    }

  CtrlEvent = CreateEvent (NULL,  // default security attributes
                           TRUE,  // manual-reset event
                           FALSE, // initial state is nonsignaled
                           NULL   // object name
  );

  if (CtrlEvent == NULL)
    {
      fprintf (stderr, "CreateEvent failed (%ld)\n", GetLastError ());
      return 1;
    }


  if (!SetConsoleCtrlHandler (ctrl_handler, TRUE))
    {
      fprintf (stderr, "Could not register Ctrl handler\n");
      return 1;
    }

  if (!GenerateConsoleCtrlEvent (CTRL_BREAK_EVENT, 0))
    {
      fprintf (stderr, "Could not simulate Ctrl+Break\n");
      return 1;
    }

  if (WaitForSingleObject (CtrlEvent, 10000 /* 10 seconds*/) != WAIT_OBJECT_0)
    {
      fprintf (stderr, "WaitForSingleObject failed (%ld)\n", GetLastError ());
      return 1;
    }
  return 0;
}

static void *
get_proc_addr (const char * module_name, const char * function_name)
{
      HMODULE module = GetModuleHandle (module_name);
      if (!module)
        return NULL;
      return (void *)GetProcAddress (module, function_name);
}

int
main (int argc, char **argv)
{
  char *end;
  void *address;
  BOOL is_ctrl_routine;
  DWORD thread_return = 0;

  if (argc == 4)
    {
      exit_code = atoi (argv[2]);
      pid = strtoul (argv[3], NULL, 0);
    }
  else if (argc == 2)
    {
      pid = 0;
    }
  else
    {
      fprintf (stderr, "Need a function name, exit code and pid\n"
               "Or needs a function name.\n");
      return 1;
    }

  is_ctrl_routine = strcmp (argv[1], "CtrlRoutine") == 0;
  address = get_proc_addr ("kernel32", argv[1]);
  if (is_ctrl_routine && !address)
    {
      /* CtrlRoutine is undocumented, and has been seen in both
       * kernel32 and kernelbase
       */
      address = get_proc_addr ("kernelbase", argv[1]);
      if (!address)
        return find_ctrl_routine_the_hard_way ();
    }

  if (!address)
    {
      fprintf (stderr, "Could not get proc address\n");
      return 1;
    }

  if (pid == 0)
    {
      printf ("%p\n", address);
      fflush (stdout);
      return 0;
    }
  HANDLE h = OpenProcess (PROCESS_CREATE_THREAD |
                          PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION |
                          PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pid);
  if (h == NULL)
    {
      fprintf (stderr, "OpenProcess failed: %ld\n", GetLastError ());
      return 1;
    }
  /* Inject the remote thread */
  if (inject_remote_thread_into_process (h, (LPTHREAD_START_ROUTINE)address,
                                         exit_code, &thread_return) < 0)
    {
      fprintf (stderr, "Could not inject thread into process %lu\n", pid);
      return 1;
    }

  if (is_ctrl_routine && thread_return)
    {
      fprintf (stderr,
               "Injected remote thread for pid %lu returned %lu\n", pid,
               thread_return);
      return 1;
    }

  return 0;
}
