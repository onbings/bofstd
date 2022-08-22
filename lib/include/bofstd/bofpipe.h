/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines routines for creating and managing a pipe communication
 * channel.
 *
 * Name:        BofPipe.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:
 * The Windows Ce version is not fully operationnal
 *
 * History:
 *
 * V 1.00  Oct 21 2002  BHA : Initial release
 */
#pragma once

/*** Include ****************************************************************/
#include <bofstd/bofstd.h>
#include <bofstd/bofcomchannel.h>
#include <bofstd/bofsocket.h>

BEGIN_BOF_NAMESPACE()

/*** Defines ****************************************************************/

enum class BOF_PIPE_TYPE:uint32_t
{
	BOF_PIPE_OVER_LOCAL_UNKNOWN = 0,
	BOF_PIPE_OVER_LOCAL_UDP,
//	BOF_PIPE_OVER_LOCAL_TCP,
  BOF_PIPE_NATIVE,
};
/*** structure **************************************************************/

struct BOFSTD_EXPORT BOF_PIPE_PARAM
{
	BOF_PIPE_TYPE						 PipeType_E;
	BOF_COM_CHANNEL_PARAM    BaseChannelParam_X;                                    // Base properties of each channel
  uint16_t PortInBase_U16;  //local Port for udp in pipe->if 0 let os decide, otherwise use this value for port in and this value+1 for port out

	BOF_PIPE_PARAM()
	{
		Reset();
	}
	void Reset()
	{
		PipeType_E = BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UNKNOWN;
		BaseChannelParam_X.Reset();
    PortInBase_U16 = 0;
	}
};


/*** Class definition *******************************************************/

/*** Pipe *********************************************************************/

class BOFSTD_EXPORT BofPipe :public BofComChannel
{
private:
	BOF_PIPE_PARAM mPipeParam_X;
	std::unique_ptr<BofSocket> mpuUdpPipeIn = nullptr;
	std::unique_ptr<BofSocket> mpuUdpPipeOut = nullptr;
#if defined(_WIN32)
#else
	int mPipe_i = -1;
#endif

public:
	BofPipe(const BOF_PIPE_PARAM & _rPipeParam_X);
	~BofPipe();

	BOFERR               V_Connect(uint32_t _TimeoutInMs_U32, const std::string &_rTarget_S, const std::string &_rOption_S) override;
	BofComChannel				*V_Listen(uint32_t _TimeoutInMs_U32, const std::string &_rOption_S) override;
	BOFERR               V_ReadData(uint32_t _TimeoutInMs_U32, uint32_t & _rNb_U32, uint8_t *pBuffer_U8) override;
	BOFERR               V_WriteData(uint32_t _TimeoutInMs_U32, uint32_t & _rNb_U32, const uint8_t *pBuffer_U8) override;
	BOFERR							 V_WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S, uint32_t & _rNb_U32) override;
	BOFERR               V_GetStatus(BOF_COM_CHANNEL_STATUS & _rStatus_X) override;
	BOFERR               V_FlushData(uint32_t _TimeoutInMs_U32) override;
	BOFERR							 V_WaitForDataToRead(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPendingByte_U32) override;

	BofPipe & operator    = (const BofPipe &) = delete; // Disallow copying
	BofPipe(const BofPipe &) = delete;
};
END_BOF_NAMESPACE()