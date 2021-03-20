/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines routines for creating and managing a stack
 * buffer.
 *
 * Name:        BofStack.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 23 2002  BHA : Initial release
 */

/*** Include Files **********************************************************/

#include <bofstd/bofsystem.h>
#include <bofstd/bofstack.h>

#include <string.h>
BEGIN_BOF_NAMESPACE()

/*** Global variable ********************************************************/

/*** BofStack ********************************************************************/

/*!
 * Description
 * The BofStack method creates an empty stack object.
 *
 *
 * Parameters
 * _rStackParam_X: Specify the stack param
 *
 * Returns
 * None
 *
 * Remarks
 * None
 */


BofStack::BofStack(const BOF_STACK_PARAM &_rStackParam_X)
{
	mErrorCode_E = _rStackParam_X.MultiThreadAware_B ? Bof_CreateMutex("BofCircularBuffer", true, true, mMtx_X) : BOF_ERR_NO_ERROR;
	if (mErrorCode_E == BOF_ERR_NO_ERROR)
	{

		mMaxStackSize_U32 = _rStackParam_X.MaxStackSize_U32;
		mSwapByte_B = _rStackParam_X.SwapByte_B;
		if (_rStackParam_X.pData)
		{
			mDataPreAllocated_B = true;
			mpStack_U8 = reinterpret_cast< uint8_t * > (_rStackParam_X.pData);
		}
		else
		{
			mDataPreAllocated_B = false;
			mpStack_U8 = new uint8_t[mMaxStackSize_U32];
		}
		mpStackLocation_U8 = mpStack_U8;

		if (mpStack_U8)
		{
			mErrorCode_E = BOF_ERR_NO_ERROR;
		}
		else
		{
			mErrorCode_E = BOF_ERR_ENOMEM;
		}
	}
}


/*** ~BofStack ********************************************************************/

/*!
 * Description
 * The ~BofStack method de-allocate the stack storage space
 *
 * Parameters
 * None
 *
 * Returns
 * None
 *
 * Remarks
 * None
 */

BofStack::~BofStack()
{
	if (!mDataPreAllocated_B)
	{
		BOF_SAFE_DELETE_ARRAY(mpStack_U8);
	}
	Bof_DestroyMutex(mMtx_X);
}




/*** PushSkip ********************************************************************/

/*!
 * Description
 * The PushSkip method simulates the write of n data byte on the stack
 *
 * Parameters
 * _NbToSkip_U32 :      Specifies the number of byte to skip on stack (push op)
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofStack::PushSkip(uint32_t _NbToSkip_U32)
{
	bool Rts_B = false;

	Bof_LockMutex(mMtx_X);
	if ((GetStackPointer() + _NbToSkip_U32) <= mMaxStackSize_U32)
	{
		mpStackLocation_U8 += _NbToSkip_U32;
		Rts_B = true;
	}
	Bof_UnlockMutex(mMtx_X);
	return Rts_B;
}


/*** Push ********************************************************************/

/*!
 * Description
 * The Push method writes (pushs) a standard representation of a byte (8 bits) variable
 * on the stack
 *
 * Parameters
 * Val_U8 :      Specifies the value to be pushed on the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofStack::Push(uint8_t Val_U8)
{
	bool Rts_B = false;

	Bof_LockMutex(mMtx_X);
	if ((GetStackPointer() + 1) <= mMaxStackSize_U32)
	{
		*mpStackLocation_U8 = Val_U8;
		mpStackLocation_U8++;
		Rts_B = true;
	}
	Bof_UnlockMutex(mMtx_X);
	return Rts_B;
}


/*** Push ********************************************************************/

/*!
 * Description
 * The Push method writes (pushs) a standard representation of a word (16 bits) variable
 * on the stack
 *
 * Parameters
 * Val_U16 :      Specifies the value to be pushed on the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofStack::Push(uint16_t Val_U16)
{
	bool Rts_B = false;

	Bof_LockMutex(mMtx_X);
	if ((GetStackPointer() + 2) <= mMaxStackSize_U32)
	{
		if (mSwapByte_B)
		{
			BOF_SWAP16(Val_U16);
		}
		*(uint16_t *) mpStackLocation_U8 = Val_U16;
		mpStackLocation_U8 += 2;
		Rts_B = true;
	}
	Bof_UnlockMutex(mMtx_X);
	return Rts_B;
}


/*** Push ********************************************************************/

/*!
 * Description
 * The Push method writes (pushs) a standard representation of a long (32 bits) variable
 * on the stack
 *
 * Parameters
 * Val_U32 :      Specifies the value to be pushed on the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofStack::Push(uint32_t Val_U32)
{
	bool Rts_B = false;

	Bof_LockMutex(mMtx_X);
	if ((GetStackPointer() + 4) <= mMaxStackSize_U32)
	{
		if (mSwapByte_B)
		{
			BOF_SWAP32(Val_U32);
		}
		*(uint32_t *) mpStackLocation_U8 = Val_U32;
		mpStackLocation_U8 += 4;
		Rts_B = true;
	}
	Bof_UnlockMutex(mMtx_X);
	return Rts_B;
}


bool BofStack::Push(uint64_t Val_U64)
{
	bool Rts_B = false;

	Bof_LockMutex(mMtx_X);
	if ((GetStackPointer() + 8) <= mMaxStackSize_U32)
	{
		if (mSwapByte_B)
		{
			BOF_SWAP64(Val_U64);
		}
		*(uint64_t *) mpStackLocation_U8 = Val_U64;
		mpStackLocation_U8 += 8;
		Rts_B = true;
	}
	Bof_UnlockMutex(mMtx_X);
	return Rts_B;
}


/*** Push ********************************************************************/

