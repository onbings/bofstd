/*!
Copyright (c) 2008, Onbings All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
PURPOSE.

Remarks

  Name:              boflist.h
  Author:            Bernard HARMEL: onbings@dscloud.me

Summary:

  The list object definition

History:
	V 1.01  May 2017 BHA:Bofstd
	*/
#pragma once

/*** Include ***********************************************************************************************************************/

//#include <string.h>
#include <bofstd/bofstd.h>
#include <bofstd/bofsystem.h>
#include <bofstd/bofperformance.h>
#include <string.h>
/*** Globals ***********************************************************************************************************************/

/*** Defines ***********************************************************************************************************************/

// Uncomment this define  
// to profile this class
//#define PROFILE_CLIST

#if !defined(NDEBUG)
#define CHECK_LIST_INTEGRITY
#endif


/*** Enums *************************************************************************************************************************/

/*!
Summary
  The various functions to profile
*/
enum
{
		PROFILE_CLIST_ADD_AFTER,
		PROFILE_CLIST_ADD_BEFORE,
		PROFILE_CLIST_ADD_FIRST,
		PROFILE_CLIST_ADD_LAST,
		PROFILE_CLIST_REMOVE,
		PROFILE_CLIST_FIND,
		PROFILE_CLIST_FIND_LAST,
		PROFILE_CLIST_CLEAR,
		PROFILE_CLIST_SORT,
		PROFILE_CLIST_MAX_ITEM
};

/*** Structures ********************************************************************************************************************/

/*** Constants *********************************************************************************************************************/

/*** Prototypes ********************************************************************************************************************/

/*** Classes ***********************************************************************************************************************/

#ifdef  __cplusplus
BEGIN_BOF_NAMESPACE()
template<typename Element>
class BofList;

/*!
Summary 
  The class representing a list node 
*/
template<typename Element>
class BofListNode
{
		friend class BofList<Element>;

public :

		BofListNode();

		virtual ~BofListNode();

		Element Value;  /*! The user value */

		class BofListNode<Element> *pPrev_O;    /*! The next node */
		class BofListNode<Element> *pNext_O;    /*! The previous node */
		class BofList<Element> *pParent_O;  /*! The parent list */

private :

};

/*!
Description
  The class constructor
  
Parameters
  _pElement   - The pointer to the element data
  _pPrev_O    - The pointer to the previous node
  _pNext_O    - The pointer to the next node 
  _pParent_O  - The pointer to the parent list
  
Returns
  Nothing

Remarks
  None

See also
  Nothing
*/
template<typename Element>
BofListNode<Element>::BofListNode()
{
	memset(&Value, 0x00, sizeof(Element));

	pPrev_O = NULL;
	pNext_O = NULL;
	pParent_O = NULL;
}

/*!
Description
  The class destructor
  
Parameters
  None
  
Returns
  Nothing

Remarks
  None

See also
  Nothing
*/
template<typename Element>
BofListNode<Element>::~BofListNode()
{

}

/*!
Summary
  The class representing the list
*/
template<typename Element>
class BofList
{
public :
		BofList(U32 _NbMaxElements_U32, bool _ThreadSafe_B = true, class BofList<Element> *_pNodePool_O = NULL);

		virtual ~BofList();

		bool AddFirst(BofListNode<Element> *_pNode_O);

		bool AddFirst(Element &_rElement);

		bool AddFirst(Element *_pElement);

		bool AddLast(BofListNode<Element> *_pNode_O);

		bool AddLast(Element &_rElement);

		bool AddLast(Element *_pElement);

		bool AddAfter(BofListNode<Element> *_pExistingNode_O, BofListNode<Element> *_pNewNode_O);

		bool AddAfter(BofListNode<Element> *_pExistingNode_O, Element *_pElement);

		bool AddBefore(BofListNode<Element> *_pExistingNode_O, BofListNode<Element> *_pNewNode_O);

		bool AddBefore(BofListNode<Element> *_pExistingNode_O, Element *_pElement);

		bool Remove(BofListNode<Element> *_pNode_O);

		bool RemoveFirst(Element *_pElement = NULL);

		bool RemoveLast(Element *_pElement = NULL);

		bool Remove(Element *_pElement, int (*_pMethod)(const void *, const void *) = NULL);

		bool Contains(Element *_pElement, int (*_pMethod)(const void *, const void *) = NULL);

		bool Sort(int (*_pMethod)(const void *, const void *) = NULL);

		BofListNode<Element> *Find(Element *_pElement, int (*_pMethod)(const void *, const void *) = NULL);

		BofListNode<Element> *FindLast(Element *_pElement, int (*_pMethod)(const void *, const void *) = NULL);

		BofListNode<Element> *GetFirst();

		BofListNode<Element> *GetLast();

		bool Clear();

		U32 GetCount();

		U32 GetCapacity();

		bool IsFull();

		bool IsEmpty();

		bool GetProfilingStats(U32 _ItemId_U32, BOF_STAT_VARIABLE *_pStats_X);

		U64 GetMemoryUsage();

		bool CheckConsistency();

		void Lock();

		void Unlock();

protected :


		BofListNode<Element> *GetFreeNode(Element *_pElement = NULL);

		BofListNode<Element> *GetFreeNode(BofListNode<Element> *_pNode_O);

		bool SetFreeNode(BofListNode<Element> *_pNode_O);

		void ListSort(int (*_pCompare)(const void *, const void *), bool _IsCircular_B, bool _IsDouble_B);

private :

		U32 mNbElements_U32;
		U32 mNbMaxElements_U32;
		BofListNode<Element> *mpNodes_O;
		BofListNode<Element> *mpHead_O;
		BofListNode<Element> *mpQueue_O;
		BofListNode<Element> *mpFreeNodesHead_O;
		BofListNode<Element> *mpFreeNodesQueue_O;
		BOF_MUTEX mMtx_X;  // *mpCriticalSection_O;
		class BofList<Element> *mpNodePoolMgr_O;

