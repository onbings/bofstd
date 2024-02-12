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

constexpr uint32_t RAW_BUF_SIZE = 0x1000;
constexpr uint32_t RAW_MAX_BUFFER_ENTRY = 16;

bool FillBuffer(uint8_t *_pData_U8, uint32_t _Offset_U32, uint32_t _Len_U32, int8_t _Step_S8, uint32_t _Tag_U32)
{
  bool Rts_B = false;
  uint32_t i_U32;
  uint8_t Val_U8;
  uint8_t *pData_U8;

  if ((_pData_U8) && (_Offset_U32 <= RAW_BUF_SIZE) && (_Len_U32 <= RAW_BUF_SIZE) && (_Len_U32 >= 4) && ((_Len_U32 + _Offset_U32) <= RAW_BUF_SIZE))
  {
    Rts_B = true;
    _pData_U8 = &_pData_U8[_Offset_U32];
    pData_U8 = _pData_U8;
    *(uint32_t *)_pData_U8 = _Tag_U32;
    _pData_U8 += 4;
    _Len_U32 -= 4;
    Val_U8 = _Step_S8;

    for (i_U32 = 0; i_U32 < _Len_U32; i_U32++)
    {
      *_pData_U8++ = Val_U8;
      Val_U8 += _Step_S8;
    }
    // printf("FillBuffer  tag %08X %08X data %02X %02X %02X %02X Val %02X\n", *(uint32_t *)pData_U8, _Tag_U32, pData_U8[4], pData_U8[5], pData_U8[6], pData_U8[7], _Step_S8);
  }
  return Rts_B;
}
bool ClearBuffer(uint8_t *_pData_U8, uint32_t _Offset_U32, uint32_t _Len_U32, uint8_t _Val_U8)
{
  bool Rts_B = false;
  uint32_t i_U32;

  if ((_pData_U8) && (_Offset_U32 <= RAW_BUF_SIZE) && (_Len_U32 <= RAW_BUF_SIZE) && (_Len_U32 >= 4) && ((_Len_U32 + _Offset_U32) <= RAW_BUF_SIZE))
  {
    Rts_B = true;
    _pData_U8 = &_pData_U8[_Offset_U32];
    memset(_pData_U8, _Val_U8, _Len_U32);
  }
  return Rts_B;
}
bool CheckBuffer(uint8_t *_pData_U8, uint32_t _Offset_U32, uint32_t _Len_U32, int8_t _Step_S8, uint32_t _Tag_U32)
{
  bool Rts_B = false;
  uint32_t i_U32;
  uint8_t Val_U8;

  if ((_pData_U8) && (_Offset_U32 <= RAW_BUF_SIZE) && (_Len_U32 <= RAW_BUF_SIZE) && (_Len_U32 >= 4) && ((_Len_U32 + _Offset_U32) <= RAW_BUF_SIZE))
  {
    _pData_U8 = &_pData_U8[_Offset_U32];
    // printf("CheckBuffer tag %08X %08X data %02X %02X %02X %02X Val %02X\n", *(uint32_t *)_pData_U8, _Tag_U32, _pData_U8[4], _pData_U8[5], _pData_U8[6], _pData_U8[7], _Step_S8);

    if (*(uint32_t *)_pData_U8 == _Tag_U32)
    {
      _pData_U8 += 4;
      _Len_U32 -= 4;
      Val_U8 = _Step_S8;

      for (i_U32 = 0; i_U32 < _Len_U32; i_U32++)
      {
        if (*_pData_U8++ != Val_U8)
        {
          printf("Val %x Expected %x\n", *(_pData_U8 - 1), Val_U8);
          break;
        }
        Val_U8 += _Step_S8;
      }
      Rts_B = (i_U32 == _Len_U32);
    }
    else
    {
      printf("Tag %x Expected %x\n", *(uint32_t *)_pData_U8, _Tag_U32);
    }
  }
  return Rts_B;
}
class RawCircularBuffer_Test : public testing::Test
{
public:
  RawCircularBuffer_Test()
      : mpBofRawCircularBuffer_O(nullptr)
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

  BofRawCircularBuffer *mpBofRawCircularBuffer_O;
  BOF_RAW_CIRCULAR_BUFFER_PARAM mBofRawCircularBufferParam_X;
  uint8_t *mpData_U8 = nullptr;

private:
};

/*** Factory functions called at the beginning/end of each test case **********/

void RawCircularBuffer_Test::SetUpTestCase()
{
}

void RawCircularBuffer_Test::TearDownTestCase()
{
}

void RawCircularBuffer_Test::SetUp()
{
  mBofRawCircularBufferParam_X.Reset();
  mBofRawCircularBufferParam_X.MultiThreadAware_B = true;
  mBofRawCircularBufferParam_X.BufferSizeInByte_U32 = RAW_BUF_SIZE;
  mBofRawCircularBufferParam_X.SlotMode_B = false;
  mBofRawCircularBufferParam_X.AlwaysContiguous_B = false;
  mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32 = RAW_MAX_BUFFER_ENTRY;
  mBofRawCircularBufferParam_X.pData_U8 = nullptr;
  mpBofRawCircularBuffer_O = new BofRawCircularBuffer(mBofRawCircularBufferParam_X);

  EXPECT_TRUE(mpBofRawCircularBuffer_O != nullptr);
  EXPECT_TRUE(mpBofRawCircularBuffer_O->LastErrorCode() == BOF_ERR_NO_ERROR);
  mpData_U8 = new uint8_t[RAW_BUF_SIZE];
  ASSERT_TRUE(mpBofRawCircularBuffer_O != nullptr);
}

void RawCircularBuffer_Test::TearDown()
{
  BOF_SAFE_DELETE(mpBofRawCircularBuffer_O);
  EXPECT_TRUE(mpBofRawCircularBuffer_O == nullptr);
}

