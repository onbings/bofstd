/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the string circular buffer class
 *
 * Name:        ut_stringcircularbuffer.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */
#include <bofstd/bofstringcircularbuffer.h>

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

class StringCircularBuffer_Test : public testing::Test
{
public:
  StringCircularBuffer_Test()
      : mpBofStringCircularBuffer_O(nullptr)
  {
  }

  // Per-test-case set-up. Called before the first test in this test case.
  static void SetUpTestCase();

  // Per-test-case tear-down. Called after the last test in this test case.
  static void TearDownTestCase();

protected:
  // You can define per-test set-up and tear-down logic as usual.
  virtual void SetUp();
  virtual void TearDown();

  BofStringCircularBuffer *mpBofStringCircularBuffer_O;
  BOF_STRING_CIRCULAR_BUFFER_PARAM mBofStringCircularBufferParam_X;

private:
};

/*** Factory functions called at the beginning/end of each test case **********/

void StringCircularBuffer_Test::SetUpTestCase()
{
}

void StringCircularBuffer_Test::TearDownTestCase()
{
}

void StringCircularBuffer_Test::SetUp()
{
  mBofStringCircularBufferParam_X.Reset();
  mBofStringCircularBufferParam_X.MultiThreadAware_B = true;
  mBofStringCircularBufferParam_X.BufferSizeInByte_U32 = 0x1000;
  mBofStringCircularBufferParam_X.Overwrite_B = true;
  mBofStringCircularBufferParam_X.pData_c = nullptr;
  mBofStringCircularBufferParam_X.Blocking_B = false;
  mpBofStringCircularBuffer_O = new BofStringCircularBuffer(mBofStringCircularBufferParam_X);

  EXPECT_TRUE(mpBofStringCircularBuffer_O != nullptr);
  EXPECT_TRUE(mpBofStringCircularBuffer_O->LastErrorCode() == BOF_ERR_NO_ERROR);
}

void StringCircularBuffer_Test::TearDown()
{
  BOF_SAFE_DELETE(mpBofStringCircularBuffer_O);
  EXPECT_TRUE(mpBofStringCircularBuffer_O == nullptr);
}

