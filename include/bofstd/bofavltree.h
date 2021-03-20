/*$T Bof/BofAvlTree.h GC 1.140 02/11/08 12:15:58 */

/*
 * File      : BofAvlTree.h
 *
 * Project   : Bof
 *
 * Package   : Bog-Include
 *
 * Company   : Sci
 *
 * Author    : Bernard HARMEL: b.harmel@gmail.com
 *
 * Purpose   : This is the definition of the BofAvlTree template
 *
 * Copyright : (C) Sci
 *
 * Version History:
 * V 1.00  Wed Sep 6 2006  BHA : Initial release
 */
#pragma once

/*** Include *****************************************************************/
#include <stdio.h>
#include <cstdint>

BEGIN_BOF_NAMESPACE()

/*** Define *****************************************************************/
#define DBG_INSERTSTRING(Index, snprintf, Remain, Rts)    { uint32_t NbChar = snprintf; Rts = 0; if (NbChar > 0) { Remain -= NbChar; Index += NbChar; } else{ Remain = 0; }  if (! Remain) { Rts = (uint32_t)-1; } }

/*** Enum ********************************************************************/
typedef enum
{
		LTREE = 0,
		KEY,
		RTREE,
		AVLTRAVERSALORDER_ALIGNON32BITS = 0xFFFFFFFF
} AVLTRAVERSALORDER;

/*** Structures *************************************************************/
/*** Template class **********************************************************/

/*!
 * Class BofAvlTree is a simple container object to "house" an BofAvlNode
 * that represents the root-node of and BofAvlTree. Most of the member
 * functions simply delegate to the root BofAvlNode.
 */
template<typename KeyType>
class BofAvlTree
{
private:
		BofAvlNode <KeyType> *mpRoot;       /*! The root of the tree*/
		uint32_t mIndex_U32;
		uint32_t mNbNode_U32;
		uint32_t mNbMaxElement_U32;
		BofAvlNode <KeyType> *mpNextRamDbFreeNode;
		BofAvlNode <KeyType> *mpNodeList;
private:

		// Disallow copying and assingment
		BofAvlTree(const BofAvlTree<KeyType> &) = delete;

		BofAvlTree &operator=(const BofAvlTree<KeyType> &) = delete;

public:
		BofAvlTree(uint32_t _NbMaxElement_U32, uint32_t _Index_U32, uint32_t *_pErrorCode_U32);

		virtual ~BofAvlTree();

		/*!
		 * See if the tree is empty
		 */
		bool IsEmpty()
		{ return mNbNode_U32 == 0; }

		bool IsFull()
		{ return mNbNode_U32 == mNbMaxElement_U32; }

		void Clear()
		{
			uint32_t i_U32;

			mNbNode_U32 = 0;
			mpRoot = nullptr;

			if (mpNodeList)
			{
				// Parent member of node is used to keep track of free avl node and for used node it keep parent node value
				mpNextRamDbFreeNode = mpNodeList;

				for (i_U32 = 0; i_U32 < mNbMaxElement_U32; i_U32++)
				{
					mpNodeList[i_U32].Clear();
					mpNodeList[i_U32].SetParent(&mpNodeList[i_U32 + 1]);
				}
				mpNodeList[mNbMaxElement_U32 - 1].SetParent(nullptr);
			}
		}

		uint32_t GetNbNode()
		{ return mNbNode_U32; }

		BofAvlNode <KeyType> *GetNextFreeNode()
		{
			BofAvlNode<KeyType> *pRts = mpNextRamDbFreeNode;

			if (pRts)
			{
				mpNextRamDbFreeNode = mpNextRamDbFreeNode->GetParent();
			}

			return pRts;
		}

		void SetNextFreeNode(BofAvlNode <KeyType> *_pNode)
		{
			_pNode->SetParent(mpNextRamDbFreeNode);
			mpNextRamDbFreeNode = _pNode;
		}

		BofAvlNode <KeyType> *Search(KeyType *_pKey, BOFCMP _Cmp_E)
		{
			return BofAvlNode<KeyType>::Search(mIndex_U32, _pKey, mpRoot, _Cmp_E);
		}

		BofAvlNode <KeyType> *Insert(KeyType *_pKey)
		{
			BofAvlNode<KeyType> *pRts = BofAvlNode<KeyType>::Insert(this, _pKey, mpRoot);

			if (!pRts)
			{
				mNbNode_U32++;
			}

			return pRts;
		}

