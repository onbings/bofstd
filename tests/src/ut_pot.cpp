/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the pot class
 *
 * Name:        ut_pot.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */
#include <bofstd/bofdatetime.h>
#include <bofstd/bofpot.h>

#include "gtestrunner.h"

#include <thread>

USE_BOF_NAMESPACE()

struct BOF_POT_ELEMENT
{
  uint32_t MagicNumber_U32;
  uint8_t Byte_U8;
  uint16_t Word_U16;
  uint32_t Long_U32;
  BOF_POT_ELEMENT()
  {
    Reset();
  }
  void Reset()
  {
    MagicNumber_U32 = 0;
    Byte_U8 = 0;
    Word_U16 = 0;
    Long_U32 = 0;
  }
};

class Pot_Test : public testing::Test
{
public:
  Pot_Test()
      : mpBofPot_O(nullptr)
  {
  }

  virtual ~Pot_Test()
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

  BofPot<BOF_POT_ELEMENT> *mpBofPot_O;
  BOF_POT_PARAM mBofPotParam_X;
  void PotTest();

private:
};

/*** Factory functions called at the beginning/end of each test case **********/

void Pot_Test::SetUpTestCase()
{
}

void Pot_Test::TearDownTestCase()
{
}

#define BOFPOTMAGIC 0x12345678

void Pot_Test::SetUp()
{
}

void Pot_Test::TearDown()
{
  BOF_SAFE_DELETE(mpBofPot_O);
  EXPECT_TRUE(mpBofPot_O == nullptr);
}

TEST_F(Pot_Test, PotWithMagic)
{
  mBofPotParam_X.Reset();
  mBofPotParam_X.MultiThreadAware_B = true;
  mBofPotParam_X.GetOpPreserveContent_B = true;
  mBofPotParam_X.MagicNumber_U32 = BOFPOTMAGIC;
  mBofPotParam_X.PotCapacity_U32 = 128;
  mBofPotParam_X.Blocking_B = false;
  mpBofPot_O = new BofPot<BOF_POT_ELEMENT>(mBofPotParam_X);

  EXPECT_TRUE(mpBofPot_O != nullptr);
  EXPECT_TRUE(mpBofPot_O->LastErrorCode() == BOF_ERR_NO_ERROR);

  PotTest();
}

