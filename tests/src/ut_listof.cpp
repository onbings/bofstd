/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the listof class
 *
 * Name:        ut_listof.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */
#include <bofstd/boflistof.h>
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


TEST(ListOf_Test, Single)
{
  constexpr uint32_t MAX_NB_ITEM = 8;
  uint32_t i_U32;
  ITEM *pSingleItem_X, pItemSingle_X[MAX_NB_ITEM * 2];
  BOF::BofSingleListOf<ITEM> SingleListOfItem;

  for (i_U32 = 0; i_U32 < (MAX_NB_ITEM * 2); i_U32++)
  {
    pItemSingle_X[i_U32].ItemPayload_X.Id_U32 = i_U32;
  }
  EXPECT_TRUE(SingleListOfItem.IsEmpty());
  EXPECT_TRUE(SingleListOfItem.GetNbItemAvailable() == 0);
  EXPECT_TRUE(SingleListOfItem.GetNbMaxItemAvailable(false) == 0);
  for (i_U32 = 0; i_U32 < MAX_NB_ITEM; i_U32++)
  {
    EXPECT_TRUE(SingleListOfItem.InsertHead(&pItemSingle_X[i_U32]));
    EXPECT_TRUE(pItemSingle_X[i_U32].ItemPayload_X.Id_U32 == i_U32);
  }
  EXPECT_TRUE(!SingleListOfItem.IsEmpty());
  EXPECT_TRUE(SingleListOfItem.GetNbItemAvailable() == MAX_NB_ITEM);
  EXPECT_TRUE(SingleListOfItem.GetNbMaxItemAvailable(true) == MAX_NB_ITEM);
  EXPECT_TRUE(SingleListOfItem.GetNbMaxItemAvailable(false) == 0);

  for (i_U32 = 0; i_U32 < MAX_NB_ITEM; i_U32++)
  {
    EXPECT_TRUE(SingleListOfItem.InsertHead(&pItemSingle_X[MAX_NB_ITEM + i_U32]));
    EXPECT_TRUE(pItemSingle_X[MAX_NB_ITEM + i_U32].ItemPayload_X.Id_U32 == (MAX_NB_ITEM + i_U32));
  }
  EXPECT_TRUE(!SingleListOfItem.IsEmpty());
  EXPECT_TRUE(SingleListOfItem.GetNbItemAvailable() == (MAX_NB_ITEM * 2));
  EXPECT_TRUE(SingleListOfItem.GetNbMaxItemAvailable(true) == (MAX_NB_ITEM * 2));

  for (i_U32 = 0; i_U32 < MAX_NB_ITEM; i_U32++)
  {
    pSingleItem_X = SingleListOfItem.RemoveHead();
    EXPECT_TRUE(pSingleItem_X);
    EXPECT_TRUE(pSingleItem_X->ItemPayload_X.Id_U32 == ((MAX_NB_ITEM * 2) - i_U32 - 1));
  }
  EXPECT_TRUE(!SingleListOfItem.IsEmpty());
  EXPECT_TRUE(SingleListOfItem.GetNbItemAvailable() == MAX_NB_ITEM);
  EXPECT_TRUE(SingleListOfItem.GetNbMaxItemAvailable(true) == 0);
  for (i_U32 = 0; i_U32 < MAX_NB_ITEM; i_U32++)
  {
    pSingleItem_X = SingleListOfItem.RemoveHead();
    EXPECT_TRUE(pSingleItem_X);
    EXPECT_TRUE(pSingleItem_X->ItemPayload_X.Id_U32 == (MAX_NB_ITEM - i_U32 - 1));
  }
  EXPECT_TRUE(SingleListOfItem.IsEmpty());
  EXPECT_TRUE(SingleListOfItem.GetNbItemAvailable() == 0);
  EXPECT_TRUE(SingleListOfItem.GetNbMaxItemAvailable(true) == 0);

  for (i_U32 = 0; i_U32 < (MAX_NB_ITEM * 2); i_U32++)
  {
    EXPECT_TRUE(SingleListOfItem.InsertHead(&pItemSingle_X[i_U32]));
    EXPECT_TRUE(SingleListOfItem.GetNbItemAvailable() == i_U32 + 1);
    EXPECT_TRUE(SingleListOfItem.GetNbMaxItemAvailable(false) == i_U32 + 1);
  }
  EXPECT_TRUE(!SingleListOfItem.IsEmpty());
  EXPECT_TRUE(SingleListOfItem.GetNbItemAvailable() == (MAX_NB_ITEM * 2));
  EXPECT_TRUE(SingleListOfItem.GetNbMaxItemAvailable(true) == (MAX_NB_ITEM * 2));

  SingleListOfItem.Reset();
  EXPECT_TRUE(SingleListOfItem.IsEmpty());
  EXPECT_TRUE(SingleListOfItem.GetNbItemAvailable() == 0);
  EXPECT_TRUE(SingleListOfItem.GetNbMaxItemAvailable(true) == 0);
}

