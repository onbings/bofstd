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
#include <bofstd/bofthread.h>
#include <bofstd/bofstring.h>
#include <bofstd/bofstringformatter.h>

#if defined (_WIN32)
//https://stackoverflow.com/questions/10121560/stdthread-naming-your-thread
#include <windows.h>
const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
  DWORD dwType; // Must be 0x1000.
  LPCSTR szName; // Pointer to name (in user addr space).
  DWORD dwThreadID; // Thread ID (-1=caller thread).
  DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

#else

#endif
//#include <bofstd/boflogger.h>
//BofLogger::S_Instance().Log("IpSwitcherLog", BOF::BOF::CRITICAL, "BofSocketIo[%d] DoINeedToConnect_B", BOF::Bof_GetMsTickCount());

BEGIN_BOF_NAMESPACE()
#if defined (_WIN32)
#else

#include <unistd.h>
#include <sys/syscall.h>

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
  mName_S = _rName_S;
  mMtx_X.Reset();
  mLockBalance.store(0);
  mpLastLocker_c[0] = 0;
  mWakeUpEvent_X.Reset();
  mThreadErrorCode_E = BOF_ERR_ENOMEM;
  mThreadErrorCode_E = Bof_CreateMutex(_rName_S + "_mtx", true, true, mMtx_X);
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
          //        printf("%d %s %X====>InitializeThread\n", Bof_GetMsTickCount(), mName_S.c_str(), mThreadHandle);
        }
      }
    }
  }
  //printf("====> Thread %s init at %d\n", mName_S.c_str(), Bof_GetMsTickCount());

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
BOFERR BofThread::LaunchBofProcessingThread(const std::string &_rName_S, bool _SignalEvent_B, uint32_t _WakeUpIntervalInMs_U32, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF_THREAD_PRIORITY _ThreadPriority_E, uint64_t _ThreadCpuCoreAffinityMask_U64,
                                            uint32_t _StartStopTimeoutInMs_U32, uint32_t /*_StackSize_U32*/)
{
  BOFERR              Rts_E;
  bool                ThreadRunning_B;
  BOF_THREAD_PRIORITY Min_E, Max_E;

  Rts_E   = BOF_ERR_INVALID_STATE;
  mName_S = _rName_S;
  if ((!Bof_IsEventSignaled(mThreadEnterEvent_X, 0)) && (!Bof_IsEventSignaled(mThreadExitEvent_X, 0)))
  {
    Rts_E = BOF_ERR_NO_ERROR;
    if ((_ThreadSchedulerPolicy_E == BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER) && (_ThreadPriority_E != BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000) && (_ThreadPriority_E != BOF_THREAD_DEFAULT_PRIORITY))
    {
      Rts_E = BOF_ERR_EINVAL;
    }
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      mWakeUpIntervalInMs_U32   = _WakeUpIntervalInMs_U32;
      mCpuCoreAffinityMask_U64  = _ThreadCpuCoreAffinityMask_U64;
      mPolicy_E                 = _ThreadSchedulerPolicy_E;
      mPriority_E               = _ThreadPriority_E;
      mStartStopTimeoutInMs_U32 = _StartStopTimeoutInMs_U32;
      if (mPriority_E == BOF_THREAD_DEFAULT_PRIORITY)
      {
        mPriority_E = (Bof_GetThreadPriorityRange(mPolicy_E, Min_E, Max_E) == BOF_ERR_NO_ERROR) ? (BOF_THREAD_PRIORITY) ((Max_E + Min_E) / 2) : (BOF_THREAD_PRIORITY) (0);
      }
      mThread                   = std::thread(&BofThread::BofThread_Thread, this);
      mThreadHandle             = mThread.native_handle();  //Its value disappear after a join or a detach http://www.bo-yang.net/2017/11/19/cpp-kill-detached-thread
      //printf("====> Thread %s launched at %d\n", mName_S.c_str(), Bof_GetMsTickCount());

      if (!mStartStopTimeoutInMs_U32)
      {
        ThreadRunning_B = true; // In this case we consider that the thread will start one day as the thread creation is ok but we do not want to wait
      }
      else
      {
        ThreadRunning_B = (Bof_WaitForEvent(mThreadEnterEvent_X, mStartStopTimeoutInMs_U32, 0) == BOF_ERR_NO_ERROR);
      }

      //printf("====> Thread %s Running %d ? at %d\n", mName_S.c_str(), ThreadRunning_B, Bof_GetMsTickCount());
      if (ThreadRunning_B)
      {
        Rts_E = BOF_ERR_NO_ERROR;
        if (mName_S != "")
        {
          //Linux: the thread name is a meaningful C language string, whose length is restricted to 16 characters, including the terminating null byte ('\0').
          if (mName_S.length() > 15)
          {
            mName_S = mName_S.substr(0, 15);
          }
#if defined(_WIN32)
          THREADNAME_INFO ThreadNameInfo_X;
          ThreadNameInfo_X.dwType = 0x1000;
          ThreadNameInfo_X.szName = mName_S.c_str();
          ThreadNameInfo_X.dwThreadID = ::GetThreadId(static_cast<HANDLE>(mThread.native_handle()));
          ThreadNameInfo_X.dwFlags = 0;

  //This stop google test from running in command console as it detect the exception... but it works inside the vs ide
          //	__try
          {
          //	RaiseException(MS_VC_EXCEPTION, 0, sizeof(ThreadNameInfo_X) / sizeof(ULONG_PTR), (ULONG_PTR*)&ThreadNameInfo_X);
          }
          //__except (EXCEPTION_EXECUTE_HANDLER)
          {
          }
#else
//        pthread_setname_np(pthread_self(), mName_S.c_str());

          pthread_setname_np(static_cast<pthread_t>(mThread.native_handle()), mName_S.c_str());
#endif
        }
        if (_SignalEvent_B)
        {
          Rts_E = Bof_SignalEvent(mWakeUpEvent_X, 0);
          //printf("Bof_SignalEvent %d\n", Rts_E);
        }
      }
      else
      {
        Rts_E = DestroyBofProcessingThread("LaunchThread");     // Thread has not started in the given time slot->MUST destroy it
        //printf("DestroyBofProcessingThread %d\n",Rts_E);
      }
    }
  }
  //printf("LaunchBofProcessingThread %d\n", Rts_E);

  return Rts_E;
}

