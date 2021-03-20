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
#include <bofstd/bofiocmdchannel.h>

#include <bofstd/ibofioconnection.h>
#include <bofstd/bofstring.h>
#include <bofstd/bofstringformatter.h>

#include <regex>

BEGIN_BOF_NAMESPACE()
const uint32_t DATA_FRAGMENT_SIZE = 0x400000;

BofIoCmdChannel::BofIoCmdChannel(const IBOF_IO_CONNECTION_PARAM &_rIBofIoConnectionParam_X, BofIoConnectionManager *_pIoConnectionManager)
	: IBofIoConnection(_rIBofIoConnectionParam_X, _pIoConnectionManager), mpIoConnectionManager(_pIoConnectionManager), mpIoDataChannel(nullptr), mConnectTimeoutInMs_U32(1000),
	  mIoCmdTimeoutInMs_U32(3000), mIoDataTimeoutInMs_U32(7000), mpBofStringCircularBuffer(nullptr), mConnectedAddress_S(""), mListenForConnection_B(false)
{
	BOF_STRING_CIRCULAR_BUFFER_PARAM BofStringCircularBufferParam_X;

	BOF_ASSERT(mpIoConnectionManager != nullptr);
	BOF_CIRCULAR_BUFFER_PARAM BofCircularBufferParam_X;
	IBOF_IO_CONNECTION_PARAM DataIBofIoConnectionParam_X = _rIBofIoConnectionParam_X;
	DataIBofIoConnectionParam_X.Name_S = DataIBofIoConnectionParam_X.Name_S + "_Data";
	mDataConnectToAddress_X.Reset();
	mErrorCode_E = BOFERR_NOT_ENOUGH_RESOURCE;
	mpIoDataChannel = new BofIoDataChannel(*this, DataIBofIoConnectionParam_X, _pIoConnectionManager);
	BOF_ASSERT(mpIoDataChannel != nullptr);
	if (mpIoDataChannel)
	{
		mErrorCode_E = mpIoDataChannel->LastErrorCode();
		if (mErrorCode_E == BOFERR_NO_ERROR)
		{
			mErrorCode_E = BOFERR_NOT_ENOUGH_RESOURCE;

			BofStringCircularBufferParam_X.Reset();
			BofStringCircularBufferParam_X.MultiThreadAware_B = true;
			BofStringCircularBufferParam_X.BufferSizeInByte_U32 = 0x2000;
			BofStringCircularBufferParam_X.Overwrite_B = false;
			BofStringCircularBufferParam_X.pData_c = nullptr;
			BofStringCircularBufferParam_X.Blocking_B = true;

			mpBofStringCircularBuffer = new BofStringCircularBuffer(BofStringCircularBufferParam_X);
			if (mpBofStringCircularBuffer != nullptr)
			{
				mErrorCode_E = mpBofStringCircularBuffer->LastErrorCode();
			}
		}
	}
}

BofIoCmdChannel::~BofIoCmdChannel()
{
	Disconnect(DEFAULT_CMD_DATA_TIMEOUT);
	BOF_SAFE_DELETE(mpIoDataChannel);
	BOF_SAFE_DELETE(mpBofStringCircularBuffer);
	//Done in ~IBofIoConnection MarkConnectionAsDeleted();
}

std::string &BofIoCmdChannel::ConnectedAddress()
{
	return mConnectedAddress_S;
}

const BOF_SOCKET_ADDRESS &BofIoCmdChannel::DataConnectToAddress(bool &_rListenForConnection_B)
{
	_rListenForConnection_B = mListenForConnection_B;
	return mDataConnectToAddress_X;
}

uint32_t BofIoCmdChannel::ConnectTimeoutInMs()
{
	return mConnectTimeoutInMs_U32;
}

uint32_t BofIoCmdChannel::IoCmdTimeoutInMs()
{
	return mIoCmdTimeoutInMs_U32;
}

uint32_t BofIoCmdChannel::IoDataTimeoutInMs()
{
	return mIoDataTimeoutInMs_U32;
}

