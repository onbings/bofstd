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
 * Name:        BofStringCircularBuffer.h
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

#include <bofstd/bofsystem.h>

#include <cstdint>
#include <string.h>

BEGIN_BOF_NAMESPACE()

struct BOF_STRING_CIRCULAR_BUFFER_PARAM
{
  bool MultiThreadAware_B;       /*! true if the object is used in a multi threaded application (use mCs)*/
  bool PriorityInversionAware_B;
  bool SnapshotMode_B;           /*! true if the buffer works in 'SnapShot' Mode. In this case the read (Pop) behavior of the buffer is modified. The Pop method does not remove the element read*/
  uint32_t BufferSizeInByte_U32; /*! Specifies the maximum number of element inside the queue*/
  char *pData_c;                 /*! Specifies a pointer to the circular buffer zone (pre-allocated buffer). Set to nullptr if the memory must be allocated by the function*/
  bool Overwrite_B;              /*! true if new data overwrite the oldest one when the queue is full. */
  bool Blocking_B;

  BOF_STRING_CIRCULAR_BUFFER_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    MultiThreadAware_B = false;
    PriorityInversionAware_B=false;
    SnapshotMode_B = false;
    BufferSizeInByte_U32 = 0;
    pData_c = nullptr;
    Overwrite_B = false;
    Blocking_B = false;
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

class BOFSTD_EXPORT BofStringCircularBuffer
{
private:
  BOF_STRING_CIRCULAR_BUFFER_PARAM mStringCircularBufferParam_X;
  uint32_t mPopSnapshotIndex_U32;       /*! Current position of the read index inside the queue in snapshot mode*/
  bool mDataPreAllocated_B;             /*! true if mpData_U8 is provided by the caller*/
  bool mOverflow_B;                     /*! true if data overflow has occured. Reset to false by IsBufferOverflow*/
  char *mpData_c;                       /*! Pointer to queue storage buffer used to record queue element*/
  uint32_t mPushIndex_U32;              /*! Current position of the write index inside the queue*/
  uint32_t mPopIndex_U32;               /*! Current position of the read index inside the queue*/
  uint32_t mNbElementInBuffer_U32;      /*! Current number of string inside the queue*/
  uint32_t mNbCharInBuffer_U32;         /*! Current number of char inside the queue*/
  uint32_t mNbSnapshotCharInBuffer_U32; /*! copy of mNbCharInBuffer_U32 used for pop Snapshot op*/
  uint32_t mLevelMax_U32;               /*! Contains the maximum buffer fill level. This one is reset by the GetMaxLevel method*/
  BOF_MUTEX mStringCircularBufferMtx_X; /*! Provide a serialized access to shared resources in a multi threaded environement*/
  BOFERR mErrorCode_E;
  BOF_EVENT mCanReadEvent_X;
  BOF_EVENT mCanWriteEvent_X;
  // bool                             mBlocking_B;

public:
  BofStringCircularBuffer(const BOF_STRING_CIRCULAR_BUFFER_PARAM &_rStringCircularBufferParam_X);
  virtual ~BofStringCircularBuffer();

  BofStringCircularBuffer &operator=(const BofStringCircularBuffer &) = delete; // Disallow copying
  BofStringCircularBuffer(const BofStringCircularBuffer &) = delete;

  BOFERR LastErrorCode();
  BOFERR LockStringCircularBuffer();
  BOFERR UnlockStringCircularBuffer();
  bool IsEmpty();
  bool IsFull();
  uint32_t GetCapacity();
  uint32_t GetNbElement();
  uint32_t GetNbChar();
  uint32_t GetNbFreeChar();
  BOFERR PushBinary(uint32_t _Size_U32, const char *_pData_c, uint32_t _BlockingTimeouItInMs_U32);
  BOFERR PushString(const char *_pData_c, uint32_t _BlockingTimeouItInMs_U32);
  BOFERR PopString(uint32_t *_pNbMax_U32, char *_pData_c, uint32_t _BlockingTimeouItInMs_U32);
  bool IsBufferOverflow();
  uint32_t GetMaxLevel();
  void Reset();
  uint32_t ResyncSnapshot(uint32_t _Index_U32);

private:
  // uint32_t                           ReSync();
};
END_BOF_NAMESPACE()