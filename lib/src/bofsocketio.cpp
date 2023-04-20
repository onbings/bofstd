/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the BofSocketIo class.
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
#include <bofstd/bofsocketio.h>
#include <bofstd/bofsocketserver.h>
#include <bofstd/bofsocketsessionmanager.h>
#include <bofstd/bofstring.h>

#include <regex>

BEGIN_BOF_NAMESPACE()

BofSocketIo::BofSocketIo(BofSocketServer *_pBofSocketServer, std::unique_ptr<BofSocket> _puSocket, const BOF_SOCKET_IO_PARAM &_rBofSocketIoParam_X)
{
  BOF_CIRCULAR_BUFFER_PARAM BofCircularBufferParam_X;

  mpBofSocketServer = _pBofSocketServer;
  mErrorCode_E = BOF_ERR_EINVAL;
  mStartSessionTime_U32 = Bof_GetMsTickCount();
  mLastIoTime_U32 = Bof_GetMsTickCount();
  mSocketIoParam_X = _rBofSocketIoParam_X;
  mpuSocket = std::move(_puSocket);

  BOF_ASSERT(mpuSocket != nullptr);
  // Done in caller BOF_ASSERT(mpuSocket->LastErrorCode() == BOF_ERR_NO_ERROR);
  BOF_ASSERT(mSocketIoParam_X.NotifyRcvBufferSize_U32 >= BOF_SOCKETIO_MIN_NOTIFY_RCV_BUFFER_SIZE);

  if ((mpuSocket) && (mSocketIoParam_X.NotifyRcvBufferSize_U32 >= BOF_SOCKETIO_MIN_NOTIFY_RCV_BUFFER_SIZE))
  {
    mErrorCode_E = mpuSocket->LastErrorCode();
    if (mErrorCode_E == BOF_ERR_NO_ERROR)
    {
      //			mErrorCode_E = Bof_CreateMutex(mSocketIoParam_X.Name_S + "_mtx", true, true, mMtx_X);
      if (mErrorCode_E == BOF_ERR_NO_ERROR)
      {

        mErrorCode_E = Bof_CreateConditionalVariable("Connected", true, mChannelConnectedCv_X);
        if (mErrorCode_E == BOF_ERR_NO_ERROR)
        {
          mErrorCode_E = mpuSocket->SetSocketBufferSize(mSocketIoParam_X.SocketRcvBufferSize_U32, mSocketIoParam_X.SocketSndBufferSize_U32);
          if (mErrorCode_E == BOF_ERR_NO_ERROR)
          {
            if (mSocketIoParam_X.NbMaxAsyncWritePendingRequest_U32)
            {
              mErrorCode_E = BOF_ERR_ENOMEM;
              BofCircularBufferParam_X.MultiThreadAware_B = true;
              BofCircularBufferParam_X.NbMaxElement_U32 = mSocketIoParam_X.NbMaxAsyncWritePendingRequest_U32;
              BofCircularBufferParam_X.Overwrite_B = false;
              mpAsyncWriteRequestCollection = new BofCircularBuffer<BOF_SOCKET_WRITE_PARAM>(BofCircularBufferParam_X);
              if (mpAsyncWriteRequestCollection)
              {
                mErrorCode_E = mpAsyncWriteRequestCollection->LastErrorCode();
              }
            }
          }
        }
      }
    }

    if (mErrorCode_E == BOF_ERR_NO_ERROR)
    {
      BofCircularBufferParam_X.MultiThreadAware_B = true;
      BofCircularBufferParam_X.NbMaxElement_U32 = 16;
      BofCircularBufferParam_X.Overwrite_B = true;
      mpReplyCollection = new BofCircularBuffer<std::string>(BofCircularBufferParam_X);
      if (mpReplyCollection)
      {
        mErrorCode_E = mpReplyCollection->LastErrorCode();
      }
    }

    if (mErrorCode_E == BOF_ERR_NO_ERROR)
    {
      mErrorCode_E = BOF_ERR_ENOMEM;
      if (mSocketIoParam_X.pData)
      {
        mDataPreAllocated_B = true;
        mpDataBuffer_U8 = reinterpret_cast<uint8_t *>(mSocketIoParam_X.pData); // Should be at least mSocketIoParam_X.NotifyRcvBufferSize_U32  long
      }
      else
      {
        mDataPreAllocated_B = false;
        mpDataBuffer_U8 = new uint8_t[mSocketIoParam_X.NotifyRcvBufferSize_U32];
      }
      if (mpDataBuffer_U8)
      {
        mErrorCode_E = BOF_ERR_NO_ERROR;
      }
    }
  }
}

BofSocketIo::~BofSocketIo()
{
  // No as derived object is already deleted	DestroySocket(); =>DestroySocket must be called from derived class destructor
  DestroySocket("~BofSocketIo"); // #Con#
  if (!mDataPreAllocated_B)
  {
    BOF_SAFE_DELETE_ARRAY(mpDataBuffer_U8);
  }
  BOF_SAFE_DELETE(mpAsyncWriteRequestCollection);
  BOF_SAFE_DELETE(mpReplyCollection);
  mDataBuffer_X.ReleaseStorage();
  //	Bof_DestroyMutex(mMtx_X);

  Bof_DestroyConditionalVariable(mChannelConnectedCv_X);
}

