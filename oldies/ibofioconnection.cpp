#include <bofstd/ibofioconnection.h>

BEGIN_BOF_NAMESPACE()

IBofIoConnection::IBofIoConnection(const IBOF_IO_CONNECTION_PARAM &_rIBofIoConnectionParam_X, BofIoConnectionManager *_pBofIoConnectionManager)
	: mIBofIoConnectionParam_X(_rIBofIoConnectionParam_X), mpBofIoConnectionManager(_pBofIoConnectionManager), mDataPreAllocated_B(false), mpDataBuffer_U8(nullptr), mWriteIndex_U32(0),
	  mDelimiterStartIndex_U32(0), mIoState_E(BOF_IO_STATE::IDLE), mpBofUvCallback_X(nullptr), mServerSession_B(false), mConnectError_E(BOFERR_NO_ERROR),
	  mProtocolType_E(BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN), mpIBofIoConnectionFactory(nullptr), mName_S(""), mpUvConnection_X(nullptr)
{
	BOF_ASSERT(mpBofIoConnectionManager != nullptr);

	mBofIoChannelParam_X.Reset();
	mConnectEvent_X.Reset();
	mEndOfIoEvent_X.Reset();
	mDisconnectEvent_X.Reset();
	mLocalAddress_X.Reset();
	mBindAddress_X.Reset();
	mDestinationAddress_X.Reset();

	mErrorCode_E = (mpBofIoConnectionManager == nullptr) ? BOFERR_INIT : BOFERR_NO_ERROR;
	if (mErrorCode_E == BOFERR_NO_ERROR)
	{
		if (mIBofIoConnectionParam_X.pData)
		{
			mDataPreAllocated_B = true;
			mpDataBuffer_U8 = reinterpret_cast<uint8_t *>(mIBofIoConnectionParam_X.pData);
		}
		else
		{
			mDataPreAllocated_B = false;
			mpDataBuffer_U8 = new uint8_t[mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32];
		}
		mErrorCode_E = mpDataBuffer_U8 ? BOFERR_NO_ERROR : BOFERR_NOT_ENOUGH_RESOURCE;
		if (mErrorCode_E == BOFERR_NO_ERROR)
		{
			mErrorCode_E = Bof_CreateEvent(mIBofIoConnectionParam_X.Name_S + "_con_evt", false, 1, false, mConnectEvent_X);
			if (mErrorCode_E == BOFERR_NO_ERROR)
			{
				mErrorCode_E = Bof_CreateEvent(mIBofIoConnectionParam_X.Name_S + "_eoi_evt", false, 1, false, mEndOfIoEvent_X);
				if (mErrorCode_E == BOFERR_NO_ERROR)
				{
					mErrorCode_E = Bof_CreateEvent(mIBofIoConnectionParam_X.Name_S + "_dsc_evt", false, 1, false, mDisconnectEvent_X);
				}
			}
		}
	}
}

IBofIoConnection::~IBofIoConnection()
{
	mpBofIoConnectionManager->EndOfConnection(mpUvConnection_X);
	mpBofIoConnectionManager->PushDisconnect(mpUvConnection_X ? mpUvConnection_X->Name_S.c_str() : "???????", mpUvConnection_X, false);
	if (!mDataPreAllocated_B)
	{
		BOF_SAFE_DELETE_ARRAY(mpDataBuffer_U8);
	}
	Bof_CloseEvent(mConnectEvent_X);
	Bof_CloseEvent(mEndOfIoEvent_X);
	Bof_CloseEvent(mDisconnectEvent_X);
	MarkConnectionAsDeleted();
}

void IBofIoConnection::MarkConnectionAsDeleted()
{
	mpBofIoConnectionManager->LockBofIoConnectionManager("MarkConnectionAsDeleted");
	if (mpUvConnection_X)
	{
		mpUvConnection_X->pIBofIoConnection = nullptr; //Signal Object is already deleted
	}
	mpBofIoConnectionManager->UnlockBofIoConnectionManager();
}

static Bof_Enum <BOF_IO_STATE> &S_IoStateEnumConverter()
{
	static Bof_Enum <BOF_IO_STATE> S_TheIoStateEnumConverter
		{
			{
				{BOF_IO_STATE::IDLE, "Idle"},
				{BOF_IO_STATE::CONNECTING, "Connecting"},
				{BOF_IO_STATE::CONNECTED, "Connected"},
				{BOF_IO_STATE::IO, "Io"},
				{BOF_IO_STATE::DISCONNECTED, "Disconnected"},
				{BOF_IO_STATE::TIMEOUT, "Timeout"},
				{BOF_IO_STATE::LISTENING, "Listening"},
			},
			BOF_IO_STATE::IDLE
		};
	return S_TheIoStateEnumConverter;
}

