/*!
   Copyright (c) 2008, Onbings All rights reserved.

   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
   PURPOSE.

   Remarks

   Name:			        ScriptLauncher.cpp
   Author:		        Bernard HARMEL: onbings@dscloud.me

   Summary:

    The class that manages launching script from
    within C code

   History:
    V 1.00  September 25 2013  BHA : Original version
 */
#include <bofstd/boffs.h>
#include <bofstd/bofprocess.h>
#include <bofstd/bofsystem.h>

#include <cstdio>
// #include <string.h>

#if defined(_WIN32)
	#define popen _popen
	#define pclose _pclose
	/* If WIFEXITED(STATUS), NOT IN WINDOWS:the low-order 8 bits of the status.*/
	#define WEXITSTATUS(status) (status)

	/* If WIFSIGNALED(STATUS), the terminating signal.  */
	#define WTERMSIG(status) ((status)&0x7f)

	/* If WIFSTOPPED(STATUS), the signal that stopped the child.  */
	#define WSTOPSIG(status) WEXITSTATUS(status)

	/* Nonzero if STATUS indicates normal termination.  */
	#define WIFEXITED(status) (WTERMSIG(status) == 0)
#else
	#include <fcntl.h>
	#include <poll.h>
	#include <spawn.h>
	#include <stdio.h>
	#include <stdlib.h>
  #include <string.h>
  #include <signal.h>
#if defined(__EMSCRIPTEN__)
	#else
		#include <sys/ptrace.h>
	#endif
	#include <sys/types.h>
	#include <sys/wait.h>
	extern char **environ;
#endif

BEGIN_BOF_NAMESPACE()

// #define SCRIPT_LAUNCHER_INTERNAL_ERROR (-1) BOF_ERR_INTERNAL
// #define SCRIPT_LAUNCHER_PROCESS_CREATION_ERROR (-2) BOF_ERR_CREATE
// #define SCRIPT_LAUNCHER_INVALID_ARGUMENTS_ERROR (-3) BOF_ERR_EINVAL
// #define SCRIPT_LAUNCHER_TIMEOUT_ERROR (-4) BOF_ERR_ETIMEDOUT
// #define SCRIPT_LAUNCHER_PROCESS_ABNORMAL_TERMINATION_ERROR (-5) BOF_ERR_RESET
// #define SCRIPT_LAUNCHER_NOT_SUPPORTED (-6) BOF_ERR_NOT_SUPPORTED
uint32_t BofProcess::S_mDefaultTimeout_U32 = 120000;
BofProcess::ExecuteMode BofProcess::S_mDefaultMode_E = BofProcess::EXECUTE_POPEN;

/*!
   Description
   This function waits for the
   specified process ID (PID).

   Parameters
   _Pid_X        - The PID to wait for
   _pStatus_i    - The pointer to where to store the process exit code
   _Timeout_U32  - The timeout value in ms

   Returns
   > 0 - The value of the PID that completed
    0 - The process has still not finished processing
   < 0 - An error occurred

   Remarks
   None
 */
BOF_PROCESS WaitForPid(BOF_PROCESS _Pid_X, int *_pStatus_i, uint32_t _Timeout_U32)
{
  BOF_PROCESS RetPid_X;
  uint32_t Start_U32, Delta_U32;

  RetPid_X.Pid = -1;
  if (_Pid_X.Pid > 0)
  {
    Start_U32 = Bof_GetMsTickCount();
    if (_pStatus_i)
    {
      do
      {
#if defined(_WIN32)
        RetPid_X.Pid = _Pid_X.Pid;
        switch (WaitForSingleObject(_Pid_X.Pi_X.hProcess, 0))
        {
          default:
          case WAIT_ABANDONED:
          case WAIT_FAILED:
            *_pStatus_i = -1;
            break;

          case WAIT_OBJECT_0:
            // process has terminated...
            *_pStatus_i = 0;
            break;

          case WAIT_TIMEOUT:
            // process is still running...
            *_pStatus_i = 0;
            break;
        }
#else
        RetPid_X.Pid = waitpid(_Pid_X.Pid, _pStatus_i, WNOHANG);
#endif
        Delta_U32 = Bof_ElapsedMsTime(Start_U32);

        if ((RetPid_X.Pid == -1) || (RetPid_X.Pid == _Pid_X.Pid) || (Delta_U32 >= _Timeout_U32))
        {
          break;
        }

        Bof_MsSleep(500);
      } while (RetPid_X.Pid != _Pid_X.Pid);
    }
  }
  return RetPid_X;
}

/*!
   Description
   This function checks for
   subprocesses in the tree
   and try to kill them

   Parameters
   _Pid_X      - The PID
   _Options_i  - The kill options

   Returns
   0 - The operation was successful
   -1 - The operation failed

   Remarks
   None
 */
