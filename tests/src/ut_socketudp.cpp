/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the udp socket comchannel class
 *
 * Name:        ut_socketudp.cpp
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
#include "bofstd/bofstring.h"
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

struct SERVER_SESSION_THREAD_CONTEXT
{
  BOF_SOCKET_PARAM BofSocketParam_X;

  SERVER_SESSION_THREAD_CONTEXT()
  {
    Reset();
  }
  void Reset()
  {
    BofSocketParam_X.Reset();
  }
};
// To use a test fixture, derive from testing::Test class
class SocketUdp_Test : public testing::Test
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

static uint64_t S_TotalSrvUdp_U64 = 0, S_TotalCltUdp_U64 = 0;

static void *S_UdpServerThread(const std::atomic<bool> &_rIsThreadLoopMustExit_B, void *_pContext)
{
  BOFERR Sts_E;
  SERVER_THREAD_CONTEXT *pThreadContext_X = (SERVER_THREAD_CONTEXT *)_pContext;
  BofSocket ListeningSocket;
  BofComChannel *pClient;
  std::vector<std::unique_ptr<BofComChannel>> ClientCollection;
  BOF_COM_CHANNEL_STATUS Status_X;
  uint8_t pBuffer_U8[0x10000];
  uint32_t i_U32, Nb_U32, NbToRead_U32;

  BOF_ASSERT(pThreadContext_X != nullptr);
  Sts_E = ListeningSocket.InitializeSocket(pThreadContext_X->BofSocketParam_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  while (!_rIsThreadLoopMustExit_B)
  {
    pClient = ListeningSocket.V_Listen(0, "");
    if (pClient)
    {
      ClientCollection.push_back(std::unique_ptr<BofComChannel>(pClient));
    }
    for (i_U32 = 0; i_U32 < ClientCollection.size(); i_U32++)
    {
      BOF_ASSERT(ClientCollection[i_U32] != nullptr);
      Sts_E = ClientCollection[i_U32]->V_GetStatus(Status_X);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      if (Status_X.NbIn_U32)
      {
        Nb_U32 = Status_X.NbIn_U32 > sizeof(pBuffer_U8) ? sizeof(pBuffer_U8) : Status_X.NbIn_U32;
        BOF_ASSERT(Nb_U32 <= sizeof(pBuffer_U8));
        NbToRead_U32 = Nb_U32;
        Sts_E = ClientCollection[i_U32]->V_ReadData(1000, NbToRead_U32, pBuffer_U8);
        S_TotalSrvUdp_U64 += Nb_U32;

        //				printf("srv Rd n %d s %d\r\n", NbToRead_U32, Sts_E);

        EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
        //				EXPECT_EQ(Nb_U32, NbToRead_U32);

        Sts_E = ClientCollection[i_U32]->V_WriteData(1000, Nb_U32, pBuffer_U8);
        //				printf("srv Wrt n %d s %d\r\n", Nb_U32, Sts_E);
        S_TotalSrvUdp_U64 += Nb_U32;

        EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
        //				EXPECT_EQ(Nb_U32, NbToRead_U32);
      }
    }
  }
  // ClientCollection is a vector of unique pointer->deallocated on return of this function
  //  ClientCollection.clear();
  return nullptr;
}

void SocketUdp_Test::SetUpTestCase()
{
}

void SocketUdp_Test::TearDownTestCase()
{
}
void SocketUdp_Test::SetUp()
{
  BOFERR Sts_E;

  mServerThreadContext_X.BofSocketParam_X.BaseChannelParam_X.ChannelName_S = "UdpServer";
  mServerThreadContext_X.BofSocketParam_X.BaseChannelParam_X.Blocking_B = true;
  mServerThreadContext_X.BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 64;      // 0->Client
  mServerThreadContext_X.BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x40000; // 0x1000000;
  mServerThreadContext_X.BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x40000; // 0x1000000;
  mServerThreadContext_X.BofSocketParam_X.BindIpAddress_S = "udp://127.0.0.1:5555";
  mServerThreadContext_X.BofSocketParam_X.ReUseAddress_B = false;
  mServerThreadContext_X.BofSocketParam_X.NoDelay_B = true;
  mServerThreadContext_X.BofSocketParam_X.Ttl_U32 = 32;
  mServerThreadContext_X.BofSocketParam_X.BroadcastPort_U16 = 0;
  mServerThreadContext_X.BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
  mServerThreadContext_X.BofSocketParam_X.MulticastSender_B = false;
  mServerThreadContext_X.BofSocketParam_X.KeepAlive_B = false;
  mServerThreadContext_X.BofSocketParam_X.EnableLocalMulticast_B = false;

  Sts_E = Bof_CreateThread("ServerThread", S_UdpServerThread, &mServerThreadContext_X, mSeverThread_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_LaunchThread(mSeverThread_X, 0x40000, 0, BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF_THREAD_PRIORITY_000, 1000);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

void SocketUdp_Test::TearDown()
{
  BOFERR Sts_E;
  Sts_E = Bof_DestroyThread(mSeverThread_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

TEST_F(SocketUdp_Test, FilterMulticastOnIpAddress)
{
  BOF_SOCKET_PARAM BofSocketParam_X;
  BofSocket *pUdp;
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);

  BofSocketParam_X.BaseChannelParam_X.ChannelName_S = "FilterMulticastOnIpAddress";
  BofSocketParam_X.BaseChannelParam_X.Blocking_B = true;
  BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0; // 0->Client
  BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x1000;
  BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x1000;

  BofSocketParam_X.BindIpAddress_S = "udp://127.0.0.1:0";
  BofSocketParam_X.ReUseAddress_B = false;
  BofSocketParam_X.NoDelay_B = true;
  BofSocketParam_X.Ttl_U32 = 32;
  BofSocketParam_X.BroadcastPort_U16 = 0;
  BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
  BofSocketParam_X.MulticastSender_B = false;
  BofSocketParam_X.KeepAlive_B = false;
  BofSocketParam_X.EnableLocalMulticast_B = false;
  BofSocketParam_X.MaxNumberOfScatterGatherEntry_U32 = 16;

  BofSocketParam_X.MulticastInterfaceIpAddress_S = "224.1.2.3";
  BofSocketParam_X.FilterMulticastOnIpAddress_B = true;
  pUdp = new BofSocket(BofSocketParam_X);
  BOF_SAFE_DELETE(pUdp);
  Bof_MsSleep(100);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 1); // Because S_UdpServerThread is running
}

constexpr uint32_t SCATTER_GATHER_IO_TIMEOUT_IN_MS = 100;

TEST_F(SocketUdp_Test, ScatterGatherIo)
{
  BOF_SOCKET_PARAM BofSocketParam_X;
  BofSocket *pUdp;
  BOF_BUFFER Buffer_X;
  std::vector<BOF_BUFFER> BufferCollection;
  uint8_t pHeader_U8[32], pData_U8[1024];
  BOF_SOCKET_ADDRESS DstIpAddress_X;
  bool PartialRead_B;
  uint32_t NbByteRead_U32, NbByteWritten_U32, Start_U32, Delta_U32;

  // Bof_MsSleep(100);
  // EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);

  BofSocketParam_X.BaseChannelParam_X.ChannelName_S = "ScatterGatherIo";
  BofSocketParam_X.BaseChannelParam_X.Blocking_B = true;
  BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;       // 0->Client
  BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x40000; // 0x1000000;
  BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x40000; // 0x1000000;

  BofSocketParam_X.BindIpAddress_S = "udp://127.0.0.1:0";
  BofSocketParam_X.ReUseAddress_B = false;
  BofSocketParam_X.NoDelay_B = true;
  BofSocketParam_X.Ttl_U32 = 32;
  BofSocketParam_X.BroadcastPort_U16 = 0;
  BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
  BofSocketParam_X.MulticastSender_B = false;
  BofSocketParam_X.KeepAlive_B = false;
  BofSocketParam_X.EnableLocalMulticast_B = false;
  BofSocketParam_X.MaxNumberOfScatterGatherEntry_U32 = 16;

  pUdp = new BofSocket(BofSocketParam_X);
  EXPECT_TRUE(pUdp != nullptr);
  EXPECT_EQ(pUdp->LastErrorCode(), BOF_ERR_NO_ERROR);

  //	DstIpAddress_X.Set(false, BOF_SOCK_TYPE::BOF_SOCK_UDP, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP, 127, 0, 0, 1, 5555);
  //	EXPECT_EQ(pUdp->SetDstIpAddress(DstIpAddress_X), BOF_ERR_NO_ERROR);

  EXPECT_EQ(pUdp->V_Connect(SCATTER_GATHER_IO_TIMEOUT_IN_MS, "udp://127.0.0.1:5555", ""), BOF_ERR_NO_ERROR);

  BufferCollection.clear();
  memset(pHeader_U8, 0x01, sizeof(pHeader_U8));
  Buffer_X.SetStorage(sizeof(pHeader_U8), sizeof(pHeader_U8), pHeader_U8);
  BufferCollection.push_back(Buffer_X);
#if 0

  memset(pData_U8, 0x02, sizeof(pData_U8));
  Buffer_X.SetStorage(sizeof(pData_U8), sizeof(pData_U8), pData_U8);
  BufferCollection.push_back(Buffer_X);

  EXPECT_EQ(pUdp->WriteScatterGatherData(SCATTER_GATHER_IO_TIMEOUT_IN_MS, BufferCollection, NbByteWritten_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(NbByteWritten_U32, sizeof(pHeader_U8) + sizeof(pData_U8));

  memset(pHeader_U8, 0x00, sizeof(pHeader_U8));
  memset(pData_U8, 0x00, sizeof(pData_U8));
  EXPECT_EQ(pUdp->ReadScatterGatherData(SCATTER_GATHER_IO_TIMEOUT_IN_MS, BufferCollection, NbByteRead_U32, PartialRead_B), BOF_ERR_NO_ERROR);
  EXPECT_EQ(NbByteRead_U32, sizeof(pHeader_U8) + sizeof(pData_U8));
  EXPECT_FALSE(PartialRead_B);

  EXPECT_EQ(pHeader_U8[0], 0x01);
  EXPECT_EQ(pData_U8[0], 0x02);

  BufferCollection.clear();
  memset(pHeader_U8, 0x01, sizeof(pHeader_U8));
  Buffer_X.SetStorage(sizeof(pHeader_U8), sizeof(pHeader_U8), pHeader_U8);
  BufferCollection.push_back(Buffer_X);

  memset(pData_U8, 0x02, sizeof(pData_U8));
  Buffer_X.SetStorage(sizeof(pData_U8), sizeof(pData_U8), pData_U8);
  BufferCollection.push_back(Buffer_X);

  EXPECT_EQ(pUdp->WriteScatterGatherData(SCATTER_GATHER_IO_TIMEOUT_IN_MS, BufferCollection, NbByteWritten_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(NbByteWritten_U32, sizeof(pHeader_U8) + sizeof(pData_U8));

  memset(pHeader_U8, 0x00, sizeof(pHeader_U8));
  memset(pData_U8, 0x00, sizeof(pData_U8));
  BufferCollection.pop_back();
  EXPECT_EQ(pUdp->ReadScatterGatherData(SCATTER_GATHER_IO_TIMEOUT_IN_MS, BufferCollection, NbByteRead_U32, PartialRead_B), BOF_ERR_NO_ERROR);
  EXPECT_EQ(NbByteRead_U32, sizeof(pHeader_U8));
  EXPECT_TRUE(PartialRead_B);

  BufferCollection.pop_back();
  BufferCollection.push_back(Buffer_X);
  Start_U32 = Bof_GetMsTickCount();
  EXPECT_NE(pUdp->ReadScatterGatherData(SCATTER_GATHER_IO_TIMEOUT_IN_MS, BufferCollection, NbByteRead_U32, PartialRead_B), BOF_ERR_NO_ERROR);
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, SCATTER_GATHER_IO_TIMEOUT_IN_MS);
  EXPECT_EQ(NbByteRead_U32, 0);
  EXPECT_FALSE(PartialRead_B);
#endif
  EXPECT_EQ(pHeader_U8[0], 0x01);
  EXPECT_NE(pData_U8[0], 0x02);
  BOF_SAFE_DELETE(pUdp);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 1 + 1); // Because S_UdpServerThread is running and we have a V_Connect udp
}

const uint32_t SERVER_NB_CLIENT = 50;
const uint32_t CLIENT_NB_LOOP = 500;

TEST_F(SocketUdp_Test, UdpClientTest)
{
  std::vector<std::unique_ptr<BofSocket>> ClientCollection;
  BOFERR Sts_E;
  BOF_SOCKET_PARAM BofSocketParam_X;
  uint64_t Total_U64, TotalSrvClt_U64;
  uint32_t Nb_U32, i_U32, j_U32, Start_U32, StartWaitEof_U32, Delta_U32, DeltaWaitEof_U32, KBPerSec_U32;
  uint8_t pBuffer_U8[0xF000];
  BOF_COM_CHANNEL_STATUS Status_X;

  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);

  for (i_U32 = 0; i_U32 < SERVER_NB_CLIENT; i_U32++)
  {
    std::unique_ptr<BofSocket> puClientSocket = std::make_unique<BofSocket>();

    BofSocketParam_X.BaseChannelParam_X.ChannelName_S = "UdpClient_" + std::to_string(i_U32);
    BofSocketParam_X.BaseChannelParam_X.Blocking_B = true;
    BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;       // 0->Client
    BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x40000; // 0x1000000;
    BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x40000; // 0x1000000;

    BofSocketParam_X.BindIpAddress_S = "udp://127.0.0.1:0";
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
    //		printf("create sck %d\r\n", puClientSocket->GetSocketHandle());
    ClientCollection.push_back(std::move(puClientSocket));
  }
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), SERVER_NB_CLIENT + 1); //+1 because S_UdpServerThread creatre a listening socket

  for (i_U32 = 0; i_U32 < ClientCollection.size(); i_U32++)
  {
    BOF_ASSERT(ClientCollection[i_U32] != nullptr);
    Sts_E = ClientCollection[i_U32]->V_Connect(100, "udp://127.0.0.1:5555", "");
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  }
  Nb_U32 = sizeof(pBuffer_U8);
  for (j_U32 = 0; j_U32 < Nb_U32; j_U32++)
  {
    pBuffer_U8[j_U32] = static_cast<uint8_t>(j_U32);
  }
  S_TotalSrvUdp_U64 = 0;
  S_TotalCltUdp_U64 = 0;
  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < ClientCollection.size(); i_U32++)
  {
    for (j_U32 = 0; j_U32 < CLIENT_NB_LOOP; j_U32++)
    {
      Nb_U32 = sizeof(pBuffer_U8);
      Sts_E = ClientCollection[i_U32]->V_WriteData(100, Nb_U32, pBuffer_U8);
      S_TotalCltUdp_U64 += Nb_U32;
      //	printf("clt Wrt %d: n %d s %d\r\n", j_U32, Nb_U32, Sts_E);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_EQ(Nb_U32, sizeof(pBuffer_U8));

      Nb_U32 = sizeof(pBuffer_U8);
      Sts_E = ClientCollection[i_U32]->V_ReadData(1000, Nb_U32, pBuffer_U8);
      S_TotalCltUdp_U64 += Nb_U32;
      //	printf("clt Rd n %d s %d\r\n", Nb_U32, Sts_E);

      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      //	EXPECT_EQ(Nb_U32, sizeof(pBuffer_U8));
    }
    Sts_E = ClientCollection[i_U32]->V_GetStatus(Status_X);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    //		EXPECT_TRUE(Status_X.Connected_B);
    //		EXPECT_EQ(Status_X.NbOut_U32, 0);
    //		EXPECT_EQ(Status_X.Flag_U32, 0);
    //		EXPECT_EQ(Status_X.Sts_E, BOF_ERR_NO_ERROR);
  }
  Total_U64 = (2L * (static_cast<uint64_t>(SERVER_NB_CLIENT) * static_cast<uint64_t>(CLIENT_NB_LOOP) * sizeof(pBuffer_U8))) * 2L;
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);

  StartWaitEof_U32 = Bof_GetMsTickCount();
  DeltaWaitEof_U32 = 0;
  while ((S_TotalSrvUdp_U64 + S_TotalCltUdp_U64) != Total_U64)
  {
    for (i_U32 = 0; i_U32 < ClientCollection.size(); i_U32++)
    {
      Nb_U32 = sizeof(pBuffer_U8);
      Sts_E = ClientCollection[i_U32]->V_ReadData(10, Nb_U32, pBuffer_U8);
      if (Sts_E == BOF_ERR_NO_ERROR)
      {
        // printf("extra rd clt %d nb %d Srv %" PRId64 "Clt %" PRId64 "\n", i_U32, Nb_U32, S_TotalSrvUdp_U64, S_TotalCltUdp_U64);
        S_TotalCltUdp_U64 += Nb_U32;
      }
      Sts_E = ClientCollection[i_U32]->V_GetStatus(Status_X);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      // EXPECT_TRUE(Status_X.Connected_B);
      // EXPECT_EQ(Status_X.Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_EQ(Status_X.NbOut_U32, 0);
      EXPECT_EQ(Status_X.Flag_U32, 0);
    }
    DeltaWaitEof_U32 = Bof_ElapsedMsTime(StartWaitEof_U32);
    TotalSrvClt_U64 = (S_TotalSrvUdp_U64 + S_TotalCltUdp_U64);
    // printf("Total %" PRId64 "/%" PRId64 " SizeDelta %" PRId64 " NbTrf %f Srv %" PRId64 " Clt %" PRId64 "\n", Total_U64, TotalSrvClt_U64, Total_U64 - TotalSrvClt_U64, (float)(Total_U64 - TotalSrvClt_U64) / (float)sizeof(pBuffer_U8), S_TotalSrvUdp_U64, S_TotalCltUdp_U64);
    if (DeltaWaitEof_U32 > 1000)
    {
      break;
    }
  }
  TotalSrvClt_U64 = (S_TotalSrvUdp_U64 + S_TotalCltUdp_U64);
  // printf("SizeDelta %" PRId64 " NbTrf %f\n", Total_U64 - TotalSrvClt_U64, (float)(Total_U64 - TotalSrvClt_U64)/(float)sizeof(pBuffer_U8));
  EXPECT_EQ(TotalSrvClt_U64, Total_U64);
  KBPerSec_U32 = (Delta_U32) ? static_cast<uint32_t>((TotalSrvClt_U64 * 1000) / 1024L) / Delta_U32 : 0;
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  // printf("%d client %d loop %d KB %d MB in %d ms (extra ms %d)->%d KB/S %d MB/S\r\n", SERVER_NB_CLIENT, CLIENT_NB_LOOP, static_cast<uint32_t>(TotalSrvClt_U64 / 1024L), static_cast<uint32_t>(TotalSrvClt_U64 / 1024L / 1024L), Delta_U32, DeltaWaitEof_U32, KBPerSec_U32, KBPerSec_U32 / 1024);
  for (i_U32 = 0; i_U32 < ClientCollection.size(); i_U32++)
  {
    Sts_E = ClientCollection[i_U32]->V_FlushData(10);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_GE(Bof_ElapsedMsTime(Start_U32), static_cast<uint32_t>(500));
  }
  // All client and server session vector of unique pointer->deallocated on return of this function and the exit of server listening thread
  // Socket level is finally checked in the next test function ChkSocketBalance
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), SERVER_NB_CLIENT + 1 + ClientCollection.size()); //+1 because S_TcpServerThread creatre a listening socket
}

TEST_F(SocketUdp_Test, ChkSocketBalance)
{
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
}