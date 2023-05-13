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
 * Name:        BofThread.cpp
 * Author:      Bernard HARMEL: b.harmel@belgacom.net
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Mar 11 2002  BHA : Initial release
 */

/*** Include files ***********************************************************/
#include <bofstd/bofsocketio.h>
#include <bofstd/bofthread.h>

#if defined(_WIN32)
// https://stackoverflow.com/questions/10121560/stdthread-naming-your-thread
#include <windows.h>
const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO
{
  DWORD dwType;     // Must be 0x1000.
  LPCSTR szName;    // Pointer to name (in user addr space).
  DWORD dwThreadID; // Thread ID (-1=caller thread).
  DWORD dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

#else

#endif
// #include <bofstd/boflogger.h>
// BofLogger::S_Instance().Log("IpSwitcherLog", BOF_NAMESPACE::BOF_LOG_CHANNEL_LEVEL::CRITICAL, "BofSocketIo[%d] DoINeedToConnect_B", BOF_NAMESPACE::Bof_GetMsTickCount());

BEGIN_BOF_NAMESPACE()
#if defined(_WIN32)
#else
#include <sys/syscall.h>
#include <unistd.h>

#endif

/*** Global variables ********************************************************/

/*** BofThread *****************************************************************/

BofThread::BofThread()
{
  mThreadErrorCode_E = InitializeThread("?");
  BOF_ASSERT(mThreadErrorCode_E == BOF_ERR_NO_ERROR);
}

BOFERR BofThread::InitializeThread(const std::string &_rName_S)
{
  mNbPollEntry.store(0);
  mName_S = _rName_S;
  mMtx_X.Reset();
  mLockBalance.store(0);
  mpLastLocker_c[0] = 0;
  mWakeUpEvent_X.Reset();
  mThreadErrorCode_E = BOF_ERR_ENOMEM;
  mThreadErrorCode_E = Bof_CreateMutex(_rName_S + "_mtx", false, false, mMtx_X);
  if (mThreadErrorCode_E == BOF_ERR_NO_ERROR)
  {
    mThreadErrorCode_E = Bof_CreateEvent(_rName_S + "_wakeup_evt", false, 1, false, mWakeUpEvent_X);
    if (mThreadErrorCode_E == BOF_ERR_NO_ERROR)
    {
      mThreadErrorCode_E = Bof_CreateEvent(_rName_S + "_enter_evt", false, 1, true, mThreadEnterEvent_X);
      if (mThreadErrorCode_E == BOF_ERR_NO_ERROR)
      {
        mThreadErrorCode_E = Bof_CreateEvent(_rName_S + "_exit_evt", false, 1, true, mThreadExitEvent_X);
        if (mThreadErrorCode_E == BOF_ERR_NO_ERROR)
        {
        }
      }
    }
  }
  // printf("====> Thread %s init at %d\n", mName_S.c_str(), Bof_GetMsTickCount());
  return mThreadErrorCode_E;
}
/*** ~BofThread ********************************************************************/

/*!
 * Description
 * The ~BofThread method delete a previously created BofThread object. All
 * dynamically allocated resources are released here
 *
 * Parameters
 * None
 *
 * Returns
 * Nothing
 *
 * Remarks
 * None
 */

BofThread::~BofThread()
{
  DestroyBofProcessingThread("~BofThread");
  Bof_DestroyEvent(mWakeUpEvent_X);
  Bof_DestroyEvent(mThreadEnterEvent_X);
  Bof_DestroyEvent(mThreadExitEvent_X);
  Bof_DestroyMutex(mMtx_X);

  mSessionCollection.clear();
  BOF_SAFE_DELETE_ARRAY(mpPollOp_X);
}

BOFERR BofThread::InitThreadErrorCode()
{
  return mThreadErrorCode_E;
}

/*** LaunchThread ********************************************************************/

/*!
 * Description
 * The StartThread method creates and activates the thread object. this one is managed
 * via the BOFThread_Thread function which provide object encapsulation to inherited object
 *
 *
 * Parameters
 * SignalEvent_B: true if the mSignalEvent_h event must be signaled
 *      _StackSize_U32: If 0 use the default stack size (1MB) otherwize specify the thread stack size in byte
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 */
//!!! Do not call this method in an intermediate caller object constructor such as in class B or C constructor. You can put it in A
//!!! class A:public B
//!!! class B:public C
//!!! If you do that you will receive "pure virtual method called" abort message as when you are in an intermediate constructor the virtual table is not ready
BOFERR BofThread::LaunchBofProcessingThread(const std::string &_rName_S, bool _SignalEvent_B, uint32_t _WakeUpIntervalInMs_U32, uint32_t /*_StackSize_U32*/, uint32_t _ThreadCpuCoreAffinity_U32, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E,
                                            BOF_THREAD_PRIORITY _ThreadPriority_E, uint32_t _StartStopTimeoutInMs_U32)
{
  BOFERR Rts_E;
  bool ThreadRunning_B;
  BOF_THREAD_PRIORITY Min_E, Max_E;

  Rts_E = BOF_ERR_INVALID_STATE;
  mName_S = _rName_S;
  if ((!Bof_IsEventSignaled(mThreadEnterEvent_X, 0)) && (!Bof_IsEventSignaled(mThreadExitEvent_X, 0)))
  {
    Rts_E = BOF_ERR_NO_ERROR;
    mWakeUpIntervalInMs_U32 = _WakeUpIntervalInMs_U32;
    mCpuCoreAffinity_U32 = _ThreadCpuCoreAffinity_U32;
    mPolicy_E = _ThreadSchedulerPolicy_E;
    mPriority_E = _ThreadPriority_E;
    mStartStopTimeoutInMs_U32 = _StartStopTimeoutInMs_U32;
    if (mPriority_E == BOF_THREAD_DEFAULT_PRIORITY)
    {
      mPriority_E = (Bof_GetThreadPriorityRange(mPolicy_E, Min_E, Max_E) == BOF_ERR_NO_ERROR) ? (BOF_THREAD_PRIORITY)((Max_E + Min_E) / 2) : (BOF_THREAD_PRIORITY)(0);
    }
    mThread = std::thread(&BofThread::BofThread_Thread, this);

    // printf("====> Thread %s launched at %d\n", mName_S.c_str(), Bof_GetMsTickCount());

    if (!mStartStopTimeoutInMs_U32)
    {
      ThreadRunning_B = true; // In this case we consider that the thread will start one day as the thread creation is ok but we do not want to wait
    }
    else
    {
      ThreadRunning_B = (Bof_WaitForEvent(mThreadEnterEvent_X, mStartStopTimeoutInMs_U32, 0) == BOF_ERR_NO_ERROR);
    }

    // printf("====> Thread %s Running %d ? at %d\n", mName_S.c_str(), ThreadRunning_B, Bof_GetMsTickCount());
    if (ThreadRunning_B)
    {
      Rts_E = BOF_ERR_NO_ERROR;
      if (mName_S != "")
      {
#if defined(_WIN32)
        THREADNAME_INFO ThreadNameInfo_X;
        ThreadNameInfo_X.dwType = 0x1000;
        ThreadNameInfo_X.szName = mName_S.c_str();
        ThreadNameInfo_X.dwThreadID = ::GetThreadId(static_cast<HANDLE>(mThread.native_handle()));
        ThreadNameInfo_X.dwFlags = 0;

        // This stop google test from running in command console as it detect the exception... but it works inside the vs ide
        //	__try
        {
            //	RaiseException(MS_VC_EXCEPTION, 0, sizeof(ThreadNameInfo_X) / sizeof(ULONG_PTR), (ULONG_PTR*)&ThreadNameInfo_X);
        } //__except (EXCEPTION_EXECUTE_HANDLER)
        {
        }
#else
        pthread_setname_np(pthread_self(), mName_S.c_str());
#endif
      }
      if (_SignalEvent_B)
      {
        Rts_E = Bof_SignalEvent(mWakeUpEvent_X, 0);
        // printf("Bof_SignalEvent %d\n", Rts_E);
      }
    }
    else
    {
      Rts_E = DestroyBofProcessingThread("LaunchThread"); // Thread has not started in the given time slot->MUST destroy it
      // printf("DestroyBofProcessingThread %d\n",Rts_E);
    }
  }
  // printf("LaunchBofProcessingThread %d\n", Rts_E);

  return Rts_E;
}

//_NbPollEntry_U32 must be at least 1 elem and entry 0 of _pPollOp_X must be free as it will be used to store ctrl poll entry
BOFERR BofThread::LaunchBofProcessingThreadWithIpcSocket(const std::string &_rName_S, bool _SignalEvent_B, uint32_t _WakeUpIntervalInMs_U32, uint32_t _StackSize_U32, uint32_t _ThreadCpuCoreAffinity_U32, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E,
                                                         BOF_THREAD_PRIORITY _ThreadPriority_E, uint32_t _StartStopTimeoutInMs_U32, uint32_t _IpcControlListenerTimeoutInMs_U32, uint16_t _IpcControlListenerPort_U16,
                                                         uint32_t _IpcControllSocketBufferSize_U32, uint32_t _NbMaxPollEntry_U32)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BOF_SOCKET_PARAM BofSocketParam_X;
  uint32_t Start_U32, Delta_U32;

  if (_NbMaxPollEntry_U32)
  {
    Rts_E = BOF_ERR_ENOMEM;

    mpPollOp_X = new BOF_POLL_SOCKET[_NbMaxPollEntry_U32];
    if (mpPollOp_X)
    {
      mNbMaxPollEntry_U32 = _NbMaxPollEntry_U32;
      memset(mpPollOp_X, 0, sizeof(BOF_POLL_SOCKET) * mNbMaxPollEntry_U32);

      mNbPollEntry.store(0);
      Rts_E = BOF_ERR_ENOMEM;
      BofSocketParam_X.BaseChannelParam_X.ChannelName_S = _rName_S;
      BofSocketParam_X.BaseChannelParam_X.Blocking_B = true; // _rBofSocketServerParam_X.Blocking_B;
      BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
      BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = _IpcControllSocketBufferSize_U32;
      BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = _IpcControllSocketBufferSize_U32;
      BofSocketParam_X.NoDelay_B = true;
      BofSocketParam_X.ReUseAddress_B = true;
      BofSocketParam_X.BindIpAddress_S = "tcp://127.0.0.1:0"; // tcpdump -i lo tcp port 8125 -vv -X

      mpuPollControlSender = std::make_unique<BofSocket>(BofSocketParam_X);
      if (mpuPollControlSender)
      {
        Rts_E = mpuPollControlSender->LastErrorCode();
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          BofSocketParam_X.BaseChannelParam_X.ChannelName_S = _rName_S;
          BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 1;
          BofSocketParam_X.BaseChannelParam_X.Blocking_B = true;
          BofSocketParam_X.BindIpAddress_S = BOF_NAMESPACE::Bof_Sprintf("tcp://127.0.0.1:%d", _IpcControlListenerPort_U16);
          BofSocketParam_X.ReUseAddress_B = true;
          BofSocketParam_X.NoDelay_B = true;
          BofSocketParam_X.Ttl_U32 = 0;
          BofSocketParam_X.BroadcastPort_U16 = 0;
          BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
          BofSocketParam_X.MulticastSender_B = false;
          BofSocketParam_X.KeepAlive_B = false;
          BofSocketParam_X.EnableLocalMulticast_B = false;
          mpuPollControlListener = std::make_unique<BofSocket>(BofSocketParam_X);
          if (mpuPollControlListener != nullptr)
          {
            Rts_E = mpuPollControlListener->LastErrorCode();
            if (Rts_E == BOF_ERR_NO_ERROR)
            {
              mPollControlListenerTimeoutInMs_U32 = _IpcControlListenerTimeoutInMs_U32;
              Rts_E = LaunchBofProcessingThread(_rName_S, _SignalEvent_B, _WakeUpIntervalInMs_U32, _StackSize_U32, _ThreadCpuCoreAffinity_U32, _ThreadSchedulerPolicy_E, _ThreadPriority_E, _StartStopTimeoutInMs_U32);
              if (Rts_E == BOF_ERR_NO_ERROR)
              {
                Start_U32 = Bof_GetMsTickCount();
                Rts_E = mpuPollControlSender->V_Connect(_IpcControlListenerTimeoutInMs_U32, BofSocketParam_X.BindIpAddress_S, "");
                if (Rts_E == BOF_ERR_NO_ERROR)
                {
                  Rts_E = Bof_ReEvaluateTimeout(Start_U32, _IpcControlListenerTimeoutInMs_U32);
                  if (Rts_E == BOF_ERR_NO_ERROR)
                  {
                    Rts_E = BOF_ERR_ETIMEDOUT;
                    do
                    {
                      Delta_U32 = Bof_ElapsedMsTime(Start_U32);
                      if (mpsPollControlReceiver != nullptr)
                      {
                        Rts_E = BOF_ERR_NO_ERROR;
                        break;
                      }
                      else
                      {
                        BOF_NAMESPACE::Bof_Sleep(10);
                      }
                    } while (Delta_U32 < _IpcControlListenerTimeoutInMs_U32);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return Rts_E;
}

BOFERR BofThread::SendIpcCommand(BOF_POLL_SOCKET_CMD &_rPollSocketCommand_X)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  uint32_t Nb_U32;

  Rts_E = BOF_ERR_INTERNAL;
  BOF_ASSERT(mpuPollControlSender != nullptr);
  BOF_ASSERT(mpuPollControlSender->LastErrorCode() == BOF_ERR_NO_ERROR);
  if ((mpuPollControlSender) && (mpuPollControlSender->LastErrorCode() == BOF_ERR_NO_ERROR))
  {
    Nb_U32 = sizeof(BOF_POLL_SOCKET_CMD);
    Rts_E = mpuPollControlSender->V_WriteData(1000, Nb_U32, reinterpret_cast<uint8_t *>(&_rPollSocketCommand_X));
  }

  BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);

  return Rts_E;
}
//_pPollOp_X[0] must contains the Fd of mpsPollControlReceiver
BOFERR BofThread::IpcPoll(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPollSet_U32, BOF_POLL_SOCKET_CMD &_rPollSocketCmd_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint32_t Nb_U32;

  _rPollSocketCmd_X.Reset();
  _rNbPollSet_U32 = 0;
  BOF_ASSERT(mNbPollEntry);
  BOF_ASSERT(mpPollOp_X);
  if ((mNbPollEntry) && (mpPollOp_X))
  {
    Rts_E = BOF_ERR_INTERNAL;
    BOF_ASSERT(mpsPollControlReceiver->NativeBofSocketPointer() != nullptr);
    BOF_ASSERT(mpsPollControlReceiver->NativeBofSocketPointer()->LastErrorCode() == BOF_ERR_NO_ERROR);
    if (mpsPollControlReceiver->NativeBofSocketPointer())
    {
      Rts_E = mpsPollControlReceiver->NativeBofSocketPointer()->LastErrorCode();
    }
    BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = Bof_Poll(_TimeoutInMs_U32, mNbPollEntry, mpPollOp_X, _rNbPollSet_U32);
      BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        if (_rNbPollSet_U32)
        {
          if (mpPollOp_X[0].Revent_U16) // CANCEL_POLL_WAIT
          {
            _rNbPollSet_U32--;
            if (mpPollOp_X[0].Revent_U16 & POLLIN) // First data
            {
              Nb_U32 = sizeof(BOF_POLL_SOCKET_CMD);
              Rts_E = mpsPollControlReceiver->NativeBofSocketPointer()->V_ReadData(0, Nb_U32, reinterpret_cast<uint8_t *>(&_rPollSocketCmd_X));
              BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
            }
            if (mpPollOp_X[0].Revent_U16 & (POLLHUP | POLLRDHUP | POLLERR | POLLNVAL)) // After error which can be set also with POLLIN (Data channel write and closed)
            {
            }
            mpPollOp_X[0].Revent_U16 = 0; // Remove it from visible Revent_U16
            ////BOF_DBG_PRINTF("@@@%s Op '%c' Id %08X NbPollEntry %d Rts %d\n", mBofSocketServerParam_X.Name_S.c_str(),_rPollSocketCmd_X.SocketOp_E,   _rPollSocketCmd_X.SessionId,_NbPollEntry.load(), Rts_E);
          }
        }
      }
    }
  }
  BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
  return Rts_E;
}

uint32_t BofThread::NbIpcPollEntry()
{
  return mNbPollEntry.load();
}

BOFERR BofThread::DestroyBofProcessingThread(const char * /*_pUser_c*/)
{
  BOFERR Rts_E = BOF_ERR_INVALID_STATE;
  bool ThreadStopTo_B;

  // printf("====> DestroyThread %s ThreadLoopMustExit %d ? Signal Enter %d Exit %d WakeUp %d this %p at %d\n", mName_S.c_str(), mThreadLoopMustExit_B,  Bof_IsEventSignaled(mThreadEnterEvent_X,0),
  // Bof_IsEventSignaled(mThreadExitEvent_X,0),mWakeUpIntervalInMs_U32, static_cast<void *>(this), Bof_GetMsTickCount());

  if (Bof_IsEventSignaled(mThreadEnterEvent_X, 0))
  {
    Rts_E = BOF_ERR_NO_ERROR;
    if (!Bof_IsEventSignaled(mThreadExitEvent_X, 0))
    {
      mThreadLoopMustExit_B = true;
      SignalThreadWakeUpEvent();
      if (!mThreadExitPosted_B)
      {
        // printf("====> Wait for %d t %d\n", mStartStopTimeoutInMs_U32, Bof_GetMsTickCount());
        ThreadStopTo_B = (Bof_WaitForEvent(mThreadExitEvent_X, mStartStopTimeoutInMs_U32, 0) != BOF_ERR_NO_ERROR);
        // printf("====> End of Wait --> %d t %d\n", ThreadStopTo_B, Bof_GetMsTickCount());
        if (ThreadStopTo_B)
        {
          // printf("====>Cannot stop thread p=%p val %d\n", static_cast<void *>(&mThreadLoopMustExit_B) ,mThreadLoopMustExit_B);
        }
      }
    }
  }
  // printf("====>joinable %d\n", mThread.joinable());
  if (mThread.joinable())
  {
    // printf("====>joinable start\n");
    mThread.join();
    // printf("====>joinable end\n");
  }
  // printf("====>DestroyThread rts %d\n", Rts_E);
  return Rts_E;
}

bool BofThread::IsThreadRunning()
{
  bool Rts_B = false;

  if (Bof_IsEventSignaled(mThreadEnterEvent_X, 0))
  {
    if (!Bof_IsEventSignaled(mThreadExitEvent_X, 0))
    {
      Rts_B = true;
    }
  }
  return Rts_B;
}

const char *BofThread::LockInfo(int32_t &_rLockBalance_S32) const
{
  _rLockBalance_S32 = mLockBalance.load();
  return mpLastLocker_c;
}

// Leave a space in front of _pLocker to insert a '+' Lock or '-' unlock (see below)
BOFERR BofThread::LockThreadCriticalSection(const char *_pLocker_c)
{
  //	printf("##############%s WAIT %s last %s count %d\n", mName_S.c_str(), _pLocker_c, mpLastLocker_c, mLockBalance.load());

  BOF_ASSERT(_pLocker_c != nullptr);
  BOFERR Rts_E = Bof_LockMutex(mMtx_X);
  mLockBalance++;
  BOF_STRNCPY_NULL_CLIPPED(mpLastLocker_c, _pLocker_c, sizeof(mpLastLocker_c));
  mpLastLocker_c[0] = '+';
  //	printf("##############%s LOCK last %s count %d\n", mName_S.c_str(), mpLastLocker_c, mLockBalance.load());

  BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
  return Rts_E;
}

BOFERR BofThread::UnlockThreadCriticalSection()
{
  //	printf("##############%s UNLOCK %s count %d\n", mName_S.c_str(), mpLastLocker_c, mLockBalance.load()-1);
  BOFERR Rts_E = Bof_UnlockMutex(mMtx_X);
  mLockBalance--;
  mpLastLocker_c[0] = '-';

  BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
  return Rts_E;
}

BOFERR BofThread::SignalThreadWakeUpEvent()
{
  return Bof_SignalEvent(mWakeUpEvent_X, 0);
}

BOFERR BofThread::WaitForThreadWakeUpEvent(uint32_t _TimeoutInMs_U32)
{
  return Bof_WaitForEvent(mWakeUpEvent_X, _TimeoutInMs_U32, 0);
}

BOFERR BofThread::SetThreadWakeUpInterval(uint32_t _WakeUpIntervalInMs_U32)
{
  mWakeUpIntervalInMs_U32 = _WakeUpIntervalInMs_U32;
  return BOF_ERR_NO_ERROR;
}

bool BofThread::IsThreadLoopMustExit()
{
  //	printf("IsThreadLoopMustExit p=%p val %d\n",&mThreadLoopMustExit_B,mThreadLoopMustExit_B);
  return mThreadLoopMustExit_B;
}

BOFERR BofThread::PostThreatExit(const char *_pUser_c)
{
  /*
   All call to DestroyThread will not try to kill the thread  (-> do not call Bof_DestroyThread) as the S_BofThread_Thread will automatically exit
   This is used to kill a BofThread when we are inside the V_OnProcessing execution path
  */
  mThreadExitPosted_B = true;
  return DestroyBofProcessingThread(_pUser_c);
  ;
}

// Used to specify callback if the caller does not inherit from BofThread
void BofThread::SetThreadCallback(BOF_THREAD_CALLBACK _OnCreate, BOF_THREAD_CALLBACK _OnProcessing, BOF_THREAD_CALLBACK _OnStop)
{
  LockThreadCriticalSection(" BofThread::SetThreadCallback");
  mOnCreate = _OnCreate;
  mOnProcessing = _OnProcessing;
  mOnStop = _OnStop;
  UnlockThreadCriticalSection();
}

/*** V_OnProcessing ********************************************************************/

/*!
 * Description
 * The V_OnProcessing method is called by the BofThread_Thread thread function which is
 * created by the Start method. This function is the main thread function and can be
 * overwritten to provide specific application  behaviour
 *
 * Parameters
 * _WaitCode_U32:  Specifies the result of the wait operation on the thread event.
 * If it is not used it is equal to WAIT_OBJECT_0
 *
 * Returns
 * uint32_t: 0 if the operation is successful
 *
 * Remarks
 * None
 */
BOFERR BofThread::V_OnProcessing()
{
  BOFERR Rts_E = mOnProcessing ? mOnProcessing() : BOF_ERR_NO_ERROR;
  return Rts_E;
}

/*** V_OnCreate ********************************************************************/

/*!
 * Description
 * The V_OnCreate method is called by the BofThread_Thread thread function which is
 * created by the Start method. This function is called when the thread starts and
 * can be overwritten to provide specific application  behaviour
 *
 * Parameters
 * None
 *
 * Returns
 * uint32_t: 0 if the operation is successful
 *
 * Remarks
 * None
 */

BOFERR BofThread::V_OnCreate()
{
  BOFERR Rts_E = mOnCreate ? mOnCreate() : BOF_ERR_NO_ERROR;
  return Rts_E;
}

/*** OnStop ********************************************************************/

/*!
 * Description
 * The OnStop method is called by the BofThread_Thread thread function which is
 * created by the Start method. This function is called when the thread ends and
 * can be overwritten to provide specific application  behaviour
 *
 * Parameters
 * None
 *
 * Returns
 * uint32_t: 0 if the operation is successful
 *
 * Remarks
 * None
 */
BOFERR BofThread::V_OnStop()
{
  BOFERR Rts_E = mOnStop ? mOnStop() : BOF_ERR_NO_ERROR;
  return Rts_E;
}

/*** BOFThread_Thread ********************************************************************/

/*!
 * Description
 * The BOFThread_Thread is the thread created by the Start method. it calls the V_OnCreate
 * virtual method and until the thread ends the On
 * If mUseEvent_B is true the V_OnProcessing methods is called one or periodically via the
 * mSignalEvent_h event
 * When the thread exit the OnStop virtual method is called
 *
 * Parameters
 * _pContext:   Specifies a pointer to the BofThread parent object
 *
 * Returns
 * uint32_t: 0 if the operation is successful
 *
 * Remarks
 * None
 */
void BofThread::BofThread_Thread()
{
  BOFERR Sts_E = BOF_ERR_NO_ERROR;
  uint32_t Delta_U32;
  BOF_SOCKET_IO_PARAM BofSocketIoParam_X;
  BofComChannel *pChannel;
  std::unique_ptr<BofSocket> puSocket;
  BOFSOCKET SessionId;
  // printf("====> Thread start\n");

  Sts_E = Bof_SignalEvent(mThreadEnterEvent_X, 0);
  BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
  // mThreadRunning_B = true;

#if defined(_WIN32)
  if (mCpuCoreAffinity_U32)
  {
    // A DWORD_PTR is not a pointer.It is an unsigned integer that is the same size as a pointer.Thus, in Win32 a DWORD_PTR is the same as a DWORD(32 bits), and in Win64 it is the same as a ULONGLONG(64 bits).
    DWORD_PTR Val = (DWORD_PTR)(1 << mCpuCoreAffinity_U32);
    SetThreadAffinityMask(GetCurrentThread(), Val);
  }
  Sts_E = (SetThreadPriority(GetCurrentThread(), mPriority_E) == TRUE) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
  // int32_t Win32Err_S32 = 0;
  // Bof_GetLastError(false, &Win32Err_S32);
  // printf("SetThreadPriority(%d)=%d\n", mPriority_E, Sts_E);
#else
  if (mCpuCoreAffinity_U32)
  {
    cpu_set_t CpuSet_X;
    CPU_ZERO(&CpuSet_X);
    CPU_SET(mCpuCoreAffinity_U32 - 1, &CpuSet_X);
    Sts_E = (sched_setaffinity(static_cast<__pid_t>(syscall(SYS_gettid)), sizeof(CpuSet_X), &CpuSet_X) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
  }

  int Status_i = 0;
  int Policy_i = mPolicy_E;
  struct sched_param Params_X;

  Sts_E = BOF_ERR_INTERNAL;
  Params_X.sched_priority = mPriority_E;
  // Set the priority
  Status_i = pthread_setschedparam(pthread_self(), mPolicy_E, &Params_X);
  // Verify
  if (Status_i == 0)
  {
    Status_i = pthread_getschedparam(pthread_self(), &Policy_i, &Params_X);
    Sts_E = ((Policy_i == mPolicy_E) && (Params_X.sched_priority == mPriority_E)) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
  }
#endif
  // printf("====> Thread starting %d\n", Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if (mpuPollControlListener != nullptr)
    {
      Sts_E = BOF_ERR_ECONNREFUSED;
      // BOF_ASSERT(mNbPollEntry);
      BOF_ASSERT(mpPollOp_X);
      if (mpPollOp_X)
      {
        pChannel = mpuPollControlListener->V_Listen(mPollControlListenerTimeoutInMs_U32, "");
        if (pChannel)
        {
          puSocket.reset(dynamic_cast<BofSocket *>(pChannel));
          if (puSocket)
          {
            Sts_E = puSocket->LastErrorCode();
            if (Sts_E == BOF_ERR_NO_ERROR)
            {
              BofSocketIoParam_X.Name_S = "CMD_POLL_WAIT";
              mpsPollControlReceiver = std::make_shared<BofSocketIo>(nullptr, std::move(puSocket), BofSocketIoParam_X);
              if (mpsPollControlReceiver)
              {
                mpsPollControlReceiver->SessionType(BOF_SOCKET_SESSION_TYPE::CMD_POLL_WAIT);

                SessionId = mpsPollControlReceiver->NativeBofSocketPointer()->GetSocketHandle();

                //_NbPollEntry_U32 must be at least 1 elem and entry 0 of _pPollOp_X must be free as it will be used to store ctrl poll entry
                mpPollOp_X[0].Fd = SessionId;
                mpPollOp_X[0].Event_U16 = POLLIN;
                mNbPollEntry++;
                BOF_ASSERT(mNbPollEntry == 1);
              }
            }
          }
        }
      }
    }
  }
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    V_OnCreate();
    // printf("====> Thread %s ThreadLoopMustExit %d ? Signal Enter %d Exit %d WakeUp %d Sts %d this %p at %d\n", mName_S.c_str(), mThreadLoopMustExit_B,  Bof_IsEventSignaled(mThreadEnterEvent_X),
    // Bof_IsEventSignaled(mThreadExitEvent_X),mWakeUpIntervalInMs_U32,Sts_E, this, Bof_GetMsTickCount());
    mLoopTimerWarning_U32 = Bof_GetMsTickCount();
    while ((Sts_E == BOF_ERR_NO_ERROR) && (!mThreadLoopMustExit_B))
    {
      if (mWakeUpIntervalInMs_U32)
      {
        Sts_E = Bof_WaitForEvent(mWakeUpEvent_X, mWakeUpIntervalInMs_U32, 0);
        if (Sts_E == BOF_ERR_ETIMEDOUT)
        {
          Sts_E = BOF_ERR_NO_ERROR;
        }
      }
      if ((Sts_E == BOF_ERR_NO_ERROR) && (!mThreadLoopMustExit_B))
      {
        LockThreadCriticalSection(" BofThread::BofThread_Thread");
        //        printf("====> Thread %s call V_OnProcessing at %d\n", mName_S.c_str(), Bof_GetMsTickCount());
        Sts_E = V_OnProcessing(); // Return BOF_ERR_CANCEL to exit without calling V_OnStop (underlying thread oject has been destroyed). Any other error code different from BOF_ERR_NO_ERROR will exit AND call V_OnStop
        UnlockThreadCriticalSection();
        //        printf("====> exit Thread %s call V_OnProcessing at %d\n", mName_S.c_str(), Bof_GetMsTickCount());
      }
      /*
      All call to DestroyThread will not try to kill the thread  (-> do not call Bof_DestroyThread) as the S_BofThread_Thread will automatically exit
      This is used to kill a BofThread when we are inside the V_OnProcessing execution path
      */
      if (mThreadExitPosted_B)
      {
        Sts_E = BOF_ERR_CANCEL;
      }
      else
      {
        Delta_U32 = Bof_ElapsedMsTime(mLoopTimerWarning_U32);
        if (Delta_U32 < 40)
        {
          //        Sts_E = V_OnProcessing();
          Delta_U32++; // Put a breakpoint here to detect full speed thread loop
        }
        mLoopTimerWarning_U32 = Bof_GetMsTickCount();
      }
    } // while
    //  printf("====> Thread %s exit %d at %d\n", mName_S.c_str(), Sts_E, Bof_GetMsTickCount());

    if (Sts_E != BOF_ERR_CANCEL)
    {
      V_OnStop();
    }
  }
  //	printf("====> Signal %s at %d\n", mName_S.c_str(), Bof_GetMsTickCount());
  // mThreadRunning_B = false;
  Sts_E = Bof_SignalEvent(mThreadExitEvent_X, 0);
  BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
}

END_BOF_NAMESPACE()