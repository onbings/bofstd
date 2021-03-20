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

#include <bofstd/bofsocketchannelmanager.h>
#include <bofstd/bofsocketos.h>

BEGIN_BOF_NAMESPACE()

struct BOF_TCP_SOCKET_PARAM;
struct BOF_UDP_SOCKET_PARAM;

enum class CHANNEL_RESOURCE_RELEASE_STATE : uint32_t
{
		IDLE = 0,
		PENDING,
		CLOSING,
		CLOSED
};

struct BOF_SOCKET_CHANNEL_PARAM
{
		uint32_t ChannelId_U32;
		std::string Name_S;
		uint32_t ListenBacklog_U32;  //If != from zero we craete a listening socket
		std::string IpAddress_S;    //Specify as tcp://[102:3c0:405:6a8:708:901:a0b:c01]:1234 our udp://0.0.0.0:0;127.0.0.1:1258
		BOF_SOCKET_NOTIFY_TYPE NotifyType_E;
		uint8_t Delimiter_U8;  //Used by BOF_IO_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND
		uint32_t NotifyRcvBufferSize_U32;
		void *pData;   /*! Specifies a pointer to the internal buffer data zone (pre-allocated buffer). Set to nullptr if the memory must be allocated by the function (size is _NotifyRcvBufferSize_U32)*/
		uint32_t RcvBufferSize_U32;
		uint32_t SndBufferSize_U32;
		uint32_t CloseTimeoutInMs_U32;
		uint32_t ClosePollingTimeInMs_U32;
		ON_SOCKET_CONNECT_CALLBACK OnSocketConnectCallback;
		ON_SOCKET_ACCEPT_CALLBACK OnSocketAcceptCallback;
		ON_SOCKET_READ_CALLBACK OnSocketReadCallback;

		BOF_SOCKET_CHANNEL_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			ChannelId_U32 = 0;
			Name_S = "";
			ListenBacklog_U32 = 0;
			IpAddress_S = "";
			NotifyType_E = BOF_SOCKET_NOTIFY_TYPE::ASAP;
			Delimiter_U8 = 0;
			NotifyRcvBufferSize_U32 = 0;
			pData = nullptr;
			RcvBufferSize_U32 = 0;
			SndBufferSize_U32 = 0;
			CloseTimeoutInMs_U32 = 0;
			ClosePollingTimeInMs_U32 = 0;

			OnSocketConnectCallback = nullptr;
			OnSocketAcceptCallback = nullptr;
			OnSocketReadCallback = nullptr;
		}

		bool operator==(const BOF_SOCKET_CHANNEL_PARAM &_Other) const
		{
			return ((Name_S == _Other.Name_S) &&
			        (ChannelId_U32 == _Other.ChannelId_U32) &&
			        (ListenBacklog_U32 == _Other.ListenBacklog_U32) &&
			        (IpAddress_S == _Other.IpAddress_S) &&
			        (NotifyRcvBufferSize_U32 == _Other.NotifyRcvBufferSize_U32) &&
			        (NotifyType_E == _Other.NotifyType_E) &&
			        (Delimiter_U8 == _Other.Delimiter_U8) &&
			        (RcvBufferSize_U32 == _Other.RcvBufferSize_U32) &&
			        (SndBufferSize_U32 == _Other.SndBufferSize_U32) &&
			        (CloseTimeoutInMs_U32 == _Other.CloseTimeoutInMs_U32) &&
			        (ClosePollingTimeInMs_U32 == _Other.ClosePollingTimeInMs_U32) &&
			        (pData == _Other.pData));
			//    (OnSocketConnectCallback == _Other.OnSocketConnectCallback) &&
			//    (OnSocketAcceptCallback == _Other.OnSocketAcceptCallback) &&
			//    (OnSocketReadCallback == _Other.OnSocketReadCallback) &&
		}
};

struct BOF_SOCKET_CHANNEL_STATISTIC
{
		uint32_t NbOpConnect_U32;
		uint32_t NbOpConnectError_U32;
		uint32_t NbOpDisconnect_U32;
		uint32_t NbOpDisconnectError_U32;
		uint32_t NbOpListenError_U32;
		uint32_t NbOpRead_U32;
		uint32_t NbOpReadError_U32;
		uint64_t NbDataByteRead_U64;
		uint32_t NbOpWrite_U32;
		uint32_t NbOpWriteError_U32;
		uint64_t NbDataByteWritten_U64;
		uint32_t NbDataEvent_U32;
		uint32_t NbWriteEvent_U32;
		uint32_t NbError_U32;
		uint32_t NbCloseEvent_U32;
		uint32_t NbConnectEvent_U32;

