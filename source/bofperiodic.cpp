#if 0
#include "Genlock.h"
#include "XtGeneric/Thread.h"

#include <inttypes.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>

using namespace EVS::Test;

using Clock = Genlock::Clock;

namespace
{
  Genlock * pInstance_O;

  void OnTimerExpiration(int signo)
  {
    if(signo != SIGALRM)
    {
      return;
    }  

    pInstance_O->Pulse();
  }

  /*!
  Description
    This function stops the periodic timer

  Parameters
    None

  Returns
    Nothing
  */
  void stop_period()
  {
    itimerval tv;

    tv.it_interval.tv_sec   = 0;
    tv.it_interval.tv_usec  = 0;  // Next value
    tv.it_value.tv_sec      = 0;
    tv.it_value.tv_usec     = 0;  // Current value
    
    setitimer(ITIMER_REAL, &tv, NULL);
  }
}

/*!
Description
  The class constructor

Parameters
  _Period_O - The genlock period

Returns
  None
*/
Genlock::Genlock(const Clock::duration & _Period_O, std::shared_ptr<Logger> _pLogger_O) :
  Period_O  (_Period_O),
  Last_O    (Clock::now()),
  Exit_B    (false),
  Count_U32 (0),
  pLogger_O (std::move(_pLogger_O))
{
  pInstance_O = this;
}

/*!
Description
  The class destructor

Parameters
  None

Returns
  None
*/
Genlock::~Genlock()
{
  this->Exit_B = true;

  this->Condition_O.notify_all(); 
}

/*!
Description
  This function starts the genlock

Parameters
  None

Returns
  None
*/
void Genlock::Start()
{
  struct sigaction Action_X;
  
  // Register method called upon timer expiration
  Action_X.sa_handler = OnTimerExpiration; 
  sigaction(SIGALRM, &Action_X, NULL);

  // Register timer
  itimerval tv;
  
  auto Seconds  = std::chrono::duration_cast<std::chrono::seconds     >(this->Period_O).count();
  auto uSeconds = std::chrono::duration_cast<std::chrono::microseconds>(this->Period_O - std::chrono::seconds(Seconds)).count();

  tv.it_interval.tv_sec   = Seconds;
  tv.it_interval.tv_usec  = uSeconds;  // Next value
  tv.it_value.tv_sec      = Seconds;
  tv.it_value.tv_usec     = uSeconds;  // Current value
  
  if(setitimer(ITIMER_REAL, &tv, NULL) != 0)
  {
    LOG_CRITICAL(this->pLogger_O, "[Genlock::Genlock] >> Failed to start timer\n");
  }
}

/*!
Description
  This function requests the thread to exit

Parameters
  None

Returns
  The number of genlock pulse
*/
void Genlock::Exit()
{
  this->Exit_B = true;
}

/*!
Description
  This function returns the number
  of genlock pulse

Parameters
  None

Returns
  The number of genlock pulse
*/
uint32_t Genlock::GetCount() const
{
  return this->Count_U32;
}

/*!
Description
  This function returns the period

Parameters
  None

Returns
  The period
*/
const Clock::duration & Genlock::GetPeriod() const
{
  return this->Period_O;
}

/*!
Description
  This function waits for a genlock pulse

Parameters
  None

Returns
  None
*/
void Genlock::Wait()
{
  // Lock the mutex
  std::unique_lock<Mutex> Lock(this->Mutex_O);
         
  this->Condition_O.wait(Lock);
}

/*!
Description
  This function simulates
  a genlock pulse

Parameters
  None

Returns
  None
*/
void Genlock::Pulse()
{
  auto Now_O       = Clock::now();
  auto Duration_O  = Now_O - this->Last_O;
  this->Last_O     = Now_O;

  if((this->Count_U32 != 0) &&
     ((Duration_O > (this->Period_O + std::chrono::milliseconds(2))) ||
      (Duration_O < (this->Period_O - std::chrono::milliseconds(2)))))
  {
    LOG_CRITICAL(this->pLogger_O, "[Genlock::Pulse] >> (%d) Delta between genlock was %" PRIu64 " ms\n",
                    this->Count_U32,
                    std::chrono::duration_cast<std::chrono::milliseconds>(Duration_O).count());
  }

  ++this->Count_U32;
    
  this->Condition_O.notify_all();

  if(this->Exit_B)
  {
    stop_period();
  }
}
#endif