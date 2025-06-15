/*
  BofPoolOf: Implement a storage pool without lock for add and remove op.
  All item managed by this list must contain an OPAQUE_ITEM_HEADER entry somewhere inside the item structure.
  The item managed by this class is struct but this one can contains objects
*/
#pragma once
#include "bofstd/boflistof.h"
#include <cassert>
#include <cstdint>

BEGIN_BOF_NAMESPACE()
template <typename T> class BofPoolOf
{
  static_assert(std::is_class<T>::value, "BofPoolOf<T> must be a struct");
  static_assert(has_opaque_item_header<T>::value, "BofPoolOf<T> must contain an OpaqueItemHeader member of type Bof::OPAQUE_ITEM_HEADER");

public:
  BofPoolOf(uint32_t _MaxNumberOfItemInPool_U32)
  {
    uint32_t i_U32;

    mMaxNumberOfItemInPool_U32 = _MaxNumberOfItemInPool_U32;
    mpPoolItem_X = new T[mMaxNumberOfItemInPool_U32];
    assert(mpPoolItem_X != nullptr);
    mNbMinPoolItemAvailable = mMaxNumberOfItemInPool_U32;
    // mNbPoolItemAvailable = mMaxNumberOfItemInPool_U32;
    for (i_U32 = 0; i_U32 < mMaxNumberOfItemInPool_U32; i_U32++)
    {
      // mpPoolItem_X[i_U32].OpaqueItemHeader.State_E = ITEM_STATE_FREE;  //Done in OPAQUE_ITEM_HEADER Header
      mItemList.InsertHead(&mpPoolItem_X[i_U32]);
    }
  }
  virtual ~BofPoolOf()
  {
    delete[] mpPoolItem_X;
  }

  T *GetItem()
  {
    uint32_t AtomicRes_U32 = 0;
    T *pPoolItemGot_X, *pRts_X = nullptr;

    // Check Inputs
    // Get item from pool
    pPoolItemGot_X = mItemList.RemoveHead();
    if (pPoolItemGot_X)
    {
      // Check ring was really free
      AtomicRes_U32 = std::atomic_exchange(&pPoolItemGot_X->OpaqueItemHeader.State_E, ITEM_STATE_TAKEN);
      assert(AtomicRes_U32 == ITEM_STATE_FREE);
      if (AtomicRes_U32 == ITEM_STATE_FREE)
      {
        // AtomicRes_U32 = --mNbPoolItemAvailable;
        AtomicRes_U32 = mItemList.GetNbItemAvailable();
        assert((AtomicRes_U32 & 0x80000000) == 0);
        if ((AtomicRes_U32 & 0x80000000) == 0)
        {
          if (AtomicRes_U32 < mNbMinPoolItemAvailable)
          {
            mNbMinPoolItemAvailable = AtomicRes_U32;
          }
          pRts_X = pPoolItemGot_X;
        }
      }
    }
    return pRts_X;
  }

  bool ReleaseItem(T *_pPoolItem_X)
  {
    bool Rts_B = false;
    uint32_t AtomicRes_U32;

    if (_pPoolItem_X)
    {
      // Check ring was really free
      AtomicRes_U32 = std::atomic_exchange(&_pPoolItem_X->OpaqueItemHeader.State_E, ITEM_STATE_FREE);
      assert(AtomicRes_U32 == ITEM_STATE_TAKEN);
      if (AtomicRes_U32 == ITEM_STATE_TAKEN)
      {
        // re-insert item to the pool
        if (mItemList.InsertHead(_pPoolItem_X))
        {
          // increase number of item
          // AtomicRes_U32 = mNbPoolItemAvailable++;
          AtomicRes_U32 = mItemList.GetNbItemAvailable();
          assert(AtomicRes_U32 <= mMaxNumberOfItemInPool_U32);
          if (AtomicRes_U32 <= mMaxNumberOfItemInPool_U32)
          {
            Rts_B = true;
          }
        }
      }
    }
    return Rts_B;
  }
  bool IsEmpty()
  {
    // return (mItemNbPoolItemAvailable == 0);
    return (mItemList.IsEmpty());
  }
  uint32_t GetNbItemAvailable()
  {
    //    return mNbPoolItemAvailable;
    return mItemList.GetNbItemAvailable();
  }
  uint32_t GetNbMinItemAvailable(bool _Reset_B)
  {
    uint32_t Rts_U32 = mNbMinPoolItemAvailable;
    if (_Reset_B)
    {
      mNbMinPoolItemAvailable = mMaxNumberOfItemInPool_U32;
    }
    return Rts_U32;
  }
  void Reset()
  {
    uint32_t i_U32;

    while (mItemList.RemoveHead() != nullptr)
    {
    }
    mNbMinPoolItemAvailable = mMaxNumberOfItemInPool_U32;
    // mNbPoolItemAvailable = mMaxNumberOfItemInPool_U32;
    for (i_U32 = 0; i_U32 < mMaxNumberOfItemInPool_U32; i_U32++)
    {
      mpPoolItem_X[i_U32].OpaqueItemHeader.State_E = ITEM_STATE_FREE;
      mItemList.InsertHead(&mpPoolItem_X[i_U32]);
    }
  }

private:
  uint32_t mMaxNumberOfItemInPool_U32 = 0;
  BofSingleListOf<T> mItemList;
  // std::atomic<uint32_t> mNbPoolItemAvailable = 0;
  std::atomic<uint32_t> mNbMinPoolItemAvailable = 0;
  // List item storage zone
  T *mpPoolItem_X = nullptr;
};

END_BOF_NAMESPACE()
