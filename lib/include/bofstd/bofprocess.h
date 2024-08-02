/*!
   Copyright (c) 2008, Onbings All rights reserved.

   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
   PURPOSE.

   Remarks

   Name:			        ScriptLauncher.h
   Author:		        Bernard HARMEL: onbings@dscloud.me

   Summary:

    The class that manages launching script from
    within C code

   History:
    V 1.00  September 25 2013  BHA : Original version
 */
#pragma once

#if defined(_WIN32)
#define NOMINMAX
#define _WINSOCKAPI_
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <bofstd/bofstd.h>

#include <cstdint>

BEGIN_BOF_NAMESPACE()

#if defined(_WIN32)
struct BOF_PROCESS
{
  int Pid;
  PROCESS_INFORMATION Pi_X;
  BOF_PROCESS()
  {
    Reset();
  }
  void Reset()
  {
    Pid = -1;
    memset(&Pi_X, 0, sizeof(Pi_X));
  }
};
#else
struct BOF_PROCESS
{
  pid_t Pid;
  BOF_PROCESS()
  {
    Reset();
  }
  void Reset()
  {
    Pid = -1;
  }
};
#endif

class BOFSTD_EXPORT BofProcess
{
public:
  enum ExecuteMode
  {
    EXECUTE_VFORK,
    EXECUTE_POPEN,
    EXECUTE_POSIX_SPAWN
  };

  static BOFERR S_SetDefaultExecuteMode(ExecuteMode _Mode_E);
  static ExecuteMode S_GetDefaultExecuteMode();
  static BOFERR S_SetDefaultTimeout(uint32_t _Timeout_U32);
  static uint32_t S_GetDefaultTimeout();

  static BOFERR S_Execute(const char *_pCommand_c, BOF_PROCESS &_rPid_X, int &_rExitCode_i);
  static BOFERR S_Execute(const char *_pCommand_c, ExecuteMode _Mode_E, BOF_PROCESS &_rPid_X, int &_rExitCode_i);
  static BOFERR S_Execute(const char *_pCommand_c, uint32_t _Timeout_U32, BOF_PROCESS &_rPid_X, int &_rExitCode_i);
  static BOFERR S_Execute(const char *_pCommand_c, uint32_t _Timeout_U32, ExecuteMode _Mode_E, BOF_PROCESS &_rPid_X, int &_rExitCode_i);
  static BOFERR S_Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, BOF_PROCESS &_rPid_X, int &_rExitCode_i);
  static BOFERR S_Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, ExecuteMode _Mode_E, BOF_PROCESS &_rPid_X, int &_rExitCode_i);
  static BOFERR S_Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32, BOF_PROCESS &_rPid_X, int &_rExitCode_i);
  static BOFERR S_Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32, ExecuteMode _Mode_E, BOF_PROCESS &_rPid_X, int &_rExitCode_i);
  //More secure than the next one
  static BOFERR S_Execute_popen(const std::string &_rCommand_S, std::string &_rOutput_S, int &_rExitCode_i);
  static BOFERR S_Execute_popen(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32, BOF_PROCESS &_rPid_X, int &_rExitCode_i);
  static BOFERR S_Execute_posix_spawn(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32, BOF_PROCESS &_rPid_X, int &_rExitCode_i);
  static BOFERR S_Execute_vfork(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32, BOF_PROCESS &_rPid_X, int &_rExitCode_i);

  static BOFERR S_ReadU32FromFile(const char *_pFile_c, uint32_t *_pValue_U32);
  static BOFERR S_WriteU32ToFile(const char *_pFile_c, uint32_t _Value_U32);

  static BOFERR S_SpawnProcess(const char *_pProgram_c, const char *_pArguments_c, uint16_t _DbgPort_U16, BOF_PROCESS &_rPid_X, int &_rExitCode_i);
  static BOFERR S_KillProcess(const char *_pProcessName_c);
  static BOFERR S_KillProcess(BOF_PROCESS _Pid_X);
  static bool S_IsProcessRunning(BOF_PROCESS _Pid_X);
  static BOFERR S_AttachProcessToDebugger(BOF_PROCESS _Pid_X);
protected:
private:
  static uint32_t S_mDefaultTimeout_U32;
  static ExecuteMode S_mDefaultMode_E;
};
END_BOF_NAMESPACE()

