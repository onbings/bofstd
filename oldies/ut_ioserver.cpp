/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the comchannel class
 *
 * Name:        ut_ioserver.cpp
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
#include <regex>

#include <bofstd/bofstd.h>
#include <bofstd/bofthread.h>
#include <bofstd/bofioconnectionmanager.h>
#include <bofstd/ibofioconnection.h>
#include <bofstd/ibofioconnectionfactory.h>

#include <bofstd/bofsystem.h>
#include <bofstd/bofstring.h>
#include <bofstd/bofparameter.h>
#include <bofstd/bofstringformatter.h>
#include <bofstd/bofqueue.h>
#include <bofstd/bofiocmdchannel.h>

/*** Class *************************************************************************************************************************/

const uint32_t TIMEOUT_IN_MS = 700000000;
const uint32_t TCP_BUFFER_SIZE = 0x10000;
const uint32_t UDP_MAX_BUFFER_SIZE = 0xFFFF - 8 - 20; //8 UDP header ? 20 byte IP header
const uint32_t DATA_FRAGMENT_SIZE = 0x400000;
//const uint32_t SERVER_PORT = 50101; //Linx udp port 1
const uint32_t SERVER_PORT = 50200; //Linx udp port 1
const uint32_t UDP_CLIENT_PORT = 50102; //Linx udp port 2
const uint32_t TCP_START_PORT = 50300;
const uint32_t TCP_STOP_PORT = 50500;
const uint32_t NBMAXCLIENTSESSION = 1; // 1;	// 4;  // 32; 
//const std::string SERVER_ADDRESS = "127.0.0.1:" + SERVER_PORT;
//const std::string MULTICAST_SERVER_ADDRESS = "udp://0.0.0.0:50600;239.193.0.1:50500";
//const std::string UDP_SERVER_ADDRESS = "udp://10.129.170.22:" + SERVER_PORT;
//const std::string UDP_CLIENT_ADDRESS = "udp://127.0.0.1:" + UDP_CLIENT_PORT;

int64_t ChooseRamdowFileSize()
{
	int64_t Rts_S64;

	switch (BOF_NAMESPACE::Bof_Random(true, 1, 4))
	{
		case 1:
			Rts_S64 = 2 * 1024LL * 1024LL * 1024LL;  //2GB
			Rts_S64 = BOF_NAMESPACE::Bof_Random(true, 100, 8 * 1024LL * 1024LL);
			break;

		case 2:
			Rts_S64 = 8 * 1024LL * 1024LL;  //8MB
			break;

		case 3:
			Rts_S64 = 1 * 1024LL * 1024LL;  //1MB
			break;

		case 4:
		default:
			Rts_S64 = BOF_NAMESPACE::Bof_Random(true, 100, 32 * 1024LL * 1024LL);
			Rts_S64 = BOF_NAMESPACE::Bof_Random(true, 100, 8 * 1024LL * 1024LL);
			break;

	}
	return Rts_S64;
}

static uint16_t S_NextPort_U16 = TCP_STOP_PORT;

int16_t ChooseRamdowPort()
{
	if (S_NextPort_U16 >= TCP_STOP_PORT)
	{
		S_NextPort_U16 = TCP_START_PORT;
	}
	else
	{
		S_NextPort_U16++;
	}
	return S_NextPort_U16;
}

