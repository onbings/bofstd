/*
 * Copyright (c) 2013-2025, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofio module
 *
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Jan 05 2019  BHA : Initial release
 */
#include <bofstd/bofsocketserver.h>

#include "gtestrunner.h"

#include <inttypes.h>
#include <regex>

USE_BOF_NAMESPACE()
/*
* See FindFreePort in this file
---Range: 50000-50059: 60 entries------------------------
---Range: 50360-50466: 107 entries------------------------
---Range: 50967-52828: 1862 entries------------------------
---Range: 52830-54034: 1205 entries------------------------
---Range: 54036-54036: 1 entries------------------------
---Range: 54038-56952: 2915 entries------------------------
---Range: 56954-56958: 5 entries------------------------
---Range: 56961-56961: 1 entries------------------------
---Range: 56966-57007: 42 entries------------------------
---Range: 57009-57863: 855 entries------------------------
---Range: 57865-58333: 469 entries------------------------
---Range: 58335-59803: 1469 entries------------------------
---Range: 59805-59999: 195 entries------------------------
10000 tested between 50000 and 60000:
  9186 free
  814 busy
*/
constexpr uint32_t WAIT_FOR_POLL_PERIOD = 500;
constexpr uint16_t DEFAULT_PORT = 55000;
constexpr uint16_t MIN_PORT_VALUE = 55000;
constexpr uint16_t MAX_PORT_VALUE = 55099;
constexpr uint16_t CONTROL_LISTER_MIN_PORT_VALUE = 55100;
constexpr uint16_t CONTROL_LISTER_MAX_PORT_VALUE = 55199;
static uint16_t S_ControlListerPort_U16 = CONTROL_LISTER_MIN_PORT_VALUE;

constexpr uint32_t NB_MAX_H4X_CLIENT = 10;
constexpr uint32_t SEND_CMD_ANSWER_TIMEOUT_IN_MS = 5000;

#if defined(_WIN32)
constexpr uint32_t NO_IO_CLOSE_TIMEOUT_IN_MS = 45000; // 20000;
constexpr uint32_t NB_MAX_SESSION = 50;
constexpr uint32_t NB_MAX_CLIENT = 50;
#else
constexpr uint32_t NO_IO_CLOSE_TIMEOUT_IN_MS = 45000;
constexpr uint32_t NB_MAX_SESSION = 32;
constexpr uint32_t NB_MAX_CLIENT = 32;
// constexpr uint16_t MIN_PORT_VALUE = 60000;	//Different from linux below to avoid test corruptionwhen doing remote debug on linux and test on win in //
// constexpr uint16_t MAX_PORT_VALUE = 64999;

#endif
// constexpr uint32_t MAX_BIG_BUFFER_SIZE = 0x8000000;
// uint8_t S_pBuffer_U8[MAX_BIG_BUFFER_SIZE];

// static uint32_t S_NbListenForDataChannelConnection_U32 = 0;	//6
static uint32_t S_NbNotConnected_U32 = 0;
static uint32_t S_NbBofSocketBalance_U32 = 0;
static std::vector<std::string> S_NowCollection;

struct XT_FILE
{
  uint16_t IdLsm_U16;
  uint8_t Camera_U8;
  uint16_t MacroSeq_U16;
  char pGuid_c[10];
  uint32_t In_U32;
  uint32_t Out_U32;
  uint8_t BlockSizeInMB_U8;
  uint32_t SizeInBlock_U32;
  uint64_t SizeInByte_U64;
  bool IsATrain_B;

  XT_FILE()
  {
    Reset();
  }

  void Reset()
  {
    IdLsm_U16 = 0;
    Camera_U8 = 0;
    MacroSeq_U16 = 0xFFFF;
    pGuid_c[0] = 0;
    In_U32 = 0;
    Out_U32 = 0;
    IsATrain_B = false;
  }
};
class MyTcpSession;
class MyTcpServer : public IBofSocketSessionFactory, public BofSocketServer
{
private:
public:
  MyTcpServer(const BOF_SOCKET_SERVER_PARAM &_rBofSocketServerParam_X);
  ~MyTcpServer();
  std::shared_ptr<BofSocketIo> V_OpenSession(BOF_SOCKET_SESSION_TYPE _SessionType_E, uint32_t _SessionIndex_U32, std::unique_ptr<BofSocket> _puSocket);
  BOFERR V_CloseSession(std::shared_ptr<BofSocketIo> _psSession);
};

class MyTcpSession : public BofSocketIo //, public std::enable_shared_from_this<MyTcpSession>
{
private:
  uint32_t mSessionId_U32 = 0xFFFFFFFF;
  BOF_SOCKET_SESSION_TYPE mSessionType_E = BOF_SOCKET_SESSION_TYPE::UNKNOWN;
  MyTcpServer *mpMyTcpServer = nullptr;
  std::shared_ptr<BofSocketIo> mpsDataSocketSession = nullptr;

public:
  MyTcpSession(MyTcpServer *_pMyTcpServer, std::unique_ptr<BofSocket> _puCmdSocket, const BOF_SOCKET_IO_PARAM &_rBofSocketIoParam_X)
      : BofSocketIo(_pMyTcpServer, std::move(_puCmdSocket), _rBofSocketIoParam_X)
  {
    mpMyTcpServer = _pMyTcpServer;
  }
  ~MyTcpSession()
  {
  }
  BOFERR V_SignalConnected(BOF_SOCKET_SESSION_TYPE _SessionType_E) override
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;

    mSessionType_E = _SessionType_E;
    //		BOF_DBG_PRINTF("###%s[%d] V_SignalConnected Type %d\n", mpMyTcpServer->SocketServerParam().Name_S.c_str(), mSessionId_U32, (int)mSessionType_E);
    return Rts_E;
  }

  BOFERR V_SignalOpened(uint32_t _SessionId_U32, std::shared_ptr<BofSocketIo> /*_psSocketSession*/) override
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;

    mSessionId_U32 = _SessionId_U32;
    //		BOF_DBG_PRINTF("###%s[%d] V_SignalOpened Type %d\n", mpMyTcpServer->SocketServerParam().Name_S.c_str(), mSessionId_U32, (int)mSessionType_E);
    if (mSessionType_E == BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL)
    {
      Rts_E = Write(100, false, "DataStreamSession from session", nullptr);
    }
    else
    {
      Rts_E = Write(100, false, "220 EVS FTP Server (v) 16.07 (mc) 160202 (d) 16/01/2018 (a) B.Harmel [L:10.129.171.21:21 R:10.129.171.30:57609]\r\n", nullptr);
    }
    BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
    return Rts_E;
  }

  BOFERR V_SignalDataRead(uint32_t _Nb_U32, const uint8_t *_pBuffer_U8) override
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;
    std::string Cmd_S(reinterpret_cast<const char *>(_pBuffer_U8), _Nb_U32);

    //	BOF_DBG_PRINTF("###%s[%d] V_SignalDataRead %d:%p Msg:'%s'\n", mpMyTcpServer->SocketServerParam().Name_S.c_str(), mSessionId_U32,  _Nb_U32, _pBuffer_U8, Cmd_S.c_str());
    if (Cmd_S.substr(0, 4) == "USER")
    {
      Rts_E = Write(NO_IO_CLOSE_TIMEOUT_IN_MS, false, "331 User name okay, need password\r\n", nullptr);
    }
    if (Cmd_S.substr(0, 4) == "PASS")
    {
      Rts_E = Write(NO_IO_CLOSE_TIMEOUT_IN_MS, false, "230 Password ok\r\n", nullptr);
    }
    if (Cmd_S.substr(0, 4) == "TYPE")
    {
      Rts_E = Write(NO_IO_CLOSE_TIMEOUT_IN_MS, false, "200 Type ok\r\n", nullptr);
    }
    if (Cmd_S.substr(0, 4) == "PASV")
    {
      Rts_E = mpMyTcpServer->ListenForDataChannelConnection(true, shared_from_this(), NO_IO_CLOSE_TIMEOUT_IN_MS / 2, "127.0.0.1");
    }
    BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);

    return Rts_E;
  }
  BOFERR V_SignalDataWritten(BOFERR /*_Sts_E*/, uint32_t /*_NbSent_U32*/, const uint8_t * /*_pBufferSent_U8*/, uint32_t /*_NbRemain_U32*/, const uint8_t * /*_pRemainingBuffer_U8*/, void * /*_pWriteContext*/) override
  // BOFERR V_SignalDataWritten(BOFERR _Sts_E, uint32_t _NbSent_U32, const uint8_t * _pBufferSent_U8, uint32_t _NbRemain_U32, const uint8_t * _pRemainingBuffer_U8, void * _pWriteContext) override
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;

    // BOF_DBG_PRINTF("###%s[%d] V_SignalDataWritten Sts %08X (%08X:%p, %08X:%p, %p) %s\n", mpMyTcpServer->SocketServerParam().Name_S.c_str(), mSessionId_U32,_Sts_E, _NbSent_U32, _pBufferSent_U8, _NbRemain_U32, _pRemainingBuffer_U8, _pUserArg,
    // std::string(reinterpret_cast<const char *>(_pBufferSent_U8), _NbSent_U32).c_str()); mSndDataCollection.push_back(std::string(reinterpret_cast<const char *>(_pRemainingBuffer_U8), _NbRemain_U32));
    return Rts_E;
  }
  BOFERR V_SignalError(BOFERR /*_Error_E*/, const char * /*_pInfo_c*/, bool /*_SessionWillBeClosed_B*/) override
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;

    // BOF_DBG_PRINTF("###%s[%d] V_SignalError (%s, %s, %s)\n", mpMyTcpServer->SocketServerParam().Name_S.c_str(), mSessionId_U32, Bof_ErrorCode(_Error_E), _pInfo_c ? _pInfo_c : "nullptr", _SessionWillBeClosed_B ? "True" : "False");
    return Rts_E;
  }

  BOFERR V_SignalDisconnected() override
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;

    // BOF_DBG_PRINTF("###%s[%d] V_SignalDisconnected\n", mpMyTcpServer->SocketServerParam().Name_S.c_str(), mSessionId_U32);

    return Rts_E;
  }
  BOFERR V_SignalClosed() override
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;

    // BOF_DBG_PRINTF("###%s[%d] V_SignalClosed\n", mpMyTcpServer->SocketServerParam().Name_S.c_str(), mSessionId_U32);
    return Rts_E;
  }
};

