/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the BofIoChannel class
 *
 * Name:        ut_socketchannelmanager.cpp
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

#include <vector>
#include "gtestrunner.h"
#include <bofstd/bofsocketchannelmanager.h>
#include <bofstd/bofsocketchannel.h>
#include <bofstd/bofstringformatter.h>
#include <bofstd/bofstring.h>

#define  EXPECT_BETWEEN(v,min,max) {EXPECT_GE(static_cast<int>(v), static_cast<int>(min));EXPECT_LE(static_cast<int>(v), static_cast<int>(max));}

TEST(SocketChannelManager_Test, ConstructorDestuctor)
{
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_MANAGER_PARAM BofSocketChannelManagerParam_X, CurrentBofSocketChannelManagerParam_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_MANAGER_STATISTIC BofSocketChannelManagerStatistic_X;
  const uint32_t NB_MAX_CHANNEL = 16;
  const uint32_t SLEEP_TIME = 1000;

  BofSocketChannelManagerParam_X.Name_S = "Manager";
  BofSocketChannelManagerParam_X.NbMaxChannel_U32 = NB_MAX_CHANNEL;
  BofSocketChannelManagerParam_X.Policy_S32 = BOF_NAMESPACE::BOF_THREAD_POLICY_OTHER;
  BofSocketChannelManagerParam_X.Priority_S32 = BOF_NAMESPACE::BOF_THREAD_DEFAULT_PRIORITY;
  BofSocketChannelManagerParam_X.StartStopTimeoutInMs_U32 = 5000;
  BofSocketChannelManagerParam_X.CloseTimeoutInMs_U32 = 1000;
  BofSocketChannelManagerParam_X.ClosePollingTimeInMs_U32 = 20;

  std::shared_ptr<BOF_NAMESPACE::BofSocketChannelManager> psSocketChannelManager= BOF_NAMESPACE::BofSocketChannelManager::S_BofSocketChannelManagerFactory(BofSocketChannelManagerParam_X);
  EXPECT_EQ(psSocketChannelManager->InitErrorCode(), BOFERR_NO_ERROR);

  EXPECT_EQ(psSocketChannelManager->LockBofSocketChannelManager("Manager"), BOFERR_NO_ERROR);
  EXPECT_EQ(psSocketChannelManager->UnlockBofSocketChannelManager(), BOFERR_NO_ERROR);

  CurrentBofSocketChannelManagerParam_X = psSocketChannelManager->BofSocketChannelManagerParam();
  EXPECT_EQ(CurrentBofSocketChannelManagerParam_X, BofSocketChannelManagerParam_X);
  BOF_NAMESPACE::Bof_Sleep(SLEEP_TIME);

  BofSocketChannelManagerStatistic_X = psSocketChannelManager->BofSocketChannelManagerStatistic();
  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncEventError_U32, 0);
  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncEvent_U32, 0);
  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncEvent_U32, 0);
  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommand_U32, 0);
  EXPECT_BETWEEN(BofSocketChannelManagerStatistic_X.NbTimerEvent_U32, (SLEEP_TIME / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS) - 1, (SLEEP_TIME / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS)+1);
  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommandPending_U32, 0);

  EXPECT_EQ(psSocketChannelManager->GetNumberOfChannel(), 0);
}

TEST(SocketChannelManager_Test, AddRemoveChannel)
{
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_MANAGER_PARAM BofSocketChannelManagerParam_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_PARAM  BofSocketChannelParam_X;
  uint32_t i_U32;
  std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> psBofSocketChannel;
  std::vector<std::shared_ptr<BOF_NAMESPACE::BofSocketChannel>> BofSocketChannelCollection;

  const uint32_t NB_MAX_CHANNEL = 16;

  BofSocketChannelManagerParam_X.Name_S = "Manager";
  BofSocketChannelManagerParam_X.NbMaxChannel_U32 = NB_MAX_CHANNEL;
  BofSocketChannelManagerParam_X.Policy_S32 = BOF_NAMESPACE::BOF_THREAD_POLICY_OTHER;
  BofSocketChannelManagerParam_X.Priority_S32 = BOF_NAMESPACE::BOF_THREAD_DEFAULT_PRIORITY;
  BofSocketChannelManagerParam_X.StartStopTimeoutInMs_U32 = 5000;
  BofSocketChannelManagerParam_X.CloseTimeoutInMs_U32 = 1000;
  BofSocketChannelManagerParam_X.ClosePollingTimeInMs_U32 = 20;

  std::shared_ptr<BOF_NAMESPACE::BofSocketChannelManager> psSocketChannelManager = BOF_NAMESPACE::BofSocketChannelManager::S_BofSocketChannelManagerFactory(BofSocketChannelManagerParam_X);
  EXPECT_EQ(psSocketChannelManager->InitErrorCode(), BOFERR_NO_ERROR);

  EXPECT_EQ(psSocketChannelManager->GetNumberOfChannel(), 0);

  for (i_U32 = 0; i_U32 < NB_MAX_CHANNEL; i_U32++)
  {
    BofSocketChannelParam_X.ChannelId_U32 = i_U32;
    BofSocketChannelParam_X.Name_S = "SocketChannel_Test_"+ std::to_string(i_U32);
    BofSocketChannelParam_X.ListenBacklog_U32 = (i_U32 % 2) ? 1:0;
    BofSocketChannelParam_X.IpAddress_S = BOF_NAMESPACE::Bof_Sprintf("tcp://0.0.0.0:0;127.0.0.1:%d", 56000 + i_U32);
    BofSocketChannelParam_X.NotifyRcvBufferSize_U32 = 0x1000;
    BofSocketChannelParam_X.NotifyType_E = BOF_NAMESPACE::BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND;
    BofSocketChannelParam_X.Delimiter_U8 = '\n';
    BofSocketChannelParam_X.pData = nullptr;
    BofSocketChannelParam_X.RcvBufferSize_U32 = 0x10000;
    BofSocketChannelParam_X.SndBufferSize_U32 = 0x10000;
    BofSocketChannelParam_X.CloseTimeoutInMs_U32 = 2000;
    BofSocketChannelParam_X.ClosePollingTimeInMs_U32 = 20;
    BofSocketChannelParam_X.OnSocketAcceptCallback = nullptr;
    BofSocketChannelParam_X.OnSocketReadCallback = nullptr;
    EXPECT_EQ(psSocketChannelManager->AddChannel(BofSocketChannelParam_X, psBofSocketChannel), BOFERR_NO_ERROR);
    BofSocketChannelCollection.push_back(psBofSocketChannel);
    EXPECT_EQ(psSocketChannelManager->GetNumberOfChannel(), i_U32+1);
  }

  for (i_U32 = 0; i_U32 < NB_MAX_CHANNEL; i_U32++)
  {
    EXPECT_EQ(psSocketChannelManager->RemoveChannel("AddRemoveChannel", BofSocketChannelCollection[i_U32]), BOFERR_NO_ERROR);
    EXPECT_EQ(psSocketChannelManager->GetNumberOfChannel(), NB_MAX_CHANNEL - i_U32 - 1);
  }
}

