/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bof shell class
 *
 * Name:        bofshell.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         None
 *
 */

#include <bofstd/boffs.h>
#include <bofstd/bofprocess.h>
#include <bofstd/bofshell.h>
#include <bofstd/bofstring.h>

#include <regex>

BEGIN_BOF_NAMESPACE()

BofShell::BofShell(BOF_SHELL_PARAM &_rShellParam_X)
    : mShellParam_X(_rShellParam_X)
{
  AddCommand("q", BOF_SHELL_CMD("Leave shell console.", {}, nullptr, BOF_BIND_2_ARG_TO_METHOD(this, BofShell::ShellQuit)));
  AddCommand("?", BOF_SHELL_CMD("Show the list of commands.", {}, nullptr, BOF_BIND_2_ARG_TO_METHOD(this, BofShell::ShellHelp)));

  AddCommand("arg", BOF_SHELL_CMD("Show descrition of the args of a command.", {"ShellCmd"}, &mArgShellCmd_S, BOF_BIND_2_ARG_TO_METHOD(this, BofShell::ShellCmdArg)));
  AddCommandArgument("arg", "ShellCmd",
                     BOF::BOFPARAMETER(nullptr, "", "Specify which shell command must show its argument", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(mArgShellCmd_S, STDSTRING, 0, 0)));
  AddCommand("spawn", BOF_SHELL_CMD("Spawn a process given in argument.", {"ProcessPath", "ProcessArg"}, &mSpawnParam_X, BOF_BIND_2_ARG_TO_METHOD(this, BofShell::ShellSpawn)));
  AddCommandArgument("spawn", "ProcessPath",
                     BOF::BOFPARAMETER(nullptr, "", "Specify the process to spawn", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG | BOF::BOFPARAMETER_ARG_FLAG::PATH_MUST_EXIST, BOF_PARAM_DEF_VARIABLE(mSpawnParam_X.ProcessPath, PATH, 0, 0)));
  AddCommandArgument("spawn", "ProcessArg",
                     BOF::BOFPARAMETER(nullptr, "", "Specify the argument of the spawned process", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(mSpawnParam_X.ProcessArg_S, STDSTRING, 0, 0)));

  AddCommand("sleep", BOF_SHELL_CMD("Sleep the script for a given number of ms.", {"SleepTimeInMs"}, &mSleepTime_U32, BOF_BIND_2_ARG_TO_METHOD(this, BofShell::ShellSleep)));
  AddCommandArgument("sleep", "SleepTimeInMs",
                     BOF::BOFPARAMETER(nullptr, "", "Specify the number of millisecond to sleep", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(mSleepTime_U32, UINT32, 0, 0)));
  AddCommand("exec", BOF_SHELL_CMD("Execute a script from a script.", {"ScripFilename"}, &mScriptPath, BOF_BIND_2_ARG_TO_METHOD(this, BofShell::ShellExec)));
  AddCommandArgument("exec", "ScripFilename",
                     BOF::BOFPARAMETER(nullptr, "", "Specify the script to execute", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG | BOF::BOFPARAMETER_ARG_FLAG::PATH_MUST_EXIST, BOF_PARAM_DEF_VARIABLE(mScriptPath, PATH, 0, 0)));

  AddCommand("echo", BOF_SHELL_CMD("Display the argument string in the output channel.", {"EchoText"}, &mEchoText_S, BOF_BIND_2_ARG_TO_METHOD(this, BofShell::ShellEcho)));
  AddCommandArgument("echo", "EchoText",
                     BOF::BOFPARAMETER(nullptr, "", "Specify which shell command must show its argument", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(mEchoText_S, STDSTRING, 0, 0)));

  AddCommand("dywtc?", BOF_SHELL_CMD("Ask if the user want to continue.", {}, &mEchoText_S, BOF_BIND_2_ARG_TO_METHOD(this, BofShell::ShellDoYouWantToContinue)));
}
BofShell::~BofShell()
{
}

BOFERR BofShell::AddCommand(const std::string &_rShellCmd_S, const BOF_SHELL_CMD &_rShellCmd_X)
{
  BOFERR Rts_E;

  auto It = mShellCmdCollection.insert(std::pair<std::string, BOF_SHELL_CMD>(Bof_StringTrim(_rShellCmd_S), _rShellCmd_X));
  Rts_E = It.second ? BOF_ERR_NO_ERROR : BOF_ERR_EEXIST;
  return Rts_E;
}

