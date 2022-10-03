/*
* Copyright (c) 2015-2025, Onbings. All rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
* PURPOSE.
*
* This module implements the BofSocketServer class.
*
* Author:      Bernard HARMEL: onbings@dscloud.me
* Web:			    onbings.dscloud.me
* Revision:    1.0
*
* Rem:         None
*
* History:
*
* V 1.00  Jan 05 2019  BHA : Initial release
*/

/*** Include files ***********************************************************/

#include <bofstd/bofsocketserver.h>
#include <regex>
#include <algorithm>

BEGIN_BOF_NAMESPACE()

BofSocketServer::BofSocketServer(IBofSocketSessionFactory *_pIBofSocketSessionFactory,const BOF_SOCKET_SERVER_PARAM & _rBofSocketServerParam_X)
								: BofSocketSessionManager(_pIBofSocketSessionFactory, this,  _rBofSocketServerParam_X)
{
	BOF_SOCKET_PARAM BofSocketParam_X;
	BOF_SOCKET_IO_PARAM BofSocketIoParam_X;
	uint32_t i_U32;
	BOF_SOCK_TYPE SocketType_E;
	uint32_t Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32;
	uint16_t Port_U16;
	BOFSOCKET				ListenSessionId;
	std::unique_ptr<BofSocket> puListenSocket;
	std::shared_ptr<BofSocketIo> psListenSocketSession;

  mErrorCode_E = BofSocketSessionManager::LastErrorCode();
	//unit test BOF_ASSERT(mErrorCode_E == BOF_ERR_NO_ERROR);
	if (mErrorCode_E == BOF_ERR_NO_ERROR)
	{
		if (mBofSocketServerParam_X.MinPortValue_U16 > mBofSocketServerParam_X.MaxPortValue_U16)
		{
			std::swap(mBofSocketServerParam_X.MinPortValue_U16, mBofSocketServerParam_X.MinPortValue_U16);
		}
		mDynamicPort_U16  = mBofSocketServerParam_X.MinPortValue_U16;
		mErrorCode_E = BOF_ERR_EINVAL;
		BOF_ASSERT(mpIBofSocketSessionFactory);
		BOF_ASSERT(mBofSocketServerParam_X.NbMaxSession_U32);
	//	BOF_ASSERT(mBofSocketServerParam_X.NoIoCloseTimeoutInMs_U32);
		if ((mpIBofSocketSessionFactory) && (mBofSocketServerParam_X.NbMaxSession_U32)) // && (mBofSocketServerParam_X.NoIoCloseTimeoutInMs_U32))
		{
			mErrorCode_E = BOF_ERR_ENOMEM;
			mpActiveSession_B = new bool[mBofSocketServerParam_X.NbMaxSession_U32];
			if (mpActiveSession_B)
			{
				for (i_U32 = 0; i_U32 < mBofSocketServerParam_X.NbMaxSession_U32; i_U32++)
				{
					mpActiveSession_B[i_U32] = false;
				}

				mErrorCode_E = Bof_SplitIpAddress(mBofSocketServerParam_X.Address_S, mSocketServerAddress_X);
				if (mErrorCode_E == BOF_ERR_NO_ERROR)
				{
					mSocketServerAddress_X.Parse(SocketType_E, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, Port_U16);
					mErrorCode_E = BOF_ERR_WRONG_MODE;
					if (SocketType_E == BOF_SOCK_TYPE::BOF_SOCK_TCP)
					{
						if (Port_U16)
						{
							mErrorCode_E = BOF_ERR_ENOMEM;

							BofSocketParam_X.BaseChannelParam_X.ChannelName_S = mBofSocketServerParam_X.Name_S;
							BofSocketParam_X.BaseChannelParam_X.Blocking_B = true;  // mBofSocketServerParam_X.Blocking_B;
							BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = mBofSocketServerParam_X.NbMaxSession_U32;
							BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = 0x1000; // mBofSocketServerParam_X.ServerRcvBufferSize_U32;
							BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = 0x1000;// mBofSocketServerParam_X.ServerSndBufferSize_U32;
							BofSocketParam_X.ReUseAddress_B = true;
							BofSocketParam_X.BindIpAddress_S = mBofSocketServerParam_X.Address_S;
							puListenSocket = std::make_unique<BofSocket>(BofSocketParam_X);
							BOF_ASSERT(puListenSocket != nullptr);
							if (puListenSocket)
							{
								mErrorCode_E = puListenSocket->LastErrorCode();
								if (mErrorCode_E == BOF_ERR_NO_ERROR)
								{
									//DP https://forum.libcinder.org/topic/solution-calling-shared-from-this-in-the-constructor 
									//const auto TrickDontRemove = std::shared_ptr<BofSocketServer>(this, [](BofSocketServer*) {});
									BofSocketIoParam_X.Name_S = "SERVER_LISTENER";
									psListenSocketSession = std::make_shared<BofSocketIo>(this, std::move(puListenSocket), BofSocketIoParam_X);
									mErrorCode_E = AddToPollList(BOF_SOCKET_SESSION_TYPE::SERVER_LISTENER, nullptr, 0, psListenSocketSession, ListenSessionId);
									mHasAListener_B = (mErrorCode_E == BOF_ERR_NO_ERROR);
								}
							}
						}
						else
						{
							//DP https://forum.libcinder.org/topic/solution-calling-shared-from-this-in-the-constructor 
							//const auto TrickDontRemove = std::shared_ptr<BofSocketServer>(this, [](BofSocketServer*) {});
							mErrorCode_E = BOF_ERR_NO_ERROR;
						}
					}
				}
			}
		}
	}
}
 

