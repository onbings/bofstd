/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines routines for creating and managing a socket communication
 * channel.
 *
 * Name:        bofsocketserver.h
 * Author:      Bernard HARMEL: b.harmel@belgacom.net
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Aug 21 2001  BHA : Initial release
 */
#pragma once


/*** Include ****************************************************************/
#include <vector>
#include <bofstd/bofsocket.h>
#include <bofstd/bofsocketsession.h>
#include <bofstd/ibofsocketsessionfactory.h>
#include <bofstd/bofthread.h>

BEGIN_BOF_NAMESPACE()

/*** Defines ****************************************************************/

/*** Macro ****************************************************************/

/*** Structures *************************************************************/

struct BOF_SOCKET_SERVER_PARAM
{
		std::string Name_S;
		int32_t ThreadPolicy_i;
		int32_t ThreadPriority_i;
		uint32_t ThreadStartStopTimeoutInMs_U32;
		uint32_t ThreadStackSizeInByte_U32; //0 for default
		uint32_t IoTimeoutInMs_U32;
		bool ReUseAddress_B;

		std::string Address_S;
		bool Blocking_B;
		uint32_t NbMaxSession_U32;
		uint32_t SessionRcvBufferSize_U32;
		uint32_t SessionSndBufferSize_U32;


		BOF_SOCKET_SERVER_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			Name_S = "";
			ThreadPolicy_i = BOF_THREAD_POLICY_OTHER;
			ThreadPriority_i = BOF_THREAD_DEFAULT_PRIORITY;
			ThreadStartStopTimeoutInMs_U32 = 5000;
			ThreadStackSizeInByte_U32 = 0;
			IoTimeoutInMs_U32 = 100;
			ReUseAddress_B = true;

			Address_S = "";
			Blocking_B = true;
			NbMaxSession_U32 = 1;
			SessionRcvBufferSize_U32 = 0x10000;
			SessionSndBufferSize_U32 = 0x10000;
		}
};

class BofSocketServer : public BofThread
{
private:
		BOF_SOCKET_SERVER_PARAM mBofSocketServerParam_X;
		//BOF_SOCKADDR_IN            mIpV4ListenAddress_X;
		//BOF_SOCKADDR_IN6           mIpV6ListenAddress_X;
		BofSocket *mpListen;
		std::vector<BofSocketSession *> mSessionCollection;
		BOF_MUTEX mMtx;
		IBofSocketSessionFactory *mpIBofSocketSessionFactory;
		BOFERR mErrorCode_E;

public:
		BofSocketServer(IBofSocketSessionFactory *_pIBofSocketSessionFactory, const BOF_SOCKET_SERVER_PARAM &_rBofSocketServerParam_X);

		virtual ~BofSocketServer();

		BofSocketServer &operator=(const BofSocketServer &) = delete; // Disallow copying
		BofSocketServer(const BofSocketServer &) = delete;

//Sends the passed data to all currently connected sessions.
		BOFERR LastErrorCode();

		uint32_t IoTimeoutInMs();

		BOFERR WriteData(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, const std::string &_rBuffer_S, void *_pUserArg);

		BOFERR WriteData(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, uint32_t _Nb_U32, const uint8_t *_pBuffer_U8, void *_pUserArg);

		template<typename InputIterator>
		BOFERR WriteData(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, InputIterator _First, InputIterator _Last, void *_pUserArg);

		BOFERR SessionDisconnected(BofSocketSession *_pSession);

		uint32_t GetNbConnectedSession() const;

		BOFERR CloseSession(uint32_t _Index_U32);

		BofSocketSession *GetSocketSession(uint32_t _Index_U32);

		//Simulation
		BOFERR SimulateConnectionRequest(BofSocketSession **_ppSession);

		BOFERR SimulateDisconnectionRequest(BofSocketSession *_pSession);

private:
		BOFERR SessionAccepted(BofSocket *_pBofSocket);

		BOFERR WaitForConnection(uint32_t _MaxTimeToWait_U32, const std::string &_rOption_S, BofSocket **_ppConnectionSocket);

		BOFERR V_OnProcessing() override;
};

template<typename InputIterator>
inline BOFERR BofSocketServer::WriteData(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, InputIterator _First, InputIterator _Last, void *_pUserArg)
{
	BOFERR Rts_E;
	uint32_t Nb_U32;

	std::vector<uint8_t> Array;;
	std::copy(_First, _Last, std::back_inserter(Array));
	Nb_U32 = Array.size();
	Rts_E = WriteData(_TimeoutInMs_U32, _AsyncMode_B, Nb_U32, Array.data(), _pUserArg);
	return Rts_E;
}
END_BOF_NAMESPACE()