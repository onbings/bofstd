/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines async class helper
 *
 * Name:        bofasync.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  May 26 2020  BHA : Initial release
 */
#pragma once

#include <asyncmulticastdelegate/DelegateLib.h>
#include <bofstd/bofcircularbuffer.h>

BEGIN_BOF_NAMESPACE()

struct BOF_COMMAND_QUEUE_PARAM
{
  uint64_t ThreadCpuCoreAffinityMask_U64;
  BOF_THREAD_SCHEDULER_POLICY ThreadSchedulerPolicy_E;
  BOF_THREAD_PRIORITY ThreadPriority_E;
  uint32_t MaxPendingRequest_U32;
  uint32_t PollTimeoutInMs_U32;

  BOF_COMMAND_QUEUE_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    ThreadCpuCoreAffinityMask_U64 = 0;
    ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
    ThreadPriority_E = BOF_THREAD_PRIORITY_000;
    MaxPendingRequest_U32 = 0;
    PollTimeoutInMs_U32 = 0;
  }
};
struct BOF_COMMAND_QUEUE_ENTRY
{
  std::string Name_S;
  std::function<void()> Cmd;

  BOF_COMMAND_QUEUE_ENTRY()
  {
    Name_S = "";
    Cmd = nullptr;
  }
  BOF_COMMAND_QUEUE_ENTRY(std::string _Name_S, std::function<void()> _Cmd)
  {
    Name_S = _Name_S;
    Cmd = _Cmd;
  }
};
class BOFSTD_EXPORT BofCommandQueue : public BofThread
{
public:
  BofCommandQueue(const BOF_COMMAND_QUEUE_PARAM &_rCommandQueueParam_X);
  virtual ~BofCommandQueue();

  BOFERR PostCommand(bool _OnlyOne_B, const BOF_COMMAND_QUEUE_ENTRY &_rCommand_X);
  bool IsProcessingCommand() const;
  bool IsCommandPending() const;
  uint32_t NumberOfCommandWaitingInQueue() const;
  BOFERR ClearCommandQueue();

private:
  BOF_COMMAND_QUEUE_PARAM mCommandQueueParam_X;
  std::unique_ptr<BofCircularBuffer<BOF_COMMAND_QUEUE_ENTRY>> mpuCommandEntryCollection = nullptr;
  //	std::atomic<bool> mCommandEntryPending;
  BOF_COMMAND_QUEUE_ENTRY mCommandPending_X;
  BOFERR V_OnProcessing() override;
};

template <class T>
using BOF_MULTICAST_ASYNC_NOTIFY_FCT = void (*)(const T *_pNotifyArg);
// typedef void (*BOF_MULTICAST_ASYNC_NOTIFY_FCT)(const BOF_MULTICAST_ASYNC_NOTIFY_ARG *_pNotifyArg_X);

struct BOF_MULTICAST_ASYNC_NOTIFIER_PARAM
{
  uint64_t ThreadCpuCoreAffinityMask_U64;
  BOF_THREAD_SCHEDULER_POLICY ThreadSchedulerPolicy_E;
  BOF_THREAD_PRIORITY ThreadPriority_E;
  // uint32_t                    MaxPendingRequest_U32;  //0 means no limit

  BOF_MULTICAST_ASYNC_NOTIFIER_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    ThreadCpuCoreAffinityMask_U64 = 0;
    ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
    ThreadPriority_E = BOF_THREAD_PRIORITY_000;
    // MaxPendingRequest_U32 = 0;
  }
};

template <class T>
class BofMulticastAsyncNotifier
{
public:
  BofMulticastAsyncNotifier(const BOF_MULTICAST_ASYNC_NOTIFIER_PARAM &_rAsyncNotifierParam_X);
  virtual ~BofMulticastAsyncNotifier();
  uint32_t NbPendingNotification();

  //  BOFERR Register(BOF_MULTICAST_ASYNC_NOTIFY_FCT _pNotifyFct);
  //  BOFERR Unregister(BOF_MULTICAST_ASYNC_NOTIFY_FCT _pNotifyFct);
  BOFERR Register(BOF_MULTICAST_ASYNC_NOTIFY_FCT<T> _pNotifyFct, void *_pUserContext);
  BOFERR Unregister(BOF_MULTICAST_ASYNC_NOTIFY_FCT<T> _pNotifyFct);
  BOFERR Notify(const T *_pNotifyArg_X);
  BOFERR WaitForNoMoreNotificationPending(uint32_t _PollTimeInMs_U32, uint32_t _TimeoutInMs_U32);

private:
  BofMsgThread mMsgThread;
  DelegateLib::MulticastDelegateSafe1<const T *> mMulticastDelegate;
};

template <class T>
BofMulticastAsyncNotifier<T>::BofMulticastAsyncNotifier(const BOF_MULTICAST_ASYNC_NOTIFIER_PARAM &_rAsyncNotifierParam_X)
{
  BOFERR Sts_E;

  Sts_E = mMsgThread.LaunchBofProcessingThread("BofAsyncNotif", false, 0, _rAsyncNotifierParam_X.ThreadSchedulerPolicy_E, _rAsyncNotifierParam_X.ThreadPriority_E, _rAsyncNotifierParam_X.ThreadCpuCoreAffinityMask_U64, 2000, 0);
  BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
}
template <class T>
BofMulticastAsyncNotifier<T>::~BofMulticastAsyncNotifier()
{
  mMulticastDelegate.Clear();
}
template <class T>
uint32_t BofMulticastAsyncNotifier<T>::NbPendingNotification()
{
  return mMsgThread.GetNbPendingRequest();
}

