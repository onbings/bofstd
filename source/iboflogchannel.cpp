/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the logger channel interface
 *
 * Name:        iboflogchannel.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */

/*** Include files ***********************************************************/
#include <bofstd/iboflogchannel.h>
#include <bofstd/bofbit.h>

BEGIN_BOF_NAMESPACE()

/*** Global variables ********************************************************/

/*** Definitions *************************************************************/

/*** Class *******************************************************************/
IBofLogChannel::IBofLogChannel()
{
	mLogLevel_E = BOF_LOG_CHANNEL_LEVEL::OFF;
	mLogMask_U32 = 0xFFFFFFFF;
	mStartLogTimeInMs_U32 = Bof_GetMsTickCount();
	mLastLogTimeInMs_U32 = Bof_GetMsTickCount();
	for (int i = 0; i < static_cast<int>(sizeof(mLogMask_U32) * 8); i++)
	{
		mpLogMaskName_S.push_back("");
	}
}

BOFERR IBofLogChannel::LogLevel(BOF_LOG_CHANNEL_LEVEL _LogLevel_E)
{
	mLogLevel_E = _LogLevel_E;
	return BOF_ERR_NO_ERROR;
}

BOF_LOG_CHANNEL_LEVEL IBofLogChannel::LogLevel() const
{
	return mLogLevel_E;
}

bool IBofLogChannel::ShouldLog(BOF_LOG_CHANNEL_LEVEL _LogLevel_E) const
{
	return _LogLevel_E >= mLogLevel_E;
}

BOFERR IBofLogChannel::LogMask(uint32_t _LogMask_U32)
{
	mLogMask_U32 = _LogMask_U32;
	return BOF_ERR_NO_ERROR;
}

uint32_t IBofLogChannel::LogMask() const
{
	return mLogMask_U32;
}

BOFERR IBofLogChannel::LogMaskName(uint32_t _LogMask_U32, const std::string &_rLogMaskName_S)
{
	BOFERR Rts_E = BOF_ERR_EINVAL;
	uint8_t BitIndex_U8 = Bof_MostSignificantBitSetPosition(_LogMask_U32);

	if ((BitIndex_U8 != 0xFF) && (BitIndex_U8 < mpLogMaskName_S.size()))
	{
		mpLogMaskName_S[BitIndex_U8] = _rLogMaskName_S;
	}
	return Rts_E;
}

const std::string IBofLogChannel::LogMaskName(uint32_t _LogMask_U32) const
{
	std::string Rts_S;
	uint8_t BitIndex_U8 = Bof_MostSignificantBitSetPosition(_LogMask_U32);

	if ((BitIndex_U8 != 0xFF) && (BitIndex_U8 < mpLogMaskName_S.size()))
	{
		Rts_S = mpLogMaskName_S[BitIndex_U8];
	}
	return Rts_S;
}

bool IBofLogChannel::ShouldLogMask(uint32_t _LogMask_U32) const
{
	return (_LogMask_U32 & mLogMask_U32) ? true : false;
}

uint32_t IBofLogChannel::EllapsedTimeInMsSinceStart()
{
	return Bof_ElapsedMsTime(mStartLogTimeInMs_U32);
}

uint32_t IBofLogChannel::EllapsedTimeInMsSinceLast()
{
	return Bof_ElapsedMsTime(mLastLogTimeInMs_U32);
}

void IBofLogChannel::ResetLastLogTimeInMs()
{
	mLastLogTimeInMs_U32 = Bof_GetMsTickCount();
}
END_BOF_NAMESPACE()