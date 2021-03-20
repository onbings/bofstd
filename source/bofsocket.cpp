/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module contains routines for creating and managing a socket communication
 * channel.
 *
 * Name:        bofsocket.cpp
 * Author:      Bernard HARMEL: b.harmel@belgacom.net
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Aug 21 2001  BHA : Initial release
 */

/*** Include files ***********************************************************/
#include <bofstd/bofsocket.h>

#if _WIN32
#pragma warning(push)
#pragma warning(disable:4996)
#include <WinSock2.h>
#include <iphlpapi.h>
#include <bofstd/bofstring.h>
#include <bofstd/bofstringformatter.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <bofstd/bofstring.h>
#include <bofstd/bofstringformatter.h>
#endif

BEGIN_BOF_NAMESPACE()

/*** Global variables ********************************************************/

std::atomic<int32_t> BofSocket::S_mBofSocketBalance;

BofSocket::BofSocket()
	: BofComChannel(BOF_COM_CHANNEL_TYPE::TSOCKET, mBofSocketParam_X.BaseChannelParam_X)
{
	mSocket = BOFSOCKET_INVALID;
	mMaxUdpLen_U32 = 0;
	mMulticastIpInterfaceAddress_X.Reset();
	mMulticastIpAddress_X.Reset();
	mDstIpAddress_X.Reset();
	mConnected_B = false;
}

BofSocket::BofSocket(const BOF_SOCKET_PARAM &_rBofSocketParam_X)
	: BofComChannel(BOF_COM_CHANNEL_TYPE::TSOCKET, mBofSocketParam_X.BaseChannelParam_X)
{
	mSocket = BOFSOCKET_INVALID;
	InitializeSocket(_rBofSocketParam_X);
}

BofSocket::BofSocket(BOFSOCKET _Socket_h, const BOF_SOCKET_PARAM &_rBofSocketParam_X)
	: BofComChannel(BOF_COM_CHANNEL_TYPE::TSOCKET, mBofSocketParam_X.BaseChannelParam_X)
{
	mSocket = (BOFSOCKET) _Socket_h;
	InitializeSocket(_rBofSocketParam_X);

}

BofSocket::~BofSocket()
{	
	S_mBofSocketBalance--;
//BOF_DBG_PRINTF("@@@%s ~BofSocket %08X Bal %04d\n", mBofSocketParam_X.BaseChannelParam_X.ChannelName_S.c_str(), mSocket,S_mBofSocketBalance.load());

	ShutdownSocket();
}

BOFERR BofSocket::ShutdownSocket()
{
	BOFERR Rts_E = BOF_ERR_NO_ERROR;
	struct ip_mreq IpV4MulticastRequest_X;   /* multicast request structure */
	struct ipv6_mreq IpV6MulticastRequest_X;   /* multicast request structure */

  //BOF_DBG_PRINTF("Socket[%08X] Shutdown\n", mSocket);
  if (mSocket != BOFSOCKET_INVALID)
	{
		if (!Bof_IsIpAddressNull(mDstIpAddress_X) && (!mBofSocketParam_X.MulticastSender_B))
		{
			if (!Bof_IsIpAddressNull(mMulticastIpAddress_X))
			{
				/*Client:  construct an IGMP join request structure */
				if (mMulticastIpAddress_X.IpV6_B)
				{
					memcpy(IpV6MulticastRequest_X.ipv6mr_multiaddr.s6_addr, mMulticastIpAddress_X.IpV6Address_X.sin6_addr.s6_addr, 16);
					IpV6MulticastRequest_X.ipv6mr_interface = htonl(INADDR_ANY);
					if (setsockopt(mSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *) &IpV6MulticastRequest_X, sizeof(IpV6MulticastRequest_X)) < 0)
					{

					}
				}
				else
				{
					IpV4MulticastRequest_X.imr_multiaddr.s_addr = mMulticastIpAddress_X.IpV4Address_X.sin_addr.s_addr;
					IpV4MulticastRequest_X.imr_interface.s_addr = htonl(INADDR_ANY);
					if (setsockopt(mSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *) &IpV4MulticastRequest_X, sizeof(IpV4MulticastRequest_X)) < 0)
					{

					}
				}
			}
		}
     
		shutdown(mSocket, 2);            // SD_BOTH ))
#if defined (_WIN32)
		closesocket(mSocket);
#else
		close(mSocket);
#endif
		mSocket = BOFSOCKET_INVALID;

		BOF_SAFE_DELETE_ARRAY(mpScatterGatherBuffer_X);
	}
	mConnected_B = false;

	return Rts_E;
}

BOFERR BofSocket::S_InitializeStack()
{
#if defined (_WIN32)
	BOFERR  Rts_E = BOF_ERR_INIT;
	WSADATA Info_X;
	if (! WSAStartup(MAKEWORD(1, 1), &Info_X) )
	{
		Rts_E = BOF_ERR_NO_ERROR;
	}
	else
	{
		BOF_ASSERT(0);
	}
#else
	BOFERR Rts_E = BOF_ERR_NO_ERROR;
#endif
	S_mBofSocketBalance.store(0);
	return Rts_E;
}

BOFERR BofSocket::S_ShutdownStack()
{
	BOFERR Rts_E = BOF_ERR_NO_ERROR;

#if defined (_WIN32)
	WSACleanup();
#else
#endif
	return Rts_E;
}

int BofSocket::S_BofSocketBalance()
{
	return S_mBofSocketBalance.load();
}

BOFERR BofSocket::InitializeSocket(const BOF_SOCKET_PARAM &_rBofSocketParam_X)
{
	BOFERR Rts_E;
	std::vector<BOF_NETWORK_INTERFACE_PARAM> ListOfNetworkInterface_X;
	bool Bind_B;
	bool IsIpV6_B = false;      //TODO
	BOF_SOCKET_ADDRESS Ip_X;

	mMaxUdpLen_U32 = 0;
	mSrcIpAddress_X.Reset();
	mMulticastIpInterfaceAddress_X.Reset();
	mMulticastIpAddress_X.Reset();
	mDstIpAddress_X.Reset();
	mpScatterGatherBuffer_X = nullptr;
	Bind_B = true;

	mBofSocketParam_X = _rBofSocketParam_X;

	Rts_E = Bof_ResolveIpAddress(mBofSocketParam_X.BindIpAddress_S, Ip_X, mSrcIpAddress_X);
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		if (Bof_IsIpAddressNull(mSrcIpAddress_X))
		{
//No it is INADDR_ANY			Rts_E = BOF_ERR_INVALID_SRC;
		}
	}

	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		if (Bof_ResolveIpAddress(mBofSocketParam_X.MulticastInterfaceIpAddress_S, mMulticastIpInterfaceAddress_X, mMulticastIpAddress_X) == BOF_ERR_NO_ERROR)
		{
			if (IsIpV6_B)
			{
				mMulticastIpAddress_X.IpV6Address_X.sin6_port = mSrcIpAddress_X.IpV6Address_X.sin6_port;
			}
			else
			{
				mMulticastIpAddress_X.IpV4Address_X.sin_port = mSrcIpAddress_X.IpV4Address_X.sin_port;	//Multicast port is specified in BindIpAddress_S
			}
		}
		else
		{
			mMulticastIpInterfaceAddress_X.Reset();
			mMulticastIpAddress_X.Reset();
		}
	}
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		if (mSocket == BOFSOCKET_INVALID)
		{
			if (IsIpV6_B)
			{
				if (IsUdp())
				{
					if (Bof_IsIpAddressLocalHost(mBofSocketParam_X.BindIpAddress_S))
					{
#if defined(_WIN32)
						mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);		
#else
//Not always supported						mSocket = socket(AF_UNIX, SOCK_DGRAM, IPPROTO_UDP);		//Udp local host is not reliable in linux except if you use unix domain socket
						mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);		//Udp local host is reliable in windows and AF_UNIX does not exist
#endif
					}
					else
					{
						mSocket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);	//Unreliable by design
					}
				}
				else
				{
					mSocket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
				}
			}
			else
			{
				if (IsUdp())
				{
					if (Bof_IsIpAddressLocalHost(mBofSocketParam_X.BindIpAddress_S))
					{
#if defined(_WIN32)
						mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);	
//						mSocket = socket(AF_UNIX, SOCK_DGRAM, IPPROTO_UDP);	//Udp local host is reliable in windows and AF_UNIX does not exist
#else
						//Not always supported								mSocket = socket(AF_UNIX, SOCK_DGRAM, IPPROTO_UDP);		//Udp local host is not reliable in linux except if you use unix domain socket
						mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);	//Unreliable by design
#endif
					}
					else
					{
						mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);	//Unreliable by design
					}
				}
				else
				{
					mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				}
			}
			if (mSocket == BOFSOCKET_INVALID)
			{
				Rts_E = BOF_ERR_CREATE;
			}
		}
		else
		{
			Bind_B = IsUdp();
		}
	}
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		Rts_E = SetupSocket(false, Bind_B);
	}
	mErrorCode_E = Rts_E;
	S_mBofSocketBalance++;
//BOF_DBG_PRINTF("@@@%s InitializeSocket %08X Bal %04d Sts %08X Ip %s\n", _rBofSocketParam_X.BaseChannelParam_X.ChannelName_S.c_str(), mSocket, S_mBofSocketBalance.load(), Rts_E, mBofSocketParam_X.BindIpAddress_S.c_str());

	return Rts_E;
}

/*** SetupSocket ********************************************************************/

/*!
 * Description
 * The SetupSocket method sets the working parameter of a given socket
 *
 * Parameters
 * _Bind_B: true if the socket must be bound
 *
 * Returns
 * Nothing
 *
 * Remarks
 * None
 */
BOFERR BofSocket::SetupSocket(bool _IpV6_B, bool _Bind_B)
{
	BOFERR Rts_E = BOF_ERR_NO_ERROR;
	socklen_t Len_i;
	uint32_t Val_U32;
	struct ip_mreq IpV4MulticastRequest_X; /* multicast request structure */
	struct ipv6_mreq IpV6MulticastRequest_X; /* multicast request structure */
	BOF_SOCKET_ADDRESS MulticastClient_X;
	std::vector<uint16_t> BinFormat;

	mConnected_B = (IsUdp()) ? true : false;
	if (mBofSocketParam_X.ReUseAddress_B)
	{
		Len_i = sizeof(uint32_t);
		Val_U32 = 1;

		if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast< char * > (&Val_U32), Len_i))
		{
			Rts_E = BOF_ERR_ADDRESS;
		}
	}

	mMaxUdpLen_U32 = 0;
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		if (IsUdp())
		{
#if _WIN32
			Len_i                       = sizeof(uint32_t);
			if (getsockopt(mSocket, SOL_SOCKET, SO_MAX_MSG_SIZE, reinterpret_cast< char * > (&mMaxUdpLen_U32), &Len_i) == BOFSOCKET_INVALID)
			{
				Rts_E = BOF_ERR_WRONG_SIZE;
			}
#else
			mMaxUdpLen_U32 = 0xFF00;
#endif
			mBofSocketParam_X.NoDelay_B = false;

			if ((Rts_E == BOF_ERR_NO_ERROR) &&
			    (mBofSocketParam_X.BroadcastPort_U16)
				)
			{
				mDstIpAddress_X.Set(_IpV6_B, BOF_SOCK_TYPE::BOF_SOCK_UDP, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP, 255, 255, 255, 255, mBofSocketParam_X.BroadcastPort_U16);

				Len_i = sizeof(uint32_t);
				Val_U32 = 0xFFFFFFFF;

				// Allows broadcast transmission for udp
				if (setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast< char * > (&Val_U32), Len_i))
				{
					Rts_E = BOF_ERR_INIT;
				}
			}
			if ((Rts_E == BOF_ERR_NO_ERROR) && (mBofSocketParam_X.Ttl_U32))
			{
				Rts_E = SetTtl(mBofSocketParam_X.Ttl_U32);
			}
		}
		else
		{
			if (mBofSocketParam_X.KeepAlive_B)
			{
				Len_i = sizeof(uint32_t);
				Val_U32 = 1;
				if (setsockopt(mSocket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast< char * > (&Val_U32), Len_i))
				{
					Rts_E = BOF_ERR_INIT;
				}
			}
		}
	}

	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		Rts_E = SetSocketBufferSize(mBofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32, mBofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32);
	}

	if ((Rts_E == BOF_ERR_NO_ERROR)
	    && (!mBofSocketParam_X.BaseChannelParam_X.Blocking_B)
		)
	{
		Rts_E = SetNonBlockingMode(!mBofSocketParam_X.BaseChannelParam_X.Blocking_B);
	}

