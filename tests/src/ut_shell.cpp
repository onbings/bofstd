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

static BOF_SHELL_PARAM S_ShellParam_X;

struct MY_SHELL_ARG
{
  int64_t PartitionId_S64;
  bool ShowFile_B;
  bool ResetStatAfterGet_B;

  bool ShowMetaData_B;
  bool ShowDirectory_B;
  bool ShowExtension_B;

  BOF::BofPath LocalPath;
  std::string LogicalPath_S;    //Not BofPath as this item can be a "pure file name" so without any dir (in this case BofPath will add current dir to string...)
  std::string JsonMetaData_S;
  BOF::BofPath LocalDirectory;
  std::string OldLogicalPath_S;  //Not BofPath as this item can be a "pure file name" so without any dir (in this case BofPath will add current dir to string...)
  std::string NewLogicalPath_S;  //Not BofPath as this item can be a "pure file name" so without any dir (in this case BofPath will add current dir to string...)
  std::string FilePattern_S;
  std::string RemoteDirectory_S; //Not BofPath as this item can be a "pure file name" so without any dir (in this case BofPath will add current dir to string...)

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


BOFERR ShellGetStorageInfo()
{
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellListMedia()
{
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellImportMedia()
{
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellImportDirectory()
{
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellDeleteMedia()
{
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellRenameMedia()
{
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellExportMedia()
{
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellCreatePartition()
{
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellDeletePartition()
{
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellFormatPartition()
{
  return BOF_ERR_NO_ERROR;
}
BOFERR ShellClearAllStorage()
{
  return BOF_ERR_NO_ERROR;
}

TEST(Shell_Test, Shell)
{

  S_ShellParam_X.ShellCmdCollection["si"] = BOF_SHELL_CMD("Get storage info.", { "PartitionId_S64", "ShowFile_B", "ResetStatAfterGet_B" },  ShellGetStorageInfo);
  S_ShellParam_X.ShellCmdCollection["ls"] = BOF_SHELL_CMD("List information on the media present on the storage.", { "ShowMetaData_B", "ShowDirectory_B", "ShowExtension_B" },  ShellListMedia);
  S_ShellParam_X.ShellCmdCollection["im"] = BOF_SHELL_CMD("Import a media from a local to a remote storage device.", { "LocalPath", "LogicalPath", "JsonMetaData_S" },  ShellImportMedia);
  S_ShellParam_X.ShellCmdCollection["id"] = BOF_SHELL_CMD("Import all the media with a specific pattern (wildcard) present in a local directory to a remote storage directory.", { "LocalDirectory", "FilePattern_S", "RemoteDirectory_S" },  ShellImportDirectory);
  S_ShellParam_X.ShellCmdCollection["rm"] = BOF_SHELL_CMD("Delete a media from a remote storage device.", { "MediaType_E", "LogicalPath" },  ShellDeleteMedia);
  S_ShellParam_X.ShellCmdCollection["mv"] = BOF_SHELL_CMD("Change the media name on a remote storage device.", { "MediaType_E", "OldLogicalPath", "NewLogicalPath" },  ShellRenameMedia);
  S_ShellParam_X.ShellCmdCollection["ex"] = BOF_SHELL_CMD("Export media from a remote to a local storage device.", { "MediaType_E", "LogicalPath", "LocalPath" },  ShellExportMedia);
  S_ShellParam_X.ShellCmdCollection["cp"] = BOF_SHELL_CMD("Create a partition on a remote storage device.", { "PartitionName_S", "LoopRecording_B", "TotalPartitionSizeInMB_U32" },  ShellCreatePartition);
  S_ShellParam_X.ShellCmdCollection["dp"] = BOF_SHELL_CMD("Delete a partition from a remote storage device.", { "PartitionName_S" },  ShellDeletePartition);
  S_ShellParam_X.ShellCmdCollection["fp"] = BOF_SHELL_CMD("Format a partition present on a remote storage device.", { "PartitionName_S", "DeleteTimelessChunk_B", "DeleteProtect_B", "RemoveTrack_B", "RemovePartition_B" },  ShellFormatPartition);
  S_ShellParam_X.ShellCmdCollection["ca"] = BOF_SHELL_CMD("Clear all data (Track, Protect, TimelessChunk) from all the partitions on a remote storage device.", {},  ShellClearAllStorage);


  S_ShellParam_X.ShellArgCollection["PartitionId_S64"] = BOF::BOFPARAMETER(nullptr, "", "Specify the PartitionId_S64 parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.PartitionId_S64, INT64, 0, 0));
  S_ShellParam_X.ShellArgCollection["ShowFile_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the ShowFile_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ShowFile_B, BOOL, 0, 0));
  S_ShellParam_X.ShellArgCollection["ResetStatAfterGet_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the ResetStat_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ResetStatAfterGet_B, BOOL, 0, 0));
  
  S_ShellParam_X.ShellArgCollection["ShowMetaData_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the ShowMetaData_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ShowMetaData_B, BOOL, 0, 0));
  S_ShellParam_X.ShellArgCollection["ShowDirectory_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the ShowDirectory_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ShowDirectory_B, BOOL, 0, 0));
  S_ShellParam_X.ShellArgCollection["ShowExtension_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the ShowExtension_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ShowExtension_B, BOOL, 0, 0));
  
  S_ShellParam_X.ShellArgCollection["LocalPath"] = BOF::BOFPARAMETER(nullptr, "", "Specify the LocalPath parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.LocalPath, PATH, 1, 512));
  S_ShellParam_X.ShellArgCollection["LogicalPath"] = BOF::BOFPARAMETER(nullptr, "", "Specify the LogicalPath parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.LogicalPath_S, STDSTRING, 0, 512));
  S_ShellParam_X.ShellArgCollection["JsonMetaData_S"] = BOF::BOFPARAMETER(nullptr, "", "Specify the JsonMetaData_S parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.JsonMetaData_S, STDSTRING, 0, 512));
  S_ShellParam_X.ShellArgCollection["LocalDirectory"] = BOF::BOFPARAMETER(nullptr, "", "Specify the LocalDirectory parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.LocalDirectory, PATH, 1, 512));
  S_ShellParam_X.ShellArgCollection["OldLogicalPath"] = BOF::BOFPARAMETER(nullptr, "", "Specify the OldLogicalPath parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.OldLogicalPath_S, STDSTRING, 1, 512));
  S_ShellParam_X.ShellArgCollection["NewLogicalPath"] = BOF::BOFPARAMETER(nullptr, "", "Specify the NewLogicalPath parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.NewLogicalPath_S, STDSTRING, 1, 512));
  S_ShellParam_X.ShellArgCollection["FilePattern_S"] = BOF::BOFPARAMETER(nullptr, "", "Specify the FilePattern_S parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.FilePattern_S, STDSTRING, 0, 512));
  S_ShellParam_X.ShellArgCollection["RemoteDirectory_S"] = BOF::BOFPARAMETER(nullptr, "", "Specify the RemoteDirectory parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.RemoteDirectory_S, STDSTRING, 0, 512));
  
  S_ShellParam_X.ShellArgCollection["PartitionName_S"] = BOF::BOFPARAMETER(nullptr, "", "Specify the PartitionName_S parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.PartitionName_S, STDSTRING, 1, 512));
  S_ShellParam_X.ShellArgCollection["LoopRecording_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the LoopRecording_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.LoopRecording_B, BOOL, 0, 0));
  S_ShellParam_X.ShellArgCollection["TotalPartitionSizeInMB_U32"] = BOF::BOFPARAMETER(nullptr, "", "Specify the TotalPartitionSizeInMB_U32 parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.TotalPartitionSizeInMB_U32, UINT32, 0, 0));
  
  S_ShellParam_X.ShellArgCollection["DeleteTimelessChunk_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the DeleteTimelessChunk_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.DeleteTimelessChunk_B, BOOL, 0, 0));
  S_ShellParam_X.ShellArgCollection["DeleteProtect_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the DeleteProtect_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.DeleteProtect_B, BOOL, 0, 0));
  S_ShellParam_X.ShellArgCollection["RemoveTrack_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the RemoveTrack_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.RemoveTrack_B, BOOL, 0, 0));
  S_ShellParam_X.ShellArgCollection["RemovePartition_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the RemovePartition_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.RemovePartition_B, BOOL, 0, 0));
  
  S_ShellParam_X.ShellArgCollection["ScriptFile"] = BOF::BOFPARAMETER(nullptr, "", "Specify the ScriptFile parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ScriptFile, PATH, 1, 512));

  S_ShellParam_X.ConioParam_X.EditOnMultiLine_B = true;
  S_ShellParam_X.ConioParam_X.HistoryMaxLength_U32 = 100;
  S_ShellParam_X.ConioParam_X.HistoryPathname = "./history.txt"; // BOF::BofPath();

  S_ShellParam_X.InteractiveMode_B = true;
  S_ShellParam_X.Prompt_S = "BofShell>";
  S_ShellParam_X.WindowTitle_S = "BOF_SHELL";
  S_ShellParam_X.ShellBackColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BLACK;
  S_ShellParam_X.ShellCmdInputColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_BLUE;
  S_ShellParam_X.ShellCmdOutputColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_GREEN;
  S_ShellParam_X.ShellErrorColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_RED;
  S_ShellParam_X.ShellScriptColor_E = CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_CYAN;

  std::unique_ptr<BofShell> puShell = std::make_unique<BofShell>(S_ShellParam_X);
  puShell->AddCommand("q", BOF_SHELL_CMD("Leave mfs console.", {}, BOF_BIND_0_ARG_TO_METHOD(puShell.get(), BofShell::ShellQuit)));
  puShell->AddCommand("?", BOF_SHELL_CMD("Display the list of commands.", {}, BOF_BIND_0_ARG_TO_METHOD(puShell.get(), BofShell::ShellHelp)));
  //puShell->ExecScript("run.script");
//  puShell->Interpreter();

}