BOFERR BofIoCmdChannel::Connect(const BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X, std::string &_rWelcomeMsg_S)
{
	BOFERR Rts_E;
	uint32_t Start_U32;
	int32_t RemainingTimeout_S32;

	_rWelcomeMsg_S = "";
	RemainingTimeout_S32 = _rBofIoChannelParam_X.ConnectTimeoutInMs_U32 + 1000;
	Start_U32 = Bof_GetMsTickCount();
	Rts_E = IoConnectionManager()->Connect(this, _rBofIoChannelParam_X);
	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = BOFERR_TIMEOUT;
		RemainingTimeout_S32 -= Bof_ElapsedMsTime(Start_U32);
		if (RemainingTimeout_S32 > 0)
		{
			Start_U32 = Bof_GetMsTickCount();
			Rts_E = WaitForConnect(RemainingTimeout_S32);

			if (Rts_E == BOFERR_NO_ERROR)
			{
				Rts_E = BOFERR_TIMEOUT;
				RemainingTimeout_S32 -= Bof_ElapsedMsTime(Start_U32);
				if (RemainingTimeout_S32 > 0)
				{
					Rts_E = WaitForEndOfCommand(RemainingTimeout_S32, _rWelcomeMsg_S);
				}
			}
		}
		if (Rts_E == BOFERR_NO_ERROR)
		{
			mConnectedAddress_S = _rBofIoChannelParam_X.Address_S;
		}
		else
		{
			IoConnectionManager()->PushDisconnect("IoCmdChannel::Connect", UvConnection(), true);
		}
	}
	return Rts_E;
}

BOFERR BofIoCmdChannel::Disconnect(uint32_t _TimeoutInMs_U32)
{
	BOFERR Rts_E;

	Rts_E = CloseDataChannel(_TimeoutInMs_U32);
	BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);

	Rts_E = IoConnectionManager()->PushDisconnect("IoCmdChannel::Disconnect", UvConnection(), false);
	if (Rts_E == BOFERR_NO_ERROR)
	{
		mConnectedAddress_S = "";
	}
	return Rts_E;
}

BOFERR BofIoCmdChannel::Login(uint32_t _TimeoutInMs_U32, const std::string &_rUser_S, const std::string &_rPassword_S)
{
	BOFERR Rts_E;
	uint32_t ReplyCode_U32;
	std::string Reply_S;

	Rts_E = SendIoCmdAndWaitForReply(_TimeoutInMs_U32, Bof_Sprintf("USER %s\r\n", _rUser_S.c_str()), 331, Reply_S, ReplyCode_U32);
	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = SendIoCmdAndWaitForReply(_TimeoutInMs_U32, Bof_Sprintf("PASS %s\r\n", _rPassword_S.c_str()), 230, Reply_S, ReplyCode_U32);
		if (Rts_E == BOFERR_NO_ERROR)
		{
			Rts_E = SendIoCmdAndWaitForReply(_TimeoutInMs_U32, "TYPE I\r\n", 200, Reply_S, ReplyCode_U32);
		}
	}
	return Rts_E;
}

BOFERR BofIoCmdChannel::SendIoCmd(uint32_t _TimeoutInMs_U32, const std::string &_rCmd_S)
{
	BOFERR Rts_E = IsConnected() ? BOFERR_NO_ERROR : BOFERR_NOT_OPENED;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = WriteData(_TimeoutInMs_U32, _rCmd_S, nullptr, true, false, false);
	}
	return Rts_E;
}

