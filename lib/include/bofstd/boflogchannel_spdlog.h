/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the logger channel interface based on spdlog
 *
 * Name:        boflogchannel_spdlog.h
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

#pragma once

/*** Include files ***********************************************************/
#include <bofstd/iboflogchannel.h>
#include <atomic>
#include <memory>

BEGIN_BOF_NAMESPACE()

/*** Global variables ********************************************************/

/*** Definitions *************************************************************/

/*** Class *******************************************************************/

class BOFSTD_EXPORT BofLogChannelSpdLog final : public IBofLogChannel
{
		// Opaque pointer design pattern: all public and protected stuff goes here ...

public:
		BofLogChannelSpdLog();

		virtual ~BofLogChannelSpdLog();                           // not virtual as class is 'final'

		BOFERR V_Add(const BOF_LOG_CHANNEL_PARAM &_rLogParam_X) override;

		BOFERR V_Remove() override;

		BOFERR V_Log(BOF_LOG_CHANNEL_LEVEL _Level_E, const std::string &_rLogMsg_S) override;

		BOFERR V_Flush() override;

		BOFERR V_LogChannelPathName(BofPath &_rLogPath) override;

		BOFERR V_OpenLogStorage(uint32_t _Offset_U32) override;

		BOFERR V_ReadLogStorage(uint32_t _MaxLineToExtract_U32, std::vector<std::string> &_rExtractedLogLine_S) const override;

		BOFERR V_CloseLogStorage() override;

		BOFERR V_DeleteLogStorage() override;

    BOFERR V_LogLevel(BOF_LOG_CHANNEL_LEVEL _LogLevel_E) const override;

    BOFERR V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL _LogLevel_E, BOF_LOG_LEVEL_COLOR _LogLevelColor_E) const override;

    BOFERR V_LogHeader(const std::string &_rPattern_S) const override;

		// Opaque pointer design pattern: opaque type here
private:
		class BofLogChannelSpdLogImplementation;

		std::unique_ptr<BofLogChannelSpdLogImplementation> mpuBofLogChannelSpdLogImplementation;
};

END_BOF_NAMESPACE()