TEST_F(RawCircularBuffer_Test, BasicPushPop)
{
  uint32_t SizeOfFirst_U32, Size_U32, TotalSize_U32, RemainingSize_U32, WriteSize_U32, Tag_U32 = 0xDEADAA55;

  EXPECT_TRUE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(0, SizeOfFirst_U32);
  EXPECT_EQ(RAW_MAX_BUFFER_ENTRY, mpBofRawCircularBuffer_O->GetCapacity(&TotalSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE, TotalSize_U32);
  EXPECT_EQ(RAW_MAX_BUFFER_ENTRY, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE, RemainingSize_U32);
  EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetMaxLevel());
  EXPECT_EQ(mpBofRawCircularBuffer_O->GetSlotSize(), 0);
  WriteSize_U32 = (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY);
  ASSERT_TRUE(FillBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32));

  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, nullptr, mpData_U8));
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, nullptr));
  EXPECT_EQ(0, Size_U32);
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, nullptr, nullptr));
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, mpData_U8));
  EXPECT_EQ(0, Size_U32);
  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32));

  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, 0, mpData_U8, nullptr));
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, 10, nullptr, nullptr));
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, 0, nullptr, nullptr));
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, WriteSize_U32, mpData_U8, nullptr));
  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32));

  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(WriteSize_U32, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32 - 1, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE - WriteSize_U32, RemainingSize_U32);
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, nullptr, mpData_U8));
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, nullptr));
  EXPECT_EQ(0, Size_U32);
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, nullptr, nullptr));

  ASSERT_TRUE(ClearBuffer(mpData_U8, 0, WriteSize_U32, 0));
  Size_U32 = RAW_BUF_SIZE;
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, mpData_U8));
  EXPECT_EQ(WriteSize_U32, Size_U32);
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, mpData_U8));
  EXPECT_EQ(0, Size_U32);

  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32));

  EXPECT_TRUE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(0, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE, RemainingSize_U32);
  EXPECT_EQ(mpBofRawCircularBuffer_O->GetMaxLevel(), WriteSize_U32);

  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32));

  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, WriteSize_U32, mpData_U8, nullptr));
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(WriteSize_U32, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32, mpBofRawCircularBuffer_O->GetCapacity(&TotalSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE, TotalSize_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32 - 1, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE - WriteSize_U32, RemainingSize_U32);

  mpBofRawCircularBuffer_O->Reset();
  EXPECT_EQ(mpBofRawCircularBuffer_O->GetMaxLevel(), 0);
  EXPECT_TRUE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(0, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32, mpBofRawCircularBuffer_O->GetCapacity(&TotalSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE, TotalSize_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE, RemainingSize_U32);
}

TEST_F(RawCircularBuffer_Test, PartialPop)
{
  uint32_t i_U32, Index_U32, SizeOfFirst_U32, Size_U32, RemainingSize_U32, WriteSize_U32, Tag_U32 = 0xDEADAA55;

  WriteSize_U32 = (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY);

  ASSERT_TRUE(FillBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32++));
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, WriteSize_U32 / 4, mpData_U8, nullptr));

  ASSERT_TRUE(FillBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32++));
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, WriteSize_U32 / 2, mpData_U8, nullptr));

  ASSERT_TRUE(FillBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32++));
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, WriteSize_U32, mpData_U8, nullptr));

  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(3, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(WriteSize_U32 / 4, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32 - 3, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE - (WriteSize_U32 / 4) - (WriteSize_U32 / 2) - WriteSize_U32, RemainingSize_U32);

  ASSERT_TRUE(ClearBuffer(mpData_U8, 0, WriteSize_U32, 0));
  Size_U32 = WriteSize_U32 * 2;
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, mpData_U8));
  EXPECT_EQ(WriteSize_U32 / 4, Size_U32);
  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, WriteSize_U32 / 4, 1, Tag_U32 - 3));

  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(2, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(WriteSize_U32 / 2, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32 - 2, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE - (WriteSize_U32 / 2) - WriteSize_U32, RemainingSize_U32);

  ASSERT_TRUE(ClearBuffer(mpData_U8, 0, WriteSize_U32, 0));
  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < 8; i_U32++)
  {
    Size_U32 = WriteSize_U32 / 2 / 8;
    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, &mpData_U8[Index_U32]));
    EXPECT_EQ(WriteSize_U32 / 2 / 8, Size_U32);
    Index_U32 += Size_U32;
  }
  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, WriteSize_U32 / 2, 1, Tag_U32 - 2));

  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(WriteSize_U32, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32 - 1, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE - WriteSize_U32, RemainingSize_U32);

  ASSERT_TRUE(ClearBuffer(mpData_U8, 0, WriteSize_U32, 0));
  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < 8; i_U32++)
  {
    Size_U32 = WriteSize_U32 / 8;
    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, &mpData_U8[Index_U32]));
    EXPECT_EQ(WriteSize_U32 / 8, Size_U32);
    Index_U32 += Size_U32;
  }
  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32 - 1));

  EXPECT_EQ(mpBofRawCircularBuffer_O->GetMaxLevel(), WriteSize_U32);
  EXPECT_TRUE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(0, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE, RemainingSize_U32);
}

TEST_F(RawCircularBuffer_Test, PushAppend)
{
  uint32_t i_U32, Index_U32, SizeOfFirst_U32, Size_U32, WriteSize_U32, Tag_U32 = 0xDEADAA55;

  WriteSize_U32 = (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY);

  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->SetAppendMode(0, false, nullptr));
  ASSERT_TRUE(FillBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32++));
  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < 8; i_U32++)
  {
    Size_U32 = WriteSize_U32 / 8;
    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, Size_U32, &mpData_U8[Index_U32], nullptr));
    EXPECT_EQ(Size_U32, (WriteSize_U32 / 8));
    Index_U32 += Size_U32;
    EXPECT_EQ(mpBofRawCircularBuffer_O->GetMaxLevel(), Size_U32);
    EXPECT_EQ((i_U32 + 1), mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
    EXPECT_EQ(SizeOfFirst_U32, Size_U32);
  }
  mpBofRawCircularBuffer_O->Reset();
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->SetAppendMode(0, true, nullptr));
  ASSERT_TRUE(FillBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32++));
  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < 8; i_U32++)
  {
    Size_U32 = WriteSize_U32 / 8;
    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, Size_U32, &mpData_U8[Index_U32], nullptr));
    EXPECT_EQ(Size_U32, (WriteSize_U32 / 8));
    Index_U32 += Size_U32;
    EXPECT_EQ(mpBofRawCircularBuffer_O->GetMaxLevel(), 0);
    EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
    EXPECT_EQ(SizeOfFirst_U32, 0);
  }
  Size_U32 = WriteSize_U32 / 8;
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, Size_U32, &mpData_U8[Index_U32], nullptr));

  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->SetAppendMode(0, false, nullptr));
  EXPECT_EQ(mpBofRawCircularBuffer_O->GetMaxLevel(), WriteSize_U32 + (WriteSize_U32 / 8));
  EXPECT_EQ(1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(SizeOfFirst_U32, WriteSize_U32 + (WriteSize_U32 / 8));
  ASSERT_TRUE(ClearBuffer(mpData_U8, 0, WriteSize_U32, 0));
  Size_U32 = WriteSize_U32;
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, mpData_U8));
  EXPECT_EQ(WriteSize_U32, Size_U32);
  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32 - 1));
}

