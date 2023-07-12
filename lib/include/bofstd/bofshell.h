/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the shell functions
 *
 * Name:        bofshell.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 */
/*
Muse.mfs:
#
#Mfs general script file (a) b.harmel@evs.com
#
?
si(0, false, true)
ls(false,false,false)
im(C:/bld/evs-muse/resources/muse_storage/Still/blue.png, ImportedBlue.Png, "")
mv(STILL, ImportedBlue.Png, ImportedBlueRenamed.Png)
ex(STILL, ImportedBlueRenamed.Png, C:/bld/evs-muse/resources/muse_storage/Export/blue.png)
ls(true,false,false)
rm(STILL, ImportedBlueRenamed.Png)
ls(true,false,false)
*/
#pragma once

#include <bofstd/bofconio.h>
#include <bofstd/bofparameter.h>

#include <map>

BEGIN_BOF_NAMESPACE()

using BOF_SHELL_INPUT_CMD = std::function<BOFERR(const std::string &_rPrompt_S, std::string &_rShellCmd_S)>;
using BOF_SHELL_OUTPUT_RES = std::function<BOFERR(bool _Error_B, const std::string &_rShellRes_S)>;
using BOF_SHELL_CMD_CALLBACK = std::function<BOFERR(void *_pArg, std::string &_rShellRes_S)>; // const std::vector<std::string> &_rCmdArgCollection) > ;

struct BOF_SHELL_CMD
{
  std::string Help_S;
  std::vector<std::string> ArgNameCollection;
  void *pBaseArg;
  BOF_SHELL_CMD_CALLBACK CmdCallback;
  BOF_SHELL_CMD()
  {
    Reset();
  }

  BOF_SHELL_CMD(const std::string &_rHelp_S, std::vector<std::string> _ArgNameCollection, void *_pBaseArg, const BOF_SHELL_CMD_CALLBACK &_rCmdCallback)
  {
    Help_S = _rHelp_S;
    ArgNameCollection = _ArgNameCollection;
    pBaseArg = _pBaseArg;
    CmdCallback = _rCmdCallback;
  }

  void Reset()
  {
    Help_S = "";
    ArgNameCollection.clear();
    CmdCallback = nullptr;
  }
};

struct BOF_SHELL_CONSOLE_PARAM
{
  bool AnsiMode_B; // True: Use Conio ansi console interfase False: use std printf/scanf C function
                   //  All the following parameter are only used if AnsiMode_B is true
  bool ClearScreen_B;
  std::string WindowTitle_S;
  BOF_CONIO_PARAM ConioParam_X;
  CONIO_TEXT_COLOR ShellBackColor_E;
  CONIO_TEXT_COLOR ShellCmdInputColor_E;
  CONIO_TEXT_COLOR ShellCmdOutputColor_E;
  CONIO_TEXT_COLOR ShellErrorColor_E;
  // CONIO_TEXT_COLOR ShellScriptColor_E;
  BOF_SHELL_CONSOLE_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    AnsiMode_B = false;
    ClearScreen_B = false;
    WindowTitle_S = "";
    ConioParam_X.Reset();
    ShellBackColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_NONE;
    ShellCmdInputColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_NONE;
    ShellCmdOutputColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_NONE;
    ShellErrorColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_NONE;
    // ShellScriptColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_NONE;
  }
};
struct BOF_SHELL_PARAM
{
  BOF_SHELL_INPUT_CMD InputStream;
  BOF_SHELL_OUTPUT_RES OutputStream;
  std::string Prompt_S;

  BOF_SHELL_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    InputStream = nullptr;
    OutputStream = nullptr;
    Prompt_S = "";
  }
};

class BOFSTD_EXPORT BofShell
{
private:
  BOF_SHELL_PARAM mShellParam_X;

public:
  BofShell(BOF_SHELL_PARAM &_rShellParam_X);
  virtual ~BofShell();

  BOFERR AddCommand(const std::string &_rShellCmd_S, const BOF_SHELL_CMD &_rShellCmd_X);
  BOFERR AddCommandArgument(const std::string &_rShellCmd_S, const std::string &_rShellArg_S, const BOFPARAMETER &_rShellArg_X);
  BOFERR Parser(const std::string &_rShellCmd_S, std::string &_rShellResult_S);
  BOFERR Interpreter(const std::string &_rFirstCommand_S);
  BOFERR Execute(const std::string &_rShellCmd_S);
  BOFERR ExecScript(const BofPath &_rScriptPath);

private:
  BOFERR ShellHelp(void *_pArg, std::string &_rShellResult_S);
  BOFERR ShellQuit(void *_pArg, std::string &_rShellResult_S);
  BOFERR ShellExec(void *_pArg, std::string &_rShellResult_S);
  BOFERR ShellCmdArg(void *_pArg, std::string &_rShellResult_S);

  bool mFinish_B = false;
  bool mExecScript_B = false;
  bool mExecuteOnlyOneCommand_B = false;
  BofPath mScriptPath;
  std::string mScriptFilename_S;
  uintptr_t mIoScript = BOF_INVALID_HANDLE_VALUE;
  std::map<std::string, BOF_SHELL_CMD> mShellCmdCollection;
  std::map<std::string, BOFPARAMETER> mShellArgCollection;
  std::string mArgShellCmd_S;
};

class BOFSTD_EXPORT BofShellConsole
{
private:
  BOF_SHELL_CONSOLE_PARAM mShellConsoleParam_X;

public:
  BofShellConsole(BOF_SHELL_CONSOLE_PARAM &_rShellConsoleParam_X);
  virtual ~BofShellConsole();

  BOFERR InputStream(const std::string &_rPrompt_S, std::string &_rShellCmd_S);
  BOFERR OutputStream(bool _Error_B, const std::string &_rShellRes_S);

private:
  std::unique_ptr<BOF::BofConio> mpuConio = nullptr;
};
END_BOF_NAMESPACE()