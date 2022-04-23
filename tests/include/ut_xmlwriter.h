/*
* Copyright (c) 2015-2025, Evs Broadcast Equipment. All rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
* PURPOSE.
*
* This module defines the unit tests of the xml parser library
*
* Name:        ut_xmlwriter.h
* Author:      Bernard HARMEL: b.harmel@evs.com
* Web:			   www.evs.com
* Revision:    1.0
*
* Rem:         Nothing
*
* History:
*
* V 1.00  Dec 26 2013  BHA : Initial release
*/

#pragma once

/*** Include ****************************************************************/

#include <gtest/gtest.h>

/*** Define *****************************************************************/

/*** Enum *******************************************************************/

/*** Struct *****************************************************************/

/*** Class definition *******************************************************/

class XmlWriter_Test : public testing::Test
{
public:

  // Per-test-case set-up. Called before the first test in this test case.
  static void SetUpTestCase();

  // Per-test-case tear-down. Called after the last test in this test case.
  static void TearDownTestCase();

protected:

  // You can define per-test set-up and tear-down logic as usual.
  virtual void SetUp();
  virtual void TearDown();

private:
};
