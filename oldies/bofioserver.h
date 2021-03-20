/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module contains routines for creating and managing a io channel server (socket/pip")
 *
 * Name:        bofioserver.h
 * Author:      Bernard HARMEL: b.harmel@belgacom.net
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Oct 13 2017  BHA : Initial release
 */
#pragma once


/*** Include ****************************************************************/
#include <map>
//#include <bofstd/ibofiosessionfactory.h>
//#include <bofstd/bofiosession.h>
#include <bofstd/bofio.h>
#include <bofstd/bofthread.h>
#include <bofstd/bofsocketos.h>

BEGIN_BOF_NAMESPACE()
class BofIoSession;

class IBofIoSessionFactory;
/*** Defines ****************************************************************/

/*** Macro ****************************************************************/

/*** Structures *************************************************************/

struct BOF_IO_SERVER_PARAM
{
		BOF_IO_PARAM BofIoParam_X;

		int32_t ThreadPolicy_i;
		int32_t ThreadPriority_i;
		uint32_t ThreadStartStopTimeoutInMs_U32;

		bool ReUseAddress_B;
		std::string Address_S;
		uint32_t NbMaxSession_U32;
		uint32_t SessionRcvBufferSize_U32;
		uint32_t SessionSndBufferSize_U32;

		BOF_IO_SERVER_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			BofIoParam_X.Reset();

			ThreadPolicy_i = BOF_THREAD_POLICY_OTHER;
			ThreadPriority_i = BOF_THREAD_DEFAULT_PRIORITY;
			ThreadStartStopTimeoutInMs_U32 = 5000;

			ReUseAddress_B = true;
			Address_S = "";
			NbMaxSession_U32 = 1;
			SessionRcvBufferSize_U32 = 0x10000;
			SessionSndBufferSize_U32 = 0x10000;
		}
};


struct BOF_UV_SESSION
{
		BofIoSession *pBofIoSession;
		uv_tcp_t *pUvTcp_X;

		BOF_UV_SESSION()
		{
			Reset();
		}

		void Reset()
		{
			pBofIoSession = nullptr;
			pUvTcp_X = nullptr;
		}
};


class BofIoServer : public BofThread, public BofIo
{
private:
		BOF_SOCK_TYPE mIoSocketType_E;
		BOF_MUTEX mMtx;
		BOF_IO_SERVER_PARAM mBofIoServerParam_X;
		std::map<BofIoSession *, BOF_UV_SESSION> mSessionCollection;
		IBofIoSessionFactory *mpIBofIoSessionFactory;
		std::unique_ptr<uv_tcp_t> mpuTcpServer_X;

public:
		BofIoServer(const BOF_IO_SERVER_PARAM &_rBofSocketServerParam_X, IBofIoSessionFactory *_pIBofIoSessionFactory);

		virtual ~BofIoServer();

		BofIoServer &operator=(const BofIoServer &) = delete; // Disallow copying
		BofIoServer(const BofIoServer &) = delete;

		uint32_t GetNbConnectedSession() const;

		BOFERR CloseSession(BofIoSession *pBofIoSession);

		BOFERR SimulateConnectionRequest(BofIoSession **_ppBofIoSession);

		BOFERR SimulateDisconnectionRequest(BofIoSession *_pBofIoSession);

		BOFERR SessionDisconnected(BofIoSession *_pBofIoSession);

		BOFERR WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S);

		BOFERR WriteData(uint32_t _TimeoutInMs_U32, uint32_t _Nb_U32, const uint8_t *_pBuffer_U8);

private:
		BOFERR V_OnProcessing() override;

		BOFERR V_OnUvAsync(BOF_UV_CALLBACK *_pBofUvCallback, uv_async_t *_pHandle) override;

		BOFERR V_OnUvTimer(BOF_UV_CALLBACK *_pBofUvCallback, uv_timer_t *_pHandle) override;

		BOFERR V_OnUvAlloc(BOF_UV_CALLBACK *_pBofUvCallback, uv_handle_t *_pHandle, size_t _SuggestedSize, uv_buf_t *_pBuffer_X) override;

//	BOFERR				V_OnUvClose(BOF_UV_CALLBACK *_pBofUvCallback, uv_handle_t *_pHandle) override;
		BOFERR V_OnUvRead(BOF_UV_CALLBACK *_pBofUvCallback, uv_stream_t *_pStream_X, ssize_t _NbRead, const uv_buf_t *_pBuffer_X) override;

		BOFERR V_OnUvWrite(BOF_UV_CALLBACK *_pBofUvCallback, uv_write_t *_pReq_X, int _Status_i) override;

		BOFERR V_OnUvShutdown(BOF_UV_CALLBACK *_pBofUvCallback, uv_shutdown_t *_pReq_X, int _Status_i) override;

		BOFERR V_OnUvConnection(BOF_UV_CALLBACK *_pBofUvCallback, uv_stream_t *_pServer, int _Status_i) override;
};


END_BOF_NAMESPACE()