BOFERR BofSocketIo::AllocateDataBuffer(uint64_t _BufferSize_U64)
{
  if (mDataBuffer_X.pData_U8)
  {
    mDataBuffer_X.ReleaseStorage();
  }
  return (mDataBuffer_X.AllocStorage(_BufferSize_U64) != nullptr) ? BOF_ERR_NO_ERROR : BOF_ERR_ENOMEM;
}
BOFERR BofSocketIo::ReleaseDataBuffer()
{
  mDataBuffer_X.ReleaseStorage();

  return BOF_ERR_NO_ERROR;
}
void BofSocketIo::ClearDataBuffer()
{
  mDataBuffer_X.Clear();
}
BOFERR BofSocketIo::TransferDataBufferOwnershipTo(
    std::shared_ptr<BofSocketIo> _psSocketSession) // After passing mDataBuffer_X from closing data session to cmd session to avoid 2 delete op->!! leak if called without releasing buffer except if you transfert ownership
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if ((_psSocketSession) && (_psSocketSession.get() != this))
  {
    _psSocketSession->DataBuffer(DataBuffer());
    mDataBuffer_X.Reset();
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}
BOFERR BofSocketIo::MemCpyInDataBuffer(uint64_t _BufferSize_U64, const uint8_t *_pData_U8)
{
  uint64_t NbWritten_U64;
  return (mDataBuffer_X.Write(_BufferSize_U64, _pData_U8, NbWritten_U64) != nullptr) ? BOF_ERR_NO_ERROR : BOF_ERR_TOO_BIG;
}
BOF::BOF_BUFFER &BofSocketIo::DataBuffer()
{
  return mDataBuffer_X;
}
void BofSocketIo::DataBuffer(const BOF_BUFFER &_rDataBuffer_X)
{
  mDataBuffer_X = _rDataBuffer_X;
}
void BofSocketIo::Connected(bool _IsConnected_B)
{
  if (_IsConnected_B)
  {
    mConnectedFromIp_S = Bof_SocketAddressToString(mpuSocket->GetSrcIpAddress(), true, true);
    mConnectedToIp_S = Bof_SocketAddressToString(mpuSocket->GetDstIpAddress(), true, true);
  }
  else
  {
    mConnectedFromIp_S = "";
    mConnectedToIp_S = "";
  }
  Bof_SignalConditionalVariable(mChannelConnectedCv_X, mChannelConnectedCvSetter, _IsConnected_B);
  // BOF_DBG_PRINTF("===@@ %p Bof_SignalConditionalVariable %d\n", &mChannelConnectedCv_X, _IsConnected_B);
}

bool BofSocketIo::Connected() const
{
  return mConnected_B;
}

uint32_t BofSocketIo::SessionIndex() const
{
  return mSessionIndex_U32;
}
void BofSocketIo::SessionIndex(uint32_t _SessionIndex_U32)
{
  mSessionIndex_U32 = _SessionIndex_U32;
}

BofSocket *BofSocketIo::NativeBofSocketPointer() const
{
  return mpuSocket.get();
}
BOFSOCKET BofSocketIo::NativeSocketHandle() const
{
  BOFSOCKET Rts = BOF_INVALID_SOCKET_VALUE;
  if (mpuSocket)
  {
    Rts = mpuSocket->GetSocketHandle();
  }
  return Rts;
}
BOF_SOCKET_SESSION_TYPE BofSocketIo::SessionType() const
{
  return mSessionType_E;
}

void BofSocketIo::SessionType(BOF_SOCKET_SESSION_TYPE _SessionType_E)
{
  mSessionType_E = _SessionType_E;
}
BOF_SOCKET_IO_NOTIFY_TYPE BofSocketIo::NotifyType() const
{
  return mSocketIoParam_X.NotifyType_E;
}

BOF_SOCKET_SESSION_STATISTIC BofSocketIo::SocketSessionStatistic() const
{
  return mSocketSessionStatistic_X;
}
void BofSocketIo::ResetSocketSessionStatistic()
{
  mSocketSessionStatistic_X.Reset();
}
uint32_t BofSocketIo::LastIoTimeInMs() const
{
  return mLastIoTime_U32;
}
void BofSocketIo::LastIoTimeInMs(uint32_t _LastIoTimeInMs_U32)
{
  mLastIoTime_U32 = _LastIoTimeInMs_U32;
}

std::shared_ptr<BofSocketIo> BofSocketIo::ChildDataChannel() const
{
  return mpsChildDataChannel;
}
void BofSocketIo::ChildDataChannel(std::shared_ptr<BofSocketIo> _psChildDataChannel)
{
  mpsChildDataChannel = _psChildDataChannel;
}
std::weak_ptr<BofSocketIo> BofSocketIo::ParentCmdChannel() const
{
  return mpwParentCmdChannel;
}
void BofSocketIo::ParentCmdChannel(std::shared_ptr<BofSocketIo> _psParentCmdChannel)
{
  mpwParentCmdChannel = _psParentCmdChannel;
}

uint32_t BofSocketIo::NoIoCloseTimeoutInMs() const
{
  return mSocketIoParam_X.NoIoCloseTimeoutInMs_U32;
}

void BofSocketIo::NoIoCloseTimeoutInMs(uint32_t _TimeoutInMs_U32)
{
  mSocketIoParam_X.NoIoCloseTimeoutInMs_U32 = _TimeoutInMs_U32;
}

BOFSOCKET BofSocketIo::SessionId()
{
  return mpuSocket ? mpuSocket->GetSocketHandle() : 0xFFFFFFFF;
}

const std::string &BofSocketIo::ConnectedFromIpAddress() const
{
  return mConnectedFromIp_S;
}
const std::string &BofSocketIo::ConnectedToIpAddress() const
{
  return mConnectedToIp_S;
}