TEST(IoServer_Test, RamBandwidth)
{
	BOFERR Sts_E;
	uint32_t Start_U32, Delta_U32, Nb_U32, i_U32, Len_U32;
	float DeltaInSec_f, BandwithInKB_f;
	uint64_t Total_U64, SizeUtillNow_U64;
	uint8_t pData_U8[TCP_BUFFER_SIZE], pFakeData_U8[sizeof(pData_U8) * 2];

	Nb_U32 = sizeof(pData_U8);
	for (i_U32 = 0; i_U32 < Nb_U32; i_U32++)
	{
		pData_U8[i_U32] = (i_U32 % 0x100) ? static_cast<uint8_t>(i_U32) : static_cast<uint8_t>(i_U32 / 0x100);
	}

	i_U32 = 0;
	Total_U64 = 2 * 1024LL * 1024LL * 1024LL;  //2GB
	SizeUtillNow_U64 = 0;
	Start_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
	while (SizeUtillNow_U64 < Total_U64)
	{
		Nb_U32 = static_cast<uint32_t>(((Total_U64 - SizeUtillNow_U64) < sizeof(pData_U8)) ? (Total_U64 - SizeUtillNow_U64) : sizeof(pData_U8));
		Len_U32 = Nb_U32;
		memcpy(&pFakeData_U8[i_U32 % sizeof(pData_U8)], pData_U8, Nb_U32);
		Sts_E = BOFERR_NO_ERROR;
		EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
		EXPECT_EQ(Len_U32, Nb_U32);
		i_U32++;
		SizeUtillNow_U64 += Nb_U32;
	}
	Delta_U32 = BOF_NAMESPACE::Bof_ElapsedMsTime(Start_U32);
	DeltaInSec_f = static_cast<float>(Delta_U32) / 1000.0f;
	BandwithInKB_f = DeltaInSec_f ? (static_cast<float>(Total_U64) / 1024.0f) / DeltaInSec_f : 0.0f;
	//printf("RAM: %lld/0x%llX bytes downloaded in %d ms->%f KB/sec %f MB/sec\n", Total_U64, Total_U64, Delta_U32, BandwithInKB_f, BandwithInKB_f / 1024.0f);
}

class IoSession : public BOF_NAMESPACE::BofIoCmdChannel
{
private:

public:
		IoSession(const BOF_NAMESPACE::IBOF_IO_CONNECTION_PARAM &_rIBofIoConnectionParam_X, BOF_NAMESPACE::BofIoConnectionManager *_pBofIoConnectionManager)
			: BOF_NAMESPACE::BofIoCmdChannel(_rIBofIoConnectionParam_X, _pBofIoConnectionManager)
		{
		};

		virtual ~IoSession()
		{
			//printf("==========>IoSession::~IoSession Delete %p\n", this);
		};
};

//TcpSession factory
class IoServer : public BOF_NAMESPACE::IBofIoConnectionFactory
{
private:
		BOF_NAMESPACE::BofIoConnectionManager *mpIoConnectionManager;

public:
		IoServer(BOF_NAMESPACE::BofIoConnectionManager *_pIoConnectionManager)
			: mpIoConnectionManager(_pIoConnectionManager)
		{
			BOF_ASSERT(mpIoConnectionManager != nullptr);
		}

		~IoServer()
		{
			mpIoConnectionManager->EndOfFactory(this);
		}

		//https://cpppatterns.com/patterns/rule-of-five.html
		IoServer(const IoServer &_rOther) = delete;

		IoServer(IoServer &&_rrOther) = delete;

		IoServer &operator=(const IoServer &_rOther) = delete;

		IoServer &operator=(IoServer &&_rrOther) = delete;

