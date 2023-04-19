/*!
  Copyright (c) 2008-2020, Evs. All rights reserved.

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  History:

  V 1.00  February 18 2014  NMA : Initial release
*/
#include <bofstd/bofsocketthread.h>

BEGIN_BOF_NAMESPACE()
constexpr uint32_t PUSH_POP_TIMEOUT = 1000;

BofSocketThread::BofSocketThread(const BOF_SOCKET_THREAD_PARAM &_rSocketThreadParam_X)
{
  BOF_CIRCULAR_BUFFER_PARAM CircularBufferParam_X;

  mSocketThreadParam_X = _rSocketThreadParam_X;
  CircularBufferParam_X.Blocking_B = true;
  CircularBufferParam_X.MultiThreadAware_B = true;
  CircularBufferParam_X.NbMaxElement_U32 = mSocketThreadParam_X.NbMaxOperationPending_U32;
  CircularBufferParam_X.Overwrite_B = false;
  CircularBufferParam_X.PopLockMode_B = false;
  mpuSocketOperationParamCollection = std::make_unique<BofCircularBuffer<BOF_SOCKET_OPERATION_PARAM>>(CircularBufferParam_X);
  if ((mpuSocketOperationParamCollection) && (mpuSocketOperationParamCollection->LastErrorCode() == BOF_ERR_NO_ERROR))
  {
    mpuSocketOperationResultCollection = std::make_unique<BofCircularBuffer<BOF_SOCKET_OPERATION_RESULT>>(CircularBufferParam_X);
    if ((mpuSocketOperationResultCollection) && (mpuSocketOperationParamCollection->LastErrorCode() == BOF_ERR_NO_ERROR))
    {
      LaunchBofProcessingThread(mSocketThreadParam_X.Name_S, false, 0, mSocketThreadParam_X.ThreadSchedulerPolicy_E, mSocketThreadParam_X.ThreadPriority_E, 0, 2000, 0);
    }
  }
}

BofSocketThread::~BofSocketThread()
{
}

BOFERR BofSocketThread::ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_LISTEN_PARAM &_rParam_X, uint32_t &_rOpTicket_U32)
{
  BOFERR Rts_E;
  BOF_SOCKET_OPERATION_PARAM Param_X;

  Param_X.Ticket_U32 = ++mTicket_U32;
  Param_X.TimeOut_U32 = _TimeOut_U32;
  Param_X.Timer_U32 = BOF::Bof_GetMsTickCount();
  Param_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN;
  Param_X.Listen_X = _rParam_X;
  Rts_E = mpuSocketOperationParamCollection->Push(&Param_X, PUSH_POP_TIMEOUT, nullptr);
  _rOpTicket_U32 = (Rts_E == BOF_ERR_NO_ERROR) ? Param_X.Ticket_U32 : 0;
  return Rts_E;
}
BOFERR BofSocketThread::ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_CONNECT_PARAM &_rParam_X, uint32_t &_rOpTicket_U32)
{
  BOFERR Rts_E;
  BOF_SOCKET_OPERATION_PARAM Param_X;

  Param_X.Ticket_U32 = ++mTicket_U32;
  Param_X.TimeOut_U32 = _TimeOut_U32;
  Param_X.Timer_U32 = BOF::Bof_GetMsTickCount();
  Param_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT;
  Param_X.Connect_X = _rParam_X;
  Rts_E = mpuSocketOperationParamCollection->Push(&Param_X, PUSH_POP_TIMEOUT, nullptr);
  _rOpTicket_U32 = (Rts_E == BOF_ERR_NO_ERROR) ? Param_X.Ticket_U32 : 0;
  return Rts_E;
}
BOFERR BofSocketThread::ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_READ_PARAM &_rParam_X, uint32_t &_rOpTicket_U32)
{
  BOFERR Rts_E;
  BOF_SOCKET_OPERATION_PARAM Param_X;

  Param_X.Ticket_U32 = ++mTicket_U32;
  Param_X.TimeOut_U32 = _TimeOut_U32;
  Param_X.Timer_U32 = BOF::Bof_GetMsTickCount();
  Param_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_READ;
  Param_X.Read_X = _rParam_X;
  Rts_E = mpuSocketOperationParamCollection->Push(&Param_X, PUSH_POP_TIMEOUT, nullptr);
  _rOpTicket_U32 = (Rts_E == BOF_ERR_NO_ERROR) ? Param_X.Ticket_U32 : 0;
  return Rts_E;
}
BOFERR BofSocketThread::ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_WRITE_PARAM &_rParam_X, uint32_t &_rOpTicket_U32)
{
  BOFERR Rts_E;
  BOF_SOCKET_OPERATION_PARAM Param_X;

  Param_X.Ticket_U32 = ++mTicket_U32;
  Param_X.TimeOut_U32 = _TimeOut_U32;
  Param_X.Timer_U32 = BOF::Bof_GetMsTickCount();
  Param_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_WRITE;
  Param_X.Write_X = _rParam_X;
  Rts_E = mpuSocketOperationParamCollection->Push(&Param_X, PUSH_POP_TIMEOUT, nullptr);
  _rOpTicket_U32 = (Rts_E == BOF_ERR_NO_ERROR) ? Param_X.Ticket_U32 : 0;
  return Rts_E;
}