MyTcpServer::MyTcpServer(const BOF_SOCKET_SERVER_PARAM &_rBofSocketServerParam_X)
    : BofSocketServer(this, _rBofSocketServerParam_X)
{
  //	BOF_DBG_PRINTF("###%s Alive at %s\n", SocketServerParam().Name_S.c_str(), SocketServerParam().Address_S.c_str());
}
MyTcpServer::~MyTcpServer()
{
  //	StopProcessingThread();

  //	BOF_DBG_PRINTF("###%s Delete\n", SocketServerParam().Name_S.c_str());
}

std::shared_ptr<BofSocketIo> MyTcpServer::V_OpenSession(BOF_SOCKET_SESSION_TYPE _SessionType_E, uint32_t /*_SessionIndex_U32*/, std::unique_ptr<BofSocket> _puSocket)
{
  std::shared_ptr<MyTcpSession> psRts = nullptr;
  BOF_SOCKET_IO_PARAM BofSocketIoParam_X;

  BOF_ASSERT(_puSocket != nullptr);
  BofSocketIoParam_X.Name_S = "Session " + std::to_string(NbConnectedSession());
  BofSocketIoParam_X.SocketRcvBufferSize_U32 = 0x10000;
  BofSocketIoParam_X.SocketSndBufferSize_U32 = 0x10000;
  BofSocketIoParam_X.NotifyRcvBufferSize_U32 = 0x4000;
  BofSocketIoParam_X.NotifyType_E = (_SessionType_E == BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL) ? BOF_SOCKET_IO_NOTIFY_TYPE::WHEN_FULL_OR_CLOSED : BOF_SOCKET_IO_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND;
  BofSocketIoParam_X.Delimiter_U8 = '\n';
  BofSocketIoParam_X.pData = nullptr;
  BofSocketIoParam_X.NbMaxAsyncWritePendingRequest_U32 = 0;
  BofSocketIoParam_X.NoIoCloseTimeoutInMs_U32 = NO_IO_CLOSE_TIMEOUT_IN_MS;
  psRts = std::make_shared<MyTcpSession>(this, std::move(_puSocket), BofSocketIoParam_X);
  //	BOF_DBG_PRINTF("###%s sess %p user %p\n", SocketServerParam().Name_S.c_str(),  psRts, (_SessionType_E == BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL) ? &mDataSessionInfo_X : &mCmdSessionInfo_X);

  return psRts;
}

BOFERR MyTcpServer::V_CloseSession(std::shared_ptr<BofSocketIo> _psSession)
{
  BOF_ASSERT(_psSession != nullptr);
  //	MyTcpSession *pMyTcpSession = dynamic_cast<MyTcpSession *>(_psSession.get());
  return BOF_ERR_NO_ERROR;
}

class MyTcpClientServer : public IBofSocketSessionFactory, public BofSocketServer
{
private:
public:
  MyTcpClientServer(const BOF_SOCKET_SERVER_PARAM &_rBofSocketServerParam_X);
  ~MyTcpClientServer();
  std::shared_ptr<BofSocketIo> V_OpenSession(BOF_SOCKET_SESSION_TYPE _SessionType_E, uint32_t _SessionIndex_U32, std::unique_ptr<BofSocket> _puSocket);
  BOFERR V_CloseSession(std::shared_ptr<BofSocketIo> _psSession);
};

class MyTcpClient : public BofSocketIo
{
private:
  uint32_t mClientId_U32 = 0xFFFFFFFF;
  BOF_SOCKET_SESSION_TYPE mSessionType_E = BOF_SOCKET_SESSION_TYPE::UNKNOWN;
  MyTcpClientServer *mpMyTcpClientServer = nullptr;

public:
  MyTcpClient(MyTcpClientServer *_pMyTcpClientServer, std::unique_ptr<BofSocket> _puCmdSocket, const BOF_SOCKET_IO_PARAM &_rBofSocketIoParam_X)
      : BofSocketIo(_pMyTcpClientServer, std::move(_puCmdSocket), _rBofSocketIoParam_X)
  {
    mpMyTcpClientServer = _pMyTcpClientServer;
    BOF_ASSERT(mpMyTcpClientServer != nullptr);
    //		BOF_DBG_PRINTF("###%s[%d] Client Created\n", mpMyTcpClientServer->SocketServerParam().Name_S.c_str(), mClientId_U32);
  }
  ~MyTcpClient()
  {
    // mpMyTcpClientServer is already deleted		BOF_DBG_PRINTF("###%s[%d] Client Deleted\n", mpMyTcpClientServer->SocketServerParam().Name_S.c_str(), mClientId_U32);
  }
  /*
  187 [00004F24] List[0000]='/000000A_0000000B_^mm#Sd1R_00000100_00516A58.TRN'
  187 [00004F24] List[0001]='/000000B_0000000E_^mm#Sd1S_00000100_00516A59.TRN'
  187 [00004F24] List[0002]='/000111A_00000001_^mm#TBaB_003BC594_003BC59A.CLP'
  203 [00004F24] List[0003]='/000111B_0000000D_^mm#TBaC_003BC594_003BC59A.CLP'
  */
  static BOFERR S_ParseXtFilename(const std::vector<BOF_FTP_FILE> &_rFtpFileCollection, std::vector<XT_FILE> &_rXtFileCollection)
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;
    // https://regex101.com/
    static const std::regex S_RegExFn("(.*?)_(.*?)_(.*?)_(.*?)_(.*?)_(.*?)\\.(.*)"); // 000000A_^mm#Sd1R_000B_00000100_00516A58_02.TRN	(tge is 000000A_^mmz_oO2_Train000011             .TRN)
    std::cmatch FnMatch;
    XT_FILE XtFile_X;
    char pIdLsm_c[32];
    uint32_t Len_U32;

    for (const auto &_rItem : _rFtpFileCollection)
    {
      Rts_E = BOF_ERR_FORMAT;

      XtFile_X.Reset();
      if ((std::regex_search(_rItem.Path.FileNameWithExtension().c_str(), FnMatch, S_RegExFn)) && (FnMatch.size() == 1 + 7))
      {
        Rts_E = BOF_ERR_NO_ERROR;
        Bof_StrNCpy(pIdLsm_c, FnMatch[1].str().c_str(), sizeof(pIdLsm_c));
        Len_U32 = static_cast<uint32_t>(strlen(pIdLsm_c));
        if (Len_U32 >= 2)
        {
          XtFile_X.Camera_U8 = pIdLsm_c[Len_U32 - 1];
          pIdLsm_c[Len_U32 - 1] = 0;
          Bof_StringToBin(10, pIdLsm_c, XtFile_X.IdLsm_U16);
        }
        Bof_StrNCpy(XtFile_X.pGuid_c, FnMatch[2].str().c_str(), sizeof(XtFile_X.pGuid_c));
        Bof_StringToBin(16, FnMatch[3].str().c_str(), XtFile_X.MacroSeq_U16);
        Bof_StringToBin(16, FnMatch[4].str().c_str(), XtFile_X.In_U32);
        Bof_StringToBin(16, FnMatch[5].str().c_str(), XtFile_X.Out_U32);
        Bof_StringToBin(16, FnMatch[6].str().c_str(), XtFile_X.BlockSizeInMB_U8);
        XtFile_X.IsATrain_B = (FnMatch[7].str() == "TRN");
        XtFile_X.SizeInByte_U64 = _rItem.Size_U64;
        XtFile_X.SizeInBlock_U32 = (XtFile_X.BlockSizeInMB_U8) ? static_cast<uint32_t>(XtFile_X.SizeInByte_U64 / XtFile_X.BlockSizeInMB_U8 / 1024 / 1024) : 0;
        _rXtFileCollection.push_back(XtFile_X);
      }
    }
    return Rts_E;
  }

  BOFERR V_SignalConnected(BOF_SOCKET_SESSION_TYPE _SessionType_E) override
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;

    mSessionType_E = _SessionType_E;
    if (mSessionType_E == BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL)
    {
      AllocateDataBuffer(0x1000000);
    }
    //		BOF_DBG_PRINTF("###%s[%d] Client V_SignalConnected Type %d\n", mpMyTcpClientServer->SocketServerParam().Name_S.c_str(), mClientId_U32, (int)mSessionType_E);
    return Rts_E;
  }

  BOFERR V_SignalOpened(uint32_t _ClientId_U32, std::shared_ptr<BofSocketIo> /*_psSocketSession*/) override
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;

    mClientId_U32 = _ClientId_U32;
    //	BOF_DBG_PRINTF("###%s[%d] Client V_SignalOpened Type %d\n", mpMyTcpClientServer->SocketServerParam().Name_S.c_str(), mClientId_U32, (int)mSessionType_E);
    ClearDataBuffer();

    BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
    return Rts_E;
  }

  BOFERR V_SignalDataRead(uint32_t _Nb_U32, const uint8_t *_pBuffer_U8) override
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;
    std::string Cmd_S(reinterpret_cast<const char *>(_pBuffer_U8), _Nb_U32);

    // BOF_DBG_PRINTF("###%s[%d] Client V_SignalDataRead %d:%p Type %d Msg:'%s'\n", mpMyTcpClientServer->SocketServerParam().Name_S.c_str(), mClientId_U32, _Nb_U32, _pBuffer_U8,(int)mSessionType_E, Cmd_S.c_str());
    if (mSessionType_E == BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL)
    {
      Rts_E = MemCpyInDataBuffer(_Nb_U32, _pBuffer_U8);
      // BOF_DBG_PRINTF("###%s[%d] Client V_SignalDataRead Copy %ld byte rts %d sz %ld/%ld\n", mpMyTcpClientServer->SocketServerParam().Name_S.c_str(), mClientId_U32, _Nb_U64, Rts_E, DataBuffer().Size_U64, DataBuffer().Capacity_U64);
    }
    BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
    return Rts_E;
  }
  BOFERR V_SignalDataWritten(BOFERR /*_Sts_E*/, uint32_t /*_NbSent_U32*/, const uint8_t * /*_pBufferSent_U8*/, uint32_t /*_NbRemain_U32*/, const uint8_t * /*_pRemainingBuffer_U8*/, void * /*_pWriteContext*/) override
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;

    // BOF_DBG_PRINTF("###%s[%d] Client V_SignalDataWritten Sts %08X (%08X:%p, %08X:%p) %s\n", mpMyTcpClientServer->SocketServerParam().Name_S.c_str(), mClientId_U32,  _Sts_E, _NbSent_U32, _pBufferSent_U8,			_NbRemain_U32, _pRemainingBuffer_U8,
    // std::string(reinterpret_cast<const char *>(_pBufferSent_U8), _NbSent_U32).c_str()); mSndDataCollection.push_back(std::string(reinterpret_cast<const char *>(_pRemainingBuffer_U8), _NbRemain_U32));
    return Rts_E;
  }
  BOFERR V_SignalError(BOFERR /*_Error_E*/, const char * /*_pInfo_c*/, bool /*_SessionWillBeClosed_B*/) override
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;

    // BOF_DBG_PRINTF("###%s[%d] Client V_SignalError (%s, %s, %s)\n", mpMyTcpClientServer->SocketServerParam().Name_S.c_str(), mClientId_U32,  Bof_ErrorCode(_Error_E), _pInfo_c ? _pInfo_c : "nullptr", _SessionWillBeClosed_B ? "True" : "False");
    return Rts_E;
  }

  BOFERR V_SignalDisconnected() override
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;
    std::shared_ptr<BofSocketIo> psParentSocketSession;

    // BOF_DBG_PRINTF("###%s[%d] Client V_SignalDisconnected\n", mpMyTcpClientServer->SocketServerParam().Name_S.c_str(), mClientId_U32);
    if (mSessionType_E == BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL)
    {
      if (CurrentIoDataCommand().substr(0, 4) == "LIST")
      {
        psParentSocketSession = (ParentCmdChannel().expired()) ? nullptr : ParentCmdChannel().lock();
        if (psParentSocketSession)
        {
          // Transfert Ownership
          // psParentSocketSession->DataBuffer(DataBuffer());
          Rts_E = TransferDataBufferOwnershipTo(psParentSocketSession);
        }
      }
    }

    if (mSessionType_E == BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL)
    {
      ReleaseDataBuffer();
    }
    BOF_ASSERT(Rts_E == BOF_ERR_NO_ERROR);
    return Rts_E;
  }
  BOFERR V_SignalClosed() override
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;

    // BOF_DBG_PRINTF("###%s[%d] Client V_SignalClosed\n", mpMyTcpClientServer->SocketServerParam().Name_S.c_str(), mClientId_U32);
    return Rts_E;
  }
};