/*!
 * Description
 * The Push method writes (pushs) a standard representation of a float (32 bits) variable
 * on the stack
 *
 * Parameters
 * Val_f :      Specifies the value to be pushed on the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofStack::Push(float Val_f)
{
	uint32_t Val_U32;

	memcpy(&Val_U32, &Val_f, sizeof(Val_U32));
	//Val_U32 = *(uint32_t *)&Val_f;
	return Push(Val_U32);
}


/*** Push ********************************************************************/

/*!
 * Description
 * The Push method writes (pushs) a standard representation of a double (64 bits) variable
 * on the stack
 *
 * Parameters
 * Val_ff :      Specifies the value to be pushed on the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofStack::Push(double Val_ff)
{
	uint64_t Val_U64;

	//Val_U64 = *(uint64_t *)&Val_ff;
	memcpy(&Val_U64, &Val_ff, sizeof(Val_U64));
	return Push(Val_U64);
}


/*** Push ********************************************************************/

/*!
 * Description
 * The Push method writes (pushs) a standard representationof a C string (zero terminated) variable
 * on the stack
 *
 * Parameters
 * _MaxSize_U32: Specify the maximum buffer size of pTxt_c
 * pTxt_c :      Specifies the value to be pushed on the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofStack::Push(char *pTxt_c)
{
	bool Rts_B = false;
	uint32_t i_U32, Len_U32;

	Len_U32 = (uint32_t) strlen(pTxt_c);
	Bof_LockMutex(mMtx_X);
	if ((GetStackPointer() + Len_U32 + 1) <= mMaxStackSize_U32)
	{
		pTxt_c = &pTxt_c[Len_U32 - 1]; // ok if Len_U32=0 as pTxt_c will not be used !
		*mpStackLocation_U8++ = 0;                    // Null terminate
		for (i_U32 = 0; i_U32 < Len_U32; i_U32++)
		{
			*mpStackLocation_U8++ = *pTxt_c--;
		}
		Rts_B = true;
	}
	Bof_UnlockMutex(mMtx_X);

	return Rts_B;
}


bool BofStack::Push(uint32_t _Nb_U32, uint8_t *_pVal_U8)
{
	bool Rts_B = false;

	Bof_LockMutex(mMtx_X);
	if ((GetStackPointer() + _Nb_U32) <= mMaxStackSize_U32)
	{
		memcpy(mpStackLocation_U8, _pVal_U8, _Nb_U32);
		mpStackLocation_U8 += _Nb_U32;
		Rts_B = true;
	}
	Bof_UnlockMutex(mMtx_X);
	return Rts_B;
}




/*** PopSkip ********************************************************************/

/*!
 * Description
 * The PopSkip method simulates the read of n data byte on the stack
 *
 * Parameters
 * _NbToSkip_U32 :      Specifies the number of byte to skip on stack (pop op)
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofStack::PopSkip(uint32_t _NbToSkip_U32)
{
	bool Rts_B = false;

	Bof_LockMutex(mMtx_X);
	if (GetStackPointer() >= _NbToSkip_U32)
	{
		mpStackLocation_U8 -= _NbToSkip_U32;
		Rts_B = true;
	}
	Bof_UnlockMutex(mMtx_X);
	return Rts_B;
}


/*** Pop ********************************************************************/

/*!
 * Description
 * The Pop method reads (pops) a standard representation of a byte (8 bits) variable
 * from the stack
 *
 * Parameters
 * pVal_U8 :      \Returns the value poped from the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofStack::Pop(uint8_t *pVal_U8)
{
	bool Rts_B = false;

	if (pVal_U8)
	{
		Bof_LockMutex(mMtx_X);
		if (GetStackPointer() >= 1)
		{
			mpStackLocation_U8--;
			*pVal_U8 = *mpStackLocation_U8;
			Rts_B = true;
		}
		Bof_UnlockMutex(mMtx_X);
	}
	return Rts_B;
}


/*** Pop ********************************************************************/

/*!
 * Description
 * The Pop method reads (pops) a standard representation of a word (16 bits) variable
 * from the stack
 *
 * Parameters
 * pVal_U16 :      \Returns the value poped from the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofStack::Pop(uint16_t *pVal_U16)
{
	bool Rts_B = false;
	uint16_t Val_U16;

	if (pVal_U16)
	{
		Bof_LockMutex(mMtx_X);
		if (GetStackPointer() >= 2)
		{
			mpStackLocation_U8 -= 2;

			if (mSwapByte_B)
			{
				Val_U16 = *(uint16_t *) mpStackLocation_U8;
				BOF_SWAP16(Val_U16);
				*pVal_U16 = Val_U16;
			}
			else
			{
				*pVal_U16 = *(uint16_t *) mpStackLocation_U8;
			}
			Rts_B = true;
		}
		Bof_UnlockMutex(mMtx_X);
	}
	return Rts_B;
}


/*** Pop ********************************************************************/