BofSocketServer::~BofSocketServer()
{
	StopProcessingThread();
	BOF_SAFE_DELETE_ARRAY(mpActiveSession_B);
}
BOFERR BofSocketServer::LastErrorCode() const
{
	return mErrorCode_E;
}
BOF_SOCKET_ADDRESS BofSocketServer::SocketServerAddress()
{
	return mSocketServerAddress_X;
}
uint32_t BofSocketServer::LookAndLockFreeSessionIndex()
{
	uint32_t Rts_U32 = 0xFFFFFFFF, i_U32;

	if ((mpActiveSession_B) && (NbConnectedSession() < mBofSocketServerParam_X.NbMaxSession_U32))
	{
		LockThreadCriticalSection(" LookAndLockFreeSessionIndex");
			for (i_U32 = 0; i_U32 < mBofSocketServerParam_X.NbMaxSession_U32; i_U32++)
			{
				if (!mpActiveSession_B[i_U32])
				{
					mpActiveSession_B[i_U32] = true;
					Rts_U32 = i_U32;
					break;
				}
			}
		UnlockThreadCriticalSection();
	}

	BOF_ASSERT(Rts_U32 != 0xFFFFFFFF);
	return Rts_U32;
}

bool BofSocketServer::LockFreeSessionIndex(uint32_t _SessionIndex_U32)
{
	bool Rts_B = false;

	if ((mpActiveSession_B) && (_SessionIndex_U32 < mBofSocketServerParam_X.NbMaxSession_U32))
	{
		LockThreadCriticalSection(" LockFreeSessionIndex");

			if (!mpActiveSession_B[_SessionIndex_U32])
			{
				mpActiveSession_B[_SessionIndex_U32] = true;
				Rts_B = true;
			}
		UnlockThreadCriticalSection();
	}

//	BOF_ASSERT(Rts_B);
	return Rts_B;
}

