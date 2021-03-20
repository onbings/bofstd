/*
 * Copyright (c) 2000-2020, Onbings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bofsystem interface. It wraps os dependent system call
 *
 * Name:        bofsystem.h
 * Author:      Bernard HARMEL: b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Jan 19 2017  BHA : Initial release
 */
/*** Include files ***********************************************************/

#include <bofstd/bofstd.h>
#include <bofstd/bofsystem.h>
#include <bofstd/bofstring.h>
#include <bofstd/bofstringformatter.h>
#include <bofstd/bofbit.h>

#include <thread>
#include <random>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>

#if defined (_WIN32)
#include <Winsock2.h>
#include <Sddl.h>
#include <conio.h>
#include <windows.h>

PSECURITY_DESCRIPTOR CreateWinSecurityDescriptor(SECURITY_ATTRIBUTES *_pSecurityAttribute_X)
{
	PSECURITY_DESCRIPTOR pRts_X = nullptr, pSecurityDescriptor_X = nullptr;

	// PSID					pSid_X=nullptr;
	// PACL					pAcl_X=nullptr;

	if ( (_pSecurityAttribute_X) &&
			 ( (pSecurityDescriptor_X = LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH) ) != 0)
			 )
	{
		memset(_pSecurityAttribute_X, 0, sizeof SECURITY_ATTRIBUTES);

		if ( (InitializeSecurityDescriptor(pSecurityDescriptor_X, SECURITY_DESCRIPTOR_REVISION) ) )
		{
			if (SetSecurityDescriptorDacl(pSecurityDescriptor_X, true, nullptr, false) )
			{
				_pSecurityAttribute_X->nLength              = sizeof(SECURITY_ATTRIBUTES);
				_pSecurityAttribute_X->lpSecurityDescriptor = pSecurityDescriptor_X;
				_pSecurityAttribute_X->bInheritHandle       = false;
				pRts_X                                      = pSecurityDescriptor_X;
			}
		}
	}

	if (! pRts_X)
	{
		if (pSecurityDescriptor_X)
		{
			LocalFree(pSecurityDescriptor_X);
		}
	}
	return pRts_X;
}

#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <signal.h>

BEGIN_BOF_NAMESPACE()

static std::random_device S_Rd;
static std::mt19937 S_RandomGenerator(S_Rd());
static std::uniform_real_distribution<float> S_RandomFloatDistribution(0.0, 1.0f);

