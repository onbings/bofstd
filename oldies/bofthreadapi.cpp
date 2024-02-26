/*!
 * Copyright (c) 2008-2020, Evs. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This is the BofThread wrapper interface
 *
 * Name:        BofThreadApi.cpp
 * Author:      Nicolas Marique : n.marique@evs.tv + bha for __APPLE__
 * Revision:    1.0
 *
 * History:
 *
 *      V 1.00  Wed May 08 2013  NMA : Initial release
 */

/*** Includes **********************************************************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#if defined( __linux__ ) || defined(__APPLE__)

#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>

#if defined(__ANDROID__)
#else

#include <sys/shm.h>
#include <sys/sem.h>

#endif
#if defined (__APPLE__)
#include <time.h>
#include <sys/time.h>
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>

#else
#include <Windows.h>
#endif

#include <cstdint>
#include <bofstd/bofthreadapi.h>
#include <bofstd/bofthreadgeneric.h>
#include <bofstd/bofstring.h>
#include <string.h>

BEGIN_BOF_NAMESPACE()

/*** Defines ***********************************************************************************************************************/

#define BOFTHREAD_MAGIC       0xABBA1234
#define BOFEVENT_MAGIC        0xCD127AC8
#define BOFMUTEX_MAGIC        0xEF342DA1
#define BOFSHAREDMEM_MAGIC    0x12D3FB5E

/*** Enums *************************************************************************************************************************/

/*** Structures ********************************************************************************************************************/

/*!
 * Summary
 * The handle enclosing BOF thread
 * implementation specific
 */
typedef struct
{
		uint32_t Magic_U32;
		CBofThreadGeneric *pThread_O;
} BOF_THREAD_HANDLE;

/*!
 * Summary
 * The handle enclosing BOF event
 * implementation specific
 */
typedef struct
{
		uint32_t Magic_U32;

#if defined( __linux__ ) || defined(__APPLE__)
		pthread_cond_t Condition_X;
		pthread_mutex_t Mutex_X;
		pthread_condattr_t CondAttr_X;

		volatile bool Signaled_B;
		volatile bool *pSignaled_B;

		bool Named_B;
		void *SharedMem_h;
		void *pMapping;

#else
		void *          Event_h;
#endif
} BOF_EVENT_HANDLE;

/*!
 * Summary
 * The handle enclosing BOF mutex
 * implementation specific
 */
typedef struct
{
		uint32_t Magic_U32;

#if defined( __linux__ ) || defined(__APPLE__)
		int MutexId_i;
		pthread_mutex_t Mutex_X;
		bool Named_B;
#else
		void *          Mutex_h;
#endif
} BOF_MUTEX_HANDLE;

/*!
 * Summary
 * The handle enclosing BOF mutex
 * implementation specific
 */
typedef struct
{
		uint32_t Magic_U32;

#if defined( __linux__ ) || defined(__APPLE__)
		int SharedMemId_i;
#else
		void * SharedMem_h;
#endif
} BOF_SHARED_MEM_HANDLE;

/*** Prototypes ********************************************************************************************************************/

bool ConvertBofPriorityToOsPriority(BOF_THREAD_PRIORITY _Priority_E, int32_t *_pPolicy_S32, int32_t *_pPriority_S32);

#if defined(_WIN32)
bool ConvertBofPriorityToOsProcessPriority(BOF_THREAD_PRIORITY _Priority_E, int32_t *_pPriority_S32);
#endif

bool ConvertOsPriorityToBofPriority(BOF_THREAD_PRIORITY *_pPriority_E, int32_t _Policy_S32, int32_t _Priority_S32);

/*!
 * Description
 * This function converts a BOF
 * thread priority to its OS equivalent
 *
 * Parameters
 * _BofPriority_S32  - The BOF thread priority
 *
 * Returns
 * The OS priority
 *
 * Remarks
 * None
 */
bool ConvertBofPriorityToOsPriority(BOF_THREAD_PRIORITY _Priority_E, int32_t *_pPolicy_S32, int32_t *_pPriority_S32)
{
	bool Ret_B = true;

	if ((_pPolicy_S32 != nullptr) && (_pPriority_S32 != nullptr))
	{
#if defined( __linux__ ) || defined(__APPLE__)
		int32_t FifoMin_S32 = 0;
		int32_t FifoMax_S32 = 0;
		int32_t OtherMin_S32 = 0;
		int32_t OtherMax_S32 = 0;
		int32_t OtherMean_S32 = 0;

		BofThread::GetPriorityRange(SCHED_FIFO, FifoMin_S32, FifoMax_S32);
		BofThread::GetPriorityRange(SCHED_OTHER, OtherMin_S32, OtherMax_S32);

		OtherMean_S32 = (OtherMin_S32 + OtherMax_S32) / 2;

		switch (_Priority_E)
		{
			case BOF_THREAD_PRIORITY_TIME_CRITICAL:
			{
				*_pPolicy_S32 = SCHED_FIFO;
				*_pPriority_S32 = FifoMax_S32;
				break;
			}

			case BOF_THREAD_PRIORITY_HIGHEST:
			{
				*_pPolicy_S32 = SCHED_OTHER;
				*_pPriority_S32 = OtherMax_S32;
				break;
			}

			case BOF_THREAD_PRIORITY_ABOVE_NORMAL:
			{
				*_pPolicy_S32 = SCHED_OTHER;
				*_pPriority_S32 = OtherMean_S32 + 1;
				break;
			}

			case BOF_THREAD_PRIORITY_NORMAL:
			{
				*_pPolicy_S32 = SCHED_OTHER;
				*_pPriority_S32 = OtherMean_S32;
				break;
			}

			case BOF_THREAD_PRIORITY_BELOW_NORMAL:
			{
				*_pPolicy_S32 = SCHED_OTHER;
				*_pPriority_S32 = OtherMean_S32 - 1;
				break;
			}

			case BOF_THREAD_PRIORITY_LOWEST:
			{
				*_pPolicy_S32 = SCHED_OTHER;
				*_pPriority_S32 = OtherMin_S32 + 2;
				break;
			}

			case BOF_THREAD_PRIORITY_ABOVE_IDLE:
			{
				*_pPolicy_S32 = SCHED_OTHER;
				*_pPriority_S32 = OtherMin_S32 + 1;
				break;
			}

			case BOF_THREAD_PRIORITY_IDLE:
			{
				*_pPolicy_S32 = SCHED_OTHER;
				*_pPriority_S32 = OtherMin_S32;
				break;
			}

			default:
			{
				Ret_B = false;
				break;
			}
		}

		// Make sure priority is in the range
		BofThread::GetPriorityRange(*_pPolicy_S32, OtherMin_S32, OtherMax_S32);

		if (*_pPriority_S32 > OtherMax_S32)
		{
			*_pPriority_S32 = OtherMax_S32;
		}
		else if (*_pPriority_S32 < OtherMin_S32)
		{
			*_pPriority_S32 = OtherMin_S32;
		}
#else
		*_pPolicy_S32 = 0;

		switch ( _Priority_E )
		{
			case BOF_THREAD_PRIORITY_TIME_CRITICAL:
			{
				*_pPriority_S32 = THREAD_PRIORITY_TIME_CRITICAL;
				break;
			}

			case BOF_THREAD_PRIORITY_HIGHEST:
			{
				*_pPriority_S32 = THREAD_PRIORITY_HIGHEST;
				break;
			}

			case BOF_THREAD_PRIORITY_ABOVE_NORMAL:
			{
				*_pPriority_S32 = THREAD_PRIORITY_ABOVE_NORMAL;
				break;
			}

			case BOF_THREAD_PRIORITY_NORMAL:
			{
				*_pPriority_S32 = THREAD_PRIORITY_NORMAL;
				break;
			}

			case BOF_THREAD_PRIORITY_BELOW_NORMAL:
			{
				*_pPriority_S32 = THREAD_PRIORITY_BELOW_NORMAL;
				break;
			}

			case BOF_THREAD_PRIORITY_LOWEST:
			{
				*_pPriority_S32 = THREAD_PRIORITY_LOWEST;
				break;
			}

			case BOF_THREAD_PRIORITY_ABOVE_IDLE:
			{
				*_pPriority_S32 = THREAD_PRIORITY_LOWEST;
				break;
			}

			case BOF_THREAD_PRIORITY_IDLE:
			{
				*_pPriority_S32 = THREAD_PRIORITY_IDLE;
				break;
			}

			default:
			{
				*_pPriority_S32 = (int32_t) _Priority_E;
				break;
			}
		}
#endif
	}
	else
	{
		Ret_B = false;
	}

	return Ret_B;
}