BOFERR BofThread::DestroyBofProcessingThread(const char * /*_pUser_c*/)
{
  BOFERR Rts_E          = BOF_ERR_INVALID_STATE;
  bool   ThreadStopTo_B = false;

  //printf("====> DestroyThread %s ThreadLoopMustExit %d ? Signal Enter %d Exit %d WakeUp %d this %p at %d\n", mName_S.c_str(), mThreadLoopMustExit_B,  Bof_IsEventSignaled(mThreadEnterEvent_X,0), Bof_IsEventSignaled(mThreadExitEvent_X,0),mWakeUpIntervalInMs_U32, static_cast<void *>(this), Bof_GetMsTickCount());
//  printf("%d %s %X====>DestroyBofProcessingThread start phase 1\n", Bof_GetMsTickCount(), mName_S.c_str(), mThreadHandle);
  if (Bof_IsEventSignaled(mThreadEnterEvent_X, 0))
  {
    Rts_E = BOF_ERR_NO_ERROR;
    if (!Bof_IsEventSignaled(mThreadExitEvent_X, 0))
    {
      mThreadLoopMustExit_B = true;
      SignalThreadWakeUpEvent();
      if (!mThreadExitPosted_B)
      {
        //      printf("====> Wait for %d t %d\n", mStartStopTimeoutInMs_U32, Bof_GetMsTickCount());
        ThreadStopTo_B = (Bof_WaitForEvent(mThreadExitEvent_X, mStartStopTimeoutInMs_U32, 0) != BOF_ERR_NO_ERROR);
        //    printf("====> End of Wait --> %d t %d\n", ThreadStopTo_B, Bof_GetMsTickCount());
        if (ThreadStopTo_B)
        {
          //    printf("====>Cannot stop thread p=%p val %d\n", static_cast<void *>(&mThreadLoopMustExit_B) ,mThreadLoopMustExit_B);
        }
      }
    }
  }
  // printf("%d %s %X====>DestroyBofProcessingThread end of phase 1, timeout %d ?\n", Bof_GetMsTickCount(), mName_S.c_str(), mThreadHandle, ThreadStopTo_B);
  if (ThreadStopTo_B)
  {
    // printf("%d %s %X====>Can't stop, start phase 2->Kill it\n", Bof_GetMsTickCount(), mName_S.c_str(), mThreadHandle);
#if defined (_WIN32)
    TerminateThread(static_cast<HANDLE>(mThreadHandle),0x69696969);
#else
    pthread_cancel(static_cast<pthread_t>(mThreadHandle));
#endif
  }
  //printf("%d %s %X====>Is joinable %d\n", Bof_GetMsTickCount(), mName_S.c_str(), mThreadHandle, mThread.joinable());
  if (mThread.joinable())
  {
//    printf("%d %s %X====>Start joining\n", Bof_GetMsTickCount(), mName_S.c_str(), mThreadHandle);
    mThread.join();
    //printf("%d %s %X====>Join done\n", Bof_GetMsTickCount(), mName_S.c_str(), mThreadHandle);
  }
  //printf("%d %s %X====>DestroyThread finished with Rts %d\n", Bof_GetMsTickCount(), mName_S.c_str(), mThreadHandle, Rts_E);
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

//Leave a space in front of _pLocker to insert a '+' Lock or '-' unlock (see below)
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
  return DestroyBofProcessingThread(_pUser_c);;
}