bool IBofIoConnection::ServerSession() const
{
	return mServerSession_B;
}

void IBofIoConnection::ServerSession(bool _State_B)
{
	mServerSession_B = _State_B;
}

BOF_UV_CONNECTION *IBofIoConnection::UvConnection() const
{
	return mpUvConnection_X;
}

void IBofIoConnection::UvConnection(BOF_UV_CONNECTION *_pUvConnection_X)
{
	mpUvConnection_X = _pUvConnection_X;
}

BOF_PROTOCOL_TYPE IBofIoConnection::ProtocolType() const
{
	return mProtocolType_E;
}

void IBofIoConnection::ProtocolType(BOF_PROTOCOL_TYPE _ProtocolType_E)
{
	mProtocolType_E = _ProtocolType_E;
}

BOF_SOCKET_ADDRESS IBofIoConnection::LocalAddress() const
{
	return mLocalAddress_X;
}

void IBofIoConnection::LocalAddress(const BOF_SOCKET_ADDRESS &_rLocalAddress_X)
{
	mLocalAddress_X = _rLocalAddress_X;
}

BOF_SOCKET_ADDRESS IBofIoConnection::BindAddress() const
{
	return mBindAddress_X;
}

void IBofIoConnection::BindAddress(const BOF_SOCKET_ADDRESS &_rBindAddress_X)
{
	mBindAddress_X = _rBindAddress_X;
}

BOF_SOCKET_ADDRESS IBofIoConnection::DestinationAddress() const
{
	return mDestinationAddress_X;
}

void IBofIoConnection::DestinationAddress(const BOF_SOCKET_ADDRESS &_rDestinationAddress_X)
{
	mDestinationAddress_X = _rDestinationAddress_X;
}

BOF_IO_CHANNEL_PARAM IBofIoConnection::IoChannelParam() const
{
	return mBofIoChannelParam_X;
}

void IBofIoConnection::IoChannelParam(const BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X)
{
	mBofIoChannelParam_X = _rBofIoChannelParam_X;
}

IBofIoConnectionFactory *IBofIoConnection::IConnectionFactory()
{
	return mpIBofIoConnectionFactory;
}

void IBofIoConnection::IConnectionFactory(IBofIoConnectionFactory *_pIBofIoConnectionFactory)
{
	mpIBofIoConnectionFactory = _pIBofIoConnectionFactory;
}

void IBofIoConnection::Name(const std::string &_rName_S)
{
	mName_S = _rName_S;
}

BofIoConnectionManager *IBofIoConnection::IoConnectionManager() const
{
	return mpBofIoConnectionManager;
}

BOFERR IBofIoConnection::LastErrorCode()
{
	return mErrorCode_E;
}

BOFERR IBofIoConnection::WaitForConnect(uint32_t _TimeoutInMs_U32)
{
	BOFERR Rts_E;

	//std::string Now_S; NOW(Now_S);	printf("### %s WaitForConnect %X %s p %p IoState %s\n", Now_S.c_str(), Bof_GetMsTickCount(), mName_S.c_str(), this, IoStateString().c_str());
	Rts_E = Bof_WaitForEvent(mConnectEvent_X, _TimeoutInMs_U32, 0);
	//NOW(Now_S); printf("### %s-------> WaitForConnect %X %s p %p IoState %s->%X\n", Now_S.c_str(), Bof_GetMsTickCount(), mName_S.c_str(), this, IoStateString().c_str(), Rts_E);
	return Rts_E;
}

BOFERR IBofIoConnection::SignalConnect()
{
	BOFERR Rts_E;

	//std::string Now_S; NOW(Now_S);	printf("### %s SignalConnect %X %s p %p IoState %s\n", Now_S.c_str(), Bof_GetMsTickCount(), mName_S.c_str(), this, IoStateString().c_str());
	Rts_E = Bof_SignalEvent(mConnectEvent_X, 0);
	return Rts_E;
}

BOFERR IBofIoConnection::WaitForEndOfIo(uint32_t _TimeoutInMs_U32)
{
	BOFERR Rts_E;
	//std::string Now_S; NOW(Now_S);
	// printf("### WaitForEndOfIo %X %s p %p IoState %s\n", Bof_GetMsTickCount(), mName_S.c_str(), this, IoStateString().c_str());
	Rts_E = Bof_WaitForEvent(mEndOfIoEvent_X, _TimeoutInMs_U32, 0);
	//NOW(Now_S);printf("### %s-------> WaitForEndOfIo %X %s p %p IoState %s->%X\n", Now_S.c_str(),Bof_GetMsTickCount(), mName_S.c_str(), this, IoStateString().c_str(), Rts_E);
	return Rts_E;
}