BOFERR BofShell::AddCommandArgument(const std::string &_rShellCmd_S, const std::string &_rShellArg_S, const BOFPARAMETER &_rShellArg_X)
{
  BOFERR Rts_E = BOF_ERR_DONT_EXIST;
  std::string Cmd_S, Prefix_S, ArgName_S;
  int Index_i;

  Cmd_S = Bof_StringTrim(_rShellCmd_S);
  auto ItCmd = mShellCmdCollection.find(Cmd_S);
  if (ItCmd != mShellCmdCollection.end())
  {
    Rts_E = BOF_ERR_EEXIST;
    Prefix_S = Cmd_S + "::";
    ArgName_S = Bof_StringTrim(_rShellArg_S);
    Index_i = BOF::Bof_StringIsPresent(ItCmd->second.ArgNameCollection, ArgName_S);
    if (Index_i < 0)
    {
      ArgName_S = Prefix_S + ArgName_S;
      Index_i = BOF::Bof_StringIsPresent(ItCmd->second.ArgNameCollection, ArgName_S);
    }
    if (Index_i >= 0)
    {
      auto It = mShellArgCollection.find(ArgName_S);
      if (It == mShellArgCollection.end())
      {
        if (ArgName_S.find(Prefix_S) != 0)
        {
          ArgName_S = Prefix_S + ArgName_S;
        }
        mShellArgCollection[ArgName_S] = _rShellArg_X;
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
  }
  // printf("Add Key: '%s' Desc '%s' Rts %d\n", ArgName_S.c_str(), _rShellArg_X.Description_S.c_str(), Rts_E);
  return Rts_E;
}
BOFERR BofShell::ExecScript(const BofPath &_rScriptPath)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  mScriptPath = _rScriptPath;
  mExecScript_B = (mScriptPath.FileNameWithExtension() != "");
  if (mExecScript_B)
  {
    mScriptFilename_S = mScriptPath.FileNameWithExtension();
    Rts_E = Interpreter("");
  }
  else
  {
    mScriptFilename_S = "";
  }
  return Rts_E;
}

BOFERR BofShell::ShellHelp(void *_pArg, std::string &_rShellRes_S)
{
  uint32_t i_U32;

  _rShellRes_S = "";

  auto pIt = mShellCmdCollection.begin();
  while (pIt != mShellCmdCollection.end())
  {
    _rShellRes_S = _rShellRes_S + Bof_Sprintf("%-12s: %s [", pIt->first.c_str(), pIt->second.Help_S.c_str());
    i_U32 = 0;
    for (auto &rIt : pIt->second.ArgNameCollection)
    {
      _rShellRes_S += rIt;
      if (i_U32 != (pIt->second.ArgNameCollection.size() - 1))
      {
        _rShellRes_S += ", ";
      }
      i_U32++;
    }
    _rShellRes_S += "]\n";
    pIt++;
  }
  return BOF_ERR_NO_ERROR;
}
BOFERR BofShell::ShellQuit(void *_pArg, std::string &_rShellRes_S)
{
  mFinish_B = true;
  return BOF_ERR_NO_ERROR;
}
BOFERR BofShell::ShellExec(void *_pArg, std::string &_rShellRes_S)
{
  BOFERR Rts_E;

  if (BOF_IS_HANDLE_VALID(mIoScript))
  {
    Bof_CloseFile(mIoScript);
    mIoScript = BOF_INVALID_HANDLE_VALUE;
    mExecScript_B = false;
  }
  Rts_E = ExecScript(mScriptPath);
  return BOF_ERR_NO_ERROR;
}
BOFERR BofShell::ShellCmdArg(void *_pArg, std::string &_rShellResult_S)
{
  BOFERR Rts_E = BOF_ERR_DONT_EXIST;
  std::string Cmd_S, Prefix_S, ArgName_S;
  int Index_i;

  mArgShellCmd_S = Bof_StringTrim(mArgShellCmd_S);
  _rShellResult_S = "Argument of command '" + mArgShellCmd_S + "' are:\n";
  auto ItCmd = mShellCmdCollection.find(mArgShellCmd_S);
  if (ItCmd != mShellCmdCollection.end())
  {
    Rts_E = BOF_ERR_NO_ERROR;
    Prefix_S = mArgShellCmd_S + "::";
    for (auto Item : ItCmd->second.ArgNameCollection)
    {
      ArgName_S = Bof_StringTrim(Item);
      if (ArgName_S.find(Prefix_S) != 0)
      {
        ArgName_S = Prefix_S + ArgName_S;
      }
      auto It = mShellArgCollection.find(ArgName_S);
      if (It != mShellArgCollection.end())
      {
        _rShellResult_S = _rShellResult_S + Bof_Sprintf("  %-22s: %s\n", It->first.c_str(), It->second.Description_S.c_str());
      }
    }
  }
  return Rts_E;
}
BOFERR BofShell::ShellEcho(void *_pArg, std::string &_rShellResult_S)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  std::string Cmd_S, Prefix_S, ArgName_S;
  int Index_i;

  mEchoText_S = Bof_StringTrim(mEchoText_S);
  _rShellResult_S = mEchoText_S;
  return Rts_E;
}