BOFERR BofIoCmdChannel::WaitForIoCmdReply(uint32_t _TimeoutInMs_U32, uint32_t _ExpectedReplyCode_U32, std::string &_rReply_S, uint32_t &_rReplyCode_U32)
{
//After QUIT cmd IsConnected is false  BOFERR Rts_E = IsConnected() ? BOFERR_NO_ERROR : BOFERR_NOT_OPENED;
	BOFERR Rts_E = BOFERR_NO_ERROR;

	_rReplyCode_U32 = 0;
	_rReply_S = "";
	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = WaitForEndOfCommand(_TimeoutInMs_U32, _rReply_S);
		if (Rts_E == BOFERR_NO_ERROR)
		{
			_rReplyCode_U32 = BofIoCmdChannel::S_ReplyCodeFromString(_rReply_S);
			if (_rReply_S.size() >= 4)
			{
				_rReply_S = _rReply_S.substr(4);
				if (_rReplyCode_U32 != _ExpectedReplyCode_U32)
				{
					Rts_E = BOFERR_INVALID_VALUE;
				}
			}
			else
			{
				Rts_E = BOFERR_INVALID_ANSWER;
			}
		}
	}
	return Rts_E;
}

BOFERR BofIoCmdChannel::SendIoCmdAndWaitForReply(uint32_t _TimeoutInMs_U32, const std::string &_rCmd_S, uint32_t _ExpectedReplyCode_U32, std::string &_rReply_S, uint32_t &_rReplyCode_U32)
{
	BOFERR Rts_E = IsConnected() ? BOFERR_NO_ERROR : BOFERR_NOT_OPENED;
	uint32_t Start_U32;
	int32_t RemainingTimeout_S32;

	_rReplyCode_U32 = 0;
	_rReply_S = "";
	if (Rts_E == BOFERR_NO_ERROR)
	{
		RemainingTimeout_S32 = _TimeoutInMs_U32;
		Start_U32 = Bof_GetMsTickCount();
		Rts_E = WriteData(_TimeoutInMs_U32, _rCmd_S, nullptr, true, false, false);
		if (Rts_E == BOFERR_NO_ERROR)
		{
			Rts_E = BOFERR_TIMEOUT;
			RemainingTimeout_S32 -= Bof_ElapsedMsTime(Start_U32);
			if (RemainingTimeout_S32 > 0)
			{
				Rts_E = WaitForIoCmdReply(RemainingTimeout_S32, _ExpectedReplyCode_U32, _rReply_S, _rReplyCode_U32);
			}
		}
	}
	return Rts_E;
}

