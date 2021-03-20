/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the ramdb class
 *
 * Name:        ut_ramdb.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */

/*** Include files ***********************************************************/
#include "../include/ut_ramdb.h"
#include <gtest/gtest.h>
#include <bofstd/boframdb.h>
#include <bofstd/bofsystem.h>
#include <string.h>

/*** Global variables ********************************************************/

USE_BOF_NAMESPACE()

#define BOFRAMDBNBMAXELEM    100

DbRow::DbRow()
{
	memset(&mDbRowData_X, 0, sizeof(mDbRowData_X) );
}


DbRow::~DbRow()
{}


uint32_t DbRow::GetKey()
{
	return mDbRowData_X.Key_U32;
}


char *DbRow::GetText80()
{
	return mDbRowData_X.pDataText80_c;
}


char *DbRow::GetText16()
{
	return mDbRowData_X.pKeyText16_c;
}


float DbRow::GetFloat()
{
	return mDbRowData_X.Val_f;
}


int DbRow::GetInt()
{
	return mDbRowData_X.Val_i;
}


uint32_t DbRow::SetKeyU32(uint32_t _Key_U32)
{
	uint32_t Rts_U32 = 0;

	mDbRowData_X.Key_U32 = _Key_U32;
	return Rts_U32;
}


uint32_t DbRow::SetText80(const char *_pText80_c)
{
	uint32_t Rts_U32 = (uint32_t)-1;

	if ( (_pText80_c) && (strlen(_pText80_c) < sizeof(mDbRowData_X.pDataText80_c) ) )
	{
		strcpy(mDbRowData_X.pDataText80_c, _pText80_c);
		Rts_U32 = 0;
	}
	return Rts_U32;
}


uint32_t DbRow::SetKeyText16(const char *_pText16_c)
{
	uint32_t Rts_U32 = (uint32_t)-1;

	if ( (_pText16_c) && (strlen(_pText16_c) < sizeof(mDbRowData_X.pKeyText16_c) ) )
	{
		strcpy(mDbRowData_X.pKeyText16_c, _pText16_c);
		Rts_U32 = 0;
	}
	return Rts_U32;
}


uint32_t DbRow::SetFloat(float _Val_f)
{
	uint32_t Rts_U32 = 0;

	mDbRowData_X.Val_f = _Val_f;
	return Rts_U32;
}


uint32_t DbRow::SetInt(int _Val_i)
{
	uint32_t Rts_U32 = 0;

	mDbRowData_X.Val_i = _Val_i;
	return Rts_U32;
}


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
void *DbRow::GetKey(uint32_t _Index_U32, BOFTYPE *_pType_E, uint32_t _MaxChar_U32, char *_pVal_c)
{
	void     *pRts = nullptr;
	uint32_t NbChar_U32;

	// DBGPRINTF(DBG_FCT_ENTRY,"GetKey(0x%X,0x%X,0x%X,0x%X)\r\n",_Index_U32,_pType_E,_MaxChar_U32,_pVal_c);
	if (_pVal_c)
	{
		*_pVal_c = 0;

		switch (_Index_U32)
		{
			case DB_INDEX_KEY_U32:
			{
				pRts      = &mDbRowData_X.Key_U32;
				snprintf(_pVal_c, _MaxChar_U32, "0x%08X", mDbRowData_X.Key_U32);
				*_pType_E = BOF_TYPE_U32;
			}
				break;

			case DB_INDEX_KEY_TEXT16:
			{
				pRts       = &mDbRowData_X.pKeyText16_c;
				NbChar_U32 = (_MaxChar_U32 < sizeof(mDbRowData_X.pKeyText16_c) ) ? _MaxChar_U32 : sizeof(mDbRowData_X.pKeyText16_c);
				memcpy(_pVal_c, mDbRowData_X.pKeyText16_c, NbChar_U32);

				if (NbChar_U32 < _MaxChar_U32)
				{
					_pVal_c[NbChar_U32] = 0;
				}
				*_pType_E  = BOF_TYPE_CHAR;
			}
				break;

			default:
			{
				pRts       = nullptr;
				_pVal_c[0] = 0;
				*_pType_E  = BOF_TYPE_NOTHING;
			}
				break;
		}
	}

	// DBGPRINTF(DBG_FCT_EXIT,"GetKey returns 0x%X\r\n",pRts);
	return pRts;
}


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
BOFCMP DbRow::Compare(uint32_t _Index_U32, DbRow *_pDbRow_O)
{
	BOFCMP Rts_E;
	int    Sts_i;

	// DBGPRINTF(DBG_FCT_ENTRY,"Compare(0x%X,0x%X)\r\n",_Index_U32,_pDbRow_O);
	switch (_Index_U32)
	{
		case DB_INDEX_KEY_U32:
		{
			if (GetKey() == _pDbRow_O->GetKey() )
			{
				Rts_E = BOF_CMP_EQUAL;
			}
			else
			{
				Rts_E = (_pDbRow_O->GetKey() < GetKey() ) ? BOF_CMP_LESS : BOF_CMP_GREATER;
			}
		}
			break;


		case DB_INDEX_KEY_TEXT16:
		{
			Sts_i = memcmp(GetText16(), _pDbRow_O->GetText16(), sizeof(mDbRowData_X.pKeyText16_c) );

			if (! Sts_i)
			{
				Rts_E = BOF_CMP_EQUAL;
			}
			else
			{
				Rts_E = (Sts_i > 0) ? BOF_CMP_LESS : BOF_CMP_GREATER;
			}
		}
			break;


		default:
		{
			Rts_E = BOF_CMP_EQUAL;
		}
			break;
	}

	// DBGPRINTF(DBG_FCT_EXIT,"Compare returns 0x%X\r\n",Rts_E);
	return Rts_E;
}