TEST_F(RawCircularBuffer_Test, Fill)
{
  uint32_t i_U32, Index_U32, SizeOfFirst_U32, Size_U32, WriteSize_U32, Tag_U32 = 0xDEADAA55;

  WriteSize_U32 = (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY);
  mpBofRawCircularBuffer_O->SetOverWriteMode(false);
  for (i_U32 = 0; i_U32 < (RAW_MAX_BUFFER_ENTRY * 2); i_U32++)
  {
    ASSERT_TRUE(FillBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32++));
    if (i_U32 >= RAW_MAX_BUFFER_ENTRY)
    {
      EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, WriteSize_U32 / 2, mpData_U8, nullptr));
      EXPECT_EQ(RAW_MAX_BUFFER_ENTRY, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
      EXPECT_EQ(SizeOfFirst_U32, WriteSize_U32 / 2);
    }
    else
    {
      EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, WriteSize_U32 / 2, mpData_U8, nullptr));
      EXPECT_EQ(i_U32 + 1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
      EXPECT_EQ(SizeOfFirst_U32, WriteSize_U32 / 2);
    }
  }
  mpBofRawCircularBuffer_O->Reset();
  mpBofRawCircularBuffer_O->SetOverWriteMode(true);
  for (i_U32 = 0; i_U32 < (RAW_MAX_BUFFER_ENTRY * 2); i_U32++)
  {
    ASSERT_TRUE(FillBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32++));
    if (i_U32 >= RAW_MAX_BUFFER_ENTRY)
    {
      EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, WriteSize_U32 / 2, mpData_U8, nullptr));
      EXPECT_EQ(RAW_MAX_BUFFER_ENTRY, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
      EXPECT_EQ(SizeOfFirst_U32, WriteSize_U32 / 2);
      EXPECT_TRUE(mpBofRawCircularBuffer_O->IsFull());
      EXPECT_TRUE(mpBofRawCircularBuffer_O->IsBufferOverflow());
    }
    else
    {
      EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, WriteSize_U32 / 2, mpData_U8, nullptr));
      EXPECT_EQ(i_U32 + 1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
      EXPECT_EQ(SizeOfFirst_U32, WriteSize_U32 / 2);
      if (i_U32 == (RAW_MAX_BUFFER_ENTRY - 1))
      {
        EXPECT_TRUE(mpBofRawCircularBuffer_O->IsFull());
      }
      else
      {
        EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
      }
      EXPECT_FALSE(mpBofRawCircularBuffer_O->IsBufferOverflow());
    }
  }
}

TEST_F(RawCircularBuffer_Test, FillWrapOverwrite)
{
  uint32_t i_U32, j_U32, Index_U32, SizeOfFirst_U32, Size_U32, Nb_U32, Nb1_U32, Nb2_U32, WriteSize_U32, WrapAt_U32, LastLen_U32, Tag_U32 = 0xDEADAA55;
  uint8_t *pBaseData_U8, *pData1_U8, *pData2_U8;
  bool IsLocked;

  WriteSize_U32 = ((RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY) / 2);
  mpBofRawCircularBuffer_O->SetOverWriteMode(true);
  for (i_U32 = WriteSize_U32; i_U32 < (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY); i_U32++)
  {
    mpBofRawCircularBuffer_O->Reset();
    Nb_U32 = (RAW_BUF_SIZE / WriteSize_U32) + 1;
    LastLen_U32 = (RAW_BUF_SIZE % WriteSize_U32);
    WrapAt_U32 = LastLen_U32 ? (RAW_BUF_SIZE / WriteSize_U32) : 0;
    for (j_U32 = 0; j_U32 < Nb_U32; j_U32++)
    {
      ASSERT_TRUE(FillBuffer(mpData_U8, 0, WriteSize_U32, (int8_t)(j_U32 + 1), Tag_U32++));
      EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, WriteSize_U32, mpData_U8, nullptr));
      EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->GetBufferPtr(0, &Nb1_U32, &pData1_U8, &Nb2_U32, &pData2_U8));
      EXPECT_EQ(1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
      // printf("i %d/%d j %02d/%02d szof %d ws %d b1 %03d:%p b2 %03d:%p\n", i_U32, (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY), j_U32, Nb_U32, SizeOfFirst_U32, WriteSize_U32, Nb1_U32, pData1_U8, Nb2_U32, pData2_U8);
      ASSERT_TRUE(ClearBuffer(mpData_U8, 0, WriteSize_U32, 0));
      Size_U32 = WriteSize_U32;
      EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, mpData_U8));
      EXPECT_EQ(WriteSize_U32, Size_U32);
      ASSERT_TRUE(CheckBuffer(mpData_U8, 0, WriteSize_U32, (int8_t)(j_U32 + 1), Tag_U32 - 1));
      EXPECT_EQ(WriteSize_U32, (Nb1_U32 + Nb2_U32));
      EXPECT_TRUE(pData1_U8 != nullptr);
      if (WrapAt_U32)
      {
        if (j_U32 == WrapAt_U32)
        {
          EXPECT_EQ(LastLen_U32, Nb1_U32);
          EXPECT_EQ(WriteSize_U32 - LastLen_U32, Nb2_U32);
          EXPECT_TRUE(pData2_U8 != nullptr);
        }
        else
        {
          EXPECT_EQ(WriteSize_U32, Nb1_U32);
          EXPECT_EQ(0, Nb2_U32);
          EXPECT_TRUE(pData2_U8 == nullptr);
        }
      }
      else
      {
        EXPECT_EQ(WriteSize_U32, Nb1_U32);
        EXPECT_EQ(0, Nb2_U32);
        EXPECT_TRUE(pData2_U8 == nullptr);
      }
    } // for (j_U32 = 0; j_U32 < (RAW_MAX_BUFFER_ENTRY * 2); j_U32++)
    WriteSize_U32++;
  } // for (i_U32 = 0; i_U32 < (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY); i_U32++)
}

