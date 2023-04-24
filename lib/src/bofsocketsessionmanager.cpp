/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the BofSocketSessionManager class.
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
#include <bofstd/bofbit.h>
#include <bofstd/bofenum.h>
#include <bofstd/bofsocketserver.h>
#include <bofstd/bofsocketsessionmanager.h>

#include <stdarg.h>

#if defined(_WIN32)
#else
#include <poll.h>
#endif
// int con = -1, lis = -1;
BEGIN_BOF_NAMESPACE()
#define SEND_OPTIONAL_ANSWER(Arg)                                                                                                                                                                                                                              \
  {                                                                                                                                                                                                                                                            \
    if (PollSocketCmd_X.AnswerTicket_U32)                                                                                                                                                                                                                      \
    {                                                                                                                                                                                                                                                          \
      PollSocketCmd_X.AnswerArg_U32 = Arg;                                                                                                                                                                                                                     \
      Nb_U32 = sizeof(BOF_POLL_SOCKET_CMD);                                                                                                                                                                                                                    \
      Sts_E = mpsPollControlReceiver->NativeBofSocketPointer()->V_WriteData(0, Nb_U32, reinterpret_cast<uint8_t *>(&PollSocketCmd_X));                                                                                                                         \
      BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);                                                                                                                                                                                                                   \
    }                                                                                                                                                                                                                                                          \
  }

constexpr uint32_t POLL_TIMEOUT_IN_MS = 500;
constexpr uint32_t SEND_CMD_TIMEOUT_IN_MS = 1000;
constexpr uint32_t SEND_CMD_ANSWER_TIMEOUT_IN_MS = 100;

static BofEnum<BOF_SOCKET_SESSION_TYPE> S_BofSocketSessionTypeEnumConverter({{BOF_SOCKET_SESSION_TYPE::UNKNOWN, "UNKNOWN"},
                                                                             {BOF_SOCKET_SESSION_TYPE::CMD_POLL_WAIT, "CMD_POLL_WAIT"},
                                                                             {BOF_SOCKET_SESSION_TYPE::SERVER_LISTENER, "SERVER_LISTENER"},
                                                                             {BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL, "COMMAND_CHANNEL"},
                                                                             {BOF_SOCKET_SESSION_TYPE::DATA_LISTENER, "DATA_LISTENER"},
                                                                             {BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL, "DATA_CHANNEL"}},
                                                                            BOF_SOCKET_SESSION_TYPE::UNKNOWN);

BofSocketSessionManager::BofSocketSessionManager(IBofSocketSessionFactory *_pIBofSocketSessionFactory, BofSocketServer *_pBofSocketServer, const BOF_SOCKET_SERVER_PARAM &_rBofSocketServerParam_X) : BofThread()
{
  BOF_SOCKET_PARAM BofSocketParam_X;
  uint32_t i_U32, PollControllSocketBufferSize_U32, Start_U32, Delta_U32, TimeoutInMs_U32;
  // BOFERR										Sts_E;

  mpIBofSocketSessionFactory = _pIBofSocketSessionFactory;
  mpBofSocketServer = _pBofSocketServer;
  mBofSocketServerParam_X = _rBofSocketServerParam_X;

#if defined(_WIN32)
  // Sts_E = Bof_CreateFile(BOF_FILE_PERMISSION_ALL_FOR_ALL, "C:/tmp/" + mBofSocketServerParam_X.Name_S + ".log", false, mIo);
#else
  // Bof_CreateFile(BOF_FILE_PERMISSION_ALL_FOR_ALL, "/tmp/"   + mBofSocketServerParam_X.Name_S + ".log", false, mIo);
#endif

  mNbPollEntry.store(0);
  mAnswerTicket.store(1); // Value 0 is forbidden

  // BOF_ASSERT(mpBofSocketServer != nullptr);	//only BOF_SOCKET_SESSION_TYPE::SERVER_LISTENER has a psSocketServer
  mErrorCode_E = BofThread::InitThreadErrorCode();
  if (mErrorCode_E == BOF_ERR_NO_ERROR)
  {
    mErrorCode_E = BOF_ERR_EINVAL;
    // BOF_ASSERT(_NbMaxSession_U32 != 0); unit tested
    if (mBofSocketServerParam_X.NbMaxSession_U32) //&& (mpBofSocketServer))
    {
      if (mBofSocketServerParam_X.ServerMode_E == BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_POLLER)
      {
        //				1 entry for tcp socket used to cancel poll wait<---- Always present and the first one in list !!
        mNbMaxPollEntry_U32 = 1 + mBofSocketServerParam_X.NbMaxSession_U32;
      }
      else
      {
        /*
        1 entry for tcp socket used to cancel poll wait<---- Always present and the first one in list !!
        1 entry for listenning tcp socket used by BofSocketServer object
        3 entries per session: 1 command and 1 data channel. One extra temporary socket is created to initialise PASV connection
        */
        mNbMaxPollEntry_U32 = 1 + 1 + (mBofSocketServerParam_X.NbMaxSession_U32 * 3);
      }
      PollControllSocketBufferSize_U32 = static_cast<uint32_t>(Bof_NextHighestPowerOf2(2 * sizeof(BOF_POLL_SOCKET_CMD) * mNbMaxPollEntry_U32));
      if (PollControllSocketBufferSize_U32 < 0x1000)
      {
        PollControllSocketBufferSize_U32 = 0x1000;
      }
      BofSocketParam_X.BaseChannelParam_X.ChannelName_S = mBofSocketServerParam_X.Name_S;
      BofSocketParam_X.BaseChannelParam_X.Blocking_B = true; // _rBofSocketServerParam_X.Blocking_B;
      BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
      BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = PollControllSocketBufferSize_U32;
      BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = PollControllSocketBufferSize_U32;
      BofSocketParam_X.NoDelay_B = true;
      BofSocketParam_X.ReUseAddress_B = true;
      BofSocketParam_X.BindIpAddress_S = "tcp://127.0.0.1:0"; // tcpdump -i lo tcp port 8125 -vv -X

      mErrorCode_E = BOF_ERR_ENOMEM;
      mpuPollControlSender = std::make_unique<BofSocket>(BofSocketParam_X);
      if (mpuPollControlSender)
      {
        mErrorCode_E = mpuPollControlSender->LastErrorCode();
        if (mErrorCode_E == BOF_ERR_NO_ERROR)
        {
          mErrorCode_E = BOF_ERR_ENOMEM;
          mpPollOp_X = new BOF_POLL_SOCKET[mNbMaxPollEntry_U32];
          BOF_ASSERT(mpPollOp_X != nullptr);
          if (mpPollOp_X)
          {
            memset(mpPollOp_X, 0, sizeof(BOF_POLL_SOCKET) * mNbMaxPollEntry_U32);
            if (mBofSocketServerParam_X.ServerMode_E == BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_POLLER)
            {
              for (i_U32 = 0; i_U32 < mNbMaxPollEntry_U32; i_U32++)
              {
                mpPollOp_X[i_U32].Fd = static_cast<BOF::BOFSOCKET>(-1);
              }
            }
            // DP https://forum.libcinder.org/topic/solution-calling-shared-from-this-in-the-constructor
            // const auto TrickDontRemove = std::shared_ptr<BofSocketServer>(this, [](BofSocketServer*) {});

            BofSocketParam_X.BaseChannelParam_X.ChannelName_S = mBofSocketServerParam_X.Name_S + "_PollControlListener";
            BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 1;
            BofSocketParam_X.BaseChannelParam_X.Blocking_B = true;
            BofSocketParam_X.BindIpAddress_S = Bof_Sprintf("tcp://127.0.0.1:%d", mBofSocketServerParam_X.PollControlListenerPort_U16);
            BofSocketParam_X.ReUseAddress_B = true;
            BofSocketParam_X.NoDelay_B = true;
            BofSocketParam_X.Ttl_U32 = 0;
            BofSocketParam_X.BroadcastPort_U16 = 0;
            BofSocketParam_X.MulticastInterfaceIpAddress_S = "";
            BofSocketParam_X.MulticastSender_B = false;
            BofSocketParam_X.KeepAlive_B = false;
            BofSocketParam_X.EnableLocalMulticast_B = false;
            mpuPollControlListener = std::make_unique<BofSocket>(BofSocketParam_X);
            if (mpuPollControlListener != nullptr)
            {
              mErrorCode_E = mpuPollControlListener->LastErrorCode();
              if (mErrorCode_E == BOF_ERR_NO_ERROR)
              {
                mErrorCode_E = LaunchBofProcessingThread(mBofSocketServerParam_X.Name_S, false, 0, mBofSocketServerParam_X.ThreadSchedulerPolicy_E, mBofSocketServerParam_X.ThreadPriority_E, mBofSocketServerParam_X.ThreadCpuCoreAffinityMask_U64, 2000, 0);
                if (mErrorCode_E == BOF_ERR_NO_ERROR)
                {
                  Start_U32 = Bof_GetMsTickCount();
                  TimeoutInMs_U32 = mBofSocketServerParam_X.PollControlListenerTimeoutInMs_U32;
#if 1
                  //									mErrorCode_E = BOF_ERR_ECONNABORTED;
                  mErrorCode_E = mpuPollControlSender->V_Connect(TimeoutInMs_U32, BofSocketParam_X.BindIpAddress_S, "");
#else
                  i_U32 = 0;
                  do
                  {
                    // printf("connect start %s\n", BofSocketParam_X.BindIpAddress_S.c_str());
                    mErrorCode_E = mpuPollControlSender->V_Connect(100, BofSocketParam_X.BindIpAddress_S, "");
                    // printf("V_Connect[%d]:%d t=%d\n", i_U32, mErrorCode_E, Bof_GetMsTickCount());
                    if (mErrorCode_E == BOF_ERR_NO_ERROR)
                    {
                      break;
                    }
                    else
                    {
                      i_U32++;
                      Delta_U32 = Bof_ElapsedMsTime(Start_U32);
                    }
                  } while (Delta_U32 < mBofSocketServerParam_X.PollControlListenerTimeoutInMs_U32);
                  if ((int)i_U32 > con)
                  {
                    con = i_U32;
                  }
#endif
                  if (mErrorCode_E == BOF_ERR_NO_ERROR)
                  {
                    mErrorCode_E = Bof_ReEvaluateTimeout(Start_U32, TimeoutInMs_U32);
                    if (mErrorCode_E == BOF_ERR_NO_ERROR)
                    {
                      mErrorCode_E = BOF_ERR_ETIMEDOUT;
                      do
                      {
                        Delta_U32 = Bof_ElapsedMsTime(Start_U32);

                        if (mpsPollControlReceiver != nullptr)
                        {
                          mErrorCode_E = BOF_ERR_NO_ERROR;
                          break;
                        }
                        else
                        {
                          Bof_MsSleep(10);
                        }
                      } while (Delta_U32 < TimeoutInMs_U32);
                    }
                  }
                }
              }
            }
          }
        }
      }
//      BOF_ASSERT(mErrorCode_E == BOF_ERR_NO_ERROR);
    }
  }
}