#if defined(_WIN32)
bool ConvertBofPriorityToOsProcessPriority(BOF_THREAD_PRIORITY _Priority_E, int32_t *_pPriority_S32)
{
	bool Ret_B = false;

	if (_pPriority_S32 != nullptr)
	{
		Ret_B = true;
		switch (_Priority_E)
		{
			case BOF_THREAD_PRIORITY_TIME_CRITICAL:
			{
				*_pPriority_S32 = REALTIME_PRIORITY_CLASS;
				break;
			}

			case BOF_THREAD_PRIORITY_HIGHEST:
			{
				*_pPriority_S32 = HIGH_PRIORITY_CLASS;
				break;
			}

			case BOF_THREAD_PRIORITY_ABOVE_NORMAL:
			{
				*_pPriority_S32 = ABOVE_NORMAL_PRIORITY_CLASS;
				break;
			}

			case BOF_THREAD_PRIORITY_NORMAL:
			{
				*_pPriority_S32 = NORMAL_PRIORITY_CLASS;
				break;
			}

			case BOF_THREAD_PRIORITY_BELOW_NORMAL:
			{
				*_pPriority_S32 = BELOW_NORMAL_PRIORITY_CLASS;
				break;
			}

			case BOF_THREAD_PRIORITY_ABOVE_IDLE:
			{
				*_pPriority_S32 = IDLE_PRIORITY_CLASS;
				break;
			}

			case BOF_THREAD_PRIORITY_IDLE:
			{
				*_pPriority_S32 = IDLE_PRIORITY_CLASS;
				break;
			}

			default:
				Ret_B = false;
				break;
		}
	}

	return Ret_B;
}
#endif

/*!
 * Description
 * This function converts an OS
 * thread priority to its BOF equivalent
 *
 * Parameters
 * _BofPriority_S32  - The OS thread priority
 *
 * Returns
 * The BOF thread priority
 *
 * Remarks
 * None
 */
bool ConvertOsPriorityToBofPriority(BOF_THREAD_PRIORITY *_pPriority_E, int32_t _Policy_S32, int32_t _Priority_S32)
{
	// Remove warnings
	_pPriority_E;
	_Priority_S32;
	_Policy_S32;

	bool Ret_B = true;

	if (_pPriority_E != nullptr)
	{
#if defined( __linux__ ) || defined(__APPLE__)
		int32_t Min_S32 = 0;
		int32_t Max_S32 = 0;
		int32_t Mean_S32 = 0;

		BofThread::GetPriorityRange(_Policy_S32, Min_S32, Max_S32);

		Mean_S32 = (Min_S32 + Max_S32) / 2;

		if ((_Policy_S32 == SCHED_FIFO) || (_Policy_S32 == SCHED_RR))
		{
			*_pPriority_E = BOF_THREAD_PRIORITY_TIME_CRITICAL;
		}
		else
		{
			if (_Priority_S32 == Max_S32)
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_HIGHEST;
			}
			else if (_Priority_S32 > Mean_S32)
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_ABOVE_NORMAL;
			}
			else if (_Priority_S32 == Mean_S32)
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_NORMAL;
			}
			else if (_Priority_S32 > Min_S32 + 2)
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_BELOW_NORMAL;
			}
			else if (_Priority_S32 > Min_S32 + 1)
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_LOWEST;
			}
			else if (_Priority_S32 > Min_S32)
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_ABOVE_IDLE;
			}
			else
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_IDLE;
			}
		}
#else
		switch ( _Priority_S32 )
		{
			case THREAD_PRIORITY_TIME_CRITICAL:
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_TIME_CRITICAL;
				break;
			}

			case THREAD_PRIORITY_HIGHEST:
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_HIGHEST;
				break;
			}

			case THREAD_PRIORITY_ABOVE_NORMAL:
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_ABOVE_NORMAL;
				break;
			}

			case THREAD_PRIORITY_NORMAL:
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_NORMAL;
				break;
			}

			case THREAD_PRIORITY_BELOW_NORMAL:
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_BELOW_NORMAL;
				break;
			}

			case THREAD_PRIORITY_LOWEST:
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_LOWEST;
				break;
			}

			case THREAD_PRIORITY_IDLE:
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_IDLE;
				break;
			}

			default:
			{
				*_pPriority_E = BOF_THREAD_PRIORITY_NORMAL;
				break;
			}
		}
#endif
	}
	else
	{
		Ret_B = false;
	}

	return Ret_B;
}


/*!
 * Description
 * This function creates a bof thread
 *
 * Parameters
 * _StackSize_U32  - The thread stack size (0 means default stack size)
 * _pFunc          - The thread function
 * _pContext       - The thread context
 * _Suspended_B    - The flag indicating if the thread should be created in suspended state
 *
 * Returns
 * The handle to the thread
 *
 * Remarks
 * None
 */
void *Bof_CreateThread(uint32_t _StackSize_U32, BOF_THREAD_FUNC _pFunc, void *_pContext, bool _Suspended_B)
{
	void *Ret_h = nullptr;
	BOF_THREAD_HANDLE *pHandle_X = new BOF_THREAD_HANDLE();
	BOFTHREADPARAM Params_X;

	if (pHandle_X != nullptr)
	{
		pHandle_X->pThread_O = new CBofThreadGeneric();

		if (pHandle_X->pThread_O != nullptr)
		{
			pHandle_X->Magic_U32 = BOFTHREAD_MAGIC;

			memset(&Params_X, 0x00, sizeof(Params_X));

			// Create the BofThread
			Params_X.CreateCriticalSection_B = true;
			Params_X.NbEvent_U32 = 0;
			Params_X.ManualResetEvent_B = false;
			Params_X.StartStopTimeOut_U32 = 10000;  //To avoid problem when detecting start of thread
			Params_X.WakeUpInterval_U32 = 0;

			ConvertBofPriorityToOsPriority(BOF_THREAD_PRIORITY_NORMAL, &Params_X.Policy_S32, &Params_X.Priority_S32);

			// Give the function and context
			pHandle_X->pThread_O->SetThreadFunction(_pFunc);
			pHandle_X->pThread_O->SetThreadContext(_pContext);
			pHandle_X->pThread_O->SetThreadStackSize(_StackSize_U32);
			pHandle_X->pThread_O->SetThreadParams(&Params_X);

			if (!_Suspended_B)
			{
				if (pHandle_X->pThread_O->InitializeThread(&Params_X) == 0)
				{
					//pHandle_X->pThread_O->StartThread(false, _StackSize_U32, &Params_X);
					pHandle_X->pThread_O->StartThread(false, _StackSize_U32);
				}
			}

			Ret_h = (void *) pHandle_X;
		}
		else
		{
			BOF_SAFE_DELETE(pHandle_X);
		}
	}

	return Ret_h;
}

uint32_t Bof_SetThreadName(void *_Thread_h, const char *_pName_c)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	BOF_THREAD_HANDLE *pHandle_X = (BOF_THREAD_HANDLE *) _Thread_h;
	BOFTHREADPARAM Params_X;
	CBofThreadGeneric *pThread_O = nullptr;

	if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFTHREAD_MAGIC) && (_pName_c))
	{
		pThread_O = pHandle_X->pThread_O;

		if (pThread_O != nullptr)
		{
			Ret_U32 = pThread_O->GetThreadParams(&Params_X);
			if (Ret_U32 == (uint32_t) BOFERR_NO_ERROR)
			{
				strncpy(Params_X.pName_c, _pName_c, sizeof(Params_X.pName_c));
				Params_X.pName_c[sizeof(Params_X.pName_c) - 1] = 0;
				Ret_U32 = pThread_O->SetThreadParams(&Params_X);
			}
		}
		else
		{
			Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
		}
	}
	else
	{
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}

	return Ret_U32;
}

