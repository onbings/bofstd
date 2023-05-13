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

#define BOF_RAW_CIRCULAR_BUFFER_LOCK(Sts)                                                                                                                                                                                                                      \
  {                                                                                                                                                                                                                                                            \
    Sts = mRawCircularBufferParam_X.MultiThreadAware_B ? Bof_LockMutex(mRawCbMtx_X) : BOF_ERR_NO_ERROR;                                                                                                                                                        \
  }
#define BOF_RAW_CIRCULAR_BUFFER_UNLOCK()                                                                                                                                                                                                                       \
  {                                                                                                                                                                                                                                                            \
    if (mRawCircularBufferParam_X.MultiThreadAware_B)                                                                                                                                                                                                          \
      Bof_UnlockMutex(mRawCbMtx_X);                                                                                                                                                                                                                            \
  }

/*!
 * Description
 * This is the constructor of a CircularBuffer
 *
 * Parameters
 * _pRawCircularBufferParam_X:  Specifies the object creation parameters and contains the retuned values.
 *
 * Returns
 * None
 *
 * Remarks
 * None
 *
 * See Also
 * ~BofRawCircularBuffer
 */

BofRawCircularBuffer::BofRawCircularBuffer(const BOF_RAW_CIRCULAR_BUFFER_PARAM &_rRawCircularBufferParam_X)
{
  mRawCircularBufferParam_X = _rRawCircularBufferParam_X;
  mNbSlot_U32 = 0;
  mSlotSize_U32 = 0;
  mNbElementInBuffer_U32 = 0;
  mpData_U8 = nullptr;
  mPushIndex_U32 = 0;
  mPopIndex_U32 = 0;
  mLevelMax_U32 = 0;

  mErrorCode_E = BOF_ERR_TOO_SMALL;
  if (mRawCircularBufferParam_X.BufferSizeInByte_U32)
  {
    mErrorCode_E = _rRawCircularBufferParam_X.MultiThreadAware_B ? Bof_CreateMutex("BofCircularBuffer", false, false, mRawCbMtx_X) : BOF_ERR_NO_ERROR;
    if (mErrorCode_E == BOF_ERR_NO_ERROR)
    {
      if (_rRawCircularBufferParam_X.pData_U8)
      {
        mDataPreAllocated_B = true;
        mpData_U8 = _rRawCircularBufferParam_X.pData_U8;

        if (mRawCircularBufferParam_X.NbMaxSlot_U32)
        {
          if ((mRawCircularBufferParam_X.BufferSizeInByte_U32 - (mRawCircularBufferParam_X.NbMaxSlot_U32 * sizeof(uint32_t))) % mRawCircularBufferParam_X.NbMaxSlot_U32)
          {
            mErrorCode_E = BOF_ERR_WRONG_SIZE;
          }
          else
          {
            mSlotSize_U32 = (mRawCircularBufferParam_X.BufferSizeInByte_U32 - (mRawCircularBufferParam_X.NbMaxSlot_U32 * static_cast<uint32_t>(sizeof(uint32_t)))) / mRawCircularBufferParam_X.NbMaxSlot_U32;

            if (!mSlotSize_U32)
            {
              mErrorCode_E = BOF_ERR_WRONG_SIZE;
            }
          }
        }
      }
      else
      {
        if (mRawCircularBufferParam_X.NbMaxSlot_U32)
        {
          if (mRawCircularBufferParam_X.BufferSizeInByte_U32 % mRawCircularBufferParam_X.NbMaxSlot_U32)
          {
            mErrorCode_E = BOF_ERR_WRONG_SIZE;
          }
          else
          {
            mSlotSize_U32 = mRawCircularBufferParam_X.BufferSizeInByte_U32 / mRawCircularBufferParam_X.NbMaxSlot_U32;

            if (!mSlotSize_U32)
            {
              mErrorCode_E = BOF_ERR_WRONG_SIZE;
            }
            else
            {
              mRawCircularBufferParam_X.BufferSizeInByte_U32 = mRawCircularBufferParam_X.BufferSizeInByte_U32 + (mRawCircularBufferParam_X.NbMaxSlot_U32 * static_cast<uint32_t>(sizeof(uint32_t))); // Space needed by length header
            }
          }
        }

        if (mErrorCode_E == BOF_ERR_NO_ERROR)
        {
          mDataPreAllocated_B = false;
          mpData_U8 = new uint8_t[mRawCircularBufferParam_X.BufferSizeInByte_U32];

          if (mpData_U8 == nullptr)
          {
            mErrorCode_E = BOF_ERR_ENOMEM;
          }
        }
      }
    }
  }
}