BOFERR BofSocketThread::ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_DISCONNECT_PARAM &_rParam_X, uint32_t &_rOpTicket_U32)
{
  BOFERR Rts_E;
  BOF_SOCKET_OPERATION_PARAM Param_X;

  Param_X.Ticket_U32 = ++mTicket_U32;
  Param_X.TimeOut_U32 = _TimeOut_U32;
  Param_X.Timer_U32 = BOF::Bof_GetMsTickCount();
  Param_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT;
  Param_X.Disconnect_X = _rParam_X;
  Rts_E = mpuSocketOperationParamCollection->Push(&Param_X, PUSH_POP_TIMEOUT, nullptr);
  _rOpTicket_U32 = (Rts_E == BOF_ERR_NO_ERROR) ? Param_X.Ticket_U32 : 0;
  return Rts_E;
}

bool BofSocketThread::GetSocketOperationResult(uint32_t _TimeOut_U32, BOF_SOCKET_OPERATION_RESULT &_rOperationResult_X)
{
  return (mpuSocketOperationResultCollection->Pop(&_rOperationResult_X, _TimeOut_U32, nullptr, nullptr) == BOF_ERR_NO_ERROR);
}

uint32_t BofSocketThread::NumberOfOperationPending()
{
  return mpuSocketOperationParamCollection->GetNbElement();
}
uint32_t BofSocketThread::NumberOfResultPending()
{
  return mpuSocketOperationResultCollection->GetNbElement();
}
BOFERR BofSocketThread::CancelAllOperation()
{
  BOFERR Rts_E = BOF_ERR_PENDING;
  BOF_SOCKET_OPERATION_PARAM Param_X;

  if (!mCancelAllOperation_B)
  {
    mCancelAllOperation_B = true;
    mpuSocketOperationParamCollection->Push(&Param_X, PUSH_POP_TIMEOUT, nullptr); // To wakeup the thread
    Rts_E = BOF_ERR_NO_ERROR;
  }

  return Rts_E;
}

BofSocket *BofSocketThread::CreateSocket(bool _Tcp_B, BOF_IPV4_ADDR_U32 &_rIpAddr_X, uint16_t _Port_U16, uint32_t _NbMaxClient_U32) // 0 for normal socket !=0 for listening one _Listen_B)
{
  BofSocket *pRts_O;
  BOF::BOF_SOCKET_PARAM SocketParams_X;

  SocketParams_X.BroadcastPort_U16 = 0;
  SocketParams_X.MulticastSender_B = false;
  SocketParams_X.Ttl_U32 = 0;
  SocketParams_X.KeepAlive_B = false;
  SocketParams_X.ReUseAddress_B = true;
  SocketParams_X.BaseChannelParam_X.Blocking_B = true;
  SocketParams_X.BaseChannelParam_X.ListenBackLog_U32 = _NbMaxClient_U32;
  SocketParams_X.NoDelay_B = true;
  SocketParams_X.BaseChannelParam_X.RcvBufferSize_U32 = 0;
  SocketParams_X.BaseChannelParam_X.SndBufferSize_U32 = 0;
  SocketParams_X.BindIpAddress_S = _Tcp_B ? "tcp://" : "udp://" + _rIpAddr_X.ToString(_Port_U16);

  pRts_O = new BofSocket(SocketParams_X);
  if (pRts_O)
  {
    if (pRts_O->LastErrorCode() != BOF_ERR_NO_ERROR)
    {
      BOF_SAFE_DELETE(pRts_O);
    }
  }
  return pRts_O;
}