uint32_t Bof_SetThreadCpuCoreAffinity(void *_Thread_h, uint64_t _CpuCoreAffinity_U64)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	BOF_THREAD_HANDLE *pHandle_X = (BOF_THREAD_HANDLE *) _Thread_h;
	BOFTHREADPARAM Params_X;
	CBofThreadGeneric *pThread_O = nullptr;

	if (pHandle_X == nullptr)
	{
//Set cpu thread affinity of main process
		Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;

#if defined( __linux__ ) || defined(__APPLE__) //http://stackoverflow.com/questions/6183888/is-it-possible-to-set-pthread-cpu-affinity-in-os-x
#if !defined(__ANDROID__) && !defined(__APPLE__)
		if (_CpuCoreAffinity_U64)
		{
			/* configures CPU affinity */
			cpu_set_t CPUSet_X;

			/* set CPU mask */
			CPU_ZERO(&CPUSet_X);
			CPU_SET((uint32_t) (_CpuCoreAffinity_U64 - 1), &CPUSet_X);

			/* set affinity */
			Ret_U32 = sched_setaffinity(syscall(SYS_gettid), sizeof(CPUSet_X), &CPUSet_X);
		}
		else
		{
			Ret_U32 = 0;
		}
#endif
#else
		if (_CpuCoreAffinity_U64)
		{
//A DWORD_PTR is not a pointer.It is an unsigned integer that is the same size as a pointer.Thus, in Win32 a DWORD_PTR is the same as a DWORD(32 bits), and in Win64 it is the same as a ULONGLONG(64 bits).
			DWORD_PTR Val = (DWORD_PTR)_CpuCoreAffinity_U64;
			Ret_U32 = SetThreadAffinityMask(GetCurrentThread(), Val);
		}
		else
		{
			Ret_U32 = 0;
		}
#endif
	}
	else
	{
		if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFTHREAD_MAGIC))
		{
			pThread_O = pHandle_X->pThread_O;

			if (pThread_O != nullptr)
			{
				Ret_U32 = pThread_O->GetThreadParams(&Params_X);
				if (Ret_U32 == (uint32_t) BOFERR_NO_ERROR)
				{
					Params_X.CpuCoreAffinity_U64 = _CpuCoreAffinity_U64;
					Ret_U32 = pThread_O->SetThreadParams(&Params_X);
				}
			}
			else
			{
				Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
			}
		}
		else
		{
			Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
		}
	}
	return Ret_U32;
}


/*!
 * Description
 * This function determines whether a priority level
 * as defined by this API is available or not
 *
 * Parameters
 * _Priority_E  - The thread priority
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 *
 * Remarks
 * None
 */
bool Bof_IsPriorityAvailable(BOF_THREAD_PRIORITY _Priority_E)
{
	bool Ret_B = false;
	BOF_THREAD_PRIORITY CheckPrio_E = _Priority_E;
	int32_t Policy_S32 = 0;
	int32_t Priority_S32 = 0;

	Ret_B = ConvertBofPriorityToOsPriority(_Priority_E, &Policy_S32, &Priority_S32);
	Ret_B &= ConvertOsPriorityToBofPriority(&CheckPrio_E, Policy_S32, Priority_S32);
	Ret_B &= (CheckPrio_E == _Priority_E);

	return Ret_B;
}

// Under Windows, the concept of policy is rather called a "class"
// and cannot be set for the thread. It's inherited from the process
// under which the thread is running
//->Use uint32_t Bof_SetProcessPriority(BOF_THREAD_PRIORITY _Priority_E) to set process priority (global for all thread of the process) 
//and then setup individual thread priority
//If you use GetThreadPriorityLevelyou will receive the current value of the thread which is must of the time the THREAD_PRIORITY_NORMAL value
//of the corresponding process priority class. The priority which is set to the thread correspond to the priority thread boost value which will 
// be used when needed (-;
/*
Base Priority
	The process priority class and thread priority level are combined to form the base priority of each thread.
	The following table shows the base priority for combinations of process priority class and thread priority value.

	Process priority class        Thread priority level         Base priority

	IDLE_PRIORITY_CLASS           THREAD_PRIORITY_IDLE          1
																THREAD_PRIORITY_LOWEST        2
																THREAD_PRIORITY_BELOW_NORMAL  3
																THREAD_PRIORITY_NORMAL        4
																THREAD_PRIORITY_ABOVE_NORMAL  5
																THREAD_PRIORITY_HIGHEST       6
																THREAD_PRIORITY_TIME_CRITICAL 15

	BELOW_NORMAL_PRIORITY_CLASS   THREAD_PRIORITY_IDLE          1
																THREAD_PRIORITY_LOWEST        4
																THREAD_PRIORITY_BELOW_NORMAL  5
																THREAD_PRIORITY_NORMAL        6
																THREAD_PRIORITY_ABOVE_NORMAL  7
																THREAD_PRIORITY_HIGHEST       8
																THREAD_PRIORITY_TIME_CRITICAL 15

	NORMAL_PRIORITY_CLASS         THREAD_PRIORITY_IDLE          1
																THREAD_PRIORITY_LOWEST        6
																THREAD_PRIORITY_BELOW_NORMAL  7
																THREAD_PRIORITY_NORMAL        8
																THREAD_PRIORITY_ABOVE_NORMAL  9
																THREAD_PRIORITY_HIGHEST       10
																THREAD_PRIORITY_TIME_CRITICAL 15

	ABOVE_NORMAL_PRIORITY_CLASS   THREAD_PRIORITY_IDLE          1
																THREAD_PRIORITY_LOWEST        8
																THREAD_PRIORITY_BELOW_NORMAL  9
																THREAD_PRIORITY_NORMAL        10
																THREAD_PRIORITY_ABOVE_NORMAL  11
																THREAD_PRIORITY_HIGHEST       12
																THREAD_PRIORITY_TIME_CRITICAL 15

	HIGH_PRIORITY_CLASS           THREAD_PRIORITY_IDLE          1
																THREAD_PRIORITY_LOWEST        11
																THREAD_PRIORITY_BELOW_NORMAL  12
																THREAD_PRIORITY_NORMAL        13
																THREAD_PRIORITY_ABOVE_NORMAL  14
																THREAD_PRIORITY_HIGHEST       15
																THREAD_PRIORITY_TIME_CRITICAL 15

	REALTIME_PRIORITY_CLASS       THREAD_PRIORITY_IDLE          16
																THREAD_PRIORITY_LOWEST        22
																THREAD_PRIORITY_BELOW_NORMAL  23
																THREAD_PRIORITY_NORMAL        24
																THREAD_PRIORITY_ABOVE_NORMAL  25
																THREAD_PRIORITY_HIGHEST       26
																THREAD_PRIORITY_TIME_CRITICAL 31
	*/

#if defined ( _WIN32 )
uint32_t Bof_SetProcessPriority(BOF_THREAD_PRIORITY _Priority_E)
{
	uint32_t  Ret_U32 = (uint32_t)BOFERR_NO_ERROR;
	int32_t  Priority_S32 = 0;
	bool Status_B = false;

	Status_B = ConvertBofPriorityToOsProcessPriority(_Priority_E,  &Priority_S32);

	if (Status_B)
	{
		Ret_U32 = SetPriorityClass(GetCurrentProcess(), Priority_S32) ? BOFERR_NO_ERROR : BOFERR_INTERNAL_ERROR;
	}
	else
	{
		Ret_U32 = (uint32_t)BOFERR_INVALID_PARAM;
	}
	return Ret_U32;
}
#endif

/*!
 * Description
 * This function sets the thread priority
 *
 * Parameters
 * _Thread_h    - The handle obtained by the call to Bof_CreateThread
 * _Priority_E  - The thread priority
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 *
 * Remarks
 * None
 */
