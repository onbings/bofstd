/*
  Copyright (c) 2013-2023, Evs Broadcast Equipment All rights reserved.

  Author:      Bernard HARMEL: b.harmel@evs.com
  Web:				 www.evs.com
  Revision:    1.0

  Rem:         Nothing

  History:

  V 1.00  May 14 2018  BHA : Initial release
*/
#include <regex>
#include <bofstd/bofsocketcmdchannel.h>
#include <bofstd/bofstringformatter.h>

BEGIN_BOF_NAMESPACE()

class BofSocketCmdChannelFactory : public BofSocketCmdChannel
{
public:
		BofSocketCmdChannelFactory(const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager, const BOF_SOCKET_CMD_CHANNEL_PARAM &_rBofSocketCmdChannelParam_X) : BofSocketCmdChannel(
			_rpsBofSocketChannelManager, _rBofSocketCmdChannelParam_X)
		{}

		virtual ~BofSocketCmdChannelFactory()
		{}
};

BofSocketCmdChannel::BofSocketCmdChannel(const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager, const BOF_SOCKET_CMD_CHANNEL_PARAM &_rBofCmdSocketChannelParam_X)
{
	BOF_SOCKET_CHANNEL_PARAM BofSocketChannelParam_X;

	mpsBofSocketChannelManager = _rpsBofSocketChannelManager;
	mBofSocketChannelParam_X = _rBofCmdSocketChannelParam_X;
	mErrorCode_E = BOFERR_NOT_ENOUGH_RESOURCE;
	mpsMtx_X = std::make_shared<BOF_NAMESPACE::BOF_MUTEX>();
	if (mpsMtx_X)
	{
		mErrorCode_E = BOF_NAMESPACE::Bof_CreateMutex("BofSocketCmdChannel", true, true, *mpsMtx_X);
		if (mErrorCode_E == BOFERR_NO_ERROR)
		{
			mErrorCode_E = BOFERR_NOT_ENOUGH_RESOURCE;
			BofSocketChannelParam_X.ChannelId_U32 = _rBofCmdSocketChannelParam_X.ChannelId_U32;
			BofSocketChannelParam_X.Name_S = _rBofCmdSocketChannelParam_X.Name_S;
			BofSocketChannelParam_X.ListenBacklog_U32 = 0;
			BofSocketChannelParam_X.IpAddress_S = _rBofCmdSocketChannelParam_X.IpAddress_S;
			BofSocketChannelParam_X.NotifyRcvBufferSize_U32 = 0x10000;
			BofSocketChannelParam_X.NotifyType_E = BOF_NAMESPACE::BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND;
			BofSocketChannelParam_X.Delimiter_U8 = '\n';
			BofSocketChannelParam_X.pData = nullptr;
			BofSocketChannelParam_X.RcvBufferSize_U32 = 0x10000;
			BofSocketChannelParam_X.SndBufferSize_U32 = 0x10000;
			BofSocketChannelParam_X.CloseTimeoutInMs_U32 = 2000;
			BofSocketChannelParam_X.ClosePollingTimeInMs_U32 = 20;

			BofSocketChannelParam_X.OnSocketConnectCallback = BOF_BIND_3_ARG_TO_METHOD(BofSocketCmdChannel::OnConnectCallback);
			BofSocketChannelParam_X.OnSocketAcceptCallback = nullptr;
			BofSocketChannelParam_X.OnSocketReadCallback = BOF_BIND_4_ARG_TO_METHOD(BofSocketCmdChannel::OnReadCallback);
			mErrorCode_E = _rpsBofSocketChannelManager->AddChannel(BofSocketChannelParam_X, mpsSocketCmdChannel);
			if (mErrorCode_E == BOFERR_NO_ERROR)
			{
				if (mpsSocketCmdChannel)
				{
					mErrorCode_E = mpsSocketCmdChannel->InitErrorCode();
					if (mErrorCode_E == BOFERR_NO_ERROR)
					{
					}
				}
			}
		}
	}
}