		BofProfiler *mpProfiler_O;
};

/*!
Description
  The class constructor
  
  REMARKS :
  
  When providing an external node pool, the CList object assume there
  is enough room to store at least _NbMaxElements_U32 elements. Moreover, in such 
  a case, no memory cleanup is performed when the CList object is destroyed. It's still the
  caller responsibility to free the provided node pool. 
  
  In case no external node pool is provided, the CList object allocates internally the memory 
  it needs. Consequently, in that case, the memory is released by the CList object when destroyed.
    
Parameters
  _NbMaxElements_U32  - The maximum number of elements in the list
  _ThreadSafe_B       - Indicates if the list should be aware of multithreading issues

Returns
  Nothing

Remarks
  None

See also
  Nothing
*/
template<typename Element>
BofList<Element>::BofList(U32 _NbMaxElements_U32, bool _ThreadSafe_B /* = TRUE */, class BofList<Element> *_pNodePool_O /* = NULL */)
{
	U32 I_U32 = 0;

#if defined(CHECK_LIST_INTEGRITY)
	BOF_ASSERT(_NbMaxElements_U32 > 0);
#endif

	// Create a critical section if needed
	if (_ThreadSafe_B)
	{
		//BOFERR Sts_E=
		Bof_CreateMutex("BofList", true, mMtx_X);
	}

	// Allocate the memory
	mNbMaxElements_U32 = _NbMaxElements_U32;

	// External memory provided
	if (_pNodePool_O != NULL)
	{
		mpNodes_O = NULL;
		mpNodePoolMgr_O = _pNodePool_O;
	}
		// Create memory internally
	else
	{
		mpNodes_O = new BofListNode<Element>[mNbMaxElements_U32];
		mpNodePoolMgr_O = NULL;

		// Initialize the items
		for (I_U32 = 0; I_U32 < mNbMaxElements_U32; I_U32++)
		{
			mpNodes_O[I_U32].pPrev_O = (I_U32 == 0) ? NULL : &mpNodes_O[I_U32 - 1];
			mpNodes_O[I_U32].pNext_O = (I_U32 == mNbMaxElements_U32 - 1) ? NULL : &mpNodes_O[I_U32 + 1];
			mpNodes_O[I_U32].pParent_O = NULL;
		}

		mpFreeNodesHead_O = &mpNodes_O[0];
		mpFreeNodesQueue_O = &mpNodes_O[mNbMaxElements_U32 - 1];
	}

	mpHead_O = NULL;
	mpQueue_O = NULL;
	mNbElements_U32 = 0;

#if defined(PROFILE_CLIST)

	mpProfiler_O = new CProfiler(PROFILE_CLIST_MAX_ITEM);

#else

	mpProfiler_O = NULL;

#endif
}

/*!
Description
  The class destructor
  
Parameters
  None

Returns
  Nothing

Remarks
  None

See also
  Nothing
*/
template<typename Element>
BofList<Element>::~BofList()
{
	if (mpNodes_O != NULL)
	{
		BOF_SAFE_DELETE_ARRAY(mpNodes_O);
	}
	Bof_DestroyMutex(mMtx_X);

	if (mpProfiler_O != NULL)
	{
		BOF_SAFE_DELETE(mpProfiler_O);
	}
}

/*!
Description
  This function locks the list in against concurrent 
  accesses in the context of the current thread
  
Parameters
  None

Returns
  Nothing

Remarks
  This function is only useful when 
  enumerating list nodes. All other
  operations are already protected

See also
  Nothing
*/
template<typename Element>
void BofList<Element>::Lock()
{
	//BOFERR Sts_E=
	Bof_LockMutex(mMtx_X);
}

/*!
Description
  This function unlocks the list in against concurrent 
  accesses in the context of the current thread
  
Parameters
  None

Returns
  Nothing

Remarks
  This function is only useful when 
  enumerating list nodes. All other
  operations are already protected

See also
  Nothing
*/
template<typename Element>
void BofList<Element>::Unlock()
{
	//BOFERR Sts_E = 
	Bof_UnlockMutex(mMtx_X);
}

