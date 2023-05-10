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
 * Interprocess pipe are implement as socket binded on 127.0.0.1
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
  std::string PipeName_S;
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
      BofSocketParam_X.BindIpAddress_S = Bof_Sprintf("udp://127.0.0.1:%d", mPipeParam_X.SrcPortBase_U16);
      mErrorCode_E = BOF_ERR_ENOMEM;
      mpuUdpPipeMst = std::make_unique<BofSocket>(BofSocketParam_X);
      if (mpuUdpPipeMst)
      {
        mErrorCode_E = mpuUdpPipeMst->LastErrorCode();
        if (mErrorCode_E == BOF_ERR_NO_ERROR)
        {
          DstIpAddress = mpuUdpPipeMst->GetSrcIpAddress();
          DstIpAddress.Port(mPipeParam_X.DstPortBase_U16);
          mErrorCode_E = mpuUdpPipeMst->SetDstIpAddress(DstIpAddress);
          if (mErrorCode_E == BOF_ERR_NO_ERROR)
          {
            // printf("PipeIn: Src %s Dst %s\n", mpuUdpPipeMst->GetSrcIpAddress().ToString(true, true).c_str(), mpuUdpPipeMst->GetDstIpAddress().ToString(true, true).c_str());
          }
        }
      }

      if (mPipeParam_X.PipeAccess_E == BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ_WRITE)
      {
        if (mErrorCode_E == BOF_ERR_NO_ERROR)
        {
          if (mPipeParam_X.SrcPortBase_U16)
          {
            mPipeParam_X.SrcPortBase_U16++;
          }
          if (mPipeParam_X.DstPortBase_U16)
          {
            mPipeParam_X.DstPortBase_U16++;
          }
          BofSocketParam_X.BaseChannelParam_X.ChannelName_S = mPipeParam_X.BaseChannelParam_X.ChannelName_S + "_Out";
          BofSocketParam_X.BindIpAddress_S = Bof_Sprintf("udp://127.0.0.1:%d", mPipeParam_X.SrcPortBase_U16);
          mErrorCode_E = BOF_ERR_ENOMEM;
          mpuUdpPipeSlv = std::make_unique<BofSocket>(BofSocketParam_X);
          if (mpuUdpPipeSlv)
          {
            mErrorCode_E = mpuUdpPipeSlv->LastErrorCode();
            if (mErrorCode_E == BOF_ERR_NO_ERROR)
            {
              DstIpAddress = mpuUdpPipeSlv->GetSrcIpAddress();
              DstIpAddress.Port(mPipeParam_X.DstPortBase_U16);
              mErrorCode_E = mpuUdpPipeSlv->SetDstIpAddress(DstIpAddress);
              if (mErrorCode_E == BOF_ERR_NO_ERROR)
              {
                // printf("PipeOut: Src %s Dst %s\n", mpuUdpPipeSlv->GetSrcIpAddress().ToString(true, true).c_str(), mpuUdpPipeSlv->GetDstIpAddress().ToString(true, true).c_str());
              }
            }
          }
        }
      }
      break;

    case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
