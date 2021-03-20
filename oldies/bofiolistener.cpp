/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bofiolistener class.
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

#include <bofstd/bofiolistener.h>

BEGIN_BOF_NAMESPACE()

BofIoListener::BofIoListener()
{
  BOF_ASSERT(BOF_NAMESPACE::Bof_CreateConditionalVariable("ListenerRunning", true, mListenRunningCv_X) == BOFERR_NO_ERROR);
}

BofIoListener::~BofIoListener()
{
  CancelListen();
  BOF_ASSERT(BOF_NAMESPACE::Bof_DestroyConditionalVariable(mListenRunningCv_X) == BOFERR_NO_ERROR);
}

BOFERR BofIoListener::Listen(uint32_t _TimeoutInMs_U32, BofComChannel &_rIoChannel)
{
  BOFERR Rts_E = BOFERR_BAD_TYPE;

  if (_rIoChannel.BaseChannelParam()->ListenBackLog_U32)
  {  
    mListenTimeoutInMs_U32 = _TimeoutInMs_U32;
    Rts_E = Bof_SignalConditionalVariable(mListenRunningCv_X, mListenRunningCvSetter, true);
    BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);
    Rts_E = Bof_SignalConditionalVariable(mListenRunningCv_X, mListenRunningCvSetter, false);
    BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);
  }
  return Rts_E;
}

BOFERR BofIoListener::CancelListen()
{
  BOFERR Rts_E = BOFERR_BAD_TYPE;

    Rts_E = Bof_WaitForConditionalVariable(mListenRunningCv_X, mListenTimeoutInMs_U32, mListenRunningCvPredicateAndReset);
    BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);
  
  return Rts_E;
}

#if 0
BOFERR BofIoListener::V_Connect(uint32_t _TimeoutInMs_U32, const std::string &_rTarget_S, const std::string &_rOption_S)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  return Rts_E;
}

BofComChannel *BofIoListener::V_Listen(uint32_t _TimeoutInMs_U32, const std::string &_rOption_S)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  return Rts_E;
}

BOFERR BofIoListener::V_ReadData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, uint8_t *_pBuffer_U8)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  return Rts_E;
}

BOFERR BofIoListener::V_WriteData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, const uint8_t *_pBuffer_U8)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  return Rts_E;
}

BOFERR BofIoListener::V_WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S, uint32_t &_rNb_U32)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  return Rts_E;
}

BOFERR BofIoListener::V_GetStatus(BOF_COM_CHANNEL_STATUS &_rStatus_X)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  return Rts_E;
}

BOFERR BofIoListener::V_FlushData(uint32_t _TimeoutInMs_U32)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  return Rts_E;
}

BOFERR BofIoListener::V_WaitForDataToRead(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPendingByte_U32)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  return Rts_E;
}
#endif
END_BOF_NAMESPACE()