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
#include <bofstd/bofcircularbuffer.h>

BEGIN_BOF_NAMESPACE()

struct BOF_RAW_CIRCULAR_BUFFER_PARAM
{
  bool MultiThreadAware_B;       /*! true if the object is used in a multi threaded application (use mCs)*/
  bool PriorityInversionAware_B;
  uint32_t BufferSizeInByte_U32; /*!	Specifies the maximum number of byte inside inside the queue*/
  bool     SlotMode_B;           /* If true, the mpData_U8 buffer of BufferSizeInByte_U32 will be divided by NbMaxBufferEntry_U32 and each slot will be BufferSizeInByte_U32/NbMaxBufferEntry_U32 */
  bool     AlwaysContiguous_B;   /*! No sens for SlotMode_B (data are always contiguous in SlotMode_B). With non SlotMode_B buffer, if this parameter is true all data element stored in the buffer 
                                  will always be contiguous, so an element at the end of the buffer will never be splitted between the end and the beginning of the buffer. If this case is detected 
                                  an empy zone will be created to align the buffer to the start and if you call GetBufferPtr(..., uint32_t *_pNb1_U32, uint8_t **_ppData1_U8, uint32_t *_pNb2_U32, uint8_t **_ppData2_U8) 
                                  *_pNb2_U32 will ALWAYS BE 0 */
  uint32_t NbMaxBufferEntry_U32; /*! Specifies the maximum number of buffer which will be tracked inside the mpData_U8 buffer of BufferSizeInByte_U32*/
  uint8_t *pData_U8;             /*! Pointer to queue storage buffer used to record queue element*/
  bool Overwrite_B;          /*! true if new data overwritte the oldest one when the queue is full. */
  bool Blocking_B;

  BOF_RAW_CIRCULAR_BUFFER_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    MultiThreadAware_B = false;
    PriorityInversionAware_B = false;
    BufferSizeInByte_U32 = 0;
    SlotMode_B = false;
    NbMaxBufferEntry_U32 = 0;
    pData_U8 = nullptr;
    Overwrite_B = false;
    Blocking_B = false;
  }
};

/*!
 * Summary
 * Raw Circular buffer class
 *
 * Description
 * This class manages a circular byte buffer instance. For each entry in the circular buffer, there is an entry in 
 * in std::unique_ptr<BofCircularBuffer<BOF_RAW_BUFFER>> mpuSlotCollection.
 * BOF_RAW_BUFFER describes a buffer portion inside the global mpData_U8 buffer
 *
 *	mpData_U8: <L1 databyte> |  <L2 databyte> | ... |  <Ln databyte>
 *
 * The filling of the buffer is controlled by SlotMode_B, NbMaxBufferEntry_U32 and BufferSizeInByte_U32
 *
 * BufferSizeInByte_U32 gives the number of byte allocated to store the list of buffers
 *
 * Each pushed buffer is stored one after each other if SlotMode_B is false up to a maximum of NbMaxBufferEntry_U32
 * pending entries which must be Poped out to be able to use the rest of the buffer. 
 *
 * If SlotMode_B is true, each buffer will be stored in a slot of
 * BufferSizeInByte_U32/NbMaxBufferEntry_U32 bytes. This will gives slot of equal size inside the
 * circular buffer which contains variable size buffer array from 1 to
 * (BufferSizeInByte_U32/NbMaxBufferEntry_U32) bytes length. 
 * 
 *	<L1 databyte>             <L2 databyte>          ...Ln:<Ln databyte>
 *  <--------SlotSize--------><--------SlotSize-------->...<--------SlotSize-------->
 * 
 * Each entry in the buffer list can be read partially byte per byte up to the size of the length of this entry.
 * Data can also be appended part by part if you call SetAppendMode(true), do not forget to call SetAppendMode(false)
 * when the data is considered as completly full
 */
