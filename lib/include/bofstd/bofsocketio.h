/*
* Copyright (c) 2015-2025, Onbings. All rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
* PURPOSE.
*
* This module defines the BofSocketIo class.
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

#include <bofstd/ibofsocketio.h>
#include <bofstd/bofsocket.h>
#include <bofstd/bofcircularbuffer.h>
#include <bofstd/bofpath.h>
#include <bofstd/boffs.h>

BEGIN_BOF_NAMESPACE()
//using BOFSOCKETID = BOFSOCKET;
class BofSocketSessionManager;
class BofSocketServer;

enum class BOF_SOCKET_IO_DATA_REQ :uint32_t
{
	DATA_IO_BUFFER = 0,	//Get buffer to read to or to write in
	DATA_IO_DONE,
	DATA_IO_STATUS,
};
enum class BOF_SOCKET_SESSION_TYPE :uint32_t
{
	UNKNOWN = 0,
	CMD_POLL_WAIT,              //Always one and only one per BofSocketManager at index 0 of mpPollOp_X !
	SERVER_LISTENER,            //Optional unique listener per BofSocketManager 
	COMMAND_CHANNEL,            //Optional multiple socket for command. They are created when a connection is made to SERVER_LISTENER or a client is created and connected to a server
	DATA_LISTENER,							//listener used for PASV connection and entry is replaced by the opened data channel
	DATA_CHANNEL,               //Optional multiple socket for Data channel i/o
	POLL_CHANNEL								//BofSocket type used by BOF_SOCKET_SERVER_MODE::BOF_SOCKET_SERVER_POLLER
};

enum class BOF_SOCKET_IO_NOTIFY_TYPE :uint32_t
{
	ASAP = 0,
	WHEN_FULL_OR_CLOSED,
	WHEN_FULL_OR_DELIMITER_FOUND,
};
typedef std::function<BOFERR(BOF_SOCKET_IO_DATA_REQ _IoDataReq_E, BOFERR Sts_E, BOF::BOF_BUFFER &_rDataBuffer_X, void *_pUserArg)> ON_SOCKET_DATA_IO_CALLBACK;
const uint32_t BOF_SOCKETIO_MIN_NOTIFY_RCV_BUFFER_SIZE = 0x200;


struct BOF_SOCKET_IO_PARAM
{
  std::string Name_S;
  uint32_t SocketRcvBufferSize_U32;
  uint32_t SocketSndBufferSize_U32;
  uint32_t NotifyRcvBufferSize_U32;		//BOF_SOCKETIO_MIN_NOTIFY_RCV_BUFFER_SIZE
  BOF_SOCKET_IO_NOTIFY_TYPE NotifyType_E;
  uint8_t Delimiter_U8;	//Used by BOF_SOCKET_DATA_READ_TYPE::WHEN_FULL_OR_DELIMITER_FOUND
  void        *pData;   /*! Notify: Specifies a pointer to the internal buffer data zone (pre-allocated buffer). Set to nullptr if the memory must be allocated by the function (size id InternalRxBufferSizeInByte_U32)*/
  uint32_t NbMaxAsyncWritePendingRequest_U32;
	uint32_t										NoIoCloseTimeoutInMs_U32;

  BOF_SOCKET_IO_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    Name_S = "";
    NotifyRcvBufferSize_U32 = BOF_SOCKETIO_MIN_NOTIFY_RCV_BUFFER_SIZE;
    SocketSndBufferSize_U32 = 0;
    SocketRcvBufferSize_U32 = 0;
    NotifyType_E = BOF_SOCKET_IO_NOTIFY_TYPE::ASAP;
    Delimiter_U8 = 0;
    pData = nullptr;
    NbMaxAsyncWritePendingRequest_U32 = 0;
		NoIoCloseTimeoutInMs_U32 = 0;
  }
};

struct BOF_SOCKET_SESSION_STATISTIC
{
	uint32_t NbOpConnect_U32;
	uint32_t NbOpConnectError_U32;
	uint32_t NbOpDisconnect_U32;
	uint32_t NbOpDisconnectError_U32;
	uint32_t NbOpListenError_U32;
	uint32_t NbOpRead_U32;
	uint32_t NbOpReadError_U32;
	uint64_t NbDataByteRead_U64;
	uint32_t NbOpWrite_U32;
	uint32_t NbOpWriteError_U32;
	uint64_t NbDataByteWritten_U64;
	uint32_t NbError_U32;
	uint32_t NbCloseEvent_U32;
	uint32_t NbConnectEvent_U32;

	uint32_t NbTcpShutdownEvent_U32;
	uint32_t NbTcpListenEvent_U32;

	uint32_t NbTimerEvent_U32;
	uint32_t NbTimerEventError_U32;

	BOF_SOCKET_SESSION_STATISTIC()
	{
		Reset();
	}