TEST_F(StringCircularBuffer_Test, ByteBuffer)
{
  BOFERR Sts_E;
  uint32_t Nb_U32, i_U32, Size_U32, Index_U32;
  bool Sts_B;
  char pData_c[0x100], pBuffer_c[0x1000];

  Sts_B = mpBofStringCircularBuffer_O->IsEmpty();
  EXPECT_TRUE(Sts_B);
  Sts_B = mpBofStringCircularBuffer_O->IsFull();
  EXPECT_FALSE(Sts_B);

  Nb_U32 = mpBofStringCircularBuffer_O->GetCapacity();
  EXPECT_EQ(mBofStringCircularBufferParam_X.BufferSizeInByte_U32, Nb_U32);
  Nb_U32 = mpBofStringCircularBuffer_O->GetNbFreeChar();
  EXPECT_EQ(mBofStringCircularBufferParam_X.BufferSizeInByte_U32, Nb_U32);
  Nb_U32 = mpBofStringCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(0, Nb_U32);
  Nb_U32 = mpBofStringCircularBuffer_O->GetNbElement();
  EXPECT_EQ(0, Nb_U32);

  strcpy(pData_c, "123456789ABCDEF"); // 15 car +1 ending 0

  Sts_E = mpBofStringCircularBuffer_O->PopString(nullptr, pData_c, 0);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofStringCircularBuffer_O->PopString(&Size_U32, nullptr, 0);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofStringCircularBuffer_O->PopString(nullptr, nullptr, 0);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Size_U32 = 16;
  Sts_E = mpBofStringCircularBuffer_O->PopString(&Size_U32, pData_c, 0);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(0, Size_U32);

  Sts_E = mpBofStringCircularBuffer_O->PushString(nullptr, 0);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);

  Size_U32 = static_cast<uint32_t>(strlen(pData_c) + 1);
  Sts_E = mpBofStringCircularBuffer_O->PushString(pData_c, 0);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  Nb_U32 = mpBofStringCircularBuffer_O->GetNbElement();
  EXPECT_EQ(1, Nb_U32);

  Sts_B = mpBofStringCircularBuffer_O->IsEmpty();
  EXPECT_FALSE(Sts_B);
  Sts_B = mpBofStringCircularBuffer_O->IsFull();
  EXPECT_FALSE(Sts_B);
  Nb_U32 = mpBofStringCircularBuffer_O->GetCapacity();
  EXPECT_EQ(mBofStringCircularBufferParam_X.BufferSizeInByte_U32, Nb_U32);
  Nb_U32 = mpBofStringCircularBuffer_O->GetNbFreeChar();
  EXPECT_EQ(Nb_U32, mBofStringCircularBufferParam_X.BufferSizeInByte_U32 - Size_U32);
  Nb_U32 = mpBofStringCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(Nb_U32, Size_U32);

  memset(pBuffer_c, 0, sizeof(pBuffer_c));
  Size_U32 = static_cast<uint32_t>(strlen(pData_c) + 1);
  Sts_E = mpBofStringCircularBuffer_O->PopString(&Size_U32, pBuffer_c, 0);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(Size_U32, strlen(pData_c) + 1);
  Nb_U32 = mpBofStringCircularBuffer_O->GetNbElement();
  EXPECT_EQ(0, Nb_U32);

  Sts_B = mpBofStringCircularBuffer_O->IsEmpty();
  EXPECT_TRUE(Sts_B);
  Sts_B = mpBofStringCircularBuffer_O->IsFull();
  EXPECT_FALSE(Sts_B);
  Nb_U32 = mpBofStringCircularBuffer_O->GetCapacity();
  EXPECT_EQ(mBofStringCircularBufferParam_X.BufferSizeInByte_U32, Nb_U32);
  Nb_U32 = mpBofStringCircularBuffer_O->GetNbFreeChar();
  EXPECT_EQ(Nb_U32, mBofStringCircularBufferParam_X.BufferSizeInByte_U32);
  Nb_U32 = mpBofStringCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(Nb_U32, Size_U32);

  Size_U32 = static_cast<uint32_t>(strlen(pData_c) + 1);
  Sts_E = mpBofStringCircularBuffer_O->PushString(pData_c, 0);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Size_U32 = 6;
  Sts_E = mpBofStringCircularBuffer_O->PopString(&Size_U32, pBuffer_c, 0);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(6, Size_U32);
  EXPECT_EQ(memcmp(pBuffer_c, &pData_c[0], Size_U32 - 1), 0);
  Nb_U32 = mpBofStringCircularBuffer_O->GetNbElement();
  EXPECT_EQ(1, Nb_U32);

  Size_U32 = 6;
  Sts_E = mpBofStringCircularBuffer_O->PopString(&Size_U32, pBuffer_c, 0);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(6, Size_U32);
  EXPECT_EQ(memcmp(pBuffer_c, &pData_c[5], Size_U32 - 1), 0);
  Nb_U32 = mpBofStringCircularBuffer_O->GetNbElement();
  EXPECT_EQ(1, Nb_U32);

  Size_U32 = 6;
  Sts_E = mpBofStringCircularBuffer_O->PopString(&Size_U32, pBuffer_c, 0);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(6, Size_U32);
  EXPECT_EQ(memcmp(pBuffer_c, &pData_c[10], Size_U32 - 1), 0);
  Nb_U32 = mpBofStringCircularBuffer_O->GetNbElement();
  EXPECT_EQ(0, Nb_U32);

  Sts_B = mpBofStringCircularBuffer_O->IsEmpty();
  EXPECT_TRUE(Sts_B);
  Sts_B = mpBofStringCircularBuffer_O->IsFull();
  EXPECT_FALSE(Sts_B);
  Nb_U32 = mpBofStringCircularBuffer_O->GetCapacity();
  EXPECT_EQ(mBofStringCircularBufferParam_X.BufferSizeInByte_U32, Nb_U32);
  Nb_U32 = mpBofStringCircularBuffer_O->GetNbFreeChar();
  EXPECT_EQ(Nb_U32, mBofStringCircularBufferParam_X.BufferSizeInByte_U32);
  Nb_U32 = mpBofStringCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(strlen(pData_c) + 1, Nb_U32);

  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < mBofStringCircularBufferParam_X.BufferSizeInByte_U32 / 16; i_U32++)
  {
    Size_U32 = 16;
    Sts_E = mpBofStringCircularBuffer_O->PushString(pData_c, 0);
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
    EXPECT_EQ(16, Size_U32);
    Index_U32 += 16;
    Nb_U32 = mpBofStringCircularBuffer_O->GetNbElement();
    EXPECT_EQ(i_U32 + 1, Nb_U32);
  }

  Sts_B = mpBofStringCircularBuffer_O->IsEmpty();
  EXPECT_FALSE(Sts_B);
  Sts_B = mpBofStringCircularBuffer_O->IsFull();
  EXPECT_TRUE(Sts_B);
  Nb_U32 = mpBofStringCircularBuffer_O->GetCapacity();
  EXPECT_EQ(mBofStringCircularBufferParam_X.BufferSizeInByte_U32, Nb_U32);
  Nb_U32 = mpBofStringCircularBuffer_O->GetNbFreeChar();
  EXPECT_EQ(Nb_U32, 0);
  Nb_U32 = mpBofStringCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(mBofStringCircularBufferParam_X.BufferSizeInByte_U32, Nb_U32);

  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < mBofStringCircularBufferParam_X.BufferSizeInByte_U32 / 16; i_U32++)
  {
    if (i_U32 == 255)
    {
      i_U32 = i_U32;
    }
    Size_U32 = 16;
    memset(&pBuffer_c[Index_U32], 0, Size_U32);
    Sts_E = mpBofStringCircularBuffer_O->PopString(&Size_U32, &pBuffer_c[Index_U32], 0);
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
    EXPECT_EQ(16, Size_U32);
    EXPECT_EQ(memcmp(&pBuffer_c[Index_U32], pData_c, Size_U32), 0);
    Nb_U32 = mpBofStringCircularBuffer_O->GetNbElement();
    EXPECT_EQ((mBofStringCircularBufferParam_X.BufferSizeInByte_U32 / 16) - i_U32 - 1, Nb_U32);

    Index_U32 += 16;
  }
  Sts_B = mpBofStringCircularBuffer_O->IsEmpty();
  EXPECT_TRUE(Sts_B);
  Sts_B = mpBofStringCircularBuffer_O->IsFull();
  EXPECT_FALSE(Sts_B);
  Nb_U32 = mpBofStringCircularBuffer_O->GetCapacity();
  EXPECT_EQ(mBofStringCircularBufferParam_X.BufferSizeInByte_U32, Nb_U32);
  Nb_U32 = mpBofStringCircularBuffer_O->GetNbFreeChar();
  EXPECT_EQ(Nb_U32, mBofStringCircularBufferParam_X.BufferSizeInByte_U32);
  Nb_U32 = mpBofStringCircularBuffer_O->GetMaxLevel();
  EXPECT_EQ(Nb_U32, mBofStringCircularBufferParam_X.BufferSizeInByte_U32);

  for (i_U32 = 0; i_U32 < mBofStringCircularBufferParam_X.BufferSizeInByte_U32 / 16; i_U32++)
  {
    Sts_E = mpBofStringCircularBuffer_O->PushString(pData_c, 0);
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
    EXPECT_EQ(16, Size_U32);
    Nb_U32 = mpBofStringCircularBuffer_O->GetNbElement();
    EXPECT_EQ(i_U32 + 1, Nb_U32);
  }

  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < (mBofStringCircularBufferParam_X.BufferSizeInByte_U32 / 16) * 5; i_U32++)
  {
    Size_U32 = 4;
    memset(&pBuffer_c[Index_U32], 0, Size_U32);
    Sts_E = mpBofStringCircularBuffer_O->PopString(&Size_U32, &pBuffer_c[Index_U32], 0);
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
    EXPECT_EQ(4, Size_U32);
    EXPECT_EQ(memcmp(&pBuffer_c[Index_U32], &pData_c[Index_U32], Size_U32 - 1), 0);

    Index_U32 += 3;
    if (Index_U32 >= strlen(pData_c))
    {
      EXPECT_EQ(memcmp(&pBuffer_c[0], &pData_c[0], strlen(pData_c)), 0);
      Index_U32 = 0;
    }
  }
  Sts_E = mpBofStringCircularBuffer_O->PopString(&Size_U32, &pBuffer_c[Index_U32], 0);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  EXPECT_EQ(0, Size_U32);
}