TEST_F(Pot_Test, PotWithoutMagic)
{
  mBofPotParam_X.Reset();
  mBofPotParam_X.MultiThreadAware_B = true;
  mBofPotParam_X.GetOpPreserveContent_B = true;
  mBofPotParam_X.MagicNumber_U32 = 0; // BOFPOTMAGIC;
  mBofPotParam_X.PotCapacity_U32 = 128;
  mBofPotParam_X.Blocking_B = false;
  mpBofPot_O = new BofPot<BOF_POT_ELEMENT>(mBofPotParam_X);

  EXPECT_TRUE(mpBofPot_O != nullptr);
  EXPECT_TRUE(mpBofPot_O->LastErrorCode() == BOF_ERR_NO_ERROR);

  PotTest();
}
void Pot_Test::PotTest()
{
  uint32_t i_U32, Nb_U32, Index_U32;
  BOF_POT_ELEMENT *pPotElem1_X, *pPotElem2_X;
  bool Sts_B;
  BOFERR Sts_E;

  Sts_E = mpBofPot_O->IsPotElementLocked(nullptr);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofPot_O->IsPotElementInUse(nullptr);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 = mpBofPot_O->GetIndexOfEntry(nullptr);
  EXPECT_TRUE(0 != Index_U32);

  Sts_E = mpBofPot_O->IsPotElementLocked((BOF_POT_ELEMENT *)0x12345678);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofPot_O->IsPotElementInUse((BOF_POT_ELEMENT *)0x12345678);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 = mpBofPot_O->GetIndexOfEntry((BOF_POT_ELEMENT *)0x12345678);
  EXPECT_TRUE(0 != Index_U32);

  Sts_E = mpBofPot_O->IsPotElementLocked((BOF_POT_ELEMENT *)&i_U32);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Sts_E = mpBofPot_O->IsPotElementInUse((BOF_POT_ELEMENT *)&i_U32);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);
  Index_U32 = mpBofPot_O->GetIndexOfEntry((BOF_POT_ELEMENT *)&i_U32);
  EXPECT_TRUE(0 != Index_U32);

  Sts_E = mpBofPot_O->Unlock((BOF_POT_ELEMENT *)&i_U32);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);

  Sts_E = mpBofPot_O->Release((BOF_POT_ELEMENT *)&i_U32);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);

  Sts_B = mpBofPot_O->IsPotEmpty();
  EXPECT_FALSE(Sts_B);
  Sts_B = mpBofPot_O->IsPotFull();
  EXPECT_TRUE(Sts_B);

  Index_U32 = mpBofPot_O->GetFirstFreeIndexToTry();
  EXPECT_EQ(0, Index_U32);

  pPotElem1_X = mpBofPot_O->Get(0);
  EXPECT_TRUE(pPotElem1_X != nullptr);
  pPotElem1_X->Byte_U8 = 1;
  pPotElem1_X->Word_U16 = 2;
  pPotElem1_X->Long_U32 = 3;
  if (mBofPotParam_X.MagicNumber_U32)
  {
    EXPECT_EQ(pPotElem1_X->MagicNumber_U32, BOFPOTMAGIC);
  }
  Index_U32 = mpBofPot_O->GetFirstFreeIndexToTry();
  EXPECT_EQ(1, Index_U32);

  Sts_E = mpBofPot_O->Unlock(pPotElem1_X);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);

  Sts_B = mpBofPot_O->IsPotFull();
  EXPECT_FALSE(Sts_B);
  Sts_B = mpBofPot_O->IsPotEmpty();
  EXPECT_FALSE(Sts_B);

  Nb_U32 = mpBofPot_O->GetNbElementOutOfThePot();
  EXPECT_EQ(1, Nb_U32);

  Nb_U32 = mpBofPot_O->GetCapacity();
  EXPECT_EQ(mBofPotParam_X.PotCapacity_U32, Nb_U32);

  Nb_U32 = mpBofPot_O->GetNbFreeElement();
  EXPECT_EQ(mBofPotParam_X.PotCapacity_U32 - 1, Nb_U32);

  pPotElem2_X = mpBofPot_O->Lock(0);
  if (mBofPotParam_X.MagicNumber_U32)
  {
    EXPECT_TRUE(pPotElem2_X == nullptr);
    Nb_U32 = mpBofPot_O->GetNbElementOutOfThePot();
    EXPECT_EQ(1, Nb_U32);

    Nb_U32 = mpBofPot_O->GetNbFreeElement();
    EXPECT_EQ(mBofPotParam_X.PotCapacity_U32 - 1, Nb_U32);
  }
  else
  {
    EXPECT_TRUE(pPotElem2_X != nullptr);
    pPotElem2_X->Byte_U8 = 4;
    pPotElem2_X->Word_U16 = 5;
    pPotElem2_X->Long_U32 = 6;

    Index_U32 = mpBofPot_O->GetFirstFreeIndexToTry();
    EXPECT_EQ(2, Index_U32);

    Sts_E = mpBofPot_O->Unlock(pPotElem1_X);
    EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);

    Nb_U32 = mpBofPot_O->GetNbElementOutOfThePot();
    EXPECT_EQ(2, Nb_U32);

    Nb_U32 = mpBofPot_O->GetNbFreeElement();
    EXPECT_EQ(mBofPotParam_X.PotCapacity_U32 - 2, Nb_U32);

    Sts_E = mpBofPot_O->IsPotElementLocked(pPotElem2_X);
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
    Sts_E = mpBofPot_O->IsPotElementInUse(pPotElem2_X);
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);
    Index_U32 = mpBofPot_O->GetIndexOfEntry(pPotElem2_X);
    EXPECT_EQ(1, Index_U32);

    Sts_E = mpBofPot_O->Unlock(pPotElem2_X);
    EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

    pPotElem2_X = mpBofPot_O->GetIndexedPotElement(1);
    EXPECT_TRUE(pPotElem2_X != nullptr);
    EXPECT_EQ(pPotElem2_X->Byte_U8, 4);
    EXPECT_EQ(pPotElem2_X->Word_U16, 5);
    EXPECT_EQ(pPotElem2_X->Long_U32, 6);
  }
  Sts_B = mpBofPot_O->IsPotFull();
  EXPECT_FALSE(Sts_B);
  Sts_B = mpBofPot_O->IsPotEmpty();
  EXPECT_FALSE(Sts_B);
  Nb_U32 = mpBofPot_O->GetCapacity();
  EXPECT_EQ(mBofPotParam_X.PotCapacity_U32, Nb_U32);

  Sts_E = mpBofPot_O->IsPotElementLocked(pPotElem1_X);
  EXPECT_NE(BOF_ERR_NO_ERROR, Sts_E);

  Sts_E = mpBofPot_O->IsPotElementInUse(pPotElem1_X);
  EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

  Index_U32 = mpBofPot_O->GetIndexOfEntry(pPotElem1_X);
  EXPECT_EQ(0, Index_U32);

  pPotElem1_X = mpBofPot_O->GetIndexedPotElement(mBofPotParam_X.PotCapacity_U32);
  EXPECT_TRUE(pPotElem1_X == nullptr);
  pPotElem1_X = mpBofPot_O->GetIndexedPotElement(mBofPotParam_X.PotCapacity_U32 - 1);
  EXPECT_TRUE(pPotElem1_X != nullptr);

  pPotElem1_X = mpBofPot_O->GetIndexedPotElement(0);
  EXPECT_TRUE(pPotElem1_X != nullptr);
  EXPECT_EQ(pPotElem1_X->Byte_U8, 1);
  EXPECT_EQ(pPotElem1_X->Word_U16, 2);
  EXPECT_EQ(pPotElem1_X->Long_U32, 3);

  Sts_E = mpBofPot_O->ClearPot(0);

  Nb_U32 = mpBofPot_O->GetNbElementOutOfThePot();
  EXPECT_EQ(0, Nb_U32);
  Nb_U32 = mpBofPot_O->GetNbFreeElement();
  EXPECT_EQ(mBofPotParam_X.PotCapacity_U32, Nb_U32);

  for (i_U32 = 0; i_U32 < mBofPotParam_X.PotCapacity_U32; i_U32++)
  {
    if (i_U32 % 2)
    {
      pPotElem1_X = mpBofPot_O->Get(0);
      EXPECT_TRUE(pPotElem1_X != nullptr);
    }
    else
    {
      if (mBofPotParam_X.MagicNumber_U32)
      {
        pPotElem2_X = mpBofPot_O->Get(0);
        EXPECT_TRUE(pPotElem2_X != nullptr);
      }
      else
      {
        pPotElem2_X = mpBofPot_O->Lock(0);
        EXPECT_TRUE(pPotElem2_X != nullptr);
      }
    }
    Nb_U32 = mpBofPot_O->GetNbElementOutOfThePot();
    EXPECT_EQ(i_U32 + 1, Nb_U32);
    Nb_U32 = mpBofPot_O->GetNbFreeElement();
    EXPECT_EQ(mBofPotParam_X.PotCapacity_U32 - i_U32 - 1, Nb_U32);
  }

  pPotElem2_X = mpBofPot_O->GetFirstUsed(0);
  EXPECT_TRUE(pPotElem1_X != nullptr);
  for (i_U32 = 0; i_U32 < mBofPotParam_X.PotCapacity_U32 - 2; i_U32++)
  {
    if (i_U32 % 2)
    {
    }
    else
    {
      Sts_E = mpBofPot_O->Release(pPotElem2_X);
      EXPECT_EQ(BOF_ERR_NO_ERROR, Sts_E);

      pPotElem2_X = mpBofPot_O->GetNextUsed(pPotElem2_X);
      EXPECT_TRUE(pPotElem2_X != nullptr);
    }
  }
  Nb_U32 = mpBofPot_O->GetNbElementOutOfThePot();
  EXPECT_EQ((mBofPotParam_X.PotCapacity_U32 / 2) + 1, Nb_U32);
  Nb_U32 = mpBofPot_O->GetNbFreeElement();
  EXPECT_EQ((mBofPotParam_X.PotCapacity_U32 / 2) - 1, Nb_U32);

  // DataType *LookForPotElementInUseStartingFromIndex(uint32_t _Index_U32);
}

