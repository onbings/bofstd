/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the circular buffer class
 *
 * Name:        ut_circularbuffer.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */
#include "gtestrunner.h"
#include <bofstd/bofcircularbuffer.h>
#include <bofstd/bofdatetime.h>
#include <bofstd/bofperformance.h>

#include <list>
#include <thread>

USE_BOF_NAMESPACE()

template <class T>
class BofCircularBufferTemplate_Test : public testing::Test
{
public:
  // Per-test-case set-up. Called before the first test in this test case.
  static void SetUpTestCase();

  // Per-test-case tear-down. Called after the last test in this test case.
  static void TearDownTestCase();

protected:
  // You can define per-test set-up and tear-down logic as usual.
  virtual void SetUp()
  {
    mpBofCircularBuffer_O = nullptr;
  }

  virtual void TearDown()
  {
    BOF_SAFE_DELETE(mpBofCircularBuffer_O);
  }

  BofCircularBuffer<T> *mpBofCircularBuffer_O;

  // Some expensive resource shared by all tests.
  static BofCircularBuffer<T> *S_mpSharedBofCircularBuffer_O;
};

class CircularBuffer_Test : public testing::Test
{
public:
  // Per-test-case set-up. Called before the first test in this test case.
  static void SetUpTestCase();

  // Per-test-case tear-down. Called after the last test in this test case.
  static void TearDownTestCase();

protected:
  // You can define per-test set-up and tear-down logic as usual.
  virtual void SetUp();
  virtual void TearDown();

private:
};

template <class T>
BofCircularBuffer<T> *BofCircularBufferTemplate_Test<T>::S_mpSharedBofCircularBuffer_O;

using testing::Types;

// The list of types we want to test.
typedef Types<char, uint8_t, uint16_t> Implementation;
TYPED_TEST_CASE(BofCircularBufferTemplate_Test, Implementation);

template <class T>
void BofCircularBufferTemplate_Test<T>::SetUpTestCase()
{
  BOF_CIRCULAR_BUFFER_PARAM BofCircularBufferParam_X;

  // Create keyboard buffer and start windows message dispatch thread
  BofCircularBufferParam_X.Reset();
  BofCircularBufferParam_X.MultiThreadAware_B = true;
  BofCircularBufferParam_X.NbMaxElement_U32 = 8;
  BofCircularBufferParam_X.Overwrite_B = true;
  BofCircularBufferParam_X.pData = nullptr;
  S_mpSharedBofCircularBuffer_O = new BofCircularBuffer<T>(BofCircularBufferParam_X);
  EXPECT_TRUE(S_mpSharedBofCircularBuffer_O != nullptr);
  EXPECT_TRUE(S_mpSharedBofCircularBuffer_O->LastErrorCode() == BOF_ERR_NO_ERROR);
}

template <class T>
void BofCircularBufferTemplate_Test<T>::TearDownTestCase()
{
  BOF_SAFE_DELETE(S_mpSharedBofCircularBuffer_O);
  EXPECT_TRUE(S_mpSharedBofCircularBuffer_O == nullptr);
}

// Then use TYPED_TEST(TestCaseName, TestName) to define a typed test, similar to TEST_F.
TYPED_TEST(BofCircularBufferTemplate_Test, TheTest)
{
  // Inside a test, refer to the special name TypeParam to get the type
  // parameter.  Since we are inside a derived class template, C++ requires
  // us to visit the members of BofCircularBufferTemplate_Test via 'this'.
  // BofCircularBuffer<TypeParam> a(nullptr);TypeParam b = 3;
  // To visit static members of the fixture, add the 'TestFixture::' prefix.
  // int d = TestFixture::S_mpSharedBofCircularBuffer_O->GetCapacity();

  // n += TestFixture::S_mpSharedBofCircularBuffer_O;

  // Since we are in the template world, C++ requires explicitly
  // writing 'this->' when referring to members of the fixture class.
  // This is something you have to learn to live with.
  // EXPECT_FALSE(this->mpBofCircularBuffer_O->GetCapacity( ) );
}

/*** Factory functions called at the beginning/end of each test case **********/

void CircularBuffer_Test::SetUpTestCase()
{
}

void CircularBuffer_Test::TearDownTestCase()
{
}

/*** Factory functions called at the beginning/end of each test *****************/

void CircularBuffer_Test::SetUp()
{
}

void CircularBuffer_Test::TearDown()
{
}

