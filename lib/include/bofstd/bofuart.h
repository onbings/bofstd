/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines routines for creating and managing a uart communication
 * channel.
 *
 * Name:        BofUart.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  May 2 1999  BHA : Initial release
 */
#pragma once

#include <bofstd/bofcomchannel.h>
#include <bofstd/bofrawcircularbuffer.h>
#include <bofstd/bofthread.h>

#if defined(_WIN32)
#else
#include <termios.h> // POSIX terminal control definitions
#endif

BEGIN_BOF_NAMESPACE()

#define BOF_UART_NOPARITY 0
#define BOF_UART_ODDPARITY 1
#define BOF_UART_EVENPARITY 2
#define BOF_UART_MARKPARITY 3
#define BOF_UART_SPACEPARITY 4

#define BOF_UART_ONESTOPBIT 0
#define BOF_UART_ONE5STOPBITS 1
#define BOF_UART_TWOSTOPBITS 2

#if defined(_WIN32)
typedef void *UART_HANDLE;
#else
typedef int UART_HANDLE;
#endif

/*! Port Uart name
 * Each instanced Uart objects can be liked to one of these four serial communication ports:
 */

// dos     linux                  IO
// name     name       major minor address
// COM1    /dev/ttyS0  4,    64;   3F8
// COM2    /dev/ttyS1  4,    65;   2F8
// COM3    /dev/ttyS2  4,    66;   3E8
// COM4    /dev/ttyS3  4,    67;   2E8
#define BOF_UART_PORT1 1 /*! Serial port 1*/
#define BOF_UART_PORT2 2 /*! Serial port 2*/
#define BOF_UART_PORT3 3 /*! Serial port 3*/
#define BOF_UART_PORT4 4 /*! Serial port 4*/

const uint32_t MAXPORT = 4;

const uint32_t UART_LISTENCONNECT_SIGN = 0x6996AA55;
const uint32_t UART_LISTENCONNECT_SIGN_REV = 0x55AA9669;

struct BOFSTD_EXPORT BOF_UART_PARAM
{
  BOF_COM_CHANNEL_PARAM BaseChannelParam_X; // Base properties of each channel

  uint32_t Port_U32;       /*! Specifies the serial port number to open (1, 2, 3 or 4)*/
  uint32_t Baud_U32;       /*! Specifies the serial transmition speed in baud (9600,..)*/
  uint8_t Data_U8;         /*! Specifies the number of data bit per serial frame (COM_DATA8,..)*/
  uint8_t Parity_U8;       /*! Specifies the serial communication parity type (COM_NOPAR,..)*/
  uint8_t Stop_U8;         /*! Specifies the number of stop bit per serial frame (COM_SBIT1,..)*/
  char EvtChar_c;          /*! Specifies the 'event' char (0 if none)*/
  bool XonXoff_B;          /*! true if Xon/Xoff flow control must be used*/
  bool RtsCts_B;           /*! true if Rts/Cts flow control must be used*/
  bool DtrDsr_B;           /*! true if Dtr/Dsr flow control must be used*/
  bool SynchronousWrite_B; /*! write op are synchronous with caller else write op are processed in a dedicated thread*/
  uint64_t ThreadCpuCoreAffinity_U64;
  BOF_THREAD_SCHEDULER_POLICY ThreadSchedulerPolicy_E;
  BOF_THREAD_PRIORITY ThreadPriority_E; /*! Tx Thread priority */

  BOF_UART_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    BaseChannelParam_X.Reset();

    Port_U32 = 0;
    Baud_U32 = 0;
    Data_U8 = 0;
    Parity_U8 = 0;
    Stop_U8 = 0;
    EvtChar_c = 0;
    XonXoff_B = 0;
    RtsCts_B = false;
    DtrDsr_B = false;
    SynchronousWrite_B = false;
    ThreadCpuCoreAffinity_U64 = 0;
    ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
    ThreadPriority_E = BOF_THREAD_DEFAULT_PRIORITY;
  }
};

/*!
 * Class Uart
 * This class provides the interface for Windows serial hardware device. It is an
 * encapsulation of the Windows standard serial API:
 * - CreateFile
 * - CloseHandle
 * - PurgeComm
 * - EscapeCommFunction
 * - ClearCommError
 * - ReadFile
 * - WriteFile
 *
 * and provides the following interface:
 * - Set Rts and Dtr line state
 * - Get number of characters in input buffer
 * - Get number of characters in output buffer
 * - Read serial data
 * - Write data on serial port
 * - Purge serial data
 *
 */

class BOFSTD_EXPORT BofUart : public BofComChannel, public BofThread
{
private:
  BOF_UART_PARAM mUartParam_X;
  UART_HANDLE mId_h; /*! Serial device handle*/
  bool mOpen_B;
  char mpTTYDeviceName_c[128];
  BofRawCircularBuffer *mpTxData_O;

public:
  BofUart(const BOF_UART_PARAM &_rpUartParam_X);

  virtual ~BofUart();

  bool IsPortOpened()
  {
    return mOpen_B;
  }

  BOFERR V_Connect(uint32_t _TimeoutInMs_U32, const std::string &_rTarget_S, const std::string &_rOption_S) override;

  BofComChannel *V_Listen(uint32_t _TimeoutInMs_U32, const std::string &_rOption_S) override;

  BOFERR V_ReadData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, uint8_t *pBuffer_U8) override;

  BOFERR V_WriteData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, const uint8_t *pBuffer_U8) override;

  BOFERR V_WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S, uint32_t &_rNb_U32) override;

  BOFERR V_GetStatus(BOF_COM_CHANNEL_STATUS &_rStatus_X) override;

  BOFERR V_FlushData(uint32_t _TimeoutInMs_U32) override;

  BOFERR V_WaitForDataToRead(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPendingByte_U32) override;

  BofUart &operator=(const BofUart &) = delete; // Disallow copying
  BofUart(const BofUart &) = delete;

  BOFERR V_OnProcessing() override;

  BOFERR ReadUntilString(uint32_t TimeOut_U32, uint32_t &_rNb_U32, uint8_t *pBuffer_U8, uint8_t *pSearch_U8);

  bool SetRtsDtrState(uint8_t RtsState_U8, uint8_t DtrState_U8);

  UART_HANDLE GetDeviceHandle()
  {
    return mId_h;
  }

  uint32_t GetDeviceNumber();

  uint32_t GetBaudRateValue(uint32_t _BaudRate_U32);

private:
  bool WriteSynchronous(const uint8_t *_pBuf_U8, uint32_t _Size_U32);

#if defined(_WIN32)
#else

  bool SetBaudRateValue(struct termios *_ptty_X, uint32_t _BaudRate_U32);

#endif
};
END_BOF_NAMESPACE()