/*printf
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
#include <bofstd/bofstring.h>
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
// BofLogger::S_Instance().Log("IpSwitcherLog", BOF::BOF::CRITICAL, "BofSocketIo[%d] DoINeedToConnect_B", BOF::Bof_GetMsTickCount());

#if defined(_WIN32)
#else
#include <sys/syscall.h>
#include <unistd.h>
#endif

BEGIN_BOF_NAMESPACE()
std::atomic<int32_t> BofThread::S_mBofThreadBalance = 0;

BofThread::BofThread()
    : BofThread(false)
{
}
BofThread::BofThread(bool _PriorityInversionAware_B)
{
  // printf("%u: CREATE THREAD\n", BOF::Bof_GetMsTickCount());
  mThreadErrorCode_E = InitializeThread("?", _PriorityInversionAware_B);
  BOF_ASSERT(mThreadErrorCode_E == BOF_ERR_NO_ERROR);
}

BOFERR BofThread::InitializeThread(const std::string &_rName_S, bool _PriorityInversionAware_B)
{
  mThreadParam_X.Name_S = _rName_S;
  mThreadMtx_X.Reset();
  mLockBalance.store(0);
  mpLastLocker_c[0] = 0;
  mWakeUpEvent_X.Reset();
  mThreadErrorCode_E = BOF_ERR_ENOMEM;
  mThreadErrorCode_E = Bof_CreateMutex(_rName_S + "_mtx", false, false, mThreadMtx_X);
  if (mThreadErrorCode_E == BOF_ERR_NO_ERROR)
  {
    mThreadErrorCode_E = Bof_CreateEvent(_rName_S + "_wakeup_evt", false, 1, false, false, _PriorityInversionAware_B, mWakeUpEvent_X);
    if (mThreadErrorCode_E == BOF_ERR_NO_ERROR)
    {
      mThreadErrorCode_E = Bof_CreateEvent(_rName_S + "_enter_evt", false, 1, true, false, _PriorityInversionAware_B, mThreadEnterEvent_X);
      if (mThreadErrorCode_E == BOF_ERR_NO_ERROR)
      {
        mThreadErrorCode_E = Bof_CreateEvent(_rName_S + "_exit_evt", false, 1, true, false, _PriorityInversionAware_B, mThreadExitEvent_X);
        if (mThreadErrorCode_E == BOF_ERR_NO_ERROR)
        {
        }
      }
    }
  }
  return mThreadErrorCode_E;
}

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
  // printf("%u: DELETE THREAD\n", BOF::Bof_GetMsTickCount());
  DestroyBofProcessingThread("~BofThread");
  Bof_DestroyEvent(mWakeUpEvent_X);
  Bof_DestroyEvent(mThreadEnterEvent_X);
  Bof_DestroyEvent(mThreadExitEvent_X);
  Bof_DestroyMutex(mThreadMtx_X);
}

BOFERR BofThread::InitThreadErrorCode()
{
  return mThreadErrorCode_E;
}

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
BOFERR BofThread::LaunchBofProcessingThread(const BOF_THREAD_PARAM &_rThreadParam_X)
{
  return LaunchBofProcessingThread(_rThreadParam_X.Name_S, _rThreadParam_X.PriorityInversionAware_B, _rThreadParam_X.SignalEvent_B, _rThreadParam_X.WakeUpIntervalInMs_U32, _rThreadParam_X.ThreadSchedulerPolicy_E,
                                   _rThreadParam_X.ThreadPriority_E, _rThreadParam_X.ThreadCpuCoreAffinityMask_U64, _rThreadParam_X.StartStopTimeoutInMs_U32, _rThreadParam_X.StackSize_U32);
}
BOFERR BofThread::LaunchBofProcessingThread(const std::string &_rName_S, bool _PriorityInversionAware_B, bool _SignalEvent_B, uint32_t _WakeUpIntervalInMs_U32, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF_THREAD_PRIORITY _ThreadPriority_E,
                                            uint64_t _ThreadCpuCoreAffinityMask_U64, uint32_t _StartStopTimeoutInMs_U32, uint32_t _StackSize_U32)
{
  BOFERR Rts_E;
  bool ThreadRunning_B;
  BOF_THREAD_PRIORITY Min_E, Max_E;

  Rts_E = BOF_ERR_INVALID_STATE;
  mThreadParam_X.Name_S = _rName_S;
  mThreadParam_X.PriorityInversionAware_B = _PriorityInversionAware_B;
  mThreadParam_X.StackSize_U32 = _StackSize_U32;
  if ((!Bof_IsEventSignaled(mThreadEnterEvent_X, 0)) && (!Bof_IsEventSignaled(mThreadExitEvent_X, 0)))
  {
    Rts_E = BOF_ERR_NO_ERROR;
    /*
    if ((_ThreadSchedulerPolicy_E == BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER) && (_ThreadPriority_E != BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000) && (_ThreadPriority_E != BOF_THREAD_DEFAULT_PRIORITY))
    {
      Rts_E = BOF_ERR_EINVAL;
    }
    if (Rts_E == BOF_ERR_NO_ERROR)
    */
    {
      mThreadParam_X.WakeUpIntervalInMs_U32 = _WakeUpIntervalInMs_U32;
      mThreadParam_X.ThreadCpuCoreAffinityMask_U64 = _ThreadCpuCoreAffinityMask_U64;
      mThreadParam_X.ThreadSchedulerPolicy_E = _ThreadSchedulerPolicy_E;
      mThreadParam_X.ThreadPriority_E = _ThreadPriority_E;
      mThreadParam_X.StartStopTimeoutInMs_U32 = _StartStopTimeoutInMs_U32;
      /*
      if (mThreadParam_X.ThreadPriority_E == BOF_THREAD_DEFAULT_PRIORITY)
      {
        mThreadParam_X.ThreadPriority_E = (Bof_GetThreadPriorityRange(mPolicy_E, Min_E, Max_E) == BOF_ERR_NO_ERROR) ? (BOF_THREAD_PRIORITY)((Max_E + Min_E) / 2) : (BOF_THREAD_PRIORITY)(0);
      }
      */
      mThread = std::thread(&BofThread::BofThread_Thread, this);
      mThreadHandle = mThread.native_handle(); // Its value disappear after a join or a detach http://www.bo-yang.net/2017/11/19/cpp-kill-detached-thread

      if (!mThreadParam_X.StartStopTimeoutInMs_U32)
      {
        ThreadRunning_B = true; // In this case we consider that the thread will start one day as the thread creation is ok but we do not want to wait
      }
      else
      {
        ThreadRunning_B = (Bof_WaitForEvent(mThreadEnterEvent_X, mThreadParam_X.StartStopTimeoutInMs_U32, 0) == BOF_ERR_NO_ERROR);
      }

      if (ThreadRunning_B)
      {
        Rts_E = BOF_ERR_NO_ERROR;
        if (mThreadParam_X.Name_S != "")
        {
          // Linux: the thread name is a meaningful C language string, whose length is restricted to 16 characters, including the terminating null byte ('\0').
          if (mThreadParam_X.Name_S.length() > 15)
          {
            mThreadParam_X.Name_S = mThreadParam_X.Name_S.substr(0, 15);
          }
#if defined(_WIN32)
          THREADNAME_INFO ThreadNameInfo_X;
          ThreadNameInfo_X.dwType = 0x1000;
          ThreadNameInfo_X.szName = mThreadParam_X.Name_S.c_str();
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
          //        pthread_setname_np(pthread_self(), mThreadParam_X.Name_S.c_str());

          pthread_setname_np(static_cast<pthread_t>(mThread.native_handle()), mThreadParam_X.Name_S.c_str());
#endif
        }
        if (_SignalEvent_B)
        {
          Rts_E = Bof_SignalEvent(mWakeUpEvent_X, 0);
        }
      }
      else
      {
        Rts_E = DestroyBofProcessingThread("LaunchThread"); // Thread has not started in the given time slot->MUST destroy it
      }
    }
  }
  return Rts_E;
}

