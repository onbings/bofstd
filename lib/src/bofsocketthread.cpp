/*!
  Copyright (c) 2008-2020, Evs. All rights reserved.

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  History:

  V 1.00  February 18 2014  NMA : Initial release

  Thread pool https://github.com/vit-vit/CTPL/blob/master/ctpl.h
*/
#include <bofstd/bofsocketthread.h>

BEGIN_BOF_NAMESPACE()
constexpr uint32_t PUSH_POP_TIMEOUT = 150; // Global To for getting command out of incoming queue, in ListeningMode_B it is half of the To specified for listen
constexpr uint32_t DEF_IO_TIMEOUT = 2000;

BofSocketThread::BofSocketThread(const BOF_SOCKET_THREAD_PARAM &_rSocketThreadParam_X)
    : BofThread()
{
  BOF_CIRCULAR_BUFFER_PARAM CircularBufferParam_X;
  BOFERR Sts_E;
  bool Sts_B;

  mSocketThreadParam_X = _rSocketThreadParam_X;
  if (mSocketThreadParam_X.SubPacketTimeout_U32 == 0)
  {
    mSocketThreadParam_X.SubPacketTimeout_U32 = DEF_IO_TIMEOUT;
  }
  Sts_E = Bof_CreateEvent(_rSocketThreadParam_X.Name_S, false, 1, false, false, mCancelEvent_X);
  BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);

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
      // printf("%d: Launch\n", BOF::Bof_GetMsTickCount());
      Sts_E = LaunchBofProcessingThread(mSocketThreadParam_X.Name_S, false, 0, mSocketThreadParam_X.ThreadSchedulerPolicy_E, mSocketThreadParam_X.ThreadPriority_E, 0, 2000, 0);
      BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
      Sts_B = IsThreadRunning(100);
      BOF_ASSERT(Sts_B);
    }
  }
}

BofSocketThread::~BofSocketThread()
{
  BOF_SOCKET_EXIT_PARAM Param_X;
  uint32_t OpTicket_U32;
  // First this and after destructor of BofThread
  // printf("%d: ProgramSocketOperation exit cmd\n", BOF::Bof_GetMsTickCount());
  BOFERR e = ProgramSocketOperation(PUSH_POP_TIMEOUT, Param_X, OpTicket_U32);
  if (e != BOF_ERR_NO_ERROR)
  {
    // printf("%d: ProgramSocketOperation rts %d\n", BOF::Bof_GetMsTickCount(), e);
  }
  // Must kill the thread from here and not in the BofThread destructor because between both destructor, all mem var of BofSocketThread will disappear (unique pointer)
  DestroyBofProcessingThread("~BofSocketThread");
}

BOFERR BofSocketThread::ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_EXIT_PARAM &_rParam_X, uint32_t &_rOpTicket_U32)
{
  BOFERR Rts_E;
  BOF_SOCKET_OPERATION_PARAM Param_X;

  Param_X.Ticket_U32 = mTicket_U32;
  BOF_INC_TICKET_NUMBER(mTicket_U32);
  Param_X.TimeOut_U32 = _TimeOut_U32;
  Param_X.Timer_U32 = BOF::Bof_GetMsTickCount();
  Param_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT;
  Param_X.Exit_X = _rParam_X;
  Rts_E = mpuSocketOperationParamCollection->Push(&Param_X, PUSH_POP_TIMEOUT, nullptr);
  // printf("BOF_SOCKET_OPERATION_EXIT %s Err %s Ticket %d Op %d\n", mSocketThreadParam_X.Name_S.c_str(), BOF::Bof_ErrorCode(Rts_E), Param_X.Ticket_U32, Param_X.Operation_E);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    _rOpTicket_U32 = Param_X.Ticket_U32;
    mOperationPending_B = true;
  }
  else
  {
    _rOpTicket_U32 = 0;
  }
  return Rts_E;
}

