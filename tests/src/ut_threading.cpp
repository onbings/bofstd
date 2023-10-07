/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * The test class of the BofThread functionalities
 *
 * Name:        ut_threading.cpp
 * Author:      Bernard HARMEL (BHA)
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  23/06/2014  BHA : Initial release
 */
#include <bofstd/bofsystem.h>
#include <bofstd/bofthread.h>

#include "gtestrunner.h"

#include <atomic>

USE_BOF_NAMESPACE()
/*
TEST(BofThreadPool_Test, Constructor)
{
  BOF_THREAD_PARAM ThreadParam_X;

  ThreadParam_X.Name_S = "ThreadPoolErrUt";
  ThreadParam_X.SignalEvent_B = false;
  ThreadParam_X.StackSize_U32 = 0;
  ThreadParam_X.StartStopTimeoutInMs_U32 = 2000;
  ThreadParam_X.ThreadCpuCoreAffinityMask_U64 = 0;
  ThreadParam_X.ThreadPriority_E = BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000;
  ThreadParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER;
  ThreadParam_X.WakeUpIntervalInMs_U32 = 1000;//!!!! no, see below !!!!
  BofThreadPool PoolErr(8, ThreadParam_X);
  EXPECT_NE(PoolErr.InitThreadPoolErrorCode(), BOF_ERR_NO_ERROR);

  ThreadParam_X.Name_S = "ThreadPoolUt";
  ThreadParam_X.WakeUpIntervalInMs_U32 = 0;//!!!! important !!!!
  BofThreadPool Pool(8, ThreadParam_X);
  EXPECT_EQ(Pool.InitThreadPoolErrorCode(), BOF_ERR_NO_ERROR);
}
*/
BOFERR PoolDispatch1()
{
  printf("%d: PoolDispatch1 starts for 1000 ms...\n", BOF::Bof_GetMsTickCount());
  BOF::Bof_MsSleep(1000);
  printf("%d: PoolDispatch1 ends.\n", BOF::Bof_GetMsTickCount());
  return BOF_ERR_NO_ERROR;
}
BOFERR PoolDispatch2()
{
  printf("%d: PoolDispatch2 starts for 800 ms...\n", BOF::Bof_GetMsTickCount());
  BOF::Bof_MsSleep(800);
  printf("%d: PoolDispatch2 ends.\n", BOF::Bof_GetMsTickCount());
  return BOF_ERR_NO_ERROR;
}
BOFERR PoolDispatch3()
{
  printf("%d: PoolDispatch3 starts for 600 ms...\n", BOF::Bof_GetMsTickCount());
  BOF::Bof_MsSleep(600);
  printf("%d: PoolDispatch3 ends.\n", BOF::Bof_GetMsTickCount());
  return BOF_ERR_NO_ERROR;
}
BOFERR PoolDispatch4()
{
  printf("%d: PoolDispatch4 starts for 400 ms...\n", BOF::Bof_GetMsTickCount());
  BOF::Bof_MsSleep(400);
  printf("%d: PoolDispatch4 ends.\n", BOF::Bof_GetMsTickCount());
  return BOF_ERR_NO_ERROR;
}
BOFERR PoolDispatch5()
{
  printf("%d: PoolDispatch5 starts for 200 ms...\n", BOF::Bof_GetMsTickCount());
  BOF::Bof_MsSleep(200);
  printf("%d: PoolDispatch5 ends.\n", BOF::Bof_GetMsTickCount());
  return BOF_ERR_NO_ERROR;
}
BOFERR PoolDispatch6()
{
  printf("%d: PoolDispatch6 starts for 400 ms...\n", BOF::Bof_GetMsTickCount());
  BOF::Bof_MsSleep(400);
  printf("%d: PoolDispatch6 ends.\n", BOF::Bof_GetMsTickCount());
  return BOF_ERR_NO_ERROR;
}
BOFERR PoolDispatch7()
{
  printf("%d: PoolDispatch7 starts for 600 ms...\n", BOF::Bof_GetMsTickCount());
  BOF::Bof_MsSleep(600);
  printf("%d: PoolDispatch7 ends.\n", BOF::Bof_GetMsTickCount());
  return BOF_ERR_NO_ERROR;
}
BOFERR PoolDispatch8()
{
  printf("%d: PoolDispatch8 starts for 800 ms...\n", BOF::Bof_GetMsTickCount());
  BOF::Bof_MsSleep(800);
  printf("%d: PoolDispatch8 ends.\n", BOF::Bof_GetMsTickCount());
  return BOF_ERR_NO_ERROR;
}
struct THREAD_POOL_CALLBACK
{
  void *pDispatchTicket;
  BOF_THREAD_CALLBACK ThreadCallback;

  THREAD_POOL_CALLBACK()
  {
    Reset();
  }

  void Reset()
  {
    pDispatchTicket = nullptr;
    ThreadCallback = nullptr;
  }
};
TEST(BofThreadPool_Test, Dispatch)
{
  BOF_THREAD_PARAM ThreadParam_X;
  uint32_t i_U32, j_U32, PoolCapacity_U32, Start_U32, Nb_U32;
  THREAD_POOL_CALLBACK pThreadPoolCallback_X[8];
  void *pDispatchTicket;

  PoolCapacity_U32 = 4;
  ThreadParam_X.Name_S = "ThreadPoolUt";
  ThreadParam_X.SignalEvent_B = false;
  ThreadParam_X.StackSize_U32 = 0;
  ThreadParam_X.StartStopTimeoutInMs_U32 = 2000;
  ThreadParam_X.ThreadCpuCoreAffinityMask_U64 = 0;
  ThreadParam_X.ThreadPriority_E = BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000;
  ThreadParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER;
  ThreadParam_X.WakeUpIntervalInMs_U32 = 0; //!!!! important !!!!
  BofThreadPool Pool(PoolCapacity_U32, ThreadParam_X);
  EXPECT_EQ(Pool.InitThreadPoolErrorCode(), BOF_ERR_NO_ERROR);

  pThreadPoolCallback_X[0].ThreadCallback = PoolDispatch1;
  pThreadPoolCallback_X[1].ThreadCallback = PoolDispatch2;
  pThreadPoolCallback_X[2].ThreadCallback = PoolDispatch3;
  pThreadPoolCallback_X[3].ThreadCallback = PoolDispatch4;
  pThreadPoolCallback_X[4].ThreadCallback = PoolDispatch5;
  pThreadPoolCallback_X[5].ThreadCallback = PoolDispatch6;
  pThreadPoolCallback_X[6].ThreadCallback = PoolDispatch7;
  pThreadPoolCallback_X[7].ThreadCallback = PoolDispatch8;
  for (i_U32 = 0; i_U32 < PoolCapacity_U32; i_U32++)
  {
    pThreadPoolCallback_X[i_U32].pDispatchTicket = nullptr;
    EXPECT_EQ(Pool.Dispatch(100, pThreadPoolCallback_X[i_U32].ThreadCallback, &pThreadPoolCallback_X[i_U32].pDispatchTicket), BOF_ERR_NO_ERROR);
    EXPECT_TRUE(pThreadPoolCallback_X[i_U32].pDispatchTicket != nullptr);
    EXPECT_NE(Pool.AckPendingDispatch(100, pThreadPoolCallback_X[i_U32].pDispatchTicket), BOF_ERR_NO_ERROR);
  }
  for (i_U32 = PoolCapacity_U32; i_U32 < BOF_NB_ELEM_IN_ARRAY(pThreadPoolCallback_X); i_U32++)
  {
    Start_U32 = BOF::Bof_GetMsTickCount();
    do
    {
      pDispatchTicket = Pool.GetFirstPendingDispatch();
      if (pDispatchTicket)
      {
        printf("%d: Dispatch %p (%s) is finished\n", BOF::Bof_GetMsTickCount(), pDispatchTicket, Pool.GetDispatchName(pDispatchTicket).c_str());
        EXPECT_EQ(Pool.AckPendingDispatch(0, pDispatchTicket), BOF_ERR_NO_ERROR);
        pThreadPoolCallback_X[i_U32].pDispatchTicket = nullptr;
        EXPECT_EQ(Pool.Dispatch(0, pThreadPoolCallback_X[i_U32].ThreadCallback, &pThreadPoolCallback_X[i_U32].pDispatchTicket), BOF_ERR_NO_ERROR);
        printf("%d: Then post immediate dispatch %p (%s)\n", BOF::Bof_GetMsTickCount(), pThreadPoolCallback_X[i_U32].pDispatchTicket, Pool.GetDispatchName(pThreadPoolCallback_X[i_U32].pDispatchTicket).c_str());
        EXPECT_TRUE(pThreadPoolCallback_X[i_U32].pDispatchTicket != nullptr);
      }
      else
      {
        if (BOF::Bof_ElapsedMsTime(Start_U32) > 2000)
        {
          EXPECT_TRUE(0);
          break;
        }
      }
    } while (pDispatchTicket == nullptr);
  }
  Nb_U32 = Pool.GetNumerOfPendingRunningDispatch();
  printf("%d: GetNumerOfPendingRunningDispatch %d\n", BOF::Bof_GetMsTickCount(), Nb_U32);
  EXPECT_EQ(Nb_U32, PoolCapacity_U32);

  Nb_U32 = Pool.GetNumerOfPendingDispatchToAck();
  printf("%d: GetNumerOfPendingDispatchToAck %d\n", BOF::Bof_GetMsTickCount(), Nb_U32);
  EXPECT_EQ(Nb_U32, 0);
  //  BOF::Bof_MsSleep(1500);
  for (i_U32 = PoolCapacity_U32; i_U32 < BOF_NB_ELEM_IN_ARRAY(pThreadPoolCallback_X); i_U32++)
  {
    Start_U32 = BOF::Bof_GetMsTickCount();
    do
    {
      pDispatchTicket = Pool.GetFirstPendingDispatch();
      if (pDispatchTicket)
      {
        printf("%d: Dispatch %p (%s) is finished\n", BOF::Bof_GetMsTickCount(), pDispatchTicket, Pool.GetDispatchName(pDispatchTicket).c_str());
        EXPECT_EQ(Pool.AckPendingDispatch(0, pDispatchTicket), BOF_ERR_NO_ERROR);
      }
      else
      {
        if (BOF::Bof_ElapsedMsTime(Start_U32) > 2000)
        {
          EXPECT_TRUE(0);
          break;
        }
      }
    } while (pDispatchTicket == nullptr);
  }
}

