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
#include <bofstd/bofshell.h>
#include <bofstd/bofstring.h>

#include <regex>

BEGIN_BOF_NAMESPACE()

BofShell::BofShell(BOF_SHELL_PARAM &_rShellParam_X) : mShellParam_X(_rShellParam_X)
{
  mShellParam_X.psConio = std::make_shared<BofConio>(mShellParam_X.ConioParam_X);
  _rShellParam_X.psConio = mShellParam_X.psConio;
}
BofShell::~BofShell()
{
}
BOFERR BofShell::SetExecScript(const BofPath &_rScriptPath)
{
  mScriptPath = _rScriptPath;
  mExecScript_B = (mScriptPath.FileNameWithExtension() != "");
  return BOF_ERR_NO_ERROR;
}

bool BofShell::DoYouWantToContinue()
{
  bool Rts_B = true;
  uint32_t Ch_U32;

  if (!mShellParam_X.ForceMode_B)
  {
    mShellParam_X.psConio->PrintfColor(mShellParam_X.ShellErrorColor_E, "%s\n", "Are you sure you want to continue (Y/N)");
    Ch_U32 = mShellParam_X.psConio->GetCh(false);
    if ((Ch_U32 & BOF::CONIO_KEY_MASK) == 'Y')
    {
      Rts_B = false;
    }
    mShellParam_X.psConio->PrintfColor(mShellParam_X.ShellCmdOutputColor_E, "%s\n", Rts_B ? "Yes" : "No");
  }
  return Rts_B;
}

BOFERR BofShell::ShellHelp()
{
  std::string Help_S;
  uint32_t i_U32;

  auto pIt = mShellParam_X.ShellCmdCollection.begin();
  while (pIt != mShellParam_X.ShellCmdCollection.end())
  {
    Help_S = Bof_Sprintf("%-8s: %s [", pIt->first.c_str(), pIt->second.Help_S.c_str());
    i_U32 = 0;
    for (auto &rIt : pIt->second.ArgNameCollection)
    {
      Help_S += rIt;
      if (i_U32 != (pIt->second.ArgNameCollection.size() - 1))
      {
        Help_S += ", ";
      }
      i_U32++;
    }
    Help_S += "]";
    mShellParam_X.psConio->PrintfColor(mShellParam_X.ShellCmdOutputColor_E, "%s\n", Help_S.c_str());
    pIt++;
  }
  return BOF_ERR_NO_ERROR;
}
BOFERR BofShell::ShellQuit()
{
  mFinish_B = true;
  return BOF_ERR_NO_ERROR;
}