int KillSubProcess(BOF_PROCESS _Pid_X, int _Options_i)
{
  int Rts_i = -1;
  if (_Pid_X.Pid > 0)
  {
#if defined(_WIN32)
    Rts_i = (TerminateProcess(_Pid_X.Pi_X.hProcess, -1)) ? 0 : -1;
#else
    char pCommand_c[0x1000];
    char pResult_c[0x1000];
    char *pLine_c;
    BOF_PROCESS Pid_X, ChildPid_X;
    BOFERR Sts_E;
    int ExitCode_i;

    Rts_i = -1;
    snprintf(pCommand_c, sizeof(pCommand_c), "ps -o ppid,pid | grep %d | awk '{ print $2 }'", _Pid_X.Pid);
    Sts_E = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), pCommand_c, 1000, Pid_X, ExitCode_i);
    Rts_i = ExitCode_i;
    if ((Sts_E == BOF_ERR_NO_ERROR) && (Rts_i == 0))
    {
      pLine_c = strtok(pResult_c, "\n");
      while (pLine_c != nullptr)
      {
        ChildPid_X.Pid = atoi(pLine_c);

        if (ChildPid_X.Pid != _Pid_X.Pid)
        {
          Rts_i |= kill(ChildPid_X.Pid, _Options_i);
        }

        pLine_c = strtok(nullptr, "\n");
      }
    }
#endif
  }
  return Rts_i;
}

/*!
   Description
   This function waits for the
   the completion of the specified
   process. If the process did not
   completed upon the specified timeout,
   the process is terminated.

   Parameters
   _Pid_X        - The PID to wait for
   _Timeout_U32  - The timeout value in ms

   Returns
   BOF_ERR_RESET (-5) - The process did not terminated normally
   BOF_ERR_ETIMEDOUT                  (-4) - The process did time out

   The return code of the process otherwise

   Remarks
   None
 */
BOFERR WaitForProcessCompletion(BOF_PROCESS _Pid_X, uint32_t _Timeout_U32, int &_rExitCode_i)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BOF_PROCESS RetPid_X;
  bool Kill_B = false;

  RetPid_X.Pid = -1;
  _rExitCode_i = 127;
  if (_Pid_X.Pid > 0)
  {
#if defined(_WIN32)
    int Status_i = -1;
    RetPid_X = WaitForPid(_Pid_X, &Status_i, _Timeout_U32);

    // Check if process exited
    if (RetPid_X.Pid == _Pid_X.Pid)
    {
      if (WIFEXITED(Status_i))
      {
        Status_i = WEXITSTATUS(Status_i);
        Rts_E = (Status_i == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_EBADRQC;
        _rExitCode_i = Status_i;
      }
      else
      {
        Kill_B = true;
        Rts_E = BOF_ERR_RESET;
      }
    }
    else
    {
      Rts_E = BOF_ERR_ETIMEDOUT;
      Kill_B = true;
    }

    if (Kill_B)
    {
      _rExitCode_i = (TerminateProcess(_Pid_X.Pi_X.hProcess, -1)) ? 0 : -1;
    }
#else
    RetPid_X = WaitForPid(_Pid_X, &_rExitCode_i, _Timeout_U32);
    Rts_E = BOF_ERR_EKEYEXPIRED;
    // Check if process exited
    if (RetPid_X.Pid == _Pid_X.Pid)
    {
      if (WIFEXITED(_rExitCode_i))
      {
        _rExitCode_i = WEXITSTATUS(_rExitCode_i);
        if (_rExitCode_i == 0)
        {
          Rts_E = BOF_ERR_NO_ERROR;
        }
      }
      else
      {
        Kill_B = true;
        Rts_E = BOF_ERR_RESET;
      }
    }
    else
    {
      Rts_E = BOF_ERR_ETIMEDOUT;
      Kill_B = true;
    }

    if (Kill_B)
    {
      // Try terminating it
      int Status_i;
      Status_i = KillSubProcess(_Pid_X, SIGTERM);
      Status_i = kill(_Pid_X.Pid, SIGTERM);

      RetPid_X = WaitForPid(_Pid_X, &Status_i, _Timeout_U32);

      if (RetPid_X.Pid != _Pid_X.Pid)
      {
        // Try killing it
        Status_i = KillSubProcess(_Pid_X, SIGKILL);
        Status_i = kill(_Pid_X.Pid, SIGKILL);

        RetPid_X = WaitForPid(_Pid_X, &Status_i, _Timeout_U32);
      }
    }
#endif
  }
  return Rts_E;
}

/*!
   Description
   This function sets the defaults timeout
   for the execute function

   Parameters
   _Timeout_U32  - The timeout value in ms

   Returns
   true  - The operation was successful
   false - The operation failed

   Remarks
   None
 */
BOFERR BofProcess::S_SetDefaultTimeout(uint32_t _Timeout_U32)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;

  BofProcess::S_mDefaultTimeout_U32 = _Timeout_U32;

  return Rts_E;
}

/*!
   Description
   This function returns the defaults timeout
   for the execute function

   Parameters
   None

   Returns
   The default timeout in ms for the execute function

   Remarks
   None
 */
uint32_t BofProcess::S_GetDefaultTimeout()
{
  return BofProcess::S_mDefaultTimeout_U32;
}

/*!
   Description
   This function sets the default
   execution mode function

   Parameters
   None

   Returns
   true  - The operation was successful
   false - The operation failed

   Remarks
   None
 */
BOFERR BofProcess::S_SetDefaultExecuteMode(BofProcess::ExecuteMode _Mode_E)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;

  BofProcess::S_mDefaultMode_E = _Mode_E;
  return Rts_E;
}

/*!
   Description
   This function returns the defaults
   execution mode function

   Parameters
   None

   Returns
   The default execution mode function

   Remarks
   None
 */
BofProcess::ExecuteMode BofProcess::S_GetDefaultExecuteMode()
{
  return BofProcess::S_mDefaultMode_E;
}

