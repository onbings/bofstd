/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines routines for creating and managing a circular
 * buffer.
 *
 * Name:        BofCircularBuffer.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Jul 26 2005  BHA : Initial release
 */
#pragma once

#include <bofstd/bofstring.h>
#include <bofstd/bofsystem.h>

#include <cstring>

BEGIN_BOF_NAMESPACE()

#define BOF_CIRCULAR_BUFFER_LOCK(Sts)                                                                         \
  {                                                                                                           \
    Sts = mCircularBufferParam_X.MultiThreadAware_B ? Bof_LockMutex(mCircularBufferMtx_X) : BOF_ERR_NO_ERROR; \
  }
#define BOF_CIRCULAR_BUFFER_UNLOCK()               \
  {                                                \
    if (mCircularBufferParam_X.MultiThreadAware_B) \
      Bof_UnlockMutex(mCircularBufferMtx_X);       \
  }

constexpr uint32_t BOF_CIRCULAR_BUFFER_DBG_MAX_ITEM = 32;

struct BOF_CIRCULAR_BUFFER_PARAM
{
  bool MultiThreadAware_B;   /*! true if the object is used in a multi threaded application (use mCircularBufferMtx_X)*/
  uint32_t NbMaxElement_U32; /*! Specifies the maximum number of element inside the queue*/
  void *pData;               /*! Specifies a pointer to the circular buffer zone (pre-allocated buffer). Set to nullptr if the memory
                              * must be allocated by the function*/
  bool Overwrite_B;          /*! true if new data overwritte the oldest one when the queue is full. */
  bool Blocking_B;
  bool PopLockMode_B; /*! In this mode all pop operation lock the poped element. All theses locked elem will return to use state when the unlockPop methow will becalled*/

  BOF_CIRCULAR_BUFFER_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    MultiThreadAware_B = false;
    NbMaxElement_U32 = 0;
    pData = nullptr;
    Blocking_B = false;
    PopLockMode_B = false;
  }
};

/*!
 * Summary
 * Circular buffer class
 *
 * Description
 * This class manages a circular byte buffer instance
 *
 * See Also
 * None
 */

template <typename DataType>
class BofCircularBuffer
{
private:
  BOF_CIRCULAR_BUFFER_PARAM mCircularBufferParam_X;
  bool mDataPreAllocated_B;              /*! true if mpData_U8 is provided by the caller*/
  DataType *mpData_T;                    /*! Pointer to queue storage buffer used to record queue element*/
  bool mOverflow_B;                      /*! true if data overflow has occured. Reset to false by IsBufferOverflow*/
  uint32_t mPushIndex_U32;               /*! Current position of the write index inside the queue*/
  uint32_t mPopIndex_U32;                /*! Current position of the read index inside the queue*/
  uint32_t mNbElementInBuffer_U32;       /*! Current number of element inside the queue including the locked one (mNbElementLockedInBuffer_U32)*/
  uint32_t mNbElementLockedInBuffer_U32; /*! Current number of element locked inside the queue*/
  uint32_t mLevelMax_U32;                /*! Contains the maximum buffer fill level. This one is reset by the GetMaxLevel method*/
  BOF_MUTEX mCircularBufferMtx_X;        /*! Provide a serialized access to shared resources in a multi threaded environment*/
  BOFERR mErrorCode_E;
  BOF_EVENT mCanReadEvent_X;
  BOF_EVENT mCanWriteEvent_X;
  uint8_t *mpLock_U8;

private:
  BOFERR SignalReadWrite();

public:
  BofCircularBuffer(const BOF_CIRCULAR_BUFFER_PARAM &_rCircularBufferParam_X);
  virtual ~BofCircularBuffer();

  BofCircularBuffer &operator=(const BofCircularBuffer &) = delete; // Disallow copying
  BofCircularBuffer(const BofCircularBuffer &) = delete;

  BOFERR LastErrorCode() const;
  BOFERR LockCircularBuffer();
  BOFERR UnlockCircularBuffer();
  bool IsEmpty() const;
  bool IsFull() const;
  uint32_t GetNbElement() const;
  uint32_t GetNbElementLocked() const;
  uint32_t GetCapacity() const;
  uint32_t GetNbFreeElement();
  BOFERR SetOverWriteMode(bool _Overwrite_B);
  uint32_t GetMaxLevel() const;
  bool IsBufferOverflow();
  void Reset();
  uint32_t GetPushIndex();
  uint32_t GetPopIndex();
  DataType *GetInternalDataBuffer() const;
  BOFERR Push(const DataType *_pData, uint32_t _BlockingTimeouItInMs_U32, uint32_t *_pIndexOf_U32, DataType **_ppStorage_X);
  BOFERR PushForNextPop(const DataType *_pData, bool _ForceIfFull_B, uint32_t _BlockingTimeouItInMs_U32);           // Old InsertAsFirst
  BOFERR Pop(DataType *_pData, uint32_t _BlockingTimeouItInMs_U32, uint32_t *_pIndexOf_U32, DataType **_ppStorage); //_ppStorage is mainly used in mCircularBufferParam_X.PopLockMode_B to provide write access to the locked storage cell
  BOFERR PopLastPush(DataType *_pData, uint32_t *_pIndexOf_U32, DataType **_ppStorage);
  BOFERR Peek(DataType *_pData, uint32_t _BlockingTimeouItInMs_U32, uint32_t *_pIndexOf_U32, DataType **_ppStorage); //_ppStorage is mainly used in mCircularBufferParam_X.PopLockMode_B to provide write access to the locked storage cell
  BOFERR PeekFromPop(uint32_t _RelativeIndexFromPop_U32, DataType *_pData, bool *_pLocked_B, DataType **_ppStorage); //_ppStorage is mainly used in mCircularBufferParam_X.PopLockMode_B to provide write access to the locked storage cell
  BOFERR PeekByIndex(uint32_t _AbsoluteIndex_U32, DataType *_pData, bool *_pLocked_B, DataType **_ppStorage);        //_ppStorage is mainly used in mCircularBufferParam_X.PopLockMode_B to provide write access to the locked storage cell);
  //_SignalIfNeeded_B:: sometime you skip just to process OverWrite mode, in this case do not signal free space
  BOFERR Skip(DataType *_pData, bool _SignalIfNeeded_B, uint32_t *_pIndexOf_U32, DataType **_ppStorage, bool *_pLocked_B);
  BOFERR UnlockPop(uint32_t _AbsoluteIndex_U32);
  bool IsLocked(uint32_t _AbsoluteIndex_U32);
  bool IsEntryFree(uint32_t _AbsoluteIndex_U32, bool *_pIsLocked_B, DataType **_ppStorage); //_ppStorage is mainly used in mCircularBufferParam_X.PopLockMode_B to provide write access to the locked storage cell
  std::string StateInfo();
};