BOFERR BofSocketIo::DestroySocket(const std::string &_rName_S)
{
  BOFERR Rts_E = BOF_ERR_ENOTCONN;
  std::shared_ptr<BofSocketIo> psParentSocketSession;

  if (SessionType() != BOF_SOCKET_SESSION_TYPE::DATA_LISTENER)
  {
    // try
    {
      // psParentSocketSession = std::shared_ptr<BofSocketIo>(mpwParentCmdChannel);
      // if (psParentSocketSession->ChildDataChannel())
      psParentSocketSession = (ParentCmdChannel().expired()) ? nullptr : ParentCmdChannel().lock();
      if (psParentSocketSession)
      {
        //				BOF_ASSERT(psParentCmdChannel->ChildDataChannel().get() == this);
        //				psParentCmdChannel->ChildDataChannel(nullptr);
        psParentSocketSession->ChildDataChannel(nullptr);
      }
    }
    if (mpsChildDataChannel)
    {
      mpsChildDataChannel->DestroySocket(_rName_S + "_mpsChildDataChannel"); // #Con#
    }
  }
  if (mpuSocket)
  {
    mpuSocket.reset(nullptr);
  }
  Rts_E = BOF_ERR_NO_ERROR;

  return Rts_E;
}

BOFERR BofSocketIo::LastErrorCode() const
{
  return mErrorCode_E;
}

BOFERR BofSocketIo::Write(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, const std::string &_rBuffer_S, void *_pWriteContext)
{
  uint32_t Nb_U32 = static_cast<uint32_t>(_rBuffer_S.size());
  return Write(_TimeoutInMs_U32, _AsyncMode_B, Nb_U32, reinterpret_cast<const uint8_t *>(_rBuffer_S.c_str()), _pWriteContext);
}

BOFERR BofSocketIo::Write(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, uint32_t &_rNb_U32, const uint8_t *_pBuffer_U8, void *_pWriteContext)
{
  BOFERR Rts_E = BOF_ERR_ECONNREFUSED;
  BOF_SOCKET_WRITE_PARAM SocketWriteParam_X;
  uint32_t Nb_U32;

  if (Connected())
  {
    if (_AsyncMode_B)
    {
      Rts_E = BOF_ERR_INVALID_STATE;
      if ((mSocketIoParam_X.NbMaxAsyncWritePendingRequest_U32) && (mpAsyncWriteRequestCollection))
      {
        SocketWriteParam_X.TimeoutInMs_U32 = _TimeoutInMs_U32;
        SocketWriteParam_X.Nb_U32 = _rNb_U32;
        SocketWriteParam_X.pBuffer_U8 = _pBuffer_U8;
        SocketWriteParam_X.pWriteContext = _pWriteContext;
        Rts_E = mpAsyncWriteRequestCollection->Push(&SocketWriteParam_X, 0, nullptr);
      }
    }
    else
    {
      Nb_U32 = _rNb_U32;
      Rts_E = mpuSocket->V_WriteData(_TimeoutInMs_U32, _rNb_U32, _pBuffer_U8);
      V_SignalDataWritten(Rts_E, _rNb_U32, _pBuffer_U8, Nb_U32 - _rNb_U32, _pBuffer_U8 + _rNb_U32, _pWriteContext);
    }
  }
  return Rts_E;
}