//Called internally by BOFERR BofSocketSessionManager::V_OnProcessing() which managed the unique_ptr ness of this socket
//Called by thread
BOFERR BofSocketServer::SignalConnectionRequest(BofSocket *_pListenSocket)
{
  BOFERR												Rts_E = BOF_ERR_INIT;
  BofComChannel									*pSession;
	std::unique_ptr<BofSocket>	 puSocket;
	uint32_t											SessionIndex_U32;
	bool													ReleaseReservation_B;
	std::shared_ptr<BofSocketIo>	psSocketSession;

  BOF_ASSERT(_pListenSocket != nullptr);
	BOF_ASSERT(mpIBofSocketSessionFactory != nullptr);
	if ((_pListenSocket) && (mpIBofSocketSessionFactory != nullptr))
	{
    Rts_E = _pListenSocket->LastErrorCode();
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
			SessionIndex_U32 = LookAndLockFreeSessionIndex();
			if (SessionIndex_U32 != 0xFFFFFFFF)
			{
				ReleaseReservation_B = true;
				Rts_E = BOF_ERR_NOT_AVAILABLE;
				pSession = _pListenSocket->V_Listen(0, "");
				if (pSession)
				{
					puSocket.reset(dynamic_cast<BofSocket *>(pSession));
					Rts_E = ConnectSession(BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL,nullptr,0, std::move(puSocket), SessionIndex_U32, psSocketSession);

					if (Rts_E == BOF_ERR_NO_ERROR)
					{
						ReleaseReservation_B = false;
					}
				}
				if ((ReleaseReservation_B) && (mpActiveSession_B))
				{
					mpActiveSession_B[SessionIndex_U32] = false;
				}
			}
    }
  }
  return Rts_E;
}
//Called by client
BOFERR BofSocketServer::Connect(uint32_t _TimeoutInMs_U32, const std::string &_rConnectFromIpAddress_S, const std::string &_rConnectToIpAddress_S, std::shared_ptr<BofSocketIo> &_rpsCmdSocketSession)
{
	BOFERR								Rts_E = BOF_ERR_INIT;
	BOF_SOCKET_PARAM			BofSocketParam_X;
	BOF_SOCKET_IO_PARAM		BofSocketIoParam_X;
	std::unique_ptr<BofSocket>	 puSocket;
	uint32_t							SessionIndex_U32, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32;
	uint16_t							Port_U16;
	BOF_SOCK_TYPE					SocketType_E;
	BOF_SOCKET_ADDRESS		SocketAddress_X;
	bool									ReleaseReservation_B;

	_rpsCmdSocketSession = nullptr;
	BOF_ASSERT(mpIBofSocketSessionFactory != nullptr);
	if (mpIBofSocketSessionFactory != nullptr)
	{
		SessionIndex_U32 = LookAndLockFreeSessionIndex();
		if (SessionIndex_U32 != 0xFFFFFFFF)
		{
			ReleaseReservation_B = true;
			BofSocketParam_X.BaseChannelParam_X.ChannelName_S = mBofSocketServerParam_X.Name_S + "_Connect";
			BofSocketParam_X.BaseChannelParam_X.Blocking_B = true;
			//  BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = no as it is set by BofSocketIoParam_X.SocketRcvBufferSize_U32 below;
			//BofSocketParam_X.BindIpAddress_S = Bof_Sprintf("tcp://0.0.0.0:0;0.0.0.0:0");
			Rts_E = Bof_SplitIpAddress(_rConnectFromIpAddress_S, SocketAddress_X);
			if (Rts_E == BOF_ERR_NO_ERROR)
			{
				Rts_E = BOF_ERR_WRONG_MODE;
				SocketAddress_X.Parse(SocketType_E, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, Port_U16);
				if (SocketType_E == BOF_SOCK_TYPE::BOF_SOCK_TCP)
				{
					Rts_E = BOF_ERR_ENOMEM;
					BofSocketParam_X.BindIpAddress_S = Bof_Sprintf("tcp://0.0.0.0:0;%d.%d.%d.%d:%d", Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, Port_U16);
					BofSocketParam_X.ReUseAddress_B = true;
					BofSocketParam_X.NoDelay_B = true;
					BofSocketParam_X.Ttl_U32 = 0;
					BofSocketParam_X.BroadcastPort_U16 = 0;
					BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
					BofSocketParam_X.MulticastSender_B = false;
					BofSocketParam_X.KeepAlive_B = false;
					BofSocketParam_X.EnableLocalMulticast_B = false;
					puSocket = std::make_unique<BofSocket>(BofSocketParam_X);
					if (puSocket)
					{
						Rts_E = puSocket->LastErrorCode();
						if (Rts_E == BOF_ERR_NO_ERROR)
						{
							Rts_E = puSocket->V_Connect(_TimeoutInMs_U32, _rConnectToIpAddress_S, "");
							if (Rts_E == BOF_ERR_NO_ERROR)
							{
								Rts_E = ConnectSession(BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL,nullptr,0, std::move(puSocket), SessionIndex_U32, _rpsCmdSocketSession);
								if (Rts_E == BOF_ERR_NO_ERROR)
								{
									ReleaseReservation_B = false;
								}
							}
						}
					}
				}
			}
			if ((ReleaseReservation_B) && (mpActiveSession_B))
			{
				mpActiveSession_B[SessionIndex_U32] = false;
			}
		}
	}
	return Rts_E;
}

