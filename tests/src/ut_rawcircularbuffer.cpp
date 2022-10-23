/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the raw circular buffer class
 *
 * Name:        ut_rawcircularbuffer.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */
#include <bofstd/bofrawcircularbuffer.h>

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

#define NBPUSHSAMESIZE    16

class BofRawCircularBufferNoSlotsize_Test :public testing::Test
{
public:
  BofRawCircularBufferNoSlotsize_Test() : mpBofRawCircularBuffer_O(nullptr)
  {
  }

  // Per-test-case set-up. Called before the first test in this test case.
  static void                   SetUpTestCase();

  // Per-test-case tear-down. Called after the last test in this test case.
  static void                   TearDownTestCase();

protected:

  // You can define per-test set-up and tear-down logic as usual.
  virtual void                  SetUp();
  virtual void                  TearDown();

  BofRawCircularBuffer *mpBofRawCircularBuffer_O;
  BOF_RAW_CIRCULAR_BUFFER_PARAM mBofRawCircularBufferParam_X;

private:
};


/*** Factory functions called at the beginning/end of each test case **********/

void BofRawCircularBufferNoSlotsize_Test::SetUpTestCase()
{}


void BofRawCircularBufferNoSlotsize_Test::TearDownTestCase()
{}


void BofRawCircularBufferNoSlotsize_Test::SetUp()
{
  mBofRawCircularBufferParam_X.Reset();
  mBofRawCircularBufferParam_X.MultiThreadAware_B = true;
  mBofRawCircularBufferParam_X.BufferSizeInByte_U32 = 0x1000;
  mBofRawCircularBufferParam_X.NbMaxSlot_U32 = 0;
  mBofRawCircularBufferParam_X.pData_U8 = nullptr;
  mpBofRawCircularBuffer_O = new BofRawCircularBuffer(mBofRawCircularBufferParam_X);

  EXPECT_TRUE(mpBofRawCircularBuffer_O != nullptr);
  EXPECT_TRUE(mpBofRawCircularBuffer_O->LastErrorCode() == BOF_ERR_NO_ERROR);
}

void BofRawCircularBufferNoSlotsize_Test::TearDown()
{
  BOF_SAFE_DELETE(mpBofRawCircularBuffer_O);
  EXPECT_TRUE(mpBofRawCircularBuffer_O == nullptr);
}