template <typename DataType>
BofCircularBuffer<DataType>::BofCircularBuffer(const BOF_CIRCULAR_BUFFER_PARAM &_rCircularBufferParam_X)
{
  mCircularBufferParam_X = _rCircularBufferParam_X;

  mNbElementInBuffer_U32 = 0;
  mNbElementLockedInBuffer_U32 = 0;
  mpData_T = nullptr;
  mPushIndex_U32 = 0;
  mPopIndex_U32 = 0;
  mOverflow_B = false;
  mLevelMax_U32 = 0;
  mpLock_U8 = nullptr;

  if (mCircularBufferParam_X.NbMaxElement_U32)
  {
    //if (mCircularBufferParam_X.Blocking_B)
    //{
    //  mErrorCode_E = (_rCircularBufferParam_X.MultiThreadAware_B) ? BOF_ERR_NO_ERROR : BOF_ERR_WRONG_MODE;
    //}
    //else
    {
      mErrorCode_E = BOF_ERR_NO_ERROR;
    }
    if (mErrorCode_E == BOF_ERR_NO_ERROR)
    {
      mErrorCode_E = mCircularBufferParam_X.Blocking_B ? Bof_CreateEvent("cb_canread_" + std::to_string(reinterpret_cast<uint64_t>(this)) + "_evt", false, 1, false, false, mCanReadEvent_X) : BOF_ERR_NO_ERROR;
      if (mErrorCode_E == BOF_ERR_NO_ERROR)
      {
        mErrorCode_E = mCircularBufferParam_X.Blocking_B ? Bof_CreateEvent("cb_canwrite_" + std::to_string(reinterpret_cast<uint64_t>(this)) + "_evt", false, 1, false, false, mCanWriteEvent_X) : BOF_ERR_NO_ERROR;
        if (mErrorCode_E == BOF_ERR_NO_ERROR)
        {
          mErrorCode_E = _rCircularBufferParam_X.MultiThreadAware_B ? Bof_CreateMutex("BofCircularBuffer", false, false, mCircularBufferMtx_X) : BOF_ERR_NO_ERROR;
          if (mErrorCode_E == BOF_ERR_NO_ERROR)
          {
            if (_rCircularBufferParam_X.pData)
            {
              mDataPreAllocated_B = true;
              mpData_T = (DataType *)_rCircularBufferParam_X.pData;
            }
            else
            {
              mDataPreAllocated_B = false;
              mpData_T = new DataType[mCircularBufferParam_X.NbMaxElement_U32];
            }

            if (mpData_T)
            {
              mErrorCode_E = mCircularBufferParam_X.Blocking_B ? Bof_SignalEvent(mCanWriteEvent_X, 0) : BOF_ERR_NO_ERROR;
            }
            else
            {
              mErrorCode_E = BOF_ERR_ENOMEM;
            }
          }
        }
      }
    }
    if (mErrorCode_E == BOF_ERR_NO_ERROR)
    {
      // We instance this buffer event if mCircularBufferParam_X.PopLockMode_B is false to avoid a lot of if (mCircularBufferParam_X.PopLockMode_B)
      mpLock_U8 = new uint8_t[mCircularBufferParam_X.NbMaxElement_U32];
      if (mpLock_U8)
      {
        memset(mpLock_U8, 0, mCircularBufferParam_X.NbMaxElement_U32 * sizeof(uint8_t));
      }
      else
      {
        mErrorCode_E = BOF_ERR_ENOMEM;
      }
    }
  }
  else
  {
    mErrorCode_E = BOF_ERR_EINVAL;
  }
  if (mpLock_U8 == nullptr)
  {
    printf("jj");
  }
}

template <typename DataType>
BofCircularBuffer<DataType>::~BofCircularBuffer()
{
  Bof_DestroyMutex(mCircularBufferMtx_X);
  if (!mDataPreAllocated_B)
  {
    BOF_SAFE_DELETE_ARRAY(mpData_T);
  }
  Bof_DestroyEvent(mCanReadEvent_X);
  Bof_DestroyEvent(mCanWriteEvent_X);
  BOF_SAFE_DELETE_ARRAY(mpLock_U8);
}

template <typename DataType>
BOFERR BofCircularBuffer<DataType>::LastErrorCode() const
{
  return mErrorCode_E;
}

template <typename DataType>
bool BofCircularBuffer<DataType>::IsEmpty() const
{
  return mNbElementInBuffer_U32 == 0;
}

template <typename DataType>
bool BofCircularBuffer<DataType>::IsFull() const
{
  return mNbElementInBuffer_U32 == mCircularBufferParam_X.NbMaxElement_U32;
}

