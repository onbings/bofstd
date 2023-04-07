/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bofdaemonservice class
 *
 * Name:        bofdaemonservice.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         None
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#include <bofstd/bofdaemonservice.h>
#include <bofstd/boffs.h>

#if defined(_WIN32)
#include <io.h>
#include <stdio.h>
#else
#include <signal.h>
#include <unistd.h>
#endif

#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

BEGIN_BOF_NAMESPACE()

#if defined(_WIN32)
/*
void SignalHandler(int _Signal_i)
{
  syslog(BofDaemonService::S_Instance().GetSyslogPriority(), "SignalHandler(%d)", _Signal_i);
  switch (_Signal_i)
  {
  case SIGHUP:
    syslog(BofDaemonService::S_Instance().GetSyslogPriority(), "Received SIGHUP signal.");
    break;
  case SIGINT:
  case SIGTERM:
    //
  case SIGSTOP:
    syslog(BofDaemonService::S_Instance().GetSyslogPriority(), "Daemon exiting");
    BofDaemonService::S_Instance().Shutdown();
    exit(EXIT_SUCCESS);
    break;
  default:
    syslog(BofDaemonService::S_Instance().GetSyslogPriority(), "Unhandled signal %s", strsignal(_Signal_i));
    break;
  }
}
*/
// https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux
void BofDaemonService::CreateDaemonService(BOF_DAEMON_SERVICE_PARAM &_rDaemonServiceParam_X)
{
  mDaemonPidFileHandle_i = -1;
  mLogActive_B = false;
  mDaemonServiceParam_X = _rDaemonServiceParam_X;
  mDaemonServiceParam_X.Sts_E = BOF_ERR_NO_ERROR;
  if (mDaemonServiceParam_X.pDaemonHasExited_B)
  {
    *mDaemonServiceParam_X.pDaemonHasExited_B = false;
  }
}
void BofDaemonService::Shutdown()
{
  if (mDaemonPidFileHandle_i > 0)
  {
    close(mDaemonPidFileHandle_i);
    mDaemonPidFileHandle_i = -1;
    Bof_DeleteFile(mDaemonServiceParam_X.PidFilePath);
  }

  if (mDaemonServiceParam_X.pDaemonHasExited_B)
  {
    *mDaemonServiceParam_X.pDaemonHasExited_B = true;
    mDaemonServiceParam_X.pDaemonHasExited_B = nullptr;
  }
  if (mLogActive_B)
  {
    // closelog();
    mLogActive_B = false;
  }
}

int BofDaemonService::GetSyslogPriority()
{
  return mDaemonServiceParam_X.DaemonServiceLogPriority_i;
}

void BofDaemonService::DaemonServiceLog(const char *_pFormat_c, ...)
{
  char pLog_c[0x1000];
  va_list Arg;

  if (mLogActive_B)
  {
    va_start(Arg, _pFormat_c);
    vsnprintf(pLog_c, sizeof(pLog_c), _pFormat_c, Arg);
    va_end(Arg);
    //		syslog(mDaemonServiceParam_X.DaemonServiceLogPriority_i, pLog_c);
  }
}
BofDaemonService::BofDaemonService()
{
}

BofDaemonService::~BofDaemonService()
{
}
BofDaemonService &BofDaemonService::S_Instance()
{
  // Since it's a static variable, if the class has already been created, It won't be created again so it **is** thread-safe in C++11.
  static BofDaemonService TheBofDaemonServiceInstance;
  return TheBofDaemonServiceInstance;
}

#else

void SignalHandler(int _Signal_i)
{
  syslog(BofDaemonService::S_Instance().GetSyslogPriority(), "SignalHandler(%d)", _Signal_i);
  switch (_Signal_i)
  {
  case SIGHUP:
    syslog(BofDaemonService::S_Instance().GetSyslogPriority(), "Received SIGHUP signal.");
    break;
  case SIGINT:
  case SIGTERM:
    //
  case SIGSTOP:
    syslog(BofDaemonService::S_Instance().GetSyslogPriority(), "Daemon exiting");
    BofDaemonService::S_Instance().Shutdown();
    exit(EXIT_SUCCESS);
    break;
  default:
    syslog(BofDaemonService::S_Instance().GetSyslogPriority(), "Unhandled signal %s", strsignal(_Signal_i));
    break;
  }
}