		BOFERR Listen(const BOF_NAMESPACE::IBOF_IO_CONNECTION_PARAM &_rIBofIoConnectionParam_X, const BOF_NAMESPACE::BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X, uint32_t _NbMaxConnection_U32,
		              BOF_NAMESPACE::IBofIoConnection **_ppIBofIoConnection)
		{
			BOFERR Rts_E;

			Rts_E = mpIoConnectionManager->Listen(this, _rIBofIoConnectionParam_X, _rBofIoChannelParam_X, _NbMaxConnection_U32, _ppIBofIoConnection);
			BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);
			return Rts_E;
		}

		BOF_NAMESPACE::IBofIoConnection *V_CreateSession(const BOF_NAMESPACE::BOF_IO_CHANNEL_INFO &_rBofIoChannelInfo_X, BOF_NAMESPACE::BofIoConnectionManager *_pIoConnectionManager,
		                                                 BOF_NAMESPACE::BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X) override
		{
			IoSession *pRts;
			BOF_NAMESPACE::IBOF_IO_CONNECTION_PARAM IBofIoConnectionParam_X;
			bool IsUdp_B = (_rBofIoChannelInfo_X.ProtocolType_E == BOF_NAMESPACE::BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP);

			BOF_ASSERT(_pIoConnectionManager != nullptr);
			if (mpIoConnectionManager)
			{
				BOF_ASSERT(_pIoConnectionManager == mpIoConnectionManager);    //Can't change
			}
			else
			{
				mpIoConnectionManager = _pIoConnectionManager;
			}
			_rBofIoChannelParam_X.RcvBufferSize_U32 = TCP_BUFFER_SIZE;
			_rBofIoChannelParam_X.SndBufferSize_U32 = TCP_BUFFER_SIZE;
//   _rBofIoChannelParam_X.NotifyRcvBufferSize_U32 = DATA_FRAGMENT_SIZE;
			if (IsUdp_B)
			{
				_rBofIoChannelParam_X.IO_CHANNEL.UDP.Broadcast_B = false;
				_rBofIoChannelParam_X.IO_CHANNEL.UDP.MulticastLoop_B = false;
				_rBofIoChannelParam_X.IO_CHANNEL.UDP.Ttl_U32 = false;
			}
			else
			{
				_rBofIoChannelParam_X.IO_CHANNEL.TCP.NoDelay_B = true;

			}
			IBofIoConnectionParam_X.Name_S = _rBofIoChannelInfo_X.Name_S;
			IBofIoConnectionParam_X.NotifyRcvBufferSize_U32 = DATA_FRAGMENT_SIZE;
			IBofIoConnectionParam_X.NotifyType_E = BOF_NAMESPACE::BOF_IO_NOTIFY_TYPE::ASAP;
			IBofIoConnectionParam_X.Delimiter_U8 = 0;
			IBofIoConnectionParam_X.pData = nullptr;
			pRts = new IoSession(IBofIoConnectionParam_X, mpIoConnectionManager);
			BOF_ASSERT(pRts != nullptr);
			return pRts;
		}

		BOFERR V_SessionDisconnected(BOF_NAMESPACE::IBofIoConnection *_pIoConnection) override
		{
//    IoServer *pIoServer = dynamic_cast<IoServer *>(_pIoConnection);
			IoSession *pIoSession = reinterpret_cast<IoSession *>(_pIoConnection);
			BOF_ASSERT(pIoSession != nullptr);
			BOF_SAFE_DELETE(pIoSession)
			return BOFERR_NO_ERROR;
		}
};

class IoClient : public BOF_NAMESPACE::BofIoCmdChannel
{
private:

public:
		IoClient(const BOF_NAMESPACE::IBOF_IO_CONNECTION_PARAM &_rIBofIoConnectionParam_X, BOF_NAMESPACE::BofIoConnectionManager *_pIoConnectionManager)
			: BOF_NAMESPACE::BofIoCmdChannel(_rIBofIoConnectionParam_X, _pIoConnectionManager)
		{
		}

		~IoClient()
		{
		}

		//https://cpppatterns.com/patterns/rule-of-five.html
		IoClient(const IoClient &_rOther) = delete;

		IoClient(IoClient &&_rrOther) = delete;

		IoClient &operator=(const IoClient &_rOther) = delete;

		IoClient &operator=(IoClient &&_rrOther) = delete;

};


static uint8_t S_pBuffer_U8[0x800000];