//Called by thread or client
//BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION
BOFERR BofSocketServer::ConnectSession(BOF_SOCKET_SESSION_TYPE _SessionType_E, std::shared_ptr<BofSocketIo> _psParentCmdChannel, uint32_t _TimeoutInMs_U32, std::unique_ptr<BofSocket> _puSocket, uint32_t _SessionIndex_U32, std::shared_ptr<BofSocketIo>	&_rpsSocketSession)
{
	BOFERR												Rts_E= BOF_ERR_BAD_TYPE;
	std::shared_ptr<BofSocketIo>	psSocketSession;
	BOFSOCKET						SessionId;

	_rpsSocketSession = nullptr;
	if (mBofSocketServerParam_X.ServerMode_E == BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION)
	{
		Rts_E = BOF_ERR_EINVAL;
		if ((_puSocket) && (mpIBofSocketSessionFactory))
		{
			Rts_E = BOF_ERR_CREATE;
			psSocketSession = mpIBofSocketSessionFactory->V_OpenSession(_SessionType_E, _SessionIndex_U32, std::move(_puSocket));
			if (psSocketSession)
			{
				psSocketSession->SessionIndex(_SessionIndex_U32);
				psSocketSession->Connected(true);	//First before V_SignalConnected
				Rts_E = psSocketSession->V_SignalConnected(_SessionType_E);
				if (Rts_E == BOF_ERR_NO_ERROR)
				{
					Rts_E = AddToPollList(_SessionType_E, _psParentCmdChannel, _TimeoutInMs_U32, psSocketSession, SessionId);
					if (Rts_E == BOF_ERR_NO_ERROR)
					{

						_rpsSocketSession = psSocketSession;
					}
				}
			}
		}
	}
	return Rts_E;
} 

//Called by thread or client
//BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_POLLER
BOFERR BofSocketServer::ConnectSession(uint32_t _SessionIndex_U32, std::unique_ptr<BofSocket> _puSocket, std::shared_ptr<BofSocketIo>	&_rpsSocketSession)
{
	BOFERR												Rts_E = BOF_ERR_BAD_TYPE;

	if (mBofSocketServerParam_X.ServerMode_E == BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_POLLER)
	{
		Rts_E = BOF_ERR_EINVAL;
		if (mpIBofSocketSessionFactory)
		{
			Rts_E = BOF_ERR_CREATE;
			_rpsSocketSession = mpIBofSocketSessionFactory->V_OpenSession(BOF_SOCKET_SESSION_TYPE::POLL_CHANNEL, _SessionIndex_U32, std::move(_puSocket));
			if (_rpsSocketSession)
			{
				_rpsSocketSession->SessionType(BOF_SOCKET_SESSION_TYPE::POLL_CHANNEL);
				_rpsSocketSession->SessionIndex(_SessionIndex_U32);
				_rpsSocketSession->Connected(true);	//First before V_SignalConnected
				Rts_E = _rpsSocketSession->V_SignalConnected(BOF_SOCKET_SESSION_TYPE::POLL_CHANNEL);
				if (Rts_E == BOF_ERR_NO_ERROR)
				{
				}
			}
		}
	}
	return Rts_E;
}



