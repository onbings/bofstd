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

#define MAX_NB_TEST_LOOP 2500
#define DEFAULT_LISTENING_PORT 60000
#define DEFAULT_INTER_PROCESS_TIMEOUT 100
#define MAX_NB_CLIENT_SESSION 4
#define MAX_NB_OP_PENDING 1

struct Foo
{  
  std::string s;
  Foo(const char *p) { s = p; std::cout << "Foo..." << s << "\n"; }
  ~Foo()  { std::cout << "~Foo..." << s << "\n"; }
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
  printf("%d: Create/Destroy %d threads\n", BOF::Bof_GetMsTickCount(), MAX_NB_TEST_LOOP);
  for (i_U32 = 0; i_U32 < MAX_NB_TEST_LOOP; i_U32++)
  {
    Timer_U32 = BOF::Bof_GetMsTickCount();
    SocketThreadParam_X.Name_S = "SockIo_Test_" + std::to_string(i_U32);
    SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_FIFO;
    SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_036;
    SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
    SocketThreadParam_X.pListeningSocket_O = nullptr;
    SocketThreadParam_X.pSocket_O = nullptr;
    //printf("%d: Create new thread\n", BOF::Bof_GetMsTickCount());
    puSocketThread = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
    ASSERT_TRUE(puSocketThread != nullptr);
    //printf("%d: Delete thread\n", BOF::Bof_GetMsTickCount());
    puSocketThread.reset();
    Delta_U32 = BOF::Bof_ElapsedMsTime(Timer_U32);
    if (Delta_U32 > Max_U32)
    {
      Max_U32 = Delta_U32;
    }
    EXPECT_LT(Delta_U32, 20);
    if (Delta_U32 > 10)
    {
//      printf("%d: Iter %d Delta %d Max is %d ms\n", BOF::Bof_GetMsTickCount(), i_U32, Delta_U32, Max_U32);
    }
  }
  printf("%d: Max is %d ms\n", BOF::Bof_GetMsTickCount(), Max_U32);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
}


TEST(SockIo_Test, ListenConnectDisconnect)
{
  BOF::BOF_SOCKET_THREAD_PARAM SocketThreadParam_X;
  BOF::BOF_SOCKET_LISTEN_PARAM ListenParam_X;
  BOF::BOF_SOCKET_CONNECT_PARAM ConnectParam_X;
  BOF::BOF_SOCKET_DISCONNECT_PARAM DisconnectParam_X;
  uint32_t i_U32, Timer_U32, Start_U32, Delta_U32, Max_U32, ListenOpTicket_U32, SessionOpTicket_U32, ClientOpTicket_U32;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadListen;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadSession;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadClient;
  BOF::BOF_SOCKET_OPERATION_RESULT ListenOperationResult_X, SessionOperationResult_X, ClientOperationResult_X;
  BOF::BofSocket *pListenSocket_O = nullptr;

  Start_U32 = BOF::Bof_GetMsTickCount();

  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);