BofSocketSessionManager::~BofSocketSessionManager()
{
  StopProcessingThread();
  LockThreadCriticalSection(" ~BofSocketSessionManager");
  // DP https://stackoverflow.com/questions/8234779/how-to-remove-from-a-map-while-iterating-it
  for (auto It = mSessionCollection.cbegin(), NxtIt = It; It != mSessionCollection.cend(); It = NxtIt)
  {
    ++NxtIt;

    // No as thread is closed -> no more SendPollSocketCommand RemoveFromPollList(It->second, SessionIndex_U32);

    if (It->second->Connected())
    {
      It->second->V_SignalDisconnected();
      It->second->Connected(false);
    }
    It->second->V_SignalClosed();
  }
  mSessionCollection.clear();
  UnlockThreadCriticalSection();

  BOF_SAFE_DELETE_ARRAY(mpPollOp_X);
  //	Bof_DestroyMutex(mSessionListMtx_X);
  Bof_CloseFile(mIo);
}

BOFERR BofSocketSessionManager::BofIoLog(const char *_pFormat_c, ...)
{
  BOFERR Rts_E = BOF_ERR_NOT_OPENED;
  char pLog_c[0x1000], pTime_c[32];
  ;
  va_list Arg_X;

  if (Bof_IsFileHandleValid(mIo))
  {
    Rts_E = BOF_ERR_EINVAL;
    if (_pFormat_c)
    {
      va_start(Arg_X, _pFormat_c);
      vsprintf(pLog_c, _pFormat_c, Arg_X);
      va_end(Arg_X);
      sprintf(pTime_c, "%d -> ", Bof_GetMsTickCount());
      Rts_E = Bof_WriteLine(mIo, pTime_c);
      Rts_E = Bof_WriteLine(mIo, pLog_c);
      Bof_FlushFile(mIo);
    }
  }
  return Rts_E;
}

BOFERR BofSocketSessionManager::StopProcessingThread()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  BOF_POLL_SOCKET_CMD PollSocketCmd_X;

  if (IsThreadRunning(10))
  {
    PollSocketCmd_X.SocketOp_E = BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_QUIT;
    Rts_E = SendPollSocketCommand(SEND_CMD_ANSWER_TIMEOUT_IN_MS, PollSocketCmd_X);
    //		BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);  //In this case V_OnProcessing thread can be already out
    Rts_E = BOF_ERR_NO_ERROR;
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = DestroyBofProcessingThread("~BofSocketSessionManager");
    }
  }
  return Rts_E;
}
std::shared_ptr<BofSocketIo> BofSocketSessionManager::FindAndGetBofSocketIo(const char *_pLocker_c, BOFSOCKET _SessionId)
{
  std::shared_ptr<BofSocketIo> psRts;

  LockThreadCriticalSection(_pLocker_c);
  const auto It = mSessionCollection.find(_SessionId); // Locked
  if (It != mSessionCollection.end())
  {
    psRts = It->second;
  }
  UnlockThreadCriticalSection();

  return psRts;
}
BOFERR BofSocketSessionManager::LastErrorCode() const
{
  return mErrorCode_E;
}

const BOF_SOCKET_SERVER_PARAM &BofSocketSessionManager::SocketServerParam() const
{
  return mBofSocketServerParam_X;
}
bool BofSocketSessionManager::IsSessionActive(uint32_t _SessionIndex_U32) const
{
  bool Rts_B = false;
  if ((mpActiveSession_B) && (_SessionIndex_U32 < mBofSocketServerParam_X.NbMaxSession_U32))
  {
    Rts_B = mpActiveSession_B[_SessionIndex_U32];
  }
  return Rts_B;
}

uint32_t BofSocketSessionManager::NbConnectedSession() const
{
  return static_cast<uint32_t>(mConnectedSessionCollection.size());
}
uint32_t BofSocketSessionManager::NbConnectedChannel(std::shared_ptr<BofSocketIo> _psSocketSession)
{
  uint32_t Rts_U32 = 0;

  if (_psSocketSession)
  {
    Rts_U32++;
    if (_psSocketSession->SessionType() == BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL)
    {
      if (_psSocketSession->ChildDataChannel())
      {
        Rts_U32++;
      }
    }
    else if (_psSocketSession->SessionType() == BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL)
    {
      if (!_psSocketSession->ParentCmdChannel().expired())
      {
        Rts_U32++;
      }
    }
  }
  return Rts_U32;
}
uint32_t BofSocketSessionManager::NbPollChannel() const
{
  return NbPollEntry();
}