uint32_t Bof_SetThreadPriority(void *_Thread_h, BOF_THREAD_PRIORITY _Priority_E)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	int32_t Policy_S32 = 0;
	int32_t Priority_S32 = 0;
	bool Status_B = false;

	Status_B = ConvertBofPriorityToOsPriority(_Priority_E, &Policy_S32, &Priority_S32);

	if (Status_B)
	{
		Ret_U32 = Bof_SetThreadPriorityNative(_Thread_h, Policy_S32, Priority_S32);
	}
	else
	{
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}

	return Ret_U32;
}


/*!
 * Description
 * This function retrieves the thread priority
 *
 * Parameters
 * _Thread_h    - The handle obtained by the call to Bof_CreateThread
 * _pPriority_E - The pointer to where to store the thread priority
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 *
 * Remarks
 * None
 */
uint32_t Bof_GetThreadPriority(void *_Thread_h, BOF_THREAD_PRIORITY *_pPriority_E)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	int32_t Policy_S32 = 0;
	int32_t Priority_S32 = 0;
	bool Status_B = false;

	if (_pPriority_E != nullptr)
	{
		Ret_U32 = Bof_GetThreadPriorityNative(_Thread_h, &Policy_S32, &Priority_S32);

		if (Ret_U32 == (uint32_t) BOFERR_NO_ERROR)
		{
			Status_B = ConvertOsPriorityToBofPriority(_pPriority_E, Policy_S32, Priority_S32);

			if (!Status_B)
			{
				Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
			}
		}
	}

	return Ret_U32;
}


/*!
 * Description
 * This function sets the thread priority
 *
 * Parameters
 * _Thread_h     - The handle obtained by the call to Bof_CreateThread
 * _Policy_S32   - The scheduling policy
 * _Priority_S32 - The thread priority
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 *
 * Remarks
 * None
 */
uint32_t Bof_SetThreadPriorityNative(void *_Thread_h, int32_t _Policy_S32, int32_t _Priority_S32)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	BOF_THREAD_HANDLE *pHandle_X = (BOF_THREAD_HANDLE *) _Thread_h;
	CBofThreadGeneric *pThread_O = nullptr;
	bool Status_B = false;
	BOFTHREADPARAM Params_X;


	if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFTHREAD_MAGIC))
	{
		pThread_O = pHandle_X->pThread_O;

		if (pThread_O != nullptr)
		{
			pThread_O->GetThreadParams(&Params_X);

			Params_X.Policy_S32 = _Policy_S32;
			Params_X.Priority_S32 = _Priority_S32;

			pThread_O->SetThreadParams(&Params_X);
			Status_B = pThread_O->SetThreadPriorityLevel(Params_X.Policy_S32, Params_X.Priority_S32);

			if (!Status_B)
			{
				Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
			}
		}
		else
		{
			Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
		}
	}
	else
	{
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}

	return Ret_U32;
}


/*!
 * Description
 * This function gets the thread priority
 *
 * Parameters
 * _Thread_h       - The handle obtained by the call to Bof_CreateThread
 * _pPolicy_S32    - The scheduling policy
 * _pPriority_S32  - The pointer to where to store the thread priority
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 *
 * Remarks
 * None
 */
uint32_t Bof_GetThreadPriorityNative(void *_Thread_h, int32_t *_pPolicy_S32, int32_t *_pPriority_S32)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	int32_t Policy_S32 = 0;
	int32_t Priority_S32 = 0;
	BOF_THREAD_HANDLE *pHandle_X = (BOF_THREAD_HANDLE *) _Thread_h;
	CBofThreadGeneric *pThread_O = nullptr;
	bool Status_B = false;

	if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFTHREAD_MAGIC) && (_pPolicy_S32 != nullptr) && (_pPriority_S32 != nullptr))
	{
		pThread_O = pHandle_X->pThread_O;

		if (pThread_O != nullptr)
		{
			Status_B = pThread_O->GetThreadPriorityLevel(Policy_S32, Priority_S32);

			if (Status_B)
			{
				*_pPolicy_S32 = Policy_S32;
				*_pPriority_S32 = Priority_S32;
			}
			else
			{
				Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
			}
		}
		else
		{
			Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
		}
	}
	else
	{
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}

	return Ret_U32;
}


/*!
 * Description
 * This function gets the exit code of the thread
 *
 * Parameters
 * _Thread_h  - The handle obtained by the call to Bof_CreateThread
 * _pRetCode  - The exit code of the thread
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 *
 * Remarks
 * None
 */
uint32_t Bof_GetThreadExitCode(void *_Thread_h, void **_ppRetCode)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	BOF_THREAD_HANDLE *pHandle_X = (BOF_THREAD_HANDLE *) _Thread_h;
	CBofThreadGeneric *pThread_O = nullptr;


	if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFTHREAD_MAGIC))
	{
		pThread_O = pHandle_X->pThread_O;

		if (pThread_O != nullptr)
		{
			if (pThread_O->IsThreadDone())
			{
				if (_ppRetCode != nullptr)
				{
					*_ppRetCode = pThread_O->GetExitCode();
				}
			}
			else
			{
				Ret_U32 = (uint32_t) BOFERR_RUNNING;
			}
		}
		else
		{
			Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
		}
	}
	else
	{
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}

	return Ret_U32;
}


/*!
 * Description
 * This function closes the BOF thread
 *
 * Parameters
 * _Thread_h  - The handle obtained by the call to Bof_CreateThread
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 *
 * Remarks
 * None
 */
uint32_t Bof_CloseThread(void *_Thread_h)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	BOF_THREAD_HANDLE *pHandle_X = (BOF_THREAD_HANDLE *) _Thread_h;
	CBofThreadGeneric *pThread_O = nullptr;

	if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFTHREAD_MAGIC))
	{
		pHandle_X->Magic_U32 = 0;
		pThread_O = pHandle_X->pThread_O;

		if (pThread_O != nullptr)
		{
			BOF_SAFE_DELETE(pThread_O);
		}
		else
		{
			Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
		}

		BOF_SAFE_DELETE(pHandle_X);
	}
	else
	{
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}

	return Ret_U32;
}


/*!
 * Description
 * This function creates a BOF event
 *
 * Parameters
 * _InitialState_B   - The event initial state
 * _pName_c          - The optional event name
 *
 * Returns
 * The handle to the created event
 *
 * Remarks
 * None
 */