void BofRamDb_Test::SetUp()
{
	uint32_t ErrorCode_U32;

	mpBofRamDb_O = new BofRamDb< DbRow > (BOFRAMDBNBMAXELEM, DbRow::DB_INDEX_MAX, &ErrorCode_U32);
	EXPECT_TRUE(mpBofRamDb_O != 0);
	EXPECT_EQ(ErrorCode_U32, 0);
}


void BofRamDb_Test::TearDown()
{
	BOF_SAFE_DELETE(mpBofRamDb_O);
	EXPECT_TRUE(mpBofRamDb_O == 0);
}


TEST_F(BofRamDb_Test, Init)
{
	uint32_t        NbRecord_U32, NbCursor_U32, Nb_U32, Err_U32;
	bool            Sts_B;
	int32_t         Val_S32;
	char            pBuffer_c[0x10000];
	BOF_RAM_DB_STAT *pStat_X;

	Sts_B        = mpBofRamDb_O->IsDbEmpty();
	EXPECT_TRUE(Sts_B == true);
	Sts_B        = mpBofRamDb_O->IsDbFull();
	EXPECT_TRUE(Sts_B == false);

// void          ClearDbAndReleaseCursor();

	NbCursor_U32 = mpBofRamDb_O->GetNbFreeCursor();
	EXPECT_EQ(NbCursor_U32, BOFRAMDB_CURSOR_MAX);

	NbRecord_U32 = mpBofRamDb_O->GetNbRecord();
	EXPECT_EQ(NbRecord_U32, 0);


	/*
	 * uint32_t           GetFirstElement(void * _Cursor_h, KeyType *_pElement);
	 * uint32_t           GetLastElement(void * _Cursor_h, KeyType *_pElement);
	 * uint32_t           GetCurrentElement(void * _Cursor_h, KeyType *_pElement);
	 * uint32_t           GetNextElement(void * _Cursor_h, KeyType *_pElement);
	 * uint32_t           GetPreviousElement(void * _Cursor_h, KeyType *_pElement);
	 *
	 * uint32_t           InsertElement(void * _Cursor_h, KeyType *_pElement);
	 * uint32_t           SearchElement(void * _Cursor_h, KeyType *_pSearchElement, KeyType *_pFoundElement, BOFCMP _Cmp_E);
	 * uint32_t           UpdateElement(void * _Cursor_h, KeyType *_pSearchElement, KeyType *_pNewElement);
	 * uint32_t           DeleteElement(KeyType *_pElement);
	 */
	Val_S32      = mpBofRamDb_O->CheckDb(false);
	EXPECT_EQ(Val_S32, 1);

	Val_S32      = mpBofRamDb_O->CheckDb(true);
	EXPECT_EQ(Val_S32, 1);

	Nb_U32       = sizeof(pBuffer_c);
	Val_S32      = mpBofRamDb_O->DumpDatabase("Title", 0xFFFFFFFF, &Nb_U32, pBuffer_c);

	// EXPECT_EQ(Val_S32, 497);
	// EXPECT_EQ(Nb_U32, 65127);

	pStat_X      = mpBofRamDb_O->GetStatistic();
	EXPECT_EQ(pStat_X->NbInsertRequest_U32, 0);
	EXPECT_EQ(pStat_X->NbInsertExecuted_U32, 0);
	EXPECT_EQ(pStat_X->NbInsertCancelled_U32, 0);

	EXPECT_EQ(pStat_X->NbSearchRequest_U32, 0);
	EXPECT_EQ(pStat_X->NbSearchExecuted_U32, 0);
	EXPECT_EQ(pStat_X->NbSearchMatch_U32, 0);

	EXPECT_EQ(pStat_X->NbDeleteRequest_U32, 0);
	EXPECT_EQ(pStat_X->NbDeleteExecuted_U32, 0);
	EXPECT_EQ(pStat_X->NbUpdateDeleteCancelled_U32, 0);

	EXPECT_EQ(pStat_X->NbUpdateRequest_U32, 0);
	EXPECT_EQ(pStat_X->NbUpdateIndexExecuted_U32, 0);
	EXPECT_EQ(pStat_X->NbUpdateDataExecuted_U32, 0);
	EXPECT_EQ(pStat_X->NbUpdateCancelled_U32, 0);

	Err_U32      = mpBofRamDb_O->ClearStatistic();
	EXPECT_EQ(Err_U32, 0);
}