BOFERR BofSocketSessionManager::WaitForNbConnectedSession(uint32_t _ExpectedConnectedSession_U32, uint32_t _PollTimeInMs_U32, uint32_t _TimeoutInMs_U32, uint32_t _ExpectedPollChannel_U32)
{
  BOFERR Rts_E = BOF_ERR_ETIMEDOUT;
  uint32_t Start_U32, Delta_U32; // , ExpectedConnectedSession_U32;

  Start_U32 = Bof_GetMsTickCount();
  do
  {
    Delta_U32 = Bof_ElapsedMsTime(Start_U32);

    // BOF_DBG_PRINTF("#@#%s LOOP Con %d/%d Poll %d/%d in %d ms\n", mBofSocketServerParam_X.Name_S.c_str(), _ExpectedConnectedSession_U32, NbConnectedSession(), _ExpectedPollChannel_U32, NbPollChannel(), Delta_U32);
    if (NbConnectedSession() == _ExpectedConnectedSession_U32)
    {
      if (_ExpectedPollChannel_U32 != 0xFFFFFFFF)
      {
        if (NbPollChannel() == _ExpectedPollChannel_U32)
        {
          Rts_E = BOF_ERR_NO_ERROR;
          break;
        }
        else
        {
          Bof_MsSleep(_PollTimeInMs_U32);
        }
      }
      else
      {
        Rts_E = BOF_ERR_NO_ERROR;
        break;
      }
    }
    else
    {
      Bof_MsSleep(_PollTimeInMs_U32);
    }
  } while (Delta_U32 < _TimeoutInMs_U32);
  // BOF_DBG_PRINTF("#@#%s END Con %d/%d Poll %d/%d in %d ms\n", mBofSocketServerParam_X.Name_S.c_str(), _ExpectedConnectedSession_U32, NbConnectedSession(), _ExpectedPollChannel_U32, NbPollChannel(), Delta_U32);

  return Rts_E;
}

// Callback are NOT sent from here but from thread as they must be fired AFTER aquiring resources
// BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION
BOFERR BofSocketSessionManager::AddToPollList(BOF_SOCKET_SESSION_TYPE _SessionType_E, std::shared_ptr<BofSocketIo> _psParentCmdChannel, uint32_t _TimeoutInMs_U32, std::shared_ptr<BofSocketIo> _psSocketSession, BOFSOCKET &_rSessionId)
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;
  BOF_POLL_SOCKET_CMD PollSocketCmd_X;
  bool NotFound_B;

  _rSessionId = BOF_INVALID_SOCKET_VALUE;
  if (mBofSocketServerParam_X.ServerMode_E == BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION)
  {
    Rts_E = BOF_ERR_EINVAL;
    if ((_psSocketSession->NativeBofSocketPointer()) && (_psSocketSession->NativeBofSocketPointer()->GetSocketHandle() != BOF_INVALID_SOCKET_VALUE))
    {
      Rts_E = BOF_ERR_INIT;
      if (_psSocketSession->NativeBofSocketPointer()->LastErrorCode() == BOF_ERR_NO_ERROR)
      {
        Rts_E = BOF_ERR_NO_MORE;
        if (mNbPollEntry < mNbMaxPollEntry_U32)
        {
          Rts_E = BOF_ERR_EEXIST;

          LockThreadCriticalSection(" AddToPollListServer");
          const auto It = mSessionCollection.find(_psSocketSession->NativeBofSocketPointer()->GetSocketHandle());
          NotFound_B = (It == mSessionCollection.end());
          if (NotFound_B)
          {
            _psSocketSession->SessionType(_SessionType_E);
            if ((_SessionType_E == BOF_SOCKET_SESSION_TYPE::DATA_LISTENER) || (_SessionType_E == BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL))
            {
              BOF_ASSERT(_psParentCmdChannel != nullptr);
              BOF_ASSERT(_TimeoutInMs_U32 != 0);

              _psSocketSession->NoIoCloseTimeoutInMs(_TimeoutInMs_U32);
              _psSocketSession->ParentCmdChannel(_psParentCmdChannel);
              if (_SessionType_E == BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL)
              {
                _psParentCmdChannel->ChildDataChannel(_psSocketSession);
              }
            }
            else
            {
              BOF_ASSERT(_psParentCmdChannel == nullptr);
              BOF_ASSERT(_TimeoutInMs_U32 == 0);
            }
            _rSessionId = _psSocketSession->NativeBofSocketPointer()->GetSocketHandle();
            mSessionCollection[_rSessionId] = _psSocketSession;
          }
          UnlockThreadCriticalSection();

          if (NotFound_B)
          {
            PollSocketCmd_X.SocketOp_E = BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_ADD_ENTRY;
            PollSocketCmd_X.SessionId = _rSessionId;
            Rts_E = SendPollSocketCommand(0, PollSocketCmd_X);
            BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
            if (Rts_E != BOF_ERR_NO_ERROR) // Should never happens but.. We try to correct
            {
              RemovePollEntry(true, _rSessionId);
            }
          }
        }
      }
    }
  }
  return Rts_E;
}

// Callback are NOT sent from here but from thread as they must be fired AFTER aquiring resources
// BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_POLLER
BOFERR BofSocketSessionManager::AddToPollList(uint32_t _SessionId_U32, std::unique_ptr<BofSocket> _puSocket, std::shared_ptr<BofSocketIo> &_rpsSocketSession, BOFSOCKET &_rSessionId)
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;
  BOF_POLL_SOCKET_CMD PollSocketCmd_X;
  bool NotFound_B;

  _rSessionId = BOF_INVALID_SOCKET_VALUE;
  if (mBofSocketServerParam_X.ServerMode_E == BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_POLLER)
  {
    Rts_E = BOF_ERR_EINVAL;
    if ((_puSocket) && (_puSocket->GetSocketHandle() != BOF_INVALID_SOCKET_VALUE))
    {
      Rts_E = BOF_ERR_INIT;
      if (_puSocket->LastErrorCode() == BOF_ERR_NO_ERROR)
      {
        Rts_E = BOF_ERR_NO_MORE;
        if (mNbPollEntry < mNbMaxPollEntry_U32)
        {
          Rts_E = BOF_ERR_INTERNAL;
          if (mpBofSocketServer)
          {
            Rts_E = BOF_ERR_EEXIST;

            LockThreadCriticalSection(" AddToPollListPoller");
            const auto It = mSessionCollection.find(_puSocket->GetSocketHandle());
            NotFound_B = (It == mSessionCollection.end());
            if (NotFound_B)
            {
              if (mpBofSocketServer->LockFreeSessionIndex(_SessionId_U32))
              {
                _rSessionId = _puSocket->GetSocketHandle();
                Rts_E = mpBofSocketServer->ConnectSession(_SessionId_U32, std::move(_puSocket), _rpsSocketSession);

                if (Rts_E == BOF_ERR_NO_ERROR)
                {
                  mSessionCollection[_rSessionId] = _rpsSocketSession;
                }
                else
                {
                  if (mpActiveSession_B)
                  {
                    mpActiveSession_B[_SessionId_U32] = false;
                  }
                }
              }
            }
            if (Rts_E != BOF_ERR_NO_ERROR)
            {
              NotFound_B = false;
            }
            UnlockThreadCriticalSection();

            /*



          _psSocketSession->SessionType(BOF_SOCKET_SESSION_TYPE::POLL_CHANNEL);
          _psSocketSession->Connected(true);
          _psSocketSession->SessionIndex(_SessionId_U32);
          _rSessionId = _psSocketSession->NativeBofSocketPointer()->GetSocketHandle();
          mSessionCollection[_rSessionId] = _psSocketSession;
          */

            if (NotFound_B)
            {
              PollSocketCmd_X.SocketOp_E = BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_ADD_ENTRY;
              PollSocketCmd_X.SessionId = _rSessionId;
              Rts_E = SendPollSocketCommand(0, PollSocketCmd_X);
              BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
              if (Rts_E != BOF_ERR_NO_ERROR) // Should never happens but.. We try to correct
              {
                RemovePollEntry(true, _rSessionId);
              }
            }
          }
        }
      }
    }
  }
  return Rts_E;
}

