/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the BofSocketPoller class.
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

/*** Include files ***********************************************************/

#include <bofstd/BofSocketPoller.h>

BEGIN_BOF_NAMESPACE()

enum POLL_SOCKET_CMD
{
  POLL_SOCKET_CMD_CANCEL_WAIT = 'C',
};
BofSocketPoller::BofSocketPoller(uint32_t _NbMaxSession_U32) : BofThread()
{
  BOF_SOCKET_PARAM BofSocketParam_X;

  mErrorCode_E = BOFERR_INVALID_VALUE;
  mBofSocketPollerParam_X = _rBofSocketPollerParam_X;
  BOF_ASSERT(mBofSocketPollerParam_X.NbMaxSession_U32 != 0);
  BOF_ASSERT(mBofSocketPollerParam_X.Listen != INVALID_SOCKET);
  if ((mBofSocketPollerParam_X.NbMaxSession_U32) && (mBofSocketPollerParam_X.Listen != INVALID_SOCKET))
  {
    mErrorCode_E = LaunchBofProcessingThread(_rBofSocketServerParam_X.Name_S, false, 0, 0, 0, mBofSocketServerParam_X.ThreadPolicy_i, mBofSocketServerParam_X.ThreadPriority_i, POLL_TIMEOUT_IN_MS * 8);
    BOF_ASSERT(mErrorCode_E == BOFERR_NO_ERROR);
    if (mErrorCode_E == BOFERR_NO_ERROR)
    {
    }

    mErrorCode_E = Bof_CreateMutex(mBofSocketPollerParam_X.Name_S + "_mtx", false, false, mMtx_X);
    if (mErrorCode_E == BOFERR_NO_ERROR)
    {
      mErrorCode_E = BOFERR_NOT_ENOUGH_RESOURCE;
      // Entry 0 is for listen socket, Entry 1 if for pipe interrupt and the next ones are for client socket: For each client you can have 2 poll socket: one for cmd and one for data
      mpPollOp_X = new BOF_POLL_SOCKET[POLL_RESERVED_INDEX_MAX + (mBofSocketPollerParam_X.NbMaxSession_U32 * 2)];
      BOF_ASSERT(mpPollOp_X != nullptr);
      if (mpPollOp_X)
      {
        memset(mpPollOp_X, 0, sizeof(BOF_POLL_SOCKET) * (POLL_RESERVED_INDEX_MAX + _rBofSocketPollerParam_X.NbMaxSession_U32));
        mpPollOp_X[POLL_RESERVED_INDEX_LISTEN].Fd = mBofSocketPollerParam_X.Listen;
        mpPollOp_X[POLL_RESERVED_INDEX_LISTEN].Event_U16 = POLLIN;

        BofSocketParam_X.BaseChannelParam_X.ChannelName_S = mBofSocketPollerParam_X.Name_S;
        BofSocketParam_X.BaseChannelParam_X.Blocking_B = true; // _rBofSocketServerParam_X.Blocking_B;
        BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
        BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = _rBofSocketPollerParam_X.NbMaxSession_U32 * 2;
        BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = _rBofSocketPollerParam_X.NbMaxSession_U32 * 2;
        BofSocketParam_X.NoDelay_B = true;
        BofSocketParam_X.ReUseAddress_B = true;
        BofSocketParam_X.BindIpAddress_S = "udp://127.0.0.1:" + std::to_string(mBofSocketPollerParam_X.LocalPortForCancellingWait_U16);

        mErrorCode_E = BOFERR_NOT_ENOUGH_RESOURCE;
        mpBofSocketForCancellingWait = new BofSocket(BofSocketParam_X);
        if (mpBofSocketForCancellingWait)
        {
          mErrorCode_E = mpBofSocketForCancellingWait->LastErrorCode();
          if (mErrorCode_E == BOFERR_NO_ERROR)
          {
            BOF_SOCKET_ADDRESS DstIpAddress; // (false, BOF_SOCK_TYPE::BOF_SOCK_UDP, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP, 127, 0, 0, 1, );
            DstIpAddress = mpBofSocketForCancellingWait->GetSrcIpAddress();
            mErrorCode_E = mpBofSocketForCancellingWait->SetDstIpAddress(DstIpAddress);
            if (mErrorCode_E == BOFERR_NO_ERROR)
            {
              mpPollOp_X[POLL_RESERVED_INDEX_CANCEL].Fd = mpBofSocketForCancellingWait->GetSocketHandle();
              mpPollOp_X[POLL_RESERVED_INDEX_CANCEL].Event_U16 = POLLIN;
              mErrorCode_E = BOFERR_NO_ERROR;
            }
          }
        }
      }
    }
  }
}

BofSocketPoller::~BofSocketPoller()
{
  BOF_SAFE_DELETE_ARRAY(mpPollOp_X);
  BOF_SAFE_DELETE(mpBofSocketForCancellingWait);
  Bof_DestroyMutex(mMtx_X);
}
BOFERR BofSocketPoller::LastErrorCode()
{
  return mErrorCode_E;
}