class BofThread_Test : public ::testing::Test
{
public:
  // Per-test-case set-up. Called before the first test in this test case.
  static void SetUpTestCase()
  {
  }

  // Per-test-case tear-down. Called after the last test in this test case.
  static void TearDownTestCase()
  {
  }

  // static void  *ThreadEntryPoint(void *_pThreadContext);

protected:
  // You can define per-test set-up and tear-down logic as usual.
  virtual void SetUp();
  virtual void TearDown();
};
#if defined(_WIN32)
const uint32_t NBTHREAD = 25; // 250;	//1500;	//1564 on my pc
#else
const uint32_t NBTHREAD = 12; // 125;	//380 on MTPC_LINUX
#endif
const uint32_t PRTPERLOOP = 100; // (NBTHREAD / 12);
const uint32_t STARTSTOPTO = 10000;
static_assert(PRTPERLOOP != 0, "PRTPERLOOP is 0");

uint8_t S_pValueToProtect_U8[10];
uint32_t S_ValueToProtect_U32;
std::atomic<uint32_t> S_NbThread_U32(0);
BOF_MUTEX S_Mtx_X;
uint32_t S_InterlockedVal_U32 = 0;

class TestBofThread : public BofThread
{
public:
  std::atomic<uint32_t> mCpt;

  TestBofThread()
      : BofThread()
  {
    mCpt = 0;
    mExit_B = false;
    S_NbThread_U32++;
  }

  ~TestBofThread()
  {
    mExit_B = true;
    S_NbThread_U32--;
  }

  BOFERR V_OnProcessing() override
  {
    mCpt++;
    return BOF_ERR_NO_ERROR;
  }

  bool mExit_B;
};

void BofThread_Test::SetUp()
{
}

void BofThread_Test::TearDown()
{
  EXPECT_EQ(S_NbThread_U32, 0);
}

struct THREAD_CONTEXT
{
  uint32_t Id_U32;
  int32_t Inc_S32;
  uint32_t NbLoop_U32;
  uint32_t SleepInMs_U32;
  BOF_EVENT Event_X;

  THREAD_CONTEXT()
  {
    Reset();
  }
  void Reset()
  {
    Id_U32 = 0;
    Inc_S32 = 1;
    NbLoop_U32 = 0;
    SleepInMs_U32 = 0;
  }
};

TEST_F(BofThread_Test, VirtualThreadTrue)
{
  TestBofThread TestBofThread_O;
  uint32_t Start_U32, Delta_U32;
  BOFERR Sts_E;

  Sts_E = TestBofThread_O.InitThreadErrorCode();
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  TestBofThread_O.mCpt = 0;
  Sts_E = TestBofThread_O.LaunchBofProcessingThread("V_Thrd_True", true, 100, BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF_THREAD_PRIORITY_000, 1, 1000, 0);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Start_U32 = Bof_GetMsTickCount();
  do
  {
    Bof_MsSleep(1);
  } while (TestBofThread_O.mCpt < 10);
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, static_cast<uint32_t>(100 * 9));
  EXPECT_LT(Delta_U32, static_cast<uint32_t>(100 * 9 * 1.2));
  EXPECT_EQ(S_NbThread_U32, 1);
}

TEST_F(BofThread_Test, VirtualThreadFalse)
{
  TestBofThread TestBofThread_O;
  uint32_t Start_U32, Delta_U32;
  BOFERR Sts_E;

  Sts_E = TestBofThread_O.InitThreadErrorCode();
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  TestBofThread_O.mCpt = 0;
  Sts_E = TestBofThread_O.LaunchBofProcessingThread("V_Thrd_True", false, 100, BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF_THREAD_PRIORITY_000, 1, 1000, 0);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Start_U32 = Bof_GetMsTickCount();
  do
  {
    Bof_MsSleep(1);
  } while (TestBofThread_O.mCpt < 10);
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, static_cast<uint32_t>(100 * 10));
  EXPECT_LT(Delta_U32, static_cast<uint32_t>(100 * 10 * 1.2));
  EXPECT_EQ(S_NbThread_U32, 1);
}

BOFERR S_TheThread(const std::atomic<bool> &_rThreadMustStop_B, void *_pThreadContext)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  THREAD_CONTEXT *pThreadContext_X = (THREAD_CONTEXT *)_pThreadContext;
  uint32_t i_U32, j_U32, SleepTime_U32;
  uint8_t pByteVal_U8[sizeof(S_pValueToProtect_U8)];
  //	uint32_t Now_U32;
  BOF_EVENT Event_X;

  if (pThreadContext_X)
  {
    BOF_ASSERT(pThreadContext_X != nullptr);
    S_NbThread_U32++;
    // Last_U32 = Bof_GetMsTickCount();
    if (pThreadContext_X->NbLoop_U32)
    {
      Rts_E = Bof_LockMutex(S_Mtx_X);
      for (i_U32 = 0; i_U32 < pThreadContext_X->NbLoop_U32; i_U32++)
      {
        // There is test without mutex EXPECT_EQ(Rts_E, BOF_ERR_NO_ERROR);
        //					Now_U32 = Bof_GetMsTickCount();
        // printf("[%06d] T %d D %d i %d\n", pThreadContext_X->Id_U32, Now_U32, Now_U32 - Last_U32,i_U32);
        // Last_U32 = Now_U32;

        for (j_U32 = 0; j_U32 < sizeof(S_pValueToProtect_U8); j_U32++)
        {
          pByteVal_U8[j_U32] = S_pValueToProtect_U8[j_U32];
        }

        SleepTime_U32 = Bof_Random(true, 10, 50);
        //			printf("[%06d] Sleep for %d\n", pThreadContext_X->Id_U32, SleepTime_U32);
        // SleepTime_U32 = 0;
        Bof_MsSleep(SleepTime_U32);

        // printf("[%06d] Wakeup\n", pThreadContext_X->Id_U32);

        for (j_U32 = 0; j_U32 < sizeof(S_pValueToProtect_U8); j_U32++)
        {
          S_pValueToProtect_U8[j_U32] = static_cast<uint8_t>(pByteVal_U8[j_U32] + 1);
        }
        // There is test without mutex EXPECT_EQ(Rts_E, BOF_ERR_NO_ERROR);
      }
      Rts_E = Bof_UnlockMutex(S_Mtx_X);
    }
    else
    {
      while (!_rThreadMustStop_B)
      {
        //			Now_U32 = Bof_GetMsTickCount();
        //			printf("[%06d] T %d D %d Thread lock start\n", pThreadContext_X->Id_U32, Now_U32, Now_U32 - Last_U32);
        //			Last_U32 = Now_U32;
        S_ValueToProtect_U32++;
        Rts_E = Bof_LockMutex(S_Mtx_X);
        EXPECT_EQ(Rts_E, BOF_ERR_NO_ERROR);

        //			S_ValueToProtect_S64 += pThreadContext_X->Inc_S32;
        SleepTime_U32 = Bof_Random(true, 10, 100);

        //			Now_U32 = Bof_GetMsTickCount();
        //			printf("[%06d] T %d D %d Sleep for %d\n", pThreadContext_X->Id_U32, Now_U32, Now_U32 - Last_U32, SleepTime_U32);
        //			Last_U32 = Now_U32;
        Bof_MsSleep(SleepTime_U32);
        Rts_E = Bof_UnlockMutex(S_Mtx_X);
        EXPECT_EQ(Rts_E, BOF_ERR_NO_ERROR);
        //			Now_U32 = Bof_GetMsTickCount();
        //			printf("[%06d] T %d D %d Thread lock end exit %d sts %d\n", pThreadContext_X->Id_U32, Now_U32, Now_U32 - Last_U32,  Rts_E);
        //			Last_U32 = Now_U32;

        //			Now_U32 = Bof_GetMsTickCount();
        //			printf("[%06d] T %d D %d Thread wait start\n", pThreadContext_X->Id_U32, Now_U32, Now_U32 - Last_U32);
        //			Last_U32 = Now_U32;
        Rts_E = Bof_WaitForEvent(pThreadContext_X->Event_X, pThreadContext_X->SleepInMs_U32, 0);
        //			Now_U32 = Bof_GetMsTickCount();
        //			printf("[%06d] T %d D %d Thread wait end exit %d sts %d\n", pThreadContext_X->Id_U32, Now_U32, Now_U32 - Last_U32, _rIsThreadMustStop_B, Sts_E);
        //			Last_U32 = Now_U32;
      }
    }
  }
  S_NbThread_U32--;
  //	Now_U32 = Bof_GetMsTickCount();
  // printf("[%06d] T %d D %d Thread EXIT end nb %d\n", pThreadContext_X->Id_U32, Now_U32, Now_U32 - Last_U32, S_NbThread_U32);
  //	Last_U32 = Now_U32;
  // Any other error code different from BOF_ERR_NO_ERROR will exit the tread loop
  // Returning BOF_ERR_EXIT_THREAD will exit the thread loop with an exit code of BOF_ERR_NO_ERROR
  // Thread will be stopped if someone calls Bof_StopThread
  Rts_E = BOF_ERR_EXIT_THREAD;
  return Rts_E;
}

