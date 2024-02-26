/*
 * Copyright (c) 2020-2040, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module test a single producer-single consumer queue
 *
 * History:
 *
 * V 1.00  May 26 2020  BHA : Initial release
 */
#include <bofstd/bofspscqueue.h>
#include <bofstd/bofsystem.h>
#include <bofstd/bofperformance.h>
#include "gtestrunner.h"

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
TEST(ut_spsc, Basic)
{
  uint32_t i_U32, j_U32, Index_U32;
  uint8_t pData_U8[0x1000];
  BOF::BOF_SPSC_QUEUE_PARAM SpScQueueParam_X;
  QUEUE_ITEM Item_X;
  constexpr uint32_t NB_QUEUE_OP = 100;
  SpScQueueParam_X.NbMaxElement_U32 = NB_QUEUE_OP;
  BOF::BofSpScQueue<QUEUE_ITEM> SpScQueue(SpScQueueParam_X);

  memset(pData_U8, 0, sizeof(pData_U8));
  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < NB_QUEUE_OP; i_U32++)
  {
    Item_X.Val_U32 = i_U32;
    Item_X.pData_U8 = &pData_U8[Index_U32];
    pData_U8[Index_U32] = 'A' + (i_U32 % 26);
    for (j_U32 = 0; j_U32 < BOF_NB_ELEM_IN_ARRAY(Item_X.pFloat_f); j_U32++)
    {
      Item_X.pFloat_f[j_U32] = i_U32*j_U32;
    }
    Item_X.Val_S = BOF::Bof_Random(false,(i_U32 *16) + j_U32, 'A','Z');
    EXPECT_EQ(SpScQueue.Push(Item_X,1000), BOF_ERR_NO_ERROR);
    Index_U32++;
    if (Index_U32 > sizeof(pData_U8))
    {
      Index_U32 = 0;
    }
  }

  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < NB_QUEUE_OP; i_U32++)
  {
    Item_X.Reset();
    EXPECT_EQ(SpScQueue.Pop(Item_X,1000), BOF_ERR_NO_ERROR);
    Index_U32++;
    if (Index_U32 > sizeof(pData_U8))
    {
      Index_U32 = 0;
    }
  }
  EXPECT_NE(SpScQueue.Pop(Item_X,1000), BOF_ERR_NO_ERROR);
}

TEST(ut_spsc, Perf)
{
  uint32_t i_U32, j_U32, Index_U32;
  uint8_t pData_U8[0x1000];
  BOF::BOF_SPSC_QUEUE_PARAM SpScQueueParam_X;
  QUEUE_ITEM Item_X;
  constexpr bool IGNORE_FIRST_SAMPLE = true;
  constexpr uint32_t NB_QUEUE_OP = 100;
  SpScQueueParam_X.NbMaxElement_U32 = NB_QUEUE_OP;
  BOF::BofSpScQueue<QUEUE_ITEM> SpScQueue(SpScQueueParam_X);
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
    EXPECT_EQ(SpScQueue.Push(Item_X,1000), BOF_ERR_NO_ERROR);
    Profiler.LeaveBench(IGNORE_FIRST_SAMPLE, 0);
    Index_U32++;
    if (Index_U32 > sizeof(pData_U8))
    {
      Index_U32 = 0;
    }
  }
  printf("%zd Push Min %zd Mean %zd Max %zd Lck %zd\n", Profiler.GetNbSample(0), Profiler.GetMin(0), Profiler.GetMean(0), Profiler.GetMax(0), Profiler.GetLockCount(0));

  Index_U32 = 0;
  for (i_U32 = 0; i_U32 < NB_QUEUE_OP; i_U32++)
  {
    Item_X.Reset();
    Profiler.EnterBench(1);
    EXPECT_EQ(SpScQueue.Pop(Item_X,1000), BOF_ERR_NO_ERROR);
    Profiler.LeaveBench(IGNORE_FIRST_SAMPLE, 1);
    Index_U32++;
    if (Index_U32 > sizeof(pData_U8))
    {
      Index_U32 = 0;
    }
  }
  EXPECT_NE(SpScQueue.Pop(Item_X,1000), BOF_ERR_NO_ERROR);
  printf("%zd Pop Min %zd Mean %zd Max %zd Lck %zd\n", Profiler.GetNbSample(1), Profiler.GetMin(1), Profiler.GetMean(1), Profiler.GetMax(1), Profiler.GetLockCount(1));
}

