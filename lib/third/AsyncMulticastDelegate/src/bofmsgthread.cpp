#include <asyncmulticastdelegate/DelegateOpt.h>
#include <cassert>

#include <asyncmulticastdelegate/ThreadMsg.h>
#include <asyncmulticastdelegate/bofmsgthread.h>

// using namespace std;
// using namespace DelegateLib;

#define MSG_DISPATCH_DELEGATE 1
#define MSG_EXIT_THREAD 2

//----------------------------------------------------------------------------
// BofMsgThread
//----------------------------------------------------------------------------
BofMsgThread::BofMsgThread()
    : BOF::BofThread()
{
}

//----------------------------------------------------------------------------
// ~BofMsgThread
//----------------------------------------------------------------------------
BofMsgThread::~BofMsgThread()
{
  ExitThread(10, 1000);
}

//----------------------------------------------------------------------------
// CreateThread
//----------------------------------------------------------------------------
bool BofMsgThread::LaunchThread(const char *threadName, BOF::BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF::BOF_THREAD_PRIORITY _ThreadPriority_E, uint64_t _ThreadCpuCoreMaskAffinity_U64)
{
  return (LaunchBofProcessingThread(threadName, false, 2000, _ThreadSchedulerPolicy_E, _ThreadPriority_E, _ThreadCpuCoreMaskAffinity_U64, 1000, 0) == BOF_ERR_NO_ERROR);
}
uint32_t BofMsgThread::GetNbPendingRequest()
{
  std::lock_guard<std::mutex> lock(m_mutex);
  return static_cast<uint32_t>(m_queue.size());
}

//----------------------------------------------------------------------------
// ExitThread
//----------------------------------------------------------------------------
void BofMsgThread::ExitThread(uint32_t _PollTimeInMs_U32, uint32_t _TimeoutInMs_U32)
{
  // Create a new ThreadMsg
  ThreadMsg *threadMsg = new ThreadMsg(MSG_EXIT_THREAD, 0);
  std::chrono::time_point<std::chrono::high_resolution_clock> Start;
  std::chrono::duration<float, std::milli> EllapsedInMs;

  // Put exit thread message into the queue
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(threadMsg);
    m_cv.notify_one();
  }
  if (_PollTimeInMs_U32 <= _TimeoutInMs_U32)
  {
    Start = std::chrono::high_resolution_clock::now();
    do
    {
      if (m_queue.empty())
      {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(_PollTimeInMs_U32));
      EllapsedInMs = std::chrono::high_resolution_clock::now() - Start;

      //     printf("EllapsedInMs %f empty %d\n",EllapsedInMs.count(), m_queue.empty());
    } while (EllapsedInMs.count() < static_cast<float>(_TimeoutInMs_U32));
  }
}

//----------------------------------------------------------------------------
// DispatchDelegate
//----------------------------------------------------------------------------
void BofMsgThread::DispatchDelegate(DelegateLib::DelegateMsgBase *msg)
{
  // Create a new ThreadMsg
  ThreadMsg *threadMsg = new ThreadMsg(MSG_DISPATCH_DELEGATE, msg);

  // Add dispatch delegate msg to queue and notify worker thread
  std::unique_lock<std::mutex> lk(m_mutex);
  m_queue.push(threadMsg);
  // printf("qs %d\n",m_queue.size());
  m_cv.notify_one();
}

//----------------------------------------------------------------------------
// Process
//----------------------------------------------------------------------------
BOFERR BofMsgThread::V_OnProcessing()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  bool Finish_B = false;

  //  printf("BofMsgThread running\n");
  while ((!IsThreadLoopMustExit()) && (Rts_E == BOF_ERR_NO_ERROR) && (!Finish_B))
  {
    ThreadMsg *msg = 0;
    {
      // Wait for a message to be added to the queue
      std::unique_lock<std::mutex> lk(m_mutex);
      while (m_queue.empty())
      {
        // printf("wait\n");
        m_cv.wait(lk);
        // printf("eowait\n");
      }

      if (m_queue.empty())
      {
        // printf("empty\n");
        continue;
      }
      msg = m_queue.front();
      // printf("qs %d=>got %p\n",m_queue.size(), msg->GetData());
      m_queue.pop();
    }
    // printf("id %d\n", msg->GetId());

    switch (msg->GetId())
    {
      case MSG_DISPATCH_DELEGATE:
        {
          assert(msg->GetData() != nullptr);

          // Convert the ThreadMsg void* data back to a DelegateMsg*
          DelegateLib::DelegateMsgBase *delegateMsg = static_cast<DelegateLib::DelegateMsgBase *>(msg->GetData());

          // Invoke the callback on the target thread
          delegateMsg->GetDelegateInvoker()->DelegateInvoke(&delegateMsg);

          // Delete dynamic data passed through message queue
          delete msg;
          break;
        }

      case MSG_EXIT_THREAD:
        {
          // printf("BofMsgThread purge\n");
          delete msg;
          std::unique_lock<std::mutex> lk(m_mutex);
          while (!m_queue.empty())
          {
            msg = m_queue.front();
            m_queue.pop();
            delete msg;
          }
          Rts_E = BOF_ERR_FINISHED;
          Finish_B = true;
          break;
        }

      default:
        assert(0);
        break;
    }
  }
  // printf("BofMsgThread exiting\n");
  return Rts_E;
}
