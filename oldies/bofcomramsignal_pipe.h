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
 * Name:        bofcomramsignal_pipe.h
 * Author:      Bernard HARMEL: b.harmel@evs.tv
 * Web:			    evs.com
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Mar 23 2017  BHA : Initial release
 */

#pragma once

/*** Include files ***********************************************************/
#include <bofstd/ibofcomramsignal.h>
#include <bofstd/bofcomram.h>
#include <bofstd/bofpipe.h>
#include <atomic>

BEGIN_BOF_NAMESPACE();

/*** Global variables ********************************************************/

/*** Definitions *************************************************************/


/*** Class *******************************************************************/

struct BOF_COMRAM_PARAM_SIGNAL_PIPE
{
	BOF_COMRAM_PARAM				ComramParam_X;
	BOF_PIPE_TYPE						PipeType_E;
	uint32_t								PipeBufferSize_U32;
	uint16_t								Port_U16;
	BOF_COMRAM_PARAM_SIGNAL_PIPE()
	{
		Reset();
	}
	void                     Reset()
	{
		ComramParam_X.Reset();
		PipeType_E = BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UNKNOWN;
		PipeBufferSize_U32 = 0;
		Port_U16 = 0;
	}
};

class BofComramSignalPipe final:public BofComram, public IBofComramSignal
{
private:
	BofPipe *mpPipe;
//	static std::atomic<uint32_t> S_mTicket_U32;

public:
	BofComramSignalPipe(const BOF_COMRAM_PARAM_SIGNAL_PIPE &_rComramSignalPipeParam_X);
	~BofComramSignalPipe();              // not virtual as class is 'final'
	BOFERR  V_Signal() override;
	BOFERR  V_WaitForSignal(uint32_t _PollTimeInMs_U32) override;
};

END_BOF_NAMESPACE();