std::shared_ptr<BofSocketCmdChannel>
BofSocketCmdChannel::S_BofSocketCmdChannelFactory(const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager, const BOF_SOCKET_CMD_CHANNEL_PARAM &_rBofSocketChannelParam_X)
{
	return std::make_shared<BofSocketCmdChannelFactory>(_rpsBofSocketChannelManager, _rBofSocketChannelParam_X);
}

BofSocketCmdChannel::~BofSocketCmdChannel()
{
	if (mpsMtx_X)
	{
		BOF_NAMESPACE::Bof_DestroyMutex(*mpsMtx_X);
	}
}

BOFERR BofSocketCmdChannel::LastErrorCode()
{
	return mErrorCode_E;
}

BOFERR BofSocketCmdChannel::Connect(uint32_t _TimeoutInMs_U32, const std::string &_rConnectToIpAddress_S)
{
	BOFERR Rts_E = mErrorCode_E;
	uint32_t ReplyCode_U32, Start_U32;
	int32_t TimeoutInMs_S32;
	std::string Reply_S;
	BOF_EVENT NotifyEvent_X;;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = BOF_NAMESPACE::Bof_LockMutex(*mpsMtx_X);
		if (Rts_E == BOFERR_NO_ERROR)
		{
			Rts_E = BOFERR_ALREADY_OPENED;
			if (!IsConnected())
			{
				Rts_E = Bof_CreateEvent("ConEvnt_" + mBofSocketChannelParam_X.Name_S, false, 1, false, NotifyEvent_X);
				if (Rts_E == BOFERR_NO_ERROR)
				{
					TimeoutInMs_S32 = _TimeoutInMs_U32;
					Start_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
					Rts_E = mpsBofSocketChannelManager->SendAsyncConnectCmd(_TimeoutInMs_U32, nullptr, mpsSocketCmdChannel, &NotifyEvent_X);
					if (Rts_E == BOFERR_NO_ERROR)
					{
						Rts_E = BofSocketCmdChannel::S_ReEvaluateTimeout(Start_U32, TimeoutInMs_S32);
						if (Rts_E == BOFERR_NO_ERROR)
						{
							Rts_E = BOF_NAMESPACE::Bof_WaitForEvent(NotifyEvent_X, _TimeoutInMs_U32, 0);
							if (Rts_E == BOFERR_NO_ERROR)
							{
								mConnectedToIp_S = _rConnectToIpAddress_S;  //For next command
								Rts_E = WaitForCommandReply(TimeoutInMs_S32, 220, ReplyCode_U32, Reply_S);
							}
						}
					}
					Bof_DestroyEvent(NotifyEvent_X);
				}
				if (Rts_E != BOFERR_NO_ERROR)
				{
					mConnectedToIp_S = "";
				}
			}
			BOF_NAMESPACE::Bof_UnlockMutex(*mpsMtx_X);
		}
	}
	return Rts_E;
}

bool BofSocketCmdChannel::IsConnected()
{
	return (mConnectedToIp_S != "");
}

bool BofSocketCmdChannel::IsLogin()
{
	return mIsLogin_B;
}

std::string BofSocketCmdChannel::ConnectedToIpAddress()
{
	return mConnectedToIp_S;
}

