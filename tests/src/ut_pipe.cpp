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

/*** Include files ***********************************************************/

#include "gtestrunner.h"
#include <bofstd/bofpipe.h>
#include <bofstd/bofsystem.h>

#include <atomic>

/*** Class *************************************************************************************************************************/

USE_BOF_NAMESPACE()

const uint32_t NB_PIPE_LOOP = 1000;
const uint32_t PIPE_TIMEOUT = 1000;

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


static void * ServerThread(const std::atomic< bool > & _rIsThreadLoopMustExit_B, void *_pContext)
{
	SERVER_CONTEXT *pContext_X = reinterpret_cast<SERVER_CONTEXT *> (_pContext);
	uint8_t pBuffer_U8[0x100];
	//char pId_c[64];
	uint32_t Nb_U32, Cpt_U32;
	BOFERR Sts_E;

	Cpt_U32 = 0;
	while (!_rIsThreadLoopMustExit_B)
	{
		printf("Server waits for data[%d]%s", Cpt_U32, Bof_Eol());
		Nb_U32 = sizeof(pBuffer_U8);
		memset(pBuffer_U8, 0, Nb_U32);
		Sts_E = pContext_X->pBofPipeServer->V_ReadData(PIPE_TIMEOUT, Nb_U32, pBuffer_U8);
		printf("Server got '%d:%s' data sts %d%s", Nb_U32, pBuffer_U8, Sts_E, Bof_Eol());
		if (Sts_E == BOF_ERR_NO_ERROR)
		{
			EXPECT_GT(Nb_U32, static_cast<uint32_t>(0));
      /*
			sprintf(pId_c, "_%d", Cpt_U32++);
			strcat(reinterpret_cast<char *>(pBuffer_U8), pId_c);
			Nb_U32 = strlen(reinterpret_cast<char *>(pBuffer_U8));
			Sts_E = pContext_X->pBofPipeServer->V_WriteData(PIPE_TIMEOUT, Nb_U32, pBuffer_U8);
			printf("Server send '%d:%s' data sts %d%s", Nb_U32, pBuffer_U8, Sts_E, Bof_Eol());
      */
			EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
			EXPECT_GT(Nb_U32, static_cast<uint32_t>(0));
		}
	}
	printf("Leave server thread\r\n");
	return nullptr;
}

//#include <czmq.h>
TEST(Pipe_Test, UdpPipe)
{
	BOF_PIPE_PARAM PipeParam_X;
	BofPipe *pBofPipeServer, *pBofPipeClient;
	uint8_t pBuffer_U8[0x100];
	uint32_t Nb_U32, Cpt_U32, Size_U32,i_U32, Start_U32,Delta_U32;
	std::string Reply_S;
	BOFERR Sts_E;
	SERVER_CONTEXT ServerContext_X;
	BOF_THREAD ServerThread_X;
//	char pId_c[64];


	PipeParam_X.PipeType_E = BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP;
	PipeParam_X.BaseChannelParam_X.ListenBackLog_U32 = 1;
	PipeParam_X.BaseChannelParam_X.ChannelName_S = "PipeServer";
	PipeParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 256;
	PipeParam_X.BaseChannelParam_X.SndBufferSize_U32 = 256;

  PipeParam_X.PortInBase_U16 = 5560;
	pBofPipeServer = new BofPipe(PipeParam_X);
	EXPECT_TRUE(pBofPipeServer != nullptr);
	EXPECT_EQ(pBofPipeServer->LastErrorCode(), BOF_ERR_NO_ERROR);

	PipeParam_X.PipeType_E = BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP;
	PipeParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
	PipeParam_X.BaseChannelParam_X.ChannelName_S = "PipeClient";
	PipeParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 256;
	PipeParam_X.BaseChannelParam_X.SndBufferSize_U32 = 256;

  PipeParam_X.PortInBase_U16 = 5560;
  pBofPipeClient = new BofPipe(PipeParam_X);
	EXPECT_TRUE(pBofPipeClient != nullptr);
	EXPECT_EQ(pBofPipeClient->LastErrorCode(), BOF_ERR_NO_ERROR);

	ServerContext_X.pBofPipeServer = pBofPipeServer;
	Sts_E = Bof_CreateThread("Server", ServerThread, &ServerContext_X, ServerThread_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	Sts_E = Bof_LaunchThread(ServerThread_X, 0, 0, BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF_THREAD_PRIORITY::BOF_THREAD_DEFAULT_PRIORITY, 1000);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	Sts_E = pBofPipeClient->V_Connect(0,"", "");
	EXPECT_EQ(Sts_E, BOF_ERR_NOT_SUPPORTED);

	Cpt_U32 = 0;
	Start_U32 = Bof_GetMsTickCount();
	for (i_U32 = 0; i_U32 < NB_PIPE_LOOP; i_U32++)
	{
		sprintf(reinterpret_cast<char *>(pBuffer_U8), "Hello_%d",i_U32);
		Nb_U32 = static_cast<uint32_t>(strlen(reinterpret_cast<char *>(pBuffer_U8)));
		Size_U32 = Nb_U32;
		Sts_E = pBofPipeClient->V_WriteData(PIPE_TIMEOUT, Nb_U32, pBuffer_U8);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
		EXPECT_EQ(Nb_U32, Size_U32);

/*
		sprintf(pId_c, "_%d", Cpt_U32++);
		Size_U32 = Nb_U32 + strlen(pId_c);

		Nb_U32 = sizeof(pBuffer_U8);
		memset(pBuffer_U8, 0, Nb_U32);
		Sts_E = pBofPipeClient->V_ReadData(PIPE_TIMEOUT, Nb_U32, pBuffer_U8);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
		EXPECT_EQ(Nb_U32, Size_U32);
    */
	}
	Delta_U32 = Bof_ElapsedMsTime(Start_U32);
	printf("%d loop in %d ms%s", i_U32, Delta_U32, Bof_Eol());

	Sts_E = Bof_DestroyThread(ServerThread_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SAFE_DELETE(pBofPipeServer);
	BOF_SAFE_DELETE(pBofPipeClient);
}
