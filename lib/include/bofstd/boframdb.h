/*$T Bof/BofRamDb.h GC 1.140 02/11/08 12:15:59 */

/*
 * File      : BofRamDb.h
 *
 * Project   : Bof
 *
 * Package   : Bog-Include
 *
 * Company   : Sci
 *
 * Author    : Bernard HARMEL: b.harmel@gmail.com
 *
 * Purpose   : This is the definition of the BofRamDb template
 *
 * Copyright : (C) Sci
 *
 * Version History:
 * V 1.00  Wed Sep 6 2006  BHA : Initial release
 */
#pragma once
/*** Include *****************************************************************/
#include <bofstd/bofstd.h>
#include <bofstd/bofsystem.h>
#include <bofstd/bofavlnode.h>
#include <bofstd/bofavltree.h>
#include <stdio.h>

BEGIN_BOF_NAMESPACE()

/*** Define *****************************************************************/

#define BOFRAMDB_CURSOR_MAX            512
#define BOFRAMDB_CURSOR_MAGICNUMBER    0x54567453

/*** Macro *******************************************************************/

/*** Structures *************************************************************/
/*** BOFRAMDBSTAT *********************************************************************/

/*!
 * Summary
 * Information collected for statistics purpose (Debug screen)
 */
struct BOF_RAM_DB_STAT
{
		uint32_t NbInsertRequest_U32;         /*! Number of 'Insert' operation requested*/
		uint32_t NbInsertExecuted_U32;        /*! Number of 'Insert' operation accepted*/
		uint32_t NbInsertCancelled_U32;       /*! Number of 'Insert' operation cancelled (same record or locked)*/

		uint32_t NbSearchRequest_U32;         /*! Number of 'Search' operation requested*/
		uint32_t NbSearchExecuted_U32;        /*! Number of 'Search' operation accepted*/
		uint32_t NbSearchMatch_U32;           /*! Number of 'Search' operation successful*/

		uint32_t NbDeleteRequest_U32;         /*! Number of 'Delete' operation requested*/
		uint32_t NbDeleteExecuted_U32;        /*! Number of 'Delete' operation accepted*/
		uint32_t NbUpdateDeleteCancelled_U32; /*! Number of 'Delete' operation cancelled (same record or locked)*/

		uint32_t NbUpdateRequest_U32;         /*! Number of 'Update' operation requested*/
		uint32_t NbUpdateIndexExecuted_U32;   /*! Number of 'Update Index' operation accepted*/
		uint32_t NbUpdateDataExecuted_U32;    /*! Number of 'Update Data' operation accepted*/
		uint32_t NbUpdateCancelled_U32;       /*! Number of 'Update' operation cancelled (same record or locked)*/

		BOF_RAM_DB_STAT()
		{
			Reset();
		}

		void Reset()
		{
			NbInsertRequest_U32 = 0;
			NbInsertExecuted_U32 = 0;
			NbInsertCancelled_U32 = 0;

			NbSearchRequest_U32 = 0;
			NbSearchExecuted_U32 = 0;
			NbSearchMatch_U32 = 0;

			NbDeleteRequest_U32 = 0;
			NbDeleteExecuted_U32 = 0;
			NbUpdateDeleteCancelled_U32 = 0;

			NbUpdateRequest_U32 = 0;
			NbUpdateIndexExecuted_U32 = 0;
			NbUpdateDataExecuted_U32 = 0;
			NbUpdateCancelled_U32 = 0;
		}
};

/*** BOFAVLCURSOR *********************************************************************/

/*!
 * Summary
 * Cursor state
 */
struct BOF_RAM_DB_CURSOR
{
		void *pElement;
		uint32_t Index_U32;
		uint32_t MagicNumber_U32;

		BOF_RAM_DB_CURSOR()
		{
			Reset();
		}

		void Reset()
		{
			pElement = nullptr;
			Index_U32 = 0;
			MagicNumber_U32 = 0;
		}
};

/*** Class *********************************************************************************/
template<typename KeyType>
class BofRamDb
{
private:
		BOF_MUTEX mMtx_X;
		uint32_t mNbMaxElement_U32;
		uint32_t mNbIndex_U32;
		uint32_t mNbRecord_U32;
		uint32_t mNbFreeCursor_U32;
		uint32_t mNextRamDbFreeElement_U32;
		uint32_t *mpRamDbFreeElementList_U32;
		BOF_RAM_DB_STAT mDbRamStat_X;  /*! Statistics data*/
		BOF_RAM_DB_CURSOR mpCursorState_X[BOFRAMDB_CURSOR_MAX];
		BofAvlTree<KeyType> **mppRamDbTree;
		KeyType *mpElementList;

public:
		BofRamDb(uint32_t _NbMaxElement_U32, uint32_t _NbIndex_U32, uint32_t *_pErrorCode_U32);

		virtual ~BofRamDb();

		bool IsDbEmpty()
		{ return mNbRecord_U32 == 0; }

		bool IsDbFull()
		{ return mNbRecord_U32 == mNbMaxElement_U32; }

		uint32_t ClearDbAndReleaseCursor();

		uint32_t GetNbFreeCursor()
		{ return mNbFreeCursor_U32; }

		uint32_t GetNbRecord()
		{ return mNbRecord_U32; }

		uint32_t GetCursor(uint32_t _Index_U32, void **_pCursor_h);

		uint32_t FreeCursor(void **_pCursor_h);

		uint32_t GetFirstElement(void *_Cursor_h, KeyType *_pElement);

		uint32_t GetLastElement(void *_Cursor_h, KeyType *_pElement);

		uint32_t GetCurrentElement(void *_Cursor_h, KeyType *_pElement);

		uint32_t GetNextElement(void *_Cursor_h, KeyType *_pElement);

		uint32_t GetPreviousElement(void *_Cursor_h, KeyType *_pElement);

		uint32_t InsertElement(void *_Cursor_h, KeyType *_pElement);