MyTcpClientServer::MyTcpClientServer(const BOF_SOCKET_SERVER_PARAM &_rBofSocketServerParam_X)
    : BofSocketServer(this, _rBofSocketServerParam_X)
{
  //	BOF_DBG_PRINTF("###%s Alive at %s\n", SocketServerParam().Name_S.c_str(), SocketServerParam().Address_S.c_str());
}
MyTcpClientServer::~MyTcpClientServer()
{
  // StopProcessingThread();

  // BOF_DBG_PRINTF("###%s Delete\n", SocketServerParam().Name_S.c_str());
}

std::shared_ptr<BofSocketIo> MyTcpClientServer::V_OpenSession(BOF_SOCKET_SESSION_TYPE _SessionType_E, uint32_t /*_SessionIndex_U32*/, std::unique_ptr<BofSocket> _puSocket)
{
  std::shared_ptr<MyTcpClient> psRts = nullptr;
  BOF_SOCKET_IO_PARAM BofSocketIoParam_X;

  BOF_ASSERT(_puSocket != nullptr);
  BofSocketIoParam_X.Name_S = "Client " + std::to_string(NbConnectedSession());
  BofSocketIoParam_X.SocketRcvBufferSize_U32 = 0x10000;
  BofSocketIoParam_X.SocketSndBufferSize_U32 = 0x10000;
  BofSocketIoParam_X.NotifyRcvBufferSize_U32 = 0x10000;
  BofSocketIoParam_X.NotifyType_E = (_SessionType_E == BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL) ? BOF_SOCKET_IO_NOTIFY_TYPE::WHEN_FULL_OR_CLOSED : BOF_SOCKET_IO_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND;
  BofSocketIoParam_X.Delimiter_U8 = '\n';
  BofSocketIoParam_X.pData = nullptr;
  BofSocketIoParam_X.NbMaxAsyncWritePendingRequest_U32 = 0;
  BofSocketIoParam_X.NoIoCloseTimeoutInMs_U32 = NO_IO_CLOSE_TIMEOUT_IN_MS;

  psRts = std::make_shared<MyTcpClient>(this, std::move(_puSocket), BofSocketIoParam_X);
  return psRts;
}
BOFERR MyTcpClientServer::V_CloseSession(std::shared_ptr<BofSocketIo> _psSession)
{
  BOF_ASSERT(_psSession != nullptr);
  //	MyTcpClient *pMyTcpClient = dynamic_cast<MyTcpClient *>(_psSession.get());
  return BOF_ERR_NO_ERROR;
}

TEST(SockIo_Client_Server_Test, DISABLED_CreateAndDestroyBofSocketServer)
{
  uint32_t i_U32;
  BOF_POLL_SOCKET_CMD PollSocketCmd_X;

  // S_FindFreePort(50000, 60000);

  BOF_SOCKET_SERVER_PARAM BofSocketServerParam_X;
  BofSocketServerParam_X.ServerMode_E = BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION;
  BofSocketServerParam_X.Name_S = "NoIoCloseTimeoutInMs";
  BofSocketServerParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  BofSocketServerParam_X.ThreadPriority_E = BOF_THREAD_PRIORITY_000;
  BofSocketServerParam_X.ThreadCpuCoreAffinityMask_U64 = 0;

  BofSocketServerParam_X.Address_S = Bof_Sprintf("tcp://0.0.0.0:%d", DEFAULT_PORT);
  BofSocketServerParam_X.NbMaxSession_U32 = 0;
  BofSocketServerParam_X.MinPortValue_U16 = MIN_PORT_VALUE;
  BofSocketServerParam_X.MaxPortValue_U16 = MAX_PORT_VALUE;
  BofSocketServerParam_X.PollControlListenerPort_U16 = S_ControlListerPort_U16++;
  if (S_ControlListerPort_U16 >= CONTROL_LISTER_MAX_PORT_VALUE)
  {
    S_ControlListerPort_U16 = CONTROL_LISTER_MIN_PORT_VALUE;
  }
  BofSocketServerParam_X.PollControlListenerTimeoutInMs_U32 = NO_IO_CLOSE_TIMEOUT_IN_MS;
  /*
  puBofSocketServer = std::make_unique<MyTcpServer>(BofSocketServerParam_X);
  ASSERT_TRUE(puBofSocketServer != nullptr);
  ASSERT_NE(puBofSocketServer->LastErrorCode(), BOF_ERR_NO_ERROR);

  BofSocketServerParam_X.Name_S = BOF::Bof_Sprintf("NoNbMaxSession");
  puBofSocketServer = std::make_unique<MyTcpServer>(BofSocketServerParam_X);
  ASSERT_TRUE(puBofSocketServer != nullptr);
  ASSERT_NE(puBofSocketServer->LastErrorCode(), BOF_ERR_NO_ERROR);
  */
  BofSocketServerParam_X.NbMaxSession_U32 = NB_MAX_SESSION;
  for (i_U32 = 0; i_U32 < 10; i_U32++) // Each iteration will take LISTEN_POLL_TIMEOUT_IN_MS
  {
    std::unique_ptr<MyTcpServer> puBofSocketServer;

    BofSocketServerParam_X.Name_S = BOF::Bof_Sprintf("Srv_%03d", i_U32);
    //		puBofSocketServer.reset(nullptr);
    puBofSocketServer = std::make_unique<MyTcpServer>(BofSocketServerParam_X);
    ASSERT_TRUE(puBofSocketServer != nullptr);
    EXPECT_EQ(puBofSocketServer->LastErrorCode(), BOF_ERR_NO_ERROR);

    PollSocketCmd_X.SocketOp_E = BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_QUIT;
    EXPECT_EQ(puBofSocketServer->SendPollSocketCommand(SEND_CMD_ANSWER_TIMEOUT_IN_MS, PollSocketCmd_X), BOF_ERR_NO_ERROR);
  }
}

TEST(SockIo_Client_Server_Test, DISABLED_SendPollSocketCommand)
{
  BOF_POLL_SOCKET_CMD PollSocketCommand_X;

  std::unique_ptr<MyTcpClientServer> puBofSocketClientServer;
  BOF_SOCKET_SERVER_PARAM BofSocketClientServerParam_X;
  BOF_SOCKET_PARAM BofSocketParam_X;
  //	std::shared_ptr<MyTcpClient> psBofSocketClient;
  //	BOF_SOCKET_IO_PARAM BofSocketIoParam_X;
  uint32_t i_U32; // , Start_U32, Delta_U32;
  std::string Reply_S, DebugInfo_S;
  int NbSock_i;

  NbSock_i = BOF::BofSocket::S_BofSocketBalance();
  BofSocketClientServerParam_X.ServerMode_E = BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION;
  BofSocketClientServerParam_X.Name_S = "MyTcpClientServer";
  BofSocketClientServerParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  BofSocketClientServerParam_X.ThreadPriority_E = BOF_THREAD_PRIORITY_000;
  ;
  BofSocketClientServerParam_X.ThreadCpuCoreAffinityMask_U64 = 0;
  ;
  BofSocketClientServerParam_X.Address_S = Bof_Sprintf("tcp://0.0.0.0:%d", 0); //: 0->Do not create listen socket
  BofSocketClientServerParam_X.NbMaxSession_U32 = NB_MAX_CLIENT;
  BofSocketClientServerParam_X.MinPortValue_U16 = MIN_PORT_VALUE;
  BofSocketClientServerParam_X.MaxPortValue_U16 = MAX_PORT_VALUE;
  BofSocketClientServerParam_X.PollControlListenerPort_U16 = S_ControlListerPort_U16++;
  if (S_ControlListerPort_U16 >= CONTROL_LISTER_MAX_PORT_VALUE)
  {
    S_ControlListerPort_U16 = CONTROL_LISTER_MIN_PORT_VALUE;
  }
  BofSocketClientServerParam_X.PollControlListenerTimeoutInMs_U32 = NO_IO_CLOSE_TIMEOUT_IN_MS;

  puBofSocketClientServer = std::make_unique<MyTcpClientServer>(BofSocketClientServerParam_X);
  ASSERT_TRUE(puBofSocketClientServer != nullptr);
  EXPECT_EQ(puBofSocketClientServer->LastErrorCode(), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), NbSock_i + 3);

  for (i_U32 = 0; i_U32 < NB_MAX_CLIENT; i_U32++)
  {
    PollSocketCommand_X.SocketOp_E = BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_TEST;
    PollSocketCommand_X.SessionId = i_U32;
    EXPECT_EQ(puBofSocketClientServer->SendPollSocketCommand(SEND_CMD_ANSWER_TIMEOUT_IN_MS, PollSocketCommand_X), BOF_ERR_NO_ERROR);
  }
  // BOF::Bof_MsSleep(5555555);
  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(0, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, NB_MAX_CLIENT + 1));
  EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), 0);
  EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), NB_MAX_CLIENT + 1);

  puBofSocketClientServer.reset(nullptr);

  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), NbSock_i);
}