BOFERR BofSocketThread::V_OnProcessing()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  BOF_SOCKET_OPERATION_PARAM Param_X;
  BOF_SOCKET_OPERATION_RESULT Result_X;
  uint32_t Size_U32, Total_U32, Remain_U32;
  uint8_t *pBuffer_U8, *pCrtBuf_U8;
  bool IsBlockingMode_B;
  BofComChannel *pClient;
  std::string Target_S;

  while (!IsThreadLoopMustExit())
  {
    Rts_E = mpuSocketOperationParamCollection->Pop(&Param_X, PUSH_POP_TIMEOUT, nullptr, nullptr);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      // First init of operation param
      Result_X.Reset();
      Result_X.Operation_E = Param_X.Operation_E;
      Result_X.Sts_E = BOF_ERR_NO_ERROR;
      Result_X.OpTicket_U32 = Param_X.Ticket_U32;
      Result_X.Time_U32 = BOF::Bof_ElapsedMsTime(Param_X.Timer_U32);
      Size_U32 = 0;
      Total_U32 = 0;
      pBuffer_U8 = nullptr;

      switch (Param_X.Operation_E)
      {
        case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_READ:
          if ((Param_X.Read_X.pDstSocket_O) && (Param_X.Read_X.pBuffer_U8))
          {
            Total_U32 = Param_X.Read_X.Nb_U32;
            pBuffer_U8 = Param_X.Read_X.pBuffer_U8;
          }
          else
          {
            Result_X.Sts_E = BOF_ERR_EINVAL;
          }
          break;

        case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_WRITE:
          if ((Param_X.Write_X.pDstSocket_O) && (Param_X.Write_X.pBuffer_U8))
          {
            Total_U32 = Param_X.Write_X.Nb_U32;
            pBuffer_U8 = Param_X.Write_X.pBuffer_U8;
          }
          else
          {
            Result_X.Sts_E = BOF_ERR_EINVAL;
          }
          break;

        case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN:
          if (Param_X.Listen_X.NbMaxClient_U32 == 0)
          {
            Result_X.Sts_E = BOF_ERR_EINVAL;
          }
          break;

        case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT:
        case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT:
        default:
          break;
      }
      pCrtBuf_U8 = pBuffer_U8;
      Remain_U32 = Total_U32;
      if (Result_X.Sts_E == BOF_ERR_NO_ERROR)
      {
        // All param ok, process request
        do //To be able to cancel a very long socket operation (read,write,..) we split it in segment of max 250 ms
        {
          switch (Param_X.Operation_E)
          {
            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN:
              Result_X.Sts_E = BOF_ERR_ENOMEM;
              Result_X.pListenSocket_O = CreateSocket(true, Param_X.Listen_X.SrcIpAddr_X, Param_X.Listen_X.SrcPort_U16, Param_X.Listen_X.NbMaxClient_U32);
              if (Result_X.pListenSocket_O)
              {
                Result_X.Sts_E = BOF_ERR_NO_CLIENT;
                pClient = Result_X.pListenSocket_O->V_Listen(Param_X.TimeOut_U32, "");
                if (pClient)
                {
                  Result_X.pDstSocket_O = dynamic_cast<BofSocket *>(pClient);
                  if (Result_X.pDstSocket_O)
                  {
                    Result_X.Sts_E = BOF_ERR_NO_ERROR;
                  }
                }
              }
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT:
              Result_X.Sts_E = BOF_ERR_ENOMEM;
              Result_X.pDstSocket_O = CreateSocket(Param_X.Connect_X.Tcp_B, Param_X.Connect_X.SrcIpAddr_X, Param_X.Connect_X.SrcPort_U16, 0);
              if (Result_X.Sts_E == BOF_ERR_NO_ERROR)
              {
                Target_S = Param_X.Connect_X.Tcp_B ? "tcp://" : "udp://" + Param_X.Connect_X.DstIpAddr_X.ToString(Param_X.Connect_X.DstPort_U16);
                Result_X.Sts_E = Result_X.pDstSocket_O->V_Connect(Param_X.TimeOut_U32, Target_S, "");
              }
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_READ:
              Size_U32 = Remain_U32;
              Result_X.Sts_E = Param_X.Read_X.pDstSocket_O->V_ReadData(250, /*Param_X.TimeOut_U32*/ Size_U32, pCrtBuf_U8);//To be able to cancel a very long socket operation (read,write,..) we split it in segment of max 250 ms
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_WRITE:
              Size_U32 = Remain_U32;
              Result_X.Sts_E = Param_X.Write_X.pDstSocket_O->V_ReadData(250, /*Param_X.TimeOut_U32*/ Size_U32, pCrtBuf_U8); //To be able to cancel a very long socket operation (read,write,..) we split it in segment of max 250 ms
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT:
              BOF_SAFE_DELETE(Param_X.Disconnect_X.pListenSocket_O);
              BOF_SAFE_DELETE(Param_X.Disconnect_X.pDstSocket_O);
              break;

            default:
              break;
          }

          Remain_U32 -= Size_U32;
          pCrtBuf_U8 += Size_U32;

          Result_X.Time_U32 = BOF::Bof_ElapsedMsTime(Param_X.Timer_U32);
          if (Result_X.Time_U32 > Param_X.TimeOut_U32)
          {
            Result_X.Sts_E = BOF_ERR_ETIMEDOUT;
          }
          else if (Result_X.Sts_E == BOF_ERR_ETIMEDOUT)
          {
            Result_X.Sts_E = BOF_ERR_NO_ERROR;
          }
        } while ((!IsThreadLoopMustExit()) && (!mCancelAllOperation_B) && (Remain_U32) && (Result_X.Sts_E == BOF_ERR_NO_ERROR));
      }
      if (mCancelAllOperation_B)
      {
        mpuSocketOperationParamCollection->Reset();
        mpuSocketOperationResultCollection->Reset();
        mCancelAllOperation_B = false;
        Rts_E = BOF_ERR_STOPPED;
      }
      else
      {
        Result_X.Size_U32 = Total_U32;
        Result_X.pBuffer_U8 = pBuffer_U8;
        Result_X.Time_U32 = BOF::Bof_ElapsedMsTime(Param_X.Timer_U32);
        mpuSocketOperationResultCollection->Push(&Result_X, PUSH_POP_TIMEOUT, nullptr);
      }
    }
  }

  return Rts_E;
}
END_BOF_NAMESPACE()
