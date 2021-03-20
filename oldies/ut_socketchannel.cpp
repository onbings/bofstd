/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the BofIoChannel class
 *
 * Name:        ut_socketchannel.cpp
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
#include <bofstd/bofsocketchannel.h>

class BofSocketChannelTest : public BOF_NAMESPACE::BofSocketChannel
{
private:
		std::string mLastDataRead_S = "";

public:
		BofSocketChannelTest(const BOF_NAMESPACE::BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X) : BofSocketChannel(nullptr, _rBofSocketChannelParam_X)
		{}

		virtual BOFERR V_DataRead(uint32_t _Nb_U32, uint8_t *_pBuffer_U8, bool _Partial_B, const BOF_NAMESPACE::BOF_SOCKET_PEER_INFO &_rPeerInfo_X) override
		{
			mLastDataRead_S = std::string(_pBuffer_U8, _pBuffer_U8 + _Nb_U32);
			return BOFERR_NO_ERROR;
		}

		std::string GetAndClearLastDataRead()
		{
			std::string Rts_S = mLastDataRead_S;
			mLastDataRead_S = "";
			return Rts_S;
		}
};

TEST(SocketChannel_Test, ConstructorDestuctor)
{
	BOF_NAMESPACE::BOF_SOCKET_CHANNEL_PARAM BofSocketChannelParam_X, CurrentBofSocketChannelParam_X;
	BOF_NAMESPACE::BOF_SOCKET_CHANNEL_STATISTIC BofSocketChannelStatistic_X;

	BofSocketChannelParam_X.ChannelId_U32 = 0;
	BofSocketChannelParam_X.Name_S = "SocketChannel_Test";
	BofSocketChannelParam_X.ListenBacklog_U32 = 0;;
	BofSocketChannelParam_X.IpAddress_S = "tcp://0.0.0.0:0;127.0.0.1:56000";
	BofSocketChannelParam_X.NotifyRcvBufferSize_U32 = 0x1000;
	BofSocketChannelParam_X.NotifyType_E = BOF_NAMESPACE::BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND;
	BofSocketChannelParam_X.Delimiter_U8 = '\n';
	BofSocketChannelParam_X.pData = nullptr;
	BofSocketChannelParam_X.RcvBufferSize_U32 = 0x10000;
	BofSocketChannelParam_X.SndBufferSize_U32 = 0x10000;
	BofSocketChannelParam_X.OnSocketAcceptCallback = nullptr;
	BofSocketChannelParam_X.OnSocketReadCallback = nullptr;
	BofSocketChannelTest SocketChannel(BofSocketChannelParam_X);
	EXPECT_EQ(SocketChannel.InitErrorCode(), BOFERR_NO_ERROR);

	CurrentBofSocketChannelParam_X = SocketChannel.BofSocketChannelParam();
	EXPECT_EQ(CurrentBofSocketChannelParam_X, BofSocketChannelParam_X);

	BofSocketChannelStatistic_X = SocketChannel.BofSocketChannelStatistic();
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnect_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnectError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnect_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnectError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpRead_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpReadError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataByteRead_U64, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWrite_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWriteError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataByteWritten_U64, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbWriteEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbCloseEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbConnectEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpEndEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpShutdownEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpListenEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTimerEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTimerEventError_U32, 0);
}

TEST(SocketChannel_Test, WriteOpWithTestLoop)
{
	std::string DataSent_S = "Hello World !\r\n";
	BOF_NAMESPACE::BOF_SOCKET_CHANNEL_PARAM BofSocketChannelParam_X, CurrentBofSocketChannelParam_X;
	BOF_NAMESPACE::BOF_SOCKET_CHANNEL_STATISTIC BofSocketChannelStatistic_X;

	BofSocketChannelParam_X.ChannelId_U32 = 0;
	BofSocketChannelParam_X.Name_S = "SocketChannel_Test";
	BofSocketChannelParam_X.ListenBacklog_U32 = 0;;
	BofSocketChannelParam_X.IpAddress_S = "tcp://0.0.0.0:0;127.0.0.1:56000";
	BofSocketChannelParam_X.NotifyRcvBufferSize_U32 = 0x1000;
	BofSocketChannelParam_X.NotifyType_E = BOF_NAMESPACE::BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND;
	BofSocketChannelParam_X.Delimiter_U8 = '\n';
	BofSocketChannelParam_X.pData = nullptr;
	BofSocketChannelParam_X.RcvBufferSize_U32 = 0x10000;
	BofSocketChannelParam_X.SndBufferSize_U32 = 0x10000;
	BofSocketChannelParam_X.OnSocketAcceptCallback = nullptr;
	BofSocketChannelParam_X.OnSocketReadCallback = nullptr;
	BofSocketChannelTest SocketChannel(BofSocketChannelParam_X);

	SocketChannel.LoopWriteOnReadBuffer(true);   //For test purpose
	EXPECT_EQ(SocketChannel.WriteData(0, DataSent_S), BOFERR_NO_ERROR);

	BofSocketChannelStatistic_X = SocketChannel.BofSocketChannelStatistic();
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnect_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnectError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnect_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnectError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpRead_U32, 1);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpReadError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataByteRead_U64, DataSent_S.size());
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWrite_U32, 1);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWriteError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataByteWritten_U64, DataSent_S.size());
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbWriteEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbCloseEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbConnectEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpEndEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpShutdownEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpListenEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTimerEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTimerEventError_U32, 0);
}