BOFERR	BofSocketServer::Disconnect(bool _CloseDataChannelIfPresent_B, std::shared_ptr<BofSocketIo> _psSocketSession)
{
	BOFERR							Rts_E=BOF_ERR_EINVAL;
	uint32_t						SessionIndex_U32;

	if (_psSocketSession)
	{
		if (_CloseDataChannelIfPresent_B)
		{
			if (_psSocketSession->ChildDataChannel())
			{
				Rts_E = CloseDataChannel(_psSocketSession->ChildDataChannel());
				BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
			}
		}
		Rts_E = RemoveFromPollList(0, _psSocketSession, SessionIndex_U32);
		BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
	}
	return Rts_E;
}

BOFERR BofSocketServer::WriteToAllSession(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, const std::string &_rBuffer_S, void *_pWriteContext)
{
	uint32_t Nb_U32 = static_cast<uint32_t>(_rBuffer_S.size());
	return WriteToAllSession(_TimeoutInMs_U32, _AsyncMode_B, Nb_U32, reinterpret_cast<const uint8_t *>(_rBuffer_S.c_str()), _pWriteContext);
}
BOFERR BofSocketServer::WriteToAllSession(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, uint32_t _Nb_U32, const uint8_t *_pBuffer_U8, void *_pWriteContext)
{
	BOFERR Rts_E = BOF_ERR_NO_ERROR, Sts_E= BOF_ERR_NO_ERROR;
	uint32_t Size_U32;

	LockThreadCriticalSection(" WriteToAllSession");
		for (const auto &_rItem : mSessionCollection)
		{
			if (_rItem.second)
			{
				Size_U32 = _Nb_U32;
				Sts_E = _rItem.second->Write(_TimeoutInMs_U32, _AsyncMode_B, Size_U32, _pBuffer_U8, _pWriteContext);
				if (Sts_E != BOF_ERR_NO_ERROR)
				{
					Rts_E = Sts_E;	//latch error
				}
			}
		}
	UnlockThreadCriticalSection();

  return Rts_E;
}

