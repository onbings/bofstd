/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements routines for creating and managing an event driver reactor actor. It is
 * based on czmq zloop
 * 
 * Name:        BofEventReactor.cpp
 * Author:      Bernard HARMEL: b.harmel@belgacom.net
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Mar 11 2002  BHA : Initial release
 */

/*** Include files ***********************************************************/
#include <bofstd/bofeventreactor.h>
#include <czmq.h>
#include <map>
#include <bofstd/bofsystem.h>

/*** Global variables ********************************************************/

/*** BofEventReactor *****************************************************************/
USE_BOF_NAMESPACE()
BEGIN_BOF_NAMESPACE()

static int S_EventReactorTimerCallback(zloop_t *_pLoop_X, int _TimerId_i, void *_pContext);

static int S_EventReactorPollCallback(zloop_t *_pLoop_X, zmq_pollitem_t *_pItem_X, void *_pContext);

static int S_EventReactorZmqPollCallback(zloop_t *_pLoop_X, zsock_t *_pReader_X, void *_pContext);

const uint32_t BOF_EVENT_REACTOR_MAGIC = 0x15B4B5C1;

struct BOF_EVENT_REACTOR_PARAM
{
		uint32_t Magic_U32;
		BOF_EVENT_REACTOR_TYPE EventType_E;
		BofEventReactorFunction ReactorCallback;
		void *pContext;
//For timer
		intptr_t TimerId;
		bool TimerDeleted_B;
//For Socket
		zmq_pollitem_t PollItem_X;

		BOF_EVENT_REACTOR_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			Magic_U32 = 0;
			EventType_E = BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_UNKNOWN;
			ReactorCallback = nullptr;
			pContext = nullptr;
			TimerId = 0;
			TimerDeleted_B = false;
			memset(&PollItem_X, 0, sizeof(PollItem_X));
		}
};


// Opaque pointer design pattern: defined privately here all private data and functions: all of these can now change without recompiling callers ...
class BofEventReactor::EventReactorImplementation
{
private:
		std::string mName_S;
		zloop_t *mpEventReactorLoop_X;
		BOF_MUTEX mMtxEventReactor;

public:

		EventReactorImplementation(const std::string &_rName_S)
		{
			Bof_CreateMutex(_rName_S + "_Mutex", true, true, mMtxEventReactor);
			mName_S = _rName_S;
			mpEventReactorLoop_X = zloop_new();
			BOF_ASSERT(mpEventReactorLoop_X != nullptr);
		}

		~EventReactorImplementation()
		{
			Bof_LockMutex(mMtxEventReactor);
			zloop_destroy(&mpEventReactorLoop_X);
			Bof_UnlockMutex(mMtxEventReactor);

			Bof_DestroyMutex(mMtxEventReactor);
		}

		BOFERR AddTimerEventGenerator(uint32_t _NbCycle_U32, uint32_t _CycleTimeoutInMs_U32, BofEventReactorFunction _ReactorCallback, void *_pContext, intptr_t &_rTimerId)
		{
			BOFERR Rts_E = BOF_ERR_EINVAL;
			int Sts_i;
			BOF_EVENT_REACTOR_PARAM *pEventReactorTimerParam_X;

			if (_ReactorCallback != nullptr)
			{
				Rts_E = BOF_ERR_ENOMEM;
				Bof_LockMutex(mMtxEventReactor);
				pEventReactorTimerParam_X = new BOF_EVENT_REACTOR_PARAM();
				if (pEventReactorTimerParam_X)
				{
					pEventReactorTimerParam_X->Reset(); //Magic is now invalid->callback does not trig
					Rts_E = BOF_ERR_OPERATION_FAILED;
					Sts_i = zloop_timer(mpEventReactorLoop_X, _CycleTimeoutInMs_U32, _NbCycle_U32, S_EventReactorTimerCallback, pEventReactorTimerParam_X);
					if (Sts_i != -1)
					{
						pEventReactorTimerParam_X->Magic_U32 = BOF_EVENT_REACTOR_MAGIC;
						pEventReactorTimerParam_X->TimerId = Sts_i;
						pEventReactorTimerParam_X->EventType_E = BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_TIMER;
						pEventReactorTimerParam_X->ReactorCallback = _ReactorCallback;
						pEventReactorTimerParam_X->pContext = _pContext;
						_rTimerId = reinterpret_cast<intptr_t>(pEventReactorTimerParam_X);
						Rts_E = BOF_ERR_NO_ERROR;
					}
					else
					{
						BOF_SAFE_DELETE(pEventReactorTimerParam_X);
					}
				}
				Bof_UnlockMutex(mMtxEventReactor);
			}
			return Rts_E;
		}

