/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module contains routines for creating and managing a circular
 * buffer.
 *
 * Name:        BofRawCircularBuffer.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Jul 26 2005  BHA : Initial release
 */
#include <bofstd/bofrawcircularbuffer.h>
#include <string.h>

BEGIN_BOF_NAMESPACE()

#define BOF_RAW_CIRCULAR_BUFFER_LOCK(Sts)                                                                           \
  {                                                                                                                 \
    Sts = mRawCircularBufferParam_X.MultiThreadAware_B ? Bof_LockMutex(mRawCircularBufferMtx_X) : BOF_ERR_NO_ERROR; \
  }
#define BOF_RAW_CIRCULAR_BUFFER_UNLOCK()              \
  {                                                   \
    if (mRawCircularBufferParam_X.MultiThreadAware_B) \
      Bof_UnlockMutex(mRawCircularBufferMtx_X);       \
  }

#define DBG_RAW_BUFFER_INFO(pUser, pRaw)                                                                                                                \
  {                                                                                                                                                     \
    printf("%-28.28s Base %p Crt %p Used %06d Rem %06d/%06d Psh Indx %06d SltNot %06d Sz.Ptr %06d/%p %06d/%p\n", pUser, mpData_U8, mpCrtBufferHead_U8,  \
           mRawCircularBufferParam_X.BufferSizeInByte_U32 - mCrtBufferRemain_U32, mCrtBufferRemain_U32, mRawCircularBufferParam_X.BufferSizeInByte_U32, \
           (pRaw)->IndexInBuffer_U32, (pRaw)->SlotEmpySpace_U32, (pRaw)->Size1_U32, (pRaw)->pData1_U8, (pRaw)->Size2_U32, (pRaw)->pData2_U8);           \
  }

BofRawCircularBuffer::BofRawCircularBuffer(const BOF_RAW_CIRCULAR_BUFFER_PARAM &_rRawCircularBufferParam_X)
{
  BOF_CIRCULAR_BUFFER_PARAM CircularBufferParam_X;

  mRawCircularBufferParam_X = _rRawCircularBufferParam_X;
  mErrorCode_E = BOF_ERR_TOO_SMALL;
  if ((mRawCircularBufferParam_X.BufferSizeInByte_U32) && (mRawCircularBufferParam_X.NbMaxBufferEntry_U32))
  {
    mErrorCode_E = _rRawCircularBufferParam_X.MultiThreadAware_B ? Bof_CreateMutex("BofRawCircularBuffer", false, false, mRawCircularBufferMtx_X) : BOF_ERR_NO_ERROR;
    if (mErrorCode_E == BOF_ERR_NO_ERROR)
    {
      if (_rRawCircularBufferParam_X.pData_U8)
      {
        mDataPreAllocated_B = true;
        mpData_U8 = _rRawCircularBufferParam_X.pData_U8;
      }
      else
      {
        mDataPreAllocated_B = false;
        mpData_U8 = new uint8_t[mRawCircularBufferParam_X.BufferSizeInByte_U32];
        if (mpData_U8 == nullptr)
        {
          mErrorCode_E = BOF_ERR_ENOMEM;
        }
      }
      if (mErrorCode_E == BOF_ERR_NO_ERROR)
      {
        mSlotSize_U32 = mRawCircularBufferParam_X.SlotMode_B ? (mRawCircularBufferParam_X.BufferSizeInByte_U32 / mRawCircularBufferParam_X.NbMaxBufferEntry_U32) : 0;
        mRawBufferToPush_X.Reset();
        mCrtBufferRemain_U32 = mRawCircularBufferParam_X.BufferSizeInByte_U32;
        mpCrtBufferHead_U8 = mpData_U8;
        mRawBufferToPush_X.pData1_U8 = mpCrtBufferHead_U8;
        mpCrtBufferEnd_U8 = &mpData_U8[mRawCircularBufferParam_X.BufferSizeInByte_U32];

        mErrorCode_E = BOF_ERR_ENOMEM;
        CircularBufferParam_X.MultiThreadAware_B = false; // Controled by the mutex of BofRawCircularBuffer
        CircularBufferParam_X.NbMaxElement_U32 = mRawCircularBufferParam_X.NbMaxBufferEntry_U32;
        CircularBufferParam_X.pData = nullptr;
        CircularBufferParam_X.Overwrite_B = false;
        CircularBufferParam_X.Blocking_B = _rRawCircularBufferParam_X.Blocking_B;
        CircularBufferParam_X.PopLockMode_B = false;
        mpuBufferCollection = std::make_unique<BofCircularBuffer<BOF_RAW_BUFFER>>(CircularBufferParam_X);
        if (mpuBufferCollection)
        {
          mErrorCode_E = mpuBufferCollection->LastErrorCode();
        }
      }
    }
  }
}