BOFERR IBofIoConnection::SignalEndOfIo()
{
	BOFERR Rts_E;
	//std::string Now_S; NOW(Now_S);
	//printf("### %s SignalEndOfIoEvent %X %s p %p IoState %s\n", Now_S.c_str(),Bof_GetMsTickCount(), mName_S.c_str(), this, IoStateString().c_str());
	Rts_E = Bof_SignalEvent(mEndOfIoEvent_X, 0);
	return Rts_E;
}

BOFERR IBofIoConnection::WaitForDisconnect(uint32_t _TimeoutInMs_U32)
{
	BOFERR Rts_E;
	//No more print it can be deleted here
	//std::string Now_S; NOW(Now_S);	printf("### %s WaitForDisconnect %X %s p %p IoState %s\n", Now_S.c_str(),Bof_GetMsTickCount(), mName_S.c_str(), this, IoStateString().c_str());
	//NO !  Rts_E = (IsConnected()) ? Bof_WaitForEvent(mDisconnectEvent_X, _TimeoutInMs_U32, 0) : BOFERR_NO_ERROR;
	Rts_E = Bof_WaitForEvent(mDisconnectEvent_X, _TimeoutInMs_U32, 0);
	//NOW(Now_S); printf("### %s -------> WaitForDisconnect %X %s p %p IoState %s->%X\n", Now_S.c_str(), Bof_GetMsTickCount(), mName_S.c_str(), this, IoStateString().c_str(), Rts_E);
	return Rts_E;
}

BOFERR IBofIoConnection::SignalDisconnect()
{
	BOFERR Rts_E;
	//std::string Now_S; NOW(Now_S);printf("### %s SignalDisconnect %X %s p %p IoState %s\n", Now_S.c_str(), Bof_GetMsTickCount(), mName_S.c_str(), this, IoStateString().c_str());
	Rts_E = Bof_SignalEvent(mDisconnectEvent_X, 0);
	return Rts_E;
}

BOF_UV_CALLBACK *IBofIoConnection::UvCallback()
{
	return mpBofUvCallback_X;
}

void IBofIoConnection::UvCallback(BOF_UV_CALLBACK *_pBofUvCallback_X)
{
	mpBofUvCallback_X = _pBofUvCallback_X;
}

BOFERR IBofIoConnection::V_DataRead(uint32_t _Nb_U32, uint8_t *_pBuffer_U8)
{
	return BOFERR_NO_ERROR;
}

BOFERR IBofIoConnection::V_DataWritten(BOFERR _Sts_E, void *_pUserArg)
{
	return BOFERR_NO_ERROR;
}

BOFERR IBofIoConnection::V_RemoteIoClosed()
{
	return BOFERR_NO_ERROR;
}

BOFERR IBofIoConnection::V_Connected()
{
	return BOFERR_NO_ERROR;
}

BOFERR IBofIoConnection::V_ConnectFailed(BOFERR _Sts_E)
{
	return BOFERR_NO_ERROR;
}

BOFERR IBofIoConnection::V_Disconnected()
{
	return BOFERR_NO_ERROR;
}

bool IBofIoConnection::IsConnected()
{
	bool Rts_B = ((mIoState_E == BOF_IO_STATE::CONNECTED) || (mIoState_E == BOF_IO_STATE::IO) ||
	              (mIoState_E == BOF_IO_STATE::LISTENING));  // BOF_IO_STATE::LISTENING for BofIoConnectionManager::Disconnect work
	//printf("=======> IsConnected %d (%d/%d)\n", Rts_B, mIoState_E , BOF_IO_STATE::CONNECTED);
	return Rts_B;
}

IBOF_IO_CONNECTION_PARAM IBofIoConnection::IBofIoConnectionParam() const
{
	return mIBofIoConnectionParam_X;
}


BOFERR IBofIoConnection::ConnectError()
{
	return mConnectError_E;
}

void IBofIoConnection::ConnectError(BOFERR _ConnectError_E)
{
	mConnectError_E = _ConnectError_E;
}

BOF_IO_STATE IBofIoConnection::IoState()
{
	return mIoState_E;
}

std::string IBofIoConnection::IoStateString()
{
	return S_IoStateEnumConverter().ToString(reinterpret_cast<BOF_IO_STATE>(mIoState_E));
}

void IBofIoConnection::IoState(BOF_IO_STATE _IoState_E)
{
	mIoState_E = _IoState_E;
}

