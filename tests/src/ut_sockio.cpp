/*
 * Copyright (c) 2013-2025, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofio module
 *
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Jan 05 2019  BHA : Initial release
 */
#include <bofstd/bofsocketthread.h>

#include "gtestrunner.h"

#define MAX_NB_TEST_LOOP 3       // 4       // 8 // 2500
#define MAX_NB_CLIENT_SESSION 2  // 64
#define MAX_IO_SIZE (128 * 1024) //(1 * 1024 * 1024)
#define DEFAULT_LISTENING_PORT 60000
#define DEFAULT_INTER_PROCESS_TIMEOUT 250 // Listen timeout is 100 Ms
#define MAX_NB_OP_PENDING 1
#define MAX_NB_CLIENT_SESSION 2 // 64

struct Foo
{
  std::string s;
  Foo(const char *p)
  {
    s = p;
    std::cout << "Foo..." << s << "\n";
  }
  ~Foo()
  {
    std::cout << "~Foo..." << s << "\n";
  }
};

TEST(SockIo_Test, CreateDelete)
{
  /*
    std::unique_ptr<Foo> up = std::make_unique<Foo>("A");
    up = std::make_unique<Foo>("B");
  gives:
    Foo...A
    Foo...B
    ~Foo...A

  AND

    std::unique_ptr<Foo> up = std::make_unique<Foo>("A");
    up.reset();
    up = std::make_unique<Foo>("B");
  gives:
    Foo...A
    ~Foo...A
    Foo...B


  So if Foo create athread the first sequence will create 2 threads before killing one
  */

  BOF::BOF_SOCKET_THREAD_PARAM SocketThreadParam_X;
  uint32_t i_U32, Timer_U32, Delta_U32, Max_U32;
  std::unique_ptr<BOF::BofSocketThread> puSocketThread;

  Max_U32 = 0;
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
  EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 0);
  printf("%d: Create/Destroy %d threads\n", BOF::Bof_GetMsTickCount(), MAX_NB_TEST_LOOP);
  for (i_U32 = 0; i_U32 < MAX_NB_TEST_LOOP; i_U32++)
  {
    Timer_U32 = BOF::Bof_GetMsTickCount();
    SocketThreadParam_X.Name_S = "SockIo_Tst_" + std::to_string(i_U32);
    // for ut under qemu/docker do not use fifo scheduler
    SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
    SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
    SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
    SocketThreadParam_X.pListeningSocket_O = nullptr;
    SocketThreadParam_X.pSocket_O = nullptr;
    // printf("%d: Create new thread\n", BOF::Bof_GetMsTickCount());
    puSocketThread = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
    ASSERT_TRUE(puSocketThread != nullptr);
    // printf("%d: Delete thread\n", BOF::Bof_GetMsTickCount());
    puSocketThread.reset();
    Delta_U32 = BOF::Bof_ElapsedMsTime(Timer_U32);
    if (Delta_U32 > Max_U32)
    {
      Max_U32 = Delta_U32;
    }
    EXPECT_LT(Delta_U32, 100);
    if (Delta_U32 > 10)
    {
      //      printf("%d: Iter %d Delta %d Max is %d ms\n", BOF::Bof_GetMsTickCount(), i_U32, Delta_U32, Max_U32);
    }
  }
  printf("%d: Max is %d ms\n", BOF::Bof_GetMsTickCount(), Max_U32);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
  EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 0);
}

TEST(SockIo_Test, ListenConnectDisconnect)
{
  BOF::BOF_SOCKET_THREAD_PARAM SocketThreadParam_X;
  BOF::BOF_SOCKET_LISTEN_PARAM ListenParam_X;
  BOF::BOF_SOCKET_CONNECT_PARAM ConnectParam_X;
  BOF::BOF_SOCKET_DISCONNECT_PARAM DisconnectParam_X;
  uint32_t i_U32, Timer_U32, Start_U32, Delta_U32, Max_U32, ServerOpTicket_U32, SessionOpTicket_U32, ClientOpTicket_U32;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadServer;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadSession;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadClient;
  BOF::BOF_SOCKET_OPERATION_RESULT ServerOperationResult_X, SessionOperationResult_X, ClientOperationResult_X;
  BOF::BofSocket *pListenSocket_O = nullptr;

  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
  EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 0);

  Start_U32 = BOF::Bof_GetMsTickCount();

  // Create server listening thread
  SocketThreadParam_X.Name_S = "SockIo_Lis_0";
  // for ut under qemu/docker do not use fifo scheduler
  SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
  SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
  SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
  SocketThreadParam_X.pListeningSocket_O = nullptr;
  SocketThreadParam_X.pSocket_O = nullptr;
  puSocketThreadServer = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
  ASSERT_TRUE(puSocketThreadServer != nullptr);
  ListenParam_X.JustOnce_B = false; // For "ever"
  ListenParam_X.NbMaxClient_U32 = 3;
  ListenParam_X.SrcIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
  ListenParam_X.SrcPort_U16 = DEFAULT_LISTENING_PORT;
  // Timeout of listen must be at least half of DEFAULT_INTER_PROCESS_TIMEOUT
  EXPECT_EQ(puSocketThreadServer->ProgramSocketOperation(100 /*DEFAULT_INTER_PROCESS_TIMEOUT*/, ListenParam_X, ServerOpTicket_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOpTicket_U32, 1);
  EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN);
  EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32);
  EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(ServerOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
  EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);
  // EXPECT_EQ(ServerOperationResult_X.Time_U32,0);
  pListenSocket_O = ServerOperationResult_X.pSocket_O;

  // Create client connect thread
  SocketThreadParam_X.Name_S = "SockIo_Clt_0";
  // for ut under qemu/docker do not use fifo scheduler
  SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
  SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
  SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
  SocketThreadParam_X.pListeningSocket_O = nullptr;
  SocketThreadParam_X.pSocket_O = nullptr;
  puSocketThreadClient = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
  ASSERT_TRUE(puSocketThreadClient != nullptr);
  ConnectParam_X.Tcp_B = true;
  ConnectParam_X.SrcIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
  ConnectParam_X.SrcPort_U16 = 0;
  ConnectParam_X.DstIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
  ConnectParam_X.DstPort_U16 = DEFAULT_LISTENING_PORT;

  Max_U32 = 0;
  BOF::Bof_MsSleep(20);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 1);
  printf("%d: Connect/Disconnect %d times\n", BOF::Bof_GetMsTickCount(), MAX_NB_TEST_LOOP);
  for (i_U32 = 0; i_U32 < MAX_NB_TEST_LOOP; i_U32++)
  {
    Timer_U32 = BOF::Bof_GetMsTickCount();
    // Connect client to server
    EXPECT_EQ(puSocketThreadClient->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ConnectParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOpTicket_U32, 1 + (i_U32 * 2));
    EXPECT_EQ(puSocketThreadClient->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT);
    EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
    EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(ClientOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
    // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);

    // Get server session creation param: answer posted due to connect event
    EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT);
    EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32 + i_U32 + 1);
    EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(ServerOperationResult_X.pSocket_O, nullptr);
    EXPECT_NE(ServerOperationResult_X.pSocket_O, ClientOperationResult_X.pSocket_O);
    EXPECT_NE(ServerOperationResult_X.pSocket_O, pListenSocket_O);
    EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);
    // EXPECT_NE(ServerOperationResult_X.Time_U32, 0);
    // Create session thread
    SocketThreadParam_X.Name_S = "SockIo_Ses_0";
    // for ut under qemu/docker do not use fifo scheduler
    SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
    SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
    SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
    SocketThreadParam_X.pListeningSocket_O = nullptr;
    SocketThreadParam_X.pSocket_O = ServerOperationResult_X.pSocket_O; // Give it by default so BOF_SOCKET_OPERATION_DISCONNECT can be executed correctly
    puSocketThreadSession = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
    ASSERT_TRUE(puSocketThreadSession != nullptr);
    // EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 3);

    // Disconnect client
    DisconnectParam_X.Unused_U32 = 0; // ClientOperationResult_X.pSocket_O;
    EXPECT_EQ(puSocketThreadClient->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, DisconnectParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOpTicket_U32, 1 + (i_U32 * 2) + 1);
    EXPECT_EQ(puSocketThreadClient->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT);
    EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
    EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(ClientOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
    // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
    // EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 2);

    // Disconnect session
    DisconnectParam_X.Unused_U32 = 0; // ServerOperationResult_X.pSocket_O;
    EXPECT_EQ(puSocketThreadSession->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, DisconnectParam_X, SessionOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(SessionOpTicket_U32, 1);
    EXPECT_EQ(puSocketThreadSession->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, SessionOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(SessionOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT);
    EXPECT_EQ(SessionOperationResult_X.OpTicket_U32, SessionOpTicket_U32);
    EXPECT_EQ(SessionOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(SessionOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(SessionOperationResult_X.Size_U32, 0);
    EXPECT_EQ(SessionOperationResult_X.pBuffer_U8, nullptr);
    // EXPECT_NE(SessionOperationResult_X.Time_U32, 0);
    // EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 1);

    Delta_U32 = BOF::Bof_ElapsedMsTime(Timer_U32);
    if (Delta_U32 > Max_U32)
    {
      Max_U32 = Delta_U32;
    }
    EXPECT_LT(Delta_U32, 100);
    if (Delta_U32 > 30)
    {
      // printf("%d: Iter %d Delta %d Max is %d ms\n", BOF::Bof_GetMsTickCount(), i_U32, Delta_U32, Max_U32);
    }
  }
  // Disconnect server
  DisconnectParam_X.Unused_U32 = 0; // pListenSocket_O;
  EXPECT_EQ(puSocketThreadServer->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, DisconnectParam_X, ServerOpTicket_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOpTicket_U32, 2);
  EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT);
  EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32);
  EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(ServerOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
  EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);
  // EXPECT_NE(ServerOperationResult_X.Time_U32, 0);

  printf("%d: Max is %d ms DeltaT %d ms\n", BOF::Bof_GetMsTickCount(), Max_U32, BOF::Bof_ElapsedMsTime(Start_U32));
  BOF::Bof_MsSleep(50);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
  EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 3); // We didn't call EXIT op
}