template <typename DataType>
uint32_t BofCircularBuffer<DataType>::GetNbElement() const
{
  return mNbElementInBuffer_U32;
}

template <typename DataType>
uint32_t BofCircularBuffer<DataType>::GetNbElementLocked() const
{
  return mNbElementLockedInBuffer_U32;
}

template <typename DataType>
uint32_t BofCircularBuffer<DataType>::GetCapacity() const
{
  return mCircularBufferParam_X.NbMaxElement_U32;
}

template <typename DataType>
BOFERR BofCircularBuffer<DataType>::LockCircularBuffer()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mCircularBufferParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_LockMutex(mCircularBufferMtx_X);
  }
  return Rts_E;
}

template <typename DataType>
BOFERR BofCircularBuffer<DataType>::UnlockCircularBuffer()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mCircularBufferParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_UnlockMutex(mCircularBufferMtx_X);
  }
  return Rts_E;
}

template <typename DataType>
uint32_t BofCircularBuffer<DataType>::GetNbFreeElement()
{
  uint32_t Rts_U32 = 0;

  BOFERR Sts_E;
  BOF_CIRCULAR_BUFFER_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    Rts_U32 = mCircularBufferParam_X.NbMaxElement_U32 - mNbElementInBuffer_U32;
    BOF_CIRCULAR_BUFFER_UNLOCK();
  }
  return Rts_U32;
}

template <typename DataType>
BOFERR BofCircularBuffer<DataType>::SetOverWriteMode(bool _Overwrite_B)
{
  mCircularBufferParam_X.Overwrite_B = _Overwrite_B;
  return BOF_ERR_NO_ERROR;
}
template <typename DataType>
uint32_t BofCircularBuffer<DataType>::GetMaxLevel() const
{
  uint32_t Rts_U32 = mLevelMax_U32;
  return Rts_U32;
}

template <typename DataType>
bool BofCircularBuffer<DataType>::IsBufferOverflow()
{
  bool Rts_B = mOverflow_B;
  mOverflow_B = false;
  return Rts_B;
}
template <typename DataType>
uint32_t BofCircularBuffer<DataType>::GetPushIndex()
{
  return mPushIndex_U32;
}
template <typename DataType>
uint32_t BofCircularBuffer<DataType>::GetPopIndex()
{
  return mPopIndex_U32;
}
template <typename DataType>
void BofCircularBuffer<DataType>::Reset()
{
  BOFERR Sts_E;
  BOF_CIRCULAR_BUFFER_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    mOverflow_B = false;
    mLevelMax_U32 = 0;
    mPushIndex_U32 = 0;
    mPopIndex_U32 = 0;
    mNbElementInBuffer_U32 = 0;
    mNbElementLockedInBuffer_U32 = 0;
    if (mpLock_U8)
    {
      memset(mpLock_U8, 0, mCircularBufferParam_X.NbMaxElement_U32 * sizeof(uint8_t));
    }
    BOF_CIRCULAR_BUFFER_UNLOCK();
  }
}

template <typename DataType>
DataType *BofCircularBuffer<DataType>::GetInternalDataBuffer() const
{
  return mpData_T;
}

template <typename DataType>
BOFERR BofCircularBuffer<DataType>::Push(const DataType *_pData, uint32_t _BlockingTimeouItInMs_U32, uint32_t *_pIndexOf_U32, DataType **_ppStorage_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  if (mpLock_U8 == nullptr)
  {
    printf("jj");
  }

  if (_pData)
  {
  RetryPush:
    Rts_E = ((mCircularBufferParam_X.Blocking_B) && (_BlockingTimeouItInMs_U32)) ? Bof_WaitForEvent(mCanWriteEvent_X, _BlockingTimeouItInMs_U32, 0) : BOF_ERR_NO_ERROR;
    //		printf("@@%d@--->PushIn %s nb %d/%d pop %d push %d islock %d block %d blockto %d err %s\n", BOF::Bof_GetMsTickCount(),mCanReadEvent_X.Name_S.c_str(),	mNbElementInBuffer_U32, mNbElementLockedInBuffer_U32, mPopIndex_U32, mPushIndex_U32,
    // mpLock_U8[mPushIndex_U32], mCircularBufferParam_X.Blocking_B, _BlockingTimeouItInMs_U32, Bof_ErrorCode(Rts_E));

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      BOF_CIRCULAR_BUFFER_LOCK(Rts_E);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        // Buffer is dimensioned to contains a an integer number of "_pData"->there is no  read/write pointer clipping during an "atomic" push or pop
        if ((mCircularBufferParam_X.Overwrite_B) || (mNbElementInBuffer_U32 < mCircularBufferParam_X.NbMaxElement_U32))
        {
          if (mpLock_U8[mPushIndex_U32] == 0)
          {
            if (_pIndexOf_U32)
            {
              *_pIndexOf_U32 = mPushIndex_U32;
            }
            if (_ppStorage_X)
            {
              *_ppStorage_X = &mpData_T[mPushIndex_U32];
            }
            mpData_T[mPushIndex_U32] = *_pData;
            BOF_ASSERT(mPushIndex_U32 < mCircularBufferParam_X.NbMaxElement_U32);
            mPushIndex_U32++;
            if (mPushIndex_U32 >= mCircularBufferParam_X.NbMaxElement_U32)
            {
              mPushIndex_U32 = 0;
            }
            mNbElementInBuffer_U32++;
            if (mCircularBufferParam_X.Overwrite_B)
            {
              BOF_ASSERT(mNbElementInBuffer_U32 <= mCircularBufferParam_X.NbMaxElement_U32 + 1);
              if (mNbElementInBuffer_U32 > mCircularBufferParam_X.NbMaxElement_U32)
              {
                mNbElementInBuffer_U32 = mCircularBufferParam_X.NbMaxElement_U32; // mCircularBufferParam_X.Overwrite_B
                mPopIndex_U32 = mPushIndex_U32;
                mOverflow_B = true;
              }
            }
            else
            {
              BOF_ASSERT(mNbElementInBuffer_U32 <= mCircularBufferParam_X.NbMaxElement_U32);
            }
            if (mNbElementInBuffer_U32 > mLevelMax_U32)
            {
              mLevelMax_U32 = mNbElementInBuffer_U32;
            }

            Rts_E = BOF_ERR_NO_ERROR;
          }
          else
          {
            Rts_E = BOF_ERR_LOCK;
            mOverflow_B = true;
          }
        }
        else
        {
          Rts_E = BOF_ERR_FULL;
          mOverflow_B = true;
        }

        if (mCircularBufferParam_X.Blocking_B) //&& (_BlockingTimeouItInMs_U32))
        {
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            Rts_E = SignalReadWrite();
          }
          else
          {
            if (_BlockingTimeouItInMs_U32)
            {
              if (Rts_E == BOF_ERR_FULL)
              {
                BOF_CIRCULAR_BUFFER_UNLOCK();
                goto RetryPush; // We have been preempt between Bof_WaitForEvent and Bof_LockMutex
              }
            }
          }
        }
        BOF_CIRCULAR_BUFFER_UNLOCK();
      }
    }
  }

  return Rts_E;
}

