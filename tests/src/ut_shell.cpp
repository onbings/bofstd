/*
 * Copyright (c) 2015-2025, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit tests of the jsonparser library
 *
 * Name:        ut_jsonparser.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */
#include <bofstd/bofshell.h>
#include <bofstd/bofstring.h>

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

struct MY_SHELL_ARG
{
  int64_t PartitionId_S64;
  bool ShowFile_B;
  bool ResetStatAfterGet_B;

  bool ShowMetaData_B;
  bool ShowDirectory_B;
  bool ShowExtension_B;

  BOF::BofPath LocalPath;
  std::string LogicalPath_S; // Not BofPath as this item can be a "pure file name" so without any dir (in this case BofPath will add current dir to string...)
  std::string JsonMetaData_S;
  BOF::BofPath LocalDirectory;
  std::string OldLogicalPath_S; // Not BofPath as this item can be a "pure file name" so without any dir (in this case BofPath will add current dir to string...)
  std::string NewLogicalPath_S; // Not BofPath as this item can be a "pure file name" so without any dir (in this case BofPath will add current dir to string...)
  std::string FilePattern_S;
  std::string RemoteDirectory_S; // Not BofPath as this item can be a "pure file name" so without any dir (in this case BofPath will add current dir to string...)

  std::string PartitionName_S;
  bool LoopRecording_B;
  uint32_t TotalPartitionSizeInMB_U32;

  bool DeleteTimelessChunk_B;
  bool DeleteProtect_B;
  bool RemoveTrack_B;
  bool RemovePartition_B;

  BOF::BofPath ScriptFile;

  //  BOF::BOF_SOCKET_ADDRESS IpAddress_X;

  MY_SHELL_ARG()
  {
    Reset();
  }

  void Reset()
  {
    PartitionId_S64 = 0;
    ShowFile_B = false;
    ResetStatAfterGet_B = false;

    ShowMetaData_B = false;
    ShowDirectory_B = false;
    ShowExtension_B = false;

    LocalPath = "";
    LogicalPath_S = "";
    JsonMetaData_S = "";
    LocalDirectory = "";
    OldLogicalPath_S = "";
    NewLogicalPath_S = "";
    FilePattern_S = "";
    RemoteDirectory_S = "";

    PartitionName_S = "";
    LoopRecording_B = false;
    TotalPartitionSizeInMB_U32 = 0;

    DeleteTimelessChunk_B = false;
    DeleteProtect_B = false;
    RemoveTrack_B = false;
    RemovePartition_B = false;

    ScriptFile = "";

    //    IpAddress_X.Reset();
  }
};

MY_SHELL_ARG S_MyShellArg_X;

