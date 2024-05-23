#pragma once

#include "DelegateOpt.h"

#include "DelegateThread.h"
#include <atomic>
#include <bofstd/bofthread.h>
#include <condition_variable>
#include <mutex>
#include <queue>

class ThreadMsg;

class BofMsgThread : public BOF::BofThread, public DelegateLib::DelegateThread
{
public:
  /// Constructor
  BofMsgThread(bool _PriorityInversionAware_B);

  /// Destructor
  ~BofMsgThread();

  /// Called once to create the worker thread
  /// @return TRUE if thread is created. FALSE otherise.
  bool LaunchThread(const char *threadName, BOF::BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF::BOF_THREAD_PRIORITY _ThreadPriority_E, uint64_t _ThreadCpuCoreMaskAffinity_U64);

  uint32_t GetNbPendingRequest();

  void DispatchDelegate(DelegateLib::DelegateMsgBase *msg) override;

private:
  BofMsgThread(const BofMsgThread &);
  BofMsgThread &operator=(const BofMsgThread &);

  /// Called once a program exit to exit the worker thread
  void ExitThread(uint32_t _PollTimeInMs_U32, uint32_t _TimeoutInMs_U32);

  /// Entry point for the thread
  BOFERR V_OnProcessing() override;

  // std::thread* m_thread;
  std::queue<ThreadMsg *> m_queue;
  BOF::BOF_MUTEX mMutex_X;
  std::condition_variable m_cv;
  // const char* THREAD_NAME;
};
