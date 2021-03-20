#pragma once

#include <bofstd/bofioconnectionmanager.h>

BEGIN_BOF_NAMESPACE()

class IBofIoConnection
{
private:
		BofIoConnectionManager *mpBofIoConnectionManager;
		IBOF_IO_CONNECTION_PARAM mIBofIoConnectionParam_X;
		bool mDataPreAllocated_B;
		uint8_t *mpDataBuffer_U8;
		uint32_t mWriteIndex_U32;
		uint32_t mDelimiterStartIndex_U32;
		BOFERR mErrorCode_E;
		volatile BOF_IO_STATE mIoState_E;
		BOF_UV_CALLBACK *mpBofUvCallback_X;
		BOF_EVENT mConnectEvent_X;
		BOF_EVENT mEndOfIoEvent_X;
		BOF_EVENT mDisconnectEvent_X;
		bool mServerSession_B;
		BOF_UV_CONNECTION *mpUvConnection_X;
		BOFERR mConnectError_E;
		BOF_PROTOCOL_TYPE mProtocolType_E;
		BOF_SOCKET_ADDRESS mLocalAddress_X;
		BOF_SOCKET_ADDRESS mBindAddress_X;
		BOF_SOCKET_ADDRESS mDestinationAddress_X;
		BOF_IO_CHANNEL_PARAM mBofIoChannelParam_X;
		IBofIoConnectionFactory *mpIBofIoConnectionFactory;
		std::string mName_S;

public:
		IBofIoConnection(const IBOF_IO_CONNECTION_PARAM &_rIBofIoConnectionParam_X, BofIoConnectionManager *_pBofIoConnectionManager);

		virtual ~IBofIoConnection();

//https://cpppatterns.com/patterns/rule-of-five.html
		IBofIoConnection(const IBofIoConnection &_rOther) = delete;

		IBofIoConnection(IBofIoConnection &&_rrOther) = delete;

		IBofIoConnection &operator=(const IBofIoConnection &_rOther) = delete;

		IBofIoConnection &operator=(IBofIoConnection &&_rrOther) = delete;

		BOFERR LastErrorCode();

		bool IsConnected();

		IBOF_IO_CONNECTION_PARAM IBofIoConnectionParam() const;

		bool ServerSession() const;

		void ServerSession(bool _State_B);

		BOF_UV_CONNECTION *UvConnection() const;

		void UvConnection(BOF_UV_CONNECTION *_pUvConnection_X);

		BOF_PROTOCOL_TYPE ProtocolType() const;

		void ProtocolType(BOF_PROTOCOL_TYPE _ProtocolType_E);

		BOF_SOCKET_ADDRESS LocalAddress() const;

		void LocalAddress(const BOF_SOCKET_ADDRESS &_rLocalAddress_X);

		BOF_SOCKET_ADDRESS BindAddress() const;

		void BindAddress(const BOF_SOCKET_ADDRESS &_rBindAddress_X);

		BOF_SOCKET_ADDRESS DestinationAddress() const;

		void DestinationAddress(const BOF_SOCKET_ADDRESS &_rDestinationAddress_X);

		BOF_IO_CHANNEL_PARAM IoChannelParam() const;

		void IoChannelParam(const BOF_IO_CHANNEL_PARAM &_rBofIoConnectionParam_X);

		IBofIoConnectionFactory *IConnectionFactory();

		void IConnectionFactory(IBofIoConnectionFactory *_pIBofIoConnectionFactory);

		void Name(const std::string &_rName_S);

		BofIoConnectionManager *IoConnectionManager() const;

		std::string Name();

		BOFERR ConnectError();

		void ConnectError(BOFERR _ConnectError_E);

		BOF_IO_STATE IoState();

		std::string IoStateString();

		void IoState(BOF_IO_STATE _IoState_E);

		BOFERR SignalConnect();

		BOFERR SignalEndOfIo();

		BOFERR SignalDisconnect();

		BOFERR WaitForConnect(uint32_t _TimeoutInMs_U32);

		BOFERR WaitForEndOfIo(uint32_t _TimeoutInMs_U32);

		BOFERR WaitForDisconnect(uint32_t _TimeoutInMs_U32);

		BOFERR GetBuffer(BOF_BUFFER &_rBuffer_X);

		BOFERR OnReadyRead(uint32_t _NbNewByte_U32);

		BOF_UV_CALLBACK *UvCallback();

		void UvCallback(BOF_UV_CALLBACK *_pBofUvCallback_X);

		BOFERR WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S, void *_pUserArg, bool _CopyDataBuffer_B, bool _SignalEndOfIo_B, bool _IamInUvCallback_B);

		BOFERR WriteData(uint32_t _TimeoutInMs_U32, uint32_t _Nb_U32, const uint8_t *_pBuffer_U8, void *_pUserArg, bool _CopyDataBuffer_B, bool _SignalEndOfIo_B, bool _IamInUvCallback_B);

		BOFERR PrepareAsyncWriteCommand(BOF_IO_ASYNC_CMD &_rBofIoAsyncCmd_X, BOF_UV_CONNECTION *_pUvConnection_X, uint32_t _TimeoutInMs_U32, uint32_t _Nb_U32, const uint8_t *_pBuffer_U8, void *_pUserArg,
		                                bool _CopyDataBuffer_B, bool _SignalEndOfIo_B);

		virtual BOFERR V_DataRead(uint32_t _Nb_U32, uint8_t *_pBuffer_U8);

		virtual BOFERR V_DataWritten(BOFERR _Sts_E, void *_pUserArg);

		virtual BOFERR V_RemoteIoClosed();

		virtual BOFERR V_Connected();

		virtual BOFERR V_ConnectFailed(BOFERR _Sts_E);

		virtual BOFERR V_Disconnected();

protected:
private:
		void MarkConnectionAsDeleted();
};

END_BOF_NAMESPACE()