/*!
 * Description
 * This is the destructor of the Circular buffer structure. It releases all
 * the resources allocated dynamically by the object
 *
 * Parameters
 * None
 *
 * Returns
 * None
 *
 * Remarks
 * None
 *
 * See Also
 * BofRawCircularBuffer
 */

BofRawCircularBuffer::~BofRawCircularBuffer()
{
  if (!mDataPreAllocated_B)
  {
    BOF_SAFE_DELETE_ARRAY(mpData_U8);
  }
  Bof_DestroyMutex(mRawCbMtx_X);
}

/*!
 * Description
 * This function stores a byte buffer of _Nb_U32 byte long inside the circular buffer.
 *
 * Parameters
 * _Nb_U32: Specify the number of byte to push inside the container
 * _pData_UB: Specifies the data byte stream to store in the circular buffer.
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * a uint32_t containing the number of bytes pushed is inserted before the data buffer for Pop operation
 *
 * See Also
 * Pop
 */
BOFERR BofRawCircularBuffer::PushBuffer(uint32_t _Nb_U32, const uint8_t *_pData_U8)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  bool EnoughSpace_B;

  if ((_Nb_U32) && (_Nb_U32 < 0x80000000) && (_pData_U8)) // Bit 31 is used to signal locked zone
  {
    BOF_RAW_CIRCULAR_BUFFER_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (mSlotSize_U32)
      {
        EnoughSpace_B = ((mNbSlot_U32 < mRawCircularBufferParam_X.NbMaxSlot_U32) && (_Nb_U32 <= mSlotSize_U32));
      }
      else
      {
        EnoughSpace_B = ((mNbElementInBuffer_U32 + _Nb_U32 + sizeof(uint32_t)) <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
      }

      if (EnoughSpace_B)
      {
        WriteNbHeader(nullptr, _Nb_U32);
        Rts_E = WritePayload(nullptr, _Nb_U32, _pData_U8);
      }
      else
      {
        Rts_E = BOF_ERR_FULL;
      }
      BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
    }
  }
  return Rts_E;
}

/*!
 * Description
 * This function removes a data byte buffer (up to '*_pNbMax_U32 long) from the circular buffer.
 *
 * Parameters
 * _pNbMax_U32: Specify the maximum number of element to pop and \returns the real number of data poped
 * _pData_U8: \Returns the data read from the circular buffer.
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 *
 * See Also
 * Push
 */
BOFERR BofRawCircularBuffer::PopBuffer(uint32_t *_pNbMax_U32, uint8_t *_pData_U8)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint32_t Nb_U32, PopIndex_U32;

  if ((_pNbMax_U32) && (_pData_U8))
  {
    BOF_RAW_CIRCULAR_BUFFER_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (mNbSlot_U32)
      {
        PopIndex_U32 = mPopIndex_U32;
        Nb_U32 = ReadNbHeader(&PopIndex_U32);

        if (Nb_U32 & 0x80000000) // Bit 31 is used to signal locked zone
        {
          Rts_E = BOF_ERR_NOT_AVAILABLE;
          Nb_U32 = 0;
        }
        else
        {
          if (Nb_U32 <= *_pNbMax_U32)
          {
            mNbElementInBuffer_U32 -= static_cast<uint32_t>(sizeof(uint32_t));
            mPopIndex_U32 = PopIndex_U32;
            Rts_E = ReadPayload(nullptr, Nb_U32, _pData_U8);
          }
          else
          {
            Rts_E = BOF_ERR_TOO_SMALL;
          }
        }
      }
      else
      {
        Rts_E = BOF_ERR_EMPTY;
        Nb_U32 = 0;
      }
      *_pNbMax_U32 = Nb_U32;
      BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
    }
  }
  return Rts_E;
}

uint32_t BofRawCircularBuffer::GetNbFreeElement()
{
  uint32_t Rts_U32 = 0;
  BOFERR Sts_E;

  BOF_RAW_CIRCULAR_BUFFER_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if (mSlotSize_U32)
    {
      Rts_U32 = mRawCircularBufferParam_X.NbMaxSlot_U32 - mNbSlot_U32;
    }
    else
    {
      Rts_U32 = ((mRawCircularBufferParam_X.BufferSizeInByte_U32 - mNbElementInBuffer_U32) > static_cast<uint32_t>(sizeof(uint32_t))) ? mRawCircularBufferParam_X.BufferSizeInByte_U32 - mNbElementInBuffer_U32 - static_cast<uint32_t>(sizeof(uint32_t)) : 0;
    }
    BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
  }
  return Rts_U32;
}