template <typename DataType>
BOFERR BofCircularBuffer<DataType>::PushForNextPop(const DataType *_pData, bool _ForceIfFull_B, uint32_t _BlockingTimeouItInMs_U32)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if (_pData)
  {
  RetryInsert:
    Rts_E = ((mCircularBufferParam_X.Blocking_B) && (_BlockingTimeouItInMs_U32) && (!_ForceIfFull_B)) ? Bof_WaitForEvent(mCanWriteEvent_X, _BlockingTimeouItInMs_U32, 0) : BOF_ERR_NO_ERROR;
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      BOF_CIRCULAR_BUFFER_LOCK(Rts_E);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        // Buffer is dimensioned to contains a an integer number of "_pData"->there is no  read/write pointer clipping during an "atomic" push or pop
        if ((mCircularBufferParam_X.Overwrite_B) || (_ForceIfFull_B) || (mNbElementInBuffer_U32 < mCircularBufferParam_X.NbMaxElement_U32))
        {
          if (mPopIndex_U32 != mPushIndex_U32)
          {
            BOF_ASSERT(mpLock_U8[mPushIndex_U32] == 0);
            BOF_ASSERT(mNbElementInBuffer_U32 < mCircularBufferParam_X.NbMaxElement_U32);
            mNbElementInBuffer_U32++;
            if (mCircularBufferParam_X.PopLockMode_B)
            {
              BOF_ASSERT(mNbElementLockedInBuffer_U32 < mCircularBufferParam_X.NbMaxElement_U32);
              mNbElementLockedInBuffer_U32++;
            }
          }
          BOF_ASSERT(mNbElementLockedInBuffer_U32 <= mNbElementInBuffer_U32);
          if (mPopIndex_U32 == 0)
          {
            mPopIndex_U32 = mCircularBufferParam_X.NbMaxElement_U32 - 1;
          }
          else
          {
            mPopIndex_U32--;
          }
          BOF_ASSERT(mPopIndex_U32 < mCircularBufferParam_X.NbMaxElement_U32);
          // NO BOF_ASSERT(mpLock_U8[mPopIndex_U32] == 0);
          if (mCircularBufferParam_X.PopLockMode_B)
          {
            mpLock_U8[mPopIndex_U32] = 1;
          }
          else
          {
            mpLock_U8[mPopIndex_U32] = 0;
          }
          mpData_T[mPopIndex_U32] = *_pData;

          if (mCircularBufferParam_X.Overwrite_B)
          {
            BOF_ASSERT(mNbElementInBuffer_U32 <= mCircularBufferParam_X.NbMaxElement_U32 + 1);
            if (mNbElementInBuffer_U32 > mCircularBufferParam_X.NbMaxElement_U32)
            {
              mNbElementInBuffer_U32 = mCircularBufferParam_X.NbMaxElement_U32; // mCircularBufferParam_X.Overwrite_B
              BOF_ASSERT(mPushIndex_U32 == mPopIndex_U32)
              mPopIndex_U32++;
              if (mPopIndex_U32 >= mCircularBufferParam_X.NbMaxElement_U32)
              {
                mPopIndex_U32 = 0;
              }
              mOverflow_B = true;
            }
          }
          else
          {
            BOF_ASSERT(mNbElementInBuffer_U32 <= mCircularBufferParam_X.NbMaxElement_U32);
          }
          if (mNbElementInBuffer_U32 > mLevelMax_U32)
          {
            mLevelMax_U32 = mNbElementInBuffer_U32;
          }

          Rts_E = BOF_ERR_NO_ERROR;
        }
        else
        {
          Rts_E = BOF_ERR_FULL;
          mOverflow_B = true;
        }

        if (mCircularBufferParam_X.Blocking_B) //&& (_BlockingTimeouItInMs_U32))
        {
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            Rts_E = SignalReadWrite();
          }
          else
          {
            if (_BlockingTimeouItInMs_U32)
            {
              if (Rts_E == BOF_ERR_FULL)
              {
                BOF_CIRCULAR_BUFFER_UNLOCK();
                goto RetryInsert; // We have been preempt between Bof_WaitForEvent and Bof_LockMutex
              }
            }
          }
        }
        BOF_CIRCULAR_BUFFER_UNLOCK();
      }
    }
  }
  return Rts_E;
}

