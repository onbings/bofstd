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
 * Name:        ut_eventreactor_socket.cpp (socket)
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
#include <bofstd/bofactor.h>
#include "bofstd/iboflogchannel.h"

/*** Class *************************************************************************************************************************/

USE_BOF_NAMESPACE()
static uint32_t S_Start_U32 = 0;

const uint32_t FTP_SERVER_MAX_CLIENT = 50;
const std::string EVENT_REACTOR_START = "START";
const std::string EVENT_REACTOR_STOP = "STOP";

struct FTP_SERVER_CONTEXT
{
	int i;

	FTP_SERVER_CONTEXT()
	{
		Reset();
	}
	void Reset()
	{
		i = 0;
	}
};
class FtpServer final
{
private:
	BofEventReactor   mEventReactor;
	BofActor					*mpActor;
	BofSocket         mServerSocket;

public:
	FtpServer(BOF_SOCKET_ADDRESS _rAddress_X);
	~FtpServer();

	FtpServer(FtpServer const &) = delete;
	FtpServer(FtpServer &&) = delete; 
	FtpServer             & operator= (FtpServer const &) = delete;
	FtpServer             & operator= (FtpServer &&) = delete;

	std::string ActorDispatcher(const std::string &_rCommand_S, void *_pContext);
};

FtpServer::FtpServer(BOF_SOCKET_ADDRESS _rAddress_X) : mEventReactor("FtpServerReactor")	//, mActor("FtpServerActor")
{
	BOF_SOCKET_PARAM BofSocketParam_X;
	BOFERR Sts_E;
	FTP_SERVER_CONTEXT FtpServerContext_X;

	BofSocketParam_X.BaseChannelParam_X.ChannelName_S = "Listen";
	BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = FTP_SERVER_MAX_CLIENT;	// (FTP_SERVER_MAX_CLIENT > 16) ? 16 : FTP_SERVER_MAX_CLIENT;
	BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x1000;
	BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x1000;

	BofSocketParam_X.BindIpAddress_S = Bof_IpAddressToString(_rAddress_X,true,true);
	BofSocketParam_X.ReUseAddress_B = true;
	BofSocketParam_X.NoDelay_B = true;
	BofSocketParam_X.Ttl_U32 = 32;
	BofSocketParam_X.BroadcastPort_U16 = 0;
	BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
	BofSocketParam_X.MulticastSender_B = false;
	BofSocketParam_X.KeepAlive_B = false;
	BofSocketParam_X.EnableLocalMulticast_B = false;
	Sts_E = mServerSocket.InitializeSocket(BofSocketParam_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_NE(mServerSocket.GetSocketHandle(), (SOCKET)0);
	EXPECT_EQ(mServerSocket.GetMaxUdpLen(), (uint32_t)0);
	EXPECT_TRUE(mServerSocket.IsTcp());
	EXPECT_FALSE(mServerSocket.IsUdp());
	EXPECT_EQ(mServerSocket.LastErrorCode(), BOF_ERR_NO_ERROR);

	mpActor=new BofActor("FtpServerActor", BOF_BIND_2_ARG_TO_METHOD(FtpServer::ActorDispatcher), &FtpServerContext_X);
}

FtpServer::~FtpServer()
{

}

std::string FtpServer::ActorDispatcher(const std::string &_rCommand_S, void * /*_pContext*/)
{
	std::string Rts_S;

	if (_rCommand_S == EVENT_REACTOR_START)
	{
		mEventReactor.StartEventReactorLoop(false, true);
		Rts_S = EVENT_REACTOR_STOP;
	}
	return Rts_S;
}
/*
BOFERR Start()
{
	return(StartEventReactorLoop(false, true));
}

std::string EventReactor(const std::string & _rCommand_S, void *_pContext)
{
	std::string				Rts_S;
	BOFERR						Sts_E;
	BofEventReactor   *pEventReactor = reinterpret_cast<BofEventReactor *>(_pContext);

	if (_rCommand_S == EVENT_REACTOR_START)
	{
		Sts_E = pSocketContext_X->pEventReactor->StartEventReactorLoop(false, true);
		EXPECT_EQ(Sts_E, BOF_ERR_CANCEL);
	}
	return Rts_S;
}
*/

struct SOCKET_CONTEXT
{
	BofEventReactor    *pEventReactor;
	intptr_t PollId;
	BofSocket          *pServer;
	BofSocket          *pClient;
	std::string        Target_S;
};

struct CONNECTION_CONTEXT
{
	BofComChannel *pClient;
	BofEventReactor    *pEventReactor;
	intptr_t PollId;


	CONNECTION_CONTEXT()
	{
		Reset();
	}
	~CONNECTION_CONTEXT() {}
	CONNECTION_CONTEXT(CONNECTION_CONTEXT const &) = delete;
	CONNECTION_CONTEXT &operator=(CONNECTION_CONTEXT const &) = delete;
	CONNECTION_CONTEXT &operator=(CONNECTION_CONTEXT &&_rrOther) NOEXCEPT
	{
		if (this != &_rrOther)
		{
			pClient = _rrOther.pClient;
			pEventReactor = _rrOther.pEventReactor;
			PollId = _rrOther.PollId;
		}
		return *this;
	}
		CONNECTION_CONTEXT(CONNECTION_CONTEXT &&_rrOther) NOEXCEPT : pClient(_rrOther.pClient), pEventReactor(_rrOther.pEventReactor), PollId(_rrOther.PollId) {}


	void Reset()
	{
		pEventReactor = nullptr;
		PollId = 0;
		pClient = nullptr;
	}

};
static std::vector < CONNECTION_CONTEXT> S_ListOfClient;

int SocketIoReactorCallback(BOF_EVENT_REACTOR_TYPE /*_EventReactorType_E*/, uint32_t _Arg_U32, void *_pContext)
{
	int                              Rts_i;
	uint32_t                         Delta_U32, Nb_U32, i_U32;
	uint8_t                          pBuffer_U8[0x100];
	CONNECTION_CONTEXT                   *pConContext_X = reinterpret_cast<CONNECTION_CONTEXT *> (_pContext),*pConnection;
	BOFERR                           Sts_E;
	//BofSocket                        *pClient;
	//std::string                      IpSrc_S, IpDst_S;



	Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
	Rts_i = 0;




	printf("---- SocketIoReactorCallback ---->Io Flag %X p %p%s", _Arg_U32, _pContext, Bof_Eol());


	/*


	std::vector<int> l = { 0, 1, 2, 3, 4, 5, 6 };
	uint32_t i_U32;

	for (i_U32 = 0; i_U32 < l.size();i_U32++)
	{
	if (i_U32 == 2)
	{
	l.erase(l.begin() + 2);
	}
	printf("%d: %d\r\n", i_U32, l[i_U32]);

	if (i_U32==3)
	{
	l.erase(l.begin() + 5);
	}
	}


	 */

	pConnection = nullptr;
	for (i_U32 = 0; i_U32 < S_ListOfClient.size();i_U32++)
	{
		if (S_ListOfClient[i_U32].PollId == pConContext_X->PollId)
		{
			pConnection = &S_ListOfClient[i_U32];
			break;
		}
	}
	EXPECT_TRUE(pConnection != nullptr);


	if (_Arg_U32 & static_cast<uint32_t>(BOF_POLL_FLAG::BOF_POLL_OUT))
	{
		strcpy(reinterpret_cast<char *> (pBuffer_U8), "Hello World from client session !");
		Nb_U32 = static_cast<uint32_t>( strlen(reinterpret_cast<char *> (pBuffer_U8)) + 1);
		Sts_E = pConnection->pClient->V_WriteData(100, Nb_U32, pBuffer_U8);
		printf("CLIENT Delta %d Send '%d:%s' Sts %d%s", Delta_U32, Nb_U32,reinterpret_cast<char *>(pBuffer_U8), Sts_E, Bof_Eol());
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

		Sts_E = pConnection->pEventReactor->ChangePollSocketEventGenerator(pConnection->PollId, BOF_POLL_FLAG::BOF_POLL_IN);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	}

	if (_Arg_U32 & static_cast<uint32_t>(BOF_POLL_FLAG::BOF_POLL_IN))
	{
		pBuffer_U8[0] = 0;
		Nb_U32 = sizeof(pBuffer_U8);
		Sts_E = pConnection->pClient->V_ReadData(100, Nb_U32, pBuffer_U8);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

		Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
		printf("CLIENT Delta %d Rcv '%d:%s' Sts %d%s", Delta_U32, Nb_U32, reinterpret_cast<char *>(pBuffer_U8), Sts_E, Bof_Eol());
	}
	return Rts_i;
}

int SocketConnectReactorCallback(BOF_EVENT_REACTOR_TYPE /*_EventReactorType_E*/, uint32_t /*_Arg_U32*/, void *_pContext)
{
	int                              Rts_i;
	SOCKET_CONTEXT                   *pSocketContext_X = reinterpret_cast< SOCKET_CONTEXT * > (_pContext);
	BOFERR                           Sts_E;
	BofComChannel									*pChannel;
	BofSocket                        *pClient;
	//std::string IpSrc_S, IpDst_S;
	//uint32_t                         Delta_U32;



	CONNECTION_CONTEXT	*pConContext_X;

//	Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
	Rts_i     = 0;
	pChannel = pSocketContext_X->pServer->V_Listen(0, "");
	EXPECT_TRUE(pChannel != nullptr);
	if (pChannel)
	{
	//	Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
		// pClient = dynamic_cast<BofSocket *>(puChannel);
		pClient   = dynamic_cast< BofSocket * > (pChannel);
		// puSocket.reset(dynamic_cast<BofSocket *>(puChannel.get()));
		//IpSrc_S=Bof_IpAddressToString(pClient->GetSrcIpAddress(), true, true);
		//IpDst_S=Bof_IpAddressToString(pClient->GetDstIpAddress(), true, true);

		pConContext_X = new CONNECTION_CONTEXT();
		if (pConContext_X)
		{
			pConContext_X->pEventReactor = pSocketContext_X->pEventReactor;
			pConContext_X->pClient = pChannel;
			Sts_E = pSocketContext_X->pEventReactor->AddPollSocketEventGenerator(pClient->GetSocketHandle(), BOF_POLL_FLAG::BOF_POLL_IN | BOF_POLL_FLAG::BOF_POLL_OUT, SocketIoReactorCallback, pConContext_X, pConContext_X->PollId);
			EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
			S_ListOfClient.push_back(std::move(*pConContext_X));
		}
	}
	return Rts_i;
}

std::string EventReactor(const std::string & _rCommand_S, void * /*_pContext*/)
{
	std::string				Rts_S;
	BOFERR						Sts_E;
//	BofEventReactor   *pEventReactor = reinterpret_cast<BofEventReactor *>(_pContext);

	if (_rCommand_S == EVENT_REACTOR_START)
	{
//		Sts_E = pSocketContext_X->pEventReactor->StartEventReactorLoop(false, true);
		EXPECT_EQ(Sts_E, BOF_ERR_CANCEL);
	}
	return Rts_S;
}

std::string ClientActor(const std::string & _rCommand_S, void *_pContext)
{
	std::string    Rts_S;
	SOCKET_CONTEXT *pSocketContext_X = reinterpret_cast< SOCKET_CONTEXT * > (_pContext);
	uint32_t       Delta_U32, Nb_U32;
	uint8_t        pBuffer_U8[0x100];
	BOFERR         Sts_E;
	std::string    IpSrc_S, IpDst_S;

	if (_rCommand_S == "CONNECT")
	{
		Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
		printf("Try to connect to %s Delta %d%s", pSocketContext_X->Target_S.c_str(), Delta_U32, Bof_Eol() );
		Sts_E     = pSocketContext_X->pClient->V_Connect(0,pSocketContext_X->Target_S, "");
		if (Sts_E != BOF_ERR_NO_ERROR)
		{
			Rts_S = "CONNECT ERR";
		}
		else
		{
			Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
			IpSrc_S=Bof_IpAddressToString(pSocketContext_X->pClient->GetSrcIpAddress(), true, true);
			IpDst_S=Bof_IpAddressToString(pSocketContext_X->pClient->GetDstIpAddress(), true, true);
			printf("CONNECT Delta %d Connected %s->%s -> Sts %d%s", Delta_U32, IpSrc_S.c_str(), IpDst_S.c_str(), Sts_E, Bof_Eol() );
			Rts_S     = "CONNECT OK";
		}
	}
	else if (_rCommand_S == "READ")
	{
		pBuffer_U8[0] = 0;
		Nb_U32        = sizeof(pBuffer_U8);
		Sts_E         = pSocketContext_X->pClient->V_ReadData(100, Nb_U32, pBuffer_U8);
		Delta_U32     = Bof_ElapsedMsTime(S_Start_U32);
		printf("READ Delta %d Rcv Hello:'%d:%s' -> Sts %d%s", Delta_U32, Nb_U32, reinterpret_cast< char * > (pBuffer_U8), Sts_E, Bof_Eol() );
		Rts_S         = "READ OK";
	}
	else if (_rCommand_S == "WRITE")
	{
		strcpy(reinterpret_cast< char * > (pBuffer_U8), "Hello World !");
		Nb_U32    = static_cast<uint32_t>(strlen(reinterpret_cast< char * > (pBuffer_U8) ) + 1);
		Sts_E     = pSocketContext_X->pClient->V_WriteData(100, Nb_U32, pBuffer_U8);
		Delta_U32 = Bof_ElapsedMsTime(S_Start_U32);
		printf("WRITE Delta %d Snd Hello:'%d:%s' -> Sts %d%s", Delta_U32, Nb_U32, reinterpret_cast< char * > (pBuffer_U8), Sts_E, Bof_Eol() );
		Rts_S     = "WRITE OK";
	}
	return (Rts_S);
}

#include <czmq.h>
void V_Listen(char *p)
{
	zsock_t *frontend = zsock_new(ZMQ_PAIR);
	printf("create listen %p lvl %d\r\n", static_cast<void *>(frontend), static_cast<uint32_t>(zsys_pipehwm()));
	assert(frontend);
	zsock_set_sndhwm(frontend, 1);	// (int)zsys_pipehwm());
	char endpoint[256];
	sprintf(endpoint, "inproc://%s", p);
	sprintf(endpoint, "tcp://127.0.0.1:5000");
	int rc = zsock_bind(frontend, "%s", endpoint);
	printf("bind to %s sts %d\r\n", endpoint, rc);
	assert(rc == 0);
	printf("Wait for msg\r\n");
	char *string = zstr_recv(frontend);
	printf("Msg rcv %s\r\n",string);
	free(string);
	zsock_destroy(&frontend);
}

void V_Connect(char *p)
{
	zsock_t *backend = zsock_new(ZMQ_PAIR);
	printf("create connect %p lvl %d\r\n", static_cast<void *>(backend), static_cast<uint32_t>(zsys_pipehwm()));
	assert(backend);

	zsock_set_sndhwm(backend, (int)zsys_pipehwm());
	//  Now bind and connect pipe ends
	char endpoint[256];
	sprintf(endpoint, "inproc://%s", p);
	sprintf(endpoint, "tcp://127.0.0.1:5000");
	int rc = zsock_connect(backend, "%s", endpoint);
	printf("connect to %s sts %d\r\n", endpoint, rc);
	assert(rc != -1);          //  Connect cannot fail
	for (uint32_t i_U32 = 0; i_U32 < 10; i_U32++)
	{
		rc = zstr_send(backend, "Hello");
		printf("send msg sts %d\r\n", rc);
	}
	Bof_Sleep(100000);
	zsock_destroy(&backend);
}

/*
Status:	Disconnected from server
Status:	Connecting to 10.129.171.21:21...
Status:	Connection established, waiting for welcome message...
Response:	220 EVS FTP Server (v) 10.09 (mc) 150212 (d) 04/07/2012 (a) B.Harmel [B: 2/8 MB L:10.129.171.21:21 R:10.129.171.30:63696]
Command:	AUTH TLS
Response:	502 Command not implemented.
Command:	AUTH SSL
Response:	502 Command not implemented.
Status:	Insecure server, it does not support FTP over TLS.
Command:	USER evs
Response:	331 User name okay, need password.
Command:	PASS ****
Response:	230 User logged in, proceed.
Status:	Server does not support non-ASCII characters.
Status:	Logged in
Status:	Retrieving directory listing...
Command:	PWD
Response:	257 Remote directory now "X:\"
Command:	TYPE I
Response:	200 Type set to "I"
Command:	PASV
Response:	227 Entering Passive Mode (10,129,171,21,161,202).
Command:	LIST
Response:	150 File status okay; about to open data connection.
Response:	226 Transfer complete, Closing data connection.
Status:	Directory listing of "X:\" successful
Command:	QUIT
Response:	221 Service closing control connection.
Status:	Connection closed by server
 */
TEST(SocketEventReactor_Test, SocketConnectEvent)
{
	BOFERR           Sts_E;
	intptr_t         PollId;
//	uint32_t         Delta_U32, MaxTime_U32,CmdTicket_U32,ReplyTicket_U32;
	BofSocket        ServerSocket;
	BofSocket        ClientSocket;
	SOCKET_CONTEXT   SocketContext_X;
	std::string      Reply_S;
	BOF_SOCKET_PARAM BofSocketParam_X;
	BofEventReactor  EventReactor("MyBofEventReactor");
	BOF_THREAD EventReactorThread_X;

	BofSocketParam_X.Reset();
	BofSocketParam_X.BaseChannelParam_X.ChannelName_S     = "Listen";
	BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 30;
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
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_NE(ServerSocket.GetSocketHandle(), (SOCKET)0);
	EXPECT_EQ(ServerSocket.GetMaxUdpLen(), (uint32_t)0);
	EXPECT_TRUE(ServerSocket.IsTcp() );
	EXPECT_FALSE(ServerSocket.IsUdp() );
	EXPECT_EQ(ServerSocket.LastErrorCode(), BOF_ERR_NO_ERROR);

	SocketContext_X.pServer                               = &ServerSocket;
	SocketContext_X.Target_S                              = BofSocketParam_X.BindIpAddress_S;

	BofSocketParam_X.Reset();
	BofSocketParam_X.BaseChannelParam_X.ChannelName_S     = "Client";
	BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
	BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x10000;
	BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x10000;

#if defined (_WIN32)
	BofSocketParam_X.BindIpAddress_S                      = "tcp://10.129.170.30:"+std::to_string(Bof_Random(false,5556,5600));
#else
	BofSocketParam_X.BindIpAddress_S = "tcp://10.129.170.21:" + std::to_string(Bof_Random(false, 5556, 5600));
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
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_NE(ClientSocket.GetSocketHandle(), (SOCKET)0);
	EXPECT_EQ(ClientSocket.GetMaxUdpLen(), (uint32_t)0);
	EXPECT_TRUE(ClientSocket.IsTcp() );
	EXPECT_FALSE(ClientSocket.IsUdp() );
	EXPECT_EQ(ClientSocket.LastErrorCode(), BOF_ERR_NO_ERROR);
	SocketContext_X.pClient                               = &ClientSocket;

	SocketContext_X.pEventReactor                         = &EventReactor;
	Sts_E = EventReactor.AddPollSocketEventGenerator(ServerSocket.GetSocketHandle(), BOF_POLL_FLAG::BOF_POLL_IN | BOF_POLL_FLAG::BOF_POLL_OUT, SocketConnectReactorCallback, &SocketContext_X, PollId);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

//	Sts_E       = Bof_CreateThread("EventReactor", EventReactorThread, &SocketContext_X, EventReactorThread_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	S_Start_U32 = Bof_GetMsTickCount();

	//Sts_E       = Bof_LaunchThread(EventReactorThread_X, 0, 0, BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF_THREAD_DEFAULT_PRIORITY, 1000);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	BofActor   Client("Client", ClientActor, &SocketContext_X);
#if defined(CLIENTPRESENT)
	Sts_E       = Client.SendCommandToActor("CONNECT",CmdTicket_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);


	Sts_E = Client.WaitForReplyFromActor(Reply_S, ReplyTicket_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_STREQ(Reply_S.c_str(), "CONNECT OK");
	EXPECT_EQ(CmdTicket_U32, ReplyTicket_U32);

	Bof_Sleep(100);

	Sts_E = Client.SendCommandToActor("READ", CmdTicket_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	Sts_E = Client.WaitForReplyFromActor(Reply_S, ReplyTicket_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_STREQ(Reply_S.c_str(), "READ OK");
	EXPECT_EQ(CmdTicket_U32, ReplyTicket_U32);

	Sts_E = Client.SendCommandToActor("WRITE", CmdTicket_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	Sts_E = Client.WaitForReplyFromActor(Reply_S, ReplyTicket_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_STREQ(Reply_S.c_str(), "WRITE OK");
	EXPECT_EQ(CmdTicket_U32, ReplyTicket_U32);
	/*
	   BofActor ServerActor("Server", SocketServerActor, &SocketTimerContext_X);
	   BofActor ClientActor("Client", SocketClientActor, &SocketTimerContext_X);


	   Sts_E = ServerActor.SendMessageToActor("LISTEN", Reply_S);
	   EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	   Bof_Sleep(100);
	   Sts_E = ClientActor.SendMessageToActor("CONNECT", Reply_S);
	   EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	 */

// Sts_E = EventReactor.StartEventReactorLoop(true, true);
// EXPECT_EQ(Sts_E, BOF_ERR_CANCEL);

//	Bof_Sleep(1000000);


	Delta_U32   = Bof_ElapsedMsTime(S_Start_U32);
	EXPECT_GT(Delta_U32, MaxTime_U32);
	EXPECT_LT(Delta_U32, MaxTime_U32 * 1.2);

	Sts_E       = Bof_DestroyThread(EventReactorThread_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	/*
	   Sts_E = Bof_DestroyThread(ClientThread_X);
	   EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	   Sts_E = Bof_DestroyThread(ServerThread_X);
	   EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	 */
#endif

}