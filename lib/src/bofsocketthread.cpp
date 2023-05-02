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
constexpr uint32_t IO_TIMEOUT = 250;       // All I/O (read/write) use this standard To value but the WHOLE I/O process is controlled by the Param_X.Timeout_U32

BofSocketThread::BofSocketThread(const BOF_SOCKET_THREAD_PARAM &_rSocketThreadParam_X) : BofThread()
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
      // printf("%d: Launch\n", BOF::Bof_GetMsTickCount());
      BOF_ASSERT(LaunchBofProcessingThread(mSocketThreadParam_X.Name_S, false, 0, mSocketThreadParam_X.ThreadSchedulerPolicy_E, mSocketThreadParam_X.ThreadPriority_E, 0, 2000, 0) == BOF_ERR_NO_ERROR);
      BOF_ASSERT(IsThreadRunning(100));
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
  _rOpTicket_U32 = (Rts_E == BOF_ERR_NO_ERROR) ? Param_X.Ticket_U32 : 0;
  return Rts_E;
}
BOFERR BofSocketThread::ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_CANCEL_PARAM &_rParam_X, uint32_t &_rOpTicket_U32)
{
  BOFERR Rts_E;
  BOF_SOCKET_OPERATION_PARAM Param_X;

  Param_X.Ticket_U32 = mTicket_U32;
  BOF_INC_TICKET_NUMBER(mTicket_U32);
  Param_X.TimeOut_U32 = _TimeOut_U32;
  Param_X.Timer_U32 = BOF::Bof_GetMsTickCount();
  Param_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CANCEL;
  Param_X.Cancel_X = _rParam_X;
  Rts_E = mpuSocketOperationParamCollection->Push(&Param_X, PUSH_POP_TIMEOUT, nullptr);
  _rOpTicket_U32 = (Rts_E == BOF_ERR_NO_ERROR) ? Param_X.Ticket_U32 : 0;
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
  _rOpTicket_U32 = (Rts_E == BOF_ERR_NO_ERROR) ? Param_X.Ticket_U32 : 0;
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
  _rOpTicket_U32 = (Rts_E == BOF_ERR_NO_ERROR) ? Param_X.Ticket_U32 : 0;
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
  _rOpTicket_U32 = (Rts_E == BOF_ERR_NO_ERROR) ? Param_X.Ticket_U32 : 0;
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
  _rOpTicket_U32 = (Rts_E == BOF_ERR_NO_ERROR) ? Param_X.Ticket_U32 : 0;
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
  //printf("================> Tim %d\n", Param_X.Timer_U32);
  Param_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT;
  Param_X.Disconnect_X = _rParam_X;
  Rts_E = mpuSocketOperationParamCollection->Push(&Param_X, PUSH_POP_TIMEOUT, nullptr);
  _rOpTicket_U32 = (Rts_E == BOF_ERR_NO_ERROR) ? Param_X.Ticket_U32 : 0;
  return Rts_E;
}

BOFERR BofSocketThread::GetSocketOperationResult(uint32_t _TimeOut_U32, BOF_SOCKET_OPERATION_RESULT &_rOperationResult_X)
{
  _rOperationResult_X.Reset();
  return mpuSocketOperationResultCollection->Pop(&_rOperationResult_X, _TimeOut_U32, nullptr, nullptr);
}