BofRawCircularBuffer::~BofRawCircularBuffer()
{
  if (!mDataPreAllocated_B)
  {
    BOF_SAFE_DELETE_ARRAY(mpData_U8);
  }
  Bof_DestroyMutex(mRawCircularBufferMtx_X);
}

void BofRawCircularBuffer::Reset()
{
  BOFERR Sts_E;

  BOF_RAW_CIRCULAR_BUFFER_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    mLevelMax_U32 = 0;
    mOverflow_B = false;
    mAppendMode_B = false;
    mpuBufferCollection->Reset();
    mRawBufferToPush_X.Reset();
    mRawBufferToPush_X.pData1_U8 = mpData_U8;
    mCrtBufferRemain_U32 = mRawCircularBufferParam_X.BufferSizeInByte_U32;
    mpCrtBufferHead_U8 = mpData_U8;
    BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
  }
}

BOFERR BofRawCircularBuffer::LastErrorCode()
{
  return mErrorCode_E;
}

bool BofRawCircularBuffer::IsEmpty()
{
  bool Rts_B;

  Rts_B = mpuBufferCollection->IsEmpty();
  return Rts_B;
}

uint32_t BofRawCircularBuffer::GetSlotSize()
{
  return mSlotSize_U32;
}

bool BofRawCircularBuffer::IsFull()
{
  bool Rts_B;

  Rts_B = mpuBufferCollection->IsFull();
  return Rts_B;
}

uint32_t BofRawCircularBuffer::GetNbElement(uint32_t *_pSizeOfFirst_U32)
{
  uint32_t Rts_U32, PopIndex_U32;
  BOF_RAW_BUFFER RawBuffer_X;
  BOFERR Sts_E;

  BOF_RAW_CIRCULAR_BUFFER_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if (_pSizeOfFirst_U32)
    {
      if (IsEmpty())
      {
        *_pSizeOfFirst_U32 = 0;
      }
      else
      {
        if (mpuBufferCollection->Peek(&RawBuffer_X, 0, nullptr, nullptr) == BOF_ERR_NO_ERROR)
        {
          *_pSizeOfFirst_U32 = RawBuffer_X.Size1_U32 + RawBuffer_X.Size2_U32;
        }
      }
    }
    Rts_U32 = mpuBufferCollection->GetNbElement();
    //printf("GetNbElement %p nb %d\n", this, mpuBufferCollection->GetNbElement());
    BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
  }
  return Rts_U32;
}

uint32_t BofRawCircularBuffer::GetNbFreeElement(uint32_t *_pRemainingSize_U32)
{
  uint32_t Rts_U32 = 0;
  BOFERR Sts_E;

  BOF_RAW_CIRCULAR_BUFFER_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    Rts_U32 = mpuBufferCollection->GetNbFreeElement();
    if (_pRemainingSize_U32)
    {
      *_pRemainingSize_U32 = mCrtBufferRemain_U32;
    }
    //printf("GetNbFreeElement %p nb %d\n", this, mpuBufferCollection->GetNbElement());
    BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
  }
  return Rts_U32;
}

uint32_t BofRawCircularBuffer::GetCapacity(uint32_t *_pTotalSize_U32)
{
  uint32_t Rts_U32;
  BOFERR Sts_E;

  BOF_RAW_CIRCULAR_BUFFER_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    Rts_U32 = mpuBufferCollection->GetCapacity();
    if (_pTotalSize_U32)
    {
      *_pTotalSize_U32 = mRawCircularBufferParam_X.BufferSizeInByte_U32;
    }
    BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
  }
  return Rts_U32;
}