/*!
Description
  This function inserts the new node after the specified existing one
  
Parameters
  _pExistingNode_O  - The pointer to the existing node in the list
  _pNewNode_O       - The pointer to the new node to insert in the list

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::AddAfter(BofListNode<Element> *_pExistingNode_O, BofListNode<Element> *_pNewNode_O)
{
	bool Ret_B = false;
	BofListNode<Element> *pNode_O;

#if defined(CHECK_LIST_INTEGRITY)
	BOF_ASSERT(_pExistingNode_O != NULL);
	BOF_ASSERT(_pNewNode_O      != NULL);
#endif

	if ((_pExistingNode_O != NULL) && (_pNewNode_O != NULL))
	{
		Lock();

#if defined(PROFILE_CLIST)
		BOF_ENTER_BENCH    (mpProfiler_O, PROFILE_CLIST_ADD_AFTER);
#endif

		// Make sure we are not messing with items from other lists
		if ((_pExistingNode_O->pParent_O == this) && (_pNewNode_O->pParent_O == NULL))
		{
			// The existing node is the queue
			if (_pExistingNode_O == mpQueue_O)
			{
				_pExistingNode_O->pNext_O = _pNewNode_O;
				_pNewNode_O->pPrev_O = _pExistingNode_O;
				_pNewNode_O->pNext_O = NULL;

				mpQueue_O = _pNewNode_O;
			}
				// The existing node is not the queue
			else
			{
				pNode_O = _pExistingNode_O->pNext_O;
				_pExistingNode_O->pNext_O = _pNewNode_O;
				_pNewNode_O->pPrev_O = _pExistingNode_O;
				_pNewNode_O->pNext_O = pNode_O;
				pNode_O->pPrev_O = _pNewNode_O;
			}

			// The new node belongs now to this list
			_pNewNode_O->pParent_O = this;

			mNbElements_U32++;

			Ret_B = true;
		}

#if defined(PROFILE_CLIST)
		BOF_LEAVE_BENCH    (mpProfiler_O, PROFILE_CLIST_ADD_AFTER);
#endif

		Unlock();
	}

	return Ret_B;
}

/*!
Description
  This function creates a new node with the specified value and inserts it after the specified existing one
  
Parameters
  _pExistingNode_O  - The pointer to the existing node in the list
  _pElement         - The pointer to the element value to give to the new node

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::AddAfter(BofListNode<Element> *_pExistingNode_O, Element *_pElement)
{
	bool Ret_B = false;
	BofListNode<Element> *pNode_O = NULL;

#if defined(CHECK_LIST_INTEGRITY)
	BOF_ASSERT(_pExistingNode_O != NULL);
	BOF_ASSERT(_pElement        != NULL);
#endif

	if ((_pExistingNode_O != NULL) && (_pElement != NULL))
	{
		Lock();

		pNode_O = GetFreeNode(_pElement);

		if (pNode_O != NULL)
		{
			Ret_B = AddAfter(_pExistingNode_O, pNode_O);
		}

		Unlock();
	}

	return Ret_B;
}

/*!
Description
  This function inserts the new node before the specified existing one
  
Parameters
  _pExistingNode_O  - The pointer to the existing node in the list
  _pNewNode_O       - The pointer to the new node to insert in the list

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::AddBefore(BofListNode<Element> *_pExistingNode_O, BofListNode<Element> *_pNewNode_O)
{
	bool Ret_B = false;
	BofListNode<Element> *pNode_O = NULL;

#if defined(CHECK_LIST_INTEGRITY)
	BOF_ASSERT(_pExistingNode_O != NULL);
	BOF_ASSERT(_pNewNode_O      != NULL);
#endif

	if ((_pExistingNode_O != NULL) && (_pNewNode_O != NULL))
	{
		Lock();

#if defined(PROFILE_CLIST)
		BOF_ENTER_BENCH    (mpProfiler_O, PROFILE_CLIST_ADD_BEFORE);
#endif

		// Make sure we are not messing with items from other lists
		if (_pExistingNode_O->pParent_O == this && _pNewNode_O->pParent_O == NULL)
		{
			// The existing node is the head
			if (_pExistingNode_O == mpHead_O)
			{
				_pExistingNode_O->pPrev_O = _pNewNode_O;
				_pNewNode_O->pNext_O = _pExistingNode_O;
				_pNewNode_O->pPrev_O = NULL;

				mpHead_O = _pNewNode_O;
			}
				// The existing node is not the head
			else
			{
				pNode_O = _pExistingNode_O->pPrev_O;
				_pExistingNode_O->pPrev_O = _pNewNode_O;
				_pNewNode_O->pNext_O = _pExistingNode_O;
				_pNewNode_O->pPrev_O = pNode_O;
				pNode_O->pNext_O = _pNewNode_O;
			}

			// The new node belongs now to this list
			_pNewNode_O->pParent_O = this;

#if defined(CHECK_LIST_INTEGRITY)
			BOF_ASSERT(mpHead_O->pParent_O == this);
#endif

			mNbElements_U32++;

			Ret_B = true;
		}

#if defined(PROFILE_CLIST)
		BOF_LEAVE_BENCH    (mpProfiler_O, PROFILE_CLIST_ADD_BEFORE);
#endif

		Unlock();
	}

	return Ret_B;
}

/*!
Description
  This function creates a new node with the specified value and inserts it before the specified existing one
  
Parameters
  _pExistingNode_O  - The pointer to the existing node in the list
  _pElement         - The pointer to the element value to give to the new node

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::AddBefore(BofListNode<Element> *_pExistingNode_O, Element *_pElement)
{
	bool Ret_B = false;
	BofListNode<Element> *pNode_O = NULL;

#if defined(CHECK_LIST_INTEGRITY)
	BOF_ASSERT(_pExistingNode_O != NULL);
	BOF_ASSERT(_pElement        != NULL);
#endif

	if ((_pExistingNode_O != NULL) && (_pElement != NULL))
	{
		Lock();

		pNode_O = GetFreeNode(_pElement);

		if (pNode_O != NULL)
		{
			Ret_B = AddBefore(_pExistingNode_O, pNode_O);
		}

		Unlock();
	}

	return Ret_B;
}

/*!
Description
  This function inserts the specified node at the head of the list
  
Parameters
  _pNode_O  - The node to insert

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::AddFirst(BofListNode<Element> *_pNode_O)
{
	bool Ret_B = false;

#if defined(CHECK_LIST_INTEGRITY)
	BOF_ASSERT(_pNode_O != NULL);
#endif

	if (_pNode_O != NULL)
	{
		Lock();

#if defined(PROFILE_CLIST)
		BOF_ENTER_BENCH    (mpProfiler_O, PROFILE_CLIST_ADD_FIRST);
#endif


		GetFreeNode(_pNode_O);

		// It's a free node
		if (_pNode_O->pParent_O == NULL)
		{
			// No item in the list
			if (mpHead_O == NULL)
			{
#if defined(CHECK_LIST_INTEGRITY)
				BOF_ASSERT(mNbElements_U32 == 0);
				BOF_ASSERT(mpQueue_O       == NULL);
#endif

				_pNode_O->pParent_O = this;
				_pNode_O->pNext_O = NULL;
				_pNode_O->pPrev_O = NULL;
				mpHead_O = _pNode_O;
				mpQueue_O = _pNode_O;
				mNbElements_U32 = 1;
				Ret_B = true;
			}
			else
			{
				Ret_B = AddBefore(mpHead_O, _pNode_O);
			}
		}

#if defined(PROFILE_CLIST)
		BOF_LEAVE_BENCH    (mpProfiler_O, PROFILE_CLIST_ADD_FIRST);
#endif

		Unlock();
	}

	return Ret_B;
}

/*!
Description
  This function creates a new node with the specified value and inserts it at the head of the list
  
Parameters
  _rElement - The reference to the element value to give to the new node

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::AddFirst(Element &_rElement)
{
	bool Ret_B = false;
	BofListNode<Element> *pNode_O = NULL;

	Lock();

	pNode_O = GetFreeNode(&_rElement);

	if (pNode_O != NULL)
	{
		Ret_B = AddFirst(pNode_O);
	}

	Unlock();

	return Ret_B;
}

/*!
Description
  This function creates a new node with the specified value and inserts it at the head of the list
  
Parameters
  _pElement - The pointer to the element value to give to the new node

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::AddFirst(Element *_pElement)
{
	bool Ret_B = false;
	BofListNode<Element> *pNode_O = NULL;

	Lock();

	pNode_O = GetFreeNode(_pElement);

	if (pNode_O != NULL)
	{
		Ret_B = AddFirst(pNode_O);
	}

	Unlock();

	return Ret_B;
}

/*!
Description
  This function inserts the specified node at the queue of the list
  
Parameters
  _pNode_O  - The node to insert

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::AddLast(BofListNode<Element> *_pNode_O)
{
	bool Ret_B = false;

#if defined(CHECK_LIST_INTEGRITY)
	BOF_ASSERT(_pNode_O != NULL);
#endif

	if (_pNode_O != NULL)
	{
		Lock();

#if defined(PROFILE_CLIST)
		BOF_ENTER_BENCH    (mpProfiler_O, PROFILE_CLIST_ADD_LAST);
#endif

		// It's a free node
		if (_pNode_O->pParent_O == NULL)
		{
			GetFreeNode(_pNode_O);

			// No item in the list
			if (mpQueue_O == NULL)
			{

#if defined(CHECK_LIST_INTEGRITY)
				BOF_ASSERT(mNbElements_U32 == 0);
				BOF_ASSERT(mpHead_O        == NULL);
#endif

				_pNode_O->pParent_O = this;
				_pNode_O->pNext_O = NULL;
				_pNode_O->pPrev_O = NULL;
				mpHead_O = _pNode_O;
				mpQueue_O = _pNode_O;
				mNbElements_U32 = 1;
				Ret_B = true;

#if defined(CHECK_LIST_INTEGRITY)
				BOF_ASSERT(mpHead_O->pParent_O == this);
#endif
			}
			else
			{
				Ret_B = AddAfter(mpQueue_O, _pNode_O);
			}
		}

#if defined(PROFILE_CLIST)
		BOF_LEAVE_BENCH    (mpProfiler_O, PROFILE_CLIST_ADD_LAST);
#endif

		Unlock();
	}

	return Ret_B;
}

/*!
Description
  This function creates a new node with the specified value and inserts it at the queue of the list
  
Parameters
  _rElement - The reference to the element value to give to the new node

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::AddLast(Element &_rElement)
{
	bool Ret_B = false;
	BofListNode<Element> *pNode_O = NULL;

	Lock();

	pNode_O = GetFreeNode(&_rElement);

	if (pNode_O != NULL)
	{
		Ret_B = AddLast(pNode_O);
	}

	Unlock();

	return Ret_B;
}

/*!
Description
  This function creates a new node with the specified value and inserts it at the queue of the list
  
Parameters
  _pElement - The pointer to the element value to give to the new node

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::AddLast(Element *_pElement)
{
	bool Ret_B = false;
	BofListNode<Element> *pNode_O = NULL;

	Lock();

	pNode_O = GetFreeNode(_pElement);

	if (pNode_O != NULL)
	{
		Ret_B = AddLast(pNode_O);
	}

	Unlock();

	return Ret_B;
}

/*!
Description
  This function removes the specified node from the list
  
Parameters
  _pNode_O - The pointer to the node to remove

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::Remove(BofListNode<Element> *_pNode_O)
{
	bool Ret_B = false;

#if defined(CHECK_LIST_INTEGRITY)
	BOF_ASSERT(_pNode_O != NULL);
#endif

	if (_pNode_O != NULL)
	{
		Lock();

#if defined(PROFILE_CLIST)
		BOF_ENTER_BENCH    (mpProfiler_O, PROFILE_CLIST_REMOVE);
#endif

		if (_pNode_O->pParent_O == this)
		{

#if defined(CHECK_LIST_INTEGRITY)
			BOF_ASSERT(mNbElements_U32 > 0);

			BOF_ASSERT(_pNode_O->pPrev_O != _pNode_O);
			BOF_ASSERT(_pNode_O->pNext_O != _pNode_O);
#endif

			// This is the only item in the list
			if ((_pNode_O == mpHead_O) && (_pNode_O == mpQueue_O))
			{
				mpHead_O = NULL;
				mpQueue_O = NULL;
			}
				// This is the head
			else if (_pNode_O == mpHead_O)
			{

#if defined(CHECK_LIST_INTEGRITY)
				BOF_ASSERT(_pNode_O->pNext_O != NULL);
#endif

				_pNode_O->pNext_O->pPrev_O = NULL;
				mpHead_O = _pNode_O->pNext_O;

				//BOF_ASSERT(mpHead_O->pParent_O == this);
				Ret_B = (mpHead_O->pParent_O == this);
			}
				// This is the queue
			else if (_pNode_O == mpQueue_O)
			{

#if defined(CHECK_LIST_INTEGRITY)
				BOF_ASSERT(_pNode_O->pPrev_O != NULL);
#endif

				_pNode_O->pPrev_O->pNext_O = NULL;
				mpQueue_O = _pNode_O->pPrev_O;
			}
				// It's a 'normal' node
			else
			{

#if defined(CHECK_LIST_INTEGRITY)
				BOF_ASSERT(_pNode_O->pPrev_O != NULL);
				BOF_ASSERT(_pNode_O->pNext_O != NULL);
#endif

				_pNode_O->pPrev_O->pNext_O = _pNode_O->pNext_O;
				_pNode_O->pNext_O->pPrev_O = _pNode_O->pPrev_O;
			}

			mNbElements_U32--;

			Ret_B = SetFreeNode(_pNode_O);
		}

#if defined(CHECK_LIST_INTEGRITY)
		BOF_ASSERT(_pNode_O->pParent_O == NULL);
#endif

#if defined(PROFILE_CLIST)
		BOF_LEAVE_BENCH    (mpProfiler_O, PROFILE_CLIST_REMOVE);
#endif

		Unlock();
	}

	return Ret_B;
}

/*!
Description
  This function removes the head element of the list
  
Parameters
  None

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::RemoveFirst(Element *_pElement /* = NULL */)
{
	bool Ret_B = false;

	Lock();

	if (mpHead_O != NULL)
	{
		if (_pElement)
		{
			*_pElement = mpHead_O->Value;
		}

		Ret_B = Remove(mpHead_O);
	}

	Unlock();

	return Ret_B;
}