BOFERR ShellGetStorageInfo(void *_pArg, std::string &_rShellResult_S)
{
  MY_SHELL_ARG *_pMyShellArg_X;

  _pMyShellArg_X = (MY_SHELL_ARG *)_pArg;
  if (_pMyShellArg_X)
  {
  }
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellListMedia(void *_pArg, std::string &_rShellResult_S)
{
  MY_SHELL_ARG *_pMyShellArg_X;

  _pMyShellArg_X = (MY_SHELL_ARG *)_pArg;
  if (_pMyShellArg_X)
  {
  }
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellImportMedia(void *_pArg, std::string &_rShellResult_S)
{
  MY_SHELL_ARG *_pMyShellArg_X;

  _pMyShellArg_X = (MY_SHELL_ARG *)_pArg;
  if (_pMyShellArg_X)
  {
  }
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellImportDirectory(void *_pArg, std::string &_rShellResult_S)
{
  MY_SHELL_ARG *_pMyShellArg_X;

  _pMyShellArg_X = (MY_SHELL_ARG *)_pArg;
  if (_pMyShellArg_X)
  {
  }
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellDeleteMedia(void *_pArg, std::string &_rShellResult_S)
{
  MY_SHELL_ARG *_pMyShellArg_X;

  _pMyShellArg_X = (MY_SHELL_ARG *)_pArg;
  if (_pMyShellArg_X)
  {
  }
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellRenameMedia(void *_pArg, std::string &_rShellResult_S)
{
  MY_SHELL_ARG *_pMyShellArg_X;

  _pMyShellArg_X = (MY_SHELL_ARG *)_pArg;
  if (_pMyShellArg_X)
  {
  }
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellExportMedia(void *_pArg, std::string &_rShellResult_S)
{
  MY_SHELL_ARG *_pMyShellArg_X;

  _pMyShellArg_X = (MY_SHELL_ARG *)_pArg;
  if (_pMyShellArg_X)
  {
  }
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellCreatePartition(void *_pArg, std::string &_rShellResult_S)
{
  MY_SHELL_ARG *_pMyShellArg_X;

  _pMyShellArg_X = (MY_SHELL_ARG *)_pArg;
  if (_pMyShellArg_X)
  {
  }
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellDeletePartition(void *_pArg, std::string &_rShellResult_S)
{
  MY_SHELL_ARG *_pMyShellArg_X;

  _pMyShellArg_X = (MY_SHELL_ARG *)_pArg;
  if (_pMyShellArg_X)
  {
  }
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellFormatPartition(void *_pArg, std::string &_rShellResult_S)
{
  MY_SHELL_ARG *_pMyShellArg_X;

  _pMyShellArg_X = (MY_SHELL_ARG *)_pArg;
  if (_pMyShellArg_X)
  {
  }
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellClearAllStorage(void *_pArg, std::string &_rShellResult_S)
{
  MY_SHELL_ARG *_pMyShellArg_X;

  _pMyShellArg_X = (MY_SHELL_ARG *)_pArg;
  if (_pMyShellArg_X)
  {
  }
  return BOF_ERR_NO_ERROR;
}

TEST(Shell_Test, Shell)
{
  BOF_SHELL_CONSOLE_PARAM ShellConsoleParam_X;
  ShellConsoleParam_X.AnsiMode_B = true;
  ShellConsoleParam_X.ClearScreen_B = true;
  ShellConsoleParam_X.WindowTitle_S = "BOF_SHELL";
  ShellConsoleParam_X.ConioParam_X.EditOnMultiLine_B = true;
  ShellConsoleParam_X.ConioParam_X.HistoryMaxLength_U32 = 100;
  ShellConsoleParam_X.ConioParam_X.HistoryPathname = "./history.txt"; // BOF::BofPath();
  ShellConsoleParam_X.ShellBackColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BLUE;
  ShellConsoleParam_X.ShellCmdInputColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_BLUE;
  ShellConsoleParam_X.ShellCmdOutputColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_MAGENTA;
  ShellConsoleParam_X.ShellErrorColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_RED;
  // ShellConsoleParam_X.ShellScriptColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_CYAN;
  BofShellConsole ShellConsole(ShellConsoleParam_X);

  BOF_SHELL_PARAM ShellParam_X;
  //  ShellParam_X.InputStream = std::bind(&BofShellConsole::InputStream, &ShellConsole, std::placeholders::_1, std::placeholders::_2);
  ShellParam_X.InputStream = BOF_BIND_2_ARG_TO_METHOD(&ShellConsole, BofShellConsole::InputStream);
  ShellParam_X.OutputStream = BOF_BIND_2_ARG_TO_METHOD(&ShellConsole, BofShellConsole::OutputStream);
  ShellParam_X.Prompt_S = "BofShell>";
  BofShell Shell(ShellParam_X);

  EXPECT_EQ(Shell.AddCommand("si", BOF_SHELL_CMD("Get storage info.", {"PartitionId_S64", "si::ShowFile_B", "ResetStatAfterGet_B"}, &S_MyShellArg_X, ShellGetStorageInfo)), BOF_ERR_NO_ERROR);
  EXPECT_NE(Shell.AddCommand("si", BOF_SHELL_CMD("Get storage info.", {"PartitionId_S64", "ShowFile_B", "ResetStatAfterGet_B"}, &S_MyShellArg_X, ShellGetStorageInfo)), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Shell.AddCommand("ls", BOF_SHELL_CMD("List information on the media present on the storage.", {"ShowMetaData_B", "ShowDirectory_B", "ShowExtension_B"}, &S_MyShellArg_X, ShellListMedia)), BOF_ERR_NO_ERROR);

  EXPECT_NE(Shell.AddCommandArgument("azert", "PartitionId_S64", BOF::BOFPARAMETER(nullptr, "", "Specify the PartitionId_S64 parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.PartitionId_S64, INT64, 0, 0))), BOF_ERR_NO_ERROR);

  EXPECT_EQ(Shell.AddCommandArgument("si", "PartitionId_S64", BOF::BOFPARAMETER(nullptr, "", "Specify the PartitionId_S64 parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.PartitionId_S64, INT64, 0, 0))), BOF_ERR_NO_ERROR);
  EXPECT_NE(Shell.AddCommandArgument("si", "azerty", BOF::BOFPARAMETER(nullptr, "", "Specify the PartitionId_S64 parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.PartitionId_S64, INT64, 0, 0))), BOF_ERR_NO_ERROR);

  EXPECT_EQ(Shell.AddCommandArgument("si", "si::ShowFile_B", BOF::BOFPARAMETER(nullptr, "", "Specify the ShowFile_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ShowFile_B, BOOL, 0, 0))), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Shell.AddCommandArgument("si", "ResetStatAfterGet_B", BOF::BOFPARAMETER(nullptr, "", "Specify the ResetStat_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ResetStatAfterGet_B, BOOL, 0, 0))), BOF_ERR_NO_ERROR);

  EXPECT_EQ(Shell.AddCommandArgument("ls", "ShowMetaData_B", BOF::BOFPARAMETER(nullptr, "", "Specify the ShowMetaData_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ShowMetaData_B, BOOL, 0, 0))), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Shell.AddCommandArgument("ls", "ShowDirectory_B",
                                     BOF::BOFPARAMETER(nullptr, "", "Specify the ShowDirectory_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ShowDirectory_B, BOOL, 0, 0))),
            BOF_ERR_NO_ERROR);
  EXPECT_EQ(Shell.AddCommandArgument("ls", "ShowExtension_B",
                                     BOF::BOFPARAMETER(nullptr, "", "Specify the ShowExtension_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ShowExtension_B, BOOL, 0, 0))),
            BOF_ERR_NO_ERROR);

  EXPECT_NE(Shell.ExecScript("../data/shell_script_1.dontexist"), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Shell.ExecScript("../data/shell_script_1.bof"), BOF_ERR_NO_ERROR);
  Shell.Execute("si(0, false, true)");
  // Shell.Interpreter("?");
}
