/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines data structures which are used by the
 * ut_ramdb unit test
 *
 * Name:        ut_ramdb.cpp
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
#include <cstdint>
#include <bofstd/bofstd.h>
#include <bofstd/boframdb.h>
#include <gtest/gtest.h>
BEGIN_BOF_NAMESPACE()
/*** Define *****************************************************************/

/*** Enum *******************************************************************/

/*** Struct *******************************************************************/

/*** Const *******************************************************************/

/*** Class definition *******************************************************/
class DbRow
{
public:
	enum DBROWINDEX
	{
		DB_INDEX_KEY_U32 = 0,
		DB_INDEX_KEY_TEXT16,
		DB_INDEX_MAX
	};

	struct DB_ROW_DATA
	{
		uint32_t    Key_U32;               // Index
		char        pKeyText16_c[16];      // Index
		char        pDataText80_c[80];
		float       Val_f;
		int         Val_i;
		DB_ROW_DATA()
		{
			Reset();
		}
		void        Reset()
		{
			Key_U32          = 0;
			pKeyText16_c[0]  = 0;
			pDataText80_c[0] = 0;
			Val_f            = 0;
			Val_i            = 0;
		}
	};
private:
	DB_ROW_DATA mDbRowData_X;
public:

	DbRow();
	~DbRow();
	uint32_t    GetKey();
	char        *GetText80();
	char        *GetText16();
	float       GetFloat();
	int         GetInt();

	uint32_t    SetKeyU32(uint32_t _Key_U32);
	uint32_t    SetText80(const char *_pText80_c);
	uint32_t    SetKeyText16(const char *_pText16_c);
	uint32_t    SetFloat(float _Val_f);
	uint32_t    SetInt(int _Val_i);

	/*** GetKey ******************************************************/

	/*!
	 * Description
	 * This method is used to get the characteristics of a GbeClip index variable.
	 *
	 * Parameters
	 * _Index_U32:	Specifiy the object variable member used as index
	 * _pType_E:	\Returns the index variable type
	 * _MaxChar_U32: Specifies the maximum number of character in _pVal_c
	 * _pVal_c:	\Returns the index variable value as a string
	 *
	 * Returns
	 * void *:  Return a pointer to the index variable storage space
	 *
	 * Remarks
	 * None
	 */
	void        *GetKey(uint32_t _Index_U32, BOFTYPE *_pType_E, uint32_t _MaxChar_U32, char *_pVal_c);

	/*** Compare ******************************************************/

	/*!
	 * Description
	 * This method is used to sort two instances of a GbeClip object.
	 *
	 * Parameters
	 * _Index_U32:	Specifiy the object variable member to use to compare the
	 * two object instance (index)
	 * _pDbRow_O: Specify the object to compare to
	 *
	 * Returns
	 * BOFCMP:  The result of the compare operation
	 *
	 * Remarks
	 * None
	 */
	BOFCMP      Compare(uint32_t _Index_U32, DbRow *_pDbRow_O);
};

class BofRamDb_Test: public::testing::Test
{
private:
public:
	BofRamDb_Test(): mpBofRamDb_O(nullptr)
	{
	}

	// Per-test-case set-up. Called before the first test in this test case.
	static void SetUpTestCase() {}

	// Per-test-case tear-down. Called after the last test in this test case.
	static void TearDownTestCase() {}
protected:

	// You can define per-test set-up and tear-down logic as usual.
	virtual void      SetUp();
	virtual void      TearDown();

	BofRamDb< DbRow > *mpBofRamDb_O;
};
END_BOF_NAMESPACE()