//Need ftp server
TEST(SocketChannelManager_Test, AsyncCommand)
{
  const uint32_t NB_MAX_CHANNEL = 16;
  const uint32_t ASYNC_CONNECT_TIMEOUT = 500;
  const uint32_t WAIT_WELCOME_TIMEOUT = 500;

  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_MANAGER_PARAM BofSocketChannelManagerParam_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_MANAGER_STATISTIC BofSocketChannelManagerStatistic_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_PARAM  BofSocketChannelParam_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_STATISTIC BofSocketChannelStatistic_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_STATE     BofSocketChannelState_X;
  BOF_NAMESPACE::BOF_EVENT pNotifyEvent_X[NB_MAX_CHANNEL];
  std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> psBofSocketChannel;
  uint32_t i_U32, j_U32;
  BOFERR Sts_E;

  BofSocketChannelManagerParam_X.Name_S = "Manager";
  BofSocketChannelManagerParam_X.NbMaxChannel_U32 = NB_MAX_CHANNEL;
  BofSocketChannelManagerParam_X.Policy_S32 = BOF_NAMESPACE::BOF_THREAD_POLICY_OTHER;
  BofSocketChannelManagerParam_X.Priority_S32 = BOF_NAMESPACE::BOF_THREAD_DEFAULT_PRIORITY;
  BofSocketChannelManagerParam_X.StartStopTimeoutInMs_U32 = 5000;
  BofSocketChannelManagerParam_X.CloseTimeoutInMs_U32 = 1000;
  BofSocketChannelManagerParam_X.ClosePollingTimeInMs_U32 = 20;

  for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
  {
    BOF_NAMESPACE::Bof_CreateEvent("ConEvnt_" + std::to_string(j_U32), false, 1, false, pNotifyEvent_X[j_U32]);
  }

  for (i_U32 = 0; i_U32 < 2; i_U32++)
  {
//    i_U32 = 1;
    std::vector<std::shared_ptr<BOF_NAMESPACE::BofSocketChannel>> BofSocketChannelCollection;
    std::shared_ptr<BOF_NAMESPACE::BofSocketChannelManager> psSocketChannelManager = BOF_NAMESPACE::BofSocketChannelManager::S_BofSocketChannelManagerFactory(BofSocketChannelManagerParam_X);
    EXPECT_EQ(psSocketChannelManager->InitErrorCode(), BOFERR_NO_ERROR);
    printf("T000=%d\n", BOF_NAMESPACE::Bof_GetMsTickCount());

    for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
    {
      BofSocketChannelParam_X.ChannelId_U32 = j_U32;
      BofSocketChannelParam_X.Name_S = "Channel_" + std::to_string(i_U32) + "_" + std::to_string(j_U32);
      BofSocketChannelParam_X.ListenBacklog_U32 = 0;
			BofSocketChannelParam_X.IpAddress_S = (i_U32==0) ? BOF_NAMESPACE::Bof_Sprintf("tcp://0.0.0.0:0;127.0.0.1:%d", 56000 + j_U32) : BOF_NAMESPACE::Bof_Sprintf("tcp://0.0.0.0:0;127.0.0.1:%d", 21);
      BofSocketChannelParam_X.NotifyRcvBufferSize_U32 = 0x1000;
      BofSocketChannelParam_X.NotifyType_E = BOF_NAMESPACE::BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND;
      BofSocketChannelParam_X.Delimiter_U8 = '\n';
      BofSocketChannelParam_X.pData = nullptr;
      BofSocketChannelParam_X.RcvBufferSize_U32 = 0x10000;
      BofSocketChannelParam_X.SndBufferSize_U32 = 0x10000;
      BofSocketChannelParam_X.CloseTimeoutInMs_U32 = 2000;
      BofSocketChannelParam_X.ClosePollingTimeInMs_U32 = 20;

      BofSocketChannelParam_X.OnSocketAcceptCallback = nullptr;
      BofSocketChannelParam_X.OnSocketReadCallback = nullptr;
      EXPECT_EQ(psSocketChannelManager->AddChannel(BofSocketChannelParam_X, psBofSocketChannel), BOFERR_NO_ERROR);
      BofSocketChannelCollection.push_back(psBofSocketChannel);
      EXPECT_EQ(psSocketChannelManager->GetNumberOfChannel(), j_U32 + 1);
    }
    printf("T001=%d\n", BOF_NAMESPACE::Bof_GetMsTickCount());

    BofSocketChannelManagerStatistic_X = psSocketChannelManager->BofSocketChannelManagerStatistic();
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncEventError_U32, 0);
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncEvent_U32, 0);
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommand_U32, 0);
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommandPending_U32, 0);

    for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
    {
      EXPECT_FALSE(BOF_NAMESPACE::Bof_IsEventSignaled(pNotifyEvent_X[j_U32], 0));
      EXPECT_EQ(psSocketChannelManager->SendAsyncConnectCmd(ASYNC_CONNECT_TIMEOUT, nullptr, BofSocketChannelCollection[j_U32], &pNotifyEvent_X[j_U32]), BOFERR_NO_ERROR);
    }
    printf("T002=%d\n", BOF_NAMESPACE::Bof_GetMsTickCount());

    for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
    {
//      EXPECT_EQ(BOF_NAMESPACE::Bof_WaitForEvent(pNotifyEvent_X[j_U32], ASYNC_CONNECT_TIMEOUT + (BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS / 2)), BOFERR_NO_ERROR); //+ To be able to check NbTimerEvent_U32
      Sts_E = BOF_NAMESPACE::Bof_WaitForEvent(pNotifyEvent_X[j_U32], ASYNC_CONNECT_TIMEOUT + (BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS / 2), 0);   //+ To be able to check NbTimerEvent_U32
      EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
//			BOF_NAMESPACE::Bof_Sleep(5555555);
      BofSocketChannelState_X = BofSocketChannelCollection[j_U32]->BofSocketChannelState();
			if (i_U32 == 0)
			{
				//Can abort on timeout or ECONNREFUSED
				EXPECT_TRUE((BofSocketChannelState_X.State_E == BOF_NAMESPACE::BOF_SOCKET_STATE::CONNECT_TIMEOUT) || (BofSocketChannelState_X.State_E == BOF_NAMESPACE::BOF_SOCKET_STATE::CONNECT_ERROR));
			}
			else
			{
				EXPECT_EQ(BofSocketChannelState_X.State_E, BOF_NAMESPACE::BOF_SOCKET_STATE::CONNECTED);
			}
    }
    printf("T003=%d\n", BOF_NAMESPACE::Bof_GetMsTickCount());

    BOF_NAMESPACE::Bof_Sleep((i_U32 == 0) ? 0:WAIT_WELCOME_TIMEOUT);  //Leave time for welcome message
    for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
    {
      EXPECT_FALSE(BOF_NAMESPACE::Bof_IsEventSignaled(pNotifyEvent_X[j_U32], 0));
      EXPECT_EQ(psSocketChannelManager->SendAsyncDisconnectCmd(ASYNC_CONNECT_TIMEOUT, nullptr, BofSocketChannelCollection[j_U32], false,&pNotifyEvent_X[j_U32]), (i_U32 == 0) ? BOFERR_NOT_OPENED: BOFERR_NO_ERROR);
      EXPECT_EQ(BOF_NAMESPACE::Bof_WaitForEvent(pNotifyEvent_X[j_U32], ASYNC_CONNECT_TIMEOUT + (BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS / 2), 0), (i_U32 == 0) ? BOFERR_TIMEOUT: BOFERR_NO_ERROR); //+ To be able to check NbTimerEvent_U32
    }
    printf("T004=%d i=%d\n", BOF_NAMESPACE::Bof_GetMsTickCount(), i_U32);

    psSocketChannelManager->WaitAsyncCommandQueueEmpty(20);
    for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
    {
      BofSocketChannelStatistic_X = BofSocketChannelCollection[j_U32]->BofSocketChannelStatistic();
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnect_U32, 1);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnectError_U32, (i_U32 == 0) ? 1:0);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnect_U32, (i_U32 == 0) ? 0:1);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnectError_U32, 0);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpListenError_U32, 0);
      EXPECT_GE(BofSocketChannelStatistic_X.NbOpRead_U32, (i_U32 == 0) ? 0:1); //Depend on ftp server: filezilla 3 proftp 1
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpReadError_U32, 0);
      EXPECT_GE(BofSocketChannelStatistic_X.NbDataByteRead_U64, (i_U32 == 0) ? 0:48);	//Depend on ftp server: filezilla 143 proftp 63
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWrite_U32, 0);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWriteError_U32, 0);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbDataByteWritten_U64, 0);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbDataEvent_U32, (i_U32 == 0) ? 0:1);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbWriteEvent_U32, 0);
      EXPECT_BETWEEN(BofSocketChannelStatistic_X.NbError_U32, 0, 1);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbCloseEvent_U32, (i_U32 == 0) ? 0:1);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbConnectEvent_U32, (i_U32 == 0) ? 0:1);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpEndEvent_U32, 0);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpShutdownEvent_U32, 0);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpListenEvent_U32, 0);
      EXPECT_BETWEEN(BofSocketChannelStatistic_X.NbTimerEvent_U32, (i_U32 == 0) ? (NB_MAX_CHANNEL * (ASYNC_CONNECT_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS)) + (ASYNC_CONNECT_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS) - 3 : (WAIT_WELCOME_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS), (i_U32 == 0) ? (NB_MAX_CHANNEL * (ASYNC_CONNECT_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS)) + (ASYNC_CONNECT_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS) + 10 : (WAIT_WELCOME_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS) + 2);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbTimerEventError_U32, 0);

      EXPECT_EQ(psSocketChannelManager->RemoveChannel("AsyncCommand", BofSocketChannelCollection[j_U32]), BOFERR_NO_ERROR);
      EXPECT_EQ(psSocketChannelManager->GetNumberOfChannel(), NB_MAX_CHANNEL - j_U32 - 1);
    }
    psSocketChannelManager->WaitAsyncCommandQueueEmpty(20); //MANDATORY to avoid extra uv callback while deleting the remaining object
    BofSocketChannelManagerStatistic_X = psSocketChannelManager->BofSocketChannelManagerStatistic();
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncEventError_U32, 0);
//    EXPECT_BETWEEN(BofSocketChannelManagerStatistic_X.NbAsyncEvent_U32, 1, 2 + 2);
   // printf("=============> NbAsyncEvent %d NbAsyncCommand %d NbAsyncCommandPending %d\n", BofSocketChannelManagerStatistic_X.NbAsyncEvent_U32, BofSocketChannelManagerStatistic_X.NbAsyncCommand_U32, BofSocketChannelManagerStatistic_X.NbAsyncCommandPending_U32);
    EXPECT_BETWEEN(BofSocketChannelManagerStatistic_X.NbAsyncEvent_U32, 1, NB_MAX_CHANNEL*2);
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommand_U32, NB_MAX_CHANNEL * 2); 
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbTimerEventError_U32, 0);
    EXPECT_BETWEEN(BofSocketChannelManagerStatistic_X.NbTimerEvent_U32, (i_U32 == 0) ? (NB_MAX_CHANNEL * (ASYNC_CONNECT_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS)) + (ASYNC_CONNECT_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS) - 3 : (WAIT_WELCOME_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS), (i_U32 == 0) ? (NB_MAX_CHANNEL * (ASYNC_CONNECT_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS)) + (ASYNC_CONNECT_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS) + 10 : (WAIT_WELCOME_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS) + 2);
		EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommandPending_U32, 0);
  }
}