TEST_F(RawCircularBuffer_Test, GetPtrSkip)
{
  uint32_t i_U32, Index_U32, SizeOfFirst_U32, Size_U32, Nb1_U32, Nb2_U32, WriteSize_U32, Tag_U32 = 0xDEADAA55;
  uint8_t *pBaseData_U8, *pData1_U8, *pData2_U8;
  bool IsLocked;

  WriteSize_U32 = (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY);
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->GetBufferPtr(0, &Nb1_U32, &pBaseData_U8, &Nb2_U32, &pData2_U8));
  EXPECT_EQ(Nb1_U32, 0);
  EXPECT_TRUE(pBaseData_U8 != nullptr);
  EXPECT_EQ(Nb2_U32, 0);
  EXPECT_TRUE(pData2_U8 == nullptr);

  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < RAW_MAX_BUFFER_ENTRY; i_U32++)
  {
    ASSERT_TRUE(FillBuffer(mpData_U8, 0, WriteSize_U32, 1, Tag_U32++));
    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, WriteSize_U32 / 2, mpData_U8, nullptr));
    EXPECT_EQ(1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
    EXPECT_EQ(SizeOfFirst_U32, WriteSize_U32 / 2);
    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->GetBufferPtr(0, &Nb1_U32, &pData1_U8, &Nb2_U32, &pData2_U8));

    EXPECT_EQ(Nb1_U32, WriteSize_U32 / 2);
    EXPECT_TRUE(pData1_U8 == &pBaseData_U8[Index_U32]);
    EXPECT_EQ(Nb2_U32, 0);
    EXPECT_TRUE(pData2_U8 == nullptr);
    ASSERT_TRUE(CheckBuffer(pData1_U8, 0, Nb1_U32, 1, Tag_U32 - 1));

    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->Skip(true, &IsLocked));
    EXPECT_FALSE(IsLocked);

    Index_U32 += Nb1_U32;
  }
  EXPECT_EQ(mpBofRawCircularBuffer_O->GetMaxLevel(), WriteSize_U32 / 2);
  EXPECT_TRUE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(0, SizeOfFirst_U32);
}

class RawCircularBufferAlwaysContiguous_Test : public testing::Test
{
public:
  RawCircularBufferAlwaysContiguous_Test()
      : mpBofRawCircularBuffer_O(nullptr)
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

  BofRawCircularBuffer *mpBofRawCircularBuffer_O;
  BOF_RAW_CIRCULAR_BUFFER_PARAM mBofRawCircularBufferParam_X;
  uint8_t *mpData_U8 = nullptr;

private:
};

/*** Factory functions called at the beginning/end of each test case **********/

void RawCircularBufferAlwaysContiguous_Test::SetUpTestCase()
{
}

void RawCircularBufferAlwaysContiguous_Test::TearDownTestCase()
{
}

void RawCircularBufferAlwaysContiguous_Test::SetUp()
{
  mBofRawCircularBufferParam_X.Reset();
  mBofRawCircularBufferParam_X.MultiThreadAware_B = true;
  mBofRawCircularBufferParam_X.BufferSizeInByte_U32 = RAW_BUF_SIZE;
  mBofRawCircularBufferParam_X.SlotMode_B = false;
  mBofRawCircularBufferParam_X.AlwaysContiguous_B = true;
  mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32 = RAW_MAX_BUFFER_ENTRY;
  mBofRawCircularBufferParam_X.pData_U8 = nullptr;
  mpBofRawCircularBuffer_O = new BofRawCircularBuffer(mBofRawCircularBufferParam_X);

  EXPECT_TRUE(mpBofRawCircularBuffer_O != nullptr);
  EXPECT_TRUE(mpBofRawCircularBuffer_O->LastErrorCode() == BOF_ERR_NO_ERROR);
  mpData_U8 = new uint8_t[RAW_BUF_SIZE];
  ASSERT_TRUE(mpBofRawCircularBuffer_O != nullptr);
}

void RawCircularBufferAlwaysContiguous_Test::TearDown()
{
  BOF_SAFE_DELETE(mpBofRawCircularBuffer_O);
  EXPECT_TRUE(mpBofRawCircularBuffer_O == nullptr);
}

