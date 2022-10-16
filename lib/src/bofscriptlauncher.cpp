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

#if defined (_WIN32)
#else
#include <bofstd/bofscriptlauncher.h>
#include <bofstd/bofsystem.h>

#include <stdio.h>
#include <sys/types.h>
#include <spawn.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
extern char **environ;



BEGIN_BOF_NAMESPACE()

#define SCRIPT_LAUNCHER_INTERNAL_ERROR                        (-1)
#define SCRIPT_LAUNCHER_PROCESS_CREATION_ERROR                (-2)
#define SCRIPT_LAUNCHER_INVALID_ARGUMENTS_ERROR               (-3)
#define SCRIPT_LAUNCHER_TIMEOUT_ERROR                         (-4)
#define SCRIPT_LAUNCHER_PROCESS_ABNORMAL_TERMINATION_ERROR    (-5)
#define SCRIPT_LAUNCHER_NOT_SUPPORTED                         (-6)

uint32_t                       BofScriptLauncher::S_mDefaultTimeout_U32 = 120000;
BofScriptLauncher::ExecuteMode BofScriptLauncher::S_mDefaultMode_E = BofScriptLauncher::EXECUTE_POPEN;

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
pid_t WaitForPid(pid_t _Pid_X, int *_pStatus_i, uint32_t _Timeout_U32)
{
  pid_t RetPid_X = -1;
  uint32_t Start_U32, Delta_U32;
  Start_U32 = Bof_GetMsTickCount();

  if (_pStatus_i)
  {
    do
    {
      RetPid_X = waitpid(_Pid_X, _pStatus_i, WNOHANG);
      Delta_U32 = Bof_ElapsedMsTime(Start_U32);

      if ((RetPid_X == -1) || (RetPid_X == _Pid_X) || (Delta_U32 >= _Timeout_U32))
      {
        break;
      }

      Bof_MsSleep(2000);
    } while (RetPid_X != _Pid_X);
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
int KillSubProcess(pid_t _Pid_X, int _Options_i)
{
  char pCommand_c[256];
  char pResult_c[256];
  int Status_i;
  char *pLine_c;
  pid_t ChildPid_X;

  snprintf(pCommand_c, sizeof(pCommand_c), "ps -o ppid,pid | grep %d | awk '{ print $2 }'", _Pid_X);

  Status_i = BofScriptLauncher::Execute(pResult_c, sizeof(pResult_c), pCommand_c, 1000);

  if (Status_i == 0)
  {
    pLine_c = strtok(pResult_c, "\n");

    while (pLine_c != nullptr)
    {
      ChildPid_X = atoi(pLine_c);

      if (ChildPid_X != _Pid_X)
      {
        Status_i |= kill(ChildPid_X, _Options_i);
      }

      pLine_c = strtok(nullptr, "\n");
    }
  }

  return Status_i;
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
   SCRIPT_LAUNCHER_PROCESS_ABNORMAL_TERMINATION_ERROR (-5) - The process did not terminated normally
   SCRIPT_LAUNCHER_TIMEOUT_ERROR                  (-4) - The process did time out

   The return code of the process otherwise

   Remarks
   None
 */
int WaitForProcessCompletion(pid_t _Pid_X, uint32_t _Timeout_U32)
{
  int Ret_i = -1;
  int Status_i = -1;
  pid_t RetPid_X = -1;
  bool Kill_B = false;

  RetPid_X = WaitForPid(_Pid_X, &Status_i, _Timeout_U32);

  // Check if process exited
  if (RetPid_X == _Pid_X)
  {
    if (WIFEXITED(Status_i))
    {
      Ret_i = WEXITSTATUS(Status_i);
    }
    else
    {
      Kill_B = true;
      Ret_i = SCRIPT_LAUNCHER_PROCESS_ABNORMAL_TERMINATION_ERROR;
    }
  }
  else
  {
    //		int error_i = errno;

    Ret_i = SCRIPT_LAUNCHER_TIMEOUT_ERROR;
    Kill_B = true;
  }

  if (Kill_B)
  {
    // Try terminating it
    Status_i = KillSubProcess(_Pid_X, SIGTERM);
    Status_i = kill(_Pid_X, SIGTERM);

    RetPid_X = WaitForPid(_Pid_X, &Status_i, _Timeout_U32);

    if (RetPid_X != _Pid_X)
    {
      // Try killing it
      Status_i = KillSubProcess(_Pid_X, SIGKILL);
      Status_i = kill(_Pid_X, SIGKILL);

      RetPid_X = WaitForPid(_Pid_X, &Status_i, _Timeout_U32);
    }
  }

  return Ret_i;
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
bool BofScriptLauncher::SetDefaultTimeout(uint32_t _Timeout_U32)
{
  bool Ret_B = true;

  BofScriptLauncher::S_mDefaultTimeout_U32 = _Timeout_U32;

  return Ret_B;
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
uint32_t BofScriptLauncher::GetDefaultTimeout()
{
  return BofScriptLauncher::S_mDefaultTimeout_U32;
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
bool BofScriptLauncher::SetDefaultExecuteMode(BofScriptLauncher::ExecuteMode _Mode_E)
{
  BofScriptLauncher::S_mDefaultMode_E = _Mode_E;
  return true;
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
BofScriptLauncher::ExecuteMode BofScriptLauncher::GetDefaultExecuteMode()
{
  return BofScriptLauncher::S_mDefaultMode_E;
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
int BofScriptLauncher::Execute(const char *_pCommand_c)
{
  return BofScriptLauncher::Execute(nullptr, 0, _pCommand_c, BofScriptLauncher::S_mDefaultTimeout_U32, BofScriptLauncher::S_mDefaultMode_E);
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
int BofScriptLauncher::Execute(const char *_pCommand_c, BofScriptLauncher::ExecuteMode _Mode_E)
{
  return BofScriptLauncher::Execute(nullptr, 0, _pCommand_c, BofScriptLauncher::S_mDefaultTimeout_U32, _Mode_E);
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
int BofScriptLauncher::Execute(const char *_pCommand_c, uint32_t _Timeout_U32)
{
  return BofScriptLauncher::Execute(nullptr, 0, _pCommand_c, _Timeout_U32, BofScriptLauncher::S_mDefaultMode_E);
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
int BofScriptLauncher::Execute(const char *_pCommand_c, uint32_t _Timeout_U32, BofScriptLauncher::ExecuteMode _Mode_E)
{
  return BofScriptLauncher::Execute(nullptr, 0, _pCommand_c, _Timeout_U32, _Mode_E);
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
int BofScriptLauncher::Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c)
{
  return BofScriptLauncher::Execute(_pOutput_c, _Size_U32, _pCommand_c, BofScriptLauncher::S_mDefaultTimeout_U32, BofScriptLauncher::S_mDefaultMode_E);
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
int BofScriptLauncher::Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32)
{
  return BofScriptLauncher::Execute(_pOutput_c, _Size_U32, _pCommand_c, _Timeout_U32, BofScriptLauncher::S_mDefaultMode_E);
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
int BofScriptLauncher::Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, BofScriptLauncher::ExecuteMode _Mode_E)
{
  return BofScriptLauncher::Execute(_pOutput_c, _Size_U32, _pCommand_c, BofScriptLauncher::S_mDefaultTimeout_U32, _Mode_E);
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
int BofScriptLauncher::Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32, BofScriptLauncher::ExecuteMode _Mode_E)
{
  int Ret_i = 0;

  switch (_Mode_E)
  {
    case BofScriptLauncher::EXECUTE_VFORK:
    {
      Ret_i = BofScriptLauncher::Execute_vfork(_pOutput_c, _Size_U32, _pCommand_c, _Timeout_U32);
      break;
    }
    case BofScriptLauncher::EXECUTE_POSIX_SPAWN:
    {
      Ret_i = BofScriptLauncher::Execute_posix_spawn(_pOutput_c, _Size_U32, _pCommand_c, _Timeout_U32);
      break;
    }
    case BofScriptLauncher::EXECUTE_POPEN:
    {
      Ret_i = BofScriptLauncher::Execute_popen(_pOutput_c, _Size_U32, _pCommand_c, _Timeout_U32);
      break;
    }

    default:
    {
      Ret_i = SCRIPT_LAUNCHER_INVALID_ARGUMENTS_ERROR;
      break;
    }
  }

  return Ret_i;
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
int BofScriptLauncher::Execute_popen(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t /*_Timeout_U32*/)
{
  int Ret_i = 0;
  FILE *pPipe_X = nullptr;
  int Len_i = 0;
  char pCommand_c[256];
  char pBuf_c[128];

  if (_pCommand_c != nullptr)
  {
    // Redirect stderr to sdout to catch all error
    snprintf(pCommand_c, sizeof(pCommand_c), "%s 2>&1", _pCommand_c);

    // Open pipe in read mode
    pPipe_X = popen(pCommand_c, "r");

    if (pPipe_X != nullptr)
    {
      // Do we have to capture output
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
      Ret_i = pclose(pPipe_X);

      // Convert it to get application return code
      Ret_i = WEXITSTATUS(Ret_i);
    }
    else
    {
      Ret_i = SCRIPT_LAUNCHER_PROCESS_CREATION_ERROR;
    }
  }
  else
  {
    Ret_i = SCRIPT_LAUNCHER_INVALID_ARGUMENTS_ERROR;
  }

  return Ret_i;
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
int BofScriptLauncher::Execute_posix_spawn(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32)
{
  int Ret_i = 0;
  int Len_i = 0;
  //	int                        Status_i  = 0;
  int I_S32 = 0;
  bool Read_B = false;
  //	bool                       Kill_B    = false;
  char *Args_c[] = { (char *)"sh", (char *)"-c", (char *)_pCommand_c, nullptr };
  char pBuf_c[128];
  pid_t Pid_X;
  //	pid_t                      RetPid_X;
  posix_spawn_file_actions_t Actions_X;
  posix_spawnattr_t Attributes_X;
  int Policy_i = 0;
  struct sched_param SchedParams_X;
  int stdout_pipe[2];
  int stderr_pipe[2];
  //	uint64_t                   Timer_U64;

  if (_pCommand_c != nullptr)
  {
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
      // setenv("PATH", "/bin:/sbin:/usr/bin:/usr/sbin:/usr/bin/X11:/usr/local/bin", 1);

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
      if (posix_spawnp(&Pid_X, Args_c[0], &Actions_X, &Attributes_X, Args_c, environ) == 0)
      {
        // Close child-size of the pipes
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        // Do we have to read output
        if ((_pOutput_c != nullptr) && (_Size_U32 > 0))
        {
          _pOutput_c[0] = '\0';

          // Read from the pipes
          pollfd pList_X[] = { {stdout_pipe[0], POLLIN, 0},
                              {stderr_pipe[0], POLLIN, 0} };

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

        Ret_i = WaitForProcessCompletion(Pid_X, _Timeout_U32);
      }
      else
      {
        Ret_i = SCRIPT_LAUNCHER_PROCESS_CREATION_ERROR;
      }

      // Clean up
      posix_spawn_file_actions_destroy(&Actions_X);
      posix_spawnattr_destroy(&Attributes_X);
    }
    else
    {
      Ret_i = SCRIPT_LAUNCHER_INTERNAL_ERROR;
    }
  }
  else
  {
    Ret_i = SCRIPT_LAUNCHER_INVALID_ARGUMENTS_ERROR;
  }

  return Ret_i;
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
int BofScriptLauncher::Execute_vfork(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32)
{
  int Ret_i = 0;
  //	int     Status_i        = 0;
  int File_i = 0;
  char *Args_c[] = { (char *)"/bin/sh", (char *)"-c", (char *)_pCommand_c, nullptr };
  pid_t Pid_X = 0;
  ssize_t Read_i = 0;
  char pTempFileName_c[256];
  char pRedirect_c[512];
  bool CaptureOutput_B = (_pOutput_c != nullptr) && (_Size_U32 > 0);

  if (_pCommand_c != nullptr)
  {
    // setenv("PATH", "/bin:/sbin:/usr/bin:/usr/sbin:/usr/bin/X11:/usr/local/bin", 1);

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

    Pid_X = vfork();

    if (Pid_X == 0)
    {
      execve(Args_c[0], Args_c, environ);

      // Won't reach here except error
      _exit(-1);
    }
    else if (Pid_X > 0)
    {
      Ret_i = WaitForProcessCompletion(Pid_X, _Timeout_U32);

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
      Ret_i = SCRIPT_LAUNCHER_PROCESS_CREATION_ERROR;
    }
  }
  else
  {
    Ret_i = SCRIPT_LAUNCHER_INVALID_ARGUMENTS_ERROR;
  }

  return Ret_i;
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
bool BofScriptLauncher::ReadU32FromFile(const char *_pFile_c, uint32_t *_pValue_U32)
{
  bool Ret_B = false;
  int Ret_i = 0;
  char pResult_c[256];
  char pCmd_c[256];

  if ((_pFile_c != nullptr) && (_pValue_U32 != nullptr))
  {
    snprintf(pCmd_c, sizeof(pCmd_c), "cat %s", _pFile_c);

    // Execute the command
    Ret_i = BofScriptLauncher::Execute(pResult_c, sizeof(pResult_c), pCmd_c);

    if (Ret_i == 0)
    {
      // Convert the result
      *_pValue_U32 = (uint32_t)atol(pResult_c);
      Ret_B = true;
    }
  }

  return Ret_B;
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
bool BofScriptLauncher::WriteU32ToFile(const char *_pFile_c, uint32_t _Value_U32)
{
  bool Ret_B = false;
  int Ret_i = 0;
  char pResult_c[256];
  char pCmd_c[256];

  if (_pFile_c != nullptr)
  {
    snprintf(pCmd_c, sizeof(pCmd_c), "echo %d > %s", _Value_U32, _pFile_c);

    // Execute the command
    Ret_i = BofScriptLauncher::Execute(pResult_c, sizeof(pResult_c), pCmd_c);

    if (Ret_i == 0)
    {
      Ret_B = true;
    }
  }

  return Ret_B;
}

END_BOF_NAMESPACE()

#endif