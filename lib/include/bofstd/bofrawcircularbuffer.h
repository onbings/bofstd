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
 * Name:        BofRawCircularBuffer.h
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

BEGIN_BOF_NAMESPACE()

struct BOFSTD_EXPORT BOF_RAW_CIRCULAR_BUFFER_PARAM
{
  bool MultiThreadAware_B;       /*! true if the object is used in a multi threaded application (use mCs)*/
  uint32_t BufferSizeInByte_U32; /*!	Specifies the maximum number of byte inside inside the queue*/
  uint32_t NbMaxSlot_U32;        /*! Specifies the maximum number of buffer inside the queue (if 0 the whole BufferSizeInByte_U32 can be used)*/
  uint8_t *pData_U8;             /*! Pointer to queue storage buffer used to record queue element*/
  BOF_RAW_CIRCULAR_BUFFER_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    MultiThreadAware_B = false;
    BufferSizeInByte_U32 = 0;
    NbMaxSlot_U32 = 0;
    pData_U8 = nullptr;
  }
};

/*!
 * Summary
 * Raw Circular buffer class
 *
 * Description
 * This class manages a circular byte buffer instance. Each entry in the circular buffer is
 * preceded by a 32 bits value containing the length of the payload:
 *
 *	L1: <L1 databyte> | L2: <L2 databyte> | ... | Ln: <Ln databyte>
 *
 * The effective range of this value is [1,0x7FFFFFFF]. When bit 31 is set, this location has been
 * reserved (locked) by LockBuffer and it will be filled by UnlockLockBuffer which will clear the bit 31
 * When this bit is set PopBuffer will fail even if the state of the buffer seems to be ready for pop
 * operation. PushBuffer can continue to work
 *
 * The filling of the buffer is controlled by BufferCapacity_U32 and NbMaxBuffer_U32
 *
 * BufferCapacity_U32 gives the number of byte allocated to store the list of buffers
 *
 * Each pushed buffer is stored one after each other if NbMaxBuffer_U32 is 0. In this case
 * the buffer capacity is optimized used in term of memory
 *	L1:<L1 databyte>L2:<L2 databyte>...Ln:<Ln databyte>
 *
 * If NbMaxBuffer_U32 is different from 0, each buffer will be stored in a slot of
 * BufferCapacity_U32/NbMaxBuffer_U32 bytes. This will gives slot of equal size inside the
 * circular buffer which contains variable size buffer array from 1 to
 * (BufferCapacity_U32/NbMaxBuffer_U32) bytes length. In this cas the BufferCapacity_U32 is
 * increased by (NbMaxBuffer_U32*sizeof(uint32_t)) to be able to store the extra buffer 32 bits value
 * containing the length of the payload
 *	L1:<L1 databyte>          L2:<L2 databyte>          ...Ln:<Ln databyte>
 *  <--------SlotSize--------><--------SlotSize-------->...<--------SlotSize-------->
 *
 * See Also
 * None
 */

class BOFSTD_EXPORT BofRawCircularBuffer
{
private:
  BOF_RAW_CIRCULAR_BUFFER_PARAM mRawCircularBufferParam_X;
  uint32_t mNbSlot_U32; /*! Current number of buffer stored inside the raw queue */
  // uint32_t                      mNbMaxSlot_U32;                            /*! Maximum number of buffer which can be stored inside the queue. If it is 0, the whole buffer size is used to store a maximum number of buffer otherwise of constant slot size
  // is used to store variable buffer length (see class comment) */
  uint32_t mSlotSize_U32;   /*! if mNbMaxSlot_U32 != 0 this is the slot size which is used to store variable buffer length (adjusted mNbElementInBuffer_U32/mNbMaxBuffer_U32 (see class comment) otherwize 0 */
  bool mDataPreAllocated_B; /*! true if mpData_U8 is provided by the caller (!! if mNbMaxBuffer_U32 is different from zero, extra storage is needed to record lenth of each slot (see class comment) */
  uint8_t *mpData_U8;       /*! Pointer to queue storage buffer used to record queue element*/
  uint32_t mPushIndex_U32;  /*! Current position of the write index inside the queue*/
  uint32_t mPopIndex_U32;   /*! Current position of the read index inside the queue*/
  // uint32_t                      mBufferCapacity_U32;                       /*! The maximum number of element inside the queue*/
  uint32_t mNbElementInBuffer_U32; /*! Current number of element inside the queue*/
  uint32_t mLevelMax_U32;          /*! Contains the maximum buffer fill level. This one is reset by the GetMaxLevel method*/
  BOF_MUTEX mRawCbMtx_X;           /*! Provide a serialized access to shared resources in a multi threaded environment*/
  BOFERR mErrorCode_E;

public:
  BofRawCircularBuffer(const BOF_RAW_CIRCULAR_BUFFER_PARAM &_rRawCircularBufferParam_X);
  virtual ~BofRawCircularBuffer();

  BofRawCircularBuffer &operator=(const BofRawCircularBuffer &) = delete; // Disallow copying
  BofRawCircularBuffer(const BofRawCircularBuffer &) = delete;

  BOFERR LastErrorCode()
  {
    return mErrorCode_E;
  }
  BOFERR LockRawQueue();
  BOFERR UnlockRawQueue();
  bool IsEmpty()
  {
    return mNbElementInBuffer_U32 == 0;
  }
  bool IsFull()
  {
    return GetNbFreeElement() == 0;
  }
  uint32_t GetSlotSize()
  {
    return mSlotSize_U32;
  }
  uint32_t GetNbElement()
  {
    return mSlotSize_U32 ? mNbSlot_U32 : mNbElementInBuffer_U32;
  }
  uint32_t GetCapacity()
  {
    return mSlotSize_U32 ? mRawCircularBufferParam_X.NbMaxSlot_U32 : mRawCircularBufferParam_X.BufferSizeInByte_U32;
  }
  uint32_t GetMaxLevel()
  {
    return mLevelMax_U32;
  }
  void Reset();
  uint32_t GetNbFreeElement();
  BOFERR PushBuffer(uint32_t _Nb_U32, const uint8_t *_pData_U8);
  BOFERR PopBuffer(uint32_t *_pNbMax_U32, uint8_t *_pData_U8);
  BOFERR Peek(uint32_t _Index_U32, uint32_t *_pNbMax_U32, uint8_t *_pData_U8);
  BOFERR Skip();
  BOFERR LockBuffer(uint32_t _Nb_U32, uint32_t *_pNb1_U32, uint8_t **_ppData1_U8, uint32_t *_pNb2_U32, uint8_t **_ppData2_U8);
  BOFERR UnlockBuffer(const uint8_t *_pLockedBuffer_U8, uint32_t _Nb_U32);

private:
  uint32_t ReadNbHeader(uint32_t *_pPopIndex_U32);
  BOFERR WriteNbHeader(uint32_t *_pPushIndex_U32, uint32_t _Nb_U32);
  BOFERR ReadPayload(uint32_t *_pPopIndex_U32, uint32_t _Nb_U32, uint8_t *_pData_U8);
  BOFERR WritePayload(uint32_t *_pPushIndex_U32, uint32_t _Nb_U32, const uint8_t *_pData_U8);
};

END_BOF_NAMESPACE()