struct BOF_RAW_BUFFER
{
  uint32_t IndexInBuffer_U32;         //Index of the first data byte (pData_U8[0]) inside the global mpData_U8 buffer.
  uint32_t SlotEmpySpace_U32;         //Number of not used byte in slot when pushed
  uint32_t Size1_U32;                 //Number of byte from pData1_U8 to the end op the buffer (no Size2_U32 for slotmode)
  uint8_t *pData1_U8;                 //Pointer to data corresponding to Size1_U32  (no pData2_U8 for SlotMode_B)
  uint32_t Size2_U32;                 //In non slot mode, data may be spreaded accross the end of the buffer, this one contains the number of byte from mpData_U8[0] to the end of the buffer
  uint8_t *pData2_U8;                 //Pointer to data corresponding to Size1_U32  (no Size2_U32 for SlotMode_B)
  BOF_RAW_BUFFER()
  {
    Reset();
  }
  void Reset()
  {
    IndexInBuffer_U32 = 0;
    SlotEmpySpace_U32 = 0;
    Size1_U32 = 0;
    pData1_U8 = nullptr;
    Size2_U32 = 0;
    pData2_U8 = nullptr;
  }
};
class BOFSTD_EXPORT BofRawCircularBuffer
{
private:
  BOF_RAW_CIRCULAR_BUFFER_PARAM mRawCircularBufferParam_X;
  uint32_t mSlotSize_U32 = 0;           /*! if SlotMode_B this is the slot size which is used to store variable buffer length (adjusted BufferSizeInByte_U32/mNbElementInBuffer_U32 */
  bool mDataPreAllocated_B=false;       /*! true if mpData_U8 is provided by the caller  */
  uint8_t *mpData_U8=nullptr;           /*! Pointer to queue storage buffer used to record queue element*/
  uint32_t mLevelMax_U32=0;             /*! Contains the maximum buffer fill level. This one is reset by the GetMaxLevel method*/
  BOF_MUTEX mRawCircularBufferMtx_X;    /*! Provide a serialized access to shared resources in a multi threaded environment*/
  BOFERR mErrorCode_E=BOF_ERR_NO_ERROR;
  bool mOverflow_B=false;               /*! true if data overflow has occured. Reset to false by IsBufferOverflow*/
  bool mAppendMode_B=false;             /*! true if we are in append mode */
  std::unique_ptr<BofCircularBuffer<BOF_RAW_BUFFER>> mpuBufferCollection = nullptr;
  BOF_RAW_BUFFER mRawBufferToPush_X;
  uint32_t mCrtBufferRemain_U32 = 0;
  uint8_t *mpCrtBufferHead_U8 = nullptr;
  uint8_t *mpCrtBufferEnd_U8 = nullptr;

public:
  BofRawCircularBuffer(const BOF_RAW_CIRCULAR_BUFFER_PARAM &_rRawCircularBufferParam_X);
  virtual ~BofRawCircularBuffer();

  BofRawCircularBuffer &operator=(const BofRawCircularBuffer &) = delete; // Disallow copying
  BofRawCircularBuffer(const BofRawCircularBuffer &) = delete;

  BOFERR LastErrorCode();
  BOFERR LockRawCircularBuffer();
  BOFERR UnlockRawCircularBuffer();
  bool IsEmpty();
  bool IsFull();
  uint32_t GetSlotSize();
  uint32_t GetNbElement(uint32_t *_pSizeOfFirst_U32);
  uint32_t GetCapacity(uint32_t *_pTotalSize_U32);
  uint32_t GetMaxLevel();
  void Reset();
  uint32_t GetNbFreeElement(uint32_t *_pRemainingSize_U32);
  BOFERR SetOverWriteMode(bool _Overwrite_B);
  BOFERR SetAppendMode(uint32_t _BlockingTimeouItInMs_U32, bool _Append_B, BOF_RAW_BUFFER **_ppStorage_X);
  bool IsBufferOverflow();
  BOFERR PushBuffer(uint32_t _BlockingTimeouItInMs_U32, uint32_t _Nb_U32, const uint8_t *_pData_U8, BOF_RAW_BUFFER **_ppStorage_X);
  BOFERR PopBuffer(uint32_t _BlockingTimeouItInMs_U32, uint32_t *_pNbMax_U32, uint8_t *_pData_U8);
  BOFERR Peek(uint32_t _BlockingTimeouItInMs_U32, uint32_t *_pNbMax_U32, uint8_t *_pData_U8);
  //Need to call Skip after GetBufferPtr
  BOFERR GetBufferPtr(uint32_t _BlockingTimeouItInMs_U32, uint32_t *_pNb1_U32, uint8_t **_ppData1_U8, uint32_t *_pNb2_U32, uint8_t **_ppData2_U8);
  //_SignalIfNeeded_B:: sometime you skip just to process OverWrite mode, in this case do not signal free space
  BOFERR Skip(bool _SignalIfNeeded_B, bool *_pLocked_B);

private:
  BOFERR PushRawBuffer(uint32_t _BlockingTimeouItInMs_U32, BOF_RAW_BUFFER **_ppStorage_X);
  BOFERR UpdatePushRawBuffer(uint32_t _SizeUpToTheEnd_U32, uint32_t _Size_U32, const uint8_t *_pData_U8);
  BOFERR PopOrPeekBuffer(bool _Pop_B, uint32_t _BlockingTimeouItInMs_U32, uint32_t *_pNbMax_U32, uint8_t *_pData_U8);
};

END_BOF_NAMESPACE()