BOFERR BofThread::DestroyBofProcessingThread(const char * /*_pUser_c*/)
{
  BOFERR Rts_E = BOF_ERR_ALREADY_CLOSED;
  bool ThreadStopTo_B = false;

  if (!mThreadMustStop_B)
  {
    mThreadMustStop_B = true;
    Rts_E = BOF_ERR_NOT_STARTED;
    // printf("%u: DESTROYBOFPROCESSINGTHREAD: CHECK IF MTHREADENTEREVENT_X SIGNALED %d\n", BOF::Bof_GetMsTickCount(), Bof_IsEventSignaled(mThreadEnterEvent_X, 0));
    if (Bof_IsEventSignaled(mThreadEnterEvent_X, 0)) // LaunchBofProcessingThread has called Bof_WaitForEvent(mThreadEnterEvent_X with success in
    {
      Rts_E = BOF_ERR_NO_ERROR;
      // printf("%u: DESTROYBOFPROCESSINGTHREAD MTHREADENTEREVENT_X SIGNALED, CHECK IF MTHREADEXITEVENT_X SIGNALED %d\n", BOF::Bof_GetMsTickCount(), Bof_IsEventSignaled(mThreadExitEvent_X, 0));
      if (!Bof_IsEventSignaled(mThreadExitEvent_X, 0))
      {
        // printf("%u: SignalThreadWakeUpEvent\n", BOF::Bof_GetMsTickCount());
        SignalThreadWakeUpEvent();
        // printf("%u: Bof_WaitForEvent start\n", BOF::Bof_GetMsTickCount(), ThreadStopTo_B);
        ThreadStopTo_B = (Bof_WaitForEvent(mThreadExitEvent_X, mThreadParam_X.StartStopTimeoutInMs_U32, 0) != BOF_ERR_NO_ERROR);
        // printf("%u: Bof_WaitForEvent stop To %d\n", BOF::Bof_GetMsTickCount(), ThreadStopTo_B);
      }

      // printf("%u: THREADSTOPTO %d ?\n", Bof_GetMsTickCount(), ThreadStopTo_B);
      if (ThreadStopTo_B)
      {
#if defined(_WIN32)
        // printf("%u: TerminateThread !!!!\n", BOF::Bof_GetMsTickCount());
        TerminateThread(static_cast<HANDLE>(mThreadHandle), 0x69696969);
#else
#if defined(__ANDROID__)
#else
        pthread_cancel(static_cast<pthread_t>(mThreadHandle));
#endif
#endif
      }
    }
    else
    {
      printf("%u: THREAD NEVER BEEN STARTED\n", BOF::Bof_GetMsTickCount());
    }
  }
  else
  {
    printf("%u: DESTROYBOFPROCESSINGTHREAD: DELETE ALREADY DONE\n", BOF::Bof_GetMsTickCount());
  }
  // printf("%u: JOINABLE ? %d\n", Bof_GetMsTickCount(), mThread.joinable());
  if (mThread.joinable()) // Needed to clanup std::thread if launch failed
  {
    // printf("%u: START JOINING\n", Bof_GetMsTickCount());
    mThread.join();
    // printf("%u: END OF JOIN\n", Bof_GetMsTickCount());
  }

  // printf("%u: %s DESTROYTHREAD FINISHED WITH RTS %d\n", Bof_GetMsTickCount(), mName_S.c_str(), Rts_E);
  return Rts_E;
}

bool BofThread::IsThreadRunning(uint32_t _Timeout_U32)
{
  bool Rts_B = false;
  uint32_t Timer_U32, Delta_U32;

  Timer_U32 = BOF::Bof_GetMsTickCount();
  do
  {
    if (Bof_IsEventSignaled(mThreadEnterEvent_X, 0))
    {
      if (!Bof_IsEventSignaled(mThreadExitEvent_X, 0))
      {
        Rts_B = true;
      }
    }
    Delta_U32 = Bof_ElapsedMsTime(Timer_U32);
    if (!Rts_B)
    {
      Bof_MsSleep(0);
    }
  } while ((!Rts_B) && (Delta_U32 < _Timeout_U32));
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
  BOF_ASSERT(_pLocker_c != nullptr);
  BOFERR Rts_E = Bof_LockMutex(mThreadMtx_X);
  mLockBalance++;
  Bof_StrNCpy(mpLastLocker_c, _pLocker_c, sizeof(mpLastLocker_c));
  mpLastLocker_c[0] = '+';

  BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
  return Rts_E;
}