TEST(Threading_Test, InterlockedCompareExchange)
{
  uint32_t Val_U32;

  S_InterlockedVal_U32 = 0;
  EXPECT_EQ(S_InterlockedVal_U32, 0);
  Val_U32 = Bof_InterlockedCompareExchange(&S_InterlockedVal_U32, 69, 1);
  EXPECT_EQ(Val_U32, 0);
  EXPECT_EQ(S_InterlockedVal_U32, 0);

  Val_U32 = Bof_InterlockedCompareExchange(&S_InterlockedVal_U32, 69, 0);
  EXPECT_EQ(Val_U32, 0);
  EXPECT_EQ(S_InterlockedVal_U32, 69);

  Val_U32 = Bof_InterlockedCompareExchange(&S_InterlockedVal_U32, 22, 68);
  EXPECT_EQ(Val_U32, 69);
  EXPECT_EQ(S_InterlockedVal_U32, 69);

  Val_U32 = Bof_InterlockedCompareExchange(&S_InterlockedVal_U32, 22, 69);
  EXPECT_EQ(Val_U32, 69);
  EXPECT_EQ(S_InterlockedVal_U32, 22);
}

TEST(Threading_Test, Mutex)
{
  BOFERR Sts_E;
  BOF_MUTEX Mtx_X, RecuMtx_X;

  EXPECT_EQ(Mtx_X.Magic_U32, 0);
  EXPECT_FALSE(Mtx_X.Recursive_B);
  EXPECT_STREQ(Mtx_X.Name_S.c_str(), "");

  Sts_E = Bof_CreateMutex("MyMutex", false, false, Mtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Mtx_X.Magic_U32, BOF_MUTEX_MAGIC);
  EXPECT_FALSE(Mtx_X.Recursive_B);
  EXPECT_STREQ(Mtx_X.Name_S.c_str(), "MyMutex");
  Sts_E = Bof_LockMutex(Mtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  // Hang forever Sts_E = Bof_LockMutex(Mtx_X);
  // EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_UnlockMutex(Mtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CreateMutex("MyMutex", false, false, Mtx_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_DestroyMutex(Mtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CreateMutex("RecuMutex", true, true, RecuMtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_LockMutex(RecuMtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_LockMutex(RecuMtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_UnlockMutex(RecuMtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_UnlockMutex(RecuMtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_DestroyMutex(RecuMtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CreateMutex("RecuMutex", true, true, RecuMtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_LockMutex(RecuMtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_LockMutex(RecuMtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_UnlockMutex(RecuMtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_UnlockMutex(RecuMtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_DestroyMutex(RecuMtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

TEST(Threading_Test, Event)
{
  BOFERR Sts_E;
  BOF_EVENT Event_X;
  uint32_t i_U32, Start_U32, Delta_U32, WaitInMs_U32;

  Sts_E = Bof_SignalEvent(Event_X, 0);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_DestroyEvent(Event_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CreateEvent("MyEvent", false, 1, false, false, Event_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Event_X.Magic_U32, BOF_EVENT_MAGIC);
  EXPECT_STREQ(Event_X.Name_S.c_str(), "MyEvent");
  EXPECT_EQ(Event_X.SignaledBitmask_U64, 0);

  Sts_E = Bof_CreateEvent("MyEvent", false, 1, false, false, Event_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Event_X.Magic_U32, BOF_EVENT_MAGIC);
  EXPECT_STREQ(Event_X.Name_S.c_str(), "MyEvent");
  EXPECT_EQ(Event_X.SignaledBitmask_U64, 0);

  for (i_U32 = 0; i_U32 < 5; i_U32++)
  {
    WaitInMs_U32 = 500;
    Start_U32 = Bof_GetMsTickCount();
    Sts_E = Bof_WaitForEvent(Event_X, WaitInMs_U32, 0);
    Delta_U32 = Bof_ElapsedMsTime(Start_U32);
    EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(Event_X.Magic_U32, BOF_EVENT_MAGIC);
    EXPECT_STREQ(Event_X.Name_S.c_str(), "MyEvent");
    EXPECT_EQ(Event_X.SignaledBitmask_U64, 0);

    EXPECT_GE(Delta_U32, WaitInMs_U32);
  }

  for (i_U32 = 0; i_U32 < 5; i_U32++)
  {
    WaitInMs_U32 = 1000;
    Start_U32 = Bof_GetMsTickCount();
    Sts_E = Bof_SignalEvent(Event_X, 0);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

    Sts_E = Bof_WaitForEvent(Event_X, WaitInMs_U32, 0);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

    Delta_U32 = Bof_ElapsedMsTime(Start_U32);
    EXPECT_EQ(Event_X.Magic_U32, BOF_EVENT_MAGIC);
    EXPECT_STREQ(Event_X.Name_S.c_str(), "MyEvent");
    EXPECT_EQ(Event_X.SignaledBitmask_U64, 0);

    EXPECT_TRUE(Delta_U32 < 100);
  }
  Sts_E = Bof_DestroyEvent(Event_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(Event_X.Magic_U32, BOF_EVENT_MAGIC);
  EXPECT_STREQ(Event_X.Name_S.c_str(), "");
  EXPECT_EQ(Event_X.SignaledBitmask_U64, 0);
  ;
}

TEST(Threading_Test, SingleThread)
{
  BOFERR Sts_E, ThreadRtsCode_E;
  BOF_THREAD_SCHEDULER_POLICY ThreadSchedulerPolicy_E;
  BOF_THREAD_PRIORITY Min_E, Max_E, ThreadPriority_E; //, NewPriority_E, MidPriority_E;
  uint32_t ThreadId_U32;
  THREAD_CONTEXT ThreadContext_X;
  BOF_THREAD Thread_X;

  Sts_E = Bof_CreateMutex("MyMutex", true, true, S_Mtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  ThreadId_U32 = Bof_CurrentThreadId();
  EXPECT_NE(ThreadId_U32, 0);

  Sts_E = Bof_GetThreadPriorityRange(BOF_THREAD_SCHEDULER_POLICY_OTHER, Min_E, Max_E);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Min_E, 0);
  EXPECT_EQ(Max_E, 0);

  Sts_E = Bof_GetThreadPriorityRange(BOF_THREAD_SCHEDULER_POLICY_FIFO, Min_E, Max_E);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Min_E, BOF_THREAD_PRIORITY_001);
  EXPECT_EQ(Max_E, BOF_THREAD_PRIORITY_099);

  Sts_E = Bof_GetThreadPriorityRange(BOF_THREAD_SCHEDULER_POLICY_ROUND_ROBIN, Min_E, Max_E);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Min_E, BOF_THREAD_PRIORITY_001);
  EXPECT_EQ(Max_E, BOF_THREAD_PRIORITY_099);

  Sts_E = Bof_StartThread(Thread_X, 0, 0, BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF_THREAD_PRIORITY_000, 1000);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_StopThread(Thread_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  S_ValueToProtect_U32 = 0;
  ThreadContext_X.Inc_S32 = 1;
  ThreadContext_X.SleepInMs_U32 = 0;
  ThreadContext_X.NbLoop_U32 = 0;
  Sts_E = Bof_CreateThread("MyThread", S_TheThread, &ThreadContext_X, Thread_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Thread_X.ThreadCpuCoreAffinity_U32, 0);
  EXPECT_EQ(Thread_X.Magic_U32, BOF_THREAD_MAGIC);
  EXPECT_STREQ(Thread_X.Name_S.c_str(), "MyThread");
  EXPECT_EQ(Thread_X.ThreadSchedulerPolicy_E, BOF_THREAD_SCHEDULER_POLICY_OTHER);
  EXPECT_EQ(Thread_X.ThreadPriority_E, BOF_THREAD_PRIORITY_000);
  EXPECT_EQ(Thread_X.StackSize_U32, 0);
  EXPECT_EQ(Thread_X.StartStopTimeoutInMs_U32, 0);
  //	EXPECT_EQ(Thread_X.ThreadFunction, S_TheThread);
#if defined(_WIN32)
  EXPECT_EQ(Thread_X.ThreadId, 0);
  EXPECT_EQ(Thread_X.pThread, nullptr);
#else
  EXPECT_EQ(Thread_X.ThreadId, 0);
#endif
  EXPECT_FALSE(Thread_X.ThreadMustStop_B);
  EXPECT_FALSE(Thread_X.ThreadRunning_B);
  EXPECT_EQ(Thread_X.pUserContext, &ThreadContext_X);
  EXPECT_EQ(S_ValueToProtect_U32, 0);

  ThreadContext_X.Reset();
  ThreadContext_X.Inc_S32 = 1;
  ThreadContext_X.SleepInMs_U32 = 10000;
  ThreadContext_X.NbLoop_U32 = 0;
  Sts_E = Bof_CreateEvent("MyEvent", false, 2, false, false, ThreadContext_X.Event_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(ThreadContext_X.Event_X.Magic_U32, BOF_EVENT_MAGIC);
  EXPECT_STREQ(ThreadContext_X.Event_X.Name_S.c_str(), "MyEvent");
  EXPECT_EQ(ThreadContext_X.Event_X.SignaledBitmask_U64, 0);

  Sts_E = Bof_CreateThread("MyThread", S_TheThread, &ThreadContext_X, Thread_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  // Sts_E = Bof_WaitForEvent(ThreadContext_X.Event_X, 1000, 0);

  S_ValueToProtect_U32 = 0;
  Sts_E = Bof_StartThread(Thread_X, 4096, 1, BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF_THREAD_PRIORITY_000, 2000);
  Bof_MsSleep(100);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Thread_X.ThreadCpuCoreAffinity_U32, 1);
  EXPECT_EQ(Thread_X.Magic_U32, BOF_THREAD_MAGIC);
  EXPECT_STREQ(Thread_X.Name_S.c_str(), "MyThread");
  EXPECT_EQ(Thread_X.ThreadSchedulerPolicy_E, BOF_THREAD_SCHEDULER_POLICY_OTHER);
  EXPECT_EQ(Thread_X.ThreadPriority_E, BOF_THREAD_PRIORITY_000);
  EXPECT_EQ(Thread_X.StackSize_U32, 4096);
  EXPECT_EQ(Thread_X.StartStopTimeoutInMs_U32, 2000);
  //	EXPECT_EQ(Thread_X.ThreadFunction, S_TheThread);
#if defined(_WIN32)
  EXPECT_TRUE(Thread_X.ThreadId != 0);
  EXPECT_FALSE(Thread_X.pThread == nullptr);
#else
  EXPECT_NE(Thread_X.ThreadId, 0);
#endif
  EXPECT_FALSE(Thread_X.ThreadMustStop_B);
  EXPECT_TRUE(Thread_X.ThreadRunning_B);
  EXPECT_EQ(Thread_X.pUserContext, &ThreadContext_X);
  Bof_MsSleep(500);
  EXPECT_EQ(S_NbThread_U32, 1);
  EXPECT_TRUE(Thread_X.ThreadRunning_B);

  Sts_E = Bof_GetThreadPriorityLevel(Thread_X, ThreadSchedulerPolicy_E, ThreadPriority_E);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(ThreadSchedulerPolicy_E, BOF_THREAD_SCHEDULER_POLICY_OTHER);
  EXPECT_EQ(ThreadPriority_E, BOF::BOF_THREAD_PRIORITY_000);

  Sts_E = Bof_SetThreadPriorityLevel(Thread_X, BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF_THREAD_PRIORITY_000);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_GetThreadPriorityLevel(Thread_X, ThreadSchedulerPolicy_E, ThreadPriority_E);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_GE(ThreadPriority_E, BOF_THREAD_PRIORITY_000);
  EXPECT_EQ(ThreadSchedulerPolicy_E, BOF_THREAD_SCHEDULER_POLICY_OTHER);
  Bof_MsSleep(500);
  EXPECT_TRUE(Thread_X.ThreadRunning_B);
  EXPECT_NE(S_ValueToProtect_U32, static_cast<uint32_t>(0));
  // Need to do the following 2 lines to start the thread exit sequence as the event has a timeout of 10 s and the startstopto is only 2 sec
  Thread_X.ThreadMustStop_B = true;
  Sts_E = Bof_SignalEvent(ThreadContext_X.Event_X, 0);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  BOF::Bof_MsSleep(500);
  EXPECT_EQ(Bof_GetThreadExitCode(Thread_X, &ThreadRtsCode_E), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ThreadRtsCode_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_StopThread(Thread_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(Thread_X.Magic_U32, BOF_THREAD_MAGIC);
#if defined(_WIN32)
  // EXPECT_TRUE(Thread_X.ThreadId_U32 == 0);
  // EXPECT_TRUE(Thread_X.pThread == nullptr);
#else
  // EXPECT_EQ(Thread_X.ThreadId, 0);
#endif
  EXPECT_TRUE(Thread_X.ThreadMustStop_B);
  EXPECT_FALSE(Thread_X.ThreadRunning_B);

  Sts_E = Bof_DestroyMutex(S_Mtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

TEST(Threading_Test, MultiThread)
{
  BOFERR Sts_E;
  BOF_THREAD_PRIORITY MidPriority_E, Min_E, Max_E;
  //	uint32_t Last_U32;
  uint32_t i_U32;
  uint64_t AvailableFreeMemory_U64, TotalMemorySize_U64;
  THREAD_CONTEXT pThreadContext_X[NBTHREAD];
  BOF_THREAD pThread_X[NBTHREAD];
  std::string Name_S;

  S_NbThread_U32 = 0;
  memset(S_pValueToProtect_U8, 0, sizeof(S_pValueToProtect_U8));

  Sts_E = Bof_GetThreadPriorityRange(BOF_THREAD_SCHEDULER_POLICY_OTHER, Min_E, Max_E);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Min_E, BOF_THREAD_PRIORITY_000);
  EXPECT_EQ(Max_E, BOF_THREAD_PRIORITY_000);

  MidPriority_E = (BOF_THREAD_PRIORITY)((Max_E + Min_E) / 2);
  Sts_E = Bof_CreateMutex("MyMutex", true, false, S_Mtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  for (i_U32 = 0; i_U32 < NBTHREAD; i_U32++)
  {
    if ((i_U32) && !(i_U32 % PRTPERLOOP))
    {
      // printf("%d Create thread %d/%d\n", Bof_GetMsTickCount(), i_U32, NBTHREAD);
    }
    pThreadContext_X[i_U32].Reset();
    pThreadContext_X[i_U32].Id_U32 = i_U32;
    pThreadContext_X[i_U32].Inc_S32 = 1;
    pThreadContext_X[i_U32].SleepInMs_U32 = 1000;
    pThreadContext_X[i_U32].NbLoop_U32 = 0;
    Name_S = "MyEvent_" + std::to_string(i_U32);
    Sts_E = Bof_CreateEvent(Name_S, false, 1, false, false, pThreadContext_X[i_U32].Event_X);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(pThreadContext_X[i_U32].Event_X.Magic_U32, BOF_EVENT_MAGIC);
    EXPECT_STREQ(pThreadContext_X[i_U32].Event_X.Name_S.c_str(), Name_S.c_str());
    EXPECT_EQ(pThreadContext_X[i_U32].Event_X.SignaledBitmask_U64, 0);

    Name_S = "MyThread_" + std::to_string(i_U32);
    Sts_E = Bof_CreateThread(Name_S, S_TheThread, &pThreadContext_X[i_U32], pThread_X[i_U32]);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(pThread_X[i_U32].ThreadCpuCoreAffinity_U32, 0);
    EXPECT_EQ(pThread_X[i_U32].Magic_U32, BOF_THREAD_MAGIC);
    EXPECT_STREQ(pThread_X[i_U32].Name_S.c_str(), Name_S.c_str());
    EXPECT_EQ(pThread_X[i_U32].ThreadSchedulerPolicy_E, BOF_THREAD_SCHEDULER_POLICY_OTHER);
    EXPECT_EQ(pThread_X[i_U32].ThreadPriority_E, BOF_THREAD_PRIORITY_000);
    EXPECT_EQ(pThread_X[i_U32].StackSize_U32, 0);
    EXPECT_EQ(pThread_X[i_U32].StartStopTimeoutInMs_U32, 0);
    //	EXPECT_EQ(pThread_X[i_U32].ThreadFunction, S_TheThread);
#if defined(_WIN32)
    EXPECT_EQ(pThread_X[i_U32].ThreadId, 0);
    EXPECT_EQ(pThread_X[i_U32].pThread, nullptr);
#else
    EXPECT_EQ(pThread_X[i_U32].ThreadId, 0);
#endif
    EXPECT_FALSE(pThread_X[i_U32].ThreadMustStop_B);
    EXPECT_FALSE(pThread_X[i_U32].ThreadRunning_B);
    EXPECT_EQ(pThread_X[i_U32].pUserContext, &pThreadContext_X[i_U32]);

    Sts_E = Bof_GetMemoryState(AvailableFreeMemory_U64, TotalMemorySize_U64);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_TRUE(AvailableFreeMemory_U64 < TotalMemorySize_U64);
    //	printf("Start thread %llx/%llx\n", AvailableFreeMemory_U64, TotalMemorySize_U64);
    Sts_E = Bof_StartThread(pThread_X[i_U32], 4096, 0, BOF_THREAD_SCHEDULER_POLICY_OTHER, MidPriority_E, STARTSTOPTO);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    if (i_U32) // Don't know why but with gdb/qemu first check is false. (Ok if app started in terminal with no debugger)
    {
      EXPECT_EQ(pThread_X[i_U32].ThreadCpuCoreAffinity_U32, 0);
      EXPECT_EQ(pThread_X[i_U32].Magic_U32, BOF_THREAD_MAGIC);
      EXPECT_STREQ(pThread_X[i_U32].Name_S.c_str(), Name_S.c_str());
      EXPECT_EQ(pThread_X[i_U32].ThreadSchedulerPolicy_E, BOF_THREAD_SCHEDULER_POLICY_OTHER);
      EXPECT_EQ(pThread_X[i_U32].ThreadPriority_E, MidPriority_E);
      EXPECT_EQ(pThread_X[i_U32].StackSize_U32, 4096);
      EXPECT_EQ(pThread_X[i_U32].StartStopTimeoutInMs_U32, STARTSTOPTO);
      //	EXPECT_EQ(Thread_X.ThreadFunction, S_TheThread);
#if defined(_WIN32)
      EXPECT_TRUE(pThread_X[i_U32].ThreadId != 0);
      EXPECT_FALSE(pThread_X[i_U32].pThread == nullptr);
#else
      EXPECT_NE(pThread_X[i_U32].ThreadId, 0);
#endif
      EXPECT_FALSE(pThread_X[i_U32].ThreadMustStop_B);
      EXPECT_TRUE(pThread_X[i_U32].ThreadRunning_B);
      EXPECT_EQ(pThread_X[i_U32].pUserContext, &pThreadContext_X[i_U32]);
    }
  }

  Bof_MsSleep(500);
  EXPECT_EQ(S_NbThread_U32, NBTHREAD);

  // Last_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NBTHREAD; i_U32++)
  {
    //		Now_U32 = Bof_GetMsTickCount();
    //		printf("[%06d] T %d D %d ->Signal thread\n", i_U32, Now_U32, Now_U32-Last_U32);
    //		Last_U32 = Now_U32;
    if ((i_U32) && !(i_U32 % PRTPERLOOP))
    {
      //			Now_U32 = Bof_GetMsTickCount();
      //			printf("T %d D %d Delete thread %d/%d\n", Now_U32, Now_U32 - Last_U32, i_U32, NBTHREAD);
      //			Last_U32 = Now_U32;
    }
    Name_S = "MyEvent_" + std::to_string(i_U32);
    Sts_E = Bof_SignalEvent(pThreadContext_X[i_U32].Event_X, 0);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(pThreadContext_X[i_U32].Event_X.Magic_U32, BOF_EVENT_MAGIC);
    EXPECT_STREQ(pThreadContext_X[i_U32].Event_X.Name_S.c_str(), Name_S.c_str());
    //		EXPECT_FALSE(pThreadContext_X[i_U32].Event_X.Signaled_B);

    //		Now_U32 = Bof_GetMsTickCount();
    //		printf("[%06d] T %d D %d ->Destroy thread start\n", i_U32, Now_U32, Now_U32 - Last_U32);
    //		Last_U32 = Now_U32;
    Sts_E = Bof_StopThread(pThread_X[i_U32]);

    //		Now_U32 = Bof_GetMsTickCount();
    //		if ((Now_U32 - Last_U32) > 200)
    {
      //			printf("[%06d] T %d D %d ->Destroy thread end\n", i_U32, Now_U32, Now_U32 - Last_U32);
      //			Last_U32 = Now_U32;
    }
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(pThread_X[i_U32].Magic_U32, BOF_THREAD_MAGIC);
#if defined(_WIN32)
//		EXPECT_TRUE(pThread_X[i_U32].ThreadId_U32 == 0);
//		EXPECT_TRUE(pThread_X[i_U32].pThread == nullptr);
#else
    //		EXPECT_EQ(pThread_X[i_U32].ThreadId, 0);
#endif
    EXPECT_TRUE(pThread_X[i_U32].ThreadMustStop_B);
    EXPECT_FALSE(pThread_X[i_U32].ThreadRunning_B);
  }
  Bof_MsSleep(500);
  EXPECT_EQ(S_NbThread_U32, 0);
  Sts_E = Bof_DestroyMutex(S_Mtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

TEST(Threading_Test, MultiThreadWithoutMutex)
{
  BOFERR Sts_E;
  int32_t Start_U32, Delta_U32;
  uint32_t i_U32;
  // uint32_t Now_U32, Last_U32;
  // uint64_t AvailableFreeMemory_U64, TotalMemorySize_U64;
  THREAD_CONTEXT pThreadContext_X[NBTHREAD];
  BOF_THREAD pThread_X[NBTHREAD];
  std::string Name_S;
  //	bool Sts_B;

  // Sts_E = Bof_CreateMutex("MyMutex", true, S_Mtx_X);
  // EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  S_NbThread_U32 = 0;
  memset(S_pValueToProtect_U8, 1, sizeof(S_pValueToProtect_U8));

  for (i_U32 = 0; i_U32 < NBTHREAD; i_U32++)
  {
    pThreadContext_X[i_U32].Reset();
    pThreadContext_X[i_U32].Id_U32 = i_U32;
    pThreadContext_X[i_U32].Inc_S32 = Bof_Random(true, 10, 100);
    pThreadContext_X[i_U32].SleepInMs_U32 = 0;
    pThreadContext_X[i_U32].NbLoop_U32 = 10;
    Name_S = "MyThread_" + std::to_string(i_U32);
    Sts_E = Bof_CreateThread(Name_S, S_TheThread, &pThreadContext_X[i_U32], pThread_X[i_U32]);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(pThread_X[i_U32].ThreadCpuCoreAffinity_U32, 0);
    EXPECT_EQ(pThread_X[i_U32].Magic_U32, BOF_THREAD_MAGIC);
    EXPECT_STREQ(pThread_X[i_U32].Name_S.c_str(), Name_S.c_str());
    EXPECT_EQ(pThread_X[i_U32].ThreadSchedulerPolicy_E, BOF_THREAD_SCHEDULER_POLICY_OTHER);
    EXPECT_EQ(pThread_X[i_U32].ThreadPriority_E, BOF_THREAD_PRIORITY_000);
    EXPECT_EQ(pThread_X[i_U32].StackSize_U32, 0);
    EXPECT_EQ(pThread_X[i_U32].StartStopTimeoutInMs_U32, 0);
#if defined(_WIN32)
    EXPECT_EQ(pThread_X[i_U32].ThreadId, 0);
    EXPECT_EQ(pThread_X[i_U32].pThread, nullptr);
#else
    EXPECT_EQ(pThread_X[i_U32].ThreadId, 0);
#endif
    EXPECT_FALSE(pThread_X[i_U32].ThreadMustStop_B);
    EXPECT_FALSE(pThread_X[i_U32].ThreadRunning_B);
    EXPECT_EQ(pThread_X[i_U32].pUserContext, &pThreadContext_X[i_U32]);

    // printf("%d: DBG magic %d:%zd\n", Bof_GetMsTickCount(), i_U32, pThread_X[i_U32].Magic_U32);

    Sts_E = Bof_StartThread(pThread_X[i_U32], 4096, 0, BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF_THREAD_PRIORITY_000, STARTSTOPTO);
    // printf("%d: DBG CHK\n", Bof_GetMsTickCount());
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    if (i_U32) // Don't know why but with gdb/qemu first check is false. (Ok if app started in terminal with no debugger)
    {
      EXPECT_EQ(pThread_X[i_U32].ThreadCpuCoreAffinity_U32, 0);
      EXPECT_EQ(pThread_X[i_U32].Magic_U32, BOF_THREAD_MAGIC);
      EXPECT_STREQ(pThread_X[i_U32].Name_S.c_str(), Name_S.c_str());
      EXPECT_EQ(pThread_X[i_U32].ThreadSchedulerPolicy_E, BOF_THREAD_SCHEDULER_POLICY_OTHER);
      EXPECT_EQ(pThread_X[i_U32].StackSize_U32, 4096);
      EXPECT_EQ(pThread_X[i_U32].StartStopTimeoutInMs_U32, STARTSTOPTO);
#if defined(_WIN32)
      EXPECT_TRUE(pThread_X[i_U32].ThreadId != 0);
      EXPECT_FALSE(pThread_X[i_U32].pThread == nullptr);
#else
      EXPECT_NE(pThread_X[i_U32].ThreadId, 0);
#endif
      EXPECT_FALSE(pThread_X[i_U32].ThreadMustStop_B);
      EXPECT_TRUE(pThread_X[i_U32].ThreadRunning_B);
      EXPECT_EQ(pThread_X[i_U32].pUserContext, &pThreadContext_X[i_U32]);
    }
  }
  Start_U32 = Bof_GetMsTickCount();
  do
  {
    Bof_MsSleep(100);
    Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  } while ((S_NbThread_U32) && (Delta_U32 < 5000));

  EXPECT_EQ(S_NbThread_U32, 0);
  //	Sts_B = true;
  for (i_U32 = 1; i_U32 < sizeof(S_pValueToProtect_U8); i_U32++)
  {
    if (S_pValueToProtect_U8[i_U32 - 1] != S_pValueToProtect_U8[i_U32])
    {
      //		Sts_B = false;
      break;
    }
  }
  //	EXPECT_FALSE(Sts_B);		// TODO !!
  // Sts_E = Bof_DestroyMutex(S_Mtx_X);
  // EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

TEST(Threading_Test, MultiThreadWithMutex)
{
  BOFERR Sts_E;
  uint32_t Start_U32, Delta_U32;
  uint32_t i_U32;
  //	uint32_t Now_U32, Last_U32;
  //	uint64_t AvailableFreeMemory_U64, TotalMemorySize_U64;
  THREAD_CONTEXT pThreadContext_X[NBTHREAD];
  BOF_THREAD pThread_X[NBTHREAD];
  std::string Name_S;
  bool Sts_B;

  S_NbThread_U32 = 0;
  memset(S_pValueToProtect_U8, 1, sizeof(S_pValueToProtect_U8));

  Sts_E = Bof_CreateMutex("MyMutex", false, false, S_Mtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  for (i_U32 = 0; i_U32 < NBTHREAD; i_U32++)
  {
    pThreadContext_X[i_U32].Reset();
    pThreadContext_X[i_U32].Id_U32 = i_U32;
    pThreadContext_X[i_U32].Inc_S32 = Bof_Random(true, 10, 100);
    pThreadContext_X[i_U32].SleepInMs_U32 = 0;
    pThreadContext_X[i_U32].NbLoop_U32 = 1;
    Name_S = "MyThread_" + std::to_string(i_U32);
    Sts_E = Bof_CreateThread(Name_S, S_TheThread, &pThreadContext_X[i_U32], pThread_X[i_U32]);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(pThread_X[i_U32].ThreadCpuCoreAffinity_U32, 0);
    EXPECT_EQ(pThread_X[i_U32].Magic_U32, BOF_THREAD_MAGIC);
    EXPECT_STREQ(pThread_X[i_U32].Name_S.c_str(), Name_S.c_str());
    EXPECT_EQ(pThread_X[i_U32].ThreadSchedulerPolicy_E, BOF_THREAD_SCHEDULER_POLICY_OTHER);
    EXPECT_EQ(pThread_X[i_U32].ThreadPriority_E, BOF_THREAD_PRIORITY_000);
    EXPECT_EQ(pThread_X[i_U32].StackSize_U32, 0);
    EXPECT_EQ(pThread_X[i_U32].StartStopTimeoutInMs_U32, 0);
#if defined(_WIN32)
    EXPECT_EQ(pThread_X[i_U32].ThreadId, 0);
    EXPECT_EQ(pThread_X[i_U32].pThread, nullptr);
#else
    EXPECT_EQ(pThread_X[i_U32].ThreadId, 0);
#endif
    EXPECT_FALSE(pThread_X[i_U32].ThreadMustStop_B);
    EXPECT_FALSE(pThread_X[i_U32].ThreadRunning_B);
    EXPECT_EQ(pThread_X[i_U32].pUserContext, &pThreadContext_X[i_U32]);

    Sts_E = Bof_StartThread(pThread_X[i_U32], 4096, 0, BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF_THREAD_PRIORITY_000, STARTSTOPTO);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    if (i_U32) // Don't know why but with gdb/qemu first check is false. (Ok if app started in terminal with no debugger)
    {

      EXPECT_EQ(pThread_X[i_U32].ThreadCpuCoreAffinity_U32, 0);
      EXPECT_EQ(pThread_X[i_U32].Magic_U32, BOF_THREAD_MAGIC);
      EXPECT_STREQ(pThread_X[i_U32].Name_S.c_str(), Name_S.c_str());
      EXPECT_EQ(pThread_X[i_U32].ThreadSchedulerPolicy_E, BOF_THREAD_SCHEDULER_POLICY_OTHER);
      EXPECT_EQ(pThread_X[i_U32].StackSize_U32, 4096);
      EXPECT_EQ(pThread_X[i_U32].StartStopTimeoutInMs_U32, STARTSTOPTO);
#if defined(_WIN32)
      EXPECT_TRUE(pThread_X[i_U32].ThreadId != 0);
      EXPECT_FALSE(pThread_X[i_U32].pThread == nullptr);
#else
      EXPECT_NE(pThread_X[i_U32].ThreadId, 0);
#endif
      EXPECT_FALSE(pThread_X[i_U32].ThreadMustStop_B);
      EXPECT_TRUE(pThread_X[i_U32].ThreadRunning_B);
      EXPECT_EQ(pThread_X[i_U32].pUserContext, &pThreadContext_X[i_U32]);
    }
  }
  Start_U32 = Bof_GetMsTickCount();
  do
  {
    Bof_MsSleep(100);
    Delta_U32 = Bof_ElapsedMsTime(Start_U32);
    if (S_NbThread_U32 == 0)
      break;
  } while ((S_NbThread_U32) && (Delta_U32 < (NBTHREAD * 100)));

  EXPECT_EQ(S_NbThread_U32, 0);
  Sts_B = true;
  for (i_U32 = 1; i_U32 < sizeof(S_pValueToProtect_U8); i_U32++)
  {
    if (S_pValueToProtect_U8[i_U32 - 1] != S_pValueToProtect_U8[i_U32])
    {
      Sts_B = false;
      break;
    }
  }
  EXPECT_TRUE(Sts_B);
  Sts_E = Bof_DestroyMutex(S_Mtx_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

const uint32_t SEMMAXVAL = 10;

TEST(Threading_Test, Semaphore)
{
  BOFERR Sts_E;
  BOF_SEMAPHORE Sem_X;
  uint32_t i_U32, Start_U32, Delta_U32;

  EXPECT_EQ(Sem_X.Magic_U32, 0);
  EXPECT_EQ(Sem_X.Name_S, "");
  EXPECT_EQ(Sem_X.Cpt_S32, 0);
  EXPECT_EQ(Sem_X.Max_S32, 0);

  Sts_E = Bof_CreateSemaphore("MySem", SEMMAXVAL, Sem_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Sem_X.Magic_U32, BOF_SEMAPHORE_MAGIC);
  EXPECT_EQ(Sem_X.Name_S, "MySem");
  EXPECT_EQ(Sem_X.Cpt_S32, SEMMAXVAL - 1);
  EXPECT_EQ(Sem_X.Max_S32, SEMMAXVAL - 1);
  for (i_U32 = 0; i_U32 < SEMMAXVAL; i_U32++)
  {
    Start_U32 = Bof_GetMsTickCount();
    Sts_E = Bof_WaitForSemaphore(Sem_X, 1000);
    Delta_U32 = Bof_ElapsedMsTime(Start_U32);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_LE(Delta_U32, static_cast<uint32_t>(500));
    EXPECT_EQ(Sem_X.Magic_U32, BOF_SEMAPHORE_MAGIC);
    EXPECT_EQ(Sem_X.Name_S, "MySem");
    EXPECT_EQ(Sem_X.Cpt_S32, SEMMAXVAL - 1 - i_U32 - 1);
    EXPECT_EQ(Sem_X.Max_S32, SEMMAXVAL - 1);
  }

  Start_U32 = Bof_GetMsTickCount();
  Sts_E = Bof_WaitForSemaphore(Sem_X, 1000);
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_GE(Delta_U32, static_cast<uint32_t>(1000));
  EXPECT_EQ(Sem_X.Magic_U32, BOF_SEMAPHORE_MAGIC);
  EXPECT_EQ(Sem_X.Name_S, "MySem");
  EXPECT_EQ(Sem_X.Cpt_S32, -1);
  EXPECT_EQ(Sem_X.Max_S32, SEMMAXVAL - 1);

  Start_U32 = Bof_GetMsTickCount();
  Sts_E = Bof_WaitForSemaphore(Sem_X, 1000);
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_GE(Delta_U32, static_cast<uint32_t>(1000));
  EXPECT_EQ(Sem_X.Magic_U32, BOF_SEMAPHORE_MAGIC);
  EXPECT_EQ(Sem_X.Name_S, "MySem");
  EXPECT_EQ(Sem_X.Cpt_S32, -1);
  EXPECT_EQ(Sem_X.Max_S32, SEMMAXVAL - 1);

  for (i_U32 = 0; i_U32 < SEMMAXVAL; i_U32++)
  {
    Sts_E = Bof_SignalSemaphore(Sem_X);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(Sem_X.Magic_U32, BOF_SEMAPHORE_MAGIC);
    EXPECT_EQ(Sem_X.Name_S, "MySem");
    EXPECT_EQ(Sem_X.Cpt_S32, i_U32);
    EXPECT_EQ(Sem_X.Max_S32, SEMMAXVAL - 1);
  }
  Sts_E = Bof_SignalSemaphore(Sem_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Sem_X.Magic_U32, BOF_SEMAPHORE_MAGIC);
  EXPECT_EQ(Sem_X.Name_S, "MySem");
  EXPECT_EQ(Sem_X.Cpt_S32, SEMMAXVAL - 1);
  EXPECT_EQ(Sem_X.Max_S32, SEMMAXVAL - 1);

  Sts_E = Bof_DestroySemaphore(Sem_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Sem_X.Magic_U32, 0);
  EXPECT_EQ(Sem_X.Name_S, "");
  EXPECT_EQ(Sem_X.Cpt_S32, 0);
  EXPECT_EQ(Sem_X.Max_S32, 0);
}

const uint32_t SHRSIZE = 0x10000;
TEST(Threading_Test, SharedMemory)
{
  BOFERR Sts_E;
  BOF_SHARED_MEMORY ShrMem_X, AnotherShrMem_X, YetAnotherShrMem_X;
  uint32_t i_U32, *pVal_U32, pCpyVal_U32[128];

  EXPECT_EQ(ShrMem_X.Magic_U32, 0);
  EXPECT_STREQ(ShrMem_X.Name_S.c_str(), "");
#if defined(_WIN32)
  EXPECT_EQ(ShrMem_X.pHandle, nullptr);
#else
#endif
  EXPECT_EQ(ShrMem_X.pBaseAddress, nullptr);
  EXPECT_NE(Bof_CloseSharedMemory(ShrMem_X, false), BOF_ERR_NO_ERROR);

  Sts_E = Bof_OpenSharedMemory("MyShr", SHRSIZE, BOF_ACCESS_TYPE::BOF_ACCESS_READ | BOF_ACCESS_TYPE::BOF_ACCESS_WRITE, "/tmp", BOF_INVALID_HANDLE_VALUE, ShrMem_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(ShrMem_X.Magic_U32, BOF_FILEMAPPING_MAGIC);
  EXPECT_STREQ(ShrMem_X.Name_S.c_str(), "MyShr");
#if defined(_WIN32)
  EXPECT_NE(ShrMem_X.pHandle, nullptr);
#else
#endif
  EXPECT_NE(ShrMem_X.pBaseAddress, nullptr);
  pVal_U32 = static_cast<uint32_t *>(ShrMem_X.pBaseAddress);
  if (pVal_U32)
  {
    for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pCpyVal_U32); i_U32++)
    {
      pVal_U32[i_U32] = i_U32;
    }
    for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pCpyVal_U32); i_U32++)
    {
      pCpyVal_U32[i_U32] = pVal_U32[i_U32];
      EXPECT_EQ(pCpyVal_U32[i_U32], i_U32);
    }
  }
  Sts_E = Bof_OpenSharedMemory("MyShr", SHRSIZE, BOF_ACCESS_TYPE::BOF_ACCESS_READ | BOF_ACCESS_TYPE::BOF_ACCESS_WRITE, "/tmp", BOF_INVALID_HANDLE_VALUE, AnotherShrMem_X);
  EXPECT_EQ(Sts_E, BOF_ERR_EEXIST);
  EXPECT_EQ(AnotherShrMem_X.Magic_U32, BOF_FILEMAPPING_MAGIC);
  EXPECT_STREQ(AnotherShrMem_X.Name_S.c_str(), "MyShr");
#if defined(_WIN32)
  EXPECT_NE(ShrMem_X.pHandle, nullptr);
#else
#endif
  EXPECT_NE(AnotherShrMem_X.pBaseAddress, nullptr);
  pVal_U32 = static_cast<uint32_t *>(AnotherShrMem_X.pBaseAddress);

  if (pVal_U32)
  {
    for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pCpyVal_U32); i_U32++)
    {
      EXPECT_EQ(pVal_U32[i_U32], i_U32);
    }
    for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pCpyVal_U32); i_U32++)
    {
      pVal_U32[i_U32] = i_U32 * 2;
    }
  }
  Sts_E = Bof_CloseSharedMemory(AnotherShrMem_X, false);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(AnotherShrMem_X.Magic_U32, BOF_FILEMAPPING_MAGIC);
  EXPECT_STREQ(AnotherShrMem_X.Name_S.c_str(), "");
#if defined(_WIN32)
  EXPECT_EQ(AnotherShrMem_X.pHandle, nullptr);
#else
#endif
  EXPECT_EQ(AnotherShrMem_X.pBaseAddress, nullptr);

  Sts_E = Bof_OpenSharedMemory("MyShr", SHRSIZE, BOF_ACCESS_TYPE::BOF_ACCESS_WRITE, "/tmp", BOF_INVALID_HANDLE_VALUE, YetAnotherShrMem_X);
  EXPECT_EQ(Sts_E, BOF_ERR_EEXIST);
  EXPECT_EQ(YetAnotherShrMem_X.Magic_U32, BOF_FILEMAPPING_MAGIC);
  EXPECT_STREQ(YetAnotherShrMem_X.Name_S.c_str(), "MyShr");
#if defined(_WIN32)
  EXPECT_NE(YetAnotherShrMem_X.pHandle, nullptr);
#else
#endif
  EXPECT_NE(YetAnotherShrMem_X.pBaseAddress, nullptr);
  pVal_U32 = static_cast<uint32_t *>(YetAnotherShrMem_X.pBaseAddress);
  if (pVal_U32)
  {
    for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pCpyVal_U32); i_U32++)
    {
      EXPECT_EQ(pVal_U32[i_U32], i_U32 * 2);
    }
    for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pCpyVal_U32); i_U32++)
    {
      pVal_U32[i_U32] = i_U32 * 3;
    }
  }
  Sts_E = Bof_CloseSharedMemory(YetAnotherShrMem_X, false);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CloseSharedMemory(ShrMem_X, true);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

TEST(Threading_Test, CriticalSection)
{
  BOFERR Sts_E;
  BOF_CONDITIONAL_VARIABLE Cv_X;

  Sts_E = Bof_CreateConditionalVariable("", false, Cv_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  bool Signaled_B = false;
  int Val_i = 0;
  BofCvSetter<bool, int> CvSetter = [&](bool _On_B, int _Val_i) {
    Signaled_B = _On_B;
    Val_i = _Val_i;
  };
  BofCvPredicateAndReset<> CvPredicateAndReset = [&]() -> bool {
    if (Signaled_B)
    {
      Val_i = 7;
      Signaled_B = false;
      return true;
    }
    return (false);
  };

  Sts_E = Bof_WaitForConditionalVariable(Cv_X, 200, CvPredicateAndReset);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_SignalConditionalVariable(Cv_X, CvSetter, true, 3);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Signaled_B, true);
  EXPECT_EQ(Val_i, 3);

  Sts_E = Bof_WaitForConditionalVariable(Cv_X, 200, CvPredicateAndReset);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Signaled_B, false);
  EXPECT_EQ(Val_i, 7);

  Sts_E = Bof_WaitForConditionalVariable(Cv_X, 200, CvPredicateAndReset);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Signaled_B, false);
  EXPECT_EQ(Val_i, 7);

  Sts_E = Bof_DestroyConditionalVariable(Cv_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_SignalConditionalVariable(Cv_X, CvSetter, true, 7);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Signaled_B, false);
  EXPECT_EQ(Val_i, 7);
}

class Channel
{
private:
  BOF::BofCvSetter<bool> mChannelOpenedCvSetter = [&](bool _ChannelOpened_B) { mChannelOpened_B = _ChannelOpened_B; };
  BOF::BofCvPredicateAndReset<> mChannelOpenedCvPredicateAndReset = [&]() -> bool { return mChannelOpened_B; };
  bool mChannelOpened_B = false;
  BOF::BOF_CONDITIONAL_VARIABLE mChannelOpenedCv_X;

public:
  Channel()
  {
    BOF_ASSERT(BOF::Bof_CreateConditionalVariable("ChannelName", true, mChannelOpenedCv_X) == BOF_ERR_NO_ERROR);
  }

  ~Channel()
  {
    BOF_ASSERT(BOF::Bof_DestroyConditionalVariable(mChannelOpenedCv_X) == BOF_ERR_NO_ERROR);
  }

  BOFERR OpenChannel()
  {
    BOFERR Rts_E = BOF_ERR_ALREADY_OPENED;

    if (!mChannelOpened_B)
    {
      BOF::Bof_SignalConditionalVariable(mChannelOpenedCv_X, mChannelOpenedCvSetter, true);
      Rts_E = BOF_ERR_NO_ERROR;
    }
    return Rts_E;
  }

  BOFERR CloseChannel()
  {
    BOFERR Rts_E = BOF_ERR_NOT_OPENED;

    if (mChannelOpened_B)
    {
      BOF::Bof_SignalConditionalVariable(mChannelOpenedCv_X, mChannelOpenedCvSetter, false);
      Rts_E = BOF_ERR_NO_ERROR;
    }
    return Rts_E;
  }

  bool IsChannelOpened() const
  {
    return mChannelOpened_B;
  }

  bool WaitForChannelOpened(uint32_t _TimeoutInMs_U32)
  {
    bool Rts_B = mChannelOpened_B;

    if (!Rts_B)
    {
      Rts_B = (BOF::Bof_WaitForConditionalVariable(mChannelOpenedCv_X, _TimeoutInMs_U32, mChannelOpenedCvPredicateAndReset) == BOF_ERR_NO_ERROR);
    }
    return Rts_B;
  }
};

TEST(Threading_Test, Cv)
{
  Channel Chnl;
  uint32_t Start_U32, Delta_U32;
  constexpr uint32_t CVTO = 500;

  EXPECT_FALSE(Chnl.IsChannelOpened());
  Start_U32 = Bof_GetMsTickCount();
  EXPECT_FALSE(Chnl.WaitForChannelOpened(CVTO));
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, CVTO - 20);

  EXPECT_EQ(Chnl.OpenChannel(), BOF_ERR_NO_ERROR);
  EXPECT_TRUE(Chnl.IsChannelOpened());
  Start_U32 = Bof_GetMsTickCount();
  EXPECT_TRUE(Chnl.WaitForChannelOpened(CVTO));
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_LE((int)Delta_U32, 20);

  EXPECT_EQ(Chnl.CloseChannel(), BOF_ERR_NO_ERROR);
  EXPECT_FALSE(Chnl.IsChannelOpened());
  Start_U32 = Bof_GetMsTickCount();
  EXPECT_FALSE(Chnl.WaitForChannelOpened(CVTO));
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, CVTO - 20);
}