TEST(SocketChannel_Test, WriteReadOpWithTestLoopAndNotifyAsap)
{
	std::string DataSent_S = "Hello World !", LastDataRead_S = "";
	BOF_NAMESPACE::BOF_SOCKET_CHANNEL_PARAM BofSocketChannelParam_X, CurrentBofSocketChannelParam_X;
	BOF_NAMESPACE::BOF_SOCKET_CHANNEL_STATISTIC BofSocketChannelStatistic_X;

	BofSocketChannelParam_X.ChannelId_U32 = 0;
	BofSocketChannelParam_X.Name_S = "SocketChannel_Test";
	BofSocketChannelParam_X.ListenBacklog_U32 = 0;;
	BofSocketChannelParam_X.IpAddress_S = "tcp://0.0.0.0:0;127.0.0.1:56000";
	BofSocketChannelParam_X.NotifyRcvBufferSize_U32 = 0x1000;
	BofSocketChannelParam_X.NotifyType_E = BOF_NAMESPACE::BOF_SOCKET_NOTIFY_TYPE::ASAP;
	BofSocketChannelParam_X.Delimiter_U8 = '\n';
	BofSocketChannelParam_X.pData = nullptr;
	BofSocketChannelParam_X.RcvBufferSize_U32 = 0x10000;
	BofSocketChannelParam_X.SndBufferSize_U32 = 0x10000;
	BofSocketChannelParam_X.OnSocketAcceptCallback = nullptr;
	BofSocketChannelParam_X.OnSocketReadCallback = nullptr;
	BofSocketChannelTest SocketChannel(BofSocketChannelParam_X);

	SocketChannel.LoopWriteOnReadBuffer(true);   //For test purpose
	EXPECT_EQ(SocketChannel.WriteData(0, DataSent_S), BOFERR_NO_ERROR);
	LastDataRead_S = SocketChannel.GetAndClearLastDataRead();
	EXPECT_STREQ(DataSent_S.c_str(), LastDataRead_S.c_str());

	BofSocketChannelStatistic_X = SocketChannel.BofSocketChannelStatistic();
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnect_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnectError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnect_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnectError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpRead_U32, 1);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpReadError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataByteRead_U64, DataSent_S.size());
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWrite_U32, 1);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWriteError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataByteWritten_U64, DataSent_S.size());
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbWriteEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbCloseEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbConnectEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpEndEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpShutdownEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpListenEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTimerEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTimerEventError_U32, 0);
}

TEST(SocketChannel_Test, WriteReadOpWithTestLoopAndNotifyWhenFull)
{
	const uint32_t NB_MESSAGE = 10;
	uint32_t i_U32;
	std::string DataSent_S = "Hello World !\r\n", LastDataRead_S = "", Reply_S = "";
	BOF_NAMESPACE::BOF_SOCKET_CHANNEL_PARAM BofSocketChannelParam_X, CurrentBofSocketChannelParam_X;
	BOF_NAMESPACE::BOF_SOCKET_CHANNEL_STATISTIC BofSocketChannelStatistic_X;

	BofSocketChannelParam_X.ChannelId_U32 = 0;
	BofSocketChannelParam_X.Name_S = "SocketChannel_Test";
	BofSocketChannelParam_X.ListenBacklog_U32 = 0;;
	BofSocketChannelParam_X.IpAddress_S = "tcp://0.0.0.0:0;127.0.0.1:56000";
	BofSocketChannelParam_X.NotifyRcvBufferSize_U32 = DataSent_S.length() * NB_MESSAGE;
	BofSocketChannelParam_X.NotifyType_E = BOF_NAMESPACE::BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL;
	BofSocketChannelParam_X.Delimiter_U8 = '\n';
	BofSocketChannelParam_X.pData = nullptr;
	BofSocketChannelParam_X.RcvBufferSize_U32 = 0x10000;
	BofSocketChannelParam_X.SndBufferSize_U32 = 0x10000;
	BofSocketChannelParam_X.OnSocketAcceptCallback = nullptr;
	BofSocketChannelParam_X.OnSocketReadCallback = nullptr;
	BofSocketChannelTest SocketChannel(BofSocketChannelParam_X);

	SocketChannel.LoopWriteOnReadBuffer(true);   //For test purpose

	for (i_U32 = 0; i_U32 < NB_MESSAGE; i_U32++)
	{
		Reply_S += DataSent_S;
		EXPECT_EQ(SocketChannel.WriteData(0, DataSent_S), BOFERR_NO_ERROR);
		LastDataRead_S = SocketChannel.GetAndClearLastDataRead();
		if (i_U32 == NB_MESSAGE - 1)
		{
			EXPECT_STREQ(Reply_S.c_str(), LastDataRead_S.c_str());
		}
		else
		{
			EXPECT_STREQ("", LastDataRead_S.c_str());
		}
	}

	BofSocketChannelStatistic_X = SocketChannel.BofSocketChannelStatistic();
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnect_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnectError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnect_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnectError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpRead_U32, 1);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpReadError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataByteRead_U64, NB_MESSAGE * DataSent_S.size());
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWrite_U32, NB_MESSAGE);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWriteError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataByteWritten_U64, NB_MESSAGE * DataSent_S.size());
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbWriteEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbCloseEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbConnectEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpEndEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpShutdownEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpListenEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTimerEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTimerEventError_U32, 0);
}