// char GL_pDbgPush_c[512], GL_pDbgPop_c[512];
BOFERR PushValue(uint32_t _Id_U32, uint32_t _NbLoop_U32, BofCircularBuffer<uint64_t> *_pBofCollection)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  uint64_t Val_U64 = 0;
  uint32_t i_U32;

  for (i_U32 = 0; i_U32 < _NbLoop_U32; i_U32++)
  {
    Val_U64++;
    //    NbElem_U32 = _pBofCollection->GetNbElement();
    //  sprintf(GL_pDbgPush_c, "%03d:Push[%d] %d\n", _Id_U32, i_U32, BOF::Bof_GetMsTickCount());
    Rts_E = _pBofCollection->Push(&Val_U64, 2000, nullptr, nullptr);
    //  printf("%03d:Push[%d]=%zd n=%d Rts %X\n", _Id_U32, i_U32, Val_U64, NbElem_U32, Rts_E);
    if (Rts_E)
    {
      // printf("%03d:Push[%d]->%d\n", _Id_U32, i_U32, BOF::Bof_GetMsTickCount());
    }
    EXPECT_EQ(Rts_E, 0);
  }
  return Rts_E;
}

BOFERR PopValue(uint32_t /*_Id_U32*/, uint32_t _NbLoop_U32, BofCircularBuffer<uint64_t> *_pBofCollection)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  uint64_t ExpectedVal_U64 = 0, Val_U64;
  uint32_t i_U32;

  for (i_U32 = 0; i_U32 < _NbLoop_U32; i_U32++)
  {
    ExpectedVal_U64++;
    //    printf("   %03d:Pop[%d]\n", _Id_U32, i_U32);
    //    NbElem_U32 = _pBofCollection->GetNbElement();
    //    printf("   %03d:Pop[%d] n=%d\n", _Id_U32, i_U32, NbElem_U32);
    Rts_E = _pBofCollection->Pop(&Val_U64, 2000, nullptr, nullptr);
    //    sprintf(GL_pDbgPop_c,"   %03d:Pop[%d]=%zd n=%d Rts %X at %d\n", _Id_U32, i_U32, Val_U64, NbElem_U32, Rts_E, BOF::Bof_GetMsTickCount());
    EXPECT_EQ(Rts_E, 0);
    //    EXPECT_EQ(ExpectedVal_U64, Val_U64);
  }
  return Rts_E;
}
TEST(CircularBuffer_Test, PopExternalStorage)
{
  BOF_CIRCULAR_BUFFER_PARAM BofCircularBufferParam_X;
  BofCircularBuffer<uint32_t> *pBofCircularBuffer;
  uint32_t pData_U32[8], i_U32, IndexOf_U32;
  uint32_t *pElem;

  BofCircularBufferParam_X.MultiThreadAware_B = true;
  BofCircularBufferParam_X.NbMaxElement_U32 = BOF_NB_ELEM_IN_ARRAY(pData_U32);
  BofCircularBufferParam_X.Overwrite_B = false;
  BofCircularBufferParam_X.Blocking_B = false;
  BofCircularBufferParam_X.PopLockMode_B = false;
  BofCircularBufferParam_X.pData = pData_U32;
  pBofCircularBuffer = new BofCircularBuffer<uint32_t>(BofCircularBufferParam_X);
  EXPECT_TRUE(pBofCircularBuffer != nullptr);
  EXPECT_EQ(pBofCircularBuffer->LastErrorCode(), BOF_ERR_NO_ERROR);
  //  pBofCircularBuffer->LockCircularBuffer();
  //  pBofCircularBuffer->UnlockCircularBuffer();

  for (i_U32 = 0; i_U32 < BofCircularBufferParam_X.NbMaxElement_U32; i_U32++)
  {
    pData_U32[i_U32] = i_U32 + 1;
    EXPECT_EQ(pBofCircularBuffer->Push(&pData_U32[i_U32], 0, &IndexOf_U32, nullptr), BOF_ERR_NO_ERROR);
    EXPECT_EQ(IndexOf_U32, i_U32);
  }
  for (i_U32 = 0; i_U32 < BofCircularBufferParam_X.NbMaxElement_U32; i_U32++)
  {
    EXPECT_EQ(pBofCircularBuffer->Pop(&pData_U32[i_U32], 0, &IndexOf_U32, &pElem), BOF_ERR_NO_ERROR);
    EXPECT_EQ(IndexOf_U32, i_U32);
    EXPECT_TRUE(pElem == &pData_U32[i_U32]);
  }
}

const uint32_t NB_MAX_CLIENT = 4;        // 16; // 32;
const uint32_t NB_PUSH_PER_CLIENT = 256; // 16000;

