/*
* Copyright (c) 2015-2020, Onbings. All rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
* PURPOSE.
*
* This module defines the bofdaemonservice class
*
* Name:        bofdaemonservice.h
* Author:      Bernard HARMEL: onbings@dscloud.me
* Web:			    onbings.dscloud.me
* Revision:    1.0
*
* Rem:         Nothing
*
* History:
*
* V 1.00  Dec 26 2013  BHA : Initial release
*/
#pragma once

#include <bofstd/bofpath.h>

#if defined(_WIN32)
#else

#include <syslog.h>

#endif
BEGIN_BOF_NAMESPACE()

struct BOFSTD_EXPORT BOF_DAEMON_SERVICE_PARAM
{
  std::string Name_S;
  BofPath PidFilePath;            //The path to where to store the PID of the daemon
  volatile bool *pDaemonHasExited_B;    //An optional pointer to a bool variable which will be set to 1 when SIGTERM is received
  //One of the following param must be != 0 to open a daemon service log chanel (linx: syslog windows: event viewer)
  int DaemonServiceLogPriority_i;    //This param is used by openlog (linux) and a syslog daemon channel is opened and active
  int DaemonServiceLogFacility_i;    //This param is used by openlog (linux) and a syslog daemon channel is opened and active
  BOFERR Sts_E;

  BOF_DAEMON_SERVICE_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    Name_S = "";
    PidFilePath = "";
    pDaemonHasExited_B = nullptr;
    DaemonServiceLogPriority_i = 0;
    DaemonServiceLogFacility_i = 0;
    Sts_E = BOF_ERR_NO_ERROR;
  }
};

class BOFSTD_EXPORT BofDaemonService
{
private:
  volatile int mDaemonPidFileHandle_i;
  BOF_DAEMON_SERVICE_PARAM mDaemonServiceParam_X;
  bool mLogActive_B;

private:
  BofDaemonService();

  virtual ~BofDaemonService();

  void DaemonExit(const char *_pExitMsg_c, int _ExitCode_i);

public:
  void Shutdown();
  void CreateDaemonService(BOF_DAEMON_SERVICE_PARAM &_rDaemonServiceParam_X);
  int GetSyslogPriority();
  void DaemonServiceLog(const char *_pFormat_c, ...);
  static BofDaemonService &S_Instance();

};

END_BOF_NAMESPACE()