BOFERR BofSocketCmdChannel::Disconnect(uint32_t _TimeoutInMs_U32)
{
	BOFERR Rts_E = mErrorCode_E;
	std::string Reply_S;
	BOF_EVENT NotifyEvent_X;
	int32_t TimeoutInMs_S32;
	uint32_t Start_U32;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = BOF_NAMESPACE::Bof_LockMutex(*mpsMtx_X);
		if (Rts_E == BOFERR_NO_ERROR)
		{
			Rts_E = BOFERR_NOT_OPENED;
			if (IsConnected())
			{
				Rts_E = Bof_CreateEvent("DisEvnt_" + mBofSocketChannelParam_X.Name_S, false, 1, false, NotifyEvent_X);
				if (Rts_E == BOFERR_NO_ERROR)
				{
					TimeoutInMs_S32 = _TimeoutInMs_U32;
					Start_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
					Rts_E = mpsBofSocketChannelManager->SendAsyncDisconnectCmd(_TimeoutInMs_U32, nullptr, mpsSocketCmdChannel, false, &NotifyEvent_X);
					if (Rts_E == BOFERR_NO_ERROR)
					{
						Rts_E = BofSocketCmdChannel::S_ReEvaluateTimeout(Start_U32, TimeoutInMs_S32);
						if (Rts_E == BOFERR_NO_ERROR)
						{
							Rts_E = BOF_NAMESPACE::Bof_WaitForEvent(NotifyEvent_X, _TimeoutInMs_U32, 0);
							if (Rts_E == BOFERR_NO_ERROR)
							{
								mIsLogin_B = false;
								mConnectedToIp_S = "";
							}
						}
					}
					Bof_DestroyEvent(NotifyEvent_X);
				}
			}
			BOF_NAMESPACE::Bof_UnlockMutex(*mpsMtx_X);
		}
	}
	return Rts_E;
}

BOFERR BofSocketCmdChannel::Login(uint32_t _TimeoutInMs_U32, const std::string &_rUser_S, const std::string &_rPassword_S)
{
	uint32_t ReplyCode_U32;
	std::string Reply_S;
	BOFERR Rts_E = mErrorCode_E;
	uint32_t Start_U32;
	int32_t TimeoutInMs_S32;

	if (Rts_E == BOFERR_NO_ERROR)
	{
//No lock is in SendCommandAndWaitForReply		Rts_E = BOF_NAMESPACE::Bof_LockMutex(mpsMtx_X); 
		if (Rts_E == BOFERR_NO_ERROR)
		{
			Rts_E = BOFERR_NOT_OPENED;
			if (IsConnected())
			{
				TimeoutInMs_S32 = _TimeoutInMs_U32;
				Start_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
				Rts_E = SendCommandAndWaitForReply(TimeoutInMs_S32, Bof_Sprintf("USER %s\r\n", _rUser_S.c_str()), 331, ReplyCode_U32, Reply_S);
				if (Rts_E == BOFERR_NO_ERROR)
				{
					Rts_E = BofSocketCmdChannel::S_ReEvaluateTimeout(Start_U32, TimeoutInMs_S32);
					if (Rts_E == BOFERR_NO_ERROR)
					{
						Rts_E = SendCommandAndWaitForReply(TimeoutInMs_S32, Bof_Sprintf("PASS %s\r\n", _rPassword_S.c_str()), 230, ReplyCode_U32, Reply_S);
						if (Rts_E == BOFERR_NO_ERROR)
						{
							Rts_E = BofSocketCmdChannel::S_ReEvaluateTimeout(Start_U32, TimeoutInMs_S32);
							if (Rts_E == BOFERR_NO_ERROR)
							{
								Rts_E = SendCommandAndWaitForReply(TimeoutInMs_S32, "TYPE I\r\n", 200, ReplyCode_U32, Reply_S); //Binary data
								if (Rts_E == BOFERR_NO_ERROR)
								{
									mIsLogin_B = true;
								}
							}
						}
					}
				}
			}
			//No lock is in SendCommandAndWaitForReply			BOF_NAMESPACE::Bof_UnlockMutex(mpsMtx_X);
		}
	}
	return Rts_E;
}

BOFERR BofSocketCmdChannel::SendCommand(uint32_t _TimeoutInMs_U32, const std::string &_rCommand_S)
{
	BOFERR Rts_E = mErrorCode_E;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		//No lock is in SendCommandAndWaitForReply		Rts_E = BOF_NAMESPACE::Bof_LockMutex(mpsMtx_X);
		if (Rts_E == BOFERR_NO_ERROR)
		{
			Rts_E = BOFERR_NOT_OPENED;
			if (IsConnected())
			{
				Rts_E = mpsSocketCmdChannel->WriteData(_TimeoutInMs_U32, true, _rCommand_S);
				if (Rts_E == BOFERR_NO_ERROR)
				{
				}
			}
			//No lock is in SendCommandAndWaitForReply			BOF_NAMESPACE::Bof_UnlockMutex(mpsMtx_X);
		}
	}
	return Rts_E;
}