uint32_t BofRawCircularBuffer::GetMaxLevel()
{
  uint32_t Rts_U32;

  Rts_U32 = mSlotSize_U32 ? mpuBufferCollection->GetMaxLevel() : mLevelMax_U32;
  return Rts_U32;
}

BOFERR BofRawCircularBuffer::SetOverWriteMode(bool _Overwrite_B)
{
  BOFERR Rts_E;

  BOF_RAW_CIRCULAR_BUFFER_LOCK(Rts_E);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    mRawCircularBufferParam_X.Overwrite_B = _Overwrite_B;
    mpuBufferCollection->SetOverWriteMode(_Overwrite_B);
    BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
  }
  return Rts_E;
}

BOFERR BofRawCircularBuffer::SetAppendMode(uint32_t _BlockingTimeouItInMs_U32, bool _Append_B, BOF_RAW_BUFFER **_ppStorage_X)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  // uint8_t pData_U8[8];

  if ((mAppendMode_B) && (_Append_B == false)) // We leave the append mode->unlock growing buffer
  {
    // Rts_E = UpdatePushRawBuffer(0, pData_U8);
    // if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = PushRawBuffer(_BlockingTimeouItInMs_U32, _ppStorage_X);
    }
  }
  mAppendMode_B = _Append_B;
  return Rts_E;
}

bool BofRawCircularBuffer::IsBufferOverflow()
{
  bool Rts_B = mOverflow_B;
  mOverflow_B = false;
  return Rts_B;
}

BOFERR BofRawCircularBuffer::LockRawCircularBuffer()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mRawCircularBufferParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_LockMutex(mRawCircularBufferMtx_X);
  }
  return Rts_E;
}

BOFERR BofRawCircularBuffer::UnlockRawCircularBuffer()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mRawCircularBufferParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_UnlockMutex(mRawCircularBufferMtx_X);
  }
  return Rts_E;
}

