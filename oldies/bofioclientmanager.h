/*
* Copyright (c) 2000-2006, Sci. All rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
* PURPOSE.
*
* This module contains routines for creating and managing io channel client (socket/pip")
*
* Name:        boficlientmanagerr.h
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
#include <uv.h>

#include <bofstd/bofioclient.h>
#include <bofstd/bofio.h>
#include <bofstd/bofthread.h>
#include <bofstd/bofsocketos.h>

BEGIN_BOF_NAMESPACE()

/*** Defines ****************************************************************/

/*** Macro ****************************************************************/

/*** Structures *************************************************************/

struct BOF_IO_CLIENT_MANAGER_PARAM
{
		BOF_SOCK_TYPE IoSocketType_E;
		BOF_IO_PARAM BofIoParam_X;
		int32_t ThreadPolicy_i;
		int32_t ThreadPriority_i;
		uint32_t ThreadStartStopTimeoutInMs_U32;

		BOF_IO_CLIENT_MANAGER_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			IoSocketType_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
			BofIoParam_X.Reset();
			ThreadPolicy_i = BOF_THREAD_POLICY_OTHER;
			ThreadPriority_i = BOF_THREAD_DEFAULT_PRIORITY;
			ThreadStartStopTimeoutInMs_U32 = 5000;
		}
};

struct BOF_UV_CLIENT
{
		std::shared_ptr<BofIoClient> psBofIoClient;
		uv_tcp_t *pUvTcp_X;
		uv_connect_t *pUvConnect_X;

		uint32_t RcvBufferSize_U32;
		uint32_t SndBufferSize_U32;
		uint32_t Timeout_U32;

		uint32_t StartConnectTime_U32;

		BOF_UV_CLIENT()
		{
			Reset();
		}

		void Reset()
		{
			psBofIoClient = nullptr;
			pUvTcp_X = nullptr;
			pUvConnect_X = nullptr;
			RcvBufferSize_U32 = 0;
			SndBufferSize_U32 = 0;
			Timeout_U32 = 100;
			StartConnectTime_U32 = 0;
		}
};


class BofIoClientManager : public BofThread, public BofIo
{
private:
//	BOF_SOCK_TYPE mIoSocketType_E;
		BOF_MUTEX mMtx;
		BOF_IO_CLIENT_MANAGER_PARAM mBofIoClientManagerParam_X;
		std::map<BofIoClient *, BOF_UV_CLIENT> mClientCollection;
//	BOFERR mErrorCode_E;

public:
		BofIoClientManager(const BOF_IO_CLIENT_MANAGER_PARAM &_rBofIoClientManagerParam_X);

		virtual ~BofIoClientManager();

		BofIoClientManager &operator=(const BofIoClientManager &) = delete; // Disallow copying
		BofIoClientManager(const BofIoClientManager &) = delete;

		BOF_SOCK_TYPE IoSocketType();

		BOFERR AddClient(std::shared_ptr<BofIoClient> _psBofIoClient, const std::string &_rName_S, uint32_t _RcvBufferSize_U32, uint32_t _SndBufferSize_U32);

		BOFERR Connect(std::shared_ptr<BofIoClient> _psBofIoClient, const BOF_IO_CONNECT_PARAM &_rBofIoConnectParam_X);

		BOFERR Disconnect(std::shared_ptr<BofIoClient> _psBofIoClient);

private:
		BOFERR RemoveClient(BofIoClient *_pBofIoClient);

		BOFERR V_OnProcessing() override;

		BOFERR V_OnUvAsync(BOF_UV_CALLBACK *_pBofUvCallback, uv_async_t *_pHandle) override;

		BOFERR V_OnUvTimer(BOF_UV_CALLBACK *_pBofUvCallback, uv_timer_t *_pHandle) override;

		BOFERR V_OnUvAlloc(BOF_UV_CALLBACK *_pBofUvCallback, uv_handle_t *_pHandle, size_t _SuggestedSize, uv_buf_t *_pBuffer_X) override;

//	BOFERR				V_OnUvClose(BOF_UV_CALLBACK *_pBofUvCallback, uv_handle_t *_pHandle) override;
		BOFERR V_OnUvRead(BOF_UV_CALLBACK *_pBofUvCallback, uv_stream_t *_pStream_X, ssize_t _NbRead, const uv_buf_t *_pBuffer_X) override;

		BOFERR V_OnUvWrite(BOF_UV_CALLBACK *_pBofUvCallback, uv_write_t *_pReq_X, int _Status_i) override;

		BOFERR V_OnUvShutdown(BOF_UV_CALLBACK *_pBofUvCallback, uv_shutdown_t *_pReq_X, int _Status_i) override;

		BOFERR V_OnUvConnect(BOF_UV_CALLBACK *_pBofUvCallback, uv_connect_t *_pReq_X, int _Status_i);

};
END_BOF_NAMESPACE()