		uint32_t SearchElement(void *_Cursor_h, KeyType *_pSearchElement, KeyType *_pFoundElement, BOFCMP _Cmp_E);

		uint32_t UpdateElement(void *_Cursor_h, KeyType *_pSearchElement, KeyType *_pNewElement);

		uint32_t DeleteElement(KeyType *_pElement);

		int32_t CheckDb(bool _ExtendedTest_B);

		uint32_t DumpDatabase(const char *_pTitle_c, uint32_t _Flag_U32, uint32_t *_pNbMaxChar_U32, char *_pBuffer_c);

		BOF_RAM_DB_STAT *GetStatistic()
		{ return &mDbRamStat_X; }

		uint32_t ClearStatistic()
		{
			mDbRamStat_X.Reset();
			return BOF_ERR_NO_ERROR;
		}
};

// Template !

/*
 * File      : BofAvlTree.cpp
 *
 * Project   : Bof
 *
 * Package   : Bog-Include
 *
 * Company   : Sci
 *
 * Author    : Bernard HARMEL: b.harmel@gmail.com
 *
 * Purpose   : This is the implementation of the BofAvlTree template
 *
 * Copyright :(C) Sci
 *
 * Version History:
 * V 1.00  Wed Sep 6 2006  BHA : Initial release
 *
 */
template<typename KeyType>
BofRamDb<KeyType>::BofRamDb(uint32_t _NbMaxElement_U32, uint32_t _NbIndex_U32, uint32_t *_pErrorCode_U32)
{
	uint32_t i_U32;

	uint32_t Rts_U32 = (uint32_t) BOF_ERR_EINVAL;

	Bof_CreateMutex("BofRamDb", true, true, mMtx_X);
	mNbMaxElement_U32 = _NbMaxElement_U32;
	mNbIndex_U32 = _NbIndex_U32;
	mNbRecord_U32 = 0;
	mNbFreeCursor_U32 = BOFRAMDB_CURSOR_MAX;
	mDbRamStat_X.Reset();
	for (i_U32 = 0; i_U32 < BOFRAMDB_CURSOR_MAX; i_U32++)
	{
		mpCursorState_X[i_U32].Reset();
	}
	mpElementList = nullptr;
	mpRamDbFreeElementList_U32 = nullptr;
	mNextRamDbFreeElement_U32 = 0;
	mppRamDbTree = nullptr;

	if ((_NbMaxElement_U32) && (_NbIndex_U32))
	{
		Rts_U32 = (uint32_t) BOF_ERR_ENOMEM;

		// Allocate static storage for data
		mpElementList = new KeyType[mNbMaxElement_U32];
		if (mpElementList)
		{
			// Allocate storage for free element list
			mpRamDbFreeElementList_U32 = new uint32_t[mNbMaxElement_U32];
			if (mpRamDbFreeElementList_U32)
			{
				mNextRamDbFreeElement_U32 = 0;

				for (i_U32 = 0; i_U32 < mNbMaxElement_U32; i_U32++)
				{
					mpRamDbFreeElementList_U32[i_U32] = i_U32 + 1;
				}

				mpRamDbFreeElementList_U32[mNbMaxElement_U32 - 1] = 0xFFFFFFFF;

				// Allocate static storage for pointer to avl tree index
				mppRamDbTree = new BofAvlTree<KeyType> *[mNbIndex_U32];
				if (mppRamDbTree)
				{
					for (i_U32 = 0; i_U32 < mNbIndex_U32; i_U32++)
					{
						// Allocate static storage for avl tree index
						mppRamDbTree[i_U32] = new BofAvlTree<KeyType>(_NbMaxElement_U32, i_U32, &Rts_U32);
						if (Rts_U32 != BOF_ERR_NO_ERROR)
						{
							break;
						}

						if (!mppRamDbTree[i_U32])
						{
							Rts_U32 = (uint32_t) BOF_ERR_ENOMEM; // Can be set to BOF_ERR_NO_ERROR in a previous call to new BofAvlTree<KeyType>
							break;
						}
					}

					if (i_U32 == mNbIndex_U32)
					{
						Rts_U32 = BOF_ERR_NO_ERROR;
					}
				}
			}
		}
	}

	if (_pErrorCode_U32)
	{
		*_pErrorCode_U32 = Rts_U32;
	}
}


template<typename KeyType>
BofRamDb<KeyType>::~BofRamDb()
{
	uint32_t i_U32;

	Bof_LockMutex(mMtx_X);

	for (i_U32 = 0; i_U32 < mNbIndex_U32; i_U32++)
	{
		BOF_SAFE_DELETE(mppRamDbTree[i_U32]);
	}

	BOF_SAFE_DELETE_ARRAY(mppRamDbTree);

	BOF_SAFE_DELETE_ARRAY(mpRamDbFreeElementList_U32);
	BOF_SAFE_DELETE_ARRAY(mpElementList);
	Bof_UnlockMutex(mMtx_X);
	Bof_DestroyMutex(mMtx_X);
}


template<typename KeyType>
uint32_t BofRamDb<KeyType>::ClearDbAndReleaseCursor()
{
	uint32_t i_U32;

	mNbRecord_U32 = 0;
	mNbFreeCursor_U32 = BOFRAMDB_CURSOR_MAX;
	for (i_U32 = 0; i_U32 < BOFRAMDB_CURSOR_MAX; i_U32++)
	{
		mpCursorState_X[i_U32].Reset();
	}
	for (i_U32 = 0; i_U32 < mNbIndex_U32; i_U32++)
	{
		mppRamDbTree[i_U32]->Clear();
	}

	mNextRamDbFreeElement_U32 = 0;

	for (i_U32 = 0; i_U32 < mNbMaxElement_U32; i_U32++)
	{
		mpRamDbFreeElementList_U32[i_U32] = i_U32 + 1;
	}

	if (mNbMaxElement_U32)
	{
		mpRamDbFreeElementList_U32[mNbMaxElement_U32 - 1] = 0xFFFFFFFF;
	}
	return BOF_ERR_NO_ERROR;
}


