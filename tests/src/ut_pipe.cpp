/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofpipe class
 *
 * Name:        ut_pipe.cpp
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
#include <bofstd/bofpipe.h>
#include <bofstd/bofsystem.h>

#include "gtestrunner.h"

#include <atomic>

USE_BOF_NAMESPACE()

const uint32_t NB_PIPE_LOOP = 10; // 250; if more than 10 we have a problem with docker socket (???)
const uint32_t PIPE_TIMEOUT = 1000;
constexpr const char *FIFO_MSG_PATTERN = "Hello_%06d\n";

struct SERVER_CONTEXT
{
  BofPipe *pBofPipeServer;

  SERVER_CONTEXT()
  {
    Reset();
  }
  void Reset()
  {
    pBofPipeServer = nullptr;
  }
};

static void *ServerThread(const std::atomic<bool> &_rIsThreadLoopMustExit_B, void *_pContext)
{
  SERVER_CONTEXT *pContext_X = reinterpret_cast<SERVER_CONTEXT *>(_pContext);
  uint8_t pBuffer_U8[0x100];
  BOF_COM_CHANNEL_STATUS Status_X;
  char *p_c;
  uint32_t Nb_U32, Cpt_U32;
  BOFERR Sts_E;
  uint32_t i_U32, Start_U32;
  int Val_i, Expected_i;

  Sts_E = pContext_X->pBofPipeServer->V_Connect(PIPE_TIMEOUT, "", "");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Expected_i = 0;
  Start_U32 = BOF::Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NB_PIPE_LOOP; i_U32++)
  {
    // printf("Server waits for data[%d]%s", Cpt_U32, Bof_Eol());
    Sts_E = pContext_X->pBofPipeServer->V_GetStatus(Status_X);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    if (Status_X.NbIn_U32)
    {
      //printf("Server nB %d\n", Status_X.NbIn_U32);
    }
    Nb_U32 = sizeof(pBuffer_U8);
    memset(pBuffer_U8, 0, Nb_U32);
    Sts_E = pContext_X->pBofPipeServer->V_ReadData(PIPE_TIMEOUT, Nb_U32, pBuffer_U8);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      p_c = strchr((char *)pBuffer_U8, '\n');
      EXPECT_TRUE(p_c != nullptr);
      if (p_c)
      {
        *p_c = 0;
      }
      Sts_E = pContext_X->pBofPipeServer->V_GetStatus(Status_X);
      // printf("Server read nb %d data %s nbi %d\n", Nb_U32, pBuffer_U8, Status_X.NbIn_U32);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      // printf("Server got '%d:%s' data %s sts %d remain %d\n", Nb_U32, pBuffer_U8, pBuffer_U8, Sts_E, Status_X.NbIn_U32);

      EXPECT_GT(Nb_U32, static_cast<uint32_t>(0));
      /*
      sprintf(pId_c, "_%d", Cpt_U32++);
      strcat(reinterpret_cast<char *>(pBuffer_U8), pId_c);
      Nb_U32 = strlen(reinterpret_cast<char *>(pBuffer_U8));
      Sts_E = pContext_X->pBofPipeServer->V_WriteData(PIPE_TIMEOUT, Nb_U32, pBuffer_U8);
      //printf("Server send '%d:%s' data sts %d%s", Nb_U32, pBuffer_U8, Sts_E, Bof_Eol());
      */
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      EXPECT_GT(Nb_U32, static_cast<uint32_t>(0));
      p_c = strchr((char *)pBuffer_U8, '_');
      EXPECT_TRUE(p_c != nullptr);
      Val_i = p_c ? atoi(p_c + 1) : 0;
      EXPECT_EQ(Expected_i, Val_i);
      Expected_i++;
      if (Expected_i == 12)
      {
        // printf("jj");
        // break;
      }
    }
  }
  printf("Leave ServerThread\n");
  return nullptr;
}

static void *BinaryServerThread(const std::atomic<bool> &_rIsThreadLoopMustExit_B, void *_pContext)
{
  SERVER_CONTEXT *pContext_X = reinterpret_cast<SERVER_CONTEXT *>(_pContext);
  uint8_t pBuffer_U8[0x100];
  BOF_COM_CHANNEL_STATUS Status_X;
  uint32_t i_U32, Nb_U32, Size_U32;
  BOFERR Sts_E;

  Sts_E = pContext_X->pBofPipeServer->V_Connect(PIPE_TIMEOUT, "", "");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  for (i_U32 = 0; i_U32 < NB_PIPE_LOOP; i_U32++)
  {
    Nb_U32 = 1;
    pBuffer_U8[0] = (uint8_t)i_U32;
    Size_U32 = Nb_U32;
    Sts_E = pContext_X->pBofPipeServer->V_WriteData(PIPE_TIMEOUT, Nb_U32, pBuffer_U8);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(Nb_U32, Size_U32);
    Sts_E = pContext_X->pBofPipeServer->V_GetStatus(Status_X);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    // printf("BinServeur nb %d val %d\n", Status_X.NbIn_U32, pBuffer_U8[0]);
  }
  // printf("Leave BinaryServerThread\n");
  return nullptr;
}