TEST_F(RawCircularBufferAlwaysContiguous_Test, Basic)
{
  uint32_t i_U32, Tag_U32, WriteSize_U32, Nb1_U32, Nb2_U32, SizeOfFirst_U32, Size_U32, NbEasyOp_U32, Op_U32;
  uint8_t *pData1_U8, *pData2_U8, *pStartOfBuff_U8;

  mpBofRawCircularBuffer_O->SetOverWriteMode(false);
  WriteSize_U32 = (RAW_BUF_SIZE / 16) - 8;
/*
Number of buffers which can be stored in the buffer before getting a not enoudh space error:
|B01|B02|B03|...|B0n|empty space up to the end which is smaller than WriteSize_U32|
<---NbEasyOp_U32---->
<------------------------------RAW_BUF_SIZE--------------------------------------->
*/
  NbEasyOp_U32 = RAW_BUF_SIZE / WriteSize_U32;  
  Op_U32 = 0;
  for (i_U32 = 0; i_U32 < 123; i_U32++, Op_U32++)
  {
    Tag_U32 = 0xDEADAA55;
    ASSERT_TRUE(FillBuffer(mpData_U8, 0, WriteSize_U32, (int8_t)(i_U32 + 1), Tag_U32++));

    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, WriteSize_U32, mpData_U8, nullptr));
    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->GetBufferPtr(0, &Nb1_U32, &pData1_U8, &Nb2_U32, &pData2_U8));
    EXPECT_EQ(1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
    printf("PUSH %d/%d Sz %x B1 %x:%p B2 %x:%p\n", i_U32, NbEasyOp_U32, SizeOfFirst_U32, Nb1_U32, pData1_U8, Nb2_U32, pData2_U8);
    if (i_U32 == 0)
    {
      pStartOfBuff_U8 = pData1_U8;
    }

    if (Op_U32 == NbEasyOp_U32)
    {
      Op_U32 = 0;
      EXPECT_TRUE(pData1_U8 == pStartOfBuff_U8);
    }
    else
    {
      EXPECT_TRUE(pData1_U8 == (pStartOfBuff_U8 + (Op_U32 * WriteSize_U32)));
    }
    ASSERT_TRUE(ClearBuffer(mpData_U8, 0, WriteSize_U32, 0));
    Size_U32 = WriteSize_U32;
    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, mpData_U8));
    printf(" POP B %x:%p\n", Size_U32, mpData_U8);
    EXPECT_EQ(WriteSize_U32, Size_U32);
    ASSERT_TRUE(CheckBuffer(mpData_U8, 0, WriteSize_U32, (int8_t)(i_U32 + 1), Tag_U32 - 1));
    EXPECT_EQ(WriteSize_U32, (Nb1_U32 + Nb2_U32));
    EXPECT_TRUE(pData1_U8 != nullptr);

    EXPECT_EQ(WriteSize_U32, Nb1_U32);
    EXPECT_EQ(0, Nb2_U32);
    EXPECT_TRUE(pData2_U8 == nullptr);
  } // for (i_U32 = 0; i_U32 < (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY); i_U32++)
}
TEST_F(RawCircularBufferAlwaysContiguous_Test, FillWrapOverwrite)
{
  uint32_t i_U32, j_U32, Index_U32, SizeOfFirst_U32, Size_U32, Nb_U32, Nb1_U32, Nb2_U32, WriteSize_U32, WrapAt_U32, LastLen_U32, RemainingSize_U32, Tag_U32 = 0xDEADAA55;
  uint8_t *pBaseData_U8, *pData1_U8, *pData2_U8, Val_U8;
  bool IsLocked;

  mpBofRawCircularBuffer_O->SetOverWriteMode(true);
  WriteSize_U32 = ((RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY) / 2);
  for (i_U32 = WriteSize_U32; i_U32 < (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY); i_U32++)
  {
    Tag_U32 = 0xDEADAA55;
    mpBofRawCircularBuffer_O->Reset();
    Nb_U32 = (RAW_BUF_SIZE / WriteSize_U32) + 1;
    LastLen_U32 = (RAW_BUF_SIZE % WriteSize_U32);
    WrapAt_U32 = LastLen_U32 ? (RAW_BUF_SIZE / WriteSize_U32) : 0;
    for (j_U32 = 0; j_U32 < Nb_U32; j_U32++)
    {
      ASSERT_TRUE(FillBuffer(mpData_U8, 0, WriteSize_U32, (int8_t)(j_U32 + 1), Tag_U32++));
      EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, WriteSize_U32, mpData_U8, nullptr));
      EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->GetBufferPtr(0, &Nb1_U32, &pData1_U8, &Nb2_U32, &pData2_U8));
      EXPECT_EQ(1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
      // uint32_t n = mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32);
      // printf("i %d/%d j %02d/%02d szof %d ws %d b1 %03d:%p b2 %03d:%p n %d Rem %d WrapAt %d LstLen %d\n", i_U32, (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY), j_U32, Nb_U32, SizeOfFirst_U32, WriteSize_U32, Nb1_U32, pData1_U8, Nb2_U32, pData2_U8, n, RemainingSize_U32, WrapAt_U32, LastLen_U32);
      ASSERT_TRUE(ClearBuffer(mpData_U8, 0, WriteSize_U32, 0));
      Size_U32 = WriteSize_U32;
      EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, mpData_U8));
      EXPECT_EQ(WriteSize_U32, Size_U32);
      ASSERT_TRUE(CheckBuffer(mpData_U8, 0, WriteSize_U32, (int8_t)(j_U32 + 1), Tag_U32 - 1));
      EXPECT_EQ(WriteSize_U32, (Nb1_U32 + Nb2_U32));
      EXPECT_TRUE(pData1_U8 != nullptr);

      EXPECT_EQ(WriteSize_U32, Nb1_U32);
      EXPECT_EQ(0, Nb2_U32);
      EXPECT_TRUE(pData2_U8 == nullptr);
    } // for (j_U32 = 0; j_U32 < (RAW_MAX_BUFFER_ENTRY * 2); j_U32++)
    WriteSize_U32++;
  } // for (i_U32 = 0; i_U32 < (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY); i_U32++)

  WriteSize_U32 = ((RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY) / 2);
  for (i_U32 = WriteSize_U32; i_U32 < (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY); i_U32++)
  {
    Tag_U32 = 0xDEADAA55;
    mpBofRawCircularBuffer_O->Reset();
    Nb_U32 = (RAW_BUF_SIZE / WriteSize_U32) + 1;
    LastLen_U32 = (RAW_BUF_SIZE % WriteSize_U32);
    WrapAt_U32 = LastLen_U32 ? (RAW_BUF_SIZE / WriteSize_U32) : 0;
    Val_U8 = 0;
    for (j_U32 = 0; j_U32 < Nb_U32; j_U32++)
    {
      ASSERT_TRUE(FillBuffer(mpData_U8, 0, WriteSize_U32, Val_U8++, Tag_U32++));
      EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, WriteSize_U32, mpData_U8, nullptr));
      EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->GetBufferPtr(0, &Nb1_U32, &pData1_U8, &Nb2_U32, &pData2_U8));
      // printf("%03d,%02d/%02d: push tag %08X data %02X %02X %02X %02X Val %02X\n", i_U32, j_U32, Nb_U32, Tag_U32 - 1, mpData_U8[4], mpData_U8[5], mpData_U8[6], mpData_U8[7], Val_U8 - 1);

      if (j_U32 < RAW_MAX_BUFFER_ENTRY)
      {
        EXPECT_EQ(j_U32 + 1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
      }
      else
      {
        EXPECT_EQ(RAW_MAX_BUFFER_ENTRY, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
      }
      // uint32_t n = mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32);
      // printf("i %d/%d j %02d/%02d szof %d ws %d b1 %03d:%p b2 %03d:%p n %d Rem %d WrapAt %d LstLen %d\n", i_U32, (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY), j_U32, Nb_U32, SizeOfFirst_U32, WriteSize_U32, Nb1_U32, pData1_U8, Nb2_U32, pData2_U8, n, RemainingSize_U32, WrapAt_U32, LastLen_U32);
      EXPECT_EQ(WriteSize_U32, (Nb1_U32 + Nb2_U32));
      EXPECT_TRUE(pData1_U8 != nullptr);

      EXPECT_EQ(WriteSize_U32, Nb1_U32);
      EXPECT_EQ(0, Nb2_U32);
      EXPECT_TRUE(pData2_U8 == nullptr);
    } // for (j_U32 = 0; j_U32 < (RAW_MAX_BUFFER_ENTRY * 2); j_U32++)

    Tag_U32 -= RAW_MAX_BUFFER_ENTRY;
    // Val_U8 = RAW_MAX_BUFFER_ENTRY + 1 - (i_U32 - ((RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY) / 2));
    for (j_U32 = 0; j_U32 < Nb_U32; j_U32++)
    {
      if (j_U32 < RAW_MAX_BUFFER_ENTRY)
      {
        EXPECT_EQ(RAW_MAX_BUFFER_ENTRY - j_U32, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
        uint32_t n = mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32);
        // printf("i %d/%d j %02d/%02d szof %d ws %d b1 %03d:%p b2 %03d:%p n %d Rem %d WrapAt %d LstLen %d\n", i_U32, (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY), j_U32, Nb_U32, SizeOfFirst_U32, WriteSize_U32, Nb1_U32, pData1_U8, Nb2_U32, pData2_U8, n, RemainingSize_U32, WrapAt_U32, LastLen_U32);
        EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->GetBufferPtr(0, &Nb1_U32, &pData1_U8, &Nb2_U32, &pData2_U8));
        Size_U32 = WriteSize_U32;
        ASSERT_TRUE(ClearBuffer(mpData_U8, 0, WriteSize_U32, 0));
        EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, mpData_U8));
        // printf("%03d,%02d/%02d: pop  tag %08X data %02X %02X %02X %02X Val %02X\n", i_U32, j_U32, Nb_U32, Tag_U32, mpData_U8[4], mpData_U8[5], mpData_U8[6], mpData_U8[7], Val_U8);
        if (j_U32 == 0)
        {
          Val_U8 = mpData_U8[4];
        }
        ASSERT_TRUE(CheckBuffer(mpData_U8, 0, WriteSize_U32, Val_U8++, Tag_U32++));
        EXPECT_EQ(WriteSize_U32, Size_U32);
        EXPECT_EQ(WriteSize_U32, (Nb1_U32 + Nb2_U32));
        EXPECT_TRUE(pData1_U8 != nullptr);
        EXPECT_EQ(WriteSize_U32, Nb1_U32);
        EXPECT_EQ(0, Nb2_U32);
        EXPECT_TRUE(pData2_U8 == nullptr);
      }
      else
      {
        EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
      }
    } // for (j_U32 = 0; j_U32 < (RAW_MAX_BUFFER_ENTRY * 2); j_U32++)
    WriteSize_U32++;
  } // for (i_U32 = 0; i_U32 < (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY); i_U32++)
}

