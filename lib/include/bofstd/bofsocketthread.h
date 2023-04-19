/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the IBofSocketIo interface.
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

#include <bofstd/bofcircularbuffer.h>
#include <bofstd/bofsocket.h>

BEGIN_BOF_NAMESPACE()
enum BOF_SOCKET_OPERATION
{
  BOF_SOCKET_OPERATION_LISTEN = 0,
  BOF_SOCKET_OPERATION_CONNECT,
  BOF_SOCKET_OPERATION_READ,
  BOF_SOCKET_OPERATION_WRITE,
  BOF_SOCKET_OPERATION_DISCONNECT,
  BOF_SOCKET_OPERATION_MAX
};

struct BOF_SOCKET_THREAD_PARAM
{
  std::string Name_S;
  uint32_t NbMaxOperationPending_U32;
  BOF_THREAD_SCHEDULER_POLICY ThreadSchedulerPolicy_E;
  BOF_THREAD_PRIORITY ThreadPriority_E;

  BOF_SOCKET_THREAD_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    Name_S = "";
    NbMaxOperationPending_U32 = 0;
    ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_MAX;
    ThreadPriority_E = BOF_THREAD_PRIORITY::BOF_THREAD_NONE;
  }
};

struct BOF_SOCKET_LISTEN_PARAM
{
  BOF::BOF_IPV4_ADDR_U32 SrcIpAddr_X;
  uint16_t SrcPort_U16;
  uint32_t NbMaxClient_U32;
  // uint32_t Flag_U32;
  //  uint32_t *pUdpDataSizeMax_U32;
  //  uint8_t *pUdpData_U8;
  BOF_SOCKET_LISTEN_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    SrcIpAddr_X.Reset();
    SrcPort_U16 = 0;
    NbMaxClient_U32 = 0;
    // Flag_U32 = 0;
  }
};

struct BOF_SOCKET_CONNECT_PARAM
{
  bool Tcp_B;
  BOF::BOF_IPV4_ADDR_U32 SrcIpAddr_X;
  uint16_t SrcPort_U16;
  BOF::BOF_IPV4_ADDR_U32 DstIpAddr_X;
  uint16_t DstPort_U16;
  //  uint32_t UdpDataSize_U32;
  //  uint8_t *pUdpData_U8;
  BOF_SOCKET_CONNECT_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    Tcp_B = false;
    SrcIpAddr_X.Reset();
    SrcPort_U16 = 0;
    DstIpAddr_X.Reset();
    DstPort_U16 = 0;
  }
};

struct BOF_SOCKET_READ_PARAM
{
  BofSocket *pDstSocket_O;
  uint32_t Nb_U32;
  uint8_t *pBuffer_U8;
  BOF_SOCKET_READ_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    pDstSocket_O = nullptr;
    Nb_U32 = 0;
    pBuffer_U8 = nullptr;
  }
};

struct BOF_SOCKET_WRITE_PARAM
{
  BofSocket *pDstSocket_O;
  uint32_t Nb_U32;
  uint8_t *pBuffer_U8;
  BOF_SOCKET_WRITE_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    pDstSocket_O = nullptr;
    Nb_U32 = 0;
    pBuffer_U8 = nullptr;
  }
};

struct BOF_SOCKET_DISCONNECT_PARAM
{
  BofSocket *pListenSocket_O; // Can be null
  BofSocket *pDstSocket_O;    // Can be null
  BOF_SOCKET_DISCONNECT_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    pListenSocket_O = nullptr;
    pDstSocket_O = nullptr;
  }
};
struct BOF_SOCKET_OPERATION_RESULT
{
  BOF_SOCKET_OPERATION Operation_E;
  uint32_t OpTicket_U32;
  BOFERR Sts_E;               /*! The operation status */
  BofSocket *pListenSocket_O; // Created socket
  BofSocket *pDstSocket_O;    // Created socket
  uint32_t Size_U32;          /*! The total size transferred in bytes */
  uint8_t *pBuffer_U8;
  uint32_t Time_U32; /*! The operation time (in ticks) */
  BOF_SOCKET_OPERATION_RESULT()
  {
    Reset();
  }
  void Reset()
  {
    Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_MAX;
    OpTicket_U32 = 0;
    Sts_E = BOF_ERR_NOT_INIT;
    Size_U32 = 0;
    pBuffer_U8 = nullptr;
    Time_U32 = 0;
    pListenSocket_O = nullptr;
    pDstSocket_O = nullptr;
  }
};

