/*
 * Copyright (Sleep100) 2013-2023, OnBings All rights reserved.
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

static uint32_t S_pCpt_U32[3];
void Sleep10()
{
  Bof_MsSleep(10);
  S_pCpt_U32[0]++;
//  printf("%u Sleep10 %d\n",  Bof_GetMsTickCount(), S_pCpt_U32[0]);
}
void Sleep50()
{
  Bof_MsSleep(50);
  S_pCpt_U32[1]++;
//  printf("%u Sleep50 %d\n",  Bof_GetMsTickCount(), S_pCpt_U32[1]);
}
void Sleep100()
{
  printf("%u Enter Sleep100 %d\n",  Bof_GetMsTickCount(), S_pCpt_U32[2]);
  Bof_MsSleep(100);
  S_pCpt_U32[2]++;
  printf("%u Leave Sleep100 %d\n",  Bof_GetMsTickCount(), S_pCpt_U32[2]);
}
TEST(Async_Test, CommandQueue)
{
  BOF_COMMAND_QUEUE_PARAM CommandQueueParam_X;
  BOF_COMMAND_QUEUE_ENTRY pCommand_X[3];
  uint32_t i_U32, Start_U32, DeltaInMs_U32;

  CommandQueueParam_X.ThreadCpuCoreAffinityMask_U64=0;
  CommandQueueParam_X.MultiThreadAware_B=true;
  CommandQueueParam_X.PriorityInversionAware_B=true;
  CommandQueueParam_X.PollTimeoutInMs_U32 = 200;
  CommandQueueParam_X.MaxPendingRequest_U32 = 4 * 3;
  CommandQueueParam_X.ThreadCpuCoreAffinityMask_U64 = 0;
  CommandQueueParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  CommandQueueParam_X.ThreadPriority_E = BOF_THREAD_PRIORITY_000;
  BofCommandQueue CmdQ(CommandQueueParam_X);

  memset(S_pCpt_U32, 0, sizeof(S_pCpt_U32));
  pCommand_X[0].Name_S = "CmdA";
  pCommand_X[0].Cmd = Sleep10;
  pCommand_X[1].Name_S = "CmdB";
  pCommand_X[1].Cmd = Sleep50;
  pCommand_X[2].Name_S = "CmdC";
  pCommand_X[2].Cmd = Sleep100;
  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < 4; i_U32++)
  {
    EXPECT_EQ(CmdQ.PostCommand(false, pCommand_X[2]), BOF_ERR_NO_ERROR);
    EXPECT_EQ(CmdQ.PostCommand(false, pCommand_X[1]), BOF_ERR_NO_ERROR);
    EXPECT_EQ(CmdQ.PostCommand(false, pCommand_X[0]), BOF_ERR_NO_ERROR);
  }
  // full
  // EXPECT_NE(CmdQ.PostCommand(false, pCommand_X[0]), BOF_ERR_NO_ERROR);

  DeltaInMs_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_LT(DeltaInMs_U32, 10);
  while (CmdQ.IsProcessingCommand())
  {
    Bof_MsSleep(10);
  }
  DeltaInMs_U32 = Bof_ElapsedMsTime(Start_U32);
  printf("DeltaCmd=%d ms\n", DeltaInMs_U32);
  // EXPECT_GT(DeltaInMs_U32, 1610);
  // EXPECT_LT(DeltaInMs_U32, 1650);
  EXPECT_EQ(S_pCpt_U32[0], 4);
  EXPECT_EQ(S_pCpt_U32[1], 4);
  EXPECT_EQ(S_pCpt_U32[2], 4);
}

TEST(Async_Test, OverloadCommandQueue)
{
  BOFERR Sts_E;
  BOF_COMMAND_QUEUE_PARAM CommandQueueParam_X;
  BOF_COMMAND_QUEUE_ENTRY Command_X;
  uint32_t i_U32, Start_U32, DeltaInMs_U32;

  CommandQueueParam_X.ThreadCpuCoreAffinityMask_U64=0;
  CommandQueueParam_X.MultiThreadAware_B=true;
  CommandQueueParam_X.PriorityInversionAware_B=true;
  CommandQueueParam_X.PollTimeoutInMs_U32 = 200;
  CommandQueueParam_X.MaxPendingRequest_U32 = 3;
  CommandQueueParam_X.ThreadCpuCoreAffinityMask_U64 = 0;
  CommandQueueParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  CommandQueueParam_X.ThreadPriority_E = BOF_THREAD_PRIORITY_000;
  BofCommandQueue CmdQ(CommandQueueParam_X);

  memset(S_pCpt_U32, 0, sizeof(S_pCpt_U32));
  Command_X.Name_S = "Cmd";
  Command_X.Cmd = Sleep100;
  Start_U32 = Bof_GetMsTickCount();
  printf("%u Start of post Cpt=%d\n",  Bof_GetMsTickCount(), S_pCpt_U32[2]);
  for (i_U32 = 0; i_U32 < 10; i_U32++)
  {
/* 
i_U32=0 Push->                    exec->Cpt=1
i_U32=1 Push->wait in queue->n=1->exec->Cpt=2
i_U32=2 Push->wait in queue->n=2->exec->Cpt=3
i_U32=3 Push->wait in queue->n=3->exec->Cpt=4
i_U32=4 Push->FULL
*/
    Sts_E = CmdQ.PostCommand(false, Command_X);
    printf("%u Post%d err %d nb %d Cpt=%d\n",  Bof_GetMsTickCount(), i_U32, Sts_E, CmdQ.NumberOfCommandWaitingInQueue(), S_pCpt_U32[2]);
  }
  // should be full
  printf("%u End of post Cpt=%d\n",  Bof_GetMsTickCount(), S_pCpt_U32[2]);
  EXPECT_NE(CmdQ.PostCommand(false, Command_X), BOF_ERR_NO_ERROR);
  printf("%u Wait for end of processing Cpt=%d\n",  Bof_GetMsTickCount(), S_pCpt_U32[2]);
  while (CmdQ.IsProcessingCommand())
  {
	//printf("nbc %d\n", CmdQ.NumberOfCommandWaitingInQueue());
    Bof_MsSleep(10);
  }
  printf("%u End of processing Cpt=%d\n",  Bof_GetMsTickCount(), S_pCpt_U32[2]);
  EXPECT_EQ(S_pCpt_U32[2], 4); 
  
  printf("%u Post another one Cpt=%d\n",  Bof_GetMsTickCount(), S_pCpt_U32[2]);
  EXPECT_EQ(CmdQ.PostCommand(false, Command_X), BOF_ERR_NO_ERROR);
  while (CmdQ.IsProcessingCommand())
  {
    Bof_MsSleep(10);
  }
  printf("%u End of processing Cpt=%d\n",  Bof_GetMsTickCount(), S_pCpt_U32[2]);
  EXPECT_EQ(S_pCpt_U32[2], 5);  
}