BOFERR Bof_OpenSharedMemory(const std::string &_rName_S, uint32_t _SizeInByte_U32, BOF_SHARED_MEMORY &_rSharedMemory_X)
{
	BOFERR Rts_E = BOF_ERR_ALREADY_OPENED;

	if (_rSharedMemory_X.Magic_U32 != BOF_FILEMAPPING_MAGIC)
	{
		_rSharedMemory_X.Reset();
		_rSharedMemory_X.Name_S = _rName_S;
		_rSharedMemory_X.SizeInByte_U32 = _SizeInByte_U32;
#if defined (_WIN32)
		WCHAR                pName_wc[0x1000];
		PSECURITY_DESCRIPTOR pSecurityDescriptor_X;
		SECURITY_ATTRIBUTES  SecurityAttribute_X;

		Rts_E = BOF_ERR_EACCES;
		// Name must start with Global\\->Responsabilities of the caller !
		if ((strncmp(_rSharedMemory_X.Name_S.c_str(), "Global\\", 7)) && (strncmp(_rSharedMemory_X.Name_S.c_str(), "Local\\", 6)))
		{
		}
		// bool k=CreateMyDACL(&SecurityAttribute_X);
		pSecurityDescriptor_X = CreateWinSecurityDescriptor(&SecurityAttribute_X);

		if ((pSecurityDescriptor_X)
			&& (Bof_MultiByteToWideChar(_rSharedMemory_X.Name_S.c_str(), sizeof(pName_wc) / sizeof(WCHAR), pName_wc)>=0)
			)
		{
			Rts_E = BOF_ERR_EINVAL;
			// Name must start with Global\\->Responsabilities of the caller ! sinon			Rts_h = BOF_INVALID_HANDLE_VALUE;
			_rSharedMemory_X.pHandle = CreateFileMapping(BOF_INVALID_HANDLE_VALUE, &SecurityAttribute_X, PAGE_READWRITE | SEC_COMMIT, 0, _SizeInByte_U32, pName_wc);
			if (_rSharedMemory_X.pHandle)
			{
        Rts_E = Bof_GetLastError(false);
				_rSharedMemory_X.Magic_U32 = BOF_FILEMAPPING_MAGIC;
				_rSharedMemory_X.pBaseAddress = MapViewOfFile(_rSharedMemory_X.pHandle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
			}
		}

#else
		std::string Name_S;
		int Handle_i;
		Rts_E = BOF_ERR_EINVAL;
		if (isalpha(_rSharedMemory_X.Name_S[0]))
		{
//Posix shm name must begin with a /
			Name_S = "/" + _rSharedMemory_X.Name_S;
			Handle_i = shm_open(Name_S.c_str(), O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
			if (Handle_i >= 0)
			{
				Rts_E = (ftruncate(Handle_i, _rSharedMemory_X.SizeInByte_U32) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_WRONG_SIZE;
			}
			else
			{
				Handle_i = shm_open(Name_S.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
				if (Handle_i >= 0)
				{
					Rts_E = BOF_ERR_EEXIST;
				}
			}
			if (Handle_i >= 0)
			{
				_rSharedMemory_X.pBaseAddress = mmap(nullptr, _SizeInByte_U32, PROT_READ | PROT_WRITE, MAP_SHARED, Handle_i, 0);
				if (_rSharedMemory_X.pBaseAddress != MAP_FAILED)
				{
					if (Rts_E != BOF_ERR_EEXIST)
					{
						Rts_E = BOF_ERR_NO_ERROR;
					}
					_rSharedMemory_X.Magic_U32 = BOF_FILEMAPPING_MAGIC;
				}
				close(Handle_i); //After a call to mmap(2) the file descriptor may be closed without affecting the memory mapping.
			}
		}
#endif
	}
	return Rts_E;
}

bool Bof_IsSharedMemoryValid(BOF_SHARED_MEMORY &_rSharedMemory_X)
{
	return(_rSharedMemory_X.Magic_U32 == BOF_FILEMAPPING_MAGIC);
}

BOFERR Bof_CloseSharedMemory(BOF_SHARED_MEMORY &_rSharedMemory_X)
{
	BOFERR Rts_E = BOF_ERR_NOT_OPENED;

	if (_rSharedMemory_X.Magic_U32 == BOF_FILEMAPPING_MAGIC)
	{
#if defined (_WIN32)
		if (_rSharedMemory_X.pBaseAddress)
		{
			Rts_E = UnmapViewOfFile(_rSharedMemory_X.pBaseAddress) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
		}
		if (_rSharedMemory_X.pHandle)
		{
			Rts_E = CloseHandle(_rSharedMemory_X.pHandle) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
		}
#else
		if (_rSharedMemory_X.pBaseAddress)
		{
			Rts_E = (munmap(_rSharedMemory_X.pBaseAddress, _rSharedMemory_X.SizeInByte_U32) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
		}
//		Rts_E=Bof_DeleteSharedMemory(_rSharedMemory_X.Name_S);

#endif
//		Rts_E = BOF_ERR_NO_ERROR;
		_rSharedMemory_X.Reset();
	}
	return Rts_E;
}

BOFERR Bof_DestroySharedMemory(const std::string &_rName_S)
{
	BOFERR Rts_E;

#if defined (_WIN32)
	Rts_E = BOF_ERR_NO_ERROR;	//Shared memory dos not survive when its father process creator exit or die <-> posix linux behaviour
#else
	std::string Name_S;
	Rts_E = BOF_ERR_EINVAL;
	if (isalpha(_rName_S[0]))
	{
		Name_S = "/" + _rName_S;
		Rts_E = (shm_unlink(Name_S.c_str()) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_EACCES;
	}
#endif
	return Rts_E;
}

BOFERR Bof_CreateMutex(const std::string &_rName_S, bool _Recursive_B, bool _PriorityInversionAware_B, BOF_MUTEX &_rMtx_X)
{
	BOFERR Rts_E = BOF_ERR_EEXIST;

	if (_rMtx_X.Magic_U32 != BOF_MUTEX_MAGIC)
	{
		_rMtx_X.Reset();
		_rMtx_X.Magic_U32 = BOF_MUTEX_MAGIC;
		_rMtx_X.Name_S = _rName_S;
		_rMtx_X.Recursive_B = _Recursive_B;
#if defined (_WIN32)
#else
		if (_PriorityInversionAware_B)
		{
			//https://sakhnik.com/2017/07/16/custom-mutex.html

			// Destroy the underlying mutex
			::pthread_mutex_destroy(_rMtx_X.Mtx.native_handle());
			::pthread_mutex_destroy(_rMtx_X.RecursiveMtx.native_handle());

			// Create mutex attribute with desired protocol
			pthread_mutexattr_t Attributes_X;

			pthread_mutexattr_init(&Attributes_X);
			pthread_mutexattr_setprotocol(&Attributes_X, PTHREAD_PRIO_INHERIT);
			::pthread_mutex_init(_rMtx_X.Mtx.native_handle(), &Attributes_X);
			::pthread_mutexattr_destroy(&Attributes_X);

			pthread_mutexattr_init(&Attributes_X);
			pthread_mutexattr_settype(&Attributes_X, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutexattr_setprotocol(&Attributes_X, PTHREAD_PRIO_INHERIT);
			::pthread_mutex_init(_rMtx_X.RecursiveMtx.native_handle(), &Attributes_X);
			::pthread_mutexattr_destroy(&Attributes_X);
		}
		//	BOFERR e=Bof_LockMutex(_rMtx_X);
		//	e=Bof_LockMutex(_rMtx_X);
#endif
		Rts_E = BOF_ERR_NO_ERROR;
	}
	return Rts_E;

}

bool Bof_IsMutexValid(BOF_MUTEX &_rMtx_X)
{
	return (_rMtx_X.Magic_U32 == BOF_MUTEX_MAGIC);
}
BOFERR Bof_LockMutex(BOF_MUTEX &_rMtx_X)
{
	BOFERR Rts_E = BOF_ERR_INIT;

	if (_rMtx_X.Magic_U32 == BOF_MUTEX_MAGIC)
	{
		_rMtx_X.Recursive_B ? _rMtx_X.RecursiveMtx.lock() : _rMtx_X.Mtx.lock();
		Rts_E = BOF_ERR_NO_ERROR;
	}
	return Rts_E;
}


BOFERR Bof_UnlockMutex(BOF_MUTEX &_rMtx_X)
{
	BOFERR Rts_E = BOF_ERR_INIT;

	if (_rMtx_X.Magic_U32 == BOF_MUTEX_MAGIC)
	{
		_rMtx_X.Recursive_B ? _rMtx_X.RecursiveMtx.unlock() : _rMtx_X.Mtx.unlock();
		Rts_E = BOF_ERR_NO_ERROR;
	}
	return Rts_E;
}


BOFERR Bof_DestroyMutex(BOF_MUTEX &_rMtx_X)
{
	BOFERR Rts_E = BOF_ERR_INIT;

	if (_rMtx_X.Magic_U32 == BOF_MUTEX_MAGIC)
	{
		_rMtx_X.Reset();
		Rts_E = BOF_ERR_NO_ERROR;
	}
	return Rts_E;
}

BOFERR Bof_CreateSemaphore(const std::string &_rName_S, int32_t _InitialCount_S32, BOF_SEMAPHORE &_rSem_X)
{
	BOFERR Rts_E = BOF_ERR_EEXIST;

	if (_rSem_X.Magic_U32 != BOF_SEMAPHORE_MAGIC)
	{
		Rts_E = BOF_ERR_EINVAL;
		if (_InitialCount_S32 > 0)
		{
			_rSem_X.Reset();
			_rSem_X.Name_S = _rName_S;
			_rSem_X.Magic_U32 = BOF_SEMAPHORE_MAGIC;
			_rSem_X.Max_S32 = _InitialCount_S32 - 1;
			_rSem_X.Cpt_S32 = _InitialCount_S32 - 1;
			Rts_E = BOF_ERR_NO_ERROR;
		}
	}
	return Rts_E;
}

bool Bof_IsSemaphoreValid(BOF_SEMAPHORE &_rSem_X)
{
	return  (_rSem_X.Magic_U32 == BOF_SEMAPHORE_MAGIC);
}
BOFERR Bof_SignalSemaphore(BOF_SEMAPHORE &_rSem_X)
{
	BOFERR Rts_E = BOF_ERR_INIT;

	if (_rSem_X.Magic_U32 == BOF_SEMAPHORE_MAGIC)
	{
		std::unique_lock<std::mutex> WaitLock_O(_rSem_X.Mtx);
		if (_rSem_X.Cpt_S32 < _rSem_X.Max_S32)
		{
			_rSem_X.Cpt_S32++;
			_rSem_X.Cv.notify_one();
			Rts_E = BOF_ERR_NO_ERROR;
		}
		else
		{
			Rts_E = BOF_ERR_TOO_BIG;
		}
	}
	return Rts_E;

}


BOFERR Bof_WaitForSemaphore(BOF_SEMAPHORE &_rSem_X, uint32_t _TimeoutInMs_U32)
{
	BOFERR Rts_E = BOF_ERR_INIT;

	if (_rSem_X.Magic_U32 == BOF_SEMAPHORE_MAGIC)
	{
		std::unique_lock<std::mutex> WaitLock_O(_rSem_X.Mtx);
		Rts_E = (_rSem_X.Cv.wait_for(WaitLock_O, std::chrono::milliseconds(_TimeoutInMs_U32), [&]() { return (_rSem_X.Cpt_S32 >= 0); })) ? BOF_ERR_NO_ERROR : BOF_ERR_ETIMEDOUT;
		if (Rts_E == BOF_ERR_NO_ERROR)
		{
			BOF_ASSERT(_rSem_X.Cpt_S32 >= 0);
			_rSem_X.Cpt_S32--;
		}
	}
	return Rts_E;
}

BOFERR Bof_DestroySemaphore(BOF_SEMAPHORE &_rSem_X)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	if (_rSem_X.Magic_U32 == BOF_SEMAPHORE_MAGIC)
	{
		_rSem_X.Reset();
		Rts_E = BOF_ERR_NO_ERROR;
	}
	return Rts_E;
}

BOFERR Bof_CreateConditionalVariable(const std::string &_rName_S, bool _NotifyAll_B, BOF_CONDITIONAL_VARIABLE &_rCv_X)
{
	BOFERR Rts_E = BOF_ERR_EEXIST;

	if (_rCv_X.Magic_U32 != BOF__CONDITIONAL_VARIABLE_MAGIC)
	{
		_rCv_X.Reset();
		_rCv_X.Name_S = _rName_S;
		_rCv_X.Magic_U32 = BOF__CONDITIONAL_VARIABLE_MAGIC;
		_rCv_X.NotifyAll_B = _NotifyAll_B;
		Rts_E = BOF_ERR_NO_ERROR;
	}
	return Rts_E;
}

BOFERR Bof_DestroyConditionalVariable(BOF_CONDITIONAL_VARIABLE &_rCv_X)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	if (_rCv_X.Magic_U32 == BOF__CONDITIONAL_VARIABLE_MAGIC)
	{
		_rCv_X.Reset();
		Rts_E = BOF_ERR_NO_ERROR;
	}
	return Rts_E;
}

BOFERR Bof_CreateEvent(const std::string &_rName_S, bool _InitialState_B, /*bool _NotifyAll_B*/ uint32_t _MaxNumberToNotify_U32, bool _WaitKeepSignaled_B, BOF_EVENT &_rEvent_X)
{
	BOFERR Rts_E = BOF_ERR_EEXIST;

	if (_rEvent_X.Magic_U32 != BOF_EVENT_MAGIC)
	{
		Rts_E = BOF_ERR_EINVAL;
		if ((_MaxNumberToNotify_U32) && (_MaxNumberToNotify_U32 <= 64))
		{
			_rEvent_X.Reset();
			_rEvent_X.Magic_U32 = BOF_EVENT_MAGIC;
			_rEvent_X.Name_S = _rName_S;
			//		_rEvent_X.Canceled_B = false;
			_rEvent_X.SignaledBitmask_U64 = _InitialState_B ? (1 << (_MaxNumberToNotify_U32 - 1)) - 1:0;
			_rEvent_X.MaxNumberToNotify_U32 = _MaxNumberToNotify_U32;
			_rEvent_X.WaitKeepSignaled_B = _WaitKeepSignaled_B;
			Rts_E = BOF_ERR_NO_ERROR;
		}
	}

	return Rts_E;
}

bool Bof_IsEventValid(BOF_EVENT &_rEvent_X)
{
	return (_rEvent_X.Magic_U32 == BOF_EVENT_MAGIC);
}

BOFERR Bof_SignalEvent(BOF_EVENT &_rEvent_X, /*bool _CancelIt_B*/ uint32_t _Instance_U32)
{
	BOFERR Rts_E = BOF_ERR_INIT;

	if (_rEvent_X.Magic_U32 == BOF_EVENT_MAGIC)
	{
		Rts_E = BOF_ERR_EINVAL;
		if (_Instance_U32 < _rEvent_X.MaxNumberToNotify_U32)
		{
			std::unique_lock<std::mutex> WaitLock_O(_rEvent_X.Mtx);
			/*
			if (_CancelIt_B)
			{
				_rEvent_X.Canceled_B = true;
			}
			else
			{
				_rEvent_X.Signaled_B = true;
			}
			 */
			_rEvent_X.SignaledBitmask_U64 |= (static_cast<uint64_t>(1) << _Instance_U32);
			//NotifyAll_B will wake up all thread but in my implemenation only one will see the signaled event. Only Canceled event are seen by all thread
			(_rEvent_X.MaxNumberToNotify_U32 == 1) ? _rEvent_X.Cv.notify_one() : _rEvent_X.Cv.notify_all() ;
			Rts_E = BOF_ERR_NO_ERROR;
		}
	}

	return Rts_E;
}

//Done in WaitForEvent but if it is not called we can need to reset it. It is also used to reset the canceled state
BOFERR Bof_ResetEvent(BOF_EVENT &_rEvent_X, uint32_t _Instance_U32)
{
	BOFERR Rts_E = BOF_ERR_INIT;

	if (_rEvent_X.Magic_U32 == BOF_EVENT_MAGIC)
	{
		Rts_E = BOF_ERR_EINVAL;
		if (_Instance_U32 < _rEvent_X.MaxNumberToNotify_U32)
		{
			std::unique_lock<std::mutex> WaitLock_O(_rEvent_X.Mtx);
			_rEvent_X.SignaledBitmask_U64 &= ~(1 << _Instance_U32);
			//_rEvent_X.Canceled_B = false;
			Rts_E = BOF_ERR_NO_ERROR;
		}
	}

	return Rts_E;
}

bool Bof_IsEventSignaled(BOF_EVENT &_rEvent_X, uint32_t _Instance_U32)
{
	bool Rts_B = false;

	if (_rEvent_X.Magic_U32 == BOF_EVENT_MAGIC)
	{
		if (_Instance_U32 < _rEvent_X.MaxNumberToNotify_U32)
		{
			std::unique_lock<std::mutex> WaitLock_O(_rEvent_X.Mtx);
			Rts_B = (_rEvent_X.SignaledBitmask_U64 & (static_cast<uint64_t>(1) << _Instance_U32)) ? true : false;
		}
	}

	return Rts_B;
}

/*
bool Bof_IsEventCanceled(BOF_EVENT &_rEvent_X)
{
	bool Rts_B = false;

	if (_rEvent_X.Magic_U32 == BOF_EVENT_MAGIC)
	{
		std::unique_lock<std::mutex> WaitLock_O(_rEvent_X.Mtx);
		Rts_B = _rEvent_X.Canceled_B;
	}
	return Rts_B;
}
*/
BOFERR Bof_WaitForEvent(BOF_EVENT &_rEvent_X, uint32_t _TimeoutInMs_U32, uint32_t _Instance_U32)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	if (_rEvent_X.Magic_U32 == BOF_EVENT_MAGIC)
	{
		Rts_E = BOF_ERR_EINVAL;
		if (_Instance_U32 < _rEvent_X.MaxNumberToNotify_U32)
		{
			std::unique_lock<std::mutex> WaitLock_O(_rEvent_X.Mtx);
			if (_rEvent_X.Cv.wait_for(WaitLock_O, std::chrono::milliseconds(_TimeoutInMs_U32), [&]() { return (_rEvent_X.SignaledBitmask_U64 & (static_cast<uint64_t>(1) << _Instance_U32) ? true : false /*|| _rEvent_X.Canceled_B*/); }))
			{
				if (!_rEvent_X.WaitKeepSignaled_B)
				{
					_rEvent_X.SignaledBitmask_U64 &= ~(1 << _Instance_U32); //Bof_ResetEvent
				}
	//Do not reset Canceled state-> all running thread waiting for it will see it->Only Bof_ResetEvent can reset canceled state
	//			Rts_E = _rEvent_X.Canceled_B ? BOF_ERR_CANCEL : BOF_ERR_NO_ERROR;
				Rts_E = BOF_ERR_NO_ERROR;
			}
			else
			{
				Rts_E = BOF_ERR_ETIMEDOUT;
			}
		}
	}

	return Rts_E;
}

BOFERR Bof_DestroyEvent(BOF_EVENT &_rEvent_X)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	if (_rEvent_X.Magic_U32 == BOF_EVENT_MAGIC)
	{
		_rEvent_X.Reset();
		Rts_E = BOF_ERR_NO_ERROR;
	}

	return Rts_E;
}
BOF_THREAD_PRIORITY Bof_ThreadPriorityFromValue(int32_t _Priority_S32)
{
	BOF_THREAD_PRIORITY Rts_E= BOF_THREAD_NONE;

	if ((_Priority_S32 >= 0) && (_Priority_S32 <= 99))
	{
#if defined (_WIN32)
		if (_Priority_S32 <= 15)
		{
			Rts_E = BOF_THREAD_PRIORITY_000;
		}
		else if (_Priority_S32 <= 32)
		{
			Rts_E = BOF_THREAD_PRIORITY_016;
		}
		else if (_Priority_S32 <= 49)
		{
			Rts_E = BOF_THREAD_PRIORITY_033;
		}
		else if (_Priority_S32 == 50)
		{
			Rts_E = BOF_THREAD_PRIORITY_050;
		}
		else if (_Priority_S32 <= 67)
		{
			Rts_E = BOF_THREAD_PRIORITY_051;
		}
		else if (_Priority_S32 <= 84)
		{
			Rts_E = BOF_THREAD_PRIORITY_068;
		}
		else
		{
			Rts_E = BOF_THREAD_PRIORITY_085;
		}
#else
		Rts_E = static_cast<BOF_THREAD_PRIORITY>(_Priority_S32);
#endif
	}
	return Rts_E;
}

int32_t Bof_ValueFromThreadPriority(BOF_THREAD_PRIORITY _Priority_E)
{
	int32_t Rts_S32 = -1024;


#if defined (_WIN32)
	if (_Priority_E == BOF_THREAD_PRIORITY_IDLE)
	{
		Rts_S32 = -15;
	}
	else if (_Priority_E == BOF_THREAD_PRIORITY_TIME_CRITICAL)
	{
		Rts_S32 = 15;
	}
	else if (_Priority_E == BOF_THREAD_DEFAULT_PRIORITY)
	{
		Rts_S32 = 0x7FFFFFFF;
	}
	else if (_Priority_E == BOF_THREAD_NONE)
	{
		Rts_S32 = 0x7FFFFFFE;
	}
	else if ((_Priority_E >= BOF_THREAD_PRIORITY_000) && (_Priority_E <= BOF_THREAD_PRIORITY_099))
	{
		if (_Priority_E <= BOF_THREAD_PRIORITY_015)
		{
			Rts_S32 = 0;
		}
		else if (_Priority_E <= BOF_THREAD_PRIORITY_032)
		{
			Rts_S32 = 16;
		}
		else if (_Priority_E <= BOF_THREAD_PRIORITY_049)
		{
			Rts_S32 = 32;
		}
		else if (_Priority_E == BOF_THREAD_PRIORITY_050)
		{
			Rts_S32 = 50;
		}
		else if (_Priority_E <= BOF_THREAD_PRIORITY_067)
		{
			Rts_S32 = 51;
		}
		else if (_Priority_E <= BOF_THREAD_PRIORITY_084)
		{
			Rts_S32 = 67;
		}
		else
		{
			Rts_S32 = 85;
		}
	}
#else
	if (_Priority_E == BOF_THREAD_PRIORITY_IDLE)
	{
		Rts_S32 = 1;
	}
	else if (_Priority_E == BOF_THREAD_PRIORITY_TIME_CRITICAL)
	{
		Rts_S32 = 99;
	}
	else if (_Priority_E == BOF_THREAD_DEFAULT_PRIORITY)
	{
		Rts_S32 = 0x7FFFFFFF;
	}
	else if (_Priority_E == BOF_THREAD_NONE)
	{
		Rts_S32 = 0x7FFFFFFE;
	}
	else if ((_Priority_E >= BOF_THREAD_PRIORITY_000) && (_Priority_E <= BOF_THREAD_PRIORITY_099))
	{
		Rts_S32 = static_cast<int32_t>(_Priority_E);
	}
#endif
	
	return Rts_S32;
}
BOFERR Bof_GetThreadPriorityRange(BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF_THREAD_PRIORITY &_rMin_E, BOF_THREAD_PRIORITY &_rMax_E)
{
	BOFERR Rts_E;

#if defined (_WIN32)
  _rMin_E = BOF_THREAD_PRIORITY_IDLE;
  _rMax_E = BOF_THREAD_PRIORITY_TIME_CRITICAL;
	Rts_E     = BOF_ERR_NO_ERROR;
#else
	int Min_i = sched_get_priority_min(_ThreadSchedulerPolicy_E);
	int Max_i = sched_get_priority_max(_ThreadSchedulerPolicy_E);

	if ((Min_i != EINVAL) && (Max_i != EINVAL))
	{
    _rMin_E = (BOF_THREAD_PRIORITY) Min_i;
    _rMax_E = (BOF_THREAD_PRIORITY) Max_i;
		Rts_E = BOF_ERR_NO_ERROR;
	}
#endif
	return Rts_E;
}

/*!
 * Description
 * This function retrieve current scheduling
 * policy and priority of the current thread
 *
 * Parameters
 * _rPolicy_E   - The policy of the thread
 * _rPriority_E - The priority of the thread
 *
 * Returns
 * true  - The operation was successful
 * false - The operation failed
 *
 * Remarks
 * None
 */
BOFERR Bof_GetThreadPriorityLevel(BOF_THREAD &_rThread_X, BOF_THREAD_SCHEDULER_POLICY &_rPolicy_E, BOF_THREAD_PRIORITY &_rPriority_E)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	if (_rThread_X.Magic_U32 == BOF_THREAD_MAGIC)
	{
#if defined (_WIN32)
		void               *Process_h = GetCurrentProcess();

    _rPolicy_E = (BOF_THREAD_SCHEDULER_POLICY)GetPriorityClass(Process_h);
    _rPriority_E = (BOF_THREAD_PRIORITY) ::GetThreadPriority(_rThread_X.pThread);
		Rts_E          = BOF_ERR_NO_ERROR;
#else
		int Status_i = 0;
		int Policy_i = 0;
		struct sched_param Params_X;

		Rts_E = BOF_ERR_EINVAL;
		Status_i = pthread_getschedparam(_rThread_X.ThreadId, &Policy_i, &Params_X);

		if (Status_i == 0)
		{
      _rPolicy_E = (BOF_THREAD_SCHEDULER_POLICY) Policy_i;
      _rPriority_E = (BOF_THREAD_PRIORITY) Params_X.sched_priority;
			Rts_E = BOF_ERR_NO_ERROR;
		}
#endif
	}
	return Rts_E;
}


/*!
 * Description
 * This function sets the policy and priority
 * of the thread
 *
 * Parameters
 * _ThreadSchedulerPolicy_E   - The policy of the thread
 * _ThreadPriority_E - The priority of the thread
 *
 * Returns
 * true  - The operation was successful
 * false - The operation failed
 *
 * Remarks
 * None
 */

BOFERR Bof_SetThreadPriorityLevel(BOF_THREAD &_rThread_X, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF_THREAD_PRIORITY _ThreadPriority_E)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	if (_rThread_X.Magic_U32 == BOF_THREAD_MAGIC)
	{
#if defined (_WIN32)
		Rts_E = (::SetThreadPriority(_rThread_X.pThread, _ThreadPriority_E) == TRUE) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
#else
		int Status_i = 0;
		int Policy_i = (int)_ThreadSchedulerPolicy_E;
		struct sched_param Params_X;
		Rts_E = BOF_ERR_EINVAL;

		if (_rThread_X.ThreadId != 0)
		{
			Params_X.sched_priority = (int)_ThreadPriority_E;

			// Set the priority
			Status_i = pthread_setschedparam(_rThread_X.ThreadId, Policy_i, &Params_X);

			// Verify
			if (Status_i == 0)
			{
				Status_i = pthread_getschedparam(_rThread_X.ThreadId, &Policy_i, &Params_X);

				Rts_E = ((Policy_i == (int)_ThreadSchedulerPolicy_E) && (Params_X.sched_priority == (int)_ThreadPriority_E)) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
			}
		}
			// Thread ID does not exist. Under linux,
			// we cannot create a suspended thread. Let's
			// assume that the thread has simply not yet
			// be created.
		else
		{
			Rts_E = BOF_ERR_NO_ERROR;
		}

#endif
	}
	return Rts_E;
}

BOFERR Bof_CreateThread(const std::string &_rName_S, BofThreadFunction _ThreadFunction, void *_pUserContext, BOF_THREAD &_rThread_X)
{
	BOFERR Rts_E = BOF_ERR_EEXIST;

	if (_rThread_X.Magic_U32 != BOF_THREAD_MAGIC)
	{
		Rts_E = BOF_ERR_EINVAL;
		if (_ThreadFunction != nullptr)
		{
			_rThread_X.Reset();
			_rThread_X.Magic_U32 = BOF_THREAD_MAGIC;
			_rThread_X.Name_S = _rName_S;
			_rThread_X.ThreadFunction = _ThreadFunction;
			_rThread_X.pUserContext = _pUserContext;
			_rThread_X.ThreadLoopMustExit_B = false;
			_rThread_X.ThreadRunning_B = false;
#if defined (_WIN32)
			_rThread_X.ThreadId_U32 = 0;
			_rThread_X.pThread = nullptr;
#else
			_rThread_X.ThreadId = 0;
#endif
			Rts_E = BOF_ERR_NO_ERROR;
		}
	}
	return Rts_E;
}

bool Bof_IsThreadValid(BOF_THREAD &_rThread_X)
{
	return (_rThread_X.Magic_U32 == BOF_THREAD_MAGIC);
}

static void *S_ThreadLauncher(void *_pContext)
{
	BOF_THREAD *pThread_X;
	void *pRts = nullptr;

	pThread_X = static_cast< BOF_THREAD * > (_pContext);
	if (pThread_X)
	{
#if defined (_WIN32)
		//Make a copy to use it after object del
				void                   *pThreadHandle=pThread_X->pThread;
#else
		BOFERR Sts_E;
		(void) Sts_E;
		int OldType_i = 0;
		Sts_E = (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &OldType_i) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
#endif

		if (pThread_X->ThreadCpuCoreAffinity_U32)
		{
#if defined (_WIN32)
			// A DWORD_PTR is not a pointer.It is an unsigned integer that is the same size as a pointer.Thus, in Win32 a DWORD_PTR is the same as a DWORD(32 bits), and in Win64 it is the same as a ULONGLONG(64 bits).
			DWORD_PTR Val = (DWORD_PTR)(static_cast<uint64_t>(1) << pThread_X->ThreadCpuCoreAffinity_U32);
			SetThreadAffinityMask(GetCurrentThread(), Val);
#else
			/* configures CPU affinity */
			cpu_set_t CpuSet_X;
			/* set CPU mask */
			CPU_ZERO(&CpuSet_X);
			CPU_SET(pThread_X->ThreadCpuCoreAffinity_U32 - 1, &CpuSet_X);
			/* set affinity */
			Sts_E = (sched_setaffinity(static_cast<__pid_t>(syscall(SYS_gettid)), sizeof(CpuSet_X), &CpuSet_X) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
#endif
		}

#if defined (_WIN32)
		//		Sts_E = (::SetThreadPriority(pThread_X->pThread, pThread_X->ThreadPriority_E) == TRUE) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
				::SetThreadPriority(pThread_X->pThread, pThread_X->ThreadPriority_E);
#else
		int Status_i = 0;
		int Policy_i = pThread_X->ThreadSchedulerPolicy_E;
		struct sched_param Params_X;
		Sts_E = BOF_ERR_INTERNAL;
		Params_X.sched_priority = pThread_X->ThreadPriority_E;
		// Set the priority
		Status_i = pthread_setschedparam(pThread_X->ThreadId, Policy_i, &Params_X);
		// Verify
		if (Status_i == 0)
		{
			Status_i = pthread_getschedparam(pThread_X->ThreadId, &Policy_i, &Params_X);
			Sts_E = ((Policy_i == pThread_X->ThreadSchedulerPolicy_E) && (Params_X.sched_priority == pThread_X->ThreadPriority_E)) ? BOF_ERR_NO_ERROR : BOF_ERR_INTERNAL;
		}

#endif
		pThread_X->ThreadRunning_B = true;
		pRts = pThread_X->ThreadFunction(pThread_X->ThreadLoopMustExit_B, pThread_X->pUserContext);

#if defined(_WIN32)
		/*
		 Needed to avoid memory leak !!!
	 */
			 CloseHandle(pThreadHandle);
#endif
		pThread_X->ThreadRunning_B = false;
	}
	return pRts;
}

BOFERR Bof_LaunchThread(BOF_THREAD &_rThread_X, uint32_t _StackSize_U32, uint32_t _ThreadCpuCoreAffinity_U32, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF_THREAD_PRIORITY _ThreadPriority_E, uint32_t _StartStopTimeoutInMs_U32)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	uint32_t Start_U32, Delta_U32;
  BOF_THREAD_PRIORITY Min_E, Max_E;

	if (_rThread_X.Magic_U32 == BOF_THREAD_MAGIC)
	{
		if (_ThreadPriority_E == BOF_THREAD_DEFAULT_PRIORITY)
		{
			Rts_E = Bof_GetThreadPriorityRange(_ThreadSchedulerPolicy_E, Min_E, Max_E);
      _ThreadPriority_E = (BOF_THREAD_PRIORITY)((Max_E + Min_E) / 2);
		}
		else
		{
			Rts_E = BOF_ERR_NO_ERROR;
		}

		if (Rts_E == BOF_ERR_NO_ERROR)
		{
			_rThread_X.StackSize_U32 = _StackSize_U32;
			_rThread_X.ThreadCpuCoreAffinity_U32 = _ThreadCpuCoreAffinity_U32;
			_rThread_X.ThreadSchedulerPolicy_E = _ThreadSchedulerPolicy_E;
			_rThread_X.ThreadPriority_E = _ThreadPriority_E;
			_rThread_X.StartStopTimeoutInMs_U32 = _StartStopTimeoutInMs_U32;

#if defined (_WIN32)
			_rThread_X.pThread = CreateThread(nullptr, _rThread_X.StackSize_U32, (LPTHREAD_START_ROUTINE)S_ThreadLauncher, (void *)&_rThread_X, _rThread_X.StackSize_U32 ? STACK_SIZE_PARAM_IS_A_RESERVATION : 0, (DWORD *)&_rThread_X.ThreadId_U32);
			Rts_E = (_rThread_X.pThread != nullptr) ? BOF_ERR_NO_ERROR : BOF_ERR_CREATE;
#else
			/*
				* Dans les exemples pr?c?dents, les threads sont cr??s en mode JOINABLE, c'est ? dire que le processus qui a cr?? le thread attend la fin de celui-ci en
				* restant bloqu? sur l'appel ? pthread_join. Lorsque le thread se termine, les resources m?moire du thread sont lib?r?es gr?ce ? l'appel ? pthread_join.
				* Si cet appel n'est pas effectu?, la m?moire n'est pas lib?r?e et il s'en suit une fuite de m?moire. Pour ?viter un appel syst?matique ? pthread_join
				* (qui peut parfois ?tre contraignant dans certaines applications), on peut cr?er le thread en mode DETACHED.
				*/
			pthread_attr_t ThreadAttr_X;
			pthread_attr_init(&ThreadAttr_X);
			pthread_attr_setdetachstate(&ThreadAttr_X, PTHREAD_CREATE_DETACHED); // PTHREAD_CREATE_JOINABLE) if joinable need to join to avoid memory leak;
			Rts_E = BOF_ERR_CREATE;
			if (_rThread_X.StackSize_U32)
			{
				pthread_attr_setstacksize(&ThreadAttr_X, _rThread_X.StackSize_U32);
			}
			if (pthread_create(&_rThread_X.ThreadId, &ThreadAttr_X, S_ThreadLauncher, (void *) &_rThread_X) == 0)
			{
				Rts_E = BOF_ERR_NO_ERROR;
			}
			else
			{
				Start_U32 = errno;
				_rThread_X.ThreadId = 0;
			}
			pthread_attr_destroy(&ThreadAttr_X);
#endif
		}
		if (Rts_E == BOF_ERR_NO_ERROR)
		{
#if defined (_WIN32)
#else
			pthread_setname_np(_rThread_X.ThreadId, _rThread_X.Name_S.c_str());
#endif
			if (!_rThread_X.StartStopTimeoutInMs_U32)
			{
				_rThread_X.ThreadRunning_B = true; // In this case we consider that the thread will start one day as the thread creation is ok but we do not want to wait
			}
			else
			{
				Start_U32 = Bof_GetMsTickCount();

				while (!_rThread_X.ThreadRunning_B)
				{
          Bof_MsSleep(0);                    // Yield scheduler
					Delta_U32 = Bof_ElapsedMsTime(Start_U32);

					if (Delta_U32 > _rThread_X.StartStopTimeoutInMs_U32)
					{
						break;
					}
				}
			}
			if (_rThread_X.ThreadRunning_B)
			{
				Rts_E = BOF_ERR_NO_ERROR;
			}
			else
			{
				Bof_DestroyThread(_rThread_X);     // Thread has not started in the given time slot->MUST destroy it
			}
		}
	}
	return Rts_E;
}


BOFERR Bof_DestroyThread(BOF_THREAD &_rThread_X)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	uint32_t Start_U32, Delta_U32;
	bool ThreadStopTo_B = false;

	if (_rThread_X.Magic_U32 == BOF_THREAD_MAGIC)
	{
		Rts_E = BOF_ERR_NO_ERROR;
		_rThread_X.Magic_U32 = 0;  //Cannot make _rThread_X.Reset() at the end of the funct as for example BofThread will clean up this memory zone on thread exit->we just cancel th Magic number to signal closure
		if (_rThread_X.ThreadRunning_B)
		{
			_rThread_X.ThreadLoopMustExit_B = true;
			if (!_rThread_X.StartStopTimeoutInMs_U32)
			{
				ThreadStopTo_B = _rThread_X.ThreadRunning_B;
			}
			else
			{
				Start_U32 = Bof_GetMsTickCount();
				while (_rThread_X.ThreadRunning_B)
				{
          Bof_MsSleep(0); //Bof_MsSleep(0);->yield is not enough
					Delta_U32 = Bof_ElapsedMsTime(Start_U32);
					if (Delta_U32 > _rThread_X.StartStopTimeoutInMs_U32)
					{
						ThreadStopTo_B = true;
						break;
					}
				}
			}
		}
#if defined (_WIN32)
		bool Sts_B;
		if (ThreadStopTo_B)
		{
			Sts_B = TerminateThread(_rThread_X.pThread, 0x69696969) ? true : false;
#if defined(DEBUG)
			printf("%d Kill thread '%s' Status %d\r\n", Bof_GetMsTickCount(), _rThread_X.Name_S.c_str(), Sts_B);
#endif
		}
#else
		if (ThreadStopTo_B)
		{
#if defined(DEBUG)
			printf("%d Should Kill thread '%s'\r\n", Bof_GetMsTickCount(), _rThread_X.Name_S.c_str());
#endif
		}
#endif
	}

	return Rts_E;
}


