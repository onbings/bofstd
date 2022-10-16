/*
* Copyright (c) 2015-2025, Onbings. All rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
* PURPOSE.
*
* This module defines the BofSocketServer class.
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

#include <bofstd/bofsocketsessionmanager.h>
#include <vector>

BEGIN_BOF_NAMESPACE()

//DP https://forum.libcinder.org/topic/solution-calling-shared-from-this-in-the-constructor and https://embeddedartistry.com/blog/2016/10/18/embedded-c-sharedfromthis
class BOFSTD_EXPORT BofSocketServer : public BofSocketSessionManager	//, public std::enable_shared_from_this<BofSocketServer>
{
private:
  BOFERR                    mErrorCode_E = BOF_ERR_INIT;
  std::mutex								mDynamicPortMtx;
  uint16_t									mDynamicPort_U16 = 0;
  bool											mHasAListener_B = false;
  BOF_SOCKET_ADDRESS			  mSocketServerAddress_X;

public:
  BofSocketServer(IBofSocketSessionFactory *_pIBofSocketSessionFactory, const BOF_SOCKET_SERVER_PARAM &_rBofSocketServerParam_X);
  virtual ~BofSocketServer();
  BofSocketServer &operator = (const BofSocketServer &) = delete; // Disallow copying
  BofSocketServer(const BofSocketServer &) = delete;
  BOFERR LastErrorCode() const;
  BOF_SOCKET_ADDRESS	SocketServerAddress();
  BOFERR WriteToAllSession(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, const std::string &_rBuffer_S, void *_pWriteContext);
  BOFERR WriteToAllSession(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, uint32_t _Nb_U32, const uint8_t *_pBuffer_U8, void *_pWriteContext);

  BOFERR Connect(uint32_t _TimeoutInMs_U32, const std::string &_rConnectFromIpAddress_S, const std::string &_rConnectToIpAddress_S, std::shared_ptr<BofSocketIo> &_rpsCmdSocketSession);
  BOFERR Disconnect(bool _CloseDataChannelIfPresent_B, std::shared_ptr<BofSocketIo> _psCmdSocketSession);

  BOFERR ConnectToDataChannel(bool _Passive_B, std::shared_ptr<BofSocketIo> _psCmdSocketSession, uint32_t _ConnectionTimeoutInMs_U32, uint32_t _NoIoCloseTimeoutInMs_U32, std::shared_ptr<BofSocketIo> &_rpsDataSocketSession);
  //Answer to pasv
  BOFERR ListenForDataChannelConnection(bool _Passive_B, std::shared_ptr<BofSocketIo> _psCmdSocketSession, uint32_t _ConnectionTimeoutInMs_U32, const std::string &_rIpAddress_S);
  BOFERR CloseDataChannel(std::shared_ptr<BofSocketIo> _psDataSocketSession);

  BOFERR    SignalConnectionRequest(BofSocket *_pListenSocket);
  uint16_t	GenerateDynamicPort();
  std::string SocketServerDebugInfo();
  //BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION
  BOFERR	ConnectSession(BOF_SOCKET_SESSION_TYPE _SessionType_E, std::shared_ptr<BofSocketIo> _psParentCmdChannel, uint32_t _TimeoutInMs_U32, std::unique_ptr<BofSocket>	_puSocket, uint32_t _SessionIndex_U32, std::shared_ptr<BofSocketIo> &_rpsSocketSession);
  //BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_POLLER
  BOFERR	ConnectSession(uint32_t _SessionIndex_U32, std::unique_ptr<BofSocket> _puSocket, std::shared_ptr<BofSocketIo> &_rpsSocketSession);

  std::shared_ptr<BofSocketIo> ConnectedCmdSession(uint32_t _Index_U32, uint32_t _PollTimeInMs_U32, uint32_t _TimeoutInMs_U32);
  std::shared_ptr<BofSocketIo> ConnectedDataSession(uint32_t _Index_U32, uint32_t _PollTimeInMs_U32, uint32_t _TimeoutInMs_U32);
  bool		LockFreeSessionIndex(uint32_t _SessionIndex_U32);

private:
  uint32_t LookAndLockFreeSessionIndex();
};

END_BOF_NAMESPACE()