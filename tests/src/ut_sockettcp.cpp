/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the tcp socket comchannel class
 *
 * Name:        ut_sockettcp.cpp
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
#include <bofstd/bofsocket.h>
#include <bofstd/bofthread.h>

#include "gtestrunner.h"

#include <inttypes.h>

USE_BOF_NAMESPACE()

struct SERVER_THREAD_CONTEXT
{
  BOF_SOCKET_PARAM BofSocketParam_X;

  SERVER_THREAD_CONTEXT()
  {
    Reset();
  }
  void Reset()
  {
    BofSocketParam_X.Reset();
  }
};

// To use a test fixture, derive from testing::Test class
class SocketTcp_Test : public testing::Test
{
public:
  // Per-test-case set-up. Called before the first test in this test case.
  static void SetUpTestCase();

  // Per-test-case tear-down. Called after the last test in this test case.
  static void TearDownTestCase();

protected:
  // You can define per-test set-up and tear-down logic as usual.
  virtual void SetUp();
  virtual void TearDown();

private:
  SERVER_THREAD_CONTEXT mServerThreadContext_X;
  BOF_THREAD mSeverThread_X;
};

static uint64_t S_TotalSrvTcp_U64 = 0, S_TotalCltTcp_U64 = 0;

static BOFERR S_TcpServerThread(const std::atomic<bool> &_rThreadMustStop_B, void *_pThreadContext)
{
  BOFERR Rts_E;
  SERVER_THREAD_CONTEXT *pThreadContext_X = (SERVER_THREAD_CONTEXT *)_pThreadContext;
  BofSocket ListeningSocket;
  BofComChannel *pClient;
  std::vector<std::unique_ptr<BofComChannel>> ClientCollection;
  BOF_COM_CHANNEL_STATUS Status_X;
  uint8_t pBuffer_U8[0x10000];
  uint32_t i_U32, Nb_U32, NbToRead_U32;

  BOF_ASSERT(pThreadContext_X != nullptr);
  Rts_E = ListeningSocket.InitializeSocket(pThreadContext_X->BofSocketParam_X);
  EXPECT_EQ(Rts_E, BOF_ERR_NO_ERROR);
  while (!_rThreadMustStop_B)
  {
    pClient = ListeningSocket.V_Listen(0, "");
    if (pClient)
    {
      BofSocket *pBofSocket = dynamic_cast<BofSocket *>(pClient);
      Rts_E = pBofSocket->V_WriteData(1000, "Hello World\n", Nb_U32);
      EXPECT_EQ(Rts_E, BOF_ERR_NO_ERROR);
      EXPECT_EQ(Nb_U32, 12);
      Rts_E = pBofSocket->V_WriteData(1000, "azerty QWERTY\n", Nb_U32);
      EXPECT_EQ(Rts_E, BOF_ERR_NO_ERROR);
      EXPECT_EQ(Nb_U32, 14);

      ClientCollection.push_back(std::unique_ptr<BofComChannel>(pClient));
    }
    for (i_U32 = 0; i_U32 < ClientCollection.size(); i_U32++)
    {
      BOF_ASSERT(ClientCollection[i_U32] != nullptr);
      Rts_E = ClientCollection[i_U32]->V_GetStatus(Status_X);
      EXPECT_EQ(Rts_E, BOF_ERR_NO_ERROR);
      if (Status_X.NbIn_U32)
      {
        Nb_U32 = Status_X.NbIn_U32 > sizeof(pBuffer_U8) ? sizeof(pBuffer_U8) : Status_X.NbIn_U32;
        BOF_ASSERT(Nb_U32 <= sizeof(pBuffer_U8));
        NbToRead_U32 = Nb_U32;
        Rts_E = ClientCollection[i_U32]->V_ReadData(1000, NbToRead_U32, pBuffer_U8);
        //				printf("Client %d/%lld srv Rd n %d s %d Total %lld -> %lld\n", i_U32, ClientCollection.size(), NbToRead_U32, Rts_E, S_TotalSrvTcp_U64, S_TotalSrvTcp_U64 + Nb_U32 + Nb_U32);

        S_TotalSrvTcp_U64 += NbToRead_U32;

        EXPECT_EQ(Rts_E, BOF_ERR_NO_ERROR);
        EXPECT_EQ(Nb_U32, NbToRead_U32);

        Rts_E = ClientCollection[i_U32]->V_WriteData(1000, Nb_U32, pBuffer_U8);
        //				printf("srv Wrt n %d s %d\n", Nb_U32, Rts_E);
        S_TotalSrvTcp_U64 += Nb_U32;

        EXPECT_EQ(Rts_E, BOF_ERR_NO_ERROR);
        EXPECT_EQ(Nb_U32, NbToRead_U32);
      }
    }
  }
  // ClientCollection is a vector of unique pointer->deallocated on return of this function
  // ClientCollection.clear();
  // Any other error code different from BOF_ERR_NO_ERROR will exit the tread loop
  // Returning BOF_ERR_EXIT_THREAD will exit the thread loop with an exit code of BOF_ERR_NO_ERROR
  // Thread will be stopped if someone calls Bof_StopThread
  Rts_E = BOF_ERR_EXIT_THREAD;
  return Rts_E;
}