/*!
Description
  This function removes the queue element of the list
  
Parameters
  None

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::RemoveLast(Element *_pElement /* = NULL */)
{
	bool Ret_B = false;

	Lock();

	if (mpQueue_O != NULL)
	{
		if (_pElement)
		{
			*_pElement = mpQueue_O->Value;
		}

		Ret_B = Remove(mpQueue_O);
	}

	Unlock();

	return Ret_B;
}

/*!
Description
  This function removes the first item that matches the characteristics
  specified by the _pElement value and the comparison method.
  
  Remark : The comparison method should be able to compare two
           element in such a way that a matching element will
           lead to a result of 0. The example below explains this
           situation.
  
  _pElem1 'is matching' _pElem2 if _pMethod(_pElem1, _pElem2) == 0
  
  If no comparison method is provided, a simple binary memory comparison
  is performed
  
Parameters
  _pElement - The characteristics of the element to search for
  _pMethod  - The comparison method

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::Remove(Element *_pElement, int (*_pMethod)(const void *, const void *) /* = NULL */)
{
	bool Ret_B = false;
	BofListNode<Element> *pNode_O = Find(_pElement, _pMethod);

	if (pNode_O != NULL)
	{
		Ret_B = Remove(pNode_O);
	}

	return Ret_B;
}