		BOFERR RemoveTimerEventGenerator(intptr_t _TimerId)
		{
			BOFERR Rts_E = BOF_ERR_NOT_FOUND;
			BOF_EVENT_REACTOR_PARAM *pEventReactorTimerParam_X = reinterpret_cast<BOF_EVENT_REACTOR_PARAM *>(_TimerId);
			int Sts_i;

			Bof_LockMutex(mMtxEventReactor);
			if ((pEventReactorTimerParam_X) && (pEventReactorTimerParam_X->Magic_U32 == BOF_EVENT_REACTOR_MAGIC))
			{
				Rts_E = BOF_ERR_BAD_TYPE;
				if (pEventReactorTimerParam_X->EventType_E == BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_TIMER)
				{
					Rts_E = BOF_ERR_OPERATION_FAILED;
					Sts_i = zloop_timer_end(mpEventReactorLoop_X, static_cast<int>(pEventReactorTimerParam_X->TimerId));
					if (!Sts_i)
					{
						Rts_E = BOF_ERR_NO_ERROR;
					}
					BOF_SAFE_DELETE(pEventReactorTimerParam_X);
				}
			}
			Bof_UnlockMutex(mMtxEventReactor);
			return Rts_E;
		}

		BOFERR SetGlobalTicketTimerCycleTimeOut(uint32_t _CycleTimeoutInMs_U32)
		{
			BOFERR Rts_E = BOF_ERR_NO_ERROR;

			zloop_set_ticket_delay(mpEventReactorLoop_X, _CycleTimeoutInMs_U32);
			return Rts_E;
		}

		BOFERR AddTicketTimerEventGenerator(BofEventReactorFunction _ReactorCallback, void *_pContext, intptr_t &_rTimerId)
		{
			BOFERR Rts_E = BOF_ERR_EINVAL;
			void *pTicketTimer;
			intptr_t TimerId;
			BOF_EVENT_REACTOR_PARAM *pEventReactorTimerParam_X;

			if (_ReactorCallback != nullptr)
			{
				Rts_E = BOF_ERR_ENOMEM;
				Bof_LockMutex(mMtxEventReactor);
				pEventReactorTimerParam_X = new BOF_EVENT_REACTOR_PARAM();
				if (pEventReactorTimerParam_X)
				{
					pEventReactorTimerParam_X->Reset(); //Magic is now invalid->callback does not trig
					Rts_E = BOF_ERR_OPERATION_FAILED;
					pTicketTimer = zloop_ticket(mpEventReactorLoop_X, S_EventReactorTimerCallback, pEventReactorTimerParam_X);
					if (pTicketTimer != nullptr)
					{
						TimerId = reinterpret_cast<intptr_t>(pTicketTimer);
						pEventReactorTimerParam_X->Magic_U32 = BOF_EVENT_REACTOR_MAGIC;
						pEventReactorTimerParam_X->TimerId = TimerId;
						pEventReactorTimerParam_X->EventType_E = BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_TICKET_TIMER;
						pEventReactorTimerParam_X->ReactorCallback = _ReactorCallback;
						pEventReactorTimerParam_X->pContext = _pContext;
						_rTimerId = reinterpret_cast<intptr_t>(pEventReactorTimerParam_X);
						Rts_E = BOF_ERR_NO_ERROR;
					}
					else
					{
						BOF_SAFE_DELETE(pEventReactorTimerParam_X);
					}
				}
				Bof_UnlockMutex(mMtxEventReactor);
			}
			return Rts_E;
		}