std::vector<std::shared_ptr<BOF_NAMESPACE::BofSocketChannel>> S_BofSocketChannelAcceptedCollection;
static std::string S_DataReadCollection_S[32];
static std::string S_DataReadAcceptedCollection_S[32];

BOFERR OnSocketReadAcceptedCallback(uint32_t _Id_U32, std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> _psBofSocketChannel, const BOF_NAMESPACE::BOF_BUFFER &_rBuffer_X, const BOF_NAMESPACE::BOF_SOCKET_PEER_INFO &/*_rPeerInfo_X*/)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  std::string CmdLine_S,Cmd_S,Arg_S, Reply_S;
  const char *pCmd_c, *pArg_c;
  uint32_t Nb_U32;
  BOF_NAMESPACE::BOF_SOCKET_ADDRESS IpAddress_X;
  BOF_NAMESPACE::BOF_SOCK_TYPE SocketType_E;
  BOF_NAMESPACE::BOF_PROTOCOL_TYPE ProtocolType_E;
  uint32_t Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32;
  uint16_t Port_U16;
  uint8_t PortLow_U8, PortHigh_U8;

  if (_Id_U32 < BOF_NB_ELEM_IN_ARRAY(S_DataReadCollection_S))
  {
    BOF_ASSERT(_rBuffer_X.pData_U8 != nullptr);
    BOF_ASSERT(_rBuffer_X.SizeInByte_U64 >= 2);
    BOF_ASSERT(_rBuffer_X.MustBeDeleted_B == false);
    BOF_ASSERT(_rBuffer_X.Partial_B == false);
    BOF_ASSERT(_rBuffer_X.pData_U8[_rBuffer_X.SizeInByte_U64 - 2] == '\r');
    BOF_ASSERT(_rBuffer_X.pData_U8[_rBuffer_X.SizeInByte_U64 - 1] == '\n');

    CmdLine_S = std::string(_rBuffer_X.pData_U8, _rBuffer_X.pData_U8 + _rBuffer_X.SizeInByte_U64);
    S_DataReadAcceptedCollection_S[_Id_U32] += CmdLine_S;
    pCmd_c = reinterpret_cast<const char *>(CmdLine_S.c_str());
    pArg_c = strchr(pCmd_c, ' ');
    if (pArg_c != nullptr)
    {
      Nb_U32 = static_cast<uint32_t>(pArg_c - pCmd_c);
      BOF_ASSERT(Nb_U32 <= 4);
      Cmd_S = std::string(pCmd_c, pCmd_c + Nb_U32);
      Arg_S = std::string(pArg_c+1, pCmd_c + _rBuffer_X.SizeInByte_U64 -2);
    }
    else
    {
      Nb_U32 = static_cast<uint32_t>(_rBuffer_X.SizeInByte_U64 - 2);
      Cmd_S = std::string(pCmd_c, pCmd_c + Nb_U32);
      Arg_S = "";
    }
    Cmd_S=BOF_NAMESPACE::Bof_StringTrim(Cmd_S);
    Arg_S=BOF_NAMESPACE::Bof_StringTrim(Arg_S);
/*
Status:	Connection established, waiting for welcome message...
Response:	220-FileZilla Server 0.9.60 beta
Response:	220-written by Tim Kosse (tim.kosse@filezilla-project.org)
Response:	220 Please visit https://filezilla-project.org/
Command:	AUTH TLS
Response:	502 Explicit TLS authentication not allowed
Command:	AUTH SSL
Response:	502 Explicit TLS authentication not allowed
Status:	Insecure server, it does not support FTP over TLS.
Command:	USER bha
Response:	331 Password required for bha
Command:	PASS ******
Response:	230 Logged on
Command:	SYST
Response:	215 UNIX emulated by FileZilla
Command:	FEAT
Response:	211-Features:
Response:	 MDTM
Response:	 REST STREAM
Response:	 SIZE
Response:	 MLST type*;size*;modify*;
Response:	 MLSD
Response:	 UTF8
Response:	 CLNT
Response:	 MFMT
Response:	 EPSV
Response:	 EPRT
Response:	211 End
Status:	Logged in
Status:	Retrieving directory listing...
Command:	PWD
Response:	257 "/" is current directory.
Command:	TYPE I
Response:	200 Type set to I
Command:	PASV
Response:	227 Entering Passive Mode (127,0,0,1,195,99)
Command:	MLSD
Response:	150 Opening data channel for directory listing of "/"
Response:	226 Successfully transferred "/"
Status:	Directory listing of "/" successful
Response:	421 Connection timed out.
Status:	Connection closed by server
*/
    if (Cmd_S == "AUTH")
    {
      Reply_S = "502 Explicit TLS authentication not allowed\r\n";
    }
    else if (Cmd_S == "USER")
    {
      Reply_S = BOF_NAMESPACE::Bof_Sprintf("331 Password required for %s\r\n", Arg_S.c_str());
    }
    else if (Cmd_S == "PASS")
    {
      Reply_S = "230 Logged on\r\n";
    }
    else if (Cmd_S == "SYST")
    {
      Reply_S = "215 UNIX emulated by Gns\r\n";
    }
    else if (Cmd_S == "FEAT")
    {
      Reply_S = "211-Features:\r\n211-SIZE\r\n211-UTF8\r\n211 End\r\n";
    }
    else if (Cmd_S == "PWD")
    {
      Reply_S = "257 \"/\" is current directory.\r\n";
    }
    else if (Cmd_S == "TYPE")
    {
      Reply_S = BOF_NAMESPACE::Bof_Sprintf("200 Type set to %s\r\n", Arg_S.c_str());
    }
    else if (Cmd_S == "PASV")
    {
      IpAddress_X = _psBofSocketChannel->LocalAddress();
      IpAddress_X.Parse(SocketType_E, ProtocolType_E, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, Port_U16);
//      Port_U16=
      PortHigh_U8 = static_cast<uint8_t>(Port_U16 >> 8);
      PortLow_U8 = static_cast<uint8_t>(Port_U16);
      Reply_S = BOF_NAMESPACE::Bof_Sprintf("227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n", Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, PortHigh_U8, PortLow_U8);
    }
    if (Reply_S != "")
    {
      _psBofSocketChannel->WriteData(BOF_NAMESPACE::MIN_TIMEOUT_VALUE_IN_MS, true, Reply_S);
    }
  }
  return Rts_E;
}
BOFERR OnSocketAcceptCallback(uint32_t _Id_U32, std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> _psBofSocketChannel, const BOF_NAMESPACE::BOF_SOCKET_PEER_INFO &/*_rPeerInfo_X*/, BOF_NAMESPACE::BOF_SOCKET_CHANNEL_PARAM *_pBofSocketChannelParam_X)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;

  if (_Id_U32 < BOF_NB_ELEM_IN_ARRAY(S_DataReadCollection_S))
  {
//You can change some default channel attributes setting (from listener) by returning a modified BOF_SOCKET_CHANNEL_PARAM structure via the _pBofSocketChannelParam_X pointer
    if (_pBofSocketChannelParam_X)
    {
     // _pBofSocketChannelParam_X->ChannelId_U32++;
     // _pBofSocketChannelParam_X->Delimiter_U8++;
      _pBofSocketChannelParam_X->Name_S = "Accept_" + _pBofSocketChannelParam_X->Name_S;
      _pBofSocketChannelParam_X->NotifyRcvBufferSize_U32 *= 2;
      _pBofSocketChannelParam_X->NotifyType_E = BOF_NAMESPACE::BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND;
      _pBofSocketChannelParam_X->OnSocketReadCallback = OnSocketReadAcceptedCallback;
      _pBofSocketChannelParam_X->pData = nullptr;
      _pBofSocketChannelParam_X->RcvBufferSize_U32 *= 2;
      _pBofSocketChannelParam_X->SndBufferSize_U32 *= 2;
    }
    Rts_E=_psBofSocketChannel->WriteData(BOF_NAMESPACE::MIN_TIMEOUT_VALUE_IN_MS,true, "220-Gns Server 1.0.0 beta\r\n220-written by Bernard HARMEL (b.harmel@gmail.com)\r\n220 Please visit http://onbings.dscloud.me/\r\n");
    S_BofSocketChannelAcceptedCollection.push_back(_psBofSocketChannel);
  }
  return Rts_E;
}

