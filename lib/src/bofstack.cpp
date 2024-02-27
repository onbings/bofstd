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
#include <bofstd/bofstack.h>
#include <bofstd/bofsystem.h>

#include <string.h>
BEGIN_BOF_NAMESPACE()
#define BOF_STACK_LOCK(Sts)                                                                 \
  {                                                                                         \
    Sts = mStackParam_X.MultiThreadAware_B ? Bof_LockMutex(mStackMtx_X) : BOF_ERR_NO_ERROR; \
  }
#define BOF_STACK_UNLOCK()                \
  {                                       \
    if (mStackParam_X.MultiThreadAware_B) \
      Bof_UnlockMutex(mStackMtx_X);       \
  }

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
  mStackParam_X = _rStackParam_X;
  mErrorCode_E = mStackParam_X.MultiThreadAware_B ? Bof_CreateMutex("BofCircularBuffer", false, mStackParam_X.PriorityInversionAware_B, mStackMtx_X) : BOF_ERR_NO_ERROR;
  if (mErrorCode_E == BOF_ERR_NO_ERROR)
  {
    if (_rStackParam_X.pData)
    {
      mDataPreAllocated_B = true;
      mpStack_U8 = reinterpret_cast<uint8_t *>(mStackParam_X.pData);
    }
    else
    {
      mDataPreAllocated_B = false;
      mpStack_U8 = new uint8_t[mStackParam_X.MaxStackSize_U32];
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
  if (mStackParam_X.MultiThreadAware_B)
  {
    Bof_DestroyMutex(mStackMtx_X);
  }
}
BOFERR BofStack::LastErrorCode()
{
  return mErrorCode_E;
}
void BofStack::SetSwapByte(bool _SwapByte_B)
{
  mStackParam_X.SwapByte_B = _SwapByte_B;
}
bool BofStack::IsSwapByte()
{
  return mStackParam_X.SwapByte_B;
}

// !!!64bits!!!
uint32_t BofStack::GetStackPointer()
{
  return (uint32_t)(mpStackLocation_U8 - mpStack_U8);
}

bool BofStack::SetStackPointer(uint32_t Ptr_U32)
{
  if (Ptr_U32 < mStackParam_X.MaxStackSize_U32)
  {
    mpStackLocation_U8 = &mpStack_U8[Ptr_U32];
    return true;
  }
  else
  {
    return false;
  }
}

bool BofStack::AdjustStackBufferLocation(int32_t _Offset_S32)
{
  int32_t NewPtr_S32 = (static_cast<int32_t>(GetStackPointer()) + _Offset_S32);
  bool Rts_B = false;

  if ((NewPtr_S32 <= static_cast<int32_t>(mStackParam_X.MaxStackSize_U32)) && (NewPtr_S32 >= 0))
  {
    mpStackLocation_U8 = &mpStack_U8[NewPtr_S32];
    Rts_B = true;
  }
  return Rts_B;
}

uint32_t BofStack::GetStackSize()
{
  return mStackParam_X.MaxStackSize_U32;
}

// uint8_t				  *GetStack()               {return(mpStack_U8);}
uint8_t *BofStack::GetStackBuffer()
{
  return mpStack_U8;
}

uint8_t *BofStack::GetCurrentStackBufferLocation()
{
  return mpStackLocation_U8;
}

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
  BOFERR Sts_E;

  BOF_STACK_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if ((GetStackPointer() + _NbToSkip_U32) <= mStackParam_X.MaxStackSize_U32)
    {
      mpStackLocation_U8 += _NbToSkip_U32;
      Rts_B = true;
    }
    BOF_STACK_UNLOCK();
  }
  return Rts_B;
}

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
  BOFERR Sts_E;

  BOF_STACK_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if ((GetStackPointer() + 1) <= mStackParam_X.MaxStackSize_U32)
    {
      *mpStackLocation_U8 = Val_U8;
      mpStackLocation_U8++;
      Rts_B = true;
    }
    BOF_STACK_UNLOCK();
  }
  return Rts_B;
}

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
  BOFERR Sts_E;

  BOF_STACK_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if ((GetStackPointer() + 2) <= mStackParam_X.MaxStackSize_U32)
    {
      if (mStackParam_X.SwapByte_B)
      {
        BOF_SWAP16(Val_U16);
      }
      *(uint16_t *)mpStackLocation_U8 = Val_U16;
      mpStackLocation_U8 += 2;
      Rts_B = true;
    }
    BOF_STACK_UNLOCK();
  }
  return Rts_B;
}

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
  BOFERR Sts_E;

  BOF_STACK_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if ((GetStackPointer() + 4) <= mStackParam_X.MaxStackSize_U32)
    {
      if (mStackParam_X.SwapByte_B)
      {
        BOF_SWAP32(Val_U32);
      }
      *(uint32_t *)mpStackLocation_U8 = Val_U32;
      mpStackLocation_U8 += 4;
      Rts_B = true;
    }
    BOF_STACK_UNLOCK();
  }
  return Rts_B;
}