TEST(CircularBuffer_Test, IsEntryFree)
{
  BOF_CIRCULAR_BUFFER_PARAM BofCircularBufferParam_X;
  BOFERR Sts_E;
  std::string Now_S;
  uint32_t i_U32, j_U32, k_U32, IndexOf_U32, Val_U32, Index_U32, Dbg_U32;
  bool Sts_B;
  BofCircularBuffer<uint32_t> *pBofCollection;
  bool pPosBusy_B[8], IsLocked_B;

  Sts_E = BOF_ERR_ENOMEM;
  BofCircularBufferParam_X.MultiThreadAware_B = true;
  BofCircularBufferParam_X.NbMaxElement_U32 = 8;
  BofCircularBufferParam_X.Overwrite_B = false;
  BofCircularBufferParam_X.Blocking_B = false;
  BofCircularBufferParam_X.PopLockMode_B = false;
  pBofCollection = new BofCircularBuffer<uint32_t>(BofCircularBufferParam_X);
  EXPECT_TRUE(pBofCollection != nullptr);
  Sts_E = pBofCollection->LastErrorCode();
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  for (j_U32 = 0; j_U32 < BofCircularBufferParam_X.NbMaxElement_U32; j_U32++)
  {
    Sts_B = pBofCollection->IsEntryFree(j_U32, &IsLocked_B, nullptr);
    EXPECT_TRUE(Sts_B);
  }

  memset(pPosBusy_B, 0, sizeof(pPosBusy_B));
  for (i_U32 = 0; i_U32 < BofCircularBufferParam_X.NbMaxElement_U32; i_U32++)
  {
    Sts_E = pBofCollection->Push(&i_U32, 0, &IndexOf_U32, nullptr);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(IndexOf_U32, i_U32);
    pPosBusy_B[IndexOf_U32] = true;
    for (j_U32 = 0; j_U32 < BofCircularBufferParam_X.NbMaxElement_U32; j_U32++)
    {
      Sts_B = pBofCollection->IsEntryFree(j_U32, &IsLocked_B, nullptr);
      // printf("1: i %d j %d Busy %d %d %d %d %d %d %d %d Nb %d\n", i_U32, j_U32, pPosBusy_B[0], pPosBusy_B[1], pPosBusy_B[2], pPosBusy_B[3], pPosBusy_B[4], pPosBusy_B[5], pPosBusy_B[6], pPosBusy_B[7], pBofCollection->GetNbElement());

      if (pPosBusy_B[j_U32])
      {
        EXPECT_FALSE(Sts_B);
      }
      else
      {
        EXPECT_TRUE(Sts_B);
      }
    }
  }
  for (i_U32 = 0; i_U32 < BofCircularBufferParam_X.NbMaxElement_U32; i_U32++)
  {
    Sts_E = pBofCollection->Pop(&Val_U32, 0, &IndexOf_U32, nullptr);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(IndexOf_U32, i_U32);
    pPosBusy_B[IndexOf_U32] = false;
    for (j_U32 = 0; j_U32 < BofCircularBufferParam_X.NbMaxElement_U32; j_U32++)
    {
      Sts_B = pBofCollection->IsEntryFree(j_U32, &IsLocked_B, nullptr);
      // printf("2: i %d j %d Busy %d %d %d %d %d %d %d %d Nb %d\n", i_U32, j_U32, pPosBusy_B[0], pPosBusy_B[1], pPosBusy_B[2], pPosBusy_B[3], pPosBusy_B[4], pPosBusy_B[5], pPosBusy_B[6], pPosBusy_B[7], pBofCollection->GetNbElement());

      if (pPosBusy_B[j_U32])
      {
        EXPECT_FALSE(Sts_B);
      }
      else
      {
        EXPECT_TRUE(Sts_B);
      }
    }
  }

  Dbg_U32 = 0;
  for (k_U32 = 1; k_U32 < BofCircularBufferParam_X.NbMaxElement_U32 - 1; k_U32++)
  {
    Dbg_U32++;
    memset(pPosBusy_B, 0, sizeof(pPosBusy_B));
    pBofCollection->Reset();
    for (i_U32 = 0; i_U32 < k_U32; i_U32++)
    {
      Dbg_U32++;
      Index_U32 = i_U32 % BofCircularBufferParam_X.NbMaxElement_U32;
      Sts_E = pBofCollection->Push(&i_U32, 0, &IndexOf_U32, nullptr);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_EQ(IndexOf_U32, Index_U32);
      pPosBusy_B[IndexOf_U32] = true;
    }
    for (i_U32 = 0; i_U32 < BofCircularBufferParam_X.NbMaxElement_U32 * 3; i_U32++)
    {
      Dbg_U32++;
      Index_U32 = (i_U32 + k_U32) % BofCircularBufferParam_X.NbMaxElement_U32;
      Sts_E = pBofCollection->Push(&i_U32, 0, &IndexOf_U32, nullptr);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_EQ(IndexOf_U32, Index_U32);
      pPosBusy_B[IndexOf_U32] = true;

      for (j_U32 = 0; j_U32 < BofCircularBufferParam_X.NbMaxElement_U32; j_U32++)
      {
        Dbg_U32++;
        Sts_B = pBofCollection->IsEntryFree(j_U32, &IsLocked_B, nullptr);
        // printf("4: k %d i %d j %d Busy %d %d %d %d %d %d %d %d Nb %d\n", k_U32, i_U32, j_U32, pPosBusy_B[0], pPosBusy_B[1], pPosBusy_B[2], pPosBusy_B[3], pPosBusy_B[4], pPosBusy_B[5], pPosBusy_B[6], pPosBusy_B[7], pBofCollection->GetNbElement());

        if (pPosBusy_B[j_U32])
        {
          EXPECT_FALSE(Sts_B);
        }
        else
        {
          EXPECT_TRUE(Sts_B);
        }
      }
      Sts_E = pBofCollection->Pop(&Val_U32, 0, &IndexOf_U32, nullptr);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      pPosBusy_B[IndexOf_U32] = false;
    }
  }
}

