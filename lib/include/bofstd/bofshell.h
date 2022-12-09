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
#pragma once

#include <bofstd/bofconio.h>
#include <bofstd/bofparameter.h>

#include <map>

BEGIN_BOF_NAMESPACE()


using BOF_SHELL_CMD_CALLBACK = std::function<BOFERR()>; // const std::vector<std::string> &_rCmdArgCollection) > ;

struct BOF_SHELL_CMD
{
  std::string            Help_S;
  std::vector<std::string> ArgNameCollection;
  BOF_SHELL_CMD_CALLBACK CmdCallback;

  BOF_SHELL_CMD()
  {
    Reset();
  }

  BOF_SHELL_CMD(const std::string &_rHelp_S, std::vector<std::string> _ArgNameCollection, const BOF_SHELL_CMD_CALLBACK &_rCmdCallback)
  {
    Help_S = _rHelp_S;
    ArgNameCollection = _ArgNameCollection;
    CmdCallback = _rCmdCallback;
  }

  void Reset()
  {
    Help_S = "";
    ArgNameCollection.clear();
    CmdCallback = nullptr;
  }
};

struct BOFSTD_EXPORT BOF_SHELL_PARAM
{
  BOF_CONIO_PARAM ConioParam_X;
  bool InteractiveMode_B;
  std::string WindowTitle_S;
  std::string Prompt_S;
  std::map<std::string, BOF_SHELL_CMD> ShellCmdCollection;
  std::map<std::string, BOFPARAMETER>  ShellArgCollection;
  CONIO_TEXT_COLOR ShellBackColor_E;
  CONIO_TEXT_COLOR ShellCmdInputColor_E;
  CONIO_TEXT_COLOR ShellCmdOutputColor_E;
  CONIO_TEXT_COLOR ShellErrorColor_E;
  CONIO_TEXT_COLOR ShellScriptColor_E;
  bool ForceMode_B;
  //Returned param (output from constructor)
  std::shared_ptr<BOF::BofConio> psConio = nullptr;

  BOF_SHELL_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    ConioParam_X.Reset();
    InteractiveMode_B = false;
    WindowTitle_S = "";
    Prompt_S = "";
    ShellCmdCollection.clear();
    ShellArgCollection.clear();
    ShellBackColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BLACK;
    ShellCmdInputColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_BLUE;
    ShellCmdOutputColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_GREEN;
    ShellErrorColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_RED;
    ShellScriptColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_CYAN;
    ForceMode_B = false;
    //Returned param (output from constructor)
    psConio = nullptr;
  }
};

class BOFSTD_EXPORT BofShell
{
private:
  BOF_SHELL_PARAM mShellParam_X;

public:
  BofShell(const BOF_SHELL_PARAM &_rShellParam_X);
  virtual ~BofShell();

  BOFERR Parser(const std::string &_rShellCmd_S);
  BOFERR Interpreter();
  BOFERR ShellHelp();
  BOFERR ShellQuit();
  bool DoYouWantToContinue();
  BOFERR ExecScript(const BofPath &_rScriptFile);
  BOFERR AddCommand(const std::string &_rCmd_S, const BOF_SHELL_CMD &_rShellCmd);

private:
  bool  mFinish_B = false;
  bool mExecScript_B = false;
  BofPath mScriptFile;
};

END_BOF_NAMESPACE()