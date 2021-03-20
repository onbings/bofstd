/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofactor class (timer)
 *
 * Name:        ut_eventreactor_timer.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */

/*** Include files ***********************************************************/

#include "gtestrunner.h"
#include <bofstd/bofstd.h>
#include <bofstd/bofsystem.h>
#include <bofstd/bofeventreactor.h>
#include <bofstd/bofsocket.h>

/*** Class *************************************************************************************************************************/

USE_BOF_NAMESPACE()

const uint32_t  NBTIMER         = 10;
const uint32_t  NBCYCLEMIN      = 10;
const uint32_t  NBCYCLEMAX      = 50;
const uint32_t  CYCLETIMEMIN    = 10;
const uint32_t  CYCLETIMEMAX    = 50;
const uint32_t  LONGCYCLETIME   = 1000;

static uint32_t S_Start_U32     = 0;
static uint32_t S_NbCall_U32    = 0;
static uint32_t S_NbCallMax_U32 = 0;
struct TICKET_TIMER_CONTEXT
{
	BofEventReactor    *pEventReactor;
	intptr_t           TimerId;
	uint32_t           ResetCpt_U32;
};

int TicketResetReactorCallback(BOF_EVENT_REACTOR_TYPE /*_EventReactorType_E*/, uint32_t /*_Arg_U32*/, void *_pContext)
{
	int                  Rts_i                  = 0;
	//uint32_t             Delta_U32;
	TICKET_TIMER_CONTEXT *pTicketTimerContext_X = reinterpret_cast< TICKET_TIMER_CONTEXT * > (_pContext);
	BOFERR               Sts_E;

	//Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
	if (pTicketTimerContext_X->ResetCpt_U32)
	{
		pTicketTimerContext_X->ResetCpt_U32--;
		Sts_E = pTicketTimerContext_X->pEventReactor->ResetTicketTimer(pTicketTimerContext_X->TimerId);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
// printf("Reset Ticket %X Cpt %d Delta %d Sts %d%s", pTicketTimerContext_X->TimerId, pTicketTimerContext_X->ResetCpt_U32, Delta_U32, Sts_E, Bof_Eol());
	}
	else
	{
// printf("DO NOT Reset Ticket %X Delta %d%s", pTicketTimerContext_X->TimerId, Delta_U32, Bof_Eol());
	}
	return Rts_i;
}

int TimerReactorCallback(BOF_EVENT_REACTOR_TYPE /*_EventReactorType_E*/, uint32_t /*_Arg_U32*/, void * /*_pContext*/)
{
	int      Rts_i;
//	uint32_t Delta_U32;

	//Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
	S_NbCall_U32++;
	Rts_i     = (S_NbCall_U32 >= S_NbCallMax_U32) ? -1 : 0;
// printf("%d: Delta %d Event Type %d pContext %p->%d%s", S_NbCall_U32, Delta_U32, _EventReactorType_E, _pContext, Rts_i, Bof_Eol());

	return Rts_i;
}