BOFERR BofIoCmdChannel::SendIoCmdToCreateDataChannel(uint32_t _TimeoutInMs_U32, uint16_t _ActivePort_U16, const BOF_BUFFER &_rNotifyBuffer_X)
{
	BOFERR Rts_E = BOFERR_RUNNING;
	uint32_t ReplyCode_U32;
	std::string Reply_S;
	BOF_IO_CHANNEL_PARAM IoChannelParam_X;
	IBOF_IO_CONNECTION_PARAM IBofIoConnectionParam_X;
	uint32_t Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, PortHigh_U32, PortLow_U32;
	uint16_t Port_U16;

	BOF_ASSERT(mpIoConnectionManager != nullptr);
	BOF_ASSERT(mpIoDataChannel != nullptr);
	if (_ActivePort_U16)
	{
		mListenForConnection_B = false;
		mDataConnectToAddress_X = LocalAddress();
		mDataConnectToAddress_X.Set(mDataConnectToAddress_X.IpV6_B, _ActivePort_U16);
		BOF_U32IPADDR_TO_U8IPADDR(mDataConnectToAddress_X.IpV4Address_X.sin_addr.s_addr, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32);
		Port_U16 = _ActivePort_U16;
		PortHigh_U32 = static_cast<uint8_t>(Port_U16 >> 8);
		PortLow_U32 = static_cast<uint8_t>(Port_U16);

		Rts_E = SendIoCmdAndWaitForReply(_TimeoutInMs_U32, Bof_Sprintf("PORT %d,%d,%d,%d,%d,%d\r\n", Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, PortHigh_U32, PortLow_U32), 200, Reply_S, ReplyCode_U32);
		if (Rts_E == BOFERR_NO_ERROR)
		{
		}
	}
	else
	{
		Rts_E = SendIoCmdAndWaitForReply(_TimeoutInMs_U32, Bof_Sprintf("PASV\r\n"), 227, Reply_S, ReplyCode_U32);
		if (Rts_E == BOFERR_NO_ERROR)
		{
			static std::regex S_RegExPasv("^.*?(\\d+),(\\d+),(\\d+),(\\d+),(\\d+),(\\d+).*$");
			std::cmatch PasvMatch;
			Rts_E = BOFERR_INVALID_ANSWER;
			/*
			*Online regex builder/test: https://regex101.com/
			If [the regex search is] successful, it is not empty and contains a series of sub_match objects:
			the first sub_match element corresponds to the entire match, and, if the regex expression contained sub-expressions
			to be matched (i.e., parentheses-delimited groups), their corresponding sub-matches are stored as successive sub_match elements
			in the match_results object.

			whatever whatever something abc something abc
			by default, regexes are greedy, meaning it will match as much as possible. Therefore /^.*abc/ would match "whatever whatever something abc something ".
			Adding the non-greedy quantifier ? makes the regex only match "whatever whatever something ".
			*/
			//Match means all the string-> use ^ for begin and $ for end
			if ((std::regex_match(Reply_S.c_str(), PasvMatch, S_RegExPasv))
			    && (PasvMatch.size() == 1 + 6))
			{
				Rts_E = BOFERR_NO_ERROR;
				Ip1_U32 = static_cast<uint32_t>(std::stoi(PasvMatch[1].str()));
				Ip2_U32 = static_cast<uint32_t>(std::stoi(PasvMatch[2].str()));
				Ip3_U32 = static_cast<uint32_t>(std::stoi(PasvMatch[3].str()));
				Ip4_U32 = static_cast<uint32_t>(std::stoi(PasvMatch[4].str()));
				PortHigh_U32 = static_cast<uint8_t>(std::stoi(PasvMatch[5].str()));
				PortLow_U32 = static_cast<uint8_t>(std::stoi(PasvMatch[6].str()));
				Port_U16 = static_cast<uint16_t>((PortHigh_U32 << 8) + PortLow_U32);
				mListenForConnection_B = true;
				mDataConnectToAddress_X.Set(false, BOF_SOCK_TYPE::BOF_SOCK_TCP, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, Port_U16);
			}
		}
	}
	if (Rts_E == BOFERR_NO_ERROR)
	{
//Make this on next io cmd 'list/stor/retr)    Rts_E = mpIoDataChannel->OpenDataChannel(_TimeoutInMs_U32, mListenForConnection_B, mDataConnectToAddress_X, 0x100000, 0x100000);
		if (Rts_E == BOFERR_NO_ERROR)
		{
		}
	}
	return Rts_E;
}

BOFERR BofIoCmdChannel::OpenDataChannel(uint32_t _TimeoutInMs_U32, bool _ListenForConnection_B, BOF_SOCKET_ADDRESS &_rConnectToAddress_X, uint32_t _RcvBufferSize_U32, uint32_t _SndBufferSize_U32)
{
	BOFERR Rts_E = BOFERR_NOT_RUNNING;
	BOF_ASSERT(mpIoDataChannel != nullptr);

	Rts_E = mpIoDataChannel->OpenDataChannel(_TimeoutInMs_U32, _ListenForConnection_B, _rConnectToAddress_X, _RcvBufferSize_U32, _SndBufferSize_U32);
	return Rts_E;
}

BOFERR BofIoCmdChannel::CloseDataChannel(uint32_t _TimeoutInMs_U32)
{
	BOFERR Rts_E = BOFERR_NOT_RUNNING;
	BOF_ASSERT(mpIoDataChannel != nullptr);

	Rts_E = mpIoDataChannel->CloseDataChannel(_TimeoutInMs_U32);
	return Rts_E;
}