BOFERR BofSocketIo::NotifyPendingData()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;

  if (mWriteIndex_U32)
  {
    Rts_E = V_SignalDataRead(mWriteIndex_U32, mpDataBuffer_U8);
    mWriteIndex_U32 = 0;
    mDelimiterStartIndex_U32 = 0;
  }
  return Rts_E;
}
BOFERR BofSocketIo::ParseAndDispatchIncomingData(uint32_t _IoTimeoutInMs_U32)
{
  BOFERR Rts_E = BOF_ERR_ENOTCONN; // , Sts_E;
  BOF_COM_CHANNEL_STATUS Status_X;
  uint32_t Nb_U32, NbRemainingByte_U32, StartIndex_U32, LastIndex_U32, i_U32, NbByteInDelimitedPacket_U32, NbByteToMove_U32;
  std::string Reply_S;

  if (Connected())
  {
    // NO Only one V_GetStatus per call to this function. If it is not the case the second V_GetStatus will returns BOF_ERR_ENETRESET
    //		for ((Rts_E = mpuSocket->V_GetStatus(Status_X)); (Rts_E == BOF_ERR_NO_ERROR); (Rts_E = mpuSocket->V_GetStatus(Status_X)))
    Rts_E = mpuSocket->V_GetStatus(Status_X);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = Status_X.Sts_E;
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Rts_E = BOF_ERR_EMPTY;
        if (Status_X.NbIn_U32)
        {
          NbRemainingByte_U32 = mSocketIoParam_X.NotifyRcvBufferSize_U32 - mWriteIndex_U32;
          Nb_U32 = (Status_X.NbIn_U32 <= NbRemainingByte_U32) ? Status_X.NbIn_U32 : NbRemainingByte_U32;

          Rts_E = mpuSocket->V_ReadData(_IoTimeoutInMs_U32, Nb_U32, &mpDataBuffer_U8[mWriteIndex_U32]);
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            StartIndex_U32 = mWriteIndex_U32;
            LastIndex_U32 = StartIndex_U32 + Nb_U32;
            mWriteIndex_U32 += Nb_U32;

            if (mSocketIoParam_X.NotifyType_E == BOF_SOCKET_IO_NOTIFY_TYPE::ASAP)
            {
              /*Sts_E = */ NotifyPendingData();
            }
            else if (mSocketIoParam_X.NotifyType_E == BOF_SOCKET_IO_NOTIFY_TYPE::WHEN_FULL_OR_CLOSED)
            {
              if (mWriteIndex_U32 == mSocketIoParam_X.NotifyRcvBufferSize_U32)
              {
                /*Sts_E =*/NotifyPendingData();
              }
            }
            else if (mSocketIoParam_X.NotifyType_E == BOF_SOCKET_IO_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND)
            {
              // We look for Delimiter_U8 in mpDataBuffer_U8 data chunk per data chunk
              BOF_ASSERT(mWriteIndex_U32 <= mSocketIoParam_X.NotifyRcvBufferSize_U32);
              BOF_ASSERT(mDelimiterStartIndex_U32 < mSocketIoParam_X.NotifyRcvBufferSize_U32);
              BOF_ASSERT(LastIndex_U32 <= mSocketIoParam_X.NotifyRcvBufferSize_U32);

              do
              {
                for (i_U32 = StartIndex_U32; i_U32 < LastIndex_U32; i_U32++)
                {
                  if (mpDataBuffer_U8[i_U32] == mSocketIoParam_X.Delimiter_U8)
                  {
                    NbByteInDelimitedPacket_U32 = i_U32 - mDelimiterStartIndex_U32 + 1;
                    BOF_ASSERT(NbByteInDelimitedPacket_U32 <= mSocketIoParam_X.NotifyRcvBufferSize_U32);
                    Reply_S = std::string(reinterpret_cast<char *>(&mpDataBuffer_U8[mDelimiterStartIndex_U32]), NbByteInDelimitedPacket_U32);
                    mpReplyCollection->Push(&Reply_S, 0, nullptr);

                    /*Sts_E =*/V_SignalDataRead(NbByteInDelimitedPacket_U32, &mpDataBuffer_U8[mDelimiterStartIndex_U32]);
                    StartIndex_U32 = i_U32 + 1;
                    BOF_ASSERT(StartIndex_U32 <= mSocketIoParam_X.NotifyRcvBufferSize_U32);

                    mDelimiterStartIndex_U32 = i_U32 + 1;
                    BOF_ASSERT(mDelimiterStartIndex_U32 <= mSocketIoParam_X.NotifyRcvBufferSize_U32);
                    break;
                  }
                }
              } while (i_U32 < LastIndex_U32);

              if (mWriteIndex_U32 >= mSocketIoParam_X.NotifyRcvBufferSize_U32) // can happens if last byte of buffer is equal to Delimiter_U8 or no Delimiter_U8 byte found before the end
              {
                mWriteIndex_U32 = mSocketIoParam_X.NotifyRcvBufferSize_U32;
                if (mDelimiterStartIndex_U32 == 0)
                {
                  Reply_S = std::string(reinterpret_cast<char *>(mpDataBuffer_U8), mSocketIoParam_X.NotifyRcvBufferSize_U32);
                  mpReplyCollection->Push(&Reply_S, 0, nullptr);
                  /*Sts_E =*/NotifyPendingData();
                }
                else
                {
                  NbByteToMove_U32 = mSocketIoParam_X.NotifyRcvBufferSize_U32 - mDelimiterStartIndex_U32;

                  BOF_ASSERT(NbByteToMove_U32 < mSocketIoParam_X.NotifyRcvBufferSize_U32);
                  if (NbByteToMove_U32)
                  {
                    memcpy(mpDataBuffer_U8, &mpDataBuffer_U8[mDelimiterStartIndex_U32], NbByteToMove_U32);
                  }
                  mWriteIndex_U32 = NbByteToMove_U32;
                  mDelimiterStartIndex_U32 = 0;
                }
              }
              BOF_ASSERT(mWriteIndex_U32 <= mSocketIoParam_X.NotifyRcvBufferSize_U32);
              BOF_ASSERT(mDelimiterStartIndex_U32 <= mSocketIoParam_X.NotifyRcvBufferSize_U32);
            }
          }
          else // if ( mpuSocket->V_ReadData == BOF_ERR_NO_ERROR)
          {
            // NO break;
          }
        }
        else // if (Status_X.NbIn_U32)
        {
          BOF_ASSERT(0); // Can happens and should be cathed by previous test: Status_X.Sts_E != BOF_ERR_NO_ERROR
          /*
                    if (!Status_X.Connected_B)
                    {
                      Rts_E = BOF_ERR_ENETRESET;
                      BOF_ASSERT(Rts_E == Status_X.Sts_E);
                    }
          */
          // NO					break;
        }
      }
      else // Status_X.Sts_E != BOF_ERR_NO_ERROR
      {
        BOF_ASSERT(Status_X.Connected_B == false);
        BOF_ASSERT(Rts_E == BOF_ERR_ENETRESET);
        // NO				break;
      }
    } // NO !!! for ((Rts_E = mpuSocket->V_GetStatus(Status_X)); (Rts_E == BOF_ERR_NO_ERROR); (Rts_E = mpuSocket->V_GetStatus(Status_X)))
  }
  else // if (Connected())
  {
  }
  return Rts_E;
}

