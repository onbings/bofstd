/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module contains routines for creating and managing a pipe communication
 * channel.
 *
 * Name:        bofpipe.cpp
 * Author:      Bernard HARMEL: b.harmel@belgacom.net
 * Revision:    1.0
 *
 * Rem:
 * Interprocess pipe are implement as ZMQ_PAIT socket binded on 127.0.0.1
 * The port value is specified in BOF_COM_CHANNEL_PARAM.ChannelName_S and the pipe size is the sum
 * of BOF_COM_CHANNEL_PARAM.RcvBufferSize_U32 and BOF_COM_CHANNEL_PARAM.SndBufferSize_U32
 *
 * History:
 *
 * V 1.00  Oct 21 2002  BHA : Initial release
 */
#include <bofstd/bofpipe.h>
#include <bofstd/bofstring.h>
#if defined(_WIN32)
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif
BEGIN_BOF_NAMESPACE()

BofPipe::BofPipe(const BOF_PIPE_PARAM &_rPipeParam_X) : BofComChannel(BOF_COM_CHANNEL_TYPE::TPIPE, mPipeParam_X.BaseChannelParam_X)
{
  BOF_SOCKET_PARAM BofSocketParam_X;
  BOF_SOCKET_ADDRESS DstIpAddress;

  mPipeParam_X = _rPipeParam_X;

  switch (mPipeParam_X.PipeType_E)
  {
  case BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP:
    BofSocketParam_X.BaseChannelParam_X.ChannelName_S = mPipeParam_X.BaseChannelParam_X.ChannelName_S + "_In";
    BofSocketParam_X.BaseChannelParam_X.Blocking_B = true;
    BofSocketParam_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
    BofSocketParam_X.BaseChannelParam_X.RcvBufferSize_U32 = mPipeParam_X.BaseChannelParam_X.RcvBufferSize_U32;
    BofSocketParam_X.BaseChannelParam_X.SndBufferSize_U32 = mPipeParam_X.BaseChannelParam_X.SndBufferSize_U32;
    BofSocketParam_X.NoDelay_B = true;
    BofSocketParam_X.ReUseAddress_B = true;
    BofSocketParam_X.BindIpAddress_S = Bof_Sprintf("udp://127.0.0.1:%d", mPipeParam_X.PortInBase_U16);
    mErrorCode_E = BOF_ERR_ENOMEM;
    mpuUdpPipeIn = std::make_unique<BofSocket>(BofSocketParam_X);
    if (mpuUdpPipeIn)
    {
      mErrorCode_E = mpuUdpPipeIn->LastErrorCode();
      if (mErrorCode_E == BOF_ERR_NO_ERROR)
      {
        DstIpAddress = mpuUdpPipeIn->GetSrcIpAddress();
        mErrorCode_E = mpuUdpPipeIn->SetDstIpAddress(DstIpAddress);
        if (mErrorCode_E == BOF_ERR_NO_ERROR)
        {
        }
      }
    }

    if (mErrorCode_E == BOF_ERR_NO_ERROR)
    {
      if (mPipeParam_X.PortInBase_U16)
      {
        mPipeParam_X.PortInBase_U16++;
      }
      BofSocketParam_X.BaseChannelParam_X.ChannelName_S = mPipeParam_X.BaseChannelParam_X.ChannelName_S + "_Out";
      BofSocketParam_X.BindIpAddress_S = Bof_Sprintf("udp://127.0.0.1:%d", mPipeParam_X.PortInBase_U16);
      mErrorCode_E = BOF_ERR_ENOMEM;
      mpuUdpPipeOut = std::make_unique<BofSocket>(BofSocketParam_X);
      if (mpuUdpPipeIn)
      {
        mErrorCode_E = mpuUdpPipeOut->LastErrorCode();
        if (mErrorCode_E == BOF_ERR_NO_ERROR)
        {
          DstIpAddress = mpuUdpPipeOut->GetSrcIpAddress();
          mErrorCode_E = mpuUdpPipeOut->SetDstIpAddress(DstIpAddress);
          if (mErrorCode_E == BOF_ERR_NO_ERROR)
          {
          }
        }
      }
    }
    break;

  case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
#if defined(_WIN32)
#else
    int Status_i;

    // Under linux a named event does not exist
    // Consequently, we're going to use a named
    // pipe instead
    Status_i = mkfifo("CDX_EVENT_NAME", S_IRWXU);

    if ((Status_i == 0) || (Bof_GetLastError(false) == BOF_ERR_EEXIST))
    {
      // From man fifo(7)
      //
      // Under Linux, opening a FIFO for read and write will succeed both in
      // blocking and nonblocking mode. POSIX leaves this behavior undefined.
      // This can be used to open a FIFO for writing while there are no readers
      // available. A process that uses both ends of the connection in order to
      // communicate with itself should be very careful to avoid deadlocks.
      mPipe_i = open("CDX_EVENT_NAME", O_RDWR | O_NONBLOCK /*O_WRONLY*/);
    }
    else
    {
      //      LOGC("[Main] >> Unable to create fifo (Status %d Error 0x%08X)\r\n", Status_i, Bof_GetLastError(false));
    }

    BOF_ASSERT(mPipe_i >= 0);

    if (mPipe_i >= 0)
    {
    }
#endif
    break;

  default:
    mErrorCode_E = BOF_ERR_EINVAL;
    break;
  }
}

BofPipe::~BofPipe()
{
#if defined(_WIN32)

#else
  close(mPipe_i);
#endif
}