//Used to specify callback if the caller does not inherit from BofThread
void BofThread::SetThreadCallback(BOF_THREAD_CALLBACK _OnCreate, BOF_THREAD_CALLBACK _OnProcessing, BOF_THREAD_CALLBACK _OnStop)
{
  LockThreadCriticalSection(" BofThread::SetThreadCallback");
  mOnCreate     = _OnCreate;
  mOnProcessing = _OnProcessing;
  mOnStop       = _OnStop;
  UnlockThreadCriticalSection();

}


std::string BofThread::S_ToString(const BOF_THREAD_PARAM &_rThreadParam_X, bool _ShowChosenCore_B)
{
  std::string Rts_S;
  uint32_t    i_U32, pRange_U32[2];
  uint64_t    Mask_U64;
  char        *p_c, pBuffer_c[0x1000], pSchedulerPolicy_c[] = {'o', 'f', 'r'};  //BOF_THREAD_SCHEDULER_POLICY_OTHER,BOF_THREAD_SCHEDULER_POLICY_FIFO,BOF_THREAD_SCHEDULER_ROUND_ROBIN
  bool        AlreadyOne_B;

  p_c   = pBuffer_c;
  if (!_rThreadParam_X.AffinityCpuSet_U64)
  {
    p_c += sprintf(p_c, "n%d:%c%d", _rThreadParam_X.Node_U32, (_rThreadParam_X.SchedulerPolicy_E < BOF_THREAD_SCHEDULER_POLICY_MAX) ? pSchedulerPolicy_c[_rThreadParam_X.SchedulerPolicy_E] : '?', Bof_ValueFromThreadPriority(_rThreadParam_X.Priority_E));
  }
  else
  {
    p_c += sprintf(p_c, "n%d:%c%d:c", _rThreadParam_X.Node_U32, (_rThreadParam_X.SchedulerPolicy_E < BOF_THREAD_SCHEDULER_POLICY_MAX) ? pSchedulerPolicy_c[_rThreadParam_X.SchedulerPolicy_E] : '?', Bof_ValueFromThreadPriority(_rThreadParam_X.Priority_E));
    pRange_U32[0] = 0xFFFFFFFF;
    pRange_U32[1] = 0xFFFFFFFF;
    AlreadyOne_B  = false;
    for (Mask_U64 = 1, i_U32 = 0; i_U32 < (sizeof(_rThreadParam_X.AffinityCpuSet_U64) * 8); i_U32++, Mask_U64 <<= 1) //affinityMask.size(); i++)
    {
      if (_rThreadParam_X.AffinityCpuSet_U64 & Mask_U64)
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
            p_c += sprintf(p_c, "%d-%d", pRange_U32[0], i_U32 - 1);  // _rThreadParam_X.NbActiveCore_U32);
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
    p_c += sprintf(p_c, " A%d/%d", _rThreadParam_X.CoreChosen_U32, _rThreadParam_X.NbActiveCore_U32);
  }
  Rts_S = pBuffer_c;
  return Rts_S;
}

BOFERR BofThread::S_AffinityMaskFromString(const char *_pAffinityOptionString_c, uint32_t _NbCore_U32, uint64_t &_rAffinityMask_U32)
{
  BOFERR     Rts_E = BOF_ERR_EINVAL;
  uint64_t   Mask_U64;
  uint32_t   i_U32, OptionLen_U32, SubOptionLen_U32, pRange_U32[2];
  const char *pComa_c, *pMinus_c;
  char       pOption_c[256];

  _rAffinityMask_U32 = 0;
  if (_pAffinityOptionString_c)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    _pAffinityOptionString_c++;
    do
    {
      pComa_c       = strchr(_pAffinityOptionString_c, ',');
      OptionLen_U32 = pComa_c ? static_cast<uint32_t>(pComa_c - _pAffinityOptionString_c) : static_cast<uint32_t>(strlen(_pAffinityOptionString_c));
      if ((OptionLen_U32) && (OptionLen_U32 < sizeof(pOption_c)))
      {
        strncpy(pOption_c, _pAffinityOptionString_c, OptionLen_U32);
        pOption_c[OptionLen_U32] = 0;

        pRange_U32[0] = -1;
        pRange_U32[1] = -1;
        pMinus_c         = strchr(pOption_c, '-');
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
            for (Mask_U64 = ((uint64_t) 1 << pRange_U32[0]), i_U32 = pRange_U32[0]; i_U32 <= pRange_U32[1]; i_U32++, Mask_U64 <<= 1)
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
          _rAffinityMask_U32 |= ((uint64_t) 1 << pRange_U32[0]);
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

BOFERR BofThread::S_ThreadParameterFromString(const char *_pThreadParameter_c, BOF_THREAD_PARAM &_rThreadParam_X)
{
  BOFERR     Rts_E = BOF_ERR_EINVAL;
  const char *pColon_c;
  char       pOption_c[256];
  uint32_t   OptionLen_U32, Core_U32, MidNbCore_U32, i_U32, Val_U32;
  uint64_t   CoreAffinity_U64, Mask_U64;
  bool       ConsiderVirtualCore_B;

  if (_pThreadParameter_c)
  {
    memset(&_rThreadParam_X, 0, sizeof(BOF_THREAD_PARAM));
    _rThreadParam_X.NbActiveCore_U32 = std::thread::hardware_concurrency();  //sysconf(_SC_NPROCESSORS_ONLN);
//    printk("num_online_cpus %d num_possible_cpus() %d num_present_cpus() %d num_active_cpus %d\n",num_online_cpus()	,num_possible_cpus()	, num_present_cpus(),num_active_cpus());
    do
    {
      Rts_E = BOF_ERR_PARSER;
			ConsiderVirtualCore_B = false;

      pColon_c      = strchr(_pThreadParameter_c, ':');
      OptionLen_U32 = pColon_c ? static_cast<uint32_t>(pColon_c - _pThreadParameter_c) : static_cast<uint32_t>(strlen(_pThreadParameter_c));
      if ((OptionLen_U32) && (OptionLen_U32 < sizeof(pOption_c)))
      {
        strncpy(pOption_c, _pThreadParameter_c, OptionLen_U32);
        pOption_c[OptionLen_U32] = 0;
        CoreAffinity_U64 = 0;
        Rts_E            = BOF_ERR_NO_ERROR;
        switch (pOption_c[0])
        {
          case 'n':
            if (!Bof_IsDecimal(pOption_c + 1, _rThreadParam_X.Node_U32))
            {
              Rts_E = BOF_ERR_FORMAT;
            }
            break;

          case 'c':
            ConsiderVirtualCore_B = true;
            //Rts_E = S_AffinityMaskFromString(pOption_c, _rThreadParam_X.NbActiveCore_U32, CoreAffinity_U64);
            Rts_E = S_AffinityMaskFromString(pOption_c, 0xFFFFFFFF, CoreAffinity_U64);
            break;

          case 'C':
//            Rts_E = S_AffinityMaskFromString(pOption_c, _rThreadParam_X.NbActiveCore_U32 / 2, CoreAffinity_U64);
            Rts_E = S_AffinityMaskFromString(pOption_c, 0xFFFFFFFF, CoreAffinity_U64);
            break;

          case 'o':
          case 'f':
          case 'r':
            if (Bof_IsDecimal(pOption_c + 1, Val_U32))
            {
              _rThreadParam_X.Priority_E = Bof_ThreadPriorityFromValue(static_cast<BOF_THREAD_PRIORITY>(Val_U32));
              if (pOption_c[0] == 'o')
              {
                _rThreadParam_X.SchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER;
              }
              else if (pOption_c[0] == 'f')
              {
                _rThreadParam_X.SchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_FIFO;
              }
              else
              {
                _rThreadParam_X.SchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_ROUND_ROBIN;
              }
            }
            else
            {
              Rts_E = BOF_ERR_FORMAT;
            }
            break;

          case 'a':
            // this is used if we want to adjust only the affinity (directly)
            if (Bof_IsDecimal(pOption_c + 1, _rThreadParam_X.CoreChosen_U32))
            {
              _rThreadParam_X.AffinityCpuSet_U64 = ((uint64_t) 1 << _rThreadParam_X.CoreChosen_U32);
            }
            else
            {
              Rts_E = BOF_ERR_FORMAT;
            }
            //CoreAffinity_U64 is 0 for below
            break;

          default:
            Rts_E = BOF_ERR_NOT_SUPPORTED;
            break;
        }

        if ((Rts_E == BOF_ERR_NO_ERROR) && (CoreAffinity_U64))
        {
          _rThreadParam_X.AffinityCpuSet_U64 = CoreAffinity_U64;
          _rThreadParam_X.CoreChosen_U32     = 0xFFFFFFFF;


//BHATODO          le /2 est un param= nb proc phys


          for (Mask_U64 = 1, i_U32 = 0; i_U32 < sizeof(_rThreadParam_X.AffinityCpuSet_U64) * 8; i_U32++, Mask_U64 <<= 1) //affinityMask.size(); i++)
          {
            if (_rThreadParam_X.AffinityCpuSet_U64 & Mask_U64)
            {
              // just set affinity to the first enabled core
              if (_rThreadParam_X.CoreChosen_U32 == 0xFFFFFFFF)
              {
                _rThreadParam_X.CoreChosen_U32 = i_U32;
              }
              MidNbCore_U32    = (_rThreadParam_X.NbActiveCore_U32 / 2);
              if (ConsiderVirtualCore_B)
              {
                Core_U32 = (_rThreadParam_X.Node_U32 * MidNbCore_U32);
                if ((i_U32 % 2) != 0)
                {
                  Core_U32 = Core_U32 + (i_U32 / 2) + _rThreadParam_X.NbActiveCore_U32;
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
              CoreAffinity_U64 = (uint64_t) 1 << Core_U32;
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
  BOFERR   Sts_E = BOF_ERR_NO_ERROR;
  uint32_t Delta_U32;

  //printf("====> Thread start\n");
//  printf("%d %s %X====>Thread running...\n", Bof_GetMsTickCount(), mName_S.c_str(), mThreadHandle);

  Sts_E = Bof_SignalEvent(mThreadEnterEvent_X, 0);
  BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
  //mThreadRunning_B = true;

#if defined (_WIN32)
  if (mCpuCoreAffinityMask_U64)
  {
  // A DWORD_PTR is not a pointer.It is an unsigned integer that is the same size as a pointer.Thus, in Win32 a DWORD_PTR is the same as a DWORD(32 bits), and in Win64 it is the same as a ULONGLONG(64 bits).
//    DWORD_PTR Val = (DWORD_PTR)(1 << mCpuCoreAffinity_U32);
    DWORD_PTR Val = (DWORD_PTR)(mCpuCoreAffinityMask_U64);
    SetThreadAffinityMask(GetCurrentThread(), Val);
  }
  Sts_E = (SetThreadPriority(GetCurrentThread(), mPriority_E) == TRUE) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
  //int32_t Win32Err_S32 = 0;
  //Bof_GetLastError(false, &Win32Err_S32);
  //printf("SetThreadPriority(%d)=%d\n", mPriority_E, Sts_E);
#else
  if (mCpuCoreAffinityMask_U64)
  {
    cpu_set_t CpuSet_X;
    uint32_t  i_U32;
    uint64_t  Mask_U64 = 1;
    CPU_ZERO(&CpuSet_X);
//    CPU_SET(mCpuCoreAffinity_U32 - 1, &CpuSet_X);
    for (Mask_U64 = 1, i_U32 = 0; i_U32 < 64; i_U32++, Mask_U64 = Mask_U64 << 1)
    {
      if (Mask_U64 & mCpuCoreAffinityMask_U64)
      {
        CPU_SET(i_U32, &CpuSet_X);
      }
    }
    Sts_E = (sched_setaffinity(static_cast<__pid_t>(syscall(SYS_gettid)), sizeof(CpuSet_X), &CpuSet_X) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
  }

  int                Status_i = 0;
  int                Policy_i = mPolicy_E;
  struct sched_param Params_X;

  Sts_E = BOF_ERR_INTERNAL;
  Params_X.sched_priority = mPriority_E;
  // Set the priority
  Status_i = pthread_setschedparam(pthread_self(), mPolicy_E, &Params_X);
  // Verify
  if (Status_i == 0)
  {
    Status_i = pthread_getschedparam(pthread_self(), &Policy_i, &Params_X);
    Sts_E    = ((Policy_i == mPolicy_E) && (Params_X.sched_priority == mPriority_E)) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
  }
#endif
  //printf("====> Thread starting %d\n", Sts_E);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    V_OnCreate();
    //printf("====> Thread %s ThreadLoopMustExit %d ? Signal Enter %d Exit %d WakeUp %d Sts %d this %p at %d\n", mName_S.c_str(), mThreadLoopMustExit_B,  Bof_IsEventSignaled(mThreadEnterEvent_X), Bof_IsEventSignaled(mThreadExitEvent_X),mWakeUpIntervalInMs_U32,Sts_E, this, Bof_GetMsTickCount());
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
        Sts_E = V_OnProcessing();  //Return BOF_ERR_CANCEL to exit without calling V_OnStop (underlying thread oject has been destroyed). Any other error code different from BOF_ERR_NO_ERROR will exit AND call V_OnStop
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
          Delta_U32++;  //Put a breakpoint here to detect full speed thread loop
        }
        mLoopTimerWarning_U32 = Bof_GetMsTickCount();
      }
    } //while
    //  printf("====> Thread %s exit %d at %d\n", mName_S.c_str(), Sts_E, Bof_GetMsTickCount());

    if (Sts_E != BOF_ERR_CANCEL)
    {
      V_OnStop();
    }
  }
//	printf("====> Signal %s at %d\n", mName_S.c_str(), Bof_GetMsTickCount());
  //mThreadRunning_B = false;
  Sts_E = Bof_SignalEvent(mThreadExitEvent_X, 0);
  BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
//  printf("%d %s %X====>Thread exit\n", Bof_GetMsTickCount(), mName_S.c_str(), mThreadHandle);
}

END_BOF_NAMESPACE()