BOFERR BofSocketIo::Login(uint32_t _TimeoutInMs_U32, const std::string &_rUser_S, const std::string &_rPassword_S)
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;
  uint32_t ReplyCode_U32, Start_U32;
  std::string Reply_S;

  if (mSessionType_E == BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL)
  {
    Rts_E = BOF_ERR_NOT_OPENED;
    if (Connected())
    {
      Start_U32 = Bof_GetMsTickCount();
      Rts_E = SendCommandAndWaitForReply(_TimeoutInMs_U32, Bof_Sprintf("USER %s\r\n", _rUser_S.c_str()), 331, ReplyCode_U32, Reply_S);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Rts_E = Bof_ReEvaluateTimeout(Start_U32, _TimeoutInMs_U32);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          Rts_E = SendCommandAndWaitForReply(_TimeoutInMs_U32, Bof_Sprintf("PASS %s\r\n", _rPassword_S.c_str()), 230, ReplyCode_U32, Reply_S);
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            Rts_E = Bof_ReEvaluateTimeout(Start_U32, _TimeoutInMs_U32);
            if (Rts_E == BOF_ERR_NO_ERROR)
            {
              Rts_E = SendCommandAndWaitForReply(_TimeoutInMs_U32, "TYPE I\r\n", 200, ReplyCode_U32, Reply_S); // Binary data
              if (Rts_E == BOF_ERR_NO_ERROR)
              {
                mIsLoggedIn_B = true;
              }
            }
          }
        }
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketIo::IoDataCommand(uint32_t _ConnectionTimeoutInMs_U32, uint32_t _NoIoCloseTimeoutInMs_U32, const std::string &_rCmd_S, std::shared_ptr<BofSocketIo> &_rpsDataBofSocketIo, uint32_t &_rReplyCode_U32)
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;
  uint32_t Start_U32;
  std::string Reply_S;

  _rpsDataBofSocketIo = nullptr;
  _rReplyCode_U32 = 0;
  if (mSessionType_E == BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL)
  {
    Rts_E = BOF_ERR_NOT_OPENED;
    if (IsLoggedIn())
    {
      Rts_E = BOF_ERR_INIT;
      if (mpBofSocketServer)
      {
        Start_U32 = Bof_GetMsTickCount();
        Rts_E = mpBofSocketServer->ConnectToDataChannel(true, shared_from_this(), _ConnectionTimeoutInMs_U32, _NoIoCloseTimeoutInMs_U32, _rpsDataBofSocketIo);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          Rts_E = Bof_ReEvaluateTimeout(Start_U32, _ConnectionTimeoutInMs_U32);
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            CurrentIoDataCommand(_rCmd_S);
            Rts_E = SendCommandAndWaitForReply(_ConnectionTimeoutInMs_U32, _rCmd_S, 0, _rReplyCode_U32, Reply_S);
            if (Rts_E != BOF_ERR_NO_ERROR)
            {
              CurrentIoDataCommand("");
            }
          }
        }
      }
    }
  }
  return Rts_E;
}
const std::string &BofSocketIo::CurrentIoDataCommand() const
{
  return mCurrentIoDataCommand_S;
}
void BofSocketIo::SetLastIoDataCommand(const std::string &_rLastIoDataCommand_S) // To avoid reccursive call in LastIoDataCommand
{
  mCurrentIoDataCommand_S = _rLastIoDataCommand_S;
}
void BofSocketIo::CurrentIoDataCommand(const std::string &_rLastIoDataCommand_S)
{
  mCurrentIoDataCommand_S = _rLastIoDataCommand_S;
  if (SessionType() == BOF_SOCKET_SESSION_TYPE::COMMAND_CHANNEL)
  {
    if (ChildDataChannel())
    {
      ChildDataChannel()->SetLastIoDataCommand(_rLastIoDataCommand_S); // To avoid reccursive call in LastIoDataCommand
    }
  }
  else if (SessionType() == BOF_SOCKET_SESSION_TYPE::DATA_CHANNEL)
  {
    std::shared_ptr<BofSocketIo> psParentSocketSession = (ParentCmdChannel().expired()) ? nullptr : ParentCmdChannel().lock();
    if (psParentSocketSession)
    {
      psParentSocketSession->SetLastIoDataCommand(_rLastIoDataCommand_S); // To avoid reccursive call in LastIoDataCommand
    }
  }
}
bool BofSocketIo::IsLoggedIn() const
{
  return mIsLoggedIn_B;
}