BOFERR BofShell::AddCommand(const std::string &_rCmd_S, const BOF_SHELL_CMD &_rShellCmd)
{
  BOFERR Rts_E;

  auto It = mShellParam_X.ShellCmdCollection.insert(std::pair<std::string, BOF_SHELL_CMD>(_rCmd_S, _rShellCmd));
  Rts_E = It.second ? BOF_ERR_NO_ERROR : BOF_ERR_EXIST;
  return Rts_E;
}
BOFERR BofShell::Parser(const std::string &_rShellCmd_S)
{
  BOFERR Rts_E = BOF_ERR_FORMAT;
  static const std::regex S_RegExFnArg("^(.*)\\((.*)\\)");
  std::smatch MatchString;
  std::string Cmd_S, CmdArg_S;
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
  auto It = mShellParam_X.ShellCmdCollection.find(Cmd_S);
  if (It != mShellParam_X.ShellCmdCollection.end())
  {
    if (CmdArgCollection.size() == It->second.ArgNameCollection.size())
    {
      i_U32 = 0;
      for (auto &rParamName : It->second.ArgNameCollection)
      {
        auto pBofParam = mShellParam_X.ShellArgCollection.find(rParamName);
        if (pBofParam != mShellParam_X.ShellArgCollection.end())
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
        Rts_E = It->second.CmdCallback();
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

BOFERR BofShell::Interpreter(const std::string &_rFirstCommand_S)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  std::string Pwd_S, ScriptLine_S, CmdLine_S;
  bool FirstCommand_B;
  uintptr_t IoScript = BOF_INVALID_HANDLE_VALUE;
  uint32_t i_U32;

  mFinish_B = true;
  if (mExecScript_B)
  {
    mShellParam_X.InteractiveMode_B = false;
  }
  if (mShellParam_X.InteractiveMode_B)
  {
    mFinish_B = false;
    mShellParam_X.psConio->SetForegroundTextColor(mShellParam_X.ShellCmdInputColor_E);
    mShellParam_X.psConio->SetBackgroundTextColor(mShellParam_X.ShellBackColor_E);
    mShellParam_X.psConio->Clear(CONIO_CLEAR::CONIO_CLEAR_ALL);
    mShellParam_X.psConio->SetTextCursorPosition(1, 1);
    mShellParam_X.psConio->SetTextWindowTitle(mShellParam_X.WindowTitle_S);
  }

  if (_rFirstCommand_S != "")
  {
    FirstCommand_B = true;
    CmdLine_S = _rFirstCommand_S;
  }
  do
  {
    if (!FirstCommand_B)
    {
      if (mShellParam_X.InteractiveMode_B)
      {
        mShellParam_X.psConio->PrintfColor(mShellParam_X.ShellCmdInputColor_E, "%s", "");
        mShellParam_X.psConio->Readline(mShellParam_X.Prompt_S, CmdLine_S);
      }
    }
    FirstCommand_B = false;

    if ((CmdLine_S != "") && (CmdLine_S[0] != '#')) // Comment in script file
    {
      Rts_E = Parser(CmdLine_S.c_str());
    }
    else
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
    if (Rts_E != BOF_ERR_NO_ERROR)
    {
      mShellParam_X.psConio->PrintfColor(mShellParam_X.ShellErrorColor_E, "Error %d -> %s\n", Rts_E, Bof_ErrorCode(Rts_E));
      if (mExecScript_B)
      {
        Bof_CloseFile(IoScript);
        IoScript = BOF_INVALID_HANDLE_VALUE;
        mExecScript_B = false;
        break;
      }
    }
    else
    {
      if (mExecScript_B)
      {
        if (IoScript == BOF_INVALID_HANDLE_VALUE)
        {
          Bof_GetCurrentDirectory(Pwd_S);
          mShellParam_X.psConio->PrintfColor(mShellParam_X.ShellCmdInputColor_E, "Execute '%s' script from '%s'\n", mScriptPath.FullPathName(false).c_str(), Pwd_S.c_str());

          Rts_E = Bof_OpenFile(mScriptPath, true, false, IoScript);
          if (Rts_E != BOF_ERR_NO_ERROR)
          {
            mShellParam_X.psConio->PrintfColor(mShellParam_X.ShellErrorColor_E, "Cannot open script file (%s)\n", Bof_ErrorCode(Rts_E));
            IoScript = BOF_INVALID_HANDLE_VALUE;
            mExecScript_B = false;
          }
        }

        if (IoScript != BOF_INVALID_HANDLE_VALUE)
        {
          if (Bof_ReadLine(IoScript, ScriptLine_S) == BOF_ERR_NO_ERROR)
          {
            ScriptLine_S = Bof_StringTrim(ScriptLine_S);
            mShellParam_X.psConio->PrintfColor(mShellParam_X.ShellScriptColor_E, "SCRIPT: %s\n", ScriptLine_S.c_str());

            CmdLine_S = ScriptLine_S;
          }
          else
          {
            Bof_CloseFile(IoScript);
            IoScript = BOF_INVALID_HANDLE_VALUE;
            mExecScript_B = false;
          }
        }
        mFinish_B = !mExecScript_B;
      }
    }
  } while (!mFinish_B);

  return Rts_E;
}

END_BOF_NAMESPACE()