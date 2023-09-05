/*
 * Copyright (c) 2000-2016, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines routines for creating and managing a circular
 * buffer.
 *
 * Name:        BofPot.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Oct 19 2008  BHA : Initial release
 */
#pragma once

#include <bofstd/bofsystem.h>

#include <string.h>

BEGIN_BOF_NAMESPACE()
#define BOF_POT_LOCK(Sts)                                                               \
  {                                                                                     \
    Sts = mPotParam_X.MultiThreadAware_B ? Bof_LockMutex(mPotMtx_X) : BOF_ERR_NO_ERROR; \
  }
#define BOF_POT_UNLOCK()                \
  {                                     \
    if (mPotParam_X.MultiThreadAware_B) \
      Bof_UnlockMutex(mPotMtx_X);       \
  }

struct BOF_POT_PARAM
{
  uint32_t MagicNumber_U32;       /*! If not zero, specify the magic number to use to tag in use pot element (first uint32_t var of pot element) */
  bool MultiThreadAware_B;        /*! true if the object is used in a multi threaded application (use mCs)*/
  bool GetOpPreserveContent_B; /*! false if the pot element is memsetted to 0 before returning it to the caller (exept magic number of course) */
  uint32_t PotCapacity_U32;       /*! Specifies the maximum number of element inside the pot*/
  bool Blocking_B;

  BOF_POT_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    MagicNumber_U32 = 0;
    MultiThreadAware_B = false;
    GetOpPreserveContent_B = false;
    PotCapacity_U32 = 0;
    Blocking_B = false;
  }
};

/*!
 * Summary
 * Pot class
 *
 * Description
 * This class manages a pot object. A pot is like an array of structure which can
 * be got or release by caller as resource. If BOFPOTPARAM.MagicNumber_U32 is not zero,
 * the first uint32_t variable of each pot element is used as MagicNumber to identify valid in use element
 *
 * See Also
 * None
 */

template <typename DataType>
class BofPot
{
private:
  BOF_POT_PARAM mPotParam_X;
  BOF_MUTEX mPotMtx_X; /*! Provide a serialized access to shared resources in a multi threaded environement*/
  DataType *mpPotDataStorage_X;
  DataType *mpFirstPotElementToTryForGet_X;
  DataType *mpLastPotElement_X;
  uint32_t mNumberOfElementOutOfThePot_U32; // Number of element reserved out of the pot
  uint8_t *mpInUseElementList_U8;           /* used list if BOFPOTPARAM.MagicNumber_U32 is zero */
  uint32_t mLevelMax_U32;                   /*! Contains the maximum buffer fill level. This one is reset by the GetMaxLevel method*/

  /*
   * The members mNbUsedReturnedUntilNow_U32/mNbMaxUsedToReturn_U32 were used by GetFirstUsed/GetNextUsed to returns the pot content
   * Now wee iterate the whole list from the first index to the last one because with the following pattern we got an error
   * Pot of 4 Elem, Elem A,B,C and D . is a hole
   * Initial state: AB.C
   *      GetFirstUsed return A and internally set mNbMaxUsedToReturn_U32 to 3
   *      D is inserted
   * State is now:  ABDC
   * GetNextUsed returns B
   * GetNextUsed returns D and stop, so we do not get the last one which is older than D...
   */

  // uint32_t                mNbUsedReturnedUntilNow_U32;
  // uint32_t                mNbMaxUsedToReturn_U32;
  BOFERR mErrorCode_E;
  BOF_EVENT mCanGetEvent_X;

public:
  BofPot(const BOF_POT_PARAM &_rPotParam_X);
  virtual ~BofPot();

  BofPot &operator=(const BofPot &) = delete; // Disallow copying
  BofPot(const BofPot &) = delete;

