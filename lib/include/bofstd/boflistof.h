/*
  BofSingleListOf: Implement a single link list without lock for add and remove op.
  BofDoubleListOf: Implement a double link list.
  These list handle only pointer to underlying object so the storage of these items must be granted by the caller of
  these template classes.
  A good stirage provider candidate could be PoolOf which can be used as a source of item for this list.
  All item managed by this list must contain an OPAQUE_ITEM_HEADER entry somewhere inside the item structure.
  The item managed by theses classes are struct but these one can contains objects
*/
#pragma once
#include "bofstd/bofstd.h"
#include <atomic>

BEGIN_BOF_NAMESPACE()
#pragma pack(1)
typedef struct
{
  uint64_t low;
  uint64_t high;
} UINT128_T;
#pragma pack()
#if defined(_WIN32)
/* Single Linked List */
typedef __declspec(align(16)) struct alignas(16) _SINGLE_LIST_ITEM // must be aligned 128bits !
{
  struct _SINGLE_LIST_ITEM *pNextItem_X;
} SINGLE_LIST_ITEM;

typedef union _SINGLE_LIST_HEAD {
#if defined(_WIN64)
  UINT128_T _Full_;
  struct
  {
    std::atomic<SINGLE_LIST_ITEM *> pNextItem_X;
    uint64_t _Tag_;
  } NextAndTag;
#else
  uint64_t _Full_;
  struct
  {
    SINGLE_LIST_ITEM *pNextItem_X;
    uint32_t _Tag_;
  } NextAndTag;
#endif
} SINGLE_LIST_HEAD;
/* Doubly Linked List */
typedef __declspec(align(16)) struct _DOUBLE_LIST_ITEM // must be aligned 128bits !
{
  struct _DOUBLE_LIST_ITEM *pNextItem_X;
  struct _DOUBLE_LIST_ITEM *pPrevItem_X;
} DOUBLE_LIST_ITEM;
#else
// If you need a true 128-bit unsigned integer on GCC/Clang, you might define it like this:
// typedef unsigned __int128 UINT128_T_LINUX;

/* Single Linked List */
typedef struct alignas(16) _SINGLE_LIST_ITEM // must be aligned 128bits (16 bytes) !
{
  struct _SINGLE_LIST_ITEM *pNextItem_X;
} SINGLE_LIST_ITEM;

typedef union _SINGLE_LIST_HEAD {
// Check for 64-bit architectures, you might also consider __LP64__ or __aarch64__
// for broader 64-bit Linux compatibility across different architectures.
#if defined(__LP64__)
  UINT128_T _Full_;
  struct
  {
    std::atomic<SINGLE_LIST_ITEM *> pNextItem_X;
    uint64_t _Tag_; // 8 bytes
  } NextAndTag;
#else // Assume 32-bit or other non-64-bit architecture
  uint64_t _Full_; // This will typically be 8 bytes, but might not be atomic 128-bit access
  struct
  {
    SINGLE_LIST_ITEM *pNextItem_X; // Pointer size is 4 bytes on 32-bit
    uint32_t _Tag_;                // 4 bytes
  } NextAndTag;
#endif
} SINGLE_LIST_HEAD;

/* Doubly Linked List */
typedef struct alignas(16) _DOUBLE_LIST_ITEM // must be aligned 128bits (16 bytes) !
{
  struct _DOUBLE_LIST_ITEM *pNextItem_X;
  struct _DOUBLE_LIST_ITEM *pPrevItem_X;
} DOUBLE_LIST_ITEM;
#endif
// Type trait to check if a struct contains a OPAQUE_ITEM_HEADER member
template <typename T> struct has_opaque_item_header
{
private:
  template <typename U> static auto test(int) -> decltype(std::declval<U>().OpaqueItemHeader, std::true_type());

  template <typename> static std::false_type test(...);

public:
  static constexpr bool value = decltype(test<T>(0))::value;
};
typedef enum : uint32_t
{
  ITEM_STATE_FREE,
  ITEM_STATE_TAKEN
} ITEM_STATE;
struct OPAQUE_ITEM_HEADER
{
  DOUBLE_LIST_ITEM Head_X;

