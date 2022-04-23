/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the comchannel class
 *
 * Name:        ut_socket.cpp
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
#include <bofstd/bofsocket.h>
#include "bofstd/bofstringformatter.h"

/*** Class *************************************************************************************************************************/

USE_BOF_NAMESPACE()

#if 0
int main(int argc, char *argv[]) {
	thread sender([] {
									boost::asio::io_service ioService;
									tcp::acceptor acceptor(ioService, tcp::endpoint(tcp::v4(), 8087) );
									tcp::iostream stream;
									acceptor.accept(*stream.rdbuf() );
									for (int i = 0 ; i < 10 ; i++)
									{
										time_t now = time(0);
										stream << "test " << ctime(&now);
										this_thread::sleep_for(chrono::milliseconds(1000) );
									}
								});

	thread receiver([] {
										tcp::iostream stream(tcp::endpoint(address::from_string("127.0.0.1"), 8087) );
										for (int i = 0 ; i < 10 ; i++)
										{
											string line;
											getline(stream, line);
											cout << line << endl;
										}
									});

	sender.join();
	receiver.join();
}

#endif

TEST(Socket_Test,ListenNonBlockingSocket)
{
	BofSocket BofSocketDef;
	BOFERR Sts_E;
	BOF_SOCKET_PARAM BofSocketParam_X;
	void *pClient;

	EXPECT_EQ(BofSocketDef.GetSocketHandle(), BOFSOCKET_INVALID);
	EXPECT_EQ(BofSocketDef.GetMaxUdpLen(), 0);
	EXPECT_FALSE(BofSocketDef.IsTcp());
	EXPECT_FALSE(BofSocketDef.IsUdp());
	EXPECT_EQ(BofSocketDef.InitErrorCode(), BOFERR_NO_ERROR);

	BofSocketParam_X.Reset();
	BofSocketParam_X.BaseChannelParam_X.ChannelName_S = "MyDefSocket";
	BofSocketParam_X.BaseChannelParam_X.Blocking_B=false;
	BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 3;                               // 0->Client
	BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32=0x10000;
	BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32=0x10000;

	BofSocketParam_X.Address_S = "udp://127.0.0.1:5555";
	BofSocketParam_X.BroadcastPort_U16 = 0;
	BofSocketParam_X.Broadcast_B = false;
	BofSocketParam_X.EnableLocalMulticast_B = false;
	BofSocketParam_X.KeepAlive_B = false;
	BofSocketParam_X.MulticastSender_B = false;
	BofSocketParam_X.NoDelay_B = true;
	BofSocketParam_X.ReUseAddress_B = false;
	BofSocketParam_X.Ttl_U32 = 32;
	Sts_E = BofSocketDef.InitializeSocket(BofSocketParam_X);
	EXPECT_EQ(Sts_E, BOFERR_NO_ERROR);
	EXPECT_EQ(BofSocketDef.GetSocketHandle(), 0);
	EXPECT_EQ(BofSocketDef.GetMaxUdpLen(), 0xFFFF);
	EXPECT_TRUE(BofSocketDef.IsTcp());
	EXPECT_FALSE(BofSocketDef.IsUdp());
	EXPECT_EQ(BofSocketDef.InitErrorCode(), BOFERR_NO_ERROR);

#if 0
	BOFERR               V_Connect(uint32_t TimeOut_U32, void *pParam, uint32_t _UdpDataSize_U32, uint8_t *_pUdpData_U8) override;
	intptr_t             V_Listen(uint32_t TimeOut_U32, uint32_t _Flag_U32, uint32_t *_pUdpDataSizeMax_U32, uint8_t *_pUdpData_U8) override;
	BOFERR               V_ReadData(uint32_t TimeOut_U32, uint32_t & _rNb_U32, uint8_t *pBuffer_U8) override;
	BOFERR               V_WriteData(uint32_t TimeOut_U32, uint32_t & _rNb_U32, const uint8_t *pBuffer_U8) override;
	BOFERR               V_GetStatus(BOF_COM_CHANNEL_STATUS & _rpStatus_X) override;
	BOFERR               V_FlushData(uint32_t TimeOut_U32) override;

	bool                 V_IsConnected() override;
#endif
}