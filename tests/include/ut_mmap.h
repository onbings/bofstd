/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines data structures which are used by the
 * ut_bofmp unit test
 *
 * Name:        ut_bofmp_crypto.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */

#ifndef UT_BOFMP_MMAPH
#define UT_BOFMP_MMAPH

/*** Include ****************************************************************/
#include <cstdint>
#include <gtest/gtest.h>
#include <bofstd/bofthreadapi.h>

/*** Define *****************************************************************/

/*** Enum *******************************************************************/

/*** Struct *******************************************************************/

/*** Class definition *******************************************************/
#if defined( __linux__ ) || defined(__APPLE__)
class Bof_VirtualMemoryMAP_Test : public::testing::Test
{
public:

	// Per-test-case set-up. Called before the first test in this test case.
	static void SetUpTestCase( )     { }

	// Per-test-case tear-down. Called after the last test in this test case.
	static void TearDownTestCase( )  { }
protected:

	// You can define per-test set-up and tear-down logic as usual.
	virtual void SetUp( );
	virtual void TearDown( );
};
#endif
#endif