	void Reset()
	{
		NbOpConnect_U32 = 0;
		NbOpConnectError_U32 = 0;
		NbOpDisconnect_U32 = 0;
		NbOpDisconnectError_U32 = 0;
		NbOpListenError_U32 = 0;

		NbOpRead_U32 = 0;
		NbOpReadError_U32 = 0;
		NbDataByteRead_U64 = 0;
		NbOpWrite_U32 = 0;
		NbOpWriteError_U32 = 0;
		NbDataByteWritten_U64 = 0;
		NbError_U32 = 0;
		NbCloseEvent_U32 = 0;
		NbConnectEvent_U32 = 0;

		NbTcpShutdownEvent_U32 = 0;
		NbTcpListenEvent_U32 = 0;

		NbTimerEvent_U32 = 0;
		NbTimerEventError_U32 = 0;
	}
};
struct BOF_SOCKET_WRITE_PARAM
{
  uint32_t TimeoutInMs_U32;
  uint32_t Nb_U32;
  const uint8_t *pBuffer_U8;
  void *pWriteContext;

  BOF_SOCKET_WRITE_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    TimeoutInMs_U32 = 0;
    Nb_U32 = 0;
    pBuffer_U8 = nullptr;
		pWriteContext = nullptr;
  }
};

struct BOF_FTP_FILE
{
  BOF_FILE_PERMISSION Right_E;  //-rwx------
  std::string User_S;
  std::string Group_S;
  uint64_t Size_U64;
  BOF_DATE_TIME DateTime_X;
  BofPath Path;

  BOF_FTP_FILE()
  {
    Reset();
  }

  void Reset()
  {
    Right_E = BOF_FILE_PERMISSION::BOF_PERM_S_NONE;
    User_S = "";
    Group_S = "";
    Size_U64 = 0;
    DateTime_X.Reset();
  }
};

//#define PTR_TO_PARENT_SESSION(ptr)		(ptr=(ParentCmdChannel().expired()) ? nullptr : ParentCmdChannel().lock())
//#define PTR_TO_DATA_SESSION(ptr)			(ptr=ChildDataChannel())

class BofSocketIo:public IBofSocketIo, public std::enable_shared_from_this<BofSocketIo>
{
private:
  BOF_SOCKET_IO_PARAM				mSocketIoParam_X;
	bool											mConnected_B = false;
	std::unique_ptr<BofSocket> mpuSocket = nullptr;
//	void											*mpSocketUserArg = nullptr;
  BofCircularBuffer<BOF_SOCKET_WRITE_PARAM> *mpAsyncWriteRequestCollection=nullptr;
	BofCircularBuffer<std::string> *mpReplyCollection = nullptr;
	std::string mCurrentIoDataCommand_S = "";

	bool											mDataPreAllocated_B=false;
  uint8_t										*mpDataBuffer_U8=nullptr;
	uint32_t									mWriteIndex_U32=0;
  uint32_t									mDelimiterStartIndex_U32=0;
	std::string               mConnectedFromIp_S = "";
	std::string               mConnectedToIp_S = "";
  bool                      mIsLoggedIn_B = false;
  uint32_t                  mLastPartialReplyCode_U32 = 0xFFFFFFFF;
	BOF_SOCKET_SESSION_TYPE   mSessionType_E= BOF_SOCKET_SESSION_TYPE::UNKNOWN;
	uint32_t                  mStartSessionTime_U32=0;
	uint32_t                  mLastIoTime_U32=0;
	uint32_t                  mSessionIndex_U32 = 0xFFFFFFFF;
	BOF_SOCKET_SESSION_STATISTIC  mSocketSessionStatistic_X;
	std::shared_ptr<BofSocketIo>	mpsChildDataChannel=nullptr;
	std::weak_ptr<BofSocketIo>		mpwParentCmdChannel;
	BofSocketServer *mpBofSocketServer = nullptr;
	BOF::BOF_BUFFER mDataBuffer_X;

	//use mConnected_B bool											mChannelConnected_B = false;
	BofCvSetter<bool>					mChannelConnectedCvSetter = [&](bool _ChannelConnected_B) { mConnected_B = _ChannelConnected_B; };
	BofCvPredicateAndReset<>	mChannelConnectedCvPredicateAndReset = [&]() -> bool { bool Rts_B = mConnected_B; /* no mConnected_B = false;*/ return Rts_B; };
	BofCvPredicateAndReset<>	mChannelDisconnectedCvPredicateAndReset = [&]() -> bool { bool Rts_B = !mConnected_B; /* no mConnected_B = true;*/ return Rts_B; };
	BOF_CONDITIONAL_VARIABLE	mChannelConnectedCv_X;
	
protected:
  BOFERR mErrorCode_E;

public:
  BofSocketIo(BofSocketServer *_pBofSocketServer,	std::unique_ptr<BofSocket> _puSocket, const BOF_SOCKET_IO_PARAM & _rBofSocketIoParam_X);
	virtual ~BofSocketIo();
	BOFERR AllocateDataBuffer(uint64_t _BufferSize_U64);
	BOFERR ReleaseDataBuffer();
	void ClearDataBuffer();
	BOFERR TransferDataBufferOwnershipTo(std::shared_ptr<BofSocketIo> _psSocketSession);
	BOFERR MemCpyInDataBuffer(uint64_t _BufferSize_U64, const uint8_t *_pData_U8);
	BOF_BUFFER &DataBuffer();
	void DataBuffer(const BOF_BUFFER &_rDataBuffer_X);
	BOFERR Write(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, const std::string &_rBuffer_S, void *_pWriteContext);
  BOFERR Write(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, uint32_t & _rNb_U32, const uint8_t *_pBuffer_U8, void *_pWriteContext);
	BOFERR LastErrorCode() const;
  BOFERR ParseAndDispatchIncomingData(uint32_t _IoTimeoutInMs_U32);//Internal use
	BOFERR NotifyPendingData();