//_ppStorage is mainly used in mCircularBufferParam_X.PopLockMode_B to provide write access to the locked storage cell
template <typename DataType>
BOFERR BofCircularBuffer<DataType>::Pop(DataType *_pData, uint32_t _BlockingTimeouItInMs_U32, uint32_t *_pIndexOf_U32, DataType **_ppStorage)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  // if (_pData)
  {
  RetryPop:
    // printf("%d: Bof_WaitForEvent blk %d To %d\n", BOF::Bof_GetMsTickCount(), mCircularBufferParam_X.Blocking_B, _BlockingTimeouItInMs_U32);
    Rts_E = ((mCircularBufferParam_X.Blocking_B) && (_BlockingTimeouItInMs_U32) && (mNbElementInBuffer_U32 == 0)) ? Bof_WaitForEvent(mCanReadEvent_X, _BlockingTimeouItInMs_U32, 0) : BOF_ERR_NO_ERROR;
    // printf("%d: Bof_WaitForEvent end with %d\n", BOF::Bof_GetMsTickCount(), Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      BOF_CIRCULAR_BUFFER_LOCK(Rts_E);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        // Buffer is dimensioned to contains a an integer number of "_pData"->there is no  read/write pointer clipping during an "atomic" push or pop
        BOF_ASSERT(mNbElementLockedInBuffer_U32 <= mNbElementInBuffer_U32);
        if (mNbElementInBuffer_U32 - mNbElementLockedInBuffer_U32)
        {
          if (mpLock_U8[mPopIndex_U32] == 0)
          {
            if (_pIndexOf_U32)
            {
              *_pIndexOf_U32 = mPopIndex_U32;
            }
            if (_pData)
            {
              *_pData = mpData_T[mPopIndex_U32];
            }
            if (_ppStorage)
            {
              *_ppStorage = &mpData_T[mPopIndex_U32];
            }
            if (mCircularBufferParam_X.PopLockMode_B)
            {
              BOF_ASSERT(mNbElementLockedInBuffer_U32 < mCircularBufferParam_X.NbMaxElement_U32);
              mNbElementLockedInBuffer_U32++;
              mpLock_U8[mPopIndex_U32] = 1;
            }
            else
            {
              BOF_ASSERT(mNbElementInBuffer_U32);
              BOF_ASSERT(mNbElementInBuffer_U32 <= mCircularBufferParam_X.NbMaxElement_U32);
              mNbElementInBuffer_U32--;
            }
            BOF_ASSERT(mPopIndex_U32 < mCircularBufferParam_X.NbMaxElement_U32);
            mPopIndex_U32++;
            if (mPopIndex_U32 >= mCircularBufferParam_X.NbMaxElement_U32)
            {
              mPopIndex_U32 = 0;
            }

            Rts_E = BOF_ERR_NO_ERROR;
          }
          else
          {
            Rts_E = BOF_ERR_LOCK;
            ;
          }
        }
        else
        {
          Rts_E = BOF_ERR_EMPTY;
        }

        if (mCircularBufferParam_X.Blocking_B) //&& (_BlockingTimeouItInMs_U32))
        {
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            Rts_E = SignalReadWrite();
          }
          else
          {
            if (_BlockingTimeouItInMs_U32)
            {
              if (Rts_E == BOF_ERR_EMPTY)
              {
                BOF_CIRCULAR_BUFFER_UNLOCK();
                goto RetryPop; // We have been preempt between Bof_WaitForEvent and Bof_LockMutex
              }
            }
          }
        }
        BOF_CIRCULAR_BUFFER_UNLOCK();
      }
    }
  }
  //	printf("@@%d@--->PopOut %s LOCKIT %d nb %d/%d pop %d push %d islock %d block %d blockto %d err %s\n",BOF::Bof_GetMsTickCount(), mCanReadEvent_X.Name_S.c_str(),mCircularBufferParam_X.PopLockMode_B, mNbElementInBuffer_U32, mNbElementLockedInBuffer_U32,
  // mPopIndex_U32, mPushIndex_U32, mpLock_U8[mPopIndex_U32], mCircularBufferParam_X.Blocking_B, _BlockingTimeouItInMs_U32, Bof_ErrorCode(Rts_E));

  return Rts_E;
}

template <typename DataType>
BOFERR BofCircularBuffer<DataType>::PopLastPush(DataType *_pData, uint32_t *_pIndexOf_U32, DataType **_ppStorage)
{
  BOFERR Rts_E;

  BOF_CIRCULAR_BUFFER_LOCK(Rts_E);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    BOF_ASSERT(mNbElementLockedInBuffer_U32 <= mNbElementInBuffer_U32);
    if (mNbElementInBuffer_U32 - mNbElementLockedInBuffer_U32)
    {
      BOF_ASSERT(mPushIndex_U32 < mCircularBufferParam_X.NbMaxElement_U32);
      mPushIndex_U32--;
      if (mPushIndex_U32 >= mCircularBufferParam_X.NbMaxElement_U32) // From 0 to -1 OxFFFFFFFF which is bigger
      {
        mPushIndex_U32 = mCircularBufferParam_X.NbMaxElement_U32 - 1;
      }

      if (mpLock_U8[mPushIndex_U32])
      {
        mNbElementLockedInBuffer_U32--;
        mpLock_U8[mPushIndex_U32] = 0;
      }
      if (_pIndexOf_U32)
      {
        *_pIndexOf_U32 = mPushIndex_U32;
      }
      if (_pData)
      {
        *_pData = mpData_T[mPushIndex_U32];
      }
      if (_ppStorage)
      {
        *_ppStorage = &mpData_T[mPushIndex_U32];
      }

      BOF_ASSERT(mNbElementInBuffer_U32);
      BOF_ASSERT(mNbElementInBuffer_U32 <= mCircularBufferParam_X.NbMaxElement_U32);
      mNbElementInBuffer_U32--;

      Rts_E = BOF_ERR_NO_ERROR;
    }
    else
    {
      Rts_E = BOF_ERR_EMPTY;
    }

    if (mCircularBufferParam_X.Blocking_B)
    {
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Rts_E = SignalReadWrite();
      }
    }
    BOF_CIRCULAR_BUFFER_UNLOCK();
  }
  return Rts_E;
}