BOFERR BofThread::UnlockThreadCriticalSection()
{
  BOFERR Rts_E = Bof_UnlockMutex(mThreadMtx_X);
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
  mThreadParam_X.WakeUpIntervalInMs_U32 = _WakeUpIntervalInMs_U32;
  return BOF_ERR_NO_ERROR;
}

bool BofThread::IsThreadLoopMustExit()
{
  //	printf("IsThreadLoopMustExit p=%p val %d\n",&mThreadLoopMustExit_B,mThreadLoopMustExit_B);
  return mThreadMustStop_B;
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
std::string BofThread::ThreadName() const
{
  return mThreadParam_X.Name_S;
}
std::string BofThread::S_ToString(const BOF_THREAD_PARSER_PARAM &_rThreadParserParam_X, bool _ShowChosenCore_B)
{
  std::string Rts_S;
  uint32_t i_U32, pRange_U32[2];
  uint64_t Mask_U64;
  char *p_c, pBuffer_c[0x1000], pSchedulerPolicy_c[] = {'o', 'f', 'r'}; // BOF_THREAD_SCHEDULER_POLICY_OTHER,BOF_THREAD_SCHEDULER_POLICY_FIFO,BOF_THREAD_SCHEDULER_ROUND_ROBIN
  bool AlreadyOne_B;

  p_c = pBuffer_c;
  if (!_rThreadParserParam_X.AffinityCpuSet_U64)
  {
    p_c += sprintf(p_c, "n%d:%c%d", _rThreadParserParam_X.Node_U32, (_rThreadParserParam_X.ThreadSchedulerPolicy_E < BOF_THREAD_SCHEDULER_POLICY_MAX) ? pSchedulerPolicy_c[_rThreadParserParam_X.ThreadSchedulerPolicy_E] : '?', _rThreadParserParam_X.ThreadPriority_E);
  }
  else
  {
    p_c += sprintf(p_c, "n%d:%c%d:c", _rThreadParserParam_X.Node_U32, (_rThreadParserParam_X.ThreadSchedulerPolicy_E < BOF_THREAD_SCHEDULER_POLICY_MAX) ? pSchedulerPolicy_c[_rThreadParserParam_X.ThreadSchedulerPolicy_E] : '?', _rThreadParserParam_X.ThreadPriority_E);
    pRange_U32[0] = 0xFFFFFFFF;
    pRange_U32[1] = 0xFFFFFFFF;
    AlreadyOne_B = false;
    for (Mask_U64 = 1, i_U32 = 0; i_U32 < (sizeof(_rThreadParserParam_X.AffinityCpuSet_U64) * 8); i_U32++, Mask_U64 <<= 1) // affinityMask.size(); i++)
    {
      if (_rThreadParserParam_X.AffinityCpuSet_U64 & Mask_U64)
      {
        if (pRange_U32[0] == 0xFFFFFFFF)
        {
          pRange_U32[0] = i_U32;
        }
        else if (pRange_U32[1] == 0xFFFFFFFF)
        {
          pRange_U32[1] = i_U32;
        }
        else
        {
          pRange_U32[1]++;
        }
        if (i_U32 == 63)
        {
          goto FlushIt;
        }
      }
      else
      {
      FlushIt:
        if ((pRange_U32[0] != 0xFFFFFFFF) && (pRange_U32[1] != 0xFFFFFFFF))
        {
          if (AlreadyOne_B)
          {
            *p_c++ = ',';
          }
          if (pRange_U32[0] == pRange_U32[1])
          {
            p_c += sprintf(p_c, "%d", pRange_U32[0]);
          }
          else
          {
            p_c += sprintf(p_c, "%d-%d", pRange_U32[0], pRange_U32[1]);
          }
          AlreadyOne_B = true;
        }
        else if (pRange_U32[0] != 0xFFFFFFFF)
        {
          if (AlreadyOne_B)
          {
            *p_c++ = ',';
          }
          if ((i_U32 - 1) == pRange_U32[0])
          {
            p_c += sprintf(p_c, "%d", pRange_U32[0]);
          }
          else
          {
            p_c += sprintf(p_c, "%d-%d", pRange_U32[0], i_U32 - 1); // _rThreadParserParam_X.NbActiveCore_U32);
          }
          AlreadyOne_B = true;
        }
        pRange_U32[0] = 0xFFFFFFFF;
        pRange_U32[1] = 0xFFFFFFFF;
      }
    }
  }
  if (_ShowChosenCore_B)
  {
    p_c += sprintf(p_c, " A%d/%d", _rThreadParserParam_X.CoreChosen_U32, _rThreadParserParam_X.NbActiveCore_U32);
  }
  Rts_S = pBuffer_c;
  return Rts_S;
}

BOFERR BofThread::S_AffinityMaskFromString(const char *_pAffinityOptionString_c, uint32_t _NbCore_U32, uint64_t &_rAffinityMask_U32)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint64_t Mask_U64;
  uint32_t i_U32, OptionLen_U32, SubOptionLen_U32, pRange_U32[2];
  const char *pComa_c, *pMinus_c;
  char pOption_c[256];

  _rAffinityMask_U32 = 0;
  if (_pAffinityOptionString_c)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    _pAffinityOptionString_c++;
    do
    {
      pComa_c = strchr(_pAffinityOptionString_c, ',');
      OptionLen_U32 = pComa_c ? static_cast<uint32_t>(pComa_c - _pAffinityOptionString_c) : static_cast<uint32_t>(strlen(_pAffinityOptionString_c));
      if ((OptionLen_U32) && (OptionLen_U32 < sizeof(pOption_c)))
      {
        strncpy(pOption_c, _pAffinityOptionString_c, OptionLen_U32);
        pOption_c[OptionLen_U32] = 0;

        pRange_U32[0] = -1;
        pRange_U32[1] = -1;
        pMinus_c = strchr(pOption_c, '-');
        SubOptionLen_U32 = pMinus_c ? static_cast<uint32_t>(pMinus_c - pOption_c) : static_cast<uint32_t>(strlen(pOption_c));

        if (SubOptionLen_U32)
        {
          pOption_c[SubOptionLen_U32] = 0;
          if (!Bof_IsDecimal(pOption_c, pRange_U32[0]))
          {
            Rts_E = BOF_ERR_FORMAT;
            break;
          }
          if (pRange_U32[0] >= _NbCore_U32)
          {
            pRange_U32[0] = -1;
            //    pRange_U32[0] = _NbCore_U32 - 1;
          }
          if (pMinus_c)
          {
            if (!Bof_IsDecimal(pMinus_c + 1, pRange_U32[1]))
            {
              Rts_E = BOF_ERR_FORMAT;
              break;
            }
            if (pRange_U32[1] >= _NbCore_U32)
            {
              //     pRange_U32[1] = -1;
              pRange_U32[1] = _NbCore_U32 - 1;
            }
          }
        }
        else
        {
          break;
        }

        if ((pRange_U32[0] != 0xFFFFFFFF) && (pRange_U32[1] != 0xFFFFFFFF))
        {
          if (pRange_U32[0] <= pRange_U32[1])
          {
            for (Mask_U64 = ((uint64_t)1 << pRange_U32[0]), i_U32 = pRange_U32[0]; i_U32 <= pRange_U32[1]; i_U32++, Mask_U64 <<= 1)
            {
              _rAffinityMask_U32 |= Mask_U64;
            }
          }
          else
          {
            Rts_E = BOF_ERR_TOO_BIG;
          }
        }
        else if (pRange_U32[0] != 0xFFFFFFFF)
        {
          _rAffinityMask_U32 |= ((uint64_t)1 << pRange_U32[0]);
        }
        _pAffinityOptionString_c += OptionLen_U32;
        if (pComa_c)
        {
          _pAffinityOptionString_c++;
        }
      }
    } while ((pComa_c) && (*_pAffinityOptionString_c) && (Rts_E == BOF_ERR_NO_ERROR));
  }
  return Rts_E;
}

