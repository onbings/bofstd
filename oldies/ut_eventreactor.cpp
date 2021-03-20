/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofactor class
 *
 * Name:        ut_eventreactor.cpp
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
// #include <bofstd/bofactor.h>

/*** Class *************************************************************************************************************************/

USE_BOF_NAMESPACE()

const uint32_t  NBTIMER         = 10;
const uint32_t  NBCYCLEMIN      = 10;
const uint32_t  NBCYCLEMAX      = 50;
const uint32_t  CYCLETIMEMIN    = 10;
const uint32_t  CYCLETIMEMAX    = 50;
const uint32_t  LONGCYCLETIME   = 1000;
/*
   const uint32_t NBTIMER = 1;
   const uint32_t NBCYCLEMIN = 1;
   const uint32_t NBCYCLEMAX = 1;
   const uint32_t CYCLETIMEMIN = 10000;
   const uint32_t CYCLETIMEMAX = 10000;
 */
static uint32_t S_Start_U32     = 0;
static uint32_t S_NbCall_U32    = 0;
static uint32_t S_NbCallMax_U32 = 0;
struct TICKET_TIMER_CONTEXT
{
	BofEventReactor    *pEventReactor;
	intptr_t           TimerId;
	uint32_t           ResetCpt_U32;
};
struct SOCKET_TIMER_CONTEXT
{
	BofSocket      *pServer;
	BofSocket      *pClient;
	std::string    Target_S;
};
int TicketResetReactorCallback(BOF_EVENT_REACTOR_TYPE _EventReactorType_E, void *_pContext)
{
	int                  Rts_i                  = 0;
	uint32_t             Delta_U32;
	TICKET_TIMER_CONTEXT *pTicketTimerContext_X = reinterpret_cast< TICKET_TIMER_CONTEXT * > (_pContext);
	BOFERR               Sts_E                  = BOFERR_FINISHED;

	Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
	if (pTicketTimerContext_X->ResetCpt_U32)
	{
		pTicketTimerContext_X->ResetCpt_U32--;
		Sts_E = pTicketTimerContext_X->pEventReactor->ResetTicketTimer(pTicketTimerContext_X->TimerId);
		EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
// printf("Reset Ticket %X Cpt %d Delta %d Sts %d%s", pTicketTimerContext_X->TimerId, pTicketTimerContext_X->ResetCpt_U32, Delta_U32, Sts_E, Bof_Eol());
	}
	else
	{
// printf("DO NOT Reset Ticket %X Delta %d%s", pTicketTimerContext_X->TimerId, Delta_U32, Bof_Eol());
	}
	return Rts_i;
}

int ReactorCallback(BOF_EVENT_REACTOR_TYPE _EventReactorType_E, void *_pContext)
{
	int      Rts_i;
	uint32_t Delta_U32;

	Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
	S_NbCall_U32++;
	Rts_i     = (S_NbCall_U32 >= S_NbCallMax_U32) ? -1 : 0;
// printf("%d: Delta %d Event Type %d pContext %p->%d%s", S_NbCall_U32, Delta_U32, _EventReactorType_E, _pContext, Rts_i, Bof_Eol());

	return Rts_i;
}