		uint32_t NbTcpEndEvent_U32;
		uint32_t NbTcpShutdownEvent_U32;
		uint32_t NbTcpListenEvent_U32;

		uint32_t NbTimerEvent_U32;
		uint32_t NbTimerEventError_U32;


		BOF_SOCKET_CHANNEL_STATISTIC()
		{
			Reset();
		}

		void Reset()
		{
			NbOpConnect_U32 = 0;
			NbOpConnectError_U32 = 0;
			NbOpDisconnect_U32 = 0;
			NbOpDisconnectError_U32 = 0;
			NbOpListenError_U32 = 0;

			NbOpRead_U32 = 0;
			NbOpReadError_U32 = 0;
			NbDataByteRead_U64 = 0;
			NbOpWrite_U32 = 0;
			NbOpWriteError_U32 = 0;
			NbDataByteWritten_U64 = 0;
			NbDataEvent_U32 = 0;
			NbWriteEvent_U32 = 0;
			NbError_U32 = 0;
			NbCloseEvent_U32 = 0;
			NbConnectEvent_U32 = 0;

			NbTcpEndEvent_U32 = 0;
			NbTcpShutdownEvent_U32 = 0;
			NbTcpListenEvent_U32 = 0;

			NbTimerEvent_U32 = 0;
			NbTimerEventError_U32 = 0;
		}
};


struct BOF_SOCKET_CHANNEL_STATE
{
		BOF_SOCKET_STATE State_E;
		uint32_t StartTimeInMs_U32;
		uint32_t TimeOutInMs_U32;
		BOF_EVENT *pNotifyEvent_X; //If null caller must poll on BofSocketChannelState to get op status otherwise the event will be nsignaled and caller can call BofSocketChannelState to get final status
		std::shared_ptr<void> psUserArg;
		BofSocketChannel *pAcceptedByThisListener;

		BOF_SOCKET_CHANNEL_STATE()
		{
			Reset();
		}

		void Reset()
		{
			State_E = BOF_SOCKET_STATE::DISCONNECTED;
			StartTimeInMs_U32 = 0;
			TimeOutInMs_U32 = 0;
			pNotifyEvent_X = nullptr;
			psUserArg = nullptr;
			pAcceptedByThisListener = nullptr;
		}
};

class BofSocketChannel : public std::enable_shared_from_this<BofSocketChannel>
{
private:
		BOF_SOCKET_CHANNEL_PARAM mBofSocketChannelParam_X;
		BOF_SOCKET_CHANNEL_STATISTIC mBofSocketChannelStatistic_X;
		bool mDataPreAllocated_B = false;
		uint8_t *mpDataBuffer_U8 = nullptr;
		uint32_t mWriteIndex_U32 = 0;
		uint32_t mDelimiterStartIndex_U32 = 0;
		BOFERR mErrorCode_E = BOFERR_NO_ERROR;
		std::shared_ptr<BofSocketChannelManager> mpsBofSocketChannelManager = nullptr;

		bool mIsUdp_B = false;
		BOF_SOCKET_ADDRESS mInterfaceBofSocketAddress_X;
		BOF_SOCKET_ADDRESS mBofSocketAddress_X;

		CHANNEL_RESOURCE_RELEASE_STATE mChannelResourceReleaseState_E = CHANNEL_RESOURCE_RELEASE_STATE::IDLE;
		std::shared_ptr<uvw::TimerHandle> mpsTimer = nullptr;
		std::shared_ptr<uvw::TcpHandle> mpsTcpHandle = nullptr;
		std::shared_ptr<uvw::UDPHandle> mpsUdpHandle = nullptr;

		BOF_SOCKET_CHANNEL_STATE mBofSocketChannelState_X;
		std::vector<std::shared_ptr<BofSocketChannel>> mBofSocketAcceptedChannelCollection;

		BOF_MUTEX mBofSocketChannelMutex;

		uint32_t mMinPort_U32 = 0;
		uint32_t mMaxPort_U32 = 0;
		uint32_t mNextAvailablePort_U32 = 0;

protected:
		BofSocketChannel(const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager, const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X);