BOFERR BofSocketIo::Logout(uint32_t _TimeoutInMs_U32)
{
  BOFERR Rts_E = BOF_ERR_NOT_OPENED;
  uint32_t ReplyCode_U32;
  std::string Reply_S;

  if (Connected())
  {
    Rts_E = BOF_ERR_EACCES;
    if (IsLoggedIn())
    {
      Rts_E = SendCommandAndWaitForReply(_TimeoutInMs_U32, "QUIT\r\n", 221, ReplyCode_U32, Reply_S);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        mIsLoggedIn_B = false;
      }
    }
  }
  return Rts_E;
}
BOFERR BofSocketIo::S_ParseListLineBuffer(const std::string &_rBaseDirectory_S, const char *_pListLineBuffer_c, std::vector<BOF_FTP_FILE> &_rFtpFileCollection)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  // https://regex101.com/
  static const std::regex S_RegExList("(.+?)\\s+(\\d+)\\s+(.+?)\\s+(.+?)\\s+(\\d+)\\s+(.+\\d+ +\\d+:\\d+)\\s+(.*)"); //-rwx------ 1 user group 1082130432000 May 26 00:00 000000A_^mmz]e^S_Train000011             .TRN
  std::cmatch ListMatch;
  std::string Fn_S, Val_S;
  // bool IsDirectory_B, IsLink_B;
  BofDateTime DateTime, Now;
  BOF_FTP_FILE FtpFile_X;
  const char *pEol_c;
  char pListLine_c[0x1000];
  uint32_t SizeOfLine_U32;

  // No clear as we can append file to collection if list is made of several buffer	_rFtpFileCollection.clear();
  if (_pListLineBuffer_c)
  {
    do
    {
      Rts_E = BOF_ERR_NO_ERROR;
      pEol_c = strchr(_pListLineBuffer_c, '\n');
      SizeOfLine_U32 = static_cast<uint32_t>((pEol_c - _pListLineBuffer_c) + 1); //+1 len
      if (!pEol_c)
      {
        break;
      }
      else
      {
        Rts_E = BOF_ERR_FORMAT;
        if ((pEol_c[-1] == '\r') && (SizeOfLine_U32 < sizeof(pListLine_c) - 16)) //-16  terminating null and paranoid
        {
          Bof_StrNCpy(pListLine_c, _pListLineBuffer_c, SizeOfLine_U32 + 1); //+1 nfor null char
          _pListLineBuffer_c += SizeOfLine_U32;
          if ((std::regex_search(pListLine_c, ListMatch, S_RegExList)) && (ListMatch.size() == 1 + 7))
          {
            /*
            for (int i = 0; i < ListMatch.size(); i++)
            {
            //BOF_DBG_PRINTF("%d: %s\r\n", i, ListMatch[i].str().c_str());
            }
            */
            //-rwx------ 1 user group 6878658560 May 26 00:00 000000A_^mmz_oO2_Train000011             .TRN
            Val_S = ListMatch[1].str();
            //						IsDirectory_B = (Val_S[0] == 'd');
            //						IsLink_B = (Val_S[0] == 'l');
            FtpFile_X.Reset();
            if (Val_S[1] == 'r')
            {
              FtpFile_X.Right_E = FtpFile_X.Right_E | BOF_FILE_PERMISSION::BOF_PERM_S_IRUSR;
            }
            if (Val_S[2] == 'w')
            {
              FtpFile_X.Right_E = FtpFile_X.Right_E | BOF_FILE_PERMISSION::BOF_PERM_S_IWUSR;
            }
            if (Val_S[3] == 'x')
            {
              FtpFile_X.Right_E = FtpFile_X.Right_E | BOF_FILE_PERMISSION::BOF_PERM_S_IXUSR;
            }
            if (Val_S[3] == 's')
            {
              FtpFile_X.Right_E = FtpFile_X.Right_E | BOF_FILE_PERMISSION::BOF_PERM_S_ISVTX;
            }

            if (Val_S[4] == 'r')
            {
              FtpFile_X.Right_E = FtpFile_X.Right_E | BOF_FILE_PERMISSION::BOF_PERM_S_IRGRP;
            }
            if (Val_S[5] == 'w')
            {
              FtpFile_X.Right_E = FtpFile_X.Right_E | BOF_FILE_PERMISSION::BOF_PERM_S_IWGRP;
            }
            if (Val_S[6] == 'x')
            {
              FtpFile_X.Right_E = FtpFile_X.Right_E | BOF_FILE_PERMISSION::BOF_PERM_S_IXGRP;
            }

            if (Val_S[7] == 'r')
            {
              FtpFile_X.Right_E = FtpFile_X.Right_E | BOF_FILE_PERMISSION::BOF_PERM_S_IROTH;
            }
            if (Val_S[8] == 'w')
            {
              FtpFile_X.Right_E = FtpFile_X.Right_E | BOF_FILE_PERMISSION::BOF_PERM_S_IWOTH;
            }
            if (Val_S[9] == 'x')
            {
              FtpFile_X.Right_E = FtpFile_X.Right_E | BOF_FILE_PERMISSION::BOF_PERM_S_IXOTH;
            }
            FtpFile_X.User_S = ListMatch[3].str();
            FtpFile_X.Group_S = ListMatch[4].str();
            FtpFile_X.Size_U64 = std::stoull(ListMatch[5].str());
            FtpFile_X.DateTime.FromString(ListMatch[6].str(), "%b %d %H:%M");
            Bof_Now(Now);
            // Will change dayofweek or isvalid FtpFile_X.DateTime_X.Year_U16 = Now_X.Year_U16;
            // Bof_ComputeDayOfWeek(FtpFile_X.DateTime_X, FtpFile_X.DateTime_X.DayOfWeek_U8);

            Fn_S = _rBaseDirectory_S + ListMatch[7].str();
            FtpFile_X.Path = Fn_S.c_str();
            _rFtpFileCollection.push_back(FtpFile_X);
            Rts_E = BOF_ERR_NO_ERROR;
          } // if ((std::regex_search(pListLine_c, ListMatch, S_RegExList)) && (ListMatch.size() == 1 + 7))
        }   // if ((pEol_c[-1] == '\r') && (SizeOfLine_U32 < sizeof(pListLine_c - 16))) /
      }     // else if (!pEol_c)
    } while (Rts_E == BOF_ERR_NO_ERROR);
  }
  return Rts_E;
}

