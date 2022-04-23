/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the BofCmdLineParser class
 *
 * Name:        ut_cmdlineparser.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */

/*** Include files ***********************************************************/

#include "gtestrunner.h"

/*** Defines *****************************************************************
 *
 * /*** Global variables ********************************************************/

/*** Class *************************************************************************************************************************/

// To use a test fixture, derive from testing::Test class
class CmdLineParser_Test : public testing::Test
{
public:

	// Per-test-case set-up. Called before the first test in this test case.
	static void SetUpTestCase( );

	// Per-test-case tear-down. Called after the last test in this test case.
	static void TearDownTestCase( );

protected:

	// You can define per-test set-up and tear-down logic as usual.
	virtual void SetUp( );
	virtual void TearDown( );
};