BOFERR BofSocketThread::ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_LISTEN_PARAM &_rParam_X, uint32_t &_rOpTicket_U32)
{
  BOFERR Rts_E;
  BOF_SOCKET_OPERATION_PARAM Param_X;

  Param_X.Ticket_U32 = mTicket_U32;
  BOF_INC_TICKET_NUMBER(mTicket_U32);
  Param_X.TimeOut_U32 = _TimeOut_U32;
  Param_X.Timer_U32 = BOF::Bof_GetMsTickCount();
  Param_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN;
  Param_X.Listen_X = _rParam_X;
  Rts_E = mpuSocketOperationParamCollection->Push(&Param_X, PUSH_POP_TIMEOUT, nullptr);
  // printf("BOF_SOCKET_OPERATION_LISTEN %s Err %s Ticket %d Op %d\n", mSocketThreadParam_X.Name_S.c_str(), BOF::Bof_ErrorCode(Rts_E), Param_X.Ticket_U32, Param_X.Operation_E);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    _rOpTicket_U32 = Param_X.Ticket_U32;
    mOperationPending_B = true;
  }
  else
  {
    _rOpTicket_U32 = 0;
  }
  return Rts_E;
}
BOFERR BofSocketThread::ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_CONNECT_PARAM &_rParam_X, uint32_t &_rOpTicket_U32)
{
  BOFERR Rts_E;
  BOF_SOCKET_OPERATION_PARAM Param_X;

  Param_X.Ticket_U32 = mTicket_U32;
  BOF_INC_TICKET_NUMBER(mTicket_U32);
  Param_X.TimeOut_U32 = _TimeOut_U32;
  Param_X.Timer_U32 = BOF::Bof_GetMsTickCount();
  Param_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT;
  Param_X.Connect_X = _rParam_X;
  Rts_E = mpuSocketOperationParamCollection->Push(&Param_X, PUSH_POP_TIMEOUT, nullptr);
  // printf("BOF_SOCKET_OPERATION_CONNECT %s Err %s Ticket %d Op %d\n", mSocketThreadParam_X.Name_S.c_str(), BOF::Bof_ErrorCode(Rts_E), Param_X.Ticket_U32, Param_X.Operation_E);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    _rOpTicket_U32 = Param_X.Ticket_U32;
    mOperationPending_B = true;
  }
  else
  {
    _rOpTicket_U32 = 0;
  }
  return Rts_E;
}
BOFERR BofSocketThread::ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_READ_PARAM &_rParam_X, uint32_t &_rOpTicket_U32)
{
  BOFERR Rts_E;
  BOF_SOCKET_OPERATION_PARAM Param_X;

  Param_X.Ticket_U32 = mTicket_U32;
  BOF_INC_TICKET_NUMBER(mTicket_U32);
  Param_X.TimeOut_U32 = _TimeOut_U32;
  Param_X.Timer_U32 = BOF::Bof_GetMsTickCount();
  Param_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_READ;
  Param_X.Read_X = _rParam_X;
  Rts_E = mpuSocketOperationParamCollection->Push(&Param_X, PUSH_POP_TIMEOUT, nullptr);
  printf("BOF_SOCKET_OPERATION_READ %s PushErr %s Ticket %d Op %d Buf %d:%p NbElem %d\n", mSocketThreadParam_X.Name_S.c_str(), BOF::Bof_ErrorCode(Rts_E), Param_X.Ticket_U32, Param_X.Operation_E, _rParam_X.Nb_U32, _rParam_X.pBuffer_U8, mpuSocketOperationParamCollection->GetNbElement());
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    _rOpTicket_U32 = Param_X.Ticket_U32;
    mOperationPending_B = true;
  }
  else
  {
    _rOpTicket_U32 = 0;
  }
  return Rts_E;
}
BOFERR BofSocketThread::ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_WRITE_PARAM &_rParam_X, uint32_t &_rOpTicket_U32)
{
  BOFERR Rts_E;
  BOF_SOCKET_OPERATION_PARAM Param_X;

  Param_X.Ticket_U32 = mTicket_U32;
  BOF_INC_TICKET_NUMBER(mTicket_U32);
  Param_X.TimeOut_U32 = _TimeOut_U32;
  Param_X.Timer_U32 = BOF::Bof_GetMsTickCount();
  Param_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_WRITE;
  Param_X.Write_X = _rParam_X;
  Rts_E = mpuSocketOperationParamCollection->Push(&Param_X, PUSH_POP_TIMEOUT, nullptr);
  printf("BOF_SOCKET_OPERATION_WRITE %s PushErr %s Ticket %d Op %d Buf %d:%p Soc %p NbElem %d\n", mSocketThreadParam_X.Name_S.c_str(), BOF::Bof_ErrorCode(Rts_E), Param_X.Ticket_U32, Param_X.Operation_E, _rParam_X.Nb_U32, _rParam_X.pBuffer_U8, Param_X.Write_X.pSocket_O, mpuSocketOperationParamCollection->GetNbElement());
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    _rOpTicket_U32 = Param_X.Ticket_U32;
    mOperationPending_B = true;
  }
  else
  {
    _rOpTicket_U32 = 0;
  }
  return Rts_E;
}