TEST_F(BofRawCircularBufferNoSlotsize_Test, ByteBuffer)
{
  uint32_t Nb_U32, i_U32, Index_U32, Size_U32;
  BOFERR   Sts_E;
  bool     Sts_B;
  uint8_t  pData_U8[0x1000];
  uint32_t pNb1_U32[8], pNb2_U32[8];
  uint8_t *ppData1_U8[8], *ppData2_U8[8];

  Sts_B = mpBofRawCircularBuffer_O->IsEmpty();
  EXPECT_TRUE(Sts_B);
  Sts_B = mpBofRawCircularBuffer_O->IsFull();
  EXPECT_FALSE(Sts_B);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbElement();
  EXPECT_EQ(0, Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetCapacity();
  EXPECT_EQ(mBofRawCircularBufferParam_X.BufferSizeInByte_U32, Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbFreeElement();
  EXPECT_EQ(mBofRawCircularBufferParam_X.BufferSizeInByte_U32 - sizeof(uint32_t), Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(0, Nb_U32);

  for (i_U32 = 0; i_U32 < sizeof(pData_U8); i_U32++)
  {
    pData_U8[i_U32] = (uint8_t)i_U32;
  }

  Size_U32 = 10;
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(nullptr, pData_U8);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Size_U32, nullptr);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(nullptr, nullptr);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Size_U32, pData_U8);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(0, Size_U32);

  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(0, pData_U8);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(10, nullptr);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(0, nullptr);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);

  Index_U32 = 0;
  Size_U32 = 10;
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Size_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Sts_B = mpBofRawCircularBuffer_O->IsEmpty();
  EXPECT_FALSE(Sts_B);
  Sts_B = mpBofRawCircularBuffer_O->IsFull();
  EXPECT_FALSE(Sts_B);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbElement();
  EXPECT_EQ(Nb_U32, Size_U32 + sizeof(uint32_t));
  Nb_U32 = mpBofRawCircularBuffer_O->GetCapacity();
  EXPECT_EQ(mBofRawCircularBufferParam_X.BufferSizeInByte_U32, Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbFreeElement();
  EXPECT_EQ(Nb_U32, mBofRawCircularBufferParam_X.BufferSizeInByte_U32 - Size_U32 - sizeof(uint32_t) - sizeof(uint32_t)); // 1 uint32_t for size and another one for next push
  Nb_U32 = mpBofRawCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(Nb_U32, Size_U32 + sizeof(uint32_t));


  Index_U32 = 0;
  Size_U32 = 9;
  memset(&pData_U8[Index_U32], 0, Size_U32);
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Size_U32, &pData_U8[Index_U32]);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(10, Size_U32);

  Index_U32 = 0;
  Size_U32 = 10;
  memset(&pData_U8[Index_U32], 0, Size_U32);
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Size_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(10, Size_U32);

  Sts_B = mpBofRawCircularBuffer_O->IsEmpty();
  EXPECT_TRUE(Sts_B);
  Sts_B = mpBofRawCircularBuffer_O->IsFull();
  EXPECT_FALSE(Sts_B);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbElement();
  EXPECT_EQ(Nb_U32, 0);
  Nb_U32 = mpBofRawCircularBuffer_O->GetCapacity();
  EXPECT_EQ(mBofRawCircularBufferParam_X.BufferSizeInByte_U32, Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbFreeElement();
  EXPECT_EQ(Nb_U32, mBofRawCircularBufferParam_X.BufferSizeInByte_U32 - sizeof(uint32_t));
  Nb_U32 = mpBofRawCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(Nb_U32, Size_U32 + sizeof(uint32_t));

  for (i_U32 = 0; i_U32 < Size_U32; i_U32++)
  {
    EXPECT_EQ(pData_U8[i_U32], (uint8_t)i_U32);
  }

  Index_U32 = 0;
  Size_U32 = (sizeof(pData_U8) / NBPUSHSAMESIZE) - sizeof(uint32_t);

  for (i_U32 = 0; i_U32 < NBPUSHSAMESIZE; i_U32++)
  {
    Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Size_U32, &pData_U8[Index_U32]);
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
    Index_U32 += Size_U32;
  }
  Index_U32 = 0;

  Sts_B = mpBofRawCircularBuffer_O->IsEmpty();
  EXPECT_FALSE(Sts_B);
  Sts_B = mpBofRawCircularBuffer_O->IsFull();
  EXPECT_TRUE(Sts_B);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbElement();
  EXPECT_EQ(Nb_U32, sizeof(pData_U8));
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbFreeElement();
  EXPECT_EQ(Nb_U32, 0);
  Nb_U32 = mpBofRawCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(Nb_U32, sizeof(pData_U8));

  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Size_U32, &pData_U8[Index_U32]);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);

  mpBofRawCircularBuffer_O->Reset();

  Sts_B = mpBofRawCircularBuffer_O->IsEmpty();
  EXPECT_TRUE(Sts_B);
  Sts_B = mpBofRawCircularBuffer_O->IsFull();
  EXPECT_FALSE(Sts_B);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbElement();
  EXPECT_EQ(0, Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetCapacity();
  EXPECT_EQ(mBofRawCircularBufferParam_X.BufferSizeInByte_U32, Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbFreeElement();
  EXPECT_EQ(mBofRawCircularBufferParam_X.BufferSizeInByte_U32 - sizeof(uint32_t), Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(0, Nb_U32);

  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Size_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Nb_U32 = Size_U32 * 2;
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Nb_U32, Size_U32);

  mpBofRawCircularBuffer_O->Reset();

  // push
  Index_U32 = 0;
  Size_U32 = (sizeof(pData_U8) / NBPUSHSAMESIZE) - sizeof(uint32_t);

  Nb_U32 = Size_U32;               // 1
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 += Nb_U32;

  Nb_U32 = Size_U32 * 2;           // 2-3
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 += Nb_U32;

  Nb_U32 = Size_U32 * 4;           // 4-7
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 += Nb_U32;

  Nb_U32 = Size_U32;               // 8
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 += Nb_U32;
  EXPECT_NE((int)Index_U32, 0);

  memset(pData_U8, 0, sizeof(pData_U8));
  Index_U32 = 0;
  // pop
  Nb_U32 = Size_U32 * 8;           // 1
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Nb_U32, Size_U32 * 1);
  Index_U32 += Nb_U32;

  Nb_U32 = Size_U32 * 8;           // 2-3
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Nb_U32, Size_U32 * 2);
  Index_U32 += Nb_U32;

  Nb_U32 = Size_U32 * 8;           // 4-7
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Nb_U32, Size_U32 * 4);
  Index_U32 += Nb_U32;

  Nb_U32 = Size_U32 * 8;           // 8
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Nb_U32, Size_U32 * 1);
  Index_U32 += Nb_U32;

  for (i_U32 = 0; i_U32 < Size_U32; i_U32++)
  {
    EXPECT_EQ(pData_U8[i_U32], (uint8_t)i_U32);
  }


  // Skip
  Index_U32 = 0;
  Size_U32 = (sizeof(pData_U8) / NBPUSHSAMESIZE) - sizeof(uint32_t);

  Nb_U32 = Size_U32;               // 1
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 += Nb_U32;

  Nb_U32 = Size_U32 * 2;           // 2-3
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 += Nb_U32;

  Nb_U32 = Size_U32 * 4;           // 4-7
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 += Nb_U32;

  Nb_U32 = Size_U32;               // 8
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 += Nb_U32;
  EXPECT_NE((int)Index_U32, 0);

  Sts_E = mpBofRawCircularBuffer_O->Skip();
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofRawCircularBuffer_O->Skip();
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofRawCircularBuffer_O->Skip();
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofRawCircularBuffer_O->Skip();
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofRawCircularBuffer_O->Skip();
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_B = mpBofRawCircularBuffer_O->IsEmpty();
  EXPECT_TRUE(Sts_B);


  // Peek
  Index_U32 = 0;
  Size_U32 = (sizeof(pData_U8) / NBPUSHSAMESIZE) - sizeof(uint32_t);

  Nb_U32 = Size_U32;               // 1
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 += Nb_U32;

  Nb_U32 = Size_U32 * 2;           // 2-3
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 += Nb_U32;

  Nb_U32 = Size_U32 * 4;           // 4-7
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 += Nb_U32;

  Nb_U32 = Size_U32;               // 8
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 += Nb_U32;

  Index_U32 = 0;
  Nb_U32 = Size_U32 * 8;
  Sts_E = mpBofRawCircularBuffer_O->Peek(0, &Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Nb_U32, Size_U32 * 1);

  Nb_U32 = Size_U32 * 8;
  Sts_E = mpBofRawCircularBuffer_O->Peek(1, &Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Nb_U32, Size_U32 * 2);

  Nb_U32 = Size_U32 * 8;
  Sts_E = mpBofRawCircularBuffer_O->Peek(2, &Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Nb_U32, Size_U32 * 4);

  Nb_U32 = Size_U32 * 8;
  Sts_E = mpBofRawCircularBuffer_O->Peek(3, &Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Nb_U32, Size_U32 * 1);

  Nb_U32 = Size_U32 * 8;
  Sts_E = mpBofRawCircularBuffer_O->Peek(4, &Nb_U32, &pData_U8[Index_U32]);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(0, Nb_U32);

  // Lock/Unlock
  mpBofRawCircularBuffer_O->Reset();

  Index_U32 = 0;
  Sts_E = mpBofRawCircularBuffer_O->LockBuffer(Size_U32, &pNb1_U32[0], &ppData1_U8[0], &pNb2_U32[0], &ppData2_U8[0]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Size_U32, pNb1_U32[0]);
  EXPECT_EQ(0, pNb2_U32[0]);
  EXPECT_TRUE(ppData1_U8[0] != nullptr);
  EXPECT_TRUE(ppData2_U8[0] == nullptr);

  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Size_U32, pData_U8);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Sts_E = mpBofRawCircularBuffer_O->LockBuffer(Size_U32, &pNb1_U32[1], &ppData1_U8[1], &pNb2_U32[1], &ppData2_U8[1]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Size_U32, pNb1_U32[1]);
  EXPECT_EQ(0, pNb2_U32[1]);
  EXPECT_TRUE(ppData1_U8[1] != nullptr);
  EXPECT_TRUE(ppData2_U8[1] == nullptr);

  Nb_U32 = mpBofRawCircularBuffer_O->GetNbElement();
  EXPECT_EQ((Size_U32 + sizeof(uint32_t)) * 3, Nb_U32);

  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(0, Nb_U32);

  Nb_U32 = Size_U32;
  Sts_E = mpBofRawCircularBuffer_O->UnlockBuffer(ppData1_U8[0], Nb_U32);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(0, Nb_U32);

  Sts_E = mpBofRawCircularBuffer_O->UnlockBuffer(ppData1_U8[0], Size_U32);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);

  Sts_E = mpBofRawCircularBuffer_O->UnlockBuffer(ppData1_U8[1], Size_U32);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Size_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Nb_U32 = mpBofRawCircularBuffer_O->GetNbElement();
  EXPECT_EQ(0, Nb_U32);


  mpBofRawCircularBuffer_O->Reset();

  Nb_U32 = Size_U32;
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, pData_U8);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Nb_U32 = mBofRawCircularBufferParam_X.BufferSizeInByte_U32 - Size_U32 - static_cast<uint32_t>(sizeof(uint32_t)) - static_cast<uint32_t>(sizeof(uint32_t)) - 16;
  Sts_E = mpBofRawCircularBuffer_O->LockBuffer(Nb_U32, &pNb1_U32[0], &ppData1_U8[0], &pNb2_U32[0], &ppData2_U8[0]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Nb_U32, pNb1_U32[0]);
  EXPECT_EQ(0, pNb2_U32[0]);
  EXPECT_TRUE(ppData1_U8[0] != nullptr);
  EXPECT_TRUE(ppData2_U8[0] == nullptr);

  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Size_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Nb_U32 = Size_U32 + 16;
  Sts_E = mpBofRawCircularBuffer_O->LockBuffer(Nb_U32, &pNb1_U32[1], &ppData1_U8[1], &pNb2_U32[1], &ppData2_U8[1]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(16 - sizeof(uint32_t), pNb1_U32[1]);
  EXPECT_EQ(Size_U32 + sizeof(uint32_t), pNb2_U32[1]);
  EXPECT_TRUE(ppData1_U8[1] != nullptr);
  EXPECT_TRUE(ppData2_U8[1] != nullptr);


  Nb_U32 = mpBofRawCircularBuffer_O->GetSlotSize();
  EXPECT_EQ(0, Nb_U32);


}