/*!
 * Description
 * The Pop method reads (pops) a standard representation of a long (32 bits) variable
 * from the stack
 *
 * Parameters
 * pVal_U32:      \Returns the value poped from the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofStack::Pop(uint32_t *pVal_U32)
{
	bool Rts_B = false;
	uint32_t Val_U32;

	if (pVal_U32)
	{
		Bof_LockMutex(mMtx_X);
		if (GetStackPointer() >= 4)
		{
			mpStackLocation_U8 -= 4;

			if (mSwapByte_B)
			{
				Val_U32 = *(uint32_t *) mpStackLocation_U8;
				BOF_SWAP32(Val_U32);
				*pVal_U32 = Val_U32;
			}
			else
			{
				*pVal_U32 = *(uint32_t *) mpStackLocation_U8;
			}
			Rts_B = true;
		}
		Bof_UnlockMutex(mMtx_X);
	}
	return Rts_B;
}


bool BofStack::Pop(uint64_t *pVal_U64)
{
	bool Rts_B = false;
	uint64_t Val_U64;

	if (pVal_U64)
	{
		Bof_LockMutex(mMtx_X);
		if (GetStackPointer() >= 8)
		{
			mpStackLocation_U8 -= 8;

			if (mSwapByte_B)
			{
				Val_U64 = *(uint64_t *) mpStackLocation_U8;
				BOF_SWAP64(Val_U64);
				*pVal_U64 = Val_U64;
			}
			else
			{
				*pVal_U64 = *(uint64_t *) mpStackLocation_U8;
			}
			Rts_B = true;
		}
		Bof_UnlockMutex(mMtx_X);
	}
	return Rts_B;
}


/*** Pop ********************************************************************/

/*!
 * Description
 * The Pop method reads (pops) a standard representation of a float (32 bits) variable
 * from the stack
 *
 * Parameters
 * pVal_f:      \Returns the value poped from the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofStack::Pop(float *pVal_f)
{
	bool Rts_B = false;
	uint32_t Val_U32;

	if (Pop(&Val_U32))
	{
//		*pVal_f = *(float *)&Val_U32;
		memcpy(pVal_f, &Val_U32, sizeof(Val_U32));
		Rts_B = true;
	}
	return Rts_B;
}


/*** Pop ********************************************************************/

/*!
 * Description
 * The Pop method reads (pops) a standard representation of a double (64 bits) variable
 * from the stack
 *
 * Parameters
 * pVal_ff:      \Returns the value poped from the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofStack::Pop(double *pVal_ff)
{
	bool Rts_B = false;
	uint64_t Val_U64;

	if (Pop(&Val_U64))
	{
//		*pVal_ff = *(double *)&Val_U64;
		memcpy(pVal_ff, &Val_U64, sizeof(Val_U64));

		Rts_B = true;
	}
	return Rts_B;
}


/*** Pop ********************************************************************/

/*!
 * Description
 * The Pop method reads (pops) a standard representation of a C string (0 terminated) variable
 * from the stack
 *
 * Parameters
 * _MaxSize_U32: Specify the maximum buffer size of pTxt_c
 * pTxt_c:      \Returns the value poped from the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofStack::Pop(char *pTxt_c)
{
	bool Rts_B = true;
	char c_c;
	uint32_t NbMaxToPop_U32;  // Nb_U32;

	if (pTxt_c)
	{
		Bof_LockMutex(mMtx_X);
		NbMaxToPop_U32 = GetStackPointer();
		if (NbMaxToPop_U32)
		{
//			Nb_U32                   = _MaxSize_U32;

			do
			{
				c_c = *mpStackLocation_U8--;
				//if (Nb_U32)
				{
					*pTxt_c++ = c_c;             // null char is also copied to string
					//Nb_U32--;
				}
				NbMaxToPop_U32--;
			} while ((c_c) && (NbMaxToPop_U32));
			if (c_c)
			{
				*pTxt_c = 0;    // To be sure to null terminate
			}
			Rts_B = (!c_c);
		}
		Bof_UnlockMutex(mMtx_X);
	}
	return Rts_B;
}


bool BofStack::Pop(uint32_t _Nb_U32, uint8_t *_pVal_U8)
{
	bool Rts_B = false;

	if (_pVal_U8)
	{
		Bof_LockMutex(mMtx_X);
		if (GetStackPointer() >= _Nb_U32)
		{
			mpStackLocation_U8 -= _Nb_U32;
			memcpy(_pVal_U8, mpStackLocation_U8, _Nb_U32);
			Rts_B = true;
		}
		Bof_UnlockMutex(mMtx_X);
	}
	return Rts_B;
}
END_BOF_NAMESPACE()