void *SocketServerThread(const std::atomic< bool > & _rIsThreadLoopMustExit_B, std::atomic< bool > & _rIsThreadLoopRunning_B, void *_pContext)
{
	SOCKET_TIMER_CONTEXT             *pSocketTimerContext_X = reinterpret_cast< SOCKET_TIMER_CONTEXT * > (_pContext);
	uint32_t                         Delta_U32,Nb_U32;
	uint8_t pBuffer_U8[0x100];
	BOFERR               Sts_E;

	std::unique_ptr< BofComChannel > puChannel;
	std::string IpSrc_S, IpDst_S;
	BofSocket *pClient;
	//std::unique_ptr< BofSocket > puSocket;

	while ((!_rIsThreadLoopMustExit_B) && (_rIsThreadLoopRunning_B))
	{
		Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
		printf("Delta %d Enter listen mode%s", Delta_U32, Bof_Eol() );
		puChannel = pSocketTimerContext_X->pServer->V_Listen(1000, "");
		if (puChannel)
		{
			Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
			//pClient = dynamic_cast<BofSocket *>(puChannel);
			pClient = dynamic_cast<BofSocket *>(puChannel.get());
			//			puSocket.reset(dynamic_cast<BofSocket *>(puChannel.release()));
			Bof_IpAddressToString(pClient->GetSrcIpAddress(), true, true, IpSrc_S);
			Bof_IpAddressToString(pClient->GetDstIpAddress(), true, true, IpDst_S);

			strcpy(reinterpret_cast<char *>(pBuffer_U8), "Hello World !");
			Nb_U32 = strlen(reinterpret_cast<char *>(pBuffer_U8));
			Sts_E=pClient->V_WriteData(100, Nb_U32, pBuffer_U8);
			printf("Delta %d Listen create client %s->%s Send Hello %d%s", Delta_U32, IpSrc_S.c_str(), IpDst_S.c_str(),Sts_E, Bof_Eol());
			EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
		}
	}
	printf("Exit server thread%s", Bof_Eol());
	return nullptr;
}
void *SocketClientThread(const std::atomic< bool > & _rIsThreadLoopMustExit_B, std::atomic< bool > & _rIsThreadLoopRunning_B, void *_pContext)
{
	uint32_t             Delta_U32, Nb_U32;
	uint8_t pBuffer_U8[0x100];
	SOCKET_TIMER_CONTEXT *pSocketTimerContext_X = reinterpret_cast< SOCKET_TIMER_CONTEXT * > (_pContext);
	BOFERR               Sts_E;
	std::string IpSrc_S,IpDst_S;

	while ((!_rIsThreadLoopMustExit_B) && (_rIsThreadLoopRunning_B))
	{
		Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
		printf("Try to connect to %s Delta %d%s", pSocketTimerContext_X->Target_S.c_str(), Delta_U32, Bof_Eol() );
		Sts_E     = pSocketTimerContext_X->pClient->V_Connect( pSocketTimerContext_X->Target_S, "");
		if (Sts_E!=BOFERR_NO_ERROR)
		{
			Bof_Sleep(100);
		}
		else
		{
			Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
			Bof_IpAddressToString(pSocketTimerContext_X->pClient->GetSrcIpAddress(), true, true, IpSrc_S);
			Bof_IpAddressToString(pSocketTimerContext_X->pClient->GetDstIpAddress(), true, true, IpDst_S);

			pBuffer_U8[0] = 0;
			Nb_U32 = sizeof(pBuffer_U8);
			Sts_E = pSocketTimerContext_X->pClient->V_ReadData(100, Nb_U32, pBuffer_U8);
			printf("Delta %d Connected %s->%s Rcv Hello:'%s'%s", Delta_U32, IpSrc_S.c_str(), IpDst_S.c_str(), pBuffer_U8,Bof_Eol());
			_rIsThreadLoopRunning_B = false;
		}
	}
	printf("Exit client thread%s", Bof_Eol());
	return nullptr;
}


int SocketConnectReactorCallback(BOF_EVENT_REACTOR_TYPE _EventReactorType_E, void *_pContext)
{
	int                  Rts_i;
	uint32_t             Delta_U32;
	SOCKET_TIMER_CONTEXT *pSocketTimerContext_X = reinterpret_cast< SOCKET_TIMER_CONTEXT * > (_pContext);
	BOFERR               Sts_E;

	Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
	S_NbCall_U32++;
	Rts_i     = -1;                      // (S_NbCall_U32 >= S_NbCallMax_U32) ? -1 : 0;
	Sts_E     = pSocketTimerContext_X->pClient->V_Connect( pSocketTimerContext_X->Target_S, "");
	printf("%d: Try to connect to %s Delta %d Event Type %d pContext %p Sts %d->%d%s", S_NbCall_U32, pSocketTimerContext_X->Target_S.c_str(), Delta_U32, _EventReactorType_E, _pContext, Sts_E, Rts_i, Bof_Eol() );

	return Rts_i;
}

