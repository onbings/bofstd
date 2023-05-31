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
#include <bofstd/boffs.h>
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

BofPipe::BofPipe(const BOF_PIPE_PARAM &_rPipeParam_X)
    : BofComChannel(BOF_COM_CHANNEL_TYPE::TPIPE, mPipeParam_X.BaseChannelParam_X)
{
  BOF_SOCKET_PARAM BofSocketParam_X;
  BOF_SOCKET_ADDRESS DstIpAddress;
  mPipeParam_X = _rPipeParam_X;

  switch (mPipeParam_X.PipeType_E)
  {
    case BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP:
      BofSocketParam_X.BaseChannelParam_X.ChannelName_S = mPipeParam_X.BaseChannelParam_X.ChannelName_S + "_In";
      BofSocketParam_X.BaseChannelParam_X.Blocking_B = mPipeParam_X.BaseChannelParam_X.Blocking_B;
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
            // printf("PipeIn: Src %s Dst %s\n", mpuUdpPipeMst->GetSrcIpAddress().ToString(true, true).c_str(),
            // mpuUdpPipeMst->GetDstIpAddress().ToString(true, true).c_str());
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
                // printf("PipeOut: Src %s Dst %s\n", mpuUdpPipeSlv->GetSrcIpAddress().ToString(true,
                // true).c_str(), mpuUdpPipeSlv->GetDstIpAddress().ToString(true, true).c_str());
              }
            }
          }
        }
      }
      break;

    case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
#if defined(_WIN32)
      DWORD OpenMode_DW, PipeMode_DW;
      mErrorCode_E = BOF_ERR_CREATE;
      mPipeName_S = "\\\\.\\pipe\\" + mPipeParam_X.BaseChannelParam_X.ChannelName_S;
      switch (mPipeParam_X.PipeAccess_E)
      {
        case BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ:
          OpenMode_DW = PIPE_ACCESS_INBOUND;
          mDesiredAccess_DW = GENERIC_READ;
          break;

        case BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_WRITE:
          OpenMode_DW = PIPE_ACCESS_OUTBOUND;
          mDesiredAccess_DW = GENERIC_WRITE;
          break;

        case BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ_WRITE:
          OpenMode_DW = PIPE_ACCESS_DUPLEX;
          mDesiredAccess_DW = GENERIC_READ | GENERIC_WRITE;
          break;

        default:
          OpenMode_DW = 0;
          mDesiredAccess_DW = 0;
          break;
      }
      if (mPipeParam_X.PipeServer_B)
      {
        PipeMode_DW = mPipeParam_X.NativeStringMode_B ? (PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE) : (PIPE_TYPE_BYTE | PIPE_READMODE_BYTE);
        PipeMode_DW |= (mPipeParam_X.BaseChannelParam_X.Blocking_B ? PIPE_WAIT : PIPE_NOWAIT);
        mPipe_h = CreateNamedPipe(mPipeName_S.c_str(), OpenMode_DW, PipeMode_DW, 1, mPipeParam_X.BaseChannelParam_X.SndBufferSize_U32, mPipeParam_X.BaseChannelParam_X.RcvBufferSize_U32, 0, nullptr);
        if (BOF_IS_HANDLE_VALID(mPipe_h))
        {
          mErrorCode_E = BOF_ERR_NO_ERROR;
        }
      }
      else
      {
        // Done in connect as server perhaps is not yet created
        //         mPipe_h = CreateFile(PipeName_S.c_str(), DesiredAccess_DW, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        mErrorCode_E = BOF_ERR_NO_ERROR;
      }
      // printf("Srv %d Pipe file '%s' err %d\n", mPipeParam_X.Server_B, PipeName_S.c_str(), GetLastError());