class BofRawCircularBufferWithSlotsize_Test :public testing::Test
{
public:
  BofRawCircularBufferWithSlotsize_Test() : mpBofRawCircularBuffer_O(nullptr)
  {
  }

  // Per-test-case set-up. Called before the first test in this test case.
  static void                   SetUpTestCase();

  // Per-test-case tear-down. Called after the last test in this test case.
  static void                   TearDownTestCase();

protected:

  // You can define per-test set-up and tear-down logic as usual.
  virtual void                  SetUp();
  virtual void                  TearDown();

  BofRawCircularBuffer *mpBofRawCircularBuffer_O;
  BOF_RAW_CIRCULAR_BUFFER_PARAM mBofRawCircularBufferParam_X;

private:
};


/*** Factory functions called at the beginning/end of each test case **********/

void BofRawCircularBufferWithSlotsize_Test::SetUpTestCase()
{}


void BofRawCircularBufferWithSlotsize_Test::TearDownTestCase()
{}


void BofRawCircularBufferWithSlotsize_Test::SetUp()
{
  mBofRawCircularBufferParam_X.Reset();
  mBofRawCircularBufferParam_X.MultiThreadAware_B = true;
  mBofRawCircularBufferParam_X.BufferSizeInByte_U32 = 0x1000;
  mBofRawCircularBufferParam_X.NbMaxSlot_U32 = NBPUSHSAMESIZE;
  mBofRawCircularBufferParam_X.pData_U8 = nullptr;
  mpBofRawCircularBuffer_O = new BofRawCircularBuffer(mBofRawCircularBufferParam_X);

  EXPECT_TRUE(mpBofRawCircularBuffer_O != nullptr);
  EXPECT_TRUE(mpBofRawCircularBuffer_O->LastErrorCode() == BOF_ERR_NO_ERROR);
}