TEST(CircularBuffer_Test, LockUnlock)
{
  BOF_CIRCULAR_BUFFER_PARAM BofCircularBufferParam_X;
  BOFERR Sts_E;
  std::string Now_S;
  uint32_t i_U32, j_U32, k_U32, IndexOf_U32, Val_U32, PopVal_U32, Index_U32, IndexOfPop_U32, IndexOfLock_U32, Nb_U32;
  bool Sts_B;
  BofCircularBuffer<uint32_t> *pBofCollection;
  bool pPosLocked_B[8];

  Sts_E = BOF_ERR_ENOMEM;
  BofCircularBufferParam_X.MultiThreadAware_B = true;
  BofCircularBufferParam_X.NbMaxElement_U32 = 8;
  BofCircularBufferParam_X.Overwrite_B = false;
  BofCircularBufferParam_X.Blocking_B = true;
  BofCircularBufferParam_X.PopLockMode_B = true;
  pBofCollection = new BofCircularBuffer<uint32_t>(BofCircularBufferParam_X);
  EXPECT_TRUE(pBofCollection != nullptr);
  Sts_E = pBofCollection->LastErrorCode();
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  for (j_U32 = 0; j_U32 < BofCircularBufferParam_X.NbMaxElement_U32; j_U32++)
  {
    Sts_B = pBofCollection->IsLocked(j_U32);
    EXPECT_FALSE(Sts_B);
  }
  Val_U32 = 10;
  for (j_U32 = 0; j_U32 < BofCircularBufferParam_X.NbMaxElement_U32; j_U32++)
  {
    Sts_E = pBofCollection->Push(&Val_U32, 0, &IndexOf_U32, nullptr);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(IndexOf_U32, j_U32);
    Nb_U32 = pBofCollection->GetNbElement();
    EXPECT_EQ(Nb_U32, j_U32 + 1);
    Val_U32++;
  }
  Sts_E = pBofCollection->Push(&i_U32, 0, &IndexOf_U32, nullptr);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Val_U32 = 10;
  for (j_U32 = 0; j_U32 < BofCircularBufferParam_X.NbMaxElement_U32; j_U32++)
  {
    Sts_E = pBofCollection->Pop(&PopVal_U32, 0, &IndexOf_U32, nullptr);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(IndexOf_U32, j_U32);
    EXPECT_EQ(PopVal_U32, Val_U32);
    Nb_U32 = pBofCollection->GetNbElement();
    EXPECT_EQ(Nb_U32, BofCircularBufferParam_X.NbMaxElement_U32);
    Val_U32++;
  }
  Sts_E = pBofCollection->Push(&Val_U32, 0, &IndexOf_U32, nullptr);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Val_U32 = 10;
  for (j_U32 = 0; j_U32 < BofCircularBufferParam_X.NbMaxElement_U32; j_U32++)
  {
    Sts_E = pBofCollection->Pop(&PopVal_U32, 0, &IndexOf_U32, nullptr);
    EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
    Nb_U32 = pBofCollection->GetNbElement();
    EXPECT_EQ(Nb_U32, BofCircularBufferParam_X.NbMaxElement_U32);
  }
  Sts_E = pBofCollection->Push(&i_U32, 0, &IndexOf_U32, nullptr);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Val_U32 = 10;
  for (j_U32 = 0; j_U32 < BofCircularBufferParam_X.NbMaxElement_U32; j_U32++)
  {
    Sts_E = pBofCollection->UnlockPop(j_U32);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    Nb_U32 = pBofCollection->GetNbElement();
    EXPECT_EQ(Nb_U32, BofCircularBufferParam_X.NbMaxElement_U32 - 1);
    Sts_E = pBofCollection->Push(&Val_U32, 0, &IndexOf_U32, nullptr);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    Val_U32++;
  }

  pBofCollection->Reset();
  Val_U32 = 10;
  for (j_U32 = 0; j_U32 < BofCircularBufferParam_X.NbMaxElement_U32; j_U32++)
  {
    Sts_E = pBofCollection->Push(&Val_U32, 0, &IndexOf_U32, nullptr);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(IndexOf_U32, j_U32);
    Nb_U32 = pBofCollection->GetNbElement();
    EXPECT_EQ(Nb_U32, j_U32 + 1);
    Val_U32++;
  }

  Val_U32 = 10;
  for (j_U32 = 0; j_U32 < BofCircularBufferParam_X.NbMaxElement_U32; j_U32++)
  {
    Sts_E = pBofCollection->UnlockPop(j_U32);
    EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
    Sts_E = pBofCollection->Pop(&PopVal_U32, 0, &IndexOf_U32, nullptr);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(IndexOf_U32, j_U32);
    Nb_U32 = pBofCollection->GetNbElement();
    EXPECT_EQ(Nb_U32, BofCircularBufferParam_X.NbMaxElement_U32);
    EXPECT_EQ(PopVal_U32, Val_U32);
    Val_U32++;
  }

  Val_U32 = 10;
  for (j_U32 = 0; j_U32 < BofCircularBufferParam_X.NbMaxElement_U32; j_U32++)
  {
    Sts_E = pBofCollection->UnlockPop(BofCircularBufferParam_X.NbMaxElement_U32 - j_U32 - 1);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    Nb_U32 = pBofCollection->GetNbElement();
    EXPECT_EQ(Nb_U32, BofCircularBufferParam_X.NbMaxElement_U32 - j_U32 - 1);
    Sts_E = pBofCollection->Push(&Val_U32, 0, &IndexOf_U32, nullptr);
    if (j_U32 == BofCircularBufferParam_X.NbMaxElement_U32 - 1)
    {
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    }
    else
    {
      EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
    }
    Val_U32++;
  }
  for (k_U32 = 1; k_U32 < BofCircularBufferParam_X.NbMaxElement_U32 - 1; k_U32++)
  {
    Val_U32 = 10;
    memset(pPosLocked_B, 0, sizeof(pPosLocked_B));
    pBofCollection->Reset();
    IndexOfPop_U32 = 0;
    for (i_U32 = 0; i_U32 < k_U32; i_U32++)
    {
      Index_U32 = i_U32 % BofCircularBufferParam_X.NbMaxElement_U32;
      Sts_E = pBofCollection->Push(&Val_U32, 0, &IndexOf_U32, nullptr);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_EQ(IndexOf_U32, Index_U32);
      Sts_E = pBofCollection->Pop(&j_U32, 0, &IndexOfLock_U32, nullptr);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_EQ(IndexOfLock_U32, Index_U32);
      EXPECT_EQ(j_U32, Val_U32);
      pPosLocked_B[IndexOfLock_U32] = true;
      Val_U32++;
    }
    for (i_U32 = 0; i_U32 < BofCircularBufferParam_X.NbMaxElement_U32 * 3; i_U32++)
    {
      Index_U32 = (i_U32 + k_U32) % BofCircularBufferParam_X.NbMaxElement_U32;
      Sts_E = pBofCollection->Push(&Val_U32, 0, &IndexOf_U32, nullptr);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_EQ(IndexOf_U32, Index_U32);
      Sts_E = pBofCollection->Pop(&j_U32, 0, &IndexOfLock_U32, nullptr);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_EQ(IndexOfLock_U32, Index_U32);
      EXPECT_EQ(j_U32, Val_U32);
      pPosLocked_B[IndexOfLock_U32] = true;
      Val_U32++;

      for (j_U32 = 0; j_U32 < BofCircularBufferParam_X.NbMaxElement_U32; j_U32++)
      {
        Sts_B = pBofCollection->IsLocked(j_U32);
        // printf("k %d i %d j %d Busy %d %d %d %d %d %d %d %d Nb %d\n", k_U32, i_U32, j_U32, pPosLocked_B[0], pPosLocked_B[1], pPosLocked_B[2], pPosLocked_B[3], pPosLocked_B[4], pPosLocked_B[5], pPosLocked_B[6], pPosLocked_B[7],
        // pBofCollection->GetNbElement());

        if (pPosLocked_B[j_U32])
        {
          EXPECT_TRUE(Sts_B);
        }
        else
        {
          EXPECT_FALSE(Sts_B);
        }
      }
      Sts_E = pBofCollection->UnlockPop(IndexOfPop_U32);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      pPosLocked_B[IndexOfPop_U32] = false;
      IndexOfPop_U32++;
      if (IndexOfPop_U32 >= BofCircularBufferParam_X.NbMaxElement_U32)
      {
        IndexOfPop_U32 = 0;
      }
      //			Sts_E = pBofCollection->Pop(&Val_U32, 0, &IndexOf_U32,true);
      //			EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    }
  }
}
#if defined(__EMSCRIPTEN__)
#else
TEST(CircularBuffer_Test, BlockingMode)
{
  BOF_CIRCULAR_BUFFER_PARAM BofCircularBufferParam_X;
  BOFERR Sts_E;
  uint32_t i_U32, j_U32;
  BofDateTime Now;
  std::string Now_S;

  BofCircularBuffer<uint64_t> *pBofCollection;
  std::thread pPushThread[NB_MAX_CLIENT];
  std::thread PopThread;

  Sts_E = BOF_ERR_ENOMEM;
  BofCircularBufferParam_X.MultiThreadAware_B = true;
  BofCircularBufferParam_X.NbMaxElement_U32 = 8;
  BofCircularBufferParam_X.Overwrite_B = false;
  BofCircularBufferParam_X.Blocking_B = true;
  pBofCollection = new BofCircularBuffer<uint64_t>(BofCircularBufferParam_X);
  if (pBofCollection)
  {
    Sts_E = pBofCollection->LastErrorCode();
  }
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  for (i_U32 = 0; i_U32 < 1; i_U32++)
  {
    Bof_Now(Now);
    Now_S = Now.ToString();

    // printf("[%X] ======= %s ====================\n", i_U32, Now_S.c_str());

    for (j_U32 = 0; j_U32 < NB_MAX_CLIENT; j_U32++)
    {
      pPushThread[j_U32] = std::thread(&PushValue, j_U32, NB_PUSH_PER_CLIENT, pBofCollection);
    }
    BOF::Bof_MsSleep(50);
    PopThread = std::thread(&PopValue, 0, NB_MAX_CLIENT * NB_PUSH_PER_CLIENT, pBofCollection);

    for (j_U32 = 0; j_U32 < NB_MAX_CLIENT; j_U32++)
    {
      pPushThread[j_U32].join();
      // printf("pPushThread %d out\n", j_U32);
    }
    PopThread.join();
    // printf("PopThread out\n");
  }
}
#endif

TEST(CircularBuffer_Test, StdString)
{
  BOF_CIRCULAR_BUFFER_PARAM BofCircularBufferParam_X;
  BofCircularBuffer<std::string> *pReplyCollection = nullptr;
  uint32_t i_U32, NbLoop_U32;
  std::string Reply_S, Read_S, Cpy_S;
  std::list<std::string> StrCollection;

  BofCircularBufferParam_X.MultiThreadAware_B = true;
  BofCircularBufferParam_X.NbMaxElement_U32 = 16;
  BofCircularBufferParam_X.Overwrite_B = true;
  pReplyCollection = new BofCircularBuffer<std::string>(BofCircularBufferParam_X);
  ASSERT_TRUE(pReplyCollection != nullptr);
  ASSERT_EQ(pReplyCollection->LastErrorCode(), BOF_ERR_NO_ERROR);

  NbLoop_U32 = 10; // 50;
  for (i_U32 = 0; i_U32 < NbLoop_U32; i_U32++)
  {
    Reply_S = Bof_Random(false, 0x10000, 'A', 'Z');
    StrCollection.push_back(Reply_S);
    ASSERT_EQ(pReplyCollection->Push(&Reply_S, 0, nullptr, nullptr), BOF_ERR_NO_ERROR);
    if (pReplyCollection->GetNbElement() == pReplyCollection->GetCapacity())
    {
      ASSERT_EQ(pReplyCollection->Pop(&Read_S, 0, nullptr, nullptr), BOF_ERR_NO_ERROR);
      Cpy_S = StrCollection.front();
      ASSERT_STREQ(Read_S.c_str(), Read_S.c_str());
      StrCollection.pop_front();
      ASSERT_EQ(StrCollection.size(), pReplyCollection->GetCapacity() - 1);
    }
  }

  BOF_SAFE_DELETE(pReplyCollection);
}
struct QUEUE_ITEM
{
  uint32_t Val_U32;
  uint8_t *pData_U8;
  float pFloat_f[8];
  std::string Val_S;

  QUEUE_ITEM()
  {
    Reset();
  }
  void Reset()
  {
    uint32_t i_U32;

    Val_U32 = 0;
    pData_U8 = nullptr;
    for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pFloat_f); i_U32++)
    {
      pFloat_f[i_U32] = 0;
    }
    Val_S = "";
  }
};
TEST(CircularBuffer_Test, Perf)
{
  uint32_t i_U32, j_U32, Index_U32;
  uint8_t pData_U8[0x1000];
  BOF::BOF_CIRCULAR_BUFFER_PARAM CbParam_X;
  QUEUE_ITEM Item_X;
  constexpr bool IGNORE_FIRST_SAMPLE = true;
  constexpr uint32_t NB_QUEUE_OP = 100;
  CbParam_X.Blocking_B = true;
  CbParam_X.MultiThreadAware_B = true;
  CbParam_X.NbMaxElement_U32 = NB_QUEUE_OP;
  BOF::BofCircularBuffer<QUEUE_ITEM> Cb(CbParam_X);
  BOF::BofProfiler Profiler(BOF::BOF_PROFILER_TYPE::BOF_PROFILER_TYPE_NORMAL, 2);

  memset(pData_U8, 0, sizeof(pData_U8));
  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < NB_QUEUE_OP; i_U32++)
  {
    Item_X.Val_U32 = i_U32;
    Item_X.pData_U8 = &pData_U8[Index_U32];
    pData_U8[Index_U32] = 'A' + (i_U32 % 26);
    for (j_U32 = 0; j_U32 < BOF_NB_ELEM_IN_ARRAY(Item_X.pFloat_f); j_U32++)
    {
      Item_X.pFloat_f[j_U32] = i_U32 * j_U32;
    }
    Item_X.Val_S = BOF::Bof_Random(false, (i_U32 * 16) + j_U32, 'A', 'Z');
    Profiler.EnterBench(0);
    EXPECT_EQ(Cb.Push(&Item_X, 1000, nullptr, nullptr), BOF_ERR_NO_ERROR);
    Profiler.LeaveBench(IGNORE_FIRST_SAMPLE, 0);
    Index_U32++;
    if (Index_U32 > sizeof(pData_U8))
    {
      Index_U32 = 0;
    }
  }
  printf("%llu Push Min %llu Mean %llu Max %llu Lck %llu\n", Profiler.GetNbSample(0), Profiler.GetMin(0), Profiler.GetMean(0), Profiler.GetMax(0), Profiler.GetLockCount(0));

  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < NB_QUEUE_OP; i_U32++)
  {
    Item_X.Reset();
    Profiler.EnterBench(1);
    EXPECT_EQ(Cb.Pop(&Item_X, 1000, nullptr, nullptr), BOF_ERR_NO_ERROR);
    Profiler.LeaveBench(IGNORE_FIRST_SAMPLE, 1);

    Index_U32++;
    if (Index_U32 > sizeof(pData_U8))
    {
      Index_U32 = 0;
    }
  }
  EXPECT_NE(Cb.Pop(&Item_X, 1000, nullptr, nullptr), BOF_ERR_NO_ERROR);
  printf("%llu Pop Min %llu Mean %llu Max %llu Lck %llu\n", Profiler.GetNbSample(1), Profiler.GetMin(1), Profiler.GetMean(1), Profiler.GetMax(1), Profiler.GetLockCount(1));
}