// Bind stage
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		if (!Bof_IsIpAddressNull(mMulticastIpAddress_X))
		{
			if (mBofSocketParam_X.MulticastSender_B)
			{
//https://www.ibm.com/support/knowledgecenter/en/ssw_i5_54/rzab6/x1multicast.htm: Disable loopback so you do not receive your own datagrams
				Len_i = sizeof(uint32_t);
				Val_U32 = (mBofSocketParam_X.EnableLocalMulticast_B) ? 1 : 0;

				if (setsockopt(mSocket, IPPROTO_IP, IP_MULTICAST_LOOP, reinterpret_cast< char * > (&Val_U32), Len_i))
				{
					Rts_E = BOF_ERR_INIT;
				}
// Set local interface for outbound multicast datagrams. The IP address specified must be associated with a local, multicast-capable interface.
				if (Rts_E == BOF_ERR_NO_ERROR)
				{
					_Bind_B = false;	//Do not call getsockname for multicast sender
					mDstIpAddress_X = mMulticastIpAddress_X;

					if (mMulticastIpInterfaceAddress_X.IpV6_B)
					{
						if (setsockopt(mSocket, IPPROTO_IP, IP_MULTICAST_IF, reinterpret_cast<char *> (&mMulticastIpInterfaceAddress_X.IpV6Address_X.sin6_addr.s6_addr), 16) < 0)
						{
							Rts_E = BOF_ERR_INIT;
						}
					}
					else
					{
						if (setsockopt(mSocket, IPPROTO_IP, IP_MULTICAST_IF, reinterpret_cast<char *> (&mMulticastIpInterfaceAddress_X.IpV4Address_X.sin_addr.s_addr), sizeof(mMulticastIpInterfaceAddress_X.IpV4Address_X.sin_addr.s_addr)) < 0)
						{
							Rts_E = BOF_ERR_INIT;
						}
					}
				}
			}
			else
			{
				/*https://www.ibm.com/support/knowledgecenter/en/ssw_i5_54/rzab6/x2multicast.htm: set reuse port to to allow multiple instances of the application to receive copies of the multicast datagrams. */
				Len_i = sizeof(uint32_t);
				Val_U32 = 1;

				if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &Val_U32, Len_i))
				{
					Rts_E = BOF_ERR_INIT;
				}

				if (Rts_E == BOF_ERR_NO_ERROR)
				{
					Bof_SocketAddressToBin(mMulticastIpAddress_X, BinFormat);
					if (_IpV6_B)
					{
//            Bind to the proper port number with the IP address specified as INADDR_ANY
#if defined (_WIN32)
#else
						if (mBofSocketParam_X.FilterMulticastOnIpAddress_B)
						{
							MulticastClient_X.Set(_IpV6_B, mMulticastIpInterfaceAddress_X.SocketType_E, mMulticastIpInterfaceAddress_X.ProtocolType_E, BinFormat[0], BinFormat[1], BinFormat[2], BinFormat[3], htons(mMulticastIpAddress_X.IpV6Address_X.sin6_port));
						}
						else
#endif
						{
							MulticastClient_X.Set(_IpV6_B, mMulticastIpInterfaceAddress_X.SocketType_E, mMulticastIpInterfaceAddress_X.ProtocolType_E, 0, 0, 0, 0, htons(mMulticastIpAddress_X.IpV6Address_X.sin6_port));
						}
						if (bind(mSocket, reinterpret_cast<BOF_SOCKADDR *> (&MulticastClient_X.IpV6Address_X), sizeof(MulticastClient_X.IpV6Address_X)) == SOCKET_ERROR)
						{
							Rts_E = BOF_ERR_BIND;
						}
					}
					else
					{
#if defined (_WIN32)
#else
						if (mBofSocketParam_X.FilterMulticastOnIpAddress_B)
						{
							MulticastClient_X.Set(_IpV6_B, mMulticastIpInterfaceAddress_X.SocketType_E, mMulticastIpInterfaceAddress_X.ProtocolType_E, BinFormat[0], BinFormat[1], BinFormat[2], BinFormat[3], htons(mMulticastIpAddress_X.IpV4Address_X.sin_port));
						}
						else
#endif
						{
							MulticastClient_X.Set(_IpV6_B, mMulticastIpInterfaceAddress_X.SocketType_E, mMulticastIpInterfaceAddress_X.ProtocolType_E, 0, 0, 0, 0, htons(mMulticastIpAddress_X.IpV4Address_X.sin_port)); // htons mandatory !!!
						}
						if (bind(mSocket, reinterpret_cast<BOF_SOCKADDR *> (&MulticastClient_X.IpV4Address_X), sizeof(MulticastClient_X.IpV4Address_X)) == SOCKET_ERROR)
						{
							Rts_E = BOF_ERR_BIND;
						}
					}
				}

				/*Client:  construct an IGMP join request structure */
				if (Rts_E == BOF_ERR_NO_ERROR)
				{
					if (_IpV6_B)
					{
						IpV6MulticastRequest_X.ipv6mr_interface = 0;  //.s6_addr, mMulticastIpInterfaceAddress_X.IpV6Address_X.sin6_addr.s6_addr, 16);
						memcpy(IpV6MulticastRequest_X.ipv6mr_multiaddr.s6_addr, mMulticastIpAddress_X.IpV6Address_X.sin6_addr.s6_addr, 16);

						if (setsockopt(mSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<char *> (&IpV6MulticastRequest_X), sizeof(IpV6MulticastRequest_X)))
						{
							Rts_E = BOF_ERR_INIT;
						}
					}
					else
					{
						IpV4MulticastRequest_X.imr_interface.s_addr = mMulticastIpInterfaceAddress_X.IpV4Address_X.sin_addr.s_addr;
						IpV4MulticastRequest_X.imr_multiaddr.s_addr = mMulticastIpAddress_X.IpV4Address_X.sin_addr.s_addr;

						if (setsockopt(mSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<char *> (&IpV4MulticastRequest_X), sizeof(IpV4MulticastRequest_X)))
						{
							Rts_E = BOF_ERR_INIT;
						}
					}
				}
			}
			mSrcIpAddress_X = mMulticastIpAddress_X;
		}
		else
		{
			if (_Bind_B)
			{
				if (_IpV6_B)
				{
					if (bind(mSocket, reinterpret_cast<BOF_SOCKADDR *> (&mSrcIpAddress_X.IpV6Address_X), sizeof(mSrcIpAddress_X.IpV6Address_X)) == SOCKET_ERROR)
					{
						Rts_E = BOF_ERR_BIND;
					}
				}
				else
				{
					if (bind(mSocket, reinterpret_cast<BOF_SOCKADDR *> (&mSrcIpAddress_X.IpV4Address_X), sizeof(mSrcIpAddress_X.IpV4Address_X)) == SOCKET_ERROR)
					{
						int32_t NativeErrorCode_S32;
						Rts_E = Bof_GetLastError(true, &NativeErrorCode_S32);
//						printf("bind to %s err %d\n", Bof_SocketAddressToString(mSrcIpAddress_X, true, true).c_str(), NativeErrorCode_S32);
//						mSrcIpAddress_X.Set(false, mSrcIpAddress_X.Port()+1);
						Rts_E = BOF_ERR_BIND;
					}
				}
			}
		}
	}

	if ((Rts_E == BOF_ERR_NO_ERROR) &&
	    (mBofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32) && (IsTcp())
		)
	{
		if (listen(mSocket, mBofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32) == SOCKET_ERROR)
		{
			Rts_E = BOF_ERR_INIT;
		}
	}

	if ((Rts_E == BOF_ERR_NO_ERROR) &&  (mBofSocketParam_X.NoDelay_B)
		)
	{
		Rts_E = DisableNagle();
	}


	if (Rts_E == BOF_ERR_NO_ERROR)
	{
    if (_Bind_B)	//Do not call getsockname for multicast sender
		{
			// We need this to get back the port value of a binding to port 0 (->ftp PASV for example
			if (_IpV6_B)
			{
				Len_i = sizeof(BOF_SOCKADDR_IN6);
				if (getsockname(mSocket, reinterpret_cast<BOF_SOCKADDR *> (&mSrcIpAddress_X.IpV6Address_X), &Len_i) == SOCKET_ERROR) // get allocated port if port was 0
				{
					Rts_E = BOF_ERR_INIT;
				}
			}
			else
			{
				Len_i = sizeof(BOF_SOCKADDR_IN);
				if (getsockname(mSocket, reinterpret_cast<BOF_SOCKADDR *> (&mSrcIpAddress_X.IpV4Address_X), &Len_i) == SOCKET_ERROR) // get allocated port if port was 0
				{
					int32_t NativeErrorCode_S32;
					Rts_E=Bof_GetLastError(true, &NativeErrorCode_S32);
					Rts_E = BOF_ERR_INIT;
				}
			}
		}
	}
	if ((Rts_E == BOF_ERR_NO_ERROR) && (mBofSocketParam_X.MaxNumberOfScatterGatherEntry_U32))
	{
		mpScatterGatherBuffer_X = new SCATTER_GATHER_BUFFER[mBofSocketParam_X.MaxNumberOfScatterGatherEntry_U32];
	}

	if (Rts_E != BOF_ERR_NO_ERROR)
	{
		ShutdownSocket();
	}
	mErrorCode_E = Rts_E;
  return Rts_E;
}


BOFERR BofSocket::SetNonBlockingMode(bool _NonBlocking_B)
{
	BOFERR Rts_E = BOF_ERR_NO_ERROR;

#if defined (_WIN32)
	// Set the socket as non-blocking
	uint32_t NonBlocking_U32 = (_NonBlocking_B) ? 1 : 0;
	if (ioctlsocket(mSocket, FIONBIO, (u_long *)&NonBlocking_U32))
	{
		Rts_E = BOF_ERR_INIT;
	}
#else
	// Get the flags
	int Flag_i = fcntl(mSocket, F_GETFL, 0);

	if (Flag_i == -1)
	{
		Flag_i = 0;
	}

	if (_NonBlocking_B)
	{
		Flag_i |= O_NONBLOCK;
	}
	else
	{
		Flag_i &= O_NONBLOCK;
	}

	if (fcntl(mSocket, F_SETFL, Flag_i | O_NONBLOCK) == -1)
	{
		Rts_E = BOF_ERR_INIT;
	}

#endif

	// Update parameters
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		mBofSocketParam_X.BaseChannelParam_X.Blocking_B = !_NonBlocking_B;
	}

	return Rts_E;
}

