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
    : BofThread(_rCommandQueueParam_X.PriorityInversionAware_B)
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
  CircularBufferParam_X.MultiThreadAware_B = _rCommandQueueParam_X.MultiThreadAware_B;
  CircularBufferParam_X.PriorityInversionAware_B = _rCommandQueueParam_X.PriorityInversionAware_B;
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
    Sts_E = LaunchBofProcessingThread("BofCommandQueue", _rCommandQueueParam_X.PriorityInversionAware_B, false, 0, _rCommandQueueParam_X.ThreadSchedulerPolicy_E, _rCommandQueueParam_X.ThreadPriority_E, _rCommandQueueParam_X.ThreadCpuCoreAffinityMask_U64, 2000, 0);
  }
}
BofCommandQueue::~BofCommandQueue()
{
  DestroyBofProcessingThread("~BofCommandQueue");
}

BOFERR BofCommandQueue::V_OnProcessing()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR, Sts_E;

  while (!IsThreadLoopMustExit())
  {
    if (mpuCommandEntryCollection)
    {
      //printf("%u: Wait for cmd for %d ms empty %d nb %d\n", Bof_GetMsTickCount(), mCommandQueueParam_X.PollTimeoutInMs_U32, mpuCommandEntryCollection->IsEmpty(), mpuCommandEntryCollection->GetNbElement());
      Sts_E = mpuCommandEntryCollection->Pop(&mCommandPending_X, mCommandQueueParam_X.PollTimeoutInMs_U32, nullptr, nullptr);
      // printf("%u Sts %d ptr %p remain %d\n", Bof_GetMsTickCount(), Sts_E, mpuCommandEntryCollection.get(), mpuCommandEntryCollection->GetNbElement());
      if (Sts_E == BOF_ERR_NO_ERROR)
      {
        if (mpuCommandEntryCollection)
        {
          // printf("%u Got cmd '%s'->Call it'\n", Bof_GetMsTickCount(), mCommandPending_X.Name_S.c_str());
          mCommandPending_X.Cmd();
          // printf("%u End of exec Empty %d Sts %d Still %d\n", Bof_GetMsTickCount(), mpuCommandEntryCollection->IsEmpty(),Sts_E, mpuCommandEntryCollection->GetNbElement());
        }
      }
	  else
	  {
	    mCommandPending_X.Name_S = "";
	  }
    }
    else
    {
      Bof_MsSleep(0);
    }
  }
  return Rts_E;
}

BOFERR BofCommandQueue::PostCommand(bool _OnlyOne_B, const BOF_COMMAND_QUEUE_ENTRY &_rCommand_X)
{
  BOFERR Rts_E = BOF_ERR_INIT;
  BOF_COMMAND_QUEUE_ENTRY Command_X;
  uint32_t i_U32;

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
        //Rts_E = mpuCommandEntryCollection->LockCircularBuffer();
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
          //mpuCommandEntryCollection->UnlockCircularBuffer();
        }
      }
    }
	
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = mpuCommandEntryCollection->Push(&_rCommand_X, 0, nullptr, nullptr);
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

  Rts_U32 = mpuCommandEntryCollection->GetNbElement();
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

  mCommandPending_X.Name_S = "";
  mpuCommandEntryCollection->Reset();
  return Rts_E;
}
END_BOF_NAMESPACE()