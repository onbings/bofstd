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
 * V 1.00  Dec 19 2017  BHA : Initial release
 */
#pragma once
//#include <bofstd/bofioconnectionmanager.h>

#include <bofstd/boferr.h>
#include <bofstd/ibofioconnection.h>
#include <bofstd/bofiodatachannel.h>
#include <bofstd/bofstringcircularbuffer.h>

BEGIN_BOF_NAMESPACE()
const uint32_t DEFAULT_CMD_DATA_TIMEOUT = 700000000;

class BofIoCmdChannel : public IBofIoConnection
{
private:
		static uint32_t S_mIoChannelId_U32;
		BofIoConnectionManager *mpIoConnectionManager;
		BofIoDataChannel *mpIoDataChannel;
		uint32_t mConnectTimeoutInMs_U32;
		uint32_t mIoCmdTimeoutInMs_U32;
		uint32_t mIoDataTimeoutInMs_U32;
		BOFERR mErrorCode_E;

		std::string mLastCommandReply_S;
		std::string mConnectedAddress_S;
		bool mListenForConnection_B;
		BOF_SOCKET_ADDRESS mDataConnectToAddress_X;
		BofStringCircularBuffer *mpBofStringCircularBuffer;

public:
		BofIoCmdChannel(const IBOF_IO_CONNECTION_PARAM &_rIBofIoConnectionParam_X, BofIoConnectionManager *_pIoConnectionManager);

		virtual ~BofIoCmdChannel();

		std::string &ConnectedAddress();

		const BOF_SOCKET_ADDRESS &DataConnectToAddress(bool &_rListenForConnection_B);

		uint32_t ConnectTimeoutInMs();

		uint32_t IoCmdTimeoutInMs();

		uint32_t IoDataTimeoutInMs();

		BOFERR Connect(const BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X, std::string &_rWelcomeMsg_S);

		BOFERR Disconnect(uint32_t _TimeoutInMs_U32);

		BOFERR Login(uint32_t _TimeoutInMs_U32, const std::string &_rUser_S, const std::string &_rPassword_S);

		BOFERR SendIoCmd(uint32_t _TimeoutInMs_U32, const std::string &_rCmd_S);

		BOFERR WaitForIoCmdReply(uint32_t _TimeoutInMs_U32, uint32_t _ExpectedReplyCode_U32, std::string &_rReply_S, uint32_t &_rReplyCode_U32);

		BOFERR SendIoCmdAndWaitForReply(uint32_t _TimeoutInMs_U32, const std::string &_rCmd_S, uint32_t _ExpectedReplyCode_U32, std::string &_rReply_S, uint32_t &_rReplyCode_U32);

		BOFERR SendIoCmdToCreateDataChannel(uint32_t _TimeoutInMs_U32, uint16_t _ActivePort_U16,
		                                    const BOF_BUFFER &_rNotifyBuffer_X);  //If _ActivePort_U16 is 0 we use passive connect (PASV) otherwize the _ActivePort_U16 value is used to specify the connection)
		BOFERR OpenDataChannel(uint32_t _TimeoutInMs_U32, bool _ListenForConnection_B, BOF_SOCKET_ADDRESS &_rConnectToAddress_X, uint32_t _RcvBufferSize_U32, uint32_t _SndBufferSize_U32);

		BOFERR CloseDataChannel(uint32_t _TimeoutInMs_U32);

		static uint32_t S_ReplyCodeFromString(const std::string &_rReply_S);

		virtual BOFERR V_DataRead(uint32_t _Nb_U32, uint8_t *_pBuffer_U8) override;

		virtual BOFERR V_DataWritten(BOFERR _Sts_E, void *_pUserArg) override;

		virtual BOFERR V_RemoteIoClosed() override;

		virtual BOFERR V_Connected() override;

		virtual BOFERR V_ConnectFailed(BOFERR _Sts_E) override;

		virtual BOFERR V_Disconnected() override;

private:
		//https://cpppatterns.com/patterns/rule-of-five.html
		BofIoCmdChannel(const BofIoCmdChannel &_rOther) = delete;

		BofIoCmdChannel(BofIoCmdChannel &&_rrOther) = delete;

		BofIoCmdChannel &operator=(const BofIoCmdChannel &_rOther) = delete;

		BofIoCmdChannel &operator=(BofIoCmdChannel &&_rrOther) = delete;

		BOFERR WaitForEndOfCommand(uint32_t _TimeoutInMs_U32, std::string &_rReply_S);
		// BOFERR WaitForEndOfTransfert(uint32_t _TimeoutInMs_U32, std::string &_rReply_S);

};

END_BOF_NAMESPACE()