void BofRawCircularBuffer::Reset()
{
  BOFERR Sts_E;

  BOF_RAW_CIRCULAR_BUFFER_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    mLevelMax_U32 = 0;
    mPushIndex_U32 = 0;
    mPopIndex_U32 = 0;
    mNbElementInBuffer_U32 = 0;
    mNbSlot_U32 = 0;

    BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
  }
}

BOFERR BofRawCircularBuffer::Peek(uint32_t _Index_U32, uint32_t *_pNbMax_U32, uint8_t *_pData_U8)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint32_t Nb_U32, PopIndex_U32, j_U32;

  if ((_pNbMax_U32) && (_pData_U8))
  {
    BOF_RAW_CIRCULAR_BUFFER_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if ((mNbSlot_U32) && (_Index_U32 < mNbSlot_U32))
      {
        PopIndex_U32 = mPopIndex_U32;

        if (_Index_U32)
        {
          for (j_U32 = 0; j_U32 < _Index_U32; j_U32++)
          {
            Nb_U32 = ReadNbHeader(&PopIndex_U32);

            if (Nb_U32 & 0x80000000) // Bit 31 is used to signal locked zone
            {
              Nb_U32 &= 0x7FFFFFFF; // Bit 31 is used to signal locked zone
            }

            if (mSlotSize_U32)
            {
              PopIndex_U32 += mSlotSize_U32;
            }
            else
            {
              PopIndex_U32 = ((PopIndex_U32 + Nb_U32) % mRawCircularBufferParam_X.BufferSizeInByte_U32);
            }

            if (PopIndex_U32 >= mRawCircularBufferParam_X.BufferSizeInByte_U32)
            {
              PopIndex_U32 = 0;
            }
          }
        }
        Nb_U32 = ReadNbHeader(&PopIndex_U32);

        if (Nb_U32 & 0x80000000) // Bit 31 is used to signal locked zone
        {
          Rts_E = BOF_ERR_NOT_AVAILABLE;
          Nb_U32 = 0;
        }
        else
        {
          // In peek mode we can 'read' less than Nb_U32 as we do not modify buffer control val
          if (Nb_U32 > *_pNbMax_U32)
          {
            Nb_U32 = *_pNbMax_U32;
          }
          Rts_E = ReadPayload(&PopIndex_U32, Nb_U32, _pData_U8);
        }
      }
      else
      {
        Rts_E = BOF_ERR_EMPTY;
        Nb_U32 = 0;
      }
      *_pNbMax_U32 = Nb_U32;
      BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
    }
  }
  return Rts_E;
}

BOFERR BofRawCircularBuffer::Skip()
{
  BOFERR Rts_E;
  uint32_t Nb_U32, PopIndex_U32;

  BOF_RAW_CIRCULAR_BUFFER_LOCK(Rts_E);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    if (mNbSlot_U32)
    {
      PopIndex_U32 = mPopIndex_U32;
      Nb_U32 = ReadNbHeader(&PopIndex_U32);

      if (Nb_U32 & 0x80000000) // Bit 31 is used to signal locked zone
      {
        Rts_E = BOF_ERR_NOT_AVAILABLE;
      }
      else
      {
        mNbElementInBuffer_U32 -= static_cast<uint32_t>(sizeof(uint32_t));
        mPopIndex_U32 = PopIndex_U32;
        Rts_E = ReadPayload(nullptr, Nb_U32, nullptr);
      }
    }
    else
    {
      Rts_E = BOF_ERR_EMPTY;
    }
    BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
  }
  return Rts_E;
}