TEST_F(BofRamDb_Test, Cursor)
{
	uint32_t Err_U32;
	int      i, j;
	void     *pCursor_h[BOFRAMDB_CURSOR_MAX], *Cursor_h;

	// void          ClearDbAndReleaseCursor();

	Err_U32 = mpBofRamDb_O->GetCursor(DbRow::DB_INDEX_MAX, &Cursor_h);
	EXPECT_EQ(Err_U32, BOF_ERR_INDEX);
	Err_U32 = mpBofRamDb_O->GetCursor(DbRow::DB_INDEX_MAX, nullptr);
	EXPECT_EQ(Err_U32, BOF_ERR_INDEX);
	Err_U32 = mpBofRamDb_O->GetCursor(DbRow::DB_INDEX_KEY_U32, nullptr);
	EXPECT_EQ(Err_U32, BOF_ERR_EINVAL);

	for (i = 0 ; i < DbRow::DB_INDEX_MAX ; i++)
	{
		for (j = 0 ; j < BOFRAMDB_CURSOR_MAX ; j++)
		{
			Err_U32 = mpBofRamDb_O->GetCursor(i, &pCursor_h[j]);
			EXPECT_EQ(Err_U32, 0);
			EXPECT_TRUE(pCursor_h[j] != 0);
		}
		Err_U32 = mpBofRamDb_O->GetCursor(DbRow::DB_INDEX_KEY_U32, &Cursor_h);
		EXPECT_EQ(Err_U32, BOF_ERR_NO_MORE);
		Err_U32 = mpBofRamDb_O->FreeCursor(&pCursor_h[0]);
		EXPECT_EQ(Err_U32, 0);
		Err_U32 = mpBofRamDb_O->GetCursor(DbRow::DB_INDEX_KEY_U32, &pCursor_h[0]);
		EXPECT_EQ(Err_U32, 0);

		Err_U32 = mpBofRamDb_O->FreeCursor(nullptr);
		EXPECT_EQ(Err_U32, BOF_ERR_CURSOR);

		for (j = 0 ; j < BOFRAMDB_CURSOR_MAX ; j++)
		{
			Err_U32 = mpBofRamDb_O->FreeCursor(&pCursor_h[j]);
			EXPECT_EQ(Err_U32, 0);
		}
		Err_U32 = mpBofRamDb_O->FreeCursor(&pCursor_h[0]);
		EXPECT_EQ(Err_U32, BOF_ERR_CURSOR);
	}
}