/*!
   Description
   This function executes the given
   command and catch the return code

   Parameters
   _pCommand_c  - The command to execute

   Returns
   The command return code or
   a script launcher error code

   Remarks
   None
 */
BOFERR BofProcess::S_Execute(const char *_pCommand_c, BOF_PROCESS &_rPid_X, int &_rExitCode_i)
{
  return BofProcess::S_Execute(nullptr, 0, _pCommand_c, BofProcess::S_mDefaultTimeout_U32, BofProcess::S_mDefaultMode_E, _rPid_X, _rExitCode_i);
}

/*!
   Description
   This function executes the given
   command and catch the return code

   Parameters
   _pCommand_c - The command to execute
   _Mode_E     - The execution mode

   Returns
   The command return code or
   a script launcher error code

   Remarks
   None
 */
BOFERR BofProcess::S_Execute(const char *_pCommand_c, BofProcess::ExecuteMode _Mode_E, BOF_PROCESS &_rPid_X, int &_rExitCode_i)
{
  return BofProcess::S_Execute(nullptr, 0, _pCommand_c, BofProcess::S_mDefaultTimeout_U32, _Mode_E, _rPid_X, _rExitCode_i);
}

/*!
   Description
   This function executes the given
   command and catch the return code

   Parameters
   _pCommand_c  - The command to execute
   _Timeout_U32 - The timeout value

   Returns
   The command return code or
   a script launcher error code

   Remarks
   None
 */
BOFERR BofProcess::S_Execute(const char *_pCommand_c, uint32_t _Timeout_U32, BOF_PROCESS &_rPid_X, int &_rExitCode_i)
{
  return BofProcess::S_Execute(nullptr, 0, _pCommand_c, _Timeout_U32, BofProcess::S_mDefaultMode_E, _rPid_X, _rExitCode_i);
}

/*!
   Description
   This function executes the given
   command and catch the return code

   Parameters
   _pCommand_c  - The command to execute
   _Timeout_U32 - The timeout value
   _Mode_E      - The execution mode

   Returns
   The command return code or
   a script launcher error code

   Remarks
   None
 */
BOFERR BofProcess::S_Execute(const char *_pCommand_c, uint32_t _Timeout_U32, BofProcess::ExecuteMode _Mode_E, BOF_PROCESS &_rPid_X, int &_rExitCode_i)
{
  return BofProcess::S_Execute(nullptr, 0, _pCommand_c, _Timeout_U32, _Mode_E, _rPid_X, _rExitCode_i);
}

/*!
   Description
   This function executes the given
   command and catch the return code

   Parameters
   _pOutput_c   - The pointer to where to store the command response
   _Size_U32    - The size in bytes of the response buffer
   _pCommand_c  - The command to execute

   Returns
   The command return code or
   a script launcher error code

   Remarks
   None
 */
BOFERR BofProcess::S_Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, BOF_PROCESS &_rPid_X, int &_rExitCode_i)
{
  return BofProcess::S_Execute(_pOutput_c, _Size_U32, _pCommand_c, BofProcess::S_mDefaultTimeout_U32, BofProcess::S_mDefaultMode_E, _rPid_X, _rExitCode_i);
}

/*!
   Description
   This function executes the given
   script and catch the return code
   and output characters

   Parameters
   _pOutput_c    - The buffer to where to store the output characters
   _Size_U32     - The buffer size
   _pCommand_c   - The command line to execute
   _Timeout_U32  - The timeout value

   Returns
   The command return code or
   a script launcher error code

   Remarks
   None
 */
BOFERR BofProcess::S_Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32, BOF_PROCESS &_rPid_X, int &_rExitCode_i)
{
  return BofProcess::S_Execute(_pOutput_c, _Size_U32, _pCommand_c, _Timeout_U32, BofProcess::S_mDefaultMode_E, _rPid_X, _rExitCode_i);
}

/*!
   Description
   This function executes the given
   script and catch the return code
   and output characters

   Parameters
   _pOutput_c    - The buffer to where to store the output characters
   _Size_U32     - The buffer size
   _pCommand_c   - The command line to execute
   _Mode_E       - The execution mode

   Returns
   The command return code or
   a script launcher error code

   Remarks
   None
 */
BOFERR BofProcess::S_Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, BofProcess::ExecuteMode _Mode_E, BOF_PROCESS &_rPid_X, int &_rExitCode_i)
{
  return BofProcess::S_Execute(_pOutput_c, _Size_U32, _pCommand_c, BofProcess::S_mDefaultTimeout_U32, _Mode_E, _rPid_X, _rExitCode_i);
}

/*!
   Description
   This function executes the given
   script and catch the return code
   and output characters

   Parameters
   _pOutput_c    - The buffer to where to store the output characters
   _Size_U32     - The buffer size
   _pCommand_c   - The command line to execute
   _Timeout_U32  - The timeout value in ms
   _Mode_E       - The execution mode

   Returns
   The command return code or
   a script launcher error code

   Remarks
   None
 */