// Callback are sent from here and ,not from thread as thi one can be stopped and they must be fired BEFORE releasing resources
BOFERR BofSocketSessionManager::RemoveFromPollList(uint32_t _TimeoutInMs_U32, std::shared_ptr<BofSocketIo> _psSocketSession, uint32_t &_rSessionIndex_U32)
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;
  BOF_POLL_SOCKET_CMD PollSocketCmd_X;

  _rSessionIndex_U32 = 0xFFFFFFFF;
  // if (mBofSocketServerParam_X.ServerMode_E == BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION)
  {
    Rts_E = BOF_ERR_EINVAL;
    if (_psSocketSession != nullptr)
    {
      Rts_E = BOF_ERR_CANNOT_STOP;
      if (_psSocketSession->SessionType() != BOF_SOCKET_SESSION_TYPE::CMD_POLL_WAIT) // Used in destructor..
      {
        _rSessionIndex_U32 = _psSocketSession->SessionIndex();
        PollSocketCmd_X.SocketOp_E = BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_REMOVE_ENTRY;
        PollSocketCmd_X.SessionId = _psSocketSession->SessionId();
        Rts_E = SendPollSocketCommand(_TimeoutInMs_U32, PollSocketCmd_X);
        BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketSessionManager::SendPollSocketCommand(uint32_t _OptionalAnswerTimeoutInMs_U32, BOF_POLL_SOCKET_CMD &_rPollSocketCommand_X)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  uint32_t Nb_U32;
  BOF_POLL_SOCKET_CMD PollSocketCommand_X;
  uint32_t Start_U32, Delta_U32;

  Rts_E = BOF_ERR_INTERNAL;
  BOF_ASSERT(mpuPollControlSender != nullptr);
  BOF_ASSERT(mpuPollControlSender->LastErrorCode() == BOF_ERR_NO_ERROR);
  if ((mpuPollControlSender) && (mpuPollControlSender->LastErrorCode() == BOF_ERR_NO_ERROR))
  {
    if (_OptionalAnswerTimeoutInMs_U32)
    {
      if (mAnswerTicket.load() == 0) // Value 0 is forbidden
      {
        mAnswerTicket.store(1);
      }
      _rPollSocketCommand_X.AnswerTicket_U32 = mAnswerTicket;
      _rPollSocketCommand_X.AnswerArg_U32 = 0;
      mAnswerTicket++;
    }
    Nb_U32 = sizeof(BOF_POLL_SOCKET_CMD);
    Rts_E = mpuPollControlSender->V_WriteData(SEND_CMD_TIMEOUT_IN_MS, Nb_U32, reinterpret_cast<uint8_t *>(&_rPollSocketCommand_X));
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (_OptionalAnswerTimeoutInMs_U32)
      {
        Rts_E = BOF_ERR_ETIMEDOUT;
        Start_U32 = BOF::Bof_GetMsTickCount();
        do
        {
          Nb_U32 = sizeof(BOF_POLL_SOCKET_CMD);
          Rts_E = mpuPollControlSender->V_ReadData(_OptionalAnswerTimeoutInMs_U32, Nb_U32, reinterpret_cast<uint8_t *>(&PollSocketCommand_X));
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            if (PollSocketCommand_X.AnswerTicket_U32 == _rPollSocketCommand_X.AnswerTicket_U32)
            {
              Rts_E = BOF_ERR_NO_ERROR;
              break;
            }
          }
          if (Bof_ReEvaluateTimeout(Start_U32, _OptionalAnswerTimeoutInMs_U32) != BOF_ERR_NO_ERROR)
          {
            break;
          }
          Delta_U32 = BOF::Bof_ElapsedMsTime(Start_U32);
        } while (Delta_U32 < _OptionalAnswerTimeoutInMs_U32);
      }
    }
  }
  if (_rPollSocketCommand_X.SocketOp_E != BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_QUIT) // In this case V_OnProcessing thread can be already out
  {
    BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
  }
  return Rts_E;
}

BOFERR BofSocketSessionManager::SocketSessionStatistic(std::shared_ptr<BofSocketIo> _psSocketSession, BOF_SOCKET_SESSION_STATISTIC &_rSocketSessionStatistic)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if (_psSocketSession != nullptr)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    _rSocketSessionStatistic = _psSocketSession->SocketSessionStatistic();
  }

  return Rts_E;
}
BOFERR BofSocketSessionManager::ResetSocketSessionStatistic(std::shared_ptr<BofSocketIo> _psSocketSession)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if (_psSocketSession != nullptr)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    _psSocketSession->ResetSocketSessionStatistic();
  }
  return Rts_E;
}

BOFERR BofSocketSessionManager::ResetAllSocketSessionStatistic()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;

  LockThreadCriticalSection(" ResetAllSocketSessionStatistic");
  for (const auto &_rItem : mSessionCollection)
  {
    ResetSocketSessionStatistic(_rItem.second);
  }
  UnlockThreadCriticalSection();

  return Rts_E;
}
uint32_t BofSocketSessionManager::NbPollEntry() const
{
  return mNbPollEntry;
}
uint32_t BofSocketSessionManager::NbMaxPollEntry() const
{
  return mNbMaxPollEntry_U32;
}
std::string BofSocketSessionManager::SocketSessionManagerDebugInfo()
{
  std::string Rts_S, ActiveSession_S;
  uint32_t i_U32;

  Rts_S = Bof_Sprintf("BofSocketSessionManager\nName:              %s\nInitError:         %s\nSocketServer:      %p\nCancelWait:        %03d:%p\n", mBofSocketServerParam_X.Name_S.c_str(), Bof_ErrorCode(mErrorCode_E), mpBofSocketServer,
                      mpsPollControlReceiver.use_count(), mpsPollControlReceiver.get());

  Rts_S += Bof_Sprintf("Poll:              %04d/%04d\n", mNbPollEntry.load(), mNbMaxPollEntry_U32);
  for (i_U32 = 0; i_U32 < mNbPollEntry; i_U32++)
  {
    Rts_S += Bof_Sprintf("  Poll[%04d]= %08X (%04X/%04X)\n", i_U32, mpPollOp_X[i_U32].Fd, mpPollOp_X[i_U32].Revent_U16, mpPollOp_X[i_U32].Revent_U16);
  }
  if (mpActiveSession_B)
  {
    for (i_U32 = 0; i_U32 < mBofSocketServerParam_X.NbMaxSession_U32; i_U32++)
    {
      ActiveSession_S += mpActiveSession_B[i_U32] ? 'T' : 'F';
    }
  }
  Rts_S += "Active Session:    " + ActiveSession_S + "\n";

  LockThreadCriticalSection(" SocketSessionManagerDebugInfo");
  ActiveSession_S = Bof_Sprintf("SessionCollection: %d\n", mSessionCollection.size());
  for (const auto &rItem : mSessionCollection)
  {
    ActiveSession_S += Bof_Sprintf("===Key %08X/Use %02d==============================================\n%s", rItem.first, rItem.second.use_count(), rItem.second->SocketIoDebugInfo().c_str());
  }
  UnlockThreadCriticalSection();

  Rts_S += ActiveSession_S;

  return Rts_S;
}