// spawn(/opt/evs/AppFs/evs-gbio/bin/gbio_irq_listen,hello)
BOFERR BofShell::ShellSpawn(void *_pArg, std::string &_rShellResult_S)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BOF_SHELL_SPAWN_PARAM *pSpawnParam_X = (BOF_SHELL_SPAWN_PARAM *)_pArg;
  int ExitCode_i;
  BOF_PROCESS Pid_X;

  _rShellResult_S = "";
  if (pSpawnParam_X)
  {
    _rShellResult_S = "Spawn '" + pSpawnParam_X->ProcessPath.ToString(false) + "'(" + pSpawnParam_X->ProcessArg_S + ")\n";
    Rts_E = BOF::BofProcess::S_SpawnProcess(pSpawnParam_X->ProcessPath.ToString(false).c_str(), pSpawnParam_X->ProcessArg_S.c_str(), 0, Pid_X, ExitCode_i);
    if ((Rts_E == BOF_ERR_NO_ERROR) && (ExitCode_i == 0) && (Pid_X.Pid > 0))
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
    else
    {
      Rts_E = BOF_ERR_ENOEXEC;
    }
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
    }
  }
  _rShellResult_S = "Spawn Sts=" + std::string(BOF::Bof_ErrorCode(Rts_E)) + "\n" + _rShellResult_S;
  return Rts_E;
}

BOFERR BofShell::ShellDoYouWantToContinue(void *_pArg, std::string &_rShellResult_S)
{
  BOFERR Rts_E = BOF_ERR_NO;

  if ((mShellParam_X.InputStream) && (mShellParam_X.OutputStream))
  {
    if (mShellParam_X.OutputStream(false, "Are you sure you want to continue (Y/N) ?\n") == BOF_ERR_NO_ERROR)
    {
      if (mShellParam_X.InputStream("", _rShellResult_S) == BOF_ERR_NO_ERROR)
      {
        if (_rShellResult_S == "Y")
        {
          _rShellResult_S = "";
          Rts_E = BOF_ERR_NO_ERROR;
        }
      }
    }
  }
  return Rts_E;
}

