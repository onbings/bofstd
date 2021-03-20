/*!
Copyright (c) 2008, EVS. All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
PURPOSE.

Remarks

  Name:              CritSect.cpp
  Author:            Julien Melchior (JME)
                     Nicolas Marique (NMA)

Summary:

  The critical section object definition

History:
  V 1.00  ???? ?? ????  JME : First Release
  V 1.01  June 15 2010  NMA : Refactoring
*/

/*** Include ***********************************************************************************************************************/
#include <stdlib.h>
#include <cstdint>

#if defined(_WIN32)     //Windows 32/64/WinCE  	
//#define WIN32_LEAN_AND_MEAN		/*! Exclude rarely-used stuff from Windows headers*/
#include <windows.h>

#elif defined(POSIX) || defined (__APPLE__) || defined(__linux__) || defined (__ANDROID__)

#else
#endif

#include <string.h>
#include <bofstd/ccritsect.h>
BEGIN_BOF_NAMESPACE();


/*** Globals ***********************************************************************************************************************/

/*** Defines ***********************************************************************************************************************/

/*** Enums *************************************************************************************************************************/

/*** Structures ********************************************************************************************************************/

/*** Statics ***********************************************************************************************************************/

#ifdef GLOBAL_CRITICAL_SECTION

CRITICAL_SECTION   GL_CritSect_X                         = {0};
void             * CCriticalSection::mpCriticalSection_h = nullptr;
uint32_t                CCriticalSection::mNbInstances_U32    =  0;

#endif

/*!
Description
  The class constructor
  
Parameters
  None
  
Returns
  Nothing

Remarks
  None

See also
  Nothing
*/
CCriticalSection::CCriticalSection()
{
#if defined(_WIN32)

#ifdef GLOBAL_CRITICAL_SECTION

  mpCriticalSection_h = &GL_CritSect_X;

  if(mNbInstances_U32++ == 0)
  {
    InitializeCriticalSection(mpCriticalSection_h);
  }

#else

  mpCriticalSection_h = new CRITICAL_SECTION();

#endif
#endif
}

/*!
Description
  The class destructor
  
Parameters
  None
  
Returns
  Nothing

Remarks
  None

See also
  Nothing
*/
CCriticalSection::~CCriticalSection()
{

#if defined(_WINDOWS) || defined (WIN32)

#if defined(GLOBAL_CRITICAL_SECTION)

  if(--mNbInstances_U32 == 0)
  {
    DeleteCriticalSection((CRITICAL_SECTION *)mpCriticalSection_h);
  }

#else
  
  DeleteCriticalSection((CRITICAL_SECTION *)mpCriticalSection_h);
  
BOF_SAFE_DELETE(mpCriticalSection_h);

#endif
    
#elif defined(POSIX) || defined (__APPLE__) || defined (__linux__) || defined(__linux__) || defined (__ANDROID__)
    
  pthread_mutex_destroy(&mCriticalSection_X);

#else
  
#endif
}

/*!
Description
  This function initializes the critical section
  
Parameters
  None
  
Returns
  Always 0

Remarks
  None

See also
  Nothing
*/
uint32_t CCriticalSection::Init()
{
   uint32_t retValue_U32 = 0;

#if defined(_WINDOWS) || defined (WIN32)

#if !defined(GLOBAL_CRITICAL_SECTION)

  InitializeCriticalSection((CRITICAL_SECTION *)mpCriticalSection_h);

#endif
    
#elif defined(POSIX) || defined (__APPLE__) || defined (__linux__) || defined(__linux__) || defined (__ANDROID__)
    
  pthread_mutexattr_t CritSectType_X;

  pthread_mutexattr_init    (&CritSectType_X);
  pthread_mutexattr_settype (&CritSectType_X, PTHREAD_MUTEX_RECURSIVE);
#if !defined (__ANDROID__)
  pthread_mutexattr_setprotocol(&CritSectType_X, PTHREAD_PRIO_INHERIT);
#endif
  retValue_U32 = pthread_mutex_init(&mCriticalSection_X, &CritSectType_X);

  pthread_mutexattr_destroy (&CritSectType_X);
  
#endif


#ifdef PROFILE_CRITSECT

  mpProfiler_O = new BofProfiler(PROFILE_CRITSECT_MAX_ITEMS);

#else

  mpProfiler_O = nullptr; 

#endif

  return retValue_U32;
}

/*!
Description
  This function indicates the current thread is trying to enter
  into the zone protected by the critical section
  
Parameters
  None
  
Returns
  Always 0

Remarks
  None

See also
  Nothing
*/
uint32_t CCriticalSection::Enter()
{
#ifdef PROFILE_CRITSECT
  BOF_ENTER_BENCH(mpProfiler_O, PROFILE_CRITSECT_ENTER);
#endif


#if defined(_WINDOWS) || defined (WIN32)

  EnterCriticalSection((CRITICAL_SECTION *)mpCriticalSection_h);
    
#elif defined(POSIX) || defined (__APPLE__) || defined (__linux__) || defined(__linux__) || defined (__ANDROID__)
    
  pthread_mutex_lock(&mCriticalSection_X);

#endif


#ifdef PROFILE_CRITSECT
  BOF_LEAVE_BENCH(mpProfiler_O, PROFILE_CRITSECT_ENTER);
#endif

  return 0;
}

/*!
Description
  This function indicates the current thread is leaving
  the zone protected by the critical section
  
Parameters
  None
  
Returns
  Always 0

Remarks
  None

See also
  Nothing
*/
uint32_t CCriticalSection::Leave()
{
#ifdef PROFILE_CRITSECT
  BOF_ENTER_BENCH(mpProfiler_O, PROFILE_CRITSECT_LEAVE);
#endif

#if defined(_WINDOWS) || defined (WIN32)

  LeaveCriticalSection((CRITICAL_SECTION *)mpCriticalSection_h);
    
#elif defined(POSIX) || defined (__APPLE__) || defined (__linux__) || defined(__linux__) || defined (__ANDROID__)
    
  pthread_mutex_unlock(&mCriticalSection_X);

#endif


#ifdef PROFILE_CRITSECT
  BOF_LEAVE_BENCH(mpProfiler_O, PROFILE_CRITSECT_LEAVE);
#endif

  return 0;
}

/*!
Description
  This function retrieves the profiling statistics associated to a particular function
  
Parameters
  _ItemId_U32 - The function ID
  _pStats_X   - The pointer to where to store the statistics
  
Returns
  true  - The operation was successful
  false - The operation failed

Remarks
  None

See also
  Nothing
*/
bool CCriticalSection::GetProfilingStats(uint32_t /*_ItemId_U32*/, STAT_VARIABLE * /*_pStats_X*/)
{
  bool Ret_B = false;

#ifdef PROFILE_CRITSECT  
  BOF_ASSERT(_pStats_X != nullptr);
  
  memset(_pStats_X, 0x00, sizeof(*_pStats_X));
  
  if(mpProfiler_O != nullptr)
  {
    Ret_B = mpProfiler_O->GetStats(_ItemId_U32, _pStats_X);
  }
#endif

  return Ret_B;
}

/*!
Description
  This function retrieves the memory used by this object
  
Parameters
  None
  
Returns
  The size in bytes of the memory used by this object

Remarks
  None

See also
  Nothing
*/
uint64_t CCriticalSection::GetMemoryUsage()
{
  uint64_t Ret_U64 = 0;
  
  Ret_U64 += sizeof(CCriticalSection);

  if(mpProfiler_O != nullptr) { Ret_U64 += mpProfiler_O->GetMemoryUsage(); }

  return Ret_U64;
}
END_BOF_NAMESPACE();