// https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux
void BofDaemonService::CreateDaemonService(BOF_DAEMON_SERVICE_PARAM &_rDaemonServiceParam_X)
{
  pid_t Pid;
  pid_t Sid;
  char pBuf_c[32];
  struct sigaction SigAction_X;
  int i;

  mDaemonPidFileHandle_i = -1;
  mLogActive_B = false;
  mDaemonServiceParam_X = _rDaemonServiceParam_X;
  mDaemonServiceParam_X.Sts_E = BOF_ERR_NO_ERROR;
  if (mDaemonServiceParam_X.pDaemonHasExited_B)
  {
    *mDaemonServiceParam_X.pDaemonHasExited_B = false;
  }
  /* already a daemon */
  if (getppid() == 1)
  {
    mDaemonServiceParam_X.Sts_E = BOF_ERR_EEXIST;
  }
  else
  {
    // Fork off the parent process & let it terminate if forking was successful. -> Because the parent process has terminated, the child process now runs in the background
    // On success, the PID of the child process is returned in the parent, and 0 is returned in the child. On failure, -1 is returned in the parent, no child process is created, and errno is set appropriately.
    Pid = fork();
    if (Pid < 0)
    {
      mDaemonServiceParam_X.Sts_E = BOF_ERR_CREATE;
      exit(EXIT_FAILURE);
    }
    else
    {
      /* If we got a good PID, then we can exit the parent process. */
      if (Pid > 0)
      {
        // Success: Let the parent terminate
        mDaemonServiceParam_X.Sts_E = BOF_ERR_NO_ERROR;
        exit(EXIT_SUCCESS);
      }

      //-------------------- At this point we are executing as the child process ----------------------------------------

      // Bof_WriteFile(BOF_FILE_PERMISSION_READ_FOR_ALL | BOF_FILE_PERMISSION_WRITE_FOR_ALL, /tmp/bha, false, "1\n")

      // Create a new SID for the child process: The calling process becomes the leader of the new session and the process group leader of the new process group.
      // The process is now detached from its controlling terminal (CTTY).
      Sid = setsid();
      if (Sid < 0)
      {
        mDaemonServiceParam_X.Sts_E = BOF_ERR_CREATE;
        exit(EXIT_FAILURE);
      }

      /*
      n Linux, a daemon is typically created by forking twice with the intermediate process exiting after forking the grandchild. This has the effect of orphaning the grandchild process.
      As a result, it becomes the responsibility of the OS to clean up after it if it terminates. The reason has to do with what are known as zombie processes which continue to live and
      consume resources after exiting because their parent, who'd normally be responsible for the cleaning up, has also died.
      */
      /* Fork off for the second time: the parent process terminate to ensure that you get rid of the session leading process. (Only session leaders may get a TTY again.)*/
      Pid = fork();
      if (Pid < 0)
      {
        mDaemonServiceParam_X.Sts_E = BOF_ERR_INTERNAL;
        exit(EXIT_FAILURE);
      }
      if (Pid > 0)
      {
        mDaemonServiceParam_X.Sts_E = BOF_ERR_NO_ERROR;
        exit(EXIT_SUCCESS);
      }

      /* Change the file mode mask: Change the file mode mask according to the needs of the daemon. */
      umask(0);

      /* Change the current working directory.  This prevents the current	directory from being locked; hence not being able to remove it. */
      if ((chdir("/")) < 0)
      {
        mDaemonServiceParam_X.Sts_E = BOF_ERR_EACCES;
        exit(EXIT_FAILURE);
      }

      /* Close all open file descriptors:  Close all open file descriptors that may be inherited from the parent process.*/
      for (i = static_cast<int>(sysconf(_SC_OPEN_MAX)); i >= 0; i--)
      {
        close(i);
      }

      if (mDaemonServiceParam_X.DaemonServiceLogPriority_i || mDaemonServiceParam_X.DaemonServiceLogFacility_i)
      {
        mLogActive_B = true;
        /* Open the log file ->check /etc/syslog.conf to check syslog channel message routing -> result in /var/log/user.log */
        setlogmask(LOG_UPTO(mDaemonServiceParam_X.DaemonServiceLogPriority_i));
        // openlog(mDaemonServiceParam_X.Name_S.c_str(), LOG_CONS | LOG_PID | LOG_NDELAY, LOG_DAEMON);
        openlog(mDaemonServiceParam_X.Name_S.c_str(), LOG_PID | LOG_NDELAY, mDaemonServiceParam_X.DaemonServiceLogFacility_i); // LOG_USER as LOG_DAEMON is not active on my system
        DaemonServiceLog("---Daemon started '%s'-----------------------------------", mDaemonServiceParam_X.Name_S.c_str());
      }
      // Register action to detect the "stop daemon"
      memset(&SigAction_X, 0, sizeof(struct sigaction));
      SigAction_X.sa_handler = &BOF::SignalHandler;
      if (sigaction(SIGTERM, &SigAction_X, nullptr) == -1)
      {
        mDaemonServiceParam_X.Sts_E = BOF_ERR_INTERNAL;
        DaemonExit("SIGTERM", EXIT_FAILURE);
      }
      if (sigaction(SIGINT, &SigAction_X, nullptr) == -1)
      {
        mDaemonServiceParam_X.Sts_E = BOF_ERR_INTERNAL;
        DaemonExit("SIGINT", EXIT_FAILURE);
      }

      if (mDaemonServiceParam_X.PidFilePath.IsValid())
      {
        DaemonServiceLog("Create file '%s'", mDaemonServiceParam_X.PidFilePath.FullPathName(false).c_str());
        mDaemonPidFileHandle_i = open(mDaemonServiceParam_X.PidFilePath.FullPathName(false).c_str(), O_RDWR | O_CREAT, 0600);
        if (mDaemonPidFileHandle_i == -1)
        {
          /* Couldn't open lock file */
          mDaemonServiceParam_X.Sts_E = BOF_ERR_NOT_OPENED;
          DaemonExit("DaemonPidFileHandle open", EXIT_FAILURE);
        }

        /* Try to lock file */
#if defined(ANDROID)
#else
        if (lockf(mDaemonPidFileHandle_i, F_TLOCK, 0) == -1)
        {
          /* Couldn't get lock on lock file */
          mDaemonServiceParam_X.Sts_E = BOF_ERR_LOCK;
          DaemonExit("DaemonPidFileHandle lock", EXIT_FAILURE);
        }
#endif
        /* Get and format PID */
        sprintf(pBuf_c, "%d\n", getpid());

        /* write pid to lockfile */
        if (write(mDaemonPidFileHandle_i, pBuf_c, strlen(pBuf_c)) == -1)
        {
          mDaemonServiceParam_X.Sts_E = BOF_ERR_WRITE;
          DaemonExit("DaemonPidFileHandle write", EXIT_FAILURE);
        }
      }
      else
      {
        DaemonServiceLog("Do not create pid file");
      }
      mDaemonServiceParam_X.Sts_E = BOF_ERR_NO_ERROR;
    }
  }
  DaemonServiceLog("Return(0x%08X) to caller for daemon processing main loop", mDaemonServiceParam_X.Sts_E);
}

