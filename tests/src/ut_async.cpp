/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofasync class
 *
 * Name:        ut_enum.cpp
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
#include <bofstd/bofasync.h>

/*** Class *************************************************************************************************************************/

USE_BOF_NAMESPACE()

static uint32_t S_pCpt_U32[33];
void a()
{
	printf("A\n");
	Bof_MsSleep(10);
	S_pCpt_U32[0]++;
}
void b()
{
	printf("B\n");
	Bof_MsSleep(50);
	S_pCpt_U32[1]++;
}
void c()
{
	printf("C\n");
	Bof_MsSleep(100);
	S_pCpt_U32[2]++;
}
TEST(Async_Test, CommandQueue)
{
	BOF_COMMAND_QUEUE_PARAM CommandQueueParam_X;
	BOF_COMMAND_QUEUE_ENTRY pCommand_X[3];
	uint32_t i_U32, Start_U32,DeltaInMs_U32;

	CommandQueueParam_X.PollTimeoutInMs_U32=200;
	CommandQueueParam_X.MaxPendingRequest_U32=10*3;
	CommandQueueParam_X.ThreadCpuCoreAffinityMask_U64=0;
	CommandQueueParam_X.ThreadSchedulerPolicy_E=BOF_THREAD_SCHEDULER_POLICY_OTHER;
	CommandQueueParam_X.ThreadPriority_E=BOF_THREAD_DEFAULT_PRIORITY;
	BofCommandQueue CmdQ(CommandQueueParam_X);

	memset(S_pCpt_U32,0,sizeof(S_pCpt_U32));
	pCommand_X[0].Name_S="CmdA";
	pCommand_X[0].Cmd=a;
	pCommand_X[1].Name_S="CmdB";
	pCommand_X[1].Cmd=b;
	pCommand_X[2].Name_S="CmdC";
	pCommand_X[2].Cmd=c;
	Start_U32=Bof_GetMsTickCount();
	for (i_U32=0;i_U32<10;i_U32++)
	{
		EXPECT_EQ(CmdQ.PostCommand(false,pCommand_X[2]), BOF_ERR_NO_ERROR);
		EXPECT_EQ(CmdQ.PostCommand(false,pCommand_X[1]), BOF_ERR_NO_ERROR);
		EXPECT_EQ(CmdQ.PostCommand(false,pCommand_X[0]), BOF_ERR_NO_ERROR);
	}
//One more during pCommand_X[2] exec
	EXPECT_EQ(CmdQ.PostCommand(false,pCommand_X[0]), BOF_ERR_NO_ERROR);
//full
	EXPECT_NE(CmdQ.PostCommand(false,pCommand_X[0]), BOF_ERR_NO_ERROR);

	DeltaInMs_U32=Bof_ElapsedMsTime(Start_U32);
	EXPECT_LT(DeltaInMs_U32, 10);
	while(CmdQ.IsProcessingCommand())
	{
		Bof_MsSleep(10);
	}
	DeltaInMs_U32=Bof_ElapsedMsTime(Start_U32);
	EXPECT_GT(DeltaInMs_U32, 1610);
	EXPECT_LT(DeltaInMs_U32, 1650);

	EXPECT_EQ(CmdQ.PostCommand(true,pCommand_X[2]), BOF_ERR_NO_ERROR);
	EXPECT_NE(CmdQ.PostCommand(true,pCommand_X[2]), BOF_ERR_NO_ERROR);
	EXPECT_EQ(CmdQ.PostCommand(false,pCommand_X[2]), BOF_ERR_NO_ERROR);
	pCommand_X[2].Name_S="";
	EXPECT_NE(CmdQ.PostCommand(false,pCommand_X[2]), BOF_ERR_NO_ERROR);
	while(CmdQ.IsProcessingCommand())
	{
		Bof_MsSleep(10);
	}
}