BOFERR OnSocketReadCallback(uint32_t _Id_U32, std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> /*_psBofSocketChannel*/, const BOF_NAMESPACE::BOF_BUFFER &_rBuffer_X, const BOF_NAMESPACE::BOF_SOCKET_PEER_INFO &/*_rPeerInfo_X*/)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;

  if (_Id_U32 < BOF_NB_ELEM_IN_ARRAY(S_DataReadCollection_S))
  {
    S_DataReadCollection_S[_Id_U32] += std::string(_rBuffer_X.pData_U8, _rBuffer_X.pData_U8 + _rBuffer_X.SizeInByte_U64);
  }
  return Rts_E;
}

//Need filezilla or ft server running
TEST(SocketChannelManager_Test, TcpConnectIo)
{
	const uint32_t NB_MAX_CHANNEL = 16;
  const uint32_t ASYNC_CONNECT_TIMEOUT = 1000;
  const uint32_t ASYNC_WRITE_TIMEOUT = 1000;
	const uint32_t WAIT_WELCOME_TIMEOUT = 500;

  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_MANAGER_PARAM BofSocketChannelManagerParam_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_MANAGER_STATISTIC BofSocketChannelManagerStatistic_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_PARAM  BofSocketChannelParam_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_STATISTIC BofSocketChannelStatistic_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_STATE     BofSocketChannelState_X;
  BOF_NAMESPACE::BOF_EVENT pNotifyEvent_X[NB_MAX_CHANNEL];
  std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> psBofSocketChannel;
  std::vector<BOF_NAMESPACE::BOF_BUFFER> BufferCollection;

  uint32_t i_U32, j_U32;
  std::string Data_S;

  BofSocketChannelManagerParam_X.Name_S = "Manager";
  BofSocketChannelManagerParam_X.NbMaxChannel_U32 = NB_MAX_CHANNEL;
  BofSocketChannelManagerParam_X.Policy_S32 = BOF_NAMESPACE::BOF_THREAD_POLICY_OTHER;
  BofSocketChannelManagerParam_X.Priority_S32 = BOF_NAMESPACE::BOF_THREAD_DEFAULT_PRIORITY;
  BofSocketChannelManagerParam_X.StartStopTimeoutInMs_U32 = 5000;
  BofSocketChannelManagerParam_X.CloseTimeoutInMs_U32 = 1000;
  BofSocketChannelManagerParam_X.ClosePollingTimeInMs_U32 = 20;

  for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
  {
    BOF_NAMESPACE::Bof_CreateEvent("ConEvnt_" + std::to_string(j_U32), false, 1, false, pNotifyEvent_X[j_U32]);
  }

  for (i_U32 = 0; i_U32 < 2; i_U32++) //i_U32: 0 connect on bad address  1 connect on good address -> need a local ftp server
  //i_U32 = 1;
  {
    DBG_OUT("--- Pass %d ---------------------------------------------------------\n", i_U32);
    if (i_U32 == 1)
    {
      printf("t000=%d\n", BOF_NAMESPACE::Bof_GetMsTickCount());
    }
    std::vector<std::shared_ptr<BOF_NAMESPACE::BofSocketChannel>> BofSocketChannelCollection;
    std::shared_ptr<BOF_NAMESPACE::BofSocketChannelManager> psSocketChannelManager = BOF_NAMESPACE::BofSocketChannelManager::S_BofSocketChannelManagerFactory(BofSocketChannelManagerParam_X);
    EXPECT_EQ(psSocketChannelManager->InitErrorCode(), BOFERR_NO_ERROR);
    for (j_U32 = 0; j_U32 < BOF_NB_ELEM_IN_ARRAY(S_DataReadCollection_S); j_U32++)
    {
      S_DataReadCollection_S[j_U32] = "";
    }

    for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
    {
      BofSocketChannelParam_X.ChannelId_U32 = j_U32;
      BofSocketChannelParam_X.Name_S = "TcpChannel_" + std::to_string(i_U32) + "_" + std::to_string(j_U32);
      BofSocketChannelParam_X.ListenBacklog_U32 = 0;
      BofSocketChannelParam_X.IpAddress_S = (i_U32==0) ? BOF_NAMESPACE::Bof_Sprintf("tcp://0.0.0.0:0;127.0.0.1:%d", 56000 + j_U32): BOF_NAMESPACE::Bof_Sprintf("tcp://0.0.0.0:0;127.0.0.1:%d", 21);
      BofSocketChannelParam_X.NotifyRcvBufferSize_U32 = 0x10000;
      BofSocketChannelParam_X.NotifyType_E = BOF_NAMESPACE::BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND;
      BofSocketChannelParam_X.Delimiter_U8 = '\n';
      BofSocketChannelParam_X.pData = nullptr;
      BofSocketChannelParam_X.RcvBufferSize_U32 = 0x10000;
      BofSocketChannelParam_X.SndBufferSize_U32 = 0x10000;
      BofSocketChannelParam_X.CloseTimeoutInMs_U32 = 2000;
      BofSocketChannelParam_X.ClosePollingTimeInMs_U32 = 20;

      BofSocketChannelParam_X.OnSocketAcceptCallback = nullptr;
      BofSocketChannelParam_X.OnSocketReadCallback = OnSocketReadCallback;
      if (i_U32 == 1)
      {
        printf("t001A=%d\n", BOF_NAMESPACE::Bof_GetMsTickCount());
      }
      EXPECT_EQ(psSocketChannelManager->AddChannel(BofSocketChannelParam_X, psBofSocketChannel), BOFERR_NO_ERROR);
      BofSocketChannelCollection.push_back(psBofSocketChannel);
      EXPECT_EQ(psSocketChannelManager->GetNumberOfChannel(), j_U32 + 1);
      if (i_U32 == 1)
      {
        printf("t001B=%d\n", BOF_NAMESPACE::Bof_GetMsTickCount());
      }
    }
    if (i_U32 == 1)
    {
      printf("t001C=%d\n", BOF_NAMESPACE::Bof_GetMsTickCount());
    }
    BofSocketChannelManagerStatistic_X = psSocketChannelManager->BofSocketChannelManagerStatistic();
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncEventError_U32, 0);
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncEvent_U32, 0);
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommand_U32, 0);
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommandPending_U32, 0);

    for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
    {
      EXPECT_FALSE(BOF_NAMESPACE::Bof_IsEventSignaled(pNotifyEvent_X[j_U32], 0));
      EXPECT_EQ(psSocketChannelManager->SendAsyncConnectCmd(ASYNC_CONNECT_TIMEOUT, nullptr, BofSocketChannelCollection[j_U32], &pNotifyEvent_X[j_U32]), BOFERR_NO_ERROR);
    }
 //   if (i_U32 == 0) goto NextIter;

    for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
    {
      EXPECT_EQ(BOF_NAMESPACE::Bof_WaitForEvent(pNotifyEvent_X[j_U32], ASYNC_CONNECT_TIMEOUT + (BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS / 2), 0), BOFERR_NO_ERROR); //+ To be able to check NbTimerEvent_U32
      BofSocketChannelState_X = BofSocketChannelCollection[j_U32]->BofSocketChannelState();
      if (i_U32 == 0)
      {
        //Can abort on timeout or ECONNREFUSED
        EXPECT_TRUE((BofSocketChannelState_X.State_E == BOF_NAMESPACE::BOF_SOCKET_STATE::CONNECT_TIMEOUT) || (BofSocketChannelState_X.State_E == BOF_NAMESPACE::BOF_SOCKET_STATE::CONNECT_ERROR));
      }
      else
      {
        EXPECT_EQ(BofSocketChannelState_X.State_E, BOF_NAMESPACE::BOF_SOCKET_STATE::CONNECTED);
      }
      //    if (i_U32 == 0) goto NextIter;
    }
    if (i_U32 == 1)
    {
      printf("t002=%d\n", BOF_NAMESPACE::Bof_GetMsTickCount());
    }