BOFERR BofSocketCmdChannel::WaitForCommandReply(uint32_t _TimeoutInMs_U32, uint32_t _ExpectedReplyCode_U32, uint32_t &_rReplyCode_U32, std::string &_rReply_S)
{
	uint32_t Nb_U32, Start_U32;
	int32_t TimeoutInMs_S32;
	uint8_t pBuffer_U8[0x4000];
	BOFERR Rts_E = mErrorCode_E;
	bool Finish_B, FullReply_B;

	_rReplyCode_U32 = 0;
	_rReply_S = "";
	if (Rts_E == BOFERR_NO_ERROR)
	{
		//No lock is in SendCommandAndWaitForReply		Rts_E = BOF_NAMESPACE::Bof_LockMutex(mpsMtx_X);
		if (Rts_E == BOFERR_NO_ERROR)
		{
			Rts_E = BOFERR_NOT_OPENED;
			if (IsConnected())
			{
				TimeoutInMs_S32 = _TimeoutInMs_U32;
				FullReply_B = false;
				do
				{
					Finish_B = true;
					Start_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
					Nb_U32 = sizeof(pBuffer_U8);
////TODO					Rts_E = mpsSocketCmdChannel->V_ReadData(TimeoutInMs_S32, Nb_U32, pBuffer_U8);
					if (Rts_E == BOFERR_NO_ERROR)
					{
						Rts_E = ParseReply(Nb_U32, pBuffer_U8, FullReply_B, _rReplyCode_U32, _rReply_S);
						if (Rts_E == BOFERR_NO_ERROR)
						{
							if (_rReplyCode_U32 != _ExpectedReplyCode_U32)
							{
								Rts_E = BOFERR_INVALID_VALUE;
							}
						}
						else
						{
							Finish_B = false;
						}
						if (!Finish_B)
						{
							if (BofSocketCmdChannel::S_ReEvaluateTimeout(Start_U32, TimeoutInMs_S32) != BOFERR_NO_ERROR)
							{
								Rts_E = (Rts_E == BOFERR_NO_ERROR) ? BOFERR_TIMEOUT : Rts_E;
								Finish_B = true;
							}
						}
					} //Read if (Rts_E == BOFERR_NO_ERROR)
				} while (!Finish_B);
			}
			//No lock is in SendCommandAndWaitForReply			BOF_NAMESPACE::Bof_UnlockMutex(mpsMtx_X);
		}
	}
	return Rts_E;
}

BOFERR BofSocketCmdChannel::SendCommandAndWaitForReply(uint32_t _TimeoutInMs_U32, const std::string &_rCommand_S, uint32_t _ExpectedReplyCode_U32, uint32_t &_rReplyCode_U32, std::string &_rReply_S)
{
	uint32_t Start_U32;
	int32_t RemainingTimeout_S32;
	BOFERR Rts_E = mErrorCode_E;

	_rReplyCode_U32 = 0;
	_rReply_S = "";
	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = BOF_NAMESPACE::Bof_LockMutex(*mpsMtx_X);
		if (Rts_E == BOFERR_NO_ERROR)
		{
			Rts_E = BOFERR_NOT_OPENED;
			if (IsConnected())
			{
				RemainingTimeout_S32 = _TimeoutInMs_U32;
				Start_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
				Rts_E = SendCommand(_TimeoutInMs_U32, _rCommand_S);
				if (Rts_E == BOFERR_NO_ERROR)
				{
					Rts_E = BOFERR_TIMEOUT;
					RemainingTimeout_S32 -= BOF_NAMESPACE::Bof_ElapsedMsTime(Start_U32);
					if (RemainingTimeout_S32 > 0)
					{
						Rts_E = WaitForCommandReply(RemainingTimeout_S32, _ExpectedReplyCode_U32, _rReplyCode_U32, _rReply_S);
					}
				}
			}
			BOF_NAMESPACE::Bof_UnlockMutex(*mpsMtx_X);
		}
	}
	return Rts_E;
}