BOFERR BofSocketPoller::AddToPollList(SOCKET _Socket, uint32_t &_rIndex_U32)
{
  BOFERR Rts_E = BOFERR_INVALID_PARAM;

  if (_Socket != INVALID_SOCKET)
  {
    Rts_E = BOFERR_NO_MORE_HANDLE;
    Bof_LockMutex(mMtx_X);
    if (mNbSessionPollOp_U32 < mBofSocketPollerParam_X.NbMaxSession_U32)
    {
      mpPollOp_X[POLL_RESERVED_INDEX_MAX + mNbSessionPollOp_U32].Fd = _Socket;
      mpPollOp_X[POLL_RESERVED_INDEX_MAX + mNbSessionPollOp_U32].Event_U16 = POLLIN;
      _rIndex_U32 = mNbSessionPollOp_U32++;
      Rts_E = BOFERR_NO_ERROR;
    }
    Bof_UnlockMutex(mMtx_X);
  }
  return Rts_E;
}

BOFERR BofSocketPoller::RemoveFromPollList(uint32_t _Index_U32)
{
  BOFERR Rts_E = BOFERR_NOT_FOUND;

  Bof_LockMutex(mMtx_X);
  if (_Index_U32 < mNbSessionPollOp_U32)
  {
    if (_Index_U32 != mNbSessionPollOp_U32 - 1) // Not the last one
    {
      _Index_U32 += POLL_RESERVED_INDEX_MAX;
      memmove(&mpPollOp_X[_Index_U32], &mpPollOp_X[_Index_U32 + 1], mNbSessionPollOp_U32 - _Index_U32);
    }
    mNbSessionPollOp_U32--;
    Rts_E = BOFERR_NO_ERROR;
  }
  Bof_UnlockMutex(mMtx_X);
  return Rts_E;
}
BOFERR BofSocketPoller::CancelPollWait()
{
  BOFERR Rts_E = BOFERR_INIT;
  uint8_t SocketCmd_U8;
  uint32_t Nb_U32;

  if (mpBofSocketForCancellingWait)
  {
    Nb_U32 = 1;
    SocketCmd_U8 = POLL_SOCKET_CMD_CANCEL_WAIT;
    Rts_E = mpBofSocketForCancellingWait->V_WriteData(0, Nb_U32, &SocketCmd_U8);
  }
  return Rts_E;
}
BOFERR BofSocketPoller::Poll(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPollSet_U32, const BOF_POLL_SOCKET **_ppListOfPollOp_X)
{
  BOFERR Rts_E = BOFERR_INVALID_PARAM;
  uint8_t SocketCmd_U8;
  uint32_t Nb_U32 = 1;

  if (_ppListOfPollOp_X)
  {
    Rts_E = Bof_Poll(_TimeoutInMs_U32, POLL_RESERVED_INDEX_MAX + mNbSessionPollOp_U32, mpPollOp_X, _rNbPollSet_U32);
    if (Rts_E == BOFERR_NO_ERROR)
    {
      *_ppListOfPollOp_X = mpPollOp_X;
      if (_rNbPollSet_U32)
      {
        if (mpPollOp_X[POLL_RESERVED_INDEX_CANCEL].Revent_U16)
        {
          if (mpBofSocketForCancellingWait)
          {
            Nb_U32 = 1;
            Rts_E = mpBofSocketForCancellingWait->V_ReadData(0, Nb_U32, &SocketCmd_U8);
          }
          else
          {
            Rts_E = BOFERR_READ;
          }
        }
      }
    }
  }
  return Rts_E;
}
BOFERR BofSocketPoller::SocketSessionStatistic(std::shared_ptr<BofSocketSession> _psSession, BOF_SOCKET_SESSION_STATISTIC &_rSocketSessionStatistic)
{
  BOFERR Rts_E = BOFERR_NOT_FOUND;

  auto const It = std::find_if(mSessionCollection.begin(), mSessionCollection.end(), [_psSession](BOF_SOCKET_SESSION Session_X) { return (Session_X.psSocketSession.get() == _psSession.get()); });
  if (It != mSessionCollection.end())
  {
    Rts_E = BOFERR_NOT_AVAILABLE;
    if (It->psSocketSession)
    {
      Rts_E = BOFERR_NO_ERROR;
      _rSocketSessionStatistic = It->SocketSessionStatistic_X;
    }
  }
  return Rts_E;
}
BOFERR BofSocketPoller::ResetSocketSessionStatistic(std::shared_ptr<BofSocketSession> _psSession)
{
  BOFERR Rts_E = BOFERR_NOT_FOUND;

  auto const It = std::find_if(mSessionCollection.begin(), mSessionCollection.end(), [_psSession](BOF_SOCKET_SESSION Session_X) { return (Session_X.psSocketSession.get() == _psSession.get()); });
  if (It != mSessionCollection.end())
  {
    Rts_E = BOFERR_NOT_AVAILABLE;
    if (It->psSocketSession)
    {
      Rts_E = BOFERR_NO_ERROR;
      It->SocketSessionStatistic_X.Reset();
    }
  }
  return Rts_E;
}

BOFERR BofSocketPoller::ResetAllSocketSessionStatistic()
{
  BOFERR Rts_E = BOFERR_NO_ERROR;

  LockThreadCriticalSection();

  for (auto Session : mSessionCollection)
  {
    if (Session.psSocketSession)
    {
      Session.SocketSessionStatistic_X.Reset();
    }
  }
  UnlockThreadCriticalSection();
  return Rts_E;
}
END_BOF_NAMESPACE()