		BOFERR RemoveTicketTimerEventGenerator(intptr_t _TimerId)
		{
			BOFERR Rts_E = BOF_ERR_NOT_FOUND;
			BOF_EVENT_REACTOR_PARAM *pEventReactorTimerParam_X = reinterpret_cast<BOF_EVENT_REACTOR_PARAM *>(_TimerId);
			void *pTicketTimer;

			Bof_LockMutex(mMtxEventReactor);
			if ((pEventReactorTimerParam_X) && (pEventReactorTimerParam_X->Magic_U32 == BOF_EVENT_REACTOR_MAGIC))
			{
				Rts_E = BOF_ERR_BAD_TYPE;
				if (pEventReactorTimerParam_X->EventType_E == BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_TICKET_TIMER)
				{
					if (!pEventReactorTimerParam_X->TimerDeleted_B)
					{
						pTicketTimer = reinterpret_cast<void *>(pEventReactorTimerParam_X->TimerId);
						zloop_ticket_delete(mpEventReactorLoop_X, pTicketTimer);
					}
					Rts_E = BOF_ERR_NO_ERROR;
					BOF_SAFE_DELETE(pEventReactorTimerParam_X);
				}
			}
			Bof_UnlockMutex(mMtxEventReactor);
			return Rts_E;
		}

		BOFERR ResetTicketTimer(intptr_t _TimerId)
		{
			BOFERR Rts_E = BOF_ERR_NOT_FOUND;
			BOF_EVENT_REACTOR_PARAM *pEventReactorTimerParam_X = reinterpret_cast<BOF_EVENT_REACTOR_PARAM *>(_TimerId);
			void *pTicketTimer;

			Bof_LockMutex(mMtxEventReactor);
			if ((pEventReactorTimerParam_X) && (pEventReactorTimerParam_X->Magic_U32 == BOF_EVENT_REACTOR_MAGIC))
			{
				Rts_E = BOF_ERR_BAD_TYPE;
				if ((pEventReactorTimerParam_X->EventType_E == BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_TICKET_TIMER) && (!pEventReactorTimerParam_X->TimerDeleted_B))
				{
					pTicketTimer = reinterpret_cast<void *>(pEventReactorTimerParam_X->TimerId);
					zloop_ticket_reset(mpEventReactorLoop_X, pTicketTimer);
					Rts_E = BOF_ERR_NO_ERROR;
				}
			}
			Bof_UnlockMutex(mMtxEventReactor);
			return Rts_E;
		}


		BOFERR AddPollSocketEventGenerator(int _Socket_i, BOF_POLL_FLAG _PollFlag_E, BofEventReactorFunction _ReactorCallback, void *_pContext, intptr_t &_rPollId)
		{
			BOFERR Rts_E = BOF_ERR_EINVAL;
			int Sts_i;
			BOF_EVENT_REACTOR_PARAM *pEventReactorSocketParam_X;

			if (_ReactorCallback != nullptr)
			{
				Rts_E = BOF_ERR_ENOMEM;
				Bof_LockMutex(mMtxEventReactor);
				pEventReactorSocketParam_X = new BOF_EVENT_REACTOR_PARAM();
				if (pEventReactorSocketParam_X)
				{
					pEventReactorSocketParam_X->Reset(); //Magic is now invalid->callback does not trig
					Rts_E = BOF_ERR_OPERATION_FAILED;
					//	https://gist.github.com/claws/6394946

					pEventReactorSocketParam_X->PollItem_X.socket = nullptr;
					pEventReactorSocketParam_X->PollItem_X.fd = _Socket_i;  // -1;
					pEventReactorSocketParam_X->PollItem_X.events = 0;
					if (Bof_IsBitFlagSet(_PollFlag_E, BOF_POLL_FLAG::BOF_POLL_IN))
					{
						pEventReactorSocketParam_X->PollItem_X.events |= ZMQ_POLLIN;
					}
					if (Bof_IsBitFlagSet(_PollFlag_E, BOF_POLL_FLAG::BOF_POLL_OUT))
					{
						pEventReactorSocketParam_X->PollItem_X.events |= ZMQ_POLLOUT;
					}
					if (Bof_IsBitFlagSet(_PollFlag_E, BOF_POLL_FLAG::BOF_POLL_ERR))
					{
						pEventReactorSocketParam_X->PollItem_X.events |= ZMQ_POLLERR;
					}
#if defined(ZMQ_POLLPRI)
					if (Bof_IsBitFlagSet(_PollFlag_E, BOF_POLL_FLAG::BOF_POLL_PRI))
					{
						pEventReactorSocketParam_X->PollItem_X.events |= ZMQ_POLLPRI;
					}
#endif
					pEventReactorSocketParam_X->PollItem_X.revents = static_cast<short>(_PollFlag_E);

					Sts_i = zloop_poller(mpEventReactorLoop_X, &pEventReactorSocketParam_X->PollItem_X, S_EventReactorPollCallback, pEventReactorSocketParam_X);
					if (Sts_i == 0)
					{
						//Must be after !
						zloop_poller_set_tolerant(mpEventReactorLoop_X, &pEventReactorSocketParam_X->PollItem_X);

						pEventReactorSocketParam_X->Magic_U32 = BOF_EVENT_REACTOR_MAGIC;
						pEventReactorSocketParam_X->TimerId = 0;
						pEventReactorSocketParam_X->EventType_E = BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_SOCKET;
						pEventReactorSocketParam_X->ReactorCallback = _ReactorCallback;
						pEventReactorSocketParam_X->pContext = _pContext;
						_rPollId = reinterpret_cast<intptr_t>(pEventReactorSocketParam_X);
						Rts_E = BOF_ERR_NO_ERROR;
					}
					else
					{
						BOF_SAFE_DELETE(pEventReactorSocketParam_X);
					}
				}
				Bof_UnlockMutex(mMtxEventReactor);
			}
			return Rts_E;
		}