BOFERR BofProcess::S_Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32, BofProcess::ExecuteMode _Mode_E, BOF_PROCESS &_rPid_X, int &_rExitCode_i)
{
  BOFERR Rts_E;

  switch (_Mode_E)
  {
    case BofProcess::EXECUTE_VFORK:
      {
        Rts_E = BofProcess::S_Execute_vfork(_pOutput_c, _Size_U32, _pCommand_c, _Timeout_U32, _rPid_X, _rExitCode_i);
        break;
      }
    case BofProcess::EXECUTE_POSIX_SPAWN:
      {
        Rts_E = BofProcess::S_Execute_posix_spawn(_pOutput_c, _Size_U32, _pCommand_c, _Timeout_U32, _rPid_X, _rExitCode_i);
        break;
      }
    case BofProcess::EXECUTE_POPEN:
      {
        Rts_E = BofProcess::S_Execute_popen(_pOutput_c, _Size_U32, _pCommand_c, _Timeout_U32, _rPid_X, _rExitCode_i);
        break;
      }

    default:
      {
        Rts_E = BOF_ERR_EINVAL;
        break;
      }
  }

  return Rts_E;
}

/*!
   Description
   This function executes the given
   command and catch the return code
   and output characters using the
   popen function

   Parameters
   _pOutput_c    - The buffer to where to store the output characters
   _Size_U32     - The buffer size
   _pCommand_c   - The command line to execute
   _Timeout_U32  - The timeout value (WARNING : This parameter is not used !!!)

   Returns
   The command return code or a script launcher error code

   Remarks
   None
 */
BOFERR BofProcess::S_Execute_popen(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32, BOF_PROCESS &_rPid_X, int &_rExitCode_i)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  FILE *pPipe_X = nullptr;
  int Len_i = 0;
  char pCommand_c[0x1000];
  char pBuf_c[0x1000];

  _rExitCode_i = 127;
  if (_pCommand_c != nullptr)
  {
    Rts_E = BOF_ERR_NOT_OPENED;
    // Redirect stderr to sdout to catch all error
    snprintf(pCommand_c, sizeof(pCommand_c), "%s 2>&1", _pCommand_c);

    // Open pipe in read mode
    pPipe_X = popen(pCommand_c, "r");

    if (pPipe_X != nullptr)
    {
      // Do we have to capture output6
      if ((_pOutput_c != nullptr) && (_Size_U32 > 0))
      {
        _pOutput_c[0] = '\0';
        Len_i = (int)_Size_U32;

        while (!feof(pPipe_X))
        {
          if (fgets(pBuf_c, sizeof(pBuf_c), pPipe_X) != nullptr)
          {
            if (Len_i > 1)
            {
              strncat(_pOutput_c, pBuf_c, Len_i - 1);

              Len_i = static_cast<int>(_Size_U32 - static_cast<uint32_t>(strlen(_pOutput_c)));
            }
          }
        }
      }
      // Grab the forked status
      _rExitCode_i = pclose(pPipe_X);
      Rts_E = (_rExitCode_i >= 0) ? BOF_ERR_NO_ERROR : BOF_ERR_CLOSE;

      // Convert it to get application return code
      _rExitCode_i = WEXITSTATUS(_rExitCode_i);
    }
    else
    {
      Rts_E = BOF_ERR_CREATE;
    }
  }

  return Rts_E;
}

/*!
   Description
   This function executes the given
   command and catch the return code
   and output characters using the
   posix_spawn function

   Parameters
   _pOutput_c    - The buffer to where to store the output characters
   _Size_U32     - The buffer size
   _pCommand_c   - The command line to execute
   _Timeout_U32  - The timeout value

   Returns
   The command return code or a script launcher error code

   Remarks
   None
 */
