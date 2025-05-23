#include <asyncmulticastdelegate/Semaphore.h>

namespace DelegateLib
{

//------------------------------------------------------------------------------
// Semaphore
//------------------------------------------------------------------------------
Semaphore::Semaphore() : m_flag(false)
{
}

//------------------------------------------------------------------------------
// ~Semaphore
//------------------------------------------------------------------------------
Semaphore::~Semaphore()
{
#if USE_WIN32_THREADS
  if (m_sema != INVALID_HANDLE_VALUE)
  {
    BOOL val = CloseHandle(m_sema);
    ASSERT_TRUE(val != 0);
    m_sema = INVALID_HANDLE_VALUE;
  }
#endif
}

//------------------------------------------------------------------------------
// Create
//------------------------------------------------------------------------------
void Semaphore::Create()
{
#if USE_WIN32_THREADS
  if (m_sema == INVALID_HANDLE_VALUE)
  {
    m_sema = CreateEvent(nullptr, TRUE, FALSE, TEXT("Semahore"));
    ASSERT_TRUE(m_sema != nullptr);
  }
#endif
}

//------------------------------------------------------------------------------
// Reset
//------------------------------------------------------------------------------
void Semaphore::Reset()
{
#if USE_WIN32_THREADS
  BOOL val = ResetEvent(m_sema);
  ASSERT_TRUE(val != 0);
#endif
}

//------------------------------------------------------------------------------
// Wait
//------------------------------------------------------------------------------
bool Semaphore::Wait(int timeout)
{
  std::unique_lock<std::mutex> lk(m_lock);
  std::cv_status status = std::cv_status::no_timeout;
  if (timeout < 0)
  {
    while (!m_flag)
      m_sema.wait(lk);
  }
  else
  {
    while (!m_flag && status == std::cv_status::no_timeout)
      status = m_sema.wait_for(lk, std::chrono::milliseconds(timeout));
  }

  if (m_flag)
  {
    m_flag = false;
    return true;
  }
  else
    return false;
}

//------------------------------------------------------------------------------
// Signal
//------------------------------------------------------------------------------
void Semaphore::Signal()
{
  std::unique_lock<std::mutex> lk(m_lock);
  m_flag = true;
  m_sema.notify_one();
}

} // namespace DelegateLib