		BOFERR AddPollSocketEventGenerator(void *_pSocket, BOF_POLL_FLAG _PollFlag_E, BofEventReactorFunction _ReactorCallback, void *_pContext, intptr_t &_rPollId)
		{
			BOFERR Rts_E = BOF_ERR_EINVAL;
			int Sts_i;
			BOF_EVENT_REACTOR_PARAM *pEventReactorSocketParam_X;

			if ((_ReactorCallback != nullptr) && (_pSocket) && (zsock_is(_pSocket)))
			{
				Bof_LockMutex(mMtxEventReactor);
				Rts_E = BOF_ERR_ENOMEM;
				pEventReactorSocketParam_X = new BOF_EVENT_REACTOR_PARAM();
				if (pEventReactorSocketParam_X)
				{
					pEventReactorSocketParam_X->Reset(); //Magic is now invalid->callback does not trig
					Rts_E = BOF_ERR_OPERATION_FAILED;
					//	https://gist.github.com/claws/6394946

					pEventReactorSocketParam_X->PollItem_X.socket = _pSocket;
					pEventReactorSocketParam_X->PollItem_X.fd = 0;
					pEventReactorSocketParam_X->PollItem_X.events = 0;
					if (Bof_IsBitFlagSet(_PollFlag_E, BOF_POLL_FLAG::BOF_POLL_IN))
					{
						pEventReactorSocketParam_X->PollItem_X.events |= ZMQ_POLLIN;
					}
					if (Bof_IsBitFlagSet(_PollFlag_E, BOF_POLL_FLAG::BOF_POLL_OUT))
					{
						pEventReactorSocketParam_X->PollItem_X.events |= ZMQ_POLLOUT;
					}
					if (Bof_IsBitFlagSet(_PollFlag_E, BOF_POLL_FLAG::BOF_POLL_ERR))
					{
						pEventReactorSocketParam_X->PollItem_X.events |= ZMQ_POLLERR;
					}
#if defined(ZMQ_POLLPRI)
					if (Bof_IsBitFlagSet(_PollFlag_E, BOF_POLL_FLAG::BOF_POLL_PRI))
					{
						pEventReactorSocketParam_X->PollItem_X.events |= ZMQ_POLLPRI;
					}
#endif
					pEventReactorSocketParam_X->PollItem_X.revents = static_cast<short>(_PollFlag_E);

					Sts_i = zloop_reader(mpEventReactorLoop_X, reinterpret_cast<zsock_t *>(_pSocket), S_EventReactorZmqPollCallback, pEventReactorSocketParam_X);
					if (Sts_i == 0)
					{
						//Must be after !
						zloop_poller_set_tolerant(mpEventReactorLoop_X, &pEventReactorSocketParam_X->PollItem_X);

						pEventReactorSocketParam_X->Magic_U32 = BOF_EVENT_REACTOR_MAGIC;
						pEventReactorSocketParam_X->TimerId = 0;
						pEventReactorSocketParam_X->EventType_E = BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_ZMQ;
						pEventReactorSocketParam_X->ReactorCallback = _ReactorCallback;
						pEventReactorSocketParam_X->pContext = _pContext;
						_rPollId = reinterpret_cast<intptr_t>(pEventReactorSocketParam_X);
						Rts_E = BOF_ERR_NO_ERROR;
					}
					else
					{
						BOF_SAFE_DELETE(pEventReactorSocketParam_X);
					}
				}
				Bof_UnlockMutex(mMtxEventReactor);
			}
			return Rts_E;
		}

