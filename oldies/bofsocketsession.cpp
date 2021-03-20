/*
* Copyright (c) 2015-2025, Onbings. All rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
* PURPOSE.
*
* This module implements the BofSocketSession class.
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
#include <bofstd/bofsocketserver.h>
#include <bofstd/bofsocketsession.h>

BEGIN_BOF_NAMESPACE();
BofSocketSession::BofSocketSession(BofSocketServer *_pBofSocketServer, IBofSocketIo *_pIBofSocketIo, const BOF_SOCKET_SESSION_PARAM & _rBofSocketSessionParam_X)
								 :mpBofSocketServer(_pBofSocketServer),mpIBofSocketIo(_pIBofSocketIo), BofSocketIo(_rBofSocketSessionParam_X.IoParam_X)
{
	BOF_ASSERT(mpBofSocketServer != nullptr);
	BOF_ASSERT(mpIBofSocketIo != nullptr);
	if (mErrorCode_E == BOFERR_NO_ERROR)
	{
		if ((mpBofSocketServer==nullptr) || (mpIBofSocketIo == nullptr))
		{
			mErrorCode_E = BOFERR_INIT;
		}
    else
    {
    }
	}
}

BofSocketSession::~BofSocketSession()
{
  printf("~BofSocketSession\n");
//Done in BofSocketIo::~BofSocketIo()	DestroyThread();
}

BOFERR BofSocketSession::V_DataRead(uint32_t _Nb_U32, const uint8_t *_pBuffer_U8)
{
	BOFERR Rts_E = BOFERR_INIT;

	if (mpIBofSocketIo)
	{
		Rts_E=mpIBofSocketIo->V_SignalDataRead(_Nb_U32, _pBuffer_U8);
	}
	return Rts_E;
}
BOFERR BofSocketSession::V_DataWritten(BOFERR _Sts_E,uint32_t _NbRemain_U32, const uint8_t *_pRemainingBuffer_U8, void *_pUserArg)
{
	BOFERR Rts_E = BOFERR_INIT;

	if (mpIBofSocketIo)
	{
		Rts_E = mpIBofSocketIo->V_SignalDataWritten(_Sts_E, _NbRemain_U32, _pRemainingBuffer_U8, _pUserArg);
	}
	return Rts_E;
}
BOFERR BofSocketSession::V_RemoteSocketClosed()
{
	BOFERR Rts_E = BOFERR_INIT;

	if (mpIBofSocketIo)
	{
		Rts_E = mpIBofSocketIo->V_SignalRemoteSocketClosed();
	}
//	Rts_E = PostThreatExit(" BofSocketSession::V_RemoteSocketClosed");	//->Session Bofthread will exit and will not call V_OnStop() and all call to Bof_DestroyThread will be cancelled
	if (mpBofSocketServer)
	{
//		mpBofSocketServer->SessionDisconnected(this);
	}
	return Rts_E;
}
END_BOF_NAMESPACE();