//Create server listening thread
  SocketThreadParam_X.Name_S = "SockIo_Listen_0";
  SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_FIFO;
  SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_036;
  SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
  SocketThreadParam_X.pListeningSocket_O = nullptr;
  SocketThreadParam_X.pSocket_O = nullptr;
  puSocketThreadListen = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
  ASSERT_TRUE(puSocketThreadListen != nullptr);
  ListenParam_X.JustOnce_B = false;  //For "ever"
  ListenParam_X.NbMaxClient_U32 = 3;
  ListenParam_X.SrcIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
  ListenParam_X.SrcPort_U16 = DEFAULT_LISTENING_PORT;
  EXPECT_EQ(puSocketThreadListen->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ListenParam_X, ListenOpTicket_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ListenOpTicket_U32, 1);
  EXPECT_EQ(puSocketThreadListen->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ListenOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ListenOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN);
  EXPECT_EQ(ListenOperationResult_X.OpTicket_U32, ListenOpTicket_U32);
  EXPECT_EQ(ListenOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(ListenOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(ListenOperationResult_X.Size_U32, 0);
  EXPECT_EQ(ListenOperationResult_X.pBuffer_U8, nullptr);
  //EXPECT_EQ(ListenOperationResult_X.Time_U32,0);
  pListenSocket_O = ListenOperationResult_X.pSocket_O;

//Create client connect thread
  SocketThreadParam_X.Name_S = "SockIo_Client_0";
  SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_FIFO;
  SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_036;
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
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 1);
  printf("%d: Connect/Disconnect %d times\n", BOF::Bof_GetMsTickCount(), MAX_NB_TEST_LOOP);
  for (i_U32 = 0; i_U32 < MAX_NB_TEST_LOOP; i_U32++)
  {
    Timer_U32 = BOF::Bof_GetMsTickCount();
//Connect client to server
    EXPECT_EQ(puSocketThreadClient->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ConnectParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOpTicket_U32, 1 + (i_U32 * 2));
    EXPECT_EQ(puSocketThreadClient->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT);
    EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
    EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(ClientOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
    //EXPECT_NE(ClientOperationResult_X.Time_U32, 0);

//Get server session creation param: answer posted due to connect event
    EXPECT_EQ(puSocketThreadListen->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ListenOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ListenOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN);
    EXPECT_EQ(ListenOperationResult_X.OpTicket_U32, ListenOpTicket_U32 + i_U32 + 1);
    EXPECT_EQ(ListenOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(ListenOperationResult_X.pSocket_O, nullptr);
    EXPECT_NE(ListenOperationResult_X.pSocket_O, ClientOperationResult_X.pSocket_O);
    EXPECT_NE(ListenOperationResult_X.pSocket_O, pListenSocket_O);
    EXPECT_EQ(ListenOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ListenOperationResult_X.pBuffer_U8, nullptr);
    //EXPECT_NE(ListenOperationResult_X.Time_U32, 0);
//Create session thread
    SocketThreadParam_X.Name_S = "SockIo_Session_0";
    SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_FIFO;
    SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_036;
    SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
    SocketThreadParam_X.pListeningSocket_O = nullptr;
    SocketThreadParam_X.pSocket_O = nullptr;
    puSocketThreadSession = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
    ASSERT_TRUE(puSocketThreadSession != nullptr);
    EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 3);

//Disconnect client
    DisconnectParam_X.Unused_U32 = 0;// ClientOperationResult_X.pSocket_O;
    EXPECT_EQ(puSocketThreadClient->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, DisconnectParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOpTicket_U32, 1 + (i_U32 * 2) + 1);
    EXPECT_EQ(puSocketThreadClient->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT);
    EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
    EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(ClientOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
    //EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
    EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 2);

//Disconnect session
    DisconnectParam_X.Unused_U32 = 0;// ListenOperationResult_X.pSocket_O;
    EXPECT_EQ(puSocketThreadSession->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, DisconnectParam_X, SessionOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(SessionOpTicket_U32, 1);
    EXPECT_EQ(puSocketThreadSession->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, SessionOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(SessionOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT);
    EXPECT_EQ(SessionOperationResult_X.OpTicket_U32, SessionOpTicket_U32);
    EXPECT_EQ(SessionOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(SessionOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(SessionOperationResult_X.Size_U32, 0);
    EXPECT_EQ(SessionOperationResult_X.pBuffer_U8, nullptr);
    //EXPECT_NE(SessionOperationResult_X.Time_U32, 0);
    EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 1);


    Delta_U32 = BOF::Bof_ElapsedMsTime(Timer_U32);
    if (Delta_U32 > Max_U32)
    {
      Max_U32 = Delta_U32;
    }
    EXPECT_LT(Delta_U32, 100);
    if (Delta_U32 > 30)
    {
      //printf("%d: Iter %d Delta %d Max is %d ms\n", BOF::Bof_GetMsTickCount(), i_U32, Delta_U32, Max_U32);
    }
  }
//Disconnect server
  DisconnectParam_X.Unused_U32 = 0;// pListenSocket_O;
  EXPECT_EQ(puSocketThreadListen->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT * 2, DisconnectParam_X, ListenOpTicket_U32), BOF_ERR_NO_ERROR);// Must be longer than the Listen timeout value to be sure to be catch
  EXPECT_EQ(ListenOpTicket_U32, 2);
  EXPECT_EQ(puSocketThreadListen->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ListenOperationResult_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(ListenOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT);
  EXPECT_EQ(ListenOperationResult_X.OpTicket_U32, ListenOpTicket_U32);
  EXPECT_EQ(ListenOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(ListenOperationResult_X.pSocket_O, nullptr);
  EXPECT_EQ(ListenOperationResult_X.Size_U32, 0);
  EXPECT_EQ(ListenOperationResult_X.pBuffer_U8, nullptr);
  EXPECT_NE(ListenOperationResult_X.Time_U32, 0);

  printf("%d: Max is %d ms DeltaT %d ms\n", BOF::Bof_GetMsTickCount(), Max_U32, BOF::Bof_ElapsedMsTime(Start_U32));
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
}


TEST(SockIo_Test, ListenMultipleConnect)
{
  BOF::BOF_SOCKET_THREAD_PARAM SocketThreadParam_X;
  BOF::BOF_SOCKET_LISTEN_PARAM ListenParam_X;
  BOF::BOF_SOCKET_CONNECT_PARAM ConnectParam_X;
  BOF::BOF_SOCKET_DISCONNECT_PARAM DisconnectParam_X;
  BOF::BOF_SOCKET_EXIT_PARAM ExitParam_X;
  uint32_t i_U32, j_U32, Timer_U32, Start_U32, Delta_U32, Max_U32, ListenOpTicket_U32, SessionOpTicket_U32, ClientOpTicket_U32, NbThread_U32;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadListen;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadClient;
  std::vector<std::unique_ptr<BOF::BofSocketThread>> puSocketThreadSessionCollection;
  std::unique_ptr<BOF::BofSocketThread> puSocketThreadSession;
  std::vector<std::unique_ptr<BOF::BofSocketThread>> puSocketThreadClientCollection;
  BOF::BOF_SOCKET_OPERATION_RESULT ListenOperationResult_X, SessionOperationResult_X, ClientOperationResult_X;
  BOF::BofSocket *pListenSocket_O = nullptr;

  Max_U32 = 0;
  Start_U32 = BOF::Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < MAX_NB_TEST_LOOP; i_U32++)
  {
    Timer_U32 = BOF::Bof_GetMsTickCount();
    NbThread_U32 = 0;
    EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
    EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);

    //Create server listening thread
    SocketThreadParam_X.Name_S = "SockIo_Listen_0";
    SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_FIFO;
    SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_036;
    SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
    SocketThreadParam_X.pListeningSocket_O = nullptr;
    SocketThreadParam_X.pSocket_O = nullptr;
    puSocketThreadListen = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
    ASSERT_TRUE(puSocketThreadListen != nullptr);

    ListenParam_X.JustOnce_B = false;  //For "ever"
    ListenParam_X.NbMaxClient_U32 = 3;
    ListenParam_X.SrcIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
    ListenParam_X.SrcPort_U16 = DEFAULT_LISTENING_PORT;
    EXPECT_EQ(puSocketThreadListen->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ListenParam_X, ListenOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ListenOpTicket_U32, 1);
    EXPECT_EQ(puSocketThreadListen->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ListenOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ListenOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN);
    EXPECT_EQ(ListenOperationResult_X.OpTicket_U32, ListenOpTicket_U32);
    EXPECT_EQ(ListenOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(ListenOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ListenOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ListenOperationResult_X.pBuffer_U8, nullptr);
    //EXPECT_EQ(ListenOperationResult_X.Time_U32,0);
    pListenSocket_O = ListenOperationResult_X.pSocket_O;
    BOF::Bof_MsSleep(1);   //Give some time to process the request
    NbThread_U32++;
    EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    if (i_U32 == 1)
    {

      flmkqsdjflksdj
      printf("jj");
    }

    ConnectParam_X.Tcp_B = true;
    ConnectParam_X.SrcIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
    ConnectParam_X.SrcPort_U16 = 0;
    ConnectParam_X.DstIpAddr_X = BOF::BOF_IPV4_ADDR_U32(127, 0, 0, 1);
    ConnectParam_X.DstPort_U16 = DEFAULT_LISTENING_PORT;
    for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
    {
      //Create client threads
      SocketThreadParam_X.Name_S = "SockIo_Client_" + std::to_string(j_U32);
      SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_FIFO;
      SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_036;
      SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
      SocketThreadParam_X.pListeningSocket_O = nullptr;
      SocketThreadParam_X.pSocket_O = nullptr;
      puSocketThreadClient = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
      ASSERT_TRUE(puSocketThreadClient != nullptr);

      //Connect client to server
      EXPECT_EQ(puSocketThreadClient->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ConnectParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ClientOpTicket_U32, 1);
      EXPECT_EQ(puSocketThreadClient->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT);
      EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
      EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_NE(ClientOperationResult_X.pSocket_O, nullptr);
      EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
      EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
      //EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
      puSocketThreadClientCollection.push_back(std::move(puSocketThreadClient));
      NbThread_U32++;
      EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);

      //Get server session creation param: answer posted due to connect event
      EXPECT_EQ(puSocketThreadListen->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ListenOperationResult_X), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ListenOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN);
      EXPECT_EQ(ListenOperationResult_X.OpTicket_U32, ListenOpTicket_U32 + j_U32 + 1);
      EXPECT_EQ(ListenOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_NE(ListenOperationResult_X.pSocket_O, nullptr);
      EXPECT_NE(ListenOperationResult_X.pSocket_O, ClientOperationResult_X.pSocket_O);
      EXPECT_NE(ListenOperationResult_X.pSocket_O, pListenSocket_O);
      EXPECT_EQ(ListenOperationResult_X.Size_U32, 0);
      EXPECT_EQ(ListenOperationResult_X.pBuffer_U8, nullptr);

      //Create session threads
      SocketThreadParam_X.Name_S = "SockIo_Session_" + std::to_string(j_U32);
      SocketThreadParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_FIFO;
      SocketThreadParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_036;
      SocketThreadParam_X.NbMaxOperationPending_U32 = MAX_NB_OP_PENDING;
      SocketThreadParam_X.pListeningSocket_O = nullptr;
      SocketThreadParam_X.pSocket_O = ListenOperationResult_X.pSocket_O;    //Give it by default so BOF_SOCKET_OPERATION_DISCONNECT can be executed correctly
      puSocketThreadSession = std::make_unique<BOF::BofSocketThread>(SocketThreadParam_X);
      ASSERT_TRUE(puSocketThreadSession != nullptr);
      puSocketThreadSessionCollection.push_back(std::move(puSocketThreadSession));
      BOF::Bof_MsSleep(1);   //Give some time to process the request
      NbThread_U32++;
      EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    }

    EXPECT_EQ(puSocketThreadClientCollection.size(), MAX_NB_CLIENT_SESSION);
    EXPECT_EQ(puSocketThreadSessionCollection.size(), MAX_NB_CLIENT_SESSION);

    //Disconnect client
    for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
    {
      DisconnectParam_X.Unused_U32 = 0;// ClientOperationResult_X.pSocket_O;
      EXPECT_EQ(puSocketThreadClientCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, DisconnectParam_X, ClientOpTicket_U32), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ClientOpTicket_U32, 2);
      EXPECT_EQ(puSocketThreadClientCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ClientOperationResult_X), BOF_ERR_NO_ERROR);
      EXPECT_EQ(ClientOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT);
      EXPECT_EQ(ClientOperationResult_X.OpTicket_U32, ClientOpTicket_U32);
      EXPECT_EQ(ClientOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_EQ(ClientOperationResult_X.pSocket_O, nullptr);
      EXPECT_EQ(ClientOperationResult_X.Size_U32, 0);
      EXPECT_EQ(ClientOperationResult_X.pBuffer_U8, nullptr);
      //EXPECT_NE(ClientOperationResult_X.Time_U32, 0);

      BOF::Bof_MsSleep(1);   //Give some time to process the request
      EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), NbThread_U32 - j_U32 - 1);
      EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    }
    //Disconnect session
    for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
    {
      DisconnectParam_X.Unused_U32 = 0;// ListenOperationResult_X.pSocket_O;
      EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, DisconnectParam_X, SessionOpTicket_U32), BOF_ERR_NO_ERROR);
      EXPECT_EQ(SessionOpTicket_U32, 1);
      EXPECT_EQ(puSocketThreadSessionCollection[j_U32]->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, SessionOperationResult_X), BOF_ERR_NO_ERROR);
      EXPECT_EQ(SessionOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT);
      EXPECT_EQ(SessionOperationResult_X.OpTicket_U32, SessionOpTicket_U32);
      EXPECT_EQ(SessionOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_EQ(SessionOperationResult_X.pSocket_O, nullptr);
      EXPECT_EQ(SessionOperationResult_X.Size_U32, 0);
      EXPECT_EQ(SessionOperationResult_X.pBuffer_U8, nullptr);
      //EXPECT_NE(SessionOperationResult_X.Time_U32, 0);
      //printf("BOF::BofSocket::S_BofSocketBalance() j %d => %d\n", j_U32, BOF::BofSocket::S_BofSocketBalance());
      BOF::Bof_MsSleep(1);   //Give some time to process the request
      EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), MAX_NB_CLIENT_SESSION - j_U32);
      EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    }
    EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 1);
    EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);
    //Disconnect server
    DisconnectParam_X.Unused_U32 = 0;// pListenSocket_O;
    EXPECT_EQ(puSocketThreadListen->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT * 2, DisconnectParam_X, ListenOpTicket_U32), BOF_ERR_NO_ERROR);// Must be longer than the Listen timeout value to be sure to be catch
    EXPECT_EQ(ListenOpTicket_U32, 2);
    EXPECT_EQ(puSocketThreadListen->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ListenOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ListenOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT);
    EXPECT_EQ(ListenOperationResult_X.OpTicket_U32, ListenOpTicket_U32);
    EXPECT_EQ(ListenOperationResult_X.Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_NE(ListenOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ListenOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ListenOperationResult_X.pBuffer_U8, nullptr);
    EXPECT_NE(ListenOperationResult_X.Time_U32, 0);

    EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
    EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32);

    //Leave client threads
    for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
    {
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
      //EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
      BOF::Bof_MsSleep(1);   //Give some time to process the request
      EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
      EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), NbThread_U32 - 1 - j_U32);

    }
    //Leave session threads
    for (j_U32 = 0; j_U32 < MAX_NB_CLIENT_SESSION; j_U32++)
    {
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
      //EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
      BOF::Bof_MsSleep(1);   //Give some time to process the request
      EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
      EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), MAX_NB_CLIENT_SESSION - j_U32);
    }
    //Leave listen threads
    ExitParam_X.Unused_U32 = 0;
    EXPECT_EQ(puSocketThreadListen->ProgramSocketOperation(DEFAULT_INTER_PROCESS_TIMEOUT, ExitParam_X, ListenOpTicket_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ListenOpTicket_U32, 3);
    EXPECT_EQ(puSocketThreadListen->GetSocketOperationResult(DEFAULT_INTER_PROCESS_TIMEOUT, ListenOperationResult_X), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ListenOperationResult_X.Operation_E, BOF::BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT);
    EXPECT_EQ(ListenOperationResult_X.OpTicket_U32, ListenOpTicket_U32);
    EXPECT_EQ(ListenOperationResult_X.Sts_E, BOF_ERR_CANCEL);
    EXPECT_EQ(ListenOperationResult_X.pSocket_O, nullptr);
    EXPECT_EQ(ListenOperationResult_X.Size_U32, 0);
    EXPECT_EQ(ListenOperationResult_X.pBuffer_U8, nullptr);
    //EXPECT_NE(ClientOperationResult_X.Time_U32, 0);
    BOF::Bof_MsSleep(1);   //Give some time to process the request
    EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
    EXPECT_EQ(BOF::BofThread::S_BofThreadBalance(), 0);

    Delta_U32 = BOF::Bof_ElapsedMsTime(Timer_U32);
    if (Delta_U32 > Max_U32)
    {
      Max_U32 = Delta_U32;
    }
    EXPECT_LT(Delta_U32, 250);
    if (Delta_U32 > 250)
    {
      printf("%d: Iter %d Delta %d Max is %d ms\n", BOF::Bof_GetMsTickCount(), i_U32, Delta_U32, Max_U32);
    }
  }


  printf("%d: Max is %d ms DeltaT %d ms\n", BOF::Bof_GetMsTickCount(), Max_U32, BOF::Bof_ElapsedMsTime(Start_U32));
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
}