// Clean up de AddToPollList (not from thread) otherwise called from thread
BOFERR BofSocketSessionManager::RemovePollEntry(bool _OnlyCleanUp_B, BOFSOCKET _SessionId)
{
  BOFERR Rts_E = BOF_ERR_TOO_SMALL; // , Sts_E;
  uint32_t i_U32, j_U32, SessionIndex_U32;
  std::shared_ptr<BofSocketIo> psSocketSession;

  if (mNbPollEntry)
  {
    Rts_E = BOF_ERR_NOT_FOUND;
    psSocketSession = FindAndGetBofSocketIo(" Find_RemovePollEntry", _SessionId);
    if (psSocketSession != nullptr)
    {
      for (i_U32 = 0; i_U32 < mNbPollEntry; i_U32++)
      {
        if (mpPollOp_X[i_U32].Fd == _SessionId)
        {
          Rts_E = BOF_ERR_NO_ERROR;
          break;
        }
      }
      if (!_OnlyCleanUp_B)
      {
        BOF_ASSERT(i_U32 < mNbPollEntry);
        BOF_ASSERT(mpPollOp_X[i_U32].Fd == _SessionId);
        BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);

        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          if (i_U32 != (mNbPollEntry - 1)) // Not the last one  (Index has been compted ant then whe have .erase -> do not sub -1
          {
            for (j_U32 = i_U32; j_U32 < mNbPollEntry; j_U32++)
            {
              mpPollOp_X[j_U32] = mpPollOp_X[j_U32 + 1];
            }
          }
          mNbPollEntry--;

          if (psSocketSession->SessionType() != BOF_SOCKET_SESSION_TYPE::DATA_LISTENER)
          {
            if (psSocketSession->Connected())
            {
              psSocketSession->V_SignalDisconnected();
              psSocketSession->Connected(false);
            }
            psSocketSession->V_SignalClosed();
            if ((psSocketSession->SessionType() == BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL) || (psSocketSession->SessionType() == BOF_SOCKET_SESSION_TYPE::POLL_CHANNEL))
            {
              // Sts_E = BOF_ERR_ECONNREFUSED;
              BOF_ASSERT(mConnectedSessionCollection.size() != 0);
              if (mConnectedSessionCollection.size())
              {
                SessionIndex_U32 = psSocketSession->SessionIndex();
                BOF_ASSERT(SessionIndex_U32 < mBofSocketServerParam_X.NbMaxSession_U32);
                BOF_ASSERT(mpActiveSession_B[SessionIndex_U32] == true);
                // Sts_E = BOF_ERR_INVALID_STATE;
                if (SessionIndex_U32 < mBofSocketServerParam_X.NbMaxSession_U32)
                {
                  // Sts_E = BOF_ERR_NO_ERROR;
                  if (mpActiveSession_B)
                  {
                    mpActiveSession_B[SessionIndex_U32] = false;
                  }
                }
                if (mpIBofSocketSessionFactory)
                {
                  mpIBofSocketSessionFactory->V_CloseSession(psSocketSession);
                }
                auto It = std::find(mConnectedSessionCollection.begin(), mConnectedSessionCollection.end(), psSocketSession);
                if (It != mConnectedSessionCollection.end())
                {
                  mConnectedSessionCollection.erase(It);
                }
              }
              else
              {
                Rts_E = BOF_ERR_INTERNAL;
              }
            }
          }
        }
      }
      else // if (!_OnlyCleanUp_B)
      {
        // We just need to remove an entry from the mSessionCollection
        BOF_ASSERT(i_U32 >= mNbPollEntry);
      }
      psSocketSession->DestroySocket(mBofSocketServerParam_X.Name_S);

      LockThreadCriticalSection(" RemovePollEntry mSessionCollection");
      mSessionCollection.erase(_SessionId);
      UnlockThreadCriticalSection();
      psSocketSession->CurrentIoDataCommand("");

    } // if (It != mSessionCollection.end())
  }

  return Rts_E;
}

