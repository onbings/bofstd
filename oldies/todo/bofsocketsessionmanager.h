/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the BofSocketSessionManager class.
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

#include <bofstd/bofsocketio.h>
#include <bofstd/bofthread.h>
#include <bofstd/ibofsocketsessionfactory.h>

#include <map>

BEGIN_BOF_NAMESPACE()
enum class BOF_SOCKET_SERVER_MODE : uint32_t
{
  BOF_SOCKET_SERVER_SESSION = 0,
  BOF_SOCKET_SERVER_POLLER,
};
class BofSocketServer;
struct BOF_SOCKET_SERVER_PARAM
{
  BOF_SOCKET_SERVER_MODE ServerMode_E;
  std::string Name_S;
  uint64_t ThreadCpuCoreAffinityMask_U64;
  bool PriorityInversionAware_B;
  BOF_THREAD_SCHEDULER_POLICY ThreadSchedulerPolicy_E;
  BOF_THREAD_PRIORITY ThreadPriority_E;
  std::string Address_S;
  uint32_t NbMaxSession_U32;
  uint16_t MinPortValue_U16;
  uint16_t MaxPortValue_U16;
  uint16_t PollControlListenerPort_U16;
  uint32_t PollControlListenerTimeoutInMs_U32;

  BOF_SOCKET_SERVER_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    ServerMode_E = BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION;
    Name_S = "";
    ThreadCpuCoreAffinityMask_U64 = 0;
    PriorityInversionAware_B = false;
    ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
    ThreadPriority_E = BOF_THREAD_PRIORITY_000;
    Address_S = "";
    NbMaxSession_U32 = 1;
    MinPortValue_U16 = 50000;
    MaxPortValue_U16 = 65535;
    PollControlListenerPort_U16 = 49152;
    PollControlListenerTimeoutInMs_U32 = 5000;
  }
};

class BOFSTD_EXPORT BofSocketSessionManager : public BofThread
{
private:
  BofSocketServer *mpBofSocketServer = nullptr;
  BOFERR mErrorCode_E = BOF_ERR_INIT;
  uint32_t mNbMaxPollEntry_U32 = 0;   // Maximum number of entries in mpPollOp_X (related to mSessionCollection.size())
  std::atomic<uint32_t> mNbPollEntry; // Curent number of entries in  mpPollOp_X
  std::unique_ptr<BofSocket> mpuPollControlListener = nullptr;
  std::unique_ptr<BofSocket> mpuPollControlSender = nullptr;
  std::shared_ptr<BofSocketIo> mpsPollControlReceiver = nullptr;
  std::atomic<uint32_t> mAnswerTicket;

protected:
  BOF_SOCKET_SERVER_PARAM mBofSocketServerParam_X;
  IBofSocketSessionFactory *mpIBofSocketSessionFactory;

  BOF_POLL_SOCKET *mpPollOp_X = nullptr;
  std::map<BOFSOCKET, std::shared_ptr<BofSocketIo>> mSessionCollection;

  std::vector<std::shared_ptr<BofSocketIo>> mConnectedSessionCollection;
  bool *mpActiveSession_B = nullptr;
  uintptr_t mIo = BOF_INVALID_HANDLE_VALUE;

public:
  BofSocketSessionManager(IBofSocketSessionFactory *_pIBofSocketSessionFactory, BofSocketServer *_pBofSocketServer, const BOF_SOCKET_SERVER_PARAM &_rBofSocketServerParam_X);
  virtual ~BofSocketSessionManager();
  BofSocketSessionManager &operator=(const BofSocketSessionManager &) = delete; // Disallow copying
  BofSocketSessionManager(const BofSocketSessionManager &) = delete;
  BOFERR BofIoLog(const char *_pFormat_c, ...);

  BOFERR LastErrorCode() const;
  BOFERR StopProcessingThread();
  const BOF_SOCKET_SERVER_PARAM &SocketServerParam() const;
  bool IsSessionActive(uint32_t _SessionIndex_U32) const;
  uint32_t NbConnectedSession() const;
  uint32_t NbConnectedChannel(std::shared_ptr<BofSocketIo> _psSocketSession);
  uint32_t NbPollChannel() const;
  BOFERR WaitForNbConnectedSession(uint32_t _ExpectedConnectedSession_U32, uint32_t _PollTimeInMs_U32, uint32_t _TimeoutInMs_U32, uint32_t _ExpectedPollChannel_U32 = 0xFFFFFFFF);

  // BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION
  BOFERR AddToPollList(BOF_SOCKET_SESSION_TYPE _SessionType_E, std::shared_ptr<BofSocketIo> _psParentCmdChannel, uint32_t _TimeoutInMs_U32, std::shared_ptr<BofSocketIo> _psSocketSession, BOFSOCKET &_rSessionId);
  // BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_POLLER
  BOFERR AddToPollList(uint32_t _SessionId_U32, std::unique_ptr<BofSocket> _puSocket, std::shared_ptr<BofSocketIo> &_rpsSocketSession, BOFSOCKET &_rSessionId);

  BOFERR RemoveFromPollList(uint32_t _TimeoutInMs_U32, std::shared_ptr<BofSocketIo> _psSocketSession, uint32_t &_rSessionIndex_U32);

  BOFERR SendPollSocketCommand(uint32_t _OptionalAnswerTimeoutInMs_U32, BOF_POLL_SOCKET_CMD &_rPollSocketCommand_X);
  BOFERR SocketSessionStatistic(std::shared_ptr<BofSocketIo> _psSocketSession, BOF_SOCKET_SESSION_STATISTIC &_rSocketSessionStatistic_X);
  BOFERR ResetSocketSessionStatistic(std::shared_ptr<BofSocketIo> _psSocketSession);
  BOFERR ResetAllSocketSessionStatistic();
  uint32_t NbPollEntry() const;
  uint32_t NbMaxPollEntry() const;
  std::string SocketSessionManagerDebugInfo();
  std::string ConnectionDebugInfo();

protected:
  std::shared_ptr<BofSocketIo> FindAndGetBofSocketIo(const char *_pLocker_c, BOFSOCKET _SessionId);

private:
  BOFERR RemovePollEntry(bool _OnlyCleanUp_B, BOFSOCKET _SessionId);
  BOFERR Poll(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPollSet_U32, BOF_POLL_SOCKET_CMD &_rPollSocketCommand_X);
  BOFERR V_OnProcessing() override;
};
END_BOF_NAMESPACE()