BOF_MUTEX S_GetReleaseMtx_X;
std::vector<uint64_t *> S_pValCollection;

BOFERR GetValue(uint32_t _Id_U32, uint32_t _NbLoop_U32, BofPot<uint64_t> *_pBofCollection)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  uint64_t *pVal_U64;
  uint32_t i_U32, Start_U32, Max_U32, Delta_U32;

  Max_U32 = 0;
  for (i_U32 = 0; i_U32 < _NbLoop_U32; i_U32++)
  {
    Start_U32 = Bof_GetMsTickCount();
    pVal_U64 = _pBofCollection->Get(2000);
    Delta_U32 = Bof_ElapsedMsTime(Start_U32);
    if ((_Id_U32 == 0) && (Delta_U32 > Max_U32))
    {
      // printf("%03d:Get[%d] in %d n=%d\n", _Id_U32, i_U32, Delta_U32, _pBofCollection->GetNbElementOutOfThePot());
      Max_U32 = Delta_U32;
    }
    EXPECT_TRUE(pVal_U64 != nullptr);

    Rts_E = Bof_LockMutex(S_GetReleaseMtx_X);
    EXPECT_EQ(Rts_E, 0);
    S_pValCollection.push_back(pVal_U64);
    //    printf("%03d:Get[%d]=%p n=%d\n", _Id_U32, i_U32, pVal_U64, _pBofCollection->GetNbElementOutOfThePot());
    Rts_E = Bof_UnlockMutex(S_GetReleaseMtx_X);
    EXPECT_EQ(Rts_E, 0);
  }
  return Rts_E;
}