template <class T>
BOFERR BofMulticastAsyncNotifier<T>::Register(BOF_MULTICAST_ASYNC_NOTIFY_FCT<T> _pNotifyFct, void *_pUserContext)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if (_pNotifyFct)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    auto Delegate = MakeDelegate(_pNotifyFct, &mMsgThread);
    ;
    Delegate.UserContext(_pUserContext);
    mMulticastDelegate += Delegate;
  }
  return Rts_E;
}
template <class T>
BOFERR BofMulticastAsyncNotifier<T>::Unregister(BOF_MULTICAST_ASYNC_NOTIFY_FCT<T> _pNotifyFct)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if (_pNotifyFct)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    mMulticastDelegate -= MakeDelegate(_pNotifyFct, &mMsgThread);
  }
  return Rts_E;
}
template <class T>
BOFERR BofMulticastAsyncNotifier<T>::Notify(const T *_pNotifyArg)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if (_pNotifyArg)
  {
    Rts_E = BOF_ERR_EMPTY;
    if (!mMulticastDelegate.Empty())
    {
      Rts_E = BOF_ERR_NO_ERROR;
      mMulticastDelegate(_pNotifyArg);
    }
  }
  return Rts_E;
}
template <class T>
BOFERR BofMulticastAsyncNotifier<T>::WaitForNoMoreNotificationPending(uint32_t _PollTimeInMs_U32, uint32_t _TimeoutInMs_U32)
{
  BOFERR Rts_E = BOF_ERR_INIT;
  uint32_t StartInMs_U32, DeltaInMs_U32;

  Rts_E = BOF_ERR_EINVAL;
  if (_PollTimeInMs_U32 <= _TimeoutInMs_U32)
  {
    StartInMs_U32 = BOF::Bof_GetMsTickCount();
    Rts_E = BOF_ERR_FULL;
    do
    {
      if (mMsgThread.GetNbPendingRequest() == 0)
      {
        Rts_E = BOF_ERR_NO_ERROR;
        break;
        // StartInMs_U32 = BOF::Bof_GetMsTickCount();
      }

      BOF::Bof_MsSleep(_PollTimeInMs_U32);
      DeltaInMs_U32 = BOF::Bof_ElapsedMsTime(StartInMs_U32);
    } while (DeltaInMs_U32 < _TimeoutInMs_U32);
  }
  return Rts_E;
}

template <class T>
using BOF_MULTICAST_SYNC_NOTIFY_FCT = void (*)(const T *_pNotifyArg);

struct BOF_MULTICAST_SYNC_NOTIFIER_PARAM
{
  uint32_t Dummy_U32;

  BOF_MULTICAST_SYNC_NOTIFIER_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    Dummy_U32 = 0;
  }
};

template <class T>
class BofMulticastSyncNotifier
{
public:
  BofMulticastSyncNotifier(const BOF_MULTICAST_SYNC_NOTIFIER_PARAM &_rSyncNotifierParam_X);
  virtual ~BofMulticastSyncNotifier();

  BOFERR Register(BOF_MULTICAST_SYNC_NOTIFY_FCT<T> _pNotifyFct, void *_pUserContext);
  BOFERR Unregister(BOF_MULTICAST_SYNC_NOTIFY_FCT<T> _pNotifyFct);
  BOFERR Notify(const T *_pNotifyArg_X);

private:
  DelegateLib::MulticastDelegateSafe1<const T *> mMulticastDelegate;
};

template <class T>
BofMulticastSyncNotifier<T>::BofMulticastSyncNotifier(const BOF_MULTICAST_SYNC_NOTIFIER_PARAM &_rSyncNotifierParam_X)
{
}
template <class T>
BofMulticastSyncNotifier<T>::~BofMulticastSyncNotifier()
{
  mMulticastDelegate.Clear();
}

template <class T>
BOFERR BofMulticastSyncNotifier<T>::Register(BOF_MULTICAST_SYNC_NOTIFY_FCT<T> _pNotifyFct, void *_pUserContext)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if (_pNotifyFct)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    auto Delegate = DelegateLib::MakeDelegate(_pNotifyFct);
    ;
    Delegate.UserContext(_pUserContext);
    mMulticastDelegate += Delegate;
  }
  return Rts_E;
}
template <class T>
BOFERR BofMulticastSyncNotifier<T>::Unregister(BOF_MULTICAST_SYNC_NOTIFY_FCT<T> _pNotifyFct)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if (_pNotifyFct)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    mMulticastDelegate -= DelegateLib::MakeDelegate(_pNotifyFct);
  }
  return Rts_E;
}
template <class T>
BOFERR BofMulticastSyncNotifier<T>::Notify(const T *_pNotifyArg)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if (_pNotifyArg)
  {
    Rts_E = BOF_ERR_EMPTY;
    if (!mMulticastDelegate.Empty())
    {
      Rts_E = BOF_ERR_NO_ERROR;
      mMulticastDelegate(_pNotifyArg);
    }
  }
  return Rts_E;
}

END_BOF_NAMESPACE()