// Buffer is protected and all param are checked by caller
BOFERR BofRawCircularBuffer::UpdatePushRawBuffer(uint32_t _SizeUpToTheEnd_U32, uint32_t _Size_U32, const uint8_t *_pData_U8)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint32_t Nb1_U32, Nb2_U32, Index_U32;

  BOF_ASSERT(mCrtBufferRemain_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
  BOF_ASSERT(mpCrtBufferHead_U8 >= mpData_U8);
  BOF_ASSERT(mpCrtBufferHead_U8 < mpCrtBufferEnd_U8);
  BOF_ASSERT(_Size_U32 <= mCrtBufferRemain_U32);

  if (mRawCircularBufferParam_X.AlwaysContiguous_B)
  {
    BOF_ASSERT(mRawBufferToPush_X.Size2_U32 == 0);
  }

  if ((_pData_U8) && (_Size_U32 <= mCrtBufferRemain_U32))
  {
    if (_SizeUpToTheEnd_U32)
    {
      BOF_ASSERT(mRawCircularBufferParam_X.AlwaysContiguous_B);
      BOF_ASSERT(mRawBufferToPush_X.Size2_U32 == 0);
      BOF_ASSERT(mRawBufferToPush_X.pData2_U8 == nullptr)

      mRawBufferToPush_X.Reset();
      // NO mCrtBufferRemain_U32 = mRawCircularBufferParam_X.BufferSizeInByte_U32;
      mpCrtBufferHead_U8 = mpData_U8;
      mRawBufferToPush_X.pData1_U8 = mpCrtBufferHead_U8;
      // printf("UpdatePushRawBuffer CrtBufferRemain %d b1 %03d:%p \n", mCrtBufferRemain_U32, mRawBufferToPush_X.Size1_U32, mRawBufferToPush_X.pData1_U8);
    }
    if (mRawBufferToPush_X.Size2_U32)
    {
      BOF_ASSERT(mRawBufferToPush_X.IndexInBuffer_U32 + mRawBufferToPush_X.Size2_U32 + _Size_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
      if (mRawBufferToPush_X.IndexInBuffer_U32 + mRawBufferToPush_X.Size2_U32 + _Size_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32)
      {
        memcpy(&mpData_U8[mRawBufferToPush_X.IndexInBuffer_U32 + mRawBufferToPush_X.Size2_U32], _pData_U8, _Size_U32);
        BOF_ASSERT(mpCrtBufferHead_U8 == &mpData_U8[mRawBufferToPush_X.IndexInBuffer_U32 + mRawBufferToPush_X.Size2_U32]);
        mCrtBufferRemain_U32 -= _Size_U32;
        mpCrtBufferHead_U8 += _Size_U32;
        mRawBufferToPush_X.Size2_U32 += _Size_U32;
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
    else
    {
      Rts_E = BOF_ERR_NO_ERROR;
      Index_U32 = mRawBufferToPush_X.IndexInBuffer_U32 + _Size_U32;
      if (Index_U32 > mRawCircularBufferParam_X.BufferSizeInByte_U32)
      {
        Nb1_U32 = mRawCircularBufferParam_X.BufferSizeInByte_U32 - (mRawBufferToPush_X.IndexInBuffer_U32 + mRawBufferToPush_X.Size1_U32);
        memcpy(&mpData_U8[mRawBufferToPush_X.IndexInBuffer_U32 + mRawBufferToPush_X.Size1_U32], _pData_U8, Nb1_U32);
        BOF_ASSERT(mpCrtBufferHead_U8 == &mpData_U8[mRawBufferToPush_X.IndexInBuffer_U32 + mRawBufferToPush_X.Size1_U32]);

        Nb2_U32 = _Size_U32 - Nb1_U32;
        memcpy(&mpData_U8[0 + 0], &_pData_U8[Nb1_U32], Nb2_U32);

        mCrtBufferRemain_U32 -= (Nb1_U32 + Nb2_U32);
        mpCrtBufferHead_U8 = &mpData_U8[Nb1_U32];

        mRawBufferToPush_X.Size1_U32 = mRawCircularBufferParam_X.BufferSizeInByte_U32 - mRawBufferToPush_X.IndexInBuffer_U32;
        mRawBufferToPush_X.pData2_U8 = mpData_U8;
        mRawBufferToPush_X.Size2_U32 = Nb2_U32; // (Index_U32 - mRawCircularBufferParam_X.BufferSizeInByte_U32);

        BOF_ASSERT(Nb2_U32 == (Index_U32 - mRawCircularBufferParam_X.BufferSizeInByte_U32));
      }
      else
      {
        memcpy(&mpData_U8[mRawBufferToPush_X.IndexInBuffer_U32 + mRawBufferToPush_X.Size1_U32], _pData_U8, _Size_U32);
        BOF_ASSERT(mpCrtBufferHead_U8 == &mpData_U8[mRawBufferToPush_X.IndexInBuffer_U32 + mRawBufferToPush_X.Size1_U32]);

        mCrtBufferRemain_U32 -= _Size_U32;
        mpCrtBufferHead_U8 += _Size_U32;

        mRawBufferToPush_X.Size1_U32 += _Size_U32;
      }
      BOF_ASSERT(mpCrtBufferHead_U8 <= mpCrtBufferEnd_U8);
      if (mpCrtBufferHead_U8 >= mpCrtBufferEnd_U8)
      {
        BOF_ASSERT(mpCrtBufferHead_U8 == mpCrtBufferEnd_U8);
        mpCrtBufferHead_U8 = mpData_U8;
      }
    }

    BOF_ASSERT(mCrtBufferRemain_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
    BOF_ASSERT(mpCrtBufferHead_U8 >= mpData_U8);
    BOF_ASSERT(mpCrtBufferHead_U8 < mpCrtBufferEnd_U8);
  }
  if (mRawCircularBufferParam_X.AlwaysContiguous_B)
  {
    BOF_ASSERT(mRawBufferToPush_X.Size2_U32 == 0);
  }
  return Rts_E;
}

// Buffer is protected and all param are checked by caller
BOFERR BofRawCircularBuffer::PushRawBuffer(uint32_t _BlockingTimeouItInMs_U32, BOF_RAW_BUFFER **_ppStorage_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint32_t Nb_U32;

  BOF_ASSERT(mCrtBufferRemain_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
  BOF_ASSERT(mpCrtBufferHead_U8 >= mpData_U8);
  BOF_ASSERT(mpCrtBufferHead_U8 < mpCrtBufferEnd_U8);

  Nb_U32 = (mRawBufferToPush_X.Size1_U32 + mRawBufferToPush_X.Size2_U32);
  if (mSlotSize_U32)
  {
    mRawBufferToPush_X.SlotEmpySpace_U32 = mSlotSize_U32 - Nb_U32; // For partial read of slot
    BOF_ASSERT(mRawBufferToPush_X.SlotEmpySpace_U32 <= mSlotSize_U32);
  }
  Rts_E = mpuBufferCollection->Push(&mRawBufferToPush_X, _BlockingTimeouItInMs_U32, nullptr, _ppStorage_X);

  BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    if (mSlotSize_U32)
    {
      mCrtBufferRemain_U32 -= mRawBufferToPush_X.SlotEmpySpace_U32;
      BOF_ASSERT(mCrtBufferRemain_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
      mpCrtBufferHead_U8 += mRawBufferToPush_X.SlotEmpySpace_U32;
      if (mpCrtBufferHead_U8 >= mpCrtBufferEnd_U8)
      {
        BOF_ASSERT(mpCrtBufferHead_U8 == mpCrtBufferEnd_U8);
        mpCrtBufferHead_U8 = mpData_U8;
      }
    }
    else
    {
      if (Nb_U32 > mLevelMax_U32)
      {
        mLevelMax_U32 = Nb_U32;
      }
    }
    mRawBufferToPush_X.Reset();
    mRawBufferToPush_X.IndexInBuffer_U32 = mpCrtBufferHead_U8 - mpData_U8;
    mRawBufferToPush_X.pData1_U8 = mpCrtBufferHead_U8;
  }
  BOF_ASSERT(mCrtBufferRemain_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
  BOF_ASSERT(mpCrtBufferHead_U8 >= mpData_U8);
  BOF_ASSERT(mpCrtBufferHead_U8 < mpCrtBufferEnd_U8);

  return Rts_E;
}

BOFERR BofRawCircularBuffer::PushBuffer(uint32_t _BlockingTimeouItInMs_U32, uint32_t _Nb_U32, const uint8_t *_pData_U8, BOF_RAW_BUFFER **_ppStorage_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  bool EnoughSpace_B;
  uint32_t Nb_U32, IndexOf_U32, SizeUpToTheEnd_U32, SizeToRecover_U32;
  // char pHeader_c[128];
  bool IsLocked_B;
  BOF_RAW_BUFFER RawBuffer_X, *pRawBuffer_X;

  if ((_Nb_U32) && (_pData_U8))
  {
    BOF_RAW_CIRCULAR_BUFFER_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      // sprintf(pHeader_c, "PushBuffer Start: Nb %d ", _Nb_U32);
      // DBG_RAW_BUFFER_INFO(pHeader_c, &mRawBufferToPush_X);
      BOF_ASSERT(mCrtBufferRemain_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
      BOF_ASSERT(mpCrtBufferHead_U8 >= mpData_U8);
      BOF_ASSERT(mpCrtBufferHead_U8 < mpCrtBufferEnd_U8);

      EnoughSpace_B = (_Nb_U32 <= mCrtBufferRemain_U32);
      SizeUpToTheEnd_U32 = 0;
      pRawBuffer_X = nullptr;
      if (EnoughSpace_B)
      {
        if (mSlotSize_U32)
        {
          if (mAppendMode_B)
          {
            EnoughSpace_B = ((_Nb_U32 + mRawBufferToPush_X.Size1_U32 + mRawBufferToPush_X.Size2_U32) <= mSlotSize_U32);
          }
          else
          {
            EnoughSpace_B = (_Nb_U32 <= mSlotSize_U32);
          }
        }
        else
        {
          if (mRawCircularBufferParam_X.AlwaysContiguous_B)
          {
            SizeUpToTheEnd_U32 = mpCrtBufferEnd_U8 - mpCrtBufferHead_U8;
            EnoughSpace_B = (_Nb_U32 <= SizeUpToTheEnd_U32);
            SizeUpToTheEnd_U32 = 0;
          }
          else
          {
            if (IsFull()) // we have only a limitted amound of entries
            {
              EnoughSpace_B = false;
            }
          }
        }
      }

      if ((!EnoughSpace_B) && (!mAppendMode_B) && (mRawCircularBufferParam_X.Overwrite_B))
      {
        if (mSlotSize_U32)
        {
          EnoughSpace_B = (_Nb_U32 <= mSlotSize_U32);
        }
        else
        {
          if (mRawCircularBufferParam_X.AlwaysContiguous_B)
          {
            SizeUpToTheEnd_U32 = mpCrtBufferEnd_U8 - mpCrtBufferHead_U8;
            EnoughSpace_B = (_Nb_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
          }
          else
          {
            EnoughSpace_B = (_Nb_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
          }
        }
        if (EnoughSpace_B)
        {
          mOverflow_B = true;
          if (mSlotSize_U32)
          {
            // Not BofRawCircularBuffer::Skip
            Rts_E = mpuBufferCollection->Skip(&RawBuffer_X, false, &IndexOf_U32, &pRawBuffer_X, &IsLocked_B); // Remove oldest buffer so we have new storage space
            BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
            if (Rts_E == BOF_ERR_NO_ERROR)
            {
              mCrtBufferRemain_U32 = mSlotSize_U32;
            }
          }
          else
          {
            // if pop>=push the remaining size to pop cannot be used by push of this one in
            if (SizeUpToTheEnd_U32)
            {
              if (mpuBufferCollection->IsEmpty())
              {
                SizeToRecover_U32 = 0;
                mOverflow_B = false;
              }
              // else if (mpuBufferCollection->IsFull())
              //{
              //   SizeToRecover_U32 = _Nb_U32;
              //   SizeUpToTheEnd_U32 = 0;
              // }
              else
              {
                if (mpCrtBufferHead_U8 == mpData_U8)
                {
                  SizeToRecover_U32 = _Nb_U32;
                }
                else
                {
                  SizeToRecover_U32 = _Nb_U32 + SizeUpToTheEnd_U32;
                }
              }
              // printf("PushBuffer align %d CrtBufferRemain %d SizeToRecover %d nb %d pop %d push %d\n", SizeUpToTheEnd_U32, mCrtBufferRemain_U32, SizeToRecover_U32, _Nb_U32, mpuBufferCollection->GetPopIndex(), mpuBufferCollection->GetPushIndex());
            }
            else
            {
              SizeToRecover_U32 = _Nb_U32;
            }
            BOF_ASSERT(SizeUpToTheEnd_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
            BOF_ASSERT(SizeToRecover_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);

            if (SizeToRecover_U32)
            {
              do
              {
                // Not BofRawCircularBuffer::Skip
                Rts_E = mpuBufferCollection->Skip(&RawBuffer_X, false, &IndexOf_U32, &pRawBuffer_X, &IsLocked_B); // Remove oldest bufferS so we have new storage space
                BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
                if (Rts_E == BOF_ERR_NO_ERROR)
                {
                  mCrtBufferRemain_U32 += (RawBuffer_X.Size1_U32 + RawBuffer_X.Size2_U32);
                }
              } while ((Rts_E == BOF_ERR_NO_ERROR) && (mCrtBufferRemain_U32 < SizeToRecover_U32));
            }
          }
          BOF_ASSERT(mCrtBufferRemain_U32 >= _Nb_U32);
        }
      }

      if (EnoughSpace_B)
      {
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          Rts_E = UpdatePushRawBuffer(SizeUpToTheEnd_U32, _Nb_U32, _pData_U8);
          BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
          if (!mAppendMode_B)
          {
            Rts_E = PushRawBuffer(_BlockingTimeouItInMs_U32,&pRawBuffer_X);
          }
        }
      }
      else
      {
        Rts_E = BOF_ERR_TOO_BIG;
      }
    }
    // DBG_RAW_BUFFER_INFO("PushBuffer End:", &mRawBufferToPush_X);
    if (_ppStorage_X)
    {
      *_ppStorage_X = pRawBuffer_X;
    }
    BOF_ASSERT(mCrtBufferRemain_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
    BOF_ASSERT(mpCrtBufferHead_U8 >= mpData_U8);
    BOF_ASSERT(mpCrtBufferHead_U8 < mpCrtBufferEnd_U8);
    //printf("PushBuffer %p nb %d\n", this, mpuBufferCollection->GetNbElement());
    BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
  }
  return Rts_E;
}

BOFERR BofRawCircularBuffer::PopOrPeekBuffer(bool _Pop_B, uint32_t _BlockingTimeouItInMs_U32, uint32_t *_pNbMax_U32, uint8_t *_pData_U8)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint32_t Size_U32, NbToRead_U32, NbRead_U32, IndexOf_U32;
  BOF_RAW_BUFFER RawBuffer_X, *pRawBuffer_X;
  bool IsLocked_B, PartialRead_B;
  // char pHeader_c[128];

  if (_pNbMax_U32)
  {
    NbRead_U32 = 0;
    if ((*_pNbMax_U32) && (_pData_U8))
    {
      BOF_RAW_CIRCULAR_BUFFER_LOCK(Rts_E);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        BOF_ASSERT(mCrtBufferRemain_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
        BOF_ASSERT(mpCrtBufferHead_U8 >= mpData_U8);
        BOF_ASSERT(mpCrtBufferHead_U8 < mpCrtBufferEnd_U8);

        //if (!IsEmpty())
        {
          Rts_E = mpuBufferCollection->Peek(&RawBuffer_X, _BlockingTimeouItInMs_U32, nullptr, &pRawBuffer_X);
          //BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            // sprintf(pHeader_c, _Pop_B ? "PopBuffer  Start: Nb %d " : "PeekBuffer Start: Nb %d ", *_pNbMax_U32);
            // DBG_RAW_BUFFER_INFO(pHeader_c, pRawBuffer_X);

            NbToRead_U32 = RawBuffer_X.Size1_U32 + RawBuffer_X.Size2_U32;
            PartialRead_B = false;
            if (NbToRead_U32 > *_pNbMax_U32)
            {
              NbToRead_U32 = *_pNbMax_U32;
              PartialRead_B = true;
            }
            NbRead_U32 = 0;
            if (NbToRead_U32)
            {
              if (RawBuffer_X.Size1_U32)
              {
                Size_U32 = (RawBuffer_X.Size1_U32 > NbToRead_U32) ? NbToRead_U32 : RawBuffer_X.Size1_U32;

                memcpy(_pData_U8, RawBuffer_X.pData1_U8, Size_U32);
                pRawBuffer_X->Size1_U32 -= Size_U32;
                pRawBuffer_X->pData1_U8 += Size_U32;
                NbToRead_U32 -= Size_U32;
                NbRead_U32 += Size_U32;
              }
            }
            if (NbToRead_U32)
            {
              if (RawBuffer_X.Size2_U32)
              {
                BOF_ASSERT(RawBuffer_X.pData2_U8);

                Size_U32 = (RawBuffer_X.Size2_U32 > NbToRead_U32) ? NbToRead_U32 : RawBuffer_X.Size2_U32;

                memcpy(&_pData_U8[NbRead_U32], RawBuffer_X.pData2_U8, Size_U32);
                pRawBuffer_X->Size2_U32 -= Size_U32;
                pRawBuffer_X->pData2_U8 += Size_U32;
                NbToRead_U32 -= Size_U32;
                NbRead_U32 += Size_U32;
              }
            }
            if (PartialRead_B)
            {
              mCrtBufferRemain_U32 += NbRead_U32;
            }
            else if (_Pop_B)
            {
              // Not BofRawCircularBuffer::Skip
              Rts_E = mpuBufferCollection->Skip(&RawBuffer_X, true, &IndexOf_U32, &pRawBuffer_X, &IsLocked_B); // Remove oldest bufferS so we have new storage space
              BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
              if (Rts_E == BOF_ERR_NO_ERROR)
              {
                mCrtBufferRemain_U32 += NbRead_U32;
                if (mSlotSize_U32)
                {
                  mCrtBufferRemain_U32 += RawBuffer_X.SlotEmpySpace_U32;
                }
                BOF_ASSERT(mCrtBufferRemain_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
                // NO !!                  mpCrtBufferHead_U8 += NbRead_U32;
              }
            }
            // DBG_RAW_BUFFER_INFO(_Pop_B ? "PopBuffer  End:" : "PeekBuffer End:", pRawBuffer_X);
          }
        }
        /*
        else
        {
          Rts_E = BOF_ERR_EMPTY;
        }
        */
        BOF_ASSERT(mCrtBufferRemain_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
        BOF_ASSERT(mpCrtBufferHead_U8 >= mpData_U8);
        BOF_ASSERT(mpCrtBufferHead_U8 < mpCrtBufferEnd_U8);
        //printf("PopOrPeekBuffer %p nb %d\n", this, mpuBufferCollection->GetNbElement());
        BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
      } // if (Rts_E == BOF_ERR_NO_ERROR)
    }   // if (_pData_U8)
    *_pNbMax_U32 = NbRead_U32;
  } // if (_pNbMax_U32)
  return Rts_E;
}

BOFERR BofRawCircularBuffer::PopBuffer(uint32_t _BlockingTimeouItInMs_U32, uint32_t *_pNbMax_U32, uint8_t *_pData_U8)
{
  BOFERR Rts_E = PopOrPeekBuffer(true, _BlockingTimeouItInMs_U32, _pNbMax_U32, _pData_U8);
  return Rts_E;
}

BOFERR BofRawCircularBuffer::Peek(uint32_t _BlockingTimeouItInMs_U32, uint32_t *_pNbMax_U32, uint8_t *_pData_U8)
{
  BOFERR Rts_E = PopOrPeekBuffer(false, _BlockingTimeouItInMs_U32, _pNbMax_U32, _pData_U8);
  return Rts_E;
}

// Need to call Skip after GetBufferPtr
BOFERR BofRawCircularBuffer::GetBufferPtr(uint32_t _BlockingTimeouItInMs_U32, uint32_t *_pNb1_U32, uint8_t **_ppData1_U8, uint32_t *_pNb2_U32, uint8_t **_ppData2_U8)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BOF_RAW_BUFFER RawBuffer_X, *pRawBuffer_X;

  if ((_pNb1_U32) && (_ppData1_U8) && (_pNb2_U32) && (_ppData2_U8))
  {
    *_pNb1_U32 = 0;
    *_ppData1_U8 = mpData_U8;
    *_pNb2_U32 = 0;
    *_ppData2_U8 = nullptr;
    BOF_RAW_CIRCULAR_BUFFER_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      //if (!IsEmpty())
      {
        Rts_E = mpuBufferCollection->Peek(&RawBuffer_X, _BlockingTimeouItInMs_U32, nullptr, &pRawBuffer_X);
        //BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          *_pNb1_U32 = RawBuffer_X.Size1_U32;
          *_ppData1_U8 = RawBuffer_X.pData1_U8;
          *_pNb2_U32 = RawBuffer_X.Size2_U32;
          *_ppData2_U8 = RawBuffer_X.pData2_U8;
        }
      }
      //else
      //{
      // no as we want to return mpData_U8 as container's base pointer Rts_E = BOF_ERR_EMPTY;
      //}
      //printf("GetBufferPtr %p nb %d\n", this, mpuBufferCollection->GetNbElement());
      BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
    }
  }
  return Rts_E;
}

BOFERR BofRawCircularBuffer::Skip(bool _SignalIfNeeded_B, bool *_pLocked_B)
{
  BOFERR Rts_E;
  uint32_t IndexOf_U32; // , Nb_U32, SizeOfFirst_U32;
  BOF_RAW_BUFFER RawBuffer_X, *pRawBuffer_X;

  BOF_RAW_CIRCULAR_BUFFER_LOCK(Rts_E);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    // Nb_U32 = mpuBufferCollection->GetNbElement();
    // printf("skip nb before %d\n", Nb_U32);
    Rts_E = mpuBufferCollection->Skip(&RawBuffer_X, _SignalIfNeeded_B, &IndexOf_U32, &pRawBuffer_X, _pLocked_B);
    // Nb_U32 = mpuBufferCollection->GetNbElement();
    // printf("skip rts %d ptr %p nb %d\n", Rts_E, mpuBufferCollection.get(), Nb_U32);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      mCrtBufferRemain_U32 += (RawBuffer_X.Size1_U32 + RawBuffer_X.Size2_U32);
      if (mSlotSize_U32)
      {
        mCrtBufferRemain_U32 += mRawBufferToPush_X.SlotEmpySpace_U32;
      }
      BOF_ASSERT(mCrtBufferRemain_U32 <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
    }
    //printf("Skip %p nb %d\n", this, mpuBufferCollection->GetNbElement());
    BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
  }
  return Rts_E;
}

END_BOF_NAMESPACE()