bool BofStack::Push(uint64_t Val_U64)
{
  bool Rts_B = false;
  BOFERR Sts_E;

  BOF_STACK_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if ((GetStackPointer() + 8) <= mStackParam_X.MaxStackSize_U32)
    {
      if (mStackParam_X.SwapByte_B)
      {
        BOF_SWAP64(Val_U64);
      }
      *(uint64_t *)mpStackLocation_U8 = Val_U64;
      mpStackLocation_U8 += 8;
      Rts_B = true;
    }
    BOF_STACK_UNLOCK();
  }
  return Rts_B;
}

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
  // Val_U32 = *(uint32_t *)&Val_f;
  return Push(Val_U32);
}

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

  // Val_U64 = *(uint64_t *)&Val_ff;
  memcpy(&Val_U64, &Val_ff, sizeof(Val_U64));
  return Push(Val_U64);
}

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
  BOFERR Sts_E;

  Len_U32 = (uint32_t)strlen(pTxt_c);
  BOF_STACK_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if ((GetStackPointer() + Len_U32 + 1) <= mStackParam_X.MaxStackSize_U32)
    {
      pTxt_c = &pTxt_c[Len_U32 - 1]; // ok if Len_U32=0 as pTxt_c will not be used !
      *mpStackLocation_U8++ = 0;     // Null terminate
      for (i_U32 = 0; i_U32 < Len_U32; i_U32++)
      {
        *mpStackLocation_U8++ = *pTxt_c--;
      }
      Rts_B = true;
    }
    BOF_STACK_UNLOCK();
  }

  return Rts_B;
}