//_ppStorage is mainly used in mCircularBufferParam_X.PopLockMode_B to provide write access to the locked storage cell
// Use peek and then pop when treatment is finished..
template <typename DataType>
BOFERR BofCircularBuffer<DataType>::Peek(DataType *_pData, uint32_t _BlockingTimeouItInMs_U32, uint32_t *_pIndexOf_U32, DataType **_ppStorage)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  Rts_E = ((mCircularBufferParam_X.Blocking_B) && (_BlockingTimeouItInMs_U32) && (mNbElementInBuffer_U32 == 0)) ? Bof_WaitForEvent(mCanReadEvent_X, _BlockingTimeouItInMs_U32, 0) : BOF_ERR_NO_ERROR;
  //  			printf("@@%d@--->PopIn %s LOCKIT %d nb %d/%d pop %d push %d islock %d block %d blockto %d err %s\n",BOF::Bof_GetMsTickCount(), mCanReadEvent_X.Name_S.c_str(),mCircularBufferParam_X.PopLockMode_B, mNbElementInBuffer_U32,
  //  mNbElementLockedInBuffer_U32, mPopIndex_U32, mPushIndex_U32, mpLock_U8[mPopIndex_U32], mCircularBufferParam_X.Blocking_B, _BlockingTimeouItInMs_U32, Bof_ErrorCode(Rts_E));
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    BOF_CIRCULAR_BUFFER_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      // Buffer is dimensioned to contains a an integer number of "_pData"->there is no  read/write pointer clipping during an "atomic" push or pop
      BOF_ASSERT(mNbElementLockedInBuffer_U32 <= mNbElementInBuffer_U32);
      if (mNbElementInBuffer_U32 - mNbElementLockedInBuffer_U32)
      {
        if (mpLock_U8[mPopIndex_U32] == 0)
        {
          if (_pIndexOf_U32)
          {
            *_pIndexOf_U32 = mPopIndex_U32;
          }
          if (_pData)
          {
            *_pData = mpData_T[mPopIndex_U32];
          }
          if (_ppStorage)
          {
            *_ppStorage = &mpData_T[mPopIndex_U32];
          }
          Rts_E = BOF_ERR_NO_ERROR;
        }
        else
        {
          Rts_E = BOF_ERR_LOCK;
        }
      }
      else
      {
        Rts_E = BOF_ERR_EMPTY;
      }
      BOF_CIRCULAR_BUFFER_UNLOCK();
    }
  }
  return Rts_E;
}

template <typename DataType>
BOFERR BofCircularBuffer<DataType>::PeekFromPop(uint32_t _RelativeIndexFromPop_U32, DataType *_pData, bool *_pLocked_B, DataType **_ppStorage)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint32_t Index_U32;

  // if (_pData)
  {
    BOF_CIRCULAR_BUFFER_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (_RelativeIndexFromPop_U32 < mNbElementInBuffer_U32)
      {
        Index_U32 = mPopIndex_U32 + _RelativeIndexFromPop_U32;

        if (Index_U32 >= mCircularBufferParam_X.NbMaxElement_U32)
        {
          Index_U32 -= mCircularBufferParam_X.NbMaxElement_U32;
        }
        if (Index_U32 < mPushIndex_U32)
        {
          if (_pData)
          {
            *_pData = mpData_T[Index_U32];
          }
          if (_ppStorage)
          {
            *_ppStorage = &mpData_T[Index_U32];
          }
          if (_pLocked_B)
          {
            *_pLocked_B = mpLock_U8[Index_U32];
          }
          Rts_E = BOF_ERR_NO_ERROR;
        }
        else
        {
          Rts_E = BOF_ERR_NOT_AVAILABLE;
        }
      }
      else
      {
        Rts_E = BOF_ERR_EMPTY;
      }
      BOF_CIRCULAR_BUFFER_UNLOCK();
    }
  }
  return Rts_E;
}

template <typename DataType>
BOFERR BofCircularBuffer<DataType>::PeekByIndex(uint32_t _AbsoluteIndex_U32, DataType *_pData, bool *_pLocked_B, DataType **_ppStorage)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  // if (_pData)
  {
    if (_AbsoluteIndex_U32 < mCircularBufferParam_X.NbMaxElement_U32)
    {
      BOF_CIRCULAR_BUFFER_LOCK(Rts_E);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        if (_pData)
        {
          *_pData = mpData_T[_AbsoluteIndex_U32];
        }
        if (_ppStorage)
        {
          *_ppStorage = &mpData_T[_AbsoluteIndex_U32];
        }
        if (_pLocked_B)
        {
          *_pLocked_B = mpLock_U8[_AbsoluteIndex_U32] ? true : false;
        }
        BOF_CIRCULAR_BUFFER_UNLOCK();
      }
    }
    else
    {
      Rts_E = BOF_ERR_NOT_AVAILABLE;
    }
  }
  return Rts_E;
}

