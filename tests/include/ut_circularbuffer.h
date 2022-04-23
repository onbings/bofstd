/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines data structures which are used by the
 * ut_circularbuffer unit test
 *
 * Name:        ut_circularbuffer.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */

#pragma once

/*** Include ****************************************************************/
#include <bofstd/bofstd.h>
BEGIN_BOF_NAMESPACE()

/*** Define *****************************************************************/

/*** Enum *******************************************************************/

/*** Struct *****************************************************************/

/*** Class definition *******************************************************/


template<class T>
class BofCircularBufferTemplate_Test : public testing::Test
{
public:

  // Per-test-case set-up. Called before the first test in this test case.
  static void SetUpTestCase( );

  // Per-test-case tear-down. Called after the last test in this test case.
  static void TearDownTestCase( );

protected:

  // You can define per-test set-up and tear-down logic as usual.
  virtual void SetUp( )
  {
    mpBofCircularBuffer_O = nullptr;
  }

  virtual void TearDown( )
  {
    BOF_SAFE_DELETE(mpBofCircularBuffer_O);
  }

  BofCircularBuffer<T> *mpBofCircularBuffer_O;

  // Some expensive resource shared by all tests.
  static BofCircularBuffer<T> *S_mpSharedBofCircularBuffer_O;
};


class BofCircularBuffer_Test : public testing::Test
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

END_BOF_NAMESPACE()