TEST(SockIo_Client_Server_Test, DISABLED_OpenCloseCmdSession)
{
  std::unique_ptr<MyTcpServer> puBofSocketServer;
  BOF_SOCKET_SERVER_PARAM BofSocketServerParam_X;
  std::unique_ptr<MyTcpClientServer> puBofSocketClientServer;
  BOF_SOCKET_SERVER_PARAM BofSocketClientServerParam_X;
  BOF_SOCKET_PARAM BofSocketParam_X;
  std::shared_ptr<MyTcpClient> psBofSocketClient;
  BOF_SOCKET_IO_PARAM BofSocketIoParam_X;
  std::shared_ptr<BofSocketIo> psCmdBofSocketIo[NB_MAX_CLIENT];
  std::shared_ptr<BofSocketIo> psDataBofSocketIo[NB_MAX_CLIENT];
  uint32_t ReplyCode_U32, i_U32; // , Start_U32, Delta_U32;
  std::string Reply_S, DebugInfo_S;

  BofSocketServerParam_X.ServerMode_E = BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION;
  BofSocketServerParam_X.Name_S = "MyTcpServer";
  BofSocketServerParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  BofSocketServerParam_X.ThreadPriority_E = BOF_THREAD_PRIORITY_000;

  BofSocketServerParam_X.ThreadCpuCoreAffinityMask_U64 = 0;

  BofSocketServerParam_X.Address_S = Bof_Sprintf("tcp://0.0.0.0:%d", DEFAULT_PORT);
  BofSocketServerParam_X.NbMaxSession_U32 = NB_MAX_SESSION;
  BofSocketServerParam_X.MinPortValue_U16 = MIN_PORT_VALUE;
  BofSocketServerParam_X.MaxPortValue_U16 = MAX_PORT_VALUE;
  BofSocketServerParam_X.PollControlListenerPort_U16 = S_ControlListerPort_U16++;
  if (S_ControlListerPort_U16 >= CONTROL_LISTER_MAX_PORT_VALUE)
  {
    S_ControlListerPort_U16 = CONTROL_LISTER_MIN_PORT_VALUE;
  }
  BofSocketServerParam_X.PollControlListenerTimeoutInMs_U32 = NO_IO_CLOSE_TIMEOUT_IN_MS;

  puBofSocketServer = std::make_unique<MyTcpServer>(BofSocketServerParam_X);
  ASSERT_TRUE(puBofSocketServer != nullptr);
  EXPECT_EQ(puBofSocketServer->LastErrorCode(), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 4);

  BofSocketClientServerParam_X.ServerMode_E = BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION;
  BofSocketClientServerParam_X.Name_S = "MyTcpClientServer";
  BofSocketClientServerParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  BofSocketClientServerParam_X.ThreadPriority_E = BOF_THREAD_PRIORITY_000;
  ;
  BofSocketClientServerParam_X.ThreadCpuCoreAffinityMask_U64 = 0;
  ;
  BofSocketClientServerParam_X.Address_S = Bof_Sprintf("tcp://0.0.0.0:%d", 0); //: 0->Do not create listen socket
  BofSocketClientServerParam_X.NbMaxSession_U32 = NB_MAX_CLIENT;
  BofSocketClientServerParam_X.MinPortValue_U16 = MIN_PORT_VALUE;
  BofSocketClientServerParam_X.MaxPortValue_U16 = MAX_PORT_VALUE;
  BofSocketClientServerParam_X.PollControlListenerPort_U16 = S_ControlListerPort_U16++;
  if (S_ControlListerPort_U16 >= CONTROL_LISTER_MAX_PORT_VALUE)
  {
    S_ControlListerPort_U16 = CONTROL_LISTER_MIN_PORT_VALUE;
  }
  BofSocketClientServerParam_X.PollControlListenerTimeoutInMs_U32 = NO_IO_CLOSE_TIMEOUT_IN_MS;

  puBofSocketClientServer = std::make_unique<MyTcpClientServer>(BofSocketClientServerParam_X);
  ASSERT_TRUE(puBofSocketClientServer != nullptr);
  EXPECT_EQ(puBofSocketClientServer->LastErrorCode(), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 7);

  for (i_U32 = 0; i_U32 < NB_MAX_CLIENT; i_U32++)
  {
    EXPECT_EQ(puBofSocketClientServer->Connect(NO_IO_CLOSE_TIMEOUT_IN_MS, Bof_Sprintf("tcp://127.0.0.1:%d", 0), Bof_Sprintf("tcp://127.0.0.1:%d", DEFAULT_PORT), psCmdBofSocketIo[i_U32]), BOF_ERR_NO_ERROR);
    ASSERT_TRUE(psCmdBofSocketIo[i_U32] != nullptr);
    EXPECT_EQ(psCmdBofSocketIo[i_U32]->WaitForCommandReply(NO_IO_CLOSE_TIMEOUT_IN_MS, 220, ReplyCode_U32, Reply_S), BOF_ERR_NO_ERROR);

    //	EXPECT_EQ(puBofSocketClientServer->OpenDataChannel(true, psCmdBofSocketIo[i_U32], NO_IO_CLOSE_TIMEOUT_IN_MS / 2, NO_IO_CLOSE_TIMEOUT_IN_MS, true, "", psDataBofSocketIo[i_U32]), BOF_ERR_NO_ERROR);

    //		EXPECT_EQ(psDataBofSocketIo[i_U32]->Write(NO_IO_CLOSE_TIMEOUT_IN_MS, false, BOF::Bof_Sprintf("Data Message From Client %08X", i_U32), nullptr), BOF_ERR_NO_ERROR);
  }

  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketServer->WaitForNbConnectedSession(NB_MAX_CLIENT, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1 + 1 + (NB_MAX_CLIENT * 1)));
  EXPECT_EQ(puBofSocketServer->NbConnectedSession(), NB_MAX_CLIENT);
  //	ASSERT_GE(puBofSocketServer->NbPollChannel(), 1 + 1 + (NB_MAX_CLIENT * 1) - 1);	//Minus 1 as a temporary poll socket is created as DATA_LISTERNER and is replaced with the data poll sock when the whole process is okk so WaitForNbConnectedSession can
  // see 4 and here we have 3 	ASSERT_LE(puBofSocketServer->NbPollChannel(), 1 + 1 + (NB_MAX_CLIENT * 1));
  EXPECT_EQ(puBofSocketServer->NbPollChannel(), 1 + 1 + (NB_MAX_CLIENT * 1));

  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(NB_MAX_CLIENT, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1 + (NB_MAX_CLIENT * 1)));
  EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), NB_MAX_CLIENT);
  EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), 1 + (NB_MAX_CLIENT * 1));

  //	ASSERT_GE((uint32_t)BOF::BofSocket::S_BofSocketBalance(), (1 + 1 + (NB_MAX_CLIENT * 1)) + (1 + (NB_MAX_CLIENT * 1)));
  //	ASSERT_LE((uint32_t)BOF::BofSocket::S_BofSocketBalance(), (1 + 1 + (NB_MAX_CLIENT * 1)) * 2);

  ASSERT_LE((uint32_t)BOF::BofSocket::S_BofSocketBalance(), (7 + (NB_MAX_CLIENT * 1)) * 2);

  for (i_U32 = 0; i_U32 < NB_MAX_CLIENT; i_U32++)
  {
    // BOF_DBG_PRINTF("===Disconnect(%d)===%p Id %08X\n", i_U32, (void *)psCmdBofSocketIo[i_U32].get(), (int)psCmdBofSocketIo[i_U32]->SessionId());
    EXPECT_EQ(puBofSocketClientServer->Disconnect(true, psCmdBofSocketIo[i_U32]), BOF_ERR_NO_ERROR);
  }

  // Bof_MsSleep(2000000);
  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketServer->WaitForNbConnectedSession(0, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1 + 1));
  EXPECT_EQ(puBofSocketServer->NbConnectedSession(), 0);
  EXPECT_EQ(puBofSocketServer->NbPollChannel(), 1 + 1);

  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(0, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1));
  EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), 0);
  EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), 1);

  puBofSocketServer.reset(nullptr);
  puBofSocketClientServer.reset(nullptr);
  psBofSocketClient.reset();
  for (i_U32 = 0; i_U32 < NB_MAX_CLIENT; i_U32++)
  {
    psCmdBofSocketIo[i_U32].reset();
    psDataBofSocketIo[i_U32].reset();
  }

  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
}