//_SignalIfNeeded_B:: sometime you skip just to process OverWrite mode, in this case do not signal free space
template <typename DataType>
BOFERR BofCircularBuffer<DataType>::Skip(DataType *_pData, bool _SignalIfNeeded_B, uint32_t *_pIndexOf_U32, DataType **_ppStorage, bool *_pLocked_B)
{
  BOFERR Rts_E;

  BOF_CIRCULAR_BUFFER_LOCK(Rts_E);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    // Buffer is dimensioned to contains a an integer number of "_pData"->there is no  read/write pointer
    // clipping during an "atomic" push or pop
    BOF_ASSERT(mNbElementLockedInBuffer_U32 <= mNbElementInBuffer_U32);
    if (mNbElementInBuffer_U32 - mNbElementLockedInBuffer_U32)
    {
      if (_pLocked_B)
      {
        *_pLocked_B = mpLock_U8[mPopIndex_U32] ? true : false;
      }
      if (mpLock_U8[mPopIndex_U32] == 0)
      {
        BOF_ASSERT(mNbElementInBuffer_U32);
        BOF_ASSERT(mNbElementInBuffer_U32 <= mCircularBufferParam_X.NbMaxElement_U32);
        mNbElementInBuffer_U32--;

        BOF_ASSERT(mPopIndex_U32 < mCircularBufferParam_X.NbMaxElement_U32);
        if (_pIndexOf_U32)
        {
          *_pIndexOf_U32 = mPopIndex_U32;
        }
        if (_pData)
        {
          *_pData = mpData_T[mPopIndex_U32];
        }
        if (_ppStorage)
        {
          *_ppStorage = &mpData_T[mPopIndex_U32];
        }
        mPopIndex_U32++;
        if (mPopIndex_U32 >= mCircularBufferParam_X.NbMaxElement_U32)
        {
          mPopIndex_U32 = 0;
        }
        if (mCircularBufferParam_X.Blocking_B) //&& (_BlockingTimeouItInMs_U32))
        {
          if (_SignalIfNeeded_B)
          {
            Rts_E = SignalReadWrite();
          }
        }
      }
      else
      {
        Rts_E = BOF_ERR_LOCK;
      }
    }
    else
    {
      Rts_E = BOF_ERR_EMPTY;
    }
    BOF_CIRCULAR_BUFFER_UNLOCK();
  }
  return Rts_E;
}

template <typename DataType>
BOFERR BofCircularBuffer<DataType>::UnlockPop(uint32_t _AbsoluteIndex_U32)
{
  BOFERR Rts_E = BOF_ERR_WRONG_MODE;
  //	uint32_t i_U32;

  //	printf("@@%d@UnlockIn %d/%d lock %d nb %d/%d pop %d push %d\n", BOF::Bof_GetMsTickCount(),_AbsoluteIndex_U32, mCircularBufferParam_X.NbMaxElement_U32, mpLock_U8[_AbsoluteIndex_U32],
  // mNbElementInBuffer_U32,mNbElementLockedInBuffer_U32,mPopIndex_U32,mPushIndex_U32);

  if (mCircularBufferParam_X.PopLockMode_B)
  {
    Rts_E = BOF_ERR_TOO_BIG;
    if (_AbsoluteIndex_U32 < mCircularBufferParam_X.NbMaxElement_U32)
    {
      BOF_CIRCULAR_BUFFER_LOCK(Rts_E);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        if (mpLock_U8[_AbsoluteIndex_U32])
        {
          mpLock_U8[_AbsoluteIndex_U32] = 0;
          BOF_ASSERT(mNbElementInBuffer_U32);
          BOF_ASSERT(mNbElementInBuffer_U32 <= mCircularBufferParam_X.NbMaxElement_U32);
          mNbElementInBuffer_U32--;
          BOF_ASSERT(mNbElementLockedInBuffer_U32);
          BOF_ASSERT(mNbElementLockedInBuffer_U32 <= mCircularBufferParam_X.NbMaxElement_U32);
          mNbElementLockedInBuffer_U32--;
          BOF_ASSERT(mNbElementLockedInBuffer_U32 <= mNbElementInBuffer_U32);
          Rts_E = SignalReadWrite();
        }
        else
        {
          Rts_E = BOF_ERR_UNLOCK;
        }
        BOF_CIRCULAR_BUFFER_UNLOCK();
      }
    }
  }
  //	printf("@@%d@UnlockOut %d/%d lock %d nb %d/%d pop %d push %d Rts %x\n", BOF::Bof_GetMsTickCount(),_AbsoluteIndex_U32, mCircularBufferParam_X.NbMaxElement_U32, mpLock_U8[_AbsoluteIndex_U32],mNbElementInBuffer_U32,mNbElementLockedInBuffer_U32,
  // mPopIndex_U32, mPushIndex_U32, Rts_E);

  return Rts_E;
}

template <typename DataType>
bool BofCircularBuffer<DataType>::IsLocked(uint32_t _AbsoluteIndex_U32)
{
  bool Rts_B = false;
  BOFERR Sts_E;
  if (_AbsoluteIndex_U32 < mCircularBufferParam_X.NbMaxElement_U32)
  {
    BOF_CIRCULAR_BUFFER_LOCK(Sts_E);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      Rts_B = (mpLock_U8[_AbsoluteIndex_U32] != 0);
      BOF_CIRCULAR_BUFFER_UNLOCK();
    }
  }
  return Rts_B;
}