BOFERR ReleaseValue(uint32_t /*_Id_U32*/, uint32_t _NbLoop_U32, BofPot<uint64_t> *_pBofCollection)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  uint64_t *pVal_U64;
  uint32_t i_U32;

  for (i_U32 = 0; i_U32 < _NbLoop_U32; i_U32++)
  {
    if (S_pValCollection.size())
    {
      Rts_E = Bof_LockMutex(S_GetReleaseMtx_X);
      EXPECT_EQ(Rts_E, 0);
      pVal_U64 = S_pValCollection.back();
      S_pValCollection.pop_back();
      Rts_E = _pBofCollection->Release(pVal_U64);
      //      printf("%03d:Release[%d](%p) n=%d->%X\n", _Id_U32, i_U32, pVal_U64, _pBofCollection->GetNbElementOutOfThePot(), Rts_E);
      EXPECT_EQ(Rts_E, 0);
      Rts_E = Bof_UnlockMutex(S_GetReleaseMtx_X);
      EXPECT_EQ(Rts_E, 0);
    }
    else
    {
      i_U32--;
      Bof_MsSleep(20);
    }
  }
  return Rts_E;
}

const uint32_t NB_MAX_CLIENT = 16;    // 32;                                   // 32;
const uint32_t NB_GET_PER_CLIENT = 8; // 16000;
TEST_F(Pot_Test, BlockingMode)
{
  BOF_POT_PARAM BofPotParam_X;
  BOFERR Sts_E;
  uint32_t i_U32, j_U32;
  BOF::BofDateTime Now;
  std::string Now_S;

  BofPot<uint64_t> *pBofCollection;
  std::thread pPushThread[NB_MAX_CLIENT];
  std::thread PopThread;

  Sts_E = BOF_ERR_ENOMEM;
  BofPotParam_X.MagicNumber_U32 = 0;
  BofPotParam_X.MultiThreadAware_B = true;
  BofPotParam_X.PotCapacity_U32 = 8;
  BofPotParam_X.GetOpPreserveContent_B = true;
  BofPotParam_X.Blocking_B = true;
  pBofCollection = new BofPot<uint64_t>(BofPotParam_X);
  if (pBofCollection)
  {
    Sts_E = pBofCollection->LastErrorCode();
  }
  EXPECT_EQ(Sts_E, 0);
  Sts_E = Bof_CreateMutex("GetRelease_mtx", false, false, S_GetReleaseMtx_X);
  EXPECT_EQ(Sts_E, 0);

  for (i_U32 = 0; i_U32 < 3; i_U32++)
  {
    S_pValCollection.clear();
    BOF::Bof_Now(Now);
    Now_S = Now.ToString();

    // printf("[%X] ======= %s ====================\n", i_U32, Now_S.c_str());

    for (j_U32 = 0; j_U32 < NB_MAX_CLIENT; j_U32++)
    {
      pPushThread[j_U32] = std::thread(&GetValue, j_U32, NB_GET_PER_CLIENT, pBofCollection);
    }
    BOF::Bof_MsSleep(50);
    EXPECT_NE((int)S_pValCollection.size(), 0);

    PopThread = std::thread(&ReleaseValue, 0, NB_MAX_CLIENT * NB_GET_PER_CLIENT, pBofCollection);

    for (j_U32 = 0; j_U32 < NB_MAX_CLIENT; j_U32++)
    {
      pPushThread[j_U32].join();
      //      printf("pGetThread %d out\n", j_U32);
    }
    PopThread.join();
    //  printf("ReleaseThread out\n");

    EXPECT_EQ(S_pValCollection.size(), 0);
  }
}