TEST(SockIo_Client_Server_Test, DISABLED_OpenCloseCmdDataSession)
{
  std::unique_ptr<MyTcpServer> puBofSocketServer;
  BOF_SOCKET_SERVER_PARAM BofSocketServerParam_X;
  std::unique_ptr<MyTcpClientServer> puBofSocketClientServer;
  BOF_SOCKET_SERVER_PARAM BofSocketClientServerParam_X;
  BOF_SOCKET_PARAM BofSocketParam_X;
  std::shared_ptr<MyTcpClient> psBofSocketClient;
  BOF_SOCKET_IO_PARAM BofSocketIoParam_X;
  std::shared_ptr<BofSocketIo> psCmdBofSocketIo[NB_MAX_CLIENT];
  std::shared_ptr<BofSocketIo> psDataBofSocketIo[NB_MAX_CLIENT];
  uint32_t ReplyCode_U32, i_U32, j_U32; // , Start_U32, Delta_U32;
  std::string Reply_S, DebugInfo_S;
  std::shared_ptr<MyTcpClient> psTcpClient;
  std::shared_ptr<MyTcpSession> psTcpSession;
  uint32_t NotConnected_U32, NbMaxClient_U32;

  BofSocketServerParam_X.ServerMode_E = BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION;
  BofSocketServerParam_X.Name_S = "MyTcpServer";
  BofSocketServerParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  BofSocketServerParam_X.ThreadPriority_E = BOF_THREAD_PRIORITY_000;
  ;
  BofSocketServerParam_X.ThreadCpuCoreAffinityMask_U64 = 0;
  ;
  BofSocketServerParam_X.Address_S = Bof_Sprintf("tcp://0.0.0.0:%d", DEFAULT_PORT);
  BofSocketServerParam_X.NbMaxSession_U32 = NB_MAX_SESSION;
  BofSocketServerParam_X.MinPortValue_U16 = MIN_PORT_VALUE;
  BofSocketServerParam_X.MaxPortValue_U16 = MAX_PORT_VALUE;
  BofSocketServerParam_X.PollControlListenerPort_U16 = S_ControlListerPort_U16++;
  if (S_ControlListerPort_U16 >= CONTROL_LISTER_MAX_PORT_VALUE)
  {
    S_ControlListerPort_U16 = CONTROL_LISTER_MIN_PORT_VALUE;
  }
  BofSocketServerParam_X.PollControlListenerTimeoutInMs_U32 = NO_IO_CLOSE_TIMEOUT_IN_MS;

  puBofSocketServer = std::make_unique<MyTcpServer>(BofSocketServerParam_X);
  ASSERT_TRUE(puBofSocketServer != nullptr);
  EXPECT_EQ(puBofSocketServer->LastErrorCode(), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 4);

  BofSocketClientServerParam_X.ServerMode_E = BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION;
  BofSocketClientServerParam_X.Name_S = "MyTcpClientServer";
  BofSocketClientServerParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  BofSocketClientServerParam_X.ThreadPriority_E = BOF_THREAD_PRIORITY_000;
  ;
  BofSocketClientServerParam_X.ThreadCpuCoreAffinityMask_U64 = 0;
  ;
  BofSocketClientServerParam_X.Address_S = Bof_Sprintf("tcp://0.0.0.0:%d", 0); //: 0->Do not create listen socket
  BofSocketClientServerParam_X.NbMaxSession_U32 = NB_MAX_CLIENT;
  BofSocketClientServerParam_X.MinPortValue_U16 = MIN_PORT_VALUE;
  BofSocketClientServerParam_X.MaxPortValue_U16 = MAX_PORT_VALUE;
  BofSocketClientServerParam_X.PollControlListenerPort_U16 = S_ControlListerPort_U16++;
  if (S_ControlListerPort_U16 >= CONTROL_LISTER_MAX_PORT_VALUE)
  {
    S_ControlListerPort_U16 = CONTROL_LISTER_MIN_PORT_VALUE;
  }
  BofSocketClientServerParam_X.PollControlListenerTimeoutInMs_U32 = NO_IO_CLOSE_TIMEOUT_IN_MS;

  puBofSocketClientServer = std::make_unique<MyTcpClientServer>(BofSocketClientServerParam_X);
  ASSERT_TRUE(puBofSocketClientServer != nullptr);
  EXPECT_EQ(puBofSocketClientServer->LastErrorCode(), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 7);

  NotConnected_U32 = 0;
  for (i_U32 = 0; i_U32 < NB_MAX_CLIENT; i_U32++)
  {
    EXPECT_EQ(puBofSocketClientServer->Connect(NO_IO_CLOSE_TIMEOUT_IN_MS, Bof_Sprintf("tcp://127.0.0.1:%d", 0), Bof_Sprintf("tcp://127.0.0.1:%d", DEFAULT_PORT), psCmdBofSocketIo[i_U32]), BOF_ERR_NO_ERROR);
    ASSERT_TRUE(psCmdBofSocketIo[i_U32] != nullptr);
    //_DISABLED as we got a t/o here...
    EXPECT_EQ(psCmdBofSocketIo[i_U32]->WaitForCommandReply(NO_IO_CLOSE_TIMEOUT_IN_MS, 220, ReplyCode_U32, Reply_S), BOF_ERR_NO_ERROR);

    EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketServer->WaitForNbConnectedSession(1 + i_U32, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1 + ((i_U32 + 1) * 2)));
    // EXPECT_EQ(puBofSocketServer->NbConnectedChannel(psCmdBofSocketIo[i_U32]), 1);
    EXPECT_EQ(puBofSocketServer->NbConnectedSession(), 1 + i_U32);
    EXPECT_EQ(puBofSocketServer->NbPollChannel(), 1 + ((i_U32 + 1) * 2));

    EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(1 + i_U32, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, ((i_U32 + 1) * 2)));
    EXPECT_EQ(puBofSocketClientServer->NbConnectedChannel(psCmdBofSocketIo[i_U32]), 1);
    EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), 1 + i_U32);
    EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), ((i_U32 + 1) * 2));

    EXPECT_EQ(psCmdBofSocketIo[i_U32]->Login(NO_IO_CLOSE_TIMEOUT_IN_MS, "bha", "bha!"), BOF_ERR_NO_ERROR);

    //		EXPECT_EQ(puBofSocketClientServer->ConnectToDataChannel(true, psCmdBofSocketIo[i_U32], NO_IO_CLOSE_TIMEOUT_IN_MS / 2, NO_IO_CLOSE_TIMEOUT_IN_MS, psDataBofSocketIo[i_U32]), BOF_ERR_NO_ERROR);
    if (puBofSocketClientServer->ConnectToDataChannel(true, psCmdBofSocketIo[i_U32], NO_IO_CLOSE_TIMEOUT_IN_MS / 2, NO_IO_CLOSE_TIMEOUT_IN_MS, psDataBofSocketIo[i_U32]) == BOF_ERR_NO_ERROR)
    {
      psTcpClient = std::dynamic_pointer_cast<MyTcpClient>(psDataBofSocketIo[i_U32]);
      ASSERT_NE(psTcpClient, nullptr);
      EXPECT_EQ(psTcpClient->SessionIndex(), i_U32);
      EXPECT_EQ(psTcpClient->SessionType(), BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL);

      std::shared_ptr<BofSocketIo> psConnectedSession = puBofSocketServer->ConnectedCmdSession(i_U32, 10, NO_IO_CLOSE_TIMEOUT_IN_MS / 2);
      psTcpSession = std::dynamic_pointer_cast<MyTcpSession>(psConnectedSession);
      ASSERT_NE(psTcpSession, nullptr);
      EXPECT_EQ(psTcpSession->SessionIndex(), i_U32);
      EXPECT_EQ(psTcpSession->SessionType(), BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL);

      EXPECT_EQ(psDataBofSocketIo[i_U32]->Write(NO_IO_CLOSE_TIMEOUT_IN_MS, false, BOF::Bof_Sprintf("Data Message From Client %08X", i_U32), nullptr), BOF_ERR_NO_ERROR);

      EXPECT_EQ(puBofSocketClientServer->NbConnectedChannel(psCmdBofSocketIo[i_U32]), 2);
    }
    else
    {
      S_NbNotConnected_U32++;
      NotConnected_U32++;
    }
  }

  NbMaxClient_U32 = NB_MAX_CLIENT - NotConnected_U32;

  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketServer->WaitForNbConnectedSession(NB_MAX_CLIENT, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1 + 1 + (NbMaxClient_U32 * 2)));
  EXPECT_EQ(puBofSocketServer->NbConnectedSession(), NbMaxClient_U32);
  ASSERT_GE(puBofSocketServer->NbPollChannel(),
            1 + 1 + (NbMaxClient_U32 * 2) - 1); // Minus 1 as a temporary poll socket is created as DATA_LISTERNER and is replaced with the data poll sock when the whole process is okk so WaitForNbConnectedSession can see 4 and here we have 3
  ASSERT_LE(puBofSocketServer->NbPollChannel(), 1 + 1 + (NbMaxClient_U32 * 2));

  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(NB_MAX_CLIENT, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1 + (NbMaxClient_U32 * 2)));
  EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), NbMaxClient_U32);
  EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), 1 + (NbMaxClient_U32 * 2));

  for (j_U32 = 0; j_U32 < 100; j_U32++)
  {
    for (i_U32 = 0; i_U32 < NB_MAX_CLIENT; i_U32++)
    {
      if (psDataBofSocketIo[i_U32]) // NbMaxClient_U32
      {
        EXPECT_EQ(psDataBofSocketIo[i_U32]->Write(NO_IO_CLOSE_TIMEOUT_IN_MS, false, BOF::Bof_Sprintf("Data Message %03d From Client %03d", j_U32, i_U32), nullptr), BOF_ERR_NO_ERROR);
      }
    }
  }

  /*
  BOF_DBG_PRINTF("===InfoBeforeClose===============================================================================\n");
  DebugInfo_S=puBofSocketClientServer->SocketServerDebugInfo();
  BOF_DBG_PRINTF("puBofSocketClientServer\n%s", DebugInfo_S.c_str());
  DebugInfo_S=puBofSocketServer->SocketServerDebugInfo();
  BOF_DBG_PRINTF("puBofSocketServer\n%s", DebugInfo_S.c_str());
  */
  // ASSERT_GE((uint32_t)BOF::BofSocket::S_BofSocketBalance(), (((1 + 1 + 1 + (NB_MAX_CLIENT * 1)) * 2) - 1));
  // ASSERT_LE((uint32_t)BOF::BofSocket::S_BofSocketBalance(), (((1 + 1 + 1 + (NB_MAX_CLIENT * 1)) * 2)));
  ASSERT_GE((uint32_t)BOF::BofSocket::S_BofSocketBalance(), (7 + (NbMaxClient_U32 * 4)));
  ASSERT_LE((uint32_t)BOF::BofSocket::S_BofSocketBalance(), (8 + (NbMaxClient_U32 * 4)));

  for (i_U32 = 0; i_U32 < NB_MAX_CLIENT; i_U32++)
  {
    // Done in Disconnect
    //		EXPECT_EQ(puBofSocketClientServer->CloseDataChannel(psDataBofSocketIo[i_U32]), BOF_ERR_NO_ERROR);
    //		BOF_DBG_PRINTF("===Disconnect(%d)===%p Id %08X\n", i_U32, (void *)psCmdBofSocketIo[i_U32].get(), (int)psCmdBofSocketIo[i_U32]->SessionId());
    EXPECT_EQ(puBofSocketClientServer->Disconnect(true, psCmdBofSocketIo[i_U32]), BOF_ERR_NO_ERROR);
  }

  // Bof_MsSleep(2000000);
  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketServer->WaitForNbConnectedSession(0, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1 + 1));
  EXPECT_EQ(puBofSocketServer->NbConnectedSession(), 0);
  EXPECT_EQ(puBofSocketServer->NbPollChannel(), 1 + 1);

  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(0, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1));
  EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), 0);
  EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), 1);

  /*
  BOF_DBG_PRINTF("===InfoAfterClose================================================================================\n");
  DebugInfo_S = puBofSocketClientServer->SocketServerDebugInfo();
  BOF_DBG_PRINTF("puBofSocketClientServer\n%s", DebugInfo_S.c_str());
  DebugInfo_S = puBofSocketServer->SocketServerDebugInfo();
  BOF_DBG_PRINTF("puBofSocketServer\n%s", DebugInfo_S.c_str());
  */

  // BOF_DBG_PRINTF("===Release=======================================================================================\n");

  puBofSocketServer.reset(nullptr);
  puBofSocketClientServer.reset(nullptr);
  psBofSocketClient.reset();
  for (i_U32 = 0; i_U32 < NB_MAX_CLIENT; i_U32++)
  {
    psCmdBofSocketIo[i_U32].reset();
    psDataBofSocketIo[i_U32].reset();
  }

  i_U32 = BOF::BofSocket::S_BofSocketBalance();
  if (i_U32 != 0)
  {
    S_NbBofSocketBalance_U32++;
    BOF::Bof_MsSleep(2000);
    i_U32 = BOF::BofSocket::S_BofSocketBalance();
    BOF::Bof_MsSleep(2000);
    i_U32 = BOF::BofSocket::S_BofSocketBalance();
  }
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
  //	Bof_MsSleep(2000);

  //	BOF_DBG_PRINTF("===LeaveFct======================================================================================\n");
}