struct BOF_SOCKET_OPERATION_PARAM
{
  BOF_SOCKET_OPERATION Operation_E; /*! The operation to perform */
  uint32_t Ticket_U32;
  uint32_t TimeOut_U32;
  uint32_t Timer_U32; /*! The associated timer */
                      // Using an union create problem: warning C4624: 'onbings::bof::BOF_SOCKET_OPERATION_PARAM': destructor was implicitly defined as deleted
  // So I inline them
  BOF_SOCKET_LISTEN_PARAM Listen_X;   /*! The listen operation params */
  BOF_SOCKET_CONNECT_PARAM Connect_X; /*! The connect operation params */
  BOF_SOCKET_READ_PARAM Read_X;       /*! The read operation params */
  BOF_SOCKET_WRITE_PARAM Write_X;     /*! The write operation params */
  BOF_SOCKET_DISCONNECT_PARAM Disconnect_X;
  BOF_SOCKET_OPERATION_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    Operation_E = BOF_SOCKET_OPERATION::BOF_SOCKET_OPERATION_MAX;
    Ticket_U32 = 0;
    TimeOut_U32 = 0;
    Timer_U32 = 0;
    Listen_X.Reset();
    Connect_X.Reset();
    Read_X.Reset();
    Write_X.Reset();
    Disconnect_X.Reset();
  }
};

class BOFSTD_EXPORT BofSocketThread : public BofThread
{
private:
  std::unique_ptr<BofCircularBuffer<BOF_SOCKET_OPERATION_PARAM>> mpuSocketOperationParamCollection = nullptr;   /*! The operation params */
  std::unique_ptr<BofCircularBuffer<BOF_SOCKET_OPERATION_RESULT>> mpuSocketOperationResultCollection = nullptr; /*! The operation result */
  bool mExit_B = false;                                                                                         /*! The exit flag */

public:
  BofSocketThread(const BOF_SOCKET_THREAD_PARAM &_rSocketThreadParam_X);
  virtual ~BofSocketThread();

  BOFERR ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_LISTEN_PARAM &_rParam_X, uint32_t &_rOpTicket_U32);
  BOFERR ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_CONNECT_PARAM &_rParam_X, uint32_t &_rOpTicket_U32);
  BOFERR ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_READ_PARAM &_rParam_X, uint32_t &_rOpTicket_U32);
  BOFERR ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_WRITE_PARAM &_rParam_X, uint32_t &_rOpTicket_U32);
  BOFERR ProgramSocketOperation(uint32_t _TimeOut_U32, BOF_SOCKET_DISCONNECT_PARAM &_rParam_X, uint32_t &_rOpTicket_U32);

  bool GetSocketOperationResult(uint32_t _TimeOut_U32, BOF_SOCKET_OPERATION_RESULT &_rOperationResult_X);
  uint32_t NumberOfOperationPending();
  uint32_t NumberOfResultPending();

  BOFERR CancelAllOperation();

protected:
  BOFERR V_OnProcessing() override;

private:
  BOF_SOCKET_THREAD_PARAM mSocketThreadParam_X;
  uint32_t mTicket_U32 = 0;
  // BOF::BofSocket *mpServerSocket_O = nullptr;       /*! The server socket */
  // BOF::BofSocket *mpClientSocket_O = nullptr;       /*! The socket opened as a client */
  // bool mListenDone_B = false;                     /*! The socket was created from a listen */
  bool mCancelAllOperation_B = false;

  BofSocket *CreateSocket(bool _Tcp_B, BOF::BOF_IPV4_ADDR_U32 &_rIpAddr_X, uint16_t _Port_U16, uint32_t _NbMaxClient_U32); // 0 for normal socket !=0 for listening one _Listen_B)
};

END_BOF_NAMESPACE()