  BOFERR LastErrorCode()
  {
    return mErrorCode_E;
  }
  BOFERR LockPot();
  BOFERR UnlockPot();
  uint32_t GetMaxLevel()
  {
    return mLevelMax_U32;
  }
  void Reset();
  DataType *Get(uint32_t _BlockingTimeouItInMs_U32);  // As soon as get is called another thread will see it in GetFirstUsed call but its content can be invalid if the thread which has called Get has been preempted
  DataType *Lock(uint32_t _BlockingTimeouItInMs_U32); // Same as get but the other thread can't see it until unlock is called
  BOFERR Unlock(DataType *_pData_X);                  // Used with lock just above
  BOFERR IsPotElementLocked(DataType *_pData_X);  //Lock is not taken to avoid recursive mutex
  BOFERR IsPotElementInUse(DataType *_pData_X);   //Lock is not taken to avoid recursive mutex
  uint32_t GetIndexOfEntry(DataType *_pData_X);
  DataType *GetIndexedPotElement(uint32_t _Index_U32);
  DataType *LookForPotElementInUseStartingFromIndex(uint32_t _Index_U32);
  BOFERR Release(DataType *_pData_X);
  DataType *GetFirstUsed(uint32_t _NbEntryToSkip_U32);
  DataType *GetNextUsed(DataType *_pFirstNextData_X);
  bool IsPotFull()
  {
    return mNumberOfElementOutOfThePot_U32 == 0;
  }
  bool IsPotEmpty()
  {
    return mNumberOfElementOutOfThePot_U32 == mPotParam_X.PotCapacity_U32;
  }
  uint32_t GetNbElementOutOfThePot()
  {
    return mNumberOfElementOutOfThePot_U32;
  }
  uint32_t GetCapacity()
  {
    return mPotParam_X.PotCapacity_U32;
  }
  uint32_t GetNbFreeElement()
  {
    return mPotParam_X.PotCapacity_U32 - mNumberOfElementOutOfThePot_U32;
  }
  uint32_t GetFirstFreeIndexToTry()
  {
    return (uint32_t)(mpFirstPotElementToTryForGet_X - mpPotDataStorage_X);
  }
  BOFERR ClearPot(uint32_t _NbFirstEntryToKeep_U32);

private:
  DataType *Get(bool _Lock_B, uint32_t _BlockingTimeouItInMs_U32);
  // !!!64bits!!!
  // Same than GetIndexOfEntry but without critical section (internal use)
  uint32_t GetIndex(DataType *_pData_X)
  {
    return (uint32_t)(_pData_X - mpPotDataStorage_X);
  }
};

/*!
 * Description
 * This is the constructor of a Pot
 *
 * Parameters
 * _pPotParam_X:  Specifies the object creation parameters and contains the retuned values.
 *
 * Returns
 * None
 *
 * Remarks
 * None
 *
 * See Also
 * ~BofPot
 */

