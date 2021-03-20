/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * Author:      Bernard HARMEL: b.harmel@evs.com
 * Web:				  www.evs.com
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  May 14 2018  BHA : Initial release
 */
#pragma once

#include <bofstd/bofsystem.h>
#include <bofstd/bofsocketchannelmanager.h>
#include <bofstd/bofsocketchannel.h>

BEGIN_BOF_NAMESPACE()

struct BOF_SOCKET_CMD_CHANNEL_PARAM
{
		uint32_t ChannelId_U32;
		std::string Name_S;
		std::string IpAddress_S;    //Specify as tcp://[102:3c0:405:6a8:708:901:a0b:c01]:1234 our udp://0.0.0.0:0;127.0.0.1:1258

		BOF_SOCKET_CMD_CHANNEL_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			ChannelId_U32 = 0;
			Name_S = "";
			IpAddress_S = "";
		}
};

class BofSocketCmdChannel
{
private:
		std::shared_ptr<BofSocketChannelManager> mpsBofSocketChannelManager = nullptr;
		BOFERR mErrorCode_E = BOFERR_NO_ERROR;
		//bool mIsConnected_B = false;
		std::string mConnectedToIp_S = "";
		bool mIsLogin_B = false;
		uint32_t mLastPartialReplyCode_U32 = 0xFFFFFFFF;
		std::shared_ptr<BofSocketChannel> mpsSocketCmdChannel = nullptr; //This pointers are shared amongst the different shared_ptr instance which use this BofSocketCmdChannel
		std::shared_ptr<BOF_MUTEX> mpsMtx_X = nullptr;
		BOF_SOCKET_CMD_CHANNEL_PARAM mBofSocketChannelParam_X;

protected:
		BofSocketCmdChannel(const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager, const BOF_SOCKET_CMD_CHANNEL_PARAM &_rBofSocketChannelParam_X);

		virtual ~BofSocketCmdChannel();

public:
		static std::shared_ptr<BofSocketCmdChannel>
		S_BofSocketCmdChannelFactory(const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager, const BOF_SOCKET_CMD_CHANNEL_PARAM &_rBofSocketChannelParam_X);

		BOFERR LastErrorCode();

		BOFERR Connect(uint32_t _TimeoutInMs_U32, const std::string &_rConnectToIpAddress_S);

		BOFERR Disconnect(uint32_t _TimeoutInMs_U32);

		bool IsConnected();

		bool IsLogin();

		std::string ConnectedToIpAddress();

		BOFERR Login(uint32_t _TimeoutInMs_U32, const std::string &_rUser_S, const std::string &_rPassword_S);

		BOFERR SendCommandAndWaitForReply(uint32_t _TimeoutInMs_U32, const std::string &_rCommand_S, uint32_t _ExpectedReplyCode_U32, uint32_t &_rReplyCode_U32, std::string &_rReply_S);

private:
		BOFERR ParseReply(uint32_t _Nb_U32, const uint8_t *_pBuffer_U8, bool &_rFullReply_B, uint32_t &_rReplyCode_U32, std::string &_rFtpMessage_S);

		BOFERR SendCommand(uint32_t _TimeoutInMs_U32, const std::string &_rCommand_S);

		BOFERR WaitForCommandReply(uint32_t _TimeoutInMs_U32, uint32_t _ExpectedReplyCode_U32, uint32_t &_rReplyCode_U32, std::string &_rReply_S);

		static BOFERR S_ReEvaluateTimeout(uint32_t _Start_U32, int32_t &_rNewTimeOut_S32);

		BOFERR OnConnectCallback(uint32_t _Id_U32, std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> _psBofSocketChannel, const BOF_SOCKET_PEER_INFO &_rPeerInfo_X);

		BOFERR OnReadCallback(uint32_t _Id_U32, std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> _psBofSocketChannel, const BOF_BUFFER &_rBuffer_X, const BOF_SOCKET_PEER_INFO &_rPeerInfo_X);
};

END_BOF_NAMESPACE()