uint32_t BofIoCmdChannel::S_ReplyCodeFromString(const std::string &_rReply_S)
{
	uint32_t Rts_U32 = 0;
	if (_rReply_S.size() >= 4)
	{
		try
		{
			Rts_U32 = std::stoi(_rReply_S.substr(0, 3));
		}
		catch (const std::exception &)
		{
			//  e.what()
			Rts_U32 = 0;
		}
	}
	return Rts_U32;
}

BOFERR BofIoCmdChannel::WaitForEndOfCommand(uint32_t _TimeoutInMs_U32, std::string &_rReply_S)
{
	BOFERR Rts_E;
	uint32_t Nb_U32;
	char pData_c[0x1000];

	Nb_U32 = sizeof(pData_c);
	Rts_E = mpBofStringCircularBuffer->PopString(&Nb_U32, pData_c, _TimeoutInMs_U32);
	if (Rts_E == BOFERR_NO_ERROR)
	{
		BOF_ASSERT(Nb_U32 >= 4);  //226    \r\n has been remove in push op
		_rReply_S = std::string(pData_c, pData_c + Nb_U32);
	}
	return Rts_E;
}

/*
BOFERR BofIoCmdChannel::WaitForEndOfTransfert(uint32_t _TimeoutInMs_U32, std::string &_rReply_S)
{
  BOFERR Rts_E;

  //    BOF_ASSERT(mRemainingIoSize_S64 != 0);
  _rReply_S = "";
  Rts_E = WaitForEndOfIo(_TimeoutInMs_U32);
  if (Rts_E == BOFERR_NO_ERROR)
  {
  }
  return Rts_E;
}
*/
BOFERR BofIoCmdChannel::V_DataRead(uint32_t _Nb_U32, uint8_t *_pBuffer_U8)
{
	BOFERR Rts_E = BOFERR_NO_ERROR;
	uint32_t Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, PortHigh_U32, PortLow_U32;
	uint16_t Port_U16;
	//https://regex101.com/
	//^.*?(\d+),(\d+),(\d+),(\d+),(\d+),(\d+).*$
	//Port command successful (12,34,56,78,90,13).
	static std::regex S_RegExPasvPort("^.*?(\\d+),(\\d+),(\\d+),(\\d+),(\\d+),(\\d+).*$");
	std::cmatch PasvPortMatch;
	BOF_IO_CHANNEL_PARAM IoChannelParam_X;
	IBOF_IO_CONNECTION_PARAM IBofIoConnectionParam_X;
	BOF_IO_DELEGATE_CMD IoDelegateCmd_X;

	BOF_ASSERT(_Nb_U32 >= 2);
	BOF_ASSERT(_pBuffer_U8[_Nb_U32 - 2] == '\r');
	BOF_ASSERT(_pBuffer_U8[_Nb_U32 - 1] == '\n');
	std::string Cmd_S(_pBuffer_U8, _pBuffer_U8 + _Nb_U32 - 2);
	std::string Reply_S;

//We can stay here for low time as we are in the libuv callback context
	//ReplyCode_U32 = IoCmdChannel::S_ReplyCodeFromString(Cmd_S);
	//if (ReplyCode_U32==0) //It is a command
	if (ServerSession())
	{
		std::vector<std::string> CmdArgCollection = Bof_StringSplit(Cmd_S, " ");

		if (CmdArgCollection[0] == "USER")
		{
			Reply_S = Bof_Sprintf("331 User name okay, need password.\r\n");
		}
		else if (CmdArgCollection[0] == "PASS")
		{
			Reply_S = Bof_Sprintf("230 User logged in, proceed.\r\n");
		}
		else if (CmdArgCollection[0] == "TYPE")
		{
			Reply_S = Bof_Sprintf("200 Type set to \"%s\"\r\n", CmdArgCollection[1].c_str());
		}
		else if (CmdArgCollection[0] == "CWD")
		{
			Reply_S = Bof_Sprintf("200 Change to '%s'\n", CmdArgCollection[1].c_str());
		}
		else if (CmdArgCollection[0] == "PWD")
		{
			Reply_S = Bof_Sprintf("257 Remote directory now \"%s\"\r\n", "/tmp");
		}
		else if (CmdArgCollection[0] == "PORT")
		{
			Rts_E = BOFERR_INVALID_ANSWER;
			if ((std::regex_match(CmdArgCollection[1].c_str(), PasvPortMatch, S_RegExPasvPort))
			    && (PasvPortMatch.size() == 1 + 6))
			{
				Rts_E = BOFERR_NO_ERROR;
				Ip1_U32 = static_cast<uint32_t>(std::stoi(PasvPortMatch[1].str()));
				Ip2_U32 = static_cast<uint32_t>(std::stoi(PasvPortMatch[2].str()));
				Ip3_U32 = static_cast<uint32_t>(std::stoi(PasvPortMatch[3].str()));
				Ip4_U32 = static_cast<uint32_t>(std::stoi(PasvPortMatch[4].str()));
				PortHigh_U32 = static_cast<uint8_t>(std::stoi(PasvPortMatch[5].str()));
				PortLow_U32 = static_cast<uint8_t>(std::stoi(PasvPortMatch[6].str()));
				Port_U16 = static_cast<uint16_t>((PortHigh_U32 << 8) + PortLow_U32);
				mListenForConnection_B = true;
				mDataConnectToAddress_X.Set(false, BOF_SOCK_TYPE::BOF_SOCK_TCP, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, Port_U16);

				Reply_S = Bof_Sprintf("200 Port command successful (%d: %d.%d.%d.%d).\r\n", Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, PortHigh_U32, PortLow_U32);

				BOF_ASSERT(mpIoDataChannel != nullptr);
				IoDelegateCmd_X.IoCmd_E = BOF_IO_CMD::IO_CMD_OPEN;
				IoDelegateCmd_X.ListenForConnection_B = mListenForConnection_B;
				Rts_E = mpIoDataChannel->PushIoDelegateCmd(IoDataTimeoutInMs(), IoDelegateCmd_X);
				BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);

			}
		}
		else if (CmdArgCollection[0] == "PASV")
		{
			Rts_E = BOFERR_NO_ERROR;
			Port_U16 = static_cast<uint16_t>(Bof_Random(false, 49100, 49200));
			PortHigh_U32 = static_cast<uint8_t>(Port_U16 >> 8);
			PortLow_U32 = static_cast<uint8_t>(Port_U16);
			mListenForConnection_B = false;
			mDataConnectToAddress_X = LocalAddress();
			mDataConnectToAddress_X.Set(mDataConnectToAddress_X.IpV6_B, Port_U16);
			BOF_U32IPADDR_TO_U8IPADDR(mDataConnectToAddress_X.IpV4Address_X.sin_addr.s_addr, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32);
			mDataConnectToAddress_X.Set(false, BOF_SOCK_TYPE::BOF_SOCK_TCP, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, Port_U16);

			Reply_S = Bof_Sprintf("227 Entering Passive Mode (%d,%d,%d,%d,%d,%d).\r\n", Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, PortHigh_U32, PortLow_U32);

			BOF_ASSERT(mpIoDataChannel != nullptr);
			IoDelegateCmd_X.IoCmd_E = BOF_IO_CMD::IO_CMD_OPEN;
			IoDelegateCmd_X.ListenForConnection_B = mListenForConnection_B;
			Rts_E = mpIoDataChannel->PushIoDelegateCmd(IoDataTimeoutInMs(), IoDelegateCmd_X);
			BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);
		}
		else if (CmdArgCollection[0] == "QUIT")
		{
			BOF_ASSERT(mpIoDataChannel != nullptr);
			IoDelegateCmd_X.IoCmd_E = BOF_IO_CMD::IO_CMD_CLOSE;
			Rts_E = mpIoDataChannel->PushIoDelegateCmd(IoDataTimeoutInMs(), IoDelegateCmd_X);
			BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);

		}
		else if (CmdArgCollection[0] == "ABOR")
		{
			BOF_ASSERT(mpIoDataChannel != nullptr);
			IoDelegateCmd_X.IoCmd_E = BOF_IO_CMD::IO_CMD_ABORT;
			Rts_E = mpIoDataChannel->PushIoDelegateCmd(IoDataTimeoutInMs(), IoDelegateCmd_X);
			BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);

		}
		else if (CmdArgCollection[0] == "LIST")
		{
			BOF_ASSERT(mpIoDataChannel != nullptr);
			IoDelegateCmd_X.IoCmd_E = BOF_IO_CMD::IO_CMD_LIST;
			Rts_E = mpIoDataChannel->PushIoDelegateCmd(IoDataTimeoutInMs(), IoDelegateCmd_X);
			BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);

		}
		else if (CmdArgCollection[0] == "STOR")
		{
			BOF_ASSERT(mpIoDataChannel != nullptr);
			IoDelegateCmd_X.IoCmd_E = BOF_IO_CMD::IO_CMD_STOR;
			Rts_E = mpIoDataChannel->PushIoDelegateCmd(IoDataTimeoutInMs(), IoDelegateCmd_X);
			BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);

		}
		else if (CmdArgCollection[0] == "RETR")
		{
			BOF_ASSERT(mpIoDataChannel != nullptr);
			IoDelegateCmd_X.IoCmd_E = BOF_IO_CMD::IO_CMD_RETR;
			Rts_E = mpIoDataChannel->PushIoDelegateCmd(IoDataTimeoutInMs(), IoDelegateCmd_X);
			BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);

		}

		if (Reply_S != "")
		{
			Rts_E = WriteData(IoCmdTimeoutInMs(), Reply_S, nullptr, true, false, true);
			if (Rts_E == BOFERR_NO_ERROR)
			{
			}
		}
	}
	else
	{
		BOF_ASSERT(_Nb_U32 >= 6);  //226 \r\n
		BOF_ASSERT(_pBuffer_U8[_Nb_U32 - 2] == '\r');
		BOF_ASSERT(_pBuffer_U8[_Nb_U32 - 1] == '\n');
		Rts_E = mpBofStringCircularBuffer->PushBinary(_Nb_U32 - 2, reinterpret_cast<const char *>(_pBuffer_U8), IoCmdTimeoutInMs());
	}
	return Rts_E;
}