template<typename KeyType>
uint32_t BofRamDb<KeyType>::GetCursor(uint32_t _Index_U32, void **_pCursor_h)
{
	uint32_t Rts_U32 = (uint32_t) BOF_ERR_INDEX;

	uint32_t i_U32;
	BOF_RAM_DB_CURSOR *pCursor_X;

	if (_Index_U32 < mNbIndex_U32)
	{
		Rts_U32 = (uint32_t) BOF_ERR_EINVAL;

		if (_pCursor_h)
		{
			Rts_U32 = (uint32_t) BOF_ERR_NO_MORE;

			if (mNbFreeCursor_U32)
			{
				Bof_LockMutex(mMtx_X);
				pCursor_X = mpCursorState_X;

				for (i_U32 = 0; i_U32 < BOFRAMDB_CURSOR_MAX; i_U32++, pCursor_X++)
				{
					if (pCursor_X->MagicNumber_U32 != BOFRAMDB_CURSOR_MAGICNUMBER)
					{
						pCursor_X->Reset();
						pCursor_X->MagicNumber_U32 = BOFRAMDB_CURSOR_MAGICNUMBER;
						pCursor_X->pElement = nullptr;
						pCursor_X->Index_U32 = _Index_U32;
						*_pCursor_h = pCursor_X;
						mNbFreeCursor_U32--;
						Rts_U32 = BOF_ERR_NO_ERROR;
						break;
					}
				}

				Bof_UnlockMutex(mMtx_X);
			}
		}
	}

	return Rts_U32;
}


template<typename KeyType>
uint32_t BofRamDb<KeyType>::FreeCursor(void **_pCursor_h)
{
	uint32_t Rts_U32 = (uint32_t) BOF_ERR_CURSOR;
	BOF_RAM_DB_CURSOR *pCursor_X;

	if (_pCursor_h)
	{
		pCursor_X = (BOF_RAM_DB_CURSOR * )(*_pCursor_h);

		if ((pCursor_X) && (pCursor_X->MagicNumber_U32 == BOFRAMDB_CURSOR_MAGICNUMBER))
		{
			Bof_LockMutex(mMtx_X);
			pCursor_X->Reset();
			*_pCursor_h = nullptr;
			mNbFreeCursor_U32++;
			Bof_UnlockMutex(mMtx_X);
			Rts_U32 = BOF_ERR_NO_ERROR;
		}
	}
	return Rts_U32;
}


template<typename KeyType>
uint32_t BofRamDb<KeyType>::GetFirstElement(void *_Cursor_h, KeyType *_pElement)
{
	uint32_t Rts_U32 = (uint32_t) BOF_ERR_CURSOR;
	BOF_RAM_DB_CURSOR *pCursor_X;

	BofAvlNode<KeyType> *pElement;

	pCursor_X = (BOF_RAM_DB_CURSOR *) _Cursor_h;

	if ((pCursor_X) && (pCursor_X->MagicNumber_U32 == BOFRAMDB_CURSOR_MAGICNUMBER))
	{
		Rts_U32 = (uint32_t) BOF_ERR_EINVAL;

		if (_pElement)
		{
			Bof_LockMutex(mMtx_X);
			pElement = mppRamDbTree[pCursor_X->Index_U32]->GetFirst();

			if (pElement)
			{
				pCursor_X->pElement = pElement;

				*_pElement = *(pElement->GetData());
				Rts_U32 = BOF_ERR_NO_ERROR;
			}
			else
			{
				Rts_U32 = (uint32_t) BOF_ERR_EMPTY;
			}

			Bof_UnlockMutex(mMtx_X);
		}
	}

	return Rts_U32;
}


template<typename KeyType>
uint32_t BofRamDb<KeyType>::GetLastElement(void *_Cursor_h, KeyType *_pElement)
{
	uint32_t Rts_U32 = BOF_ERR_CURSOR;
	BOF_RAM_DB_CURSOR *pCursor_X;

	BofAvlNode<KeyType> *pElement;

	pCursor_X = (BOF_RAM_DB_CURSOR *) _Cursor_h;

	if ((pCursor_X) && (pCursor_X->MagicNumber_U32 == BOFRAMDB_CURSOR_MAGICNUMBER))
	{
		Rts_U32 = BOF_ERR_EINVAL;

		if (_pElement)
		{
			Bof_LockMutex(mMtx_X);
			pElement = mppRamDbTree[pCursor_X->Index_U32]->GetLast();

			if (pElement)
			{
				pCursor_X->pElement = pElement;
				*_pElement = *(pElement->GetData());
				Rts_U32 = BOF_ERR_NO_ERROR;
			}
			else
			{
				Rts_U32 = BOF_ERR_EMPTY;
			}

			Bof_UnlockMutex(mMtx_X);
		}
	}

	return Rts_U32;
}


template<typename KeyType>
uint32_t BofRamDb<KeyType>::GetCurrentElement(void *_Cursor_h, KeyType *_pElement)
{
	uint32_t Rts_U32 = BOF_ERR_CURSOR;
	BOF_RAM_DB_CURSOR *pCursor_X;

	BofAvlNode<KeyType> *pElement;

	pCursor_X = (BOF_RAM_DB_CURSOR *) _Cursor_h;

	if ((pCursor_X) && (pCursor_X->MagicNumber_U32 == BOFRAMDB_CURSOR_MAGICNUMBER))
	{
		Rts_U32 = BOF_ERR_EINVAL;

		if (_pElement)
		{
			Bof_LockMutex(mMtx_X);
			pElement = (BofAvlNode<KeyType> *) pCursor_X->pElement;

			if (pElement)
			{
				*_pElement = *(pElement->GetData());
				Rts_U32 = BOF_ERR_NO_ERROR;
			}
			else
			{
				Rts_U32 = BOF_ERR_EMPTY;
			}

			Bof_UnlockMutex(mMtx_X);
		}
	}

	return Rts_U32;
}


