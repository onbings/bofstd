/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * Author:      Bernard HARMEL: b.harmel@evs.com
 * Web:				 www.evs.com
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 19 2017  BHA : Initial release
 */
#pragma once

#include <bofstd/ibofioconnection.h>
#include <bofstd/ibofioconnectionfactory.h>

BEGIN_BOF_NAMESPACE()

class BofIoCmdChannel;

enum class BOF_IO_CMD : uint32_t
{
		IO_CMD_NONE = 0,
		IO_CMD_OPEN,
		IO_CMD_LIST,
		IO_CMD_RETR,
		IO_CMD_STOR,
		IO_CMD_CLOSE,
		IO_CMD_ABORT,

};

struct BOF_IO_DELEGATE_CMD
{
		BOF_IO_CMD IoCmd_E;
		bool ListenForConnection_B;

		BOF_IO_DELEGATE_CMD()
		{
			Reset();
		}

		void Reset()
		{
			IoCmd_E = BOF_IO_CMD::IO_CMD_NONE;
			ListenForConnection_B = false;
		}
};

class BofIoDataChannel : public IBofIoConnection, public IBofIoConnectionFactory, public BofThread
{
private:
		BofIoCmdChannel &mrIoCmdChannel;
		BOF_BUFFER mNotifyBuffer_X;
		uint32_t mRcvBufferSize_U32;
		uint32_t mSndBufferSize_U32;
		BofCircularBuffer <BOF_IO_DELEGATE_CMD> *mpIoDelegateCmdCollection;
		BOFERR mErrorCode_E;
		IBofIoConnection *mpIBofIoConnectionListener;

		bool mDataChannelOpened_B;

public:
		BofIoDataChannel(BofIoCmdChannel &_rIoCmdChannel, const IBOF_IO_CONNECTION_PARAM &_rIBofIoConnectionParam_X, BofIoConnectionManager *_pIoConnectionManager);

		virtual ~BofIoDataChannel();

		BOFERR OpenDataChannel(uint32_t _TimeoutInMs_U32, bool _ListenForConnection_B, BOF_SOCKET_ADDRESS &_rConnectToAddress_X, uint32_t _RcvBufferSize_U32,
		                       uint32_t _SndBufferSize_U32);  //If _ActivePort_U16 is 0 we use passive connect (PASV) otherwize the _ActivePort_U16 value is used to specify the connection)
		BOFERR CloseDataChannel(uint32_t _TimeoutInMs_U32);

		BOFERR PushIoDelegateCmd(uint32_t _TimeoutInMs_U32, const BOF_IO_DELEGATE_CMD &_rIoDelegateCmd_X);

		virtual BOFERR V_DataRead(uint32_t _Nb_U32, uint8_t *_pBuffer_U8) override;

		virtual BOFERR V_DataWritten(BOFERR _Sts_E, void *_pUserArg) override;

		virtual BOFERR V_RemoteIoClosed() override;

		virtual BOFERR V_Connected() override;

		virtual BOFERR V_ConnectFailed(BOFERR _Sts_E) override;

		virtual BOFERR V_Disconnected() override;

private:
		virtual IBofIoConnection *V_CreateSession(const BOF_IO_CHANNEL_INFO &_rBofIoChannelInfo_X, BofIoConnectionManager *_pIoConnectionManager, BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X) override;

		virtual BOFERR V_SessionDisconnected(IBofIoConnection *_pIoConnection) override;

		BOFERR V_OnProcessing() override;


};

END_BOF_NAMESPACE()
