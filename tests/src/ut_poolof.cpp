
/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the poolof class
 *
 * Name:        ut_poolof.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */
#include <bofstd/bofpoolof.h>
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
TEST(PoolOf_Test, Test)
{
  constexpr uint32_t MAX_NB_ITEM = 8;
  uint32_t i_U32;
  ITEM *pItem_X, *ppItemPool_X[MAX_NB_ITEM * 2];
  BOF::BofPoolOf<ITEM> PoolOfItem(MAX_NB_ITEM * 2);

  EXPECT_TRUE(!PoolOfItem.IsEmpty());
  EXPECT_TRUE(PoolOfItem.GetNbItemAvailable() == (MAX_NB_ITEM * 2));
  EXPECT_TRUE(PoolOfItem.GetNbMinItemAvailable(false) == (MAX_NB_ITEM * 2));
  for (i_U32 = 0; i_U32 < MAX_NB_ITEM; i_U32++)
  {
    ppItemPool_X[i_U32] = PoolOfItem.GetItem();
    EXPECT_TRUE(ppItemPool_X[i_U32]);
    ppItemPool_X[i_U32]->ItemPayload_X.Id_U32 = i_U32;
  }
  EXPECT_TRUE(!PoolOfItem.IsEmpty());
  EXPECT_TRUE(PoolOfItem.GetNbItemAvailable() == MAX_NB_ITEM);
  EXPECT_TRUE(PoolOfItem.GetNbMinItemAvailable(true) == MAX_NB_ITEM);
  EXPECT_TRUE(PoolOfItem.GetNbMinItemAvailable(false) == (MAX_NB_ITEM * 2));

  for (i_U32 = 0; i_U32 < MAX_NB_ITEM; i_U32++)
  {
    ppItemPool_X[MAX_NB_ITEM + i_U32] = PoolOfItem.GetItem();
    EXPECT_TRUE(ppItemPool_X[MAX_NB_ITEM + i_U32]);
    ppItemPool_X[MAX_NB_ITEM + i_U32]->ItemPayload_X.Id_U32 = MAX_NB_ITEM + i_U32;
    EXPECT_TRUE(PoolOfItem.GetNbItemAvailable() == MAX_NB_ITEM - i_U32 - 1);
    EXPECT_TRUE(PoolOfItem.GetNbMinItemAvailable(false) == MAX_NB_ITEM - i_U32 - 1);
  }
  EXPECT_TRUE(PoolOfItem.IsEmpty());
  EXPECT_TRUE(PoolOfItem.GetNbItemAvailable() == 0);
  EXPECT_TRUE(PoolOfItem.GetNbMinItemAvailable(true) == 0);
  EXPECT_TRUE(PoolOfItem.GetNbMinItemAvailable(false) == (MAX_NB_ITEM * 2));

  for (i_U32 = 0; i_U32 < MAX_NB_ITEM; i_U32++)
  {
    EXPECT_TRUE(PoolOfItem.ReleaseItem(ppItemPool_X[MAX_NB_ITEM + i_U32]));
  }
  EXPECT_TRUE(!PoolOfItem.IsEmpty());
  EXPECT_TRUE(PoolOfItem.GetNbItemAvailable() == MAX_NB_ITEM);
  EXPECT_TRUE(PoolOfItem.GetNbMinItemAvailable(true) == (MAX_NB_ITEM * 2));
  for (i_U32 = 0; i_U32 < MAX_NB_ITEM; i_U32++)
  {
    pItem_X = PoolOfItem.GetItem();
    EXPECT_TRUE(pItem_X);
    EXPECT_TRUE(pItem_X == ppItemPool_X[(MAX_NB_ITEM * 2) - i_U32 - 1]);
    EXPECT_TRUE(pItem_X->ItemPayload_X.Id_U32 == ((MAX_NB_ITEM * 2) - i_U32 - 1));
    EXPECT_TRUE(PoolOfItem.GetNbItemAvailable() == MAX_NB_ITEM - i_U32 - 1);
    EXPECT_TRUE(PoolOfItem.GetNbMinItemAvailable(false) == MAX_NB_ITEM - i_U32 - 1);
  }
  EXPECT_TRUE(PoolOfItem.IsEmpty());
  EXPECT_TRUE(PoolOfItem.GetNbItemAvailable() == 0);
  EXPECT_TRUE(PoolOfItem.GetNbMinItemAvailable(true) == 0);
  /*
  for (i_U32 = 0; i_U32 < (MAX_NB_ITEM * 2); i_U32++)
  {
    EXPECT_TRUE(PoolOfItem.ReleaseItem(ppItemPool_X[i_U32]));
  }
  EXPECT_TRUE(!PoolOfItem.IsEmpty());
  EXPECT_TRUE(PoolOfItem.GetNbItemAvailable() == (MAX_NB_ITEM * 2));
  EXPECT_TRUE(PoolOfItem.GetNbMinItemAvailable(true) == (MAX_NB_ITEM * 2));
  */
  PoolOfItem.Reset();
  EXPECT_TRUE(!PoolOfItem.IsEmpty());
  EXPECT_TRUE(PoolOfItem.GetNbItemAvailable() == (MAX_NB_ITEM * 2));
  EXPECT_TRUE(PoolOfItem.GetNbMinItemAvailable(true) == (MAX_NB_ITEM * 2));
}