TEST(SockIo_Client_Server_Test, DISABLED_ServerOpenCloseCmdDataSession)
{
  std::unique_ptr<MyTcpServer> puBofSocketServer;
  BOF_SOCKET_SERVER_PARAM BofSocketServerParam_X;

  BOF_SOCKET_PARAM BofSocketParam_X;
  std::shared_ptr<MyTcpClient> psBofSocketClient;
  BOF_SOCKET_IO_PARAM BofSocketIoParam_X;
  std::string Reply_S, DebugInfo_S;

  BofSocketServerParam_X.ServerMode_E = BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION;
  BofSocketServerParam_X.Name_S = "MyTcpServer";
  BofSocketServerParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  BofSocketServerParam_X.ThreadPriority_E = BOF_THREAD_PRIORITY_000;

  BofSocketServerParam_X.ThreadCpuCoreAffinityMask_U64 = 0;

  BofSocketServerParam_X.Address_S = Bof_Sprintf("tcp://0.0.0.0:%d", DEFAULT_PORT);
  BofSocketServerParam_X.NbMaxSession_U32 = NB_MAX_SESSION;
  BofSocketServerParam_X.MinPortValue_U16 = MIN_PORT_VALUE;
  BofSocketServerParam_X.MaxPortValue_U16 = MAX_PORT_VALUE;
  BofSocketServerParam_X.PollControlListenerPort_U16 = DEFAULT_PORT + 1;
  BofSocketServerParam_X.PollControlListenerTimeoutInMs_U32 = NO_IO_CLOSE_TIMEOUT_IN_MS;

  puBofSocketServer = std::make_unique<MyTcpServer>(BofSocketServerParam_X);
  ASSERT_TRUE(puBofSocketServer != nullptr);
  EXPECT_EQ(puBofSocketServer->LastErrorCode(), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 4);

  for (int i = 0; i < 3; i++)
  {
    BOF_DBG_PRINTF("SERVER LOOP Con %d Poll %d\n", puBofSocketServer->NbConnectedSession(), puBofSocketServer->NbPollChannel());

    Bof_MsSleep(100);
  } // while (1);	// puBofSocketServer->NbConnectedSession() == 0);
  /*
    EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketServer->WaitForNbConnectedSession(0, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1 + 1));
    EXPECT_EQ(puBofSocketServer->NbConnectedSession(), 0);
    EXPECT_EQ(puBofSocketServer->NbPollChannel(), 1 + 1);

    puBofSocketServer.reset(nullptr);
    psBofSocketClient.reset();
    EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
    */
}

#if 0

TEST(SockIo_Client_Server_Test, ClientOpenCloseCmdDataSession)
{
  std::unique_ptr<MyTcpClientServer> puBofSocketClientServer;
  BOF_SOCKET_SERVER_PARAM BofSocketClientServerParam_X;
  BOF_SOCKET_PARAM BofSocketParam_X;
  std::shared_ptr<MyTcpClient> psBofSocketClient;
  BOF_SOCKET_IO_PARAM BofSocketIoParam_X;
  std::shared_ptr<BofSocketIo> psCmdBofSocketIo[NB_MAX_CLIENT];
  std::shared_ptr<BofSocketIo> psDataBofSocketIo[NB_MAX_CLIENT];
  uint32_t ReplyCode_U32, i_U32, j_U32;	// , Start_U32, Delta_U32;
  std::string Reply_S, DebugInfo_S;
  uint32_t NotConnected_U32, NbMaxClient_U32;

  BofSocketClientServerParam_X.ServerMode_E = BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION;
  BofSocketClientServerParam_X.Name_S = "MyTcpClientServer";
  BofSocketClientServerParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  BofSocketClientServerParam_X.ThreadPriority_E = BOF_THREAD_PRIORITY_000;;
  BofSocketClientServerParam_X.ThreadCpuCoreAffinityMask_U64 = 0;;
  BofSocketClientServerParam_X.Address_S = Bof_Sprintf("tcp://0.0.0.0:%d", 0);		//:0->Do not create listen socket
  BofSocketClientServerParam_X.NbMaxSession_U32 = NB_MAX_CLIENT;
  BofSocketClientServerParam_X.MinPortValue_U16 = MIN_PORT_VALUE;
  BofSocketClientServerParam_X.MaxPortValue_U16 = MAX_PORT_VALUE;
  BofSocketClientServerParam_X.PollControlListenerPort_U16 = S_ControlListerPort_U16++;
  if (S_ControlListerPort_U16 >= CONTROL_LISTER_MAX_PORT_VALUE)
  {
    S_ControlListerPort_U16 = CONTROL_LISTER_MIN_PORT_VALUE;
  }
  BofSocketClientServerParam_X.PollControlListenerTimeoutInMs_U32 = NO_IO_CLOSE_TIMEOUT_IN_MS;

  puBofSocketClientServer = std::make_unique<MyTcpClientServer>(BofSocketClientServerParam_X);
  ASSERT_TRUE(puBofSocketClientServer != nullptr);
  EXPECT_EQ(puBofSocketClientServer->LastErrorCode(), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 3);

  NotConnected_U32 = 0;
  for (i_U32 = 0; i_U32 < NB_MAX_CLIENT; i_U32++)
  {
    EXPECT_EQ(puBofSocketClientServer->Connect(NO_IO_CLOSE_TIMEOUT_IN_MS, Bof_Sprintf("tcp://127.0.0.1:%d", 0), Bof_Sprintf("tcp://127.0.0.1:%d", BofSocketClientServerParam_X.PollControlListenerPort_U16), psCmdBofSocketIo[i_U32]), BOF_ERR_NO_ERROR);
    ASSERT_TRUE(psCmdBofSocketIo[i_U32] != nullptr);
    EXPECT_EQ(psCmdBofSocketIo[i_U32]->WaitForCommandReply(NO_IO_CLOSE_TIMEOUT_IN_MS, 220, ReplyCode_U32, Reply_S), BOF_ERR_NO_ERROR);

    EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(1 + i_U32, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, (1 + i_U32) * 2));
    EXPECT_EQ(puBofSocketClientServer->NbConnectedChannel(psCmdBofSocketIo[i_U32]), 1);
    EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), 1 + i_U32);
    EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), (1 + i_U32) * 2);

    EXPECT_EQ(psCmdBofSocketIo[i_U32]->Login(NO_IO_CLOSE_TIMEOUT_IN_MS, "bha", "bha!"), BOF_ERR_NO_ERROR);

    //		EXPECT_EQ(puBofSocketClientServer->ConnectToDataChannel(true, psCmdBofSocketIo[i_U32], NO_IO_CLOSE_TIMEOUT_IN_MS / 2, NO_IO_CLOSE_TIMEOUT_IN_MS, psDataBofSocketIo[i_U32]), BOF_ERR_NO_ERROR);
    if (puBofSocketClientServer->ConnectToDataChannel(true, psCmdBofSocketIo[i_U32], NO_IO_CLOSE_TIMEOUT_IN_MS / 2, NO_IO_CLOSE_TIMEOUT_IN_MS, psDataBofSocketIo[i_U32]) == BOF_ERR_NO_ERROR)
    {
      EXPECT_EQ(psDataBofSocketIo[i_U32]->Write(NO_IO_CLOSE_TIMEOUT_IN_MS, false, BOF::Bof_Sprintf("Data Message From Client %08X", i_U32), nullptr), BOF_ERR_NO_ERROR);

      EXPECT_EQ(puBofSocketClientServer->NbConnectedChannel(psCmdBofSocketIo[i_U32]), 2);
    }
    else
    {
      S_NbNotConnected_U32++;
      NotConnected_U32++;
    }
  }

  NbMaxClient_U32 = NB_MAX_CLIENT - NotConnected_U32;

  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(NB_MAX_CLIENT, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1 + (NbMaxClient_U32 * 2)));
  EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), NbMaxClient_U32);
  EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), 1 + (NbMaxClient_U32 * 2));

  for (j_U32 = 0; j_U32 < 100; j_U32++)
  {
    for (i_U32 = 0; i_U32 < NB_MAX_CLIENT; i_U32++)
    {
      if (psDataBofSocketIo[i_U32])	//NbMaxClient_U32
      {
        EXPECT_EQ(psDataBofSocketIo[i_U32]->Write(NO_IO_CLOSE_TIMEOUT_IN_MS, false, BOF::Bof_Sprintf("Data Message %03d From Client %03d", j_U32, i_U32), nullptr), BOF_ERR_NO_ERROR);
      }
    }
  }


  ASSERT_GE((uint32_t)BOF::BofSocket::S_BofSocketBalance(), (3 + (NbMaxClient_U32 * 2)));
  ASSERT_LE((uint32_t)BOF::BofSocket::S_BofSocketBalance(), (4 + (NbMaxClient_U32 * 2)));


  for (i_U32 = 0; i_U32 < NB_MAX_CLIENT; i_U32++)
  {
    //BOF_DBG_PRINTF("===Disconnect(%d)===%p Id %08X\n", i_U32, (void *)psCmdBofSocketIo[i_U32].get(), (int)psCmdBofSocketIo[i_U32]->SessionId());
    EXPECT_EQ(puBofSocketClientServer->Disconnect(true, psCmdBofSocketIo[i_U32]), BOF_ERR_NO_ERROR);
  }

  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(0, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1));
  EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), 0);
  EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), 1);

  puBofSocketClientServer.reset(nullptr);
  psBofSocketClient.reset();
  for (i_U32 = 0; i_U32 < NB_MAX_CLIENT; i_U32++)
  {
    psCmdBofSocketIo[i_U32].reset();
    psDataBofSocketIo[i_U32].reset();
  }

  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
  //BOF::Bof_MsSleep(2000); //Leave time for server to detect end of session
}