#if defined(_WIN32)
      DWORD OpenMode_DW, DesiredAccess_DW;
      mErrorCode_E = BOF_ERR_CREATE;
      PipeName_S = "\\\\.\\pipe\\" + mPipeParam_X.BaseChannelParam_X.ChannelName_S;
      switch (mPipeParam_X.PipeAccess_E)
      {
        case BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ:
          OpenMode_DW = PIPE_ACCESS_INBOUND;
          DesiredAccess_DW = GENERIC_READ;
          break;

        case BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_WRITE:
          OpenMode_DW = PIPE_ACCESS_OUTBOUND;
          DesiredAccess_DW = GENERIC_WRITE;
          break;

        case BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ_WRITE:
          OpenMode_DW = PIPE_ACCESS_DUPLEX;
          DesiredAccess_DW = GENERIC_READ | GENERIC_WRITE;
          break;

        default:
          OpenMode_DW = 0;
          DesiredAccess_DW = 0;
          break;
      }
      if (mPipeParam_X.PipeServer_B)
      {

        mPipe_h = CreateNamedPipe(PipeName_S.c_str(), OpenMode_DW, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | (mPipeParam_X.BaseChannelParam_X.Blocking_B ? 0 : PIPE_NOWAIT), 1, mPipeParam_X.BaseChannelParam_X.SndBufferSize_U32,
                                  mPipeParam_X.BaseChannelParam_X.RcvBufferSize_U32, 0, nullptr);
      }
      else
      {
        mPipe_h = CreateFile(PipeName_S.c_str(), DesiredAccess_DW, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
      }
      // printf("Srv %d Pipe file '%s' err %d\n", mPipeParam_X.Server_B, PipeName_S.c_str(), GetLastError());
      if (BOF_IS_HANDLE_VALID(mPipe_h))
      {
        mErrorCode_E = BOF_ERR_NO_ERROR;
      }
#else
      int Status_i, OpenFlag_i, CreateFlag_i;
      mErrorCode_E = BOF_ERR_CREATE;
      switch (mPipeParam_X.PipeAccess_E)
      {
        case BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ:
          OpenFlag_i = O_RDONLY;
          CreateFlag_i = S_IRUSR;
          break;

        case BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_WRITE:
          OpenFlag_i = O_WRONLY;
          CreateFlag_i = S_IWUSR;
          break;

        case BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ_WRITE:
          OpenFlag_i = O_RDWR;
          CreateFlag_i = S_IRWXU;
          break;

        default:
          OpenFlag_i = 0;
          CreateFlag_i = 0;
          break;
      }
      mPipe_i = -1;
      if (mPipeParam_X.PipeServer_B)
      {
        // Under linux a named event does not exist
        // Consequently, we're going to use a named
        // pipe instead
        Status_i = mkfifo(mPipeParam_X.BaseChannelParam_X.ChannelName_S.c_str(), CreateFlag_i);
        if ((Status_i == 0) || (Bof_GetLastError(false) == BOF_ERR_EEXIST))
        {
          // From man fifo(7)
          //
          // Under Linux, opening a FIFO for read and write will succeed both in
          // blocking and nonblocking mode. POSIX leaves this behavior undefined.
          // This can be used to open a FIFO for writing while there are no readers
          // available. A process that uses both ends of the connection in order to
          // communicate with itself should be very careful to avoid deadlocks.
          mPipe_i = open(mPipeParam_X.BaseChannelParam_X.ChannelName_S.c_str(), OpenFlag_i | (mPipeParam_X.BaseChannelParam_X.Blocking_B ? 0 : O_NONBLOCK));
        }
      }
      else
      {
        mPipe_i = open(mPipeParam_X.BaseChannelParam_X.ChannelName_S.c_str(), OpenFlag_i | (mPipeParam_X.BaseChannelParam_X.Blocking_B ? 0 : O_NONBLOCK));
      }
      if (mPipe_i >= 0)
      {
        mErrorCode_E = BOF_ERR_NO_ERROR;
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
  CloseHandle(mPipe_h);
#else
  close(mPipe_i);
#endif
}

// TODO: BOF_PIPE_ACCESS_READ_WRITE not finished...
BOFERR BofPipe::SelectPipeChannel(bool _Master_B) // Used by  BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ_WRITE
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if ((mPipeParam_X.PipeType_E == BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP) && (mPipeParam_X.PipeAccess_E == BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ_WRITE))
  {
    mFullDuplexUseMaster_B = _Master_B;
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

BOFERR BofPipe::V_FlushData(uint32_t _TimeoutInMs_U32)
{
  BOFERR Rts_E = BOF_ERR_INIT;
  switch (mPipeParam_X.PipeType_E)
  {
    case BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP:
      if (mpuUdpPipeMst)
      {
        Rts_E = mFullDuplexUseMaster_B ? mpuUdpPipeMst->V_FlushData(_TimeoutInMs_U32) : mpuUdpPipeSlv->V_FlushData(_TimeoutInMs_U32);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          Rts_E = mpuUdpPipeSlv->V_FlushData(_TimeoutInMs_U32);
        }
      }
      break;

    case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
      Rts_E = BOF_ERR_NO_ERROR;
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
      if (mpuUdpPipeMst)
      {
        Rts_E = mFullDuplexUseMaster_B ? mpuUdpPipeMst->V_WaitForDataToRead(_TimeoutInMs_U32, _rNbPendingByte_U32) : mpuUdpPipeSlv->V_WaitForDataToRead(_TimeoutInMs_U32, _rNbPendingByte_U32);
      }
      break;

    case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
      Rts_E = BOF_ERR_NO_ERROR;
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
      if (mpuUdpPipeMst)
      {
        Rts_E = mFullDuplexUseMaster_B ? mpuUdpPipeMst->V_GetStatus(_rStatus_X) : mpuUdpPipeSlv->V_GetStatus(_rStatus_X);
        BOF_ASSERT(_rStatus_X.NbOut_U32 == 0);
      }
      break;

    case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
      Rts_E = BOF_ERR_NO_ERROR;
      break;

    default:
      Rts_E = BOF_ERR_INIT;
      break;
  }

  return Rts_E;
}

BOFERR BofPipe::V_ReadData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, uint8_t *_pBuffer_U8)
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;
#if defined(_WIN32)
  DWORD NumBytesRead_DW = 0;
#else
  int Nb_i = 0;