bool BofSocket::IsBlocking()
{
	return mBofSocketParam_X.BaseChannelParam_X.Blocking_B;
}

/*** SetSocketBufferSize ********************************************************************/

/*!
 * Description
 * This function define the communication buffer size.
 *
 *
 * Parameters
 * _RcvBufferSize_U32: Specifies the maximum size of the socket receive buffer
 * _SndBufferSize_U32: Specifies the maximum size of the socket send buffer
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * By default Udp socket are limitted to 8 KB
 */
BOFERR BofSocket::SetSocketBufferSize(uint32_t _RcvBufferSize_U32, uint32_t _SndBufferSize_U32)
{
	BOFERR Rts_E = BOF_ERR_NO_ERROR;
	uint32_t Val_U32;
	socklen_t Len_i;
	int Sts_i;

	if (_RcvBufferSize_U32)
	{
		Len_i = sizeof(uint32_t);
		Val_U32 = _RcvBufferSize_U32;

		// Sets the maximum socket receive buffer in bytes. The kernel double this
		// value when it is set using setsockopt(2), and this doubled value is
		// returned by the /proc/sys/net/core/rmem_default file, and the maximum
		// allowed value is set by the /proc/sys/net/core/rmem_max file. The minimum
		// (doubled) value for this option is 256.
		Sts_i = setsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char *) &Val_U32, Len_i);
		if (Sts_i)
		{
			Rts_E = BOF_ERR_INIT;
		}
		else
		{
			Sts_i = getsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char *) &Val_U32, &Len_i);

			if (Val_U32 < _RcvBufferSize_U32)
			{
				Rts_E = BOF_ERR_INIT;
			}
		}
	}

	if ((Rts_E == BOF_ERR_NO_ERROR) &&
	    (_SndBufferSize_U32)
		)
	{
		Len_i = sizeof(uint32_t);
		Val_U32 = _SndBufferSize_U32;

		// Sets the maximum socket send buffer in bytes. The kernel double this
		// value when it is set using setsockopt(2), and this doubled value is
		// returned by the /proc/sys/net/core/wmem_default file, and the maximum
		// allowed value is set by the /proc/sys/net/core/wmem_max file. The minimum
		// (doubled) value for this option is 2048.
		Sts_i = setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char *) &Val_U32, Len_i);
		if (Sts_i)
		{
			Rts_E = BOF_ERR_INIT;
		}
		else
		{
			Sts_i = getsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char *) &Val_U32, &Len_i);

			if (Val_U32 < _SndBufferSize_U32)
			{
				Rts_E = BOF_ERR_INIT;
			}
		}
	}

	return Rts_E;
}


BOFERR BofSocket::DisableNagle()
{
	int Len_i = sizeof(uint32_t);
	uint32_t Val_U32 = 0xFFFFFFFF;
	BOFERR Rts_E = BOF_ERR_NO_ERROR;

	if (setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &Val_U32, Len_i))
	{
		Rts_E = BOF_ERR_INIT;
	}
	return Rts_E;
}

#if 0
BOF_SOCKET_ADDRESS BofSocket::GetLocalEndpoint()
{
	BOF_SOCKET_ADDRESS Rts_X = mBofSocketParam_X.LocalIpAddress_X;
	return Rts_X;
}


BOF_SOCKET_ADDRESS BofSocket::GetRemoteEndpoint()
{
	BOF_SOCKET_ADDRESS Rts_X = mDstIpAddress_X;
	return Rts_X;
}


BOFERR BofSocket::SetDstEndpoint(BOF_SOCKET_ADDRESS _Address_X)
{
	BOFERR Rts_E = BOF_ERR_NO_ERROR;
	mDstIpAddress_X = _Address_X;

	if (mpSocketState_X)
	{
		mpSocketState_X->DstIpAddress_X = mDstIpAddress_X;
	}
	return Rts_E;
}
#endif

BOFERR BofSocket::SetTtl(uint32_t _Ttl_U32)
{
	BOFERR Rts_E;

	if (setsockopt(mSocket, IPPROTO_IP, IP_MULTICAST_TTL, (char *) &_Ttl_U32, sizeof(_Ttl_U32)))
	{
		Rts_E = BOF_ERR_INIT;
	}
	else
	{
		mBofSocketParam_X.Ttl_U32 = _Ttl_U32;
		Rts_E = BOF_ERR_NO_ERROR;
	}

	return Rts_E;
}

/*** V_Connect ********************************************************************/

/*!
 * Description
 * The Connect method  establishes a connection to a specified socket.
 *
 *
 * Parameters
 * _TimeoutInMs_U32: Specifies the maximum time allowed to perform the operation
 * pTarget_X: Specify a BOF_SOCKET_ADDRESS structure describing the target socket to connect
 * _UdpDataSize_U32:  For Udp "connection", it specifies the size of the _pUdpData_U8 buffer which will be sent to Udp server (Connect string)
 * _pUdpData_U8:  For Udp "connection", it is used to send welcome message to udp server
 *
 * If no _pUdpDataSizeMax_U32 or _pUdpData_U8 is specified on a UDP socket default value are used.
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * This function is needed by the BOFComChannel abstraction class
 */
BOFERR BofSocket::V_Connect(uint32_t _TimeoutInMs_U32, const std::string &_rTarget_S, const std::string &_rOption_S)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	int Err_i, Nb_i;
	socklen_t Len;
	char pUdpConnect_c[0x10000];
	BOF_SOCKET_ADDRESS Ip_X;
	bool ProcessUdpConnectProtocol_B;
	std::string Val_S, Option_S;
//	uint32_t Err_U32;

	bool IsIpV6_B = false, Blocking_B;      //TODO

	if (mSocket != BOFSOCKET_INVALID)
	{
		//Option_S = _rOption_S;

		Rts_E = Bof_ResolveIpAddress(_rTarget_S, Ip_X, mDstIpAddress_X);
		if (Rts_E == BOF_ERR_NO_ERROR)
		{
			if (mBofSocketParam_X.BroadcastPort_U16)
			{
				if (IsIpV6_B)
				{
					mDstIpAddress_X.IpV6_B = true;
					memset(mDstIpAddress_X.IpV6Address_X.sin6_addr.s6_addr, 0xFF, 16);
					mDstIpAddress_X.IpV6Address_X.sin6_port = htons(mBofSocketParam_X.BroadcastPort_U16);
					mDstIpAddress_X.IpV6Address_X.sin6_family = AF_INET6;
				}
				else
				{
					mDstIpAddress_X.IpV6_B = false;
					mDstIpAddress_X.IpV4Address_X.sin_addr.s_addr = 0xFFFFFFFF;
					mDstIpAddress_X.IpV4Address_X.sin_port = htons(mBofSocketParam_X.BroadcastPort_U16);
					mDstIpAddress_X.IpV4Address_X.sin_family = AF_INET;
				}
			}
/*No it is INADDR_ANY	
			if (Bof_IsIpAddressNull(mSrcIpAddress_X) )
			{
				Rts_E = BOF_ERR_INVALID_DST;
			}
			else 
*/
			if (mSrcIpAddress_X.SocketType_E != mDstIpAddress_X.SocketType_E)
			{
				Rts_E = BOF_ERR_BAD_TYPE;
			}
			else if (mSrcIpAddress_X.ProtocolType_E != mDstIpAddress_X.ProtocolType_E)
			{
				Rts_E = BOF_ERR_PROTOCOL;
			}
		}
	}


	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		if (IsUdp())
		{
			//TO=100;CON=HELLO WORLD
			//TimeoutInMs_U32 = (Bof_GetUnsignedIntegerFromMultipleKeyValueString(_rOption_S, ";", "TO", '=', Val_U32) == BOF_ERR_NO_ERROR) ? Val_U32 : 100;
			Option_S = (Bof_GetStringFromMultipleKeyValueString(_rOption_S, ";", "CON", '=', Val_S) == BOF_ERR_NO_ERROR) ? Val_S : BOFSOCKET_UDP_CONNECT;

			ProcessUdpConnectProtocol_B = (mBofSocketParam_X.BroadcastPort_U16 == 0);
			if (ProcessUdpConnectProtocol_B)
			{
				Rts_E = BOF_ERR_ENOTCONN;
				if (IsIpV6_B)
				{
					Nb_i = static_cast<int>(sendto(mSocket, Option_S.c_str(), static_cast<int>(Option_S.size()), 0, reinterpret_cast<BOF_SOCKADDR *> (&mDstIpAddress_X.IpV6Address_X), sizeof(BOF_SOCKADDR_IN6)));
				}
				else
				{
					Nb_i = static_cast<int>(sendto(mSocket, Option_S.c_str(), static_cast<int>(Option_S.size()), 0, reinterpret_cast<BOF_SOCKADDR *> (&mDstIpAddress_X.IpV4Address_X), sizeof(BOF_SOCKADDR_IN)));
				}
				if (Nb_i > 0)
				{
					Rts_E = SetWriteTimeout(_TimeoutInMs_U32);
					if (Rts_E == BOF_ERR_NO_ERROR)
					{
						Rts_E = BOF_ERR_READ;
						if (IsIpV6_B)
						{
							Len = sizeof(mDstIpAddress_X.IpV6Address_X);
							Nb_i = static_cast<int>(recvfrom(mSocket, pUdpConnect_c, sizeof(pUdpConnect_c), 0, reinterpret_cast<BOF_SOCKADDR *> (&mDstIpAddress_X.IpV6Address_X), &Len));
						}
						else
						{
							Len = sizeof(mDstIpAddress_X.IpV4Address_X);
							Nb_i = static_cast<int>(recvfrom(mSocket, pUdpConnect_c, sizeof(pUdpConnect_c), 0, reinterpret_cast<BOF_SOCKADDR *> (&mDstIpAddress_X.IpV4Address_X), &Len));
						}
						if (Nb_i > 0)
						{
							Rts_E = BOF_ERR_INVALID_CHECKSUM;
							pUdpConnect_c[Nb_i] = 0;
							if (!memcmp(Option_S.c_str(), pUdpConnect_c, Nb_i))
							{
								Rts_E = BOF_ERR_NO_ERROR;
							}
						}
					}
				}
			}
			else
			{
				Rts_E = BOF_ERR_NO_ERROR;
			}
		}
		else
		{
			Blocking_B = mBofSocketParam_X.BaseChannelParam_X.Blocking_B;
			if (Blocking_B)
			{
				Rts_E = SetNonBlockingMode(true);
			}
			if (Rts_E == BOF_ERR_NO_ERROR)
			{
				Rts_E = BOF_ERR_ENOTCONN;
				if (IsIpV6_B)
				{
					Err_i = connect(mSocket, reinterpret_cast<BOF_SOCKADDR *> (&mDstIpAddress_X.IpV6Address_X), sizeof(mDstIpAddress_X.IpV6Address_X));
				}
				else
				{
					Err_i = connect(mSocket, reinterpret_cast<BOF_SOCKADDR *> (&mDstIpAddress_X.IpV4Address_X), sizeof(mDstIpAddress_X.IpV4Address_X));
				}

				if (Err_i < 0)
				{
          Rts_E = Bof_GetLastError(true);
					if ((Rts_E == BOF_ERR_EINPROGRESS) || (Rts_E == BOF_ERR_EWOULDBLOCK))
					{
						Rts_E = SetWriteTimeout(_TimeoutInMs_U32);
						if (Rts_E == BOF_ERR_NO_ERROR)
						{
							Len = sizeof(Err_i);
							getsockopt(mSocket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&Err_i), &Len);
							if (Err_i)
							{
								Rts_E = BOF_ERR_ENOTCONN;
							}
						}
					}
				}
				else
				{
					Rts_E = BOF_ERR_NO_ERROR;
				}
				if (Blocking_B)
				{
					SetNonBlockingMode(false);
				}
			}
		}

		if (Rts_E == BOF_ERR_NO_ERROR)
		{
			if (IsIpV6_B)
			{
				Len = sizeof(mSrcIpAddress_X.IpV6Address_X);
				if (getsockname(mSocket, reinterpret_cast<BOF_SOCKADDR *> (&mSrcIpAddress_X.IpV6Address_X), &Len) == SOCKET_ERROR)
				{
					Rts_E = BOF_ERR_ENOTCONN;
				}
				else
				{
					mConnected_B = true;
				}
			}
			else
			{
				Len = sizeof(mSrcIpAddress_X.IpV4Address_X);
				if (getsockname(mSocket, reinterpret_cast<BOF_SOCKADDR *> (&mSrcIpAddress_X.IpV4Address_X), &Len) == SOCKET_ERROR)
				{
					Rts_E = BOF_ERR_ENOTCONN;
				}
				else
				{
					mConnected_B = true;
				}
			}
		}
	}

	return Rts_E;
}