template <typename DataType>
BofPot<DataType>::BofPot(const BOF_POT_PARAM &_rPotParam_X)
{
  uint32_t i_U32;
  DataType *pData_X;

  mLevelMax_U32 = 0;
  mpPotDataStorage_X = nullptr;
  mpFirstPotElementToTryForGet_X = nullptr;
  mpLastPotElement_X = nullptr;
  mNumberOfElementOutOfThePot_U32 = 0;
  mpInUseElementList_U8 = nullptr;
  mErrorCode_E = BOF_ERR_EINVAL;
  if (_rPotParam_X.PotCapacity_U32)
  {
    mPotParam_X = _rPotParam_X;
    if (mPotParam_X.Blocking_B)
    {
      mErrorCode_E = (mPotParam_X.MultiThreadAware_B) ? BOF_ERR_NO_ERROR : BOF_ERR_WRONG_MODE;
    }
    else
    {
      mErrorCode_E = BOF_ERR_NO_ERROR;
    }
    if (mErrorCode_E == BOF_ERR_NO_ERROR)
    {
      mErrorCode_E = mPotParam_X.Blocking_B ? Bof_CreateEvent("pot_canget_" + std::to_string(mPotParam_X.PotCapacity_U32) + "_evt", false, 1, false, mCanGetEvent_X) : BOF_ERR_NO_ERROR;
      if (mErrorCode_E == BOF_ERR_NO_ERROR)
      {
        mErrorCode_E = mPotParam_X.MultiThreadAware_B ? Bof_CreateMutex("BofPot", false, false, mPotMtx_X) : BOF_ERR_NO_ERROR;
        if (mErrorCode_E == BOF_ERR_NO_ERROR)
        {
          mErrorCode_E = BOF_ERR_ENOMEM;

          mpPotDataStorage_X = new DataType[mPotParam_X.PotCapacity_U32];
          mpFirstPotElementToTryForGet_X = mpPotDataStorage_X;
          mpLastPotElement_X = nullptr;
          mpInUseElementList_U8 = nullptr;

          // mNbUsedReturnedUntilNow_U32    = 0;
          // mNbMaxUsedToReturn_U32         = 0;

          if (mpPotDataStorage_X)
          {
            mpLastPotElement_X = &mpPotDataStorage_X[mPotParam_X.PotCapacity_U32 - 1];
            pData_X = mpPotDataStorage_X;

            if (mPotParam_X.MagicNumber_U32)
            {
              for (i_U32 = 0; i_U32 < mPotParam_X.PotCapacity_U32; i_U32++, pData_X++)
              {
                *(uint32_t *)pData_X = 0;
              }
              mErrorCode_E = BOF_ERR_NO_ERROR;
            }
            else
            {
              mpInUseElementList_U8 = new uint8_t[mPotParam_X.PotCapacity_U32];

              if (mpInUseElementList_U8)
              {
                for (i_U32 = 0; i_U32 < mPotParam_X.PotCapacity_U32; i_U32++)
                {
                  mpInUseElementList_U8[i_U32] = 0x00;
                }

                mErrorCode_E = BOF_ERR_NO_ERROR;
              }
            }
            if ((mErrorCode_E == BOF_ERR_NO_ERROR) && (mPotParam_X.Blocking_B))
            {
              Bof_SignalEvent(mCanGetEvent_X, 0);
            }
          }
        }
      }
    }
  }
}

/*!
 * Description
 * This is the destructor of the Pot object. It releases all
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
 * BofPot
 */

template <typename DataType>
BofPot<DataType>::~BofPot()
{
  Bof_DestroyMutex(mPotMtx_X);
  BOF_SAFE_DELETE_ARRAY(mpPotDataStorage_X);
  BOF_SAFE_DELETE_ARRAY(mpInUseElementList_U8);
  Bof_DestroyEvent(mCanGetEvent_X);
}

/*!
 * Description
 * This function returns a pointer to an indexed element of the pot without checking if it
 * is used or not. This function is intended to initialize all pot elements with given data
 * and can be used in cunjunction with BOFPOTPARAM.GetDoNotErasePotElement_B to return pre-filled
 * pot element
 *
 * Parameters
 * None
 *
 * Returns
 * DataType *: A pointer to an indexed element from the pot if the operation is successful
 *
 * Remarks
 * None
 *
 * See Also
 * Pop
 */

template <typename DataType>
DataType *BofPot<DataType>::GetIndexedPotElement(uint32_t _Index_U32)
{
  DataType *pRts_X = nullptr;
  BOFERR Sts_E;

  BOF_POT_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if (_Index_U32 < mPotParam_X.PotCapacity_U32)
    {
      pRts_X = &mpPotDataStorage_X[_Index_U32];
    }
    BOF_POT_UNLOCK();
  }
  return pRts_X;
}

/*!
 * Description
 * This function starts looking inside the pot starting form element index number _Index_U32.
 * It stops when it finds one element in use. This function is intended to enumerate pot element using an
 * integer based iterator
 *
 * Parameters
 * None
 *
 * Returns
 * DataType *: A pointer to an indexed element from the pot if the operation is successful
 *
 * Remarks
 * None
 *
 * See Also
 * Pop
 */