void *Bof_CreateEvent(bool _InitialState_B, const char *_pName_c)
{
	void *Ret_h = nullptr;
	BOF_EVENT_HANDLE *pHandle_X = new BOF_EVENT_HANDLE();

	if (pHandle_X != nullptr)
	{
#if defined( __linux__ ) || defined(__APPLE__)
		bool AlreadyExist_B;
		void *pBaseAddress;
		// Set the clock to be CLOCK_MONOTONIC
		pthread_condattr_init(&pHandle_X->CondAttr_X);
#if defined( __linux__ ) && (!defined(__ANDROID__))
		if (pthread_condattr_setclock(&pHandle_X->CondAttr_X, CLOCK_MONOTONIC) == 0)
#endif
		{
			// Create the mutex
			if (pthread_mutex_init(&pHandle_X->Mutex_X, nullptr) == 0)
			{
				if (pthread_cond_init(&pHandle_X->Condition_X, &pHandle_X->CondAttr_X) == 0)
				{
					pHandle_X->Magic_U32 = BOFEVENT_MAGIC;
					pHandle_X->Signaled_B = false;
					pHandle_X->pSignaled_B = &pHandle_X->Signaled_B;
					pHandle_X->pMapping = nullptr;
					// This is a named event
					if (_pName_c != nullptr)
					{
						// Create a shared memory
						// where to store the condition
						pHandle_X->Named_B = true;

						pHandle_X->SharedMem_h = Bof_OpenFileMapping(_pName_c, sizeof(bool), false, &AlreadyExist_B, &pBaseAddress);
						if (pHandle_X->SharedMem_h != BOF_INVALID_HANDLE_VALUE)
						{
							pHandle_X->pMapping = pBaseAddress;  // Bof_MapViewOfFile(pHandle_X->SharedMem_h, BOF_THREAD_ACCESS_WRITE, 0, sizeof(bool));

							if (pHandle_X->pMapping != nullptr)
							{
								pHandle_X->pSignaled_B = (bool *) pHandle_X->pMapping;
								if (AlreadyExist_B)
								{
									//    						printf("AlreadyExist Create event %p state is now %d you want %d\r\n", pHandle_X, *pHandle_X->pSignaled_B, _InitialState_B);  						
								}
								else
								{
									//    						printf("Create event %p state %d\r\n", pHandle_X, _InitialState_B); 						
								}
								*pHandle_X->pSignaled_B = _InitialState_B;
								Ret_h = (void *) pHandle_X;
							}
						}
					}
					else
					{
						pHandle_X->Named_B = false;
						pHandle_X->Signaled_B = _InitialState_B;
						pHandle_X->pSignaled_B = &pHandle_X->Signaled_B;
						Ret_h = (void *) pHandle_X;
					}
				}
				else
				{
					// Error : Release the mutex
					pthread_mutex_destroy(&pHandle_X->Mutex_X);
				}
			}
		}
#else
		TCHAR pName_W[256] = { 0 };
		TCHAR *pName_c     = pName_W;
	
#if UNICODE
			if ( _pName_c != nullptr )
			{
				// On convertit le nom du programme et les arguments en Unicode
				Bof_MultiByteToWideChar(_pName_c, sizeof(pName_W) / sizeof(pName_W[0]), pName_W);
			}
			else
			{
				pName_c = nullptr;
			}
	
#else
			pName_c = (TCHAR *) _pName_c;
#endif
	
			pHandle_X->Event_h = CreateEvent(nullptr, false, _InitialState_B, pName_c);
	
			if ( pHandle_X->Event_h != nullptr )
			{
				pHandle_X->Magic_U32 = BOFEVENT_MAGIC;
	
				Ret_h = (void *) pHandle_X;
			}
#endif

		if ((Ret_h == nullptr)
		    || (Ret_h == BOF_INVALID_HANDLE_VALUE))
		{
			BOF_SAFE_DELETE(pHandle_X);
		}
	}

	return Ret_h;
}


/*!
 * Description
 * This function sets a BOF event
 *
 * Parameters
 * _Event_h  - The handle of the event
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 *
 * Remarks
 * None
 */
BOFERR Bof_SetEvent(void *_Event_h)
{
	BOFERR Rts_E = BOFERR_NO_ERROR;
	BOF_EVENT_HANDLE *pHandle_X = (BOF_EVENT_HANDLE *) _Event_h;

	if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFEVENT_MAGIC))
	{
#if defined( __linux__ ) || defined(__APPLE__)
		pthread_mutex_lock(&pHandle_X->Mutex_X);

		// Signal the "event"
		*pHandle_X->pSignaled_B = true;

		pthread_cond_signal(&pHandle_X->Condition_X);
		pthread_mutex_unlock(&pHandle_X->Mutex_X);
		if (pHandle_X->Named_B)
		{
//      printf("SetEvent %p\r\n", pHandle_X);
		}
#else
		SetEvent(pHandle_X->Event_h);
#endif
	}
	else
	{
		Rts_E = BOFERR_INVALID_PARAM;
	}

	return Rts_E;
}


/*!
 * Description
 * This function resets a BOF event
 *
 * Parameters
 * _Event_h  - The handle of the event
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 *
 * Remarks
 * None
 */
uint32_t Bof_ResetEvent(void *_Event_h)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	BOF_EVENT_HANDLE *pHandle_X = (BOF_EVENT_HANDLE *) _Event_h;

	if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFEVENT_MAGIC))
	{
#if defined( __linux__ ) || defined(__APPLE__)
		pthread_mutex_lock(&pHandle_X->Mutex_X);

		// Set the event to "non signaled"
		*pHandle_X->pSignaled_B = false;

		pthread_mutex_unlock(&pHandle_X->Mutex_X);
		if (pHandle_X->Named_B)
		{
//      printf("ResetEvent %p\r\n", pHandle_X);
		}
#else
		ResetEvent(pHandle_X->Event_h);
#endif
	}
	else
	{
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}

	return Ret_U32;
}


/*!
 * Description
 * This function resets a BOF event
 *
 * Parameters
 * _Event_h      - The handle of the event
 * _Timeout_U32  - The timeout value
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 * BOFERR_TIMEOUT   - A timeout occurred
 *
 * Remarks
 * None
 */
uint32_t Bof_WaitForEvent(void *_Event_h, uint32_t _Timeout_U32)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	uint32_t Status_U32 = 0;
	BOF_EVENT_HANDLE *pHandle_X = (BOF_EVENT_HANDLE *) _Event_h;

	if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFEVENT_MAGIC))
	{
#if defined( __linux__ ) || defined(__APPLE__)
		struct timespec TimeSpec_X;
		int Status_i = 0;

		pthread_mutex_lock(&pHandle_X->Mutex_X);

		if (_Timeout_U32 == BOF_THREAD_INFINITE_TIMEOUT)
		{
			while ((*pHandle_X->pSignaled_B == false) /*&& (Status_i == 0)*/ )
			{
				Status_i = pthread_cond_wait(&pHandle_X->Condition_X, &pHandle_X->Mutex_X);
			}
		}
		else
		{
			// Get the current time and add it timeout
#if defined (__APPLE__)
			clock_serv_t cclock;
			mach_timespec_t mts;
			host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
			clock_get_time(cclock, &mts);
			mach_port_deallocate(mach_task_self(), cclock);
			TimeSpec_X.tv_sec = mts.tv_sec;
			TimeSpec_X.tv_nsec= mts.tv_nsec;

#else
			clock_gettime(CLOCK_MONOTONIC, &TimeSpec_X);
#endif
			TimeSpec_X.tv_sec += (_Timeout_U32 / 1000);
			TimeSpec_X.tv_nsec += (_Timeout_U32 % 1000) * 1000000;

			// Make sure nsec is not greater than it should be
			while (TimeSpec_X.tv_nsec >= 1000000000)
			{
				TimeSpec_X.tv_sec += 1;
				TimeSpec_X.tv_nsec -= 1000000000;
			}
			Status_i = 0;
			while ((*pHandle_X->pSignaled_B == false) && (Status_i != ETIMEDOUT))
			{
				if (_Timeout_U32 == 0)  //pthread_cond_timedwait with this value wait for ever
				{
					Status_i = ETIMEDOUT;
					break;
				}
				else
				{
					Status_i = pthread_cond_timedwait(&pHandle_X->Condition_X, &pHandle_X->Mutex_X, &TimeSpec_X);

					// An error occurred : get out
					if ((Status_i != ETIMEDOUT) || (Status_i != 0))
					{
						break;
					}
				}
			}
			if (Status_i == ETIMEDOUT)
			{
				Ret_U32 = (uint32_t) BOFERR_TIMEOUT;
			}
			else if (Status_i != 0)
			{
				Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
			}
			if ((pHandle_X->Named_B) && (_Timeout_U32 == 1)) //Dbg
			{
//        printf("Wait for %p to %d signaled %d st %d/%d\r\n", pHandle_X, _Timeout_U32, *pHandle_X->pSignaled_B, Status_i, Ret_U32);
			}
		}

		*pHandle_X->pSignaled_B = false;

		pthread_mutex_unlock(&pHandle_X->Mutex_X);
#else
		Status_U32 = WaitForSingleObject(pHandle_X->Event_h, _Timeout_U32);

		if ( Status_U32 == WAIT_TIMEOUT )
		{
			Ret_U32 = (uint32_t)BOFERR_TIMEOUT;
		}
		else if ( Status_U32 != WAIT_OBJECT_0 )
		{
			Ret_U32 = (uint32_t)BOFERR_INTERNAL_ERROR;
		}
#endif
	}
	else
	{
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}

	return Ret_U32;
}


/*!
 * Description
 * This function closes an opened BOF event
 * and releases its associated resources
 *
 * Parameters
 * _Event_h  - The handle of the event
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 * BOFERR_TIMEOUT   - A timeout occurred
 *
 * Remarks
 * None
 */