uint32_t Bof_CurrentThreadId()
{
#if defined (_WIN32)
  uint32_t Rts_U32 = static_cast<uint32_t>(GetCurrentThreadId());
#else
  uint32_t Rts_U32 = static_cast<uint32_t>(pthread_self());
#endif
  return Rts_U32;
}
uint32_t Bof_InterlockedCompareExchange(uint32_t volatile *_pDestination_U32, uint32_t _ValueToSetIfEqual_U32, uint32_t _CheckIfEqualToThis_U32)
{
	uint32_t Rts_U32;

#if defined (_WIN32)
	// The function returns the initial value of the Destination parameter.
	Rts_U32 = InterlockedCompareExchange((LONG *)_pDestination_U32, _ValueToSetIfEqual_U32, _CheckIfEqualToThis_U32);
#else
	// The function returns the initial value of the Destination parameter.
	Rts_U32 = __sync_val_compare_and_swap(_pDestination_U32, _CheckIfEqualToThis_U32, _ValueToSetIfEqual_U32);
#endif
	return Rts_U32;
}

BOFERR Bof_GetMemoryState(uint64_t &_rAvailableFreeMemory_U64, uint64_t &_rTotalMemorySize_U64)
{
	BOFERR Rts_E = BOF_ERR_INTERNAL;

#if defined (_WIN32)
	MEMORYSTATUSEX Ms_X;
	Ms_X.dwLength             = sizeof(Ms_X);
	if (GlobalMemoryStatusEx(&Ms_X) )
	{
		_rAvailableFreeMemory_U64 = Ms_X.ullAvailPhys;
		_rTotalMemorySize_U64     = Ms_X.ullTotalPhys;

		Rts_E                     = BOF_ERR_NO_ERROR;
	}
	else
	{
		_rAvailableFreeMemory_U64 = 0;
		_rTotalMemorySize_U64     = 0;
	}
#else
	int PageSize_i = static_cast<int>(sysconf(_SC_PAGE_SIZE));

	_rAvailableFreeMemory_U64 = (get_avphys_pages() * PageSize_i);
	_rTotalMemorySize_U64 = (get_phys_pages() * PageSize_i);
	Rts_E = BOF_ERR_NO_ERROR;
#endif
	return Rts_E;
}
/*** Bof_FileTimeToSystemTime ******************************************************/

/*!
* Description
* The FileTimeToSystemTime function converts a file time to system time format.
*
* Parameters
* _pFileTime_U64: Pointer to a FILETIME structure containing the file time to convert to system date and time format.
* This value must be less than 0x8000000000000000. Otherwise,the function fails.
* _pSystemTime_X:	\Returns the result
*
* Returns
* bool: true if the function succeeds,
*
* Remarks
* Prototype based on Win32 function
*/
BOFERR Bof_FileTimeToSystemTime(uint64_t _FileTime_U64, BOF_DATE_TIME &_rDateTime_X)
{
  BOFERR Rts_E = BOF_ERR_INTERNAL;

#if defined( _WIN32 )
  FILETIME FileTime_X;
  SYSTEMTIME SystemTime_X;

  FileTime_X.dwLowDateTime = (uint32_t)_FileTime_U64;
  FileTime_X.dwHighDateTime = (uint32_t)(_FileTime_U64 >> 32);

  if (FileTimeToSystemTime(&FileTime_X, &SystemTime_X))
  {
    _rDateTime_X.DayOfWeek_U8 = (uint8_t)SystemTime_X.wDayOfWeek;
    _rDateTime_X.Day_U8 = (uint8_t)SystemTime_X.wDay;
    _rDateTime_X.Month_U8 = (uint8_t)SystemTime_X.wMonth;
    _rDateTime_X.Year_U16 = (uint16_t)SystemTime_X.wYear;
    _rDateTime_X.Hour_U8 = (uint8_t)SystemTime_X.wHour;
    _rDateTime_X.Minute_U8 = (uint8_t)SystemTime_X.wMinute;
    _rDateTime_X.Second_U8 = (uint8_t)SystemTime_X.wSecond;
    _rDateTime_X.Millisecond_U16 = (uint16_t)SystemTime_X.wMilliseconds;
    Rts_E = BOF_ERR_NO_ERROR;
  }
#else
  time_t t = (time_t)_FileTime_U64;
  struct tm *pNow_X = localtime(&t); //Or gmtime ?

  if (pNow_X)
  {
    //    char buff[128];
    //    strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", localtime(&t));
    _rDateTime_X.DayOfWeek_U8 = (uint8_t)pNow_X->tm_wday;
    _rDateTime_X.Day_U8 = (uint8_t)pNow_X->tm_mday;
    _rDateTime_X.Month_U8 = (uint8_t)(pNow_X->tm_mon + 1);
    _rDateTime_X.Year_U16 = (uint16_t)(pNow_X->tm_year + 1900);
    _rDateTime_X.Hour_U8 = (uint8_t)pNow_X->tm_hour;
    _rDateTime_X.Minute_U8 = (uint8_t)pNow_X->tm_min;
    _rDateTime_X.Second_U8 = (uint8_t)pNow_X->tm_sec;
    _rDateTime_X.Millisecond_U16 = (uint16_t)0;
    Rts_E = BOF_ERR_NO_ERROR;
  }
#endif

  return Rts_E;
}

BOFERR Bof_TimeInSecSince1970_To_BofDateTime(time_t _TimeInSecSice1970, BOF_DATE_TIME &_rDateTime_X)
{
	BOFERR Rts_E = BOF_ERR_EINVAL;
	struct tm *pTimeInfo_X;
	pTimeInfo_X = localtime(&_TimeInSecSice1970);
	_rDateTime_X.Reset();
	if (pTimeInfo_X)
	{
		_rDateTime_X.Year_U16 = static_cast< uint16_t > (pTimeInfo_X->tm_year + 1900); // Or gmtime ?
		_rDateTime_X.Month_U8 = static_cast< uint8_t > (pTimeInfo_X->tm_mon + 1);
		_rDateTime_X.DayOfWeek_U8 = static_cast< uint8_t > (pTimeInfo_X->tm_wday);
		_rDateTime_X.Day_U8 = static_cast< uint8_t > (pTimeInfo_X->tm_mday);
		_rDateTime_X.Hour_U8 = static_cast< uint8_t > (pTimeInfo_X->tm_hour);
		_rDateTime_X.Minute_U8 = static_cast< uint8_t > (pTimeInfo_X->tm_min);
		_rDateTime_X.Second_U8 = static_cast< uint8_t > (pTimeInfo_X->tm_sec);
		_rDateTime_X.Millisecond_U16 = 0;
		Rts_E = BOF_ERR_NO_ERROR;
	}
	return (Rts_E);
}

BOFERR Bof_DateInDaySince1970_To_BofDateTime(time_t _DateInDaySince1970, BOF_DATE_TIME &_rDateTime_X)
{
	BOFERR Rts_E = BOF_ERR_EINVAL;
	struct tm *pTimeInfo_X, t;
	time_t Time;

	memset(&t, 0, sizeof(t));
	t.tm_mday = 1;
	t.tm_mon = 0; /* Jan */
	t.tm_year = 70; /* since 1900 */
	t.tm_isdst = -1;

	t.tm_mday += static_cast<int>(_DateInDaySince1970);
	Time = mktime(&t);
	pTimeInfo_X = localtime(&Time);
	_rDateTime_X.Reset();
	if (pTimeInfo_X)
	{
		_rDateTime_X.Year_U16 = static_cast< uint16_t > (pTimeInfo_X->tm_year + 1900); // Or gmtime ?
		_rDateTime_X.Month_U8 = static_cast< uint8_t > (pTimeInfo_X->tm_mon + 1);
		_rDateTime_X.DayOfWeek_U8 = static_cast< uint8_t > (pTimeInfo_X->tm_wday);
		_rDateTime_X.Day_U8 = static_cast< uint8_t > (pTimeInfo_X->tm_mday);
		_rDateTime_X.Hour_U8 = static_cast< uint8_t > (pTimeInfo_X->tm_hour);
		_rDateTime_X.Minute_U8 = static_cast< uint8_t > (pTimeInfo_X->tm_min);
		_rDateTime_X.Second_U8 = static_cast< uint8_t > (pTimeInfo_X->tm_sec);
		_rDateTime_X.Millisecond_U16 = 0;
		Rts_E = BOF_ERR_NO_ERROR;
	}
	return (Rts_E);
}

BOFERR Bof_BofDateTime_To_DateInDaySince1970(const BOF_DATE_TIME &_rDateTime_X, time_t &_rDateInDaySince1970)
{
	BOF_DATE_TIME EpochDateTime_X, DiffTime_X;
	uint32_t DiffDay_U32;
	BOFERR Rts_E;

	EpochDateTime_X.Day_U8 = 1;
	EpochDateTime_X.Month_U8 = 1;
	EpochDateTime_X.Year_U16 = 1970;
	EpochDateTime_X.Hour_U8 = 0;
	EpochDateTime_X.Minute_U8 = 0;
	EpochDateTime_X.Second_U8 = 0;
	EpochDateTime_X.Millisecond_U16 = 0;
	Rts_E = Bof_DiffDateTime(_rDateTime_X, DiffTime_X, EpochDateTime_X, DiffDay_U32);
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		_rDateInDaySince1970 = DiffDay_U32;
	}
	return (Rts_E);
}

const std::string Bof_DateTimeToString(const BOF_DATE_TIME &_rDateTime_X, const std::string &_rFormat_S)
{
	struct tm Tm_X;
	char pWork_c[0x100];
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime for more information about date/time format (""%Y-%m-%d %H:%M:%S")
	Tm_X.tm_sec = _rDateTime_X.Second_U8;
	Tm_X.tm_min = _rDateTime_X.Minute_U8;
	Tm_X.tm_hour = _rDateTime_X.Hour_U8;
	Tm_X.tm_mday = _rDateTime_X.Day_U8;
	Tm_X.tm_mon = _rDateTime_X.Month_U8 - 1;
	Tm_X.tm_year = _rDateTime_X.Year_U16 - 1900;
	Tm_X.tm_wday = _rDateTime_X.DayOfWeek_U8;
	Tm_X.tm_yday = 0;
	Tm_X.tm_isdst = 0;

	strftime(pWork_c, sizeof(pWork_c), _rFormat_S.c_str(), &Tm_X);
	return std::string(pWork_c);
}

BOF_DATE_TIME Bof_DateTimeFromString(const std::string &_rDateTime_S, const std::string &_rFormat_S)
{
	BOF_DATE_TIME Rts_X;
	struct tm Tm_X;
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime for more information about date/time format (""%Y-%m-%d %H:%M:%S")

	memset(&Tm_X, 0, sizeof(Tm_X));
	if (strptime(_rDateTime_S.c_str(), _rFormat_S.c_str(), &Tm_X) != nullptr)
	{
		Rts_X.Second_U8 = static_cast<uint8_t>(Tm_X.tm_sec);
		Rts_X.Minute_U8 = static_cast<uint8_t>(Tm_X.tm_min);
		Rts_X.Hour_U8 = static_cast<uint8_t>(Tm_X.tm_hour);
		Rts_X.Day_U8 = static_cast<uint8_t>(Tm_X.tm_mday);
		Rts_X.Month_U8 = static_cast<uint8_t>(Tm_X.tm_mon + 1);
		Rts_X.Year_U16 = static_cast<uint16_t>(Tm_X.tm_year + 1900);
		Bof_ComputeDayOfWeek(Rts_X, Rts_X.DayOfWeek_U8); //  static_cast<uint8_t>(Tm_X.tm_wday);
	}
	return Rts_X;
}

BOFERR Bof_DeltaMsToHms(uint32_t _DeltaInMs_U32, uint32_t &_rDay_U32, uint32_t &_rHour_U32, uint32_t &_rMinute_U32, uint32_t &_rSecond_U32, uint32_t &_rMs_U32)
{
  BOFERR Rts_E=BOF_ERR_NO_ERROR;

  _rMs_U32=_DeltaInMs_U32 % 1000;
  _DeltaInMs_U32/=1000;
  _rSecond_U32=_DeltaInMs_U32 % 60;
  _DeltaInMs_U32/=60;
  _rMinute_U32=_DeltaInMs_U32 % 60;
  _DeltaInMs_U32/=60;
  _rHour_U32=_DeltaInMs_U32 % 24;
  _DeltaInMs_U32/=24;
  _rDay_U32=_DeltaInMs_U32 ;
  return Rts_E;
}
BOFERR Bof_Now(BOF_DATE_TIME &_rDateTime_X)
{
	BOFERR Rts_E;
	Rts_E = Bof_TimeInSecSince1970_To_BofDateTime(time(nullptr), _rDateTime_X);
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
    _rDateTime_X.Millisecond_U16 = 0; // static_cast<uint16_t>(Bof_GetMsTickCount() % 1000);->no as we can have 2 call in the same day with milli2<milli1
	}
	return Rts_E;
}

BOFERR Bof_ComputeDayOfWeek(const BOF_DATE_TIME &_rDateTime_X, uint8_t &_DayOfWeek_U8)  //0 is sunday
{
	//https://stackoverflow.com/questions/6054016/c-program-to-find-day-of-week-given-date
	BOFERR Rts_E = BOF_ERR_EINVAL;
	int d, m, y;

	if ((_rDateTime_X.Day_U8 >= 1) &&
	    (_rDateTime_X.Day_U8 <= 31) &&
	    (_rDateTime_X.Month_U8 >= 1) &&
	    (_rDateTime_X.Month_U8 <= 12) &&
	    (_rDateTime_X.Year_U16 >= 1970) &&
	    (_rDateTime_X.Year_U16 <= 3070)            // This should be enought for me (-;
		)
	{
		Rts_E = BOF_ERR_NO_ERROR;

		d = _rDateTime_X.Day_U8; //Day     1-31
		m = _rDateTime_X.Month_U8; //Month   1-12`
		y = _rDateTime_X.Year_U16; //Year    2013` 

		_DayOfWeek_U8 = static_cast<uint8_t>((d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) % 7);
	}
	return Rts_E;
}

BOFERR Bof_SetDateTime(const BOF_DATE_TIME &_rDateTime_X)
{
	BOFERR Rts_E;

	Rts_E = Bof_ValidateDateTime(_rDateTime_X);
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
#if defined (_WIN32)
		SYSTEMTIME           SystemTime_X;

		SystemTime_X.wYear         = _rDateTime_X.Year_U16;
		SystemTime_X.wMonth        = _rDateTime_X.Month_U8;
		SystemTime_X.wDayOfWeek    = _rDateTime_X.DayOfWeek_U8;
		SystemTime_X.wDay          = _rDateTime_X.Day_U8;
		SystemTime_X.wHour         = _rDateTime_X.Hour_U8;
		SystemTime_X.wMinute       = _rDateTime_X.Minute_U8;
		SystemTime_X.wSecond       = _rDateTime_X.Second_U8;
		SystemTime_X.wMilliseconds = _rDateTime_X.Millisecond_U16;

		if (! SetSystemTime(&SystemTime_X) )
		{
			Rts_E = BOF_ERR_INTERNAL;
		}
#else
		struct tm *pDate_X;
		time_t Time_X;

		time(&Time_X);
		pDate_X = localtime(&Time_X); // Or gmtime ?
		pDate_X->tm_year = _rDateTime_X.Year_U16 - 1900;
		pDate_X->tm_mon = _rDateTime_X.Month_U8 - 1;
		pDate_X->tm_wday = _rDateTime_X.DayOfWeek_U8;
		pDate_X->tm_mday = _rDateTime_X.Day_U8;
		pDate_X->tm_hour = _rDateTime_X.Hour_U8;
		pDate_X->tm_min = _rDateTime_X.Minute_U8;
		pDate_X->tm_sec = _rDateTime_X.Second_U8;

		const struct timeval Tv_X = {
			mktime(pDate_X), 0
		};
		if (settimeofday(&Tv_X, nullptr))
		{
			Rts_E = BOF_ERR_INTERNAL;
		}

#endif
	}
	return Rts_E;
}