TEST(SocketChannel_Test, WriteReadOpWithTestLoopAndNotifyWhenFullOrDelimiterFound)
{
	const uint32_t NB_MESSAGE = 10;
	uint32_t i_U32;
	std::string DataSent_S = "Hello World !\r", LastDataRead_S = "", Reply_S = "", Delimiter_S;
	BOF_NAMESPACE::BOF_SOCKET_CHANNEL_PARAM BofSocketChannelParam_X, CurrentBofSocketChannelParam_X;
	BOF_NAMESPACE::BOF_SOCKET_CHANNEL_STATISTIC BofSocketChannelStatistic_X;

	BofSocketChannelParam_X.ChannelId_U32 = 0;
	BofSocketChannelParam_X.Name_S = "SocketChannel_Test";
	BofSocketChannelParam_X.ListenBacklog_U32 = 0;;
	BofSocketChannelParam_X.IpAddress_S = "tcp://0.0.0.0:0;127.0.0.1:56000";
	BofSocketChannelParam_X.NotifyRcvBufferSize_U32 = 0x1000;
	BofSocketChannelParam_X.NotifyType_E = BOF_NAMESPACE::BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND;
	BofSocketChannelParam_X.Delimiter_U8 = '\n';
	BofSocketChannelParam_X.pData = nullptr;
	BofSocketChannelParam_X.RcvBufferSize_U32 = 0x10000;
	BofSocketChannelParam_X.SndBufferSize_U32 = 0x10000;
	BofSocketChannelParam_X.OnSocketAcceptCallback = nullptr;
	BofSocketChannelParam_X.OnSocketReadCallback = nullptr;
	BofSocketChannelTest SocketChannel(BofSocketChannelParam_X);

	SocketChannel.LoopWriteOnReadBuffer(true);   //For test purpose

	for (i_U32 = 0; i_U32 < NB_MESSAGE; i_U32++)
	{
		if (i_U32 == NB_MESSAGE - 1)
		{
			Delimiter_S = BofSocketChannelParam_X.Delimiter_U8;
			Reply_S += Delimiter_S;
			EXPECT_EQ(SocketChannel.WriteData(0, Delimiter_S), BOFERR_NO_ERROR);
		}
		else
		{
			Reply_S += DataSent_S;
			EXPECT_EQ(SocketChannel.WriteData(0, DataSent_S), BOFERR_NO_ERROR);
		}
		LastDataRead_S = SocketChannel.GetAndClearLastDataRead();
		if (i_U32 == NB_MESSAGE - 1)
		{
			EXPECT_STREQ(Reply_S.c_str(), LastDataRead_S.c_str());
		}
		else
		{
			EXPECT_STREQ("", LastDataRead_S.c_str());
		}
	}

	BofSocketChannelStatistic_X = SocketChannel.BofSocketChannelStatistic();
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnect_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpConnectError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnect_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpDisconnectError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpRead_U32, 1);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpReadError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataByteRead_U64, ((NB_MESSAGE - 1) * DataSent_S.size()) + Delimiter_S.size());
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWrite_U32, NB_MESSAGE);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbOpWriteError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataByteWritten_U64, ((NB_MESSAGE - 1) * DataSent_S.size()) + Delimiter_S.size());
	EXPECT_EQ(BofSocketChannelStatistic_X.NbDataEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbWriteEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbError_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbCloseEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbConnectEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpEndEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpShutdownEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTcpListenEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTimerEvent_U32, 0);
	EXPECT_EQ(BofSocketChannelStatistic_X.NbTimerEventError_U32, 0);
}

/*
BOFERR OnConnect(const BOF_SOCKET_ASYNC_CMD &_rBofSocketAsyncCmd_X);
BOFERR OnDisconnect(const BOF_SOCKET_ASYNC_CMD &_rBofSocketAsyncCmd_X);
BOFERR OnWrite(const BOF_SOCKET_ASYNC_CMD &_rBofSocketAsyncCmd_X);
*/