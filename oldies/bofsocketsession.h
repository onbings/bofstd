/*
* Copyright (c) 2015-2025, Onbings. All rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
* PURPOSE.
*
* This module defines the BofSocketSession class.
*
* Author:      Bernard HARMEL: onbings@dscloud.me
* Web:			   onbings.dscloud.me
* Revision:    1.0
*
* Rem:         Nothing
*
* History:
*
* V 1.00  Jan 05 2019  BHA : Initial release
*/
#pragma once
#include <bofstd/ibofsocketio.h>
#include <bofstd/bofsocketio.h>

BEGIN_BOF_NAMESPACE();
class BofSocketServer;

struct BOF_SOCKET_SESSION_PARAM
{
  BOF_SOCKET_IO_PARAM IoParam_X;

  BOF_SOCKET_SESSION_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    IoParam_X.Reset();
  }
};
class BofSocketSession : public BofSocketIo
{
private:
	BOF_SOCKET_SESSION_PARAM mBofSocketSessionParam_X;
	BofSocketServer *mpBofSocketServer;
	IBofSocketIo *mpIBofSocketIo;

public:
	BofSocketSession(BofSocketServer *_pBofSocketServer, IBofSocketIo *_pIBofSocketIo, const BOF_SOCKET_SESSION_PARAM & _rBofSocketSessionParam_X);
	virtual ~BofSocketSession();

private:
	BOFERR V_DataWritten(BOFERR _Sts_E, uint32_t _NbRemain_U32, const uint8_t *_pRemainingBuffer_U8, void *_pUserArg) override;
	BOFERR V_DataRead(uint32_t _Nb_U32, const uint8_t *_pBuffer_U8) override;
	BOFERR V_RemoteSocketClosed() override;
};

END_BOF_NAMESPACE();