void SocketTcp_Test::SetUpTestCase()
{
}

void SocketTcp_Test::TearDownTestCase()
{
}
void SocketTcp_Test::SetUp()
{
  BOFERR Sts_E;

  mServerThreadContext_X.BofSocketParam_X.BaseChannelParam_X.ChannelName_S = "TcpServer";
  mServerThreadContext_X.BofSocketParam_X.BaseChannelParam_X.Blocking_B = true;
  mServerThreadContext_X.BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 64;      // 0->Client
  mServerThreadContext_X.BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x40000; // 0x1000000;
  mServerThreadContext_X.BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x40000; // 0x1000000;
  mServerThreadContext_X.BofSocketParam_X.BindIpAddress_S = "tcp://127.0.0.1:5555";
  mServerThreadContext_X.BofSocketParam_X.ReUseAddress_B = false;
  mServerThreadContext_X.BofSocketParam_X.NoDelay_B = true;
  mServerThreadContext_X.BofSocketParam_X.Ttl_U32 = 32;
  mServerThreadContext_X.BofSocketParam_X.BroadcastPort_U16 = 0;
  mServerThreadContext_X.BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
  mServerThreadContext_X.BofSocketParam_X.MulticastSender_B = false;
  mServerThreadContext_X.BofSocketParam_X.KeepAlive_B = false;
  mServerThreadContext_X.BofSocketParam_X.EnableLocalMulticast_B = false;

  Sts_E = Bof_CreateThread("ServerThread", S_TcpServerThread, &mServerThreadContext_X, mSeverThread_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_StartThread(mSeverThread_X, 0x40000, 0, BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF_THREAD_PRIORITY_000, 1000);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

void SocketTcp_Test::TearDown()
{
  BOFERR Sts_E;
  Sts_E = Bof_StopThread(mSeverThread_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

const uint32_t SERVER_NB_CLIENT = 8; // 50;
const uint32_t CLIENT_NB_LOOP = 100; // 500;

TEST_F(SocketTcp_Test, TcpClientTest)
{
  std::vector<std::unique_ptr<BofSocket>> ClientCollection;

  BOFERR Sts_E;
  BOF_SOCKET_PARAM BofSocketParam_X;
  uint64_t Total_U64, TotalSrvClt_U64;
  uint32_t Nb_U32, i_U32, j_U32, Start_U32, StartWaitEof_U32, Delta_U32, DeltaWaitEof_U32, KBPerSec_U32;
  uint8_t pBuffer_U8[0x10000];
  BOF_COM_CHANNEL_STATUS Status_X;
  std::string Str_S;

  BOF::Bof_MsSleep(20);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 1);

  for (i_U32 = 0; i_U32 < SERVER_NB_CLIENT; i_U32++)
  {
    std::unique_ptr<BofSocket> puClientSocket = std::make_unique<BofSocket>();

    BofSocketParam_X.BaseChannelParam_X.ChannelName_S = "TcpClient_" + std::to_string(i_U32);
    BofSocketParam_X.BaseChannelParam_X.Blocking_B = true;
    BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;       // 0->Client
    BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x40000; // 0x1000000;
    BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x40000; // 0x1000000;

    BofSocketParam_X.BindIpAddress_S = "tcp://127.0.0.1:0";
    BofSocketParam_X.ReUseAddress_B = false;
    BofSocketParam_X.NoDelay_B = true;
    BofSocketParam_X.Ttl_U32 = 32;
    BofSocketParam_X.BroadcastPort_U16 = 0;
    BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
    BofSocketParam_X.MulticastSender_B = false;
    BofSocketParam_X.KeepAlive_B = false;
    BofSocketParam_X.EnableLocalMulticast_B = false;
    Sts_E = puClientSocket->InitializeSocket(BofSocketParam_X);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

    // printf("create sck %d bal %d\n", puClientSocket->GetSocketHandle(), BOF::BofSocket::S_BofSocketBalance());
    ClientCollection.push_back(std::move(puClientSocket));
  }
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), SERVER_NB_CLIENT + 1); //+1 because S_TcpServerThread creatre a listening socket

  for (i_U32 = 0; i_U32 < ClientCollection.size(); i_U32++)
  {
    BOF_ASSERT(ClientCollection[i_U32] != nullptr);
    Sts_E = ClientCollection[i_U32]->V_Connect(100, "tcp://127.0.0.1:5555", "");
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

    Sts_E = ClientCollection[i_U32]->ReadString(10000, Str_S, '\n');
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_STREQ("Hello World\n", Str_S.c_str());
    Sts_E = ClientCollection[i_U32]->ReadString(10000, Str_S, '@');
    EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_STREQ("Hello World\n", Str_S.c_str());
    Sts_E = ClientCollection[i_U32]->ReadString(10000, Str_S, ' ');
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_STREQ("azerty ", Str_S.c_str());
    Sts_E = ClientCollection[i_U32]->ReadString(10000, Str_S, '\n');
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_STREQ("QWERTY\n", Str_S.c_str());
  }
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), SERVER_NB_CLIENT + 1 + ClientCollection.size()); //+1 because S_TcpServerThread creatre a listening socket

  Nb_U32 = sizeof(pBuffer_U8);
  for (j_U32 = 0; j_U32 < Nb_U32; j_U32++)
  {
    pBuffer_U8[j_U32] = static_cast<uint8_t>(j_U32);
  }
  S_TotalSrvTcp_U64 = 0;
  S_TotalCltTcp_U64 = 0;
  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < ClientCollection.size(); i_U32++)
  {
    for (j_U32 = 0; j_U32 < CLIENT_NB_LOOP; j_U32++)
    {
      Nb_U32 = sizeof(pBuffer_U8);
      Sts_E = ClientCollection[i_U32]->V_WriteData(100, Nb_U32, pBuffer_U8);
      S_TotalCltTcp_U64 += Nb_U32;
      //	printf("clt Wrt %d: n %d s %d\n", j_U32, Nb_U32, Sts_E);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_EQ(Nb_U32, sizeof(pBuffer_U8));
      //			printf("client %d W %d->%lld\n", i_U32, Nb_U32, S_TotalCltTcp_U64);

      Nb_U32 = sizeof(pBuffer_U8);
      Sts_E = ClientCollection[i_U32]->V_ReadData(100, Nb_U32, pBuffer_U8);
      S_TotalCltTcp_U64 += Nb_U32;
      //	printf("clt Rd n %d s %d\n", Nb_U32, Sts_E);

      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      // EXPECT_EQ(Nb_U32, sizeof(pBuffer_U8));

      // printf("Con %d NbOut %d Flg %x Sts %d\n", Status_X.Connected_B, Status_X.NbOut_U32, Status_X.Flag_U32, Status_X.Sts_E);

      Sts_E = ClientCollection[i_U32]->V_GetStatus(Status_X);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      if (Status_X.NbIn_U32)
      {
      }
      // Connect is async and siconnect at the end			EXPECT_TRUE(Status_X.Connected_B);
      //			EXPECT_EQ(Status_X.Sts_E, BOF_ERR_NO_ERROR);
      //			EXPECT_EQ(Status_X.NbOut_U32, 0);
      //			EXPECT_EQ(Status_X.Flag_U32, 0);
    }
  }
  Total_U64 = (2L * (static_cast<uint64_t>(SERVER_NB_CLIENT) * static_cast<uint64_t>(CLIENT_NB_LOOP) * sizeof(pBuffer_U8))) * 2L;
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);

  StartWaitEof_U32 = Bof_GetMsTickCount();
  DeltaWaitEof_U32 = 0;
  while ((S_TotalSrvTcp_U64 + S_TotalCltTcp_U64) != Total_U64)
  {
    for (i_U32 = 0; i_U32 < ClientCollection.size(); i_U32++)
    {
      Nb_U32 = sizeof(pBuffer_U8);
      Sts_E = ClientCollection[i_U32]->V_ReadData(10, Nb_U32, pBuffer_U8);
      if (Sts_E == BOF_ERR_NO_ERROR)
      {
        // printf("extra rd clt %d nb %d Srv %" PRId64 "Clt %" PRId64 "\n", i_U32, Nb_U32, S_TotalSrvTcp_U64, S_TotalCltTcp_U64);
        S_TotalCltTcp_U64 += Nb_U32;
      }
      Sts_E = ClientCollection[i_U32]->V_GetStatus(Status_X);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      // EXPECT_TRUE(Status_X.Connected_B);
      // EXPECT_EQ(Status_X.Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_EQ(Status_X.NbOut_U32, 0);
      EXPECT_EQ(Status_X.Flag_U32, 0);
    }
    DeltaWaitEof_U32 = Bof_ElapsedMsTime(StartWaitEof_U32);
    TotalSrvClt_U64 = (S_TotalSrvTcp_U64 + S_TotalCltTcp_U64);
    // printf("Total %" PRId64 "/%" PRId64 " SizeDelta %" PRId64 " NbTrf %f Srv %" PRId64 " Clt %" PRId64 "\n", Total_U64, TotalSrvClt_U64, Total_U64 - TotalSrvClt_U64, (float)(Total_U64 - TotalSrvClt_U64) / (float)sizeof(pBuffer_U8), S_TotalSrvTcp_U64, S_TotalCltTcp_U64);
    if (DeltaWaitEof_U32 > 3000)
    {
      break;
    }
  }
  TotalSrvClt_U64 = (S_TotalSrvTcp_U64 + S_TotalCltTcp_U64);
  // printf("SizeDelta %" PRId64 " NbTrf %f\n", Total_U64 - TotalSrvClt_U64, (float)(Total_U64 - TotalSrvClt_U64)/(float)sizeof(pBuffer_U8));
  EXPECT_EQ(TotalSrvClt_U64, Total_U64);
  KBPerSec_U32 = (Delta_U32) ? static_cast<uint32_t>((TotalSrvClt_U64 * 1000) / 1024L) / Delta_U32 : 0;
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  // printf("%d client %d loop %d KB %d MB in %d ms (extra ms %d)->%d KB/S %d MB/S\n", SERVER_NB_CLIENT, CLIENT_NB_LOOP, static_cast<uint32_t>(TotalSrvClt_U64 / 1024L), static_cast<uint32_t>(TotalSrvClt_U64 / 1024L / 1024L), Delta_U32, DeltaWaitEof_U32,KBPerSec_U32, KBPerSec_U32 / 1024);

  for (i_U32 = 0; i_U32 < ClientCollection.size(); i_U32++)
  {
    Sts_E = ClientCollection[i_U32]->V_FlushData(10);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    //    EXPECT_GE(Bof_ElapsedMsTime(Start_U32), static_cast<uint32_t>(500));
    Sts_E = ClientCollection[i_U32]->V_FlushData(10);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  }

  // All client and server session vector of unique pointer->deallocated on return of this function and the exit of server listening thread
  // Socket level is finally checked in the next test function ChkSocketBalance
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), SERVER_NB_CLIENT + 1 + ClientCollection.size()); //+1 because S_TcpServerThread creatre a listening socket
}