TEST(Pipe_Test, UdpPipeSingle)
{
  BOF_PIPE_PARAM PipeParam_X;
  BofPipe *pBofPipeServer, *pBofPipeClient;
  uint8_t pBuffer_U8[0x100];
  uint32_t Nb_U32, Cpt_U32, Size_U32, i_U32, Start_U32, Delta_U32;
  std::string Reply_S;
  BOFERR Sts_E;
  SERVER_CONTEXT ServerContext_X;
  BOF_THREAD ServerThread_X;
  BOF_COM_CHANNEL_STATUS Status_X;
  std::atomic<bool> ThreadLoopMustExit_B = false;

  PipeParam_X.PipeType_E = BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP;
  PipeParam_X.PipeAccess_E = BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ;
  PipeParam_X.PipeServer_B = true;
  PipeParam_X.BaseChannelParam_X.Blocking_B = false;
  PipeParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
  PipeParam_X.BaseChannelParam_X.ChannelName_S = "PipeServer";
  PipeParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 4096;
  PipeParam_X.BaseChannelParam_X.SndBufferSize_U32 = 4096;

  PipeParam_X.SrcPortBase_U16 = 55000;
  PipeParam_X.DstPortBase_U16 = 55010;
  pBofPipeServer = new BofPipe(PipeParam_X);
  EXPECT_TRUE(pBofPipeServer != nullptr);
  EXPECT_EQ(pBofPipeServer->LastErrorCode(), BOF_ERR_NO_ERROR);

  PipeParam_X.PipeType_E = BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP;
  PipeParam_X.PipeAccess_E = BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_WRITE;
  PipeParam_X.PipeServer_B = false;
  PipeParam_X.BaseChannelParam_X.Blocking_B = true;
  PipeParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
  PipeParam_X.BaseChannelParam_X.ChannelName_S = "PipeClient";
  PipeParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x2000;
  PipeParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x2000;

  PipeParam_X.SrcPortBase_U16 = 55010;
  PipeParam_X.DstPortBase_U16 = 55000;
  pBofPipeClient = new BofPipe(PipeParam_X);
  EXPECT_TRUE(pBofPipeClient != nullptr);
  EXPECT_EQ(pBofPipeClient->LastErrorCode(), BOF_ERR_NO_ERROR);

  ServerContext_X.pBofPipeServer = pBofPipeServer;
  /*
  BOF::BofSocket *pSrv = pBofPipeServer->GetUdpPipeMst();
  BOF::BofSocket *pClt = pBofPipeClient->GetUdpPipeMst();

  Nb_U32 = 3;
  Sts_E = pClt->V_WriteData(PIPE_TIMEOUT, Nb_U32, pBuffer_U8);
  Sts_E = pSrv->V_GetStatus(Status_X);
  Nb_U32 = 5;
  Sts_E = pClt->V_WriteData(PIPE_TIMEOUT, Nb_U32, pBuffer_U8);
  Sts_E = pSrv->V_GetStatus(Status_X);
  Sts_E = pSrv->V_ReadData(PIPE_TIMEOUT, Nb_U32, pBuffer_U8);
  Sts_E = pSrv->V_GetStatus(Status_X);
*/
  Sts_E = Bof_CreateThread("Server", ServerThread, &ServerContext_X, ServerThread_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_LaunchThread(ServerThread_X, 0, 0, BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000, 1000);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = pBofPipeClient->V_Connect(PIPE_TIMEOUT, "", "");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Cpt_U32 = 0;
  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NB_PIPE_LOOP; i_U32++)
  {
    sprintf(reinterpret_cast<char *>(pBuffer_U8), FIFO_MSG_PATTERN, i_U32);
    Nb_U32 = static_cast<uint32_t>(strlen(reinterpret_cast<char *>(pBuffer_U8)));
    Size_U32 = Nb_U32;
    Sts_E = pBofPipeClient->V_WriteData(PIPE_TIMEOUT, Nb_U32, pBuffer_U8);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(Nb_U32, Size_U32);
    /*
        Sts_E = pBofPipeClient->V_GetStatus(Status_X);
        EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
        printf("Aft Clt Pnd %d Sts %d\n", Status_X.NbIn_U32, Sts_E);

        Sts_E = pBofPipeServer->V_GetStatus(Status_X);
        EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
        printf("Aft Srv Pnd %d Sts %d\n", Status_X.NbIn_U32, Sts_E);
        */
  }
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  // ServerThread(ThreadLoopMustExit_B, &ServerContext_X);
  BOF::Bof_MsSleep(1000); // Wait until server thread has consumed the data sent by the client
  Sts_E = Bof_DestroyThread(ServerThread_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  BOF_SAFE_DELETE(pBofPipeServer);
  BOF_SAFE_DELETE(pBofPipeClient);
}

TEST(Pipe_Test, NativePipeSingleString)
{
  BOF_PIPE_PARAM PipeParam_X;
  BofPipe *pBofPipeServer, *pBofPipeClient;
  uint8_t pBuffer_U8[0x100];
  uint32_t Nb_U32, Cpt_U32, Size_U32, i_U32, Start_U32, Delta_U32, Total_U32;
  std::string Reply_S;
  BOFERR Sts_E;
  SERVER_CONTEXT ServerContext_X;
  BOF_THREAD ServerThread_X;
  BOF_COM_CHANNEL_STATUS StatusIn_X, StatusOut_X;

  PipeParam_X.PipeType_E = BOF_PIPE_TYPE::BOF_PIPE_NATIVE;
  PipeParam_X.PipeAccess_E = BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ;
  PipeParam_X.PipeServer_B = true;
  PipeParam_X.NativeStringMode_B = true;
  PipeParam_X.BaseChannelParam_X.Blocking_B = false;
  PipeParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
  PipeParam_X.BaseChannelParam_X.ChannelName_S = "/tmp/fifo";
  PipeParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 4096;
  PipeParam_X.BaseChannelParam_X.SndBufferSize_U32 = 4096;
  PipeParam_X.SrcPortBase_U16 = 0;
  PipeParam_X.DstPortBase_U16 = 0;
  pBofPipeServer = new BofPipe(PipeParam_X);
  EXPECT_TRUE(pBofPipeServer != nullptr);
  EXPECT_EQ(pBofPipeServer->LastErrorCode(), BOF_ERR_NO_ERROR);
  PipeParam_X.PipeType_E = BOF_PIPE_TYPE::BOF_PIPE_NATIVE;
  PipeParam_X.PipeAccess_E = BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_WRITE;
  PipeParam_X.PipeServer_B = false;
  PipeParam_X.NativeStringMode_B = true;
  PipeParam_X.BaseChannelParam_X.Blocking_B = false;
  PipeParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
  PipeParam_X.BaseChannelParam_X.ChannelName_S = "/tmp/fifo";
  PipeParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 4096;
  PipeParam_X.BaseChannelParam_X.SndBufferSize_U32 = 4096;
  PipeParam_X.SrcPortBase_U16 = 0;
  PipeParam_X.DstPortBase_U16 = 0;
  pBofPipeClient = new BofPipe(PipeParam_X);
  EXPECT_TRUE(pBofPipeClient != nullptr);
  EXPECT_EQ(pBofPipeClient->LastErrorCode(), BOF_ERR_NO_ERROR);
  ServerContext_X.pBofPipeServer = pBofPipeServer;

  Sts_E = Bof_CreateThread("Server", ServerThread, &ServerContext_X, ServerThread_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_LaunchThread(ServerThread_X, 0, 0, BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000, 1000);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = pBofPipeClient->V_Connect(PIPE_TIMEOUT, "", "");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Cpt_U32 = 0;
  Start_U32 = Bof_GetMsTickCount();

  Total_U32 = 0;
  for (i_U32 = 0; i_U32 < NB_PIPE_LOOP; i_U32++)
  {
    Nb_U32 = sprintf(reinterpret_cast<char *>(pBuffer_U8), FIFO_MSG_PATTERN, i_U32);
    Size_U32 = Nb_U32;
    Total_U32 += Nb_U32;

    Sts_E = pBofPipeClient->V_WriteData(PIPE_TIMEOUT, Nb_U32, pBuffer_U8);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(Nb_U32, Size_U32);
    Sts_E = pBofPipeClient->V_GetStatus(StatusIn_X);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    // Sts_E = pBofPipeServer->V_GetStatus(StatusOut_X);
    // EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

    // EXPECT_EQ(StatusIn_X.NbIn_U32, StatusOut_X.NbIn_U32);
    // EXPECT_EQ(StatusIn_X.NbIn_U32, Total_U32);
    // printf("CLIENT SEND Total %d\n", Total_U32);
  }
  // std::atomic<bool> MustExit_B=false;
  // ServerThread(MustExit_B, &ServerContext_X);

  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  // printf("%d loop in %d ms%s", i_U32, Delta_U32, Bof_Eol());

  Sts_E = Bof_DestroyThread(ServerThread_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  BOF_SAFE_DELETE(pBofPipeServer);
  BOF_SAFE_DELETE(pBofPipeClient);
}

TEST(Pipe_Test, NativePipeSingleBinary)
{
  BOF_PIPE_PARAM PipeParam_X;
  BofPipe *pBofPipeServer, *pBofPipeClient;
  uint8_t pBuffer_U8[0x100];
  uint32_t Nb_U32, Cpt_U32, Size_U32, i_U32, Start_U32, Delta_U32;
  std::string Reply_S;
  BOFERR Sts_E;
  SERVER_CONTEXT ServerContext_X;
  BOF_THREAD ServerThread_X;
  BOF_COM_CHANNEL_STATUS Status_X;

  PipeParam_X.PipeType_E = BOF_PIPE_TYPE::BOF_PIPE_NATIVE;
  PipeParam_X.PipeAccess_E = BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ;
  PipeParam_X.PipeServer_B = false;
  PipeParam_X.NativeStringMode_B = false;
  PipeParam_X.BaseChannelParam_X.Blocking_B = false;
  PipeParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
  PipeParam_X.BaseChannelParam_X.ChannelName_S = "/tmp/fifo";
  PipeParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 4096;
  PipeParam_X.BaseChannelParam_X.SndBufferSize_U32 = 4096;
  PipeParam_X.SrcPortBase_U16 = 0;
  PipeParam_X.DstPortBase_U16 = 0;
  pBofPipeClient = new BofPipe(PipeParam_X);
  EXPECT_TRUE(pBofPipeClient != nullptr);
  EXPECT_EQ(pBofPipeClient->LastErrorCode(), BOF_ERR_NO_ERROR);

  PipeParam_X.PipeType_E = BOF_PIPE_TYPE::BOF_PIPE_NATIVE;
  PipeParam_X.PipeAccess_E = BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_WRITE;
  PipeParam_X.PipeServer_B = true;
  PipeParam_X.NativeStringMode_B = false;
  PipeParam_X.BaseChannelParam_X.Blocking_B = false;
  PipeParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
  PipeParam_X.BaseChannelParam_X.ChannelName_S = "/tmp/fifo";
  PipeParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 4096;
  PipeParam_X.BaseChannelParam_X.SndBufferSize_U32 = 4096;
  PipeParam_X.SrcPortBase_U16 = 0;
  PipeParam_X.DstPortBase_U16 = 0;
  pBofPipeServer = new BofPipe(PipeParam_X);
  EXPECT_TRUE(pBofPipeServer != nullptr);
  EXPECT_EQ(pBofPipeServer->LastErrorCode(), BOF_ERR_NO_ERROR);
  ServerContext_X.pBofPipeServer = pBofPipeServer;

  Sts_E = Bof_CreateThread("Server", BinaryServerThread, &ServerContext_X, ServerThread_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_LaunchThread(ServerThread_X, 0, 0, BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000, 1000);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = pBofPipeClient->V_Connect(PIPE_TIMEOUT, "", "");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Cpt_U32 = 0;
  Start_U32 = Bof_GetMsTickCount();

  for (i_U32 = 0; i_U32 < NB_PIPE_LOOP; i_U32++)
  {
    Sts_E = pBofPipeClient->V_GetStatus(Status_X);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    Nb_U32 = 1;
    memset(pBuffer_U8, 0, Nb_U32);
    Sts_E = pBofPipeClient->V_ReadData(PIPE_TIMEOUT, Nb_U32, pBuffer_U8);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(Nb_U32, 1);
    EXPECT_EQ(pBuffer_U8[0], (uint8_t)i_U32);
    // printf("Client rcv %d nb %d\n", pBuffer_U8[0], Status_X.NbIn_U32);
  }
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  // printf("%d loop in %d ms\n", i_U32, Delta_U32);

  Sts_E = Bof_DestroyThread(ServerThread_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  BOF_SAFE_DELETE(pBofPipeServer);
  BOF_SAFE_DELETE(pBofPipeClient);
}