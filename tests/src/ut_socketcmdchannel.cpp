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
#include <bofstd/bofsocketcmdchannel.h>
#include <bofstd/bofstringformatter.h>
#include <bofstd/bofstring.h>
#include <bofstd/bofsocketos.h>

std::vector<std::shared_ptr<BOF_NAMESPACE::BofSocketChannel>> S_BofSocketChannelAcceptedCollection2;
static std::string S_DataReadCollection_S[32];
static std::string S_DataReadAcceptedCollection_S[32];

BOFERR OnSocketReadAcceptedCallback2(uint32_t _Id_U32, std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> _psBofSocketChannel, const BOF_NAMESPACE::BOF_BUFFER &_rBuffer_X, const BOF_NAMESPACE::BOF_SOCKET_PEER_INFO &/*_rPeerInfo_X*/)
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
BOFERR OnSocketAcceptCallback2(uint32_t _Id_U32, std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> _psBofSocketChannel, const BOF_NAMESPACE::BOF_SOCKET_PEER_INFO &/*_rPeerInfo_X*/, BOF_NAMESPACE::BOF_SOCKET_CHANNEL_PARAM *_pBofSocketChannelParam_X)
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
      _pBofSocketChannelParam_X->OnSocketReadCallback = OnSocketReadAcceptedCallback2;
      _pBofSocketChannelParam_X->pData = nullptr;
      _pBofSocketChannelParam_X->RcvBufferSize_U32 *= 2;
      _pBofSocketChannelParam_X->SndBufferSize_U32 *= 2;
    }
    Rts_E=_psBofSocketChannel->WriteData(BOF_NAMESPACE::MIN_TIMEOUT_VALUE_IN_MS,true, "220-Gns Server 1.0.0 beta\r\n220-written by Bernard HARMEL (b.harmel@gmail.com)\r\n220 Please visit http://onbings.dscloud.me/\r\n");
    S_BofSocketChannelAcceptedCollection2.push_back(_psBofSocketChannel);
  }
  return Rts_E;
}

BOFERR OnSocketReadCallback2(uint32_t _Id_U32, std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> /*_psBofSocketChannel*/, const BOF_NAMESPACE::BOF_BUFFER &_rBuffer_X, const BOF_NAMESPACE::BOF_SOCKET_PEER_INFO &/*_rPeerInfo_X*/)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;

  if (_Id_U32 < BOF_NB_ELEM_IN_ARRAY(S_DataReadCollection_S))
  {
    S_DataReadCollection_S[_Id_U32] += std::string(_rBuffer_X.pData_U8, _rBuffer_X.pData_U8 + _rBuffer_X.SizeInByte_U64);
  }
  return Rts_E;
}

//Need filezilla or ft server running
TEST(SocketCmdChannel_Test, CmdChannel)
{
  const uint32_t NB_MAX_CHANNEL = 16;
  const uint32_t ASYNC_CONNECT_TIMEOUT = 1000;
//  const uint32_t ASYNC_WRITE_TIMEOUT = 1000;

  std::shared_ptr<BOF_NAMESPACE::BofSocketChannelManager> psSocketChannelManager;
  std::shared_ptr<BOF_NAMESPACE::BofSocketCmdChannel> psBofSocketCmdChannel;
  BOF_NAMESPACE::BOF_SOCKET_CHANNEL_MANAGER_PARAM BofSocketChannelManagerParam_X;
  BOF_NAMESPACE::BOF_SOCKET_CMD_CHANNEL_PARAM  BofSocketCmdChannelParam_X;
  uint32_t i_U32;
  BOFERR Sts_E;

  BofSocketChannelManagerParam_X.Name_S = "Manager";
  BofSocketChannelManagerParam_X.NbMaxChannel_U32 = NB_MAX_CHANNEL;
  BofSocketChannelManagerParam_X.Policy_S32 = BOF_NAMESPACE::BOF_THREAD_POLICY_OTHER;
  BofSocketChannelManagerParam_X.Priority_S32 = BOF_NAMESPACE::BOF_THREAD_DEFAULT_PRIORITY;
  BofSocketChannelManagerParam_X.StartStopTimeoutInMs_U32 = 5000;
  BofSocketChannelManagerParam_X.CloseTimeoutInMs_U32 = 1000;
  BofSocketChannelManagerParam_X.ClosePollingTimeInMs_U32 = 20;

  psSocketChannelManager = BOF_NAMESPACE::BofSocketChannelManager::S_BofSocketChannelManagerFactory(BofSocketChannelManagerParam_X);
  EXPECT_EQ(psSocketChannelManager->InitErrorCode(), BOFERR_NO_ERROR);
  for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(S_DataReadCollection_S); i_U32++)
  {
    S_DataReadCollection_S[i_U32] = "";
  }
  for (i_U32 = 0; i_U32 < NB_MAX_CHANNEL; i_U32++)
  {
    BofSocketCmdChannelParam_X.ChannelId_U32 = i_U32;
    BofSocketCmdChannelParam_X.Name_S = "TcpChannel_" + std::to_string(i_U32);
    BofSocketCmdChannelParam_X.IpAddress_S = BOF_NAMESPACE::Bof_Sprintf("tcp://0.0.0.0:0;127.0.0.1:%d", 21);
//    BofSocketChannelParam_X.OnSocketAcceptCallback = nullptr;
////TODO    BofSocketCmdChannelParam_X.OnSocketReadCallback = OnSocketReadCallback2;
    psBofSocketCmdChannel = BOF_NAMESPACE::BofSocketCmdChannel::S_BofSocketCmdChannelFactory(psSocketChannelManager, BofSocketCmdChannelParam_X);
    EXPECT_TRUE(psBofSocketCmdChannel != nullptr);
    EXPECT_EQ(psBofSocketCmdChannel->LastErrorCode(), BOFERR_NO_ERROR);
    EXPECT_FALSE(psBofSocketCmdChannel->IsConnected());
    EXPECT_FALSE(psBofSocketCmdChannel->IsLogin());

    Sts_E=psBofSocketCmdChannel->Connect(ASYNC_CONNECT_TIMEOUT, "127.0.0.1");
    EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
    EXPECT_TRUE(psBofSocketCmdChannel->IsConnected());
    EXPECT_FALSE(psBofSocketCmdChannel->IsLogin());
    Sts_E = psBofSocketCmdChannel->Login(ASYNC_CONNECT_TIMEOUT, "bha", "azert");
    EXPECT_NE(Sts_E, BOFERR_NO_ERROR);
    EXPECT_TRUE(psBofSocketCmdChannel->IsConnected());
    EXPECT_FALSE(psBofSocketCmdChannel->IsLogin());

    Sts_E = psBofSocketCmdChannel->Login(ASYNC_CONNECT_TIMEOUT, "bh", "azerty");
    EXPECT_NE(Sts_E, BOFERR_NO_ERROR);
    EXPECT_TRUE(psBofSocketCmdChannel->IsConnected());
    EXPECT_FALSE(psBofSocketCmdChannel->IsLogin());

    Sts_E = psBofSocketCmdChannel->Login(ASYNC_CONNECT_TIMEOUT, "bha", "azerty");
    EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
    EXPECT_TRUE(psBofSocketCmdChannel->IsConnected());
    EXPECT_TRUE(psBofSocketCmdChannel->IsLogin());

  }
}