template <typename DataType>
bool BofCircularBuffer<DataType>::IsEntryFree(uint32_t _AbsoluteIndex_U32, bool *_pIsLocked_B, DataType **_ppStorage)
{
  bool Rts_B = false;
  // uint32_t Index_U32, EndPush_U32;
  BOFERR Sts_E;

  BOF_CIRCULAR_BUFFER_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    //		printf("@@%d@IsEntryFreeIn %d/%d lock %d nb %d pop %d push %d\n", BOF::Bof_GetMsTickCount(),_AbsoluteIndex_U32, mCircularBufferParam_X.NbMaxElement_U32, mpLock_U8[_AbsoluteIndex_U32],mNbElementInBuffer_U32, mPopIndex_U32,mPushIndex_U32);

    if (_AbsoluteIndex_U32 < mCircularBufferParam_X.NbMaxElement_U32)
    {
      if (mNbElementInBuffer_U32) // At least one element
      {
        if (mNbElementInBuffer_U32 < mCircularBufferParam_X.NbMaxElement_U32) // If == full->not free
        {
          if (mPopIndex_U32 <= mPushIndex_U32)
          {
            if ((_AbsoluteIndex_U32 >= mPushIndex_U32) || (_AbsoluteIndex_U32 < mPopIndex_U32))
            {
              Rts_B = mpLock_U8[_AbsoluteIndex_U32] ? false : true;
            }
          }
          else
          {
            if ((_AbsoluteIndex_U32 >= mPushIndex_U32) && (_AbsoluteIndex_U32 < mPopIndex_U32))
            {
              Rts_B = mpLock_U8[_AbsoluteIndex_U32] ? false : true;
            }
          }
        }
      }
      else
      {
        Rts_B = true; // Empty
      }
    }
    if (_pIsLocked_B)
    {
      *_pIsLocked_B = mpLock_U8[_AbsoluteIndex_U32] ? true : false;
    }
    if (_ppStorage)
    {
      *_ppStorage = &mpData_T[_AbsoluteIndex_U32];
    }
    BOF_CIRCULAR_BUFFER_UNLOCK();
  }
  //	printf("@@%d@IsEntryFreeOut %d/%d lock %d nb %d pop %d push %d->%X\n", BOF::Bof_GetMsTickCount(),_AbsoluteIndex_U32, mCircularBufferParam_X.NbMaxElement_U32, mpLock_U8[_AbsoluteIndex_U32],mNbElementInBuffer_U32, mPopIndex_U32,mPushIndex_U32,Rts_B);
  return Rts_B;
}

template <typename DataType>
BOFERR BofCircularBuffer<DataType>::SignalReadWrite()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;

  if (mCircularBufferParam_X.Blocking_B)
  {
    if (mNbElementInBuffer_U32 - mNbElementLockedInBuffer_U32)
    {
      if (mpLock_U8[mPopIndex_U32] == 0)
      {
        Rts_E = Bof_SignalEvent(mCanReadEvent_X, 0);
        //        printf("@@%d@Signalread %s nb %d/%d pop %d push %d Rts %x\n", BOF::Bof_GetMsTickCount(), mCanReadEvent_X.Name_S.c_str(), mNbElementInBuffer_U32, mNbElementLockedInBuffer_U32, mPopIndex_U32, mPushIndex_U32, Rts_E);
      }
    }
    if ((mCircularBufferParam_X.Overwrite_B) || (mNbElementInBuffer_U32 < mCircularBufferParam_X.NbMaxElement_U32))
    {
      if (mpLock_U8[mPushIndex_U32] == 0)
      {
        Rts_E = Bof_SignalEvent(mCanWriteEvent_X, 0);
        //        printf("@@%d@SignalWRITE %s nb %d/%d pop %d push %d Rts %x\n", BOF::Bof_GetMsTickCount(), mCanReadEvent_X.Name_S.c_str(), mNbElementInBuffer_U32, mNbElementLockedInBuffer_U32, mPopIndex_U32, mPushIndex_U32, Rts_E);
      }
    }
  }
  return Rts_E;
}

template <typename DataType>
std::string BofCircularBuffer<DataType>::StateInfo()
{
  std::string Rts_S;
  uint32_t i_U32, Nb_U32;
  char pDbg_c[((BOF_CIRCULAR_BUFFER_DBG_MAX_ITEM + 1) * 2) + 1]; //+1 \n +1 nullterm
  BOFERR Sts_E;

  BOF_CIRCULAR_BUFFER_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    Rts_S = Bof_Sprintf("LckMod %d Pop %d Push %d Nb %d/%d Lck %d (%p) OverWrt %d PreAlloc %d pData %p LvlMx %d (%d) R %d W %d B %d E %X\n", mCircularBufferParam_X.PopLockMode_B, mPopIndex_U32, mPushIndex_U32, mNbElementInBuffer_U32,
                        mCircularBufferParam_X.NbMaxElement_U32, mNbElementLockedInBuffer_U32, mpLock_U8, mCircularBufferParam_X.Overwrite_B, mDataPreAllocated_B, mpData_T, mLevelMax_U32, mOverflow_B, Bof_IsEventSignaled(mCanReadEvent_X, 0),
                        Bof_IsEventSignaled(mCanWriteEvent_X, 0), mCircularBufferParam_X.Blocking_B, mErrorCode_E);

    Nb_U32 = std::min(mCircularBufferParam_X.NbMaxElement_U32, BOF_CIRCULAR_BUFFER_DBG_MAX_ITEM);
    for (i_U32 = 0; i_U32 < Nb_U32; i_U32++)
    {
      if (mPopIndex_U32 == mPushIndex_U32)
      {
        pDbg_c[i_U32] = (i_U32 == mPopIndex_U32) ? 'V' : '.';
      }
      else
      {
        pDbg_c[i_U32] = (i_U32 == mPopIndex_U32) ? 'R' : (i_U32 == mPushIndex_U32) ? 'W'
                                                                                   : '.';
      }
      if (mpLock_U8)
      {
        pDbg_c[i_U32 + Nb_U32 + 1] = (mpLock_U8[i_U32]) ? 'L' : 'f';
      }
    }
    pDbg_c[Nb_U32] = '\n';
    pDbg_c[Nb_U32 + Nb_U32 + 1] = '\n';
    pDbg_c[Nb_U32 + Nb_U32 + 2] = 0;
    Rts_S += pDbg_c;
    BOF_CIRCULAR_BUFFER_UNLOCK();
  }
  return Rts_S;
}
END_BOF_NAMESPACE()