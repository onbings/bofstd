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
#include <bofstd/bofscriptlauncher.h>
#include <bofstd/bofstd.h>

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

// To use a test fixture, derive from testing::Test class
class BofScriptLauncher_Test : public testing::Test
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

#if defined(_WIN32)
#else

TEST_F(BofScriptLauncher_Test, Execute_popen)
{

  int Status_i;
  char pResult_c[256];

  memset(pResult_c, 0x00, sizeof(pResult_c));

  BofScriptLauncher::SetDefaultExecuteMode(BofScriptLauncher::EXECUTE_POPEN);

  Status_i = BofScriptLauncher::Execute(pResult_c, sizeof(pResult_c), "echo $PATH");

  //printf("PATH is %s\r\n", pResult_c);

  Status_i = BofScriptLauncher::Execute(pResult_c, sizeof(pResult_c), "echo Babar");

  EXPECT_TRUE(Status_i == 0);
  EXPECT_TRUE(memcmp(pResult_c, "Babar", strlen("Babar")) == 0);

  Status_i = BofScriptLauncher::Execute(pResult_c, sizeof(pResult_c), "ThisCommandDoesNotExist");

  EXPECT_TRUE(Status_i == 127);
}

TEST_F(BofScriptLauncher_Test, Execute_vfork)
{

  int Status_i;
  char pResult_c[256];

  memset(pResult_c, 0x00, sizeof(pResult_c));

  BofScriptLauncher::SetDefaultExecuteMode(BofScriptLauncher::EXECUTE_VFORK);

  Status_i = BofScriptLauncher::Execute(pResult_c, sizeof(pResult_c), "echo $PATH");

  //printf("PATH is %s\r\n", pResult_c);

  Status_i = BofScriptLauncher::Execute(pResult_c, sizeof(pResult_c), "echo Babar");

  EXPECT_TRUE(Status_i == 0);
  EXPECT_TRUE(memcmp(pResult_c, "Babar", strlen("Babar")) == 0);

  Status_i = BofScriptLauncher::Execute(pResult_c, sizeof(pResult_c), "ThisCommandDoesNotExist");

  EXPECT_TRUE(Status_i == 127);
}

TEST_F(BofScriptLauncher_Test, Execute_posix_spawn)
{

  int Status_i;
  char pResult_c[256];

  memset(pResult_c, 0x00, sizeof(pResult_c));

  BofScriptLauncher::SetDefaultExecuteMode(BofScriptLauncher::EXECUTE_POSIX_SPAWN);

  Status_i = BofScriptLauncher::Execute(pResult_c, sizeof(pResult_c), "echo $PATH");

  //printf("PATH is %s\r\n", pResult_c);

  Status_i = BofScriptLauncher::Execute(pResult_c, sizeof(pResult_c), "echo Babar");

  EXPECT_TRUE(Status_i == 0);
  EXPECT_TRUE(memcmp(pResult_c, "Babar", strlen("Babar")) == 0);

  Status_i = BofScriptLauncher::Execute(pResult_c, sizeof(pResult_c), "ThisCommandDoesNotExist");

  EXPECT_TRUE(Status_i == 127);
}

#endif