TEST(TimerEventReactor_Test, TimerEvent)
{
	BOFERR          Sts_E;
	intptr_t        pTimerId[NBTIMER];
	uint32_t        Delta_U32, Time_U32, MaxTime_U32, i_U32, NbCycle_U32, CycleTime_U32;

	BofEventReactor EventReactor("MyBofEventReactor");

	S_NbCallMax_U32 = 0;
	MaxTime_U32     = 0;
	for (i_U32 = 0 ; i_U32 < NBTIMER ; i_U32++)
	{
		NbCycle_U32      = Bof_Random(false, NBCYCLEMIN, NBCYCLEMAX);
		CycleTime_U32    = Bof_Random(false, CYCLETIMEMIN, CYCLETIMEMAX);
		Sts_E            = EventReactor.AddTimerEventGenerator(NbCycle_U32, CycleTime_U32, TimerReactorCallback, this, pTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
		S_NbCallMax_U32 += NbCycle_U32;
		Time_U32         = (NbCycle_U32 * CycleTime_U32);
		if (Time_U32 > MaxTime_U32)
		{
			MaxTime_U32 = Time_U32;
		}
	}

	S_NbCall_U32    = 0;
	S_Start_U32     = Bof_GetMsTickCount();
	Sts_E           = EventReactor.StartEventReactorLoop(false, true);
	EXPECT_EQ(Sts_E, BOF_ERR_CANCEL);
	//Bof_Sleep(100);
	Delta_U32       = Bof_ElapsedMsTime(S_Start_U32);
	EXPECT_GE(Delta_U32, MaxTime_U32);
	EXPECT_LT(Delta_U32, MaxTime_U32 * 1.3);

	for (i_U32 = 0 ; i_U32 < NBTIMER ; i_U32++)
	{
		Sts_E = EventReactor.ResetTicketTimer(pTimerId[i_U32]);
		EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

		Sts_E = EventReactor.RemoveTicketTimerEventGenerator(pTimerId[i_U32]);
		EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

		Sts_E = EventReactor.RemoveTimerEventGenerator(pTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	}

}

TEST(TimerEventReactor_Test, TicketTimerEventNoReset)
{
	BOFERR          Sts_E;
	intptr_t        pTimerId[NBTIMER];
	uint32_t        Delta_U32, MaxTime_U32, i_U32;

	BofEventReactor EventReactor("MyBofEventReactor");

	Sts_E           = EventReactor.SetGlobalTicketTimerCycleTimeOut(CYCLETIMEMAX);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	for (i_U32 = 0 ; i_U32 < NBTIMER ; i_U32++)
	{
		Sts_E = EventReactor.AddTicketTimerEventGenerator(TimerReactorCallback, this, pTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	}
	MaxTime_U32     = CYCLETIMEMAX;
	S_NbCallMax_U32 = NBTIMER;
	S_NbCall_U32    = 0;
	S_Start_U32     = Bof_GetMsTickCount();
	Sts_E           = EventReactor.StartEventReactorLoop(false, true);
	EXPECT_EQ(Sts_E, BOF_ERR_CANCEL);

	Delta_U32       = Bof_ElapsedMsTime(S_Start_U32);
	EXPECT_GE(Delta_U32, MaxTime_U32);
	EXPECT_LT(Delta_U32, MaxTime_U32 * 1.3);

	for (i_U32 = 0 ; i_U32 < NBTIMER ; i_U32++)
	{
		Sts_E = EventReactor.RemoveTimerEventGenerator(pTimerId[i_U32]);
		EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

		Sts_E = EventReactor.RemoveTicketTimerEventGenerator(pTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	}

}

TEST(TimerEventReactor_Test, TicketTimerEventReset)
{
	BOFERR               Sts_E;
	TICKET_TIMER_CONTEXT pTicketTimer_X[NBTIMER];
	intptr_t             pTimerId[NBTIMER], pTicketTimerId[NBTIMER];;
	uint32_t             Delta_U32, MaxTime_U32, i_U32;

	BofEventReactor      EventReactor("MyBofEventReactor");

	Sts_E           = EventReactor.SetGlobalTicketTimerCycleTimeOut(LONGCYCLETIME);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	for (i_U32 = 0 ; i_U32 < NBTIMER ; i_U32++)
	{
		Sts_E = EventReactor.AddTicketTimerEventGenerator(TimerReactorCallback, this, pTicketTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	}

	for (i_U32 = 0 ; i_U32 < NBTIMER ; i_U32++)
	{
		pTicketTimer_X[i_U32].ResetCpt_U32  = i_U32;
		pTicketTimer_X[i_U32].pEventReactor = &EventReactor;
		pTicketTimer_X[i_U32].TimerId       = pTicketTimerId[i_U32];
		Sts_E                               = EventReactor.AddTimerEventGenerator(0, LONGCYCLETIME / 10, TicketResetReactorCallback, &pTicketTimer_X[i_U32], pTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	}

	MaxTime_U32     = LONGCYCLETIME + ( (NBTIMER - 1) * (LONGCYCLETIME / 10) );
	S_NbCallMax_U32 = NBTIMER;
	S_NbCall_U32    = 0;
	S_Start_U32     = Bof_GetMsTickCount();
	Sts_E           = EventReactor.StartEventReactorLoop(false, true);
	EXPECT_EQ(Sts_E, BOF_ERR_CANCEL);

	Delta_U32       = Bof_ElapsedMsTime(S_Start_U32);
	EXPECT_GT(Delta_U32, MaxTime_U32);
	EXPECT_LT(Delta_U32, MaxTime_U32 * 1.2);

	for (i_U32 = 0 ; i_U32 < NBTIMER ; i_U32++)
	{
		Sts_E = EventReactor.RemoveTimerEventGenerator(pTicketTimerId[i_U32]);
		EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

		Sts_E = EventReactor.RemoveTicketTimerEventGenerator(pTimerId[i_U32]);
		EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

		Sts_E = EventReactor.RemoveTimerEventGenerator(pTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

		Sts_E = EventReactor.RemoveTicketTimerEventGenerator(pTicketTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	}
}