template <typename DataType>
DataType *BofPot<DataType>::LookForPotElementInUseStartingFromIndex(uint32_t _Index_U32)
{
  uint32_t i_U32, CrtIndex_U32;
  DataType *pRts_X = nullptr, *pData_X;
  uint8_t *pLocked_U8;
  BOFERR Sts_E;

  BOF_POT_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if (_Index_U32 < mPotParam_X.PotCapacity_U32)
    {
      pData_X = &mpPotDataStorage_X[0];
      pLocked_U8 = &mpInUseElementList_U8[0];

      for (CrtIndex_U32 = i_U32 = 0; i_U32 < mPotParam_X.PotCapacity_U32; i_U32++, pData_X++, pLocked_U8++)
      {
        if (mPotParam_X.MagicNumber_U32)
        {
          if ((*(uint32_t *)pData_X == mPotParam_X.MagicNumber_U32) || (*(uint32_t *)pData_X == ~mPotParam_X.MagicNumber_U32))
          {
            if (CrtIndex_U32 == _Index_U32)
            {
              pRts_X = pData_X;
              break;
            }
            CrtIndex_U32++;
          }
        }
        else
        {
          if (*pLocked_U8)
          {
            if (CrtIndex_U32 == _Index_U32)
            {
              pRts_X = pData_X;
              break;
            }
            CrtIndex_U32++;
          }
        }
      }
    }
    BOF_POT_UNLOCK();
  }
  return pRts_X;
}

/*!
 * Description
 * This function gets an element from the pot and mark it as "reserved" to avoid that as soon as an entry is poped out of the pot
 * another thread could see it in GetFirstUsed call but its content can be invalid if the thread which has called poped the entry has been preempted
 *
 * If BOFPOTPARAM.MagicNumber_U32 is not zero, the first uint32_t variable of this element
 * is used as MagicNumber to identify valid in use element
 *
 * Parameters
 * None
 *
 * Returns
 * DataType *: A pointer to a free element from the pot if the operation is successful
 *
 * Remarks
 * DataType *Get();		//As soon as get is called another thread will see it in GetFirstUsed call but its content can be invalid if the thread which has called Get has been preempted
 * DataType *Lock();		//Same as get but the other thread can't see it until unlock is called
 * void Unlock();			//Used with lock just above
 *
 * See Also
 * Pop
 */
template <typename DataType>
DataType *BofPot<DataType>::Get(uint32_t _BlockingTimeouItInMs_U32)
{
  return Get(false, _BlockingTimeouItInMs_U32);
}

template <typename DataType>
DataType *BofPot<DataType>::Lock(uint32_t _BlockingTimeouItInMs_U32)
{
  // No because you can kill magic number which has a special value when it is locked (~mPotParam_X.MagicNumber_U32) by using the returned lock pointer pRts
  // and executing *pRts=Userstructure_X (Userstructure_X can have an uncontrolled MagicNumber_U32 value or the "good final" value MagicNumber_U32 and not ~MagicNumber_U32)
  return (mPotParam_X.MagicNumber_U32) ? nullptr : Get(true, _BlockingTimeouItInMs_U32);
}