	bool	 Connected() const;
	void Connected(bool _IsConnected_B);
	BOFERR DestroySocket(const std::string &_rName_S);
	uint32_t SessionIndex() const;
	void SessionIndex(uint32_t _SessionIndex_U32);

	std::shared_ptr<BofSocketIo> ChildDataChannel() const;
	void ChildDataChannel(std::shared_ptr<BofSocketIo> _psChildDataChannel);
	std::weak_ptr<BofSocketIo> ParentCmdChannel() const;
	void ParentCmdChannel(std::shared_ptr<BofSocketIo> _psParentCmdChannel);
	BofSocket *NativeBofSocketPointer() const;
	BOFSOCKET NativeSocketHandle() const;
	BOF_SOCKET_SESSION_TYPE SessionType() const;
	void SessionType(BOF_SOCKET_SESSION_TYPE _SessionType_E);
	BOF_SOCKET_IO_NOTIFY_TYPE NotifyType() const;
	BOF_SOCKET_SESSION_STATISTIC  SocketSessionStatistic() const;
	void ResetSocketSessionStatistic();
	uint32_t LastIoTimeInMs() const;
	void LastIoTimeInMs(uint32_t _LastIoTimeInMs_U32);
//	void WaitingForReply(bool _WaitingForReply_B);

	uint32_t NoIoCloseTimeoutInMs() const;
	void		 NoIoCloseTimeoutInMs(uint32_t _TimeoutInMs_U32);
	BOFSOCKET SessionId();
	const std::string &ConnectedFromIpAddress() const;
	const std::string &ConnectedToIpAddress() const;

  BOFERR Login(uint32_t _TimeoutInMs_U32, const std::string &_rUser_S, const std::string &_rPassword_S);
  bool IsLoggedIn() const;
  BOFERR Logout(uint32_t _TimeoutInMs_U32);

	BOFERR IoDataCommand(uint32_t _ConnectionTimeoutInMs_U32, uint32_t _NoIoCloseTimeoutInMs_U32, const std::string &_rCmd_S, std::shared_ptr<BofSocketIo> &_rpsDataBofSocketIo, uint32_t &_rReplyCode_U32);
	const std::string &CurrentIoDataCommand() const;
	void CurrentIoDataCommand(const std::string &_rLastIoDataCommand_S);

 // BOFERR Download(uint32_t _InterIoTimeoutInMs_U32, bool _Passive_B, uint16_t _MinPortValue_U16, uint16_t _MaxPortValue_U16, const std::string &_rIpAddress_S, const BofPath &_rPath, ON_SOCKET_DATA_IO_CALLBACK _Callback, void *_pUserArg);
//	BOFERR Upload(uint32_t _InterIoTimeoutInMs_U32, bool _Passive_B, uint16_t _MinPortValue_U16, uint16_t _MaxPortValue_U16, const std::string &_rIpAddress_S, const BofPath &_rPath, ON_SOCKET_DATA_IO_CALLBACK _Callback, void *_pUserArg);
//If _ExpectedReplyCode_U32 is 0 Do not wait
  BOFERR SendCommandAndWaitForReply(uint32_t _TimeoutInMs_U32, const std::string &_rCommand_S, uint32_t _ExpectedReplyCode_U32, uint32_t &_rReplyCode_U32, std::string &_rReply_S);
	//If _ExpectedReplyCode_U32 is 0 wait for any reply
	BOFERR WaitForCommandReply(uint32_t _TimeoutInMs_U32, uint32_t _ExpectedReplyCode_U32, uint32_t &_rReplyCode_U32, std::string &_rReply_S);

//	BOFERR LockSocketCriticalSection(const char *_pLocker_c);
//	BOFERR UnlockSocketCriticalSection();
	std::string SocketIoDebugInfo();

  static BOFERR S_ParseListLineBuffer(const std::string &_rBaseDirectory_S, const char *_pListLineBuffer_c, std::vector<BOF_FTP_FILE> &_rFtpFileCollection);
	BOFERR WaitForChannelConnected(uint32_t _TimeoutInMs_U32);
	BOFERR WaitForChannelDisconnected(uint32_t _TimeoutInMs_U32);

protected:

private:
	BOFERR DispatchIncomingLine(uint32_t _IoTimeoutInMs_U32, std::string &_rIncomingLine_S);
	BOFERR ParseReply(const std::string &_rLine_S, bool &_rFullReply_B, uint32_t &_FtpReplyCode_U32, std::string &_rFtpMessage_S);
	void SetLastIoDataCommand(const std::string &_rLastIoDataCommand_S);	//To avoid reccursive call in LastIoDataCommand
};
END_BOF_NAMESPACE()