uint32_t Bof_CloseEvent(void *_Event_h)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	BOF_EVENT_HANDLE *pHandle_X = (BOF_EVENT_HANDLE *) _Event_h;

	if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFEVENT_MAGIC))
	{
#if defined( __linux__ ) || defined(__APPLE__)
		pthread_mutex_destroy(&pHandle_X->Mutex_X);
		pthread_condattr_destroy(&pHandle_X->CondAttr_X);
		pthread_cond_destroy(&pHandle_X->Condition_X);

		if (pHandle_X->Named_B)
		{
			if (pHandle_X->SharedMem_h != BOF_INVALID_HANDLE_VALUE)
			{
				Bof_CloseFileMapping(pHandle_X->SharedMem_h, pHandle_X->pMapping, sizeof(bool), true);
			}
		}
#else
		CloseHandle(pHandle_X->Event_h);
#endif

		pHandle_X->Magic_U32 = 0;

		BOF_SAFE_DELETE(pHandle_X);
	}
	else
	{
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}

	return Ret_U32;
}


/*!
 * Description
 * This function creates a mutex
 *
 * If the mutex already exist the _HasOwnership_B
 * is simply ignored
 *
 * The name of the mutex is depend of the OS system.
 *
 * On Windows, a global mutex might be named "Global\MyMutex"
 *
 * On Linux, it should be a combination between a valid file path
 * and an ID (different of 0) ideally related to the current application to
 * avoid clashes. E.g. "/home/user/MyProg.a/12" where "/home/user/MyProg.a"
 * is a path to a file that exists and "12" is a unique ID
 *
 * Parameters
 * _HasOwnership_B - The flag indicating if we should have ownership of the created mutex
 * _pName_c        - The name of the mutex (nullptr means anonymous)
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 * BOFERR_TIMEOUT   - A timeout occurred
 * BOFERR_ALREADY_EXIST  - The object already exist
 *
 * Remarks
 * None
 */
void *Bof_CreateMutex(bool _HasOwnership_B, const char *_pName_c)
{
	void *Ret_h = nullptr;
	BOF_MUTEX_HANDLE *pHandle_X = new BOF_MUTEX_HANDLE();
	bool Ok_B = false;

	if (pHandle_X != nullptr)
	{
#if defined( __linux__ ) || defined(__APPLE__)
		key_t SemKey = -1;
		int Flag_i;
		//char          pFile_c[512];
		//char          *pId_c;
		int Id_i;
		if (_pName_c != nullptr)
		{
#if defined(__ANDROID__)
#else
			//BOF_SEM_UNION Arg_X;
			BOF_SEM_SETGETVAL Ctl_X;
			pHandle_X->Named_B = true;

			SemKey = Bof_GenerateSystemVKey(true, _pName_c, 0);
			if (SemKey != -1)
			{
				// Get the semaphore if it already exists
				pHandle_X->MutexId_i = semget(SemKey, 0, 0);
				// The semaphore does not exist : create it
				if (pHandle_X->MutexId_i == -1)
				{
					Flag_i = IPC_CREAT;                                                                              // Creation flag
					Flag_i |= S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;                                                  // Access flags

					// Create or open the associated semaphore set
					pHandle_X->MutexId_i = semget(SemKey, 1, Flag_i);
					if (pHandle_X->MutexId_i >= 0)
					{
						// Initialize the semaphore value
						Ctl_X.val = (_HasOwnership_B ? 0 : 1);

						if (semctl(pHandle_X->MutexId_i, 0, SETVAL, Ctl_X) != -1)
						{
							Ok_B = true;
						}
					}
				}
				else
				{
					Ok_B = true;
				}
			}

#endif
		}
		else
		{
			pHandle_X->Named_B = false;

			if (pthread_mutex_init(&pHandle_X->Mutex_X, nullptr) == 0)
			{
				if (_HasOwnership_B)
				{
					// Lock the mutex
					if (pthread_mutex_lock(&pHandle_X->Mutex_X) == 0)
					{
						Ok_B = true;
					}
				}
				else
				{
					Ok_B = true;
				}
			}
		}

#else
		TCHAR pName_W[256] = { 0 };
		TCHAR *pName_c     = pName_W;
	
#if UNICODE
			if ( _pName_c != nullptr )
			{
				// On convertit le nom du programme et les arguments en Unicode
				Bof_MultiByteToWideChar(_pName_c, sizeof(pName_W) / sizeof(pName_W[0]), pName_W);
			}
			else
			{
				pName_c = nullptr;
			}
	
#else
			pName_c = (TCHAR *) _pName_c;
#endif
	
			pHandle_X->Mutex_h = CreateMutex(nullptr, _HasOwnership_B, pName_c);
	
			if ( pHandle_X->Mutex_h != nullptr )
			{
				Ok_B = true;
			}
#endif

		if (Ok_B)
		{
			pHandle_X->Magic_U32 = BOFMUTEX_MAGIC;
			Ret_h = (void *) pHandle_X;
		}
		else
		{
			BOF_SAFE_DELETE(pHandle_X);
		}
	}

	return Ret_h;
}


/*!
 * Description
 * This function acquires the given mutex
 *
 * Parameters
 * _Mutex_h  - The handle of the mutex
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 * BOFERR_TIMEOUT   - A timeout occurred
 * BOFERR_ALREADY_EXIST  - The object already exist
 *
 * Remarks
 * None
 */
uint32_t Bof_WaitForMutex(void *_Mutex_h)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	BOF_MUTEX_HANDLE *pHandle_X = (BOF_MUTEX_HANDLE *) _Mutex_h;

	if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFMUTEX_MAGIC))
	{
#if defined( __linux__ ) || defined(__APPLE__)
		if (pHandle_X->Named_B)
		{
#if defined(__ANDROID__)
#else
			struct sembuf SemBuf_X;

			SemBuf_X.sem_num = 0;
			SemBuf_X.sem_op = -1;
			SemBuf_X.sem_flg = SEM_UNDO;

			if (semop(pHandle_X->MutexId_i, &SemBuf_X, 1) != 0)
			{
				Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
			}
#endif
		}
		else
		{
			if (pthread_mutex_lock(&pHandle_X->Mutex_X) != 0)
			{
				Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
			}
		}

#else
		if ( WaitForSingleObject(pHandle_X->Mutex_h, INFINITE) != WAIT_OBJECT_0 )
		{
			Ret_U32 = (uint32_t)BOFERR_INTERNAL_ERROR;
		}
#endif
	}
	else
	{
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}

	return Ret_U32;
}


/*!
 * Description
 * This function releases the given mutex
 *
 * Parameters
 * _Mutex_h  - The handle of the mutex
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 * BOFERR_TIMEOUT   - A timeout occurred
 * BOFERR_ALREADY_EXIST  - The object already exist
 *
 * Remarks
 * None
 */
uint32_t Bof_ReleaseMutex(void *_Mutex_h)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	BOF_MUTEX_HANDLE *pHandle_X = (BOF_MUTEX_HANDLE *) _Mutex_h;

	if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFMUTEX_MAGIC))
	{
#if defined( __linux__ ) || defined(__APPLE__)
		if (pHandle_X->Named_B)
		{
#if defined(__ANDROID__)
#else
			struct sembuf SemBuf_X;

			SemBuf_X.sem_num = 0;
			SemBuf_X.sem_op = +1;
			SemBuf_X.sem_flg = SEM_UNDO;

			if (semop(pHandle_X->MutexId_i, &SemBuf_X, 1) != 0)
			{
				Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
			}
#endif
		}
		else
		{
			if (pthread_mutex_unlock(&pHandle_X->Mutex_X) != 0)
			{
				Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
			}
		}

#else
		if ( !ReleaseMutex(pHandle_X->Mutex_h) )
		{
			Ret_U32 = (uint32_t)BOFERR_INTERNAL_ERROR;
		}
#endif
	}
	else
	{
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}

	return Ret_U32;
}


/*!
 * Description
 * This function closes the given mutex
 * and frees its associated resources
 *
 * Parameters
 * _Mutex_h  - The handle of the mutex
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 * BOFERR_TIMEOUT   - A timeout occurred
 * BOFERR_ALREADY_EXIST  - The object already exist
 *
 * Remarks
 * None
 */