/*!
 * Description
 * This function unlock an element to the pot which has been locked by Lock()
 * If BOFPOTPARAM.MagicNumber_U32 is not zero, the first uint32_t variable of this
 * element is used as MagicNumber to identify valid in use element
 *
 * Parameters
 * _pData_X: A pointer to a previously lock element. The Lock method returns this value
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

template <typename DataType>
BOFERR BofPot<DataType>::Unlock(DataType *_pData_X)
{
  BOFERR Rts_E;
  uint32_t Index_U32;

  BOF_POT_LOCK(Rts_E);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Rts_E = IsPotElementLocked(_pData_X);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (mPotParam_X.MagicNumber_U32)
      {
        *(uint32_t *)_pData_X = mPotParam_X.MagicNumber_U32;
      }
      else
      {
        Index_U32 = GetIndex(_pData_X);
        mpInUseElementList_U8[Index_U32] = 0xFF;
      }
    }
    BOF_POT_UNLOCK();
  }
  return Rts_E;
}

template <typename DataType>
DataType *BofPot<DataType>::Get(bool _Lock_B, uint32_t _BlockingTimeouItInMs_U32)
{
  BOFERR Sts_E;
  uint32_t i_U32, NumberOfElementOutOfThePot_U32;
  DataType *pRts_X = nullptr, *pData_X;
  uint8_t *pLocked_U8;

RetryGet:
  Sts_E = ((mPotParam_X.Blocking_B) && (_BlockingTimeouItInMs_U32)) ? Bof_WaitForEvent(mCanGetEvent_X, _BlockingTimeouItInMs_U32, 0) : BOF_ERR_NO_ERROR;
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    BOF_POT_LOCK(Sts_E);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      NumberOfElementOutOfThePot_U32 = mNumberOfElementOutOfThePot_U32;
      if (mNumberOfElementOutOfThePot_U32 < mPotParam_X.PotCapacity_U32)
      {
        pData_X = mpFirstPotElementToTryForGet_X;
        pLocked_U8 = &mpInUseElementList_U8[GetFirstFreeIndexToTry()];

        for (i_U32 = 0; i_U32 < mPotParam_X.PotCapacity_U32; i_U32++, pData_X++, pLocked_U8++)
        {
          if (pData_X > mpLastPotElement_X)
          {
            pData_X = mpPotDataStorage_X;
            pLocked_U8 = mpInUseElementList_U8;
          }

          if (mPotParam_X.MagicNumber_U32)
          {
            if ((*(uint32_t *)pData_X != mPotParam_X.MagicNumber_U32) && (*(uint32_t *)pData_X != ~mPotParam_X.MagicNumber_U32))
            {
              if (!mPotParam_X.GetOpPreserveContent_B)
              {
                memset(pData_X, 0, sizeof(DataType)); // Before setting Magic number
              }

              if (_Lock_B)
              {
                *(uint32_t *)pData_X = ~mPotParam_X.MagicNumber_U32;
              }
              else
              {
                *(uint32_t *)pData_X = mPotParam_X.MagicNumber_U32;
              }
              pRts_X = pData_X;
              break;
            }
          }
          else
          {
            if (!*pLocked_U8)
            {
              if (!mPotParam_X.GetOpPreserveContent_B)
              {
                memset(pData_X, 0, sizeof(DataType));
              }

              if (_Lock_B)
              {
                *pLocked_U8 = 0xAA;
              }
              else
              {
                *pLocked_U8 = 0xFF;
              }
              pRts_X = pData_X;
              break;
            }
          }
        }

        if (pRts_X)
        {
          Sts_E = BOF_ERR_NO_ERROR;

          mNumberOfElementOutOfThePot_U32++;
          mpFirstPotElementToTryForGet_X = pRts_X + 1;
          if (mNumberOfElementOutOfThePot_U32 > mLevelMax_U32)
          {
            mLevelMax_U32 = mNumberOfElementOutOfThePot_U32;
          }

          if (mpFirstPotElementToTryForGet_X > mpLastPotElement_X)
          {
            mpFirstPotElementToTryForGet_X = mpPotDataStorage_X;
          }
          NumberOfElementOutOfThePot_U32 = mNumberOfElementOutOfThePot_U32;
        }
      }
      else
      {
        Sts_E = BOF_ERR_EMPTY;
      }
      BOF_POT_UNLOCK()
      if (mPotParam_X.Blocking_B)
      {
        if (Sts_E == BOF_ERR_NO_ERROR)
        {
          if (NumberOfElementOutOfThePot_U32 < mPotParam_X.PotCapacity_U32)
          {
            Bof_SignalEvent(mCanGetEvent_X, 0);
          }
        }
        if (_BlockingTimeouItInMs_U32)
        {
          if (Sts_E == BOF_ERR_EMPTY)
          {
            goto RetryGet;
          }
        }
      }
    }
  }
  return pRts_X;
}

/*!
 * Description
 * This function check if an element is out of the Pot
 *
 * Parameters
 * _pData_X: A pointer to a previously got element. The Get method returns this value
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 *
 * See Also
 * Release
 */