class RawCircularBufferInSlotMode_Test : public testing::Test
{
public:
  RawCircularBufferInSlotMode_Test()
      : mpBofRawCircularBuffer_O(nullptr)
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

  BofRawCircularBuffer *mpBofRawCircularBuffer_O;
  BOF_RAW_CIRCULAR_BUFFER_PARAM mBofRawCircularBufferParam_X;
  uint8_t *mpData_U8 = nullptr;

private:
};

void RawCircularBufferInSlotMode_Test::SetUp()
{
  mBofRawCircularBufferParam_X.Reset();
  mBofRawCircularBufferParam_X.MultiThreadAware_B = true;
  mBofRawCircularBufferParam_X.BufferSizeInByte_U32 = RAW_BUF_SIZE; // 0x1000;
  mBofRawCircularBufferParam_X.SlotMode_B = true;
  mBofRawCircularBufferParam_X.AlwaysContiguous_B = false;
  mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32 = RAW_MAX_BUFFER_ENTRY;
  mBofRawCircularBufferParam_X.pData_U8 = nullptr;
  mpBofRawCircularBuffer_O = new BofRawCircularBuffer(mBofRawCircularBufferParam_X);

  ASSERT_TRUE(mpBofRawCircularBuffer_O != nullptr);
  ASSERT_TRUE(mpBofRawCircularBuffer_O->LastErrorCode() == BOF_ERR_NO_ERROR);
  mpData_U8 = new uint8_t[RAW_BUF_SIZE];
  ASSERT_TRUE(mpBofRawCircularBuffer_O != nullptr);
}

void RawCircularBufferInSlotMode_Test::TearDown()
{
  BOF_SAFE_DELETE(mpBofRawCircularBuffer_O);
  ASSERT_TRUE(mpBofRawCircularBuffer_O == nullptr);
  BOF_SAFE_DELETE_ARRAY(mpData_U8);
  ASSERT_TRUE(mpData_U8 == nullptr);
}

/*** Factory functions called at the beginning/end of each test case **********/
void RawCircularBufferInSlotMode_Test::SetUpTestCase()
{
}

void RawCircularBufferInSlotMode_Test::TearDownTestCase()
{
}