BOFERR FtpConnect(BOF_NAMESPACE::BofIoCmdChannel *_pFtpCmdChannel, uint32_t _Session_U32, const BOF_NAMESPACE::BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X)
{
	BOFERR Rts_E;
	std::string WelcomeMsg_S, Reply_S;
	uint32_t ReplyCode_U32;
	BOF_NAMESPACE::BOF_BUFFER NotifyBuffer_X;
	BOF_NAMESPACE::BOF_SOCKET_ADDRESS DataConnectToAddress_X;
//  bool ListenForConnection_B;

	Rts_E = _pFtpCmdChannel->Connect(_rBofIoChannelParam_X, WelcomeMsg_S);

	EXPECT_EQ(Rts_E, 0);
	EXPECT_STREQ(WelcomeMsg_S.c_str(), "220 EVS FTP Server (v) 09.08 (mc) 1606 (d) 26/05/64 (a) B.Harmel [B: 2/8 MB L:127.0.0.1:xxxx R:127.0.0.1:yyyy]");

	Rts_E = _pFtpCmdChannel->Login(TIMEOUT_IN_MS, "evs", "evs!");
	EXPECT_EQ(Rts_E, 0);

	Rts_E = _pFtpCmdChannel->SendIoCmdAndWaitForReply(TIMEOUT_IN_MS, "PWD\r\n", 257, Reply_S, ReplyCode_U32);
	EXPECT_EQ(Rts_E, 0);
	EXPECT_EQ(ReplyCode_U32, 257);
	EXPECT_STREQ(Reply_S.c_str(), "Remote directory now \"/tmp\"");

#if 0
	ListenForConnection_B = true;
	NotifyBuffer_X.pData_U8 = S_pBuffer_U8;
	NotifyBuffer_X.SizeInByte_U32 = sizeof(S_pBuffer_U8);
	Rts_E = _pFtpCmdChannel->SendIoCmdToCreateDataChannel(TIMEOUT_IN_MS, ChooseRamdowPort(), NotifyBuffer_X);  //PASV mode is tested in download/upload
	EXPECT_EQ(Rts_E, 0);

	Rts_E = _pFtpCmdChannel->SendIoCmdAndWaitForReply(TIMEOUT_IN_MS, "LIST\r\n", 150, Reply_S, ReplyCode_U32);
	EXPECT_EQ(Rts_E, 0);
	EXPECT_EQ(ReplyCode_U32, 150);
	EXPECT_STREQ(Reply_S.c_str(), "File status okay; about to open data connection.");

	DataConnectToAddress_X = _pFtpCmdChannel->DataConnectToAddress(ListenForConnection_B);
	Rts_E = _pFtpCmdChannel->OpenDataChannel(_pFtpCmdChannel->ConnectTimeoutInMs(), ListenForConnection_B, DataConnectToAddress_X, 0x10000, 0x10000);
	EXPECT_EQ(Rts_E, 0);

	Rts_E = _pFtpCmdChannel->WaitForIoCmdReply(TIMEOUT_IN_MS, 226, Reply_S, ReplyCode_U32);
	EXPECT_EQ(Rts_E, 0);
	EXPECT_EQ(ReplyCode_U32, 226);
	EXPECT_STREQ(Reply_S.c_str(), "Transfer complete, Closing data connection.");
#endif
#if 0
	Rts_E = _pFtpCmdChannel->CloseDataChannel(TIMEOUT_IN_MS);
	EXPECT_EQ(Rts_E, 0);

#endif
	Rts_E = _pFtpCmdChannel->SendIoCmdAndWaitForReply(TIMEOUT_IN_MS, "QUIT\r\n", 221, Reply_S, ReplyCode_U32);
	EXPECT_EQ(Rts_E, 0);
	EXPECT_EQ(ReplyCode_U32, 221);
	EXPECT_STREQ(Reply_S.c_str(), "Service closing control connection.");

	Rts_E = _pFtpCmdChannel->Disconnect(TIMEOUT_IN_MS);
	EXPECT_EQ(Rts_E, 0);

	return Rts_E;
}