BOFERR BofThread::S_ThreadParserParamFromString(const char *_pThreadParameter_c, BOF_THREAD_PARSER_PARAM &_rThreadParserParam_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  const char *pColon_c;
  char pOption_c[256];
  uint32_t OptionLen_U32, Core_U32, MidNbCore_U32, i_U32, Val_U32;
  uint64_t CoreAffinity_U64, Mask_U64;
  bool ConsiderVirtualCore_B;

  if (_pThreadParameter_c)
  {
    memset(&_rThreadParserParam_X, 0, sizeof(BOF_THREAD_PARSER_PARAM));
    _rThreadParserParam_X.NbActiveCore_U32 = std::thread::hardware_concurrency(); // sysconf(_SC_NPROCESSORS_ONLN);
    //    printk("num_online_cpus %d num_possible_cpus() %d num_present_cpus() %d num_active_cpus %d\n",num_online_cpus()	,num_possible_cpus()	, num_present_cpus(),num_active_cpus());
    do
    {
      Rts_E = BOF_ERR_PARSER;
      ConsiderVirtualCore_B = false;

      pColon_c = strchr(_pThreadParameter_c, ':');
      OptionLen_U32 = pColon_c ? static_cast<uint32_t>(pColon_c - _pThreadParameter_c) : static_cast<uint32_t>(strlen(_pThreadParameter_c));
      if ((OptionLen_U32) && (OptionLen_U32 < sizeof(pOption_c)))
      {
        strncpy(pOption_c, _pThreadParameter_c, OptionLen_U32);
        pOption_c[OptionLen_U32] = 0;
        CoreAffinity_U64 = 0;
        Rts_E = BOF_ERR_NO_ERROR;
        switch (pOption_c[0])
        {
          case 'n':
            if (!Bof_IsDecimal(pOption_c + 1, _rThreadParserParam_X.Node_U32))
            {
              Rts_E = BOF_ERR_FORMAT;
            }
            break;

          case 'c':
            ConsiderVirtualCore_B = true;
            // Rts_E = S_AffinityMaskFromString(pOption_c, _rThreadParserParam_X.NbActiveCore_U32, CoreAffinity_U64);
            Rts_E = S_AffinityMaskFromString(pOption_c, 0xFFFFFFFF, CoreAffinity_U64);
            break;

          case 'C':
            //            Rts_E = S_AffinityMaskFromString(pOption_c, _rThreadParserParam_X.NbActiveCore_U32 / 2, CoreAffinity_U64);
            Rts_E = S_AffinityMaskFromString(pOption_c, 0xFFFFFFFF, CoreAffinity_U64);
            break;

          case 'o':
          case 'f':
          case 'r':
            if (Bof_IsDecimal(pOption_c + 1, Val_U32))
            {
              _rThreadParserParam_X.ThreadPriority_E = static_cast<BOF_THREAD_PRIORITY>(Val_U32);
              if (pOption_c[0] == 'o')
              {
                _rThreadParserParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER;
              }
              else if (pOption_c[0] == 'f')
              {
                _rThreadParserParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_FIFO;
              }
              else
              {
                _rThreadParserParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_ROUND_ROBIN;
              }
            }
            else
            {
              Rts_E = BOF_ERR_FORMAT;
            }
            break;

          case 'a':
            // this is used if we want to adjust only the affinity (directly)
            if (Bof_IsDecimal(pOption_c + 1, _rThreadParserParam_X.CoreChosen_U32))
            {
              _rThreadParserParam_X.AffinityCpuSet_U64 = ((uint64_t)1 << _rThreadParserParam_X.CoreChosen_U32);
            }
            else
            {
              Rts_E = BOF_ERR_FORMAT;
            }
            // CoreAffinity_U64 is 0 for below
            break;

          default:
            Rts_E = BOF_ERR_NOT_SUPPORTED;
            break;
        }

        if ((Rts_E == BOF_ERR_NO_ERROR) && (CoreAffinity_U64))
        {
          _rThreadParserParam_X.AffinityCpuSet_U64 = CoreAffinity_U64;
          _rThreadParserParam_X.CoreChosen_U32 = 0xFFFFFFFF;

          // BHATODO          le /2 est un param= nb proc phys

          for (Mask_U64 = 1, i_U32 = 0; i_U32 < sizeof(_rThreadParserParam_X.AffinityCpuSet_U64) * 8; i_U32++, Mask_U64 <<= 1) // affinityMask.size(); i++)
          {
            if (_rThreadParserParam_X.AffinityCpuSet_U64 & Mask_U64)
            {
              // just set affinity to the first enabled core
              if (_rThreadParserParam_X.CoreChosen_U32 == 0xFFFFFFFF)
              {
                _rThreadParserParam_X.CoreChosen_U32 = i_U32;
              }
              MidNbCore_U32 = (_rThreadParserParam_X.NbActiveCore_U32 / 2);
              if (ConsiderVirtualCore_B)
              {
                Core_U32 = (_rThreadParserParam_X.Node_U32 * MidNbCore_U32);
                if ((i_U32 % 2) != 0)
                {
                  Core_U32 = Core_U32 + (i_U32 / 2) + _rThreadParserParam_X.NbActiveCore_U32;
                }
                else
                {
                  Core_U32 = Core_U32 + (i_U32 / 2);
                }
              }
              else
              {
                Core_U32 = i_U32;
              }
              CoreAffinity_U64 = (uint64_t)1 << Core_U32;
            }
          }
        }

        _pThreadParameter_c += OptionLen_U32;
        if (pColon_c)
        {
          _pThreadParameter_c++;
        }
      }
      //      } while ((pColon_c) && (*_pThreadParameter_c) && (OptionLen_U32) && (OptionLen_U32 < sizeof(pOption_c)) && (Rts_E==BOF_ERR_NO_ERROR));
    } while ((*_pThreadParameter_c) && (Rts_E == BOF_ERR_NO_ERROR));
  }
  return Rts_E;
}

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
  // printf("%u: BOFTHREAD::V_ONPROCESSING mOnProcessing\n", BOF::Bof_GetMsTickCount());
  return Rts_E;
}

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
  BOFERR Sts_E;
  uint32_t Delta_U32;

  S_mBofThreadBalance++;
  // printf("%u: Start of thread '%s' BAL %d this %p\n", Bof_GetMsTickCount(), mThreadParam_X.Name_S.c_str(), S_mBofThreadBalance.load(), this);
  Sts_E = Bof_SignalEvent(mThreadEnterEvent_X, 0);
  // printf("%u: ENTER THREAD SIGNAL MTHREADENTEREVENT_X %d\n", BOF::Bof_GetMsTickCount(), Sts_E);
  BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    if (mThreadParam_X.ThreadCpuCoreAffinityMask_U64)
    {
#if defined(_WIN32)
      // A DWORD_PTR is not a pointer.It is an unsigned integer that is the same size as a pointer.Thus, in Win32 a DWORD_PTR is the same as a DWORD(32 bits), and in Win64 it is the same as a ULONGLONG(64 bits).
      //    DWORD_PTR Val = (DWORD_PTR)(1 << mCpuCoreAffinity_U32);
      DWORD_PTR Val = (DWORD_PTR)(mThreadParam_X.ThreadCpuCoreAffinityMask_U64);
      Sts_E = SetThreadAffinityMask(GetCurrentThread(), Val) ? BOF_ERR_NO_ERROR : BOF_ERR_ERANGE;
#else
      cpu_set_t CpuSet_X;
      uint32_t i_U32;
      uint64_t Mask_U64 = 1;
      CPU_ZERO(&CpuSet_X);
      //    CPU_SET(mCpuCoreAffinity_U32 - 1, &CpuSet_X);
      for (Mask_U64 = 1, i_U32 = 0; i_U32 < 64; i_U32++, Mask_U64 = Mask_U64 << 1)
      {
        if (Mask_U64 & mThreadParam_X.ThreadCpuCoreAffinityMask_U64)
        {
          CPU_SET(i_U32, &CpuSet_X);
        }
      }
#if defined(__EMSCRIPTEN__)
      Sts_E = BOF_ERR_NO_ERROR;
#else
      Sts_E = (sched_setaffinity(static_cast<__pid_t>(syscall(SYS_gettid)), sizeof(CpuSet_X), &CpuSet_X) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
#endif
#endif
    }
  }

  BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