  std::atomic<uint32_t> State_E; // Check and prevent copy of this part of the structure
  OPAQUE_ITEM_HEADER()
  {
    Reset();
  }
  void Reset()
  {
    // No !! Head_X = {};
    State_E = ITEM_STATE_FREE;
  }
};
/* Singly Linked List */
/* These list uses Atomic Access by default so there so need to protect access */
/* Also, ABA issue is avoided by using a 32/64 bits tag */
template <typename T> class BofSingleListOf
{
  static_assert(std::is_class<T>::value, "BofSingleListOf<T> must be a struct");
  static_assert(has_opaque_item_header<T>::value, "BofSingleListOf<T> must contain an OpaqueItemHeader member of type Bof::OPAQUE_ITEM_HEADER");

public:
  // BofSingleListOf<T>()
  BofSingleListOf() // C++20
  {
    mNbItemAvailable = 0;
    mNbMaxItemAvailable = 0;
    mListHead_X.NextAndTag.pNextItem_X = (SINGLE_LIST_ITEM *)&mListHead_X;
    mListHead_X.NextAndTag._Tag_ = 0;
  }
  // virtual ~BofSingleListOf<T>()
  virtual ~BofSingleListOf() // C++20
  {
  }

  bool InsertHead(T *_pItem_X)
  {
    bool Rts_B = false;
    SINGLE_LIST_ITEM *pTempItem_X;

    if (_pItem_X)
    {
      /*
      #if defined(_WIN32)
            do
            {
              pTempItem_X = mListHead_X.NextAndTag.pNextItem_X;
              _pItem_X->OpaqueItemHeader.Head_X.pNextItem_X = (DOUBLE_LIST_ITEM *)pTempItem_X;
            } while (InterlockedCompareExchangePointer((PVOID volatile *)&mListHead_X.NextAndTag.pNextItem_X, (PVOID)_pItem_X, (PVOID)pTempItem_X) !=
      pTempItem_X); #else
      */
      do
      {
        pTempItem_X = std::atomic_load(&mListHead_X.NextAndTag.pNextItem_X);
        _pItem_X->OpaqueItemHeader.Head_X.pNextItem_X = (DOUBLE_LIST_ITEM *)pTempItem_X;
      } while (!std::atomic_compare_exchange_weak(&mListHead_X.NextAndTag.pNextItem_X, &pTempItem_X, (SINGLE_LIST_ITEM *)_pItem_X));
      // #endif
      if (pTempItem_X != mListHead_X.NextAndTag.pNextItem_X)
      {
        Rts_B = true;
        mNbItemAvailable++;
        if (mNbItemAvailable > mNbMaxItemAvailable)
        {
          mNbMaxItemAvailable.store(mNbItemAvailable);
        }
      }
    }
    return Rts_B;
  }

  T *RemoveHead()
  {
    T *pRts_X = nullptr;
    alignas(16) SINGLE_LIST_HEAD NewHead_X = {};
    alignas(16) SINGLE_LIST_HEAD OldHead_X = {};
    // 64 bit version here...
    do
    {
      OldHead_X._Full_.high = *(volatile uint64_t *)&mListHead_X._Full_.high;
      OldHead_X._Full_.low = *(volatile uint64_t *)&mListHead_X._Full_.low;
      NewHead_X.NextAndTag._Tag_ = OldHead_X.NextAndTag._Tag_ + 1;
      NewHead_X.NextAndTag.pNextItem_X = OldHead_X.NextAndTag.pNextItem_X.load()->pNextItem_X;
#if defined(_WIN32)
    } while (InterlockedCompareExchange128((volatile LONG64 *)&mListHead_X.NextAndTag.pNextItem_X, (LONG64)NewHead_X.NextAndTag._Tag_,
                                           (LONG64)NewHead_X.NextAndTag.pNextItem_X.load(), (LONG64 *)&OldHead_X.NextAndTag.pNextItem_X) == 0);
#else
    } while (!__atomic_compare_exchange(&mListHead_X._Full_, // pointer to the 128-bit value to update
                                        &OldHead_X._Full_,   // expected old value (updated with actual on failure)
                                        &NewHead_X._Full_,   // desired new value
                                        false,               // strong compare-exchange
                                        __ATOMIC_SEQ_CST,    // memory order for success
                                        __ATOMIC_SEQ_CST));
#endif
    if (OldHead_X.NextAndTag.pNextItem_X != (SINGLE_LIST_ITEM *)&mListHead_X)
    {
      mNbItemAvailable--;
      pRts_X = BOF_CONTAINER_OF(OldHead_X.NextAndTag.pNextItem_X.load(), T, OpaqueItemHeader.Head_X);
      // pRts_X = (SINGLE_LIST_ITEM *)OldHead_X.pNextItem_X;
    }
    return pRts_X;
  }
  bool IsEmpty()
  {
    return (mListHead_X.NextAndTag.pNextItem_X == (SINGLE_LIST_ITEM *)&mListHead_X);
  }
  uint32_t GetNbItemAvailable()
  {
    return mNbItemAvailable;
  }
  uint32_t GetNbMaxItemAvailable(bool _Reset_B)
  {
    uint32_t Rts_U32 = mNbMaxItemAvailable;
    if (_Reset_B)
    {
      mNbMaxItemAvailable = 0;
    }
    return Rts_U32;
  }
  void Reset()
  {
    while (RemoveHead() != nullptr)
    {
    }
    mNbMaxItemAvailable = 0;
  }