template<typename KeyType>
uint32_t BofRamDb<KeyType>::GetNextElement(void *_Cursor_h, KeyType *_pElement)
{
	uint32_t Rts_U32 = (uint32_t) BOF_ERR_CURSOR;
	BOF_RAM_DB_CURSOR *pCursor_X;

	BofAvlNode<KeyType> *pElement;

	pCursor_X = (BOF_RAM_DB_CURSOR *) _Cursor_h;

	if ((pCursor_X) && (pCursor_X->MagicNumber_U32 == BOFRAMDB_CURSOR_MAGICNUMBER))
	{
		Rts_U32 = (uint32_t) BOF_ERR_EINVAL;

		if (_pElement)
		{
			Bof_LockMutex(mMtx_X);
			pElement = mppRamDbTree[pCursor_X->Index_U32]->GetNext((BofAvlNode<KeyType> *) (pCursor_X->pElement));

			if (pElement)
			{
				pCursor_X->pElement = pElement;
				*_pElement = *(pElement->GetData());
				Rts_U32 = BOF_ERR_NO_ERROR;
			}
			else
			{
				Rts_U32 = (uint32_t) BOF_ERR_EOF;
			}

			Bof_UnlockMutex(mMtx_X);
		}
	}

	return Rts_U32;
}


template<typename KeyType>
uint32_t BofRamDb<KeyType>::GetPreviousElement(void *_Cursor_h, KeyType *_pElement)
{
	uint32_t Rts_U32 = BOF_ERR_CURSOR;
	BOF_RAM_DB_CURSOR *pCursor_X;

	BofAvlNode<KeyType> *pElement;

	pCursor_X = (BOF_RAM_DB_CURSOR *) _Cursor_h;

	if ((pCursor_X) && (pCursor_X->MagicNumber_U32 == BOFRAMDB_CURSOR_MAGICNUMBER))
	{
		Rts_U32 = BOF_ERR_EINVAL;

		if (_pElement)
		{
			Bof_LockMutex(mMtx_X);
			pElement = mppRamDbTree[pCursor_X->Index_U32]->GetPrevious((BofAvlNode<KeyType> *) (pCursor_X->pElement));

			if (pElement)
			{
				pCursor_X->pElement = pElement;
				*_pElement = *(pElement->GetData());
				Rts_U32 = BOF_ERR_NO_ERROR;
			}
			else
			{
				Rts_U32 = BOF_ERR_EOF;
			}

			Bof_UnlockMutex(mMtx_X);
		}
	}

	return Rts_U32;
}


template<typename KeyType>
uint32_t BofRamDb<KeyType>::InsertElement(void *_Cursor_h, KeyType *_pElement)
{
	uint32_t Rts_U32 = (uint32_t) BOF_ERR_CURSOR, Tree_U32, i_U32;
	BOF_RAM_DB_CURSOR *pCursor_X;
	BofAvlNode<KeyType> *pNode_O;

	mDbRamStat_X.NbInsertRequest_U32++;
	pCursor_X = (BOF_RAM_DB_CURSOR *) _Cursor_h;

	if ((pCursor_X) && (pCursor_X->MagicNumber_U32 == BOFRAMDB_CURSOR_MAGICNUMBER))
	{
		Rts_U32 = (uint32_t) BOF_ERR_EINVAL;

		if (_pElement)
		{
			Rts_U32 = (uint32_t) BOF_ERR_FULL;
			Bof_LockMutex(mMtx_X);

			if (mNextRamDbFreeElement_U32 != 0xFFFFFFFF)
			{
				mpElementList[mNextRamDbFreeElement_U32] = *_pElement;
				Rts_U32 = BOF_ERR_NO_ERROR;

				for (Tree_U32 = 0; Tree_U32 < mNbIndex_U32; Tree_U32++)
				{
					pNode_O = mppRamDbTree[Tree_U32]->Insert(&mpElementList[mNextRamDbFreeElement_U32]); // mNextRamDbFreeElement_U32))

/*
 * if (pNode_O)
 * {
 * pData_O=pNode_O->GetData();
 * }
 * else
 * {
 * pData_O=_pElement;
 * }
 * char p[128];
 * printf("[]Insert %d k %8.8s s %X Node %X\r\n",Tree_U32,pData_O->GetGuid(nullptr,p),pData_O->GetSeq(),pNode_O);
 */
					if (pNode_O)
					{
						Rts_U32 = (uint32_t) BOF_ERR_DUPLICATE;
						break;
					}
				}

				if (Rts_U32 == BOF_ERR_NO_ERROR)
				{
					mNbRecord_U32++;
					mDbRamStat_X.NbInsertExecuted_U32++;
					mNextRamDbFreeElement_U32 = mpRamDbFreeElementList_U32[mNextRamDbFreeElement_U32];
				}
				else
				{
					mDbRamStat_X.NbInsertCancelled_U32++;

					for (i_U32 = 0; i_U32 < Tree_U32; i_U32++)
					{
						mppRamDbTree[i_U32]->Delete(&mpElementList[mNextRamDbFreeElement_U32], BOF_CMP_EQUAL);
					}
				}
			}

			Bof_UnlockMutex(mMtx_X);
		}
	}

	return Rts_U32;
}