BOFERR BofSocketThread::ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_DISCONNECT_PARAM &_rParam_X, uint32_t &_rOpTicket_U32)
{
  BOFERR Rts_E;
  BOF_SOCKET_OPERATION_PARAM Param_X;

  Param_X.Ticket_U32 = mTicket_U32;
  BOF_INC_TICKET_NUMBER(mTicket_U32);
  Param_X.TimeOut_U32 = _TimeOut_U32;
  Param_X.Timer_U32 = BOF::Bof_GetMsTickCount();
  // printf("================> Tim %d\n", Param_X.Timer_U32);
  Param_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT;
  Param_X.Disconnect_X = _rParam_X;
  Rts_E = mpuSocketOperationParamCollection->Push(&Param_X, PUSH_POP_TIMEOUT, nullptr);
  // printf("BOF_SOCKET_OPERATION_DISCONNECT %s Err %s Ticket %d Op %d\n", mSocketThreadParam_X.Name_S.c_str(), BOF::Bof_ErrorCode(Rts_E), Param_X.Ticket_U32, Param_X.Operation_E);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    _rOpTicket_U32 = Param_X.Ticket_U32;
    mOperationPending_B = true;
  }
  else
  {
    _rOpTicket_U32 = 0;
  }
  return Rts_E;
}

BOFERR BofSocketThread::GetSocketOperationResult(uint32_t _TimeOut_U32, BOF_SOCKET_OPERATION_RESULT &_rOperationResult_X)
{
  BOFERR Rts_E;

  _rOperationResult_X.Reset();
  Rts_E = mpuSocketOperationResultCollection->Pop(&_rOperationResult_X, _TimeOut_U32, nullptr, nullptr);
  printf("BOF_SOCKET_OPERATION_GET_RESULT %s Err %s Ticket %d Op %d Sts %s\n", mSocketThreadParam_X.Name_S.c_str(), BOF::Bof_ErrorCode(Rts_E), _rOperationResult_X.OpTicket_U32, _rOperationResult_X.Operation_E, BOF::Bof_ErrorCode(_rOperationResult_X.Sts_E));
  return Rts_E;
}

uint32_t BofSocketThread::NumberOfOperationWaiting()
{
  // printf("NumberOfOperationPending %d\n", mpuSocketOperationParamCollection->GetNbElement());
  return mpuSocketOperationParamCollection->GetNbElement();
}
bool BofSocketThread::IsOperationPending()
{
  // printf("NumberOfOperationPending %d\n", mpuSocketOperationParamCollection->GetNbElement());
  return mOperationPending_B; // only cleared by ClearSocketOperation or CancelSocketOperation
}
uint32_t BofSocketThread::NumberOfResultPending()
{
  // printf("NumberOfResultPending %d\n", mpuSocketOperationResultCollection->GetNbElement());
  return mpuSocketOperationResultCollection->GetNbElement();
}
BOFERR BofSocketThread::CancelSocketOperation(uint32_t _TimeOut_U32)
{
  BOFERR Rts_E = BOF_ERR_PENDING;

  if (!mCancel_B)
  {
    mCancel_B = true;
    if (_TimeOut_U32)
    {
      Rts_E = BOF::Bof_WaitForEvent(mCancelEvent_X, _TimeOut_U32, 0);
    }
    else
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      mOperationPending_B = false;
    }
  }
  return Rts_E;
}

BOFERR BofSocketThread::ClearSocketOperation()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;

  mOperationPending_B = false;
  mCancel_B = false;
  Bof_SetEventMask(mCancelEvent_X, 0);
  return Rts_E;
}

BOFERR BofSocketThread::ResetSocketOperation()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;

  mpuSocketOperationParamCollection->Reset();
  mpuSocketOperationResultCollection->Reset();
  ClearSocketOperation();

  return Rts_E;
}