template <typename DataType>
BOFERR BofPot<DataType>::IsPotElementInUse(DataType *_pData_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint32_t Index_U32;
  bool InUse_B;

  if ((_pData_X) && (_pData_X >= mpPotDataStorage_X) && (_pData_X <= mpLastPotElement_X))
  {
    //Lock is not taken to avoid recursive mutex BOF_POT_LOCK(Rts_E);
    Rts_E = BOF_ERR_NO_ERROR;
    //if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (mPotParam_X.MagicNumber_U32)
      {
        InUse_B = ((*(uint32_t *)_pData_X == mPotParam_X.MagicNumber_U32) || (*(uint32_t *)_pData_X == ~mPotParam_X.MagicNumber_U32));
      }
      else
      {
        Index_U32 = GetIndex(_pData_X);
        InUse_B = (mpInUseElementList_U8[Index_U32] != 0);
      }

      if (!InUse_B)
      {
        Rts_E = BOF_ERR_EBUSY;
      }
      //BOF_POT_UNLOCK()
    }
  }
  return Rts_E;
}

/*!
 * Description
 * This function check if an element is out of the Pot in the locked stated
 *
 * Parameters
 * _pData_X: A pointer to a previously lock element. The Lock method returns this value
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 *
 * See Also
 * Release
 */

template <typename DataType>
BOFERR BofPot<DataType>::IsPotElementLocked(DataType *_pData_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint32_t Index_U32;
  bool Locked_B;

  if ((_pData_X) && (_pData_X >= mpPotDataStorage_X) && (_pData_X <= mpLastPotElement_X))
  {
    //Lock is not taken to avoid recursive mutex BOF_POT_LOCK(Rts_E);
    Rts_E = BOF_ERR_NO_ERROR;
    //if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (mPotParam_X.MagicNumber_U32)
      {
        Locked_B = (*(uint32_t *)_pData_X == ~mPotParam_X.MagicNumber_U32);
      }
      else
      {
        Index_U32 = GetIndex(_pData_X);
        Locked_B = (mpInUseElementList_U8[Index_U32] == 0xAA);
      }

      Rts_E = Locked_B ? BOF_ERR_NO_ERROR : BOF_ERR_UNLOCK;
      //BOF_POT_UNLOCK()
    }
  }
  return Rts_E;
}

/*!
 * Description
 * This function returns the storage index of an entry inside the pot
 *
 * Parameters
 * _pData_X: A pointer to a previously got element. The Get method returns this value
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 *
 * See Also
 * Release
 */

template <typename DataType>
uint32_t BofPot<DataType>::GetIndexOfEntry(DataType *_pData_X)
{
  uint32_t Rts_U32 = (uint32_t)-1;
  BOFERR Sts_E;

  BOF_POT_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if (IsPotElementInUse(_pData_X) == BOF_ERR_NO_ERROR)
    {
      Rts_U32 = (uint32_t)(_pData_X - mpPotDataStorage_X);
    }
    BOF_POT_UNLOCK()
  }
  return Rts_U32;
}

/*!
 * Description
 * This function returns an element to the pot.
 * If BOFPOTPARAM.MagicNumber_U32 is not zero, the first uint32_t variable of this
 * element is used as MagicNumber to identify valid in use element
 *
 * Parameters
 * _pData_X: A pointer to a previously got element. The Get method returns this value
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

template <typename DataType>
BOFERR BofPot<DataType>::Release(DataType *_pData_X)
{
  uint32_t Index_U32;
  BOFERR Rts_E;

  BOF_POT_LOCK(Rts_E);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Rts_E = IsPotElementInUse(_pData_X);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (mNumberOfElementOutOfThePot_U32)
      {
        mNumberOfElementOutOfThePot_U32--;
        // No prevent reusing too fast pot element      mpFirstPotElementToTryForGet_X=_pData_X;
        if (mPotParam_X.MagicNumber_U32)
        {
          *(uint32_t *)_pData_X = 0; // ~mPotParam_X.MagicNumber_U32;
        }
        else
        {
          Index_U32 = GetIndex(_pData_X);
          mpInUseElementList_U8[Index_U32] = 0x00;
        }
        Rts_E = BOF_ERR_NO_ERROR;
      }
      else
      {
        Rts_E = BOF_ERR_INTERNAL;
      }
    }
    else
    {
      Rts_E = BOF_ERR_NOT_AVAILABLE;
    }
    BOF_POT_UNLOCK()
  }
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    if (mPotParam_X.Blocking_B)
    {
      Bof_SignalEvent(mCanGetEvent_X, 0);
    }
  }
  return Rts_E;
}

/*!
 * Description
 * This method release all the element of a pot
 *
 * Parameters
 * _NbFirstEntryToKeep_U32:  If this argu�ment is different from 0, the first
 * _NbFirstEntryToKeep_U32 locked Index entries of the pot are kept.
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 *
 * See Also
 * ~BofPot
 */