bool BofStack::Push(uint32_t _Nb_U32, uint8_t *_pVal_U8)
{
  bool Rts_B = false;
  BOFERR Sts_E;

  BOF_STACK_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if ((GetStackPointer() + _Nb_U32) <= mStackParam_X.MaxStackSize_U32)
    {
      memcpy(mpStackLocation_U8, _pVal_U8, _Nb_U32);
      mpStackLocation_U8 += _Nb_U32;
      Rts_B = true;
    }
    BOF_STACK_UNLOCK();
  }
  return Rts_B;
}

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
  BOFERR Sts_E;

  BOF_STACK_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if (GetStackPointer() >= _NbToSkip_U32)
    {
      mpStackLocation_U8 -= _NbToSkip_U32;
      Rts_B = true;
    }
    BOF_STACK_UNLOCK();
  }
  return Rts_B;
}

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
  BOFERR Sts_E;

  if (pVal_U8)
  {
    BOF_STACK_LOCK(Sts_E);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      if (GetStackPointer() >= 1)
      {
        mpStackLocation_U8--;
        *pVal_U8 = *mpStackLocation_U8;
        Rts_B = true;
      }
      BOF_STACK_UNLOCK();
    }
  }
  return Rts_B;
}

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
  BOFERR Sts_E;

  if (pVal_U16)
  {
    BOF_STACK_LOCK(Sts_E);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      if (GetStackPointer() >= 2)
      {
        mpStackLocation_U8 -= 2;

        if (mStackParam_X.SwapByte_B)
        {
          Val_U16 = *(uint16_t *)mpStackLocation_U8;
          BOF_SWAP16(Val_U16);
          *pVal_U16 = Val_U16;
        }
        else
        {
          *pVal_U16 = *(uint16_t *)mpStackLocation_U8;
        }
        Rts_B = true;
      }
      BOF_STACK_UNLOCK();
    }
  }
  return Rts_B;
}

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
  BOFERR Sts_E;

  if (pVal_U32)
  {
    BOF_STACK_LOCK(Sts_E);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      if (GetStackPointer() >= 4)
      {
        mpStackLocation_U8 -= 4;

        if (mStackParam_X.SwapByte_B)
        {
          Val_U32 = *(uint32_t *)mpStackLocation_U8;
          BOF_SWAP32(Val_U32);
          *pVal_U32 = Val_U32;
        }
        else
        {
          *pVal_U32 = *(uint32_t *)mpStackLocation_U8;
        }
        Rts_B = true;
      }
      BOF_STACK_UNLOCK();
    }
  }
  return Rts_B;
}

bool BofStack::Pop(uint64_t *pVal_U64)
{
  bool Rts_B = false;
  uint64_t Val_U64;
  BOFERR Sts_E;

  if (pVal_U64)
  {
    BOF_STACK_LOCK(Sts_E);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      if (GetStackPointer() >= 8)
      {
        mpStackLocation_U8 -= 8;

        if (mStackParam_X.SwapByte_B)
        {
          Val_U64 = *(uint64_t *)mpStackLocation_U8;
          BOF_SWAP64(Val_U64);
          *pVal_U64 = Val_U64;
        }
        else
        {
          *pVal_U64 = *(uint64_t *)mpStackLocation_U8;
        }
        Rts_B = true;
      }
      BOF_STACK_UNLOCK();
    }
  }
  return Rts_B;
}

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
  uint32_t NbMaxToPop_U32; // Nb_U32;
  BOFERR Sts_E;

  if (pTxt_c)
  {
    BOF_STACK_LOCK(Sts_E);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      NbMaxToPop_U32 = GetStackPointer();
      if (NbMaxToPop_U32)
      {
        //			Nb_U32                   = _MaxSize_U32;
        do
        {
          c_c = *mpStackLocation_U8--;
          // if (Nb_U32)
          {
            *pTxt_c++ = c_c; // null char is also copied to string
            // Nb_U32--;
          }
          NbMaxToPop_U32--;
        } while ((c_c) && (NbMaxToPop_U32));
        if (c_c)
        {
          *pTxt_c = 0; // To be sure to null terminate
        }
        Rts_B = (!c_c);
      }
      BOF_STACK_UNLOCK();
    }
  }
  return Rts_B;
}

bool BofStack::Pop(uint32_t _Nb_U32, uint8_t *_pVal_U8)
{
  bool Rts_B = false;
  BOFERR Sts_E;

  if (_pVal_U8)
  {
    BOF_STACK_LOCK(Sts_E);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      if (GetStackPointer() >= _Nb_U32)
      {
        mpStackLocation_U8 -= _Nb_U32;
        memcpy(_pVal_U8, mpStackLocation_U8, _Nb_U32);
        Rts_B = true;
      }
      BOF_STACK_UNLOCK();
    }
  }
  return Rts_B;
}

BOFERR BofStack::LockStack()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mStackParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_LockMutex(mStackMtx_X);
  }
  return Rts_E;
}
BOFERR BofStack::UnlockStack()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mStackParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_UnlockMutex(mStackMtx_X);
  }
  return Rts_E;
}
END_BOF_NAMESPACE()