		//Internal use for listen/create client
		BofSocketChannel(const BOF_SOCKET_CHANNEL_STATE &_rInitialBofChannelState_X, const std::shared_ptr<uvw::TcpHandle> &_rpsAlreadyCreatedTcpHandle,
		                 const std::shared_ptr<uvw::UDPHandle> &_rpsAlreadyCreatedUdpHandle, const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager,
		                 const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X);

public:
		virtual ~BofSocketChannel();

		static std::shared_ptr<BofSocketChannel>
		S_BofSocketChannelFactory(const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager, const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X);

		//Internal use for listen/create client
		static std::shared_ptr<BofSocketChannel> S_BofSocketChannelFactory(const BOF_SOCKET_CHANNEL_STATE &_rInitialBofChannelState_X, const std::shared_ptr<uvw::TcpHandle> &_rpsAlreadyCreatedTcpHandle,
		                                                                   const std::shared_ptr<uvw::UDPHandle> &_rpsAlreadyCreatedUdpHandle,
		                                                                   const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager,
		                                                                   const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X);

		//https://cpppatterns.com/patterns/rule-of-five.html
		BofSocketChannel(const BofSocketChannel &_rOther) = delete;

		BofSocketChannel(BofSocketChannel &&_rrOther) = delete;

		BofSocketChannel &operator=(const BofSocketChannel &_rOther) = delete;

		BofSocketChannel &operator=(BofSocketChannel &&_rrOther) = delete;

		BOFERR LockBofSocketChannel(const std::string &_rLockerName_S);

		BOFERR UnlockBofSocketChannel();
		// const std::vector<std::shared_ptr<BofSocketChannel>>  &BofSocketAcceptedChannelCollection();

		BOFERR CleanUpFromLibUvContext();

		BOFERR WaitForEndOfClose(uint32_t _TimeoutInMs_U32, uint32_t PollingTimeInMs_U32);

		BOFERR InitErrorCode() const;

		const BOF_SOCKET_CHANNEL_PARAM &BofSocketChannelParam() const;

		const BOF_SOCKET_CHANNEL_STATISTIC &BofSocketChannelStatistic() const;

		void ResetStat();

		const BOF_SOCKET_CHANNEL_STATE &BofSocketChannelState() const;

		bool IsConnected() const;

		bool IsAnAcceptedClient() const;

		bool IsAListener() const;

		CHANNEL_RESOURCE_RELEASE_STATE ChannelResourceReleaseState() const;

		void ChannelResourceReleaseState(CHANNEL_RESOURCE_RELEASE_STATE _ReleaseState_E);

		const BOF_SOCKET_ADDRESS &LocalAddress() const;

		const BOF_SOCKET_ADDRESS &PeerAddress() const;

		uint32_t GetAndIncrementNextAvailablePort();

		BOFERR ChannelId(uint32_t _ChannelId_U32);

		BOFERR Delimiter(uint8_t _Delimiter_U8);

		BOFERR Name(const std::string &_rName_S);

		BOFERR NotifyType(BOF_SOCKET_NOTIFY_TYPE _NotifyType_E);

		BOFERR NotifyRcvBufferSize(uint32_t _NotifyRcvBufferSize_U32,
		                           void *_pData);   /*! Specifies a pointer to the internal buffer data zone (pre-allocated buffer). Set to nullptr if the memory must be allocated by the function (size is _NotifyRcvBufferSize_U32)*/
		BOFERR OnSocketReadCallback(ON_SOCKET_READ_CALLBACK _OnSocketReadCallback);

		BOFERR RcvBufferSize(uint32_t _RcvBufferSize_U32);

		BOFERR SndBufferSize(uint32_t _SndBufferSize_U32);

		BOFERR WriteData(uint32_t _TimeoutInMs_U32, bool _MakeACopy_B, const std::string &_rData_S);

		BOFERR WriteData(uint32_t _TimeoutInMs_U32, bool _MakeACopy_B, uint32_t _Size_U32, const uint8_t *_pData_U8);

		BOFERR OnDataRead(uint32_t _Nb_U32, uint8_t *_pBuffer_U8, bool _Partial_B, const BOF_SOCKET_PEER_INFO &_rPeerInfo_X);

		BOFERR GetBuffer(BOF_BUFFER &_rBuffer_X);

		BOFERR OnAsyncConnect(const BOF_SOCKET_ASYNC_CMD &_rBofSocketAsyncCmd_X);

		BOFERR OnAsyncDisconnect(const BOF_SOCKET_ASYNC_CMD &_rBofSocketAsyncCmd_X);

		BOFERR OnAsyncListen(const BOF_SOCKET_ASYNC_CMD &_rBofSocketAsyncCmd_X);