BOFERR BofProcess::S_Execute_posix_spawn(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32, BOF_PROCESS &_rPid_X, int &_rExitCode_i)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  _rExitCode_i = 127;
  if (_pCommand_c)
  {
    Rts_E = BOF_ERR_NOT_OPENED;
#if defined(_WIN32)
    char pCmd_c[0x1000], *pArg_c, pSpawnOut_c[0x1000];
    int Len_i;
    std::string Out_S;
    bool Sts_B;

    std::tmpnam(pSpawnOut_c);
    sprintf(pCmd_c, "%s > %s", _pCommand_c, pSpawnOut_c);
    pArg_c = strchr(pCmd_c, ' ');
    if (pArg_c)
    {
      *pArg_c = 0;
      pArg_c++;
    }
    Rts_E = BofProcess::S_SpawnProcess(pCmd_c, pArg_c, 0, _rPid_X, _rExitCode_i);
    if ((Rts_E == BOF_ERR_NO_ERROR) && (_rExitCode_i == 0) && (_rPid_X.Pid > 0))
    {
      switch (WaitForSingleObject(_rPid_X.Pi_X.hProcess, _Timeout_U32))
      {
        default:
        case WAIT_ABANDONED:
        case WAIT_FAILED:
          Sts_B = false;
          break;

        case WAIT_OBJECT_0:
          // process has terminated...
          Sts_B = true;
          break;

        case WAIT_TIMEOUT:
          // process is still running...
          Sts_B = false;
          break;
      } // Wait until child process exits.

      if (!Sts_B)
      {
        Sts_B = TerminateProcess(_rPid_X.Pi_X.hProcess, -1);
      }
      if (Sts_B)
      {
        Bof_ReadFile(pSpawnOut_c, Out_S);
      }
      Bof_DeleteFile(pSpawnOut_c);
      if (_pOutput_c)
      {
        Len_i = BOF::BOF_MIN(_Size_U32 - 1, (uint32_t)Out_S.size());
        memcpy(_pOutput_c, Out_S.c_str(), Len_i);
        _pOutput_c[Len_i] = 0;
      }
      // Close process and thread handles.
      CloseHandle(_rPid_X.Pi_X.hProcess);
      CloseHandle(_rPid_X.Pi_X.hThread);
      Rts_E = Sts_B ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
      _rExitCode_i = 0;
    }

#else
    int Len_i = 0;
    int I_S32 = 0;
    bool Read_B = false;
    char *Args_c[] = {(char *)"sh", (char *)"-c", (char *)_pCommand_c, nullptr};
    char pBuf_c[0x1000];
    posix_spawn_file_actions_t Actions_X;
    posix_spawnattr_t Attributes_X;
    int Policy_i = 0;
    struct sched_param SchedParams_X;
    int stdout_pipe[2];
    int stderr_pipe[2];

    if ((pipe(stdout_pipe) == 0) && (pipe(stderr_pipe) == 0))
    {
      // Create rules to redirect stdout and stderr of spawned process
      posix_spawn_file_actions_init(&Actions_X);
      posix_spawn_file_actions_addclose(&Actions_X, stdout_pipe[0]);
      posix_spawn_file_actions_addclose(&Actions_X, stderr_pipe[0]);
      posix_spawn_file_actions_adddup2(&Actions_X, stdout_pipe[1], 1);
      posix_spawn_file_actions_adddup2(&Actions_X, stderr_pipe[1], 2);
      posix_spawn_file_actions_addclose(&Actions_X, stdout_pipe[1]);
      posix_spawn_file_actions_addclose(&Actions_X, stderr_pipe[1]);

      // MUL-2103 : Make sure the system path are in PATH
      // setenv("PATH", "/bin:/sbin:/usr/bin:/usr/sbin:/usr/bin/X11:/usr/local/bin:/opt/evs/evs-gbio/bin", 1);

      // Create attributes

      // As a special case, if the POSIX_SPAWN_SETPGROUP flag is set in the spawn-flags attribute
      // of the object referenced by attrp, and the spawn-pgroup attribute of the same object is
      // set to zero, then the child shall be in a new process group with a process group ID equal
      // to its process ID
      posix_spawnattr_init(&Attributes_X);
      posix_spawnattr_setflags(&Attributes_X, POSIX_SPAWN_SETPGROUP | POSIX_SPAWN_SETSCHEDULER);
      posix_spawnattr_setpgroup(&Attributes_X, 0);

      // If the POSIX_SPAWN_SETSCHEDULER flag is set in the spawn-flags attribute of the object
      // referenced by attrp (regardless of the setting of the POSIX_SPAWN_SETSCHEDPARAM flag),
      // the new process image shall initially have the scheduling policy specified in the spawn-schedpolicy
      // attribute of the object referenced by attrp and the scheduling parameters specified in
      // the spawn-schedparam attribute of the same object

      // Get current thread scheduling policy
      pthread_getschedparam(pthread_self(), &Policy_i, &SchedParams_X);

      posix_spawnattr_setschedparam(&Attributes_X, &SchedParams_X);
      posix_spawnattr_setschedpolicy(&Attributes_X, Policy_i);

      // Spawn it
      if (posix_spawnp(&_rPid_X.Pid, Args_c[0], &Actions_X, &Attributes_X, Args_c, environ) == 0)
      {
        // Close child-size of the pipes
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        // Do we have to read output
        if ((_pOutput_c != nullptr) && (_Size_U32 > 0))
        {
          _pOutput_c[0] = '\0';

          // Read from the pipes
          pollfd pList_X[] = {{stdout_pipe[0], POLLIN, 0}, {stderr_pipe[0], POLLIN, 0}};

          while ((poll(pList_X, 2, (int)_Timeout_U32) > 0) && (_Size_U32 > 0))
          {
            Read_B = false;

            for (I_S32 = 0; I_S32 < 2; I_S32++)
            {
              if (pList_X[I_S32].revents & POLLIN)
              {
                Len_i = static_cast<int>(read(pList_X[I_S32].fd, pBuf_c, sizeof(pBuf_c)));

                if (Len_i > 0)
                {
                  Len_i = (_Size_U32 > static_cast<uint32_t>(Len_i)) ? Len_i : _Size_U32;

                  strncat(_pOutput_c, pBuf_c, Len_i);

                  _Size_U32 -= Len_i;
                }

                Read_B = true;
              }
            }

            // Nothing left to read
            if (!Read_B)
            {
              break;
            }
          }
        }

        Rts_E = WaitForProcessCompletion(_rPid_X, _Timeout_U32, _rExitCode_i);
      }
      else
      {
        Rts_E = BOF_ERR_CREATE;
      }

      // Clean up
      posix_spawn_file_actions_destroy(&Actions_X);
      posix_spawnattr_destroy(&Attributes_X);
    }
    else
    {
      Rts_E = BOF_ERR_INTERNAL;
    }
#endif
  }
  return Rts_E;
}

/*!
   Description
   This function executes the given
   command and catch the return code
   and output characters using the
   vfork function

   Parameters
   _pOutput_c    - The buffer to where to store the ouptut characters
   _Size_U32     - The buffer size
   _pCommand_c   - The command line to execute
   _Timeout_U32  - The timeout value

   Returns
   The command return code or a script launcher error code

   Remarks
   None
 */