/*** V_Listen ********************************************************************/

/*!
 * Description
 * The V_Listen method uses a socket placed in a state where it is listening for an incoming
 * connection (ListeningState_B=true during BofSocket::BofSocket).It waits or not until an incomming
 * connection is detected on the socket. To be able not to wait for incoming connection
 * (WaitUntilConnect_B=false), the socket must be created in non blocking mode
 *
 *
 * Parameters
 * _TimeoutInMs_U32: Specifies the maximum time allowed to perform the operation
 * _Flag_U32: Specify command options. Bit 0: For udp listen it will purge the socket buffer and
 * connect to the last Udp v_connect client request
 * _pUdpDataSizeMax_U32:  For Udp server, it specifies the maximum size of the _pUdpData_U8 buffer
 * _pUdpData_U8:  For Udp server, it is used to receive welcome message from udp station. It is first
 * filled by the caller with a zero terminated string which contains the expected connect string
 *
 * If no _pUdpDataSizeMax_U32 or _pUdpData_U8 is specified on a UDP socket default value are used.
 * if _pUdpData_U8 contains a ':' we just compare connect string up to this char
 *
 * Returns
 * void *: A handle to use if the operation is successful
 *
 * Remarks
 * None
 */
BofComChannel *BofSocket::V_Listen(uint32_t _TimeoutInMs_U32, const std::string & /*_rOption_S*/)
{
	BOFERR Sts_E;
	int NbUdpRcv_i, Nb_i;
	socklen_t Len;
	BOFSOCKET DstSocket_X;
	BOF_SOCKET_ADDRESS DstIpAddress_X;
	BofComChannel *pRts = nullptr;
	//std::vector<std::string> OptionList_S;
	std::string ConnectString_S;
	BofSocket *pClient;

	BOF_SOCKET_PARAM BofSocketParam_X;
	char pUdpConnect_c[0x10000];
	BOF_SOCKET_ADDRESS IpAddress_X;
	bool IsIpV6_B = false;      //TODO

	if (mSocket != BOFSOCKET_INVALID)
	{
		NbUdpRcv_i = 0;
		DstSocket_X = BOFSOCKET_INVALID;
		DstIpAddress_X.Reset();
		//	OptionList_S=Bof_StringSplit(_rOption_S, ";");

		if (IsUdp())
		{
//			if (Option_S == "")
			{
				ConnectString_S = BOFSOCKET_UDP_CONNECT;
			}
			Sts_E = SetReadTimeout(_TimeoutInMs_U32);
			if (Sts_E == BOF_ERR_NO_ERROR)
			{
				if (IsIpV6_B)
				{
					Len = sizeof(DstIpAddress_X.IpV6Address_X);
					NbUdpRcv_i = static_cast<int>(recvfrom(mSocket, pUdpConnect_c, sizeof(pUdpConnect_c), 0, reinterpret_cast<BOF_SOCKADDR *> (&DstIpAddress_X.IpV6Address_X), &Len));
				}
				else
				{
					Len = sizeof(DstIpAddress_X.IpV4Address_X);
					NbUdpRcv_i = static_cast<int>(recvfrom(mSocket, pUdpConnect_c, sizeof(pUdpConnect_c), 0, reinterpret_cast<BOF_SOCKADDR *> (&DstIpAddress_X.IpV4Address_X), &Len));
				}
				if (NbUdpRcv_i > 0)
				{
					pUdpConnect_c[NbUdpRcv_i] = 0;
					if (!memcmp(ConnectString_S.c_str(), pUdpConnect_c, NbUdpRcv_i))
					{
						if (IsIpV6_B)
						{
							DstSocket_X = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
						}
						else
						{
							DstSocket_X = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
						}
						Sts_E = BOF_ERR_NO_ERROR;
					}
				}
				else
				{
					Sts_E = BOF_ERR_INVALID_CHECKSUM;
				}
			}
		}
		else
		{
			Sts_E = SetReadTimeout(_TimeoutInMs_U32);
			if (Sts_E == BOF_ERR_NO_ERROR)
			{
				if (IsIpV6_B)
				{
					Len = sizeof(DstIpAddress_X.IpV6Address_X);
					DstSocket_X = accept(mSocket, reinterpret_cast<BOF_SOCKADDR *> (&DstIpAddress_X.IpV6Address_X), &Len);
				}
				else
				{
					Len = sizeof(DstIpAddress_X.IpV4Address_X);
					DstSocket_X = accept(mSocket, reinterpret_cast<BOF_SOCKADDR *> (&DstIpAddress_X.IpV4Address_X), &Len);
				}
				if (DstSocket_X != BOFSOCKET_INVALID)
				{
					Sts_E = BOF_ERR_NO_ERROR;
				}

			}
		}

		if (Sts_E == BOF_ERR_NO_ERROR)
		{
			BOF_ASSERT(DstSocket_X != BOFSOCKET_INVALID);
			//Sts_E                                                 = BOF_ERR_ENOTCONN;
			IpAddress_X = mSrcIpAddress_X;
			if (IsIpV6_B)
			{
				IpAddress_X.IpV6_B = true;
				IpAddress_X.IpV6Address_X.sin6_port = 0;
				mSrcIpAddress_X.IpV6Address_X.sin6_port = 0;
			}
			else
			{
				IpAddress_X.IpV6_B = false;
				IpAddress_X.IpV4Address_X.sin_port = 0;
				mSrcIpAddress_X.IpV4Address_X.sin_port = 0;
			}
			BofSocketParam_X.BindIpAddress_S = Bof_BuildIpAddress(IpAddress_X, mSrcIpAddress_X);
			BofSocketParam_X.BaseChannelParam_X.Blocking_B = mBofSocketParam_X.BaseChannelParam_X.Blocking_B;
			BofSocketParam_X.BroadcastPort_U16 = 0;
			BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
			BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = mBofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32;
			BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = mBofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32;
			BofSocketParam_X.BaseChannelParam_X.ChannelName_S = Bof_Sprintf("%s_%d", mBofSocketParam_X.BaseChannelParam_X.ChannelName_S.c_str(), ++S_mListenCounter_U32);
			pRts = new BofSocket(DstSocket_X, BofSocketParam_X);

			if ((pRts) &&
			    (!pRts->LastErrorCode())
				)
			{
				pClient = dynamic_cast<BofSocket *>(pRts);
				DstIpAddress_X.SocketType_E = mSrcIpAddress_X.SocketType_E;
				DstIpAddress_X.ProtocolType_E = mSrcIpAddress_X.ProtocolType_E;
				pClient->SetDstIpAddress(DstIpAddress_X);
				//Sts_E = BOF_ERR_NO_ERROR;

				pClient->SetConnectedState(true);
				if (IsUdp())
				{
					Sts_E = SetWriteTimeout(_TimeoutInMs_U32);
					if (Sts_E == BOF_ERR_NO_ERROR)
					{
						if (IsIpV6_B)
						{
							Nb_i = static_cast<int>(sendto(DstSocket_X, pUdpConnect_c, NbUdpRcv_i, 0, reinterpret_cast<BOF_SOCKADDR *> (&DstIpAddress_X.IpV6Address_X), sizeof(BOF_SOCKADDR_IN6)));
						}
						else
						{
							Nb_i = static_cast<int>(sendto(DstSocket_X, pUdpConnect_c, NbUdpRcv_i, 0, reinterpret_cast<BOF_SOCKADDR *> (&DstIpAddress_X.IpV4Address_X), sizeof(BOF_SOCKADDR_IN)));
						}
						if (Nb_i < 0)
						{
							//		Sts_E = BOF_ERR_WRITE;
							BOF_SAFE_DELETE(pRts);
						}
					}
				}
			}
			else
			{
				BOF_SAFE_DELETE(pRts);
			}
		}
	}
	return pRts;
}

/*** V_ReadData ********************************************************************/

/*!
 * Description
 * The V_Read method reads a number of bytes from an TCP socket.
 *
 *
 * Parameters
 * _TimeoutInMs_U32: Specifies the maximum time allowed to perform the operation
 * pNb_U32:     Specifies and returns the number of bytes to read
 * pBuffer_U8: Specifies a buffer where the data read will be stored.
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 */

BOFERR BofSocket::V_ReadData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, uint8_t *_pBuffer_U8)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	uint32_t Nb_U32;
	socklen_t Len_i;
	size_t Size;
	bool IsIpV6_B = false;      //TODO

	if (mSocket != BOFSOCKET_INVALID)
	{
		Rts_E = BOF_ERR_EINVAL;
		if (_pBuffer_U8)
		{
			if (_rNb_U32)
			{
				Rts_E = SetReadTimeout(_TimeoutInMs_U32);
				if (Rts_E == BOF_ERR_NO_ERROR)
				{
					Rts_E = BOF_ERR_READ;
					Size = _rNb_U32;
					_rNb_U32 = 0;
					if (IsUdp())
					{
//Under linux if you read a datagram bigger than the buff you got the first byte of the buffer
//Under windows the read fails->need to use wsarecfrom to get this behavior MSG_TRUNK is not valid as flag in recvfrom !!!!
						if (IsIpV6_B)
						{
							Len_i = sizeof(mDstIpAddress_X.IpV6Address_X);
							Nb_U32 = static_cast<uint32_t>(recvfrom(mSocket, reinterpret_cast<char *> (_pBuffer_U8), static_cast<int>(Size), 0, reinterpret_cast<BOF_SOCKADDR *> (&mDstIpAddress_X.IpV6Address_X), &Len_i));
						}
						else
						{
							Len_i = sizeof(mDstIpAddress_X.IpV4Address_X);
							Nb_U32 = static_cast<uint32_t>(recvfrom(mSocket, reinterpret_cast<char *> (_pBuffer_U8), static_cast<int>(Size), 0, reinterpret_cast<BOF_SOCKADDR *> (&mDstIpAddress_X.IpV4Address_X), &Len_i));
						}
//MSG_TRUNC(since Linux 2.2): seems also to exist in windows
//For  UNIX datagram(since Linux 3.4) sockets and raw(AF_PACKET), Internet datagram(since Linux 2.4.27 / 2.6.8) and netlink(since Linux 2.6.22): return the real length of the packet or datagram, even when it was longer 
//than the passed buffer.Not implemented for UNIX domain(unix(7)) sockets.
/*
						if (static_cast<int> (Nb_U32) > 0) // https://stackoverflow.com/questions/2416944/can-read-function-on-a-connected-socket-return-zero-bytes: No, you should consider -1 as an error and 0 as a normal disconnect, and close the socket in either case. �
						{
							if (Nb_U32 > Size)
							{
								Nb_U32 = 0xFFFFFFFF;
							}
						}
*/
					}
					else
					{
						if (IsIpV6_B)
						{
							Len_i = sizeof(mDstIpAddress_X.IpV6Address_X);
							Nb_U32 = static_cast<uint32_t>(recvfrom(mSocket, reinterpret_cast<char *> (_pBuffer_U8), static_cast<int>(Size), 0, reinterpret_cast<BOF_SOCKADDR *> (&mDstIpAddress_X.IpV6Address_X), &Len_i));
						}
						else
						{
							//						Nb_U32 = recv(mSocket, reinterpret_cast< char * > (_pBuffer_U8), Size, 0);
							Len_i = sizeof(mDstIpAddress_X.IpV4Address_X);
							Nb_U32 = static_cast<uint32_t>(recvfrom(mSocket, reinterpret_cast<char *> (_pBuffer_U8), static_cast<int>(Size), 0, reinterpret_cast<BOF_SOCKADDR *> (&mDstIpAddress_X.IpV4Address_X), &Len_i));
						}
					}
					if (static_cast< int > (Nb_U32) > 0) // https://stackoverflow.com/questions/2416944/can-read-function-on-a-connected-socket-return-zero-bytes: No, you should consider -1 as an error and 0 as a normal disconnect, and close the socket in either case. �
					{
						_rNb_U32 = Nb_U32;
						Rts_E = BOF_ERR_NO_ERROR;
					}
					else
					{
	//No as SetReadTimeout was ok					mConnected_B = false;
	//					Rts_E = BOF_ERR_ENETRESET;
						Rts_E = BOF_ERR_TOO_SMALL;	//Partial read->buffer to small
					}
				}
			}
		}
	}

	return Rts_E;
}