		KeyType *Delete(KeyType *_pKey, BOFCMP _Cmp_E)
		{
			KeyType *pRts = BofAvlNode<KeyType>::Delete(this, _pKey, mpRoot, _Cmp_E);

			if (pRts)
			{
				mNbNode_U32--;
			}

			return pRts;
		}

		uint32_t GetIndex() const
		{
			return mIndex_U32;
		}

		BofAvlNode <KeyType> *GetFirst();

		BofAvlNode <KeyType> *GetLast();

		BofAvlNode <KeyType> *GetNext(BofAvlNode <KeyType> *_pNode);

		BofAvlNode <KeyType> *GetPrevious(BofAvlNode <KeyType> *_pNode);

		int32_t Check(uint32_t *_pNbNode_U32) const;

		uint32_t DumpTree(uint32_t *_pNbMaxChar_U32, char *_pBuffer_c);
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
BofAvlTree<KeyType>::BofAvlTree(uint32_t _NbMaxElement_U32, uint32_t _Index_U32, uint32_t *_pErrorCode_U32)
{
	uint32_t i_U32, Rts_U32 = (uint32_t) BOF_ERR_EINVAL;

	mpRoot = nullptr;
	mIndex_U32 = _Index_U32;
	mNbNode_U32 = 0;
	mNbMaxElement_U32 = _NbMaxElement_U32;
	mpNextRamDbFreeNode = nullptr;

	if (_NbMaxElement_U32)
	{
		Rts_U32 = BOF_ERR_ENOMEM;
		mpNodeList = new BofAvlNode<KeyType>[mNbMaxElement_U32];

		if (mpNodeList)
		{
			// Parent member of node is used to keep track of free avl node and for used node it keep parent node value
			mpNextRamDbFreeNode = mpNodeList;

			for (i_U32 = 0; i_U32 < mNbMaxElement_U32; i_U32++)
			{
				mpNodeList[i_U32].SetParent(&mpNodeList[i_U32 + 1]);
			}
			mpNodeList[mNbMaxElement_U32 - 1].SetParent(nullptr);
			Rts_U32 = BOF_ERR_NO_ERROR;
		}
	}

	if (_pErrorCode_U32)
	{
		*_pErrorCode_U32 = Rts_U32;
	}
}


template<typename KeyType>
BofAvlTree<KeyType>::~BofAvlTree()
{
	BOF_SAFE_DELETE_ARRAY(mpNodeList);
}


template<typename KeyType>
static void Dump(uint32_t _Index_U32, AVLTRAVERSALORDER Order_E, const BofAvlNode <KeyType> *_pNode, int32_t _Level_S32, uint32_t *_pNbMaxChar_U32, char *_pBuffer_c, uint32_t *_pNbCharWritten_U32)
{
	uint32_t Len_U32, Remain_U32, Sts_U32;
	BOFTYPE KeyType_E;
	char pVal_c[2048], pIndent_c[2048];
	(void) Sts_U32;

	if ((_pNbMaxChar_U32)
	    && (_pNbCharWritten_U32)
	    && (_pBuffer_c)
		)
	{
		Remain_U32 = (*_pNbMaxChar_U32 - 2); // nullptr terminating + paranoid
		if (Remain_U32)
		{
			Len_U32 = (_Level_S32 * 16) + 1;

			if (Len_U32 < sizeof(pIndent_c))
			{
				memset(pIndent_c, ' ', Len_U32);
				pIndent_c[Len_U32] = 0;

				if ((Order_E == LTREE) && (_pNode->GetSubtree(LEFTSUBTREE) == nullptr))
				{
					DBG_INSERTSTRING(*_pNbCharWritten_U32, snprintf(&_pBuffer_c[*_pNbCharWritten_U32], Remain_U32, "%s                ->nullptr\r\n", pIndent_c), Remain_U32, Sts_U32);
				}

				if (Remain_U32)
				{
					if (Order_E == KEY)
					{
						_pNode->GetKey(_Index_U32, &KeyType_E, sizeof(pVal_c), pVal_c);
						DBG_INSERTSTRING(*_pNbCharWritten_U32, snprintf(&_pBuffer_c[*_pNbCharWritten_U32], Remain_U32, "%s%s (%d)\r\n", pIndent_c, pVal_c, _pNode->GetBalance()), Remain_U32, Sts_U32);

						if (_pNode->GetSubtree(PARENTSUBTREE))
						{
							_pNode->GetSubtree(PARENTSUBTREE)->GetKey(_Index_U32, &KeyType_E, sizeof(pVal_c), pVal_c);
							DBG_INSERTSTRING(*_pNbCharWritten_U32, snprintf(&_pBuffer_c[*_pNbCharWritten_U32], Remain_U32, "%sP: %s\r\n", pIndent_c, pVal_c), Remain_U32, Sts_U32);
						}
						else
						{
							DBG_INSERTSTRING(*_pNbCharWritten_U32, snprintf(&_pBuffer_c[*_pNbCharWritten_U32], Remain_U32, "%sP: nullptr\r\n", pIndent_c), Remain_U32, Sts_U32);
						}
					}
				}

				if (Remain_U32)
				{
					if ((Order_E == RTREE) && (_pNode->GetSubtree(RIGHTSUBTREE) == nullptr))
					{
						DBG_INSERTSTRING(*_pNbCharWritten_U32, snprintf(&_pBuffer_c[*_pNbCharWritten_U32], Remain_U32, "%s                ->nullptr\r\n", pIndent_c), Remain_U32, Sts_U32);
					}
				}
			}
		}
	}
}


template<typename KeyType>
static void Dump(uint32_t _Index_U32, const BofAvlNode <KeyType> *_pNode, int32_t _Level_S32, uint32_t *_pNbMaxChar_U32, char *_pBuffer_c, uint32_t *_pNbCharWritten_U32)
{
	uint32_t Remain_U32, Sts_U32;

	(void) Sts_U32;

	if ((_pNbMaxChar_U32)
	    && (_pBuffer_c)
	    && (_pNbCharWritten_U32)
		)
	{
		Remain_U32 = (*_pNbMaxChar_U32 - 2); // nullptr terminating + paranoid
		if (Remain_U32)
		{
			if (_pNode == nullptr)
			{
				DBG_INSERTSTRING(*_pNbCharWritten_U32, snprintf(&_pBuffer_c[*_pNbCharWritten_U32], Remain_U32, "***EMPTY TREE***\r\n"), Remain_U32, Sts_U32);
			}
			else
			{
				Dump(_Index_U32, RTREE, _pNode, _Level_S32, _pNbMaxChar_U32, _pBuffer_c, _pNbCharWritten_U32);
				Remain_U32 = *_pNbMaxChar_U32;

				if (Remain_U32)
				{
					if (_pNode->GetSubtree(RIGHTSUBTREE) != nullptr)
					{
						Dump(_Index_U32, _pNode->GetSubtree(RIGHTSUBTREE), _Level_S32 + 1, _pNbMaxChar_U32, _pBuffer_c, _pNbCharWritten_U32);
						Remain_U32 = *_pNbMaxChar_U32;
					}

					if (Remain_U32)
					{
						Dump(_Index_U32, KEY, _pNode, _Level_S32, _pNbMaxChar_U32, _pBuffer_c, _pNbCharWritten_U32);
						Remain_U32 = *_pNbMaxChar_U32;

						if (Remain_U32)
						{
							if (_pNode->GetSubtree(LEFTSUBTREE) != nullptr)
							{
								Dump(_Index_U32, _pNode->GetSubtree(LEFTSUBTREE), _Level_S32 + 1, _pNbMaxChar_U32, _pBuffer_c, _pNbCharWritten_U32);
								Remain_U32 = *_pNbMaxChar_U32;
							}

							if (Remain_U32)
							{
								Dump(_Index_U32, LTREE, _pNode, _Level_S32, _pNbMaxChar_U32, _pBuffer_c, _pNbCharWritten_U32);
							}
						}
					}
				}
			}
		}                                  // if non-empty tree
	}
}


// Dump the tree to the given output stream
template<typename KeyType>
uint32_t BofAvlTree<KeyType>::DumpTree(uint32_t *_pNbMaxChar_U32, char *_pBuffer_c)
{
	uint32_t Rts_U32 = 0;

	if ((_pNbMaxChar_U32)
	    && (_pBuffer_c)
		)
	{
		Dump(mIndex_U32, mpRoot, 0, _pNbMaxChar_U32, _pBuffer_c, &Rts_U32);
	}
	return Rts_U32;
}


template<typename KeyType>
BofAvlNode <KeyType> *BofAvlTree<KeyType>::GetFirst()
{
	BofAvlNode<KeyType> *pRts = nullptr;

	if (mpRoot)
	{
		pRts = mpRoot;

		while (pRts->GetSubtree(LEFTSUBTREE))
		{
			pRts = pRts->GetSubtree(LEFTSUBTREE);
		}
	}

	return pRts;
}


template<typename KeyType>
BofAvlNode <KeyType> *BofAvlTree<KeyType>::GetLast()
{
	BofAvlNode<KeyType> *pRts = nullptr;

	if (mpRoot)
	{
		pRts = mpRoot;

		while (pRts->GetSubtree(RIGHTSUBTREE))
		{
			pRts = pRts->GetSubtree(RIGHTSUBTREE);
		}
	}

	return pRts;
}


template<typename KeyType>
BofAvlNode <KeyType> *BofAvlTree<KeyType>::GetNext(BofAvlNode <KeyType> *_pNode)
{
	BofAvlNode<KeyType> *pRts = nullptr;
	bool SearchHead_B;

	if (_pNode)
	{
		if (!_pNode->GetSubtree(RIGHTSUBTREE))
		{
			SearchHead_B = true;

			do
			{
				if (!_pNode->GetSubtree(PARENTSUBTREE))
				{
					SearchHead_B = false;
				}
				else
				{
					if (_pNode->GetSubtree(PARENTSUBTREE)->Compare(mIndex_U32, _pNode->GetData(), BOF_CMP_EQUAL) == BOF_CMP_LESS)
					{
						pRts = _pNode->GetSubtree(PARENTSUBTREE);
						SearchHead_B = false;
					}
					else
					{
						_pNode = _pNode->GetSubtree(PARENTSUBTREE);
					}
				}
			} while (SearchHead_B);
		}
		else
		{
			_pNode = _pNode->GetSubtree(RIGHTSUBTREE);

			while (_pNode->GetSubtree(LEFTSUBTREE))
			{
				_pNode = _pNode->GetSubtree(LEFTSUBTREE);
			}

			pRts = _pNode;
		}
	}

	return pRts;
}


template<typename KeyType>
BofAvlNode <KeyType> *BofAvlTree<KeyType>::GetPrevious(BofAvlNode <KeyType> *_pNode)
{
	BofAvlNode<KeyType> *pRts = nullptr;
	bool SearchHead_B;

	if (_pNode)
	{
		if (!_pNode->GetSubtree(LEFTSUBTREE))
		{
			SearchHead_B = true;

			do
			{
				if (!_pNode->GetSubtree(PARENTSUBTREE))
				{
					SearchHead_B = false;
				}
				else
				{
					if (_pNode->GetSubtree(PARENTSUBTREE)->Compare(mIndex_U32, _pNode->GetData(), BOF_CMP_EQUAL) == BOF_CMP_GREATER)
					{
						pRts = _pNode->GetSubtree(PARENTSUBTREE);
						SearchHead_B = false;
					}
					else
					{
						_pNode = _pNode->GetSubtree(PARENTSUBTREE);
					}
				}
			} while (SearchHead_B);
		}
		else
		{
			_pNode = _pNode->GetSubtree(LEFTSUBTREE);

			while (_pNode->GetSubtree(RIGHTSUBTREE))
			{
				_pNode = _pNode->GetSubtree(RIGHTSUBTREE);
			}

			pRts = _pNode;
		}
	}

	return pRts;
}


template<typename KeyType>
int32_t BofAvlTree<KeyType>::Check(uint32_t *_pNbNode_U32) const
{
	int32_t Rts_S32 = 1;
	uint32_t i_U32, Total_U32;

	BofAvlNode<KeyType> *pNodeList;

	if (mpRoot)
	{
		(*_pNbNode_U32)++;
		Rts_S32 = mpRoot->Check(mIndex_U32, _pNbNode_U32);

		// Check free list
		Total_U32 = 0;
		pNodeList = mpNextRamDbFreeNode;

		for (i_U32 = 0; i_U32 < mNbMaxElement_U32; i_U32++)
		{
			if (pNodeList)
			{
				pNodeList = pNodeList->GetParent();
				Total_U32++;
			}
		}

		Total_U32 = mNbMaxElement_U32 - Total_U32;

		if (Total_U32 != mNbNode_U32)
		{
// BOFDBG_OUTPUT_0(DBG_DB,0,"Node free list contains %d used entry which is different of the current number of node %d\r\n",Total_U32,mNbNode_U32);
			Rts_S32 = 0;
		}
	}

	return Rts_S32;
}

END_BOF_NAMESPACE()