BOFERR BofSocketIo::SendCommandAndWaitForReply(uint32_t _TimeoutInMs_U32, const std::string &_rCommand_S, uint32_t _ExpectedReplyCode_U32, uint32_t &_rReplyCode_U32, std::string &_rReply_S)
{
  BOFERR Rts_E = BOF_ERR_NOT_OPENED;
  uint32_t Start_U32;
  int32_t RemainingTimeout_S32;

  _rReplyCode_U32 = 0;
  _rReply_S = "";
  if (Connected())
  {
    RemainingTimeout_S32 = _TimeoutInMs_U32;
    Start_U32 = Bof_GetMsTickCount();
    mpReplyCollection->Reset();
    Rts_E = Write(_TimeoutInMs_U32, false, _rCommand_S, nullptr);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = BOF_ERR_ETIMEDOUT;
      RemainingTimeout_S32 -= Bof_ElapsedMsTime(Start_U32);
      if (RemainingTimeout_S32 > 0)
      {
        Rts_E = WaitForCommandReply(RemainingTimeout_S32, _ExpectedReplyCode_U32, _rReplyCode_U32, _rReply_S);
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketIo::WaitForCommandReply(uint32_t _TimeoutInMs_U32, uint32_t _ExpectedReplyCode_U32, uint32_t &_rReplyCode_U32, std::string &_rReply_S)
{
  BOFERR Rts_E = BOF_ERR_NOT_OPENED;
  uint32_t Start_U32;
  std::string FtpReply_S, FullFtpReply_S, IncomingLine_S;
  //	uint32_t Test_U32 = 0;

  bool Finish_B, FullReply_B; // , MultiLine_B, LookForEos_B;;

  _rReplyCode_U32 = 0;
  _rReply_S = "";
  if (Connected())
  {
    // No lock is in SendCommandAndWaitForReply		Rts_E = Bof_LockMutex(mpsMtx_X);
    do
    {
      Finish_B = true;
      Start_U32 = Bof_GetMsTickCount();
      // Rts_E = DispatchIncomingLine(TimeoutInMs_S32, IncomingLine_S);
      /*
      switch (Test_U32)
      {
      case 0:
      //IncomingLine_S = "220-FileZilla Server 0.9.60 beta\r\n";
      IncomingLine_S = "123-First line\r\n";
      break;
      case 1:
      //IncomingLine_S = "220-written by Tim Kosse(tim.kosse@filezilla-project.org)\r\n";
      IncomingLine_S = "    Second line\r\n";
      break;
      case 2:
      //				IncomingLine_S = "220 Please visit https ://filezilla-project.org/\r\n";
      IncomingLine_S = "    234 A line beginning with numbers\r\n";
      break;
      case 3:
      IncomingLine_S = "123 The last line\r\n";
      break;
      }
      Test_U32++;
      */

      // Rts_E=Bof_WaitForConditionalVariable(mReplyReceivedCv_X, TimeoutInMs_S32, mReplyReceivedCvPredicateAndReset);
      Rts_E = mpReplyCollection->Pop(&IncomingLine_S, _TimeoutInMs_U32, nullptr, nullptr);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        FullFtpReply_S = FullFtpReply_S + IncomingLine_S;
        Rts_E = ParseReply(IncomingLine_S, FullReply_B, _rReplyCode_U32, FtpReply_S);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          _rReply_S = _rReply_S + FtpReply_S;
          if (!FullReply_B)
          {
            Finish_B = false;
            _rReply_S += "\r\n";
          }
        }
      } // Read if (Rts_E == BOF_ERR_NO_ERROR)
      else
      {
        if (Bof_ReEvaluateTimeout(Start_U32, _TimeoutInMs_U32) == BOF_ERR_NO_ERROR)
        {
          Finish_B = false;
        }
      }
    } while (!Finish_B);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (_ExpectedReplyCode_U32)
      {
        if (_rReplyCode_U32 != _ExpectedReplyCode_U32)
        {
          Rts_E = BOF_ERR_INVALID_ANSWER;
        }
      }
    }
    // No lock is in SendCommandAndWaitForReply			Bof_UnlockMutex(mpsMtx_X);
    //		WaitingForReply(false);	//Should be there AND in SendCommandAndWaitForReply to catch 220 welcome message on connect
  }
  return Rts_E;
}
/*
BOFERR BofSocketIo::LockSocketCriticalSection(const char *_pLocker_c)
{
  BOFERR Rts_E = Bof_LockMutex(mMtx_X);
  //BOFERR Rts_E = BOF_ERR_NO_ERROR;
  Bof_StrNCpy(mpLastLocker_c, _pLocker_c, sizeof(mpLastLocker_c));
  return Rts_E;
}

BOFERR BofSocketIo::UnlockSocketCriticalSection()
{
  BOFERR Rts_E = Bof_UnlockMutex(mMtx_X);
//	BOFERR Rts_E = BOF_ERR_NO_ERROR;
  mpLastLocker_c[0] = 0;
  return Rts_E;
}
*/
BOFERR BofSocketIo::ParseReply(const std::string &_rLine_S, bool &_rFullReply_B, uint32_t &_rFtpReplyCode_U32, std::string &_rFtpMessage_S)
{
  BOFERR Rts_E;
  uint32_t ReplyCode_U32;
  // char pReply_c[] = "220-FileZilla Server 0.9.60 beta\r\n220-written by Tim Kosse(tim.kosse@filezilla-project.org)\r\n220 Please visit https ://filezilla-project.org/\r\n";
  // 123-First line
  //	  Second line
  //	  234 A line beginning with numbers
  // 123 The last line
  static const std::regex S_RegExFtpReply("^(\\d\\d\\d|   )([ -])(.*)([\\r\\n]*)");
  std::cmatch FtpReplyMatch;
  const char *pBuffer_c = reinterpret_cast<const char *>(_rLine_S.c_str());

  _rFullReply_B = false;
  _rFtpReplyCode_U32 = 0xFFFFFFFF;
  _rFtpMessage_S = "";

  Rts_E = BOF_ERR_FORMAT;
  if (std::regex_search(pBuffer_c, FtpReplyMatch, S_RegExFtpReply))
  {
    if (FtpReplyMatch.size() == 5)
    {
      if ((FtpReplyMatch[1].length() == 3) && (FtpReplyMatch[2].length() == 1) && (FtpReplyMatch[4].length() == 2))
      {
        try
        {
          ReplyCode_U32 = static_cast<uint32_t>(std::stoi(FtpReplyMatch[1].str()));
          _rFullReply_B = FtpReplyMatch[2].str() == " ";
        }
        catch (const std::exception &) // Just for multiline (FtpReplyMatch[1].str()=="   ")
        {
          ReplyCode_U32 = mLastPartialReplyCode_U32;
          _rFullReply_B = false;
        }
        _rFtpMessage_S = FtpReplyMatch[3].str();
        BOF_ASSERT(FtpReplyMatch[4].str() == "\r\n");
        if (FtpReplyMatch[4].str() == "\r\n")
        {
          Rts_E = BOF_ERR_NO_ERROR;
          if (mLastPartialReplyCode_U32 != 0xFFFFFFFF)
          {
            BOF_ASSERT(ReplyCode_U32 == mLastPartialReplyCode_U32);
            if (ReplyCode_U32 != mLastPartialReplyCode_U32)
            {
              Rts_E = BOF_ERR_BAD_STATUS;
            }
          }
          mLastPartialReplyCode_U32 = _rFullReply_B ? 0xFFFFFFFF : ReplyCode_U32;
          _rFtpReplyCode_U32 = ReplyCode_U32;
        }
      }
    }
  }
  if (Rts_E != BOF_ERR_NO_ERROR)
  {
    mLastPartialReplyCode_U32 = 0xFFFFFFFF;
  }
  return Rts_E;
}