/*!
Description
  This function indicates if an item that matches the characteristics
  specified by the _pElement value and the comparison method is contained
  in the list.
  
  Remark : The comparison method should be able to compare two
           element in such a way that a matching element will
           lead to a result of 0. The example below explains this
           situation.
  
  _pElem1 'is matching' _pElem2 if _pMethod(_pElem1, _pElem2) == 0
  
  If no comparison method is provided, a simple binary memory comparison
  is performed
  
Parameters
  _pElement - The characteristics of the element to search for
  _pMethod  - The comparison method

Returns
  TRUE  - An element that matches those criterion is contained in the list
  FALSE - No such element can be found

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::Contains(Element *_pElement, int (*_pMethod)(const void *, const void *) /* = NULL */)
{
	bool Ret_B = (Find(_pElement, _pMethod) != NULL);

	return Ret_B;
}

/*!
Description
  This function sorts the list using the comparison method specified.
  
  Remark : The comparison method should be able to compare two
           element in such a way that the result should be 0, -1 or +1
           depending if the element are equal, smaller or greater to each other
  
  If no comparison method is provided, a simple binary memory comparison
  is performed
  
Parameters
  _pMethod  - The comparison method

Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::Sort(int (*_pMethod)(const void *, const void *) /* = NULL */)
{
	bool Ret_B = true;

	Lock();

#if defined(PROFILE_CLIST)
	BOF_ENTER_BENCH    (mpProfiler_O, PROFILE_CLIST_SORT);
#endif

	ListSort(_pMethod, false, true);

#if defined(PROFILE_CLIST)
	BOF_LEAVE_BENCH    (mpProfiler_O, PROFILE_CLIST_SORT);
#endif

	Unlock();

	return Ret_B;
}

