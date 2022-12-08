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

BOFERR ShellQuit()
{
  //mFinish_B = true;
  return BOF_ERR_NO_ERROR;
}

BOFERR ShellHelp()
{
  /*
  std::string Help_S;
  uint32_t i_U32;

  auto pIt = mShellCmdCollection.begin();
  while (pIt != mShellCmdCollection.end())
  {
    Help_S = BOF::Bof_Sprintf("%-8s: %s [", pIt->first.c_str(), pIt->second.Help_S.c_str());
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
    PrintfColor(BOF::CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_YELLOW, "%s\n", Help_S.c_str());
    pIt++;
  }
  */
  return BOF_ERR_NO_ERROR;
}
bool DoYouContinue()
{
  bool Rts_B = true;
/*
  uint32_t Ch_U32;

  if (!ForceMode_B)
  {
    PrintfColor(BOF::CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_RED, "%s\n", "Are you sure that you want to do that (Y/N)");
    Ch_U32 = mpuConio->GetCh(false);
    if ((Ch_U32 & BOF::CONIO_KEY_MASK) == 'Y')
    {
      Rts_B = false;
    }
  }
  */
  return Rts_B;
}
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
BOFERR ShellExec()
{
  BOFERR Rts_E;
  Rts_E = DoYouContinue() ? BOF_ERR_NO_ERROR : BOF_ERR_CANCEL;
  /*
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    mExecScript_B = true;
  }
  */
  return Rts_E;
}
TEST(Shell_Test, Shell)
{
  BOF_SHELL_PARAM ShellParam_X;

//  ShellParam_X.ShellCmdCollection["q"] = BOF_SHELL_CMD("Leave mfs console.", {},  ShellQuit));
  ShellParam_X.ShellCmdCollection["q"] = BOF_SHELL_CMD("Leave mfs console.", {}, ShellQuit);
  ShellParam_X.ShellCmdCollection["?"] = BOF_SHELL_CMD("Display the list of commands.", {},  ShellHelp);
  ShellParam_X.ShellCmdCollection["si"] = BOF_SHELL_CMD("Get storage info.", { "PartitionId_S64", "ShowFile_B", "ResetStatAfterGet_B" },  ShellGetStorageInfo);
  ShellParam_X.ShellCmdCollection["ls"] = BOF_SHELL_CMD("List information on the media present on the storage.", { "ShowMetaData_B", "ShowDirectory_B", "ShowExtension_B" },  ShellListMedia);
  ShellParam_X.ShellCmdCollection["im"] = BOF_SHELL_CMD("Import a media from a local to a remote storage device.", { "LocalPath", "LogicalPath", "JsonMetaData_S" },  ShellImportMedia);
  ShellParam_X.ShellCmdCollection["id"] = BOF_SHELL_CMD("Import all the media with a specific pattern (wildcard) present in a local directory to a remote storage directory.", { "LocalDirectory", "FilePattern_S", "RemoteDirectory_S" },  ShellImportDirectory);
  ShellParam_X.ShellCmdCollection["rm"] = BOF_SHELL_CMD("Delete a media from a remote storage device.", { "MediaType_E", "LogicalPath" },  ShellDeleteMedia);
  ShellParam_X.ShellCmdCollection["mv"] = BOF_SHELL_CMD("Change the media name on a remote storage device.", { "MediaType_E", "OldLogicalPath", "NewLogicalPath" },  ShellRenameMedia);
  ShellParam_X.ShellCmdCollection["ex"] = BOF_SHELL_CMD("Export media from a remote to a local storage device.", { "MediaType_E", "LogicalPath", "LocalPath" },  ShellExportMedia);
  ShellParam_X.ShellCmdCollection["cp"] = BOF_SHELL_CMD("Create a partition on a remote storage device.", { "PartitionName_S", "LoopRecording_B", "TotalPartitionSizeInMB_U32" },  ShellCreatePartition);
  ShellParam_X.ShellCmdCollection["dp"] = BOF_SHELL_CMD("Delete a partition from a remote storage device.", { "PartitionName_S" },  ShellDeletePartition);
  ShellParam_X.ShellCmdCollection["fp"] = BOF_SHELL_CMD("Format a partition present on a remote storage device.", { "PartitionName_S", "DeleteTimelessChunk_B", "DeleteProtect_B", "RemoveTrack_B", "RemovePartition_B" },  ShellFormatPartition);
  ShellParam_X.ShellCmdCollection["ca"] = BOF_SHELL_CMD("Clear all data (Track, Protect, TimelessChunk) from all the partitions on a remote storage device.", {},  ShellClearAllStorage);
  ShellParam_X.ShellCmdCollection["run"] = BOF_SHELL_CMD("Execute a script file con,taining Mfs command.", { "ScriptFile" },  ShellExec);


  ShellParam_X.ShellArgCollection["PartitionId_S64"] = BOF::BOFPARAMETER(nullptr, "", "Specify the PartitionId_S64 parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.PartitionId_S64, INT64, 0, 0));
  ShellParam_X.ShellArgCollection["ShowFile_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the ShowFile_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ShowFile_B, BOOL, 0, 0));
  ShellParam_X.ShellArgCollection["ResetStatAfterGet_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the ResetStat_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ResetStatAfterGet_B, BOOL, 0, 0));
  
  ShellParam_X.ShellArgCollection["ShowMetaData_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the ShowMetaData_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ShowMetaData_B, BOOL, 0, 0));
  ShellParam_X.ShellArgCollection["ShowDirectory_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the ShowDirectory_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ShowDirectory_B, BOOL, 0, 0));
  ShellParam_X.ShellArgCollection["ShowExtension_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the ShowExtension_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ShowExtension_B, BOOL, 0, 0));
  
  ShellParam_X.ShellArgCollection["LocalPath"] = BOF::BOFPARAMETER(nullptr, "", "Specify the LocalPath parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.LocalPath, PATH, 1, 512));
  ShellParam_X.ShellArgCollection["LogicalPath"] = BOF::BOFPARAMETER(nullptr, "", "Specify the LogicalPath parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.LogicalPath_S, STDSTRING, 0, 512));
  ShellParam_X.ShellArgCollection["JsonMetaData_S"] = BOF::BOFPARAMETER(nullptr, "", "Specify the JsonMetaData_S parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.JsonMetaData_S, STDSTRING, 0, 512));
  ShellParam_X.ShellArgCollection["LocalDirectory"] = BOF::BOFPARAMETER(nullptr, "", "Specify the LocalDirectory parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.LocalDirectory, PATH, 1, 512));
  ShellParam_X.ShellArgCollection["OldLogicalPath"] = BOF::BOFPARAMETER(nullptr, "", "Specify the OldLogicalPath parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.OldLogicalPath_S, STDSTRING, 1, 512));
  ShellParam_X.ShellArgCollection["NewLogicalPath"] = BOF::BOFPARAMETER(nullptr, "", "Specify the NewLogicalPath parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.NewLogicalPath_S, STDSTRING, 1, 512));
  ShellParam_X.ShellArgCollection["FilePattern_S"] = BOF::BOFPARAMETER(nullptr, "", "Specify the FilePattern_S parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.FilePattern_S, STDSTRING, 0, 512));
  ShellParam_X.ShellArgCollection["RemoteDirectory_S"] = BOF::BOFPARAMETER(nullptr, "", "Specify the RemoteDirectory parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.RemoteDirectory_S, STDSTRING, 0, 512));
  
  ShellParam_X.ShellArgCollection["PartitionName_S"] = BOF::BOFPARAMETER(nullptr, "", "Specify the PartitionName_S parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.PartitionName_S, STDSTRING, 1, 512));
  ShellParam_X.ShellArgCollection["LoopRecording_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the LoopRecording_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.LoopRecording_B, BOOL, 0, 0));
  ShellParam_X.ShellArgCollection["TotalPartitionSizeInMB_U32"] = BOF::BOFPARAMETER(nullptr, "", "Specify the TotalPartitionSizeInMB_U32 parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.TotalPartitionSizeInMB_U32, UINT32, 0, 0));
  
  ShellParam_X.ShellArgCollection["DeleteTimelessChunk_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the DeleteTimelessChunk_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.DeleteTimelessChunk_B, BOOL, 0, 0));
  ShellParam_X.ShellArgCollection["DeleteProtect_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the DeleteProtect_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.DeleteProtect_B, BOOL, 0, 0));
  ShellParam_X.ShellArgCollection["RemoveTrack_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the RemoveTrack_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.RemoveTrack_B, BOOL, 0, 0));
  ShellParam_X.ShellArgCollection["RemovePartition_B"] = BOF::BOFPARAMETER(nullptr, "", "Specify the RemovePartition_B parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.RemovePartition_B, BOOL, 0, 0));
  
  ShellParam_X.ShellArgCollection["ScriptFile"] = BOF::BOFPARAMETER(nullptr, "", "Specify the ScriptFile parameter", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MyShellArg_X.ScriptFile, PATH, 1, 512));

  ShellParam_X.ConioParam_X.EditOnMultiLine_B = true;
  ShellParam_X.ConioParam_X.HistoryMaxLength_U32 = 100;
  ShellParam_X.ConioParam_X.HistoryPathname = "./history.txt"; // BOF::BofPath();

  ShellParam_X.InteractiveMode_B = true;
  ShellParam_X.Prompt_S = "BofShell>";
  ShellParam_X.WindowTitle_S = "BOF_SHELL";
  std::unique_ptr<BofShell> puShell = std::make_unique<BofShell>(ShellParam_X);

  puShell->Interpreter("");

}

