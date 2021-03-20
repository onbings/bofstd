/*
* Copyright (c) 2013-2023, Evs Broadcast Equipment All rights reserved.
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
#include <bofstd/bofiodatachannel.h>
#include <bofstd/bofiocmdchannel.h>

#include <bofstd/bofstringformatter.h>

BEGIN_BOF_NAMESPACE()
BofIoDataChannel::BofIoDataChannel(BofIoCmdChannel &_rIoCmdChannel, const IBOF_IO_CONNECTION_PARAM &_rIBofIoConnectionParam_X, BofIoConnectionManager *_pIoConnectionManager)
	: IBofIoConnection(_rIBofIoConnectionParam_X, _pIoConnectionManager), BofThread(_rIBofIoConnectionParam_X.Name_S), mrIoCmdChannel(_rIoCmdChannel), mpIoDelegateCmdCollection(nullptr),
	  mRcvBufferSize_U32(0), mSndBufferSize_U32(0), mpIBofIoConnectionListener(nullptr), mDataChannelOpened_B(false)
{
	BOF_CIRCULAR_BUFFER_PARAM BofCircularBufferParam_X;
	mNotifyBuffer_X.Reset();

	mErrorCode_E = InitThreadErrorCode();
	if (mErrorCode_E == BOFERR_NO_ERROR)
	{
		mErrorCode_E = BOFERR_NOT_ENOUGH_RESOURCE;
		//Normalement il ne peut y avoir qu'une commande � la fois dans mpIoDelegateCmdCollection !
		BofCircularBufferParam_X.MultiThreadAware_B = true;
		BofCircularBufferParam_X.NbMaxElement_U32 = 8;
		BofCircularBufferParam_X.Overwrite_B = false;
		BofCircularBufferParam_X.Blocking_B = true; // false;
		mpIoDelegateCmdCollection = new BofCircularBuffer<BOF_IO_DELEGATE_CMD>(BofCircularBufferParam_X);
		if (mpIoDelegateCmdCollection)
		{
			mErrorCode_E = mpIoDelegateCmdCollection->LastErrorCode();
			if (mErrorCode_E == BOFERR_NO_ERROR)
			{
				mErrorCode_E = LaunchThread(false, 0, 0x40000, 0, BOF_THREAD_POLICY_OTHER, BOF_THREAD_DEFAULT_PRIORITY, 5000);
			}
		}
	}
}

BofIoDataChannel::~BofIoDataChannel()
{
	BOFERR Sts_E;

	Sts_E = DestroyThread("~BofIoDataChannel");
	BOF_ASSERT(Sts_E == BOFERR_NO_ERROR);

	Sts_E = CloseDataChannel(DEFAULT_CMD_DATA_TIMEOUT);
	BOF_ASSERT(Sts_E == BOFERR_NO_ERROR);

	BOF_SAFE_DELETE(mpIoDelegateCmdCollection);

	if (mNotifyBuffer_X.MustBeDeleted_B)
	{
		BOF_SAFE_DELETE(mNotifyBuffer_X.pData_U8);
	}
	//Done in ~IBofIoConnection MarkConnectionAsDeleted();
}

BOFERR BofIoDataChannel::OpenDataChannel(uint32_t _TimeoutInMs_U32, bool _ListenForConnection_B, BOF_SOCKET_ADDRESS &_rConnectToAddress_X, uint32_t _RcvBufferSize_U32, uint32_t _SndBufferSize_U32)
{
	BOFERR Rts_E = BOFERR_RUNNING;
	BOF_SOCK_TYPE SocketType_E;
	BOF_PROTOCOL_TYPE ProtocolType_E;
	BOF_IO_CHANNEL_PARAM IoChannelParam_X;
	IBOF_IO_CONNECTION_PARAM IBofIoConnectionParam_X;
	uint32_t Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32;
	uint16_t Port_U16;

	mRcvBufferSize_U32 = _RcvBufferSize_U32;
	mSndBufferSize_U32 = _SndBufferSize_U32;
	_rConnectToAddress_X.Parse(SocketType_E, ProtocolType_E, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, Port_U16);

	IoChannelParam_X.RcvBufferSize_U32 = mRcvBufferSize_U32;
	IoChannelParam_X.SndBufferSize_U32 = mSndBufferSize_U32;
	IoChannelParam_X.ConnectTimeoutInMs_U32 = _TimeoutInMs_U32;
	IoChannelParam_X.IO_CHANNEL.TCP.NoDelay_B = true;

	if (_ListenForConnection_B)
	{
		IoChannelParam_X.Address_S = Bof_Sprintf("tcp://%d.%d.%d.%d:%d;0.0.0.0:0", Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, Port_U16);

		IBofIoConnectionParam_X.Name_S = IBofIoConnectionParam().Name_S;
		IBofIoConnectionParam_X.NotifyRcvBufferSize_U32 = 0x1000;
		IBofIoConnectionParam_X.NotifyType_E = BOF_IO_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND;
		IBofIoConnectionParam_X.Delimiter_U8 = '\n';
		Rts_E = IoConnectionManager()->Listen(this, IBofIoConnectionParam_X, IoChannelParam_X, 1, &mpIBofIoConnectionListener);
		if (Rts_E == BOFERR_NO_ERROR)
		{
			Rts_E = this->WaitForConnect(_TimeoutInMs_U32);
		}
	}
	else
	{
		IoChannelParam_X.Address_S = Bof_Sprintf("tcp://0.0.0.0:0;%d.%d.%d.%d:%d", Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, Port_U16);

		Rts_E = IoConnectionManager()->Connect(this, IoChannelParam_X);
		if (Rts_E == BOFERR_NO_ERROR)
		{
			Rts_E = this->WaitForConnect(_TimeoutInMs_U32);
		}
	}
	mDataChannelOpened_B = (Rts_E == BOFERR_NO_ERROR);

	return Rts_E;
}

BOFERR BofIoDataChannel::PushIoDelegateCmd(uint32_t _TimeoutInMs_U32, const BOF_IO_DELEGATE_CMD &_rIoDelegateCmd_X)
{
	BOFERR Rts_E;
	BOF_ASSERT(mpIoDelegateCmdCollection != nullptr);
	Rts_E = mpIoDelegateCmdCollection->Push(&_rIoDelegateCmd_X, _TimeoutInMs_U32);
	return Rts_E;
}

BOFERR BofIoDataChannel::CloseDataChannel(uint32_t _TimeoutInMs_U32)
{
	BOFERR Rts_E = BOFERR_NO_ERROR;

	if (mDataChannelOpened_B)
	{
		mDataChannelOpened_B = false;

		while (mpIoDelegateCmdCollection->GetNbElement())
		{
			Rts_E = WaitForEndOfIo(_TimeoutInMs_U32);
			BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);
		}
		Rts_E = IoConnectionManager()->PushDisconnect("CloseDataChannel_this", UvConnection(), false);
		if (mpIBofIoConnectionListener)
		{
			Rts_E = IoConnectionManager()->PushDisconnect("CloseDataChannel_mpIBofIoConnectionListener", mpIBofIoConnectionListener->UvConnection(), false);
		}
	}

	return Rts_E;
}

BOFERR BofIoDataChannel::V_DataRead(uint32_t _Nb_U32, uint8_t *_pBuffer_U8)
{
	BOFERR Rts_E = BOFERR_BAD_ID;
	return Rts_E;
}

BOFERR BofIoDataChannel::V_DataWritten(BOFERR _Sts_E, void *_pUserArg)
{
	return BOFERR_NO_ERROR;
}

BOFERR BofIoDataChannel::V_RemoteIoClosed()
{
	return BOFERR_NO_ERROR;
}

BOFERR BofIoDataChannel::V_Connected()
{
	return BOFERR_NO_ERROR;
}

BOFERR BofIoDataChannel::V_ConnectFailed(BOFERR _Sts_E)
{
	return BOFERR_NO_ERROR;
}

BOFERR BofIoDataChannel::V_Disconnected()
{
	BOFERR Rts_E;

	Rts_E = CloseDataChannel(DEFAULT_CMD_DATA_TIMEOUT);
	return Rts_E;
}

IBofIoConnection *BofIoDataChannel::V_CreateSession(const BOF_IO_CHANNEL_INFO &_rBofIoChannelInfo_X, BofIoConnectionManager *_pIoConnectionManager, BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X)
{
	BofIoDataChannel *pRts;
	IBOF_IO_CONNECTION_PARAM IBofIoConnectionParam_X;
	bool IsUdp_B = (_rBofIoChannelInfo_X.ProtocolType_E == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP);

	BOF_ASSERT(_pIoConnectionManager != nullptr);
	BOF_ASSERT(_pIoConnectionManager == IoConnectionManager());    //Can't change
	BOF_ASSERT(IsUdp_B == false);

	_rBofIoChannelParam_X.RcvBufferSize_U32 = mRcvBufferSize_U32;
	_rBofIoChannelParam_X.SndBufferSize_U32 = mSndBufferSize_U32;
	_rBofIoChannelParam_X.IO_CHANNEL.TCP.NoDelay_B = true;

	IBofIoConnectionParam_X.Name_S = _rBofIoChannelInfo_X.Name_S;
	IBofIoConnectionParam_X.NotifyRcvBufferSize_U32 = mNotifyBuffer_X.SizeInByte_U32;
	IBofIoConnectionParam_X.pData = mNotifyBuffer_X.pData_U8;
	IBofIoConnectionParam_X.NotifyType_E = BOF_IO_NOTIFY_TYPE::ASAP;
	IBofIoConnectionParam_X.Delimiter_U8 = 0;
	pRts = this;
	BOF_ASSERT(pRts != nullptr);
	return pRts;
}

BOFERR BofIoDataChannel::V_SessionDisconnected(IBofIoConnection *_pIoConnection)
{
	BofIoDataChannel *pIoDataChannel = reinterpret_cast<BofIoDataChannel *>(_pIoConnection);
	BOF_ASSERT(pIoDataChannel != nullptr);
//it is me !  BOF_SAFE_DELETE(pIoDataChannel);
	return BOFERR_NO_ERROR;
}

BOFERR BofIoDataChannel::V_OnProcessing()
{
	BOFERR Rts_E = BOFERR_NO_ERROR, Sts_E;
	BOF_IO_DELEGATE_CMD IoDelegateCmd_X;
	uint32_t i_U32;
	std::string Reply_S;
	BOF_SOCKET_ADDRESS DataConnectToAddress_X;
	bool ListenForConnection_B;

	while (!IsThreadLoopMustExit())
	{
		if (mpIoDelegateCmdCollection->Pop(&IoDelegateCmd_X, 1000, false) == BOFERR_NO_ERROR)
		{
			switch (IoDelegateCmd_X.IoCmd_E)
			{
				default:
				case BOF_IO_CMD::IO_CMD_NONE:
					break;

				case BOF_IO_CMD::IO_CMD_OPEN:
					break;

				case BOF_IO_CMD::IO_CMD_CLOSE:
					Reply_S = Bof_Sprintf("221 Service closing control connection.\r\n");
					Sts_E = mrIoCmdChannel.WriteData(mrIoCmdChannel.IoCmdTimeoutInMs(), Reply_S, nullptr, true, false, false);
					Rts_E = mrIoCmdChannel.Disconnect(DEFAULT_CMD_DATA_TIMEOUT);
					break;

				case BOF_IO_CMD::IO_CMD_ABORT:
					break;

				case BOF_IO_CMD::IO_CMD_LIST:
/*
125, 150  
        125 Data connection already open; transfer starting.  
        150 File status okay; about to open data connection.
226, 250 
        226 Closing data connection. Requested file action successful (for example, file transfer or file abort). 
        250 Requested file action okay, completed.
425, 426, 451 
        425 Can't open data connection.  
        426 Connection closed; transfer aborted. 
        451 Requested action aborted: local error in processing.
450 
        450 Requested file action not taken.  File unavailable (e.g., file busy).
500, 501, 502, 421, 530 
        500 Syntax error, command unrecognized. This may include errors such as command line too long. 
        501 Syntax error in parameters or arguments.
        502 Command not implemented.
        421 Service not available, closing control connection. This may be a reply to any command if the service knows it must shut down.
        530 Not logged in.
 */

					Reply_S = "150 File status okay; about to open data connection.\r\n";
					Sts_E = mrIoCmdChannel.WriteData(mrIoCmdChannel.IoCmdTimeoutInMs(), Reply_S, nullptr, true, false, false);
					if (Sts_E == BOFERR_NO_ERROR)
					{
						DataConnectToAddress_X = mrIoCmdChannel.DataConnectToAddress(ListenForConnection_B);
						Sts_E = mrIoCmdChannel.OpenDataChannel(mrIoCmdChannel.ConnectTimeoutInMs(), ListenForConnection_B, DataConnectToAddress_X, 0x10000, 0x10000);
						if (Sts_E == BOFERR_NO_ERROR)
						{
							for (i_U32 = 0; i_U32 < 100; i_U32++)
							{
								if (!(i_U32 % 1)) // FS_FILE_ATTRIBUTE_DIRECTORY)
								{
									Reply_S = Bof_Sprintf("drwx------ 1 user group %010d %s %02d %02d:%02d Dir_%04d\r\n", 0, "May", 26, 6, 30, i_U32);
								}
								else
								{
									Reply_S = Bof_Sprintf("-rwx------ 1 user group %010lld %s %02d %02d:%02d %s\r\n", 0x10000000, "Jun", 27, 18, 15, i_U32);
								}
								if (!(i_U32 % 4)) //FS_FILE_ATTRIBUTE_READONLY)
								{
									Reply_S[2] = '-';
								}
								Sts_E = WriteData(mrIoCmdChannel.IoDataTimeoutInMs(), Reply_S, nullptr, true, false, false);
								BOF_ASSERT(Sts_E == BOFERR_NO_ERROR);
								Bof_Sleep(10);   //Simulate delay
							}
							Sts_E = CloseDataChannel(DEFAULT_CMD_DATA_TIMEOUT);
							if (Sts_E == BOFERR_NO_ERROR)
							{
								Reply_S = Bof_Sprintf("226 Transfer complete, Closing data connection.\r\n");
							}
							else
							{
								Reply_S = Bof_Sprintf("426 Connection closed; transfer aborted.\r\n");
							}
							Sts_E = mrIoCmdChannel.WriteData(mrIoCmdChannel.IoCmdTimeoutInMs(), Reply_S, nullptr, true, false, false);

						}
						else
						{
							Reply_S = Bof_Sprintf("425 Can't open data connection.\r\n");
							Sts_E = mrIoCmdChannel.WriteData(mrIoCmdChannel.IoCmdTimeoutInMs(), Reply_S, nullptr, true, false, false);
						}
					}
					break;

				case BOF_IO_CMD::IO_CMD_RETR:
					break;

				case BOF_IO_CMD::IO_CMD_STOR:
					break;

			}
			Sts_E = SignalEndOfIo();
		}

	}
	return Rts_E;
}
END_BOF_NAMESPACE()