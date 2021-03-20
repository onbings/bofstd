/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines routines for creating and managing an event driver reactor actor. It is
 * based on czmq zloop
 *
 * Name:        BofEventReactor.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Mar 11 2002  BHA : Initial release
 */

#pragma once

/*** Include ****************************************************************/
#include <functional>
#include <bofstd/bofstd.h>
#include <bofstd/bofflag.h>
#include <cstdint>
#include <memory>

BEGIN_BOF_NAMESPACE()

/*** Define *****************************************************************/

/*** Enum *****************************************************************/

enum class BOF_EVENT_REACTOR_TYPE : uint32_t
{
		BOF_EVENT_REACTOR_UNKNOWN,
		BOF_EVENT_REACTOR_SOCKET,
		BOF_EVENT_REACTOR_ZMQ,
		BOF_EVENT_REACTOR_TIMER,
		BOF_EVENT_REACTOR_TICKET_TIMER,
};

enum class BOF_POLL_FLAG : short
{
		BOF_POLL_NONE = 0x0000,
		BOF_POLL_IN = 0x0001,
		BOF_POLL_OUT = 0x0002,
		BOF_POLL_ERR = 0x0004,
		BOF_POLL_PRI = 0x0008,
};

template<>
struct IsItAnEnumBitFLag<BOF_POLL_FLAG> : std::true_type
{
};

/*** Structure **************************************************************/

using BofEventReactorFunction = std::function<int(BOF_EVENT_REACTOR_TYPE _EventReactorType_E, uint32_t _Arg_U32, void *_pContext)>;

/*** BofEventReactor **********************************************************************/

class BofEventReactor
{
		// Opaque pointer design pattern: all public and protected stuff goes here ...
public:
		BofEventReactor(const std::string &_rName_S);

		BofEventReactor &operator=(const BofEventReactor &) = delete;     // Disallow copying
		BofEventReactor(const BofEventReactor &) = delete;

		virtual ~BofEventReactor();

		//_NbCycle_U32=0 set infinite BOF_EVENT_REACTOR_INDIVIDUAL_TIMER
		BOFERR AddTimerEventGenerator(uint32_t _NbCycle_U32, uint32_t _CycleTimeoutInMs_U32, BofEventReactorFunction _ReactorCallback, void *_pContext, intptr_t &_rTimerId);

		BOFERR RemoveTimerEventGenerator(intptr_t _TimerId);

		BOFERR SetGlobalTicketTimerCycleTimeOut(uint32_t _CycleTimeoutInMs_U32);

		BOFERR AddTicketTimerEventGenerator(BofEventReactorFunction _ReactorCallback, void *_pContext, intptr_t &_rTimerId);

		BOFERR ResetTicketTimer(intptr_t _TimerId);

		BOFERR RemoveTicketTimerEventGenerator(intptr_t _TimerId);

		BOFERR AddPollSocketEventGenerator(int _Socket_i, BOF_POLL_FLAG _PollFlag_E, BofEventReactorFunction _ReactorCallback, void *_pContext, intptr_t &_rPollId);

		BOFERR ChangePollSocketEventGenerator(intptr_t _rPollId, BOF_POLL_FLAG _PollFlag_E);

		BOFERR AddPollSocketEventGenerator(void *_pSocket, BOF_POLL_FLAG _PollFlag_E, BofEventReactorFunction _ReactorCallback, void *_pContext, intptr_t &_rPollId);

		BOFERR RemovePollEventGenerator(intptr_t _rPollId);

		BOFERR StartEventReactorLoop(bool _Verbose_B, bool _NonStop_B);
		// Opaque pointer design pattern: opaque type here
public:
		class EventReactorImplementation;

		std::unique_ptr<EventReactorImplementation> mpuEventReactorImplementation;
};

END_BOF_NAMESPACE()