/*!
Description
  This function sorts the list using the comparison method specified.
  
  Remark : The comparison method should be able to compare two
           element in such a way that the result should be 0, -1 or +1
           depending if the element are equal, smaller or greater to each other
  
Parameters
  _pHead_O      - The pointer to the head node of the list
  _pCompare     - The comparison method
  _IsCircular_B - Indicates if the list is a circular one
  _IsDouble_B   - Indicates if the list is a doubly linked one

Returns
  The pointer to the new head element

Remarks
  None

See also
  Nothing
*/
template<typename Element>
void BofList<Element>::ListSort(int (*_pCompare)(const void *, const void *), bool _IsCircular_B, bool _IsDouble_B)
{
	BofListNode<Element> *pNode1_O = NULL;
	BofListNode<Element> *pNode2_O = NULL;
	BofListNode<Element> *pNode3_O = NULL;
	BofListNode<Element> *pOldHead_O = NULL;
	S32 InSize_S32 = 0;
	S32 NbMerges_S32 = 0;
	S32 Size1_S32 = 0;
	S32 Size2_S32 = 0;
	S32 I_S32 = 0;
	bool Exit_B = false;

	// If the list is empty
	if (!mpHead_O)
	{ return; }

	InSize_S32 = 1;

	while (!Exit_B)
	{
		pNode1_O = mpHead_O;
		pOldHead_O = mpHead_O;
		mpHead_O = NULL;
		mpQueue_O = NULL;

		NbMerges_S32 = 0;

		while (pNode1_O)
		{
			// There exists a merge to be done
			// Step InSize_S32 places along from pNode1_O
			NbMerges_S32++;
			pNode2_O = pNode1_O;
			Size1_S32 = 0;

			for (I_S32 = 0; I_S32 < InSize_S32; I_S32++)
			{
				Size1_S32++;

				if (_IsCircular_B)
				{
					pNode2_O = (pNode2_O->pNext_O == pOldHead_O ? NULL : pNode2_O->pNext_O);
				}
				else
				{
					pNode2_O = pNode2_O->pNext_O;
				}

				if (!pNode2_O)
				{ break; }
			}

			// If pNode2_O hasn't fallen off the end, we have to lists to merge
			Size2_S32 = InSize_S32;

			// Merge the two lists
			while (Size1_S32 > 0 || (Size2_S32 > 0 && pNode2_O))
			{
				/* decide whether next element of merge comes from p or q */
				// Decide whether the next element of the merge comes from pNode1_O or pNode2_O

				// pNode1_O is empty : it must come from pNode2_O
				if (Size1_S32 == 0)
				{
					pNode3_O = pNode2_O;
					pNode2_O = pNode2_O->pNext_O;
					Size2_S32--;

					if (_IsCircular_B && pNode2_O == pOldHead_O)
					{ pNode2_O = NULL; }
				}
					// pNode2_O is empty : it must come from pNode1_O
				else if (Size2_S32 == 0 || !pNode2_O)
				{
					pNode3_O = pNode1_O;
					pNode1_O = pNode1_O->pNext_O;
					Size1_S32--;

					if (_IsCircular_B && pNode1_O == pOldHead_O)
					{ pNode1_O = NULL; }
				}
					// The first element from pNode1_O is lower or equal to the one from pNode2_O
				else if (((_pCompare != NULL) && (_pCompare(&pNode1_O->Value, &pNode2_O->Value) <= 0)) ||
				         ((_pCompare == NULL) && (memcmp(&pNode1_O->Value, &pNode2_O->Value, sizeof(Element)) <= 0)))
				{
					pNode3_O = pNode1_O;
					pNode1_O = pNode1_O->pNext_O;
					Size1_S32--;

					if (_IsCircular_B && pNode1_O == pOldHead_O)
					{ pNode1_O = NULL; }
				}
					// The first element from pNode1_O is greater than the one from pNode2_O
				else
				{
					pNode3_O = pNode2_O;
					pNode2_O = pNode2_O->pNext_O;
					Size2_S32--;

					if (_IsCircular_B && pNode2_O == pOldHead_O)
					{ pNode2_O = NULL; }
				}

				// Add the next element to the merged list
				if (mpQueue_O)
				{
					mpQueue_O->pNext_O = pNode3_O;
				}
				else
				{
					mpHead_O = pNode3_O;
				}

				if (_IsDouble_B)
				{
					// Maintain reverse pointers in a doubly linked list.
					pNode3_O->pPrev_O = mpQueue_O;
				}

				mpQueue_O = pNode3_O;
			}

			// Now pNode1_O has stepped InSize_S32 places along and pNode2_O has too
			pNode1_O = pNode2_O;
		}

		if (_IsCircular_B)
		{
			mpQueue_O->pNext_O = mpHead_O;

			if (_IsDouble_B)
			{
				mpHead_O->pPrev_O = mpQueue_O;
			}
		}
		else
		{
			mpQueue_O->pNext_O = NULL;
		}

		// If we have done only on merge, we are done
		if (NbMerges_S32 <= 1)
		{
			Exit_B = true;
		}

		// Otherwise, repeat by merging lists twice the size
		InSize_S32 *= 2;
	}
}

/*!
Description
  This function searches for the first item that matches the characteristics
  specified by the _pElement value and the comparison method.
  
  Remark : The comparison method should be able to compare two
           element in such a way that a matching element will
           lead to a result of 0. The example below explains this
           situation.
  
  _pElem1 'is matching' _pElem2 if _pMethod(_pElem1, _pElem2) == 0
  
  If no comparison method is provided, a simple binary memory comparison
  is performed
  
Parameters
  _pElement - The characteristics of the element to search for
  _pMethod  - The comparison method

Returns
  TRUE  - An element that matches those criterion is contained in the list
  FALSE - No such element can be found

Remarks
  None

See also
  Nothing
*/
template<typename Element>
BofListNode<Element> *BofList<Element>::Find(Element *_pElement, int (*_pMethod)(const void *, const void *) /* = NULL */)
{
	BofListNode<Element> *Ret_O = NULL;

#if defined(CHECK_LIST_INTEGRITY)
	BOF_ASSERT(_pElement != NULL);
#endif

	if (_pElement != NULL)
	{
		Lock();

#if defined(PROFILE_CLIST)
		BOF_ENTER_BENCH    (mpProfiler_O, PROFILE_CLIST_FIND);
#endif

		Ret_O = mpHead_O;

		while (Ret_O != NULL)
		{
			// It's a match
			if (((_pMethod != NULL) && (_pMethod(&Ret_O->Value, _pElement) == 0)) ||
			    ((_pMethod == NULL) && (memcmp(&Ret_O->Value, _pElement, sizeof(Element)) == 0)))
			{

#if defined(CHECK_LIST_INTEGRITY)
				BOF_ASSERT(Ret_O->pParent_O == this);
#endif

				break;
			}

			Ret_O = Ret_O->pNext_O;

#if defined(CHECK_LIST_INTEGRITY)
			if(Ret_O != NULL)
			{
				BOF_ASSERT(Ret_O->pParent_O == this);
			}
#endif

		}

#if defined(PROFILE_CLIST)
		BOF_LEAVE_BENCH    (mpProfiler_O, PROFILE_CLIST_FIND);
#endif

		Unlock();
	}

	return Ret_O;
}

