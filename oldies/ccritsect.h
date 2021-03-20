/*!
Copyright (c) 2008, EVS. All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
PURPOSE.

Remarks

  Name:              CritSect.h
  Author:            Julien Melchior (JME)
                     Nicolas Marique (NMA)

Summary:

  The critical section object definition

History:
  V 1.00  ???? ?? ????  JME : First Release
  V 1.01  June 15 2010  NMA : Refactoring
*/

#pragma once

/*** Include ***********************************************************************************************************************/
#include <cstdint>
#include <bofstd/bofstd.h>
#include <bofstd/bofperformance.h>

#if defined(_WINDOWS) || defined (WIN32) || defined(WIN64)
#elif defined(POSIX) || defined (__APPLE__) || defined (__linux__) || defined(__linux__) || defined (__ANDROID__)
#include <pthread.h>
#endif

/*** Defines ***********************************************************************************************************************/

BEGIN_BOF_NAMESPACE();
// Define this field if there should
// only be one global critical section
//#define GLOBAL_CRITICAL_SECTION

// Uncomment this define  
// to profile this class
//#define PROFILE_CRITSECT

#define CCRITSECT_ENTER(X)  if((X) != nullptr) { (X)->Enter(); }
#define CCRITSECT_LEAVE(X)  if((X) != nullptr) { (X)->Leave(); }

/*** Enums *************************************************************************************************************************/

enum
{
  PROFILE_CRITSECT_ENTER,
  PROFILE_CRITSECT_LEAVE,
  PROFILE_CRITSECT_MAX_ITEMS
};

/*** Structures ********************************************************************************************************************/

/*** Classes ***********************************************************************************************************************/

/*!
Summary
  The class representing a critical section
*/
class CCriticalSection
{
public :

         CCriticalSection   ();
        ~CCriticalSection   ();

  uint32_t   Init                ();
  uint32_t   Enter               ();
  uint32_t   Leave               ();
  
  bool  GetProfilingStats   (uint32_t _ItemId_U32, STAT_VARIABLE * _pStats_X);
  uint64_t   GetMemoryUsage      ();

protected :

private :
public:
  BofProfiler * mpProfiler_O;

#if defined(POSIX) || defined (__APPLE__) || defined (__linux__) || defined(__linux__) || defined (__ANDROID__)
  pthread_mutex_t           mCriticalSection_X;
#else
#ifdef GLOBAL_CRITICAL_SECTION
	static void             * mpCriticalSection_h;
	static U32                mNbInstances_U32;
#else	
	void             * mpCriticalSection_h;
#endif
#endif
};

END_BOF_NAMESPACE();