TEST(SockIo_Client_Server_Test, ClientForH4X)
{
  std::unique_ptr<MyTcpClientServer> puBofSocketClientServer;
  BOF_SOCKET_SERVER_PARAM BofSocketClientServerParam_X;
  BOF_SOCKET_PARAM BofSocketParam_X;
  std::shared_ptr<MyTcpClient> psBofSocketClient;
  BOF_SOCKET_IO_PARAM BofSocketIoParam_X;
  std::shared_ptr<BofSocketIo> psCmdBofSocketIo[NB_MAX_H4X_CLIENT];
  std::shared_ptr<BofSocketIo> psDataBofSocketIo[NB_MAX_H4X_CLIENT];
  uint32_t ReplyCode_U32, i_U32, j_U32;
  std::string Reply_S, DebugInfo_S;
  std::shared_ptr<MyTcpClient> psTcpClient;
  BOF::BOF_BUFFER DataBuffer_X;
  std::vector<BOF_FTP_FILE> FtpFileCollection;
  std::vector<XT_FILE> XtFileCollection;

  BofSocketClientServerParam_X.ServerMode_E = BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION;
  BofSocketClientServerParam_X.Name_S = "MyTcpClientServer";
  BofSocketClientServerParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  BofSocketClientServerParam_X.ThreadPriority_E = BOF_THREAD_PRIORITY_000;;
  BofSocketClientServerParam_X.ThreadCpuCoreAffinityMask_U64 = 0;;
  BofSocketClientServerParam_X.Address_S = Bof_Sprintf("tcp://0.0.0.0:%d", 0);		//:0->Do not create listen socket
  BofSocketClientServerParam_X.NbMaxSession_U32 = NB_MAX_H4X_CLIENT;
  BofSocketClientServerParam_X.MinPortValue_U16 = MIN_PORT_VALUE;
  BofSocketClientServerParam_X.MaxPortValue_U16 = MAX_PORT_VALUE;
  BofSocketClientServerParam_X.PollControlListenerPort_U16 = S_ControlListerPort_U16++;
  if (S_ControlListerPort_U16 >= CONTROL_LISTER_MAX_PORT_VALUE)
  {
    S_ControlListerPort_U16 = CONTROL_LISTER_MIN_PORT_VALUE;
  }
  BofSocketClientServerParam_X.PollControlListenerTimeoutInMs_U32 = NO_IO_CLOSE_TIMEOUT_IN_MS;

  puBofSocketClientServer = std::make_unique<MyTcpClientServer>(BofSocketClientServerParam_X);
  ASSERT_TRUE(puBofSocketClientServer != nullptr);
  EXPECT_EQ(puBofSocketClientServer->LastErrorCode(), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 3);

  for (i_U32 = 0; i_U32 < NB_MAX_H4X_CLIENT; i_U32++)
  {
    EXPECT_EQ(puBofSocketClientServer->Connect(NO_IO_CLOSE_TIMEOUT_IN_MS, Bof_Sprintf("tcp://10.129.170.30:%d", 0), Bof_Sprintf("tcp://10.129.170.39:%d", 21), psCmdBofSocketIo[i_U32]), BOF_ERR_NO_ERROR);
    psTcpClient = std::dynamic_pointer_cast<MyTcpClient>(psCmdBofSocketIo[i_U32]);
    ASSERT_TRUE(psTcpClient != nullptr);
    ASSERT_TRUE(psCmdBofSocketIo[i_U32] != nullptr);
    EXPECT_EQ(psCmdBofSocketIo[i_U32]->WaitForCommandReply(NO_IO_CLOSE_TIMEOUT_IN_MS, 220, ReplyCode_U32, Reply_S), BOF_ERR_NO_ERROR);

    EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(1 + i_U32, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1 + 1 + i_U32));
    EXPECT_EQ(puBofSocketClientServer->NbConnectedChannel(psCmdBofSocketIo[i_U32]), 1);
    EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), 1 + i_U32);
    EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), 1 + 1 + i_U32);

    EXPECT_EQ(psCmdBofSocketIo[i_U32]->Login(NO_IO_CLOSE_TIMEOUT_IN_MS, "bha", "bha!"), BOF_ERR_NO_ERROR);

    EXPECT_EQ(psTcpClient->DataBuffer().Size_U64, 0);
    EXPECT_EQ(psCmdBofSocketIo[i_U32]->IoDataCommand(NO_IO_CLOSE_TIMEOUT_IN_MS / 2, NO_IO_CLOSE_TIMEOUT_IN_MS, "LIST\r\n", psDataBofSocketIo[i_U32], ReplyCode_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ReplyCode_U32, 150);

    EXPECT_EQ(psCmdBofSocketIo[i_U32]->WaitForCommandReply(NO_IO_CLOSE_TIMEOUT_IN_MS, 226, ReplyCode_U32, Reply_S), BOF_ERR_NO_ERROR);
    EXPECT_EQ(psDataBofSocketIo[i_U32]->WaitForChannelDisconnected(NO_IO_CLOSE_TIMEOUT_IN_MS), BOF_ERR_NO_ERROR);	//Can happen after 226 reply

    BOF_DBG_PRINTF("DataBuffer[%04d]=%d sz %" PRId64 "\n", i_U32, psTcpClient->SessionIndex(), psTcpClient->DataBuffer().Size_U64);

    ASSERT_NE(psTcpClient->DataBuffer().Size_U64, 0);
    ASSERT_TRUE(psTcpClient->DataBuffer().Size_U64 <= psTcpClient->DataBuffer().Capacity_U64);
    psTcpClient->DataBuffer().pData_U8[psTcpClient->DataBuffer().Size_U64] = 0;
    FtpFileCollection.clear();
    XtFileCollection.clear();
    EXPECT_EQ(BofSocketIo::S_ParseListLineBuffer("/", reinterpret_cast<const char *>(psTcpClient->DataBuffer().pData_U8), FtpFileCollection), BOF_ERR_NO_ERROR);
    EXPECT_EQ(MyTcpClient::S_ParseXtFilename(FtpFileCollection, XtFileCollection), BOF_ERR_NO_ERROR);

    ASSERT_NE(FtpFileCollection.size(), (size_t)0);
    EXPECT_EQ(FtpFileCollection.size(), XtFileCollection.size());
    for (j_U32 = 0; j_U32 < FtpFileCollection.size(); j_U32++)
    {
      BOF_DBG_PRINTF("List[%04d]='%s'\n", j_U32, FtpFileCollection[j_U32].Path.FullPathName(false).c_str());
      BOF_DBG_PRINTF("           %04d%c '%s' I %08X O %08X Seq %04X Bs %03d MB SzBlk %08d SzByte %10" PRId64 "\n", XtFileCollection[j_U32].IdLsm_U16, XtFileCollection[j_U32].Camera_U8, XtFileCollection[j_U32].pGuid_c, XtFileCollection[j_U32].In_U32, XtFileCollection[j_U32].Out_U32,
                     XtFileCollection[j_U32].MacroSeq_U16, XtFileCollection[j_U32].BlockSizeInMB_U8, XtFileCollection[j_U32].SizeInBlock_U32, XtFileCollection[j_U32].SizeInByte_U64);

    }


    //		EXPECT_EQ(puBofSocketClientServer->ConnectedDataSession()->, BOF_ERR_NO_ERROR);

  }
  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(NB_MAX_H4X_CLIENT, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1 + (NB_MAX_H4X_CLIENT)));
  EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), NB_MAX_H4X_CLIENT);
  EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), 1 + (NB_MAX_H4X_CLIENT));

  EXPECT_EQ((uint32_t)BOF::BofSocket::S_BofSocketBalance(), (3 + (NB_MAX_H4X_CLIENT)));
  //	ASSERT_LE((uint32_t)BOF::BofSocket::S_BofSocketBalance(), (4 + (NB_MAX_H4X_CLIENT)));

  for (i_U32 = 0; i_U32 < NB_MAX_H4X_CLIENT; i_U32++)
  {
    EXPECT_EQ(puBofSocketClientServer->Disconnect(true, psCmdBofSocketIo[i_U32]), BOF_ERR_NO_ERROR);
  }

  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(0, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1));
  EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), 0);
  EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), 1);

  puBofSocketClientServer.reset(nullptr);
  psBofSocketClient.reset();
  for (i_U32 = 0; i_U32 < NB_MAX_H4X_CLIENT; i_U32++)
  {
    psCmdBofSocketIo[i_U32].reset();
    psDataBofSocketIo[i_U32].reset();
  }

  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
}
#endif

#if 0
#include <ucodeproxyserver.h>
TEST(SockIo_Client_Server_Test, UcodeServer)
{
  std::unique_ptr<UcodeProxyServer>  mpuUcodeProxyServer = nullptr;
  BOF::BOF_SOCKET_SERVER_PARAM BofSocketServerParam_X;
  bool Finish_B = false;
  uint32_t i_U32, Start_U32, Periodic_U32, Delta_U32;

  BofSocketServerParam_X.ServerMode_E = BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION;
  BofSocketServerParam_X.Name_S = BOF::Bof_Sprintf("UcodeProxyServer");
 //for ut under qemu/docker do not use fifo scheduler    
  BofSocketServerParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY_OTHER; 
  BofSocketServerParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY_000;                  // BOF::BOF_THREAD_PRIORITY_036;
  BofSocketServerParam_X.ThreadCpuCoreAffinityMask_U64 = 0;;
  BofSocketServerParam_X.Address_S = BOF::Bof_Sprintf("tcp://%s:%d", "127.0.0.1", 60000);
  BofSocketServerParam_X.Address_S = BOF::Bof_Sprintf("tcp://%s:%d", "10.129.170.200", 60000);	//On ubuntu vm
  BofSocketServerParam_X.NbMaxSession_U32 = NB_MAX_H4X_CLIENT;
  BofSocketServerParam_X.MinPortValue_U16 = 55000;
  BofSocketServerParam_X.MaxPortValue_U16 = 55099;
  BofSocketServerParam_X.PollControlListenerPort_U16 = 55100;
  BofSocketServerParam_X.PollControlListenerTimeoutInMs_U32 = 3000;

  mpuUcodeProxyServer = UcodeProxyServer::S_UcodeProxyServerFactory(BofSocketServerParam_X);
  Start_U32 = Bof_GetMsTickCount();
  Periodic_U32 = Start_U32;
  i_U32 = 0;
  while (!Finish_B)
  {
    mpuUcodeProxyServer->BgTaskUcodeProxyServer();
    Bof_MsSleep(10);
    Delta_U32 = BOF::Bof_ElapsedMsTime(Periodic_U32);
    if (Delta_U32 > 1000)
    {
      Periodic_U32 = Bof_GetMsTickCount();
      BOF_DBG_PRINTF("SERVER LOOP[%d] Con %d Poll %d\n", i_U32++, mpuUcodeProxyServer->NbConnectedSession(), mpuUcodeProxyServer->NbPollChannel());
    }
    Delta_U32 = BOF::Bof_ElapsedMsTime(Start_U32);
    if ((Delta_U32 > 30000))	//|| (mpuUcodeProxyServer->NbConnectedSession() >= 10))
    {
      Finish_B = true;
    }
  }
}