#if defined(_WIN32)
    int32_t WndPrio_S32 = Bof_PriorityValueFromThreadPriority(mThreadParam_X.ThreadPriority_E);
    Sts_E = (SetThreadPriority(GetCurrentThread(), WndPrio_S32) == TRUE) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
    BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      Sts_E = (WndPrio_S32 == GetThreadPriority(GetCurrentThread())) ? BOF_ERR_NO_ERROR : BOF_ERR_PRIORITY;
    }

#else
#if defined(__EMSCRIPTEN__)
#else
    int Status_i = 0;
    int Policy_i = mThreadParam_X.ThreadSchedulerPolicy_E;
    struct sched_param Params_X;
    Sts_E = BOF_ERR_SCHEDULER;
    // Status_i = pthread_getschedparam(pthread_self(), &Policy_i, &Params_X);
    //  printf("0: Sts %d Pol %d Prio %d\n", Status_i, Policy_i, Params_X.sched_priority);

    Params_X.sched_priority = Bof_PriorityValueFromThreadPriority(mThreadParam_X.ThreadPriority_E);
    // Set the priority
    Status_i = pthread_setschedparam(pthread_self(), mThreadParam_X.ThreadSchedulerPolicy_E, &Params_X);
    // Verify
    // printf("1: Sts %d Pol %d Prio %d errno %d\n", Status_i, mPolicy_E, Params_X.sched_priority, errno);
    if (Status_i == 0)
    {
      Status_i = pthread_getschedparam(pthread_self(), &Policy_i, &Params_X);
      // printf("2: Sts %d Pol %d Prio %d (%d->%d)\n", Status_i, Policy_i, Params_X.sched_priority, mThreadParam_X.ThreadPriority_E, Bof_PriorityValueFromThreadPriority(mThreadParam_X.ThreadPriority_E));
      if (Status_i == 0)
      {
        Sts_E = ((Policy_i == mThreadParam_X.ThreadSchedulerPolicy_E) && (Params_X.sched_priority == Bof_PriorityValueFromThreadPriority(mThreadParam_X.ThreadPriority_E))) ? BOF_ERR_NO_ERROR : BOF_ERR_PRIORITY;
      }
    }
