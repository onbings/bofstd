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

#if defined (_WIN32)
#else

#include <bofstd/bofstd.h>

#include <cstdint>

BEGIN_BOF_NAMESPACE()

class BOFSTD_EXPORT BofScriptLauncher
{
public:

  enum ExecuteMode
  {
    EXECUTE_VFORK,
    EXECUTE_POPEN,
    EXECUTE_POSIX_SPAWN
  };

  static int Execute(const char *_pCommand_c);

  static int Execute(const char *_pCommand_c, ExecuteMode _Mode_E);

  static int Execute(const char *_pCommand_c, uint32_t _Timeout_U32);

  static int Execute(const char *_pCommand_c, uint32_t _Timeout_U32, ExecuteMode _Mode_E);

  static int Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c);

  static int Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, ExecuteMode _Mode_E);

  static int Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32);

  static int Execute(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32, ExecuteMode _Mode_E);

  static int Execute_popen(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32);

  static int Execute_posix_spawn(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32);

  static int Execute_vfork(char *_pOutput_c, uint32_t _Size_U32, const char *_pCommand_c, uint32_t _Timeout_U32);

  static bool SetDefaultExecuteMode(ExecuteMode _Mode_E);

  static ExecuteMode GetDefaultExecuteMode();

  static bool SetDefaultTimeout(uint32_t _Timeout_U32);

  static uint32_t GetDefaultTimeout();

  static bool ReadU32FromFile(const char *_pFile_c, uint32_t *_pValue_U32);

  static bool WriteU32ToFile(const char *_pFile_c, uint32_t _Value_U32);

protected:

private:

  static uint32_t S_mDefaultTimeout_U32;
  static ExecuteMode S_mDefaultMode_E;
};
END_BOF_NAMESPACE()

#endif