/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * The test class of the BofThread functionalities
 *
 * Name:        ut_threading.cpp
 * Author:      Bernard HARMEL
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  23/06/2014  BHA : Initial release
 */

#pragma once

/*** Include ****************************************************************/
#include <cstdint>
#include <gtest/gtest.h>
#include <bofstd/bofthreadapi.h>

/*** Define *******************************************************************/

/*** Enum *********************************************************************/

/*** Struct *******************************************************************/

/*** Class definition *********************************************************/

class BofThread_Test : public::testing::Test
{
public:

	// Per-test-case set-up. Called before the first test in this test case.
	static void SetUpTestCase( )     { }

	// Per-test-case tear-down. Called after the last test in this test case.
	static void TearDownTestCase( )  { }

  static void * ThreadEntryPoint(void * _pContext);

protected:

	// You can define per-test set-up and tear-down logic as usual.
	virtual void SetUp    ();
	virtual void TearDown ();
};