TEST(CircularBuffer_Test, PerfThread)
{
  constexpr bool IGNORE_FIRST_SAMPLE = true;
  constexpr uint32_t NB_QUEUE_OP = 100;
  constexpr uint32_t NB_QUEUE_PRODUCER_CONSUMER = 8;
  uint8_t pData_U8[0x1000];
  BOF::BOF_CIRCULAR_BUFFER_PARAM CbParam_X;
  CbParam_X.Blocking_B = true;
  CbParam_X.MultiThreadAware_B = true;
  CbParam_X.NbMaxElement_U32 = (NB_QUEUE_PRODUCER_CONSUMER * NB_QUEUE_OP);
  BOF::BofCircularBuffer<QUEUE_ITEM> Cb(CbParam_X);
  BOF::BofProfiler Profiler(BOF::BOF_PROFILER_TYPE::BOF_PROFILER_TYPE_NORMAL, NB_QUEUE_PRODUCER_CONSUMER + NB_QUEUE_PRODUCER_CONSUMER);
  std::thread pThread[NB_QUEUE_PRODUCER_CONSUMER + NB_QUEUE_PRODUCER_CONSUMER];
  uint32_t i_U32, j_U32, Index_U32, pDequeued_U32[NB_QUEUE_PRODUCER_CONSUMER * NB_QUEUE_OP], NbLastMax_U32;
  BOF_STAT_MAX<uint64_t> pLastMax_X[BOF_STAT_KEEP_LAST_NB_MAX_VAL];
  static bool S_AllThreadReady_B = false;

  memset(pDequeued_U32, 0, sizeof(pDequeued_U32));
  // Producers
  for (i_U32 = 0; i_U32 < NB_QUEUE_PRODUCER_CONSUMER; i_U32++)
  {
    pThread[i_U32] = std::thread([&](uint32_t _Id_U32) {
      QUEUE_ITEM Item_X;
      uint32_t j_U32=0;

      while (!S_AllThreadReady_B)
      {
        BOF::Bof_MsSleep(1);
      }
      for (j_U32 = 0; j_U32 < NB_QUEUE_OP; j_U32++)
      {
        Item_X.Val_U32 = (_Id_U32 * NB_QUEUE_OP) + j_U32;
        // printf("Thread %d push %d\n", _Id_U32, Item_X.Val_U32);
        Profiler.EnterBench(_Id_U32);
        EXPECT_EQ(Cb.Push(&Item_X, 1000, nullptr, nullptr), BOF_ERR_NO_ERROR);
        Profiler.LeaveBench(IGNORE_FIRST_SAMPLE, _Id_U32);
      }
    },
                                 i_U32);
  }
  // Consumers
  for (i_U32 = NB_QUEUE_PRODUCER_CONSUMER; i_U32 < (NB_QUEUE_PRODUCER_CONSUMER + NB_QUEUE_PRODUCER_CONSUMER); i_U32++)
  {
    pThread[i_U32] = std::thread([&](uint32_t _Id_U32) {
      QUEUE_ITEM Item_X;
      uint32_t j_U32 = 0;

      while (!S_AllThreadReady_B)
      {
        BOF::Bof_MsSleep(1);
      }
      for (j_U32 = NB_QUEUE_OP; j_U32 < (NB_QUEUE_OP + NB_QUEUE_OP); j_U32++)
      {
        Profiler.EnterBench(_Id_U32);
        EXPECT_EQ(Cb.Pop(&Item_X, 1000, nullptr, nullptr), BOF_ERR_NO_ERROR);
        Profiler.LeaveBench(IGNORE_FIRST_SAMPLE, _Id_U32);
        // printf("Thread %d pop %d\n", _Id_U32, Item_X.Val_U32);

        ++pDequeued_U32[Item_X.Val_U32];
      }
    },
                                 i_U32);
  }
  S_AllThreadReady_B = true;
  // Wait for all pThread
  for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pThread); i_U32++)
  {
    pThread[i_U32].join();
  }

  // Collect any leftovers (could be some if e.g. consumers finish before producers)
  QUEUE_ITEM Item_X;
  while (Cb.Pop(&Item_X, 1000, nullptr, nullptr) == BOF_ERR_NO_ERROR)
  {
    ++pDequeued_U32[Item_X.Val_U32];
  }

  // Make sure everything went in and came back out!
  for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pDequeued_U32); i_U32++)
  {
    EXPECT_EQ(pDequeued_U32[i_U32], 1);
  }
  printf("---PUSH--------------------------\n");
  for (i_U32 = 0; i_U32 < NB_QUEUE_PRODUCER_CONSUMER; i_U32++)
  {
    NbLastMax_U32 = Profiler.GetLastMax(i_U32, pLastMax_X);
    printf("NbOp %llu NbMax %d Push[%d] Min %llu Mean %llu Max %llu Lck %llu\n", Profiler.GetNbSample(i_U32), NbLastMax_U32, i_U32, Profiler.GetMin(i_U32), Profiler.GetMean(i_U32), Profiler.GetMax(i_U32), Profiler.GetLockCount(i_U32));
    EXPECT_EQ(Profiler.GetNbSample(i_U32), NB_QUEUE_OP);
    for (j_U32 = 0; j_U32 < NbLastMax_U32; j_U32++)
    {
      printf("  Index %llu Max %llu\n", pLastMax_X[j_U32].MaxIndex_U64, pLastMax_X[j_U32].Max);
    }
  }
  printf("---POP---------------------------\n");
  for (i_U32 = NB_QUEUE_PRODUCER_CONSUMER; i_U32 < (NB_QUEUE_PRODUCER_CONSUMER + NB_QUEUE_PRODUCER_CONSUMER); i_U32++)
  {
    NbLastMax_U32 = Profiler.GetLastMax(i_U32, pLastMax_X);
    printf("NbOp %llu NbMax %d Pop[%d] Min %llu Mean %llu Max %llu Lck %llu\n", Profiler.GetNbSample(i_U32), NbLastMax_U32, i_U32, Profiler.GetMin(i_U32), Profiler.GetMean(i_U32), Profiler.GetMax(i_U32), Profiler.GetLockCount(i_U32));
    EXPECT_EQ(Profiler.GetNbSample(i_U32), NB_QUEUE_OP);
    for (j_U32 = 0; j_U32 < NbLastMax_U32; j_U32++)
    {
      printf("  Index %llu Max %llu\n", pLastMax_X[j_U32].MaxIndex_U64, pLastMax_X[j_U32].Max);
    }
  }
}