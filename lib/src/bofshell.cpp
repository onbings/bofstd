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

#include <bofstd/bofshell.h>
#include <bofstd/bofstring.h>
#include <bofstd/boffs.h>

#include <regex>

BEGIN_BOF_NAMESPACE()

BofShell::BofShell(const BOF_SHELL_PARAM &_rShellParam_X) : mShellParam_X(_rShellParam_X)
{

}
BofShell::~BofShell()
{

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
      //if (i_U32 >= It->second.ArgNameCollection.size())
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

BOFERR BofShell::Interpreter(const BofPath &_rScriptFile)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  std::string Pwd_S, ScriptLine_S, CmdLine_S;
  intptr_t IoScript = BOF_FS_INVALID_HANDLE;
  uint32_t i_U32;
  
  mFinish_B = true;
  if (_rScriptFile.FileNameWithExtension() != "")
  {
    mExecScript_B = true;
  }
  else
  {
    if (mShellParam_X.InteractiveMode_B)
    {
      mFinish_B = false;
      mpuConio = std::make_unique<BofConio>(mShellParam_X.ConioParam_X);

      mpuConio->SetTextWindowTitle(mShellParam_X.WindowTitle_S);
      mpuConio->SetForegroundTextColor(CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_WHITE);
      mpuConio->SetBackgroundTextColor(CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BLACK);
      mpuConio->Clear(CONIO_CLEAR::CONIO_CLEAR_ALL);
      mpuConio->SetTextCursorPosition(1, 1);
    }
  }

  do
  {
    if (mShellParam_X.InteractiveMode_B)
    {
      mpuConio->PrintfColor(CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_BLUE, "%s", "");
      mpuConio->Readline(mShellParam_X.Prompt_S, CmdLine_S);
    }
    if (!mExecScript_B)
    {
      mpuConio->PrintfColor(CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_BLUE, "%s%s\n", mShellParam_X.Prompt_S.c_str(), CmdLine_S.c_str());
    }
    if (CmdLine_S[0] != '#') //Comment in script file
    {
      Rts_E = Parser(CmdLine_S.c_str());
    }
    else
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
    if (Rts_E != BOF_ERR_NO_ERROR)
    {
      mpuConio->PrintfColor(CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_RED, "Error %d -> %s\n", Rts_E, Bof_ErrorCode(Rts_E));
      if (mExecScript_B)
      {
        Bof_CloseFile(IoScript);
        IoScript = BOF_FS_INVALID_HANDLE;
        mExecScript_B = false;
        break;
      }
    }
    else
    {
      if (mExecScript_B)
      {
        if (IoScript == BOF_FS_INVALID_HANDLE)
        {
          Bof_GetCurrentDirectory(Pwd_S);
          mpuConio->PrintfColor(CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_BLUE, "Execute '%s' script from '%s'\n", _rScriptFile.FullPathName(false).c_str(), Pwd_S.c_str());

          Rts_E = Bof_OpenFile(_rScriptFile, true, IoScript);
          if (Rts_E != BOF_ERR_NO_ERROR)
          {
            mpuConio->PrintfColor(CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_RED, "Cannot open script file (%s)\n", Bof_ErrorCode(Rts_E));
            IoScript = BOF_FS_INVALID_HANDLE;
            mExecScript_B = false;
          }
        }

        if (IoScript != BOF_FS_INVALID_HANDLE)
        {
          if (Bof_ReadLine(IoScript, ScriptLine_S) == BOF_ERR_NO_ERROR)
          {
            ScriptLine_S = Bof_StringTrim(ScriptLine_S);
            mpuConio->PrintfColor(CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_CYAN, "SCRIPT: %s\n", ScriptLine_S.c_str());

            CmdLine_S = ScriptLine_S;
          }
          else
          {
            Bof_CloseFile(IoScript);
            IoScript = BOF_FS_INVALID_HANDLE;
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