#endif
#endif
  }
  BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);

  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    V_OnCreate();
    while ((Sts_E == BOF_ERR_NO_ERROR) && (!mThreadMustStop_B))
    {
      if (mThreadParam_X.WakeUpIntervalInMs_U32)
      {
        Sts_E = Bof_WaitForEvent(mWakeUpEvent_X, mThreadParam_X.WakeUpIntervalInMs_U32, 0);
        if (Sts_E == BOF_ERR_ETIMEDOUT)
        {
          Sts_E = BOF_ERR_NO_ERROR;
        }
      }

      if (!mThreadMustStop_B)
      {
        if (Sts_E == BOF_ERR_NO_ERROR)
        {
          LockThreadCriticalSection(" BofThread::BofThread_Thread");
          // printf("%u: V_ONPROCESSING START mThreadLoopMustExit_B %d %s BAL %d\n", BOF::Bof_GetMsTickCount(), mThreadLoopMustExit_B.load(), mName_S.c_str(), S_BofThreadBalance());
          // Any other error code different from BOF_ERR_NO_ERROR will exit the tread loop
          // Returning BOF_ERR_EXIT_THREAD will exit the thread loop with an exit code of BOF_ERR_NO_ERROR
          // Thread will be stopped if someone calls  DestroyBofProcessingThread or destroy the object
          Sts_E = V_OnProcessing();
          UnlockThreadCriticalSection();
          if (Sts_E == BOF_ERR_EXIT_THREAD)
          {
            Sts_E = BOF_ERR_NO_ERROR;
            break;
          }
        }
      }
      // printf("%u: V_ONPROCESSING WHILE mThreadLoopMustExit_B %d Sts %d\n", BOF::Bof_GetMsTickCount(), mThreadLoopMustExit_B.load(), Sts_E);
    } // while
    V_OnStop();
  }
  Sts_E = Bof_SignalEvent(mThreadExitEvent_X, 0);
  S_mBofThreadBalance--;
  // Bof_ErrorCode can fail does to app shudown (static initializer)
  // printf("%u: BofThread_Thread End of thread '%s' BAL %d, ExitCode %d MustStop %d\n", Bof_GetMsTickCount(), mThreadParam_X.Name_S.c_str(), S_mBofThreadBalance.load(), Sts_E, mThreadMustStop_B.load());
  // BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
}
int BofThread::S_BofThreadBalance()
{
  return S_mBofThreadBalance.load();
}
#if 1
BofThreadPool::BofThreadPool(const BOF_THREAD_POOL_PARAM &_rThreadPoolParam_X)
{
  uint32_t i_U32;

  mThreadPoolParam_X = _rThreadPoolParam_X;
  for (i_U32 = 0; i_U32 < mThreadPoolParam_X.PoolSize_U32; i_U32++)
  {
    std::unique_ptr<BOF::BofThread> puThread = std::make_unique<BOF::BofThread>();
    puThread->SetThreadCallback(nullptr, BOF_BIND_0_ARG_TO_METHOD(this, BofThreadPool::OnProcessing), nullptr);
    puThread->LaunchBofProcessingThread(mThreadPoolParam_X.BaseName_S + "_" + std::to_string(i_U32), mThreadPoolParam_X.PriorityInversionAware_B, false, 0,
                                        mThreadPoolParam_X.ThreadSchedulerPolicy_E, mThreadPoolParam_X.ThreadPriority_E,
                                        mThreadPoolParam_X.ThreadCpuCoreAffinityMask_U64, 1000, mThreadPoolParam_X.StackSize_U32);
    mThreadCollection.emplace_back(std::move(puThread));
  }
}

BofThreadPool::~BofThreadPool()
{
  // Stop all worker threads...
  {
    std::unique_lock<std::mutex> Lock(mMtx);
    mDoShutdown_B = true;
  }
  mDoShedulCv.notify_all();
  // Join...
  for (auto &puThread : mThreadCollection)
  {
    puThread.reset(nullptr);
  }
}
bool BofThreadPool::Enqueue(std::function<void(void *)> _Fn, void *_pArg)
{
  bool Rts_B = false;
  THREAD_PARAM ThreadParam_X;
  {
    ThreadParam_X.Fn = _Fn;
    ThreadParam_X.pArg = _pArg;
    std::unique_lock<std::mutex> Lock(mMtx);
    if ((mThreadPoolParam_X.MaxQueuedRequests_U32 == 0) || (mJobCollection.size() < mThreadPoolParam_X.MaxQueuedRequests_U32))
    {
      mJobCollection.push_back(ThreadParam_X);
      Rts_B = true;
    }
  }
  mDoShedulCv.notify_one();
  return Rts_B;
}