#else
      int Status_i, OpenFlag_i, CreateFlag_i;
      long Size;
      mErrorCode_E = BOF_ERR_CREATE;

      switch (mPipeParam_X.PipeAccess_E)
      {
        case BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ:
          OpenFlag_i = O_RDONLY;
          CreateFlag_i = S_IRWXU;
          break;

        case BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_WRITE:
          OpenFlag_i = O_RDWR;
          CreateFlag_i = S_IRWXU;
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
        Status_i = unlink(mPipeParam_X.BaseChannelParam_X.ChannelName_S.c_str());
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
          mPipe_i = open(mPipeParam_X.BaseChannelParam_X.ChannelName_S.c_str(),
                         OpenFlag_i | O_NONBLOCK); // OpenFlag_i | (mPipeParam_X.BaseChannelParam_X.Blocking_B ? 0 : O_NONBLOCK));
        }
      }
      else
      {
        mPipe_i = open(mPipeParam_X.BaseChannelParam_X.ChannelName_S.c_str(), OpenFlag_i | (mPipeParam_X.BaseChannelParam_X.Blocking_B ? 0 : O_NONBLOCK));
      }
      if (mPipe_i >= 0)
      {
        // Default is 64KB
        if ((mPipeParam_X.BaseChannelParam_X.RcvBufferSize_U32) || (mPipeParam_X.BaseChannelParam_X.SndBufferSize_U32))
        {
          mErrorCode_E = BOF_ERR_WRONG_SIZE;
          Size = (long)fcntl(mPipe_i, F_GETPIPE_SZ);
          Status_i = fcntl(mPipe_i, F_SETPIPE_SZ, (mPipeParam_X.BaseChannelParam_X.RcvBufferSize_U32 + mPipeParam_X.BaseChannelParam_X.SndBufferSize_U32));
          if (Status_i >= 0)
          {
            Size = (long)fcntl(mPipe_i, F_GETPIPE_SZ);
            // Min seems to be 4KB
            if (Size >= (mPipeParam_X.BaseChannelParam_X.RcvBufferSize_U32 + mPipeParam_X.BaseChannelParam_X.SndBufferSize_U32))
            {
              mErrorCode_E = BOF_ERR_NO_ERROR;
            }
          }
        }
        else
        {
          mErrorCode_E = BOF_ERR_NO_ERROR;
        }
        if (mErrorCode_E != BOF_ERR_NO_ERROR)
        {
          close(mPipe_i);
          mPipe_i = -1;
        }
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
#if defined(_WIN32)
#else
  int Nb_i, Sts_i;
  struct pollfd pFds_X[4];
#endif

  _rNbPendingByte_U32 = 0;
  switch (mPipeParam_X.PipeType_E)
  {
    case BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UDP:
      if (mpuUdpPipeMst)
      {
        Rts_E = mFullDuplexUseMaster_B ? mpuUdpPipeMst->V_WaitForDataToRead(_TimeoutInMs_U32, _rNbPendingByte_U32) : mpuUdpPipeSlv->V_WaitForDataToRead(_TimeoutInMs_U32, _rNbPendingByte_U32);
      }
      break;

    case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
#if defined(_WIN32)
      if (BOF_IS_HANDLE_VALID(mPipe_h))
      {
        _rNbPendingByte_U32 = 1;
        Rts_E = BOF_ERR_NO_ERROR;
#else
      if (mPipe_i >= 0)
      {
        Rts_E = BOF_ERR_ETIMEDOUT;
        pFds_X[0].fd = mPipe_i;
        pFds_X[0].events = (POLLIN);
        if (poll(pFds_X, 1, _TimeoutInMs_U32) == 1)
        {
          int Nb_i, Sts_i;
          BOF_IOCTL(mPipe_i, FIONREAD, sizeof(Nb_i), &Nb_i, 0, nullptr, Sts_i);
          Rts_E = (Sts_i >= 0) ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            _rNbPendingByte_U32 = (uint32_t)Nb_i;
          }
        }
#endif
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

  _rStatus_X.Reset();
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
#if defined(_WIN32)
#if 1
      _rStatus_X.NbIn_U32 = 1;
      Rts_E = BOF_ERR_NO_ERROR;
#else
      DWORD Error_DW;
      COMSTAT ComStat_X;
      Rts_E = (ClearCommError(mPipe_h, &Error_DW, &ComStat_X)) ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        _rStatus_X.NbIn_U32 = (uint32_t)ComStat_X.cbInQue;
        _rStatus_X.NbOut_U32 = (uint32_t)ComStat_X.cbOutQue;
      }
#endif
#else
      int Nb_i, Sts_i;
      BOF_IOCTL(mPipe_i, FIONREAD, sizeof(Nb_i), &Nb_i, 0, nullptr, Sts_i);
      Rts_E = (Sts_i >= 0) ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        _rStatus_X.NbIn_U32 = (uint32_t)Nb_i;
      }
#endif
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
  uint32_t i_U32, NbPendingByte_U32;
#if defined(_WIN32)
  DWORD NumBytesRead_DW = 0;
  uint32_t Start_U32;
#else
  int Nb_i = 0;
  uint32_t Len_U32;
  char c_c;
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
          // printf("PipeRead: Src %s Dst %s %d:%p %s\n", mpuUdpPipeMst->GetSrcIpAddress().ToString(true,
          // true).c_str(), mpuUdpPipeMst->GetDstIpAddress().ToString(true, true).c_str(), _rNb_U32, _pBuffer_U8,
          // Bof_ErrorCode(Rts_E));
        }
        break;

      case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