#endif

  if ((mPipeParam_X.PipeAccess_E == BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ) || (mPipeParam_X.PipeAccess_E == BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_WRITE))
  {
    Rts_E = BOF_ERR_INIT;
    switch (mPipeParam_X.PipeType_E)
    {
      case BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP:
        if (mpuUdpPipeMst)
        {
          Rts_E = mFullDuplexUseMaster_B ? mpuUdpPipeMst->V_ReadData(_TimeoutInMs_U32, _rNb_U32, _pBuffer_U8) : mpuUdpPipeSlv->V_ReadData(_TimeoutInMs_U32, _rNb_U32, _pBuffer_U8);
          // printf("PipeRead: Src %s Dst %s %d:%p %s\n", mpuUdpPipeMst->GetSrcIpAddress().ToString(true, true).c_str(), mpuUdpPipeMst->GetDstIpAddress().ToString(true, true).c_str(), _rNb_U32, _pBuffer_U8, Bof_ErrorCode(Rts_E));
        }
        break;

      case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
#if defined(_WIN32)
        Rts_E = BOF_ERR_READ;
        if (ReadFile(mPipe_h, _pBuffer_U8, _rNb_U32, &NumBytesRead_DW, nullptr))
        {
          Rts_E = BOF_ERR_NO_ERROR;
        }
        _rNb_U32 = NumBytesRead_DW;
#else
        if (mPipe_i >= 0)
        {
          Rts_E = BOF_ERR_READ;
          Nb_i = static_cast<int>(read(mPipe_i, _pBuffer_U8, _rNb_U32));
          if (Nb_i >= 0)
          {
            Rts_E = BOF_ERR_NO_ERROR;
          }
          else
          {
            Nb_i = 0;
          }
        }
        _rNb_U32 = Nb_i;
#endif
        break;

      default:
        Rts_E = BOF_ERR_INIT;
        break;
    }
  }
  return Rts_E;
}

BOFERR BofPipe::V_WriteData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, const uint8_t *_pBuffer_U8)
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;
#if defined(_WIN32)
  DWORD NumBytesWritten_DW = 0;
#else
  int Nb_i = 0;
#endif
  if ((mPipeParam_X.PipeAccess_E == BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_WRITE) || (mPipeParam_X.PipeAccess_E == BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_WRITE))
  {
    Rts_E = BOF_ERR_INIT;
    switch (mPipeParam_X.PipeType_E)
    {
      case BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP:
        if (mpuUdpPipeMst)
        {
          Rts_E = mFullDuplexUseMaster_B ? mpuUdpPipeMst->V_WriteData(_TimeoutInMs_U32, _rNb_U32, _pBuffer_U8) : mpuUdpPipeSlv->V_WriteData(_TimeoutInMs_U32, _rNb_U32, _pBuffer_U8);
          // printf("PipeWrite: Src %s Dst %s %d:%p %s\n", mpuUdpPipeMst->GetSrcIpAddress().ToString(true, true).c_str(), mpuUdpPipeMst->GetDstIpAddress().ToString(true, true).c_str(), _rNb_U32, _pBuffer_U8, Bof_ErrorCode(Rts_E));
        }
        break;
      case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
#if defined(_WIN32)
        Rts_E = BOF_ERR_WRITE;
        if (WriteFile(mPipe_h, _pBuffer_U8, _rNb_U32, &NumBytesWritten_DW, nullptr))
        {
          Rts_E = BOF_ERR_NO_ERROR;
        }
        _rNb_U32 = NumBytesWritten_DW;
#else
        if (mPipe_i >= 0)
        {
          Rts_E = BOF_ERR_WRITE;
          Nb_i = static_cast<int>(write(mPipe_i, _pBuffer_U8, _rNb_U32));
          // if (static_cast<uint32_t>(Nb_i) == _rNb_U32)
          if (Nb_i >= 0)
          {
            Rts_E = BOF_ERR_NO_ERROR;
          }
          else
          {
            Nb_i = 0;
          }
        }
        _rNb_U32 = Nb_i;
#endif
        break;

      default:
        Rts_E = BOF_ERR_INIT;
        break;
    }
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
      if (mpuUdpPipeMst)
      {
        Rts_E = BOF_ERR_NO_ERROR;
        //      Rts_E = mpuUdpPipe->V_Connect(_TimeoutInMs_U32, _rTarget_S, _rOption_S);
      }
      break;
    case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
#if defined(_WIN32)
      if (mPipeParam_X.PipeServer_B)
      {
        bool Connected_B;
        // This call blocks until a client process connects to the pipe
        // Wait for the client to connect; if it succeeds,
        // the function returns a nonzero value. If the function
        // returns zero, GetLastError returns ERROR_PIPE_CONNECTED.

        Connected_B = ConnectNamedPipe(mPipe_h, nullptr) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);
        Rts_E = BOF_ERR_ECONNREFUSED;
        if (Connected_B)
        {
          Rts_E = BOF_ERR_NO_ERROR;
        }
      }
      else
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
      // printf("Srv %d err %d rts %d\n", mPipeParam_X.Server_B, GetLastError(), Rts_E);

#else
      if (mPipe_i >= 0)
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
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

  return pRts;
}
END_BOF_NAMESPACE()