void BofDaemonService::Shutdown()
{
  DaemonServiceLog("Shutdown: FileHandle %d Path %s\n", mDaemonPidFileHandle_i, mDaemonServiceParam_X.PidFilePath.FullPathName(false).c_str());

  if (mDaemonPidFileHandle_i > 0)
  {
    close(mDaemonPidFileHandle_i);
    mDaemonPidFileHandle_i = -1;
    Bof_DeleteFile(mDaemonServiceParam_X.PidFilePath);
  }

  if (mDaemonServiceParam_X.pDaemonHasExited_B)
  {
    DaemonServiceLog("Send Exit signal to %p\n", mDaemonServiceParam_X.pDaemonHasExited_B);
    *mDaemonServiceParam_X.pDaemonHasExited_B = true;
    mDaemonServiceParam_X.pDaemonHasExited_B = nullptr;
  }
  if (mLogActive_B)
  {
    closelog();
    mLogActive_B = false;
  }
}

int BofDaemonService::GetSyslogPriority()
{
  return mDaemonServiceParam_X.DaemonServiceLogPriority_i;
}

void BofDaemonService::DaemonServiceLog(const char *_pFormat_c, ...)
{
  char pLog_c[0x1000];
  va_list Arg;

  if (mLogActive_B)
  {
    va_start(Arg, _pFormat_c);
    vsnprintf(pLog_c, sizeof(pLog_c), _pFormat_c, Arg);
    va_end(Arg);
    syslog(mDaemonServiceParam_X.DaemonServiceLogPriority_i, "%s", pLog_c);
  }
}

void BofDaemonService::DaemonExit(const char *_pExitMsg_c, int _ExitCode_i)
{
  DaemonServiceLog(_pExitMsg_c);
  exit(_ExitCode_i);
}

BofDaemonService::BofDaemonService()
{
}

BofDaemonService::~BofDaemonService()
{
}

BofDaemonService &BofDaemonService::S_Instance()
{
  // Since it's a static variable, if the class has already been created, It won't be created again so it **is** thread-safe in C++11.
  static BofDaemonService TheBofDaemonServiceInstance;
  return TheBofDaemonServiceInstance;
}

#endif
END_BOF_NAMESPACE()