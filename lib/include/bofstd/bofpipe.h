/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines routines for creating and managing a pipe communication
 * channel.
 *
 * Name:        BofPipe.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:
 * The Windows Ce version is not fully operationnal
 *
 * History:
 *
 * V 1.00  Oct 21 2002  BHA : Initial release
 */
#pragma once

#include <bofstd/bofcomchannel.h>
#include <bofstd/bofsocket.h>

BEGIN_BOF_NAMESPACE()

enum class BOF_PIPE_TYPE : uint32_t
{
  BOF_PIPE_OVER_LOCAL_UNKNOWN = 0,
  BOF_PIPE_OVER_LOCAL_UDP,
  BOF_PIPE_NATIVE,
};
enum class BOF_PIPE_ACCESS : uint32_t
{
  BOF_PIPE_ACCESS_READ = 0,     //For server or client in non full duplex mode, one should use read and the other write
  BOF_PIPE_ACCESS_WRITE,        //For server or client in non full duplex mode, one should use read and the other write
  BOF_PIPE_ACCESS_READ_WRITE,   //TODO: Not finished/validated For Full duplex 
};
struct BOF_PIPE_PARAM
{
  BOF_PIPE_TYPE PipeType_E;
  bool PipeServer_B;
  bool NativeStringMode_B;  //BOF_PIPE_NATIVE: if true message should be delimitted by '\n. In this case, the caller must provide a \n terminated buffer to the write function
  BOF_PIPE_ACCESS PipeAccess_E;
  BOF_COM_CHANNEL_PARAM BaseChannelParam_X; // Base properties of each channel
  uint16_t SrcPortBase_U16;                  //for BOF_PIPE_OVER_LOCAL_UDP, local Port for udp in pipe->if 0 let os decide, otherwise use this value for port in and this value+1 for port out
  uint16_t DstPortBase_U16;                  

  BOF_PIPE_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    PipeType_E = BOF_PIPE_TYPE::BOF_PIPE_OVER_LOCAL_UNKNOWN;
    PipeServer_B = false;
    NativeStringMode_B = false;
    PipeAccess_E = BOF_PIPE_ACCESS::BOF_PIPE_ACCESS_READ;
    BaseChannelParam_X.Reset();
    SrcPortBase_U16 = 0;
    DstPortBase_U16 = 0;
  }
};

class BOFSTD_EXPORT BofPipe : public BofComChannel
{
public:
  BofPipe(const BOF_PIPE_PARAM &_rPipeParam_X);
  ~BofPipe();

  BOFERR V_Connect(uint32_t _TimeoutInMs_U32, const std::string &_rTarget_S, const std::string &_rOption_S) override;
  BofComChannel *V_Listen(uint32_t _TimeoutInMs_U32, const std::string &_rOption_S) override;
  BOFERR V_ReadData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, uint8_t *pBuffer_U8) override;
  BOFERR V_WriteData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, const uint8_t *pBuffer_U8) override;
  BOFERR V_WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S, uint32_t &_rNb_U32) override;
  BOFERR V_GetStatus(BOF_COM_CHANNEL_STATUS &_rStatus_X) override;
  BOFERR V_FlushData(uint32_t _TimeoutInMs_U32) override;
  BOFERR V_WaitForDataToRead(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPendingByte_U32) override;
  BOFERR SelectPipeChannel(bool _Master_B); //Used by BOF_PIPE_ACCESS_READ_WRITE

  BofPipe &operator=(const BofPipe &) = delete; // Disallow copying
  BofPipe(const BofPipe &) = delete;

private:
  BOF_PIPE_PARAM mPipeParam_X;
  std::unique_ptr<BofSocket> mpuUdpPipeMst = nullptr;
  std::unique_ptr<BofSocket> mpuUdpPipeSlv = nullptr;
  bool mFullDuplexUseMaster_B = true;   //to be able to read and write from both side
#if defined(_WIN32)
  HANDLE mPipe_h = 0;
#else
  int mPipe_i = -1;
#endif
};
END_BOF_NAMESPACE()