BOFERR BofShell::ShellSleep(void *_pArg, std::string &_rShellResult_S)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;

  BOF::Bof_MsSleep(mSleepTime_U32);
  return Rts_E;
}
BOFERR BofShell::Parser(const std::string &_rShellCmd_S, std::string &_rShellRes_S)
{
  BOFERR Rts_E = BOF_ERR_FORMAT;
  static const std::regex S_RegExFnArg("^(.*)\\((.*)\\)");
  std::smatch MatchString;
  std::string Cmd_S, CmdArg_S, Prefix_S;
  const char *pBofDefaultTrimCharList_c = " \a\f\n\r\t\v\"";
  std::vector<std::string> CmdArgCollection;
  uint32_t i_U32;

  Cmd_S = Bof_StringTrim(_rShellCmd_S);
  CmdArg_S = "";
  Rts_E = BOF_ERR_NO_ERROR;
  if (std::regex_search(_rShellCmd_S, MatchString, S_RegExFnArg))
  {
    /*
    printf("%z result:\n", MatchString.size());
    for (auto i = 0; i < MatchString.size(); i++)
    {
      printf("[%02d]='%s'\n", i, MatchString[i].str().c_str());
    }
    */
    Cmd_S = "";
    if (MatchString.size() > 1)
    {
      Cmd_S = Bof_StringTrim(MatchString[1].str());
      if (MatchString.size() > 2)
      {
        CmdArg_S = Bof_StringTrim(MatchString[2].str());
        CmdArgCollection = Bof_StringSplit(CmdArg_S, ",");
        for (auto &rIt : CmdArgCollection)
        {
          rIt = Bof_StringTrim(rIt, " \"");
        }
        if ((CmdArgCollection.size() == 1) && (CmdArgCollection[0] == ""))
        {
          CmdArgCollection.clear();
        }
      }
    }
  }
  auto It = mShellCmdCollection.find(Cmd_S);
  if (It != mShellCmdCollection.end())
  {
    if (CmdArgCollection.size() == It->second.ArgNameCollection.size())
    {
      i_U32 = 0;
      Prefix_S = Cmd_S + "::";
      for (auto &rParamName : It->second.ArgNameCollection)
      {
        if (rParamName.find(Prefix_S) != 0)
        {
          rParamName = Prefix_S + rParamName;
        }
        /*
        for (auto it = mShellArgCollection.begin(); it != mShellArgCollection.end(); ++it)
        {
          printf("check Key: '%s' Desc '%s'\n", it->first.c_str(), it->second.Description_S.c_str());
        }
        */
        auto pBofParam = mShellArgCollection.find(rParamName);
        if (pBofParam == mShellArgCollection.end())
        {
          pBofParam = mShellArgCollection.find(Cmd_S + "::" + rParamName);
        }
        if (pBofParam != mShellArgCollection.end())
        {
          Rts_E = BofParameter::S_Parse(0, pBofParam->second, CmdArgCollection[i_U32].c_str(), nullptr);
          if (Rts_E != BOF_ERR_NO_ERROR)
          {
            break;
          }
        }
        else
        {
          Rts_E = BOF_ERR_DONT_EXIST;
          break;
        }
        i_U32++;
      }
      // if (i_U32 >= It->second.ArgNameCollection.size())
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Rts_E = It->second.CmdCallback(It->second.pBaseArg, _rShellRes_S);
      }
    }
    else
    {
      Rts_E = BOF_ERR_ETOOMANYREFS;
    }
  }
  else
  {
    Rts_E = BOF_ERR_NOT_FOUND;
  }
  return Rts_E;
}
BOFERR BofShell::Execute(const std::string &_rShellCmd_S)
{
  BOFERR Rts_E;

  mExecuteOnlyOneCommand_B = true;
  Rts_E = Interpreter(_rShellCmd_S);
  mExecuteOnlyOneCommand_B = false;
  return Rts_E;
}
BOFERR BofShell::Interpreter(const std::string &_rFirstCommand_S)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  std::string Pwd_S, ScriptLine_S, CmdLine_S;
  bool FirstCommand_B = false;

  uint32_t i_U32;
  std::string ShellRes_S;

  mFinish_B = mShellParam_X.InputStream ? false : true;
  if (mExecuteOnlyOneCommand_B)
  {
    mFinish_B = true;
  }
  if (_rFirstCommand_S != "")
  {
    FirstCommand_B = true;
    CmdLine_S = _rFirstCommand_S;
    if (mShellParam_X.OutputStream)
    {
      mShellParam_X.OutputStream(false, (mExecuteOnlyOneCommand_B ? "EXEC: " : "FIRST: ") + CmdLine_S + '\n');
      // std::string Cwd_S;
      // Bof_GetCurrentDirectory(Cwd_S);
      // printf("cwd %s\n", Cwd_S.c_str());
    }
  }
  do
  {
    if ((!mExecScript_B) && (!FirstCommand_B))
    {
      if (mShellParam_X.InputStream)
      {
        mShellParam_X.InputStream(mShellParam_X.Prompt_S, CmdLine_S);
      }
    }
    FirstCommand_B = false;

    if ((CmdLine_S != "") && (CmdLine_S[0] != '#')) // Comment in script file
    {
      ShellRes_S = "";
      Rts_E = Parser(CmdLine_S.c_str(), ShellRes_S);
      if ((ShellRes_S != "") && (mShellParam_X.OutputStream))
      {
        mShellParam_X.OutputStream(false, ShellRes_S + '\n');
      }
    }
    else
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
    if (Rts_E != BOF_ERR_NO_ERROR)
    {
      if (mShellParam_X.OutputStream)
      {
        mShellParam_X.OutputStream(true, BOF::Bof_Sprintf("Error %d -> %s\n", Rts_E, Bof_ErrorCode(Rts_E)));
      }
      if (mExecScript_B)
      {
        Bof_CloseFile(mIoScript);
        mIoScript = BOF_INVALID_HANDLE_VALUE;
        mExecScript_B = false;
        break;
      }
    }
    else
    {
      if (mExecScript_B)
      {
        if (!BOF_IS_HANDLE_VALID(mIoScript))
        {
          Bof_GetCurrentDirectory(Pwd_S);
          if (mShellParam_X.OutputStream)
          {
            mShellParam_X.OutputStream(false, BOF::Bof_Sprintf("Execute '%s' script from '%s'\n", mScriptPath.FullPathName(false).c_str(), Pwd_S.c_str()));
          }
          Rts_E = Bof_OpenFile(mScriptPath, true, false, mIoScript);
          if (Rts_E != BOF_ERR_NO_ERROR)
          {
            if (mShellParam_X.OutputStream)
            {
              mShellParam_X.OutputStream(true, BOF::Bof_Sprintf("Cannot open script file (%s)\n", Bof_ErrorCode(Rts_E)));
            }
            mIoScript = BOF_INVALID_HANDLE_VALUE;
            mExecScript_B = false;
          }
        }

        if (BOF_IS_HANDLE_VALID(mIoScript))
        {
          if (Bof_ReadLine(mIoScript, ScriptLine_S) == BOF_ERR_NO_ERROR)
          {
            ScriptLine_S = Bof_StringTrim(ScriptLine_S);
            if (mShellParam_X.OutputStream)
            {
              mShellParam_X.OutputStream(false, BOF::Bof_Sprintf("[%s]: %s\n", mScriptFilename_S.c_str(), ScriptLine_S.c_str()));
            }
            CmdLine_S = ScriptLine_S;
          }
          else
          {
            Bof_CloseFile(mIoScript);
            mIoScript = BOF_INVALID_HANDLE_VALUE;
            mExecScript_B = false;
          }
        }
        mFinish_B = !mExecScript_B;
      }
    }
  } while (!mFinish_B);

  return Rts_E;
}

