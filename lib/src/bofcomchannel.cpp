/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the base class of all Bof Com Objects
 *
 * Name:        BofComChannel.cpp
 * Author:      Bernard HARMEL: b.harmel@belgacom.net
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Aug 21 2001  BHA : Initial release
 */

/*** Include files ***********************************************************/
#include <bofstd/bofcomchannel.h>
#include <bofstd/bofsystem.h>

BEGIN_BOF_NAMESPACE()

/*** Global variables ********************************************************/

uint32_t BofComChannel::S_mListenCounter_U32 = 0;

BofComChannel::BofComChannel(BOF_COM_CHANNEL_TYPE _Type_E, const BOF_COM_CHANNEL_PARAM &_rBaseChannelParam_X)
{
	mErrorCode_E = BOF_ERR_NO_ERROR;
	mComType_E = _Type_E;
	mpBaseChannelParam_X = &_rBaseChannelParam_X;
	mConnectionTimer_U32 = Bof_GetMsTickCount();
}


BofComChannel::~BofComChannel()
{
  //BOF_DBG_PRINTF("~BofComChannel()\n");
}

BOFERR BofComChannel::LastErrorCode() const
{
	return mErrorCode_E;
}

const BOF_COM_CHANNEL_PARAM *BofComChannel::BaseChannelParam() const
{
	return mpBaseChannelParam_X;
}
END_BOF_NAMESPACE()