TEST_F(RawCircularBufferInSlotMode_Test, BasicPushPop)
{
  uint32_t SizeOfFirst_U32, SlotSize_U32, Size_U32, TotalSize_U32, RemainingSize_U32, Tag_U32 = 0xDEADAA55;

  EXPECT_TRUE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(0, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32, mpBofRawCircularBuffer_O->GetCapacity(&TotalSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE, TotalSize_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE, RemainingSize_U32);
  EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetMaxLevel());
  SlotSize_U32 = mpBofRawCircularBuffer_O->GetSlotSize();
  EXPECT_EQ(SlotSize_U32, (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY));
  ASSERT_TRUE(FillBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32));

  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, nullptr, mpData_U8));
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, nullptr));
  EXPECT_EQ(0, Size_U32);
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, nullptr, nullptr));
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, mpData_U8));
  EXPECT_EQ(0, Size_U32);
  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32));

  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, 0, mpData_U8, nullptr));
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, 10, nullptr, nullptr));
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, 0, nullptr, nullptr));
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, SlotSize_U32, mpData_U8, nullptr));
  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32));

  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(SlotSize_U32, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32 - 1, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE - SlotSize_U32, RemainingSize_U32);

  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, nullptr, mpData_U8));
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, nullptr));
  EXPECT_EQ(0, Size_U32);
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, nullptr, nullptr));

  ASSERT_TRUE(ClearBuffer(mpData_U8, 0, SlotSize_U32, 0));
  Size_U32 = RAW_BUF_SIZE;
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, mpData_U8));
  EXPECT_EQ(SlotSize_U32, Size_U32);
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, mpData_U8));
  EXPECT_EQ(0, Size_U32);

  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32));

  EXPECT_TRUE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(0, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE, RemainingSize_U32);

  EXPECT_EQ(mpBofRawCircularBuffer_O->GetMaxLevel(), 1);

  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32));

  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, SlotSize_U32, mpData_U8, nullptr));
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(SlotSize_U32, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32, mpBofRawCircularBuffer_O->GetCapacity(&TotalSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE, TotalSize_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32 - 1, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE - SlotSize_U32, RemainingSize_U32);

  mpBofRawCircularBuffer_O->Reset();
  EXPECT_EQ(mpBofRawCircularBuffer_O->GetMaxLevel(), 0);
  EXPECT_TRUE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(0, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32, mpBofRawCircularBuffer_O->GetCapacity(&TotalSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE, TotalSize_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE, RemainingSize_U32);
}

TEST_F(RawCircularBufferInSlotMode_Test, PartialPop)
{
  uint32_t i_U32, Index_U32, SizeOfFirst_U32, SlotSize_U32, Size_U32, RemainingSize_U32, Tag_U32 = 0xDEADAA55;

  SlotSize_U32 = mpBofRawCircularBuffer_O->GetSlotSize();
  EXPECT_EQ(SlotSize_U32, (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY));

  ASSERT_TRUE(FillBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32++));
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, SlotSize_U32 / 4, mpData_U8, nullptr));

  ASSERT_TRUE(FillBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32++));
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, SlotSize_U32 / 2, mpData_U8, nullptr));

  ASSERT_TRUE(FillBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32++));
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, SlotSize_U32, mpData_U8, nullptr));

  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(3, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(SlotSize_U32 / 4, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32 - 3, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE - SlotSize_U32 - SlotSize_U32 - SlotSize_U32, RemainingSize_U32);

  ASSERT_TRUE(ClearBuffer(mpData_U8, 0, SlotSize_U32, 0));
  Size_U32 = SlotSize_U32 * 2;
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, mpData_U8));
  EXPECT_EQ(SlotSize_U32 / 4, Size_U32);
  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, SlotSize_U32 / 4, 1, Tag_U32 - 3));

  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(2, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(SlotSize_U32 / 2, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32 - 2, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE - SlotSize_U32 - SlotSize_U32, RemainingSize_U32);

  ASSERT_TRUE(ClearBuffer(mpData_U8, 0, SlotSize_U32, 0));
  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < 8; i_U32++)
  {
    Size_U32 = SlotSize_U32 / 2 / 8;
    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, &mpData_U8[Index_U32]));
    EXPECT_EQ(SlotSize_U32 / 2 / 8, Size_U32);
    Index_U32 += Size_U32;
  }
  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, SlotSize_U32 / 2, 1, Tag_U32 - 2));

  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(SlotSize_U32, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32 - 1, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE - SlotSize_U32, RemainingSize_U32);

  ASSERT_TRUE(ClearBuffer(mpData_U8, 0, SlotSize_U32, 0));
  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < 8; i_U32++)
  {
    Size_U32 = SlotSize_U32 / 8;
    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, &mpData_U8[Index_U32]));
    EXPECT_EQ(SlotSize_U32 / 8, Size_U32);
    Index_U32 += Size_U32;
  }
  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32 - 1));

  EXPECT_EQ(mpBofRawCircularBuffer_O->GetMaxLevel(), 3);
  EXPECT_TRUE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(0, SizeOfFirst_U32);
  EXPECT_EQ(mBofRawCircularBufferParam_X.NbMaxBufferEntry_U32, mpBofRawCircularBuffer_O->GetNbFreeElement(&RemainingSize_U32));
  EXPECT_EQ(RAW_BUF_SIZE, RemainingSize_U32);
}

TEST_F(RawCircularBufferInSlotMode_Test, PushAppend)
{
  uint32_t i_U32, Index_U32, SizeOfFirst_U32, SlotSize_U32, Size_U32, Tag_U32 = 0xDEADAA55;

  SlotSize_U32 = mpBofRawCircularBuffer_O->GetSlotSize();
  EXPECT_EQ(SlotSize_U32, (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY));

  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->SetAppendMode(0, false, nullptr));
  ASSERT_TRUE(FillBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32++));
  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < 8; i_U32++)
  {
    Size_U32 = SlotSize_U32 / 8;
    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, Size_U32, &mpData_U8[Index_U32], nullptr));
    EXPECT_EQ(Size_U32, (SlotSize_U32 / 8));
    Index_U32 += Size_U32;
    EXPECT_EQ(mpBofRawCircularBuffer_O->GetMaxLevel(), i_U32 + 1);
    EXPECT_EQ(i_U32 + 1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
    EXPECT_EQ(SizeOfFirst_U32, Size_U32);
  }
  mpBofRawCircularBuffer_O->Reset();
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->SetAppendMode(0, true, nullptr));
  ASSERT_TRUE(FillBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32++));
  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < 8; i_U32++)
  {
    Size_U32 = SlotSize_U32 / 8;
    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, Size_U32, &mpData_U8[Index_U32], nullptr));
    EXPECT_EQ(Size_U32, (SlotSize_U32 / 8));
    Index_U32 += Size_U32;
    EXPECT_EQ(mpBofRawCircularBuffer_O->GetMaxLevel(), 0);
    EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
    EXPECT_EQ(SizeOfFirst_U32, 0);
  }
  Size_U32 = SlotSize_U32 / 8;
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, Size_U32, &mpData_U8[Index_U32], nullptr));

  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->SetAppendMode(0, false, nullptr));
  EXPECT_EQ(mpBofRawCircularBuffer_O->GetMaxLevel(), 1);
  EXPECT_EQ(1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(SizeOfFirst_U32, SlotSize_U32);
  ASSERT_TRUE(ClearBuffer(mpData_U8, 0, SlotSize_U32, 0));
  Size_U32 = SlotSize_U32;
  EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PopBuffer(0, &Size_U32, mpData_U8));
  EXPECT_EQ(SlotSize_U32, Size_U32);
  ASSERT_TRUE(CheckBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32 - 1));
}