BOFERR BofThreadPool::OnProcessing()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;

  while (1)
  {
    THREAD_PARAM ThreadParam_X;
    {
      std::unique_lock<std::mutex> Lock(mMtx);
      mDoShedulCv.wait(Lock, [&] { return ((!mJobCollection.empty()) || (mDoShutdown_B)); });
      if ((mDoShutdown_B) && (mJobCollection.empty()))
      {
        break;
      }
      ThreadParam_X = mJobCollection.front();
      mJobCollection.pop_front();
    }
    // assert(true == static_cast<bool>(Fn));
    ThreadParam_X.Fn(ThreadParam_X.pArg);
  }
  return Rts_E;
}
#else
class ThreadPoolExecutor
{
public:
  ThreadPoolExecutor(BofThreadPool *_pThreadPool, BOF_THREAD_POOL_ENTRY *_pThreadPoolEntry_X)
  {
    BOF_ASSERT(_pThreadPool != nullptr);
    BOF_ASSERT(_pThreadPoolEntry_X != nullptr);
    BOF_ASSERT(_pThreadPoolEntry_X->FctToExec != nullptr);
    mpThreadPool = _pThreadPool;
    mpThreadPoolEntry_X = _pThreadPoolEntry_X;
  }
  ~ThreadPoolExecutor()
  {
  }

  BOFERR V_OnCreate()
  {
    return BOF_ERR_NO_ERROR;
  }
  BOFERR V_OnProcessing()
  {
    BOFERR Rts_E = BOF_ERR_INTERNAL;

    if (mpThreadPoolEntry_X->FctToExec != nullptr)
    {
      mSts_E = mpThreadPoolEntry_X->FctToExec();
      // NO !! Rts_E = BOF_ERR_EXIT_THREAD;
      mpThreadPool->ReleaseDispatch(mSts_E, mpThreadPoolEntry_X, this);
      Rts_E = BOF_ERR_NO_ERROR;
    }
    return Rts_E;
  }
  BOFERR V_OnStop()
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;

    return Rts_E;
  }

private:
  BOF_THREAD_POOL_ENTRY *mpThreadPoolEntry_X = nullptr;
  BofThreadPool *mpThreadPool = nullptr;
  BOFERR mSts_E = BOF_ERR_NO_ERROR;
};
#define THREAD_POOL_ENTRY_MAGIC_NUMBER 0x251DFB65

BofThreadPool::BofThreadPool(uint32_t _NbThreadInPool_U32, const BOF_THREAD_PARAM &_rThreadParam_X)
{
  uint32_t i_U32;
  BofThread *pBofThread;
  BOF::BOF_POT_PARAM BofPotParam_X;
  BOF_THREAD_POOL_ENTRY *pThreadPoolEntry_X;

  mThreadParam_X = _rThreadParam_X;
  Bof_CreateMutex(mThreadParam_X.Name_S, false, mThreadParam_X.PriorityInversionAware_B, mMtxPendingDispatchCollection_X);
  if (mThreadParam_X.WakeUpIntervalInMs_U32)
  {
    mThreadPoolErrorCode_E = BOF_ERR_EINVAL;
  }
  else
  {
    mThreadParam_X.WakeUpIntervalInMs_U32 = 0xFFFFFFFF; // Block all thread at startup
    BofPotParam_X.Blocking_B = true;
    BofPotParam_X.MultiThreadAware_B = true;
    BofPotParam_X.GetOpPreserveContent_B = true;
    BofPotParam_X.PotCapacity_U32 = _NbThreadInPool_U32;
    BofPotParam_X.MagicNumber_U32 = THREAD_POOL_ENTRY_MAGIC_NUMBER;
    mpThreadCollection = new BOF::BofPot<BOF_THREAD_POOL_ENTRY>(BofPotParam_X);
    mThreadPoolErrorCode_E = mpThreadCollection ? mpThreadCollection->LastErrorCode() : BOF_ERR_ENOMEM;
    if (mThreadPoolErrorCode_E == BOF_ERR_NO_ERROR)
    {
      for (i_U32 = 0; i_U32 < _NbThreadInPool_U32; i_U32++)
      {
        pThreadPoolEntry_X = mpThreadCollection->GetIndexedPotElement(i_U32);
        if (pThreadPoolEntry_X)
        {
          mThreadPoolErrorCode_E = BOF_ERR_ENOMEM;
          pBofThread = new BofThread(mThreadParam_X.PriorityInversionAware_B);
          if (pBofThread)
          {
            mThreadPoolErrorCode_E = pBofThread->InitThreadErrorCode();
            if (mThreadPoolErrorCode_E != BOF_ERR_NO_ERROR)
            {
              break;
            }
            else
            {
              mThreadParam_X.Name_S = "Pool_" + std::to_string(i_U32) + '_' + _rThreadParam_X.Name_S;
              mThreadPoolErrorCode_E = pBofThread->LaunchBofProcessingThread(mThreadParam_X);
            }
          }
          if (mThreadPoolErrorCode_E != BOF_ERR_NO_ERROR)
          {
            break;
          }
          else
          {
            pThreadPoolEntry_X->pBofThread = pBofThread;
          }
        }
        else
        {
          mThreadPoolErrorCode_E = BOF_ERR_INTERNAL;
          break;
        }
      }
    }
  }
}
BofThreadPool::~BofThreadPool()
{
  uint32_t i_U32;
  BOF_THREAD_POOL_ENTRY *pThreadPoolEntry_X;

  if (mpThreadCollection)
  {
    // std::lock_guard<std::mutex> Lock(mMtxPendingDispatchCollection_X.Mtx);
    for (i_U32 = 0; i_U32 < mpThreadCollection->GetCapacity(); i_U32++)
    {
      pThreadPoolEntry_X = mpThreadCollection->GetIndexedPotElement(i_U32);
      if (pThreadPoolEntry_X)
      {
        BOF_SAFE_DELETE(pThreadPoolEntry_X->pBofThread);
        pThreadPoolEntry_X->pBofThread = nullptr;
      }
    }
    BOF_SAFE_DELETE(mpThreadCollection);
  }
  Bof_DestroyMutex(mMtxPendingDispatchCollection_X);
}
BOFERR BofThreadPool::InitThreadPoolErrorCode()
{
  return mThreadPoolErrorCode_E;
}