#if defined(_WIN32)
        Rts_E = BOF_ERR_READ;
        // mPipeParam_X.NativeStringMode_B: naturally supported as pipe is created in this case with
        // (PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE)
        Start_U32 = BOF::Bof_GetMsTickCount();
        do
        {
          if (ReadFile(mPipe_h, _pBuffer_U8, _rNb_U32, &NumBytesRead_DW, nullptr))
          {
            Rts_E = BOF_ERR_NO_ERROR;
          }
          if ((Rts_E != BOF_ERR_NO_ERROR) && (_TimeoutInMs_U32))
          {
            BOF::Bof_MsSleep(50);
          }
        } while ((Rts_E != BOF_ERR_NO_ERROR) && (BOF::Bof_ElapsedMsTime(Start_U32) < _TimeoutInMs_U32));

        _rNb_U32 = NumBytesRead_DW;
#else
        if (mPipe_i >= 0)
        {
          Rts_E = V_WaitForDataToRead(_TimeoutInMs_U32, NbPendingByte_U32);
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            if (mPipeParam_X.NativeStringMode_B)
            {
              Rts_E = BOF_ERR_EINVAL;
              if ((_pBuffer_U8) && (_rNb_U32))
              {
                // Does not work on pipe handle (seek)            Rts_E = Bof_ReadLine(mPipe_i, _rNb_U32,
                // (char
                // *)_pBuffer_U8);
                Len_U32 = _rNb_U32 - 1;
                for (i_U32 = 0; i_U32 < Len_U32; i_U32++)
                {
                  if (read(mPipe_i, &c_c, 1) > 0)
                  {
                    _pBuffer_U8[i_U32] = c_c;
                    if (c_c == '\n')
                    {
                      i_U32++;
                      break;
                    }
                  }
                  else
                  {
                    break;
                  }
                }
                _pBuffer_U8[i_U32] = 0;
                Nb_i = i_U32;
              }
            }
            else
            {
              Nb_i = static_cast<int>(read(mPipe_i, _pBuffer_U8, _rNb_U32));
            }
            if (Nb_i > 0)
            {
              Rts_E = BOF_ERR_NO_ERROR;
            }
            else
            {
              Nb_i = 0;
            }
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
          // printf("PipeWrite: Src %s Dst %s %d:%p %s\n", mpuUdpPipeMst->GetSrcIpAddress().ToString(true,
          // true).c_str(), mpuUdpPipeMst->GetDstIpAddress().ToString(true, true).c_str(), _rNb_U32, _pBuffer_U8,
          // Bof_ErrorCode(Rts_E));
        }
        break;
      case BOF_PIPE_TYPE::BOF_PIPE_NATIVE:
        if (mPipeParam_X.NativeStringMode_B)
        {
          Rts_E = BOF_ERR_EINVAL;
          if ((_pBuffer_U8) && (_rNb_U32))
          {
            Rts_E = (_pBuffer_U8[_rNb_U32 - 1] == '\n') ? BOF_ERR_NO_ERROR : BOF_ERR_FORMAT;
          }
        }
        else
        {
          Rts_E = BOF_ERR_NO_ERROR;
        }
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
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
            if (Nb_i > 0)
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
        }
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
BOFERR BofPipe::V_Connect(uint32_t _TimeoutInMs_U32, const std::string & /*_rTarget_S*/, const std::string & /*_rOption_S*/)
{
  BOFERR Rts_E = BOF_ERR_INIT;
#if defined(_WIN32)
  uint32_t Start_U32;
#endif
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
      Rts_E = BOF_ERR_ECONNREFUSED;
      // Done in connect as server perhaps is not yet created when constructor is called
      Start_U32 = BOF::Bof_GetMsTickCount();
      do
      {
        if (mPipeParam_X.PipeServer_B)
        {
          bool Connected_B;
          // This call blocks until a client process connects to the pipe
          // Wait for the client to connect; if it succeeds,
          // the function returns a nonzero value. If the function
          // returns zero, GetLastError returns ERROR_PIPE_CONNECTED.

          Connected_B = ConnectNamedPipe(mPipe_h, nullptr) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);
          if (Connected_B)
          {
            Rts_E = BOF_ERR_NO_ERROR;
          }
        }
        else
        {
          mPipe_h = CreateFile(mPipeName_S.c_str(), mDesiredAccess_DW, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
          if (BOF_IS_HANDLE_VALID(mPipe_h))
          {
            Rts_E = BOF_ERR_NO_ERROR;
          }
        }
        if ((Rts_E != BOF_ERR_NO_ERROR) && (_TimeoutInMs_U32))
        {
          BOF::Bof_MsSleep(50);
        }
      } while ((Rts_E != BOF_ERR_NO_ERROR) && (BOF::Bof_ElapsedMsTime(Start_U32) < _TimeoutInMs_U32));

      // printf("%s err %d rts %d\n", mPipeParam_X.PipeServer_B ? "PipeSrv":"PipeClt", GetLastError(), Rts_E);
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