TEST_F(RawCircularBufferInSlotMode_Test, Fill)
{
  uint32_t i_U32, Index_U32, SizeOfFirst_U32, SlotSize_U32, Size_U32, Tag_U32 = 0xDEADAA55;

  SlotSize_U32 = mpBofRawCircularBuffer_O->GetSlotSize();
  EXPECT_EQ(SlotSize_U32, (RAW_BUF_SIZE / RAW_MAX_BUFFER_ENTRY));
  mpBofRawCircularBuffer_O->SetOverWriteMode(false);
  for (i_U32 = 0; i_U32 < (RAW_MAX_BUFFER_ENTRY * 2); i_U32++)
  {
    ASSERT_TRUE(FillBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32++));
    if (i_U32 >= RAW_MAX_BUFFER_ENTRY)
    {
      EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, SlotSize_U32 / 2, mpData_U8, nullptr));
      EXPECT_EQ(RAW_MAX_BUFFER_ENTRY, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
      EXPECT_EQ(SizeOfFirst_U32, SlotSize_U32 / 2);
    }
    else
    {
      EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, SlotSize_U32 / 2, mpData_U8, nullptr));
      EXPECT_EQ(i_U32 + 1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
      EXPECT_EQ(SizeOfFirst_U32, SlotSize_U32 / 2);
    }
  }
  mpBofRawCircularBuffer_O->Reset();
  mpBofRawCircularBuffer_O->SetOverWriteMode(true);
  for (i_U32 = 0; i_U32 < (RAW_MAX_BUFFER_ENTRY * 2); i_U32++)
  {
    ASSERT_TRUE(FillBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32++));
    if (i_U32 >= RAW_MAX_BUFFER_ENTRY)
    {
      EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, SlotSize_U32 / 2, mpData_U8, nullptr));
      EXPECT_EQ(RAW_MAX_BUFFER_ENTRY, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
      EXPECT_EQ(SizeOfFirst_U32, SlotSize_U32 / 2);
      EXPECT_TRUE(mpBofRawCircularBuffer_O->IsFull());
      EXPECT_TRUE(mpBofRawCircularBuffer_O->IsBufferOverflow());
    }
    else
    {
      EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, SlotSize_U32 / 2, mpData_U8, nullptr));
      EXPECT_EQ(i_U32 + 1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
      EXPECT_EQ(SizeOfFirst_U32, SlotSize_U32 / 2);
      if (i_U32 == (RAW_MAX_BUFFER_ENTRY - 1))
      {
        EXPECT_TRUE(mpBofRawCircularBuffer_O->IsFull());
      }
      else
      {
        EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
      }
      EXPECT_FALSE(mpBofRawCircularBuffer_O->IsBufferOverflow());
    }
  }
}

TEST_F(RawCircularBufferInSlotMode_Test, GetPtrSkip)
{
  uint32_t i_U32, Index_U32, SizeOfFirst_U32, SlotSize_U32, Size_U32, Nb1_U32, Nb2_U32, Tag_U32 = 0xDEADAA55;
  uint8_t *pBaseData_U8, *pData1_U8, *pData2_U8;
  bool IsLocked;

  SlotSize_U32 = mpBofRawCircularBuffer_O->GetSlotSize();
  EXPECT_NE(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->GetBufferPtr(0, &Nb1_U32, &pBaseData_U8, &Nb2_U32, &pData2_U8));
  EXPECT_EQ(Nb1_U32, 0);
  EXPECT_TRUE(pBaseData_U8 != nullptr);
  EXPECT_EQ(Nb2_U32, 0);
  EXPECT_TRUE(pData2_U8 == nullptr);

  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < RAW_MAX_BUFFER_ENTRY; i_U32++)
  {
    ASSERT_TRUE(FillBuffer(mpData_U8, 0, SlotSize_U32, 1, Tag_U32++));
    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->PushBuffer(0, SlotSize_U32 / 2, mpData_U8, nullptr));
    EXPECT_EQ(1, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
    EXPECT_EQ(SizeOfFirst_U32, SlotSize_U32 / 2);
    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->GetBufferPtr(0, &Nb1_U32, &pData1_U8, &Nb2_U32, &pData2_U8));

    EXPECT_EQ(Nb1_U32, SlotSize_U32 / 2);
    EXPECT_TRUE(pData1_U8 == &pBaseData_U8[Index_U32]);
    EXPECT_EQ(Nb2_U32, 0);
    EXPECT_TRUE(pData2_U8 == nullptr);
    ASSERT_TRUE(CheckBuffer(pData1_U8, 0, Nb1_U32, 1, Tag_U32 - 1));

    EXPECT_EQ(BOF_ERR_NO_ERROR, mpBofRawCircularBuffer_O->Skip(true, &IsLocked));
    EXPECT_FALSE(IsLocked);

    Index_U32 += SlotSize_U32;
  }
  EXPECT_EQ(mpBofRawCircularBuffer_O->GetMaxLevel(), 1);
  EXPECT_TRUE(mpBofRawCircularBuffer_O->IsEmpty());
  EXPECT_FALSE(mpBofRawCircularBuffer_O->IsFull());
  EXPECT_EQ(0, mpBofRawCircularBuffer_O->GetNbElement(&SizeOfFirst_U32));
  EXPECT_EQ(0, SizeOfFirst_U32);
}