TEST_F(SocketTcp_Test, ChkSocketBalance)
{
  Bof_MsSleep(20);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 1);
}
#if 0
TEST(SocketTcp_Test, BasicUdpNonBlockingSocket)
{
  BofSocket BofSocketDef;
  BOFERR Sts_E;
  BOF_SOCKET_PARAM BofSocketParam_X;
  uint32_t Nb_U32, i_U32, Start_U32;
  uint8_t pBuffer_U8[0x10000];
  BOF_SOCKET_ADDRESS TargetAddress_X;
  BOF_COM_CHANNEL_STATUS Status_X;
  BofComChannel *pClient;

  EXPECT_EQ(BofSocketDef.GetSocketHandle(), BOFSOCKET_INVALID);
  EXPECT_EQ(BofSocketDef.GetMaxUdpLen(), 0);
  EXPECT_FALSE(BofSocketDef.IsTcp());
  EXPECT_FALSE(BofSocketDef.IsUdp());
  EXPECT_EQ(BofSocketDef.LastErrorCode(), BOF_ERR_NO_ERROR);

  BofSocketParam_X.Reset();
  BofSocketParam_X.BaseChannelParam_X.ChannelName_S = "MyDefSocket";
  BofSocketParam_X.BaseChannelParam_X.Blocking_B = false;
  BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 3;                               // 0->Client
  BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x10000;
  BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x10000;
#if defined(_WIN32)
  BofSocketParam_X.BindIpAddress_S = "udp://10.129.170.30:5555";
#else
  BofSocketParam_X.BindIpAddress_S = "udp://10.129.170.21:5555";
#endif
  BofSocketParam_X.ReUseAddress_B = false;
  BofSocketParam_X.NoDelay_B = true;
  BofSocketParam_X.Ttl_U32 = 32;
  BofSocketParam_X.BroadcastPort_U16 = 0;
  BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
  BofSocketParam_X.MulticastSender_B = false;
  BofSocketParam_X.KeepAlive_B = false;
  BofSocketParam_X.EnableLocalMulticast_B = false;
  Sts_E = BofSocketDef.InitializeSocket(BofSocketParam_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(BofSocketDef.GetSocketHandle(), (BOFSOCKET)0);
  EXPECT_GE(BofSocketDef.GetMaxUdpLen(), (uint32_t)0xFF00);
  EXPECT_FALSE(BofSocketDef.IsTcp());
  EXPECT_TRUE(BofSocketDef.IsUdp());
  EXPECT_EQ(BofSocketDef.LastErrorCode(), BOF_ERR_NO_ERROR);

  pClient = BofSocketDef.V_Listen(100, "");
  EXPECT_TRUE(pClient != nullptr);

#if defined(_WIN32)
  Sts_E = BofSocketDef.V_Connect(100, "udp://10.129.170.21:5556", "");
#else
  Sts_E = BofSocketDef.V_Connect(100, "udp://10.129.170.30:5556", "");
#endif
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Nb_U32 = sizeof(pBuffer_U8);
  for (i_U32 = 0; i_U32 < Nb_U32; i_U32++)
  {
    pBuffer_U8[i_U32] = static_cast<uint8_t>(i_U32);
  }
  for (i_U32 = 0; i_U32 < 10; i_U32++)
  {
    Nb_U32 = sizeof(pBuffer_U8);
    Sts_E = BofSocketDef.V_WriteData(100, Nb_U32, pBuffer_U8);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(Nb_U32, sizeof(pBuffer_U8));
  }
  Start_U32 = Bof_GetMsTickCount();
  Nb_U32 = sizeof(pBuffer_U8);
  Sts_E = BofSocketDef.V_ReadData(1000, Nb_U32, pBuffer_U8);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Nb_U32, 0);
  EXPECT_GE(Bof_ElapsedMsTime(Start_U32), static_cast<uint32_t>(1000));
  EXPECT_LT(Bof_ElapsedMsTime(Start_U32), static_cast<uint32_t>(1100));

  Sts_E = BofSocketDef.V_GetStatus(Status_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(Status_X.Connected_B);
  EXPECT_EQ(Status_X.NbIn_U32, 0);
  EXPECT_EQ(Status_X.NbOut_U32, 0);
  EXPECT_EQ(Status_X.Flag_U32, 0);
  EXPECT_EQ(Status_X.Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = BofSocketDef.V_FlushData(500);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  //BOF_SET_SOCKET_ADDRESS(TargetAddress_X, BOF_SOCK_TYPE::BOF_SOCK_STREAM, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP, 127, 0, 0, 1, 5555);
}

TEST(SocketTcp_Test, BasicUdpBlockingSocket)
{
  BofSocket BofSocketDef;
  BOFERR Sts_E;
  BOF_SOCKET_PARAM BofSocketParam_X;
  uint32_t Nb_U32, i_U32, Start_U32;
  uint8_t pBuffer_U8[0x10000];
  BOF_SOCKET_ADDRESS TargetAddress_X;
  BOF_COM_CHANNEL_STATUS Status_X;
  BofComChannel *pClient;

  BofSocketParam_X.Reset();
  BofSocketParam_X.BaseChannelParam_X.ChannelName_S = "MyDefSocket";
  BofSocketParam_X.BaseChannelParam_X.Blocking_B = true;
  BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 3;                               // 0->Client
  BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x10000;
  BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x10000;

#if defined(_WIN32)
  BofSocketParam_X.BindIpAddress_S = "udp://10.129.170.30:5555";
#else
  BofSocketParam_X.BindIpAddress_S = "udp://10.129.170.21:5555";
#endif
  BofSocketParam_X.ReUseAddress_B = false;
  BofSocketParam_X.NoDelay_B = true;
  BofSocketParam_X.Ttl_U32 = 32;
  BofSocketParam_X.BroadcastPort_U16 = 0;
  BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
  BofSocketParam_X.MulticastSender_B = false;
  BofSocketParam_X.KeepAlive_B = false;
  BofSocketParam_X.EnableLocalMulticast_B = false;
  Sts_E = BofSocketDef.InitializeSocket(BofSocketParam_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(BofSocketDef.GetSocketHandle(), (BOFSOCKET)0);
  EXPECT_GE(BofSocketDef.GetMaxUdpLen(), (uint32_t)0xFF00);
  EXPECT_FALSE(BofSocketDef.IsTcp());
  EXPECT_TRUE(BofSocketDef.IsUdp());
  EXPECT_EQ(BofSocketDef.LastErrorCode(), BOF_ERR_NO_ERROR);

  pClient = BofSocketDef.V_Listen(100, "");
  EXPECT_TRUE(pClient != nullptr);

#if defined(_WIN32)
  Sts_E = BofSocketDef.V_Connect(100, "udp://10.129.170.21:5556", "");
#else
  Sts_E = BofSocketDef.V_Connect(100, "udp://10.129.170.30:5556", "");
#endif
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Nb_U32 = sizeof(pBuffer_U8);
  for (i_U32 = 0; i_U32 < Nb_U32; i_U32++)
  {
    pBuffer_U8[i_U32] = static_cast<uint8_t>(i_U32);
  }
  for (i_U32 = 0; i_U32 < 10; i_U32++)
  {
    Nb_U32 = sizeof(pBuffer_U8);
    Sts_E = BofSocketDef.V_WriteData(100, Nb_U32, pBuffer_U8);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(Nb_U32, sizeof(pBuffer_U8));
  }
  Start_U32 = Bof_GetMsTickCount();
  Nb_U32 = sizeof(pBuffer_U8);
  Sts_E = BofSocketDef.V_ReadData(1000, Nb_U32, pBuffer_U8);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Nb_U32, 0);
  EXPECT_GE(Bof_ElapsedMsTime(Start_U32), static_cast<uint32_t>(1000));
  EXPECT_LT(Bof_ElapsedMsTime(Start_U32), static_cast<uint32_t>(1100));

  Sts_E = BofSocketDef.V_GetStatus(Status_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(Status_X.Connected_B);
  EXPECT_EQ(Status_X.NbIn_U32, 0);
  EXPECT_EQ(Status_X.NbOut_U32, 0);
  EXPECT_EQ(Status_X.Flag_U32, 0);
  EXPECT_EQ(Status_X.Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = BofSocketDef.V_FlushData(500);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  //BOF_SET_SOCKET_ADDRESS(TargetAddress_X, BOF_SOCK_TYPE::BOF_SOCK_STREAM, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP, 127, 0, 0, 1, 5555);
}


TEST(SocketTcp_Test, BasicTcpNonBlockingSocket)
{
  BofSocket BofSocketDef;
  BOFERR Sts_E;
  BOF_SOCKET_PARAM BofSocketParam_X;
  uint32_t Nb_U32, i_U32, Start_U32;
  uint8_t pBuffer_U8[0x10000];
  BOF_SOCKET_ADDRESS TargetAddress_X;
  BOF_COM_CHANNEL_STATUS Status_X;
  BofComChannel *pClient;

  EXPECT_EQ(BofSocketDef.GetSocketHandle(), BOFSOCKET_INVALID);
  EXPECT_EQ(BofSocketDef.GetMaxUdpLen(), 0);
  EXPECT_FALSE(BofSocketDef.IsTcp());
  EXPECT_FALSE(BofSocketDef.IsUdp());
  EXPECT_EQ(BofSocketDef.LastErrorCode(), BOF_ERR_NO_ERROR);

  BofSocketParam_X.Reset();
  BofSocketParam_X.BaseChannelParam_X.ChannelName_S = "MyDefSocket";
  BofSocketParam_X.BaseChannelParam_X.Blocking_B = false;
  BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 3;                               // 0->Client
  BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x10000;
  BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x10000;
#if defined(_WIN32)
  BofSocketParam_X.BindIpAddress_S = "udp://10.129.170.30:5555";
#else
  BofSocketParam_X.BindIpAddress_S = "udp://10.129.170.21:5555";
#endif
  BofSocketParam_X.ReUseAddress_B = false;
  BofSocketParam_X.NoDelay_B = true;
  BofSocketParam_X.Ttl_U32 = 32;
  BofSocketParam_X.BroadcastPort_U16 = 0;
  BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
  BofSocketParam_X.MulticastSender_B = false;
  BofSocketParam_X.KeepAlive_B = false;
  BofSocketParam_X.EnableLocalMulticast_B = false;
  Sts_E = BofSocketDef.InitializeSocket(BofSocketParam_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(BofSocketDef.GetSocketHandle(), (BOFSOCKET)0);
  EXPECT_GE(BofSocketDef.GetMaxUdpLen(), (uint32_t)0xFF00);
  EXPECT_FALSE(BofSocketDef.IsTcp());
  EXPECT_TRUE(BofSocketDef.IsUdp());
  EXPECT_EQ(BofSocketDef.LastErrorCode(), BOF_ERR_NO_ERROR);

  pClient = BofSocketDef.V_Listen(100, "");
  EXPECT_TRUE(pClient != nullptr);

#if defined(_WIN32)
  Sts_E = BofSocketDef.V_Connect(100, "udp://10.129.170.21:5556", "");
#else, "udp://10.129.170.30:5556", "");
#endif
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Nb_U32 = sizeof(pBuffer_U8);
  for (i_U32 = 0; i_U32 < Nb_U32; i_U32++)
  {
    pBuffer_U8[i_U32] = static_cast<uint8_t>(i_U32);
  }
  for (i_U32 = 0; i_U32 < 10; i_U32++)
  {
    Nb_U32 = sizeof(pBuffer_U8);
    Sts_E = BofSocketDef.V_WriteData(100, Nb_U32, pBuffer_U8);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(Nb_U32, sizeof(pBuffer_U8));
  }
  Start_U32 = Bof_GetMsTickCount();
  Nb_U32 = sizeof(pBuffer_U8);
  Sts_E = BofSocketDef.V_ReadData(1000, Nb_U32, pBuffer_U8);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Nb_U32, 0);
  EXPECT_GE(Bof_ElapsedMsTime(Start_U32), static_cast<uint32_t>(1000));
  EXPECT_LT(Bof_ElapsedMsTime(Start_U32), static_cast<uint32_t>(1100));

  Sts_E = BofSocketDef.V_GetStatus(Status_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(Status_X.Connected_B);
  EXPECT_EQ(Status_X.NbIn_U32, 0);
  EXPECT_EQ(Status_X.NbOut_U32, 0);
  EXPECT_EQ(Status_X.Flag_U32, 0);
  EXPECT_EQ(Status_X.Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = BofSocketDef.V_FlushData(500);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  //BOF_SET_SOCKET_ADDRESS(TargetAddress_X, BOF_SOCK_TYPE::BOF_SOCK_STREAM, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP, 127, 0, 0, 1, 5555);
}


TEST(SocketTcp_Test, BasicTcpBlockingSocket)
{
  BofSocket BofSocketDef;
  BOFERR Sts_E;
  BOF_SOCKET_PARAM BofSocketParam_X;
  uint32_t Nb_U32, i_U32, Start_U32;
  uint8_t pBuffer_U8[0x10000];
  BOF_SOCKET_ADDRESS TargetAddress_X;
  BOF_COM_CHANNEL_STATUS Status_X;
  BofComChannel *pClient;

  BofSocketParam_X.Reset();
  BofSocketParam_X.BaseChannelParam_X.ChannelName_S = "MyDefSocket";
  BofSocketParam_X.BaseChannelParam_X.Blocking_B = true;
  BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 3;                               // 0->Client
  BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x10000;
  BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x10000;

#if defined(_WIN32)
  BofSocketParam_X.BindIpAddress_S = "tcp://10.129.170.30:5555";
#else
  BofSocketParam_X.BindIpAddress_S = "tcp://10.129.170.21:5555";
#endif
  BofSocketParam_X.ReUseAddress_B = false;
  BofSocketParam_X.NoDelay_B = true;
  BofSocketParam_X.Ttl_U32 = 32;
  BofSocketParam_X.BroadcastPort_U16 = 0;
  BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
  BofSocketParam_X.MulticastSender_B = false;
  BofSocketParam_X.KeepAlive_B = false;
  BofSocketParam_X.EnableLocalMulticast_B = false;
  Sts_E = BofSocketDef.InitializeSocket(BofSocketParam_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(BofSocketDef.GetSocketHandle(), (BOFSOCKET)0);
  EXPECT_EQ(BofSocketDef.GetMaxUdpLen(), (uint32_t)0);
  EXPECT_TRUE(BofSocketDef.IsTcp());
  EXPECT_FALSE(BofSocketDef.IsUdp());
  EXPECT_EQ(BofSocketDef.LastErrorCode(), BOF_ERR_NO_ERROR);

  pClient = BofSocketDef.V_Listen(100, "");
  EXPECT_TRUE(pClient != nullptr);

#if defined(_WIN32)
  Sts_E = BofSocketDef.V_Connect(100, "tcp://10.129.170.21:5556", "");
#else
  Sts_E = BofSocketDef.V_Connect(100, "tcp://10.129.170.30:5556", "");
#endif
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Nb_U32 = sizeof(pBuffer_U8);
  for (i_U32 = 0; i_U32 < Nb_U32; i_U32++)
  {
    pBuffer_U8[i_U32] = static_cast<uint8_t>(i_U32);
  }
  for (i_U32 = 0; i_U32 < 10; i_U32++)
  {
    Nb_U32 = sizeof(pBuffer_U8);
    Sts_E = BofSocketDef.V_WriteData(100, Nb_U32, pBuffer_U8);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(Nb_U32, sizeof(pBuffer_U8));
  }
  Start_U32 = Bof_GetMsTickCount();
  Nb_U32 = sizeof(pBuffer_U8);
  Sts_E = BofSocketDef.V_ReadData(1000, Nb_U32, pBuffer_U8);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Nb_U32, 0);
  EXPECT_GE(Bof_ElapsedMsTime(Start_U32), static_cast<uint32_t>(1000));
  EXPECT_LT(Bof_ElapsedMsTime(Start_U32), static_cast<uint32_t>(1100));

  Sts_E = BofSocketDef.V_GetStatus(Status_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(Status_X.Connected_B);
  EXPECT_EQ(Status_X.NbIn_U32, 0);
  EXPECT_EQ(Status_X.NbOut_U32, 0);
  EXPECT_EQ(Status_X.Flag_U32, 0);
  EXPECT_EQ(Status_X.Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = BofSocketDef.V_FlushData(500);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  //BOF_SET_SOCKET_ADDRESS(TargetAddress_X, BOF_SOCK_TYPE::BOF_SOCK_STREAM, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP, 127, 0, 0, 1, 5555);
}

#endif