BOFERR BofPipe::V_FlushData(uint32_t _TimeoutInMs_U32)
{
  BOFERR Rts_E = BOF_ERR_INIT;
  switch (mPipeParam_X.PipeType_E)
  {
  case BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP:
    if ((mpuUdpPipeIn) && (mpuUdpPipeOut))
    {
      Rts_E = mpuUdpPipeIn->V_FlushData(_TimeoutInMs_U32);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Rts_E = mpuUdpPipeOut->V_FlushData(_TimeoutInMs_U32);
      }
    }
    break;

  default:
    Rts_E = BOF_ERR_INIT;
    break;
  }

  return Rts_E;
}
BOFERR BofPipe::V_WaitForDataToRead(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPendingByte_U32)
{
  BOFERR Rts_E = BOF_ERR_INIT;
  switch (mPipeParam_X.PipeType_E)
  {
  case BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP:
    if ((mpuUdpPipeIn) && (mpuUdpPipeOut))
    {
      Rts_E = mpuUdpPipeIn->V_WaitForDataToRead(_TimeoutInMs_U32, _rNbPendingByte_U32);
    }
    break;

  default:
    Rts_E = BOF_ERR_INIT;
    break;
  }

  return Rts_E;
}

BOFERR BofPipe::V_GetStatus(BOF_COM_CHANNEL_STATUS &_rStatus_X)
{
  BOFERR Rts_E = BOF_ERR_INIT;
  BOF_COM_CHANNEL_STATUS Status_X;

  switch (mPipeParam_X.PipeType_E)
  {
  case BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP:
    if ((mpuUdpPipeIn) && (mpuUdpPipeOut))
    {
      Rts_E = mpuUdpPipeIn->V_GetStatus(_rStatus_X);
      BOF_ASSERT(_rStatus_X.NbOut_U32 == 0);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Rts_E = mpuUdpPipeOut->V_GetStatus(Status_X);
        BOF_ASSERT(Status_X.NbIn_U32 == 0);
        _rStatus_X.NbOut_U32 = Status_X.NbOut_U32;
      }
    }
    break;

  default:
    Rts_E = BOF_ERR_INIT;
    break;
  }

  return Rts_E;
}

BOFERR BofPipe::V_ReadData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, uint8_t *_pBuffer_U8)
{
  BOFERR Rts_E = BOF_ERR_INIT;
  switch (mPipeParam_X.PipeType_E)
  {
  case BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP:
    if ((mpuUdpPipeIn) && (mpuUdpPipeOut))
    {
      Rts_E = mpuUdpPipeIn->V_ReadData(_TimeoutInMs_U32, _rNb_U32, _pBuffer_U8);
    }
    break;

  case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
#if defined(_WIN32)
#else
    int Nb_i;
    if (mPipe_i >= 0)
    {
      Rts_E = BOF_ERR_READ;
      _rNb_U32 = 0;
      Nb_i = static_cast<int>(read(mPipe_i, _pBuffer_U8, _rNb_U32));
      if (Nb_i > 0)
      {
        _rNb_U32 = Nb_i;
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
#endif
    break;

  default:
    Rts_E = BOF_ERR_INIT;
    break;
  }

  return Rts_E;
}

BOFERR BofPipe::V_WriteData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, const uint8_t *_pBuffer_U8)
{
  BOFERR Rts_E = BOF_ERR_INIT;
  switch (mPipeParam_X.PipeType_E)
  {
  case BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP:
    if ((mpuUdpPipeIn) && (mpuUdpPipeOut))
    {
      Rts_E = mpuUdpPipeOut->V_WriteData(_TimeoutInMs_U32, _rNb_U32, _pBuffer_U8);
    }
    break;

  case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
#if defined(_WIN32)
#else
    int Nb_i;
    if (mPipe_i >= 0)
    {
      Rts_E = BOF_ERR_WRITE;
      Nb_i = static_cast<int>(write(mPipe_i, _pBuffer_U8, _rNb_U32));
      if (static_cast<uint32_t>(Nb_i) == _rNb_U32)
      {
        _rNb_U32 = Nb_i;
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
#endif
    break;

  default:
    Rts_E = BOF_ERR_INIT;
    break;
  }

  return Rts_E;
}

BOFERR BofPipe::V_WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S, uint32_t &_rNb_U32)
{
  _rNb_U32 = static_cast<uint32_t>(_rBuffer_S.size());
  return V_WriteData(_TimeoutInMs_U32, _rNb_U32, reinterpret_cast<const uint8_t *>(_rBuffer_S.c_str()));
}
BOFERR BofPipe::V_Connect(uint32_t /*_TimeoutInMs_U32*/, const std::string & /*_rTarget_S*/, const std::string & /*_rOption_S*/)
{
  BOFERR Rts_E = BOF_ERR_INIT;
  switch (mPipeParam_X.PipeType_E)
  {
  case BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP:
    if ((mpuUdpPipeIn) && (mpuUdpPipeOut))
    {
      Rts_E = BOF_ERR_NOT_SUPPORTED;
      //      Rts_E = mpuUdpPipe->V_Connect(_TimeoutInMs_U32, _rTarget_S, _rOption_S);
    }
    break;

  case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
#if defined(_WIN32)
#else

#endif
    break;
  default:
    Rts_E = BOF_ERR_INIT;
    break;
  }

  return Rts_E;
}

BofComChannel *BofPipe::V_Listen(uint32_t /*_TimeoutInMs_U32*/, const std::string & /*_rOption_S*/)
{
  BofComChannel *pRts = nullptr;
  switch (mPipeParam_X.PipeType_E)
  {
  case BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP:
    if ((mpuUdpPipeIn) && (mpuUdpPipeOut))
    {
      //      Rts_E = BOF_ERR_NOT_SUPPORTED;
      //     pRts = mpuUdpPipe->V_Listen(_TimeoutInMs_U32, _rOption_S);
    }
    break;

  default:
    pRts = nullptr;
    break;
  }

  return pRts;
}
END_BOF_NAMESPACE()