/*** V_WriteData ********************************************************************/

/*!
 * Description
 * The V_Write method writes a number of bytes to an TCP channel.
 *
 *
 * Parameters
 * _TimeoutInMs_U32: Specifies the maximum time allowed to perform the operation
 * pNb_U32:     Specifies and returns the number of bytes to write
 * pBuffer_U8: Specifies a buffer containing the data to write.
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 */
BOFERR BofSocket::V_WriteData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, const uint8_t *pBuffer_U8)
{
	uint32_t i_U32, Total_U32, NbLoop_U32, LastOne_U32, Nb_U32;
	BOFERR Rts_E = BOF_ERR_INIT;
	bool IsIpV6_B = false;      //TODO

	Total_U32 = 0;
	if (mSocket != BOFSOCKET_INVALID)
	{
		Rts_E = BOF_ERR_EINVAL;
		if (pBuffer_U8)
		{
			if (_rNb_U32)
			{
				if (IsUdp())
				{
#if 0
	SCATTER_GATHER_BUFFER pScatterGatherBuffer_X[4096];	//Up to 256MB in udp
					std::vector<BOF_BUFFER> BufferCollection;
					uint32_t NbBuffer_U32, NbByteWritten_U32;
					uint64_t Total_U64;
					BOF_BUFFER Buffer_X(_rNb_U32, _rNb_U32, const_cast<uint8_t *>(pBuffer_U8));
					BufferCollection.push_back(Buffer_X);
					Rts_E = ComputeScatterGatherList(BufferCollection, BOF_NB_ELEM_IN_ARRAY(pScatterGatherBuffer_X), pScatterGatherBuffer_X, NbBuffer_U32, Total_U64);
					if (Rts_E == BOF_ERR_NO_ERROR)
					{
						Rts_E=WriteScatterGatherData(_TimeoutInMs_U32, BufferCollection, NbByteWritten_U32);
					}
#else
					Total_U32 = _rNb_U32;

					if (mMaxUdpLen_U32)
					{
						NbLoop_U32 = Total_U32 / mMaxUdpLen_U32;
					}
					else
					{
						NbLoop_U32 = 0;
					}
					LastOne_U32 = Total_U32 - (NbLoop_U32 * mMaxUdpLen_U32);
					Total_U32 = 0;

					Rts_E = BOF_ERR_NO_ERROR;
					for (i_U32 = 0; i_U32 < NbLoop_U32; i_U32++)
					{
						Rts_E = SetWriteTimeout(_TimeoutInMs_U32);
						if (Rts_E == BOF_ERR_NO_ERROR)
						{
							Rts_E = BOF_ERR_WRITE;
							if (IsIpV6_B)
							{
								Nb_U32 = static_cast<uint32_t>(sendto(mSocket, reinterpret_cast<const char *>(&pBuffer_U8[Total_U32]), mMaxUdpLen_U32, 0, (BOF_SOCKADDR *) &mDstIpAddress_X.IpV6Address_X,
								                                      sizeof(mDstIpAddress_X.IpV6Address_X)));
							}
							else
							{
								Nb_U32 = static_cast<uint32_t>(sendto(mSocket, reinterpret_cast<const char *>(&pBuffer_U8[Total_U32]), mMaxUdpLen_U32, 0, (BOF_SOCKADDR *) &mDstIpAddress_X.IpV4Address_X,
								                                      sizeof(mDstIpAddress_X.IpV4Address_X)));
							}
							if (Nb_U32 == mMaxUdpLen_U32)
							{
								Rts_E = BOF_ERR_NO_ERROR;
								Total_U32 += Nb_U32;
							}
						}
						if (Rts_E != BOF_ERR_NO_ERROR)
						{
							break;
						}
					}

					if ((Rts_E == BOF_ERR_NO_ERROR) &&
					    (LastOne_U32)
						)
					{
						Rts_E = SetWriteTimeout(_TimeoutInMs_U32);
						if (Rts_E == BOF_ERR_NO_ERROR)
						{
							Rts_E = BOF_ERR_WRITE;
							if (IsIpV6_B)
							{
								Nb_U32 = static_cast<uint32_t>(sendto(mSocket, reinterpret_cast<const char *>(&pBuffer_U8[Total_U32]), LastOne_U32, 0, reinterpret_cast<BOF_SOCKADDR *> (&mDstIpAddress_X.IpV6Address_X), sizeof(mDstIpAddress_X.IpV6Address_X)));
							}
							else
							{
								Nb_U32 = static_cast<uint32_t>(sendto(mSocket, reinterpret_cast<const char *>(&pBuffer_U8[Total_U32]), LastOne_U32, 0, reinterpret_cast<BOF_SOCKADDR *> (&mDstIpAddress_X.IpV4Address_X), sizeof(mDstIpAddress_X.IpV4Address_X)));
							}
							if (Nb_U32 == LastOne_U32)
							{
								Rts_E = BOF_ERR_NO_ERROR;
								Total_U32 += Nb_U32;
							}
						}
					}
#endif
				}
				else
				{
					Total_U32 = 0;
					do
					{
						Rts_E = SetWriteTimeout(_TimeoutInMs_U32);
						if (Rts_E == BOF_ERR_NO_ERROR)
						{
							Rts_E = BOF_ERR_WRITE;
							Nb_U32 = static_cast<uint32_t>(send(mSocket, reinterpret_cast<const char *>(&pBuffer_U8[Total_U32]), (_rNb_U32 - Total_U32), 0));
							if (static_cast< int > (Nb_U32) > 0)  //>= 0)	// https://stackoverflow.com/questions/2416944/can-read-function-on-a-connected-socket-return-zero-bytes: No, you should consider -1 as an error and 0 as a normal disconnect, and close the socket in either case. �
							{
								Total_U32 += Nb_U32;
								Rts_E = BOF_ERR_NO_ERROR;
							}
							else
							{
						    mConnected_B = false;
								Rts_E = BOF_ERR_ENOTCONN;
							}
						}
					} while ((Rts_E == BOF_ERR_NO_ERROR) && (Total_U32 != _rNb_U32));
				}
			}
			else
			{
				Rts_E = BOF_ERR_NO_ERROR;
				Total_U32 = 0;
			}
		}
	}
	_rNb_U32 = Total_U32;
	return Rts_E;
}


BOFERR BofSocket::V_WaitForDataToRead(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPendingByte_U32)
{
	BOFERR Rts_E;
	BOF_COM_CHANNEL_STATUS Status_X;

	_rNbPendingByte_U32 = 0;
	Rts_E = SetReadTimeout(_TimeoutInMs_U32);
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		Rts_E = V_GetStatus(Status_X);
		if (Rts_E == BOF_ERR_NO_ERROR)
		{
			_rNbPendingByte_U32 = Status_X.NbIn_U32;
		}
	}
	return Rts_E;
}

BOFERR BofSocket::WaitForDataToReadOrSpaceToWrite(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPendingByte_U32, bool &_rCanWrite_B)
{
	BOFERR Rts_E;
	BOF_COM_CHANNEL_STATUS Status_X;
	bool ReadDataPending_B, SpaceAvailableForWrite_B;

	_rNbPendingByte_U32 = 0;
	Rts_E = SetReadOrWriteTimeout(_TimeoutInMs_U32, ReadDataPending_B, SpaceAvailableForWrite_B);
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		_rCanWrite_B = SpaceAvailableForWrite_B;
		if (ReadDataPending_B)
		{
			Rts_E = V_GetStatus(Status_X);
			if (Rts_E == BOF_ERR_NO_ERROR)
			{
				_rNbPendingByte_U32 = Status_X.NbIn_U32;
			}
		}
	}
	return Rts_E;
}

BOFERR BofSocket::V_WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S, uint32_t &_rNb_U32)
{
  _rNb_U32 = static_cast<uint32_t>(_rBuffer_S.size());
	return V_WriteData(_TimeoutInMs_U32, _rNb_U32, reinterpret_cast<const uint8_t *>(_rBuffer_S.c_str()));
}