BOFERR BofSocketServer::ConnectToDataChannel(bool _Passive_B, std::shared_ptr<BofSocketIo> _psCmdSocketSession, uint32_t _ConnectionTimeoutInMs_U32, uint32_t _NoIoCloseTimeoutInMs_U32, std::shared_ptr<BofSocketIo> &_rpsDataSocketSession)
{
	BOFERR Rts_E = BOF_ERR_EINVAL;
	uint16_t Port_U16;
	uint32_t Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, PortHigh_U32, PortLow_U32, ReplyCode_U32, SessionIndex_U32;
	BOF_SOCKET_PARAM BofSocketParam_X;
	BOF_SOCKET_IO_PARAM BofSocketIoParam_X;
	std::unique_ptr<BofSocket> puListen = nullptr;
	BOF_SOCKET_ADDRESS SocketAddress_X;
	std::string Cmd_S, Reply_S, TargetIp_S;
	std::unique_ptr<BofSocket>		puDataSocket;
	std::shared_ptr<BofSocketIo> psListenDataSocket;

	if (_psCmdSocketSession)
	{
		Rts_E = BOF_ERR_BAD_TYPE;
		if (_psCmdSocketSession->SessionType() == BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL)
		{
			Rts_E = BOF_ERR_NOT_SUPPORTED;
			if (_Passive_B)
			{
				SessionIndex_U32 = _psCmdSocketSession->SessionIndex();

				Cmd_S = Bof_Sprintf("PASV\r\n");
				Rts_E = _psCmdSocketSession->SendCommandAndWaitForReply(_ConnectionTimeoutInMs_U32, Cmd_S, 227, ReplyCode_U32, Reply_S);
				if (Rts_E == BOF_ERR_NO_ERROR)
				{
					static const std::regex   S_RegExPasv("^.*?(\\d+),(\\d+),(\\d+),(\\d+),(\\d+),(\\d+).*$");
					std::cmatch         PasvMatch;
					Rts_E = BOF_ERR_INVALID_ANSWER;
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
						Ip1_U32 = 0;
						Ip2_U32 = 0;
						Ip3_U32 = 0;
						Ip4_U32 = 0;
						PortHigh_U32 = 0;
						PortLow_U32 = 0;

						try
						{
							Ip1_U32 = static_cast<uint32_t>(std::stoi(PasvMatch[1].str()));
							Ip2_U32 = static_cast<uint32_t>(std::stoi(PasvMatch[2].str()));
							Ip3_U32 = static_cast<uint32_t>(std::stoi(PasvMatch[3].str()));
							Ip4_U32 = static_cast<uint32_t>(std::stoi(PasvMatch[4].str()));
							PortHigh_U32 = static_cast<uint8_t>(std::stoi(PasvMatch[5].str()));
							PortLow_U32 = static_cast<uint8_t>(std::stoi(PasvMatch[6].str()));
							Rts_E = BOF_ERR_NO_ERROR;
						}
						catch (const std::exception &)
						{
							Rts_E = BOF_ERR_EINVAL;
						}
						if (Rts_E == BOF_ERR_NO_ERROR)
						{
							Port_U16 = static_cast<uint16_t>((PortHigh_U32 << 8) + PortLow_U32);
							Rts_E = BOF_ERR_OUT_OF_RANGE;
							if ((Port_U16 >= mBofSocketServerParam_X.MinPortValue_U16) && (Port_U16 <= mBofSocketServerParam_X.MaxPortValue_U16))
							{
								//Debug no connect
								//Port_U16++;

								Rts_E = BOF_ERR_ENOMEM;
								TargetIp_S = Bof_Sprintf("tcp://0.0.0.0:0;%d.%d.%d.%d:%d", Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, Port_U16);
								BofSocketParam_X.BaseChannelParam_X.ChannelName_S = mBofSocketServerParam_X.Name_S + "_ConnectToData";
								BofSocketParam_X.BindIpAddress_S = Bof_Sprintf("tcp://0.0.0.0:0;0.0.0.0:0");
								BofSocketParam_X.ReUseAddress_B = true;
								BofSocketParam_X.NoDelay_B = true;
								BofSocketParam_X.Ttl_U32 = 0;
								BofSocketParam_X.BroadcastPort_U16 = 0;
								BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
								BofSocketParam_X.MulticastSender_B = false;
								BofSocketParam_X.KeepAlive_B = false;
								BofSocketParam_X.EnableLocalMulticast_B = false;
								puDataSocket.reset(new BofSocket(BofSocketParam_X));

								if (puDataSocket != nullptr)
								{
									Rts_E = puDataSocket->V_Connect(_ConnectionTimeoutInMs_U32, TargetIp_S, "");
									if (Rts_E == BOF_ERR_NO_ERROR)
									{
										Rts_E = ConnectSession(BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL, _psCmdSocketSession, _NoIoCloseTimeoutInMs_U32, std::move(puDataSocket), SessionIndex_U32, _rpsDataSocketSession);
										if (Rts_E == BOF_ERR_NO_ERROR)
										{
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return Rts_E;
}

//Answer to pasv
BOFERR BofSocketServer::ListenForDataChannelConnection(bool _Passive_B, std::shared_ptr<BofSocketIo> _psCmdSocketSession, uint32_t _ConnectionTimeoutInMs_U32,const std::string &_rIpAddress_S)
{
	BOFERR Rts_E = BOF_ERR_EINVAL;
	uint16_t Port_U16;
	uint32_t Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, PortHigh_U32, PortLow_U32;	// , SessionIndex_U32;
	BOF_SOCKET_PARAM BofSocketParam_X;
	BOF_SOCKET_IO_PARAM BofSocketIoParam_X;
	std::unique_ptr<BofSocket> puListen = nullptr;
	BOF_SOCK_TYPE SocketType_E;
	BOF_SOCKET_ADDRESS SocketAddress_X;
	std::string Cmd_S, Reply_S, TargetIp_S;
	std::unique_ptr<BofSocket>		puDataSocket;
	std::shared_ptr<BofSocketIo> psDataSocketSession;
	std::shared_ptr<BofSocketIo> psListenDataSocket;
	BOFSOCKET SessionId;

	if (_psCmdSocketSession)
	{
		Rts_E = BOF_ERR_BAD_TYPE;
		if (_psCmdSocketSession->SessionType() == BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL)
		{
			Rts_E = BOF_ERR_NOT_SUPPORTED;

			if (_Passive_B)
			{
				//SessionIndex_U32 = psSocketSession->SessionIndex();
				//Non initiator need ip target address

				Rts_E = Bof_SplitIpAddress(_rIpAddress_S, SocketAddress_X);
				if (Rts_E == BOF_ERR_NO_ERROR)
				{
					Rts_E = BOF_ERR_ENOMEM;

					SocketAddress_X.Parse(SocketType_E, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, Port_U16);
					Port_U16 = GenerateDynamicPort();
					PortHigh_U32 = static_cast<uint32_t>(Port_U16 >> 8);
					PortLow_U32 = static_cast<uint32_t>(Port_U16 & 0xFF);
					BofSocketParam_X.BaseChannelParam_X.ChannelName_S = mBofSocketServerParam_X.Name_S + "_DataListener";
					BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 1;
					BofSocketParam_X.BaseChannelParam_X.Blocking_B = true;
					//BofSocketParam_X.BindIpAddress_S = Bof_Sprintf("tcp://%d.%d.%d.%d:%d;0.0.0.0:0", Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, Port_U16);
					BofSocketParam_X.BindIpAddress_S = Bof_Sprintf("tcp://0.0.0.0:0;%d.%d.%d.%d:%d", Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, Port_U16);
					BofSocketParam_X.ReUseAddress_B = true;
					BofSocketParam_X.NoDelay_B = true;
					BofSocketParam_X.Ttl_U32 = 0;
					BofSocketParam_X.BroadcastPort_U16 = 0;
					BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
					BofSocketParam_X.MulticastSender_B = false;
					BofSocketParam_X.KeepAlive_B = false;
					BofSocketParam_X.EnableLocalMulticast_B = false;
					puListen = std::make_unique<BofSocket>(BofSocketParam_X);

					if (puListen != nullptr)
					{
						BofSocketIoParam_X.Name_S = "DATA_LISTENER";
						psListenDataSocket = std::make_shared<BofSocketIo>(this, std::move(puListen), BofSocketIoParam_X);
						//Done in AddToPollList								psListenDataSocket->NoIoCloseTimeoutInMs(_ConnectionTimeoutInMs_U32);
						Rts_E = BOF_ERR_ENOMEM;

						if (psListenDataSocket)
						{
							Rts_E = psListenDataSocket->LastErrorCode();
							if (Rts_E == BOF_ERR_NO_ERROR)
							{
//Put it below		
								Rts_E = AddToPollList(BOF_SOCKET_SESSION_TYPE::DATA_LISTENER, _psCmdSocketSession, _ConnectionTimeoutInMs_U32, psListenDataSocket, SessionId);
								if (Rts_E == BOF_ERR_NO_ERROR)
								{
									//Debug no connect
									//PortLow_U32++;
									Rts_E = _psCmdSocketSession->Write(_ConnectionTimeoutInMs_U32, false, Bof_Sprintf("227 Entering Passive Mode (%d,%d,%d,%d,%d,%d).\r\n", Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, PortHigh_U32, PortLow_U32), nullptr);
								}
							}
						}
					}
				}
			}
		}
	}
	return Rts_E;
}


BOFERR BofSocketServer::CloseDataChannel(std::shared_ptr<BofSocketIo> _psDataSocketSession)
{
	BOFERR Rts_E = BOF_ERR_EINVAL;
	uint32_t SessionIndex_U32;

	if (_psDataSocketSession)
	{
		BOF_ASSERT(_psDataSocketSession->SessionType() == BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL);
		Rts_E = BOF_ERR_BAD_TYPE;
		if (_psDataSocketSession->SessionType() == BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL)
		{
			Rts_E = RemoveFromPollList(0, _psDataSocketSession, SessionIndex_U32);
			BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
		}
	}
	return Rts_E;
}

uint16_t BofSocketServer::GenerateDynamicPort()
{
	uint16_t Rts_U16, NextPort_U16;

	std::lock_guard<std::mutex> Lock(mDynamicPortMtx);
	Rts_U16 = mDynamicPort_U16;
	NextPort_U16 = static_cast<uint16_t>(Rts_U16 + 1);	//Rollover on 0
	mDynamicPort_U16 = ((NextPort_U16 < mBofSocketServerParam_X.MinPortValue_U16) || (NextPort_U16 > mBofSocketServerParam_X.MaxPortValue_U16)) ? mBofSocketServerParam_X.MinPortValue_U16 : NextPort_U16;
	return Rts_U16;
}

std::shared_ptr<BofSocketIo> BofSocketServer::ConnectedCmdSession(uint32_t _Index_U32, uint32_t _PollTimeInMs_U32, uint32_t _TimeoutInMs_U32)
{
	std::shared_ptr<BofSocketIo> psRts = nullptr;
	uint32_t Start_U32, Delta_U32;

	Start_U32 = BOF::Bof_GetMsTickCount();
	do
	{
		if (_Index_U32 < mConnectedSessionCollection.size())
		{
			psRts = mConnectedSessionCollection[_Index_U32];
			break;
		}
		else
		{
			if (_PollTimeInMs_U32)
			{
        BOF::Bof_MsSleep(_PollTimeInMs_U32);
			}
		}
		if (_TimeoutInMs_U32)
		{
			Delta_U32 = BOF::Bof_ElapsedMsTime(Start_U32);
		}
		else
		{
			break;
		}
	} while (Delta_U32 < _TimeoutInMs_U32);
	return psRts;
}

std::shared_ptr<BofSocketIo> BofSocketServer::ConnectedDataSession(uint32_t _Index_U32, uint32_t _PollTimeInMs_U32, uint32_t _TimeoutInMs_U32)
{
	std::shared_ptr<BofSocketIo> psRts = nullptr;

	std::shared_ptr<BofSocketIo> psConnectedSession = ConnectedCmdSession(_Index_U32, _PollTimeInMs_U32, _TimeoutInMs_U32);
	if (psConnectedSession)
	{
		psRts = psConnectedSession->ChildDataChannel();
	}
	return psRts;
}

std::string BofSocketServer::SocketServerDebugInfo()
{
	std::string Rts_S;

	Rts_S = Bof_Sprintf("BofSocketServer\nName:              %s\nInitError:         %s\nCpuCoreAffinity:   %016lX\nSchedulerPolicy:   %d\nPriority:          %03d\nAddress:           %s\nSession:           %04d/%04d\nPort:              %05d <= %05d <= %05d\nFactory:           %p\nPortMtx:           %p\n", mBofSocketServerParam_X.Name_S.c_str(),
		Bof_ErrorCode(mErrorCode_E), mBofSocketServerParam_X.ThreadCpuCoreAffinityMask_U64, mBofSocketServerParam_X.ThreadSchedulerPolicy_E, mBofSocketServerParam_X.ThreadPriority_E, mBofSocketServerParam_X.Address_S.c_str(),
		NbConnectedSession(), mBofSocketServerParam_X.NbMaxSession_U32, mBofSocketServerParam_X.MinPortValue_U16, mDynamicPort_U16, mBofSocketServerParam_X.MaxPortValue_U16, mpIBofSocketSessionFactory, mDynamicPortMtx.native_handle());

	Rts_S += SocketSessionManagerDebugInfo();
	
	return Rts_S;
}
END_BOF_NAMESPACE()