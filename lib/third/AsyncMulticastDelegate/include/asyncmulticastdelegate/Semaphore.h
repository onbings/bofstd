#pragma once

#include "DelegateOpt.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
// On std API, the semaphore is a condition variable
#define SEMA std::condition_variable

namespace DelegateLib
{

/// @brief A semaphore wrapper class.
class Semaphore
{
public:
  Semaphore();
  ~Semaphore();

  /// Called to create the semaphore. Calling multiple times on the same instance
  // is safe and will not recreate the semaphore.
  void Create();

  /// Rest the semaphore in readiness to be signalled again.
  void Reset();

  /// Called to wait on a semaphore to be signaled.
  /// @param[in] timeout - timeout in milliseconds. If less 0, wait time is infinite.
  /// @return Return true if semaphore signaled, false if timeout occurred.
  bool Wait(int timeout);

  /// Called to signal a semaphore.
  void Signal();

private:
  // Prevent copying objects
  Semaphore(const Semaphore &);
  Semaphore &operator=(const Semaphore &);

  SEMA m_sema;

  std::mutex m_lock;
  std::atomic<bool> m_flag;
};

} // namespace DelegateLib