BOFERR BofRawCircularBuffer::LockBuffer(uint32_t _Nb_U32, uint32_t *_pNb1_U32, uint8_t **_ppData1_U8, uint32_t *_pNb2_U32, uint8_t **_ppData2_U8)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint32_t Nb_U32;
  bool EnoughSpace_B;

  if ((_Nb_U32) && (_pNb1_U32) && (_ppData1_U8) && (_pNb2_U32) && (_ppData2_U8))
  {
    *_pNb1_U32 = 0;
    *_ppData1_U8 = nullptr;
    *_pNb2_U32 = 0;
    *_ppData2_U8 = nullptr;

    BOF_RAW_CIRCULAR_BUFFER_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (mSlotSize_U32)
      {
        EnoughSpace_B = ((mNbSlot_U32 < mRawCircularBufferParam_X.NbMaxSlot_U32) && (_Nb_U32 <= mSlotSize_U32));
      }
      else
      {
        EnoughSpace_B = ((mNbElementInBuffer_U32 + _Nb_U32 + sizeof(uint32_t)) <= mRawCircularBufferParam_X.BufferSizeInByte_U32);
      }

      if (EnoughSpace_B)
      {
        WriteNbHeader(nullptr, _Nb_U32 | 0x80000000); // Bit 31 is used to signal locked zone (PopBuffer)
        *_ppData1_U8 = &mpData_U8[mPushIndex_U32];

        if (mSlotSize_U32)
        {
          *_pNb1_U32 = _Nb_U32;
        }
        else
        {
          Nb_U32 = (mRawCircularBufferParam_X.BufferSizeInByte_U32 - mPushIndex_U32);

          if (Nb_U32 >= _Nb_U32)
          {
            *_pNb1_U32 = _Nb_U32;
          }
          else
          {
            *_ppData2_U8 = &mpData_U8[0];
            *_pNb1_U32 = Nb_U32;
            *_pNb2_U32 = _Nb_U32 - Nb_U32;
          }
        }

        if (WritePayload(nullptr, _Nb_U32, nullptr) == 0)
        {
          Rts_E = BOF_ERR_NO_ERROR;
        }
        else
        {
          Rts_E = BOF_ERR_WRITE;
        }
      }
      else
      {
        Rts_E = BOF_ERR_FULL;
      }
      BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
    }
  }
  return Rts_E;
}

BOFERR BofRawCircularBuffer::UnlockBuffer(const uint8_t *_pLockedBuffer_U8, uint32_t _Nb_U32) // , const uint8_t *_pData_U8)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint32_t PopIndex_U32, PushIndex_U32, Nb_U32;

  if ((_Nb_U32) && (_pLockedBuffer_U8))
  {
    if ((_pLockedBuffer_U8 >= mpData_U8) && (_pLockedBuffer_U8 < &mpData_U8[mRawCircularBufferParam_X.BufferSizeInByte_U32]) && ((_pLockedBuffer_U8 + _Nb_U32) <= &mpData_U8[mRawCircularBufferParam_X.BufferSizeInByte_U32]))
    {
      BOF_RAW_CIRCULAR_BUFFER_LOCK(Rts_E);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        PopIndex_U32 = static_cast<uint32_t>(_pLockedBuffer_U8 - mpData_U8 - sizeof(uint32_t));
        PushIndex_U32 = PopIndex_U32;
        Nb_U32 = ReadNbHeader(&PopIndex_U32); // Bit 31 is used to signal locked zone: Written by LockBuffer

        if (Nb_U32 & 0x80000000)
        {
          Nb_U32 &= 0x7FFFFFFF; // Bit 31 is used to signal locked zone

          if (_Nb_U32 <= Nb_U32) // Normally they should be equal...
          {
            Rts_E = WriteNbHeader(&PushIndex_U32,
                                  Nb_U32); // Write Nb_U32 and not _Nb_U32 because all other var control have been computed by LockBuffer based on this value...
            // Rts_U32 = WritePayload(&PushIndex_U32, _Nb_U32, _pData_U8);
          }
          else
          {
            Rts_E = BOF_ERR_WRONG_SIZE;
          }
        }
        else
        {
          Rts_E = BOF_ERR_NOT_AVAILABLE;
        }
        BOF_RAW_CIRCULAR_BUFFER_UNLOCK();
      }
    }
  }
  return Rts_E;
}