		BOFERR OnAsyncWrite(const BOF_SOCKET_ASYNC_CMD &_rBofSocketAsyncCmd_X);

private:
		BOFERR SetupBofSocketChannel(const BOF_SOCKET_CHANNEL_STATE &_rInitialBofChannelState_X, const std::shared_ptr<uvw::TcpHandle> &_rpsAlreadyCreatedTcpHandle,
		                             const std::shared_ptr<uvw::UDPHandle> &_rpsAlreadyCreatedUdpHandle, const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager,
		                             const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X);

		BOFERR AddAcceptedChannel(const std::shared_ptr<BofSocketChannel> &_rpsBofSocketChannel);

		BOFERR RemoveAcceptedChannel(const std::string &_rCaller_S, const std::shared_ptr<BofSocketChannel> &_rpsBofSocketChannel);

		BOFERR CloseListenerChannelAndItsAcceptedChannel();

		BOFERR NotifyEvent(std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> _psBofSocketChannel, BOF_SOCKET_PEER_INFO *_pPeerInfo_X, BOF_SOCKET_CHANNEL_PARAM *_pBofSocketChannelParamFromAcceptedClient_X);

		BOFERR OnReadyRead(uint32_t _NbNewByte_U32, bool _Partial_B, const BOF_SOCKET_PEER_INFO &_rPeerInfo_X);

		BOFERR SetSocketBufferSizeOption(const std::shared_ptr<uvw::UDPHandle> &_rpsHandle, uint32_t _RcvBufferSize_U32, uint32_t _SndBufferSize_U32);

		BOFERR SetSocketChannelOption(const std::shared_ptr<uvw::UDPHandle> &_rpsHandle, const BOF_UDP_SOCKET_PARAM &_rBofUdpSocketParam_X);

		BOFERR OnUdpDataEvent(const uvw::UDPDataEvent &_rEvent, uvw::UDPHandle &_rHandle);

		BOFERR OnUdpDataPreAllocatedEvent(const uvw::UDPDataPreAllocatedEvent &_rEvent, uvw::UDPHandle &_rHandle);

		BOFERR OnUdpSendEvent(const uvw::SendEvent &_rEvent, uvw::UDPHandle &_rHandle);

		BOFERR OnUdpError(const uvw::ErrorEvent &_rEvent, uvw::UDPHandle &_rHandle);

		BOFERR OnUdpCloseEvent(const uvw::CloseEvent &_rEvent, uvw::UDPHandle &_rHandle);

		BOFERR SetSocketBufferSizeOption(const std::shared_ptr<uvw::TcpHandle> &_rpsHandle, uint32_t _RcvBufferSize_U32, uint32_t _SndBufferSize_U32);

		BOFERR SetSocketChannelOption(const std::shared_ptr<uvw::TcpHandle> &_rpsHandle, const BOF_TCP_SOCKET_PARAM &_rBofTcpSocketParam_X);

		BOFERR OnTcpDataEvent(const uvw::DataEvent &_rEvent, uvw::TcpHandle &_rHandle);

		BOFERR OnTcpDataPreallocatedEvent(const uvw::DataPreAllocatedEvent &_rEvent, uvw::TcpHandle &_rHandle); //BHA
		BOFERR OnTcpWriteEvent(const uvw::WriteEvent &_rEvent, uvw::TcpHandle &_rHandle);

		BOFERR OnTcpCloseEvent(const uvw::CloseEvent &_rEvent, uvw::TcpHandle &_rHandle);

		BOFERR OnTcpError(const uvw::ErrorEvent &_rEvent, uvw::TcpHandle &_rHandle);

		BOFERR OnTcpEndEvent(const uvw::EndEvent &_rEvent, uvw::TcpHandle &_rHandle);

		BOFERR OnTcpShutdownEvent(const uvw::ShutdownEvent &_rEvent, uvw::TcpHandle &_rHandle);

		BOFERR OnTcpListenEvent(const uvw::ListenEvent &_rEvent, uvw::TcpHandle &_rHandle);

		BOFERR OnTcpConnect(const uvw::ConnectEvent &_rEvent, uvw::TcpHandle &_rHandle);

		BOFERR OnTimerEvent(const uvw::TimerEvent &_rEvent, uvw::TimerHandle &_rHandle);

		BOFERR OnTimerError(const uvw::ErrorEvent &_rEvent, uvw::TimerHandle &_rHandle);

};

END_BOF_NAMESPACE()