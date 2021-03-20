/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofactor class
 *
 * Name:        ut_actor.cpp
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
#include <bofstd/bofstd.h>
#include <bofstd/bofactor.h>
#include "bofstd/bofsocket.h"
#include "bofstd/bofstringformatter.h"

/*** Class *************************************************************************************************************************/

USE_BOF_NAMESPACE()

std::string TheActor(const std::string &_rMessage_S, void *_pContext)
{
	uint32_t Context_U32 = *reinterpret_cast<uint32_t *>(_pContext);

	EXPECT_EQ(Context_U32, 0x12345678);
	Bof_Sleep(100);
	return _rMessage_S+"_"+std::to_string(Context_U32);
}

TEST(Actor_Test, Actor)
{
	uint32_t Context_U32 = 0x12345678, i_U32, CmdTicket_U32, ReplyTicket_U32,ExpectedTicket_U32;
	std::string Cmd_S,Reply_S,Expected_S;
	BOFERR Sts_E;
	bool CommandRunning_B, ReplyPending_B;

	BofActor MyActor("MyActor", TheActor, &Context_U32);
	ExpectedTicket_U32 = 1;
	for (i_U32 = 0; i_U32 < 10; i_U32++)
	{
		Cmd_S=Bof_Sprintf( "Cmd_%d", i_U32);
		Expected_S= Bof_Sprintf( "%s_%d", Cmd_S.c_str(), Context_U32);

		CommandRunning_B = MyActor.IsCommandRunning();
		EXPECT_FALSE(CommandRunning_B);

		ReplyPending_B = MyActor.IsReplyPending();
		EXPECT_FALSE(ReplyPending_B);

		Sts_E = MyActor.SendCommandToActor(Cmd_S, CmdTicket_U32);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
		EXPECT_EQ(CmdTicket_U32, ExpectedTicket_U32);

		CommandRunning_B = MyActor.IsCommandRunning();
//Depends on timing		EXPECT_TRUE(CommandRunning_B);

		ReplyPending_B = MyActor.IsReplyPending();
		//Depends on timing		EXPECT_FALSE(ReplyPending_B);

		Bof_Sleep(200);

		CommandRunning_B = MyActor.IsCommandRunning();
		EXPECT_FALSE(CommandRunning_B);

		ReplyPending_B = MyActor.IsReplyPending();
		EXPECT_TRUE(ReplyPending_B);

		Sts_E = MyActor.WaitForReplyFromActor(Reply_S, ReplyTicket_U32);
		EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
		EXPECT_STREQ(Reply_S.c_str(), Expected_S.c_str());
		EXPECT_EQ(ReplyTicket_U32, ExpectedTicket_U32);
		EXPECT_EQ(ReplyTicket_U32, CmdTicket_U32);
		ExpectedTicket_U32++;

		CommandRunning_B = MyActor.IsCommandRunning();
		EXPECT_FALSE(CommandRunning_B);

		ReplyPending_B = MyActor.IsReplyPending();
		EXPECT_FALSE(ReplyPending_B);
	}
}