BOFERR BofIoCmdChannel::V_DataWritten(BOFERR _Sts_E, void *_pUserArg)
{
	//LOGGER_INFORMATION(DBG_INIT, 0, "V_DataWritten Sts %s UserArg %p",Bof_ErrorCode(_Sts_E), _pUserArg);
	return BOFERR_NO_ERROR;
}

BOFERR BofIoCmdChannel::V_RemoteIoClosed()
{
	// LOGGER_INFORMATION(DBG_INIT, 0, "V_RemoteIoClosed");
	return BOFERR_NO_ERROR;
}

BOFERR BofIoCmdChannel::V_Connected()
{
	//printf("V_Connected %s\n",Name().c_str());
	BOFERR Rts_E;
	Rts_E = ServerSession() ? WriteData(IoCmdTimeoutInMs(), "220 EVS FTP Server (v) 09.08 (mc) 1606 (d) 26/05/64 (a) B.Harmel [B: 2/8 MB L:127.0.0.1:xxxx R:127.0.0.1:yyyy]\r\n", nullptr, true, false,
	                                    true) : BOFERR_NO_ERROR;
	return Rts_E;
}

BOFERR BofIoCmdChannel::V_ConnectFailed(BOFERR _Sts_E)
{
	// LOGGER_INFORMATION(DBG_INIT, 0, "V_ConnectFailed Sts %s", Bof_ErrorCode(_Sts_E));
	return BOFERR_NO_ERROR;
}

BOFERR BofIoCmdChannel::V_Disconnected()
{
	BOFERR Rts_E;

	Rts_E = Disconnect(DEFAULT_CMD_DATA_TIMEOUT);
	return Rts_E;
}
END_BOF_NAMESPACE()