std::string BofSocketIo::SocketIoDebugInfo()
{
  std::string Rts_S, ActiveSession_S;

  Rts_S = Bof_Sprintf("Name:              %s\nType:              %d\nIndex:             %03d\nTime Start/Last:   %08X/%08X\nBufferSize R/S:    %08X/%08X\nNotify type/size:  %d/%08X\npData:             %p %s:%p\nWrite Index:       %04d\nDelim Index:       "
                      "%04d\nDelim:             %02X (%c)\nNbMaxAsyncWrite:   %06d %06d:%p (max %06d)\nNoIo Close To:     %d\n",
                      mSocketIoParam_X.Name_S.c_str(), mSessionType_E, mSessionIndex_U32, mStartSessionTime_U32, mLastIoTime_U32, mSocketIoParam_X.SocketRcvBufferSize_U32, mSocketIoParam_X.SocketSndBufferSize_U32, mSocketIoParam_X.NotifyType_E,
                      mSocketIoParam_X.NotifyRcvBufferSize_U32, mSocketIoParam_X.pData, mDataPreAllocated_B ? "True" : "False", mpDataBuffer_U8, mWriteIndex_U32, mDelimiterStartIndex_U32, mSocketIoParam_X.Delimiter_U8,
                      (mSocketIoParam_X.Delimiter_U8 < 32) ? '?' : mSocketIoParam_X.Delimiter_U8, mSocketIoParam_X.NbMaxAsyncWritePendingRequest_U32, mpAsyncWriteRequestCollection ? mpAsyncWriteRequestCollection->GetNbElement() : 0,
                      mpAsyncWriteRequestCollection, mpAsyncWriteRequestCollection ? mpAsyncWriteRequestCollection->GetMaxLevel() : 0, mSocketIoParam_X.NoIoCloseTimeoutInMs_U32);

  Rts_S += Bof_Sprintf("Reply:             %06d:%p (max %06d) Last: %03d\npSocket:           %p\nConnected:         %s '%s'->'%s' (Logged: %s)\n", mpReplyCollection ? mpReplyCollection->GetNbElement() : 0, mpReplyCollection,
                       mpReplyCollection ? mpReplyCollection->GetMaxLevel() : 0, mLastPartialReplyCode_U32, mpuSocket.get(), mConnected_B ? "True" : "False", mConnectedFromIp_S.c_str(), mConnectedToIp_S.c_str(), mIsLoggedIn_B ? "True" : "False");

  //	Rts_S += Bof_Sprintf("Child:             %d:%p\nParent:            %d:%s\nLock:              %08X %s\n", mpsChildDataChannel.use_count(), mpsChildDataChannel.get(), mpwParentCmdChannel.use_count(), mpwParentCmdChannel.expired() ? "True" : "False",
  //mMtx_X.Mtx.native_handle(), mpLastLocker_c);
  Rts_S += Bof_Sprintf("Child:             %d:%p\nParent:            %d:%s\n", mpsChildDataChannel.use_count(), mpsChildDataChannel.get(), mpwParentCmdChannel.use_count(), mpwParentCmdChannel.expired() ? "True" : "False");

  Rts_S +=
      Bof_Sprintf("NbOpConnect:       %06d\nNbOpConnectError:  %06d\nNbOpDisconnect:    %06d\nNbOpDisconnectErr: %06d\nNbOpListenError:   %06d\nNbOpRead:          %06d\nNbOpReadError:     %06d\nNbDataByteRead:    %06lld\nNbOpWrite:         "
                  "%06d\nNbOpWriteError:    %06d\nNbDataByteWritten: %06lld\nNbError:           %06d\nNbCloseEvent:      %06d\nNbConnectEvent:    %06d\nNbTcpShutdownEvnt: %06d\nNbTcpListenEvent:  %06d\nNbTimerEvent:      %06d\nNbTimerEventError: %06d\n",
                  mSocketSessionStatistic_X.NbOpConnect_U32, mSocketSessionStatistic_X.NbOpConnectError_U32, mSocketSessionStatistic_X.NbOpDisconnect_U32, mSocketSessionStatistic_X.NbOpDisconnectError_U32, mSocketSessionStatistic_X.NbOpListenError_U32,
                  mSocketSessionStatistic_X.NbOpRead_U32, mSocketSessionStatistic_X.NbOpReadError_U32, mSocketSessionStatistic_X.NbDataByteRead_U64, mSocketSessionStatistic_X.NbOpWrite_U32, mSocketSessionStatistic_X.NbOpWriteError_U32,
                  mSocketSessionStatistic_X.NbDataByteWritten_U64, mSocketSessionStatistic_X.NbError_U32, mSocketSessionStatistic_X.NbCloseEvent_U32, mSocketSessionStatistic_X.NbConnectEvent_U32, mSocketSessionStatistic_X.NbTcpShutdownEvent_U32,
                  mSocketSessionStatistic_X.NbTcpListenEvent_U32, mSocketSessionStatistic_X.NbTimerEvent_U32, mSocketSessionStatistic_X.NbTimerEventError_U32);

  return Rts_S;
}
BOFERR BofSocketIo::WaitForChannelConnected(uint32_t _TimeoutInMs_U32)
{
  BOFERR Rts_E;

  Rts_E = Bof_WaitForConditionalVariable(mChannelConnectedCv_X, _TimeoutInMs_U32, mChannelConnectedCvPredicateAndReset);
  // BOF_DBG_PRINTF("===@@ %p WaitForChannelConnected %d\n", &mChannelConnectedCv_X,Rts_E);

  return Rts_E;
}
BOFERR BofSocketIo::WaitForChannelDisconnected(uint32_t _TimeoutInMs_U32)
{
  BOFERR Rts_E;

  Rts_E = Bof_WaitForConditionalVariable(mChannelConnectedCv_X, _TimeoutInMs_U32, mChannelDisconnectedCvPredicateAndReset);
  // BOF_DBG_PRINTF("===@@ %p WaitForChannelDisconnected %d\n", &mChannelConnectedCv_X, Rts_E);

  return Rts_E;
}
END_BOF_NAMESPACE()