BofSocket *BofSocketThread::CreateTcpSocket(BOF::BOF_IPV4_ADDR_U32 &_rSrcIpAddr_X, uint16_t _SrcPort_U16, uint32_t _NbMaxClient_U32) // 0 for normal socket !=0 for listening one _Listen_B)
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
  SocketParams_X.BindIpAddress_S = "tcp://" + _rSrcIpAddr_X.ToString(_SrcPort_U16);

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
BofSocket *BofSocketThread::CreateUdpSocket(BOF::BOF_IPV4_ADDR_U32 &_rSrcIpAddr_X, uint16_t _SrcPort_U16, BOF::BOF_IPV4_ADDR_U32 &_rDstIpAddr_X, uint16_t _DstPort_U16)
{
  BofSocket *pRts_O;
  BOF::BOF_SOCKET_PARAM SocketParams_X;
  BOF::BOF_SOCKET_ADDRESS DstIp_X;

  SocketParams_X.BroadcastPort_U16 = 0;
  SocketParams_X.MulticastSender_B = false;
  SocketParams_X.Ttl_U32 = 0;
  SocketParams_X.KeepAlive_B = false;
  SocketParams_X.ReUseAddress_B = true;
  SocketParams_X.BaseChannelParam_X.Blocking_B = true;
  SocketParams_X.BaseChannelParam_X.ListenBackLog_U32 = 0;
  SocketParams_X.NoDelay_B = true;
  SocketParams_X.BaseChannelParam_X.RcvBufferSize_U32 = 0;
  SocketParams_X.BaseChannelParam_X.SndBufferSize_U32 = 0;
  SocketParams_X.BindIpAddress_S = "udp://" + _rSrcIpAddr_X.ToString(_SrcPort_U16);

  pRts_O = new BofSocket(SocketParams_X);
  if (pRts_O)
  {
    if (pRts_O->LastErrorCode() != BOF_ERR_NO_ERROR)
    {
      BOF_SAFE_DELETE(pRts_O);
    }
    else
    {
      DstIp_X = BOF::BOF_SOCKET_ADDRESS("udp://" + _rDstIpAddr_X.ToString(_DstPort_U16));
      if (pRts_O->SetDstIpAddress(DstIp_X) != BOF_ERR_NO_ERROR)
      {
        BOF_SAFE_DELETE(pRts_O);
      }
    }
  }
  return pRts_O;
}
BofSocket *BofSocketThread::GetListeningSocket()
{
  return mSocketThreadParam_X.pListeningSocket_O;
}