// if _pPopIndex_U32 is not null we do not update circular buffer control var and we return the new popIndex value to the caller
uint32_t BofRawCircularBuffer::ReadNbHeader(uint32_t *_pPopIndex_U32)
{
  uint32_t Rts_U32, PopIndex_U32;

  // No security check (private fct caller has checked everything)
  if (_pPopIndex_U32)
  {
    PopIndex_U32 = *_pPopIndex_U32;
  }
  else
  {
    PopIndex_U32 = mPopIndex_U32;
  }
  Rts_U32 = ((uint32_t)(mpData_U8[PopIndex_U32]) << 24);
  PopIndex_U32++;

  if (PopIndex_U32 >= mRawCircularBufferParam_X.BufferSizeInByte_U32)
  {
    PopIndex_U32 = 0;
  }

  Rts_U32 |= ((uint32_t)(mpData_U8[PopIndex_U32]) << 16);
  PopIndex_U32++;

  if (PopIndex_U32 >= mRawCircularBufferParam_X.BufferSizeInByte_U32)
  {
    PopIndex_U32 = 0;
  }

  Rts_U32 |= ((uint32_t)(mpData_U8[PopIndex_U32]) << 8);
  PopIndex_U32++;

  if (PopIndex_U32 >= mRawCircularBufferParam_X.BufferSizeInByte_U32)
  {
    PopIndex_U32 = 0;
  }

  Rts_U32 |= ((uint32_t)(mpData_U8[PopIndex_U32]));
  PopIndex_U32++;

  if (PopIndex_U32 >= mRawCircularBufferParam_X.BufferSizeInByte_U32)
  {
    PopIndex_U32 = 0;
  }

  if (_pPopIndex_U32)
  {
    *_pPopIndex_U32 = PopIndex_U32;
  }
  else
  {
    mPopIndex_U32 = PopIndex_U32;
    mNbElementInBuffer_U32 -= static_cast<uint32_t>(sizeof(uint32_t));
  }
  return Rts_U32;
}

BOFERR BofRawCircularBuffer::WriteNbHeader(uint32_t *_pPushIndex_U32, uint32_t _Nb_U32)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  uint32_t PushIndex_U32;

  // No security check (private fct caller has checked everything)
  if (_pPushIndex_U32)
  {
    PushIndex_U32 = *_pPushIndex_U32;
  }
  else
  {
    PushIndex_U32 = mPushIndex_U32;
  }
  mpData_U8[PushIndex_U32] = (uint8_t)(_Nb_U32 >> 24);
  PushIndex_U32++;

  if (PushIndex_U32 >= mRawCircularBufferParam_X.BufferSizeInByte_U32)
  {
    PushIndex_U32 = 0;
  }

  mpData_U8[PushIndex_U32] = (uint8_t)(_Nb_U32 >> 16);
  PushIndex_U32++;

  if (PushIndex_U32 >= mRawCircularBufferParam_X.BufferSizeInByte_U32)
  {
    PushIndex_U32 = 0;
  }

  mpData_U8[PushIndex_U32] = (uint8_t)(_Nb_U32 >> 8);
  PushIndex_U32++;

  if (PushIndex_U32 >= mRawCircularBufferParam_X.BufferSizeInByte_U32)
  {
    PushIndex_U32 = 0;
  }

  mpData_U8[PushIndex_U32] = (uint8_t)(_Nb_U32);
  PushIndex_U32++;

  if (PushIndex_U32 >= mRawCircularBufferParam_X.BufferSizeInByte_U32)
  {
    PushIndex_U32 = 0;
  }

  if (_pPushIndex_U32)
  {
    *_pPushIndex_U32 = PushIndex_U32;
  }
  else
  {
    mPushIndex_U32 = PushIndex_U32;
    mNbElementInBuffer_U32 += static_cast<uint32_t>(sizeof(uint32_t));
  }

  return Rts_E;
}

BOFERR BofRawCircularBuffer::ReadPayload(uint32_t *_pPopIndex_U32, uint32_t _Nb_U32, uint8_t *_pData_U8)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  uint32_t Nb_U32, PopIndex_U32, NbElementInBuffer_U32;

  // No security check (private fct caller has checked everything)
  if (_pPopIndex_U32)
  {
    PopIndex_U32 = *_pPopIndex_U32;
  }
  else
  {
    PopIndex_U32 = mPopIndex_U32;
    mNbSlot_U32--;
  }
  NbElementInBuffer_U32 = mNbElementInBuffer_U32;

  if (mSlotSize_U32)
  {
    if (_pData_U8)
    {
      memcpy(_pData_U8, &mpData_U8[PopIndex_U32], _Nb_U32);
    }
    PopIndex_U32 += mSlotSize_U32;
    NbElementInBuffer_U32 -= mSlotSize_U32;
  }
  else
  {
    Nb_U32 = (mRawCircularBufferParam_X.BufferSizeInByte_U32 - PopIndex_U32);

    if (Nb_U32 >= _Nb_U32)
    {
      if (_pData_U8)
      {
        memcpy(_pData_U8, &mpData_U8[PopIndex_U32], _Nb_U32);
      }
      PopIndex_U32 += _Nb_U32;
    }
    else
    {
      // Copy from the end of the buffer
      if (_pData_U8)
      {
        memcpy(_pData_U8, &mpData_U8[PopIndex_U32], Nb_U32);

        // Copy the remaining byte from the beginning of the buffer
        _pData_U8 += Nb_U32;
        Nb_U32 = _Nb_U32 - Nb_U32;
        memcpy(_pData_U8, &mpData_U8[0], Nb_U32);
      }
      else
      {
        Nb_U32 = _Nb_U32 - Nb_U32;
      }
      PopIndex_U32 = Nb_U32;
    }
    NbElementInBuffer_U32 -= _Nb_U32;
  }

  if (PopIndex_U32 >= mRawCircularBufferParam_X.BufferSizeInByte_U32)
  {
    PopIndex_U32 = 0;
  }

  if (_pPopIndex_U32)
  {
    *_pPopIndex_U32 = PopIndex_U32;
  }
  else
  {
    mPopIndex_U32 = PopIndex_U32;
    mNbElementInBuffer_U32 = NbElementInBuffer_U32;
  }
  return Rts_E;
}