template<typename KeyType>
uint32_t BofRamDb<KeyType>::SearchElement(void *_Cursor_h, KeyType *_pSearchElement, KeyType *_pFoundElement, BOFCMP _Cmp_E)
{
	uint32_t Rts_U32 = (uint32_t) BOF_ERR_CURSOR;
	BOF_RAM_DB_CURSOR *pCursor_X;

	BofAvlNode<KeyType> *pElement;

	mDbRamStat_X.NbSearchRequest_U32++;
	pCursor_X = (BOF_RAM_DB_CURSOR *) _Cursor_h;

	if ((pCursor_X) && (pCursor_X->MagicNumber_U32 == BOFRAMDB_CURSOR_MAGICNUMBER))
	{
		Rts_U32 = (uint32_t) BOF_ERR_EINVAL;

		if ((_pSearchElement) && (_pFoundElement))
		{
			Bof_LockMutex(mMtx_X);
			mDbRamStat_X.NbSearchExecuted_U32++;
			pElement = mppRamDbTree[pCursor_X->Index_U32]->Search(_pSearchElement, _Cmp_E);

			if (pElement)
			{
				mDbRamStat_X.NbSearchMatch_U32++;
				pCursor_X->pElement = pElement;
				*_pFoundElement = *(pElement->GetData());
				Rts_U32 = BOF_ERR_NO_ERROR;
			}
			else
			{
				Rts_U32 = (uint32_t) BOF_ERR_EOF;
			}

			Bof_UnlockMutex(mMtx_X);
		}
	}

	return Rts_U32;
}


template<typename KeyType>
uint32_t BofRamDb<KeyType>::DeleteElement(KeyType *_pElement)
{
	uint32_t Rts_U32 = (uint32_t) BOF_ERR_EINVAL, i_U32, Position_U32; // ,NbNext_U32,NbPrev_U32;

// BOFRAMDBCURSOR			*pCursor_X, *pCurs_X;
	BofAvlNode<KeyType> *pElement;

// void						*pCursorPos[BOFRAMDB_CURSOR_MAX];
	KeyType *pData;

// void						*pCursorPos[BOFRAMDB_CURSOR_MAX];
	KeyType Next;

	mDbRamStat_X.NbDeleteRequest_U32++;

	if (_pElement)
	{
		Rts_U32 = BOF_ERR_NO_ERROR;
		Bof_LockMutex(mMtx_X);

		// Check if the element is present in all index
		for (i_U32 = 0; i_U32 < mNbIndex_U32; i_U32++)
		{
			pElement = mppRamDbTree[i_U32]->Search(_pElement, BOF_CMP_EQUAL);

/*
 * char p[128];
 * pData=pElement ? pElement->GetData():nullptr;
 * printf("[]Delete %d %X/%X g %8.8s s %X\r\n",i_U32,_pElement,pElement,pData ? pData->GetGuid(nullptr,p):"nullptr",pData->GetSeq());
 */
			if (!pElement)
			{
				break;
			}
		}

		if (i_U32 == mNbIndex_U32)
		{
			/*
			 *                NbNext_U32=0;
			 *                pCurs_X=mpCursorState_X;
			 *                for (i_U32=0;i_U32<BOFRAMDB_CURSOR_MAX;i_U32++,pCurs_X++)
			 *                {
			 *                        if (pCurs_X->MagicNumber_U32==BOFRAMDB_CURSOR_MAGICNUMBER)
			 *                        {
			 *                                if (pCurs_X->pElement==pElement)
			 *                                {
			 * //Try to go tio the next record
			 *                                        pCursorPos[i_U32]=pCurs_X->pElement;
			 *                                        if (GetNextElement((void *)pCurs_X,&Next)!=BOF_ERR_NO_ERROR) break;
			 *                                                else NbNext_U32++;
			 *                                }
			 *                        }
			 *                }
			 *                if (i_U32==BOFRAMDB_CURSOR_MAX)
			 */
			{
				mDbRamStat_X.NbDeleteExecuted_U32++;

				pData = nullptr;

				for (i_U32 = 0; i_U32 < mNbIndex_U32; i_U32++)
				{
					pData = mppRamDbTree[i_U32]->Delete(_pElement, BOF_CMP_EQUAL);

/*
 * char p[128];
 * printf("[]DoDelete %d g %8.8s s %X\r\n",i_U32,pData ? pData->GetGuid(nullptr,p):"nullptr",pData->GetSeq());
 */

					// All pData in all index points to the same static element
					// No error can occurs !!!!			if (Rts_U32!=BOF_ERR_NO_ERROR) break;
				}

				if (pData)
				{
// !!!64bits!!!
					Position_U32 = (uint32_t) (pData - mpElementList);

					if (Position_U32 < mNbMaxElement_U32)
					{
						mNbRecord_U32--;
						mpRamDbFreeElementList_U32[Position_U32] = mNextRamDbFreeElement_U32;
						mNextRamDbFreeElement_U32 = Position_U32;
					}
					else
					{
						Rts_U32 = (uint32_t) BOF_ERR_INTERNAL;
					}
				}
				else
				{
					Rts_U32 = (uint32_t) BOF_ERR_INTERNAL;
				}
			}

			/*
			 *                else
			 *                {
			 * //Rollback next
			 *                        NbPrev_U32=0;
			 *                        pCurs_X=mpCursorState_X;
			 *                        for (i_U32=0;i_U32<BOFRAMDB_CURSOR_MAX;i_U32++,pCurs_X++)
			 *                        {
			 *                                if (pCurs_X->MagicNumber_U32==BOFRAMDB_CURSOR_MAGICNUMBER)
			 *                                {
			 *                                        if (NbPrev_U32>NbNext_U32) break;
			 *                                        pCurs_X->pElement=pCursorPos[i_U32];
			 *                                }
			 *                        }
			 *                        Rts_U32=BOFRAMDB_ERROR_LOCK;
			 *                }
			 */
		}
		else
		{
			Rts_U32 = (uint32_t) BOF_ERR_NOT_FOUND;
		}

		Bof_UnlockMutex(mMtx_X);
	}

	return Rts_U32;
}