/*!
Description
  This function searches for the last item that matches the characteristics
  specified by the _pElement value and the comparison method.
  
  Remark : The comparison method should be able to compare two
           element in such a way that a matching element will
           lead to a result of 0. The example below explains this
           situation.
  
  _pElem1 'is matching' _pElem2 if _pMethod(_pElem1, _pElem2) == 0
  
  If no comparison method is provided, a simple binary memory comparison
  is performed
  
Parameters
  _pElement - The characteristics of the element to search for
  _pMethod  - The comparison method

Returns
  TRUE  - An element that matches those criterion is contained in the list
  FALSE - No such element can be found

Remarks
  None

See also
  Nothing
*/
template<typename Element>
BofListNode<Element> *BofList<Element>::FindLast(Element *_pElement, int (*_pMethod)(const void *, const void *) /* = NULL */)
{
	BofListNode<Element> *Ret_O = NULL;

#if defined(CHECK_LIST_INTEGRITY)
	BOF_ASSERT(_pElement != NULL);
#endif

	if (_pElement != NULL)
	{
		Lock();

#if defined(PROFILE_CLIST)
		BOF_ENTER_BENCH    (mpProfiler_O, PROFILE_CLIST_FIND_LAST);
#endif

		Ret_O = mpQueue_O;

		while (Ret_O != NULL)
		{
			// It's a match
			if (((_pMethod != NULL) && (_pMethod(&Ret_O->Value, _pElement) == 0)) ||
			    ((_pMethod == NULL) && (memcmp(&Ret_O->Value, _pElement, sizeof(Element)) == 0)))
			{
				break;
			}

			Ret_O = Ret_O->pPrev_O;
		}

#if defined(PROFILE_CLIST)
		BOF_LEAVE_BENCH    (mpProfiler_O, PROFILE_CLIST_FIND_LAST);
#endif

		Unlock();
	}

	return Ret_O;
}

/*!
Description
  This function retrieves the head element of the list.
  
  If the _Remove_B flag is set, the element is removed in the same time.
  
Parameters
  None
  
Returns
  The pointer to the retrieved node

Remarks
  None

See also
  Nothing
*/
template<typename Element>
BofListNode<Element> *BofList<Element>::GetFirst()
{
	BofListNode<Element> *pRet_O = mpHead_O;

	return pRet_O;
}

/*!
Description
  This function retrieves the queue element of the list.
  
  If the _Remove_B flag is set, the element is removed in the same time.
  
Parameters
  _Remove_B - Indicates if the element should be removed in the same time
  
Returns
  The pointer to the retrieved node

Remarks
  None

See also
  Nothing
*/
template<typename Element>
BofListNode<Element> *BofList<Element>::GetLast()
{
	BofListNode<Element> *pRet_O = mpQueue_O;

	return pRet_O;
}

/*!
Description
  This function gets a node from the free node pool.
  
Parameters
  _pElement - The pointer to an optional value to initialize the node with
  
Returns
  The pointer to the retrieved node

Remarks
  None

See also
  Nothing
*/
template<typename Element>
BofListNode<Element> *BofList<Element>::GetFreeNode(Element *_pElement /* = NULL */)
{
	BofListNode<Element> *pRet_O = NULL;

	Lock();

	if (mpNodePoolMgr_O != NULL)
	{
		pRet_O = mpNodePoolMgr_O->GetFreeNode(_pElement);
	}
	else if (mpFreeNodesHead_O != NULL)
	{
		pRet_O = mpFreeNodesHead_O;
		mpFreeNodesHead_O = pRet_O->pNext_O;

		if (mpFreeNodesHead_O != NULL)
		{ mpFreeNodesHead_O->pPrev_O = NULL; }
		else
		{ mpFreeNodesQueue_O = NULL; }

		// Clear node attributes
		pRet_O->pParent_O = NULL;
		pRet_O->pNext_O = NULL;
		pRet_O->pPrev_O = NULL;

		if (_pElement != NULL)
		{ pRet_O->Value = *_pElement; }
		else
		{ memset(&pRet_O->Value, 0x00, sizeof(Element)); }
	}

	Unlock();

	return pRet_O;
}

/*!
Description
  This function gets the specified node from the free node pool.
  
Parameters
  _pElement - The pointer to the node
  
Returns
  The pointer to the retrieved node

Remarks
  None

See also
  Nothing
*/
template<typename Element>
BofListNode<Element> *BofList<Element>::GetFreeNode(BofListNode<Element> *_pNode_O)
{
	BofListNode<Element> *pRet_O = _pNode_O;

	Lock();

	if (mpNodePoolMgr_O != NULL)
	{
		pRet_O = mpNodePoolMgr_O->GetFreeNode(_pNode_O);
	}
	else
	{
		// This node is part of the free node pool
		if (((_pNode_O->pNext_O != NULL) || (_pNode_O->pPrev_O != NULL) ||
		     (mpFreeNodesHead_O == _pNode_O) || (mpFreeNodesQueue_O == _pNode_O)) &&
		    (_pNode_O->pParent_O == NULL))
		{
			// It's the only item of the list
			if ((_pNode_O == mpFreeNodesHead_O) && (_pNode_O == mpFreeNodesQueue_O))
			{
				mpFreeNodesHead_O = NULL;
				mpFreeNodesQueue_O = NULL;
			}
				// It's the head
			else if (_pNode_O == mpFreeNodesHead_O)
			{

#if defined(CHECK_LIST_INTEGRITY)
				BOF_ASSERT(_pNode_O->pNext_O != NULL);
#endif

				_pNode_O->pNext_O->pPrev_O = NULL;
				mpFreeNodesHead_O = _pNode_O->pNext_O;
			}
				// It's the tail
			else if (_pNode_O == mpFreeNodesQueue_O)
			{

#if defined(CHECK_LIST_INTEGRITY)
				BOF_ASSERT(_pNode_O->pPrev_O != NULL);
#endif

				_pNode_O->pPrev_O->pNext_O = NULL;
				mpFreeNodesQueue_O = _pNode_O->pPrev_O;
			}
				// It's an internal node
			else
			{

#if defined(CHECK_LIST_INTEGRITY)
				BOF_ASSERT(_pNode_O->pNext_O != NULL);
				BOF_ASSERT(_pNode_O->pPrev_O != NULL);
#endif

				_pNode_O->pPrev_O->pNext_O = _pNode_O->pNext_O;
				_pNode_O->pNext_O->pPrev_O = _pNode_O->pPrev_O;
			}
		}

#if defined(CHECK_LIST_INTEGRITY)
		BOF_ASSERT(_pNode_O->pParent_O == NULL);
#endif

		// Clear node attributes
		pRet_O->pParent_O = NULL;
		pRet_O->pNext_O = NULL;
		pRet_O->pPrev_O = NULL;
	}

	Unlock();

	return pRet_O;
}

