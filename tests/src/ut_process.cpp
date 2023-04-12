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
#include <bofstd/bofstd.h>

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
  int Status_i;
  char pResult_c[256];

  memset(pResult_c, 0x00, sizeof(pResult_c));

  BofProcess::S_SetDefaultExecuteMode(BofProcess::EXECUTE_POPEN);

  Status_i = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "echo $PATH");

  Status_i = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "echo Babar");

  EXPECT_TRUE(Status_i == 0);
  EXPECT_TRUE(memcmp(pResult_c, "Babar", strlen("Babar")) == 0);

  Status_i = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "ThisCommandDoesNotExist");
  EXPECT_TRUE(Status_i != 0);//Linux 127 Win32 1;
}

TEST_F(BofProcess_Test, Execute_vfork)
{

  int Status_i;
  char pResult_c[256];

  memset(pResult_c, 0x00, sizeof(pResult_c));

  BofProcess::S_SetDefaultExecuteMode(BofProcess::EXECUTE_VFORK);

  Status_i = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "echo $PATH");
#if defined(_WIN32)
  Status_i = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "C:\\Windows\\system32\\cmd.exe /C echo Babar");
#else
  Status_i = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "echo Babar");
#endif
  EXPECT_TRUE(Status_i == 0);
  EXPECT_TRUE(memcmp(pResult_c, "Babar", strlen("Babar")) == 0);

  Status_i = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "ThisCommandDoesNotExist");

  EXPECT_TRUE(Status_i != 0);    //127 for linux and 1 for windows
}

TEST_F(BofProcess_Test, Execute_posix_spawn)
{

  int Status_i;
  char pResult_c[256];

  memset(pResult_c, 0x00, sizeof(pResult_c));

  BofProcess::S_SetDefaultExecuteMode(BofProcess::EXECUTE_POSIX_SPAWN);

  Status_i = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "echo $PATH");
#if defined(_WIN32)
  Status_i = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "C:\\Windows\\system32\\cmd.exe /C echo Babar");
#else
  Status_i = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "echo Babar");
#endif
  EXPECT_TRUE(Status_i == 0);
  EXPECT_TRUE(memcmp(pResult_c, "Babar", strlen("Babar")) == 0);

  Status_i = BofProcess::S_Execute(pResult_c, sizeof(pResult_c), "ThisCommandDoesNotExist");

  EXPECT_TRUE(Status_i != 0);    //127 for linux and 1 for windows
}

TEST_F(BofProcess_Test, Process)
{
  BOF_PROCESS Pid_X;

  EXPECT_FALSE(BofProcess::S_KillAllPidByName("ThisAppNameDoesNotExist.exe"));
  EXPECT_FALSE(BofProcess::S_IsProcessRunning(Pid_X));
  EXPECT_FALSE(BofProcess::S_KillProcess(Pid_X));

  Pid_X = BofProcess::S_SpawnProcess("ThisAppNameDoesNotExist.exe", "", 0);
  EXPECT_EQ(Pid_X.Pid, -1);
  EXPECT_EQ(Pid_X.Pi_X.dwProcessId, 0);
  EXPECT_EQ(Pid_X.Pi_X.dwThreadId, 0);
  EXPECT_EQ(Pid_X.Pi_X.hProcess, (HANDLE)0);
  EXPECT_EQ(Pid_X.Pi_X.hThread, (HANDLE)0);
#if defined(_WIN32)
  //Need to specify full path, do not follow the search path
  Pid_X = BofProcess::S_SpawnProcess("C:\\Windows\\notepad.exe", "/A FileDoesNotExist.txt", 0);
#else
  Pid_X = BofProcess::S_SpawnProcess("nano", "FileDoesNotExist.txt", 0);
#endif
  EXPECT_EQ(Pid_X.Pid, 0);
  EXPECT_NE(Pid_X.Pi_X.dwProcessId, 0);
  EXPECT_NE(Pid_X.Pi_X.dwThreadId, 0);
  EXPECT_NE(Pid_X.Pi_X.hProcess, (HANDLE)0);
  EXPECT_NE(Pid_X.Pi_X.hThread, (HANDLE)0);
  EXPECT_TRUE(BofProcess::S_IsProcessRunning(Pid_X));
  EXPECT_TRUE(BofProcess::S_KillAllPidByName("notepad.exe"));
#if defined(_WIN32)
  //Need to specify full path, do not follow the search path
  Pid_X = BofProcess::S_SpawnProcess("C:\\Windows\\notepad.exe", "/A FileDoesNotExist.txt", 0);
#else
  Pid_X = BofProcess::S_SpawnProcess("nano", "FileDoesNotExist.txt", 0);
#endif
  EXPECT_EQ(Pid_X.Pid, 0);
  EXPECT_TRUE(BofProcess::S_KillProcess(Pid_X));
}