		BOFERR ChangePollSocketEventGenerator(intptr_t _rPollId, BOF_POLL_FLAG _PollFlag_E)
		{
			BOFERR Rts_E = BOF_ERR_NOT_FOUND;
			BOF_EVENT_REACTOR_PARAM *pEventReactorSocketParam_X = reinterpret_cast<BOF_EVENT_REACTOR_PARAM *>(_rPollId);
			int Sts_i;

			Bof_LockMutex(mMtxEventReactor);
			if ((pEventReactorSocketParam_X) && (pEventReactorSocketParam_X->Magic_U32 == BOF_EVENT_REACTOR_MAGIC))
			{
				Rts_E = BOF_ERR_BAD_TYPE;
				if (pEventReactorSocketParam_X->EventType_E == BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_SOCKET)
				{
					Rts_E = BOF_ERR_OPERATION_FAILED;

					zloop_poller_end(mpEventReactorLoop_X, &pEventReactorSocketParam_X->PollItem_X);

					pEventReactorSocketParam_X->PollItem_X.events = 0;
					if (Bof_IsBitFlagSet(_PollFlag_E, BOF_POLL_FLAG::BOF_POLL_IN))
					{
						pEventReactorSocketParam_X->PollItem_X.events |= ZMQ_POLLIN;
					}
					if (Bof_IsBitFlagSet(_PollFlag_E, BOF_POLL_FLAG::BOF_POLL_OUT))
					{
						pEventReactorSocketParam_X->PollItem_X.events |= ZMQ_POLLOUT;
					}
					if (Bof_IsBitFlagSet(_PollFlag_E, BOF_POLL_FLAG::BOF_POLL_ERR))
					{
						pEventReactorSocketParam_X->PollItem_X.events |= ZMQ_POLLERR;
					}
#if defined(ZMQ_POLLPRI)
					if (Bof_IsBitFlagSet(_PollFlag_E, BOF_POLL_FLAG::BOF_POLL_PRI))
					{
						pEventReactorSocketParam_X->PollItem_X.events |= ZMQ_POLLPRI;
					}
#endif
					pEventReactorSocketParam_X->PollItem_X.revents = static_cast<short>(_PollFlag_E);

					Sts_i = zloop_poller(mpEventReactorLoop_X, &pEventReactorSocketParam_X->PollItem_X, S_EventReactorPollCallback, pEventReactorSocketParam_X);
					if (Sts_i == 0)
					{
						//Must be after !
						zloop_poller_set_tolerant(mpEventReactorLoop_X, &pEventReactorSocketParam_X->PollItem_X);
						Rts_E = BOF_ERR_NO_ERROR;
					}
					else
					{
						BOF_SAFE_DELETE(pEventReactorSocketParam_X);
					}
				}
			}
			Bof_UnlockMutex(mMtxEventReactor);
			return Rts_E;
		}

		BOFERR RemovePollEventGenerator(intptr_t _rPollId)
		{
			BOFERR Rts_E = BOF_ERR_NOT_FOUND;
			BOF_EVENT_REACTOR_PARAM *pEventReactorSocketParam_X = reinterpret_cast<BOF_EVENT_REACTOR_PARAM *>(_rPollId);

			Bof_LockMutex(mMtxEventReactor);
			if ((pEventReactorSocketParam_X) && (pEventReactorSocketParam_X->Magic_U32 == BOF_EVENT_REACTOR_MAGIC))
			{
				Rts_E = BOF_ERR_BAD_TYPE;
				if (pEventReactorSocketParam_X->EventType_E == BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_SOCKET)
				{
					zloop_poller_end(mpEventReactorLoop_X, &pEventReactorSocketParam_X->PollItem_X);
					BOF_SAFE_DELETE(pEventReactorSocketParam_X);

					Rts_E = BOF_ERR_NO_ERROR;
				}
				else if (pEventReactorSocketParam_X->EventType_E == BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_ZMQ)
				{
					zloop_reader_end(mpEventReactorLoop_X, reinterpret_cast<zsock_t *>(pEventReactorSocketParam_X->PollItem_X.socket));
					BOF_SAFE_DELETE(pEventReactorSocketParam_X);

					Rts_E = BOF_ERR_NO_ERROR;
				}
			}
			Bof_UnlockMutex(mMtxEventReactor);
			return Rts_E;
		}