void BofRawCircularBufferWithSlotsize_Test::TearDown()
{
  BOF_SAFE_DELETE(mpBofRawCircularBuffer_O);
  EXPECT_TRUE(mpBofRawCircularBuffer_O == nullptr);
}

TEST_F(BofRawCircularBufferWithSlotsize_Test, CheckByteBuffer)
{
  uint32_t Nb_U32, i_U32, Index_U32, Size_U32;
  BOFERR   Sts_E;
  bool     Sts_B;
  uint8_t  pData_U8[0x1000];
  uint32_t pNb1_U32[8], pNb2_U32[8];
  uint8_t *ppData1_U8[8], *ppData2_U8[8];

  Sts_B = mpBofRawCircularBuffer_O->IsEmpty();
  EXPECT_TRUE(Sts_B);
  Sts_B = mpBofRawCircularBuffer_O->IsFull();
  EXPECT_FALSE(Sts_B);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbElement();
  EXPECT_EQ(0, Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetCapacity();
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxSlot_U32, Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbFreeElement();
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxSlot_U32, Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(0, Nb_U32);

  for (i_U32 = 0; i_U32 < sizeof(pData_U8); i_U32++)
  {
    pData_U8[i_U32] = (uint8_t)i_U32;
  }

  Size_U32 = 10;
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(nullptr, pData_U8);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Size_U32, nullptr);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(nullptr, nullptr);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Size_U32, pData_U8);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(0, Size_U32);

  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(0, pData_U8);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(10, nullptr);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(0, nullptr);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);

  Index_U32 = 0;
  Size_U32 = 10;
  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Size_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Sts_B = mpBofRawCircularBuffer_O->IsEmpty();
  EXPECT_FALSE(Sts_B);
  Sts_B = mpBofRawCircularBuffer_O->IsFull();
  EXPECT_FALSE(Sts_B);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbElement();
  EXPECT_EQ(Nb_U32, 1);
  Nb_U32 = mpBofRawCircularBuffer_O->GetCapacity();
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxSlot_U32, Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbFreeElement();
  EXPECT_EQ(Nb_U32, mBofRawCircularBufferParam_X.NbMaxSlot_U32 - 1);
  Nb_U32 = mpBofRawCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(Nb_U32, 1);


  Index_U32 = 0;
  Size_U32 = 9;
  memset(&pData_U8[Index_U32], 0, Size_U32);
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Size_U32, &pData_U8[Index_U32]);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(10, Size_U32);

  Index_U32 = 0;
  Size_U32 = 10;
  memset(&pData_U8[Index_U32], 0, Size_U32);
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Size_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(10, Size_U32);

  Sts_B = mpBofRawCircularBuffer_O->IsEmpty();
  EXPECT_TRUE(Sts_B);
  Sts_B = mpBofRawCircularBuffer_O->IsFull();
  EXPECT_FALSE(Sts_B);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbElement();
  EXPECT_EQ(Nb_U32, 0);
  Nb_U32 = mpBofRawCircularBuffer_O->GetCapacity();
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxSlot_U32, Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbFreeElement();
  EXPECT_EQ(Nb_U32, mBofRawCircularBufferParam_X.NbMaxSlot_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(Nb_U32, 1);

  for (i_U32 = 0; i_U32 < Size_U32; i_U32++)
  {
    EXPECT_EQ(pData_U8[i_U32], (uint8_t)i_U32);
  }

  Index_U32 = 0;
  Size_U32 = (sizeof(pData_U8) / NBPUSHSAMESIZE);

  for (i_U32 = 0; i_U32 < NBPUSHSAMESIZE; i_U32++)
  {
    Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Size_U32, &pData_U8[Index_U32]);
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
    Index_U32 += Size_U32;
  }
  Index_U32 = 0;

  Sts_B = mpBofRawCircularBuffer_O->IsEmpty();
  EXPECT_FALSE(Sts_B);
  Sts_B = mpBofRawCircularBuffer_O->IsFull();
  EXPECT_TRUE(Sts_B);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbElement();
  EXPECT_EQ(Nb_U32, mBofRawCircularBufferParam_X.NbMaxSlot_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbFreeElement();
  EXPECT_EQ(Nb_U32, 0);
  Nb_U32 = mpBofRawCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(Nb_U32, mBofRawCircularBufferParam_X.NbMaxSlot_U32);

  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Size_U32, &pData_U8[Index_U32]);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);

  mpBofRawCircularBuffer_O->Reset();

  Sts_B = mpBofRawCircularBuffer_O->IsEmpty();
  EXPECT_TRUE(Sts_B);
  Sts_B = mpBofRawCircularBuffer_O->IsFull();
  EXPECT_FALSE(Sts_B);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbElement();
  EXPECT_EQ(0, Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetCapacity();
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxSlot_U32, Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetNbFreeElement();
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxSlot_U32, Nb_U32);
  Nb_U32 = mpBofRawCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(0, Nb_U32);

  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Size_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Nb_U32 = Size_U32 * 2;
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Nb_U32, Size_U32);

  mpBofRawCircularBuffer_O->Reset();

  // push
  Index_U32 = 0;
  Size_U32 = (sizeof(pData_U8) / NBPUSHSAMESIZE);

  for (i_U32 = 0; i_U32 < NBPUSHSAMESIZE; i_U32++)
  {
    Nb_U32 = Size_U32;
    Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
    Index_U32 += Nb_U32;
  }
  memset(pData_U8, 0, sizeof(pData_U8));
  Index_U32 = 0;
  // pop
  for (i_U32 = 0; i_U32 < NBPUSHSAMESIZE; i_U32++)
  {
    Nb_U32 = Size_U32;
    Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
    EXPECT_EQ(Nb_U32, Size_U32 * 1);
    Index_U32 += Nb_U32;
  }
  for (i_U32 = 0; i_U32 < Size_U32; i_U32++)
  {
    EXPECT_EQ(pData_U8[i_U32], (uint8_t)i_U32);
  }


  // Skip
  Index_U32 = 0;
  Size_U32 = (sizeof(pData_U8) / NBPUSHSAMESIZE);

  for (i_U32 = 0; i_U32 < NBPUSHSAMESIZE; i_U32++)
  {
    Nb_U32 = Size_U32;
    Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
    Index_U32 += Nb_U32;
  }

  for (i_U32 = 0; i_U32 < NBPUSHSAMESIZE; i_U32++)
  {
    Sts_E = mpBofRawCircularBuffer_O->Skip();
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  }
  Sts_E = mpBofRawCircularBuffer_O->Skip();
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_B = mpBofRawCircularBuffer_O->IsEmpty();
  EXPECT_TRUE(Sts_B);

  // Peek
  Index_U32 = 0;
  Size_U32 = (sizeof(pData_U8) / NBPUSHSAMESIZE);

  for (i_U32 = 0; i_U32 < NBPUSHSAMESIZE; i_U32++)
  {
    Nb_U32 = Size_U32;
    Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Nb_U32, &pData_U8[Index_U32]);
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
    Index_U32 += Nb_U32;
  }

  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < NBPUSHSAMESIZE; i_U32++)
  {
    Nb_U32 = Size_U32;
    Sts_E = mpBofRawCircularBuffer_O->Peek(i_U32, &Nb_U32, &pData_U8[Index_U32]);
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
    EXPECT_EQ(Nb_U32, Size_U32);
  }

  Nb_U32 = Size_U32;
  Sts_E = mpBofRawCircularBuffer_O->Peek(i_U32, &Nb_U32, &pData_U8[Index_U32]);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(0, Nb_U32);

  // Lock/Unlock
  mpBofRawCircularBuffer_O->Reset();

  Index_U32 = 0;
  Sts_E = mpBofRawCircularBuffer_O->LockBuffer(Size_U32, &pNb1_U32[0], &ppData1_U8[0], &pNb2_U32[0], &ppData2_U8[0]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Size_U32, pNb1_U32[0]);
  EXPECT_EQ(0, pNb2_U32[0]);
  EXPECT_TRUE(ppData1_U8[0] != nullptr);
  EXPECT_TRUE(ppData2_U8[0] == nullptr);

  Sts_E = mpBofRawCircularBuffer_O->PushBuffer(Size_U32, pData_U8);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Sts_E = mpBofRawCircularBuffer_O->LockBuffer(Size_U32, &pNb1_U32[1], &ppData1_U8[1], &pNb2_U32[1], &ppData2_U8[1]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Size_U32, pNb1_U32[1]);
  EXPECT_EQ(0, pNb2_U32[1]);
  EXPECT_TRUE(ppData1_U8[1] != nullptr);
  EXPECT_TRUE(ppData2_U8[1] == nullptr);

  Nb_U32 = mpBofRawCircularBuffer_O->GetNbElement();
  EXPECT_EQ(3, Nb_U32);

  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(0, Nb_U32);

  Nb_U32 = Size_U32;
  Sts_E = mpBofRawCircularBuffer_O->UnlockBuffer(ppData1_U8[0], Nb_U32);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(0, Nb_U32);

  Sts_E = mpBofRawCircularBuffer_O->UnlockBuffer(ppData1_U8[0], Size_U32);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);

  Sts_E = mpBofRawCircularBuffer_O->UnlockBuffer(ppData1_U8[1], Size_U32);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Nb_U32 = Size_U32;
  Sts_E = mpBofRawCircularBuffer_O->PopBuffer(&Nb_U32, &pData_U8[Index_U32]);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Nb_U32 = mpBofRawCircularBuffer_O->GetNbElement();
  EXPECT_EQ(0, Nb_U32);

  Nb_U32 = mpBofRawCircularBuffer_O->GetSlotSize();
  EXPECT_EQ(Size_U32, Nb_U32);


}