private:
  alignas(16) SINGLE_LIST_HEAD mListHead_X = {};
  std::atomic<uint32_t> mNbItemAvailable = 0;
  std::atomic<uint32_t> mNbMaxItemAvailable = 0;
};

template <typename T> class BofDoubleListOf
{
  static_assert(std::is_class<T>::value, "BofDoubleListOf<T> must be a struct");
  static_assert(has_opaque_item_header<T>::value, "BofDoubleListOf<T> must contain an OpaqueItemHeader member of type Bof::OPAQUE_ITEM_HEADER");

public:
  BofDoubleListOf()
  {
    mNbItemAvailable = 0;
    mNbMaxItemAvailable = 0;
    mListHead_X.pNextItem_X = mListHead_X.pPrevItem_X = &mListHead_X;
  }
  virtual ~BofDoubleListOf()
  {
  }
  bool IsEmpty()
  {
    return ((mListHead_X.pNextItem_X == &mListHead_X) && (mListHead_X.pPrevItem_X == &mListHead_X));
  }
  static DOUBLE_LIST_ITEM *S_GetNextItem(DOUBLE_LIST_ITEM *_pItem_X)
  {
    return _pItem_X ? _pItem_X->pNextItem_X : nullptr;
  }
  static DOUBLE_LIST_ITEM *S_GetPrevItem(DOUBLE_LIST_ITEM *_pItem_X)
  {
    return _pItem_X ? _pItem_X->pPrevItem_X : nullptr;
  }

  bool InsertHead(T *_pItem_X)
  {
    bool Rts_B = false;
    DOUBLE_LIST_ITEM *pListItem_X, *pItem_X;

    if (_pItem_X)
    {
      Rts_B = true;
      pListItem_X = mListHead_X.pNextItem_X;
      pItem_X = (DOUBLE_LIST_ITEM *)_pItem_X;
      pItem_X->pNextItem_X = mListHead_X.pNextItem_X;
      mListHead_X.pNextItem_X = pItem_X;

      pListItem_X->pPrevItem_X = pItem_X;
      pItem_X->pPrevItem_X = &mListHead_X;

      mNbItemAvailable++;
      if (mNbItemAvailable > mNbMaxItemAvailable)
      {
        mNbMaxItemAvailable.store(mNbItemAvailable);
      }
    }
    return Rts_B;
  }
  bool InsertTail(T *_pItem_X)
  {
    bool Rts_B = false;
    DOUBLE_LIST_ITEM *pListItem_X, *pItem_X;

    if (_pItem_X)
    {
      Rts_B = true;
      pListItem_X = mListHead_X.pPrevItem_X;
      pItem_X = (DOUBLE_LIST_ITEM *)_pItem_X;

      pItem_X->pPrevItem_X = mListHead_X.pPrevItem_X;
      mListHead_X.pPrevItem_X = pItem_X;

      pListItem_X->pNextItem_X = pItem_X;
      pItem_X->pNextItem_X = &mListHead_X;

      mNbItemAvailable++;
      if (mNbItemAvailable > mNbMaxItemAvailable)
      {
        mNbMaxItemAvailable.store(mNbItemAvailable);
      }
    }
    return Rts_B;
  }

  T *RemoveHead()
  {
    T *pRts_X = nullptr;
    DOUBLE_LIST_ITEM *pItem_X;

    if (!IsEmpty())
    {
      mNbItemAvailable--;
      pItem_X = mListHead_X.pNextItem_X;
      mListHead_X.pNextItem_X = pItem_X->pNextItem_X;
      pItem_X->pNextItem_X->pPrevItem_X = &mListHead_X;
      pRts_X = BOF_CONTAINER_OF(pItem_X, T, OpaqueItemHeader.Head_X);
    }
    return pRts_X;
  }

  T *RemoveTail()
  {
    T *pRts_X = nullptr;
    DOUBLE_LIST_ITEM *pItem_X;

    if (!IsEmpty())
    {
      mNbItemAvailable--;
      pItem_X = mListHead_X.pPrevItem_X;

      mListHead_X.pPrevItem_X = pItem_X->pPrevItem_X;
      pItem_X->pPrevItem_X->pNextItem_X = &mListHead_X;
      pRts_X = BOF_CONTAINER_OF(pItem_X, T, OpaqueItemHeader.Head_X);
    }
    return pRts_X;
  }
  bool RemoveItem(T *_pItem_X)
  {
    bool Rts_B = false;
    DOUBLE_LIST_ITEM *pItem_X, *pNextListItem_X, *pPrevListItem_X;

    if ((_pItem_X) && (!IsEmpty()))
    {
      mNbItemAvailable--;
      Rts_B = true;
      pItem_X = _pItem_X->ItemHeader_X.Head_X;
      pNextListItem_X = pItem_X->pNextItem_X;
      pPrevListItem_X = pItem_X->pPrevItem_X;

      pNextListItem_X->pPrevItem_X = pPrevListItem_X;
      pPrevListItem_X->pNextItem_X = pNextListItem_X;
    }
    return Rts_B;
  }
  uint32_t GetNbItemAvailable()
  {
    return mNbItemAvailable;
  }
  uint32_t GetNbMaxItemAvailable(bool _Reset_B)
  {
    uint32_t Rts_U32 = mNbMaxItemAvailable;
    if (_Reset_B)
    {
      mNbMaxItemAvailable = 0;
    }
    return Rts_U32;
  }
  void Reset()
  {
    while (RemoveHead() != nullptr) //&mListHead_X)
    {
    }
    mNbMaxItemAvailable = 0;
  }

private:
  alignas(16) DOUBLE_LIST_ITEM mListHead_X = {};
  std::atomic<uint32_t> mNbItemAvailable = 0;
  std::atomic<uint32_t> mNbMaxItemAvailable = 0;
};


END_BOF_NAMESPACE()