BOFERR BofSocket::ComputeScatterGatherList(const std::vector<BOF_BUFFER> &_rBufferCollection, uint32_t _MaxNumberOfScatterGatherEntry_U32, SCATTER_GATHER_BUFFER  *_pScatterGatherBuffer_X, uint32_t &_rNbBuffer_U32, uint64_t &_rTotal_U64)
{
	BOFERR Rts_E = BOF_ERR_EINVAL;
	uint32_t i_U32, j_U32,Nb_U32,MaxLen_U32, NbLoop_U32, LastOne_U32, NbBuffer_U32;
	uint8_t *pData_U8;

	_rTotal_U64 = 0;
	_rNbBuffer_U32 = 0;
	if (_pScatterGatherBuffer_X)
	{
		Rts_E = BOF_ERR_TOO_SMALL;
		if (_rBufferCollection.size()) 
		{
			Rts_E = BOF_ERR_TOO_BIG;
			if (_rBufferCollection.size() <= _MaxNumberOfScatterGatherEntry_U32)
			{
				MaxLen_U32 = (IsUdp()) ? mMaxUdpLen_U32 : 0xFFFFFFFF;
				NbBuffer_U32 = 0;
				for (i_U32 = 0; i_U32 < _rBufferCollection.size(); i_U32++)
				{
					Nb_U32 = static_cast<uint32_t>(_rBufferCollection[i_U32].Size_U64);
					Rts_E = BOF_ERR_EINVAL;
					if ((Nb_U32) && (_rBufferCollection[i_U32].pData_U8))
					{
						if (MaxLen_U32)
						{
							NbLoop_U32 = Nb_U32 / MaxLen_U32;
						}
						else
						{
							NbLoop_U32 = 0;
						}
						LastOne_U32 = Nb_U32 - (NbLoop_U32 * MaxLen_U32);
						Rts_E = BOF_ERR_TOO_SMALL;
						if ((NbBuffer_U32 + NbLoop_U32 + (LastOne_U32) ? (uint32_t)1 : (uint32_t)0) < _MaxNumberOfScatterGatherEntry_U32)
						{
							Rts_E = BOF_ERR_NO_ERROR;
							_rTotal_U64 += Nb_U32;
							pData_U8 = _rBufferCollection[i_U32].pData_U8;
							for (j_U32 = 0; j_U32 < NbLoop_U32; j_U32++)
							{
#if defined(_WIN32)
								_pScatterGatherBuffer_X[NbBuffer_U32].len = MaxLen_U32;
								_pScatterGatherBuffer_X[NbBuffer_U32].buf = reinterpret_cast<char *>(pData_U8);
#else
								_pScatterGatherBuffer_X[NbBuffer_U32].iov_len = MaxLen_U32;
								_pScatterGatherBuffer_X[NbBuffer_U32].iov_base = reinterpret_cast<char *>(pData_U8);
#endif
								pData_U8 += MaxLen_U32;
								NbBuffer_U32++;
							}
							if (LastOne_U32)
							{
#if defined(_WIN32)
								_pScatterGatherBuffer_X[NbBuffer_U32].len = LastOne_U32;
								_pScatterGatherBuffer_X[NbBuffer_U32].buf = reinterpret_cast<char *>(pData_U8);
#else
								_pScatterGatherBuffer_X[NbBuffer_U32].iov_len = LastOne_U32;
								_pScatterGatherBuffer_X[NbBuffer_U32].iov_base = reinterpret_cast<char *>(pData_U8);
#endif
								pData_U8 += LastOne_U32;
								NbBuffer_U32++;
							}
						} //if ((Index_U32 + NbLoop_U32 + (LastOne_U32) ? 1 : 0) < BOF_MAX_NUMBER_OF_SCATTER_GATHER_SOCKET_BUFFER)
					} //if ((Nb_U32) && (_rBufferCollection[i_U32].pData_U8))
					if (Rts_E != BOF_ERR_NO_ERROR)
					{
						break;
					}
				} //for (i_U32 = 0; i_U32 < _rBufferCollection.size(); i_U32++)
				_rNbBuffer_U32 = NbBuffer_U32;
			}
		}
	}
	return Rts_E;
}
BOFERR BofSocket::WriteScatterGatherData(uint32_t _TimeoutInMs_U32, const std::vector<BOF_BUFFER> &_rBufferCollection, uint32_t &_rNbByteWritten_U32)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	bool IsIpV6_B = false;      //TODO
	uint32_t NbBuffer_U32;
	uint64_t Total_U64;

#if defined(_WIN32)
	DWORD		NbByteSent,Flag;
#else
	ssize_t NbByteSent;
	struct msghdr Msg_X;
#endif
	_rNbByteWritten_U32 = 0;
	if (mSocket != BOFSOCKET_INVALID)
	{
		Rts_E = ComputeScatterGatherList(_rBufferCollection, mBofSocketParam_X.MaxNumberOfScatterGatherEntry_U32,mpScatterGatherBuffer_X, NbBuffer_U32, Total_U64);
		if (Rts_E == BOF_ERR_NO_ERROR)
		{
			Rts_E = SetWriteTimeout(_TimeoutInMs_U32);
			if (Rts_E == BOF_ERR_NO_ERROR)
			{
				Rts_E = BOF_ERR_WRITE;
				if (IsUdp())
				{
#if defined(_WIN32)
					NbByteSent = 0;
					Flag = 0;
					if (IsIpV6_B)
					{
						if (WSASendTo(mSocket, mpScatterGatherBuffer_X, NbBuffer_U32, &NbByteSent, Flag, (BOF_SOCKADDR *)&mDstIpAddress_X.IpV6Address_X, sizeof(mDstIpAddress_X.IpV6Address_X), nullptr, nullptr) == 0)
						{
							Rts_E = BOF_ERR_WRONG_SIZE;
							if (NbByteSent == Total_U64)
							{
								Rts_E = BOF_ERR_NO_ERROR;
							}
						}
					}
					else
					{
						if (WSASendTo(mSocket, mpScatterGatherBuffer_X, NbBuffer_U32, &NbByteSent, Flag, (BOF_SOCKADDR *)&mDstIpAddress_X.IpV4Address_X, sizeof(mDstIpAddress_X.IpV4Address_X), nullptr, nullptr) == 0)
						{
							Rts_E = BOF_ERR_WRONG_SIZE;
							if (NbByteSent == Total_U64)
							{
								Rts_E = BOF_ERR_NO_ERROR;
							}
						}
					}
					/*
					int uu;
					Bof_GetLastError(true, &uu);
					Bof_GetLastError(true, &uu);
					*/
#else

					if (IsIpV6_B)
					{
						Msg_X.msg_name = (BOF_SOCKADDR *)&mDstIpAddress_X.IpV6Address_X;
						Msg_X.msg_namelen = sizeof(mDstIpAddress_X.IpV6Address_X);
					}
					else
					{
						Msg_X.msg_name = (BOF_SOCKADDR *)&mDstIpAddress_X.IpV4Address_X;
						Msg_X.msg_namelen = sizeof(mDstIpAddress_X.IpV4Address_X);
					}
					Msg_X.msg_iovlen = NbBuffer_U32;
					Msg_X.msg_iov = mpScatterGatherBuffer_X;
					Msg_X.msg_control = nullptr;
					Msg_X.msg_controllen = 0;
					Msg_X.msg_flags = 0;
					NbByteSent = sendmsg(mSocket, &Msg_X, 0);
					Rts_E = BOF_ERR_WRONG_SIZE;
					if (NbByteSent == static_cast<int64_t>(Total_U64))
					{
						Rts_E = BOF_ERR_NO_ERROR;
					}

#endif
					if (NbByteSent > 0)
					{
						_rNbByteWritten_U32 = static_cast<uint32_t>(NbByteSent);
					}
				}
//						else
//faire le cas tcp
			}
		}
	}
	return Rts_E;
}

BOFERR BofSocket::ReadScatterGatherData(uint32_t _TimeoutInMs_U32, const std::vector<BOF_BUFFER> &_rBufferCollection, uint32_t &_rNbByteRead_U32, bool &_rPartialRead_B)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	bool IsIpV6_B = false;      //TODO
	uint32_t NbBuffer_U32;
	uint64_t Total_U64;

#if defined(_WIN32)
	DWORD		NbByteRead,Flag;
	INT Len;
	int Err_i;
	int32_t NativeErrorCode_S32;
#else
	ssize_t NbByteRead;
	struct msghdr Msg_X;
#endif
	_rNbByteRead_U32 = 0;
	_rPartialRead_B = false;
	if (mSocket != BOFSOCKET_INVALID)
	{
		Rts_E = ComputeScatterGatherList(_rBufferCollection, mBofSocketParam_X.MaxNumberOfScatterGatherEntry_U32, mpScatterGatherBuffer_X, NbBuffer_U32, Total_U64);
		if (Rts_E == BOF_ERR_NO_ERROR)
		{
			Rts_E = SetReadTimeout(_TimeoutInMs_U32);
			if (Rts_E == BOF_ERR_NO_ERROR)
			{
				Rts_E = BOF_ERR_READ;
				if (IsUdp())
				{
#if defined(_WIN32)
					NbByteRead = 0;
					Flag = MSG_PARTIAL;
					if (IsIpV6_B)
					{
						Len = sizeof(mDstIpAddress_X.IpV6Address_X);
						Err_i = WSARecvFrom(mSocket, mpScatterGatherBuffer_X, NbBuffer_U32, &NbByteRead, &Flag, (BOF_SOCKADDR *)&mDstIpAddress_X.IpV6Address_X, &Len, nullptr, nullptr);
					}
					else
					{
						Len = sizeof(mDstIpAddress_X.IpV4Address_X);
						Err_i = WSARecvFrom(mSocket, mpScatterGatherBuffer_X, NbBuffer_U32, &NbByteRead, &Flag, (BOF_SOCKADDR *)&mDstIpAddress_X.IpV4Address_X, &Len, nullptr, nullptr);
					}
					Bof_GetLastError(true, &NativeErrorCode_S32);
					if (NativeErrorCode_S32 == WSAEMSGSIZE)
					{
						_rPartialRead_B = true;
						Err_i = 0;
					}
					if (Err_i==0)
					{
						Rts_E = BOF_ERR_WRONG_SIZE;
						if (NbByteRead == Total_U64)
						{
							Rts_E = BOF_ERR_NO_ERROR;
						}
					}
#else

					if (IsIpV6_B)
					{
						Msg_X.msg_name = (BOF_SOCKADDR *)&mDstIpAddress_X.IpV6Address_X;
						Msg_X.msg_namelen = sizeof(mDstIpAddress_X.IpV6Address_X);
					}
					else
					{
						Msg_X.msg_name = (BOF_SOCKADDR *)&mDstIpAddress_X.IpV4Address_X;
						Msg_X.msg_namelen = sizeof(mDstIpAddress_X.IpV4Address_X);
					}
					Msg_X.msg_iovlen = NbBuffer_U32;
					Msg_X.msg_iov = mpScatterGatherBuffer_X;
					Msg_X.msg_control = nullptr;
					Msg_X.msg_controllen = 0;
					Msg_X.msg_flags = 0;
					NbByteRead = recvmsg(mSocket, &Msg_X, 0);
					if (Msg_X.msg_flags & MSG_TRUNC)
					{
						_rPartialRead_B = true;
					}

					Rts_E = BOF_ERR_WRONG_SIZE;
					if (NbByteRead == static_cast<int64_t>(Total_U64))
					{
						Rts_E = BOF_ERR_NO_ERROR;
					}

#endif
					if (NbByteRead > 0)
					{
						_rNbByteRead_U32 = static_cast<uint32_t>(NbByteRead);
					}
				}
				//						else
				//faire le cas tcp
			}
		}
	}
	return Rts_E;
}