template <typename DataType>
BOFERR BofPot<DataType>::ClearPot(uint32_t _NbFirstEntryToKeep_U32)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint32_t i_U32, NbLockedKept_U32;
  DataType *pData_X;

  if (_NbFirstEntryToKeep_U32 < mPotParam_X.PotCapacity_U32)
  {
    pData_X = &mpPotDataStorage_X[0];
    NbLockedKept_U32 = 0;
    BOF_POT_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      for (i_U32 = 0; i_U32 < _NbFirstEntryToKeep_U32; i_U32++, pData_X++)
      {
        if (IsPotElementInUse(pData_X) == BOF_ERR_NO_ERROR)
        {
          NbLockedKept_U32++;
        }
      }
      pData_X = &mpPotDataStorage_X[_NbFirstEntryToKeep_U32];

      for (i_U32 = _NbFirstEntryToKeep_U32; i_U32 < mPotParam_X.PotCapacity_U32; i_U32++, pData_X++)
      {
        if (mPotParam_X.MagicNumber_U32)
        {
          *(uint32_t *)pData_X = 0; // ~mPotParam_X.MagicNumber_U32;
        }
        else
        {
          mpInUseElementList_U8[i_U32] = 0;
        }
      }
      mNumberOfElementOutOfThePot_U32 = NbLockedKept_U32;
      if (mPotParam_X.Blocking_B)
      {
        if (mNumberOfElementOutOfThePot_U32 < mPotParam_X.PotCapacity_U32)
        {
          Bof_SignalEvent(mCanGetEvent_X, 0);
        }
      }

      // mNbUsedReturnedUntilNow_U32    = 0;
      // mNbMaxUsedToReturn_U32         = 0;
      mpFirstPotElementToTryForGet_X = &mpPotDataStorage_X[_NbFirstEntryToKeep_U32];
      Rts_E = BOF_ERR_NO_ERROR;
      BOF_POT_UNLOCK()
    }
  }
  return Rts_E;
}

/*!
 * Description
 * This function returns the first used element in the pot
 *
 * Parameters
 *       _NbEntryToSkip_U32: Specifies the number of entries to skip before starting enumerator
 *
 * Returns
 * DataType *: A pointer to a used element from the pot if the operation is successful
 *
 * Remarks
 * None
 *
 * See Also
 * GetNextUsed
 */

template <typename DataType>
DataType *BofPot<DataType>::GetFirstUsed(uint32_t _NbEntryToSkip_U32)
{
  uint32_t i_U32, Nb_U32;
  DataType *pRts_X = nullptr, *pData_X;
  uint8_t *pLocked_U8;
  BOFERR Sts_E;

  BOF_POT_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    // mNbUsedReturnedUntilNow_U32 = 0;
    // mNbMaxUsedToReturn_U32      = 0;

    if (_NbEntryToSkip_U32 < mNumberOfElementOutOfThePot_U32)
    {
      // mNbMaxUsedToReturn_U32 = mNumberOfElementOutOfThePot_U32 - _NbEntryToSkip_U32;
      Nb_U32 = 0;
      pData_X = mpPotDataStorage_X;
      pLocked_U8 = mpInUseElementList_U8;

      for (i_U32 = 0; i_U32 < mPotParam_X.PotCapacity_U32; i_U32++, pData_X++, pLocked_U8++)
      {
        if (mPotParam_X.MagicNumber_U32)
        {
          if (*(uint32_t *)pData_X == mPotParam_X.MagicNumber_U32) // NO !! || (*(uint32_t *)pData_X == ~mPotParam_X.MagicNumber_U32))
          {
            if (Nb_U32 == _NbEntryToSkip_U32)
            {
              pRts_X = pData_X;

              // mNbUsedReturnedUntilNow_U32++;
              break;
            }
            Nb_U32++;
          }
        }
        else
        {
          if (*pLocked_U8 == 0xFF)
          {
            if (Nb_U32 == _NbEntryToSkip_U32)
            {
              pRts_X = pData_X;

              // mNbUsedReturnedUntilNow_U32++;
              break;
            }
            Nb_U32++;
          }
        }
      }
    }
    BOF_POT_UNLOCK()
  }
  return pRts_X;
}