BOFERR BofProcess::S_Execute_vfork(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32, BOF_PROCESS &_rPid_X, int &_rExitCode_i)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  _rExitCode_i = 127;

  if (_pCommand_c)
  {
    Rts_E = BOF_ERR_NOT_OPENED;
#if defined(_WIN32)
    char pCmd_c[0x1000], *pArg_c, pSpawnOut_c[0x1000];
    int Len_i;
    std::string Out_S;
    bool Sts_B;

    std::tmpnam(pSpawnOut_c);
    sprintf(pCmd_c, "%s > %s", _pCommand_c, pSpawnOut_c);
    pArg_c = strchr(pCmd_c, ' ');
    if (pArg_c)
    {
      *pArg_c = 0;
      pArg_c++;
    }
    Rts_E = BofProcess::S_SpawnProcess(pCmd_c, pArg_c, 0, _rPid_X, _rExitCode_i);
    if ((Rts_E == BOF_ERR_NO_ERROR) && (_rExitCode_i == 0) && (_rPid_X.Pid > 0))
    {
      switch (WaitForSingleObject(_rPid_X.Pi_X.hProcess, _Timeout_U32))
      {
        default:
        case WAIT_ABANDONED:
        case WAIT_FAILED:
          Sts_B = false;
          break;

        case WAIT_OBJECT_0:
          // process has terminated...
          Sts_B = true;
          break;

        case WAIT_TIMEOUT:
          // process is still running...
          Sts_B = false;
          break;
      } // Wait until child process exits.

      if (!Sts_B)
      {
        Sts_B = TerminateProcess(_rPid_X.Pi_X.hProcess, -1);
      }
      if (Sts_B)
      {
        Bof_ReadFile(pSpawnOut_c, Out_S);
      }
      Bof_DeleteFile(pSpawnOut_c);
      if (_pOutput_c)
      {
        Len_i = BOF::BOF_MIN(_Size_U32 - 1, (uint32_t)Out_S.size());
        memcpy(_pOutput_c, Out_S.c_str(), Len_i);
        _pOutput_c[Len_i] = 0;
      }
      // Close process and thread handles.
      CloseHandle(_rPid_X.Pi_X.hProcess);
      CloseHandle(_rPid_X.Pi_X.hThread);
      if (Sts_B)
      {
        Rts_E = BOF_ERR_NO_ERROR;
        _rExitCode_i = 0;
      }
      else
      {
        Rts_E = BOF_ERR_INTERNAL;
      }
    }
#else
    //	int     Status_i        = 0;
    int File_i = 0;
    char *Args_c[] = {(char *)"/bin/sh", (char *)"-c", (char *)_pCommand_c, nullptr};
    ssize_t Read_i = 0;
    char pTempFileName_c[0x1000];
    char pRedirect_c[0x1000];
    bool CaptureOutput_B = (_pOutput_c != nullptr) && (_Size_U32 > 0);

    if (CaptureOutput_B)
    {
      // Create a temporary file
      snprintf(pTempFileName_c, sizeof(pTempFileName_c), "/tmp/OutXXXXXX");

      mktemp(pTempFileName_c);

      if (strlen(pTempFileName_c) > 0)
      {
        snprintf(pRedirect_c, sizeof(pRedirect_c), "%s > %s", _pCommand_c, pTempFileName_c);

        Args_c[2] = pRedirect_c;
      }
    }

    _rPid_X.Pid = vfork();

    if (_rPid_X.Pid == 0)
    {
      execve(Args_c[0], Args_c, environ);

      // Won't reach here except error
      _exit(-1);
    }
    else if (_rPid_X.Pid > 0)
    {
      Rts_E = WaitForProcessCompletion(_rPid_X, _Timeout_U32, _rExitCode_i);

      // Do we have to capture output
      if (CaptureOutput_B)
      {
        _pOutput_c[0] = '\0';

        File_i = open(pTempFileName_c, O_RDONLY);

        if (File_i != -1)
        {
          Read_i = read(File_i, _pOutput_c, _Size_U32 - 1);

          _pOutput_c[Read_i] = '\0';

          close(File_i);
        }

        remove(pTempFileName_c);
      }
    }
    else
    {
      Rts_E = BOF_ERR_CREATE;
    }
#endif
  }
  return Rts_E;
}

/*!
   Description
   This function reads an uint32_t
   value from the specified file

   Parameters
   _pFile_c - The path to the file

   Returns
   true  - The operation was successful
   false - The operation failed

   Remarks
   None
 */
BOFERR BofProcess::S_ReadU32FromFile(const char *_pFile_c, uint32_t *_pValue_U32)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  int ExitCode_i;
  BOF_PROCESS Pid_X;
  char pResult_c[0x1000];
  char pCmd_c[0x1000];

  if ((_pFile_c != nullptr) && (_pValue_U32 != nullptr))
  {
    snprintf(pCmd_c, sizeof(pCmd_c), "cat %s", _pFile_c);

    // Execute the command
    Rts_E = S_Execute(pResult_c, sizeof(pResult_c), pCmd_c, Pid_X, ExitCode_i);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      // Convert the result
      *_pValue_U32 = (uint32_t)atol(pResult_c);
    }
  }
  return Rts_E;
}

/*!
   Description
   This function writes an uint32_t
   to the specified file

   Parameters
   _pFile_c    - The path to the file
   _Value_U32  - The value to write

   Returns
   true  - The operation was successful
   false - The operation failed

   Remarks
   None
 */