TEST(ListOf_Test, Double)
{
  constexpr uint32_t MAX_NB_ITEM = 8;
  uint32_t i_U32;
  ITEM *pDoubleItem_X, pItemDouble_X[MAX_NB_ITEM * 2];
  BOF::BofDoubleListOf<ITEM> DoubleListOfItem;

  for (i_U32 = 0; i_U32 < (MAX_NB_ITEM * 2); i_U32++)
  {
    pItemDouble_X[i_U32].ItemPayload_X.Id_U32 = i_U32;
  }
  EXPECT_TRUE(DoubleListOfItem.IsEmpty());
  EXPECT_TRUE(DoubleListOfItem.GetNbItemAvailable() == 0);
  EXPECT_TRUE(DoubleListOfItem.GetNbMaxItemAvailable(false) == 0);
  for (i_U32 = 0; i_U32 < MAX_NB_ITEM; i_U32++)
  {
    EXPECT_TRUE(DoubleListOfItem.InsertHead(&pItemDouble_X[i_U32]));
    EXPECT_TRUE(pItemDouble_X[i_U32].ItemPayload_X.Id_U32 == i_U32);
  }
  EXPECT_TRUE(!DoubleListOfItem.IsEmpty());
  EXPECT_TRUE(DoubleListOfItem.GetNbItemAvailable() == MAX_NB_ITEM);
  EXPECT_TRUE(DoubleListOfItem.GetNbMaxItemAvailable(true) == MAX_NB_ITEM);
  EXPECT_TRUE(DoubleListOfItem.GetNbMaxItemAvailable(false) == 0);

  for (i_U32 = 0; i_U32 < MAX_NB_ITEM; i_U32++)
  {
    EXPECT_TRUE(DoubleListOfItem.InsertTail(&pItemDouble_X[MAX_NB_ITEM + i_U32]));
    EXPECT_TRUE(pItemDouble_X[MAX_NB_ITEM + i_U32].ItemPayload_X.Id_U32 == (MAX_NB_ITEM + i_U32));
    EXPECT_TRUE(DoubleListOfItem.GetNbItemAvailable() == (MAX_NB_ITEM + i_U32 + 1));
    EXPECT_TRUE(DoubleListOfItem.GetNbMaxItemAvailable(true) == (MAX_NB_ITEM + i_U32 + 1));
  }
  EXPECT_TRUE(!DoubleListOfItem.IsEmpty());
  EXPECT_TRUE(DoubleListOfItem.GetNbItemAvailable() == (MAX_NB_ITEM * 2));
  EXPECT_TRUE(DoubleListOfItem.GetNbMaxItemAvailable(true) == 0);

  for (i_U32 = 0; i_U32 < MAX_NB_ITEM; i_U32++)
  {
    pDoubleItem_X = DoubleListOfItem.RemoveHead();
    EXPECT_TRUE(pDoubleItem_X);
    EXPECT_TRUE(pDoubleItem_X->ItemPayload_X.Id_U32 == (MAX_NB_ITEM - i_U32 - 1));
  }
  EXPECT_TRUE(!DoubleListOfItem.IsEmpty());
  EXPECT_TRUE(DoubleListOfItem.GetNbItemAvailable() == MAX_NB_ITEM);
  EXPECT_TRUE(DoubleListOfItem.GetNbMaxItemAvailable(true) == 0);
  for (i_U32 = 0; i_U32 < MAX_NB_ITEM; i_U32++)
  {
    pDoubleItem_X = DoubleListOfItem.RemoveTail();
    EXPECT_TRUE(pDoubleItem_X);
    EXPECT_TRUE(pDoubleItem_X->ItemPayload_X.Id_U32 == ((MAX_NB_ITEM * 2) - i_U32 - 1));
  }
  EXPECT_TRUE(DoubleListOfItem.IsEmpty());
  EXPECT_TRUE(DoubleListOfItem.GetNbItemAvailable() == 0);
  EXPECT_TRUE(DoubleListOfItem.GetNbMaxItemAvailable(true) == 0);

  for (i_U32 = 0; i_U32 < (MAX_NB_ITEM * 2); i_U32++)
  {
    EXPECT_TRUE(DoubleListOfItem.InsertHead(&pItemDouble_X[i_U32]));
  }
  EXPECT_TRUE(!DoubleListOfItem.IsEmpty());
  EXPECT_TRUE(DoubleListOfItem.GetNbItemAvailable() == (MAX_NB_ITEM * 2));
  EXPECT_TRUE(DoubleListOfItem.GetNbMaxItemAvailable(true) == (MAX_NB_ITEM * 2));

  DoubleListOfItem.Reset();
  EXPECT_TRUE(DoubleListOfItem.IsEmpty());
  EXPECT_TRUE(DoubleListOfItem.GetNbItemAvailable() == 0);
  EXPECT_TRUE(DoubleListOfItem.GetNbMaxItemAvailable(true) == 0);
}