BOFERR BofSocketCmdChannel::ParseReply(uint32_t _Nb_U32, const uint8_t *_pBuffer_U8, bool &_rFullReply_B, uint32_t &_rReplyCode_U32, std::string &_rFtpMessage_S)
{
	BOFERR Rts_E;
	uint32_t Total_U32, ReplyCode_U32;
// char pReply_c[] = "220-FileZilla Server 0.9.60 beta\r\n220-written by Tim Kosse(tim.kosse@filezilla-project.org)\r\n220 Please visit https ://filezilla-project.org/\r\n";
// Size_U32 = strlen(pReply_c);
	static std::regex S_RegExFtpReply("^(\\d\\d\\d)([ -])(.*)([\\r\\n]*)");
	std::cmatch FtpReplyMatch;
	bool Finish_B = true;
	const char *pBuffer_c = reinterpret_cast<const char *>(_pBuffer_U8);

	_rFullReply_B = false;
	_rReplyCode_U32 = 0xFFFFFFFF;
	_rFtpMessage_S = "";
	Total_U32 = 0;
	do
	{
		Rts_E = BOFERR_INVALID_FORMAT;
		if (std::regex_search(pBuffer_c, FtpReplyMatch, S_RegExFtpReply))
		{
			if (FtpReplyMatch.size() == 5)
			{
				if ((FtpReplyMatch[1].length() == 3) && (FtpReplyMatch[2].length() == 1) && (FtpReplyMatch[4].length() == 2))
				{
					ReplyCode_U32 = static_cast<uint32_t>(std::stoi(FtpReplyMatch[1].str()));
					_rFullReply_B = FtpReplyMatch[2].str() == " ";
					_rFtpMessage_S = FtpReplyMatch[3].str();
					BOF_ASSERT(FtpReplyMatch[4].str() == "\r\n");
					if (FtpReplyMatch[4].str() == "\r\n")
					{
						if (mLastPartialReplyCode_U32 != 0xFFFFFFFF)
						{
							BOF_ASSERT(ReplyCode_U32 == mLastPartialReplyCode_U32);
							mLastPartialReplyCode_U32 = ReplyCode_U32;
						}

						if (_rFullReply_B)
						{
							mLastPartialReplyCode_U32 = 0xFFFFFFFF;
						}
						_rReplyCode_U32 = ReplyCode_U32;

						Total_U32 += static_cast<uint32_t>(FtpReplyMatch[0].length());
						pBuffer_c += static_cast<uint32_t>(FtpReplyMatch[0].length());
						Rts_E = BOFERR_NO_ERROR;
						Finish_B = (Total_U32 >= _Nb_U32);
					}
				}
			}
		}
	} while (!Finish_B);

	return Rts_E;
}

BOFERR BofSocketCmdChannel::OnConnectCallback(uint32_t /*_Id_U32*/, std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> /*_psBofSocketChannel*/, const BOF_SOCKET_PEER_INFO & /*_rPeerInfo_X*/)
{
	BOFERR Rts_E = BOFERR_NO_ERROR;
	return Rts_E;
}

BOFERR BofSocketCmdChannel::OnReadCallback(uint32_t /*_Id_U32*/, std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> /*_psBofSocketChannel*/, const BOF_BUFFER &/*_rBuffer_X*/,
                                           const BOF_SOCKET_PEER_INFO &/*_rPeerInfo_X*/)
{
	BOFERR Rts_E = BOFERR_NO_ERROR;
	return Rts_E;
}

BOFERR BofSocketCmdChannel::S_ReEvaluateTimeout(uint32_t _Start_U32, int32_t &_rNewTimeOut_S32)
{
	BOFERR Rts_E = BOFERR_NO_ERROR;
	uint32_t Delta_U32;

	Delta_U32 = BOF_NAMESPACE::Bof_ElapsedMsTime(_Start_U32);
	_rNewTimeOut_S32 -= Delta_U32;
	if (_rNewTimeOut_S32 <= 0)
	{
		Rts_E = BOFERR_TIMEOUT;
	}
	return Rts_E;
}
END_BOF_NAMESPACE()