TEST(Threading_Test, ThreadParameterFromString)
{
  BOF_THREAD_PARSER_PARAM ThreadParserParam_X;
  std::string Str_S;

  EXPECT_NE(BofThread::S_ThreadParserParamFromString("", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_NE(BofThread::S_ThreadParserParamFromString("z", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_NE(BofThread::S_ThreadParserParamFromString("z2", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_NE(BofThread::S_ThreadParserParamFromString("n", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_NE(BofThread::S_ThreadParserParamFromString("na", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_NE(BofThread::S_ThreadParserParamFromString(":C1", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_NE(BofThread::S_ThreadParserParamFromString("n0:o50,c0-0", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_NE(BofThread::S_ThreadParserParamFromString("n0:o50:c2-1", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_NE(BofThread::S_ThreadParserParamFromString("n0:o50:c0;0", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_NE(BofThread::S_ThreadParserParamFromString("n0:g50:c0-0", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_NE(BofThread::S_ThreadParserParamFromString("n0:o:c0-0", ThreadParserParam_X), BOF_ERR_NO_ERROR);

  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f30:c13,11,12,10,2-4,29,1-2,15,30-64", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_TRUE(ThreadParserParam_X.NbActiveCore_U32 != 0);
  EXPECT_EQ(ThreadParserParam_X.Node_U32, 1);
  EXPECT_EQ(ThreadParserParam_X.AffinityCpuSet_U64, 0xFFFFFFFFE000BC1E);
  EXPECT_EQ(ThreadParserParam_X.CoreChosen_U32, 1);
  EXPECT_EQ(ThreadParserParam_X.ThreadSchedulerPolicy_E, BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_FIFO);

  Str_S = BofThread::S_ToString(ThreadParserParam_X, true);
  Str_S = Str_S.substr(0, Str_S.find('/', 0));
  EXPECT_EQ(ThreadParserParam_X.ThreadPriority_E, BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_030);
  EXPECT_STREQ(Str_S.c_str(), "n1:f30:c1-4,10-13,15,29-63 A1");

  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f60:c0:f55:c2", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:o-1:c2", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f70:c3", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:c0:o-1", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f60:c4", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f80:c0", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f80:c4", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f80:c5", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f70:c8", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f70:c8", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f70:c9", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f20:c7", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f70:c6", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f69:c6", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f90:c0", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f70:c0", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f1:c6", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f10:c0", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f70:c0", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f69:c0", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f60:c0", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f80:c2", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f5:c2", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f65:c2", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f1:c6", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f1:c8", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f73:c14", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f72:c14", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f70:c14", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f70:c14", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f0:c14-15", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f30:c10,11,12,13", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n1:f30:c10,11,12,13", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f92:c14", ThreadParserParam_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofThread::S_ThreadParserParamFromString("n0:f91:c15", ThreadParserParam_X), BOF_ERR_NO_ERROR);
}
