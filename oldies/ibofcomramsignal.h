/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the comram signaling mechanism interface
 *
 * Name:        ibofcomramsignal.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */

#pragma once

/*** Include files ***********************************************************/
#include <cstdint>
#include <functional>
#include <bofstd/bofstd.h>

BEGIN_BOF_NAMESPACE()

/*** Global variables ********************************************************/

/*** Definitions *************************************************************/

/*** Class *******************************************************************/

using BofSignalFunction = std::function<BOFERR(uint32_t _Ticket_U32, uint32_t &_rNb_U32, uint8_t *_pBuffer_U8, void *_pContext)>;

class IBofComramSignal
{
public:
		// Empty virtual destructor for proper cleanup
		virtual ~IBofComramSignal()
		{}

		virtual BOFERR V_Signal() = 0;

		virtual BOFERR V_WaitForSignal(uint32_t _PollTimeInMs_U32) = 0;
};

END_BOF_NAMESPACE()