TEST(IoServer_Test, Ftp)
{
	BOFERR Sts_E;
	uint32_t Loop_U32, Last_U32, Now_U32, Delta_U32, Start_U32, i_U32;
	std::thread pThread[NBMAXCLIENTSESSION];
	BOF_NAMESPACE::BofIoCmdChannel *pTheFtpClient[NBMAXCLIENTSESSION];
	BOF_NAMESPACE::BOF_DATE_TIME Now_X;
	std::string Now_S;
	BOF_NAMESPACE::IBofIoConnection *pIBofIoListenConnection;
	std::string WelcomeMsg_S, Reply_S;


	Last_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
	try
	{
		//   for (Loop_U32 = 0; Loop_U32 < 999999999; Loop_U32++)
		for (Loop_U32 = 0; Loop_U32 < 1; Loop_U32++)
		{
			Start_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();

			if (!(Loop_U32 % 100))
			{
				BOF_NAMESPACE::Bof_Now(Now_X);
				Now_S = BOF_NAMESPACE::Bof_FormatDateTime(Now_X);

				Now_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
				Delta_U32 = Now_U32 - Last_U32;
				printf("[%X] = %s => delta %d %d tests\n", Now_U32, Now_S.c_str(), Delta_U32, Loop_U32);
				Last_U32 = Now_U32;
			}
			BOF_NAMESPACE::BOF_IO_CONNECTION_MANAGER_PARAM BofIoConnectionManagerParam_X;
			BofIoConnectionManagerParam_X.Name_S = "IoConnectionManager";
			BofIoConnectionManagerParam_X.EventTimeoutInMs_U32 = BOF_NAMESPACE::DEFAULT_CMD_DATA_TIMEOUT;
			BofIoConnectionManagerParam_X.Policy_S32 = BOF_NAMESPACE::BOF_THREAD_POLICY_OTHER;
			BofIoConnectionManagerParam_X.Priority_S32 = BOF_NAMESPACE::BOF_THREAD_DEFAULT_PRIORITY;
			BofIoConnectionManagerParam_X.StartStopTimeoutInMs_U32 = 5000;
			BofIoConnectionManagerParam_X.NbMaxConnection_U32 = NBMAXCLIENTSESSION * 8; //TODO lower value 1 for listen 1 for cmd and one for data client and session on the same con manager
			BofIoConnectionManagerParam_X.NbMaxOpPending_U32 = (NBMAXCLIENTSESSION * 4);//TODO lower value
			BofIoConnectionManagerParam_X.NbMaxOpPending_U32 = (NBMAXCLIENTSESSION * 100);
			BOF_NAMESPACE::BofIoConnectionManager TheIoConnectionManager(BofIoConnectionManagerParam_X);

			IoServer TheIoServer(&TheIoConnectionManager);

			BOF_NAMESPACE::BOF_IO_CHANNEL_PARAM IoChannelParam_X;
			IoChannelParam_X.RcvBufferSize_U32 = 0x1000;
			IoChannelParam_X.SndBufferSize_U32 = 0x1000;
			IoChannelParam_X.ConnectTimeoutInMs_U32 = BOF_NAMESPACE::DEFAULT_CMD_DATA_TIMEOUT;
			IoChannelParam_X.Address_S = BOF_NAMESPACE::Bof_Sprintf("tcp://0.0.0.0:%d;0.0.0.0:0", SERVER_PORT);;
			IoChannelParam_X.IO_CHANNEL.TCP.NoDelay_B = true;

			BOF_NAMESPACE::IBOF_IO_CONNECTION_PARAM IBofIoConnectionParam_X;
			IBofIoConnectionParam_X.Name_S = "FtpSrv";
			IBofIoConnectionParam_X.NotifyRcvBufferSize_U32 = 0x1000;
			IBofIoConnectionParam_X.NotifyType_E = BOF_NAMESPACE::BOF_IO_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND;
			IBofIoConnectionParam_X.Delimiter_U8 = '\n';
			Sts_E = TheIoServer.Listen(IBofIoConnectionParam_X, IoChannelParam_X, BofIoConnectionManagerParam_X.NbMaxConnection_U32, &pIBofIoListenConnection);
			EXPECT_EQ(Sts_E, 0);

			for (i_U32 = 0; i_U32 < NBMAXCLIENTSESSION; i_U32++)
			{
				//		BOF_NAMESPACE::Bof_Sleep(BOF_NAMESPACE::Bof_Random(false, 0, 100));

				IoChannelParam_X.RcvBufferSize_U32 = 0x10000;
				IoChannelParam_X.SndBufferSize_U32 = 0x10000;
				IoChannelParam_X.ConnectTimeoutInMs_U32 = BOF_NAMESPACE::DEFAULT_CMD_DATA_TIMEOUT;
				IoChannelParam_X.Address_S = BOF_NAMESPACE::Bof_Sprintf("tcp://0.0.0.0:0;127.0.0.1:%d", SERVER_PORT);
				IoChannelParam_X.IO_CHANNEL.TCP.NoDelay_B = true;

				IBofIoConnectionParam_X.Name_S = "FtpClt";
				IBofIoConnectionParam_X.NotifyRcvBufferSize_U32 = 0x1000;
				IBofIoConnectionParam_X.NotifyType_E = BOF_NAMESPACE::BOF_IO_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND;
				IBofIoConnectionParam_X.Delimiter_U8 = '\n';
				IBofIoConnectionParam_X.pData = nullptr;
				pTheFtpClient[i_U32] = new BOF_NAMESPACE::BofIoCmdChannel(IBofIoConnectionParam_X, &TheIoConnectionManager);

#if 1
				pThread[i_U32] = std::thread(&FtpConnect, pTheFtpClient[i_U32], i_U32, IoChannelParam_X);
#else
				switch (i_U32 % 3)
				{
				case 0:
					BofIoChannelParam_X.Address_S = "tcp://0.0.0.0:0;" + SERVER_ADDRESS;
					pThread[i_U32] = std::thread(&Connect, &IoConnectionManager, i_U32, BofIoChannelParam_X);
					//            pThread[i_U32] = std::thread(&UploadData, &IoConnectionManager, i_U32, TCP_SERVER_ADDRESS);
					break;

				case 1:
					pThread[i_U32] = std::thread(&UploadData, &IoConnectionManager, i_U32, BofIoChannelParam_X);
					break;

				case 2:
					pThread[i_U32] = std::thread(&DownloadData, &IoConnectionManager, i_U32, BofIoChannelParam_X);
					break;
				}
#endif
			}


			for (i_U32 = 0; i_U32 < NBMAXCLIENTSESSION; i_U32++)
			{
#if 0
				BOF_NAMESPACE::Bof_Sleep(1000);

				Dbg_S = pTheFtpClient[i_U32]->IoConnectionManager()->BuildConnectionInfo();
				DBG_OUT(":::: Quit\n\n%s", Dbg_S.c_str());

				Sts_E = pTheFtpClient[i_U32]->SendFtpCommandAndWaitForReply(TIMEOUT_IN_MS, "QUIT\r\n", 221, Reply_S, ReplyCode_U32);
				if (Sts_E)
				{
					EXPECT_EQ(Sts_E, 0);
				}
				EXPECT_EQ(Sts_E, 0);
				EXPECT_EQ(ReplyCode_U32, 221);
				EXPECT_STREQ(Reply_S.c_str(), "Service closing control connection.");


				Sts_E = pTheFtpClient[i_U32]->Disconnect(DEFAULT_CMD_DATA_TIMEOUT);
				EXPECT_EQ(Sts_E, 0);
#else
				pThread[i_U32].join();
#endif

				std::string Dbg_S;
				Dbg_S = TheIoConnectionManager.BuildConnectionInfo();
				DBG_OUT("Before BOF_SAFE_DELETE\n\n%s", Dbg_S.c_str());


				BOF_SAFE_DELETE(pTheFtpClient[i_U32]);
				printf("-------------------------------------------------------------------------------------->thread %d out\n", i_U32);
			}

			Delta_U32 = BOF_NAMESPACE::Bof_ElapsedMsTime(Start_U32);

		} //for (Loop_U32 =0; Loop_U32<5; Loop_U32++)
	}

	catch (std::exception &e)
	{
		printf("IoServer_Test exception %s\n", e.what());
	}
}