/*!
 * Description
 *       This function returns the next used element in the pot
 *
 * Parameters
 * _pFirstNextData_X:specifies the pointer to the previously returned element
 * This pointer +1 is used as a starting point for the GetNext function call
 * This value is returned by the GetFirstUsed/GetNextUsed functions.
 *
 * By doing like this we can have multiple process iterating in the
 * same pot: It is the caller which store and provide iteration 'context'
 *
 * Returns
 * DataType *: A pointer to a free element from the pot if the operation is successful
 *
 * Remarks
 * None
 *
 * See Also
 * GetFirstUsed
 */

template <typename DataType>
DataType *BofPot<DataType>::GetNextUsed(DataType *_pFirstNextData_X)
{
  uint32_t i_U32, Nb_U32;
  DataType *pRts_X = nullptr, *pData_X;
  uint8_t *pLocked_U8;
  BOFERR Sts_E;

  if ((_pFirstNextData_X) && (_pFirstNextData_X <= mpLastPotElement_X))
  {
    if ((_pFirstNextData_X) && (_pFirstNextData_X >= mpPotDataStorage_X) && (_pFirstNextData_X <= mpLastPotElement_X)
        // && ( mNbUsedReturnedUntilNow_U32 < mNbMaxUsedToReturn_U32 )                           //mNbMaxUsedToReturn_U32 instead of mNumberOfElementOutOfThePot_U32 because we can delete elem during enumeration
    )
    {
      pData_X = _pFirstNextData_X + 1;
      BOF_POT_LOCK(Sts_E);
      if (Sts_E == BOF_ERR_NO_ERROR)
      {
        if (pData_X <= mpLastPotElement_X)
        {
          // !!!64bits!!!
          Nb_U32 = (uint32_t)(mpLastPotElement_X - pData_X + 1);
          pLocked_U8 = &mpInUseElementList_U8[pData_X - mpPotDataStorage_X];

          for (i_U32 = 0; i_U32 < Nb_U32; i_U32++, pData_X++, pLocked_U8++)
          {
            if (mPotParam_X.MagicNumber_U32)
            {
              if (*(uint32_t *)pData_X == mPotParam_X.MagicNumber_U32) // NO !! || (*(uint32_t *)pData_X == ~mPotParam_X.MagicNumber_U32))
              {
                pRts_X = pData_X;

                // mNbUsedReturnedUntilNow_U32++;
                break;
              }
            }
            else
            {
              if (*pLocked_U8 == 0xFF)
              {
                pRts_X = pData_X;

                // mNbUsedReturnedUntilNow_U32++;
                break;
              }
            }
          }
        }
        BOF_POT_UNLOCK()
      }
    }
  }
  return pRts_X;
}

template <typename DataType>
void BofPot<DataType>::Reset()
{
  BOFERR Sts_E;

  BOF_POT_LOCK(Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    mLevelMax_U32 = 0;
    BOF_POT_UNLOCK()
  }
}
template <typename DataType>
BOFERR BofPot<DataType>::LockPot()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mPotParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_LockMutex(mPotMtx_X);
  }
  return Rts_E;
}
template <typename DataType>
BOFERR BofPot<DataType>::UnlockPot()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mPotParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_UnlockMutex(mPotMtx_X);
  }
  return Rts_E;
}
END_BOF_NAMESPACE()