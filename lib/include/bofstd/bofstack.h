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
 * Name:        BofStack.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 23 2002  BHA : Initial release
 */
#pragma once

/*** Include **********************************************************/

#include <bofstd/bofstd.h>
#include <bofstd/bofsystem.h>
#include <cstdint>

BEGIN_BOF_NAMESPACE()

/*** Defines ****************************************************************/

/*** Macro ******************************************************************/

/*** enum *******************************************************************/

/*** struct *******************************************************************/



/*** Macro *******************************************************/

/*** BofStack *****************************************************************/


struct BOFSTD_EXPORT BOF_STACK_PARAM
{
		bool MultiThreadAware_B;                                /*! true if the object is used in a multi threaded application (use mCs)*/
		uint32_t MaxStackSize_U32;
		bool SwapByte_B;
		void *pData;                                            /*! Specifies a pointer to the stack buffer zone (pre-allocated buffer). Set to nullptr if the memory must be allocated/deleted by the object */

		BOF_STACK_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			MultiThreadAware_B = false;
			MaxStackSize_U32 = 0;
			SwapByte_B = false;
		}
};

/*!
 * Class BofStack
 *
 * This class encapsulates a basic stack object and provides the following interface
 * - Stack creation from scratch or from initial value
 * - Push operations on basic type: byte, word, long, float, string,...
 * - Pop operations on basic type: byte, word, long, float, string,...
 */

class BOFSTD_EXPORT BofStack
{
private:
		bool mDataPreAllocated_B;                       /*! true if mpData_U8 is provided by the caller*/
		uint32_t mMaxStackSize_U32;                         /*!<Maximum size of stack*/
		BOF_MUTEX mMtx_X;                                    /*! Provide a serialized access to shared resources in a multi threaded environement*/
		BOFERR mErrorCode_E;
protected:
		uint8_t *mpStack_U8;                               /*!<Pointer to stack storage*/
		bool mSwapByte_B;                               /*!<true if binary data must be swapped (little/Big endian representation*/
		uint8_t *mpStackLocation_U8;
public:
		BofStack(const BOF_STACK_PARAM &_rStackParam_X);

		virtual ~BofStack();

		BofStack &operator=(const BofStack &) = delete;             // Disallow copying
		BofStack(const BofStack &) = delete;

		BOFERR LastErrorCode()
		{ return mErrorCode_E; }

		void SetSwapByte(bool _SwapByte_B)
		{ mSwapByte_B = _SwapByte_B; }

		bool PushSkip(uint32_t _NbToSkip_U32);

		bool Push(uint8_t Val_U8);

		bool Push(uint16_t Val_U16);

		bool Push(uint32_t Val_U32);

		bool Push(uint64_t Val_U64);

		bool Push(float Val_f);

		bool Push(double Val_ff);

		bool Push(char *pTxt_c);

		bool Push(uint32_t _Nb_U32, uint8_t *_pVal_U8);

		bool PopSkip(uint32_t _NbToSkip_U32);

		bool Pop(uint8_t *pVal_U8);

		bool Pop(uint16_t *pVal_U16);

		bool Pop(uint32_t *pVal_U32);

		bool Pop(uint64_t *pVal_U64);

		bool Pop(float *pVal_f);

		bool Pop(double *pVal_ff);

		bool Pop(char *pTxt_c);

		bool Pop(uint32_t _Nb_U32, uint8_t *_pVal_U8);

		bool IsSwapByte()
		{ return mSwapByte_B; }

// !!!64bits!!!
		uint32_t GetStackPointer()
		{ return (uint32_t) (mpStackLocation_U8 - mpStack_U8); }

		bool SetStackPointer(uint32_t Ptr_U32)
		{
			if (Ptr_U32 < mMaxStackSize_U32)
			{
				mpStackLocation_U8 = &mpStack_U8[Ptr_U32];
				return true;
			}
			else
			{ return false; }
		}

		bool AdjustStackBufferLocation(int32_t _Offset_S32)
		{
			int32_t NewPtr_S32 = (static_cast< int32_t > (GetStackPointer()) + _Offset_S32);
			bool Rts_B = false;

			if ((NewPtr_S32 <= static_cast< int32_t > (mMaxStackSize_U32)) && (NewPtr_S32 >= 0))
			{
				mpStackLocation_U8 = &mpStack_U8[NewPtr_S32];
				Rts_B = true;
			}
			return Rts_B;
		}

		uint32_t GetStackSize()
		{ return mMaxStackSize_U32; }

// uint8_t				  *GetStack()               {return(mpStack_U8);}
		uint8_t *GetStackBuffer()
		{ return mpStack_U8; }

		uint8_t *GetCurrentStackBufferLocation()
		{ return mpStackLocation_U8; }

		void LockStack()
		{ Bof_LockMutex(mMtx_X); }

		void UnlockStack()
		{ Bof_UnlockMutex(mMtx_X); }
};
END_BOF_NAMESPACE()