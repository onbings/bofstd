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
#include <bofstd/bofthread.h>
#include <bofstd/bofcircularbuffer.h>

BEGIN_BOF_NAMESPACE()

struct BOF_COMMAND_QUEUE_PARAM
{
  uint64_t ThreadCpuCoreAffinityMask_U64;
  bool MultiThreadAware_B;
  bool PriorityInversionAware_B;
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
    MultiThreadAware_B=false;
    PriorityInversionAware_B=false;
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

END_BOF_NAMESPACE()