TEST(EventReactor_Test, TimerEvent)
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
		Sts_E            = EventReactor.AddTimerEventGenerator(NbCycle_U32, CycleTime_U32, ReactorCallback, this, pTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
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
	EXPECT_EQ(Sts_E, BOFERR_CANCEL);

	Delta_U32       = Bof_ElapsedMsTime(S_Start_U32);
	EXPECT_GT(Delta_U32, MaxTime_U32);
	EXPECT_LT(Delta_U32, MaxTime_U32 * 1.2);

	for (i_U32 = 0 ; i_U32 < NBTIMER ; i_U32++)
	{
		Sts_E = EventReactor.ResetTicketTimer(pTimerId[i_U32]);
		EXPECT_NE(Sts_E, BOFERR_NO_ERROR);

		Sts_E = EventReactor.RemoveTicketTimerEventGenerator(pTimerId[i_U32]);
		EXPECT_NE(Sts_E, BOFERR_NO_ERROR);

		Sts_E = EventReactor.RemoveTimerEventGenerator(pTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
	}

}

TEST(EventReactor_Test, TicketTimerEventNoReset)
{
	BOFERR          Sts_E;
	intptr_t        pTimerId[NBTIMER];
	uint32_t        Delta_U32, MaxTime_U32, i_U32;

	BofEventReactor EventReactor("MyBofEventReactor");

	Sts_E           = EventReactor.SetGlobalTicketTimerCycleTimeOut(CYCLETIMEMAX);
	EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);

	for (i_U32 = 0 ; i_U32 < NBTIMER ; i_U32++)
	{
		Sts_E = EventReactor.AddTicketTimerEventGenerator(ReactorCallback, this, pTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
	}
	MaxTime_U32     = CYCLETIMEMAX;
	S_NbCallMax_U32 = NBTIMER;
	S_NbCall_U32    = 0;
	S_Start_U32     = Bof_GetMsTickCount();
	Sts_E           = EventReactor.StartEventReactorLoop(false, true);
	EXPECT_EQ(Sts_E, BOFERR_CANCEL);

	Delta_U32       = Bof_ElapsedMsTime(S_Start_U32);
	EXPECT_GT(Delta_U32, MaxTime_U32);
	EXPECT_LT(Delta_U32, MaxTime_U32 * 1.2);

	for (i_U32 = 0 ; i_U32 < NBTIMER ; i_U32++)
	{
		Sts_E = EventReactor.RemoveTimerEventGenerator(pTimerId[i_U32]);
		EXPECT_NE(Sts_E, BOFERR_NO_ERROR);

		Sts_E = EventReactor.RemoveTicketTimerEventGenerator(pTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
	}

}

TEST(EventReactor_Test, TicketTimerEventReset)
{
	BOFERR               Sts_E;
	TICKET_TIMER_CONTEXT pTicketTimer_X[NBTIMER];
	intptr_t             pTimerId[NBTIMER], pTicketTimerId[NBTIMER];;
	uint32_t             Delta_U32, MaxTime_U32, i_U32;

	BofEventReactor      EventReactor("MyBofEventReactor");

	Sts_E           = EventReactor.SetGlobalTicketTimerCycleTimeOut(LONGCYCLETIME);
	EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);

	for (i_U32 = 0 ; i_U32 < NBTIMER ; i_U32++)
	{
		Sts_E = EventReactor.AddTicketTimerEventGenerator(ReactorCallback, this, pTicketTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
	}

	for (i_U32 = 0 ; i_U32 < NBTIMER ; i_U32++)
	{
		pTicketTimer_X[i_U32].ResetCpt_U32  = i_U32;
		pTicketTimer_X[i_U32].pEventReactor = &EventReactor;
		pTicketTimer_X[i_U32].TimerId       = pTicketTimerId[i_U32];
		Sts_E                               = EventReactor.AddTimerEventGenerator(0, LONGCYCLETIME / 10, TicketResetReactorCallback, &pTicketTimer_X[i_U32], pTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
	}

	MaxTime_U32     = LONGCYCLETIME + ( (NBTIMER - 1) * (LONGCYCLETIME / 10) );
	S_NbCallMax_U32 = NBTIMER;
	S_NbCall_U32    = 0;
	S_Start_U32     = Bof_GetMsTickCount();
	Sts_E           = EventReactor.StartEventReactorLoop(false, true);
	EXPECT_EQ(Sts_E, BOFERR_CANCEL);

	Delta_U32       = Bof_ElapsedMsTime(S_Start_U32);
	EXPECT_GT(Delta_U32, MaxTime_U32);
	EXPECT_LT(Delta_U32, MaxTime_U32 * 1.2);

	for (i_U32 = 0 ; i_U32 < NBTIMER ; i_U32++)
	{
		Sts_E = EventReactor.RemoveTimerEventGenerator(pTicketTimerId[i_U32]);
		EXPECT_NE(Sts_E, BOFERR_NO_ERROR);

		Sts_E = EventReactor.RemoveTicketTimerEventGenerator(pTimerId[i_U32]);
		EXPECT_NE(Sts_E, BOFERR_NO_ERROR);

		Sts_E = EventReactor.RemoveTimerEventGenerator(pTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);

		Sts_E = EventReactor.RemoveTicketTimerEventGenerator(pTicketTimerId[i_U32]);
		EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
	}
}
TEST(EventReactor_Test, SocketConnectEvent)
{
	BOFERR               Sts_E;
	intptr_t             PollId;
	uint32_t             Delta_U32, MaxTime_U32;
	BofSocket            ServerSocket;
	BofSocket            ClientSocket;
	SOCKET_TIMER_CONTEXT SocketTimerContext_X;
	std::string          Reply_S;

	BofEventReactor      EventReactor("MyBofEventReactor");


	BOF_SOCKET_PARAM     BofSocketParam_X;
	BofSocketParam_X.Reset();
	BofSocketParam_X.BaseChannelParam_X.ChannelName_S     = "Listen";
	BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 3;
	BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x1000;
	BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x1000;

#if defined (_WIN32)
	BofSocketParam_X.BindIpAddress_S                      = "tcp://10.129.170.30:5555";
#else
	BofSocketParam_X.BindIpAddress_S                      = "tcp://10.129.170.21:5555";
#endif
	BofSocketParam_X.ReUseAddress_B                       = true;
	BofSocketParam_X.NoDelay_B                            = true;
	BofSocketParam_X.Ttl_U32                              = 32;
	BofSocketParam_X.BroadcastPort_U16                    = 0;
	BofSocketParam_X.MulticastInterfaceIpAddress_S        = "";
	BofSocketParam_X.MulticastSender_B                    = false;
	BofSocketParam_X.KeepAlive_B                          = false;
	BofSocketParam_X.EnableLocalMulticast_B               = false;
	Sts_E = ServerSocket.InitializeSocket(BofSocketParam_X);
	EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
	EXPECT_NE(ServerSocket.GetSocketHandle(), (SOCKET)0);
	EXPECT_EQ(ServerSocket.GetMaxUdpLen(), (uint32_t)0);
	EXPECT_TRUE(ServerSocket.IsTcp() );
	EXPECT_FALSE(ServerSocket.IsUdp() );
	EXPECT_EQ(ServerSocket.InitErrorCode(), BOFERR_NO_ERROR);

	SocketTimerContext_X.pServer                          = &ServerSocket;
	SocketTimerContext_X.Target_S                         = BofSocketParam_X.BindIpAddress_S;

	BofSocketParam_X.Reset();
	BofSocketParam_X.BaseChannelParam_X.ChannelName_S     = "Client";
	BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
	BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x10000;
	BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x10000;

#if defined (_WIN32)
	BofSocketParam_X.BindIpAddress_S                      = "tcp://10.129.170.30:5554";
#else
	BofSocketParam_X.BindIpAddress_S                      = "tcp://10.129.170.21:5554";
#endif
	BofSocketParam_X.ReUseAddress_B                       = true;
	BofSocketParam_X.NoDelay_B                            = true;
	BofSocketParam_X.Ttl_U32                              = 32;
	BofSocketParam_X.BroadcastPort_U16                    = 0;
	BofSocketParam_X.MulticastInterfaceIpAddress_S        = "";
	BofSocketParam_X.MulticastSender_B                    = false;
	BofSocketParam_X.KeepAlive_B                          = false;
	BofSocketParam_X.EnableLocalMulticast_B               = false;
	Sts_E = ClientSocket.InitializeSocket(BofSocketParam_X);
	EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
	EXPECT_NE(ClientSocket.GetSocketHandle(), (SOCKET)0);
	EXPECT_EQ(ClientSocket.GetMaxUdpLen(), (uint32_t)0);
	EXPECT_TRUE(ClientSocket.IsTcp() );
	EXPECT_FALSE(ClientSocket.IsUdp() );
	EXPECT_EQ(ClientSocket.InitErrorCode(), BOFERR_NO_ERROR);
	SocketTimerContext_X.pClient                          = &ClientSocket;

	Sts_E = EventReactor.AddPollSocketEventGenerator(ServerSocket.GetSocketHandle(), SocketConnectReactorCallback, this, PollId);
	EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);


	BOF_THREAD ServerThread_X;
	Sts_E = Bof_CreateThread("Server", SocketServerThread, &SocketTimerContext_X, ServerThread_X);
	EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);

	BOF_THREAD ClientThread_X;
	Sts_E = Bof_CreateThread("Client", SocketClientThread, &SocketTimerContext_X, ClientThread_X);
	EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);

	S_Start_U32 = Bof_GetMsTickCount();

	Sts_E = Bof_LaunchThread(ServerThread_X, 0, 0, BOF_THREAD_POLICY_OTHER, BOF_THREAD_DEFAULT_PRIORITY, 1000);
	EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);

	Bof_Sleep(100);

	Sts_E = Bof_LaunchThread(ClientThread_X, 0, 0, BOF_THREAD_POLICY_OTHER, BOF_THREAD_DEFAULT_PRIORITY, 1000);
	EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);


	S_Start_U32 = Bof_GetMsTickCount();

	/*
	   BofActor ServerActor("Server", SocketServerActor, &SocketTimerContext_X);
	   BofActor ClientActor("Client", SocketClientActor, &SocketTimerContext_X);


	   Sts_E = ServerActor.SendMessageToActor("LISTEN", Reply_S);
	   EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);

	   Bof_Sleep(100);
	   Sts_E = ClientActor.SendMessageToActor("CONNECT", Reply_S);
	   EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
	 */

	Sts_E = EventReactor.StartEventReactorLoop(true, true);
	EXPECT_EQ(Sts_E, BOFERR_CANCEL);

	Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
	EXPECT_GT(Delta_U32, MaxTime_U32);
	EXPECT_LT(Delta_U32, MaxTime_U32 * 1.2);

	Sts_E = Bof_DestroyThread(ClientThread_X);
	EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);

	Sts_E = Bof_DestroyThread(ServerThread_X);
	EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);

}