// if _pData_U8 is null we just reserve (lock) a buffer for later use (LockBuffer)
// if _pPushIndex_U32 not null, Data is pushed from *_pPushIndex_U32 (Push Locked Buffer)
BOFERR BofRawCircularBuffer::WritePayload(uint32_t *_pPushIndex_U32, uint32_t _Nb_U32, const uint8_t *_pData_U8)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  uint32_t Nb_U32, PushIndex_U32, NbElementInBuffer_U32;

  // No security check (private fct caller has checked everything)
  if (_pPushIndex_U32)
  {
    PushIndex_U32 = *_pPushIndex_U32;
  }
  else
  {
    PushIndex_U32 = mPushIndex_U32;
    mNbSlot_U32++;
  }
  NbElementInBuffer_U32 = mNbElementInBuffer_U32;

  if (mSlotSize_U32)
  {
    if (_pData_U8)
    {
      memcpy(&mpData_U8[PushIndex_U32], _pData_U8, _Nb_U32);
    }
    PushIndex_U32 += mSlotSize_U32;
    NbElementInBuffer_U32 += mSlotSize_U32;

    if (mNbSlot_U32 > mLevelMax_U32)
    {
      mLevelMax_U32 = mNbSlot_U32;
    }
  }
  else
  {
    Nb_U32 = (mRawCircularBufferParam_X.BufferSizeInByte_U32 - PushIndex_U32);

    if (Nb_U32 >= _Nb_U32)
    {
      if (_pData_U8)
      {
        memcpy(&mpData_U8[PushIndex_U32], _pData_U8, _Nb_U32);
      }
      PushIndex_U32 += _Nb_U32;
    }
    else
    {
      // Copy to the end of the buffer
      if (_pData_U8)
      {
        memcpy(&mpData_U8[PushIndex_U32], _pData_U8, Nb_U32);

        // Copy the remaining byte at the beginning of the buffer
        _pData_U8 += Nb_U32;
      }
      Nb_U32 = _Nb_U32 - Nb_U32;

      if (_pData_U8)
      {
        memcpy(&mpData_U8[0], _pData_U8, Nb_U32);
      }
      PushIndex_U32 = Nb_U32;
    }
    NbElementInBuffer_U32 += _Nb_U32;

    if (NbElementInBuffer_U32 > mLevelMax_U32)
    {
      mLevelMax_U32 = NbElementInBuffer_U32;
    }
  }

  if (PushIndex_U32 >= mRawCircularBufferParam_X.BufferSizeInByte_U32)
  {
    PushIndex_U32 = 0;
  }

  if (_pPushIndex_U32)
  {
    *_pPushIndex_U32 = PushIndex_U32;
  }
  else
  {
    mNbElementInBuffer_U32 = NbElementInBuffer_U32;
    mPushIndex_U32 = PushIndex_U32;
  }
  return Rts_E;
}
BOFERR BofRawCircularBuffer::LockRawQueue()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mRawCircularBufferParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_LockMutex(mRawCbMtx_X);
  }
  return Rts_E;
}
BOFERR BofRawCircularBuffer::UnlockRawQueue()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mRawCircularBufferParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_UnlockMutex(mRawCbMtx_X);
  }
  return Rts_E;
}
END_BOF_NAMESPACE()