/*
 * S_pDaysPerMonth_U8[0] = non leap year
 * S_pDaysPerMonth_U8[1] = leap year

static uint8_t S_pDaysPerMonth_U8[2][13] = {{0xFF, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
                                            {0xFF, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

static const char *S_pMonth_c[13] = {"None", "Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sept", "Oct", "Nov", "Dec"};
*/
BOFERR Bof_ValidateDateTime(const BOF_DATE_TIME &_rDateTime_X)
{
	BOFERR Rts_E = BOF_ERR_EINVAL;
	if ((_rDateTime_X.Day_U8 >= 1) &&
	    (_rDateTime_X.Day_U8 <= 31) &&
	    (_rDateTime_X.Month_U8 >= 1) &&
	    (_rDateTime_X.Month_U8 <= 12) &&
	    (_rDateTime_X.Year_U16 >= 1970) &&
	    (_rDateTime_X.Year_U16 <= 3070) &&            // This should be enought for me (-;
	    //	    (_rDateTime_X.DayOfWeek_U8 >= 0) &&
	    (_rDateTime_X.DayOfWeek_U8 <= 6) &&
	    //	    (_rDateTime_X.Hour_U8 >= 0) &&
	    (_rDateTime_X.Hour_U8 <= 23) &&
	    //	    (_rDateTime_X.Minute_U8 >= 0) &&
	    (_rDateTime_X.Minute_U8 <= 59) &&
	    //	    (_rDateTime_X.Second_U8 >= 0) &&
	    (_rDateTime_X.Second_U8 <= 60) &&    // seconds after the minute - [0, 60] including leap second
	    //	    (_rDateTime_X.Millisecond_U16 >= 0) &&
	    (_rDateTime_X.Millisecond_U16 <= 999)
		)
	{
		if (_rDateTime_X.Month_U8 == 2)
		{
			if (Bof_IsLeapYear(_rDateTime_X.Year_U16))
			{
				if (_rDateTime_X.Day_U8 <= 29)
				{
					Rts_E = BOF_ERR_NO_ERROR;
				}
			}
			else
			{
				if (_rDateTime_X.Day_U8 <= 28)
				{
					Rts_E = BOF_ERR_NO_ERROR;
				}
			}
		}
		else
		{
			Rts_E = BOF_ERR_NO_ERROR;
		}
	}
	return Rts_E;
}

/*
 * Depuis l'ajustement du calendrier gr?gorien, sont bissextiles les ann?es1 :
 * soit divisibles par 4 mais non divisibles par 100;
 * soit divisibles par 400.
 */
bool Bof_IsLeapYear(uint16_t _Year_U16)
{
	return ((_Year_U16 % 4 == 0 && _Year_U16 % 100 != 0) || (_Year_U16 % 400 == 0)) ? true : false;
}

// http://alcor.concordia.ca/~gpkatch/gdate-algorithm.html
BOFERR Bof_DateTimeToNumber(const BOF_DATE_TIME &_rDateTime_X, double &_rDayNumber_lf)
{
	BOFERR Rts_E = Bof_ValidateDateTime(_rDateTime_X);
	int Month_i, Year_i;
	double FracDay_lf;

	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		Month_i = (_rDateTime_X.Month_U8 + 9) % 12;
		Year_i = _rDateTime_X.Year_U16 - Month_i / 10;
		_rDayNumber_lf = static_cast<double>((365 * Year_i) + (Year_i / 4) - (Year_i / 100) + (Year_i / 400) + ((Month_i * 306 + 5) / 10) + (_rDateTime_X.Day_U8 - 1));
		FracDay_lf = ((static_cast<double>(_rDateTime_X.Hour_U8) * static_cast<double>(60.0f * 60.0f * 1000.0f)) + (static_cast<double>(_rDateTime_X.Minute_U8) * static_cast<double>(60.0f * 1000.0f)) +
		              (static_cast<double>(_rDateTime_X.Second_U8) * static_cast<double>(1000.0f)) + (static_cast<double>(_rDateTime_X.Millisecond_U16))) / static_cast<double>(24.0f * 60.0f * 60.0f * 1000.0f);
		_rDayNumber_lf = _rDayNumber_lf + FracDay_lf;
		Rts_E = BOF_ERR_NO_ERROR;
	}
	return Rts_E;
}


