
/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the fifoof class
 *
 * Name:        ut_fifoof.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */
#include <bofstd/boffifoof.h>
#include "gtestrunner.h"

struct ITEM
{
  BOF::OPAQUE_ITEM_HEADER OpaqueItemHeader;
  struct
  {
    uint32_t Id_U32;
  } ItemPayload_X;
  ITEM()
  {
    Reset();
  }
  void Reset()
  {
    OpaqueItemHeader.Reset();
    ItemPayload_X.Id_U32 = 0;
  }
};

TEST(FifoOf_Test, Test)
{
  constexpr uint32_t MAX_NB_ITEM = 8;
  uint32_t i_U32;
  ITEM *pItem_X, *ppItemFifo_X[MAX_NB_ITEM * 2];
  BOF::BofFifoOf<ITEM> FifoOfItem(MAX_NB_ITEM * 2);
  bool MustQuit_B;
  uint64_t Timer_U64, Delta_U64;

  assert(FifoOfItem.IsEmpty());
  assert(FifoOfItem.GetNbItemAvailable() == 0);
  assert(FifoOfItem.GetNbMaxItemAvailable(false) == 0);
  for (i_U32 = 0; i_U32 < MAX_NB_ITEM; i_U32++)
  {
    ppItemFifo_X[i_U32] = FifoOfItem.GetFreeInternalFifoItem();
    assert(ppItemFifo_X[i_U32]);
    ppItemFifo_X[i_U32]->ItemPayload_X.Id_U32 = i_U32;
    assert(FifoOfItem.Insert(ppItemFifo_X[i_U32], false));
    assert(!FifoOfItem.IsEmpty());
    assert(FifoOfItem.GetNbItemAvailable() == (i_U32 + 1));
    assert(FifoOfItem.GetNbMaxItemAvailable(true) == (i_U32 + 1));
  }
  MustQuit_B = false;
  for (i_U32 = 0; i_U32 < MAX_NB_ITEM; i_U32++)
  {
    Timer_U64 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
    assert(FifoOfItem.WaitForItem(500, &MustQuit_B, &pItem_X));
    Delta_U64 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count() - Timer_U64;
    assert(pItem_X == ppItemFifo_X[i_U32]);
    assert(pItem_X->ItemPayload_X.Id_U32 == i_U32);
    if (i_U32 == (MAX_NB_ITEM - 1))
    {
      assert(FifoOfItem.IsEmpty());
    }
    else
    {
      assert(!FifoOfItem.IsEmpty());
    }
    assert(FifoOfItem.GetNbItemAvailable() == (MAX_NB_ITEM - i_U32 - 1));
    assert(FifoOfItem.GetNbMaxItemAvailable(true) == 0);
    assert(Delta_U64 < 100);
    assert(FifoOfItem.ReleaseInternalFifoItem(pItem_X));
  }
  Timer_U64 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
  assert(!FifoOfItem.WaitForItem(500, &MustQuit_B, &pItem_X));
  Delta_U64 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count() - Timer_U64;
  assert(Delta_U64 >= 500 * 1000);
  assert(Delta_U64 < 2 * 500 * 1000);
}