template<typename KeyType>
uint32_t BofRamDb<KeyType>::UpdateElement(void *_Cursor_h, KeyType *_pSearchElement, KeyType *_pNewElement)
{
	uint32_t Rts_U32 = (uint32_t) BOF_ERR_CURSOR;
	uint32_t Position_U32;
	uint32_t i_U32;
	BOF_RAM_DB_CURSOR *pCursor_X;
	BofAvlNode<KeyType> *pElement = nullptr;
	KeyType PreviousValue;
	KeyType Next;

	mDbRamStat_X.NbUpdateRequest_U32++;
	pCursor_X = (BOF_RAM_DB_CURSOR *) _Cursor_h;

	if ((pCursor_X) && (pCursor_X->MagicNumber_U32 == BOFRAMDB_CURSOR_MAGICNUMBER))
	{
		Rts_U32 = (uint32_t) BOF_ERR_EINVAL;

		if ((_pSearchElement) && (_pNewElement))
		{
			if (*_pSearchElement != *_pNewElement)
			{
				Bof_LockMutex(mMtx_X);

				// Check if the element is present in all index
				// BOFDBG_OUTPUT_0(DBG_FS,0,"UpdateElement Check if the element is present in all index\r\n");
				for (i_U32 = 0; i_U32 < mNbIndex_U32; i_U32++)
				{
					pElement = mppRamDbTree[i_U32]->Search(_pSearchElement, BOF_CMP_EQUAL);

					if (!pElement)
					{
						break;
					}
				}

				// BOFDBG_OUTPUT_0(DBG_FS,0,"UpdateElement Present %d/%d\r\n",i_U32,mNbIndex_U32);
				if (i_U32 == mNbIndex_U32)
				{
					/*
					 *                            NbNext_U32=0;
					 *                            pCurs_X=mpCursorState_X;
					 *                            for (i_U32=0;i_U32<BOFRAMDB_CURSOR_MAX;i_U32++,pCurs_X++)
					 *                            {
					 *                                    if (pCurs_X->MagicNumber_U32==BOFRAMDB_CURSOR_MAGICNUMBER)
					 *                                    {
					 *                                            if (pCurs_X->pElement==pElement)
					 *                                            {
					 *    //Try to go tio the next record
					 *                                                    pCursorPos[i_U32]=pCurs_X->pElement;
					 *                                                    if (GetNextElement((void *)pCurs_X,&Next)!=BOF_ERR_NO_ERROR) break;
					 *                                                            else NbNext_U32++;
					 *                                            }
					 *                                    }
					 *                            }
					 *                            if (i_U32==BOFRAMDB_CURSOR_MAX)
					 */
					{
						// Check if the index value of the new element are identical to the found one
						for (i_U32 = 0; i_U32 < mNbIndex_U32; i_U32++)
						{
							if (_pNewElement->Compare(i_U32, _pSearchElement) != BOF_CMP_EQUAL)
							{
								break;
							}
						}

						// BOFDBG_OUTPUT_0(DBG_FS,0,"UpdateElement Data/index %d/%d\r\n",i_U32,mNbIndex_U32);
						if ((i_U32 == mNbIndex_U32) && (pElement))
						{
							// Les index sont identique seuls les donnÈes sont diffÈrente
// !!!64bits!!!
							Position_U32 = (uint32_t) (pElement->GetData() - mpElementList);

							// BOFDBG_OUTPUT_0(DBG_FS,0,"UpdateElement Pos %d/%d\r\n",Position_U32,mNbMaxElement_U32);
							if (Position_U32 < mNbMaxElement_U32)
							{
								mDbRamStat_X.NbUpdateDataExecuted_U32++;
								Rts_U32 = BOF_ERR_NO_ERROR;
								mpElementList[Position_U32] = *_pNewElement;
								pCursor_X->pElement = pElement;
							}
							else
							{
								Rts_U32 = (uint32_t) BOF_ERR_INTERNAL;
							}
						}
						else
						{
							// Some index value are different->Save previous value (rollback) and erase old date
							PreviousValue = *(pElement->GetData());

							// DumpDb();
							Rts_U32 = DeleteElement(_pSearchElement);

							// DumpDb();
							if (Rts_U32 == BOF_ERR_NO_ERROR)
							{
								// Check if the new data is valid (!! duplicate index value !!)
								for (i_U32 = 0; i_U32 < mNbIndex_U32; i_U32++)
								{
									if (mppRamDbTree[i_U32]->Search(_pNewElement, BOF_CMP_EQUAL))
									{
										Rts_U32 = (uint32_t) BOF_ERR_DUPLICATE;
										break;
									}
								}

								if (Rts_U32 == BOF_ERR_NO_ERROR)
								{
									// No duplicate value insert new data
									Rts_U32 = InsertElement(_Cursor_h, _pNewElement);

									if (Rts_U32 != BOF_ERR_NO_ERROR)
									{
										InsertElement(_Cursor_h, &PreviousValue); // Rollback delete
									}
									else
									{
										mDbRamStat_X.NbUpdateIndexExecuted_U32++;
									}
								}
								else
								{
									InsertElement(_Cursor_h, &PreviousValue);   // Rollback delete
								}
							}

							if (Rts_U32 == BOF_ERR_NO_ERROR)
							{
								/*
								 *                                              pElement=mppRamDbTree[pCursor_X->Index_U32]->Search(_pSearchElement,BOF_CMP_EQUAL);
								 *                                              pCursor_X->pElement=pElement;
								 */
							}
							else
							{
								mDbRamStat_X.NbUpdateCancelled_U32++;
							}
						}
					}

					/*
					 *                            else
					 *                            {
					 * //Rollback next
					 *                                    NbPrev_U32=0;
					 *                                    pCurs_X=mpCursorState_X;
					 *                                    for (i_U32=0;i_U32<BOFRAMDB_CURSOR_MAX;i_U32++,pCurs_X++)
					 *                                    {
					 *                                            if (pCurs_X->MagicNumber_U32==BOFRAMDB_CURSOR_MAGICNUMBER)
					 *                                            {
					 *                                                    NbPrev_U32++;
					 *                                                    if (NbPrev_U32>NbNext_U32) break;
					 *                                                    pCurs_X->pElement=pCursorPos[i_U32];
					 *                                            }
					 *                                    }
					 *                                    Rts_U32=BOFRAMDB_ERROR_LOCK;
					 *                            }
					 */
				}

				Bof_UnlockMutex(mMtx_X);
			}
			else
			{
				Rts_U32 = BOF_ERR_NO_ERROR;
				mDbRamStat_X.NbUpdateCancelled_U32++;
			}
		}
	}

	// BOFDBG_OUTPUT_0(DBG_FS,Rts_U32,"UpdateElement Rts %X\r\n",Rts_U32);
	return Rts_U32;
}