uint32_t BofSocketThread::NumberOfOperationPending()
{
  return mpuSocketOperationParamCollection->GetNbElement();
}
uint32_t BofSocketThread::NumberOfResultPending()
{
  return mpuSocketOperationResultCollection->GetNbElement();
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
  SocketParams_X.BindIpAddress_S = (_Tcp_B ? "tcp://" : "udp://") + _rIpAddr_X.ToString(_Port_U16);

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

BofSocket *BofSocketThread::GetListeningSocket()
{
  return mSocketThreadParam_X.pListeningSocket_O;
}

BofSocket *BofSocketThread::GetSocket()
{
  return mSocketThreadParam_X.pSocket_O;
}

BOFERR BofSocketThread::V_OnProcessing()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR, Sts_E;
  BOF_SOCKET_OPERATION_PARAM Param_X, PendingParam_X;
  BOF_SOCKET_OPERATION_RESULT Result_X;
  uint32_t Size_U32, Total_U32, Remain_U32, PollTimeout_U32, ListenTimeout_U32, ListenTicketId_U32;
  uint8_t *pBuffer_U8, *pCrtBuf_U8;
  bool ListeningMode_B, NewCommandRcv_B, SendResult_B;
  BofComChannel *pClient;
  std::string Target_S;
  BofSocket *pIoSocket_O;

  ListeningMode_B = false;
  PollTimeout_U32 = PUSH_POP_TIMEOUT; // Will be changed if we goes in ListeningMode_B
  do
  {
    printf("%d: Alive Wait %s for %d Bal %d ListenMode %d\n", BOF::Bof_GetMsTickCount(), mSocketThreadParam_X.Name_S.c_str(), PollTimeout_U32, BofThread::S_BofThreadBalance(), ListeningMode_B);
    Rts_E = mpuSocketOperationParamCollection->Pop(&Param_X, PollTimeout_U32, nullptr, nullptr);
    if ((Rts_E != BOF_ERR_ETIMEDOUT) && (Rts_E != BOF_ERR_NO_ERROR))
    {
      // printf("%d: End of Wait rts %d cmd %d\n", BOF::Bof_GetMsTickCount(), Rts_E, Param_X.Operation_E);
    }
    // Check if a new cmd has been received or just command timeout on the queue
    NewCommandRcv_B = (Rts_E == BOF_ERR_NO_ERROR);
    printf("%d: Alive Wait err %d ListenMode %d NewCmd %d Op %d Ticket %d To %d\n", BOF::Bof_GetMsTickCount(), Rts_E, ListeningMode_B, NewCommandRcv_B, Param_X.Operation_E, Param_X.Ticket_U32, Param_X.TimeOut_U32);
    if ((NewCommandRcv_B) || (ListeningMode_B))
    {
      // First init of operation param
      Result_X.Reset();
      Result_X.Sts_E = BOF_ERR_NO_ERROR;
      Result_X.Operation_E = Param_X.Operation_E;
      Result_X.OpTicket_U32 = Param_X.Ticket_U32;
      Result_X.Time_U32 = BOF::Bof_ElapsedMsTime(Param_X.Timer_U32);

      // So ListeningMode_B is perhaps true, but nevertheless a new command must be processed
      SendResult_B = false;
      if (NewCommandRcv_B)
      {
        Total_U32 = 0;
        pBuffer_U8 = nullptr;
        SendResult_B = true;

        switch (Param_X.Operation_E)
        {
          case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT:
            // printf("%d: BOF_SOCKET_OPERATION_EXIT\n", BOF::Bof_GetMsTickCount());
            Result_X.Sts_E = BOF_ERR_CANCEL; // To skip if below
            Rts_E = BOF_ERR_CANCEL;          // To exit loop here and in BofThread:
            break;

          case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_READ:
            if (!mSocketThreadParam_X.pSocket_O) // If no previous connect, we take the socket from the optional param and make it equal to mpSocket_O
            {
              pIoSocket_O = Param_X.Read_X.pSocket_O;
              mSocketThreadParam_X.pSocket_O = pIoSocket_O;
            }
            else // If something is defined in the optional param we use it without impacting mpSocket_O
            {
              pIoSocket_O = Param_X.Read_X.pSocket_O ? Param_X.Read_X.pSocket_O : mSocketThreadParam_X.pSocket_O;
            }
            if ((pIoSocket_O) && (Param_X.Read_X.pBuffer_U8))
            {
              Total_U32 = Param_X.Read_X.Nb_U32;
              pBuffer_U8 = Param_X.Read_X.pBuffer_U8;
              Result_X.pSocket_O = pIoSocket_O;
            }
            else
            {
              Result_X.Sts_E = BOF_ERR_EINVAL;
            }
            break;

          case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_WRITE:
            if (!mSocketThreadParam_X.pSocket_O) // If no previous connect, we take the socket from the optional param and make it equal to mpSocket_O
            {
              pIoSocket_O = Param_X.Write_X.pSocket_O;
              mSocketThreadParam_X.pSocket_O = pIoSocket_O;
            }
            else // If something is defined in the optional param we use it without impacting mpSocket_O
            {
              pIoSocket_O = Param_X.Write_X.pSocket_O ? Param_X.Read_X.pSocket_O : mSocketThreadParam_X.pSocket_O;
            }
            if ((pIoSocket_O) && (Param_X.Write_X.pBuffer_U8))
            {
              Total_U32 = Param_X.Write_X.Nb_U32;
              pBuffer_U8 = Param_X.Write_X.pBuffer_U8;
              Result_X.pSocket_O = pIoSocket_O;
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
            else
            {
              if (!ListeningMode_B) // Already in listen (JustOnce_B was false)
              {
                if (!mSocketThreadParam_X.pListeningSocket_O) // Specified in the constructor ?
                {
                  mSocketThreadParam_X.pListeningSocket_O = CreateSocket(true, Param_X.Listen_X.SrcIpAddr_X, Param_X.Listen_X.SrcPort_U16, Param_X.Listen_X.NbMaxClient_U32);
                }
                if (mSocketThreadParam_X.pListeningSocket_O)
                {
                  if (Param_X.Listen_X.JustOnce_B)
                  {
                    ListenTimeout_U32 = Param_X.TimeOut_U32;
                  }
                  else
                  {
                    // If we want to create an "permanent" listen socket to manage incoming connect, we first need to answer to the LISTEM command
                    // and after, for each incoming connection we will send another message
                    Result_X.pSocket_O = mSocketThreadParam_X.pListeningSocket_O;
                    Result_X.Sts_E = mpuSocketOperationResultCollection->Push(&Result_X, PUSH_POP_TIMEOUT, nullptr);
                    if (Result_X.Sts_E == BOF_ERR_NO_ERROR)
                    {
                      ListenTimeout_U32 = Param_X.TimeOut_U32 / 2; // Param_X.TimeOut_U32;
                      PollTimeout_U32 = Param_X.TimeOut_U32 / 2;
                      Param_X.TimeOut_U32 = 0xFFFFFFFF;
                      ListenTicketId_U32 = Param_X.Ticket_U32;
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

          case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CANCEL:
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
        Param_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN;
        Param_X.Listen_X.JustOnce_B = true; // because ListeningMode_B is true
      }
      // A command has been received with good param->it can be executed -> A result will be sent to the caller
      if (Result_X.Sts_E == BOF_ERR_NO_ERROR) // A command has been received and it can be executed -> A sesult will be sent to the caller
      {
        // To be able to cancel a very long socket operation (read,write,..) we split it in segment of max IO_TIMEOUT ms
        do
        {
          switch (Param_X.Operation_E)
          {
            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CANCEL:
              mpuSocketOperationParamCollection->Reset();
              mpuSocketOperationResultCollection->Reset();
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN:
              if (ListeningMode_B)
              {
                Result_X.Sts_E = BOF_ERR_NO_ERROR;
              }
              else
              {
                Result_X.Sts_E = BOF_ERR_NO_CLIENT;
              }
              // Just to detect DISCONNECT extra command on listening for ever thread
              printf("%d: Alive Listen Wait %s for %d Bal %d\n", BOF::Bof_GetMsTickCount(), mSocketThreadParam_X.Name_S.c_str(), ListenTimeout_U32, BofThread::S_BofThreadBalance());
              pClient = mSocketThreadParam_X.pListeningSocket_O->V_Listen(ListenTimeout_U32, "");
              if (pClient)
              {
                Result_X.pSocket_O = dynamic_cast<BofSocket *>(pClient);
                if (Result_X.pSocket_O)
                {
                  SendResult_B = true;
                  //mSocketThreadParam_X.pSocket_O = mSocketThreadParam_X.pListeningSocket_O;
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
              //printf("----listen------------------------------------->id=%d\n", ListenTicketId_U32);
              printf("%d: Alive Listen Wait end %p SndRes %d\n", BOF::Bof_GetMsTickCount(), pClient, SendResult_B);
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT:
              Result_X.Sts_E = BOF_ERR_ENOMEM;
              Result_X.pSocket_O = CreateSocket(Param_X.Connect_X.Tcp_B, Param_X.Connect_X.SrcIpAddr_X, Param_X.Connect_X.SrcPort_U16, 0);
              if (Result_X.pSocket_O)
              {
                Target_S = (Param_X.Connect_X.Tcp_B ? "tcp://" : "udp://") + Param_X.Connect_X.DstIpAddr_X.ToString(Param_X.Connect_X.DstPort_U16);
                Result_X.Sts_E = Result_X.pSocket_O->V_Connect(Param_X.TimeOut_U32, Target_S, "");
                if (Result_X.Sts_E == BOF_ERR_NO_ERROR)
                {
                  mSocketThreadParam_X.pSocket_O = Result_X.pSocket_O;
                }
                else
                {
                  BOF_SAFE_DELETE(Result_X.pSocket_O);
                }
              }
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_READ:
              Size_U32 = Remain_U32;
              Result_X.Sts_E = pIoSocket_O->V_ReadData(IO_TIMEOUT, /*Param_X.TimeOut_U32*/ Size_U32, pCrtBuf_U8); // To be able to cancel a very long socket operation (read,write,..) we split it in segment of max IO_TIMEOUT ms
              // printf("%d: Read: Total %d Size %d Remain %d Sts %d\n", Bof_GetMsTickCount(), Total_U32, Size_U32, Remain_U32, Result_X.Sts_E);
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_WRITE:
              Size_U32 = Remain_U32;
              Result_X.Sts_E = pIoSocket_O->V_WriteData(IO_TIMEOUT, /*Param_X.TimeOut_U32*/ Size_U32, pCrtBuf_U8); // To be able to cancel a very long socket operation (read,write,..) we split it in segment of max IO_TIMEOUT ms
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT:
              if (ListeningMode_B)
              {
                Result_X.pSocket_O = mSocketThreadParam_X.pListeningSocket_O;
                ListeningMode_B = false;
                PollTimeout_U32 = PUSH_POP_TIMEOUT;
                //Result_X.Time_U32 = BOF::Bof_ElapsedMsTime(Param_X.Timer_U32);
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

          Result_X.Time_U32 = BOF::Bof_ElapsedMsTime(Param_X.Timer_U32);
          if (Result_X.Time_U32 > Param_X.TimeOut_U32)
          {
            Result_X.Sts_E = BOF_ERR_ETIMEDOUT;
          }
          else if (Result_X.Sts_E == BOF_ERR_ETIMEDOUT)
          {
            Result_X.Sts_E = BOF_ERR_NO_ERROR;
          }
          // printf("inner loop exit %d rem %d sts %d\n", IsThreadLoopMustExit(), Remain_U32, Result_X.Sts_E);
        } while ((!IsThreadLoopMustExit()) && (Remain_U32) && (Result_X.Sts_E == BOF_ERR_NO_ERROR));
      }
      if (SendResult_B)
      {
        Result_X.Size_U32 = Total_U32 - Remain_U32;
        Result_X.pBuffer_U8 = pBuffer_U8;
        Result_X.Time_U32 = BOF::Bof_ElapsedMsTime(Param_X.Timer_U32);
        Sts_E = mpuSocketOperationResultCollection->Push(&Result_X, PUSH_POP_TIMEOUT, nullptr);
        printf("%d: Alive Push Op %d Sts %d Bal %d\n", BOF::Bof_GetMsTickCount(), Result_X.Operation_E, Sts_E, BofThread::S_BofThreadBalance());
      }
    }
  } while ((!IsThreadLoopMustExit()) && ((Rts_E == BOF_ERR_NO_ERROR) || (!NewCommandRcv_B)));

  BOF_SAFE_DELETE(mSocketThreadParam_X.pListeningSocket_O);
  BOF_SAFE_DELETE(mSocketThreadParam_X.pSocket_O);

  // printf("%d: Exit with Rts %d sts %d Name %s Bal %d\n", BOF::Bof_GetMsTickCount(), Rts_E, Result_X.Sts_E, mSocketThreadParam_X.Name_S.c_str(), BofThread::S_BofThreadBalance());
  return Rts_E;
}
END_BOF_NAMESPACE()
