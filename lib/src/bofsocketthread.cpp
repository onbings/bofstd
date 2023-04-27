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
constexpr uint32_t PUSH_POP_TIMEOUT = 150;

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
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  BOF_SOCKET_OPERATION_PARAM Param_X, ListenParam_X;
  BOF_SOCKET_OPERATION_RESULT Result_X;
  uint32_t Size_U32, Total_U32, Remain_U32;
  uint8_t *pBuffer_U8, *pCrtBuf_U8;
  bool IsBlockingMode_B;
  BofComChannel *pClient;
  std::string Target_S;
  BofSocket *pIoSocket_O;

  do
  {
    //printf("%d: Wait %s Bal %d\n", BOF::Bof_GetMsTickCount(), mSocketThreadParam_X.Name_S.c_str(), BofThread::S_BofThreadBalance());
    Rts_E = mpuSocketOperationParamCollection->Pop(&Param_X, PUSH_POP_TIMEOUT, nullptr, nullptr);
    //printf("%d: Wait end %d\n", BOF::Bof_GetMsTickCount(), Rts_E);
    if ((Rts_E != BOF_ERR_ETIMEDOUT) && (Rts_E != BOF_ERR_NO_ERROR))
    {
      // printf("%d: End of Wait rts %d cmd %d\n", BOF::Bof_GetMsTickCount(), Rts_E, Param_X.Operation_E);
    }
    if (Rts_E == BOF_ERR_ETIMEDOUT)
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
    else if (Rts_E == BOF_ERR_NO_ERROR) // Or timeout
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
        case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT:
          // printf("%d: BOF_SOCKET_OPERATION_EXIT\n", BOF::Bof_GetMsTickCount());
          Result_X.Sts_E = BOF_ERR_CANCEL; // To skip if below
          Rts_E = BOF_ERR_CANCEL;          // To exit loop here and in BofThread:
          break;

        case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_READ:
          if (!mSocketThreadParam_X.pSocket_O)//If no previous connect, we take the socket from the optional param and make it equal to mpSocket_O
          {
            pIoSocket_O = Param_X.Read_X.pSocket_O;
            mSocketThreadParam_X.pSocket_O = pIoSocket_O;
          }
          else //If something is defined in the optional param we use it without impacting mpSocket_O
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
          if (!mSocketThreadParam_X.pSocket_O)//If no previous connect, we take the socket from the optional param and make it equal to mpSocket_O
          {
            pIoSocket_O = Param_X.Write_X.pSocket_O;
            mSocketThreadParam_X.pSocket_O = pIoSocket_O;
          }
          else //If something is defined in the optional param we use it without impacting mpSocket_O
          {
            pIoSocket_O = Param_X.Read_X.pSocket_O ? Param_X.Read_X.pSocket_O : mSocketThreadParam_X.pSocket_O;
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
            if (!mSocketThreadParam_X.pListeningSocket_O)    //Specified in the constructor ?
            {
              mSocketThreadParam_X.pListeningSocket_O = CreateSocket(true, Param_X.Listen_X.SrcIpAddr_X, Param_X.Listen_X.SrcPort_U16, Param_X.Listen_X.NbMaxClient_U32);
            }
            if (mSocketThreadParam_X.pListeningSocket_O)
            {
              if (Param_X.Listen_X.JustOnce_B)
              {
              }
              else
              {
                // If we want to create an "permanent" listen socket to manage incoming connect, we first need to answer to the LISTEM command
                // and after, for each incoming connection we will send another message
                Result_X.pSocket_O = mSocketThreadParam_X.pListeningSocket_O;
                Result_X.Sts_E = mpuSocketOperationResultCollection->Push(&Result_X, PUSH_POP_TIMEOUT, nullptr);
                if (Result_X.Sts_E == BOF_ERR_NO_ERROR)
                {
                  Total_U32 = 1; //->Total_U32 will be assigned to Remain_U32 with Size_U32 = 0 -> Activate the i/o loop for "ever"
                }
              }
            }
            else
            {
              Result_X.Sts_E = BOF_ERR_ENOMEM;
            }
          }
          break;

        case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CANCEL:
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
        do // To be able to cancel a very long socket operation (read,write,..) we split it in segment of max 250 ms
        {
          switch (Param_X.Operation_E)
          {
            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CANCEL:
              mpuSocketOperationParamCollection->Reset();
              mpuSocketOperationResultCollection->Reset();
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN:
              if (Param_X.Listen_X.JustOnce_B)
              {
                Result_X.Sts_E = BOF_ERR_NO_CLIENT;
              }
              else
              {
                Result_X.Sts_E = BOF_ERR_NO_ERROR;
                // Just to detect DISCONNECT extra command on listening for ever thread
                if (mpuSocketOperationParamCollection->Pop(&ListenParam_X, 0, nullptr, nullptr) == BOF_ERR_NO_ERROR)
                {
                  switch (ListenParam_X.Operation_E)
                  {
                    case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_EXIT:
                      Result_X.Sts_E = BOF_ERR_CANCEL; // To skip if below
                      Rts_E = BOF_ERR_CANCEL;          // To exit loop here and in BofThread:
                      Result_X.Operation_E = ListenParam_X.Operation_E;
                      Result_X.OpTicket_U32 = ListenParam_X.Ticket_U32;
                      Result_X.pSocket_O = nullptr;
                      Total_U32 = 0; // Wil exit i/o loop
                      Remain_U32 = 0;
                      break;

                    case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT:
                      //printf("%d: Cmd disc rcv %p/%p\n", BOF::Bof_GetMsTickCount(), ListenParam_X.Disconnect_X.pSocket_O, mpListenSocket_O);
                      //BOF_ASSERT(ListenParam_X.Disconnect_X.pSocket_O == mSocketThreadParam_X.pListenSocket_O);
                      BOF_SAFE_DELETE(mSocketThreadParam_X.pListeningSocket_O);
                      Result_X.Operation_E = ListenParam_X.Operation_E;
                      Result_X.OpTicket_U32 = ListenParam_X.Ticket_U32;
                      Total_U32 = 0; // Wil exit i/o loop
                      Remain_U32 = 0;
                      break;

                    default:
                      break;
                  }
                }
                else
                {
                  pClient = mSocketThreadParam_X.pListeningSocket_O->V_Listen(Param_X.TimeOut_U32, "");
                  if (pClient)
                  {
                    Result_X.pSocket_O = dynamic_cast<BofSocket *>(pClient);
                    if (Result_X.pSocket_O)
                    {
                      if (Param_X.Listen_X.JustOnce_B)
                      {
                        Result_X.Sts_E = BOF_ERR_NO_ERROR;
                      }
                      else
                      {
                        // In this case Remain_U32 = Total_U32 = 1; Size_U32=0
                        BOF_INC_TICKET_NUMBER(Result_X.OpTicket_U32);
                        Result_X.Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_CONNECT;
                        Result_X.Time_U32 = BOF::Bof_ElapsedMsTime(Param_X.Timer_U32);
                        Result_X.Sts_E = mpuSocketOperationResultCollection->Push(&Result_X, PUSH_POP_TIMEOUT, nullptr);
                        if (Result_X.Sts_E == BOF_ERR_NO_ERROR)
                        {
                          Param_X.Timer_U32 = Bof_GetMsTickCount();
                        }
                      }
                    }
                  }
                }
              }
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
              Result_X.Sts_E = pIoSocket_O->V_ReadData(250, /*Param_X.TimeOut_U32*/ Size_U32, pCrtBuf_U8); // To be able to cancel a very long socket operation (read,write,..) we split it in segment of max 250 ms
              //printf("%d: Read: Total %d Size %d Remain %d Sts %d\n", Bof_GetMsTickCount(), Total_U32, Size_U32, Remain_U32, Result_X.Sts_E);
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_WRITE:
              Size_U32 = Remain_U32;
              Result_X.Sts_E = pIoSocket_O->V_WriteData(250, /*Param_X.TimeOut_U32*/ Size_U32, pCrtBuf_U8); // To be able to cancel a very long socket operation (read,write,..) we split it in segment of max 250 ms
              break;

            case BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_DISCONNECT:
              BOF_SAFE_DELETE(mSocketThreadParam_X.pSocket_O);
              //printf("B==>S_BofSocketBalance %d S_BofThreadBalance %d\n", BOF::BofSocket::S_BofSocketBalance(), BOF::BofThread::S_BofThreadBalance());
              break;

            default:
              break;
          }

          Remain_U32 -= Size_U32;
          pCrtBuf_U8 += Size_U32;

          Result_X.Time_U32 = BOF::Bof_ElapsedMsTime(Param_X.Timer_U32);
          if (Param_X.Operation_E != BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_LISTEN)
          {
            if (Result_X.Time_U32 > Param_X.TimeOut_U32)
            {
              Result_X.Sts_E = BOF_ERR_ETIMEDOUT;
            }
            else if (Result_X.Sts_E == BOF_ERR_ETIMEDOUT)
            {
              Result_X.Sts_E = BOF_ERR_NO_ERROR;
            }
          }
          //printf("inner loop exit %d rem %d sts %d\n", IsThreadLoopMustExit(), Remain_U32, Result_X.Sts_E);
        } while ((!IsThreadLoopMustExit()) && (Remain_U32) && (Result_X.Sts_E == BOF_ERR_NO_ERROR));
      } 

      Result_X.Size_U32 = Total_U32 - Remain_U32;
      Result_X.pBuffer_U8 = pBuffer_U8;
      Result_X.Time_U32 = BOF::Bof_ElapsedMsTime(Param_X.Timer_U32);
      if (Param_X.Operation_E == BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_READ)
      {
        //printf("%d: End of Read: Total %d Size %d Remain %d -> ResSz %d Time %d ms\n", Bof_GetMsTickCount(), Total_U32, Size_U32, Remain_U32, Result_X.Size_U32, Result_X.Time_U32);
      }
      mpuSocketOperationResultCollection->Push(&Result_X, PUSH_POP_TIMEOUT, nullptr);
    }
    //printf("outer loop exit %d sts %d\n", IsThreadLoopMustExit(), Result_X.Sts_E);
  } while ((!IsThreadLoopMustExit()) && (Rts_E == BOF_ERR_NO_ERROR));
  BOF_SAFE_DELETE(mSocketThreadParam_X.pListeningSocket_O);
  BOF_SAFE_DELETE(mSocketThreadParam_X.pSocket_O);

  //printf("%d: Exit with Rts %d sts %d Name %s Bal %d\n", BOF::Bof_GetMsTickCount(), Rts_E, Result_X.Sts_E, mSocketThreadParam_X.Name_S.c_str(), BofThread::S_BofThreadBalance());
  return Rts_E;
}
END_BOF_NAMESPACE()