BOFERR BofProcess::S_WriteU32ToFile(const char *_pFile_c, uint32_t _Value_U32)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  int ExitCode_i;
  BOF_PROCESS Pid_X;
  char pResult_c[0x1000];
  char pCmd_c[0x1000];

  if (_pFile_c != nullptr)
  {
    snprintf(pCmd_c, sizeof(pCmd_c), "echo %d > %s", _Value_U32, _pFile_c);
    Rts_E = S_Execute(pResult_c, sizeof(pResult_c), pCmd_c, Pid_X, ExitCode_i);
  }

  return Rts_E;
}

/*!
Description
  This function spawns another process

Parameters
  _pProgram_c   - The process to spawn
  _pArguments_c - The associated command line
  _DbgPort_U16 - If different from zero we lauch the gdb server with the process in arg

Returns
  >0 - The PID of the spawned process
  <0 - An error occurred

Remarks
  None
*/
BOFERR BofProcess::S_SpawnProcess(const char *_pProgram_c, const char *_pArguments_c, uint16_t _DbgPort_U16, BOF_PROCESS &_rPid_X, int &_rExitCode_i)
{
  BOFERR Rts_E = BOF_ERR_DONT_EXIST;
  char pTemp_c[0x1000];
  bool ItIsADirectory_B;

  _rExitCode_i = 127;
  _rPid_X.Reset();
  if ((Bof_IsPathExist(_pProgram_c, ItIsADirectory_B)) && (!ItIsADirectory_B)) // Bof_GetFileType
  {
    Rts_E = BOF_ERR_NOT_OPENED;
#if defined(_WIN32)
    STARTUPINFO Si_X;

    if (_pArguments_c != nullptr)
    {
      strncpy(pTemp_c, _pArguments_c, sizeof(pTemp_c));
    }
    else
    {
      strncpy(pTemp_c, "", sizeof(pTemp_c));
    }
    memset(&Si_X, 0, sizeof(Si_X));
    Si_X.cb = sizeof(Si_X);
    // Start the child process.
    if (CreateProcess(_pProgram_c,  // Module name
                      pTemp_c,      // Command line
                      nullptr,      // Process handle not inheritable
                      nullptr,      // Thread handle not inheritable
                      false,        // Set handle inheritance to FALSE
                      0,            // No creation flags
                      nullptr,      // Use parent's environment block
                      nullptr,      // Use parent's starting directory
                      &Si_X,        // Pointer to STARTUPINFO structure
                      &_rPid_X.Pi_X // Pointer to PROCESS_INFORMATION structure
                      ))
    {
      _rPid_X.Pid = (uintptr_t)_rPid_X.Pi_X.hProcess;
      _rExitCode_i = 0;
      Rts_E = BOF_ERR_NO_ERROR;
#if 0
    // Wait until child process exits.
    WaitForSingleObject(Rts_X.Pi_X.hProcess, INFINITE);

    // Close process and thread handles.
    CloseHandle(Rts_X.Pi_X.hProcess);
    CloseHandle(Rts_X.Pi_X.hThread);
#endif
    }
    else
    {
      // DWORD Err_DW = GetLastError();
      _rPid_X.Pid = -1;
    }
#else
    char *ppArgs_c[128];
    char pArgZero_c[0x1000];
    int NbArgs_i;
    char *pPtr_c;
    int Pid_i, Sts_i;

    Pid_i = fork();
    // If PID is null, it means
    // we are in the child process
    if (Pid_i == 0)
    {
      //_rPid_X.Pid = getpid();
      // printf("in child pid %d %d\n", _rPid_X.Pid, getpid());
      if (_pArguments_c != nullptr)
      {
        strncpy(pTemp_c, _pArguments_c, sizeof(pTemp_c));
      }
      else
      {
        strncpy(pTemp_c, "", sizeof(pTemp_c));
      }

      pPtr_c = pTemp_c;
      NbArgs_i = 1;
      Bof_StrNCpy(pArgZero_c, _pProgram_c, sizeof(pArgZero_c));
      ppArgs_c[0] = pArgZero_c;

      while ((pPtr_c != nullptr) && (NbArgs_i < (static_cast<int>((sizeof(ppArgs_c) / sizeof(ppArgs_c[0]))) - 1)))
      {
        ppArgs_c[NbArgs_i++] = pPtr_c;
        pPtr_c = strchr(pPtr_c, ' ');
        // printf("in child NbArgs_i %d pPtr_c %p\n", NbArgs_i, pPtr_c);
        if (pPtr_c != nullptr)
        {
          *pPtr_c = '\0';
          pPtr_c += 1;
        }
      }

      ppArgs_c[NbArgs_i] = nullptr;
      // printf("in child _DbgPort_U16 %d\n", _DbgPort_U16);
      if (_DbgPort_U16)
      {
        if ((_pArguments_c != nullptr) && (_pArguments_c[0] != '\0'))
        {
          snprintf(pTemp_c, sizeof(pTemp_c), "gdbserver :%d %s %s", _DbgPort_U16, _pProgram_c, _pArguments_c);
        }
        else
        {
          snprintf(pTemp_c, sizeof(pTemp_c), "gdbserver :%d %s", _DbgPort_U16, _pProgram_c);
        }

        BOF::BofProcess::S_Execute(pTemp_c, _rPid_X, _rExitCode_i);
      }
      else
      {
        /// printf("execv %s %s\n", _pProgram_c, ppArgs_c[0]);
        _rExitCode_i = 0;
        Rts_E = BOF_ERR_NO_ERROR;
        // printf("Before execv pid is %d Sts %d errno %d Exit %d Rts %d\n", _rPid_X.Pid, Sts_i, errno, _rExitCode_i, Rts_E);
        // for (i = 0; i < NbArgs_i; i++)
        //{
        //  printf("arg[%d]=%s\n", i, ppArgs_c[i]);
        //}
        Sts_i = execv(_pProgram_c, ppArgs_c);
        // Should never come back in this code except if error
        BOF_ASSERT(Sts_i < 0);
        _rPid_X.Reset();
        Rts_E = BOF_ERR_DONT_EXIST;

        // printf("execv pid is %d Sts %d errno %d Exit %d Rts %d\n", _rPid_X.Pid, Sts_i, errno, _rExitCode_i, Rts_E);
      }

      _exit(0);
    }
    else
    {
      _rPid_X.Pid = Pid_i;
      // Just special values because we can't say more: we call execv in a child process....
      _rExitCode_i = 0;
      Rts_E = BOF_ERR_NO_ERROR;
      // printf("Main pid is %d Rts %d\n", Pid_i, Rts_E);
      // int t = wait(&Sts_i);
      // printf("t=%d S=%d\n", t, Sts_i);
    }
#endif
  }
  return Rts_E;
}
/*!
Description
  This function kills all processes
  IDs whose name is matching the specified
  one

Parameters
  _pProcessName_c - The process name

Returns
  Nothing

Remarks
  None
*/
BOFERR BofProcess::S_KillProcess(const char *_pProcessName_c)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  char pCmd_c[0x1000];
  int ExitCode_i;
  BOF_PROCESS Pid_X;

  if (_pProcessName_c != nullptr)
  {
#if defined(_WIN32)
    snprintf(pCmd_c, sizeof(pCmd_c), "taskkill /IM %s", _pProcessName_c);
#else
    const char *pProcessname_c = strrchr(_pProcessName_c, '/');
    snprintf(pCmd_c, sizeof(pCmd_c), "pkill %s", pProcessname_c ? pProcessname_c + 1 : _pProcessName_c);
#endif
    Rts_E = S_Execute_popen(nullptr, 0, pCmd_c, 0, Pid_X, ExitCode_i);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (ExitCode_i)
      {
        Rts_E = BOF_ERR_DONT_EXIST;
#if defined(_WIN32)
#else
        snprintf(pCmd_c, sizeof(pCmd_c), "killall %s", pProcessname_c ? pProcessname_c + 1 : _pProcessName_c);
        Rts_E = S_Execute_popen(nullptr, 0, pCmd_c, 0, Pid_X, ExitCode_i);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          if (ExitCode_i)
          {
            Rts_E = BOF_ERR_DONT_EXIST;
          }
        }
#endif
      }
    }
  }
  return Rts_E;
}

