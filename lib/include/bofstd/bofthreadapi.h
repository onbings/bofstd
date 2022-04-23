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
 * Name:        BofThreadApi.h
 * Author:      Nicolas Marique : n.marique@evs.tv
 * Revision:    1.0
 *
 * History:
 *
 *      V 1.00  Wed May 08 2013  NMA : Initial release
 */

#pragma once
/*** Includes **********************************************************************************************************************/
#include <bofstd/bofstd.h>

BEGIN_BOF_NAMESPACE()



#include <cstdint>

/*** Defines ***********************************************************************************************************************/


typedef void *(*BOF_THREAD_FUNC)(void *);

#define BOF_THREAD_INFINITE_TIMEOUT    ( (uint32_t) -1 )

/*!
 * Summary
 * The various access types
 */

#define BOF_THREAD_ACCESS_NONE                 ( 0x01 )
#define BOF_THREAD_ACCESS_READ                 ( 0x02 )
#define BOF_THREAD_ACCESS_WRITE                ( 0x04 )
#define BOF_THREAD_ACCESS_EXECUTE              ( 0x08 )

#define BOF_THREAD_ACCESS_READWRITE            ( BOF_THREAD_ACCESS_READ | BOF_THREAD_ACCESS_WRITE )
#define BOF_THREAD_ACCESS_EXECUTE_READ         ( BOF_THREAD_ACCESS_READ | BOF_THREAD_ACCESS_EXECUTE )
#define BOF_THREAD_ACCESS_EXECUTE_WRITE        ( BOF_THREAD_ACCESS_WRITE | BOF_THREAD_ACCESS_EXECUTE )
#define BOF_THREAD_ACCESS_EXECUTE_READWRITE    ( BOF_THREAD_ACCESS_WRITE | BOF_THREAD_ACCESS_WRITE | BOF_THREAD_ACCESS_EXECUTE )

/*** Enums *************************************************************************************************************************/

/*!
 * Summary
 * The BOF thread priority levels
 */
typedef enum
{
		BOF_THREAD_PRIORITY_TIME_CRITICAL = 0,
		BOF_THREAD_PRIORITY_HIGHEST = 1,
		BOF_THREAD_PRIORITY_ABOVE_NORMAL = 2,
		BOF_THREAD_PRIORITY_NORMAL = 3,
		BOF_THREAD_PRIORITY_BELOW_NORMAL = 4,
		BOF_THREAD_PRIORITY_LOWEST = 5,
		BOF_THREAD_PRIORITY_ABOVE_IDLE = 6,
		BOF_THREAD_PRIORITY_IDLE = 7,
		BOF_THREAD_PRIORITY_MAX
} BOF_THREAD_PRIORITY;

#if !defined( __APPLE__ )
typedef union semun
{
		int val;                                                /* Value for SETVAL */
		struct semid_ds *buf;                                                /* Buffer for IPC_STAT, IPC_SET */
		unsigned short *array;                                              /* Array for GETALL, SETALL */
		struct seminfo *__buf;                                              /* Buffer for IPC_INFO
																																			 *                      (Linux-specific) */
}
	BOF_SEM_UNION;
#endif

/*** Structures ********************************************************************************************************************/

/*** Prototypes ********************************************************************************************************************/

BOF_EXTERN_C void *Bof_CreateThread(uint32_t _StackSize_U32, BOF_THREAD_FUNC _pFunc, void *_pContext, bool _Suspended_B);
BOF_EXTERN_C uint32_t Bof_SetThreadName(void *_Thread_h, const char *_pName_c);
BOF_EXTERN_C uint32_t Bof_SetThreadCpuCoreAffinity(void *_Thread_h, uint64_t _CpuCoreAffinity_U64);
//BOF_EXTERN_C uint32_t Bof_ResumeThread(void * _Thread_h);
//BOF_EXTERN_C uint32_t Bof_SuspendThread(void * _Thread_h);
BOF_EXTERN_C bool Bof_IsPriorityAvailable(BOF_THREAD_PRIORITY _Priority_E);
#if defined ( _WIN32 )
BOF_EXTERN_C uint32_t Bof_SetProcessPriority(BOF_THREAD_PRIORITY _Priority_E);
#endif
BOF_EXTERN_C uint32_t Bof_SetThreadPriority(void *_Thread_h, BOF_THREAD_PRIORITY _Priority_E);
BOF_EXTERN_C uint32_t Bof_GetThreadPriority(void *_Thread_h, BOF_THREAD_PRIORITY *_pPriority_E);
BOF_EXTERN_C uint32_t Bof_SetThreadPriorityNative(void *_Thread_h, int32_t _Policy_S32, int32_t _Priority_S32);
BOF_EXTERN_C uint32_t Bof_GetThreadPriorityNative(void *_Thread_h, int32_t *_pPolicy_S32, int32_t *_pPriority_S32);
BOF_EXTERN_C uint32_t Bof_GetThreadExitCode(void *_Thread_h, void **_pRetCode);
BOF_EXTERN_C uint32_t Bof_CloseThread(void *_Thread_h);

BOF_EXTERN_C void *Bof_CreateEvent(bool _InitialState_B, const char *_pName_c);
BOF_EXTERN_C BOFERR Bof_SetEvent(void *_Event_h);
BOF_EXTERN_C uint32_t Bof_ResetEvent(void *_Event_h);
BOF_EXTERN_C uint32_t Bof_WaitForEvent(void *_Event_h, uint32_t _Timeout_U32);
BOF_EXTERN_C uint32_t Bof_CloseEvent(void *_Event_h);

BOF_EXTERN_C void *Bof_CreateMutex(bool _HasOwnership_B, const char *_pName_c);
BOF_EXTERN_C uint32_t Bof_WaitForMutex(void *_Mutex_h);
BOF_EXTERN_C uint32_t Bof_ReleaseMutex(void *_Mutex_h);
BOF_EXTERN_C uint32_t Bof_CloseMutex(void *_Mutex_h);

BOF_EXTERN_C void *Bof_CreateFileMapping(uint32_t _AccessType_U32, const char *_pName_c, uint32_t _Size_U32, bool *_pAlreadyExist_B);
BOF_EXTERN_C uint32_t Bof_UnmapViewOfFile(void *_pMap);
BOF_EXTERN_C uint32_t Bof_RemoveFileMapping(void *_FileMap_h);  //__ to avoid name collision with BofPortability

BOF_EXTERN_C void *Bof_MapViewOfFile(void *_FileMap_h, uint32_t _AccessType_U32, uint32_t _Offset_U32, uint32_t _Size_U32);
BOF_EXTERN_C void *Bof_CreateVirtualMemoryMap(uint32_t _AccessType_U32, const char *_pName_c, uint32_t _Offset_U32, uint32_t _Length_U32);
BOF_EXTERN_C uint32_t Bof_CloseVirtualMemoryMap(void *pMap, uint32_t _Length_U32);

END_BOF_NAMESPACE()
