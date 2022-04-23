/*!
 * Copyright (c) 2008-2020, Evs. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * The generic class that derives from a BofThread class
 *
 * Name:        BofThreadGeneric.h
 * Author:      Nicolas Marique : n.marique@evs.tv
 * Revision:    1.0
 *
 * History:
 *
 *      V 1.00  Wed May 08 2013  NMA : Initial release
 */

/*** Includes **********************************************************************************************************************/
#include <cstdint>
#include <bofstd/bofthreadgeneric.h>
#include <string.h>

BEGIN_BOF_NAMESPACE()

/*** Defines ***********************************************************************************************************************/

/*** Enums *************************************************************************************************************************/

/*** Structures ********************************************************************************************************************/

/*** Prototypes ********************************************************************************************************************/

/*!
 * Description
 * The class constructor
 *
 * Parameters
 * None
 *
 * Returns
 * Nothing
 *
 * Remarks
 * None
 */
CBofThreadGeneric::CBofThreadGeneric() : BofThread()
{
	mFunc = nullptr;
	mpContext = nullptr;
	mpRetCode = nullptr;
	mDone_B = false;
	mStackSize_U32 = 0;

	//NO ! memset(&mParams_X, 0x00, sizeof( mParams_X ) );
	mParams_X = GetThreadParam();
}


/*!
 * Description
 * The class destructor
 *
 * Parameters
 * None
 *
 * Returns
 * Nothing
 *
 * Remarks
 * None
 */
CBofThreadGeneric::~CBofThreadGeneric()
{
}


/*!
 * Description
 * This function registers the thread
 * function to execute
 *
 * Parameters
 * _pFunc  - The thread function
 *
 * Returns
 * 0 - The operation was successful
 * !0 - The operation failed
 *
 * Remarks
 * None
 */
uint32_t CBofThreadGeneric::SetThreadFunction(BOF_THREAD_GENERIC_FUNC _pFunc)
{
	uint32_t Ret_U32 = 0;

	mFunc = _pFunc;

	return Ret_U32;
}


/*!
 * Description
 * This function registers the context
 * argument to pass to the thread function
 *
 * Parameters
 * _pContext  - The thread context
 *
 * Returns
 * 0 - The operation was successful
 * !0 - The operation failed
 *
 * Remarks
 * None
 */
uint32_t CBofThreadGeneric::SetThreadContext(void *_pContext)
{
	uint32_t Ret_U32 = 0;

	mpContext = _pContext;

	return Ret_U32;
}


/*!
 * Description
 * This function sets the size of the
 * stack to use for the thread
 *
 * Parameters
 * _StackSize_U32 - The thread context
 *
 * Returns
 * 0 - The operation was successful
 * !0 - The operation failed
 *
 * Remarks
 * None
 */
uint32_t CBofThreadGeneric::SetThreadStackSize(uint32_t _StackSize_U32)
{
	uint32_t Ret_U32 = 0;

	mStackSize_U32 = _StackSize_U32;

	return Ret_U32;
}


/*!
 * Description
 * This function sets the thread parameters
 *
 * Parameters
 * _pParams_X - The thread parameters
 *
 * Returns
 * 0 - The operation was successful
 * !0 - The operation failed
 *
 * Remarks
 * None
 */
uint32_t CBofThreadGeneric::SetThreadParams(BOFTHREADPARAM *_pParams_X)
{
	uint32_t Ret_U32 = (uint32_t) -1;
//BOFTHREADPARAM *pThreadState_X;

	if (_pParams_X != nullptr)
	{
		memcpy(&mParams_X, _pParams_X, sizeof(mParams_X));
		/*
		pThreadState_X = GetThreadState();
		if (pThreadState_X)
		{
			memcpy(pThreadState_X, _pParams_X, sizeof(mParams_X));
		}
*/
		Ret_U32 = 0;
	}

	return Ret_U32;
}


/*!
 * Description
 * This function returns the thread stack size
 *
 * Parameters
 * None
 *
 * Returns
 * The thread stack size
 *
 * Remarks
 * None
 */
uint32_t CBofThreadGeneric::GetThreadStackSize()
{
	return mStackSize_U32;
}


/*!
 * Description
 * This function retrieves the thread parameters
 *
 * Parameters
 * _pParams_X - The pointer to where to store the thread parameters
 *
 * Returns
 * 0 - The operation was successful
 * !0 - The operation failed
 *
 * Remarks
 * None
 */
uint32_t CBofThreadGeneric::GetThreadParams(BOFTHREADPARAM *_pParams_X)
{
	uint32_t Ret_U32 = (uint32_t) -1;

	if (_pParams_X != nullptr)
	{
		memcpy(_pParams_X, &mParams_X, sizeof(mParams_X));

		Ret_U32 = 0;
	}

	return Ret_U32;
}


/*!
 * Description
 * The V_OnCreate method is called by the BofThread_Thread thread function which is
 * created by the Start method. This function is called when the thread starts and
 * can be overwritten to provide specific application  behaviour
 *
 * Parameters
 * None
 *
 * Returns
 * 0 - The operation was successful
 * !0 - The operation failed
 *
 * Remarks
 * None
 */
BOFERR CBofThreadGeneric::V_OnCreate()
{
	return BOFERR_NO_ERROR;
}


/*!
 * Description
 * The V_OnProcessing method is called by the BofThread_Thread thread function which is
 * created by the Start method. This function is the main thread function and can be
 * overwritten to provide specific application  behaviour
 *
 * Parameters
 * _WaitCode_U32 - Specifies the result of the wait operation on the thread event.
 *                If it is not used it is equal to WAIT_OBJECT_0
 *
 * Returns
 * 0 - The operation was successful
 * !0 - The operation failed
 *
 * Remarks
 * None
 */
BOFERR CBofThreadGeneric::V_OnProcessing()
{
	BOFERR Rts_E = BOFERR_NO_ERROR;

	mDone_B = false;

	if (mFunc != nullptr)
	{
		mpRetCode = mFunc(mpContext);
	}

	mDone_B = true;

	return Rts_E;
}


/*!
 * Description
 * The OnStop method is called by the BofThread_Thread thread function which is
 * created by the Start method. This function is called when the thread ends and
 * can be overwritten to provide specific application  behaviour
 *
 * Parameters
 * None
 *
 * Returns
 * 0 - The operation was successful
 * !0 - The operation failed
 *
 * Remarks
 * None
 */
BOFERR CBofThreadGeneric::V_OnStop()
{
	return BOFERR_NO_ERROR;
}


/*!
 * Description
 * This function indicates if the thread
 * processing is done or not
 *
 * Parameters
 * None
 *
 * Returns
 * true  - The thread process is done
 * false - The thread process is not done
 *
 * Remarks
 * None
 */
bool CBofThreadGeneric::IsThreadDone()
{
	return mDone_B;
}


/*!
 * Description
 * This function returns the thread exit code
 *
 * Parameters
 * None
 *
 * Returns
 * The thread exit code
 *
 * Remarks
 * None
 */
void *CBofThreadGeneric::GetExitCode()
{
	return mpRetCode;
}
END_BOF_NAMESPACE()