/*!
Description
  This function checks if the process
  identified by the given PID is
  running or not

Parameters
  _Pid_i  - The PID of the process

Returns
  true  - The process is running
  false - The process is not running

Remarks
  None
*/
bool BofProcess::S_IsProcessRunning(BOF_PROCESS _Pid_X)
{
  bool Rts_B = false;

#if defined(_WIN32)
  switch (WaitForSingleObject(_Pid_X.Pi_X.hProcess, 0))
  {
    default:
    case WAIT_ABANDONED:
    case WAIT_FAILED:
      Rts_B = false;
      break;

    case WAIT_OBJECT_0:
      // process has terminated...
      Rts_B = false;
      break;

    case WAIT_TIMEOUT:
      // process is still running...
      Rts_B = true;
      break;
  }
#else
  int Status_i = 0;
  int Rts_i = 0;

  // Does PID exist
  if (kill(_Pid_X.Pid, 0) == 0)
  {
    Rts_i = waitpid(_Pid_X.Pid, &Status_i, WNOHANG);

    // An error occurred : maybe the PID no longer exist
    if (Rts_i == -1)
    {
      Rts_B = false;
    }
    else if (Rts_i == 0)
    {
      Rts_B = true;
    }
    else
    {
      Rts_B = !(WIFEXITED(Status_i) | WIFSIGNALED(Status_i));
    }
  }
  else
  {
    Rts_B = false;
  }
#endif
  return Rts_B;
}

/*!
Description
  This function kills the specified process

Parameters
  _Pid_i  - The PID of the process to kill

Returns
  true  - The operation was successful
  false - The operation failed

Remarks
  None
*/
BOFERR BofProcess::S_KillProcess(BOF_PROCESS _Pid_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if (_Pid_X.Pid > 0)
  {
#if defined(_WIN32)
    if (TerminateProcess(_Pid_X.Pi_X.hProcess, -1))
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
#else
    int Sts_i;

    Sts_i = kill(_Pid_X.Pid, SIGTERM);
    if (Sts_i < 0)
    {
      // sleep(1);
      Sts_i = kill(_Pid_X.Pid, SIGKILL);
    }
    if (Sts_i >= 0)
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
    // Rts_E = S_IsProcessRunning(_Pid_X) ? BOF_ERR_INTERNAL : BOF_ERR_NO_ERROR;
#endif
  }
  return Rts_E;
}

BOFERR BofProcess::S_AttachProcessToDebugger(BOF_PROCESS _Pid_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
#if defined(__linux__)
  if (ptrace(PTRACE_ATTACH, _Pid_X.Pid, nullptr, nullptr) >= 0)
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
#endif
  return Rts_E;
}
END_BOF_NAMESPACE()