//    psSocketChannelManager->ResetStat();
 //To check for memory leak   for (j_U32 = 0; j_U32 < 0x2000/10; j_U32++)
    {
      Data_S += "USER bha\r\n";
    }
    for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
    {
      //      BofSocketChannelCollection[j_U32]->ResetStat();
      EXPECT_FALSE(BOF_NAMESPACE::Bof_IsEventSignaled(pNotifyEvent_X[j_U32], 0));
      EXPECT_EQ(psSocketChannelManager->Write(ASYNC_WRITE_TIMEOUT, nullptr, BofSocketChannelCollection[j_U32],true, Data_S, &pNotifyEvent_X[j_U32]), (i_U32 == 0) ? BOFERR_NOT_OPENED : BOFERR_NO_ERROR);
      if (i_U32 == 0)
      {

      }
      else
      {
        EXPECT_EQ(BOF_NAMESPACE::Bof_WaitForEvent(pNotifyEvent_X[j_U32], ASYNC_WRITE_TIMEOUT + (BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS / 2), 0), BOFERR_NO_ERROR); //+ To be able to check NbTimerEvent_U32
        BofSocketChannelState_X = BofSocketChannelCollection[j_U32]->BofSocketChannelState();
        EXPECT_EQ(BofSocketChannelState_X.State_E, BOF_NAMESPACE::BOF_SOCKET_STATE::IO);
      }
    }
		BOF_NAMESPACE::Bof_Sleep((i_U32 == 0) ? 0 : WAIT_WELCOME_TIMEOUT);  //Leave time for welcome message

    for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
    {
//      BofSocketChannelCollection[j_U32]->ResetStat();

      EXPECT_FALSE(BOF_NAMESPACE::Bof_IsEventSignaled(pNotifyEvent_X[j_U32], 0));
      printf("_______ send disconnect %p\n", static_cast<void *>(BofSocketChannelCollection[j_U32].get()));
      EXPECT_EQ(psSocketChannelManager->SendAsyncDisconnectCmd(ASYNC_CONNECT_TIMEOUT, nullptr, BofSocketChannelCollection[j_U32], false,&pNotifyEvent_X[j_U32]), (i_U32 == 0) ? BOFERR_NOT_OPENED : BOFERR_NO_ERROR);
      if (i_U32 == 0)
      {

      }
      else
      {
        EXPECT_EQ(BOF_NAMESPACE::Bof_WaitForEvent(pNotifyEvent_X[j_U32], ASYNC_CONNECT_TIMEOUT + (BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS / 2), 0), BOFERR_NO_ERROR); //+ To be able to check NbTimerEvent_U32
      }
    }
    if (i_U32 == 1)
    {
      printf("t003=%d\n", BOF_NAMESPACE::Bof_GetMsTickCount());
    }
    psSocketChannelManager->WaitAsyncCommandQueueEmpty(20);
    for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
    {
      EXPECT_GE(static_cast<int>(S_DataReadCollection_S[j_U32].size()), (i_U32 == 0) ? 0:16);
    }

    for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
    {
      BofSocketChannelStatistic_X = BofSocketChannelCollection[j_U32]->BofSocketChannelStatistic();
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnect_U32, 1);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnectError_U32, (i_U32 == 0) ? 1:0);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnect_U32, (i_U32 == 0) ? 0:1);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnectError_U32, 0);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpListenError_U32, 0);
      EXPECT_GE(BofSocketChannelStatistic_X.NbOpRead_U32, (i_U32 == 0) ? 0:3);	//Depend on ftp server: filezilla 5 proftp 3
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpReadError_U32, 0);
      EXPECT_GE(BofSocketChannelStatistic_X.NbDataByteRead_U64, (i_U32 == 0) ? 0:16);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWrite_U32, 0);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWriteError_U32, 0);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbDataByteWritten_U64, 0);
      EXPECT_BETWEEN(BofSocketChannelStatistic_X.NbDataEvent_U32, (i_U32 == 0) ? 0:1, (i_U32 == 0) ? 0 : 3);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbWriteEvent_U32, (i_U32 == 0) ? 0 : 1);
      EXPECT_BETWEEN(BofSocketChannelStatistic_X.NbError_U32, (i_U32 == 0) ? 0 : 0, (i_U32 == 0) ? 1 : 0);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbCloseEvent_U32, (i_U32 == 0) ? 0:1);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbConnectEvent_U32, (i_U32 == 0) ? 0 : 1);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpEndEvent_U32, 0);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpShutdownEvent_U32, 0);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpListenEvent_U32, 0);
      if (i_U32 == 1)
      {
        printf("t004=%d\n", BOF_NAMESPACE::Bof_GetMsTickCount());
      }
      EXPECT_BETWEEN(BofSocketChannelStatistic_X.NbTimerEvent_U32, (i_U32 == 0) ? 1 : (WAIT_WELCOME_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS), (i_U32 == 0) ? (ASYNC_CONNECT_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS) + 1 : (WAIT_WELCOME_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS) + 4);
      EXPECT_EQ(BofSocketChannelStatistic_X.NbTimerEventError_U32, 0);

      EXPECT_EQ(psSocketChannelManager->RemoveChannel("TcpConnectIo", BofSocketChannelCollection[j_U32]), BOFERR_NO_ERROR);
      EXPECT_EQ(psSocketChannelManager->GetNumberOfChannel(), NB_MAX_CHANNEL - j_U32 - 1);
    }
    psSocketChannelManager->WaitAsyncCommandQueueEmpty(20);   //MANDATORY to avoid extra uv callback while deleting the remaining object

    BofSocketChannelManagerStatistic_X = psSocketChannelManager->BofSocketChannelManagerStatistic();
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncEventError_U32, 0);
    //    EXPECT_BETWEEN(BofSocketChannelManagerStatistic_X.NbAsyncEvent_U32, 1, 2 + 2);
    EXPECT_BETWEEN(BofSocketChannelManagerStatistic_X.NbAsyncEvent_U32, 1, (i_U32 == 0) ? (NB_MAX_CHANNEL * 2) : (NB_MAX_CHANNEL * 3));
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommand_U32,  (i_U32 == 0) ? (NB_MAX_CHANNEL * 2) : (NB_MAX_CHANNEL * 3));  //SOCKET_ASYNC_CMD_CONNECT,SOCKET_ASYNC_CMD_RELEASE_RESOURCE or  SOCKET_ASYNC_CMD_CONNECT,SOCKET_ASYNC_CMD_WRITE, SOCKET_ASYNC_CMD_RELEASE_RESOURCE
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbTimerEventError_U32, 0);
    EXPECT_BETWEEN(BofSocketChannelManagerStatistic_X.NbTimerEvent_U32, (i_U32 == 0) ? 1 : (WAIT_WELCOME_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS), (i_U32 == 0) ? (ASYNC_CONNECT_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS) + 1 : (WAIT_WELCOME_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS) + 4);
    // NextIter:
    EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommandPending_U32, 0);
  }
}