TEST(SockIo_Test, ListenMultipleConnect)
{
  BOF::BOF_SOCKET_THREAD_PARAM SocketThreadParam_X;
  BOF::BOF_SOCKET_LISTEN_PARAM ListenParam_X;
  BOF::BOF_SOCKET_CONNECT_PARAM ConnectParam_X;
  BOF::BOF_SOCKET_DISCONNECT_PARAM DisconnectParam_X;
  BOF::BOF_SOCKET_EXIT_PARAM ExitParam_X;
  uint32_t i_U32, j_U32, Timer_U32, Start_U32, Delta_U32, Max_U32, ServerOpTicket_U32, SessionOpTicket_U32, ClientOpTicket_U32, NbThread_U32;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadServer;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadClient;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadSession;
  std::vector<std::unique_ptr<BOF::BofSocketThread>> puSocketThreadSessionCollection;
  std::vector<std::unique_ptr<BOF::BofSocketThread>> puSocketThreadClientCollection;
  BOF::BOF_SOCKET_OPERATION_RESULT ServerOperationResult_X, SessionOperationResult_X, ClientOperationResult_X;
  BOF::BofSocket *pListenSocket_O = nullptr;

  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
  EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 0);

  Max_U32 = 0;
  Start_U32 = BOF::Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < MAX_NB_TEST_LOOP; i_U32++)
  {
    Timer_U32 = BOF::Bof_GetMsTickCount();
    NbThread_U32 = 0;
    EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
    // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    puSocketThreadSessionCollection.clear();
    puSocketThreadClientCollection.clear();
    // Create server listening thread
    SocketThreadParam_X.Reset();
    SocketThreadParam_X.Name_S = "SockIo_Lis_0";
    // for ut under qemu/docker do not use fifo scheduler
    SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
    SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
    SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
    SocketThreadParam_X.pListeningSocket_O = nullptr;
    SocketThreadParam_X.pSocket_O = nullptr;
    puSocketThreadServer = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
    ASSERT_TRUE(puSocketThreadServer != nullptr);

    ListenParam_X.Reset();
    ListenParam_X.JustOnce_B = false; // For "ever"
    ListenParam_X.NbMaxClient_U32 = 3;
    ListenParam_X.SrcIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
    ListenParam_X.SrcPort_U16 = DEFAULT_LISTENING_PORT;

    // Timeout of listen must be at least half of DEFAULT_INTER_PROCESS_TIMEOUT
    EXPECT_EQ(puSocketThreadServer->ProgramSocketOperation(100 /*DEFAULT_INTER_PROCESS_TIMEOUT*/, ListenParam_X, ServerOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ServerOpTicket_U32, 1);
    EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN);
    EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32);
    EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(ServerOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);
    // EXPECT_EQ(ServerOperationResult_X.Time_U32,0);
    pListenSocket_O = ServerOperationResult_X.pSocket_O;
    NbThread_U32++;
    // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    ConnectParam_X.Reset();
    ConnectParam_X.Tcp_B = true;
    ConnectParam_X.SrcIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
    ConnectParam_X.SrcPort_U16 = 0;
    ConnectParam_X.DstIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
    ConnectParam_X.DstPort_U16 = DEFAULT_LISTENING_PORT;
    for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
    {
      // Create client threads
      SocketThreadParam_X.Reset();
      SocketThreadParam_X.Name_S = "SockIo_Clt_" + std::to_string(j_U32);
      // for ut under qemu/docker do not use fifo scheduler
      SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
      SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
      SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
      SocketThreadParam_X.pListeningSocket_O = nullptr;
      SocketThreadParam_X.pSocket_O = nullptr;
      puSocketThreadClient = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
      ASSERT_TRUE(puSocketThreadClient != nullptr);

      // Connect client to server
      EXPECT_EQ(puSocketThreadClient->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ConnectParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ClientOpTicket_U32, 1);
      ClientOperationResult_X.Reset();
      EXPECT_EQ(puSocketThreadClient->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT);
      EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
      EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_NE(ClientOperationResult_X.pSocket_O, nullptr);
      EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
      EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
      // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
      puSocketThreadClientCollection.push_back(std::move(puSocketThreadClient));
      NbThread_U32++;
      // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);

      // Get server session creation param: answer posted due to connect event
      EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT);
      EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32 + j_U32 + 1);
      EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_NE(ServerOperationResult_X.pSocket_O, nullptr);
      EXPECT_NE(ServerOperationResult_X.pSocket_O, ClientOperationResult_X.pSocket_O);
      EXPECT_NE(ServerOperationResult_X.pSocket_O, pListenSocket_O);
      EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
      EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);

      // Create session threads
      SocketThreadParam_X.Reset();
      SocketThreadParam_X.Name_S = "SockIo_Ses_" + std::to_string(j_U32);
      // for ut under qemu/docker do not use fifo scheduler
      SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
      SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
      SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
      SocketThreadParam_X.pListeningSocket_O = nullptr;
      SocketThreadParam_X.pSocket_O = ServerOperationResult_X.pSocket_O; // Give it by default so BOF_SOCKET_OPERATION_DISCONNECT can be executed correctly
      puSocketThreadSession = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
      ASSERT_TRUE(puSocketThreadSession != nullptr);
      puSocketThreadSessionCollection.push_back(std::move(puSocketThreadSession));
      NbThread_U32++;
      // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    }
    EXPECT_EQ(puSocketThreadClientCollection.size(), MAX_NB_CLIENT_SESSION);
    EXPECT_EQ(puSocketThreadSessionCollection.size(), MAX_NB_CLIENT_SESSION);

    // Disconnect client
    for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
    {
      DisconnectParam_X.Reset();
      DisconnectParam_X.Unused_U32 = 0; // ClientOperationResult_X.pSocket_O;
      EXPECT_EQ(puSocketThreadClientCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, DisconnectParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ClientOpTicket_U32, 2);
      EXPECT_EQ(puSocketThreadClientCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT);
      EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
      EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_NE(ClientOperationResult_X.pSocket_O, nullptr);
      EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
      EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
      // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);

      EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), NbThread_U32 - j_U32 - 1);
      // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    }

    // Disconnect session
    for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
    {
      DisconnectParam_X.Reset();
      DisconnectParam_X.Unused_U32 = 0; // ServerOperationResult_X.pSocket_O;
      EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, DisconnectParam_X, SessionOpTicket_U32), BOF_ERR_NO_ERROR);
      EXPECT_EQ(SessionOpTicket_U32, 1);
      EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, SessionOperationResult_X), BOF_ERR_NO_ERROR);
      EXPECT_EQ(SessionOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT);
      EXPECT_EQ(SessionOperationResult_X.OpTicket_U32, SessionOpTicket_U32);
      EXPECT_EQ(SessionOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_NE(SessionOperationResult_X.pSocket_O, nullptr);
      EXPECT_EQ(SessionOperationResult_X.Size_U32, 0);
      EXPECT_EQ(SessionOperationResult_X.pBuffer_U8, nullptr);
      // EXPECT_NE(SessionOperationResult_X.Time_U32, 0);
      // printf("BOF::BofSocket::S_BofSocketBalance() j %d => %d\n", j_U32, BOF::BofSocket::S_BofSocketBalance());
      // BOF_GTEST_EXPECT_BETWEEN(MAX_NB_CLIENT_SESSION - j_U32 - 1, BOF::BofSocket::S_BofSocketBalance(), MAX_NB_CLIENT_SESSION - j_U32);
      // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    }
    BOF::Bof_MsSleep(20);
    EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 1);
    // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    //  Disconnect server
    DisconnectParam_X.Reset();
    DisconnectParam_X.Unused_U32 = 0; // pListenSocket_O;
    EXPECT_EQ(puSocketThreadServer->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, DisconnectParam_X, ServerOpTicket_U32), BOF_ERR_NO_ERROR);

    EXPECT_EQ(ServerOpTicket_U32, 2);
    EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);

    EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT);
    EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32);
    EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(ServerOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);
    // EXPECT_NE(ServerOperationResult_X.Time_U32, 0);

    EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
    // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);

    // Leave client threads
    for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
    {
      ExitParam_X.Reset();
      ExitParam_X.Unused_U32 = 0;
      EXPECT_EQ(puSocketThreadClientCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ExitParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ClientOpTicket_U32, 3);
      EXPECT_EQ(puSocketThreadClientCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT);
      EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
      EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_CANCEL);
      EXPECT_EQ(ClientOperationResult_X.pSocket_O, nullptr);
      EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
      EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
      // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
      EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
      // BOF_GTEST_EXPECT_BETWEEN(NbThread_U32 - j_U32 - 1, BOF::BofThread::S_BofThreadBalance(), NbThread_U32 - j_U32 + 3);
    }

    // Leave session threads
    for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
    {
      ExitParam_X.Reset();
      ExitParam_X.Unused_U32 = 0;
      EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ExitParam_X, SessionOpTicket_U32), BOF_ERR_NO_ERROR);
      EXPECT_EQ(SessionOpTicket_U32, 2);
      EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, SessionOperationResult_X), BOF_ERR_NO_ERROR);
      EXPECT_EQ(SessionOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT);
      EXPECT_EQ(SessionOperationResult_X.OpTicket_U32, SessionOpTicket_U32);
      EXPECT_EQ(SessionOperationResult_X.Sts_E, BOF_ERR_CANCEL);
      EXPECT_EQ(SessionOperationResult_X.pSocket_O, nullptr);
      EXPECT_EQ(SessionOperationResult_X.Size_U32, 0);
      EXPECT_EQ(SessionOperationResult_X.pBuffer_U8, nullptr);
      // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
      EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
      // BOF_GTEST_EXPECT_BETWEEN(MAX_NB_CLIENT_SESSION - j_U32, BOF::BofThread::S_BofThreadBalance(), MAX_NB_CLIENT_SESSION - j_U32 + 5);
    }
    // Leave listen threads
    ExitParam_X.Reset();
    ExitParam_X.Unused_U32 = 0;
    EXPECT_EQ(puSocketThreadServer->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ExitParam_X, ServerOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ServerOpTicket_U32, 3);
    EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT);
    EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32);
    EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_CANCEL);
    EXPECT_EQ(ServerOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);
    // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
    EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
    // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 0);

    Delta_U32 = BOF::Bof_ElapsedMsTime(Timer_U32);
    if (Delta_U32 > Max_U32)
    {
      Max_U32 = Delta_U32;
    }
    EXPECT_LT(Delta_U32, 1200);
    if ((Delta_U32 > 1000) || ((i_U32 % 50) == 0))
    {
      printf("%d: Iter %d Delta %d Max is %d ms\n", BOF::Bof_GetMsTickCount(), i_U32, Delta_U32, Max_U32);
    }
  }

  printf("%d: Max is %d ms DeltaT %d ms\n", BOF::Bof_GetMsTickCount(), Max_U32, BOF::Bof_ElapsedMsTime(Start_U32));
  BOF::Bof_MsSleep(50);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
  EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 0);
}
TEST(SockIo_Test, DISABLED_ListenReadWrite)
{
  BOF::BOF_SOCKET_THREAD_PARAM SocketThreadParam_X;
  BOF::BOF_SOCKET_LISTEN_PARAM ListenParam_X;
  BOF::BOF_SOCKET_CONNECT_PARAM ConnectParam_X;
  BOF::BOF_SOCKET_READ_PARAM ReadParam_X;
  BOF::BOF_SOCKET_WRITE_PARAM WriteParam_X;
  BOF::BOF_SOCKET_DISCONNECT_PARAM DisconnectParam_X;
  BOF::BOF_SOCKET_EXIT_PARAM ExitParam_X;
  uint32_t i_U32, Timer_U32, Start_U32, Delta_U32, Max_U32, ServerOpTicket_U32, ClientOpTicket_U32, SessionOpTicket_U32;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadServer;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadClient;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadSession;
  BOF::BOF_SOCKET_OPERATION_RESULT ServerOperationResult_X, ClientOperationResult_X, SessionOperationResult_X;
  BOF::BofSocket *pListenSocket_O = nullptr;
  const char pWelcome_c[] = "220 Welcome !\r\n";
  uint8_t pBuffer_U8[0x100];

  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
  EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 0);

  // Create server listening thread
  SocketThreadParam_X.Reset();
  SocketThreadParam_X.Name_S = "SockIo_Lis_0";
  // for ut under qemu/docker do not use fifo scheduler
  SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
  SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
  SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
  SocketThreadParam_X.pListeningSocket_O = nullptr;
  SocketThreadParam_X.pSocket_O = nullptr;
  puSocketThreadServer = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
  ASSERT_TRUE(puSocketThreadServer != nullptr);
  // uint16_t Free_U16 = BOF::BofSocket::S_FindFreePort(false, DEFAULT_LISTENING_PORT, 65535);

  ListenParam_X.Reset();
  ListenParam_X.JustOnce_B = false; // For "ever"
  ListenParam_X.NbMaxClient_U32 = 3;
  ListenParam_X.SrcIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
  ListenParam_X.SrcPort_U16 = DEFAULT_LISTENING_PORT;

  // Timeout of listen must be at least half of DEFAULT_INTER_PROCESS_TIMEOUT
  EXPECT_EQ(puSocketThreadServer->ProgramSocketOperation(100 /*DEFAULT_INTER_PROCESS_TIMEOUT*/, ListenParam_X, ServerOpTicket_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOpTicket_U32, 1);
  EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN);
  EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32);
  EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(ServerOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
  EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);
  // EXPECT_EQ(ServerOperationResult_X.Time_U32,0);
  pListenSocket_O = ServerOperationResult_X.pSocket_O;
  // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 1);

  ConnectParam_X.Reset();
  ConnectParam_X.Tcp_B = true;
  ConnectParam_X.SrcIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
  ConnectParam_X.SrcPort_U16 = 0;
  ConnectParam_X.DstIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
  ConnectParam_X.DstPort_U16 = DEFAULT_LISTENING_PORT;
  // Create client threads
  SocketThreadParam_X.Reset();
  SocketThreadParam_X.Name_S = "SockIo_Clt_0";
  // for ut under qemu/docker do not use fifo scheduler
  SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
  SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
  SocketThreadParam_X.pListeningSocket_O = nullptr;
  SocketThreadParam_X.pSocket_O = nullptr;
  puSocketThreadClient = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
  ASSERT_TRUE(puSocketThreadClient != nullptr);

  // Connect client to server
  EXPECT_EQ(puSocketThreadClient->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ConnectParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ClientOpTicket_U32, 1);
  EXPECT_EQ(puSocketThreadClient->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT);
  EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
  EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(ClientOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
  EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
  // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);

  // Get server session creation param: answer posted due to connect event
  ServerOperationResult_X.Reset();
  EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT);
  EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32 + 1);
  EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(ServerOperationResult_X.pSocket_O, nullptr);
  EXPECT_NE(ServerOperationResult_X.pSocket_O, ClientOperationResult_X.pSocket_O);
  EXPECT_NE(ServerOperationResult_X.pSocket_O, pListenSocket_O);
  EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
  EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);

  // Create session threads
  SocketThreadParam_X.Reset();
  SocketThreadParam_X.Name_S = "SockIo_Ses_0";
  // for ut under qemu/docker do not use fifo scheduler
  SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
  SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
  SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
  SocketThreadParam_X.pListeningSocket_O = nullptr;
  SocketThreadParam_X.pSocket_O = ServerOperationResult_X.pSocket_O; // Give it by default so BOF_SOCKET_OPERATION_DISCONNECT can be executed correctly
  puSocketThreadSession = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
  ASSERT_TRUE(puSocketThreadSession != nullptr);

  Max_U32 = 0;
  Start_U32 = BOF::Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < 500; i_U32++)
  {
    Timer_U32 = BOF::Bof_GetMsTickCount();
    WriteParam_X.Reset();
    WriteParam_X.pSocket_O = nullptr; // pListenSocket_O;
    WriteParam_X.Nb_U32 = strlen(pWelcome_c);
    WriteParam_X.pBuffer_U8 = (uint8_t *)pWelcome_c;
    //    BOFERR e = puSocketThreadClient->GetSocket()->V_WriteData(1000, WriteParam_X.Nb_U32, WriteParam_X.pBuffer_U8);
    // BOF::BofSocket *p = puSocketThreadServer->GetSocket();
    // BOFERR e = puSocketThreadServer->GetSocket()->V_WriteData(1000, WriteParam_X.Nb_U32, WriteParam_X.pBuffer_U8);
    EXPECT_EQ(puSocketThreadSession->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, WriteParam_X, SessionOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(SessionOpTicket_U32, 1 + i_U32);
    EXPECT_EQ(puSocketThreadSession->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, SessionOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(SessionOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_WRITE);
    EXPECT_EQ(SessionOperationResult_X.OpTicket_U32, SessionOpTicket_U32);
    EXPECT_EQ(SessionOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(SessionOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(SessionOperationResult_X.Size_U32, WriteParam_X.Nb_U32);
    EXPECT_EQ(SessionOperationResult_X.pBuffer_U8, WriteParam_X.pBuffer_U8);
    EXPECT_LT(SessionOperationResult_X.Time_U32, DEFAULT_INTER_PROCESS_TIMEOUT);

    memset(pBuffer_U8, 0, sizeof(pBuffer_U8));
    ReadParam_X.Reset();
    ReadParam_X.pSocket_O = nullptr;
    ReadParam_X.Nb_U32 = strlen(pWelcome_c);
    ReadParam_X.pBuffer_U8 = pBuffer_U8;
    EXPECT_EQ(puSocketThreadClient->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ReadParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOpTicket_U32, 2 + i_U32);
    EXPECT_EQ(puSocketThreadClient->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_READ);
    EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
    EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(ClientOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ClientOperationResult_X.Size_U32, ReadParam_X.Nb_U32);
    EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, ReadParam_X.pBuffer_U8);
    EXPECT_LT(ClientOperationResult_X.Time_U32, DEFAULT_INTER_PROCESS_TIMEOUT);

    EXPECT_STREQ(pWelcome_c, (char *)ReadParam_X.pBuffer_U8);
    Delta_U32 = BOF::Bof_ElapsedMsTime(Timer_U32);
    if (Delta_U32 > Max_U32)
    {
      Max_U32 = Delta_U32;
    }
    EXPECT_LT(Delta_U32, 40);
    if ((Delta_U32 > 30) || ((i_U32 % 50) == 0))
    {
      printf("%d: Iter %d Delta %d Max is %d ms\n", BOF::Bof_GetMsTickCount(), i_U32, Delta_U32, Max_U32);
    }
  }
  printf("%d: Max is %d ms DeltaT %d ms\n", BOF::Bof_GetMsTickCount(), Max_U32, BOF::Bof_ElapsedMsTime(Start_U32));

  ExitParam_X.Reset();
  ExitParam_X.Unused_U32 = 0;
  EXPECT_EQ(puSocketThreadClient->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ExitParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ClientOpTicket_U32, 2 + i_U32); // Sesion 0 is used to test error just above
  EXPECT_EQ(puSocketThreadClient->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT);
  EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
  EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_CANCEL);
  EXPECT_EQ(ClientOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
  EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
  // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);

  // Leave session threads

  ExitParam_X.Reset();
  ExitParam_X.Unused_U32 = 0;
  EXPECT_EQ(puSocketThreadSession->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ExitParam_X, SessionOpTicket_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(SessionOpTicket_U32, 1 + i_U32); // Sesion 0 is used to test error just above
  EXPECT_EQ(puSocketThreadSession->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, SessionOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(SessionOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT);
  EXPECT_EQ(SessionOperationResult_X.OpTicket_U32, SessionOpTicket_U32);
  EXPECT_EQ(SessionOperationResult_X.Sts_E, BOF_ERR_CANCEL);
  EXPECT_EQ(SessionOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(SessionOperationResult_X.Size_U32, 0);
  EXPECT_EQ(SessionOperationResult_X.pBuffer_U8, nullptr);
  // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);

  // Leave listen thread
  ExitParam_X.Reset();
  ExitParam_X.Unused_U32 = 0;
  EXPECT_EQ(puSocketThreadServer->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ExitParam_X, ServerOpTicket_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOpTicket_U32, 2);
  EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT);
  EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32);
  EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_CANCEL);

  EXPECT_EQ(ServerOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
  EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);
  // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);

  BOF::Bof_MsSleep(50);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
  EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 0);
}

TEST(SockIo_Test, ReadWrite)
{
  BOF::BOF_SOCKET_THREAD_PARAM SocketThreadParam_X;
  BOF::BOF_SOCKET_LISTEN_PARAM ListenParam_X;
  BOF::BOF_SOCKET_CONNECT_PARAM ConnectParam_X;
  BOF::BOF_SOCKET_READ_PARAM ReadParam_X;
  BOF::BOF_SOCKET_WRITE_PARAM WriteParam_X;
  BOF::BOF_SOCKET_DISCONNECT_PARAM DisconnectParam_X;
  BOF::BOF_SOCKET_EXIT_PARAM ExitParam_X;
  uint32_t i_U32, j_U32, k_U32, Timer_U32, Start_U32, Delta_U32, Max_U32, ServerOpTicket_U32, SessionOpTicket_U32, ClientOpTicket_U32, NbThread_U32;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadServer;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadClient;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadSession;
  std::vector<std::unique_ptr<BOF::BofSocketThread>> puSocketThreadSessionCollection;
  std::vector<std::unique_ptr<BOF::BofSocketThread>> puSocketThreadClientCollection;
  BOF::BOF_SOCKET_OPERATION_RESULT ServerOperationResult_X, SessionOperationResult_X, ClientOperationResult_X;
  BOF::BofSocket *pListenSocket_O = nullptr;
  uint8_t *pRxBuffer_U8[MAX_NB_CLIENT_SESSION];
  uint8_t *pTxBuffer_U8[MAX_NB_CLIENT_SESSION];

  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
  EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 0);

  // Create server listening thread
  NbThread_U32 = 0;
  SocketThreadParam_X.Reset();
  SocketThreadParam_X.Name_S = "SockIo_Lis_0";
  // for ut under qemu/docker do not use fifo scheduler
  SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
  SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
  SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
  SocketThreadParam_X.pListeningSocket_O = nullptr;
  SocketThreadParam_X.pSocket_O = nullptr;
  puSocketThreadServer = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
  ASSERT_TRUE(puSocketThreadServer != nullptr);
  NbThread_U32++;

  ListenParam_X.Reset();
  ListenParam_X.JustOnce_B = false; // For "ever"
  ListenParam_X.NbMaxClient_U32 = 3;
  ListenParam_X.SrcIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
  ListenParam_X.SrcPort_U16 = DEFAULT_LISTENING_PORT;

  // Timeout of listen must be at least half of DEFAULT_INTER_PROCESS_TIMEOUT
  EXPECT_EQ(puSocketThreadServer->ProgramSocketOperation(100 /*DEFAULT_INTER_PROCESS_TIMEOUT*/, ListenParam_X, ServerOpTicket_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOpTicket_U32, 1);
  EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN);
  EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32);
  EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(ServerOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
  EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);
  // EXPECT_EQ(ServerOperationResult_X.Time_U32,0);
  pListenSocket_O = ServerOperationResult_X.pSocket_O;
  // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 1);

  ConnectParam_X.Reset();
  ConnectParam_X.Tcp_B = true;
  ConnectParam_X.SrcIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
  ConnectParam_X.SrcPort_U16 = 0;
  ConnectParam_X.DstIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
  ConnectParam_X.DstPort_U16 = DEFAULT_LISTENING_PORT;
  for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
  {
    // Alloc and init data buffer
    pRxBuffer_U8[j_U32] = new uint8_t[MAX_IO_SIZE];
    ASSERT_TRUE(pRxBuffer_U8[j_U32] != nullptr);
    pTxBuffer_U8[j_U32] = new uint8_t[MAX_IO_SIZE];
    ASSERT_TRUE(pTxBuffer_U8[j_U32] != nullptr);
    for (k_U32 = 0; k_U32 < MAX_IO_SIZE; k_U32++)
    {
      pTxBuffer_U8[j_U32][k_U32] = static_cast<uint8_t>(k_U32);
    }
    // Create client threads
    SocketThreadParam_X.Reset();
    SocketThreadParam_X.Name_S = "SockIo_Clt_" + std::to_string(j_U32);
    // for ut under qemu/docker do not use fifo scheduler
    SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
    SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
    SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
    SocketThreadParam_X.pListeningSocket_O = nullptr;
    SocketThreadParam_X.pSocket_O = nullptr;
    puSocketThreadClient = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
    ASSERT_TRUE(puSocketThreadClient != nullptr);

    // Connect client to server
    EXPECT_EQ(puSocketThreadClient->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ConnectParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOpTicket_U32, 1);
    EXPECT_EQ(puSocketThreadClient->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT);
    EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
    EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(ClientOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
    // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
    puSocketThreadClientCollection.push_back(std::move(puSocketThreadClient));
    NbThread_U32++;
    // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);

    // Get server session creation param: answer posted due to connect event
    ServerOperationResult_X.Reset();
    EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT);
    EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32 + j_U32 + 1);
    EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(ServerOperationResult_X.pSocket_O, nullptr);
    EXPECT_NE(ServerOperationResult_X.pSocket_O, ClientOperationResult_X.pSocket_O);
    EXPECT_NE(ServerOperationResult_X.pSocket_O, pListenSocket_O);
    EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);

    // Create session threads
    SocketThreadParam_X.Reset();
    SocketThreadParam_X.Name_S = "SockIo_Ses_" + std::to_string(j_U32);
    // for ut under qemu/docker do not use fifo scheduler
    SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
    SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
    SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
    SocketThreadParam_X.pListeningSocket_O = nullptr;
    SocketThreadParam_X.pSocket_O = ServerOperationResult_X.pSocket_O; // Give it by default so BOF_SOCKET_OPERATION_DISCONNECT can be executed correctly
    puSocketThreadSession = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
    ASSERT_TRUE(puSocketThreadSession != nullptr);
    puSocketThreadSessionCollection.push_back(std::move(puSocketThreadSession));
    NbThread_U32++;
    // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
  }
  EXPECT_EQ(puSocketThreadClientCollection.size(), MAX_NB_CLIENT_SESSION);
  EXPECT_EQ(puSocketThreadSessionCollection.size(), MAX_NB_CLIENT_SESSION);

  Max_U32 = 0;
  Start_U32 = BOF::Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < MAX_NB_TEST_LOOP; i_U32++)
  {
    Timer_U32 = BOF::Bof_GetMsTickCount();
    NbThread_U32 = 1 + (2 * MAX_NB_CLIENT_SESSION);
    EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), NbThread_U32);
    // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
    {
      WriteParam_X.Reset();
      WriteParam_X.pSocket_O = nullptr;
      WriteParam_X.Nb_U32 = MAX_IO_SIZE;
      WriteParam_X.pBuffer_U8 = pTxBuffer_U8[j_U32];
      EXPECT_EQ(puSocketThreadClientCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, WriteParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ClientOpTicket_U32, 2 + i_U32);

      memset(pRxBuffer_U8[j_U32], 0, MAX_IO_SIZE);
      ReadParam_X.Reset();
      ReadParam_X.pSocket_O = nullptr;
      ReadParam_X.Nb_U32 = MAX_IO_SIZE;
      ReadParam_X.pBuffer_U8 = pRxBuffer_U8[j_U32];
      EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ReadParam_X, SessionOpTicket_U32), BOF_ERR_NO_ERROR);
      EXPECT_EQ(SessionOpTicket_U32, 1 + i_U32);
      EXPECT_EQ(puSocketThreadClientCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_WRITE);
      EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
      EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_NE(ClientOperationResult_X.pSocket_O, nullptr);
      EXPECT_EQ(ClientOperationResult_X.Size_U32, MAX_IO_SIZE);
      EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, pTxBuffer_U8[j_U32]);
      EXPECT_LT(ClientOperationResult_X.Time_U32, DEFAULT_INTER_PROCESS_TIMEOUT);

      EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, SessionOperationResult_X), BOF_ERR_NO_ERROR);
      EXPECT_EQ(SessionOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_READ);
      EXPECT_EQ(SessionOperationResult_X.OpTicket_U32, SessionOpTicket_U32);
      EXPECT_EQ(SessionOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_NE(SessionOperationResult_X.pSocket_O, nullptr);
      EXPECT_EQ(SessionOperationResult_X.Size_U32, MAX_IO_SIZE);
      EXPECT_EQ(SessionOperationResult_X.pBuffer_U8, pRxBuffer_U8[j_U32]);
      EXPECT_LT(SessionOperationResult_X.Time_U32, DEFAULT_INTER_PROCESS_TIMEOUT);

      for (k_U32 = 0; k_U32 < MAX_IO_SIZE; k_U32++)
      {
        EXPECT_EQ(pRxBuffer_U8[j_U32][k_U32], static_cast<uint8_t>(k_U32));
      }
    }
    Delta_U32 = BOF::Bof_ElapsedMsTime(Timer_U32);
    if (Delta_U32 > Max_U32)
    {
      Max_U32 = Delta_U32;
    }
    EXPECT_LT(Delta_U32, 2000);
    if ((Delta_U32 > 1500) || ((i_U32 % 50) == 0))
    {
      printf("%d: Iter %d Delta %d Max is %d ms\n", BOF::Bof_GetMsTickCount(), i_U32, Delta_U32, Max_U32);
    }
  }
  printf("%d: Max is %d ms DeltaT %d ms\n", BOF::Bof_GetMsTickCount(), Max_U32, BOF::Bof_ElapsedMsTime(Start_U32));
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), NbThread_U32);
  // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);

  // Error
  j_U32 = 0;
  WriteParam_X.Reset();
  WriteParam_X.pSocket_O = nullptr;
  WriteParam_X.Nb_U32 = MAX_IO_SIZE;
  WriteParam_X.pBuffer_U8 = pTxBuffer_U8[j_U32];
  EXPECT_EQ(puSocketThreadClientCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, WriteParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ClientOpTicket_U32, 2 + MAX_NB_TEST_LOOP);

  memset(pRxBuffer_U8[j_U32], 0, MAX_IO_SIZE);
  ReadParam_X.Reset();
  ReadParam_X.pSocket_O = nullptr;
  ReadParam_X.Nb_U32 = MAX_IO_SIZE + 1;
  ReadParam_X.pBuffer_U8 = pRxBuffer_U8[j_U32];
  EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ReadParam_X, SessionOpTicket_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(SessionOpTicket_U32, 1 + MAX_NB_TEST_LOOP);
  EXPECT_EQ(puSocketThreadClientCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_WRITE);
  EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
  EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(ClientOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(ClientOperationResult_X.Size_U32, MAX_IO_SIZE);
  EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, pTxBuffer_U8[j_U32]);
  EXPECT_LT(ClientOperationResult_X.Time_U32, DEFAULT_INTER_PROCESS_TIMEOUT);

  EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT * 1.5f, SessionOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(SessionOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_READ);
  EXPECT_EQ(SessionOperationResult_X.OpTicket_U32, SessionOpTicket_U32);
  EXPECT_EQ(SessionOperationResult_X.Sts_E, BOF_ERR_ETIMEDOUT);
  EXPECT_NE(SessionOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(SessionOperationResult_X.Size_U32, MAX_IO_SIZE);
  EXPECT_EQ(SessionOperationResult_X.pBuffer_U8, pRxBuffer_U8[j_U32]);
  EXPECT_GE(SessionOperationResult_X.Time_U32, DEFAULT_INTER_PROCESS_TIMEOUT);

  // Leave client threads
  for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
  {
    ExitParam_X.Reset();
    ExitParam_X.Unused_U32 = 0;
    EXPECT_EQ(puSocketThreadClientCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ExitParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOpTicket_U32, 2 + MAX_NB_TEST_LOOP + ((j_U32 == 0) ? 1 : 0)); // Sesion 0 is used to test error just above
    EXPECT_EQ(puSocketThreadClientCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT);
    EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
    EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_CANCEL);
    EXPECT_EQ(ClientOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
    // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
    NbThread_U32--;
    // EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), NbThread_U32);
    // BOF::Bof_MsSleep(1);
    // BOF_GTEST_EXPECT_BETWEEN(NbThread_U32, BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
  }

  // Leave session threads
  for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
  {
    ExitParam_X.Reset();
    ExitParam_X.Unused_U32 = 0;
    EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ExitParam_X, SessionOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(SessionOpTicket_U32, 1 + MAX_NB_TEST_LOOP + ((j_U32 == 0) ? 1 : 0)); // Sesion 0 is used to test error just above
    EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, SessionOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(SessionOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT);
    EXPECT_EQ(SessionOperationResult_X.OpTicket_U32, SessionOpTicket_U32);
    EXPECT_EQ(SessionOperationResult_X.Sts_E, BOF_ERR_CANCEL);
    EXPECT_EQ(SessionOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(SessionOperationResult_X.Size_U32, 0);
    EXPECT_EQ(SessionOperationResult_X.pBuffer_U8, nullptr);
    // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
    NbThread_U32--;
    // EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), NbThread_U32);
    // BOF::Bof_MsSleep(1);
    // BOF_GTEST_EXPECT_BETWEEN(NbThread_U32, BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    BOF_SAFE_DELETE_ARRAY(pRxBuffer_U8[j_U32]);
    BOF_SAFE_DELETE_ARRAY(pTxBuffer_U8[j_U32]);
  }
  // Leave listen threads
  ExitParam_X.Reset();
  ExitParam_X.Unused_U32 = 0;
  EXPECT_EQ(puSocketThreadServer->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ExitParam_X, ServerOpTicket_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOpTicket_U32, 2);
  EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT);
  EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32);
  EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_CANCEL);

  EXPECT_EQ(ServerOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
  EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);
  // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);

  BOF::Bof_MsSleep(50);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
  EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 0);
}