template<typename KeyType>
int32_t BofRamDb<KeyType>::CheckDb(bool _ExtendedTest_B)
{
	uint32_t i_U32;

	uint32_t NbNode_U32;

	uint32_t Total_U32;

	uint32_t NxtFree_U32;

	uint32_t j_U32;

	uint32_t Sts_U32;

	uint32_t Position_U32;

	uint32_t Nxt_U32;
	int32_t Rts_S32 = 1;
	BOF_RAM_DB_CURSOR *pCursor_X;
	void *Cursor_h;
	KeyType Elem;

	BofAvlNode<KeyType> *pElement;

	NbNode_U32 = 0;

	for (i_U32 = 0; i_U32 < mNbIndex_U32; i_U32++)
	{
		// BOFDBG_OUTPUT_0(DBG_DB,0,"Index %d\r\n",i_U32);
		if (mppRamDbTree[i_U32])
		{
			Rts_S32 *= mppRamDbTree[i_U32]->Check(&NbNode_U32);
		}

		// BOFDBG_OUTPUT_0(DBG_DB,0,"-------------------------------------------------------------\r\n");
	}

	// Check number of record
	Total_U32 = 0;

	if (mppRamDbTree[0])
	{
		Total_U32 = mppRamDbTree[0]->GetNbNode();
	}

	if (NbNode_U32 != (mNbIndex_U32 * Total_U32))
	{
// BOFDBG_OUTPUT_0(DBG_DB, 0, "%d index * %d node != %d total node\r\n", mNbIndex_U32, Total_U32, NbNode_U32);
		Rts_S32 = 0;
	}

	if (Total_U32 != mNbRecord_U32)
	{
// BOFDBG_OUTPUT_0(DBG_DB, 0, "%d node per tree != %d total record\r\n", Total_U32, mNbRecord_U32);
		Rts_S32 = 0;
	}

	for (i_U32 = 1; i_U32 < mNbIndex_U32; i_U32++)
	{
		if (mppRamDbTree[i_U32])
		{
			if (mppRamDbTree[i_U32]->GetNbNode() != Total_U32)
			{
// BOFDBG_OUTPUT_0(DBG_DB,0,"Index %d has %d records instead of %d\r\n",i_U32,mppRamDbTree[i_U32]->GetNbNode(),Total_U32);
				Rts_S32 = 0;
			}
		}
	}

	// Check cursor
	Total_U32 = 0;
	pCursor_X = mpCursorState_X;

	for (i_U32 = 0; i_U32 < BOFRAMDB_CURSOR_MAX; i_U32++, pCursor_X++)
	{
		if (pCursor_X->MagicNumber_U32 == BOFRAMDB_CURSOR_MAGICNUMBER)
		{
			Total_U32++;
		}
	}

	Total_U32 = BOFRAMDB_CURSOR_MAX - Total_U32;

	if (Total_U32 != mNbFreeCursor_U32)
	{
// BOFDBG_OUTPUT_0(DBG_DB, 0, "%d free cursor detected instead of %d\r\n", Total_U32, mNbFreeCursor_U32);
		Rts_S32 = 0;
	}

	if (_ExtendedTest_B)
	{
		// Check free list
		Total_U32 = 0;
		NxtFree_U32 = mNextRamDbFreeElement_U32;

		for (i_U32 = 0; i_U32 < mNbMaxElement_U32; i_U32++)
		{
			if (NxtFree_U32 != 0xFFFFFFFF)
			{
				if (NxtFree_U32 < mNbMaxElement_U32)
				{
					Nxt_U32 = mpRamDbFreeElementList_U32[NxtFree_U32];

					for (j_U32 = 0; j_U32 < mNbMaxElement_U32; j_U32++)
					{
						if (Nxt_U32 != 0xFFFFFFFF)
						{
							if (NxtFree_U32 == Nxt_U32)
							{
								break;
							}
						}
						else
						{
							break;
						}

						Nxt_U32 = mpRamDbFreeElementList_U32[Nxt_U32];
					}

					if (NxtFree_U32 == Nxt_U32)
					{
// BOFDBG_OUTPUT_0(DBG_DB, 0, "Free entry %d is identical to entry %d (%d)\r\n", i_U32, j_U32, NxtFree_U32);
						Rts_S32 = 0;
						break;
					}
					else
					{
						NxtFree_U32 = mpRamDbFreeElementList_U32[NxtFree_U32];
						Total_U32++;
					}
				}
				else
				{
// BOFDBG_OUTPUT_0(DBG_DB,0,"Free entry %d contains index %d which is greater than %d\r\n",i_U32,NxtFree_U32,mNbMaxElement_U32);
					Rts_S32 = 0;
					break;
				}
			}
			else
			{
				break;
			}
		}

		Total_U32 = mNbMaxElement_U32 - Total_U32;

		if (Total_U32 != mppRamDbTree[0]->GetNbNode())           // All index have the same number of record normally
		{
// BOFDBG_OUTPUT_0(DBG_DB,0,"Free list contains %d used entry which is different of the current number of record %d\r\n", Total_U32, mppRamDbTree[0]->GetNbNode());
			Rts_S32 = 0;
		}
	}

	if (_ExtendedTest_B)
	{
		// no record can be located in a free entry
		Total_U32 = 0;
		Sts_U32 = GetCursor(0, &Cursor_h);
		pCursor_X = (BOF_RAM_DB_CURSOR *) Cursor_h;

		if (Sts_U32 != BOF_ERR_NO_ERROR)
		{
// BOFDBG_OUTPUT_0(DBG_DB, 0, "'GetCursor' failed %d\r\n", Sts_U32);
			Rts_S32 = 0;
		}
		else
		{
			Sts_U32 = GetFirstElement(Cursor_h, &Elem);

			if (Sts_U32 != BOF_ERR_NO_ERROR)
			{
				if (Sts_U32 != BOF_ERR_EMPTY)
				{
// BOFDBG_OUTPUT_0(DBG_DB, 0, "'GetFirstElement' failed %d\r\n", Sts_U32);
					Rts_S32 = 0;
				}
			}
			else
			{
				for (i_U32 = 0; i_U32 < mNbMaxElement_U32; i_U32++) // All index have the same number of record normally
				{
					pElement = (BofAvlNode<KeyType> *) pCursor_X->pElement;

// !!!64bits!!!
					Position_U32 = (uint32_t) (pElement->GetData() - mpElementList);

					if (Position_U32 < mNbMaxElement_U32)
					{
						NxtFree_U32 = mNextRamDbFreeElement_U32;

						for (j_U32 = 0; j_U32 < mNbMaxElement_U32; j_U32++)
						{
							if (NxtFree_U32 != 0xFFFFFFFF)
							{
								if (NxtFree_U32 == Position_U32)
								{
// BOFDBG_OUTPUT_0(DBG_DB,0,"Record (%d) is stored in a free entry %d (max is %d)\r\n",i_U32,j_U32, mNbMaxElement_U32);
									Rts_S32 = 0;
									break;
								}

								NxtFree_U32 = mpRamDbFreeElementList_U32[NxtFree_U32];
							}
							else
							{
								j_U32 = mNbMaxElement_U32;
							}
						}

						if (j_U32 < mNbMaxElement_U32)
						{
							break;
						}
					}
					else
					{
// BOFDBG_OUTPUT_0(DBG_DB,0,"Record (%d) is located outside of the free list %d (max is %d)\r\n", i_U32, Position_U32,mNbMaxElement_U32);
						Rts_S32 = 0;
						break;
					}

					Total_U32++;

					Sts_U32 = GetNextElement(Cursor_h, &Elem);

					if (Sts_U32 == BOF_ERR_EOF)
					{
						break;
					}

					if (Sts_U32 != BOF_ERR_NO_ERROR)
					{
// BOFDBG_OUTPUT_0(DBG_DB, 0, "'GetNextElement' failed %d\r\n", Sts_U32);
						Rts_S32 = 0;
						break;
					}
				}

				if (Total_U32 != mppRamDbTree[0]->GetNbNode()) // All index have the same number of record normally
				{
// BOFDBG_OUTPUT_0(DBG_DB,0,"Tree contains %d record which is different of the current number of record %d\r\n",Total_U32,mppRamDbTree[0]->GetNbNode());
					Rts_S32 = 0;
				}
			}

			Sts_U32 = FreeCursor(&Cursor_h);

			if (Sts_U32 != BOF_ERR_NO_ERROR)
			{
// BOFDBG_OUTPUT_0(DBG_DB, 0, "'FreeCursor' failed %d\r\n", Sts_U32);
				Rts_S32 = 0;
			}
		}
	}

	return Rts_S32;
}