//Need echo server running see tool dir echotool /p udp /s %1 on port 56000
TEST(SocketChannelManager_Test, UdpConnectIo)
{
  const uint32_t NB_MAX_CHANNEL = 1;
  const uint32_t ASYNC_CONNECT_TIMEOUT = 1000;
  const uint32_t ASYNC_WRITE_TIMEOUT = 1000;

  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_MANAGER_PARAM BofSocketChannelManagerParam_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_MANAGER_STATISTIC BofSocketChannelManagerStatistic_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_PARAM  BofSocketChannelParam_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_STATISTIC BofSocketChannelStatistic_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_STATE     BofSocketChannelState_X;
  BOF_NAMESPACE::BOF_EVENT pNotifyEvent_X[NB_MAX_CHANNEL];
  std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> psBofSocketChannel;
  std::vector<BOF_NAMESPACE::BOF_BUFFER> BufferCollection;

  uint32_t j_U32;
  std::string Data_S;

  BofSocketChannelManagerParam_X.Name_S = "Manager";
  BofSocketChannelManagerParam_X.NbMaxChannel_U32 = NB_MAX_CHANNEL;
  BofSocketChannelManagerParam_X.Policy_S32 = BOF_NAMESPACE::BOF_THREAD_POLICY_OTHER;
  BofSocketChannelManagerParam_X.Priority_S32 = BOF_NAMESPACE::BOF_THREAD_DEFAULT_PRIORITY;
  BofSocketChannelManagerParam_X.StartStopTimeoutInMs_U32 = 5000;
  BofSocketChannelManagerParam_X.CloseTimeoutInMs_U32 = 1000;
  BofSocketChannelManagerParam_X.ClosePollingTimeInMs_U32 = 20;

  for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
  {
    BOF_NAMESPACE::Bof_CreateEvent("ConEvnt_" + std::to_string(j_U32), false, 1, false, pNotifyEvent_X[j_U32]);
  }

  std::vector<std::shared_ptr<BOF_NAMESPACE::BofSocketChannel>> BofSocketChannelCollection;
  std::shared_ptr<BOF_NAMESPACE::BofSocketChannelManager> psSocketChannelManager = BOF_NAMESPACE::BofSocketChannelManager::S_BofSocketChannelManagerFactory(BofSocketChannelManagerParam_X);
  EXPECT_EQ(psSocketChannelManager->InitErrorCode(), BOFERR_NO_ERROR);
  for (j_U32 = 0; j_U32 < BOF_NB_ELEM_IN_ARRAY(S_DataReadCollection_S); j_U32++)
  {
    S_DataReadCollection_S[j_U32] = "";
  }
  for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
  {
    BofSocketChannelParam_X.ChannelId_U32 = j_U32;
    BofSocketChannelParam_X.Name_S = "UdpChannel_" + std::to_string(j_U32);
    BofSocketChannelParam_X.ListenBacklog_U32 = 0;
    BofSocketChannelParam_X.IpAddress_S = BOF_NAMESPACE::Bof_Sprintf("udp://0.0.0.0:0;127.0.0.1:%d", 56000 + j_U32);
    BofSocketChannelParam_X.NotifyRcvBufferSize_U32 = 0x10000;
    BofSocketChannelParam_X.NotifyType_E = BOF_NAMESPACE::BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND;
    BofSocketChannelParam_X.Delimiter_U8 = '\n';
    BofSocketChannelParam_X.pData = nullptr;
    BofSocketChannelParam_X.RcvBufferSize_U32 = 0x10000;
    BofSocketChannelParam_X.SndBufferSize_U32 = 0x10000;
    BofSocketChannelParam_X.CloseTimeoutInMs_U32 = 2000;
    BofSocketChannelParam_X.ClosePollingTimeInMs_U32 = 20;

    BofSocketChannelParam_X.OnSocketAcceptCallback = nullptr;
    BofSocketChannelParam_X.OnSocketReadCallback = OnSocketReadCallback;
    EXPECT_EQ(psSocketChannelManager->AddChannel(BofSocketChannelParam_X, psBofSocketChannel), BOFERR_NO_ERROR);
    BofSocketChannelCollection.push_back(psBofSocketChannel);
    EXPECT_EQ(psSocketChannelManager->GetNumberOfChannel(), j_U32 + 1);
  }

  BofSocketChannelManagerStatistic_X = psSocketChannelManager->BofSocketChannelManagerStatistic();
  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncEventError_U32, 0);
  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncEvent_U32, 0);
  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommand_U32, 0);
  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommandPending_U32, 0);

  for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
  {
    EXPECT_FALSE(BOF_NAMESPACE::Bof_IsEventSignaled(pNotifyEvent_X[j_U32], 0));
    EXPECT_EQ(psSocketChannelManager->SendAsyncConnectCmd(ASYNC_CONNECT_TIMEOUT, nullptr, BofSocketChannelCollection[j_U32], &pNotifyEvent_X[j_U32]), BOFERR_NO_ERROR);
  }
  //   if (i_U32 == 0) goto NextIter;

  for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
  {
    EXPECT_EQ(BOF_NAMESPACE::Bof_WaitForEvent(pNotifyEvent_X[j_U32], ASYNC_CONNECT_TIMEOUT + (BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS / 2), 0), BOFERR_NO_ERROR); //+ To be able to check NbTimerEvent_U32
    BofSocketChannelState_X = BofSocketChannelCollection[j_U32]->BofSocketChannelState();
    EXPECT_EQ(BofSocketChannelState_X.State_E, BOF_NAMESPACE::BOF_SOCKET_STATE::CONNECTED);
  }
  //To check for memory leak   for (j_U32 = 0; j_U32 < 0x2000/10; j_U32++)
  {
    Data_S += "USER bha\r\n";
  }
  for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
  {
    EXPECT_FALSE(BOF_NAMESPACE::Bof_IsEventSignaled(pNotifyEvent_X[j_U32], 0));
    EXPECT_EQ(psSocketChannelManager->Write(ASYNC_WRITE_TIMEOUT, nullptr, BofSocketChannelCollection[j_U32], true, Data_S, &pNotifyEvent_X[j_U32]), BOFERR_NO_ERROR);
    EXPECT_EQ(BOF_NAMESPACE::Bof_WaitForEvent(pNotifyEvent_X[j_U32], ASYNC_WRITE_TIMEOUT + (BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS / 2), 0), BOFERR_NO_ERROR); //+ To be able to check NbTimerEvent_U32
    BofSocketChannelState_X = BofSocketChannelCollection[j_U32]->BofSocketChannelState();
    EXPECT_EQ(BofSocketChannelState_X.State_E, BOF_NAMESPACE::BOF_SOCKET_STATE::IO);
  }

  BOF_NAMESPACE::Bof_Sleep(50); //Give some time for echo
  for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
  {
    EXPECT_FALSE(BOF_NAMESPACE::Bof_IsEventSignaled(pNotifyEvent_X[j_U32], 0));
    EXPECT_EQ(psSocketChannelManager->SendAsyncDisconnectCmd(ASYNC_CONNECT_TIMEOUT, nullptr, BofSocketChannelCollection[j_U32], false, &pNotifyEvent_X[j_U32]), BOFERR_NO_ERROR);
    EXPECT_EQ(BOF_NAMESPACE::Bof_WaitForEvent(pNotifyEvent_X[j_U32], ASYNC_CONNECT_TIMEOUT + (BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS / 2), 0), BOFERR_NO_ERROR); //+ To be able to check NbTimerEvent_U32
  }

  psSocketChannelManager->WaitAsyncCommandQueueEmpty(20);
  for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
  {
    EXPECT_EQ(S_DataReadCollection_S[j_U32].size(), Data_S.size());
  }

  for (j_U32 = 0; j_U32 < NB_MAX_CHANNEL; j_U32++)
  {
    BofSocketChannelStatistic_X = BofSocketChannelCollection[j_U32]->BofSocketChannelStatistic();
    EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnect_U32, 1);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnectError_U32, 0);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnect_U32,  1);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnectError_U32, 0);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbOpListenError_U32, 0);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbOpRead_U32, 1);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbOpReadError_U32, 0);
    EXPECT_GE(BofSocketChannelStatistic_X.NbDataByteRead_U64, Data_S.size());
    EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWrite_U32, 0);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWriteError_U32, 0);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbDataByteWritten_U64, 0);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbDataEvent_U32, 1);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbWriteEvent_U32,  1);
    EXPECT_BETWEEN(BofSocketChannelStatistic_X.NbError_U32,  0,  0);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbCloseEvent_U32,  1);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbConnectEvent_U32, 1);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpEndEvent_U32, 0);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpShutdownEvent_U32, 0);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpListenEvent_U32, 0);
    EXPECT_BETWEEN(BofSocketChannelStatistic_X.NbTimerEvent_U32,  1,  1);
    EXPECT_EQ(BofSocketChannelStatistic_X.NbTimerEventError_U32, 0);

    EXPECT_EQ(psSocketChannelManager->RemoveChannel("UdpConnectIo", BofSocketChannelCollection[j_U32]), BOFERR_NO_ERROR);
    EXPECT_EQ(psSocketChannelManager->GetNumberOfChannel(), NB_MAX_CHANNEL - j_U32 - 1);
  }
  psSocketChannelManager->WaitAsyncCommandQueueEmpty(20);   //MANDATORY to avoid extra uv callback while deleting the remaining object

  BofSocketChannelManagerStatistic_X = psSocketChannelManager->BofSocketChannelManagerStatistic();
  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncEventError_U32, 0);
  //    EXPECT_BETWEEN(BofSocketChannelManagerStatistic_X.NbAsyncEvent_U32, 1, 2 + 2);