BofSocket *BofSocketThread::GetSocket()
{
  return mSocketThreadParam_X.pSocket_O;
}
const BOF_SOCKET_OPERATION_PARAM &BofSocketThread::GetCurrentOpParam()
{
  return mCurrentOpParam_X;
}
BOFERR BofSocketThread::V_OnProcessing()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR, Sts_E;
  BOF_SOCKET_OPERATION_RESULT Result_X;
  uint32_t Size_U32, Total_U32, Remain_U32, PollTimeout_U32, ListenTimeout_U32, ListenTicketId_U32, Start_U32, Delta_U32, KBPerS_U32;
  uint8_t *pBuffer_U8, *pCrtBuf_U8;
  bool ListeningMode_B, NewCommandRcv_B, SendResult_B;
  BofComChannel *pClient;
  std::string Target_S, IpParam_S;
  BofSocket *pIoSocket_O;

  ListeningMode_B = false;
  PollTimeout_U32 = PUSH_POP_TIMEOUT; // Will be changed if we goes in ListeningMode_B
  do
  {
    // printf("%d: Alive Wait %s for %d Bal %d ListenMode %d\n", BOF::Bof_GetMsTickCount(), mSocketThreadParam_X.Name_S.c_str(), PollTimeout_U32, BofThread::S_BofThreadBalance(), ListeningMode_B);
    // Use Peek instead of Pop to use the queue length as an io pending flag (pop is made at the end of the command exec)
    Rts_E = mpuSocketOperationParamCollection->Peek(&mCurrentOpParam_X, PollTimeout_U32, nullptr, nullptr);
    if ((Rts_E != BOF_ERR_ETIMEDOUT) && (Rts_E != BOF_ERR_NO_ERROR))
    {
      // printf("%d: End of Wait rts %d cmd %d\n", BOF::Bof_GetMsTickCount(), Rts_E, mCurrentOpParam_X.Operation_E);
    }
    // Check if a new cmd has been received or just command timeout on the queue
    NewCommandRcv_B = (Rts_E == BOF_ERR_NO_ERROR);
    if ((NewCommandRcv_B) || (ListeningMode_B))
    {
      // printf("%d: %s Alive Wait err %d ListenMode %d NewCmd %d Op %d Ticket %d To %d NbP %d NbR %d\n", BOF::Bof_GetMsTickCount(), mSocketThreadParam_X.Name_S.c_str(), Rts_E, ListeningMode_B, NewCommandRcv_B, mCurrentOpParam_X.Operation_E, mCurrentOpParam_X.Ticket_U32,
      // mCurrentOpParam_X.TimeOut_U32, mpuSocketOperationParamCollection->GetNbElement(), mpuSocketOperationResultCollection->GetNbElement()); First init of operation param
      Result_X.Reset();
      Result_X.Sts_E = BOF_ERR_NO_ERROR;
      Result_X.Operation_E = mCurrentOpParam_X.Operation_E;
      Result_X.OpTicket_U32 = mCurrentOpParam_X.Ticket_U32;
      Result_X.Time_U32 = BOF::Bof_ElapsedMsTime(mCurrentOpParam_X.Timer_U32);

      // So ListeningMode_B is perhaps true, but nevertheless a new command must be processed
      SendResult_B = false;
      if (NewCommandRcv_B)
      {
        mOperationPending_B = true; // can be resetted by canceled
        Total_U32 = 0;
        pBuffer_U8 = nullptr;
        SendResult_B = true;
        Start_U32 = 0;

        switch (mCurrentOpParam_X.Operation_E)
        {
          case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT:
            // printf("%d: BOF_SOCKET_OPERATION_EXIT\n", BOF::Bof_GetMsTickCount());
            Result_X.Sts_E = BOF_ERR_CANCEL; // To skip if below
            Rts_E = BOF_ERR_CANCEL;          // To exit loop here and in BofThread:
            break;

          case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_READ:
            Start_U32 = Bof_GetMsTickCount();
            // If something is defined in the optional param we use it without impacting mpSocket_O
            pIoSocket_O = mCurrentOpParam_X.Read_X.pSocket_O ? mCurrentOpParam_X.Read_X.pSocket_O : mSocketThreadParam_X.pSocket_O;
            // If no previous connect, we take the socket from the optional param and make it equal to mpSocket_O
            if (!mSocketThreadParam_X.pSocket_O) // If no previous connect, we take the socket from the optional param and make it equal to mpSocket_O
            {
              mSocketThreadParam_X.pSocket_O = pIoSocket_O;
            }
            if ((pIoSocket_O) && (mCurrentOpParam_X.Read_X.pBuffer_U8))
            {
              printf("BOF_SOCKET_OPERATION_READ IoBuf %d:%p Ticket %d Op %d START on %s\n", mCurrentOpParam_X.Read_X.Nb_U32, mCurrentOpParam_X.Read_X.pBuffer_U8, mCurrentOpParam_X.Ticket_U32, mCurrentOpParam_X.Operation_E, pIoSocket_O->ToString().c_str());
              Total_U32 = mCurrentOpParam_X.Read_X.Nb_U32;
              pBuffer_U8 = mCurrentOpParam_X.Read_X.pBuffer_U8;
              Result_X.pSocket_O = pIoSocket_O;
            }
            else
            {
              Result_X.Sts_E = BOF_ERR_EINVAL;
            }
            break;

          case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_WRITE:
            Start_U32 = Bof_GetMsTickCount();
            // If something is defined in the optional param we use it without impacting mSocketThreadParam_X.pSocket_O
            pIoSocket_O = mCurrentOpParam_X.Write_X.pSocket_O ? mCurrentOpParam_X.Write_X.pSocket_O : mSocketThreadParam_X.pSocket_O;
            // If no previous connect, we take the socket from the optional param and make it equal to mpSocket_O
            if (!mSocketThreadParam_X.pSocket_O) // If no previous connect, we take the socket from the optional param and make it equal to mpSocket_O
            {
              mSocketThreadParam_X.pSocket_O = pIoSocket_O;
            }
            if ((pIoSocket_O) && (mCurrentOpParam_X.Write_X.pBuffer_U8))
            {
              printf("BOF_SOCKET_OPERATION_WRITE IoBuf %d:%p Ticket %d Op %d START on %s\n", mCurrentOpParam_X.Write_X.Nb_U32, mCurrentOpParam_X.Write_X.pBuffer_U8, mCurrentOpParam_X.Ticket_U32, mCurrentOpParam_X.Operation_E, pIoSocket_O->ToString().c_str());
              Total_U32 = mCurrentOpParam_X.Write_X.Nb_U32;
              pBuffer_U8 = mCurrentOpParam_X.Write_X.pBuffer_U8;
              Result_X.pSocket_O = pIoSocket_O;
            }
            else
            {
              Result_X.Sts_E = BOF_ERR_EINVAL;
            }
            break;

          case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN:
            if (mCurrentOpParam_X.Listen_X.NbMaxClient_U32 == 0)
            {
              Result_X.Sts_E = BOF_ERR_EINVAL;
            }
            else
            {
              if (!ListeningMode_B) // Already in listen (JustOnce_B was false)
              {
                if (!mSocketThreadParam_X.pListeningSocket_O) // Specified in the constructor ?
                {
                  mSocketThreadParam_X.pListeningSocket_O = CreateTcpSocket(mCurrentOpParam_X.Listen_X.SrcIpAddr_X, mCurrentOpParam_X.Listen_X.SrcPort_U16, mCurrentOpParam_X.Listen_X.NbMaxClient_U32);
                }
                if (mSocketThreadParam_X.pListeningSocket_O)
                {
                  if (mCurrentOpParam_X.Listen_X.JustOnce_B)
                  {
                    ListenTimeout_U32 = mCurrentOpParam_X.TimeOut_U32;
                  }
                  else
                  {
                    // If we want to create an "permanent" listen socket to manage incoming connect, we first need to answer to the LISTEM command
                    // and after, for each incoming connection we will send another message
                    Result_X.pSocket_O = mSocketThreadParam_X.pListeningSocket_O;
                    Result_X.Sts_E = mpuSocketOperationResultCollection->Push(&Result_X, PUSH_POP_TIMEOUT, nullptr);
                    if (Result_X.Sts_E == BOF_ERR_NO_ERROR)
                    {
                      ListenTimeout_U32 = mCurrentOpParam_X.TimeOut_U32 / 2; // mCurrentOpParam_X.TimeOut_U32;
                      PollTimeout_U32 = mCurrentOpParam_X.TimeOut_U32 / 2;
                      mCurrentOpParam_X.TimeOut_U32 = 0xFFFFFFFF;
                      ListenTicketId_U32 = mCurrentOpParam_X.Ticket_U32;
                      ListeningMode_B = true;
                      SendResult_B = false;
                    }
                  }
                }
                else
                {
                  Result_X.Sts_E = BOF_ERR_ENOMEM;
                }
              }
              else
              {
                Result_X.Sts_E = BOF_ERR_ALREADY_OPENED;
              }
            }
            break;

          case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT:
          case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT:
            break;

          default:
            Result_X.Sts_E = BOF_ERR_INVALID_COMMAND;
            break;
        }

        pCrtBuf_U8 = pBuffer_U8;
        Remain_U32 = Total_U32;
        Size_U32 = 0;
      }
      else // So ListeningMode_B is true
      {
        Result_X.Sts_E = BOF_ERR_NO_ERROR;
        mCurrentOpParam_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN;
        mCurrentOpParam_X.Listen_X.JustOnce_B = true; // because ListeningMode_B is true
      }
      // A command has been received with good param->it can be executed -> A result will be sent to the caller
      if (Result_X.Sts_E == BOF_ERR_NO_ERROR) // A command has been received and it can be executed -> A sesult will be sent to the caller
      {
        // To be able to cancel a very long socket operation (read,write,..) we split it in segment of max IO_TIMEOUT ms
        do
        {
          switch (mCurrentOpParam_X.Operation_E)
          {
            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN:
              if (ListeningMode_B)
              {
                Result_X.Sts_E = BOF_ERR_NO_ERROR;
                mCurrentOpParam_X.Timer_U32 = BOF::Bof_GetMsTickCount();
              }
              else
              {
                Result_X.Sts_E = BOF_ERR_NO_CLIENT;
              }
              // Just to detect DISCONNECT extra command on listening for ever thread
              // printf("%d: Alive Listen Wait %s for %d Bal %d\n", BOF::Bof_GetMsTickCount(), mSocketThreadParam_X.Name_S.c_str(), ListenTimeout_U32, BofThread::S_BofThreadBalance());
              pClient = mSocketThreadParam_X.pListeningSocket_O->V_Listen(ListenTimeout_U32, "");
              if (pClient)
              {
                Result_X.pSocket_O = dynamic_cast<BofSocket *>(pClient);
                if (Result_X.pSocket_O)
                {
                  SendResult_B = true;
                  // mSocketThreadParam_X.pSocket_O = mSocketThreadParam_X.pListeningSocket_O;
                  Result_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT;
                  if (ListeningMode_B)
                  {
                    // In this case Remain_U32 = Total_U32 = 1; Size_U32=0
                    BOF_INC_TICKET_NUMBER(ListenTicketId_U32);
                    Result_X.OpTicket_U32 = ListenTicketId_U32;
                  }
                  else
                  {
                    Result_X.Sts_E = BOF_ERR_NO_ERROR;
                  }
                }
              }
              // printf("----listen------------------------------------->id=%d\n", ListenTicketId_U32);
              // printf("%d: Alive Listen Wait end %p SndRes %d\n", BOF::Bof_GetMsTickCount(), pClient, SendResult_B);
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT:
              Result_X.Sts_E = BOF_ERR_ENOMEM;
              if (mCurrentOpParam_X.Connect_X.Tcp_B)
              {
                Result_X.pSocket_O = CreateTcpSocket(mCurrentOpParam_X.Connect_X.SrcIpAddr_X, mCurrentOpParam_X.Connect_X.SrcPort_U16, 0);
                if (Result_X.pSocket_O)
                {
                  Target_S = (mCurrentOpParam_X.Connect_X.Tcp_B ? "tcp://" : "udp://") + mCurrentOpParam_X.Connect_X.DstIpAddr_X.ToString(mCurrentOpParam_X.Connect_X.DstPort_U16);
                  Result_X.Sts_E = Result_X.pSocket_O->V_Connect(mCurrentOpParam_X.TimeOut_U32, Target_S, "");
                  if (Result_X.Sts_E == BOF_ERR_NO_ERROR)
                  {
                    mSocketThreadParam_X.pSocket_O = Result_X.pSocket_O;
                  }
                  else
                  {
                    BOF_SAFE_DELETE(Result_X.pSocket_O);
                  }
                }
              }
              else
              {
                Result_X.Sts_E = BOF_ERR_EINVAL;
                Result_X.pSocket_O = CreateUdpSocket(mCurrentOpParam_X.Connect_X.SrcIpAddr_X, mCurrentOpParam_X.Connect_X.SrcPort_U16, mCurrentOpParam_X.Connect_X.DstIpAddr_X, mCurrentOpParam_X.Connect_X.DstPort_U16);
                if (Result_X.pSocket_O)
                {
                  Result_X.Sts_E = BOF_ERR_NO_ERROR;
                  mSocketThreadParam_X.pSocket_O = Result_X.pSocket_O;
                }
              }
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_READ:
              Size_U32 = Remain_U32;
              Result_X.Sts_E = pIoSocket_O->V_ReadData(mSocketThreadParam_X.SubPacketTimeout_U32, /*mCurrentOpParam_X.TimeOut_U32*/ Size_U32, pCrtBuf_U8); // To be able to cancel a very long socket operation (read,write,..) we split it in segment of max mSocketThreadParam_X.SubPacketTimeout_U32 ms
              // printf("%d: Read: Total %d Size %d Remain %d Sts %d\n", Bof_GetMsTickCount(), Total_U32, Size_U32, Remain_U32, Result_X.Sts_E);
              Delta_U32 = Bof_ElapsedMsTime(Start_U32);
              KBPerS_U32 = Delta_U32 ? (Size_U32 * 1000) / (Delta_U32 * 1024) : 99999999;
              if ((Result_X.Sts_E) || (Size_U32 == Remain_U32))
              {
                printf("BOF_SOCKET_OPERATION_READ LstIoBuf %d:%p Ticket %d Op %d END Sts %s Delta %d Sz %d KB/S %d Rem %d\n", Size_U32, pCrtBuf_U8, mCurrentOpParam_X.Ticket_U32, mCurrentOpParam_X.Operation_E, Bof_ErrorCode(Result_X.Sts_E), Delta_U32, Size_U32, KBPerS_U32, Remain_U32 - Size_U32);
              }
              if ((Result_X.Sts_E == BOF_ERR_ETIMEDOUT) && (Size_U32))
              {
                printf("BOF_SOCKET_OPERATION_READ Timeout cancelled as we rx some byte (%d)\n", Size_U32);
                Result_X.Sts_E = BOF_ERR_NO_ERROR;
              }
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_WRITE:
              Size_U32 = Remain_U32;
              Result_X.Sts_E = pIoSocket_O->V_WriteData(mSocketThreadParam_X.SubPacketTimeout_U32, /*mCurrentOpParam_X.TimeOut_U32*/ Size_U32, pCrtBuf_U8); // To be able to cancel a very long socket operation (read,write,..) we split it in segment of max mSocketThreadParam_X.SubPacketTimeout_U32 ms
              Delta_U32 = Bof_ElapsedMsTime(Start_U32);
              KBPerS_U32 = Delta_U32 ? (Size_U32 * 1000) / (Delta_U32 * 1024) : 99999999;
              if ((Result_X.Sts_E) || (Size_U32 == Remain_U32))
              {
                printf("BOF_SOCKET_OPERATION_WRITE LstIoBuf %d:%p Ticket %d Op %d END Sts %s Delta %d Sz %d KB/S %d Rem %d\n", Size_U32, pCrtBuf_U8, mCurrentOpParam_X.Ticket_U32, mCurrentOpParam_X.Operation_E, Bof_ErrorCode(Result_X.Sts_E), Delta_U32, Size_U32, KBPerS_U32, Remain_U32 - Size_U32);
              }
              if ((Result_X.Sts_E == BOF_ERR_ETIMEDOUT) && (Size_U32))
              {
                printf("BOF_SOCKET_OPERATION_WRITE Timeout cancelled as we tx some byte (%d)\n", Size_U32);
                Result_X.Sts_E = BOF_ERR_NO_ERROR;
              }
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT:
              if (ListeningMode_B)
              {
                Result_X.pSocket_O = mSocketThreadParam_X.pListeningSocket_O;
                ListeningMode_B = false;
                PollTimeout_U32 = PUSH_POP_TIMEOUT;
                // Result_X.Time_U32 = BOF::Bof_ElapsedMsTime(mCurrentOpParam_X.Timer_U32);
                BOF_SAFE_DELETE(mSocketThreadParam_X.pListeningSocket_O);
              }
              else
              {
                Result_X.pSocket_O = mSocketThreadParam_X.pSocket_O;
                BOF_SAFE_DELETE(mSocketThreadParam_X.pSocket_O);
              }
              // printf("B==>S_BofSocketBalance %d S_BofThreadBalance %d\n", BOF::BofSocket::S_BofSocketBalance(), BOF::BofThread::S_BofThreadBalance());
              break;

            default:
              break;
          }

          Remain_U32 -= Size_U32;
          pCrtBuf_U8 += Size_U32;

          Result_X.Time_U32 = BOF::Bof_ElapsedMsTime(mCurrentOpParam_X.Timer_U32);
          if (Result_X.Time_U32 > mCurrentOpParam_X.TimeOut_U32)
          {
            printf("Op %d TIMEOUT %d > %d\n !!!", mCurrentOpParam_X.Operation_E, Result_X.Time_U32, mCurrentOpParam_X.TimeOut_U32);
            Result_X.Sts_E = BOF_ERR_ETIMEDOUT;
          }
          if (mCancel_B)
          {
            // printf("inner loop exit %d snd %d rem %d sts %d cancel %d op %d tck %d\n", IsThreadLoopMustExit(), SendResult_B, Remain_U32, Result_X.Sts_E, mCancel_B.load(), Result_X.Operation_E, Result_X.OpTicket_U32);
          }
        } while ((!IsThreadLoopMustExit()) && (!mCancel_B) && (Remain_U32) && (Result_X.Sts_E == BOF_ERR_NO_ERROR));
        if (mCancel_B)
        {
          SendResult_B = false;
        }
      }
      if (SendResult_B)
      {
        Result_X.Size_U32 = Total_U32 - Remain_U32;
        Result_X.pBuffer_U8 = pBuffer_U8;
        Result_X.Time_U32 = BOF::Bof_ElapsedMsTime(mCurrentOpParam_X.Timer_U32);
        Sts_E = mpuSocketOperationResultCollection->Push(&Result_X, PUSH_POP_TIMEOUT, nullptr);
        if (mSocketThreadParam_X.Callback)
        {
          Sts_E = mSocketThreadParam_X.Callback(mCurrentOpParam_X.Ticket_U32, Sts_E);
        }
        // printf("%d: %s Alive Push Op %d Sts %s/%s Bal %d\n", BOF::Bof_GetMsTickCount(), mSocketThreadParam_X.Name_S.c_str(), Result_X.Operation_E, BOF::Bof_ErrorCode(Sts_E), BOF::Bof_ErrorCode(Result_X.Sts_E), BofThread::S_BofThreadBalance());
      }
    } // if ((NewCommandRcv_B) || (ListeningMode_B))
    if (NewCommandRcv_B)
    {
      // mCancel_B if executed after this !!!BOF_ASSERT!!!
      if (mpuSocketOperationParamCollection->Pop(&mCurrentOpParam_X, PollTimeout_U32, nullptr, nullptr) != BOF_ERR_NO_ERROR)
      {
        BOF_ASSERT(0);
      }
    }
    if (mCancel_B)
    {
      // printf("%d: %s Alive Cancel nb op %d res %d\n", BOF::Bof_GetMsTickCount(), mSocketThreadParam_X.Name_S.c_str(), IsThreadLoopMustExit(), mpuSocketOperationParamCollection->GetNbElement(), mpuSocketOperationResultCollection->GetNbElement());

      mpuSocketOperationParamCollection->Reset();
      mpuSocketOperationResultCollection->Reset();
      mCancel_B = false;
      Bof_SignalEvent(mCancelEvent_X, 0);
    }
  } while ((!IsThreadLoopMustExit()) && ((Rts_E == BOF_ERR_NO_ERROR) || (!NewCommandRcv_B)));
  /*
  if (mSocketThreadParam_X.pListeningSocket_O == mSocketThreadParam_X.pSocket_O)
  {
    if (mSocketThreadParam_X.pSocket_O)
    {
    }
  }
  */
  BOF_SAFE_DELETE(mSocketThreadParam_X.pListeningSocket_O);
  BOF_SAFE_DELETE(mSocketThreadParam_X.pSocket_O);

  // printf("%d: Exit with Rts %d sts %d Name %s Bal %d\n", BOF::Bof_GetMsTickCount(), Rts_E, Result_X.Sts_E, mSocketThreadParam_X.Name_S.c_str(), BofThread::S_BofThreadBalance());
  return Rts_E;
}
END_BOF_NAMESPACE()
