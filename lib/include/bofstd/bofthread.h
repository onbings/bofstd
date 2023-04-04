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

#include <thread>

BEGIN_BOF_NAMESPACE()

struct BOF_THREAD_PARAM
{
  // bool SpreadCpuCore_B;
  // std::string ThreadParamToString_S
  uint32_t NbActiveCore_U32;
  uint32_t Node_U32;
  uint64_t AffinityCpuSet_U64;
  uint32_t CoreChosen_U32;
  BOF_THREAD_SCHEDULER_POLICY SchedulerPolicy_E;
  BOF_THREAD_PRIORITY Priority_E;

  BOF_THREAD_PARAM()
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
    SchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_MAX;
    Priority_E = BOF_THREAD_PRIORITY::BOF_THREAD_DEFAULT_PRIORITY;
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
  BOFERR mThreadErrorCode_E = BOF_ERR_NO_ERROR;
  std::thread mThread;
  std::thread::native_handle_type mThreadHandle;
  std::string mName_S = "";
  BOF_MUTEX mMtx_X;
  std::atomic<int32_t> mLockBalance;
  char mpLastLocker_c[64];
  BOF_EVENT mThreadEnterEvent_X;
  BOF_EVENT mThreadExitEvent_X;
  BOF_EVENT mWakeUpEvent_X;

  uint32_t mStartStopTimeoutInMs_U32 = 0;
  uint32_t mWakeUpIntervalInMs_U32 = 0;
  uint64_t mCpuCoreAffinityMask_U64 = 0;
  BOF_THREAD_SCHEDULER_POLICY mPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  BOF_THREAD_PRIORITY mPriority_E = BOF_THREAD_DEFAULT_PRIORITY;
  bool mThreadLoopMustExit_B = false;
  //		bool mThreadRunning_B = false;
  bool mThreadExitPosted_B = false;

  BOF_THREAD_CALLBACK mOnCreate = nullptr; // Used to specify callback if the caller does not inherit from BofThread->Setup with SetThreadCallback
  BOF_THREAD_CALLBACK mOnProcessing = nullptr;
  BOF_THREAD_CALLBACK mOnStop = nullptr;
  uint32_t mLoopTimerWarning_U32 = 0;

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
  BOFERR LaunchBofProcessingThread(const std::string &_rName_S, bool _SignalEvent_B, uint32_t _WakeUpIntervalInMs_U32, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF_THREAD_PRIORITY _ThreadPriority_E, uint64_t _ThreadCpuCoreAffinityMask_U64,
                                   uint32_t _StartStopTimeoutInMs_U32, uint32_t _StackSize_U32);
  BOFERR DestroyBofProcessingThread(const char *_pUser_c);
  bool IsThreadRunning();
  BOFERR PostThreatExit(const char *_pUser_c);
  void SetThreadCallback(BOF_THREAD_CALLBACK _OnCreate, BOF_THREAD_CALLBACK _OnProcessing, BOF_THREAD_CALLBACK _OnStop);

  virtual BOFERR V_OnCreate();
  virtual BOFERR V_OnProcessing();
  virtual BOFERR V_OnStop();
  static std::string S_ToString(const BOF_THREAD_PARAM &_rThreadParam_X, bool _ShowChosenCore_B);
  static BOFERR S_ThreadParameterFromString(const char *_pThreadParameter_c, BOF_THREAD_PARAM &_rThreadParam_X);

private:
  void BofThread_Thread();

  BOFERR InitializeThread(const std::string &_rName_S);
  static BOFERR S_AffinityMaskFromString(const char *_pAffinityOptionString_c, uint32_t _NbCore_U32, uint64_t &_rAffinityMask_U32);
};

END_BOF_NAMESPACE()