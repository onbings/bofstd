/*
  BofFifoOf: Implement a fifo with or without internal storage. Locking is used in this class to provide multi thread safe behaviour.
  All item managed by this list must contain an OPAQUE_ITEM_HEADER entry somewhere inside the item structure.
  The item managed by this class is struct but this one can contains objects
*/
#pragma once
#include <condition_variable>
#include "bofstd/bofpoolof.h"

BEGIN_BOF_NAMESPACE()
template <typename T> class BofFifoOf
{
  static_assert(std::is_class<T>::value, "BofFifoOf<T> must be a struct");
  static_assert(has_opaque_item_header<T>::value, "BofFifoOf<T> must contain an OpaqueItemHeader member of type Bof::OPAQUE_ITEM_HEADER");

public:
  // If _MaxNumberOfItemInFifo_U32 is equal to 0, caller must allocate and keep alive the structures stored in this object
  // Otherwise this object will allocate and manage the storage of the items.
  // In this case caller must call GetFreeInternalFifoItem to get a new item, fill it with data and then call Insert to put it in the fifo.
  // When an item is got using WaitForItem, it must be released using the ReleaseInternalFifoItem method when it has been processed.
  BofFifoOf(uint32_t _MaxNumberOfItemInFifo_U32)
      : mpInternalFifoStorage(_MaxNumberOfItemInFifo_U32 ? new BofPoolOf<T>(_MaxNumberOfItemInFifo_U32) : nullptr)
  {
  }
  virtual ~BofFifoOf()
  {
    if (mpInternalFifoStorage)
    {
      delete mpInternalFifoStorage;
    }
  }
  T *GetFreeInternalFifoItem()
  {
    T *pRts_X = nullptr;

    if (mpInternalFifoStorage)
    {
      pRts_X = mpInternalFifoStorage->GetItem();
    }
    return pRts_X;
  }
  bool ReleaseInternalFifoItem(T *_pItem_X)
  {
    bool Rts_B = false;

    if ((mpInternalFifoStorage) && (_pItem_X))
    {
      Rts_B = mpInternalFifoStorage->ReleaseItem(_pItem_X);
    }
    return Rts_B;
  }
  bool Insert(T *_pItem_X, bool _MustWakeUp_B)
  {
    bool Rts_B = false;

    std::lock_guard<std::mutex> lock(mFifoMtx);
    {
      Rts_B = mFifoItemList.InsertHead(_pItem_X);
    }

    if ((Rts_B) && (_MustWakeUp_B))
    {
      WakeUp();
    }
    return Rts_B;
  }
  void WakeUp()
  {
    mWakeUpCondition.notify_all();
  }
  bool WaitForItem(uint32_t _TimeOutInMs_U32, volatile bool *_pMustQuit_B, T **_ppItem_X)
  {
    bool Rts_B = false;
    bool MustQuit_B;

    if (_ppItem_X)
    {
      *_ppItem_X = nullptr;
      if (_pMustQuit_B == nullptr)
      {
        MustQuit_B = false;
        _pMustQuit_B = &MustQuit_B;
      }

      std::unique_lock<std::mutex> Lock(mFifoMtx);
      Rts_B = true;

      auto Predicate = [this, _pMustQuit_B]() { return((!mFifoItemList.IsEmpty()) || (*_pMustQuit_B)); };

      if (_TimeOutInMs_U32 != 0xFFFFFFFF)
      {
        while ((mFifoItemList.IsEmpty()) && (Rts_B) && (*_pMustQuit_B == false))
        {
          if (mWakeUpCondition.wait_for(Lock, std::chrono::milliseconds(_TimeOutInMs_U32), Predicate))
          {
            // Predicate true: either item available or must quit
            break;
          }
          else
          {
            // Timeout
            Rts_B = false;
          }
        }
      }
      else
      {
        while ((mFifoItemList.IsEmpty()) && (Rts_B) && (*_pMustQuit_B == false))
        {
          mWakeUpCondition.wait(Lock);
        }
      }

      if ((Rts_B) && (!mFifoItemList.IsEmpty()))
      {
        *_ppItem_X = mFifoItemList.RemoveTail();
      }
    }
    return Rts_B;
  }

  
  bool IsEmpty()
  {
    bool Rts_B = false;

    std::lock_guard<std::mutex> lock(mFifoMtx);
    {
      Rts_B = mFifoItemList.IsEmpty();
    }

    return Rts_B;
  }
  uint32_t GetNbItemAvailable()
  {
    uint32_t Rts_U32;
    std::lock_guard<std::mutex> lock(mFifoMtx);
    {
      Rts_U32 = mFifoItemList.GetNbItemAvailable();
    }
    return Rts_U32;
  }
  uint32_t GetNbMaxItemAvailable(bool _Reset_B)
  {
    uint32_t Rts_U32;
    std::lock_guard<std::mutex> lock(mFifoMtx);
    {
      Rts_U32 = mFifoItemList.GetNbMaxItemAvailable(_Reset_B);
    }
    return Rts_U32;
  }
  void Reset()
  {
    std::lock_guard<std::mutex> lock(mFifoMtx);
    {
      mFifoItemList.Reset();
      if (mpInternalFifoStorage)
      {
        mpInternalFifoStorage->Reset();
      }
    }
  }

private:
  std::mutex mFifoMtx;
  std::condition_variable mWakeUpCondition;
  BofDoubleListOf<T> mFifoItemList;
  BofPoolOf<T> *mpInternalFifoStorage = nullptr;
};

END_BOF_NAMESPACE()