TEST(SockIo_Client_Server_Test, ClientForUcode)
{
  std::unique_ptr<MyTcpClientServer> puBofSocketClientServer;
  BOF_SOCKET_SERVER_PARAM BofSocketClientServerParam_X;
  BOF_SOCKET_PARAM BofSocketParam_X;
  std::shared_ptr<MyTcpClient> psBofSocketClient;
  BOF_SOCKET_IO_PARAM BofSocketIoParam_X;
  std::shared_ptr<BofSocketIo> psCmdBofSocketIo[NB_MAX_H4X_CLIENT];
  std::shared_ptr<BofSocketIo> psDataBofSocketIo[NB_MAX_H4X_CLIENT];
  uint32_t ReplyCode_U32, i_U32, j_U32;
  std::string Reply_S, DebugInfo_S;
  std::shared_ptr<MyTcpClient> psTcpClient;
  BOF::BOF_BUFFER DataBuffer_X;
  std::vector<BOF_FTP_FILE> FtpFileCollection;
  std::vector<XT_FILE> XtFileCollection;

  BofSocketClientServerParam_X.ServerMode_E = BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_SESSION;
  BofSocketClientServerParam_X.Name_S = "ClientForUcode";
  BofSocketClientServerParam_X.ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
  BofSocketClientServerParam_X.ThreadPriority_E = BOF_THREAD_PRIORITY_000;;
  BofSocketClientServerParam_X.ThreadCpuCoreAffinityMask_U64 = 0;;
  BofSocketClientServerParam_X.Address_S = Bof_Sprintf("tcp://0.0.0.0:%d", 0);		//:0->Do not create listen socket
  BofSocketClientServerParam_X.NbMaxSession_U32 = NB_MAX_H4X_CLIENT;
  BofSocketClientServerParam_X.MinPortValue_U16 = MIN_PORT_VALUE;
  BofSocketClientServerParam_X.MaxPortValue_U16 = MAX_PORT_VALUE;
  BofSocketClientServerParam_X.PollControlListenerPort_U16 = S_ControlListerPort_U16++;
  if (S_ControlListerPort_U16 >= CONTROL_LISTER_MAX_PORT_VALUE)
  {
    S_ControlListerPort_U16 = CONTROL_LISTER_MIN_PORT_VALUE;
  }
  BofSocketClientServerParam_X.PollControlListenerTimeoutInMs_U32 = NO_IO_CLOSE_TIMEOUT_IN_MS;

  puBofSocketClientServer = std::make_unique<MyTcpClientServer>(BofSocketClientServerParam_X);
  ASSERT_TRUE(puBofSocketClientServer != nullptr);
  EXPECT_EQ(puBofSocketClientServer->LastErrorCode(), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 3);

  for (i_U32 = 0; i_U32 < NB_MAX_H4X_CLIENT; i_U32++)
  {
    //		EXPECT_EQ(puBofSocketClientServer->Connect(NO_IO_CLOSE_TIMEOUT_IN_MS, Bof_Sprintf("tcp:///127.0.0.1:%d", 0), Bof_Sprintf("tcp://127.0.0.1:%d", 60000), psCmdBofSocketIo[i_U32]), BOF_ERR_NO_ERROR);
    EXPECT_EQ(puBofSocketClientServer->Connect(NO_IO_CLOSE_TIMEOUT_IN_MS, Bof_Sprintf("tcp:///127.0.0.1:%d", 0), Bof_Sprintf("tcp://10.129.170.200:%d", 60000), psCmdBofSocketIo[i_U32]), BOF_ERR_NO_ERROR); //On ubuntu vm
    psTcpClient = std::dynamic_pointer_cast<MyTcpClient>(psCmdBofSocketIo[i_U32]);
    ASSERT_TRUE(psTcpClient != nullptr);
    ASSERT_TRUE(psCmdBofSocketIo[i_U32] != nullptr);
    EXPECT_EQ(psCmdBofSocketIo[i_U32]->WaitForCommandReply(NO_IO_CLOSE_TIMEOUT_IN_MS, 220, ReplyCode_U32, Reply_S), BOF_ERR_NO_ERROR);

    EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(1 + i_U32, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1 + 1 + i_U32));
    EXPECT_EQ(puBofSocketClientServer->NbConnectedChannel(psCmdBofSocketIo[i_U32]), 1);
    EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), 1 + i_U32);
    EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), 1 + 1 + i_U32);

    EXPECT_EQ(psCmdBofSocketIo[i_U32]->Login(NO_IO_CLOSE_TIMEOUT_IN_MS, "bha", "bha!"), BOF_ERR_NO_ERROR);

    EXPECT_EQ(psTcpClient->DataBuffer().Size_U64, 0);
    EXPECT_EQ(psCmdBofSocketIo[i_U32]->IoDataCommand(NO_IO_CLOSE_TIMEOUT_IN_MS / 2, NO_IO_CLOSE_TIMEOUT_IN_MS, "LIST\r\n", psDataBofSocketIo[i_U32], ReplyCode_U32), BOF_ERR_NO_ERROR);
    EXPECT_EQ(ReplyCode_U32, 150);

    EXPECT_EQ(psCmdBofSocketIo[i_U32]->WaitForCommandReply(NO_IO_CLOSE_TIMEOUT_IN_MS, 226, ReplyCode_U32, Reply_S), BOF_ERR_NO_ERROR);
    EXPECT_EQ(psDataBofSocketIo[i_U32]->WaitForChannelDisconnected(NO_IO_CLOSE_TIMEOUT_IN_MS), BOF_ERR_NO_ERROR);	//Can happen after 226 reply

    BOF_DBG_PRINTF("DataBuffer[%04d]=%d sz %" PRId64 "\n", i_U32, psTcpClient->SessionIndex(), psTcpClient->DataBuffer().Size_U64);


    ASSERT_NE(psTcpClient->DataBuffer().Size_U64, 0);
    ASSERT_TRUE(psTcpClient->DataBuffer().Size_U64 <= psTcpClient->DataBuffer().Capacity_U64);
    psTcpClient->DataBuffer().pData_U8[psTcpClient->DataBuffer().Size_U64] = 0;
    FtpFileCollection.clear();
    XtFileCollection.clear();
    EXPECT_EQ(BofSocketIo::S_ParseListLineBuffer("/", reinterpret_cast<const char *>(psTcpClient->DataBuffer().pData_U8), FtpFileCollection), BOF_ERR_NO_ERROR);
    EXPECT_EQ(MyTcpClient::S_ParseXtFilename(FtpFileCollection, XtFileCollection), BOF_ERR_NO_ERROR);

    ASSERT_NE(FtpFileCollection.size(), (size_t)0);
    EXPECT_EQ(FtpFileCollection.size(), XtFileCollection.size());
    for (j_U32 = 0; j_U32 < FtpFileCollection.size(); j_U32++)
    {
      BOF_DBG_PRINTF("List[%04d]='%s'\n", j_U32, FtpFileCollection[j_U32].Path.FullPathName(false).c_str());
      BOF_DBG_PRINTF("           %04d%c '%s' I %08X O %08X Seq %04X Bs %03d MB SzBlk %08d SzByte %10" PRId64 "\n", XtFileCollection[j_U32].IdLsm_U16, XtFileCollection[j_U32].Camera_U8, XtFileCollection[j_U32].pGuid_c, XtFileCollection[j_U32].In_U32, XtFileCollection[j_U32].Out_U32,
                     XtFileCollection[j_U32].MacroSeq_U16, XtFileCollection[j_U32].BlockSizeInMB_U8, XtFileCollection[j_U32].SizeInBlock_U32, XtFileCollection[j_U32].SizeInByte_U64);

    }


    //		EXPECT_EQ(puBofSocketClientServer->ConnectedDataSession()->, BOF_ERR_NO_ERROR);

  }
  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(NB_MAX_H4X_CLIENT, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1 + (NB_MAX_H4X_CLIENT)));
  EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), NB_MAX_H4X_CLIENT);
  EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), 1 + (NB_MAX_H4X_CLIENT));

  EXPECT_EQ((uint32_t)BOF::BofSocket::S_BofSocketBalance(), (3 + (NB_MAX_H4X_CLIENT)));
  //	ASSERT_LE((uint32_t)BOF::BofSocket::S_BofSocketBalance(), (4 + (NB_MAX_H4X_CLIENT)));

  for (i_U32 = 0; i_U32 < NB_MAX_H4X_CLIENT; i_U32++)
  {
    EXPECT_EQ(puBofSocketClientServer->Disconnect(true, psCmdBofSocketIo[i_U32]), BOF_ERR_NO_ERROR);
  }

  EXPECT_EQ(BOF_ERR_NO_ERROR, puBofSocketClientServer->WaitForNbConnectedSession(0, WAIT_FOR_POLL_PERIOD, NO_IO_CLOSE_TIMEOUT_IN_MS / 2, 1));
  EXPECT_EQ(puBofSocketClientServer->NbConnectedSession(), 0);
  EXPECT_EQ(puBofSocketClientServer->NbPollChannel(), 1);

  puBofSocketClientServer.reset(nullptr);
  psBofSocketClient.reset();
  for (i_U32 = 0; i_U32 < NB_MAX_H4X_CLIENT; i_U32++)
  {
    psCmdBofSocketIo[i_U32].reset();
    psDataBofSocketIo[i_U32].reset();
  }

  EXPECT_EQ(BOF::BofSocket::S_BofSocketBalance(), 0);
}

#endif
