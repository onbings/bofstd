/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the base class of all Bof Com Objects
 *
 * Name:        BofComChannel.h
 * Author:      Bernard HARMEL: b.harmel@belgacom.net
 * Revision:    1.0
 *
 * Rem:         All timeout value are always in ms
 *
 * History:
 *
 * V 1.00  Aug 21 2001  BHA : Initial release
 */
#pragma once

/*** Include ****************************************************************/
#include <bofstd/bofstd.h>
#include <cstdint>
#include <string>
#include <memory>

BEGIN_BOF_NAMESPACE()

/*** Defines ****************************************************************/

#define BOF_DEFAULT_CONNECT_STRING  "BOF_BHA_CON_0100"

/*** enum *******************************************************************/

enum class BOF_COM_CHANNEL_TYPE
{
		TUART = 0,
		TPIPE,
		TSOCKET,
		TCOMRAM
};

enum class BOF_COM_CHANNEL_LOCK
{
		FREE = 0,
		SERVER,
		CLIENT,
		INTERNAL,
};

/*** Structures *************************************************************/

struct BOF_COM_CHANNEL_STATUS
{
		uint32_t NbIn_U32;
		uint32_t NbOut_U32;
		bool Connected_B;
		uint32_t Flag_U32;
		BOFERR Sts_E;

		BOF_COM_CHANNEL_STATUS()
		{
			Reset();
		}

		void Reset()
		{
			NbIn_U32 = 0;
			NbOut_U32 = 0;
			Connected_B = false;
			Flag_U32 = 0;
			Sts_E = BOF_ERR_NO_ERROR;
		}
};

struct BOF_COM_CHANNEL_PARAM
{
		std::string ChannelName_S;
		bool Blocking_B;
		uint32_t ListenBackLog_U32;                               // 0->Client
		uint32_t RcvBufferSize_U32;
		uint32_t SndBufferSize_U32;

		BOF_COM_CHANNEL_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			ChannelName_S = "";
			Blocking_B = false;
			ListenBackLog_U32 = 0;
			RcvBufferSize_U32 = 0;
			SndBufferSize_U32 = 0;
		}
};

/*** Function **************************************************************/

class BofComChannel
{
protected:
		BOF_COM_CHANNEL_TYPE mComType_E;
		const BOF_COM_CHANNEL_PARAM *mpBaseChannelParam_X;              // Base properties of each channel: we keep a pointer inside the member variable of the derived object to keep track of derived class actions on these data
		static uint32_t S_mListenCounter_U32;
		uint32_t mConnectionTimer_U32;
		BOFERR mErrorCode_E;

public:
		BofComChannel(BOF_COM_CHANNEL_TYPE _Type_E, const BOF_COM_CHANNEL_PARAM &_rBaseChannelParam_X);

		virtual ~BofComChannel();

		virtual BOFERR LastErrorCode() const;

		virtual BOFERR V_Connect(uint32_t _TimeoutInMs_U32, const std::string &_rTarget_S, const std::string &_rOption_S) = 0;

		virtual BofComChannel *V_Listen(uint32_t _TimeoutInMs_U32, const std::string &_rOption_S) = 0;

		virtual BOFERR V_ReadData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, uint8_t *_pBuffer_U8) = 0;

		virtual BOFERR V_WriteData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, const uint8_t *_pBuffer_U8) = 0;

		virtual BOFERR V_WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S, uint32_t &_rNb_U32) = 0;

		virtual BOFERR V_GetStatus(BOF_COM_CHANNEL_STATUS &_rStatus_X) = 0;

		virtual BOFERR V_FlushData(uint32_t _TimeoutInMs_U32) = 0;

		virtual BOFERR V_WaitForDataToRead(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPendingByte_U32) = 0;

    /// @private
    BofComChannel(BofComChannel const &) = delete; // Copy construct
    /// @private
    BofComChannel(BofComChannel &&) = delete; // Move construct
    /// @private
    BofComChannel &operator=(BofComChannel const &) = delete; // Copy assign
    /// @private
    BofComChannel &operator=(BofComChannel &&) = delete; // Move assign

		const BOF_COM_CHANNEL_PARAM *BaseChannelParam() const;
};

END_BOF_NAMESPACE()