std::string IBofIoConnection::Name()
{
	return mIBofIoConnectionParam_X.Name_S;
}

BOFERR IBofIoConnection::GetBuffer(BOF_BUFFER &_rBuffer_X)
{
	BOFERR Rts_E = BOFERR_NO_ERROR;
	_rBuffer_X.SizeInByte_U32 = mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32 - mWriteIndex_U32;
	_rBuffer_X.pData_U8 = &mpDataBuffer_U8[mWriteIndex_U32];

	return Rts_E;
}

BOFERR IBofIoConnection::OnReadyRead(uint32_t _NbNewByte_U32)
{
	BOFERR Rts_E = BOFERR_NO_ERROR, Sts_E;
	uint32_t Nb_U32, NbRemainingByte_U32, StartIndex_U32, LastIndex_U32, i_U32, NbByteInDelimitedPacket_U32, NbByteToMove_U32;

	if (_NbNewByte_U32)
	{
		NbRemainingByte_U32 = mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32 - mWriteIndex_U32;
		Nb_U32 = (_NbNewByte_U32 <= NbRemainingByte_U32) ? _NbNewByte_U32 : NbRemainingByte_U32;

		if (Nb_U32)
		{
			//       memcpy(&mpDataBuffer_U8[mWriteIndex_U32], _rBuffer_X.pData_U8, Nb_U32);
			StartIndex_U32 = mWriteIndex_U32;
			LastIndex_U32 = StartIndex_U32 + Nb_U32;
			mWriteIndex_U32 += Nb_U32;

			if (mIBofIoConnectionParam_X.NotifyType_E == BOF_IO_NOTIFY_TYPE::ASAP)
			{
				Sts_E = V_DataRead(Nb_U32, mpDataBuffer_U8);
				mWriteIndex_U32 = 0;
				mDelimiterStartIndex_U32 = 0;
			}
			else if (mIBofIoConnectionParam_X.NotifyType_E == BOF_IO_NOTIFY_TYPE::WHEN_FULL)
			{
				if (mWriteIndex_U32 == mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32)
				{
					Sts_E = V_DataRead(mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32, mpDataBuffer_U8);
					mWriteIndex_U32 = 0;
					mDelimiterStartIndex_U32 = 0;
				}
			}
			else if (mIBofIoConnectionParam_X.NotifyType_E == BOF_IO_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND)
			{
				//We look for Delimiter_U8 in mpDataBuffer_U8 data chunk per data chunk
				BOF_ASSERT(mWriteIndex_U32 <= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);
				BOF_ASSERT(mDelimiterStartIndex_U32 < mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);
				BOF_ASSERT(LastIndex_U32 <= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);

				do
				{
					for (i_U32 = StartIndex_U32; i_U32 < LastIndex_U32; i_U32++)
					{
						if (mpDataBuffer_U8[i_U32] == mIBofIoConnectionParam_X.Delimiter_U8)
						{
							NbByteInDelimitedPacket_U32 = i_U32 - mDelimiterStartIndex_U32 + 1;
							BOF_ASSERT(NbByteInDelimitedPacket_U32 <= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);

							Sts_E = V_DataRead(NbByteInDelimitedPacket_U32, &mpDataBuffer_U8[mDelimiterStartIndex_U32]);

							StartIndex_U32 = i_U32 + 1;
							BOF_ASSERT(StartIndex_U32 <= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);

							mDelimiterStartIndex_U32 = i_U32 + 1;
							BOF_ASSERT(mDelimiterStartIndex_U32 <= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);
							break;
						}
					}
				} while (i_U32 < LastIndex_U32);

				if (mWriteIndex_U32 >= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32)  //can happens if last byte of buffer is equal to Delimiter_U8 or no Delimiter_U8 byte found before the end
				{
					if (mDelimiterStartIndex_U32 == 0)
					{
						Sts_E = V_DataRead(mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32, mpDataBuffer_U8);
						mWriteIndex_U32 = 0;
						mDelimiterStartIndex_U32 = 0;
					}
					else
					{
						NbByteToMove_U32 = mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32 - mDelimiterStartIndex_U32;

						BOF_ASSERT(NbByteToMove_U32 < mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);
						if (NbByteToMove_U32)
						{
							memcpy(mpDataBuffer_U8, &mpDataBuffer_U8[mDelimiterStartIndex_U32], NbByteToMove_U32);
						}
						mWriteIndex_U32 = NbByteToMove_U32;
						mDelimiterStartIndex_U32 = 0;
					}
				}
				BOF_ASSERT(mWriteIndex_U32 <= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);
				BOF_ASSERT(mDelimiterStartIndex_U32 <= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);
			}
		}
	}
	return Rts_E;
}