uint32_t Bof_CloseMutex(void *_Mutex_h)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	BOF_MUTEX_HANDLE *pHandle_X = (BOF_MUTEX_HANDLE *) _Mutex_h;

	if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFMUTEX_MAGIC))
	{
#if defined( __linux__ ) || defined(__APPLE__)
		if (pHandle_X->Named_B)
		{
#if defined(__ANDROID__)
#else
			if (semctl(pHandle_X->MutexId_i, 0, IPC_RMID) != 0)
			{
				Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
			}
#endif
		}
		else
		{
			if (pthread_mutex_destroy(&pHandle_X->Mutex_X) == -1)
			{
				Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
			}
		}
#else
		if ( !CloseHandle(pHandle_X->Mutex_h) )
		{
			Ret_U32 = (uint32_t)BOFERR_INTERNAL_ERROR;
		}
#endif

		pHandle_X->Magic_U32 = 0;

		BOF_SAFE_DELETE(pHandle_X);
	}
	else
	{
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}

	return Ret_U32;
}


/*!
 * Description
 * This function creates a file mapping
 *
 * Parameters
 * _AccessType_U32   - The requested access type
 * _pName_c          - The name of the shared memory
 * _Size_U32         - The size in bytes of the shared memory
 * _pAlreadyExist_B  - The optional pointer to where to store the flag indicating if the shared memory already exists
 *
 * Returns
 * The handle to the shared memory
 *
 * Remarks
 * None
 */
//BHA->Should use Bof_OpenFileMapping and co in bofportability 
//and Bof_CreateFileMapping does not correcly generate SemKey = ftok(pFile_c, (char) Id_i); ->use GenerateSystemVKey
// if app crash shared memory stay active=>this is mainly use by nma to store event name->this should also work with Bof_OpenFileMapping=> a confimer avec nma
void *Bof_CreateFileMapping(uint32_t _AccessType_U32, const char *_pName_c, uint32_t _Size_U32, bool *_pAlreadyExist_B)
{
	void *Ret_h = nullptr;
	BOF_SHARED_MEM_HANDLE *pHandle_X = new BOF_SHARED_MEM_HANDLE();
	bool Ok_B = false;
	uint32_t Protect_U32 = 0;

	if (pHandle_X != nullptr)
	{
#if defined( __linux__ ) || defined(__APPLE__)

		key_t SemKey;
//  int   Flag_i;
		//char  pFile_c[512];
		//char  *pId_c;
		int Id_i, Flag_i;


		if (_pName_c != nullptr)
		{
#if defined(__ANDROID__)
#else

			//    IdKey = ftok("/dev/null", _pName_c[0]);                     //ftok need an exiting filename /dev/null is supposed to be present on alll unix station...
			SemKey = Bof_GenerateSystemVKey(true, _pName_c, 0);
			if (SemKey != -1)
			{
				// Get the semaphore if it already exists
				pHandle_X->SharedMemId_i = shmget(SemKey, (size_t) _Size_U32, 0);

				// The semaphore does not exist : create it
				if (pHandle_X->SharedMemId_i == -1)
				{
					if (_pAlreadyExist_B != nullptr)
					{
						*_pAlreadyExist_B = false;
					}

					// Creation flag
					Flag_i = IPC_CREAT;

					// Set the permissions flags
					if ((_AccessType_U32 & BOF_THREAD_ACCESS_READ) == BOF_THREAD_ACCESS_READ)
					{
						Flag_i |= (S_IRUSR | S_IRGRP);
					}

					if ((_AccessType_U32 & BOF_THREAD_ACCESS_WRITE) == BOF_THREAD_ACCESS_WRITE)
					{
						Flag_i |= (S_IWUSR | S_IWGRP);
					}

					if ((_AccessType_U32 & BOF_THREAD_ACCESS_EXECUTE) == BOF_THREAD_ACCESS_EXECUTE)
					{
						Flag_i |= (S_IXUSR | S_IXGRP);
					}

					// Create the shared memory
					pHandle_X->SharedMemId_i = (int) shmget(SemKey, (size_t) _Size_U32, Flag_i);

					if (pHandle_X->SharedMemId_i >= 0)
					{
						Ok_B = true;
					}
				}
				else
				{
					if (_pAlreadyExist_B != nullptr)
					{
						*_pAlreadyExist_B = true;
					}

					Ok_B = true;
				}
			}
#endif
		}
#else
		TCHAR pName_W[256] = { 0 };
		TCHAR *pName_c     = pName_W;
	
#if UNICODE
			if ( _pName_c != nullptr )
			{
				// On convertit le nom du programme et les arguments en Unicode
				Bof_MultiByteToWideChar(_pName_c, sizeof(pName_W) / sizeof(pName_W[0]), pName_W);
			}
			else
			{
				pName_c = nullptr;
			}
	
#else
			pName_c = (TCHAR *) _pName_c;
#endif
	
			// Set the protection bits
			if ( ( _AccessType_U32 & BOF_THREAD_ACCESS_NONE ) == BOF_THREAD_ACCESS_NONE )
			{
				Protect_U32 = PAGE_NOACCESS;
			}
			else
			{
				if ( ( _AccessType_U32 & BOF_THREAD_ACCESS_EXECUTE_READWRITE ) == BOF_THREAD_ACCESS_EXECUTE_READWRITE )
				{
					Protect_U32 |= PAGE_EXECUTE_READWRITE;
				}
				else if ( ( _AccessType_U32 & BOF_THREAD_ACCESS_EXECUTE_READ ) == BOF_THREAD_ACCESS_EXECUTE_READ )
				{
					Protect_U32 |= PAGE_EXECUTE_READ;
				}
				else if ( ( _AccessType_U32 & BOF_THREAD_ACCESS_EXECUTE_WRITE ) == BOF_THREAD_ACCESS_EXECUTE_WRITE )
				{
					Protect_U32 |= PAGE_EXECUTE_WRITECOPY;
				}
				else if ( ( _AccessType_U32 & BOF_THREAD_ACCESS_READWRITE ) == BOF_THREAD_ACCESS_READWRITE )
				{
					Protect_U32 |= PAGE_READWRITE;
				}
				else if ( ( _AccessType_U32 & BOF_THREAD_ACCESS_READ ) == BOF_THREAD_ACCESS_READ )
				{
					Protect_U32 |= PAGE_READONLY;
				}
				else if ( ( _AccessType_U32 & BOF_THREAD_ACCESS_WRITE ) == BOF_THREAD_ACCESS_WRITE )
				{
					Protect_U32 |= PAGE_WRITECOPY;
				}
			}
	
			// Create the file mapping
			pHandle_X->SharedMem_h = CreateFileMapping(nullptr, nullptr, Protect_U32, 0, _Size_U32, pName_c);
	
			if ( pHandle_X->SharedMem_h != nullptr )
			{
				Ok_B = true;
	
				if ( _pAlreadyExist_B != nullptr )
				{
					*_pAlreadyExist_B = ( GetLastError( ) == ERROR_ALREADY_EXISTS );
				}
			}
#endif

		if (Ok_B)
		{
			pHandle_X->Magic_U32 = BOFSHAREDMEM_MAGIC;

			Ret_h = (void *) pHandle_X;
		}
		else
		{
			BOF_SAFE_DELETE(pHandle_X);
		}
	}

	return Ret_h;
}


/*!
 * Description
 * This function unmaps an opened file mapping
 * from the current process space
 *
 * Parameters
 * _pMap - The pointer to the mapped zone
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 * BOFERR_TIMEOUT   - A timeout occurred
 * BOFERR_ALREADY_EXIST  - The object already exist
 *
 * Remarks
 * None
 */
uint32_t Bof_UnmapViewOfFile(void *_pMap)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;

	if (_pMap != nullptr)
	{
#if defined( __linux__ ) || defined(__APPLE__)
#if defined(__ANDROID__)
#else
		if (shmdt(_pMap) == -1)
		{
			Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
		}
#endif
#else
		if ( !UnmapViewOfFile(_pMap) )
		{
			Ret_U32 = (uint32_t)BOFERR_INTERNAL_ERROR;
		}
#endif
	}
	else
	{
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}

	return Ret_U32;
}


