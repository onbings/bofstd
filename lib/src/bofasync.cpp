/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements async class helper
 *
 * Name:        bofasync.cpp
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
#include <asyncmulticastdelegate/DelegateLib.h>
#include <asyncmulticastdelegate/bofmsgthread.h>
#include <bofstd/bofasync.h>

BEGIN_BOF_NAMESPACE()

BofCommandQueue::BofCommandQueue(const BOF_COMMAND_QUEUE_PARAM &_rCommandQueueParam_X)
{
  BOFERR Sts_E;
  BOF_CIRCULAR_BUFFER_PARAM CircularBufferParam_X;

  mCommandQueueParam_X = _rCommandQueueParam_X;
  if (mCommandQueueParam_X.PollTimeoutInMs_U32 == 0)
  {
    mCommandQueueParam_X.PollTimeoutInMs_U32 = 100;
  }
  mCommandPending_X.Name_S = "";
  Sts_E = BOF_ERR_ENOMEM;
  CircularBufferParam_X.Blocking_B = true;
  CircularBufferParam_X.MultiThreadAware_B = true;
  CircularBufferParam_X.NbMaxElement_U32 = mCommandQueueParam_X.MaxPendingRequest_U32;
  CircularBufferParam_X.Overwrite_B = false;
  CircularBufferParam_X.pData = nullptr;
  CircularBufferParam_X.PopLockMode_B = false;

  mpuCommandEntryCollection = std::make_unique<BofCircularBuffer<BOF_COMMAND_QUEUE_ENTRY>>(CircularBufferParam_X);
  if (mpuCommandEntryCollection)
  {
    Sts_E = mpuCommandEntryCollection->LastErrorCode();
  }
  if (Sts_E != BOF_ERR_NO_ERROR)
  {
    mpuCommandEntryCollection = nullptr;
  }
  else
  {
    mpuCommandQueueThread = std::make_unique<BofThread>();
    if (mpuCommandQueueThread)
    {
      mpuCommandQueueThread->SetThreadCallback(nullptr, BOF_BIND_0_ARG_TO_METHOD(this, BofCommandQueue::OnProcessing), nullptr);
      Sts_E = mpuCommandQueueThread->LaunchBofProcessingThread("BofCommandQueue", false, 0, _rCommandQueueParam_X.ThreadSchedulerPolicy_E, _rCommandQueueParam_X.ThreadPriority_E, _rCommandQueueParam_X.ThreadCpuCoreAffinityMask_U64, 2000, 0);
    }
  }
}
BofCommandQueue::~BofCommandQueue()
{
}

BOFERR BofCommandQueue::OnProcessing()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR, Sts_E;

  while ((mpuCommandEntryCollection) && (!mpuCommandQueueThread->IsThreadLoopMustExit()))
  {
    Sts_E = mpuCommandEntryCollection->Pop(&mCommandPending_X, mCommandQueueParam_X.PollTimeoutInMs_U32, nullptr, nullptr);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      if (mpuCommandEntryCollection)
      {
        mCommandPending_X.Cmd();
        Sts_E = mpuCommandEntryCollection->LockQueue();
        if (Sts_E == BOF_ERR_NO_ERROR)
        {
          if (mpuCommandEntryCollection->IsEmpty())
          {
            mCommandPending_X.Name_S = "";
          }
          else
          {
            mpuCommandEntryCollection->Peek(&mCommandPending_X, 0, nullptr, nullptr);
          }
          mpuCommandEntryCollection->UnlockQueue();
        }
      }
    }
  }
  return Rts_E;
}

BOFERR BofCommandQueue::PostCommand(bool _OnlyOne_B, const BOF_COMMAND_QUEUE_ENTRY &_rCommand_X)
{
  BOFERR Rts_E = BOF_ERR_INIT;
  BOF_COMMAND_QUEUE_ENTRY Command_X;
  uint32_t i_U32;

  if (mpuCommandQueueThread) // If yes then mpuCommandEntryCollection is not null
  {
    Rts_E = BOF_ERR_EINVAL;
    if (_rCommand_X.Name_S != "")
    {
      Rts_E = BOF_ERR_NO_ERROR;
      if (_OnlyOne_B)
      {
        if (mCommandPending_X.Name_S == _rCommand_X.Name_S)
        {
          Rts_E = BOF_ERR_DUPLICATE;
        }
        else
        {
          Rts_E = mpuCommandEntryCollection->LockQueue();
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            for (i_U32 = 0; i_U32 < mpuCommandEntryCollection->GetNbElement(); i_U32++)
            {
              Rts_E = mpuCommandEntryCollection->PeekFromPop(i_U32, &Command_X, nullptr, nullptr);
              if (Rts_E == BOF_ERR_NO_ERROR)
              {
                if (Command_X.Name_S == _rCommand_X.Name_S)
                {
                  Rts_E = BOF_ERR_DUPLICATE;
                  break;
                }
              }
              else
              {
                break;
              }
            }
            mpuCommandEntryCollection->UnlockQueue();
          }
        }
      }
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Rts_E = mpuCommandEntryCollection->Push(&_rCommand_X, 0, nullptr);
      }
    }
  }
  return Rts_E;
}

bool BofCommandQueue::IsProcessingCommand() const
{
  return (mCommandPending_X.Name_S != "") || (mpuCommandEntryCollection ? (mpuCommandEntryCollection->GetNbElement() != 0) : false);
}

// If we call this one in the context of the last cmd present in the queue it will return 0
uint32_t BofCommandQueue::NumberOfCommandWaitingInQueue() const
{
  uint32_t Rts_U32 = 0;

  if (mpuCommandQueueThread) // If yes then mpuCommandEntryCollection is not null
  {
    Rts_U32 = mpuCommandEntryCollection->GetNbElement();
  }
  return Rts_U32;
}
// If we call this one in the context of the last cmd present in the queue it will return true (itself) but NumberOfChannelCommandWaitingInQueue return 0
bool BofCommandQueue::IsCommandPending() const
{
  return (mCommandPending_X.Name_S != "");
}
BOFERR BofCommandQueue::ClearCommandQueue()
{
  BOFERR Rts_E = BOF_ERR_INIT;

  if (mpuCommandQueueThread) // If yes then mpuCommandEntryCollection is not null
  {
    mCommandPending_X.Name_S = "";
    mpuCommandEntryCollection->Reset();
  }
  return Rts_E;
}
END_BOF_NAMESPACE()