BOFERR IBofIoConnection::WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S, void *_pUserArg, bool _CopyDataBuffer_B, bool _SignalEndOfIo_B, bool _IamInUvCallback_B)
{
	BOFERR Rts_E;
	Rts_E = WriteData(_TimeoutInMs_U32, _rBuffer_S.size(), reinterpret_cast<uint8_t *>(const_cast<char *>(_rBuffer_S.c_str())), _pUserArg, _CopyDataBuffer_B, _SignalEndOfIo_B, _IamInUvCallback_B);
	return Rts_E;
}

BOFERR IBofIoConnection::WriteData(uint32_t _TimeoutInMs_U32, uint32_t _Nb_U32, const uint8_t *_pBuffer_U8, void *_pUserArg, bool _CopyDataBuffer_B, bool _SignalEndOfIo_B, bool _IamInUvCallback_B)
{
	BOFERR Rts_E;
	BOF_IO_ASYNC_CMD BofIoAsyncCmd_X;

	BOF_ASSERT(_pBuffer_U8 != nullptr);
	BOF_ASSERT(mpUvConnection_X != nullptr);
	//BofIoAsyncCmd_X.pUvConnection_X = mpUvConnection_X;

	Rts_E = PrepareAsyncWriteCommand(BofIoAsyncCmd_X, mpUvConnection_X, _TimeoutInMs_U32, _Nb_U32, _pBuffer_U8, _pUserArg, _CopyDataBuffer_B, _SignalEndOfIo_B);
	if (Rts_E == BOFERR_NO_ERROR)
	{
		if (_IamInUvCallback_B)
		{
			mpBofIoConnectionManager->OnUvAsyncWrite(BofIoAsyncCmd_X);
		}
		else
		{
			Rts_E = mpBofIoConnectionManager->SendAsyncCmd(false, BofIoAsyncCmd_X);
		}
	}
	return Rts_E;
}

BOFERR IBofIoConnection::PrepareAsyncWriteCommand(BOF_IO_ASYNC_CMD &_rBofIoAsyncCmd_X, BOF_UV_CONNECTION *_pUvConnection_X, uint32_t _TimeoutInMs_U32, uint32_t _Nb_U32, const uint8_t *_pBuffer_U8,
                                                  void *_pUserArg, bool _CopyDataBuffer_B, bool _SignalEndOfIo_B)
{
	BOFERR Rts_E = BOFERR_NOT_ENOUGH_RESOURCE;

	_rBofIoAsyncCmd_X.Cmd_E = ASYNC_CMD::ASYNC_CMD_WRITE;
	_rBofIoAsyncCmd_X.NbBufferValid_U32 = 1;
	_rBofIoAsyncCmd_X.pBuffer_X[0].MustBeDeleted_B = false;
	_rBofIoAsyncCmd_X.pBuffer_X[0].SizeInByte_U32 = _Nb_U32;
	_rBofIoAsyncCmd_X.SignalEndOfIo_B = _SignalEndOfIo_B;
	if (_CopyDataBuffer_B)
	{
		//    printf("[[[alloc %d\n", _rBofIoAsyncCmd_X.pBuffer_X[0].SizeInByte_U32);
		_rBofIoAsyncCmd_X.pBuffer_X[0].MustBeDeleted_B = true;
		_rBofIoAsyncCmd_X.pBuffer_X[0].pData_U8 = new uint8_t[_rBofIoAsyncCmd_X.pBuffer_X[0].SizeInByte_U32];
		if (_rBofIoAsyncCmd_X.pBuffer_X[0].pData_U8)
		{
			memcpy(_rBofIoAsyncCmd_X.pBuffer_X[0].pData_U8, _pBuffer_U8, _rBofIoAsyncCmd_X.pBuffer_X[0].SizeInByte_U32);
		}
	}
	else
	{
		_rBofIoAsyncCmd_X.pBuffer_X[0].pData_U8 = const_cast<uint8_t *>(_pBuffer_U8);
	}
//  if (_rBofIoAsyncCmd_X.pBuffer_X[0].pData_U8)
	{
		_rBofIoAsyncCmd_X.pUvConnection_X = _pUvConnection_X;
		_rBofIoAsyncCmd_X.pUserArg = _pUserArg;
		_rBofIoAsyncCmd_X.TimeoutInMs_U32 = _TimeoutInMs_U32;
		_rBofIoAsyncCmd_X.StartTimeInMs_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
		Rts_E = BOFERR_NO_ERROR;
	}
	return Rts_E;
}


END_BOF_NAMESPACE()