BOFERR BofSocket::ReadString(uint32_t _TimeoutInMs_U32, std::string &_rStr_S, char _EolDelimiter_c)
{
	BOFERR Rts_E = BOF_ERR_INIT, Sts_E;
	uint32_t Nb_U32, NbToRead_U32;
	socklen_t Len_i;
	size_t Size;
	char pBuffer_c[0x10000], *p_c;
	bool IsIpV6_B = false;      //TODO

	if (mSocket != BOFSOCKET_INVALID)
	{
		Rts_E = SetReadTimeout(_TimeoutInMs_U32);
		if (Rts_E == BOF_ERR_NO_ERROR)
		{
			Rts_E = BOF_ERR_READ;
			Size = sizeof(pBuffer_c) - 1;
			if (IsUdp())
			{
				if (IsIpV6_B)
				{
					Len_i = sizeof(mDstIpAddress_X.IpV6Address_X);
					Nb_U32 = static_cast<uint32_t>(recvfrom(mSocket, pBuffer_c, static_cast<int>(Size), 0, reinterpret_cast<BOF_SOCKADDR *> (&mDstIpAddress_X.IpV6Address_X), &Len_i));
				}
				else
				{
					Len_i = sizeof(mDstIpAddress_X.IpV4Address_X);
					Nb_U32 = static_cast<uint32_t>(recvfrom(mSocket, pBuffer_c, static_cast<int>(Size), 0, reinterpret_cast<BOF_SOCKADDR *> (&mDstIpAddress_X.IpV4Address_X), &Len_i));
				}
			}
			else
			{
				Nb_U32 = static_cast<uint32_t>(recv(mSocket, pBuffer_c, static_cast<int>(Size), MSG_PEEK));
			}
			if (static_cast< int > (Nb_U32) > 0)
			{
				pBuffer_c[Nb_U32] = 0;
				p_c = strchr(pBuffer_c, _EolDelimiter_c);
				if (p_c)
				{
					if (IsUdp())
					{
						Rts_E = BOF_ERR_NO_ERROR;
					}
					else
					{
						NbToRead_U32 = static_cast<uint32_t>(p_c - pBuffer_c + 1);
						BOF_ASSERT(NbToRead_U32 < sizeof(pBuffer_c));
						Rts_E = V_ReadData(0, NbToRead_U32, reinterpret_cast<uint8_t *>(pBuffer_c));
						BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
						BOF_ASSERT(NbToRead_U32 == static_cast<uint32_t>(p_c - pBuffer_c + 1));
						BOF_ASSERT(pBuffer_c[NbToRead_U32 - 1] == _EolDelimiter_c);
						if (Rts_E == BOF_ERR_NO_ERROR)
						{
							pBuffer_c[NbToRead_U32] = 0;
							_rStr_S = pBuffer_c;
						}
					}
				}
				else
				{
//if (Nb_U32>= Size)  data is not present in this big buffer->Cancel it
					Rts_E = (Nb_U32 >= Size) ? BOF_ERR_NOT_FOUND : BOF_ERR_ETIMEDOUT;
					if (Rts_E == BOF_ERR_NOT_FOUND)
					{
						Sts_E = V_ReadData(0, Nb_U32, reinterpret_cast<uint8_t *>(pBuffer_c));
						BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
						BOF_ASSERT(Nb_U32 >= Size);
					}
				}
			}
		}
	}
	return Rts_E;
}


/*** V_GetStatus ********************************************************************/

/*!
 * Description
 * The V_GetStatus method returns the status of a TCP socket.
 *
 *
 * Parameters
 * pStatus_X: \Returns socket status information
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 */
BOFERR BofSocket::V_GetStatus(BOF_COM_CHANNEL_STATUS &_rStatus_X)
{
	BOFERR Rts_E = BOF_ERR_NO_ERROR;	// , Sts_E;
	int PollStatus_i;
//	bool DataAvailableOnRead_B=false;
	bool SocketIsWritable_B = false;
	struct pollfd Fds_X;

	_rStatus_X.Reset();
	_rStatus_X.Connected_B = mConnected_B;
	_rStatus_X.Sts_E = mConnected_B ? BOF_ERR_NO_ERROR : BOF_ERR_ENOTCONN;
	if (mSocket != BOFSOCKET_INVALID)
	{
		if (IsTcp())
		{
			Fds_X.fd = mSocket;
			Fds_X.events = (BOF_POLL_OUT | BOF_POLL_RDHUP);
//			Fds_X.events = (BOF_POLL_IN | BOF_POLL_RDHUP);
#if defined (_WIN32)
			PollStatus_i = WSAPoll(&Fds_X, 1, 0);	// == 1); // Better than select (==1 can also be BOF_POLL_ERR, BOF_POLL_HUP, or BOF_POLL_NVAL)
#else
			PollStatus_i = poll(&Fds_X, 1, 0);    // Better than select (==1 can also be BOF_POLL_ERR, BOF_POLL_HUP, or BOF_POLL_NVAL)
#endif

			if (PollStatus_i > 0)
			{
				//Error which can be set also with BOF_POLL_IN (Data channel write and closed)
				if (Fds_X.revents & (BOF_POLL_RDHUP | BOF_POLL_ERR | BOF_POLL_HUP | BOF_POLL_NVAL))
				{
					if (!(Fds_X.revents & BOF_POLL_OUT))
					{
						PollStatus_i = -1;
					}
				}
			}
			if (PollStatus_i > 0)
			{
				//DataAvailableOnRead_B = true;
				SocketIsWritable_B = true;
				Rts_E = BOF_ERR_NO_ERROR;
			}
			else if (PollStatus_i == 0)
			{
				Rts_E = BOF_ERR_ETIMEDOUT;
				Rts_E = BOF_ERR_NO_ERROR;		//Normal
			}
			else
			{
				mConnected_B = false;
				_rStatus_X.Sts_E = BOF_ERR_ENETRESET;
				_rStatus_X.Connected_B = false;
				Rts_E = BOF_ERR_NO_ERROR;
			}
		}
		BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);	//DataAvailableOnRead_B makes the difference

#if defined (_WIN32)
		u_long Nb;
		Rts_E = (ioctlsocket(mSocket, FIONREAD, &Nb) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
		if (Rts_E == BOF_ERR_NO_ERROR)
		{
			_rStatus_X.NbIn_U32 = (uint32_t)Nb;
		}
#else
		int Nb;
		Rts_E = (ioctl(mSocket, FIONREAD, &Nb) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
		if (Rts_E == BOF_ERR_NO_ERROR)
		{
			_rStatus_X.NbIn_U32 = (uint32_t) Nb;
		}
#endif



		/*
		In TCP there is only one way to detect an orderly disconnect, and that is by getting zero as a return value from read()/recv()/recvXXX() when reading.
		There is also only one reliable way to detect a broken connection: by writing to it. After enough writes to a broken connection, TCP will have done enough retries and timeouts to
		know that it's broken and will eventually cause write()/send()/sendXXX() to return -1 with an errno/WSAGetLastError() value of ECONNRESET, or in some cases 'connection timed out'.
		Note that the latter is different from 'connect timeout', which can occur in the connect phase.

		You should also set a reasonable read timeout, and drop connections that fail it.

		The answer here about ioctl() and FIONREAD is compete nonsense. All that does is tell you how many bytes are presently in the socket receive buffer, available to be read without blocking.
		If a client doesn't send you anything for five minutes that doesn't constitute a disconnect, but it does cause FIONREAD to be zero. Not the same thing: not even close.
		*/

		/*
		Not true: https://stackoverflow.com/questions/283375/detecting-tcp-client-disconnect  ????????

		mDstIpAddress_X.Reset();
		*/

		//if ((IsTcp()) && (mConnected_B) && (Rts_E == BOF_ERR_NO_ERROR))
		if ((IsTcp()) && (Rts_E == BOF_ERR_NO_ERROR))
		{
			//if ((DataAvailableOnRead_B) && (_rStatus_X.NbIn_U32 == 0))
			if ((SocketIsWritable_B) && (_rStatus_X.NbIn_U32 == 0))
			{
					mConnected_B = false;
					_rStatus_X.Sts_E = BOF_ERR_ENETRESET;
					_rStatus_X.Connected_B = false;
			}
		}


	}

	return Rts_E;
}


/*** V_FlushData ********************************************************************/

/*!
 * Description
 * The V_Purge method flushes the receive buffer of a TCP socket.
 *
 *
 * Parameters
 * _TimeoutInMs_U32: Specifies the maximum time allowed to perform the operation
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 */
BOFERR BofSocket::V_FlushData(uint32_t _TimeoutInMs_U32)
{
	BOFERR Rts_E;
	uint32_t Nb_U32, Start_U32, Delta_U32;
	uint8_t pBuffer_U8[0x1000];

	Start_U32 = Bof_GetMsTickCount();
	do
	{
		Nb_U32 = sizeof(pBuffer_U8);
		Rts_E = V_ReadData(_TimeoutInMs_U32, Nb_U32, pBuffer_U8);
		Delta_U32 = Bof_ElapsedMsTime(Start_U32);
		if (Delta_U32 >= _TimeoutInMs_U32)
		{
			Rts_E = BOF_ERR_ETIMEDOUT;
		}

	} while (Rts_E == BOF_ERR_NO_ERROR);

	Rts_E = BOF_ERR_NO_ERROR;
	return Rts_E;
}


BOFERR BofSocket::SetDstIpAddress(BOF_SOCKET_ADDRESS &_rDstIpAddress_X)
{
	BOFERR Rts_E = BOF_ERR_NO_ERROR;
	mDstIpAddress_X = _rDstIpAddress_X;
	return Rts_E;
}

BOF_SOCKET_ADDRESS BofSocket::GetSrcIpAddress()
{
	return mSrcIpAddress_X;
}

BOF_SOCKET_ADDRESS BofSocket::GetDstIpAddress()
{
	return mDstIpAddress_X;

}

BOFERR BofSocket::SetReadTimeout(uint32_t _TimeoutInMs_U32)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	struct pollfd Fds_X;
	int PollStatus_i;

	if (mSocket != BOFSOCKET_INVALID)
	{
		Fds_X.fd = mSocket;
		Fds_X.events = (BOF_POLL_IN | BOF_POLL_RDHUP);
#if defined (_WIN32)
		PollStatus_i = WSAPoll(&Fds_X, 1, _TimeoutInMs_U32);	// == 1); // Better than select (==1 can also be BOF_POLL_ERR, BOF_POLL_HUP, or BOF_POLL_NVAL)
#else
		PollStatus_i = poll(&Fds_X, 1, _TimeoutInMs_U32);    // Better than select (==1 can also be BOF_POLL_ERR, BOF_POLL_HUP, or BOF_POLL_NVAL)
#endif
		if (PollStatus_i > 0)
		{
			//Error which can be set also with BOF_POLL_IN (Data channel write and closed)
			if (Fds_X.revents & (BOF_POLL_RDHUP | BOF_POLL_ERR | BOF_POLL_HUP | BOF_POLL_NVAL))
			{
				if (!(Fds_X.revents & BOF_POLL_IN))
				{
					PollStatus_i = -1;
				}
			}
		}
		if (PollStatus_i > 0)
		{
			Rts_E = BOF_ERR_NO_ERROR;
		} 
		else if (PollStatus_i == 0)
		{
			Rts_E = BOF_ERR_ETIMEDOUT;
		}
		else
		{
			mConnected_B = false;
			Rts_E = BOF_ERR_ENETRESET;
		}
	}
	return Rts_E;
}

BOFERR BofSocket::SetWriteTimeout(uint32_t _TimeoutInMs_U32)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	struct pollfd Fds_X;
	int PollStatus_i;

	if (mSocket != BOFSOCKET_INVALID)
	{
		Fds_X.fd = mSocket;
		Fds_X.events = (BOF_POLL_OUT | BOF_POLL_RDHUP);
#if defined (_WIN32)
		PollStatus_i = (WSAPoll(&Fds_X, 1, _TimeoutInMs_U32) == 1); // Better than select (==1 can also be BOF_POLL_ERR, BOF_POLL_HUP, or BOF_POLL_NVAL)
#else
		PollStatus_i = (poll(&Fds_X, 1, _TimeoutInMs_U32) == 1);    // Better than select (==1 can also be BOF_POLL_ERR, BOF_POLL_HUP, or BOF_POLL_NVAL)
#endif
		if (PollStatus_i > 0)
		{
			//Error which can be set also with BOF_POLL_IN (Data channel write and closed)
			if (Fds_X.revents & (BOF_POLL_RDHUP | BOF_POLL_ERR | BOF_POLL_HUP | BOF_POLL_NVAL))
			{
				if (!(Fds_X.revents & BOF_POLL_OUT))
				{
					PollStatus_i = -1;
				}
			}
		}
		if (PollStatus_i > 0)
		{
			Rts_E = BOF_ERR_NO_ERROR;
		}
		else if (PollStatus_i == 0)
		{
			Rts_E = BOF_ERR_ETIMEDOUT;
		}
		else
		{
			mConnected_B = false;
			Rts_E = BOF_ERR_ENETRESET;
		}
	}
	return Rts_E;
}