std::string BofSocketSessionManager::ConnectionDebugInfo()
{
  std::string Rts_S;
  std::shared_ptr<BofSocketIo> psSocketSession, psSocketDataSession;
  uint32_t i_U32;

  LockThreadCriticalSection(" ConnectionDebugInfo");
  Rts_S = Bof_Sprintf("%s has %d poll entries and %d con entries:\n", mBofSocketServerParam_X.Name_S.c_str(), mNbPollEntry.load(), NbConnectedSession());

  // if ( mBofSocketServerParam_X.Name_S != "MyTcpServer")
  {
    for (i_U32 = 0; i_U32 < mNbPollEntry; i_U32++)
    {
      Rts_S += Bof_Sprintf("%s Poll Fd[%d]=%08X evt %04X (msk %04X)\n", mBofSocketServerParam_X.Name_S.c_str(), i_U32, mpPollOp_X[i_U32].Fd, mpPollOp_X[i_U32].Revent_U16, mpPollOp_X[i_U32].Event_U16);
      psSocketSession = FindAndGetBofSocketIo(" Find_ConnectionDebugInfo", mpPollOp_X[i_U32].Fd);
      if (psSocketSession == nullptr)
      {
        Rts_S += Bof_Sprintf("  WARNING: Id %08X has no BofSocketIo in mSessionCollection (%d)\n", mpPollOp_X[i_U32].Fd, mSessionCollection.size());
      }
      else
      {
        psSocketDataSession = (psSocketSession->ParentCmdChannel().expired()) ? nullptr : psSocketSession->ParentCmdChannel().lock();
        Rts_S += Bof_Sprintf("  %s Type %s Con %s '%s'->'%s' Parent '%s' Child '%s' Id %08X Indx %d\n", mBofSocketServerParam_X.Name_S.c_str(), S_BofSocketSessionTypeEnumConverter.ToString(psSocketSession->SessionType()).c_str(),
                             psSocketSession->Connected() ? "True" : "False", psSocketSession->ConnectedFromIpAddress().c_str(), psSocketSession->ConnectedToIpAddress().c_str(),
                             (psSocketDataSession == nullptr) ? "" : psSocketDataSession->ConnectedToIpAddress().c_str(), (psSocketSession->ChildDataChannel() == nullptr) ? "" : psSocketSession->ChildDataChannel()->ConnectedToIpAddress().c_str(),
                             psSocketSession->SessionId(), psSocketSession->SessionIndex());
      }
    }
  }
  UnlockThreadCriticalSection();

  return Rts_S;
}
BOFERR BofSocketSessionManager::Poll(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPollSet_U32, BOF_POLL_SOCKET_CMD &_rPollSocketCmd_X)
{
  BOFERR Rts_E = BOF_ERR_INTERNAL;
  uint32_t Nb_U32;

  _rPollSocketCmd_X.Reset();
  _rNbPollSet_U32 = 0;
  BOF_ASSERT(mpsPollControlReceiver->NativeBofSocketPointer() != nullptr);
  BOF_ASSERT(mpsPollControlReceiver->NativeBofSocketPointer()->LastErrorCode() == BOF_ERR_NO_ERROR);
  if (mpsPollControlReceiver->NativeBofSocketPointer())
  {
    Rts_E = mpsPollControlReceiver->NativeBofSocketPointer()->LastErrorCode();
  }
  BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);

  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Rts_E = Bof_Poll(_TimeoutInMs_U32, mNbPollEntry, mpPollOp_X, _rNbPollSet_U32);
    BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (_rNbPollSet_U32)
      {
        if (mpPollOp_X[0].Revent_U16) // CANCEL_POLL_WAIT
        {
          _rNbPollSet_U32--;
          if (mpPollOp_X[0].Revent_U16 & BOF_POLL_IN) // First data
          {
            Nb_U32 = sizeof(BOF_POLL_SOCKET_CMD);
            Rts_E = mpsPollControlReceiver->NativeBofSocketPointer()->V_ReadData(0, Nb_U32, reinterpret_cast<uint8_t *>(&_rPollSocketCmd_X));
            BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
          }
          if (mpPollOp_X[0].Revent_U16 & (BOF_POLL_HUP | BOF_POLL_RDHUP | BOF_POLL_ERR | BOF_POLL_NVAL)) // After error which can be set also with BOF_POLL_IN (Data channel write and closed)
          {
          }
          mpPollOp_X[0].Revent_U16 = 0; // Remove it from visible Revent_U16
          ////BOF_DBG_PRINTF("@@@%s Op '%c' Id %08X NbPollEntry %d Rts %d\n", mBofSocketServerParam_X.Name_S.c_str(),_rPollSocketCmd_X.SocketOp_E,   _rPollSocketCmd_X.SessionId,mNbPollEntry.load(), Rts_E);
        }
      }
    }
  }
  BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
  return Rts_E;
}
/*
The following member can only be MODIFIED here
mNbPollEntry_U32: Curent number of entries in  mpPollOp_X
mpPollOp_X: Array of poll op
mSessionCollection.erase;
mConnectedSessionCollection .push_back and erase
mpActiveSession_B[SessionIndex_U32] = false;

Outside:
mSessionCollection[;
mpActiveSession_B[SessionIndex_U32] = true;
*/
BOFERR BofSocketSessionManager::V_OnProcessing()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR, Sts_E;
  BOF_SOCKET_ADDRESS SrcIpAddress_X, DstIpAddress_X;
  BOFSOCKET SessionId;
  std::shared_ptr<BofSocketIo> psSocketSession;
  std::shared_ptr<BofSocketIo> psAddSocketSession;
  uint16_t REvent_U16;
  uint32_t i_U32, NbPollSet_U32, NbPollSignaled_U32, Delta_U32;
  BOF_POLL_SOCKET_CMD PollSocketCmd_X;
  bool EndOfCmdProcessing_B, EntryPresentInOpList_B;

  BofComChannel *pChannel;
  std::unique_ptr<BofSocket> puDataSocket;
  std::shared_ptr<BofSocketIo> psParentSocketSession;
  std::shared_ptr<BofSocketIo> psDataSocketSession;

  BOF_SOCKET_IO_PARAM BofSocketIoParam_X;
  std::unique_ptr<BofSocket> puSocket;
  uint32_t Nb_U32;

  UnlockThreadCriticalSection(); // Release lock put by  BofThread::BofThread_Thread()

  Rts_E = BOF_ERR_INIT;
  BOF_ASSERT(mpuPollControlListener != nullptr);

  if (mpuPollControlListener != nullptr)
  {
    Rts_E = BOF_ERR_ECONNREFUSED;
#if 1
    pChannel = mpuPollControlListener->V_Listen(mBofSocketServerParam_X.PollControlListenerTimeoutInMs_U32, "");

#else
    i_U32 = 0;
    Start_U32 = Bof_GetMsTickCount();
    do
    {
      pChannel = mpuPollControlListener->V_Listen(100, "");
      //			printf("listen[%d]:%p t=%d\n", i_U32, pChannel, Bof_GetMsTickCount());

      if (pChannel)
      {
        break;
      }
      else
      {
        i_U32++;
        Delta_U32 = Bof_ElapsedMsTime(Start_U32);
      }
    } while (Delta_U32 < mBofSocketServerParam_X.PollControlListenerTimeoutInMs_U32);
    if ((int)i_U32 > lis)
    {
      lis = i_U32;
    }
#endif
    if (pChannel)
    {
      puSocket.reset(dynamic_cast<BofSocket *>(pChannel));

      if (puSocket)
      {
        Rts_E = puSocket->LastErrorCode();

        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          BofSocketIoParam_X.Name_S = "CMD_POLL_WAIT";
          mpsPollControlReceiver = std::make_shared<BofSocketIo>(nullptr, std::move(puSocket), BofSocketIoParam_X);
          if (mpsPollControlReceiver)
          {
            mpsPollControlReceiver->SessionType(BOF_SOCKET_SESSION_TYPE::CMD_POLL_WAIT);

            SessionId = mpsPollControlReceiver->NativeBofSocketPointer()->GetSocketHandle();

            LockThreadCriticalSection("V_OnProcessing");
            mSessionCollection[SessionId] = mpsPollControlReceiver;
            UnlockThreadCriticalSection();

            mpPollOp_X[mNbPollEntry].Fd = SessionId;
            mpPollOp_X[mNbPollEntry].Event_U16 = BOF_POLL_IN;
            mNbPollEntry++;
            BOF_ASSERT(mNbPollEntry == 1);
          }
        }
      }
    }
  }
  // if (Rts_E != BOF_ERR_NO_ERROR)
  //{
  //	tt++;
  //	goto l;
  // }
  BOF_ASSERT(mNbPollEntry == 1);

  BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);

  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    while (!IsThreadLoopMustExit())
    {
      EndOfCmdProcessing_B = false;
      Sts_E = Poll(POLL_TIMEOUT_IN_MS, NbPollSet_U32, PollSocketCmd_X);
      // BOF_DBG_PRINTF("----------------------------------POLL thread of %s: PollRcv Id %08X NbPoll %d NbSet %d Cmd '%c' for Id %08X\n", mBofSocketServerParam_X.Name_S.c_str(), mpPollOp_X[0].Fd, mNbPollEntry.load(), NbPollSet_U32,
      // PollSocketCmd_X.SocketOp_E, PollSocketCmd_X.SessionId);

      /*
      BOF_POLL_HUP means the socket is no longer connected. In TCP, this means FIN has been received and sent.
      BOF_POLL_ERR means the socket got an asynchronous error. In TCP, this typically means a RST has been received or sent. If the file descriptor is not a socket, BOF_POLL_ERR might mean the device does not support polling.
      For both of the conditions above, the socket file descriptor is still open, and has not yet been closed (but shutdown() may have already been called). A close() on the file descriptor will release resources that are still being reserved on behalf of
      the socket. In theory, it should be possible to reuse the socket immediately (e.g., with another connect() call).

      BOF_POLL_NVAL means the socket file descriptor is not open. It would be an error to close() it.
      */

      if (Sts_E == BOF_ERR_NO_ERROR)
      {
        if (PollSocketCmd_X.SocketOp_E != BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_QUIT)
        {
          if (NbPollSet_U32)
          {
            // BOF_DBG_PRINTF("thread of %s\n", mBofSocketServerParam_X.Name_S.c_str());
            // BOF_DBG_PRINTF("%s", ConnectionDebugInfo().c_str());
          }

          NbPollSignaled_U32 = 0;
          for (i_U32 = 1; i_U32 < mNbPollEntry; i_U32++) // mpPollOp_X[0] has been managed by BofSocketSessionManager::Poll
          {
            REvent_U16 = mpPollOp_X[i_U32].Revent_U16;
            if (REvent_U16)
            {
              NbPollSignaled_U32++;
            }

            if (REvent_U16) // NbPollSet_U32)
            {
              //							BOF_DBG_PRINTF("@@@%s iter %d on %d evt %04X sz %d chk %d/%d Fd[%d]=%08X evt %04X\n",  mBofSocketServerParam_X.Name_S.c_str(), i_U32, mNbPollEntry.load(), REvent_U16, static_cast<uint32_t>(mSessionCollection.size()),
              //NbPollSignaled_U32, NbPollSet_U32, i_U32, mpPollOp_X[i_U32].Fd, REvent_U16);
            }
            Rts_E = BOF_ERR_NOT_FOUND;
            SessionId = mpPollOp_X[i_U32].Fd;
            psSocketSession = FindAndGetBofSocketIo(" Find_V_OnProcessing_1", SessionId);
            if (psSocketSession != nullptr)
            {
              if (REvent_U16)
              {
                //								BOF_DBG_PRINTF("@@@%s evt %X LastIoTime %d now %d -> %d ms for %d:%p id %08X typ %d\n",  mBofSocketServerParam_X.Name_S.c_str(), REvent_U16, psSocketSession->LastIoTimeInMs(), Bof_GetMsTickCount(), Bof_GetMsTickCount() -
                //psSocketSession->LastIoTimeInMs(), psSocketSession.use_count(), psSocketSession.get(), psSocketSession->SessionId(), psSocketSession->SessionType());

                psSocketSession->LastIoTimeInMs(Bof_GetMsTickCount());
                switch (psSocketSession->SessionType())
                {
                default:
                case BOF_SOCKET_SESSION_TYPE::UNKNOWN:
                case BOF_SOCKET_SESSION_TYPE::CMD_POLL_WAIT:
                  BOF_ASSERT(0); // mpPollOp_X[0] has been managed by BofSocketSessionManager::Poll
                  break;

                case BOF_SOCKET_SESSION_TYPE::SERVER_LISTENER:
                  if (REvent_U16 & BOF_POLL_IN) // First data
                  {
                    if (mpBofSocketServer) // Only BOF_SOCKET_SESSION_TYPE::SERVER_LISTENER/DATA_LISTENER have a psSocketServer
                    {
                      Sts_E = mpBofSocketServer->SignalConnectionRequest(psSocketSession->NativeBofSocketPointer());
                    }
                  }
                  if (REvent_U16 & (BOF_POLL_HUP | BOF_POLL_RDHUP | BOF_POLL_ERR | BOF_POLL_NVAL)) // After error which can be set also with BOF_POLL_IN (Data channel write and closed)
                  {
                  }
                  break;

                case BOF_SOCKET_SESSION_TYPE::DATA_LISTENER:
                  if (REvent_U16 & BOF_POLL_IN) // First data
                  {
                    psParentSocketSession = psSocketSession->ParentCmdChannel().expired() ? nullptr : psSocketSession->ParentCmdChannel().lock();
                    if ((mpBofSocketServer) && (psSocketSession->NativeBofSocketPointer()) && (psParentSocketSession)) // only BOF_SOCKET_SESSION_TYPE::SERVER_LISTENER/DATA_LISTENER have a psSocketServer
                    {
                      Sts_E = BOF_ERR_ECONNREFUSED;
                      pChannel = psSocketSession->NativeBofSocketPointer()->V_Listen(0, "");
                      if (pChannel)
                      {
                        puDataSocket.reset(dynamic_cast<BofSocket *>(pChannel));
                        if (puDataSocket)
                        {
                          ////BOF_DBG_PRINTF("@@@%s DATA_LISTENER Parent typ %d Id %08X To %d ses %d   Vs   Sock typ %d Id %08X To %d ses %d\n",  mBofSocketServerParam_X.Name_S.c_str(), psParentSocketSession->SessionType(),
                          ///psParentSocketSession->SessionId(), psParentSocketSession->NoIoCloseTimeoutInMs(), psParentSocketSession->SessionIndex(), psSocketSession->SessionType(), psSocketSession->SessionId(), psSocketSession->NoIoCloseTimeoutInMs(),
                          ///psSocketSession->SessionIndex());
                          Sts_E = mpBofSocketServer->ConnectSession(BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL, psParentSocketSession, psParentSocketSession->NoIoCloseTimeoutInMs(), std::move(puDataSocket), psParentSocketSession->SessionIndex(),
                                                                    psDataSocketSession);
                        }
                      }
                    }
                  }
                  if (REvent_U16 & (BOF_POLL_HUP | BOF_POLL_RDHUP | BOF_POLL_ERR | BOF_POLL_NVAL)) // After error which can be set also with BOF_POLL_IN (Data channel write and closed)
                  {
                  }
                  ////BOF_DBG_PRINTF("@@@%s DATA_LISTENER Start Remove Id %08X %d/%d\n",  mBofSocketServerParam_X.Name_S.c_str(), SessionId, i_U32, mNbPollEntry.load());
                  Sts_E = RemovePollEntry(false, SessionId);
                  if (Sts_E == BOF_ERR_NO_ERROR) // We are in the for (i_U32 = 1; i_U32 < mNbPollEntry_U32; i_U32++) loop AND mNbPollEntry_U32 has been reduced -> dec i_U32 to prevent  i_U32++ in loop
                  {
                    i_U32--;
                  }
                  ////BOF_DBG_PRINTF("@@@%s DATA_LISTENER End Remove Id %08X Sts %d i %d/%d\n",  mBofSocketServerParam_X.Name_S.c_str(), SessionId, Sts_E,i_U32,mNbPollEntry.load());
                  break;

                case BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL:
                case BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL:
                case BOF_SOCKET_SESSION_TYPE::POLL_CHANNEL:

                  if (REvent_U16 & BOF_POLL_IN) // First data
                  {
                    if (psSocketSession->Connected())
                    {
                      if (psSocketSession->NativeBofSocketPointer())
                      {
                        if (psSocketSession->SessionType() == BOF_SOCKET_SESSION_TYPE::POLL_CHANNEL)
                        {
                          Sts_E = psSocketSession->V_SignalPoll(REvent_U16, psSocketSession);
                        }
                        else
                        {
                          Sts_E = psSocketSession->ParseAndDispatchIncomingData(0); // TODO: Cmd or data byte stream
                          // BOF_DBG_PRINTF("@@@%s ParseAndDispatchIncomingData Id %08X Sts %s\n",  mBofSocketServerParam_X.Name_S.c_str(), SessionId, Bof_ErrorCode(Sts_E));
                          if (Sts_E == BOF_ERR_ENETRESET)
                          {
                            REvent_U16 |= BOF_POLL_HUP; // Force it to avoid duplicate V_SignalError (below and if (REvent_U16 &  (BOF_POLL_HUP | BOF_POLL_RDHUP | BOF_POLL_ERR | BOF_POLL_NVAL))	//After err
                            Sts_E = BOF_ERR_NO_ERROR;
                            /*
                                                      psSocketSession->V_SignalError(psSocketSession->SocketUserArg(), Sts_E, "V_OnProcessing2", true);
                                                      Sts_E = RemovePollEntry(false, SessionId);
                                                      if (Sts_E == BOF_ERR_NO_ERROR)	//We are in the for (i_U32 = 1; i_U32 < mNbPollEntry_U32; i_U32++) loop AND mNbPollEntry_U32 has been reduced -> dec i_U32 to prevent  i_U32++ in loop
                                                      {
                                                        i_U32--;
                                                      }
                            */
                          }
                        }
                        BOF_ASSERT((Sts_E == BOF_ERR_NO_ERROR) || (Sts_E == BOF_ERR_EMPTY)); // data cannot be read->Stay in BOF_POLL_IN
                      }
                    }
                  }

                  if (REvent_U16 & BOF_POLL_OUT)
                  {
                  }

                  if (REvent_U16 & (BOF_POLL_HUP | BOF_POLL_RDHUP | BOF_POLL_ERR | BOF_POLL_NVAL)) // After error which can be set also with BOF_POLL_IN (Data channel write and closed)
                  {
                    if (psSocketSession->NotifyType() == BOF_SOCKET_IO_NOTIFY_TYPE::WHEN_FULL_OR_CLOSED)
                    {
                      Sts_E = psSocketSession->NotifyPendingData();
                    }
                    Sts_E = BOF_ERR_INTERNAL;
                    //	if (SocketSession_X.pSocketIo)  //BOF_SOCKET_SESSION_TYPE::CANCEL/LISTENER has no session
                    if (REvent_U16 & (BOF_POLL_HUP | BOF_POLL_RDHUP))
                    {
                      Sts_E = BOF_ERR_ENETRESET;
                    }
                    else if (REvent_U16 & BOF_POLL_ERR)
                    {
                      Sts_E = BOF_ERR_ECONNABORTED;
                    }
                    else if (REvent_U16 & BOF_POLL_NVAL)
                    {
                      Sts_E = BOF_ERR_NOT_OPENED;
                    }
                    psSocketSession->V_SignalError(Sts_E, "V_OnProcessing1", true);
                    Sts_E = RemovePollEntry(false, SessionId);
                    BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
                    if (Sts_E == BOF_ERR_NO_ERROR) // We are in the for (i_U32 = 1; i_U32 < mNbPollEntry_U32; i_U32++) loop AND mNbPollEntry_U32 has been reduced -> dec i_U32 to prevent  i_U32++ in loop
                    {
                      i_U32--;
                    }
                  }

                } // switch
              }
              else // if (REvent_U16)
              {
                switch (psSocketSession->SessionType())
                {
                default:
                case BOF_SOCKET_SESSION_TYPE::UNKNOWN:
                case BOF_SOCKET_SESSION_TYPE::CMD_POLL_WAIT:
                  BOF_ASSERT(0); // mpPollOp_X[0] has been managed by BofSocketSessionManager::Poll
                  break;

                case BOF_SOCKET_SESSION_TYPE::SERVER_LISTENER:
                  break;

                case BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL:
                case BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL:
                case BOF_SOCKET_SESSION_TYPE::DATA_LISTENER:
                case BOF_SOCKET_SESSION_TYPE::POLL_CHANNEL:
                  if (psSocketSession->NoIoCloseTimeoutInMs())
                  {
                    Delta_U32 = Bof_ElapsedMsTime(psSocketSession->LastIoTimeInMs());
                    if (Delta_U32 > psSocketSession->NoIoCloseTimeoutInMs())
                    {
                      ////BOF_DBG_PRINTF("@@@%s Timeout -> I'm going to RemovePollEntry Id %08X delta %d/%d for %d:%p\n",  mBofSocketServerParam_X.Name_S.c_str(), SessionId, Delta_U32, psSocketSession->NoIoCloseTimeoutInMs(), psSocketSession.use_count(),
                      ///(void *)psSocketSession.get());

                      Sts_E = RemovePollEntry(false, SessionId);
                      if (Sts_E == BOF_ERR_NO_ERROR) // We are in the for (i_U32 = 1; i_U32 < mNbPollEntry_U32; i_U32++) loop AND mNbPollEntry_U32 has been reduced -> dec i_U32 to prevent  i_U32++ in loop
                      {
                        i_U32--;
                      }
                      ////BOF_DBG_PRINTF("@@@%s Timeout i %d Nb %d/%d\n",  mBofSocketServerParam_X.Name_S.c_str(), i_U32, NbPollSignaled_U32, NbPollSet_U32);
                    }
                  }
                  break;
                }
              }
            }
            else // if (psSocketSession != nullptr)
            {
              EntryPresentInOpList_B = (PollSocketCmd_X.SessionId == SessionId);
              if ((PollSocketCmd_X.SocketOp_E != BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_TEST) && (PollSocketCmd_X.SocketOp_E != BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_NONE))
              {
                // Element just erased ?
                if ((PollSocketCmd_X.SocketOp_E != BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_REMOVE_ENTRY) || (!EntryPresentInOpList_B))
                {
                  BOF_ASSERT(0);
                }
              }
            }
          } // for (i_U32 = 1; i_U32 < mNbPollEntry_U32; i_U32++)

          BOF_ASSERT(NbPollSignaled_U32 == NbPollSet_U32);
        } // if (PollSocketCmd_X.SocketOp_E != BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_QUIT)
        switch (PollSocketCmd_X.SocketOp_E)
        {
        case BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_NONE:
          break;

        case BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_QUIT:
          ////BOF_DBG_PRINTF("@@@%s POLL_SOCKET_OP_QUIT\n",  mBofSocketServerParam_X.Name_S.c_str());
          EndOfCmdProcessing_B = true;
          Rts_E = BOF_ERR_STOPPED; // Not  BOF_ERR_CANCEL !
          SEND_OPTIONAL_ANSWER(Rts_E);
          break;

        case BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_ADD_ENTRY:
          // BOF_DBG_PRINTF("@@@%s POLL_SOCKET_OP_ADD_ENTRY NbPollEntry %d Max %d Id %08X\n",  mBofSocketServerParam_X.Name_S.c_str(), mNbPollEntry.load(), mNbMaxPollEntry_U32, PollSocketCmd_X.SessionId);
          Sts_E = BOF_ERR_TOO_BIG;
          if (mNbPollEntry < mNbMaxPollEntry_U32)
          {
            Sts_E = BOF_ERR_NOT_FOUND;
            psAddSocketSession = FindAndGetBofSocketIo(" Find_V_OnProcessing_2", PollSocketCmd_X.SessionId);
            if (psAddSocketSession != nullptr)
            {
              Sts_E = psAddSocketSession->V_SignalOpened(psAddSocketSession->SessionIndex(), psAddSocketSession); // , (psSocketSession->ParentCmdChannel().expired()) ? nullptr : psSocketSession->ParentCmdChannel().lock());
              // BOF_DBG_PRINTF("@@@%s POLL_SOCKET_OP_ADD_ENTRY AddIt Fd[%d]=%08X Session %d\n", mBofSocketServerParam_X.Name_S.c_str(), mNbPollEntry.load(), PollSocketCmd_X.SessionId, psAddSocketSession->SessionIndex());
              if (Sts_E == BOF_ERR_NO_ERROR)
              {
                mpPollOp_X[mNbPollEntry].Fd = PollSocketCmd_X.SessionId;
                mpPollOp_X[mNbPollEntry].Event_U16 = (BOF_POLL_IN | BOF_POLL_RDHUP);
                mNbPollEntry++;
                if ((psAddSocketSession->SessionType() == BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL) || (psAddSocketSession->SessionType() == BOF_SOCKET_SESSION_TYPE::POLL_CHANNEL))
                {
                  LockThreadCriticalSection(" ConnectSession");
                  mConnectedSessionCollection.push_back(psAddSocketSession);
                  UnlockThreadCriticalSection();
                }
              }
            }
          }
          SEND_OPTIONAL_ANSWER(Sts_E);
          BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
          break;

        case BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_REMOVE_ENTRY:
          // BOF_DBG_PRINTF("@@@%s POLL_SOCKET_OP_REMOVE_ENTRY NbPollEntry %d RemItT Fd[?]=%08X\n",  mBofSocketServerParam_X.Name_S.c_str(), mNbPollEntry.load(), PollSocketCmd_X.SessionId);
          Sts_E = RemovePollEntry(false, PollSocketCmd_X.SessionId);
          BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
          SEND_OPTIONAL_ANSWER(Sts_E);
          break;

        case BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_TEST:
          ////BOF_DBG_PRINTF("@@@%s POLL_SOCKET_OP_TEST NbPollEntry %d RemItT Fd[?]=%08X\n",  mBofSocketServerParam_X.Name_S.c_str(), mNbPollEntry.load(), PollSocketCmd_X.SessionId);
          mNbPollEntry++;
          SEND_OPTIONAL_ANSWER(BOF_ERR_NO_ERROR);
          break;

        default:
          break;
        } // switch
        if (EndOfCmdProcessing_B)
        {
          break;
        }
      } // if (Sts_E == BOF_ERR_NO_ERROR)
      else
      {
#if defined(_WIN32) //	WSACleanup(); has been called in bof_destructor
        int32_t Win32Err_S32 = 0;
        Bof_GetLastError(true, &Win32Err_S32);
        if (Win32Err_S32 == 10093) // Either the application has not called WSAStartup, or WSAStartup failed.
        {
          Bof_MsSleep(POLL_TIMEOUT_IN_MS);
        }
        else
        {
          BOF_ASSERT(0);
        }
#else
        BOF_ASSERT(0);
#endif
      }
    } // while (!IsThreadLoopMustExit())
  }
  LockThreadCriticalSection(" Exit BofSocketSessionManager::V_OnProcessing"); // Get the lock as it has been set by BofThread::BofThread_Thread()
  return Rts_E;
}

END_BOF_NAMESPACE()