/*!
 * Description
 * This function closes an opened file mapping
 *
 * Parameters
 * _FileMap_h - The handle to the file mapping
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 * BOFERR_TIMEOUT   - A timeout occurred
 * BOFERR_ALREADY_EXIST  - The object already exist
 *
 * Remarks
 * None
 */
uint32_t Bof_RemoveFileMapping(void *_FileMap_h)
{
	uint32_t Ret_U32 = (uint32_t) BOFERR_NO_ERROR;
	BOF_SHARED_MEM_HANDLE *pHandle_X = (BOF_SHARED_MEM_HANDLE *) _FileMap_h;

	if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFSHAREDMEM_MAGIC))
	{
#if defined( __linux__ ) || defined(__APPLE__)
#if defined(__ANDROID__)
#else
		if (shmctl(pHandle_X->SharedMemId_i, IPC_RMID, nullptr) != 0)
		{
			Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
		}
#endif
#else
		if ( !CloseHandle(pHandle_X->SharedMem_h) )
		{
			Ret_U32 = (uint32_t)BOFERR_INTERNAL_ERROR;
		}
#endif

		pHandle_X->Magic_U32 = 0;

		BOF_SAFE_DELETE(pHandle_X);
	}
	else
	{
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}

	return Ret_U32;
}


/*!
* Description
* This function maps an opened file mapping
* into the current process space
*
* Parameters
* _FileMap_h      - The handle to the shared memory
* _AccessType_U32 - The requested access type
* _Offset_U32     - The offset in the mapped zone
* _Size_U32       - The size in bytes to map
*
* Returns
* The pointer to the mapped zone
*
* Remarks
* None
*/
void *Bof_MapViewOfFile(void *_FileMap_h, uint32_t _AccessType_U32, uint32_t _Offset_U32, uint32_t _Size_U32)
{
	void *pRet = nullptr;
	BOF_SHARED_MEM_HANDLE *pHandle_X = (BOF_SHARED_MEM_HANDLE *) _FileMap_h;
	uint32_t Protect_U32 = 0;

	if ((pHandle_X != nullptr) && (pHandle_X->Magic_U32 == BOFSHAREDMEM_MAGIC))
	{
#if defined( __linux__ ) || defined(__APPLE__)
#if defined(__ANDROID__)
#else
		int Flag_i = 0;

		if (_AccessType_U32 == BOF_THREAD_ACCESS_READ)
		{
			Flag_i = SHM_RDONLY;
		}

		pRet = shmat(pHandle_X->SharedMemId_i, nullptr, Flag_i);
#endif

#else

		// Set the protection bits
		if ((_AccessType_U32 & BOF_THREAD_ACCESS_EXECUTE) == BOF_THREAD_ACCESS_EXECUTE)
		{
			Protect_U32 = FILE_MAP_WRITE;
		}
		else if ((_AccessType_U32 & BOF_THREAD_ACCESS_WRITE) == BOF_THREAD_ACCESS_WRITE)
		{
			Protect_U32 = FILE_MAP_WRITE;
		}
		else if ((_AccessType_U32 & BOF_THREAD_ACCESS_READ) == BOF_THREAD_ACCESS_READ)
		{
			Protect_U32 = FILE_MAP_READ;
		}

		pRet = MapViewOfFile(pHandle_X->SharedMem_h, Protect_U32, 0, _Offset_U32, _Size_U32);
#endif
	}

	return pRet;
}

/*!
 * Description
 * This function maps an opened file mapping
 * into the virtual memory space
 *
 * Parameters
 * _AccessType_U32 - The requested access type
 * _pName_c        - The device name to Map
 * _Offset_U32     - The offset in the mapped zone
 * _Length_U32     - The length in bytes to map
 *
 * Returns
 * The pointer to the mapped zone
 *
 * Remarks
 * None
 */
void *Bof_CreateVirtualMemoryMap(uint32_t _AccessType_U32, const char *_pName_c, uint32_t _Offset_U32, uint32_t _Length_U32)
{
	// Remove warnings
	_AccessType_U32;
	_pName_c;
	_Offset_U32;
	_Length_U32;

	void *pRet = nullptr;

#if defined( __linux__ ) || defined(__APPLE__)
	int _fdmem;
	struct stat sb;
	off_t pa_offset;
	size_t length = (size_t) _Length_U32;
	int FlagOpen_i = 0;
	int FlagMap_i = 0;

	/* Set the permissions flags */
	if ((_AccessType_U32 & BOF_THREAD_ACCESS_READWRITE) == BOF_THREAD_ACCESS_READWRITE)
	{
		FlagOpen_i = O_RDWR;
		FlagMap_i = (PROT_READ | PROT_WRITE);
	}
	else if ((_AccessType_U32 & BOF_THREAD_ACCESS_READ) == BOF_THREAD_ACCESS_READ)
	{
		FlagOpen_i = O_RDONLY;
		FlagMap_i = PROT_READ;
	}
	else if ((_AccessType_U32 & BOF_THREAD_ACCESS_WRITE) == BOF_THREAD_ACCESS_WRITE)
	{
		FlagOpen_i = O_WRONLY;
		FlagMap_i = PROT_WRITE;
	}

	/* open memDevice */
	_fdmem = open(_pName_c, FlagOpen_i | O_SYNC);

	if (_fdmem < 0)
	{
//    perror("Failed to open the device/file !\n");
	}
	else
	{
		/* To obtain file size */
		if (fstat(_fdmem, &sb) == -1)
		{
//      perror("Failed to obtain file size fstat !\n");
		}
		else
		{
			/* offset for mmap() must be page aligned */
			pa_offset = (off_t) _Offset_U32 & ~(sysconf(_SC_PAGE_SIZE) - 1);

			if (pa_offset != (off_t) _Offset_U32)
			{
//        perror("offset is not page aligned !\n");
			}
			else
			{
				if ((off_t) _Offset_U32 >= sb.st_size)
				{
//          perror("offset is past end of file !\n");
				}
				else
				{
					if (pa_offset + length > sb.st_size)
					{
						//length = sb.st_size - pa_offset; /* Cannot display bytes past end of file */
//            perror("Length past end of file !\n");
					}
					else
					{
						/* map */
						pRet = (void *) (mmap(nullptr, length, FlagMap_i, MAP_SHARED, _fdmem, pa_offset));

						if (pRet == MAP_FAILED)
						{
							pRet = nullptr;
//              perror("Failed to map !\n");
						}
					}
				}
			}
		}
	}

	/* close the character device */
	close(_fdmem);
#endif

	return pRet;
}


/*!
 * Description
 * This function unmaps an opened file mapping
 * from the current virtual memory space
 *
 * Parameters
 * _pMap       - The pointer to the virtual memory mapped zone
 * _Length_U32 - Length of the virtual memory mapped zone
 *
 * Returns
 * BOFERR_NO_ERROR             - The operation was successful
 * BOFERR_INVALID_PARAM  - At least one parameter is not valid
 * BOFERR_INTERNAL_ERROR - An internal error occurred
 *
 * Remarks
 * None
 */
uint32_t Bof_CloseVirtualMemoryMap(void *pMap, uint32_t _Length_U32)
{
	// Remove warnings
	pMap;
	_Length_U32;

	uint32_t Ret_U32 = (uint32_t) BOFERR_NOT_SUPPORTED;

#if defined( __linux__ ) || defined(__APPLE__)
	Ret_U32 = (uint32_t) BOFERR_NO_ERROR;

	if (pMap != nullptr)
	{
		/* unmap */
		if (munmap(pMap, (size_t) _Length_U32) == -1)
		{
//      perror("Error munmap()");
			Ret_U32 = (uint32_t) BOFERR_INTERNAL_ERROR;
		}
	}
	else
	{
//    perror("Error pMap nullptr pointer");
		Ret_U32 = (uint32_t) BOFERR_INVALID_PARAM;
	}
#endif

	return Ret_U32;
}
END_BOF_NAMESPACE()