// millisecond are not used
BOFERR Bof_DiffDateTime(const BOF_DATE_TIME &_rFirstDateTime_X, const BOF_DATE_TIME &_rSecondDateTime_X, BOF_DATE_TIME &_rDiffTime_X, uint32_t &_rDiffDay_U32)
{
	BOFERR Rts_E = Bof_ValidateDateTime(_rFirstDateTime_X);
	double DayNumber1_lf, DayNumber2_lf, Val_lf, FracVal_lf, ValInt_lf;

	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		Rts_E = Bof_ValidateDateTime(_rSecondDateTime_X);
		if (Rts_E == BOF_ERR_NO_ERROR)
		{
			_rDiffDay_U32 = 0;
			_rDiffTime_X.Reset();
			Rts_E = Bof_DateTimeToNumber(_rFirstDateTime_X, DayNumber1_lf);

			if (Rts_E == BOF_ERR_NO_ERROR)
			{
				Rts_E = Bof_DateTimeToNumber(_rSecondDateTime_X, DayNumber2_lf);

				if (Rts_E == BOF_ERR_NO_ERROR)
				{
					Rts_E = BOF_ERR_OPERATION_FAILED;
					if (DayNumber1_lf >= DayNumber2_lf)
					{
						Rts_E = BOF_ERR_NO_ERROR;
						Val_lf = DayNumber1_lf - DayNumber2_lf;
						FracVal_lf = modf(Val_lf, &ValInt_lf);
						_rDiffDay_U32 = static_cast<uint32_t>(ValInt_lf);
						Val_lf = 24.0f * FracVal_lf;
						FracVal_lf = modf(Val_lf, &ValInt_lf);
						_rDiffTime_X.Hour_U8 = static_cast<uint8_t>(ValInt_lf);
						Val_lf = 60.0f * FracVal_lf;
						FracVal_lf = modf(Val_lf, &ValInt_lf);
						_rDiffTime_X.Minute_U8 = static_cast<uint8_t>(ValInt_lf);
						Val_lf = 60.0f * FracVal_lf;
						FracVal_lf = modf(Val_lf, &ValInt_lf);
						_rDiffTime_X.Second_U8 = static_cast<uint8_t>(ValInt_lf);
						Val_lf = 1000.0f * FracVal_lf;
						FracVal_lf = modf(Val_lf, &ValInt_lf);
						_rDiffTime_X.Millisecond_U16 = static_cast<uint16_t>(ValInt_lf);
						if (_rDiffTime_X.Millisecond_U16 > 995)
						{
							_rDiffTime_X.Millisecond_U16 = 0;
							_rDiffTime_X.Second_U8++;
							if (_rDiffTime_X.Second_U8 > 59)
							{
								_rDiffTime_X.Second_U8 = 0;
								_rDiffTime_X.Minute_U8++;
								if (_rDiffTime_X.Minute_U8 > 59)
								{
									_rDiffTime_X.Minute_U8 = 0;
									_rDiffTime_X.Hour_U8++;
									if (_rDiffTime_X.Hour_U8 > 23)
									{
										_rDiffTime_X.Hour_U8 = 0;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return Rts_E;
}


BOFERR Bof_GetLastError(bool _NetError_B, int32_t *_pNativeErrorCode_S32)
{
  BOFERR Rts_E;

#if defined (_WIN32)
  int Err_i;

	if (_NetError_B)
	{
    Err_i = WSAGetLastError();
    if (_pNativeErrorCode_S32)
    {
      *_pNativeErrorCode_S32 = Err_i;
    }
    switch (Err_i)
    {
      case 0:
        Rts_E = BOF_ERR_NO_ERROR;
        break;
      case WSAEINTR:
        Rts_E = BOF_ERR_EINTR;
        break;
      case WSAEBADF:
        Rts_E = BOF_ERR_EBADF;
        break;
      case WSAEACCES:
        Rts_E = BOF_ERR_EACCES;
        break;
      case WSAEFAULT:
        Rts_E = BOF_ERR_EFAULT;
        break;
      case WSAEINVAL:
        Rts_E = BOF_ERR_EINVAL;
        break;
      case WSAEMFILE:
        Rts_E = BOF_ERR_EMFILE;
        break;
      case WSAEWOULDBLOCK:
        Rts_E = BOF_ERR_EWOULDBLOCK;
        break; 
      case WSAEINPROGRESS:
        Rts_E = BOF_ERR_EINPROGRESS;
        break;
      case WSAEALREADY:
        Rts_E = BOF_ERR_EALREADY;
        break;
      case WSAENOTSOCK:
        Rts_E = BOF_ERR_ENOTSOCK;
        break;
      case WSAEDESTADDRREQ:
        Rts_E = BOF_ERR_EDESTADDRREQ;
        break;
      case WSAEMSGSIZE:
        Rts_E = BOF_ERR_EMSGSIZE;
        break;
      case WSAEPROTOTYPE:
        Rts_E = BOF_ERR_EPROTOTYPE;
        break;
      case WSAENOPROTOOPT:
        Rts_E = BOF_ERR_ENOPROTOOPT;
        break;
      case WSAEPROTONOSUPPORT:
        Rts_E = BOF_ERR_EPROTONOSUPPORT;
        break;
      case WSAESOCKTNOSUPPORT:
        Rts_E = BOF_ERR_ESOCKTNOSUPPORT;
        break;
      case WSAEOPNOTSUPP:
        Rts_E = BOF_ERR_EOPNOTSUPP;
        break;
      case WSAEPFNOSUPPORT:
        Rts_E = BOF_ERR_EPFNOSUPPORT;
        break;
      case WSAEAFNOSUPPORT:
        Rts_E = BOF_ERR_EAFNOSUPPORT;
        break;
      case WSAEADDRINUSE:
        Rts_E = BOF_ERR_EADDRINUSE;
        break;
      case WSAEADDRNOTAVAIL:
        Rts_E = BOF_ERR_EADDRNOTAVAIL;
        break;
      case WSAENETDOWN:
        Rts_E = BOF_ERR_ENETDOWN;
        break;
      case WSAENETUNREACH:
        Rts_E = BOF_ERR_ENETUNREACH;
        break;
      case WSAENETRESET:
        Rts_E = BOF_ERR_ENETRESET;
        break;
      case WSAECONNABORTED:
        Rts_E = BOF_ERR_ECONNABORTED;
        break;
      case WSAECONNRESET:
        Rts_E = BOF_ERR_ECONNRESET;
        break;
      case WSAENOBUFS:
        Rts_E = BOF_ERR_ENOBUFS;
        break;
      case WSAEISCONN:
        Rts_E = BOF_ERR_EISCONN;
        break;
      case WSAENOTCONN:
        Rts_E = BOF_ERR_ENOTCONN;
        break;
      case WSAESHUTDOWN:
        Rts_E = BOF_ERR_ESHUTDOWN;
        break;
      case WSAETOOMANYREFS:
        Rts_E = BOF_ERR_ETOOMANYREFS;
        break;
      case WSAETIMEDOUT:
        Rts_E = BOF_ERR_ETIMEDOUT;
        break;
      case WSAECONNREFUSED:
        Rts_E = BOF_ERR_ECONNREFUSED;
        break;
      case WSAELOOP:
        Rts_E = BOF_ERR_ELOOP;
        break;
      case WSAENAMETOOLONG:
        Rts_E = BOF_ERR_ENAMETOOLONG;
        break;
      case WSAEHOSTDOWN:
        Rts_E = BOF_ERR_EHOSTDOWN;
        break;
      case WSAEHOSTUNREACH:
        Rts_E = BOF_ERR_EHOSTUNREACH;
        break;
      case WSAENOTEMPTY:
        Rts_E = BOF_ERR_ENOTEMPTY;
        break;
      case WSAEUSERS:
        Rts_E = BOF_ERR_EUSERS;
        break;
      case WSAEDQUOT:
        Rts_E = BOF_ERR_EDQUOT;
        break;
      case WSAESTALE:
        Rts_E = BOF_ERR_ESTALE;
        break;
      case WSAEREMOTE:
        Rts_E = BOF_ERR_EREMOTE;
        break;
/*
      case WSAEPROCLIM:
        Rts_E = BOF_ERR_EPROCLIM;
        break;
      case WSASYSNOTREADY:
        Rts_E = BOF_ERR_SYSNOTREADY;
        break;
      case WSAVERNOTSUPPORTED:
        Rts_E = BOF_ERR_ERNOTSUPPORTED;
        break;
      case WSANOTINITIALISED:
        Rts_E = BOF_ERR_NOTINITIALISED;
        break;
      case WSAEDISCON:
        Rts_E = BOF_ERR_EDISCON;
        break;
      case WSAENOMORE:
        Rts_E = BOF_ERR_ENOMORE;
        break;
      case WSAECANCELLED:
        Rts_E = BOF_ERR_ECANCELLED;
        break;
      case WSAEINVALIDPROCTABLE:
        Rts_E = BOF_ERR_EINVALIDPROCTABLE;
        break;
      case WSAEINVALIDPROVIDER:
        Rts_E = BOF_ERR_EINVALIDPROVIDER;
        break;
      case WSAEPROVIDERFAILEDINIT:
        Rts_E = BOF_ERR_EPROVIDERFAILEDINIT;
        break;
      case WSASYSCALLFAILURE:
        Rts_E = BOF_ERR_SYSCALLFAILURE;
        break;
      case WSASERVICE_NOT_FOUND:
        Rts_E = BOF_ERR_SERVICE_NOT_FOUND;
        break;
      case WSATYPE_NOT_FOUND:
        Rts_E = BOF_ERR_TYPE_NOT_FOUND;
        break;
      case WSA_E_NO_MORE:
        Rts_E = BOF_ERR_E_NO_MORE;
        break;
      case WSA_E_CANCELLED:
        Rts_E = BOF_ERR_E_CANCELLED;
        break;
      case WSAEREFUSED:
        Rts_E = BOF_ERR_EREFUSED;
        break;
*/
    default:
      Rts_E = BOF_ERR_ENOTTY;    //Return an "never" used standard errno "Not a typewriter"
      break;
    }
	}
  else
  {
    Err_i = GetLastError();
    if (_pNativeErrorCode_S32)
    {
      *_pNativeErrorCode_S32 = Err_i;
    }
    //https://github.com/scottt/cygwin-vmci-sockets/blob/master/winsup/cygwin/errno.cc
    switch (Err_i)
    {
    case 0:
      Rts_E = BOF_ERR_NO_ERROR;
      break;
    case ERROR_ACCESS_DENIED:
      Rts_E = BOF_ERR_EACCES;
      break;
    case ERROR_ACTIVE_CONNECTIONS:
      Rts_E = BOF_ERR_EAGAIN;
      break;
    case ERROR_ALREADY_EXISTS:
      Rts_E = BOF_ERR_EEXIST;
      break;
    case ERROR_BAD_DEVICE:
      Rts_E = BOF_ERR_ENODEV;
      break;
    case ERROR_BAD_EXE_FORMAT:
      Rts_E = BOF_ERR_ENOEXEC;
      break;
    case ERROR_BAD_NETPATH:
      Rts_E = BOF_ERR_ENOENT;
      break;
    case ERROR_BAD_NET_NAME:
      Rts_E = BOF_ERR_ENOENT;
      break;
    case ERROR_BAD_NET_RESP:
      Rts_E = BOF_ERR_ENOSYS;
      break;
    case ERROR_BAD_PATHNAME:
      Rts_E = BOF_ERR_ENOENT;
      break;
    case ERROR_BAD_PIPE:
      Rts_E = BOF_ERR_EINVAL;
      break;
    case ERROR_BAD_UNIT:
      Rts_E = BOF_ERR_ENODEV;
      break;
    case ERROR_BAD_USERNAME:
      Rts_E = BOF_ERR_EINVAL;
      break;
    case ERROR_BEGINNING_OF_MEDIA:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_BROKEN_PIPE:
      Rts_E = BOF_ERR_EPIPE;
      break;
    case ERROR_BUSY:
      Rts_E = BOF_ERR_EBUSY;
      break;
    case ERROR_BUS_RESET:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_CALL_NOT_IMPLEMENTED:
      Rts_E = BOF_ERR_ENOSYS;
      break;
    case ERROR_CANCELLED:
      Rts_E = BOF_ERR_EINTR;
      break;
    case ERROR_CANNOT_MAKE:
      Rts_E = BOF_ERR_EPERM;
      break;
    case ERROR_CHILD_NOT_COMPLETE:
      Rts_E = BOF_ERR_EBUSY;
      break;
    case ERROR_COMMITMENT_LIMIT:
      Rts_E = BOF_ERR_EAGAIN;
      break;
    case ERROR_CONNECTION_REFUSED:
      Rts_E = BOF_ERR_ECONNREFUSED;
      break;
    case ERROR_CRC:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_DEVICE_DOOR_OPEN:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_DEVICE_IN_USE:
      Rts_E = BOF_ERR_EAGAIN;
      break;
    case ERROR_DEVICE_REQUIRES_CLEANING:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_DEV_NOT_EXIST:
      Rts_E = BOF_ERR_ENOENT;
      break;
    case ERROR_DIRECTORY:
      Rts_E = BOF_ERR_ENOTDIR;
      break;
    case ERROR_DIR_NOT_EMPTY:
      Rts_E = BOF_ERR_ENOTEMPTY;
      break;
    case ERROR_DISK_CORRUPT:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_DISK_FULL:
      Rts_E = BOF_ERR_ENOSPC;
      break;
    case ERROR_DS_GENERIC_ERROR:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_DUP_NAME:
      Rts_E = BOF_ERR_ENOTUNIQ;
      break;
    case ERROR_EAS_DIDNT_FIT:
      Rts_E = BOF_ERR_ENOSPC;
      break;
    case ERROR_EAS_NOT_SUPPORTED:
      Rts_E = BOF_ERR_EOPNOTSUPP;
      break;
    case ERROR_EA_LIST_INCONSISTENT:
      Rts_E = BOF_ERR_EINVAL;
      break;
    case ERROR_EA_TABLE_FULL:
      Rts_E = BOF_ERR_ENOSPC;
      break;
    case ERROR_END_OF_MEDIA:
      Rts_E = BOF_ERR_ENOSPC;
      break;
    case ERROR_EOM_OVERFLOW:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_EXE_MACHINE_TYPE_MISMATCH:
      Rts_E = BOF_ERR_ENOEXEC;
      break;
    case ERROR_EXE_MARKED_INVALID:
      Rts_E = BOF_ERR_ENOEXEC;
      break;
    case ERROR_FILEMARK_DETECTED:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_FILENAME_EXCED_RANGE:
      Rts_E = BOF_ERR_ENAMETOOLONG;
      break;
    case ERROR_FILE_CORRUPT:
      Rts_E = BOF_ERR_EEXIST;
      break;
    case ERROR_FILE_EXISTS:
      Rts_E = BOF_ERR_EEXIST;
      break;
    case ERROR_FILE_INVALID:
      Rts_E = BOF_ERR_ENXIO;
      break;
    case ERROR_FILE_NOT_FOUND:
      Rts_E = BOF_ERR_ENOENT;
      break;
    case ERROR_HANDLE_DISK_FULL:
      Rts_E = BOF_ERR_ENOSPC;
      break;
    case ERROR_HANDLE_EOF:
      Rts_E = BOF_ERR_ENODATA;
      break;
    case ERROR_INVALID_ADDRESS:
      Rts_E = BOF_ERR_EINVAL;
      break;
    case ERROR_INVALID_AT_INTERRUPT_TIME:
      Rts_E = BOF_ERR_EINTR;
      break;
    case ERROR_INVALID_BLOCK_LENGTH:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_INVALID_DATA:
      Rts_E = BOF_ERR_EINVAL;
      break;
    case ERROR_INVALID_DRIVE:
      Rts_E = BOF_ERR_ENODEV;
      break;
    case ERROR_INVALID_EA_NAME:
      Rts_E = BOF_ERR_EINVAL;
      break;
    case ERROR_INVALID_EXE_SIGNATURE:
      Rts_E = BOF_ERR_ENOEXEC;
      break;
    case ERROR_INVALID_FUNCTION:
      Rts_E = BOF_ERR_EBADRQC;
      break;
    case ERROR_INVALID_HANDLE:
      Rts_E = BOF_ERR_EBADF;
      break;
    case ERROR_INVALID_NAME:
      Rts_E = BOF_ERR_ENOENT;
      break;
    case ERROR_INVALID_PARAMETER:
      Rts_E = BOF_ERR_EINVAL;
      break;
    case ERROR_INVALID_SIGNAL_NUMBER:
      Rts_E = BOF_ERR_EINVAL;
      break;
    case ERROR_IOPL_NOT_ENABLED:
      Rts_E = BOF_ERR_ENOEXEC;
      break;
    case ERROR_IO_DEVICE:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_IO_INCOMPLETE:
      Rts_E = BOF_ERR_EAGAIN;
      break;
    case ERROR_IO_PENDING:
      Rts_E = BOF_ERR_EAGAIN;
      break;
    case ERROR_LOCK_VIOLATION:
      Rts_E = BOF_ERR_EBUSY;
      break;
    case ERROR_MAX_THRDS_REACHED:
      Rts_E = BOF_ERR_EAGAIN;
      break;
    case ERROR_META_EXPANSION_TOO_LONG:
      Rts_E = BOF_ERR_EINVAL;
      break;
    case ERROR_MOD_NOT_FOUND:
      Rts_E = BOF_ERR_ENOENT;
      break;
    case ERROR_MORE_DATA:
      Rts_E = BOF_ERR_EMSGSIZE;
      break;
    case ERROR_NEGATIVE_SEEK:
      Rts_E = BOF_ERR_ESPIPE;  // BOF_ERR_EINVAL;
      break;
    case ERROR_NETNAME_DELETED:
      Rts_E = BOF_ERR_ENOENT;
      break;
    case ERROR_NOACCESS:
      Rts_E = BOF_ERR_EFAULT;
      break;
    case ERROR_NONE_MAPPED:
      Rts_E = BOF_ERR_EINVAL;
      break;
    case ERROR_NONPAGED_SYSTEM_RESOURCES:
      Rts_E = BOF_ERR_EAGAIN;
      break;
    case ERROR_NOT_CONNECTED:
      Rts_E = BOF_ERR_ENOLINK;
      break;
    case ERROR_NOT_ENOUGH_MEMORY:
      Rts_E = BOF_ERR_ENOMEM;
      break;
    case ERROR_NOT_ENOUGH_QUOTA:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_NOT_OWNER:
      Rts_E = BOF_ERR_EPERM;
      break;
    case ERROR_NOT_READY:
      Rts_E = BOF_ERR_ENOMEDIUM;
      break;
    case ERROR_NOT_SAME_DEVICE:
      Rts_E = BOF_ERR_EXDEV;
      break;
    case ERROR_NOT_SUPPORTED:
      Rts_E = BOF_ERR_ENOSYS;
      break;
    case ERROR_NO_DATA:
      Rts_E = BOF_ERR_EPIPE;
      break;
    case ERROR_NO_DATA_DETECTED:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_NO_MEDIA_IN_DRIVE:
      Rts_E = BOF_ERR_ENOMEDIUM;
      break;
    case ERROR_NO_MORE_FILES:
      Rts_E = BOF_ERR_EMFILE;
      break;
    case ERROR_NO_MORE_ITEMS:
      Rts_E = BOF_ERR_EMFILE;
      break;
    case ERROR_NO_MORE_SEARCH_HANDLES:
      Rts_E = BOF_ERR_ENFILE;
      break;
    case ERROR_NO_PROC_SLOTS:
      Rts_E = BOF_ERR_EAGAIN;
      break;
    case ERROR_NO_SIGNAL_SENT:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_NO_SYSTEM_RESOURCES:
      Rts_E = BOF_ERR_EFBIG;
      break;
    case ERROR_NO_TOKEN:
      Rts_E = BOF_ERR_EINVAL;
      break;
    case ERROR_OPEN_FAILED:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_OPEN_FILES:
      Rts_E = BOF_ERR_EAGAIN;
      break;
    case ERROR_OUTOFMEMORY:
      Rts_E = BOF_ERR_ENOMEM;
      break;
    case ERROR_PAGED_SYSTEM_RESOURCES:
      Rts_E = BOF_ERR_EAGAIN;
      break;
    case ERROR_PAGEFILE_QUOTA:
      Rts_E = BOF_ERR_EAGAIN;
      break;
    case ERROR_PATH_NOT_FOUND:
      Rts_E = BOF_ERR_ENOENT;
      break;
    case ERROR_PIPE_BUSY:
      Rts_E = BOF_ERR_EBUSY;
      break;
    case ERROR_PIPE_CONNECTED:
      Rts_E = BOF_ERR_EBUSY;
      break;
    case ERROR_PIPE_LISTENING:
      Rts_E = BOF_ERR_ECOMM;
      break;
    case ERROR_PIPE_NOT_CONNECTED:
      Rts_E = BOF_ERR_ECOMM;
      break;
    case ERROR_POSSIBLE_DEADLOCK:
      Rts_E = BOF_ERR_EDEADLK;
      break;
    case ERROR_PRIVILEGE_NOT_HELD:
      Rts_E = BOF_ERR_EPERM;
      break;
    case ERROR_PROCESS_ABORTED:
      Rts_E = BOF_ERR_EFAULT;
      break;
    case ERROR_PROC_NOT_FOUND:
      Rts_E = BOF_ERR_ESRCH;
      break;
    case ERROR_REM_NOT_LIST:
      Rts_E = BOF_ERR_ENONET;
      break;
    case ERROR_SECTOR_NOT_FOUND:
      Rts_E = BOF_ERR_EINVAL;
      break;
    case ERROR_SEEK:
      Rts_E = BOF_ERR_ESPIPE; // BOF_ERR_EINVAL;
      break;
    case ERROR_SERVICE_REQUEST_TIMEOUT:
      Rts_E = BOF_ERR_ETIMEDOUT;
      break;
    case ERROR_SETMARK_DETECTED:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_SHARING_BUFFER_EXCEEDED:
      Rts_E = BOF_ERR_ENOLCK;
      break;
    case ERROR_SHARING_VIOLATION:
      Rts_E = BOF_ERR_EBUSY;
      break;
    case ERROR_SIGNAL_PENDING:
      Rts_E = BOF_ERR_EBUSY;
      break;
    case ERROR_SIGNAL_REFUSED:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_SXS_CANT_GEN_ACTCTX:
      Rts_E = BOF_ERR_ELIBBAD;
      break;
    case ERROR_THREAD_1_INACTIVE:
      Rts_E = BOF_ERR_EINVAL;
      break;
    case ERROR_TIMEOUT:
      Rts_E = BOF_ERR_ETIMEDOUT;
      break;
    case ERROR_TOO_MANY_LINKS:
      Rts_E = BOF_ERR_EMLINK;
      break;
    case ERROR_TOO_MANY_OPEN_FILES:
      Rts_E = BOF_ERR_EMFILE;
      break;
    case ERROR_UNEXP_NET_ERR:
      Rts_E = BOF_ERR_EIO;
      break;
    case ERROR_WAIT_NO_CHILDREN:
      Rts_E = BOF_ERR_ECHILD;
      break;
    case ERROR_WORKING_SET_QUOTA:
      Rts_E = BOF_ERR_EAGAIN;
      break;
    case ERROR_WRITE_PROTECT:
      Rts_E = BOF_ERR_EROFS;
      break;

    default:
      Rts_E = BOF_ERR_ENOTTY;    //Return an "never" used standard errno "Not a typewriter"
      break;
    }
  }
#else
	(void) _NetError_B;
  if (_pNativeErrorCode_S32)
  {
    *_pNativeErrorCode_S32 = errno;
  }
	Rts_E = static_cast<BOFERR>(errno);
#endif
	return Rts_E;
}


bool Bof_IsPidRunning(uint32_t _Pid_U32)
{
	bool Rts_B = false;

#if defined (_WIN32)
	/*
	 * GetModuleFileNameEx(process, 0, buffer, 256);
	 * EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	 * GetModuleBaseName(hProcess, 0, buffer, 50);
	 */
	void     *Process_h;
	uint32_t ExitCode_U32;

	Process_h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, _Pid_U32);

	if (Process_h)
	{
		GetExitCodeProcess(Process_h, (DWORD *)&ExitCode_U32);

		if (ExitCode_U32 == STILL_ACTIVE)
		{
			Rts_B = true;
		}
		CloseHandle(Process_h);
	}
#else
	/*
	 * In Linux, use the kill subroutine to send the signal specified by the Signal parameter to the process specified by the Process
	 * parameter (processId). The Signal parameter is a null value, the error checking is performed, but no signal is sent.
	 */
	if ((kill(_Pid_U32, 0) == -1) &&
	    (errno == ESRCH)                // No process can be found corresponding to processId
		)
	{}
	else
	{
		Rts_B = true;
	}
#endif
	return Rts_B;
}


uint32_t Bof_GetCurrentPid()
{
	uint32_t Rts_U32;

#if defined (_WIN32)
	Rts_U32 = GetCurrentProcessId();
#else
	Rts_U32 = getpid();
#endif
	return Rts_U32;
}


bool Bof_PatternCompare(const char *_pString_c, const char *_pPattern_c)
{
	if ((_pString_c) && (_pPattern_c))
	{
//		return *_pPattern_c - '*' ? *_pString_c ? (*_pPattern_c == '?') || ((*_pString_c) == (*_pPattern_c)) && Bof_PatternCompare(_pString_c + 1, _pPattern_c + 1) : !*_pPattern_c :
//		       Bof_PatternCompare(_pString_c, _pPattern_c + 1) || *_pString_c && Bof_PatternCompare(_pString_c + 1, _pPattern_c);

		return *_pPattern_c - '*' ? *_pString_c ? (*_pPattern_c == '?') || (((*_pString_c) == (*_pPattern_c)) && Bof_PatternCompare(_pString_c + 1, _pPattern_c + 1)) : !*_pPattern_c :
		       Bof_PatternCompare(_pString_c, _pPattern_c + 1) || (*_pString_c && Bof_PatternCompare(_pString_c + 1, _pPattern_c));
	}
	else
	{
		return false;
	}
}

BOFERR Bof_Exec(const std::string &_rCommand_S, std::string *_pCapturedOutput_S, int32_t &_rExitCode_S32)
{
	BOFERR Rts_E = BOF_ERR_CANNOT_START;
	FILE *pPipeOut_X;
	char pData_c[4096];
	std::string Command_S, CapturedOutput_S;

	_rExitCode_S32 = -1;
	Command_S = Bof_Sprintf("%s 2>&1", _rCommand_S.c_str());

#if defined (_WIN32)
	pPipeOut_X = _popen(Command_S.c_str(), "r");
#else
	pPipeOut_X = popen(Command_S.c_str(), "r");
#endif
	if (pPipeOut_X)
	{
		while (fgets(pData_c, sizeof(pData_c), pPipeOut_X) != nullptr)
		{
			if (_pCapturedOutput_S)
			{
				CapturedOutput_S += pData_c;
			}
		}
		Rts_E = BOF_ERR_NO_ERROR;
		if (_pCapturedOutput_S)
		{
			*_pCapturedOutput_S = CapturedOutput_S;
		}
#if defined (_WIN32)
		_rExitCode_S32 = _pclose(pPipeOut_X);
#else
		_rExitCode_S32 = pclose(pPipeOut_X);
#endif
		_rExitCode_S32 = _rExitCode_S32 / 256; // Exit code is in upper bits
	}

	return (Rts_E);
}


const char *Bof_GetEnvVar(const char *_pName_c)
{
	const char *pRts_c = nullptr;

	if (_pName_c)
	{
		pRts_c = getenv(_pName_c);
	}
	return (pRts_c);
}

int Bof_SetEnvVar(const char *_pName_c, const char *_pValue_c, int _Overwrite_i)
{
	int Rts_i = -1;

	if ((_pName_c) && (_pValue_c))
	{
		if (_Overwrite_i)
		{
			Rts_i = 0;
		}
		else
		{
			Rts_i = (Bof_GetEnvVar(_pName_c) != nullptr) ? -2 : 0;
		}
		if (Rts_i == 0)
		{
#if defined (_WIN32)
			Rts_i = _putenv_s(_pName_c, _pValue_c);
#else
			char pEnv_c[1024];
			snprintf(pEnv_c, sizeof(pEnv_c), "%s=%s", _pName_c, _pValue_c);
			Rts_i = putenv(pEnv_c);
#endif
		}
	}
	return (Rts_i);
}

BOFERR Bof_LockMem(uint64_t _SizeInByte_U64, void *_pData)
{
  BOFERR Rts_E;

#if defined(_WIN32)
//	Rts_E = VirtualLock(_pData, _SizeInByte_U64) ? BOF_ERR_NO_ERROR : BOF_ERR_LOCK;
	Rts_E = BOF_ERR_LOCK;
#else
	Rts_E = (mlock(_pData, _SizeInByte_U64) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_LOCK;
#endif
  return Rts_E;
}
BOFERR Bof_UnlockMem(uint64_t _SizeInByte_U64, void *_pData)
{
  BOFERR Rts_E;

#if defined(_WIN32)
//	Rts_E = VirtualUnlock(_pData, _SizeInByte_U64) ? BOF_ERR_NO_ERROR : BOF_ERR_LOCK;
	Rts_E = BOF_ERR_LOCK;
#else
	Rts_E = (munlock(_pData, _SizeInByte_U64)==0) ? BOF_ERR_NO_ERROR : BOF_ERR_LOCK;
#endif
  return Rts_E;
}
bool Bof_AlignedMemCpy8(volatile void *_pDst, const volatile void *_pSrc, uint32_t _SizeInByte_U32)
{
	bool Rts_B = true;
	uint32_t i_U32;
	volatile uint8_t *pDst_U8 = reinterpret_cast<volatile uint8_t *>(_pDst);
	const volatile uint8_t *pSrc_U8 = reinterpret_cast<const volatile uint8_t *>(_pSrc);

	for (i_U32 = 0; i_U32<_SizeInByte_U32; i_U32++)
	{
		pDst_U8[i_U32] = pSrc_U8[i_U32];
	}
	return Rts_B;
}

bool Bof_AlignedMemCpy16(volatile void *_pDst, const volatile void *_pSrc, uint32_t _SizeInByte_U32)
{
	bool Rts_B = false;
	uint32_t i_U32, SizeInWord_U32;
	volatile uint16_t *pDst_U16 = reinterpret_cast<volatile uint16_t *>(_pDst);
	const volatile uint16_t *pSrc_U16 = reinterpret_cast<const volatile uint16_t *>(_pSrc);


	if (((reinterpret_cast<uint64_t>(_pDst) & 0x01) == 0) && ((reinterpret_cast<uint64_t>(_pSrc) & 0x01) == 0))
	{
		SizeInWord_U32 = _SizeInByte_U32 >> 1;
		for (i_U32 = 0; i_U32 < SizeInWord_U32; i_U32++)
		{
			pDst_U16[i_U32] = pSrc_U16[i_U32];
		}
		Rts_B = true;
	}
	return Rts_B;
}

bool Bof_AlignedMemCpy32(volatile void *_pDst, const volatile void *_pSrc, uint32_t _SizeInByte_U32)
{
	bool Rts_B = false;
	uint32_t i_U32, SizeInDword_U32;
	volatile uint32_t *pDst_U32 = reinterpret_cast<volatile uint32_t *>(_pDst);
	const volatile uint32_t *pSrc_U32 = reinterpret_cast<const volatile uint32_t *>(_pSrc);


	if (((reinterpret_cast<uint64_t>(_pDst) & 0x03) == 0) && ((reinterpret_cast<uint64_t>(_pSrc) & 0x03) == 0))
	{
		//		SizeInDword_U32 = (_SizeInByte_U32 & 0x00000003) ? ((_SizeInByte_U32 | 0x00000003) + 1) >> 2 : _SizeInByte_U32 >> 2;
		SizeInDword_U32 = _SizeInByte_U32 >> 2;
		for (i_U32 = 0; i_U32 < SizeInDword_U32; i_U32++)
		{
			pDst_U32[i_U32] = pSrc_U32[i_U32];
		}
		Rts_B = true;
	}
	return Rts_B;
}

//https://stackoverflow.com/questions/32652833/how-to-allocate-huge-pages-for-c-application-on-linux
//constexpr char MMGW_HUGE_PAGE_PATH[]="/sys/kernel/mm/hugepages/hugepages-2048kB/page_%08X";  //"/var/lib/hugetlbfs/global/pagesize-2MB/page_%08X";
  constexpr char BOF_HUGE_PAGE_PATH[] = "/tmp/hugepages/page_%08X";
  constexpr uint32_t BOF_HUGE_PAGE_SIZE = 2 * 1024 * 1024;
BOFERR Bof_AlignedMemAlloc(BOF_BUFFER_ALLOCATE_ZONE _AllocateZone_E,uint32_t _AligmentInByte_U32,  uint32_t _SizeInByte_U32, bool _LockIt_B, bool _ClearIt_B, BOF_BUFFER &_rAllocatedBuffer_X)  //, uint32_t _Offset_U32)
{
	BOFERR Rts_E=BOF_ERR_EINVAL;
	BOF_BUFFER_ALLOCATE_HEADER AllocateBuffer_X;
  static std::atomic<uint32_t> S_HugePageId(0);

	_rAllocatedBuffer_X.Reset();
	if (Bof_IsAPowerOf2(_AligmentInByte_U32))
	{
		Rts_E = BOF_ERR_ENOMEM;

		_rAllocatedBuffer_X.MustBeDeleted_B = true;
		_rAllocatedBuffer_X.Capacity_U64 = _SizeInByte_U32;
		_rAllocatedBuffer_X.Size_U64 = 0;
    _rAllocatedBuffer_X.pData_U8=nullptr;
    AllocateBuffer_X.AllocateZone_E = _AllocateZone_E;
		switch (_AllocateZone_E)
		{
		default:
		case BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM:
#if defined(_WIN32)
			_rAllocatedBuffer_X.pData_U8 = reinterpret_cast<uint8_t *>(_aligned_malloc(_SizeInByte_U32, _AligmentInByte_U32));//malloc(size);   // TODO pChannel->getBoard()->getNUMANode() !!!
			//_rAllocatedBuffer_X.pData_U8 = reinterpret_cast<uint8_t*>(VirtualAlloc(nullptr, _SizeInByte_U32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
#else
			_rAllocatedBuffer_X.pData_U8 = reinterpret_cast<uint8_t *>(aligned_alloc(_AligmentInByte_U32, _SizeInByte_U32));//malloc(size);   // TODO pChannel->getBoard()->getNUMANode() !!!
#endif
			break;

        /*
         * On a NUMA platform, the kernel will attempt to distribute the huge page pool
  over all the set of allowed nodes specified by the NUMA memory policy of the
  task that modifies nr_hugepages. https://www.kernel.org/doc/Documentation/vm/hugetlbpage.txt
         */
      case BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_HUGE_PAGE:
#if defined(_WIN32)
#else
		  void* pBuffer;

        S_HugePageId++;
        BOF_SNPRINTF_NULL_CLIPPED(AllocateBuffer_X.pHugePath_c, sizeof(AllocateBuffer_X.pHugePath_c), BOF_HUGE_PAGE_PATH, S_HugePageId.load());
        AllocateBuffer_X.Io_i = open(AllocateBuffer_X.pHugePath_c, O_CREAT | O_RDWR, 0755);
        if (AllocateBuffer_X.Io_i >= 0)
        {
//          BOFERR Bof_OpenSharedMemory(const std::string &_rName_S, uint32_t _SizeInByte_U32, BOF_SHARED_MEMORY &_rSharedMemory_X)
          pBuffer = ::mmap(0, _SizeInByte_U32 < BOF_HUGE_PAGE_SIZE ? BOF_HUGE_PAGE_SIZE : _SizeInByte_U32, PROT_READ | PROT_WRITE, MAP_SHARED, AllocateBuffer_X.Io_i, 0);
          if (pBuffer == MAP_FAILED)
          {
            ::close(AllocateBuffer_X.Io_i);
            unlink(AllocateBuffer_X.pHugePath_c);
          }
          else
          {
            _rAllocatedBuffer_X.pData_U8 = reinterpret_cast<uint8_t *>(pBuffer);
          }
        }
#endif
        break;
		}
		if (_rAllocatedBuffer_X.pData_U8)
		{
			_rAllocatedBuffer_X.pUser = new BOF_BUFFER_ALLOCATE_HEADER;
			if (_rAllocatedBuffer_X.pUser) 
			{
        Rts_E = BOF_ERR_NO_ERROR;
				*reinterpret_cast<BOF_BUFFER_ALLOCATE_HEADER *>(_rAllocatedBuffer_X.pUser) = AllocateBuffer_X;
        if (_LockIt_B)
        {
          Rts_E=Bof_LockMem(_SizeInByte_U32,_rAllocatedBuffer_X.pData_U8);
          if (Rts_E!=BOF_ERR_NO_ERROR)
          {
            Bof_AlignedMemFree(_rAllocatedBuffer_X);
          }
        }
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          if (_ClearIt_B)
          {
            memset(_rAllocatedBuffer_X.pData_U8, 0, _SizeInByte_U32);
          }
        }
			}
			else
			{
				_rAllocatedBuffer_X.MustBeDeleted_B = false;
				_rAllocatedBuffer_X.pUser = &AllocateBuffer_X;
				Bof_AlignedMemFree(_rAllocatedBuffer_X);
			}
		}
	}
	//printf("=======> alloc Zone %d Must %d handle %x:%p data %x:%p\n",AllocateBuffer_X.AllocateZone_E, _rAllocatedBuffer_X.MustBeDeleted_B, sizeof(BOF_BUFFER_ALLOCATE_HEADER), _rAllocatedBuffer_X.pUser, _SizeInByte_U32, _rAllocatedBuffer_X.pData_U8);
	return Rts_E;
}

BOFERR Bof_AlignedMemFree(BOF_BUFFER &_rBuffer_X)
{
	BOF_BUFFER_ALLOCATE_HEADER *pAllocateBuffer_X;
	BOFERR Rts_E = BOF_ERR_EINVAL;

	pAllocateBuffer_X = reinterpret_cast<BOF_BUFFER_ALLOCATE_HEADER *>(_rBuffer_X.pUser);
	if (pAllocateBuffer_X)
	{
		Rts_E = BOF_ERR_NO_ERROR;
    if (pAllocateBuffer_X->Locked_B)
    {
      Rts_E=Bof_UnlockMem(_rBuffer_X.Capacity_U64,_rBuffer_X.pData_U8);
    }
		switch (pAllocateBuffer_X->AllocateZone_E)
		{
		default:
		case BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM:
#if defined(_WIN32)
			_aligned_free(_rBuffer_X.pData_U8);
//			VirtualFree(_rBuffer_X.pData_U8, _rBuffer_X.Capacity_U64, MEM_RELEASE);
#else
			free(_rBuffer_X.pData_U8);
#endif
			break;

      case BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_HUGE_PAGE:
#if defined(_WIN32)
#else
        ::munmap(_rBuffer_X.pData_U8, _rBuffer_X.Size_U64);
        ::close(pAllocateBuffer_X->Io_i);
        unlink(pAllocateBuffer_X->pHugePath_c);
#endif
        break;
		}
		//	printf("=======> DELETE Zone %d Must %d handle %x:%p data %lx:%p\n",pAllocateBuffer_X->AllocateZone_E, _rBuffer_X.MustBeDeleted_B, sizeof(BOF_BUFFER_ALLOCATE_HEADER), _rBuffer_X.pUser, _rBuffer_X.SizeInByte_U64, _rBuffer_X.pData_U8);

		if (_rBuffer_X.MustBeDeleted_B)
		{
			BOF_SAFE_DELETE(pAllocateBuffer_X);
		}

		_rBuffer_X.Reset();
	}
	return Rts_E;
}



std::string Bof_DumpMemoryZone(const BOF_DUMP_MEMORY_ZONE_PARAM &_rDumpMemoryZoneParam_X)
{
	const uint32_t MAX_NBBYTEPERLINE = 1024;
	uint32_t i_U32, j_U32,IndexInMemoryZone_U32, NbBytePerRead_U32, MemoryZoneSizeInByte_U32, NbItemBytePerLine_U32;
	int32_t Remain_S32;
	uint8_t Data_U8, pData_U8[8];
	const volatile uint8_t *pMemoryZone_U8;
	char *pBinaryData_c, pBinary_c[2 + 8 + 1 + (MAX_NBBYTEPERLINE * 5) + MAX_NBBYTEPERLINE + 2 + 1], pAscii_c[MAX_NBBYTEPERLINE + 1]; // 2: 0x 1: Separator 5: 0x%2Separator
	std::string Rts_S = "";
	uint64_t VirtualOffset_S64;
	volatile const uint64_t *pDataVal_U64;
	volatile uint64_t *pDataPtr_U64;
	volatile const uint32_t *pDataVal_U32;
	volatile uint32_t *pDataPtr_U32;
	volatile const uint16_t *pDataVal_U16;
	volatile uint16_t *pDataPtr_U16;

	if ((_rDumpMemoryZoneParam_X.pMemoryZone) && (_rDumpMemoryZoneParam_X.Separator_c != 0))
	{
		pMemoryZone_U8 = reinterpret_cast<const volatile uint8_t *>(_rDumpMemoryZoneParam_X.pMemoryZone);
		if ((_rDumpMemoryZoneParam_X.GenerateVirtualOffset) || (_rDumpMemoryZoneParam_X.GenerateBinaryData_B) || (_rDumpMemoryZoneParam_X.GenerateAsciiData_B))
		{
			IndexInMemoryZone_U32 = 0;
			VirtualOffset_S64=(_rDumpMemoryZoneParam_X.VirtualOffset_S64 == -1) ? reinterpret_cast<int64_t>(_rDumpMemoryZoneParam_X.pMemoryZone): _rDumpMemoryZoneParam_X.VirtualOffset_S64;
			switch (_rDumpMemoryZoneParam_X.AccessSize_E)
			{
			case BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_64:
				NbBytePerRead_U32 = 8;
				MemoryZoneSizeInByte_U32 = (_rDumpMemoryZoneParam_X.NbItemToDump_U32 * NbBytePerRead_U32);
				Remain_S32 = static_cast<int32_t>(MemoryZoneSizeInByte_U32) & 0xFFFFFFF8;
				break;

			case BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_32:
				NbBytePerRead_U32 = 4;
				MemoryZoneSizeInByte_U32 = (_rDumpMemoryZoneParam_X.NbItemToDump_U32 * NbBytePerRead_U32);
				Remain_S32 = static_cast<int32_t>(MemoryZoneSizeInByte_U32) & 0xFFFFFFFC;
				break;

			case BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_16:
				NbBytePerRead_U32 = 2;
				MemoryZoneSizeInByte_U32 = (_rDumpMemoryZoneParam_X.NbItemToDump_U32 * NbBytePerRead_U32);
				Remain_S32 = static_cast<int32_t>(MemoryZoneSizeInByte_U32) & 0xFFFFFFFE;
				break;

			case BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_8:
			default:
				NbBytePerRead_U32 = 1;
				MemoryZoneSizeInByte_U32 = (_rDumpMemoryZoneParam_X.NbItemToDump_U32 * NbBytePerRead_U32);
				Remain_S32 = static_cast<int32_t>(MemoryZoneSizeInByte_U32) & 0xFFFFFFFF;
				break;
			}
			if (_rDumpMemoryZoneParam_X.NbItemPerLine_U32 <= (MAX_NBBYTEPERLINE / NbBytePerRead_U32))
			{
				while (Remain_S32 > 0)
				{
					pBinaryData_c = pBinary_c;
					if (_rDumpMemoryZoneParam_X.GenerateVirtualOffset)
					{
						if (_rDumpMemoryZoneParam_X.ShowHexaPrefix_B)
						{
							if (VirtualOffset_S64 > 0xFFFFFFFF)
							{
								pBinaryData_c += sprintf(pBinaryData_c, "0x%016" PRIX64 "%c%c%c", VirtualOffset_S64, _rDumpMemoryZoneParam_X.Separator_c,_rDumpMemoryZoneParam_X.Separator_c,_rDumpMemoryZoneParam_X.Separator_c);
							}
							else
							{
								pBinaryData_c += sprintf(pBinaryData_c, "0x%08X%c%c%c", static_cast<int32_t>(VirtualOffset_S64), _rDumpMemoryZoneParam_X.Separator_c,_rDumpMemoryZoneParam_X.Separator_c,_rDumpMemoryZoneParam_X.Separator_c);
							}
						}
						else
						{
							if (VirtualOffset_S64 > 0xFFFFFFFF)
							{
								pBinaryData_c += sprintf(pBinaryData_c, "%016" PRIX64  "%c%c%c", VirtualOffset_S64, _rDumpMemoryZoneParam_X.Separator_c,_rDumpMemoryZoneParam_X.Separator_c,_rDumpMemoryZoneParam_X.Separator_c);
							}
							else
							{
								pBinaryData_c += sprintf(pBinaryData_c, "%08X%c%c%c", static_cast<int32_t>(VirtualOffset_S64), _rDumpMemoryZoneParam_X.Separator_c,_rDumpMemoryZoneParam_X.Separator_c,_rDumpMemoryZoneParam_X.Separator_c);
							}
						}
					}

					NbItemBytePerLine_U32 = _rDumpMemoryZoneParam_X.NbItemPerLine_U32*NbBytePerRead_U32;
					for (i_U32 = 0; i_U32 < NbItemBytePerLine_U32; i_U32 += NbBytePerRead_U32)
					{
						if (IndexInMemoryZone_U32 >= MemoryZoneSizeInByte_U32)
						{
							if (_rDumpMemoryZoneParam_X.GenerateBinaryData_B)
							{
								switch (_rDumpMemoryZoneParam_X.AccessSize_E)
								{
								case BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_64:
									if (_rDumpMemoryZoneParam_X.ShowHexaPrefix_B)
									{
										pBinaryData_c += sprintf(pBinaryData_c, "                   ");
									}
									else
									{
										pBinaryData_c += sprintf(pBinaryData_c, "                 ");
									}
									break;

								case BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_32:
									if (_rDumpMemoryZoneParam_X.ShowHexaPrefix_B)
									{
										pBinaryData_c += sprintf(pBinaryData_c, "           ");
									}
									else
									{
										pBinaryData_c += sprintf(pBinaryData_c, "         ");
									}
									break;

								case BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_16:
									if (_rDumpMemoryZoneParam_X.ShowHexaPrefix_B)
									{
										pBinaryData_c += sprintf(pBinaryData_c, "       ");
									}
									else
									{
										pBinaryData_c += sprintf(pBinaryData_c, "     ");
									}
									break;

								case BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_8:
								default:
									if (_rDumpMemoryZoneParam_X.ShowHexaPrefix_B)
									{
										pBinaryData_c += sprintf(pBinaryData_c, "     ");
									}
									else
									{
										pBinaryData_c += sprintf(pBinaryData_c, "   ");
									}
									break;
								}

							}
							if (_rDumpMemoryZoneParam_X.GenerateAsciiData_B)
							{
								for (j_U32 = 0; j_U32 < NbBytePerRead_U32; j_U32++)
								{
									pAscii_c[i_U32 + j_U32] = ' ';
								}
							}
						}
						else
						{
							if (_rDumpMemoryZoneParam_X.GenerateBinaryData_B)
							{
								switch (_rDumpMemoryZoneParam_X.AccessSize_E)
								{
								case BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_64:
//									*(uint64_t *)pData_U8 = *(uint64_t *)(&pMemoryZone_U8[IndexInMemoryZone_U32]);
									pDataPtr_U64 = reinterpret_cast<uint64_t *>(pData_U8);
									pDataVal_U64 = reinterpret_cast<volatile const uint64_t *>(&pMemoryZone_U8[IndexInMemoryZone_U32]);
									*pDataPtr_U64 = *pDataVal_U64;
									if (_rDumpMemoryZoneParam_X.ReverseEndianness_B)
									{
										BOF_SWAP64PTR(pData_U8);
									}

									if (_rDumpMemoryZoneParam_X.ShowHexaPrefix_B)
									{
										pBinaryData_c += sprintf(pBinaryData_c, "0x%016" PRIX64  "%c", *pDataPtr_U64, _rDumpMemoryZoneParam_X.Separator_c);
									}
									else
									{
										pBinaryData_c += sprintf(pBinaryData_c, "%016" PRIX64  "%c", *pDataPtr_U64, _rDumpMemoryZoneParam_X.Separator_c);
									}

									break;

								case BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_32:
//									*(uint32_t *)pData_U8 = *(uint32_t *)(&pMemoryZone_U8[IndexInMemoryZone_U32]);
									pDataPtr_U32 = reinterpret_cast<uint32_t *>(pData_U8);
									pDataVal_U32 = reinterpret_cast<volatile const uint32_t *>(&pMemoryZone_U8[IndexInMemoryZone_U32]);
									*pDataPtr_U32 = *pDataVal_U32;

									if (_rDumpMemoryZoneParam_X.ReverseEndianness_B)
									{
										BOF_SWAP32PTR(pData_U8);
									}
									if (_rDumpMemoryZoneParam_X.ShowHexaPrefix_B)
									{
										pBinaryData_c += sprintf(pBinaryData_c, "0x%08X%c", *pDataPtr_U32, _rDumpMemoryZoneParam_X.Separator_c);
									}
									else
									{
										pBinaryData_c += sprintf(pBinaryData_c, "%08X%c", *pDataPtr_U32, _rDumpMemoryZoneParam_X.Separator_c);
									}
									break;

								case BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_16:
//									*(uint16_t *)pData_U8 = *(uint16_t *)(&pMemoryZone_U8[IndexInMemoryZone_U32]);
									pDataPtr_U16 = reinterpret_cast<uint16_t *>(pData_U8);
									pDataVal_U16 = reinterpret_cast<volatile const uint16_t *>(&pMemoryZone_U8[IndexInMemoryZone_U32]);
									*pDataPtr_U16 = *pDataVal_U16;

									if (_rDumpMemoryZoneParam_X.ReverseEndianness_B)
									{
										BOF_SWAP16PTR(pData_U8);
									}
									if (_rDumpMemoryZoneParam_X.ShowHexaPrefix_B)
									{
										pBinaryData_c += sprintf(pBinaryData_c, "0x%04X%c", *pDataPtr_U16, _rDumpMemoryZoneParam_X.Separator_c);
									}
									else
									{
										pBinaryData_c += sprintf(pBinaryData_c, "%04X%c", *pDataPtr_U16, _rDumpMemoryZoneParam_X.Separator_c);
									}
									break;

								case BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_8:
								default:
									pData_U8[0] = pMemoryZone_U8[IndexInMemoryZone_U32 + 0];
									if (_rDumpMemoryZoneParam_X.ShowHexaPrefix_B)
									{
										pBinaryData_c += sprintf(pBinaryData_c, "0x%02X%c", *pData_U8, _rDumpMemoryZoneParam_X.Separator_c);
									}
									else
									{
										pBinaryData_c += sprintf(pBinaryData_c, "%02X%c", *pData_U8, _rDumpMemoryZoneParam_X.Separator_c);
									}
									break;
								}
							}
							if (_rDumpMemoryZoneParam_X.GenerateAsciiData_B)
							{
								for (j_U32 = 0; j_U32 < NbBytePerRead_U32; j_U32++)
								{
									Data_U8 = pMemoryZone_U8[IndexInMemoryZone_U32 + j_U32];
									pAscii_c[i_U32 + j_U32] = (Data_U8 < 32) ? '?' : Data_U8;
								}
							}
							IndexInMemoryZone_U32 += NbBytePerRead_U32;
							Remain_S32 -= NbBytePerRead_U32;
						}                            // else if (IndexInMemoryZone_U32 >= _MemoryZoneSizeInByte_U32)
					}                              // for (i_U32 = 0; i_U32 < _NbDataPerLine_U32; i_U32++)
					if (_rDumpMemoryZoneParam_X.GenerateBinaryData_B)
					{
						Rts_S += pBinary_c;
					}
					if (_rDumpMemoryZoneParam_X.GenerateAsciiData_B)
					{
						pAscii_c[NbItemBytePerLine_U32] = 0;
						Rts_S += pAscii_c;
					}
					Rts_S += Bof_Eol();
					VirtualOffset_S64 += NbItemBytePerLine_U32;
				}                                // while (Remain_S32 > 0)
			}
		}                                  // if ((_GenerateVirtualOffset) || (_GenerateBinaryData_B) || (_GenerateAsciiData_B))
	}                                    // if ((_pMemoryZone_U8) && (_NbDataPerLine_U32 <= MAX_NBBYTEPERLINE) && (_Separator_c != 0))
//	printf("%s", Rts_S.c_str());
	return Rts_S;
}


// spin, yield or sleep. use the time passed since last op as a hint
#if 0
void Bof_SpinYieldOrSleep(const std::chrono::time_point<std::chrono::system_clock> &_rNow, const std::chrono::time_point<std::chrono::system_clock> &_rLastOpTime, uint32_t _SpinLimitInMicro_U32,
                          uint32_t _YieldLimitInMicro_U32, uint32_t _SleepLimitInMicro_U32)
{
	auto time_since_op = _rNow - _rLastOpTime;

	// spin upto 50 micros
	if (time_since_op <= std::chrono::microseconds(_SpinLimitInMicro_U32))
	{
		return;
	}

	// yield upto 150 micros
	if (time_since_op <= std::chrono::microseconds(_YieldLimitInMicro_U32))
	{
		return std::this_thread::yield();
	}


	// sleep for 20 ms upto 200 ms
	if (time_since_op <= std::chrono::microseconds(_SleepLimitInMicro_U32))
	{
		return std::this_thread::sleep_for(std::chrono::microseconds(_SleepLimitInMicro_U32));
	}

	// sleep for 200 ms
	return std::this_thread::sleep_for(std::chrono::microseconds(_SleepLimitInMicro_U32 * 10));
}
#endif
void Bof_MsSleep(uint32_t _Ms_U32)
{
	if (_Ms_U32 == 0)
	{
		std::this_thread::yield();
	}
	else
	{
#if defined (_WIN32)
		//same as Sleep for windows zclock_sleep (_Ms_U32);
		timeBeginPeriod(1);
#else
#endif
		std::this_thread::sleep_for(std::chrono::milliseconds(_Ms_U32));
#if defined (_WIN32)
		timeEndPeriod(1);
#else
#endif
	}
}
void Bof_UsSleep(uint32_t _Us_U32)
{
  if (_Us_U32 == 0)
  {
    std::this_thread::yield();
  }
  else
  {
#if defined (_WIN32)
    //same as Sleep for windows zclock_sleep (_Ms_U32);
		timeBeginPeriod(1);
#else
#endif
    std::this_thread::sleep_for(std::chrono::microseconds(_Us_U32));
#if defined (_WIN32)
    timeEndPeriod(1);
#else
#endif
  }
}

uint32_t Bof_GetMsTickCount()
{
	uint64_t NbMs_U64 = std::chrono::steady_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
	return static_cast<uint32_t>(NbMs_U64);
}
//change if system time/date change
uint64_t Bof_GetUsTickCount()
{
  uint64_t Rts_U64;
  // Number of tick count in 1 sec
  // 10000000 -> 1 tick = 100 nano std::cout << std::chrono::high_resolution_clock::period::den << std::endl;
  // std::chrono::time_point<std::chrono::high_resolution_clock> Now
  Rts_U64 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
  // std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << ":";
  // Rts_U64 = Now.

  return Rts_U64;
}
uint64_t Bof_GetNsTickCount()
{
  uint64_t Rts_U64;
  Rts_U64 = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
  return Rts_U64;
}

uint32_t Bof_ElapsedMsTime(uint32_t _StartInMs_U32)
{
  uint32_t Rts_U32;

  Rts_U32 = Bof_GetMsTickCount() - _StartInMs_U32;   // Unsigned arithmetic makes it works in any case even if Now_U64<_Start_U64
  return Rts_U32;
}

uint64_t Bof_ElapsedUsTime(uint64_t _StartInUs_U64)
{
  uint64_t Rts_U64;

  Rts_U64 = Bof_GetUsTickCount() - _StartInUs_U64; // Unsigned arithmetic makes it works in any case even if Now_U64<_Start_U64
  return Rts_U64;
}

uint64_t Bof_ElapsedNsTime(uint64_t _StartInNs_U64)
{
  uint64_t Rts_U64;

  Rts_U64 = Bof_GetNsTickCount() - _StartInNs_U64; // Unsigned arithmetic makes it works in any case even if Now_U64<_Start_U64
  return Rts_U64;
}

BOFERR Bof_ReEvaluateTimeout(uint32_t _Start_U32, uint32_t &_rNewTimeOut_U32)
{
	BOFERR Rts_E = BOF_ERR_NO_ERROR;
	uint32_t Delta_U32;

	Delta_U32 = Bof_ElapsedMsTime(_Start_U32);
	if (Delta_U32 >= _rNewTimeOut_U32)
	{
		Rts_E = BOF_ERR_ETIMEDOUT;
		_rNewTimeOut_U32 = 0;
	}
	else
	{
		_rNewTimeOut_U32 -= Delta_U32;
	}

	return Rts_E;
}


bool Bof_IsElapsedTimeInMs(uint32_t _Start_U32, uint32_t _TimeoutInMs_U32)
{
	return (Bof_ElapsedMsTime(_Start_U32) >= _TimeoutInMs_U32);
}

/*
const char*Bof_Eol()
{
#if defined (_WIN32)
	return "\r\n";
#else
	return "\n";
#endif
}

char Bof_FilenameSeparator()
{
#if defined (_WIN32)
	return '\\';
#else
	return '/';
#endif
}
*/
// Min and Max value are include
int32_t Bof_Random(bool _Reset_B, int32_t _MinValue_S32, int32_t _MaxValue_S32)
{
	int32_t Rts_S32, Range_S32;

	if (_Reset_B)
	{
		S_RandomGenerator.seed(S_Rd());
	}
	if (_MinValue_S32 > _MaxValue_S32)
	{
		std::swap(_MinValue_S32, _MaxValue_S32);
	}
	Range_S32 = _MaxValue_S32 - _MinValue_S32 + 1;

	Rts_S32 = _MinValue_S32 + static_cast< int32_t > (S_RandomFloatDistribution(S_RandomGenerator) * static_cast< float > (Range_S32));

	return Rts_S32;
}

std::string Bof_Random(bool _Reset_B, uint32_t _Size_U32, char _MinValue_c, char _MaxValue_c)
{
	std::string Rts_S;
	uint32_t i_U32;

	for (i_U32 = 0; i_U32 < _Size_U32; i_U32++)
	{
		Rts_S += static_cast< char > (Bof_Random(_Reset_B, _MinValue_c, _MaxValue_c));
	}
	return Rts_S;
}

  BOFERR Bof_SystemUsageInfo(BOF_SYSTEM_USAGE_INFO &_rSystemUsageInfo_X)
  {
    BOFERR            Rts_E=BOF_ERR_NOT_SUPPORTED;

    memset(&_rSystemUsageInfo_X, 0, sizeof(BOF_NAMESPACE::BOF_SYSTEM_USAGE_INFO));
#if defined (_WIN32)
#else
    struct rusage  Usage_X;
    struct sysinfo SysInfo_X;
    float LoadDivisor_f;

    int Sts_i;
    Sts_i = getrusage(RUSAGE_SELF, &Usage_X);
    if (Sts_i == 0)
    {
      _rSystemUsageInfo_X.TIME.UserCpuUsedInSec_f   = static_cast<float>(Usage_X.ru_utime.tv_sec) + (static_cast<float>(Usage_X.ru_utime.tv_usec) / 1000000.0f);
      _rSystemUsageInfo_X.TIME.SystemCpuUsedInSec_f = static_cast<float>(Usage_X.ru_stime.tv_sec) + (static_cast<float>(Usage_X.ru_stime.tv_usec) / 1000000.0f);
      _rSystemUsageInfo_X.MEMORY.MaxRssInKB_U64             =Usage_X.ru_maxrss;
      _rSystemUsageInfo_X.OS.NbSoftPageFault_U64            =Usage_X.ru_minflt;
      _rSystemUsageInfo_X.OS.NbHardPageFault_U64            =Usage_X.ru_majflt;
      _rSystemUsageInfo_X.OS.NbBlkInputOp_U64               =Usage_X.ru_inblock;
      _rSystemUsageInfo_X.OS.NbBlkOutputOp_U64              =Usage_X.ru_oublock;
      _rSystemUsageInfo_X.OS.NbVoluntaryContextSwitch_U64   =Usage_X.ru_nvcsw;
      _rSystemUsageInfo_X.OS.NbInvoluntaryContextSwitch_U64 =Usage_X.ru_nivcsw;

      Sts_i = sysinfo(&SysInfo_X);
      if (Sts_i == 0)
      {
        LoadDivisor_f=static_cast<float>(1<<SI_LOAD_SHIFT);
        _rSystemUsageInfo_X.TIME.UpTimeInSec_U64 =SysInfo_X.uptime;
        _rSystemUsageInfo_X.OS.pLoad_f[0] =static_cast<float>(SysInfo_X.loads[0]) / LoadDivisor_f;
        _rSystemUsageInfo_X.OS.pLoad_f[1] =static_cast<float>(SysInfo_X.loads[1]) / LoadDivisor_f;
        _rSystemUsageInfo_X.OS.pLoad_f[2] =static_cast<float>(SysInfo_X.loads[2]) / LoadDivisor_f;
        _rSystemUsageInfo_X.MEMORY.TotalRamInKB_U64  = (SysInfo_X.totalram * SysInfo_X.mem_unit) / 1024;
        _rSystemUsageInfo_X.MEMORY.FreeRamInKB_U64   = (SysInfo_X.freeram * SysInfo_X.mem_unit) / 1024;
        _rSystemUsageInfo_X.MEMORY.SharedRamInKB_U64 = (SysInfo_X.sharedram * SysInfo_X.mem_unit) / 1024;
        _rSystemUsageInfo_X.MEMORY.BufferRamInKB_U64 = (SysInfo_X.bufferram * SysInfo_X.mem_unit) / 1024;
        _rSystemUsageInfo_X.MEMORY.TotalSwapInKB_U64 = (SysInfo_X.totalswap * SysInfo_X.mem_unit) / 1024;
        _rSystemUsageInfo_X.MEMORY.FreeSwapInKB_U64  = (SysInfo_X.freeswap * SysInfo_X.mem_unit) / 1024;
        _rSystemUsageInfo_X.OS.NbProcess_U64 =SysInfo_X.procs;
        Rts_E=BOF_ERR_NO_ERROR;
      }
    }
#endif
    return Rts_E;
  }
  std::string Bof_SystemUsageInfoToString(const BOF_SYSTEM_USAGE_INFO &_rSystemUsageInfo_X, const BOF_SYSTEM_USAGE_INFO *_pPreviousSystemUsageInfo_X)
  {
    std::string Rts_S;

    Rts_S=Bof_Sprintf("Up time/User/System CPU time used            %ld/%f/%f s\n", _rSystemUsageInfo_X.TIME.UpTimeInSec_U64, _rSystemUsageInfo_X.TIME.UserCpuUsedInSec_f, _rSystemUsageInfo_X.TIME.SystemCpuUsedInSec_f);
    if (_pPreviousSystemUsageInfo_X)
    {
      Rts_S+=Bof_Sprintf("   Delta                                     %ld/%f/%f s\n", _rSystemUsageInfo_X.TIME.UpTimeInSec_U64-_pPreviousSystemUsageInfo_X->TIME.UpTimeInSec_U64, _rSystemUsageInfo_X.TIME.UserCpuUsedInSec_f-_pPreviousSystemUsageInfo_X->TIME.UserCpuUsedInSec_f, _rSystemUsageInfo_X.TIME.SystemCpuUsedInSec_f-_pPreviousSystemUsageInfo_X->TIME.SystemCpuUsedInSec_f);
    }

    Rts_S+=Bof_Sprintf("Page reclaims/faults (soft/hard page faults) %ld/%ld\n", _rSystemUsageInfo_X.OS.NbSoftPageFault_U64, _rSystemUsageInfo_X.OS.NbHardPageFault_U64);
    if (_pPreviousSystemUsageInfo_X)
    {
      Rts_S+=Bof_Sprintf("   Delta                                     %ld/%ld\n", _rSystemUsageInfo_X.OS.NbSoftPageFault_U64-_pPreviousSystemUsageInfo_X->OS.NbSoftPageFault_U64, _rSystemUsageInfo_X.OS.NbHardPageFault_U64-_pPreviousSystemUsageInfo_X->OS.NbHardPageFault_U64);
    }
    Rts_S+=Bof_Sprintf("Block input/output operations                %ld/%ld\n", _rSystemUsageInfo_X.OS.NbBlkInputOp_U64, _rSystemUsageInfo_X.OS.NbBlkOutputOp_U64);
    if (_pPreviousSystemUsageInfo_X)
    {
      Rts_S+=Bof_Sprintf("   Delta                                     %ld/%ld\n", _rSystemUsageInfo_X.OS.NbBlkInputOp_U64-_pPreviousSystemUsageInfo_X->OS.NbBlkInputOp_U64, _rSystemUsageInfo_X.OS.NbBlkOutputOp_U64-_pPreviousSystemUsageInfo_X->OS.NbBlkOutputOp_U64);
    }
    Rts_S+=Bof_Sprintf("Voluntary/Involuntary context switches       %ld/%ld\n", _rSystemUsageInfo_X.OS.NbVoluntaryContextSwitch_U64, _rSystemUsageInfo_X.OS.NbInvoluntaryContextSwitch_U64);
    if (_pPreviousSystemUsageInfo_X)
    {
      Rts_S+=Bof_Sprintf("   Delta                                     %ld/%ld\n", _rSystemUsageInfo_X.OS.NbVoluntaryContextSwitch_U64-_pPreviousSystemUsageInfo_X->OS.NbVoluntaryContextSwitch_U64, _rSystemUsageInfo_X.OS.NbInvoluntaryContextSwitch_U64-_pPreviousSystemUsageInfo_X->OS.NbInvoluntaryContextSwitch_U64);
    }
    Rts_S+=Bof_Sprintf("Nb Process: Load 1/5/15 Min                  %ld: %f/%f/%f\n", _rSystemUsageInfo_X.OS.NbProcess_U64, _rSystemUsageInfo_X.OS.pLoad_f[0], _rSystemUsageInfo_X.OS.pLoad_f[1], _rSystemUsageInfo_X.OS.pLoad_f[2]);
    if (_pPreviousSystemUsageInfo_X)
    {
      Rts_S+=Bof_Sprintf("   Delta                                     %ld: %f/%f/%f\n", _rSystemUsageInfo_X.OS.NbProcess_U64-_pPreviousSystemUsageInfo_X->OS.NbProcess_U64, _rSystemUsageInfo_X.OS.pLoad_f[0] - _pPreviousSystemUsageInfo_X->OS.pLoad_f[0], _rSystemUsageInfo_X.OS.pLoad_f[1] - _pPreviousSystemUsageInfo_X->OS.pLoad_f[1], _rSystemUsageInfo_X.OS.pLoad_f[2] - _pPreviousSystemUsageInfo_X->OS.pLoad_f[2]);
    }

    Rts_S+=Bof_Sprintf("Rss/Free/Total/Shared/Buffer memory          %ld/%ld/%ld/%ld/%ld KB\n", _rSystemUsageInfo_X.MEMORY.MaxRssInKB_U64, _rSystemUsageInfo_X.MEMORY.FreeRamInKB_U64, _rSystemUsageInfo_X.MEMORY.TotalRamInKB_U64, _rSystemUsageInfo_X.MEMORY.SharedRamInKB_U64, _rSystemUsageInfo_X.MEMORY.BufferRamInKB_U64);
    if (_pPreviousSystemUsageInfo_X)
    {
      Rts_S+=Bof_Sprintf("   Delta                                     %ld/%ld/%ld/%ld/%ld KB\n", _rSystemUsageInfo_X.MEMORY.MaxRssInKB_U64-_pPreviousSystemUsageInfo_X->MEMORY.MaxRssInKB_U64, _rSystemUsageInfo_X.MEMORY.FreeRamInKB_U64-_pPreviousSystemUsageInfo_X->MEMORY.FreeRamInKB_U64, _rSystemUsageInfo_X.MEMORY.TotalRamInKB_U64-_pPreviousSystemUsageInfo_X->MEMORY.TotalRamInKB_U64, _rSystemUsageInfo_X.MEMORY.SharedRamInKB_U64-_pPreviousSystemUsageInfo_X->MEMORY.SharedRamInKB_U64, _rSystemUsageInfo_X.MEMORY.BufferRamInKB_U64-_pPreviousSystemUsageInfo_X->MEMORY.BufferRamInKB_U64);
    }
    Rts_S+=Bof_Sprintf("Swap High                                    %ld/%ld %ld/%ld KB\n", _rSystemUsageInfo_X.MEMORY.FreeSwapInKB_U64, _rSystemUsageInfo_X.MEMORY.TotalSwapInKB_U64, _rSystemUsageInfo_X.MEMORY.FreeHighInKB_U64, _rSystemUsageInfo_X.MEMORY.TotaHighInKB_U64);
    if (_pPreviousSystemUsageInfo_X)
    {
      Rts_S+=Bof_Sprintf("   Delta                                     %ld/%ld %ld/%ld KB\n", _rSystemUsageInfo_X.MEMORY.FreeSwapInKB_U64-_pPreviousSystemUsageInfo_X->MEMORY.FreeSwapInKB_U64, _rSystemUsageInfo_X.MEMORY.TotalSwapInKB_U64-_pPreviousSystemUsageInfo_X->MEMORY.TotalSwapInKB_U64, _rSystemUsageInfo_X.MEMORY.FreeHighInKB_U64-_pPreviousSystemUsageInfo_X->MEMORY.FreeHighInKB_U64, _rSystemUsageInfo_X.MEMORY.TotaHighInKB_U64-_pPreviousSystemUsageInfo_X->MEMORY.TotaHighInKB_U64);
    }
    // printf("Maximum resident set size/Free/Total memory  %ld KB\n", Usage_X.ru_maxrss, (get_avphys_pages() * getpagesize()) / 1024, (get_phys_pages() * getpagesize()) / 1024);

    return Rts_S;
/*
    char pData_c[1024];
    FILE *pIo_X = fopen("/proc/slabinfo", "r");
    if (pIo_X)
    {
      while (fgets(pData_c, sizeof(pData_c),pIo_X)!=nullptr)
      {
        SYSLOG(SYSLOG_MUL_CHANNEL_GENERAL, SYSLOG_LEVEL_CRITICAL, 0, 0, 0, ">>>%s", pData_c);
      }
      fclose(pIo_X);
    }
    pIo_X = fopen("/proc/pagetypeinfo", "r");
    if (pIo_X)
    {
      while (fgets(pData_c, sizeof(pData_c),pIo_X)!=nullptr)
      {
        SYSLOG(SYSLOG_MUL_CHANNEL_GENERAL, SYSLOG_LEVEL_CRITICAL, 0, 0, 0, "]]]%s", pData_c);
      }
      fclose(pIo_X);
    }
    pIo_X = fopen("/proc/meminfo", "r");
    if (pIo_X)
    {
      while (fgets(pData_c, sizeof(pData_c), pIo_X) != nullptr)
      {
        SYSLOG(SYSLOG_MUL_CHANNEL_GENERAL, SYSLOG_LEVEL_CRITICAL, 0, 0, 0, "[[[%s", pData_c);
      }
      fclose(pIo_X);
    }
 //   si_swapinfo(&i);

   */


  }
#if defined (_WIN32)
#else
//Lock all code in RAM, preventing code/data swap
#include <sys/mman.h>
#include <malloc.h>
//https://rt.wiki.kernel.org/index.php/Threaded_RT-application_with_memory_locking_and_stack_handling_example

#pragma GCC push_options
#pragma GCC optimize ("O0")
static void Prefaulter(uint32_t _StackSizeInByte_U32)
{
  uint8_t pBuffer_U8[_StackSizeInByte_U32];

  memset(pBuffer_U8, 0x00, sizeof(pBuffer_U8));
}

static BOFERR reserve_process_memory(uint64_t _ReserveProcessMemoryInByte_U64)
{
  uint64_t  i;
  char *buffer;
  BOFERR Rts_E=BOF_ERR_ENOMEM;

  buffer = (char *) malloc(_ReserveProcessMemoryInByte_U64);

  if (buffer)
  {
    Rts_E=BOF_ERR_NO_ERROR;
    /* Touch each page in this piece of memory to get it mapped into RAM */
    for (i = 0; i < _ReserveProcessMemoryInByte_U64; i += sysconf(_SC_PAGESIZE))
    {
      /* Each write to this buffer will generate a pagefault.
         Once the pagefault is handled a page will be locked in
         memory and never given back to the system. */
      buffer[i] = 0;
    }

    /* buffer will now be released. As Glibc is configured such that it
       never gives back memory to the kernel, the memory allocated above is
       locked for this process. All malloc() and new() calls come from
       the memory pool reserved and locked above. Issuing free() and
       delete() does NOT make this locking undone. So, with this locking
       mechanism we can build C++ applications that will never run into
       a major/minor pagefault, even with swapping enabled. */
    free(buffer);
  }
  return Rts_E;
}
#pragma GCC pop_options

static void configure_malloc_behavior()
{
  /* Now lock all current and future pages
     from preventing of being paged */
  if (mlockall(MCL_CURRENT | MCL_FUTURE))
  {
    perror("mlockall failed:");
  }

  /* Turn off malloc trimming.*/
  mallopt(M_TRIM_THRESHOLD, -1);

  /* Turn off mmap usage. */
//  mallopt(M_MMAP_MAX, 0);
}
void Bof_LockRam_ShowNewPagefaultCount(const char *logtext,  const char *allowed_maj, const char *allowed_min)
{
  static long int    last_majflt = 0, last_minflt = 0;
  struct rusage usage;

  getrusage(RUSAGE_SELF, &usage);

  printf("%-30.30s: Pagefaults, Major:%ld (Allowed %s), Minor:%ld (Allowed %s)\n", logtext, usage.ru_majflt - last_majflt, allowed_maj, usage.ru_minflt - last_minflt, allowed_min);

  last_majflt = usage.ru_majflt;
  last_minflt = usage.ru_minflt;
}

BOFERR Bof_LockRam(uint32_t _StackSizeInByte_U32,uint64_t _ReserveProcessMemoryInByte_U64)
{
  BOFERR Rts_E;

  // Reserve enough memory for mlockall
  Prefaulter(_StackSizeInByte_U32);

  Bof_LockRam_ShowNewPagefaultCount("Initial count", ">=0", ">=0");

  configure_malloc_behavior();

  Bof_LockRam_ShowNewPagefaultCount("mlockall() generated", ">=0", ">=0");

  Rts_E=reserve_process_memory(_ReserveProcessMemoryInByte_U64);

  Bof_LockRam_ShowNewPagefaultCount("malloc() and touch generated",  ">=0", ">=0");

#if 0
  /* Now allocate the memory for the 2nd time and prove the number of pagefaults are zero */
  reserve_process_memory(PRE_ALLOCATION_SIZE);
  show_new_pagefault_count("2nd malloc() and use generated","0", "0");

  printf("\n\nLook at the output of ps -leyf, and see that the RSS is now about %d [MB]\n", PRE_ALLOCATION_SIZE / (1024 * 1024));

  start_rt_thread();

  //<do your RT-thing>

  printf("Press <ENTER> to exit\n");
  getchar();
#endif
  return Rts_E;
}
#endif

END_BOF_NAMESPACE()


#if defined (_WIN32)
#include <ctype.h>
#include <string.h>
#include <time.h>


/*
* We do not implement alternate representations. However, we always
* check whether a given modifier is allowed for a certain conversion.
*/
#define ALT_E          0x01
#define ALT_O          0x02
//#define LEGAL_ALT(x)       { if (alt_format & ~(x)) return (0); }
#define LEGAL_ALT(x)       { ; }
#define TM_YEAR_BASE   (1900) //BHA (1970)

static   int conv_num(const char **, int *, int, int);
static int strncasecmp(char *s1, char *s2, size_t n);

static const char *day[7] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
	"Friday", "Saturday"
};
static const char *abday[7] = {
	"Sun","Mon","Tue","Wed","Thu","Fri","Sat"
};
static const char *mon[12] = {
	"January", "February", "March", "April", "May", "June", "July",
	"August", "September", "October", "November", "December"
};
static const char *abmon[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
static const char *am_pm[2] = {
	"AM", "PM"
};


char * strptime(const char *buf, const char *fmt, struct tm *tm)
{
	char c;
	const char *bp;
	size_t len = 0;
	int alt_format, i, split_year = 0;

	bp = buf;

	while ((c = *fmt) != '\0')
	{
		/* Clear `alternate' modifier prior to new conversion. */
		alt_format = 0;

		/* Eat up white-space. */
		if (isspace(c))
		{
			while (isspace(*bp))
				bp++;

			fmt++;
			continue;
		}

		if ((c = *fmt++) != '%')
			goto literal;


	again:        switch (c = *fmt++)
	{
	case '%': /* "%%" is converted to "%". */
		literal :
			if (c != *bp++)
				return (0);
		break;

		/*
		* "Alternative" modifiers. Just set the appropriate flag
		* and start over again.
		*/
	case 'E': /* "%E?" alternative conversion modifier. */
		LEGAL_ALT(0);
		alt_format |= ALT_E;
		goto again;

	case 'O': /* "%O?" alternative conversion modifier. */
		LEGAL_ALT(0);
		alt_format |= ALT_O;
		goto again;

		/*
		* "Complex" conversion rules, implemented through recursion.
		*/
	case 'c': /* Date and time, using the locale's format. */
		LEGAL_ALT(ALT_E);
		bp = strptime(bp, "%x %X", tm);
		if (!bp)
			return (0);
		break;

	case 'D': /* The date as "%m/%d/%y". */
		LEGAL_ALT(0);
		bp = strptime(bp, "%m/%d/%y", tm);
		if (!bp)
			return (0);
		break;

	case 'R': /* The time as "%H:%M". */
		LEGAL_ALT(0);
		bp = strptime(bp, "%H:%M", tm);
		if (!bp)
			return (0);
		break;

	case 'r': /* The time in 12-hour clock representation. */
		LEGAL_ALT(0);
		bp = strptime(bp, "%I:%M:%S %p", tm);
		if (!bp)
			return (0);
		break;

	case 'T': /* The time as "%H:%M:%S". */
		LEGAL_ALT(0);
		bp = strptime(bp, "%H:%M:%S", tm);
		if (!bp)
			return (0);
		break;

	case 'X': /* The time, using the locale's format. */
		LEGAL_ALT(ALT_E);
		bp = strptime(bp, "%H:%M:%S", tm);
		if (!bp)
			return (0);
		break;

	case 'x': /* The date, using the locale's format. */
		LEGAL_ALT(ALT_E);
		bp = strptime(bp, "%m/%d/%y", tm);
		if (!bp)
			return (0);
		break;

		/*
		* "Elementary" conversion rules.
		*/
	case 'A': /* The day of week, using the locale's form. */
	case 'a':
		LEGAL_ALT(0);
		for (i = 0; i < 7; i++)
		{
			/* Full name. */
			len = strlen(day[i]);
			if (strncasecmp((char *)(day[i]), (char *)bp, len) == 0)
				break;

			/* Abbreviated name. */
			len = strlen(abday[i]);
			if (strncasecmp((char *)(abday[i]), (char *)bp, len) == 0)
				break;
		}

		/* Nothing matched. */
		if (i == 7)
			return (0);

		tm->tm_wday = i;
		bp += len;
		break;

	case 'B': /* The month, using the locale's form. */
	case 'b':
	case 'h':
		LEGAL_ALT(0);
		for (i = 0; i < 12; i++)
		{
			/* Full name. */

			len = strlen(mon[i]);
			if (strncasecmp((char *)(mon[i]), (char *)bp, len) == 0)
				break;

			/* Abbreviated name. */
			len = strlen(abmon[i]);
			if (strncasecmp((char *)(abmon[i]), (char *)bp, len) == 0)
				break;
		}

		/* Nothing matched. */
		if (i == 12)
			return (0);

		tm->tm_mon = i;
		bp += len;
		break;

	case 'C': /* The century number. */
		LEGAL_ALT(ALT_E);
		if (!(conv_num(&bp, &i, 0, 99)))
			return (0);

		if (split_year)
		{
			tm->tm_year = (tm->tm_year % 100) + (i * 100);
		}
		else {
			tm->tm_year = i * 100;
			split_year = 1;
		}
		break;

	case 'd': /* The day of month. */
	case 'e':
		LEGAL_ALT(ALT_O);
		if (!(conv_num(&bp, &tm->tm_mday, 1, 31)))
			return (0);
		break;

	case 'k': /* The hour (24-hour clock representation). */
		LEGAL_ALT(0);
		/* FALLTHROUGH */
	case 'H':
		LEGAL_ALT(ALT_O);
		if (!(conv_num(&bp, &tm->tm_hour, 0, 23)))
			return (0);
		break;

	case 'l': /* The hour (12-hour clock representation). */
		LEGAL_ALT(0);
		/* FALLTHROUGH */
	case 'I':
		LEGAL_ALT(ALT_O);
		if (!(conv_num(&bp, &tm->tm_hour, 1, 12)))
			return (0);
		if (tm->tm_hour == 12)
			tm->tm_hour = 0;
		break;

	case 'j': /* The day of year. */
		LEGAL_ALT(0);
		if (!(conv_num(&bp, &i, 1, 366)))
			return (0);
		tm->tm_yday = i - 1;
		break;

	case 'M': /* The minute. */
		LEGAL_ALT(ALT_O);
		if (!(conv_num(&bp, &tm->tm_min, 0, 59)))
			return (0);
		break;

	case 'm': /* The month. */
		LEGAL_ALT(ALT_O);
		if (!(conv_num(&bp, &i, 1, 12)))
			return (0);
		tm->tm_mon = i - 1;
		break;

		//            case 'p': /* The locale's equivalent of AM/PM. */
		//                LEGAL_ALT(0);
		//                /* AM? */
		//                if (strcasecmp(am_pm[0], bp) == 0)
		//                {
		//                    if (tm->tm_hour > 11)
		//                        return (0);
		//
		//                    bp += strlen(am_pm[0]);
		//                    break;
		//                }
		//                /* PM? */
		//                else if (strcasecmp(am_pm[1], bp) == 0)
		//                {
		//                    if (tm->tm_hour > 11)
		//                        return (0);
		//
		//                    tm->tm_hour += 12;
		//                    bp += strlen(am_pm[1]);
		//                    break;
		//                }
		//
		//                /* Nothing matched. */
		//                return (0);

	case 'S': /* The seconds. */
		LEGAL_ALT(ALT_O);
		if (!(conv_num(&bp, &tm->tm_sec, 0, 61)))
			return (0);
		break;

	case 'U': /* The week of year, beginning on sunday. */
	case 'W': /* The week of year, beginning on monday. */
		LEGAL_ALT(ALT_O);
		/*
		* XXX This is bogus, as we can not assume any valid
		* information present in the tm structure at this
		* point to calculate a real value, so just check the
		* range for now.
		*/
		if (!(conv_num(&bp, &i, 0, 53)))
			return (0);
		break;

	case 'w': /* The day of week, beginning on sunday. */
		LEGAL_ALT(ALT_O);
		if (!(conv_num(&bp, &tm->tm_wday, 0, 6)))
			return (0);
		break;

	case 'Y': /* The year. */
		LEGAL_ALT(ALT_E);
		if (!(conv_num(&bp, &i, 0, 9999)))
			return (0);

		tm->tm_year = i - TM_YEAR_BASE;
		break;

	case 'y': /* The year within 100 years of the epoch. */
		LEGAL_ALT(ALT_E | ALT_O);
		if (!(conv_num(&bp, &i, 0, 99)))
			return (0);

		if (split_year)
		{
			tm->tm_year = ((tm->tm_year / 100) * 100) + i;
			break;
		}
		split_year = 1;
		if (i <= 68)
			tm->tm_year = i + 2000 - TM_YEAR_BASE;
		else
			tm->tm_year = i + 1900 - TM_YEAR_BASE;
		break;

		/*
		* Miscellaneous conversions.
		*/
	case 'n': /* Any kind of white-space. */
	case 't':
		LEGAL_ALT(0);
		while (isspace(*bp))
			bp++;
		break;


	default: /* Unknown/unsupported conversion. */
		return (0);
	}


	}

	/* LINTED functional specification */
	return ((char *)bp);
}


static int conv_num(const char **buf, int *dest, int llim, int ulim)
{
	int result = 0;

	/* The limit also determines the number of valid digits. */
	int rulim = ulim;

	if (**buf < '0' || **buf > '9')
		return (0);

	do {
		result *= 10;
		result += *(*buf)++ - '0';
		rulim /= 10;
	} while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

	if (result < llim || result > ulim)
		return (0);

	*dest = result;
	return (1);
}

int strncasecmp(char *s1, char *s2, size_t n)
{
	if (n == 0)
		return 0;

	while (n-- != 0 && tolower(*s1) == tolower(*s2))
	{
		if (n == 0 || *s1 == '\0' || *s2 == '\0')
			break;
		s1++;
		s2++;
	}

	return tolower(*(unsigned char *)s1) - tolower(*(unsigned char *)s2);
}
#else
#endif