template<typename KeyType>
uint32_t BofRamDb<KeyType>::DumpDatabase(const char *_pTitle_c, uint32_t _Flag_U32, uint32_t *_pNbMaxChar_U32, char *_pBuffer_c)
{
	uint32_t i_U32, Rts_U32, Remain_U32, Mask_U32, Sts_U32;
	bool AtLeastOne_B;

	Rts_U32 = 0;
	(void) Sts_U32;
	if ((_pNbMaxChar_U32)
	    && (_pBuffer_c)
		)
	{
		Remain_U32 = (*_pNbMaxChar_U32 - 2);              // nullptr terminating + paranoid
		//	Sts_U32      = 0;

		if (_pTitle_c)
		{
			DBG_INSERTSTRING(Rts_U32, snprintf(&_pBuffer_c[Rts_U32], Remain_U32, "%s", _pTitle_c), Remain_U32, Sts_U32);
		}
		AtLeastOne_B = false;

		for (Mask_U32 = 1, i_U32 = 0; i_U32 < mNbIndex_U32; i_U32++, Mask_U32 <<= 1)
		{
			if (Mask_U32 & _Flag_U32)
			{
				if ((Remain_U32)
				    && (AtLeastOne_B)
					)
				{
					DBG_INSERTSTRING(Rts_U32, snprintf(&_pBuffer_c[Rts_U32], Remain_U32, "-------------------------------------------------------------\r\n"), Remain_U32, Sts_U32);
				}

				if (Remain_U32)
				{
					AtLeastOne_B = true;
					DBG_INSERTSTRING(Rts_U32, snprintf(&_pBuffer_c[Rts_U32], Remain_U32, "Db 0x%p NbElem %d Index %d\r\n", static_cast<void *>(this), GetNbRecord(), i_U32), Remain_U32, Sts_U32);
					if (Remain_U32)
					{
						if (mppRamDbTree[i_U32])
						{
							*_pNbMaxChar_U32 = Remain_U32;
							Rts_U32 += mppRamDbTree[i_U32]->DumpTree(_pNbMaxChar_U32, &_pBuffer_c[Rts_U32]);
							Remain_U32 = *_pNbMaxChar_U32;
						}
					}
				}
			}

			if (!Remain_U32)
			{
				break;
			}
		}
	}
	return Rts_U32;
}
END_BOF_NAMESPACE()