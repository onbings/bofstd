/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines routines for creating and managing a thread. It also
 * manage and control an event and a critical section which can be used by
 * child objects:
 * - GetSignalEvent
 * - GetCriticalSection
 * - LockCriticalSection
 * - UnlockCriticalSection
 *
 * Name:        BofThread.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Mar 11 2002  BHA : Initial release
 */
#pragma once

#include <bofstd/bofsystem.h>
#include <bofstd/bofpot.h>

#include <thread>
#include <vector>

BEGIN_BOF_NAMESPACE()
struct BOF_THREAD_PARAM
{
  std::string Name_S;
  bool SignalEvent_B;
  uint32_t WakeUpIntervalInMs_U32;
  BOF_THREAD_SCHEDULER_POLICY ThreadSchedulerPolicy_E;
  BOF_THREAD_PRIORITY ThreadPriority_E;
  uint64_t ThreadCpuCoreAffinityMask_U64;
  uint32_t StartStopTimeoutInMs_U32;
  uint32_t StackSize_U32;

  BOF_THREAD_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    Name_S="";
    SignalEvent_B=false;
    WakeUpIntervalInMs_U32=0;
    ThreadSchedulerPolicy_E= BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_MAX;
    ThreadPriority_E= BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000;
    ThreadCpuCoreAffinityMask_U64=0;
    StartStopTimeoutInMs_U32=0;
    StackSize_U32=0;
  }
};

struct BOF_THREAD_PARSER_PARAM
{
  // bool SpreadCpuCore_B;
  // std::string ThreadParamToString_S
  uint32_t NbActiveCore_U32;
  uint32_t Node_U32;
  uint64_t AffinityCpuSet_U64;
  uint32_t CoreChosen_U32;
  BOF_THREAD_SCHEDULER_POLICY ThreadSchedulerPolicy_E;
  BOF_THREAD_PRIORITY ThreadPriority_E;

  BOF_THREAD_PARSER_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    // SpreadCpuCore_B=false;
    // ThreadParamToString_S="";
    NbActiveCore_U32 = 0;
    Node_U32 = 0;
    AffinityCpuSet_U64 = 0;
    CoreChosen_U32 = 0;
    ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_MAX;
    ThreadPriority_E = BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000;
  }
};

// Used to specify callback if the caller does not inherit from BofThread->Setup with SetThreadCallback
typedef std::function<BOFERR()> BOF_THREAD_CALLBACK;

/*!
 * Summary
 * BofThread class
 *
 * Description
 * This class encapsulates the Windows Thread functions and provide an object
 * view of the thread concept.
 *
 * See Also
 * See thread Microsoft MSDN documentation for more details
 */

class BOFSTD_EXPORT BofThread
{
private:
  BOF_THREAD_PARAM mThreadParam_X;
  BOFERR mThreadErrorCode_E = BOF_ERR_NO_ERROR;
  std::thread mThread;
  std::thread::native_handle_type mThreadHandle;
  BOF_MUTEX mThreadMtx_X;
  std::atomic<int32_t> mLockBalance;
  char mpLastLocker_c[64];
  BOF_EVENT mThreadEnterEvent_X;
  BOF_EVENT mThreadExitEvent_X;
  BOF_EVENT mWakeUpEvent_X;
  /*
  std::string mName_S = "";
  uint32_t mStartStopTimeoutInMs_U32 = 0;
  uint32_t mWakeUpIntervalInMs_U32 = 0;
  uint64_t mCpuCoreAffinityMask_U64 = 0;
  BOF_THREAD_SCHEDULER_POLICY mPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  BOF_THREAD_PRIORITY mPriority_E = BOF_THREAD_PRIORITY_000;
  */
  std::atomic<bool> mThreadMustStop_B = false;

  BOF_THREAD_CALLBACK mOnCreate = nullptr; // Used to specify callback if the caller does not inherit from BofThread->Setup with SetThreadCallback
  BOF_THREAD_CALLBACK mOnProcessing = nullptr;
  BOF_THREAD_CALLBACK mOnStop = nullptr;

public:
#if 0
#if defined(_WIN32)
  void *GetThreadHandle();
#else
  pthread_t            GetThreadHandle();
#endif
#endif

  BofThread();

  //	BofThread(const std::string &_rName_S);