TEST_F(BofRamDb_Test, Insert)
{
	uint32_t        Err_U32, Val_U32;
	int             i, j;
	void            *pCursor_h[DbRow::DB_INDEX_MAX];
	DbRow           DbRow_O;
	char            pKeyText_c[16];
	BOF_RAM_DB_STAT *pStat_X;

	for (i = 0 ; i < DbRow::DB_INDEX_MAX ; i++)
	{
		DbRow_O.SetKeyU32(3);
		DbRow_O.SetKeyText16("hello");

		DbRow_O.SetFloat(1.0f);
		DbRow_O.SetInt(2);
		DbRow_O.SetText80("world");

		Err_U32 = mpBofRamDb_O->GetCursor(i, &pCursor_h[i]);
		EXPECT_EQ(Err_U32, 0);

		Err_U32 = mpBofRamDb_O->GetCursor(i, &pCursor_h[i]);
		EXPECT_EQ(Err_U32, 0);

		Err_U32 = mpBofRamDb_O->InsertElement(nullptr, &DbRow_O);
		EXPECT_EQ(Err_U32, BOF_ERR_CURSOR);
		Err_U32 = mpBofRamDb_O->InsertElement(nullptr, nullptr);
		EXPECT_EQ(Err_U32, BOF_ERR_CURSOR);
		Err_U32 = mpBofRamDb_O->InsertElement(pCursor_h[i], nullptr);
		EXPECT_EQ(Err_U32, BOF_ERR_EINVAL);

// 3 Insert 0 Exec
		Err_U32 = mpBofRamDb_O->InsertElement(pCursor_h[i], &DbRow_O);
		EXPECT_EQ(Err_U32, 0);
		Val_U32 = mpBofRamDb_O->GetNbRecord();
		EXPECT_EQ(Val_U32, 1);

// 4 Insert 1 exec
		Err_U32 = mpBofRamDb_O->InsertElement(pCursor_h[i], &DbRow_O);
		EXPECT_EQ(Err_U32, BOF_ERR_DUPLICATE);
		DbRow_O.SetKeyU32(4);
		Err_U32 = mpBofRamDb_O->InsertElement(pCursor_h[i], &DbRow_O);
		EXPECT_EQ(Err_U32, BOF_ERR_DUPLICATE);
		DbRow_O.SetKeyText16("world");
		Err_U32 = mpBofRamDb_O->InsertElement(pCursor_h[i], &DbRow_O);
		EXPECT_EQ(Err_U32, 0);
		Val_U32 = mpBofRamDb_O->GetNbRecord();
		EXPECT_EQ(Val_U32, 2);

// 7 Insert 2 Exec 2 Cancelled
		Err_U32 = mpBofRamDb_O->ClearDbAndReleaseCursor();
		EXPECT_EQ(Err_U32, 0);

		Err_U32 = mpBofRamDb_O->GetCursor(i, &pCursor_h[i]);
		EXPECT_EQ(Err_U32, 0);

		Val_U32 = mpBofRamDb_O->GetNbRecord();
		EXPECT_EQ(Val_U32, 0);

		for (j = 0 ; j < BOFRAMDBNBMAXELEM ; j++)
		{
			DbRow_O.SetKeyU32(j);
			sprintf(pKeyText_c, "Key16_%06d", j);
			DbRow_O.SetKeyText16(pKeyText_c);

			Err_U32 = mpBofRamDb_O->InsertElement(pCursor_h[i], &DbRow_O);
			EXPECT_EQ(Err_U32, 0);
		}

// 107 Insert 102 Exec 2 Cancelled

		DbRow_O.SetKeyU32(j);
		sprintf(pKeyText_c, "Key16_%06d", j);
		DbRow_O.SetKeyText16(pKeyText_c);

		Err_U32 = mpBofRamDb_O->InsertElement(pCursor_h[i], &DbRow_O);
		EXPECT_EQ(Err_U32, BOF_ERR_FULL);

		pStat_X = mpBofRamDb_O->GetStatistic();
		EXPECT_EQ(pStat_X->NbInsertRequest_U32, BOFRAMDBNBMAXELEM + 8);
		EXPECT_EQ(pStat_X->NbInsertExecuted_U32, BOFRAMDBNBMAXELEM + 2);
		EXPECT_EQ(pStat_X->NbInsertCancelled_U32, 2);

		EXPECT_EQ(pStat_X->NbSearchRequest_U32, 0);
		EXPECT_EQ(pStat_X->NbSearchExecuted_U32, 0);
		EXPECT_EQ(pStat_X->NbSearchMatch_U32, 0);

		EXPECT_EQ(pStat_X->NbDeleteRequest_U32, 0);
		EXPECT_EQ(pStat_X->NbDeleteExecuted_U32, 0);
		EXPECT_EQ(pStat_X->NbUpdateDeleteCancelled_U32, 0);

		EXPECT_EQ(pStat_X->NbUpdateRequest_U32, 0);
		EXPECT_EQ(pStat_X->NbUpdateIndexExecuted_U32, 0);
		EXPECT_EQ(pStat_X->NbUpdateDataExecuted_U32, 0);
		EXPECT_EQ(pStat_X->NbUpdateCancelled_U32, 0);

		Err_U32 = mpBofRamDb_O->ClearStatistic();
		EXPECT_EQ(Err_U32, 0);

		pStat_X = mpBofRamDb_O->GetStatistic();
		EXPECT_EQ(pStat_X->NbInsertRequest_U32, 0);

		Err_U32 = mpBofRamDb_O->ClearDbAndReleaseCursor();
		EXPECT_EQ(Err_U32, 0);
	}
}