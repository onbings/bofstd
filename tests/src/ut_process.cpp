/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the script launcher class
 *
 * Name:        ut_scriptlauncher.cpp
 * Author:      BHA
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  22/01/2015 BHA Initial Release
 */
#include <bofstd/bofprocess.h>
#include <bofstd/bofsystem.h>

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

// To use a test fixture, derive from testing::Test class
class BofProcess_Test : public testing::Test
{
protected:
  /*!
     Summary
     The test initialization method
   */
  virtual void SetUp()
  {
  }

  /*!
     Summary
     The test cleanup method
   */
  virtual void TearDown()
  {
  }
};

TEST_F(BofProcess_Test, Execute_popen)
{
  int ExitCode_i;
  BOFERR Sts_E;
  BOF_PROCESS Pid_X;
  char pResult_c[256];

  memset(pResult_c, 0x00, sizeof(pResult_c));

  BofProcess::S_SetDefaultExecuteMode(BofProcess::EXECUTE_POPEN);

  Sts_E = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "echo $PATH", Pid_X, ExitCode_i);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(ExitCode_i, 0);

  Sts_E = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "echo Babar", Pid_X, ExitCode_i);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(ExitCode_i, 0);
  EXPECT_TRUE(memcmp(pResult_c, "Babar", strlen("Babar")) == 0);

  Sts_E = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "ThisCommandDoesNotExist", Pid_X, ExitCode_i);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(ExitCode_i, 0);
}

TEST_F(BofProcess_Test, Execute_vfork)
{
  int ExitCode_i;
  BOFERR Sts_E;
  BOF_PROCESS Pid_X;
  char pResult_c[256];

  memset(pResult_c, 0x00, sizeof(pResult_c));

  BofProcess::S_SetDefaultExecuteMode(BofProcess::EXECUTE_VFORK);

  Sts_E = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "echo $PATH", Pid_X, ExitCode_i);
#if defined(_WIN32)
  Sts_E = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "C:\\Windows\\system32\\cmd.exe /C echo Babar", Pid_X, ExitCode_i);
#else
  Sts_E = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "echo Babar", Pid_X, ExitCode_i);
#endif
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(ExitCode_i, 0);
  EXPECT_TRUE(memcmp(pResult_c, "Babar", strlen("Babar")) == 0);

  Sts_E = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "ThisCommandDoesNotExist", Pid_X, ExitCode_i);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(ExitCode_i, 0);
}

TEST_F(BofProcess_Test, Execute_posix_spawn)
{
  int ExitCode_i;
  BOFERR Sts_E;
  BOF_PROCESS Pid_X;
  char pResult_c[256];

  memset(pResult_c, 0x00, sizeof(pResult_c));

  BofProcess::S_SetDefaultExecuteMode(BofProcess::EXECUTE_POSIX_SPAWN);

  Sts_E = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "echo $PATH", Pid_X, ExitCode_i);
#if defined(_WIN32)
  Sts_E = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "C:\\Windows\\system32\\cmd.exe /C echo Babar", Pid_X, ExitCode_i);
#else
  Sts_E = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "echo Babar", Pid_X, ExitCode_i);
#endif
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(ExitCode_i, 0);
  EXPECT_TRUE(memcmp(pResult_c, "Babar", strlen("Babar")) == 0);

  Sts_E = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "ThisCommandDoesNotExist", Pid_X, ExitCode_i);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(ExitCode_i, 0);
}

TEST_F(BofProcess_Test, Process)
{
  BOF_PROCESS Pid_X;
  int ExitCode_i;
  BOFERR Sts_E;

  EXPECT_NE(BofProcess::S_KillProcess("ThisAppNameDoesNotExist.exe"), BOF_ERR_NO_ERROR);
  EXPECT_FALSE(BofProcess::S_IsProcessRunning(Pid_X));
  EXPECT_NE(BofProcess::S_KillProcess(Pid_X), BOF_ERR_NO_ERROR);

  Sts_E = BofProcess::S_SpawnProcess("ThisAppNameDoesNotExist.exe", "", 0, Pid_X, ExitCode_i);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(ExitCode_i, 0);
  EXPECT_EQ(Pid_X.Pid, -1);
#if defined(_WIN32)
  EXPECT_EQ(Pid_X.Pi_X.dwProcessId, 0);
  EXPECT_EQ(Pid_X.Pi_X.dwThreadId, 0);
  EXPECT_EQ(Pid_X.Pi_X.hProcess, (HANDLE)0);
  EXPECT_EQ(Pid_X.Pi_X.hThread, (HANDLE)0);
  // Need to specify full path, do not follow the search path
  Sts_E = BofProcess::S_SpawnProcess("C:\\Windows\\notepad.exe", "/A FileDoesNotExist.txt", 0, Pid_X, ExitCode_i);
#else
  Sts_E = BofProcess::S_SpawnProcess("/usr/bin/nano", "FileDoesNotExist.txt", 0, Pid_X, ExitCode_i);
  // printf("Rts %d Exit %d Pid %d\n", Sts_E, ExitCode_i, Pid_X.Pid);
#endif
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(ExitCode_i, 0);
  EXPECT_GT(Pid_X.Pid, 0);
#if defined(_WIN32)
  EXPECT_NE(Pid_X.Pi_X.dwProcessId, 0);
  EXPECT_NE(Pid_X.Pi_X.dwThreadId, 0);
  EXPECT_NE(Pid_X.Pi_X.hProcess, (HANDLE)0);
  EXPECT_NE(Pid_X.Pi_X.hThread, (HANDLE)0);
  EXPECT_TRUE(BofProcess::S_IsProcessRunning(Pid_X));
  EXPECT_EQ(BofProcess::S_KillProcess("notepad.exe"), BOF_ERR_NO_ERROR);
  // Need to specify full path, do not follow the search path
  Sts_E = BofProcess::S_SpawnProcess("C:\\Windows\\notepad.exe", "/A FileDoesNotExist.txt", 0, Pid_X, ExitCode_i);
#else
  EXPECT_TRUE(BofProcess::S_IsProcessRunning(Pid_X));
  // Marche aussi  EXPECT_EQ(BofProcess::S_KillProcess("nano"), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofProcess::S_KillProcess("/usr/bin/nano"), BOF_ERR_NO_ERROR);
  Sts_E = BofProcess::S_SpawnProcess("/usr/bin/nano", "FileDoesNotExist.txt", 0, Pid_X, ExitCode_i);
#endif

  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(ExitCode_i, 0);
  EXPECT_GT(Pid_X.Pid, 0);
  EXPECT_EQ(BofProcess::S_KillProcess(Pid_X), BOF_ERR_NO_ERROR);
}