  virtual ~BofThread();
  BOFERR InitThreadErrorCode();
  BofThread &operator=(const BofThread &) = delete; // Disallow copying
  BofThread(const BofThread &) = delete;
  BOFERR SignalThreadWakeUpEvent();
  BOFERR WaitForThreadWakeUpEvent(uint32_t _TimeoutInMs_U32);
  BOFERR LockThreadCriticalSection(const char *_pLocker_c);
  BOFERR UnlockThreadCriticalSection();
  const char *LockInfo(int32_t &_rLockBalance_S32) const;
  BOFERR SetThreadWakeUpInterval(uint32_t _WakeUpIntervalInMs_U32);
  bool IsThreadLoopMustExit();

  //!!! Do not call this method in an intermediate caller object constructor such as in class B or C constructor. You can put it in A
  //!!! class A:public B
  //!!! class B:public C
  //!!! If you do that you will receive "pure virtual method called" abort message as when you are in an intermediate constructor the virtual table is not ready
  BOFERR LaunchBofProcessingThread(const BOF_THREAD_PARAM &_rThreadParam_X);
  BOFERR LaunchBofProcessingThread(const std::string &_rName_S, bool _SignalEvent_B, uint32_t _WakeUpIntervalInMs_U32, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF_THREAD_PRIORITY _ThreadPriority_E, uint64_t _ThreadCpuCoreAffinityMask_U64,
                                   uint32_t _StartStopTimeoutInMs_U32, uint32_t _StackSize_U32);

  BOFERR DestroyBofProcessingThread(const char *_pUser_c);
  bool IsThreadRunning(uint32_t _Timeout_U32);
  void SetThreadCallback(BOF_THREAD_CALLBACK _OnCreate, BOF_THREAD_CALLBACK _OnProcessing, BOF_THREAD_CALLBACK _OnStop);

  virtual BOFERR V_OnCreate();
  virtual BOFERR V_OnProcessing();
  virtual BOFERR V_OnStop();
  std::string ThreadName() const;
  static std::string S_ToString(const BOF_THREAD_PARSER_PARAM &_rThreadParserParam_X, bool _ShowChosenCore_B);
  static BOFERR S_ThreadParserParamFromString(const char *_pThreadParameter_c, BOF_THREAD_PARSER_PARAM &_rThreadParserParam_X);
  static int S_BofThreadBalance();

private:
  static std::atomic<int32_t> S_mBofThreadBalance;
  void BofThread_Thread();

  BOFERR InitializeThread(const std::string &_rName_S);
  static BOFERR S_AffinityMaskFromString(const char *_pAffinityOptionString_c, uint32_t _NbCore_U32, uint64_t &_rAffinityMask_U32);
};

struct BOF_THREAD_POOL_ENTRY
{
  uint32_t MagicNumber_U32;       /*! Must be the first uint32_t var of pot element (see BofPot) */
  bool Running_B;
  BofThread *pBofThread;
  BOF_THREAD_CALLBACK FctToExec;
  BOF_THREAD_POOL_ENTRY()
  {
    Reset();
  }
  void Reset()
  {
    MagicNumber_U32 = 0;
    Running_B = false;
    pBofThread = nullptr;
    FctToExec = nullptr;
  }
};

class ThreadPoolExecutor;
class BOFSTD_EXPORT BofThreadPool
{
private:
  BOF_THREAD_PARAM mThreadParam_X;
  BOFERR mThreadPoolErrorCode_E = BOF_ERR_NO_ERROR;
  BOF::BofPot<BOF_THREAD_POOL_ENTRY> *mpThreadCollection = nullptr;
  std::mutex mMtxPendingDispatchCollection;
  std::vector<BOF_THREAD_POOL_ENTRY *> mPendingDispatchCollection;

public:
  BofThreadPool(uint32_t _NbThreadInPool_U32, const BOF_THREAD_PARAM &_rThreadParam_X);
  virtual ~BofThreadPool();
  BOFERR InitThreadPoolErrorCode();

  BOFERR Dispatch(uint32_t TimeoutInMs_U32, BOF_THREAD_CALLBACK _FctToExec, void **_ppDispatchTicket);
  BOFERR AckPendingDispatch(uint32_t _TimeoutInMs_U32, const void *_pDispatchTicket);
  std::string GetDispatchName(const void *_pDispatchTicket);

  uint32_t GetNumerOfPendingRunningDispatch();
  uint32_t GetNumerOfPendingDispatchToAck();
  void *GetFirstPendingDispatch(); //Next should call AckPendingDispatch if rts is not nullptr

  BOFERR ReleaseDispatch(BOFERR _Sts_E, BOF_THREAD_POOL_ENTRY *_pThreadPoolEntry_X, ThreadPoolExecutor *_pThreadPoolExecutor);   //Called internally, do not call this method (ThreadPoolExecutor is internal)
};

END_BOF_NAMESPACE()