BOFERR BofThreadPool::Dispatch(uint32_t TimeoutInMs_U32, BOF_THREAD_CALLBACK _FctToExec, void **_ppDispatchTicket)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BOF_THREAD_POOL_ENTRY *pThreadPoolEntry_X;
  ThreadPoolExecutor *pThreadPoolExecutor;

  if (_ppDispatchTicket)
  {
    *_ppDispatchTicket = nullptr;
    Rts_E = BOF_ERR_EBUSY;
    pThreadPoolEntry_X = mpThreadCollection->Get(TimeoutInMs_U32);
    if (pThreadPoolEntry_X)
    {
      Rts_E = BOF_ERR_ENOMEM;
      pThreadPoolEntry_X->FctToExec = _FctToExec;
      pThreadPoolExecutor = new ThreadPoolExecutor(this, pThreadPoolEntry_X); // Deleted by ReleaseDispatch
      if (pThreadPoolExecutor)
      {
        pThreadPoolEntry_X->pBofThread->SetThreadCallback(BOF_BIND_0_ARG_TO_METHOD(pThreadPoolExecutor, ThreadPoolExecutor::V_OnCreate), BOF_BIND_0_ARG_TO_METHOD(pThreadPoolExecutor, ThreadPoolExecutor::V_OnProcessing), BOF_BIND_0_ARG_TO_METHOD(pThreadPoolExecutor, ThreadPoolExecutor::V_OnStop));
        Rts_E = pThreadPoolEntry_X->pBofThread->SignalThreadWakeUpEvent();
      }
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        pThreadPoolEntry_X->Running_B = true;
        printf("Dispatch ptr %p running ...\n", pThreadPoolEntry_X);
        *_ppDispatchTicket = pThreadPoolEntry_X;
      }
      else
      {
        BOF_SAFE_DELETE(pThreadPoolExecutor);
      }
    }
  }
  return Rts_E;
}
BOFERR BofThreadPool::AckPendingDispatch(uint32_t _TimeoutInMs_U32, const void *_pDispatchTicket)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BOF_THREAD_POOL_ENTRY *pThreadPoolEntry_X = (BOF_THREAD_POOL_ENTRY *)_pDispatchTicket;
  uint32_t Start_U32, Delta_U32;

  if (pThreadPoolEntry_X)
  {
    Rts_E = mpThreadCollection->IsPotElementInUse(pThreadPoolEntry_X);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Start_U32 = Bof_GetMsTickCount();
      do
      {
        Delta_U32 = Bof_ElapsedMsTime(Start_U32);
        if (pThreadPoolEntry_X->Running_B)
        {
          if (_TimeoutInMs_U32)
          {
            Bof_MsSleep(10);
            Delta_U32 = Bof_ElapsedMsTime(Start_U32);
            Rts_E = BOF_ERR_ETIMEDOUT;
          }
          else
          {
            Rts_E = BOF_ERR_BAD_STATUS;
            break;
          }
        }
        else
        {
          std::lock_guard<std::mutex> Lock(mMtxPendingDispatchCollection_X.Mtx);
          {
            auto It = std::find(mPendingDispatchCollection.begin(), mPendingDispatchCollection.end(), pThreadPoolEntry_X);
            BOF_ASSERT(It != mPendingDispatchCollection.end());
            if (It != mPendingDispatchCollection.end())
            {
              mPendingDispatchCollection.erase(It);
            }
          }
          // Do not use any object var after this call because the object has been deleted by ->Release
          Rts_E = mpThreadCollection->Release(pThreadPoolEntry_X);
          BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
          break;
        }
      } while (Delta_U32 < _TimeoutInMs_U32);
    }
  }
  return Rts_E;
}
std::string BofThreadPool::GetDispatchName(const void *_pDispatchTicket)
{
  std::string Rts_S;
  BOF_THREAD_POOL_ENTRY *pThreadPoolEntry_X = (BOF_THREAD_POOL_ENTRY *)_pDispatchTicket;

  if ((pThreadPoolEntry_X) && (pThreadPoolEntry_X->MagicNumber_U32 == THREAD_POOL_ENTRY_MAGIC_NUMBER))
  {
    if (pThreadPoolEntry_X->pBofThread)
    {
      Rts_S = pThreadPoolEntry_X->pBofThread->ThreadName();
    }
  }
  return Rts_S;
}

uint32_t BofThreadPool::GetNumerOfPendingRunningDispatch()
{
  return mpThreadCollection ? mpThreadCollection->GetNbElementOutOfThePot() : 0;
}
uint32_t BofThreadPool::GetNumerOfPendingDispatchToAck()
{
  return mPendingDispatchCollection.size();
}
void *BofThreadPool::GetFirstPendingDispatch() // Next should call AckPendingDispatch if rts is not nullptr
{
  void *pRts = nullptr;
  std::lock_guard<std::mutex> Lock(mMtxPendingDispatchCollection_X.Mtx);
  {
    if (mPendingDispatchCollection.size())
    {
      pRts = mPendingDispatchCollection[0];
    }
  }
  return pRts;
}
BOFERR BofThreadPool::ReleaseDispatch(BOFERR _Sts_E, BOF_THREAD_POOL_ENTRY *_pThreadPoolEntry_X, ThreadPoolExecutor *_pThreadPoolExecutor)
{
  BOFERR Rts_E = _Sts_E;

  // No done in AckPendingDispatch to avoid race condition  mpThreadCollection->Release(_pThreadPoolEntry_X);
  BOF_ASSERT(_pThreadPoolEntry_X->Running_B);
  _pThreadPoolEntry_X->Running_B = false;
  std::lock_guard<std::mutex> Lock(mMtxPendingDispatchCollection_X.Mtx);
  {
    auto It = std::find(mPendingDispatchCollection.begin(), mPendingDispatchCollection.end(), _pThreadPoolEntry_X);
    BOF_ASSERT(It == mPendingDispatchCollection.end());
    if (It == mPendingDispatchCollection.end())
    {
      mPendingDispatchCollection.push_back(_pThreadPoolEntry_X);
    }
  }

  BOF_SAFE_DELETE(_pThreadPoolExecutor);
  return Rts_E;
}
#endif
END_BOF_NAMESPACE()