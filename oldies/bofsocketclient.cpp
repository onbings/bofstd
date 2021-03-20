/*
* Copyright (c) 2015-2025, Onbings. All rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
* PURPOSE.
*
* This module implements the BofSocketClient class.
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
#include <bofstd/bofsocketclient.h>
#include <bofstd/bofstringformatter.h>

BEGIN_BOF_NAMESPACE();
BofSocketClient::BofSocketClient(IBofSocketIo *_pIBofSocketIo, const BOF_SOCKET_CLIENT_PARAM & _rBofSocketClientParam_X)
	:mpIBofSocketIo(_pIBofSocketIo), BofSocketIo(_rBofSocketClientParam_X.IoParam_X)
{
	if (mErrorCode_E == BOFERR_NO_ERROR)
	{
		if (mpIBofSocketIo == nullptr)
		{
			mErrorCode_E = BOFERR_INIT;
		}
		else
		{
		}
	}
} 

BofSocketClient::~BofSocketClient() 
{
}

BOFERR BofSocketClient::Connect(const BOF_SOCKET_CONNECT_PARAM &_rBofSocketConnectParam_X)
{
	BOFERR Rts_E = BOFERR_INIT;
	//Rts_E = StartAsyncConnect(_rBofSocketConnectParam_X);
	return Rts_E;
}
BOFERR BofSocketClient::Disconnect()
{
	BOFERR Rts_E = BOFERR_INIT;
	//Rts_E = StartAsyncDisconnect();
	return Rts_E;
}

BOFERR BofSocketClient::V_DataRead(uint32_t _Nb_U32, const uint8_t *_pBuffer_U8)
{
	BOFERR Rts_E = BOFERR_INIT;

	if (mpIBofSocketIo)
	{
		Rts_E = mpIBofSocketIo->V_SignalDataRead(_Nb_U32, _pBuffer_U8);
	}
	return Rts_E;
}
BOFERR BofSocketClient::V_DataWritten(BOFERR _Sts_E, uint32_t _NbRemain_U32, const uint8_t *_pRemainingBuffer_U8, void *_pUserArg)
{
	BOFERR Rts_E = BOFERR_INIT;

	if (mpIBofSocketIo)
	{
		Rts_E = mpIBofSocketIo->V_SignalDataWritten(_Sts_E, _NbRemain_U32, _pRemainingBuffer_U8, _pUserArg);
	}
	return Rts_E;
}
BOFERR BofSocketClient::V_RemoteSocketClosed()
{
	BOFERR Rts_E = BOFERR_INIT;

	if (mpIBofSocketIo)
	{
		Rts_E = mpIBofSocketIo->V_SignalRemoteSocketClosed();
	}
	return Rts_E;
}
BOFERR BofSocketClient::V_TryToConnect(bool _Running_B, uint32_t _Try_U32, uint32_t _MaxTry_U32)
{
	BOFERR Rts_E = BOFERR_INIT;

	if (mpIBofSocketIo)
	{
		Rts_E = mpIBofSocketIo->V_SignalTryToConnect(_Running_B, _Try_U32, _MaxTry_U32);
	}
	return Rts_E;
}

BOFERR BofSocketClient::V_Connected()
{
	BOFERR Rts_E = BOFERR_INIT;

	if (mpIBofSocketIo)
	{
		Rts_E = mpIBofSocketIo->V_SignalConnected();
	}
	return Rts_E;
}
BOFERR BofSocketClient::V_Disconnected()
{
	BOFERR Rts_E = BOFERR_INIT;

	if (mpIBofSocketIo)
	{
		//Rts_E = mpIBofSocketIo->V_SignalDisconnected();
	}
	return Rts_E;
}
END_BOF_NAMESPACE();
