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

/*** Include ****************************************************************/
#include <bofstd/bofsystem.h>
#include <bofstd/bofsocketos.h>
#include <thread>
#include <map>
BEGIN_BOF_NAMESPACE()
class BofSocket;
class BofSocketIo;
struct BOF_POLL_SOCKET;
struct BOF_POLL_SOCKET_CMD;

//Used to specify callback if the caller does not inherit from BofThread->Setup with SetThreadCallback
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

class BofThread
{
private:
  BOFERR mThreadErrorCode_E = BOF_ERR_NO_ERROR;
  std::thread mThread;
  std::string mName_S = "";
  BOF_MUTEX mMtx_X;
	std::atomic<int32_t> mLockBalance;
  char mpLastLocker_c[64];
	BOF_EVENT mThreadEnterEvent_X;
	BOF_EVENT mThreadExitEvent_X;
	BOF_EVENT mWakeUpEvent_X;

	uint32_t mStartStopTimeoutInMs_U32 = 0;
	uint32_t mWakeUpIntervalInMs_U32 = 0;
	uint32_t mCpuCoreAffinity_U32 = 0;
  BOF_THREAD_SCHEDULER_POLICY mPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  BOF_THREAD_PRIORITY mPriority_E = BOF_THREAD_DEFAULT_PRIORITY;
	bool mThreadLoopMustExit_B = false;
//		bool mThreadRunning_B = false;
	bool mThreadExitPosted_B = false;

	BOF_THREAD_CALLBACK mOnCreate = nullptr;  //Used to specify callback if the caller does not inherit from BofThread->Setup with SetThreadCallback
	BOF_THREAD_CALLBACK mOnProcessing = nullptr;
	BOF_THREAD_CALLBACK mOnStop = nullptr;
  uint32_t  mLoopTimerWarning_U32 = 0;

	std::unique_ptr<BofSocket> mpuPollControlListener = nullptr;
	std::unique_ptr<BofSocket> mpuPollControlSender = nullptr;
	std::shared_ptr<BofSocketIo> mpsPollControlReceiver = nullptr;
	uint32_t mPollControlListenerTimeoutInMs_U32 = 0;
	uint32_t							mNbMaxPollEntry_U32 = 0;
	std::atomic<uint32_t> mNbPollEntry;				//Current number of entries in mpPollerPram_X
	BOF_POLL_SOCKET				*mpPollOp_X = nullptr;
	std::map<BOFSOCKET, std::shared_ptr<BofSocketIo>> mSessionCollection;
public:

#if 0
#if defined (_WIN32)
		void                 *GetThreadHandle();
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
		BOFERR LaunchBofProcessingThread(const std::string &_rName_S, bool _SignalEvent_B, uint32_t _WakeUpIntervalInMs_U32, uint32_t _StackSize_U32, uint32_t _ThreadCpuCoreAffinity_U32, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E,BOF_THREAD_PRIORITY _ThreadPriority_E,  uint32_t _StartStopTimeoutInMs_U32);

		//_NbPollEntry_U32 must be at least 1 elem and entry 0 of _pPollOp_X must be free as it will be used to store ctrl poll entry
		BOFERR LaunchBofProcessingThreadWithIpcSocket(const std::string &_rName_S, bool _SignalEvent_B, uint32_t _WakeUpIntervalInMs_U32, uint32_t _StackSize_U32, uint32_t _ThreadCpuCoreAffinity_U32, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF_THREAD_PRIORITY _ThreadPriority_E, uint32_t _StartStopTimeoutInMs_U32, uint32_t _IpcControlListenerTimeoutInMs_U32, uint16_t _IpcControlListenerPort_U16, uint32_t _IpcControllSocketBufferSize_U32, uint32_t _NbMaxPollEntry_U32);
		BOFERR SendIpcCommand(BOF_POLL_SOCKET_CMD &_rPollSocketCommand_X);
		BOFERR IpcPoll(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPollSet_U32, BOF_POLL_SOCKET_CMD &_rPollSocketCmd_X);
		uint32_t NbIpcPollEntry();

		BOFERR DestroyBofProcessingThread(const char *_pUser_c);

		bool   IsThreadRunning();

		BOFERR PostThreatExit(const char *_pUser_c);

		void SetThreadCallback(BOF_THREAD_CALLBACK _OnCreate, BOF_THREAD_CALLBACK _OnProcessing, BOF_THREAD_CALLBACK _OnStop);

		virtual BOFERR V_OnCreate();

		virtual BOFERR V_OnProcessing();

		virtual BOFERR V_OnStop();

private:
		void BofThread_Thread();

		BOFERR InitializeThread(const std::string &_rName_S);
};

END_BOF_NAMESPACE()