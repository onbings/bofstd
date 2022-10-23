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
#include <bofstd/bofstringcircularbuffer.h>

BEGIN_BOF_NAMESPACE()

#define BOF_STRING_CIRCULAR_BUFFER_LOCK(Sts)   {Sts=mStringCircularBufferParam_X.MultiThreadAware_B ? Bof_LockMutex(mStringCbMtx_X):BOF_ERR_NO_ERROR;}
#define BOF_STRING_CIRCULAR_BUFFER_UNLOCK()    {if (mStringCircularBufferParam_X.MultiThreadAware_B) Bof_UnlockMutex(mStringCbMtx_X);}

/*!
 * Description
 * This is the constructor of a CircularBuffer
 *
 * Parameters
 * _pStringCircularBufferParam_X:  Specifies the object creation parameters and contains the retuned values.
 *
 * Returns
 * None
 *
 * Remarks
 * None
 *
 * See Also
 * ~BofStringCircularBuffer
 */

BofStringCircularBuffer::BofStringCircularBuffer(const BOF_STRING_CIRCULAR_BUFFER_PARAM &_rStringCircularBufferParam_X)
{
	mNbCharInBuffer_U32 = 0;
	mNbSnapshotCharInBuffer_U32 = 0;
	mNbElementInBuffer_U32 = 0;
	mpData_c = nullptr;
	mPushIndex_U32 = 0;
	mPopIndex_U32 = 0;
	mPopSnapshotIndex_U32 = 0;
	mOverflow_B = false;
	mLevelMax_U32 = 0;

  mStringCircularBufferParam_X = _rStringCircularBufferParam_X;
	if (mStringCircularBufferParam_X.Blocking_B)
	{
		mErrorCode_E = (mStringCircularBufferParam_X.MultiThreadAware_B) ? BOF_ERR_NO_ERROR : BOF_ERR_WRONG_MODE;
	}
	else
	{
		mErrorCode_E = BOF_ERR_NO_ERROR;
	}
	if (mErrorCode_E == BOF_ERR_NO_ERROR)
	{
		mErrorCode_E = mStringCircularBufferParam_X.Blocking_B ? Bof_CreateEvent("cbs_canread_" + std::to_string(_rStringCircularBufferParam_X.BufferSizeInByte_U32) + "_evt", false, 1, false, mCanReadEvent_X) : BOF_ERR_NO_ERROR;
		if (mErrorCode_E == BOF_ERR_NO_ERROR)
		{
			mErrorCode_E = mStringCircularBufferParam_X.Blocking_B ? Bof_CreateEvent("cbs_canwrite_" + std::to_string(_rStringCircularBufferParam_X.BufferSizeInByte_U32) + "_evt", false, 1, false, mCanWriteEvent_X)
			                           : BOF_ERR_NO_ERROR;
			if (mErrorCode_E == BOF_ERR_NO_ERROR)
			{
				mErrorCode_E = _rStringCircularBufferParam_X.MultiThreadAware_B ? Bof_CreateMutex("BofCircularBuffer", true, true, mStringCbMtx_X) : BOF_ERR_NO_ERROR;
				if (mErrorCode_E == BOF_ERR_NO_ERROR)
				{
					if (mErrorCode_E == BOF_ERR_NO_ERROR)
					{
						if (_rStringCircularBufferParam_X.pData_c)
						{
							mDataPreAllocated_B = true;
							mpData_c = _rStringCircularBufferParam_X.pData_c;
						}
						else
						{
							mDataPreAllocated_B = false;

							// mpData_c=(DataType *)new uint8_t[mStringCircularBufferParam_X.BufferSizeInByte_U32 * sizeof(DataType)];
							mpData_c = new char[mStringCircularBufferParam_X.BufferSizeInByte_U32];
						}

						if (mpData_c)
						{
							mErrorCode_E = mStringCircularBufferParam_X.Blocking_B ? Bof_SignalEvent(mCanWriteEvent_X, 0) : BOF_ERR_NO_ERROR;
						}
						else
						{
							mErrorCode_E = BOF_ERR_ENOMEM;
						}
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
 * BofStringCircularBuffer
 */

BofStringCircularBuffer::~BofStringCircularBuffer()
{
	Bof_DestroyMutex(mStringCbMtx_X);

	if (!mDataPreAllocated_B)
	{
		BOF_SAFE_DELETE_ARRAY(mpData_c);
	}
	Bof_DestroyEvent(mCanReadEvent_X);
	Bof_DestroyEvent(mCanWriteEvent_X);
}

/*!
* Description
* This function stores a byte stream (up to _Size_U32+1 long) inside the circular buffer.
* If the last data byte of the stream is not the mByteStreamDelimiter_U8 it is replaced by this delimiter
* If this one is full the structure reacts according to the _Overwrite_B parameter specified
* when the circular buffer has been created.
*
* Parameters
* _Size_U32: Specify the number of element to push 
* _pData_U8: Specifies the data byte stream to store in the circular buffer.
*
* Returns
* uint32_t: BOF_ERR_NO_ERROR if the operation is successful
*
* Remarks
* None
*
* See Also
* Pop
*/

BOFERR BofStringCircularBuffer::PushBinary(uint32_t _Size_U32, const char *_pData_c, uint32_t _BlockingTimeouItInMs_U32)
{
	BOFERR Rts_E = BOF_ERR_EINVAL;
	uint32_t i_U32, Nb_U32;
	uint32_t NbElementInBuffer_U32;

	if (_pData_c)
	{
		RetryPush:
		NbElementInBuffer_U32 = mNbElementInBuffer_U32;
		Rts_E = ((mStringCircularBufferParam_X.Blocking_B) && (_BlockingTimeouItInMs_U32)) ? Bof_WaitForEvent(mCanWriteEvent_X, _BlockingTimeouItInMs_U32, 0) : BOF_ERR_NO_ERROR;
		if (Rts_E == BOF_ERR_NO_ERROR)
		{
      BOF_STRING_CIRCULAR_BUFFER_LOCK(Rts_E);
      if (Rts_E==BOF_ERR_NO_ERROR)
      {
        Nb_U32 = _Size_U32 + 1;                   // Copy data and add a null-terminated string delimitter

        if ((!mStringCircularBufferParam_X.Overwrite_B) &&
            ((mNbCharInBuffer_U32 + Nb_U32) > mStringCircularBufferParam_X.BufferSizeInByte_U32)
          )
        {
          Rts_E = BOF_ERR_FULL;
        }
        else
        {
          Rts_E = BOF_ERR_NO_ERROR;
          if (Nb_U32)
          {
            Nb_U32--;  //0 will be 'hard' copied as it cold not be present in the buffer
            for (i_U32 = 0; i_U32 < Nb_U32; i_U32++)
            {
              mpData_c[mPushIndex_U32] = _pData_c[i_U32];
              mNbCharInBuffer_U32++;

              mPushIndex_U32++;

              if (mPushIndex_U32 >= mStringCircularBufferParam_X.BufferSizeInByte_U32)
              {
                mPushIndex_U32 = 0;
              }
            }
            //Push 0
            mpData_c[mPushIndex_U32] = 0;
            mNbCharInBuffer_U32++;

            mPushIndex_U32++;

            if (mPushIndex_U32 >= mStringCircularBufferParam_X.BufferSizeInByte_U32)
            {
              mPushIndex_U32 = 0;
            }

            if (mNbCharInBuffer_U32 > mStringCircularBufferParam_X.BufferSizeInByte_U32)
            {
              mNbCharInBuffer_U32 = mStringCircularBufferParam_X.BufferSizeInByte_U32; // mStringCircularBufferParam_X.Overwrite_B
              if (mStringCircularBufferParam_X.Overwrite_B)
              {
                mOverflow_B = true;
                mPopIndex_U32 += Nb_U32;
                if (mPopIndex_U32 >= mStringCircularBufferParam_X.BufferSizeInByte_U32)
                {
                  mPopIndex_U32 -= mStringCircularBufferParam_X.BufferSizeInByte_U32;
                }
              }
            }

            if (mNbCharInBuffer_U32 > mLevelMax_U32)
            {
              mLevelMax_U32 = mNbCharInBuffer_U32;
            }
            mNbElementInBuffer_U32++;
          }
        }
        BOF_STRING_CIRCULAR_BUFFER_UNLOCK();
      }
			if ((mStringCircularBufferParam_X.Blocking_B) && (_BlockingTimeouItInMs_U32))
			{
				if (Rts_E == BOF_ERR_NO_ERROR)
				{
					if ((mStringCircularBufferParam_X.Overwrite_B) || (NbElementInBuffer_U32 < mStringCircularBufferParam_X.BufferSizeInByte_U32))
					{
						Bof_SignalEvent(mCanWriteEvent_X, 0);
					}
					Bof_SignalEvent(mCanReadEvent_X, 0);
				}
				if (Rts_E == BOF_ERR_FULL)
				{
					goto RetryPush; //We have been preempt between Bof_WaitForEvent and Bof_LockMutex
				}
			}
		}
	}
	return Rts_E;
}

/*!
 * Description
 * This function stores a byte stream (up to '*_pNbMax_U32 long) inside the circular buffer.
 * If the last data byte of the stream is not the mByteStreamDelimiter_U8 it is replaced by this delimiter
 * If this one is full the structure reacts according to the _Overwrite_B parameter specified
 * when the circular buffer has been created.
 *
 * Parameters
 * _pNbMax_U32: Specify the maximum number of element to push and \returns
 *                               The real number of data pushed
 * _pData_UB: Specifies the data byte stream to store in the circular buffer.
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 *
 * See Also
 * Pop
 */

BOFERR BofStringCircularBuffer::PushString(const char *_pData_c, uint32_t _BlockingTimeouItInMs_U32)
{
	BOFERR Rts_E = BOF_ERR_EINVAL;

	if (_pData_c)
	{
		Rts_E = PushBinary(static_cast<uint32_t>(strlen(_pData_c)), _pData_c, _BlockingTimeouItInMs_U32);
	}
	return Rts_E;
}

/*!
 * Description
 * This function removes a data byte stream (up to '*_pNbMax_U32 long) from the circular buffer.
 * Data read operation stops when the mByteStreamDelimiter_U8 is found or up to *_pNbMax_U32 long
 * have been read. In this case the last character is erased by a mByteStreamDelimiter_U8
 *
 * Parameters
 * _pNbMax_U32: Specify the maximum number of element to pop and \returns
 *                               The real number of data poped
 * _pData_c: \Returns the data read from the circular buffer.
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
BOFERR BofStringCircularBuffer::PopString(uint32_t *_pNbMax_U32, char *_pData_c, uint32_t _BlockingTimeouItInMs_U32)
{
	BOFERR Rts_E = BOF_ERR_EINVAL;
	uint32_t i_U32, Nb_U32, NbElementInBuffer_U32;

	if ((_pNbMax_U32) && (_pData_c))
	{
		RetryPop:
		Rts_E = ((mStringCircularBufferParam_X.Blocking_B) && (_BlockingTimeouItInMs_U32)) ? Bof_WaitForEvent(mCanReadEvent_X, _BlockingTimeouItInMs_U32, 0) : BOF_ERR_NO_ERROR;
		if (Rts_E == BOF_ERR_NO_ERROR)
		{
      BOF_STRING_CIRCULAR_BUFFER_LOCK(Rts_E);
      if (Rts_E==BOF_ERR_NO_ERROR)
      {
        NbElementInBuffer_U32 = mNbElementInBuffer_U32;
        Nb_U32                = 0;
        Rts_E                 = BOF_ERR_EMPTY;

        if (mStringCircularBufferParam_X.SnapshotMode_B)
        {
          if (mNbSnapshotCharInBuffer_U32)
          {
            Nb_U32 = *_pNbMax_U32;
            if (Nb_U32 > mNbSnapshotCharInBuffer_U32)
            {
              Nb_U32 = mNbSnapshotCharInBuffer_U32;
            }
            // Buffer is dimensioned to contains a an integer number of "_pData"->there is no  read/write pointer clipping during an "atomic" push or pop
            if (Nb_U32)
            {
              for (i_U32 = 0; i_U32 < Nb_U32; i_U32++)
              {
                // Is the last one character to read a terminating char
                if ((i_U32 == (Nb_U32 - 1)) && (mpData_c[mPopSnapshotIndex_U32] != 0))
                {
                  break;
                }
                _pData_c[i_U32] = mpData_c[mPopSnapshotIndex_U32];
                mNbSnapshotCharInBuffer_U32--;
                mPopSnapshotIndex_U32++;

                if (mPopSnapshotIndex_U32 >= mStringCircularBufferParam_X.BufferSizeInByte_U32)
                {
                  mPopSnapshotIndex_U32 = 0;
                }

                if (_pData_c[i_U32] == 0)
                {
                  Nb_U32 = i_U32 + 1;
                  break;
                }
              }
              _pData_c[Nb_U32 - 1] = 0;
              Rts_E = BOF_ERR_NO_ERROR;
            }
          }
        }
        else
        {
          if (mNbCharInBuffer_U32)
          {
            Nb_U32 = *_pNbMax_U32;
            if (Nb_U32 > mNbCharInBuffer_U32)
            {
              Nb_U32 = mNbCharInBuffer_U32;
            }
            // Buffer is dimensioned to contains a an integer number of "_pData"->there is no  read/write pointer clipping during an "atomic" push or pop
            if (Nb_U32)
            {
              for (i_U32 = 0; i_U32 < Nb_U32; i_U32++)
              {
                // Is the last one character to read a terminating char
                if ((i_U32 == (Nb_U32 - 1)) && (mpData_c[mPopIndex_U32] != 0))
                {
                  // NO the string will be read in more than one op mNbElementInBuffer_U32--;
                  break;
                }
                _pData_c[i_U32] = mpData_c[mPopIndex_U32];
                mNbCharInBuffer_U32--;
                mPopIndex_U32++;

                if (mPopIndex_U32 >= mStringCircularBufferParam_X.BufferSizeInByte_U32)
                {
                  mPopIndex_U32 = 0;
                }

                if (_pData_c[i_U32] == 0)
                {
                  mNbElementInBuffer_U32--;
                  NbElementInBuffer_U32 = mNbElementInBuffer_U32;
                  Nb_U32                = i_U32 + 1;
                  break;
                }
              }
              _pData_c[Nb_U32 - 1] = 0;
              Rts_E = BOF_ERR_NO_ERROR;
            }
          }
        }
        *_pNbMax_U32 = Nb_U32;

        BOF_STRING_CIRCULAR_BUFFER_UNLOCK();
      }
			if ((mStringCircularBufferParam_X.Blocking_B) && (_BlockingTimeouItInMs_U32))
			{
				if (Rts_E == BOF_ERR_NO_ERROR)
				{
					if (NbElementInBuffer_U32)
					{
						Bof_SignalEvent(mCanReadEvent_X, 0);
					}
					Bof_SignalEvent(mCanWriteEvent_X, 0);
				}
				if (Rts_E == BOF_ERR_EMPTY)
				{
					goto RetryPop;
				}
			}
		}
	}
	return Rts_E;
}

/*!
 * Description
 * This function is used to resync snapshot read pointer on write pointer.
 * This can happens if we do not read the whole number of entry in a "Snapshot" circular buffer.
 *
 * Parameters
 *      _Index_U32: Specifiy where to relocate the read pointer:
 *              0 On the oldest buffer element
 *              1 One entry after the oldest
 *              2 Two entries after the oldest
 *              ....
 *
 * Returns
 *      The number of element in the buffer
 *
 * Remarks
 * This function lmust be called before any call to PopString to initialize correctly mNbSnapshotCharInBuffer_U32
 *
 * See Also
 * Reset
 */

uint32_t BofStringCircularBuffer::ResyncSnapshot(uint32_t _Index_U32)
{
	uint32_t Rts_U32=0;
	int32_t IndexOnTheOldest_S32;
  BOFERR Sts_E;

  BOF_STRING_CIRCULAR_BUFFER_LOCK(Sts_E);
  if (Sts_E==BOF_ERR_NO_ERROR)
  {
    Rts_U32 = mNbCharInBuffer_U32;

    if (mStringCircularBufferParam_X.SnapshotMode_B)
    {
      mNbSnapshotCharInBuffer_U32 = mNbCharInBuffer_U32;
      if (mNbCharInBuffer_U32)
      {
        if (_Index_U32 >= mNbCharInBuffer_U32)
        {
          _Index_U32 = mNbCharInBuffer_U32 - 1;
        }

        IndexOnTheOldest_S32 = static_cast<int32_t>(mPushIndex_U32 - Rts_U32);

        if (IndexOnTheOldest_S32 < 0)
        {
          IndexOnTheOldest_S32 = mStringCircularBufferParam_X.BufferSizeInByte_U32 + IndexOnTheOldest_S32;
        }

        mPopSnapshotIndex_U32 = IndexOnTheOldest_S32 + _Index_U32;

        if (mPopSnapshotIndex_U32 >= mStringCircularBufferParam_X.BufferSizeInByte_U32)
        {
          mPopSnapshotIndex_U32 -= mStringCircularBufferParam_X.BufferSizeInByte_U32;
        }
      }
    }

    BOF_STRING_CIRCULAR_BUFFER_UNLOCK();
  }
	return Rts_U32;
}

void BofStringCircularBuffer::Reset()
{
  BOFERR Sts_E;

  BOF_STRING_CIRCULAR_BUFFER_LOCK(Sts_E);
  if (Sts_E==BOF_ERR_NO_ERROR)
  {
    mOverflow_B   = false;
    mLevelMax_U32 = mPushIndex_U32 = mPopIndex_U32 = mPopSnapshotIndex_U32 = mNbCharInBuffer_U32 = mNbSnapshotCharInBuffer_U32 = mNbElementInBuffer_U32 = 0;

    BOF_STRING_CIRCULAR_BUFFER_UNLOCK();
  }
}
BOFERR BofStringCircularBuffer::LockStringQueue()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mStringCircularBufferParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_LockMutex(mStringCbMtx_X);
  }
  return Rts_E;
}
BOFERR BofStringCircularBuffer::UnlockStringQueue()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mStringCircularBufferParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_UnlockMutex(mStringCbMtx_X);
  }
  return Rts_E;
}
END_BOF_NAMESPACE()