		//  By default the reactor stops if the process receives a SIGINT or SIGTERM
		//  signal. This makes it impossible to shut-down message based architectures
		//  like zactors. This method lets you switch off break handling. The default
		//  nonstop setting is off (false).
		BOFERR StartEventReactorLoop(bool _Verbose_B, bool _NonStop_B)
		{
			BOFERR Rts_E;
			int Sts_i;

			zsys_set_logstream(_Verbose_B ? stdout : nullptr);
			zloop_set_verbose(mpEventReactorLoop_X, _Verbose_B);
			if (_NonStop_B)  //This is not a bug, see zloop_set_nonstop implementation code
			{
				zloop_set_nonstop(mpEventReactorLoop_X, _NonStop_B);
			}
			Sts_i = zloop_start(mpEventReactorLoop_X);      //Returns 0 if interrupted, -1 if cancelled by a handler.
			Rts_E = (Sts_i == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_CANCEL;
			return Rts_E;
		}
};

static int S_EventReactorTimerCallback(zloop_t * /*_pLoop_X*/, int /*_TimerId_i*/, void *_pContext)
{
	int Rts_i = -1;
	BOF_EVENT_REACTOR_PARAM *pEventReactorTimerParam_X = reinterpret_cast<BOF_EVENT_REACTOR_PARAM *>(_pContext);
	if ((pEventReactorTimerParam_X) && (pEventReactorTimerParam_X->Magic_U32 == BOF_EVENT_REACTOR_MAGIC))
	{
		if ((pEventReactorTimerParam_X->EventType_E == BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_TIMER) ||
		    (pEventReactorTimerParam_X->EventType_E == BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_TICKET_TIMER))
		{
			if (pEventReactorTimerParam_X->EventType_E == BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_TICKET_TIMER)
			{
				//In this case this ticket timer will be delete by czmq->Signal it to RemoveTicketTimerEventGenerator to avoid assert in zloop_ticket_delete
				pEventReactorTimerParam_X->TimerDeleted_B = true;
			}
			if (pEventReactorTimerParam_X->ReactorCallback)
			{
				Rts_i = pEventReactorTimerParam_X->ReactorCallback(pEventReactorTimerParam_X->EventType_E, 0, pEventReactorTimerParam_X->pContext);
			}
		}
	}
	return Rts_i;
}

static int S_EventReactorPollCallback(zloop_t * /*_pLoop_X*/, zmq_pollitem_t *_pItem_X, void *_pContext)
{
	int Rts_i = -1;
	uint32_t Arg_U32;

	BOF_EVENT_REACTOR_PARAM *pEventReactorSocketParam_X = reinterpret_cast<BOF_EVENT_REACTOR_PARAM *>(_pContext);
	if ((pEventReactorSocketParam_X) && (pEventReactorSocketParam_X->Magic_U32 == BOF_EVENT_REACTOR_MAGIC))
	{
		if (pEventReactorSocketParam_X->EventType_E == BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_SOCKET)
		{
			if ((pEventReactorSocketParam_X->ReactorCallback) && (_pItem_X))
			{
				Arg_U32 = 0;
				if (_pItem_X->revents & ZMQ_POLLIN)
				{
					Arg_U32 |= static_cast<uint32_t>(BOF_POLL_FLAG::BOF_POLL_IN);
				}
				if (_pItem_X->revents & ZMQ_POLLOUT)
				{
					Arg_U32 |= static_cast<uint32_t>(BOF_POLL_FLAG::BOF_POLL_OUT);
				}
				if (_pItem_X->revents & ZMQ_POLLERR)
				{
					Arg_U32 |= static_cast<uint32_t>(BOF_POLL_FLAG::BOF_POLL_ERR);
				}
#if defined(ZMQ_POLLPRI)
			  if (_pItem_X->revents & ZMQ_POLLPRI)
				{
					Arg_U32 |= static_cast<uint32_t>(BOF_POLL_FLAG::BOF_POLL_PRI);
				}
#endif
				Rts_i = pEventReactorSocketParam_X->ReactorCallback(pEventReactorSocketParam_X->EventType_E, Arg_U32, pEventReactorSocketParam_X->pContext);
			}
		}
	}
	return Rts_i;
}

static int S_EventReactorZmqPollCallback(zloop_t * /*_pLoop_X*/, zsock_t *_pReader_X, void *_pContext)
{
	int Rts_i = -1;
	uint32_t Arg_U32;

	BOF_EVENT_REACTOR_PARAM *pEventReactorSocketParam_X = reinterpret_cast<BOF_EVENT_REACTOR_PARAM *>(_pContext);
	if ((pEventReactorSocketParam_X) && (pEventReactorSocketParam_X->Magic_U32 == BOF_EVENT_REACTOR_MAGIC))
	{
		if (pEventReactorSocketParam_X->EventType_E == BOF_EVENT_REACTOR_TYPE::BOF_EVENT_REACTOR_ZMQ)
		{
			if ((pEventReactorSocketParam_X->ReactorCallback) && (_pReader_X))
			{
				Arg_U32 = 0;
				Arg_U32 |= static_cast<uint32_t>(BOF_POLL_FLAG::BOF_POLL_IN);
				Rts_i = pEventReactorSocketParam_X->ReactorCallback(pEventReactorSocketParam_X->EventType_E, Arg_U32, pEventReactorSocketParam_X->pContext);
			}
		}
	}
	return Rts_i;
}

// Opaque pointer design pattern: ... set Implementation values ...
BofEventReactor::BofEventReactor(const std::string &_rName_S)
	: mpuEventReactorImplementation(new EventReactorImplementation(_rName_S))
{}


BofEventReactor::~BofEventReactor()
{}

BOFERR BofEventReactor::AddTimerEventGenerator(uint32_t _NbCycle_U32, uint32_t _CycleTimeoutInMs_U32, BofEventReactorFunction _ReactorCallback, void *_pContext, intptr_t &_rTimerId)
{
	return mpuEventReactorImplementation->AddTimerEventGenerator(_NbCycle_U32, _CycleTimeoutInMs_U32, _ReactorCallback, _pContext, _rTimerId);
}

BOFERR BofEventReactor::RemoveTimerEventGenerator(intptr_t _TimerId)
{
	return mpuEventReactorImplementation->RemoveTimerEventGenerator(_TimerId);
}

BOFERR BofEventReactor::SetGlobalTicketTimerCycleTimeOut(uint32_t _CycleTimeoutInMs_U32)
{
	return mpuEventReactorImplementation->SetGlobalTicketTimerCycleTimeOut(_CycleTimeoutInMs_U32);
}

BOFERR BofEventReactor::AddTicketTimerEventGenerator(BofEventReactorFunction _ReactorCallback, void *_pContext, intptr_t &_rTimerId)
{
	return mpuEventReactorImplementation->AddTicketTimerEventGenerator(_ReactorCallback, _pContext, _rTimerId);
}

BOFERR BofEventReactor::ResetTicketTimer(intptr_t _TimerId)
{
	return mpuEventReactorImplementation->ResetTicketTimer(_TimerId);
}

BOFERR BofEventReactor::RemoveTicketTimerEventGenerator(intptr_t _TimerId)
{
	return mpuEventReactorImplementation->RemoveTicketTimerEventGenerator(_TimerId);
}

BOFERR BofEventReactor::AddPollSocketEventGenerator(int _Socket_i, BOF_POLL_FLAG _PollFlag_E, BofEventReactorFunction _ReactorCallback, void *_pContext, intptr_t &_rPollId)
{
	return mpuEventReactorImplementation->AddPollSocketEventGenerator(_Socket_i, _PollFlag_E, _ReactorCallback, _pContext, _rPollId);
}

BOFERR BofEventReactor::ChangePollSocketEventGenerator(intptr_t _rPollId, BOF_POLL_FLAG _PollFlag_E)
{
	return mpuEventReactorImplementation->ChangePollSocketEventGenerator(_rPollId, _PollFlag_E);
}

BOFERR BofEventReactor::AddPollSocketEventGenerator(void *_pSocket, BOF_POLL_FLAG _PollFlag_E, BofEventReactorFunction _ReactorCallback, void *_pContext, intptr_t &_rPollId)
{
	return mpuEventReactorImplementation->AddPollSocketEventGenerator(_pSocket, _PollFlag_E, _ReactorCallback, _pContext, _rPollId);
}

BOFERR BofEventReactor::RemovePollEventGenerator(intptr_t _rPollId)
{
	return mpuEventReactorImplementation->RemovePollEventGenerator(_rPollId);
}

BOFERR BofEventReactor::StartEventReactorLoop(bool _Verbose_B, bool _NonStop_B)
{
	return mpuEventReactorImplementation->StartEventReactorLoop(_Verbose_B, _NonStop_B);
}

END_BOF_NAMESPACE()