//  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncEvent_U32, (NB_MAX_CHANNEL * 3) + 1);
  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncEvent_U32, (NB_MAX_CHANNEL * 3));
//  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommand_U32, NB_MAX_CHANNEL * 4);
  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommand_U32, NB_MAX_CHANNEL * 3);
  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbTimerEventError_U32, 0);
  EXPECT_BETWEEN(BofSocketChannelManagerStatistic_X.NbTimerEvent_U32,  0,  2);
  // NextIter:
  EXPECT_EQ(BofSocketChannelManagerStatistic_X.NbAsyncCommandPending_U32, 0);
  
}

TEST(SocketChannelManager_Test, TcpListenConnectIo)
{
  const uint32_t NB_MAX_CHANNEL = 16;
  const uint32_t LISTEN_FOR_TIMEOUT = 10000; //Use 10000 to have time if you use filezilla client to connect
//  const uint32_t ASYNC_CONNECT_TIMEOUT = 1000;
//  const uint32_t ASYNC_WRITE_TIMEOUT = 1000;

  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_MANAGER_PARAM BofSocketChannelManagerParam_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_MANAGER_STATISTIC BofSocketChannelManagerStatistic_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_PARAM  BofSocketChannelParam_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_STATISTIC BofSocketChannelStatistic_X;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_STATE     BofSocketChannelState_X;
  BOF_NAMESPACE::BOF_EVENT pNotifyEvent_X[2];
  std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> psBofSocketChannel;
  std::vector<BOF_NAMESPACE::BOF_BUFFER> BufferCollection;
  uint32_t i_U32, j_U32;
  std::string Data_S;

  for (j_U32 = 0; j_U32 < BOF_NB_ELEM_IN_ARRAY(S_DataReadCollection_S); j_U32++)
  {
    S_DataReadCollection_S[j_U32] = "";
  }
  S_BofSocketChannelAcceptedCollection.clear();

  BofSocketChannelManagerParam_X.Name_S = "Manager";
  BofSocketChannelManagerParam_X.NbMaxChannel_U32 = NB_MAX_CHANNEL+1;   //+1 for listener
  BofSocketChannelManagerParam_X.Policy_S32 = BOF_NAMESPACE::BOF_THREAD_POLICY_OTHER;
  BofSocketChannelManagerParam_X.Priority_S32 = BOF_NAMESPACE::BOF_THREAD_DEFAULT_PRIORITY;
  BofSocketChannelManagerParam_X.StartStopTimeoutInMs_U32 = 5000;
  BofSocketChannelManagerParam_X.CloseTimeoutInMs_U32 = 1000;
  BofSocketChannelManagerParam_X.ClosePollingTimeInMs_U32 = 20;

  std::vector<std::shared_ptr<BOF_NAMESPACE::BofSocketChannel>> BofSocketChannelCollection;
  std::shared_ptr<BOF_NAMESPACE::BofSocketChannelManager> psSocketChannelManager = BOF_NAMESPACE::BofSocketChannelManager::S_BofSocketChannelManagerFactory(BofSocketChannelManagerParam_X);
  EXPECT_EQ(psSocketChannelManager->InitErrorCode(), BOFERR_NO_ERROR);

//Use filezilla to connect on first port 55000
  for (i_U32 = 0; i_U32 < 2; i_U32++)
  {
    BofSocketChannelParam_X.ChannelId_U32 = i_U32;
    BofSocketChannelParam_X.Name_S = "TcpListenChannel_" + std::to_string(55000 + i_U32) + "_" + std::to_string(i_U32);
    BofSocketChannelParam_X.ListenBacklog_U32 = NB_MAX_CHANNEL;
    BofSocketChannelParam_X.IpAddress_S = BOF_NAMESPACE::Bof_Sprintf("tcp://0.0.0.0:%d;0.0.0.0:0", 55000 + i_U32);  //For listener, the listening port is on the interface address
    BofSocketChannelParam_X.NotifyRcvBufferSize_U32 = 0x10000;  //For listener, this parameter is used for client creation parameter. Its value can be changed later during OnSocketAcceptCallback callback processing
    BofSocketChannelParam_X.NotifyType_E = BOF_NAMESPACE::BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND; //For listener, this parameter is used for client creation parameter. Its value can be changed later during OnSocketAcceptCallback callback processing
    BofSocketChannelParam_X.Delimiter_U8 = '\n';  //For listener, this parameter is used for client creation parameter. Its value can be changed later during OnSocketAcceptCallback callback processing
    BofSocketChannelParam_X.pData = nullptr;  //For listener, this parameter is used for client creation parameter. Its value can be changed later during OnSocketAcceptCallback callback processing
    BofSocketChannelParam_X.RcvBufferSize_U32 = 0x10000;  //For listener, this parameter is used for client creation parameter. Its value can be changed later during OnSocketAcceptCallback callback processing
    BofSocketChannelParam_X.SndBufferSize_U32 = 0x10000;  //For listener, this parameter is used for client creation parameter. Its value can be changed later during OnSocketAcceptCallback callback processing
    BofSocketChannelParam_X.CloseTimeoutInMs_U32 = 2000;
    BofSocketChannelParam_X.ClosePollingTimeInMs_U32 = 20;

    BofSocketChannelParam_X.OnSocketAcceptCallback = OnSocketAcceptCallback;
    BofSocketChannelParam_X.OnSocketReadCallback = OnSocketReadCallback;  //For listener, this parameter is used for client creation parameter. Its value can be changed later during OnSocketAcceptCallback callback processing
    EXPECT_EQ(psSocketChannelManager->AddChannel(BofSocketChannelParam_X, psBofSocketChannel), BOFERR_NO_ERROR);
    //BofSocketChannelCollection.push_back(psBofSocketChannel);
    EXPECT_EQ(psSocketChannelManager->GetNumberOfChannel(), (i_U32 == 0) ? 1: (S_BofSocketChannelAcceptedCollection.size() == 1) ? 3:1);  //0: Listen0 1: Listen0,ClientListen0,Listen1 

    BOF_NAMESPACE::Bof_CreateEvent("LisConEvnt_" + std::to_string(i_U32), false, 1, false, pNotifyEvent_X[i_U32]);
    EXPECT_FALSE(BOF_NAMESPACE::Bof_IsEventSignaled(pNotifyEvent_X[i_U32], 0));
    EXPECT_EQ(psSocketChannelManager->SendAsyncListenCmd(LISTEN_FOR_TIMEOUT, nullptr, psBofSocketChannel, 50000, 50500, &pNotifyEvent_X[i_U32]), BOFERR_NO_ERROR);

    EXPECT_EQ(BOF_NAMESPACE::Bof_WaitForEvent(pNotifyEvent_X[i_U32], LISTEN_FOR_TIMEOUT, 0), BOFERR_NO_ERROR);
    BofSocketChannelState_X = psBofSocketChannel->BofSocketChannelState();
    if (S_BofSocketChannelAcceptedCollection.size() == 1) //Time or not to use filezilla
    {
      EXPECT_EQ(BofSocketChannelState_X.State_E, (i_U32 == 0) ? BOF_NAMESPACE::BOF_SOCKET_STATE::LISTENING : BOF_NAMESPACE::BOF_SOCKET_STATE::LISTENING_TIMEOUT);
    }
    BOF_NAMESPACE::Bof_Sleep(100);
    if (i_U32 == 0)
    {
      if (S_BofSocketChannelAcceptedCollection.size() == 1) //Time or not to use filezilla
      {
        BofSocketChannelState_X = S_BofSocketChannelAcceptedCollection[i_U32]->BofSocketChannelState();
        EXPECT_EQ(BofSocketChannelState_X.State_E, BOF_NAMESPACE::BOF_SOCKET_STATE::IO);
        BofSocketChannelStatistic_X = S_BofSocketChannelAcceptedCollection[i_U32]->BofSocketChannelStatistic();
        EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnect_U32, 0);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnectError_U32, 0);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnect_U32, 0);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnectError_U32, 0);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbOpListenError_U32, 0);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbOpRead_U32, 8);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbOpReadError_U32, 0);
        EXPECT_GE(BofSocketChannelStatistic_X.NbDataByteRead_U64, 68);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWrite_U32, 0);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWriteError_U32, 0);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbDataByteWritten_U64, 0);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbDataEvent_U32, BofSocketChannelStatistic_X.NbOpRead_U32);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbWriteEvent_U32, BofSocketChannelStatistic_X.NbOpRead_U32);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbError_U32, 0);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbCloseEvent_U32, 0);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbConnectEvent_U32, 0);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpEndEvent_U32, 0);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpShutdownEvent_U32, 0);
        EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpListenEvent_U32, 0);
        //    EXPECT_BETWEEN(BofSocketChannelStatistic_X.NbTimerEvent_U32, 1, (LISTEN_FOR_TIMEOUT / BOF_NAMESPACE::TIMER_GRANULARITY_IN_MS));
        EXPECT_EQ(BofSocketChannelStatistic_X.NbTimerEventError_U32, 0);
      }
    }
    else
    {
  //    EXPECT_EQ(S_BofSocketChannelAcceptedCollection.size(),1);
    }
  }
  //S_BofSocketChannelAcceptedCollection.clear();
}