/*!
Description
  This function frees a node and gives it back to the free node pool.
  
Parameters
  _pNode_O - The node to free
  
Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::SetFreeNode(BofListNode<Element> *_pNode_O)
{
	bool Ret_B = false;

#if defined(CHECK_LIST_INTEGRITY)
	BOF_ASSERT(_pNode_O != NULL);
#endif

	if (_pNode_O != NULL)
	{
		Lock();

		if (mpNodePoolMgr_O != NULL)
		{
			Ret_B = mpNodePoolMgr_O->SetFreeNode(_pNode_O);
		}
		else
		{
			_pNode_O->pParent_O = NULL;

			// The free pool is empty
			if (mpFreeNodesQueue_O == NULL)
			{
				_pNode_O->pNext_O = NULL;
				_pNode_O->pPrev_O = NULL;
				mpFreeNodesHead_O = _pNode_O;
				mpFreeNodesQueue_O = _pNode_O;
			}
			else
			{
				// Put back the node on end of the free pool
				_pNode_O->pNext_O = NULL;
				_pNode_O->pPrev_O = mpFreeNodesQueue_O;
				mpFreeNodesQueue_O->pNext_O = _pNode_O;
				mpFreeNodesQueue_O = _pNode_O;
			}

			Ret_B = true;
		}

		Unlock();
	}

	return Ret_B;
}

/*!
Description
  This function clears the list
  
Parameters
  None
  
Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::Clear()
{
	bool Ret_B = false;

	Lock();

#if defined(PROFILE_CLIST)
	BOF_ENTER_BENCH    (mpProfiler_O, PROFILE_CLIST_CLEAR);
#endif

	while (GetFirst() != NULL)
	{ RemoveFirst(); };

#if defined(CHECK_LIST_INTEGRITY)

	BOF_ASSERT(mNbElements_U32 == 0);
	BOF_ASSERT(mpHead_O        == NULL);
	BOF_ASSERT(mpQueue_O       == NULL);

#endif

	Ret_B = true;

#if defined(PROFILE_CLIST)
	BOF_LEAVE_BENCH    (mpProfiler_O, PROFILE_CLIST_CLEAR);
#endif

	Unlock();

	return Ret_B;
}

/*!
Description
  This function returns the number of nodes in the list
  
Parameters
  None
  
Returns
  The number of nodes in the list
  
Remarks
  None

See also
  Nothing
*/
template<typename Element>
U32 BofList<Element>::GetCount()
{
	U32 Ret_U32 = 0;

	Ret_U32 = mNbElements_U32;

	return Ret_U32;
}

/*!
Description
  This function returns the maximum number of nodes 
  that can be contained in the list
  
Parameters
  None
  
Returns
  The maximum number of nodes that can be stored in the list
  
Remarks
  None

See also
  Nothing
*/
template<typename Element>
U32 BofList<Element>::GetCapacity()
{
	U32 Ret_U32 = 0;

	Ret_U32 = mNbMaxElements_U32;

	return Ret_U32;
}

/*!
Description
  This function indicates if the list is full
  
Parameters
  None
  
Returns
  TRUE  - The list is full
  FALSE - The list is not full
  
Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::IsFull()
{
	bool Ret_B = false;

	Ret_B = (GetCount() >= GetCapacity());

	return Ret_B;
}

/*!
Description
  This function indicates if the list is empty
  
Parameters
  None
  
Returns
  TRUE  - The list is empty
  FALSE - The list is not empty
  
Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::IsEmpty()
{
	bool Ret_B = false;

	Ret_B = (GetCount() == 0);

	return Ret_B;
}

/*!
Description
  This function retrieves the profiling statistics associated to a particular function
  
Parameters
  _ItemId_U32 - The function ID
  _pStats_X   - The pointer to where to store the statistics
  
Returns
  TRUE  - The operation was successful
  FALSE - The operation failed

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::GetProfilingStats(U32 _ItemId_U32, BOF_STAT_VARIABLE *_pStats_X)
{
	bool Ret_B = false;

#if defined(CHECK_LIST_INTEGRITY)
	BOF_ASSERT(_pStats_X != NULL);
#endif

	if (_pStats_X != NULL)
	{
		memset(_pStats_X, 0x00, sizeof(_pStats_X));

		if (mpProfiler_O != NULL)
		{
			Ret_B = mpProfiler_O->GetStats(_ItemId_U32, _pStats_X);
		}
	}

	return Ret_B;
}

/*!
Description
  This function retrieves the memory used by this object
  
Parameters
  None
  
Returns
  The size in bytes of the memory used by this object

Remarks
  None

See also
  Nothing
*/
template<typename Element>
U64 BofList<Element>::GetMemoryUsage()
{
	U64 Ret_U64 = 0;

	Ret_U64 += sizeof(BofList<Element>);

	if (mpNodes_O != NULL)
	{ Ret_U64 += (sizeof(BofListNode<Element>) * mNbMaxElements_U32); }
	if (mpProfiler_O != NULL)
	{ Ret_U64 += mpProfiler_O->GetMemoryUsage(); }

	return Ret_U64;
}

/*!
Description
  This function checks the consistency of the list.
  
  That is, it verifies that the physical list of elements
  is matching the number of elements recorded
  
Parameters
  None
  
Returns
  TRUE  - The list is consistent
  FALSE - The list is not consistent

Remarks
  None

See also
  Nothing
*/
template<typename Element>
bool BofList<Element>::CheckConsistency()
{
	bool Ret_B = false;
	U32 NbElements_U32 = 0;
	BofListNode<Element> *pNode_O = NULL;

	Lock();

	pNode_O = mpHead_O;

	while (pNode_O != NULL)
	{
		NbElements_U32 += 1;

		pNode_O = pNode_O->pNext_O;
	}

	Ret_B = (NbElements_U32 == mNbElements_U32);

	Unlock();

	return Ret_B;
}
END_BOF_NAMESPACE()
#endif /* __cplusplus */