BofShellConsole::BofShellConsole(BOF_SHELL_CONSOLE_PARAM &_rShellConsoleParam_X)
{
  mShellConsoleParam_X = _rShellConsoleParam_X;

  if (mShellConsoleParam_X.AnsiMode_B)
  {
    mpuConio = std::make_unique<BofConio>(mShellConsoleParam_X.ConioParam_X);
    if (mShellConsoleParam_X.ShellCmdInputColor_E != CONIO_TEXT_COLOR_NONE)
    {
      mpuConio->SetForegroundTextColor(mShellConsoleParam_X.ShellCmdInputColor_E);
    }
    if (mShellConsoleParam_X.ShellBackColor_E != CONIO_TEXT_COLOR_NONE)
    {
      mpuConio->SetBackgroundTextColor(mShellConsoleParam_X.ShellBackColor_E);
    }
    if (mShellConsoleParam_X.ClearScreen_B)
    {
      mpuConio->Clear(CONIO_CLEAR::CONIO_CLEAR_ALL);
      mpuConio->SetTextCursorPosition(1, 1);
    }
    if (mShellConsoleParam_X.WindowTitle_S != "")
    {
      mpuConio->SetTextWindowTitle(mShellConsoleParam_X.WindowTitle_S);
    }
  }
}
BofShellConsole::~BofShellConsole()
{
}
BOFERR BofShellConsole::InputStream(const std::string &_rPrompt_S, std::string &_rShellCmd_S)
{
  BOFERR Rts_E;
  char pInput_c[0x1000];

  if (mShellConsoleParam_X.AnsiMode_B)
  {
    mpuConio->PrintfColor(mShellConsoleParam_X.ShellCmdInputColor_E, "%s", "");
    Rts_E = mpuConio->Readline(_rPrompt_S, _rShellCmd_S);
  }
  else
  {
    if (fgets(pInput_c, sizeof(pInput_c), stdin) == nullptr)
    {
      Rts_E = BOF_ERR_EOF;
    }
    else
    {
      _rShellCmd_S = pInput_c;
      Rts_E = BOF_ERR_NO_ERROR;
    }
  }
  return Rts_E;
}

BOFERR BofShellConsole::OutputStream(bool _Error_B, const std::string &_rShellRes_S)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;

  if (mShellConsoleParam_X.AnsiMode_B)
  {
    mpuConio->PrintfColor(_Error_B ? mShellConsoleParam_X.ShellErrorColor_E : mShellConsoleParam_X.ShellCmdOutputColor_E, "%s", _rShellRes_S.c_str());
  }
  else
  {
    printf("%s", _rShellRes_S.c_str());
  }
  return Rts_E;
}
END_BOF_NAMESPACE()