TEST(SockIo_Test, CancelReadWrite)
{
  BOF::BOF_SOCKET_THREAD_PARAM SocketThreadParam_X;
  BOF::BOF_SOCKET_LISTEN_PARAM ListenParam_X;
  BOF::BOF_SOCKET_CONNECT_PARAM ConnectParam_X;
  BOF::BOF_SOCKET_READ_PARAM ReadParam_X;
  BOF::BOF_SOCKET_WRITE_PARAM WriteParam_X;
  BOF::BOF_SOCKET_DISCONNECT_PARAM DisconnectParam_X;
  BOF::BOF_SOCKET_EXIT_PARAM ExitParam_X;
  uint32_t i_U32, j_U32, k_U32, Timer_U32, Start_U32, Delta_U32, Max_U32, ServerOpTicket_U32, SessionOpTicket_U32, ClientOpTicket_U32, NbThread_U32;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadServer;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadClient;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadSession;
  std::vector<std::unique_ptr<BOF::BofSocketThread>> puSocketThreadSessionCollection;
  std::vector<std::unique_ptr<BOF::BofSocketThread>> puSocketThreadClientCollection;
  BOF::BOF_SOCKET_OPERATION_RESULT ServerOperationResult_X, SessionOperationResult_X, ClientOperationResult_X;
  BOF::BofSocket *pListenSocket_O = nullptr;
  uint8_t *pRxBuffer_U8[MAX_NB_CLIENT_SESSION];
  uint8_t *pTxBuffer_U8[MAX_NB_CLIENT_SESSION];

  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
  EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 0);

  // Create server listening thread
  NbThread_U32 = 0;
  SocketThreadParam_X.Reset();
  SocketThreadParam_X.Name_S = "SockIo_Lis_0";
  // for ut under qemu/docker do not use fifo scheduler
  SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
  SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
  SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
  SocketThreadParam_X.pListeningSocket_O = nullptr;
  SocketThreadParam_X.pSocket_O = nullptr;
  puSocketThreadServer = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
  ASSERT_TRUE(puSocketThreadServer != nullptr);
  NbThread_U32++;

  ListenParam_X.Reset();
  ListenParam_X.JustOnce_B = false; // For "ever"
  ListenParam_X.NbMaxClient_U32 = 3;
  ListenParam_X.SrcIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
  ListenParam_X.SrcPort_U16 = DEFAULT_LISTENING_PORT;

  // Timeout of listen must be at least half of DEFAULT_INTER_PROCESS_TIMEOUT
  EXPECT_EQ(puSocketThreadServer->ProgramSocketOperation(100 /*DEFAULT_INTER_PROCESS_TIMEOUT*/, ListenParam_X, ServerOpTicket_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOpTicket_U32, 1);
  EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN);
  EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32);
  EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(ServerOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
  EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);
  // EXPECT_EQ(ServerOperationResult_X.Time_U32,0);
  pListenSocket_O = ServerOperationResult_X.pSocket_O;
  // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 1);

  ConnectParam_X.Reset();
  ConnectParam_X.Tcp_B = true;
  ConnectParam_X.SrcIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
  ConnectParam_X.SrcPort_U16 = 0;
  ConnectParam_X.DstIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
  ConnectParam_X.DstPort_U16 = DEFAULT_LISTENING_PORT;
  for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
  {
    // Alloc and init data buffer
    pRxBuffer_U8[j_U32] = new uint8_t[MAX_IO_SIZE];
    ASSERT_TRUE(pRxBuffer_U8[j_U32] != nullptr);
    pTxBuffer_U8[j_U32] = new uint8_t[MAX_IO_SIZE];
    ASSERT_TRUE(pTxBuffer_U8[j_U32] != nullptr);
    for (k_U32 = 0; k_U32 < MAX_IO_SIZE; k_U32++)
    {
      pTxBuffer_U8[j_U32][k_U32] = static_cast<uint8_t>(k_U32);
    }
    // Create client threads
    SocketThreadParam_X.Reset();
    SocketThreadParam_X.Name_S = "SockIo_Clt_" + std::to_string(j_U32);
    // for ut under qemu/docker do not use fifo scheduler
    SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
    SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
    SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
    SocketThreadParam_X.pListeningSocket_O = nullptr;
    SocketThreadParam_X.pSocket_O = nullptr;
    puSocketThreadClient = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
    ASSERT_TRUE(puSocketThreadClient != nullptr);

    // Connect client to server
    EXPECT_EQ(puSocketThreadClient->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ConnectParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOpTicket_U32, 1);
    EXPECT_EQ(puSocketThreadClient->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT);
    EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
    EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(ClientOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
    // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
    puSocketThreadClientCollection.push_back(std::move(puSocketThreadClient));
    NbThread_U32++;
    // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);

    // Get server session creation param: answer posted due to connect event
    ServerOperationResult_X.Reset();
    EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT);
    EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32 + j_U32 + 1);
    EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(ServerOperationResult_X.pSocket_O, nullptr);
    EXPECT_NE(ServerOperationResult_X.pSocket_O, ClientOperationResult_X.pSocket_O);
    EXPECT_NE(ServerOperationResult_X.pSocket_O, pListenSocket_O);
    EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);

    // Create session threads
    SocketThreadParam_X.Reset();
    SocketThreadParam_X.Name_S = "SockIo_Ses_" + std::to_string(j_U32);
    // for ut under qemu/docker do not use fifo scheduler
    SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER;
    SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;
    SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
    SocketThreadParam_X.pListeningSocket_O = nullptr;
    SocketThreadParam_X.pSocket_O = ServerOperationResult_X.pSocket_O; // Give it by default so BOF_SOCKET_OPERATION_DISCONNECT can be executed correctly
    puSocketThreadSession = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
    ASSERT_TRUE(puSocketThreadSession != nullptr);
    puSocketThreadSessionCollection.push_back(std::move(puSocketThreadSession));
    NbThread_U32++;
    // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
  }
  EXPECT_EQ(puSocketThreadClientCollection.size(), MAX_NB_CLIENT_SESSION);
  EXPECT_EQ(puSocketThreadSessionCollection.size(), MAX_NB_CLIENT_SESSION);

  Max_U32 = 0;
  Start_U32 = BOF::Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < MAX_NB_TEST_LOOP; i_U32++)
  {
    Timer_U32 = BOF::Bof_GetMsTickCount();
    NbThread_U32 = 1 + (2 * MAX_NB_CLIENT_SESSION);
    EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), NbThread_U32);
    // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
    {
      EXPECT_EQ(puSocketThreadClientCollection[j_U32]->CancelSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT * 2), BOF_ERR_NO_ERROR);
      WriteParam_X.Reset();
      WriteParam_X.pSocket_O = nullptr;
      WriteParam_X.Nb_U32 = MAX_IO_SIZE;
      WriteParam_X.pBuffer_U8 = pTxBuffer_U8[j_U32];
      EXPECT_EQ(puSocketThreadClientCollection[j_U32]->NumberOfOperationPending(), 0);
      EXPECT_EQ(puSocketThreadClientCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, WriteParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
      EXPECT_EQ(puSocketThreadClientCollection[j_U32]->NumberOfOperationPending(), 1);
      EXPECT_EQ(puSocketThreadClientCollection[j_U32]->CancelSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT * 2), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ClientOpTicket_U32, 2 + i_U32);

      memset(pRxBuffer_U8[j_U32], 0, MAX_IO_SIZE);
      ReadParam_X.Reset();
      ReadParam_X.pSocket_O = nullptr;
      ReadParam_X.Nb_U32 = MAX_IO_SIZE;
      ReadParam_X.pBuffer_U8 = pRxBuffer_U8[j_U32];
      EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->NumberOfOperationPending(), 0);
      EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ReadParam_X, SessionOpTicket_U32), BOF_ERR_NO_ERROR);
      EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->NumberOfOperationPending(), 1);
      EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->CancelSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT * 2), BOF_ERR_NO_ERROR);
      EXPECT_EQ(SessionOpTicket_U32, 1 + i_U32);

      EXPECT_EQ(puSocketThreadClientCollection[j_U32]->NumberOfOperationPending(), 0);
      EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->NumberOfOperationPending(), 0);

      EXPECT_NE(puSocketThreadClientCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
      EXPECT_NE(puSocketThreadSessionCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, SessionOperationResult_X), BOF_ERR_NO_ERROR);
    }
    Delta_U32 = BOF::Bof_ElapsedMsTime(Timer_U32);
    if (Delta_U32 > Max_U32)
    {
      Max_U32 = Delta_U32;
    }
    EXPECT_LT(Delta_U32, 4000);
    if ((Delta_U32 > 3600) || ((i_U32 % 50) == 0))
    {
      printf("%d: Iter %d Delta %d Max is %d ms\n", BOF::Bof_GetMsTickCount(), i_U32, Delta_U32, Max_U32);
    }
  }
  printf("%d: Max is %d ms DeltaT %d ms\n", BOF::Bof_GetMsTickCount(), Max_U32, BOF::Bof_ElapsedMsTime(Start_U32));
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), NbThread_U32);
  // EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);

  // Leave client threads
  for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
  {
    ExitParam_X.Reset();
    ExitParam_X.Unused_U32 = 0;
    EXPECT_EQ(puSocketThreadClientCollection[j_U32]->NumberOfResultPending(), 0);
    EXPECT_EQ(puSocketThreadClientCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ExitParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
    // EXPECT_EQ(puSocketThreadClientCollection[j_U32]->NumberOfResultPending(), 1);
    EXPECT_EQ(ClientOpTicket_U32, 2 + MAX_NB_TEST_LOOP);
    EXPECT_EQ(puSocketThreadClientCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT);
    EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
    EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_CANCEL);
    EXPECT_EQ(ClientOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
    // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
    NbThread_U32--;
    // EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), NbThread_U32);
    // BOF::Bof_MsSleep(1);
    // BOF_GTEST_EXPECT_BETWEEN(NbThread_U32, BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
  }

  // Leave session threads
  for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
  {
    ExitParam_X.Reset();
    ExitParam_X.Unused_U32 = 0;
    EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->NumberOfResultPending(), 0);
    EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ExitParam_X, SessionOpTicket_U32), BOF_ERR_NO_ERROR);
    // EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->NumberOfResultPending(), 1);
    EXPECT_EQ(SessionOpTicket_U32, 1 + MAX_NB_TEST_LOOP);
    EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, SessionOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(SessionOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT);
    EXPECT_EQ(SessionOperationResult_X.OpTicket_U32, SessionOpTicket_U32);
    EXPECT_EQ(SessionOperationResult_X.Sts_E, BOF_ERR_CANCEL);
    EXPECT_EQ(SessionOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(SessionOperationResult_X.Size_U32, 0);
    EXPECT_EQ(SessionOperationResult_X.pBuffer_U8, nullptr);
    // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
    NbThread_U32--;
    // EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), NbThread_U32);
    // BOF::Bof_MsSleep(1);
    // BOF_GTEST_EXPECT_BETWEEN(NbThread_U32, BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    BOF_SAFE_DELETE_ARRAY(pRxBuffer_U8[j_U32]);
    BOF_SAFE_DELETE_ARRAY(pTxBuffer_U8[j_U32]);
  }
  // Leave listen threads
  ExitParam_X.Reset();
  ExitParam_X.Unused_U32 = 0;
  EXPECT_EQ(puSocketThreadServer->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ExitParam_X, ServerOpTicket_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOpTicket_U32, 2);
  EXPECT_EQ(puSocketThreadServer->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ServerOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ServerOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT);
  EXPECT_EQ(ServerOperationResult_X.OpTicket_U32, ServerOpTicket_U32);
  EXPECT_EQ(ServerOperationResult_X.Sts_E, BOF_ERR_CANCEL);

  EXPECT_EQ(ServerOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(ServerOperationResult_X.Size_U32, 0);
  EXPECT_EQ(ServerOperationResult_X.pBuffer_U8, nullptr);
  // EXPECT_NE(ClientOperationResult_X.Time_U32, 0);

  BOF::Bof_MsSleep(50);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
  EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 0);
}
