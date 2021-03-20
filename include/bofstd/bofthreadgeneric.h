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

#pragma once

/*** Includes **********************************************************************************************************************/

#include <cstdint>
#include <bofstd/bofthread.h>

BEGIN_BOF_NAMESPACE()


/*** Defines ***********************************************************************************************************************/

typedef void *(*BOF_THREAD_GENERIC_FUNC)(void *);

/*** Enums *************************************************************************************************************************/

/*** Structures ********************************************************************************************************************/

/*** Prototypes ********************************************************************************************************************/


/*!
 * Summary
 * The generic BofThread class
 */
class CBofThreadGeneric : public BofThread
{
public:

		CBofThreadGeneric();

		virtual ~CBofThreadGeneric();

		uint32_t SetThreadFunction(BOF_THREAD_GENERIC_FUNC _pFunc);

		uint32_t SetThreadContext(void *_pContext);

		uint32_t SetThreadStackSize(uint32_t _StackSize_U32);

		uint32_t SetThreadParams(BOFTHREADPARAM *_pParams_X);

		uint32_t GetThreadStackSize();

		uint32_t GetThreadParams(BOFTHREADPARAM *_pParams_X);

		bool IsThreadDone();

		void *GetExitCode();

protected:

		virtual BOFERR V_OnCreate() override;

		virtual BOFERR V_OnProcessing() override;

		virtual BOFERR V_OnStop() override;

private:

		BOF_THREAD_GENERIC_FUNC mFunc;
		void *mpContext;
		void *mpRetCode;
		bool mDone_B;
		uint32_t mStackSize_U32;
		BOFTHREADPARAM mParams_X;
};
END_BOF_NAMESPACE()