BOFERR BofSocket::SetReadOrWriteTimeout(uint32_t _TimeoutInMs_U32, bool &_ReadDataPending_B, bool &_SpaceAvailableForWrite_B)
{
	BOFERR Rts_E = BOF_ERR_INIT;
	struct pollfd Fds_X;
	int PollStatus_i;

	_ReadDataPending_B = false;
	_SpaceAvailableForWrite_B = false;
	if (mSocket != BOFSOCKET_INVALID)
	{
		Fds_X.fd = mSocket;
		Fds_X.events = (BOF_POLL_IN | BOF_POLL_RDHUP | POLLWRNORM);
#if defined (_WIN32)
		PollStatus_i = WSAPoll(&Fds_X, 1, _TimeoutInMs_U32);	// == 1); // Better than select (==1 can also be BOF_POLL_ERR, BOF_POLL_HUP, or BOF_POLL_NVAL)
#else
		PollStatus_i = poll(&Fds_X, 1, _TimeoutInMs_U32);    // Better than select (==1 can also be BOF_POLL_ERR, BOF_POLL_HUP, or BOF_POLL_NVAL)
#endif
		if (PollStatus_i > 0)
		{
			if (Fds_X.revents & (BOF_POLL_RDHUP | BOF_POLL_ERR | BOF_POLL_HUP | BOF_POLL_NVAL))
			{
				PollStatus_i = -1;
			}
		}
		if (PollStatus_i > 0)
		{
			_ReadDataPending_B = (Fds_X.revents & BOF_POLL_IN) ? true : false;;
			_SpaceAvailableForWrite_B = (Fds_X.revents & POLLWRNORM) ? true : false;
			Rts_E = BOF_ERR_NO_ERROR;
		}
		else if (PollStatus_i == 0)
		{
			Rts_E = BOF_ERR_ETIMEDOUT;
		}
		else
		{
			mConnected_B = false;
			Rts_E = BOF_ERR_ENETRESET;
		}
	}
	return Rts_E;
}

BOFERR BofSocket::SetTimeoutOption(int _Option_i, uint32_t _TimeoutInMs_U32)
{
	BOFERR Rts_E = BOF_ERR_NO_ERROR;
	int Len_i;
	const char *pData_c;

#if defined (_WIN32)
	pData_c      = (const char *)&_TimeoutInMs_U32;
	Len_i        = sizeof(uint32_t);
#else
	struct timeval To_X;

	To_X.tv_sec = (_TimeoutInMs_U32 / 1000);
	To_X.tv_usec = (_TimeoutInMs_U32 % 1000) * 1000;

	pData_c = (const char *) &To_X;
	Len_i = sizeof(To_X);
#endif

	if (setsockopt(mSocket, SOL_SOCKET, _Option_i, pData_c, Len_i) == SOCKET_ERROR)
	{
		Rts_E = BOF_ERR_EINVAL;
	}

	return Rts_E;
}


BOFSOCKET BofSocket::GetSocketHandle() const
{
	return mSocket;
}

uint32_t BofSocket::GetMaxUdpLen() const
{
	return mMaxUdpLen_U32;
}

bool BofSocket::IsTcp() const
{
	return mSrcIpAddress_X.SocketType_E == BOF_SOCK_TYPE::BOF_SOCK_TCP;
}

bool BofSocket::IsUdp() const
{
	return mSrcIpAddress_X.SocketType_E == BOF_SOCK_TYPE::BOF_SOCK_UDP;
}

void BofSocket::SetConnectedState(bool _ConnectedState_B)
{
	mConnected_B = _ConnectedState_B;
}
/*!
 * Description
 * This function sets the interface to send multicast on for this socket
 *
 *
 * Parameters
 * _pInterface_c
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * Obsolete
 */

#if 1

BOFERR BofSocket::SetSendInterfaceForMulticast(const BOF_SOCKET_ADDRESS &_rInterfaceIpAddress_X)
{
	BOFERR Rts_E = BOF_ERR_BAD_TYPE;
	BOF_IN_ADDR IpV4InterfaceAddress_X;
	BOF_IN_ADDR6 IpV6InterfaceAddress_X;
	bool IsIpV6_B = false;      //TODO

	if (mBofSocketParam_X.MulticastSender_B)
	{
		Rts_E = BOF_ERR_INVALID_COMMAND;
		if (IsIpV6_B)
		{
			IpV6InterfaceAddress_X = _rInterfaceIpAddress_X.IpV6Address_X.sin6_addr;

			if (setsockopt(mSocket, IPPROTO_IP, IP_MULTICAST_IF, reinterpret_cast<char *> (&IpV6InterfaceAddress_X), sizeof(IpV6InterfaceAddress_X)) == 0)
			{
				Rts_E = BOF_ERR_NO_ERROR;
			}
		}
		else
		{
			IpV4InterfaceAddress_X = _rInterfaceIpAddress_X.IpV4Address_X.sin_addr;

			if (setsockopt(mSocket, IPPROTO_IP, IP_MULTICAST_IF, reinterpret_cast<char *> (&IpV4InterfaceAddress_X), sizeof(IpV4InterfaceAddress_X)) == 0)
			{
				Rts_E = BOF_ERR_NO_ERROR;
			}
		}
	}
	return Rts_E;
}

#else
BOFERR BofSocket::SetSendInterfaceForMulticast(const std::string & _rInterface_S)
{
	BOFERR       Rts_E    = BOF_ERR_INVALID_COMMAND;
	unsigned int Index_ui = 0;

	if (mBofSocketParam_X.MulticastSender_B)
	{
		Index_ui = if_nametoindex(_rInterface_S.c_str() );

		if (Index_ui != 0)
		{
#if defined (_WIN32)
			DWORD           Index_dw;
			Index_dw           = Index_ui;
			Rts_E              = ( (setsockopt(this->mSocket, IPPROTO_IP, IP_MULTICAST_IF, (const char *)&Index_dw, sizeof(Index_dw) ) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_OPERATION_FAILED);
#else
			struct ip_mreqn Addr_X;
			memset(&Addr_X, 0, sizeof(Addr_X) );
			Addr_X.imr_ifindex = Index_ui;
			Rts_E              = ( (setsockopt(this->mSocket, IPPROTO_IP, IP_MULTICAST_IF, &Addr_X, sizeof(Addr_X) ) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_OPERATION_FAILED);
#endif
		}
		else
		{
			Rts_E = BOF_ERR_EINVAL;
		}
	}
	return Rts_E;
}
#endif


BofSocketThread::BofSocketThread(const std::string &_rName_S, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF_THREAD_PRIORITY _ThreadPriority_E) : BofThread()
{
//	BOFERR Sts_E;

	mSocketOperationParam_X.Reset();
	mSocketOperationResult_X.Reset();
	mExit_B = false;
	mInUse_B = false;
	mDone_B = false;
	mCancel_B = false;
	mOldBlockingMode_B = false;
	LaunchBofProcessingThread(_rName_S, false, 0, _ThreadSchedulerPolicy_E, _ThreadPriority_E,0, 2000,0);
}

BofSocketThread::~BofSocketThread()
{
//	StopThread();
}

BOFERR BofSocketThread::ProgramSocketOperation(BOF_SOCKET_OPERATION_PARAM &_rParam_X)
{
	BOFERR Ret_E = BOF_ERR_EINVAL;

	// No operation is currently pending
	if (!mInUse_B)
	{
		// Get the parameters
		mSocketOperationParam_X = _rParam_X;

		mInUse_B = true;
		mDone_B = false;
		mCancel_B = false;
		mExit_B = false;
		// Launch the thread
		Ret_E = SignalThreadWakeUpEvent();
	}
	return Ret_E;
}


bool BofSocketThread::IsOperationDone()
{
	return mDone_B;
}


bool BofSocketThread::IsOperationPending()
{
	return mInUse_B;
}


BOFERR BofSocketThread::CancelOperation()
{
	BOFERR Rts_E = BOF_ERR_NOT_RUNNING;

	if (IsOperationPending())
	{
		mCancel_B = true;
		Rts_E = BOF_ERR_NO_ERROR;
	}
	return Rts_E;
}


BOFERR BofSocketThread::GetParams(BOF_SOCKET_OPERATION_PARAM &_rParam_X)
{
	BOFERR Rts_E;

	_rParam_X = mSocketOperationParam_X;
	Rts_E = BOF_ERR_NO_ERROR;
	return Rts_E;
}

BOFERR BofSocketThread::GetResult(BOF_SOCKET_OPERATION_RESULT &_rResult_X)
{
	BOFERR Rts_E;

	_rResult_X = mSocketOperationResult_X;
	Rts_E = BOF_ERR_NO_ERROR;
	return Rts_E;
}

BOFERR BofSocketThread::ClearOperation()
{
	BOFERR Rts_E;

	mSocketOperationParam_X.Reset();
	mSocketOperationResult_X.Reset();
	mInUse_B = false;
	mDone_B = false;
	mCancel_B = false;
	Rts_E = BOF_ERR_NO_ERROR;
	return Rts_E;
}

BOFERR BofSocketThread::V_OnProcessing()
{
	uint32_t Size_U32;
	BOFERR Rts_E = BOF_ERR_NO_ERROR;
	uint32_t Total_U32;
	uint8_t *pBuf_U8;

	while (!IsThreadLoopMustExit())
	{
		Rts_E = WaitForThreadWakeUpEvent(0xFFFFFFFF);
		if (Rts_E == BOF_ERR_NO_ERROR)
		{
			if (!IsThreadLoopMustExit())
			{
				// Grab parameters
				Size_U32 = mSocketOperationParam_X.Size_U32;
				pBuf_U8 = mSocketOperationParam_X.pBuffer_U8;
				Total_U32 = 0;

				// Set socket to blocking mode
				mOldBlockingMode_B = mSocketOperationParam_X.pSocket_O->IsBlocking();
				if (!mOldBlockingMode_B)
				{
					mSocketOperationParam_X.pSocket_O->SetNonBlockingMode(false);
				}
				do
				{
					if (mSocketOperationParam_X.Read_B)
					{
						Rts_E = mSocketOperationParam_X.pSocket_O->V_ReadData(100, Size_U32, pBuf_U8);
					}
					else
					{
						Rts_E = mSocketOperationParam_X.pSocket_O->V_WriteData(100, Size_U32, pBuf_U8);
					}
					if (Rts_E == BOF_ERR_NO_ERROR)
					{
						Total_U32 += Size_U32;
						pBuf_U8 += Size_U32;
						Size_U32 = (mSocketOperationParam_X.Size_U32 - Total_U32);
					}
					if (Rts_E == BOF_ERR_ETIMEDOUT)
					{
						Rts_E = BOF_ERR_NO_ERROR;
					}

				} while (!mExit_B && !mCancel_B && (Total_U32 < mSocketOperationParam_X.Size_U32) && (Rts_E == BOF_ERR_NO_ERROR));

				if (!mOldBlockingMode_B)
				{
					mSocketOperationParam_X.pSocket_O->SetNonBlockingMode(mOldBlockingMode_B);
				}

				// Act that operation was cancelled
				if (mCancel_B)
				{
					Rts_E = BOF_ERR_STOPPED;
				}

				// Set results
				mSocketOperationResult_X.Sts_E = Rts_E;
				mSocketOperationResult_X.Size_U32 = Total_U32;

				// Flag it as done
				mDone_B = true;
			}
		}
	}

	return Rts_E;
}

#if _WIN32
#pragma warning(pop)
#endif


END_BOF_NAMESPACE()