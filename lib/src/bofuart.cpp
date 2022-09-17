/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module contains routines for creating and managing a uart communication
 * channel.
 *
 * Name:        BofUart.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  May 2 1999  BHA : Initial release
 */

/*** Include files ***********************************************************/
#include <bofstd/bofstring.h>
#include <bofstd/bofuart.h>

#include <fcntl.h>
#include <string.h>

#if defined (_WIN32)
#include <windows.h>
#else

#include <termio.h>
#include <unistd.h>
#include <linux/serial.h>

#endif

BEGIN_BOF_NAMESPACE()

/*** Prototypes  ********************************************************************/
uint32_t GetBaudRateConstantValue(uint32_t _BaudRate_U32);

/*** BofUart::BofUart ***************************************************************/

/*
 * This function opens and setup a serial port.
 *
 * Description
 * This is the constructor of a BofUart
 *
 * Parameters
 * _pUartParam_X:  Specifies the object creation parameters and contains the retuned values.
 *
 * Returns
 * None
 *
 * Remarks
 * None
 *
 * See Also
 * ~BofCircularBuffer
 */
BofUart::BofUart(const BOF_UART_PARAM &_rUartParam_X)
	: BofComChannel(BOF_COM_CHANNEL_TYPE::TUART, mUartParam_X.BaseChannelParam_X),
	  BofThread()                 // , 0, _rUartParam_X.SynchronousWritePriority_U32, 1000, (uint32_t)-1)
// Set startStopTimeout to 0 to prevent time loss when closing port
// BofThread("BofUart", _rUartParam_X.SynchronousWritePriority_U32 ? SCHED_FIFO:SCHED_OTHER, _rUartParam_X.SynchronousWritePriority_U32, 0, -1)
{
	uint32_t i_U32;
	BOF_RAW_CIRCULAR_BUFFER_PARAM BofRawCircularBufferParam_X;

	mUartParam_X = _rUartParam_X;
	mpTxData_O = nullptr;
	mOpen_B = false;
  BofComChannel::mErrorCode_E = BOF_ERR_INIT;

#if defined (_WIN32)
	mId_h        = BOF_INVALID_HANDLE_VALUE;
#else
	UART_HANDLE TTYDeviceFileHandle_h = -1;
	struct termios tty_X;
	bool ConfigOK_B = true;
	mId_h = (UART_HANDLE) (-1);
#endif

	i_U32 = (mUartParam_X.Port_U32 - BOF_UART_PORT1);

	if (i_U32 < MAXPORT)
	{
		if (!mOpen_B)
		{
#if defined (_WIN32)
			char pWork_c[32];
			DCB   Dcb_X;
			sprintf(pWork_c, "COM%d:", i_U32 + 1);
			mId_h = CreateFileA(pWork_c, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (mId_h != BOF_INVALID_HANDLE_VALUE)
			{
				CloseHandle(mId_h);

				mId_h = CreateFileA(pWork_c, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

				// DBGOUTPUT(TEXT("BofUart::OPEN CreateFile1: %X\r\n"),mId_h);
				if (mId_h != BOF_INVALID_HANDLE_VALUE)
				{
					memset(&Dcb_X, 0, sizeof(Dcb_X) );
					Dcb_X.DCBlength     = sizeof(DCB);
					Dcb_X.BaudRate      = GetBaudRateConstantValue(mUartParam_X.Baud_U32);

					Dcb_X.fBinary       = true;
					Dcb_X.fParity = (mUartParam_X.Parity_U8 != BOF_UART_NOPARITY);
					Dcb_X.ByteSize      = mUartParam_X.Data_U8;
					Dcb_X.Parity        = mUartParam_X.Parity_U8;
					Dcb_X.StopBits      = mUartParam_X.Stop_U8;

					if (mUartParam_X.RtsCts_B)
					{
						Dcb_X.fOutxCtsFlow = true;
						Dcb_X.fRtsControl  = RTS_CONTROL_HANDSHAKE;
					}
					else
					{
						Dcb_X.fOutxCtsFlow = false;
						Dcb_X.fRtsControl  = RTS_CONTROL_DISABLE;
					}

					if (mUartParam_X.DtrDsr_B)
					{
						Dcb_X.fDtrControl     = DTR_CONTROL_HANDSHAKE;
						Dcb_X.fOutxDsrFlow    = true;
						Dcb_X.fDsrSensitivity = true;
					}
					else
					{
						Dcb_X.fDtrControl     = DTR_CONTROL_DISABLE;
						Dcb_X.fOutxDsrFlow    = false;
						Dcb_X.fDsrSensitivity = false;
					}

					if (mUartParam_X.XonXoff_B)
					{
						Dcb_X.fTXContinueOnXoff = true;
						Dcb_X.XonLim            = (uint16_t)(mUartParam_X.BaseChannelParam_X.RcvBufferSize_U32 / 3);
						Dcb_X.XoffLim           = (uint16_t)(mUartParam_X.BaseChannelParam_X.RcvBufferSize_U32 / 3);
						Dcb_X.XonChar           = 0x10;
						Dcb_X.XoffChar          = 0x13;
						Dcb_X.fOutX             = true;
						Dcb_X.fInX              = true;
					}

					Dcb_X.fErrorChar    = false; // for parity error detection
					Dcb_X.fNull         = false;
					Dcb_X.fAbortOnError = false;
					Dcb_X.wReserved     = 0;
					Dcb_X.ErrorChar     = 0;
					Dcb_X.EofChar       = 0x00;
					Dcb_X.EvtChar       = mUartParam_X.EvtChar_c;

					// DBGOUTPUT(TEXT("BofUart::OPEN: SetCommState\r\n"));
					if (SetCommState(mId_h, &Dcb_X) )
					{
						// DBGOUTPUT(TEXT("BofUart::OPEN: SetupComm\r\n"));
						if (SetupComm(mId_h, mUartParam_X.BaseChannelParam_X.RcvBufferSize_U32, mUartParam_X.BaseChannelParam_X.SndBufferSize_U32) )
						{
							/*test app (BHA)
							 * char p[128];
							 * int i,n;
							 * uint32_t nb,Error_DW;
							 * bool s;
							 * COMSTAT Status_X;
							 *
							 * for (i=0;i<100000;i++)
							 * {
							 *
							 *      n=sprintf(p,"Hello world %d",i);
							 * s=WriteFile(mId_h,p,n,&nb,nullptr);
							 *
							 * s=ClearCommError(mId_h,&Error_DW,&Status_X);
							 *      Sleep(1);
							 *      if (Status_X.cbInQue)
							 *      {
							 *              n=Status_X.cbInQue;
							 * s=ReadFile(mId_h,p,n,&nb,nullptr);
							 *      }
							 * }
							 *
							 */
							if (mUartParam_X.SynchronousWrite_B)
							{
								mpTxData_O = nullptr;
								mOpen_B    = true;
							}
							else
							{
								BofRawCircularBufferParam_X.Reset();
								BofRawCircularBufferParam_X.MultiThreadAware_B   = true;
								BofRawCircularBufferParam_X.BufferSizeInByte_U32 = mUartParam_X.BaseChannelParam_X.SndBufferSize_U32;
								BofRawCircularBufferParam_X.NbMaxSlot_U32        = 0;
								BofRawCircularBufferParam_X.pData_U8             = nullptr;
								mpTxData_O                                       = new BofRawCircularBuffer(BofRawCircularBufferParam_X);

								if (mpTxData_O)
								{
																		//LaunchThread(false, 0, 0x40000, _rUartParam_X.ThreadCpuCoreAffinity_U32, _rUartParam_X.SynchronousWritePriority_U32 ? BOF_THREAD_SCHEDULER_POLICY_FIFO : BOF_THREAD_SCHEDULER_POLICY_OTHER, _rUartParam_X.SynchronousWritePriority_U32, 1000);
									LaunchBofProcessingThread("BofUart", false, 0,  _rUartParam_X.ThreadSchedulerPolicy_E, _rUartParam_X.ThreadPriority_E, _rUartParam_X.ThreadCpuCoreAffinity_U64, 1000, 0x1000);
									mOpen_B = true;
								}
							}
						}
					}
				}
			}
#else
			/* Open device /dev/ttySx */
			snprintf(mpTTYDeviceName_c, sizeof(mpTTYDeviceName_c) - 1, "/dev/ttyS%d", i_U32);
			TTYDeviceFileHandle_h = open(mpTTYDeviceName_c, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);

			if (TTYDeviceFileHandle_h < 0)
			{
// printf("Failed to open device %s: %s\n", mpTTYDeviceName_c, strerror(errno) );
			}
			else
			{
				if (!isatty(TTYDeviceFileHandle_h))
				{
					// printf("%s is not a tty device: %s\n", mpTTYDeviceName_c, strerror(errno) );
				}
				else
				{
					mId_h = TTYDeviceFileHandle_h;
				}
			}

			if (mId_h != (UART_HANDLE) (-1))
			{
				memset(&tty_X, 0, sizeof tty_X);

				/* Error Handling */
				if (tcgetattr(mId_h, &tty_X) != 0)
				{
// printf("Failed to get device %s attributes: %s\n", mpTTYDeviceName_c, strerror(errno) );
				}
				else
				{
					/* Set Baud Rate */
					if (SetBaudRateValue(&tty_X, mUartParam_X.Baud_U32) == false)
					{
// printf("Failed to set device %s baud rate: %s\n", mpTTYDeviceName_c, strerror(errno) );
					}
					else
					{
						/* General default settings */
						tty_X.c_lflag &= ~(ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHONL | NOFLSH | TOSTOP | IEXTEN);
						tty_X.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR | PARMRK | ICRNL | IGNCR | INLCR | IXON | IXOFF | IXANY | INPCK | ISTRIP);
						tty_X.c_oflag &= ~(OPOST | ONLCR | OCRNL | ONOCR | ONLRET | OFILL | NLDLY | CRDLY | TABDLY | BSDLY | VTDLY | FFDLY);
						tty_X.c_cflag &= ~(HUPCL | CRTSCTS);
						tty_X.c_cflag |= (CLOCAL | CREAD);

						/* Initialize all control characters TODO ??? */

						// tty.c_cc[VINTR] = 0;     /* Ctrl-c */
						// tty.c_cc[VQUIT] = 0;     /* Ctrl-\ */
						// tty.c_cc[VERASE] = 0;     /* del */
						// tty.c_cc[VKILL] = 0;     /* @ */
						// tty.c_cc[VEOF] = 4;     /* Ctrl-d */
						// tty.c_cc[VSWTC] = 0;     /* '\0' */
						// tty.c_cc[VSTART] = 0;     /* Ctrl-q */
						// tty.c_cc[VSTOP] = 0;     /* Ctrl-s */
						// tty_X.c_cc[VSUSP] = 0;     /* Ctrl-z */
						// tty_X.c_cc[VEOL] = 0;     /* '\0' */
						// tty_X.c_cc[VREPRINT] = 0;     /* Ctrl-r */
						// tty_X.c_cc[VDISCARD] = 0;     /* Ctrl-u */
						// tty_X.c_cc[VWERASE] = 0;     /* Ctrl-w */
						// tty_X.c_cc[VLNEXT] = 0;     /* Ctrl-v */
						// tty_X.c_cc[VEOL2] = 0;     /* '\0' */

						/* One input byte is enough to return from read()
						 * Note that opening with O_NONBLOCK cause read to return even if nothing to read */
						tty_X.c_cc[VMIN] = 1;
						/* Inter-character timer off */
						tty_X.c_cc[VTIME] = 0;

						/* EvtChar settings */
						if (mUartParam_X.EvtChar_c != 0)
						{
							ConfigOK_B = false;
// printf("Failed to set device %s event char 0x%x \n", mpTTYDeviceName_c, mUartParam_X.EvtChar_c);
						}

						/* Parity settings */
						switch (mUartParam_X.Parity_U8)
						{
							case (BOF_UART_NOPARITY):
							{
								/* Disable parity checking */
								tty_X.c_cflag &= ~(PARENB);
								break;
							}

							case (BOF_UART_ODDPARITY):
							{
								/* Enable odd parity checking */
								tty_X.c_iflag |= (INPCK | ISTRIP);
								tty_X.c_cflag |= (PARENB | PARODD);
								break;
							}

							case (BOF_UART_EVENPARITY):
							{
								/* Enable even parity checking */
								tty_X.c_iflag |= (INPCK | ISTRIP);
								tty_X.c_cflag |= (PARENB);
								tty_X.c_cflag &= ~(PARODD);
								break;
							}

							default:
							{
								ConfigOK_B = false;
// printf("Failed to set device %s parity %d settings\n", mpTTYDeviceName_c, mUartParam_X.Parity_U8);
								break;
							}
						}

						/* Data bits settings */
						tty_X.c_cflag &= ~CSIZE;   /* Mask the character size bits */

						switch (mUartParam_X.Data_U8)
						{
							case 5:
							{
								tty_X.c_cflag |= CS5;
								break;
							}                        /* Select 5 data bits */

							case 6:
							{
								tty_X.c_cflag |= CS6;
								break;
							}                        /* Select 6 data bits */

							case 7:
							{
								tty_X.c_cflag |= CS7;
								break;
							}                        /* Select 7 data bits */

							case 8:
							{
								tty_X.c_cflag |= CS8;
								break;
							}                        /* Select 8 data bits */

							default:
							{
								ConfigOK_B = false;
// printf("Failed to set device %s data bits %d settings\n", mpTTYDeviceName_c, mUartParam_X.Data_U8);
								break;
							}
						}

						/* Stop settings */
						switch (mUartParam_X.Stop_U8)
						{
							case BOF_UART_ONESTOPBIT:
							{
								tty_X.c_cflag &= ~CSTOPB;
								break;
							}                        /* One Stop bit  */

							case BOF_UART_TWOSTOPBITS:
							{
								tty_X.c_cflag |= CSTOPB;
								break;
							}                        /* Two Stop bits */

							default:
							{
								ConfigOK_B = false;
// printf("Failed to set device %s stop bits %d settings\n", mpTTYDeviceName_c, mUartParam_X.Stop_U8);
								break;
							}
						}

						/* XonXoff settings */
						if (mUartParam_X.XonXoff_B)
						{
							tty_X.c_iflag |= (IXON | IXOFF | IXANY);
							tty_X.c_cc[VSTART] = 0x11;
							tty_X.c_cc[VSTOP] = 0x13;
						}

						/* RtsCts settings */
						if (mUartParam_X.RtsCts_B)
						{
							tty_X.c_cflag |= CRTSCTS;
						}

						/* DtrDsr settings */
						if (mUartParam_X.DtrDsr_B)
						{
							ConfigOK_B = false;      /* unsupported on linux */
						}

						if (ConfigOK_B)
						{
							/* Flush in and ou ports, then applies attributes */
							tcflush(mId_h, TCIOFLUSH);

							if (tcsetattr(mId_h, TCSANOW, &tty_X) != 0)
							{
// printf("Failed to set device %s attributes: %s\n", mpTTYDeviceName_c, strerror(errno) );
							}
							else
							{
								/* Setting Output buffer */
								BofRawCircularBufferParam_X.Reset();
							}

							if (mUartParam_X.SynchronousWrite_B)
							{
								mpTxData_O = nullptr;
								mOpen_B = true;
							}
							else
							{
								BofRawCircularBufferParam_X.Reset();
								BofRawCircularBufferParam_X.MultiThreadAware_B = true;
								BofRawCircularBufferParam_X.BufferSizeInByte_U32 = mUartParam_X.BaseChannelParam_X.SndBufferSize_U32;
								BofRawCircularBufferParam_X.NbMaxSlot_U32 = 0;
								BofRawCircularBufferParam_X.pData_U8 = nullptr;
								mpTxData_O = new BofRawCircularBuffer(BofRawCircularBufferParam_X);

								/* Setting Output buffer */
								if (mpTxData_O)
								{
									LaunchBofProcessingThread("BofUart", false, 0, _rUartParam_X.ThreadSchedulerPolicy_E, _rUartParam_X.ThreadPriority_E,_rUartParam_X.ThreadCpuCoreAffinity_U64, 1000,  0x10000);
									mOpen_B = true;
								}
							}
						}
					}
				}
			}
#endif
		}
	}

	// There's no need for thread when
	// using synchronous writesStop the thread i
	if (mUartParam_X.SynchronousWrite_B)
	{
//		StopThread();
	}

	if (mOpen_B)
	{
    BofComChannel::mErrorCode_E = BOF_ERR_NO_ERROR;
	}
}


/*** BofUart::~BofUart **************************************************************/

/*!\internal
 * This function closes a previously opened serial port
 *
 * @remark             none
 *
 */

BofUart::~BofUart()
{
// BOF_COM_CHANNEL_STATUS Status_X;
// uint32_t                 Start_U32, Delta_U32;

	if (mOpen_B)
	{
		/* NO need to purge as we close the port !!!! (can block ittask for 1 sec)
		 * Start_U32 = Bof_GetMsTickCount();
		 *
		 * do
		 * {
		 *   V_GetStatus(&Status_X);
		 *   Bof_MsSleep(0);
		 *   Delta_U32 = Bof_ElapsedMsTime(Start_U32);
		 * }
		 * while ( ( Status_X.NbOut_U32 )
		 *         && ( Delta_U32 < 1000 )
		 *         );
		 */

		// Stop the thread if needed
//done in destructorof bofthread		StopThread();

#if defined (_WIN32)
		CloseHandle(mId_h);
#else
		close(mId_h);
#endif
		mOpen_B = false;
	}
	BOF_SAFE_DELETE(mpTxData_O);
}


/*** BofUart::V_PurgeData **************************************************************/

/*!
 * This function discards all characters from the serial input buffer.
 *
 * @param              _TimeoutInMs_U32
 *
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * @remark             none
 */
BOFERR BofUart::V_FlushData(uint32_t _TimeoutInMs_U32)
{
	BOFERR Rts_E = BOF_ERR_PURGE;

#if defined (_WIN32)
	if (PurgeComm(mId_h, PURGE_RXCLEAR) )
	{
		Rts_E = BOF_ERR_NO_ERROR;
	}
#else
	BOF_COM_CHANNEL_STATUS Status_X;
	uint32_t Start_U32, Delta_U32;

	if (mOpen_B)
	{
		Start_U32 = Bof_GetMsTickCount();

		do
		{
			V_GetStatus(Status_X);
      Bof_MsSleep(0);
			Delta_U32 = Bof_ElapsedMsTime(Start_U32);
			tcflush(mId_h, TCIFLUSH);
		} while ((Status_X.NbOut_U32) && (Delta_U32 < _TimeoutInMs_U32));
	}
	Rts_E = BOF_ERR_NO_ERROR;
#endif
	return Rts_E;
}

BOFERR BofUart::V_WaitForDataToRead(uint32_t /*_TimeoutInMs_U32*/, uint32_t &/*_rNbPendingByte_U32*/)
{
	return BOF_ERR_NOT_SUPPORTED;
}

/*** BofUart::SetRtsDtrState *****************************************************/

/*!\internal
 * This function clears or sets the Rts and Dtr serial signal line.
 *
 * @param RtsState_U8 : Specifies the if Rts signal must be
 *                      - 0=Turned off
 *                      - 1=Turned on
 *                      - 2=Unchanged
 *
 * @param DtrState_U8 : Specifies the if Dtr signal must be
 *                      - 0=Turned off
 *                      - 1=Turned on
 *                      - 2=Unchanged
 *
 * @return bool  :     The status of the function execution:
 *                     - true indicates success.
 *                     - false indicates failure.
 *
 * @remark             none
 *
 * \warning If you use the OS to execute the request, it can take 25 mS under Windows CE
 */
bool BofUart::SetRtsDtrState(uint8_t RtsState_U8, uint8_t DtrState_U8)
{
	bool Rts_B = false;

	if (mOpen_B)
	{
#if defined (_WIN32)
		if (DtrState_U8 == 1)
		{
			if (EscapeCommFunction(mId_h, SETDTR) )
			{
				Rts_B = true;
			}
		}
		else if (DtrState_U8 == 0)
		{
			if (EscapeCommFunction(mId_h, CLRDTR) )
			{
				Rts_B = true;
			}
		}
		else
		{
			Rts_B = true;
		}

		if (Rts_B)
		{
			Rts_B = false;

			if (RtsState_U8 == 1)
			{
				if (EscapeCommFunction(mId_h, SETRTS) )
				{
					Rts_B = true;
				}
			}
			else if (RtsState_U8 == 0)
			{
				if (EscapeCommFunction(mId_h, CLRRTS) )
				{
					Rts_B = true;
				}
			}
			else
			{
				Rts_B = true;
			}
		}
#else
		int iFlags;
		if (DtrState_U8 == 1)
		{
			iFlags = TIOCM_DTR;

			if (ioctl(mId_h, TIOCMBIS, &iFlags) == 0)
			{
				Rts_B = true;
			}
			else
			{
// printf("Failed to set device %s dtr on: %s\n", mpTTYDeviceName_c, strerror(errno) );
			}
		}
		else if (DtrState_U8 == 0)
		{
			iFlags = TIOCM_DTR;

			if (ioctl(mId_h, TIOCMBIC, &iFlags) == 0)
			{
				Rts_B = true;
			}
			else
			{
// printf("Failed to set device %s dtr off: %s\n", mpTTYDeviceName_c, strerror(errno) );
			}
		}
		else
		{
			Rts_B = true;
		}

		if (Rts_B)
		{
			Rts_B = false;

			if (RtsState_U8 == 1)
			{
				iFlags = TIOCM_RTS;

				if (ioctl(mId_h, TIOCMBIS, &iFlags) == 0)
				{
					Rts_B = true;
				}
				else
				{
// printf("Failed to set device %s rts on: %s\n", mpTTYDeviceName_c, strerror(errno) );
				}
			}
			else if (RtsState_U8 == 0)
			{
				iFlags = TIOCM_RTS;

				if (ioctl(mId_h, TIOCMBIC, &iFlags) == 0)
				{
					Rts_B = true;
				}
				else
				{
// printf("Failed to set device %s rts off: %s\n", mpTTYDeviceName_c, strerror(errno) );
				}
			}
			else
			{
				Rts_B = true;
			}
		}
#endif
	}


	/* check
	 * iFlags = 0;
	 * if (ioctl(mId_h, TIOCMGET, &iFlags) == 0)
	 * {
	 * Rts_B = true;
	 * }
	 */
	return Rts_B;
}


/*** BofUart::V_GetStatus ********************************************************/

/*!\internal
 * This function clears the error flag of the device to enable additional input and output (I/O)
 * operations and returns the number of bytes received by the serial provider, but not yet read by
 * a ReadFile operation.
 *
 * @param _rStatus_X :     Returns the number of byte waiting in the receive buffer.
 *
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 *
 * @remark             none
 */
BOFERR BofUart::V_GetStatus(BOF_COM_CHANNEL_STATUS &_rStatus_X)
{
	BOFERR Rts_E = BOF_ERR_OPERATION_FAILED;
	uint32_t Nb_U32;

	if (mOpen_B)
	{
#if defined (_WIN32)
		DWORD    Error_DW;
		COMSTAT  Status_X;

		if (ClearCommError(mId_h, &Error_DW, &Status_X) )
		{
			_rStatus_X.NbIn_U32  = Status_X.cbInQue;
			Nb_U32               = mpTxData_O ? mpTxData_O->GetNbElement() : 0;
			_rStatus_X.NbOut_U32 = (Nb_U32 > Status_X.cbOutQue) ? Nb_U32 : Status_X.cbOutQue;
			Rts_E                = BOF_ERR_NO_ERROR;
		}
#else
		uint32_t NbIn_U32 = 0;
		uint32_t NbOut_U32 = 0;
		if ((ioctl(mId_h, FIONREAD, &NbIn_U32) < 0) ||
		    (ioctl(mId_h, TIOCOUTQ, &NbOut_U32) < 0))
		{
			NbIn_U32 = 0;
			NbOut_U32 = 0;
// printf("Failed ioctl device %s FIONREAD or TIOCOUTQ: %s\n", mpTTYDeviceName_c, strerror(errno) );
		}
		else
		{
			_rStatus_X.NbIn_U32 = NbIn_U32;
			Nb_U32 = mpTxData_O ? mpTxData_O->GetNbElement() : 0;
			_rStatus_X.NbOut_U32 = (Nb_U32 > NbOut_U32) ? Nb_U32 : NbOut_U32;
			Rts_E = BOF_ERR_NO_ERROR;
		}


#endif
	}
	return Rts_E;
}


/*** BofUart::V_ReadData ***************************************************************/

/*!\internal
 * This function reads data sent from a device at the other end of a serial connection.
 *
 * @param TimeOut_U32 :  Specifies in milliseconds the maximum time allowed to perform the operation
 *                      - -1: the function will block until the operation is complete
 *                      - 0:  the function will return imediatelly after reading a maximum of *pNb_U32 characters
 *
 * @param _rNb_U32 :     Specifies the number of bytes to read and returns the number of byte read
 *
 * @param pBuffer_U8 : Pointer to the buffer that receives the data read from the serial port
 *
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 *
 * @remark              none
 */
BOFERR BofUart::V_ReadData(uint32_t TimeOut_U32, uint32_t &_rNb_U32, uint8_t *pBuffer_U8)
{
	BOFERR Rts_E = BOF_ERR_READ;
	uint32_t Remain_U32, NbInQueue_U32;
	uint32_t Start_U32, Delta_U32, CntError_U32;
	bool To_B = false;

	Remain_U32 = _rNb_U32;

	if (mOpen_B)
	{
		To_B = false;
		Start_U32 = Bof_GetMsTickCount();
		CntError_U32 = 0;

		while ((Remain_U32) && (!To_B) && (CntError_U32 < 32))
		{
#if defined (_WIN32)
			COMSTAT Status_X;
			DWORD   Error_DW;
			DWORD   NbByteRead_DW;

			Error_DW = 0;

			if ( (! ClearCommError(mId_h, &Error_DW, &Status_X) ) || (Error_DW) )
			{
				CntError_U32++;
			}
			else
			{
				NbInQueue_U32 = Status_X.cbInQue;

				if (NbInQueue_U32)
				{
					if (NbInQueue_U32 > Remain_U32)
					{
						NbInQueue_U32 = Remain_U32;
					}

					if (! ReadFile(mId_h, pBuffer_U8, NbInQueue_U32, &NbByteRead_DW, nullptr) ||
							(NbInQueue_U32 != (uint32_t)NbByteRead_DW)
							)
					{
						Remain_U32 -= (uint64_t)NbByteRead_DW;
						break;
					}
					pBuffer_U8 += NbInQueue_U32;
					Remain_U32 -= NbInQueue_U32;
				}
				else if (TimeOut_U32)
				{
					Delta_U32 = Bof_ElapsedMsTime(Start_U32);
					To_B      = (Delta_U32 > TimeOut_U32);

					if (! To_B)
					{
						Sleep(0);
					}
				}
				else
				{
					break;
				}
			}
#else
			uint32_t NbRead_U32 = 0;

			if (ioctl(mId_h, FIONREAD, &NbInQueue_U32) < 0)
			{
				NbInQueue_U32 = 0;
// printf("Failed ioctl device %s FIONREAD: %s\n", mpTTYDeviceName_c, strerror(errno) );
			}

			if (NbInQueue_U32)
			{
				if (NbInQueue_U32 > Remain_U32)
				{
					NbInQueue_U32 = Remain_U32;
				}
				NbRead_U32 = static_cast<uint32_t>(read(mId_h, pBuffer_U8, NbInQueue_U32));
			}

			if (NbRead_U32 == 0xFFFFFFFF)
			{
				CntError_U32++;
// printf("Error reading device %s : %s\n", mpTTYDeviceName_c, strerror(errno) );
			}
			else if (NbRead_U32)
			{
				Remain_U32 -= NbRead_U32;
				pBuffer_U8 += NbRead_U32;
			}

			if (TimeOut_U32)
			{
				Delta_U32 = Bof_ElapsedMsTime(Start_U32);
				To_B = (Delta_U32 > TimeOut_U32);

				if (!To_B)
				{
          Bof_MsSleep(0);
				}
			}
			else
			{
				break;
			}
#endif
		}
	}

	if (Remain_U32 == 0)
	{
		Rts_E = BOF_ERR_NO_ERROR;
	}

	if (To_B)
	{
		Rts_E = BOF_ERR_ETIMEDOUT;
	}

	_rNb_U32 -= Remain_U32;
	return Rts_E;
}


/*** BofUart::ReadUntilString ****************************************************/

/*!\internal
 * This function reads data sent from a device at the other end of a serial connection until a
 * specified string of byte is detected on the serial port.
 *
 * @param TimeOut_U32 :  Specifies in milliseconds the maximum time allowed to perform the operation
 *                      - -1: the function will block until the operation is complete
 *                      - 0:  the function will return imediatelly after reading a maximum of *pNb_U32 characters
 *
 * @param _rNb_U32 :     Specifies the number of bytes to read and returns the number of byte read
 *
 * @param pBuffer_U8 : Pointer to the buffer that receives the data read from the serial port
 *
 * @param pSearch_U8 :  Pointer to the search string pattern.
 *
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * @remark              none
 */
BOFERR BofUart::ReadUntilString(uint32_t _TimeOut_U32, uint32_t &_rNb_U32, uint8_t *_pBuffer_U8, uint8_t *_pSearch_U8)
{
	BOFERR Rts_E = BOF_ERR_READ;
	uint32_t Remain_U32, CntError_U32;  //NbInQueue_U32;
	bool To_B;

	(void) _pBuffer_U8;
	(void) _pSearch_U8;
	(void) _TimeOut_U32;

	Remain_U32 = _rNb_U32;

	if (mOpen_B)
	{
		To_B = false;
		CntError_U32 = 0;

		while ((Remain_U32) && (!To_B) && (CntError_U32 < 32) && (Rts_E != BOF_ERR_NO_ERROR))
		{
#if defined (_WIN32)
			DWORD   NbByteRead_DW;
			DWORD   Error_DW = 0;
			COMSTAT Status_X;
			uint32_t NbInQueue_U32, j_U32, Start_U32, Delta_U32;
			uint8_t  *p_U8, *pStart_U8;
			Start_U32 = Bof_GetMsTickCount();
			pStart_U8    = _pBuffer_U8;
			if ( (! ClearCommError(mId_h, &Error_DW, &Status_X) ) || (Error_DW) )
			{
				CntError_U32++;
			}
			else
			{
				NbInQueue_U32 = Status_X.cbInQue;

				if (NbInQueue_U32)
				{
					if (NbInQueue_U32 > Remain_U32)
					{
						NbInQueue_U32 = Remain_U32;
					}

					if (! ReadFile(mId_h, _pBuffer_U8, NbInQueue_U32, &NbByteRead_DW, nullptr) ||
							(NbInQueue_U32 != (uint32_t)NbByteRead_DW)
							)
					{
						Remain_U32 -= (uint32_t)NbByteRead_DW;
						break;
					}
					_pBuffer_U8 += NbInQueue_U32;
					Remain_U32 -= NbInQueue_U32;
					*_pBuffer_U8 = 0;

/* cannot use this because a byte 0 can be received in the middle of a string !!! power off/on
 *             if (strnstr((char *)pStart_U8,(char *)pSearch_U8))
 *             {
 * pNb_U32-=Remain_U32;
 *                return(true);
 *             }
 */
					if (_pSearch_U8)
					{
						NbInQueue_U32 = _rNb_U32 - Remain_U32;

						for (p_U8 = pStart_U8, j_U32 = 0 ; j_U32 < NbInQueue_U32 ; j_U32++, p_U8++)
						{
							if (*p_U8 == *_pSearch_U8)
							{
								if (strstr( (char *)p_U8, (char *)_pSearch_U8) )
								{
									Rts_E = BOF_ERR_NO_ERROR;
									break;
								}
							}
						}
					}
					else
					{
						Rts_E = BOF_ERR_NO_ERROR;
					}
				}
				else if (_TimeOut_U32)
				{
					Delta_U32 = Bof_ElapsedMsTime(Start_U32);
					To_B      = (Delta_U32 > _TimeOut_U32);

					if (! To_B)
					{
						Sleep(0);
					}
				}
			}
#else
#endif
		}
	}
	_rNb_U32 -= Remain_U32;
	return Rts_E;
}


/*** BofUart::V_WriteData **************************************************************/

/*!\internal
 * This function transfers data through the serial connection to another device.
 *
 * @param TimeOut_U32 :  Specifies in milliseconds the maximum time allowed to perform the operation
 *                      - -1: the function will block until the operation is complete
 *                      - 0:  the function will return imediatelly after writting a maximum of *pNb_U32 characters
 *
 * @param _rNb_U32 :     Specifies the number of bytes to write and returns the number of byte written
 *
 * @param pBuffer_U8 : Pointer to the buffer containing the data to be written on the serial port
 *
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 *
 * @remark              none
 */
BOFERR BofUart::V_WriteData(uint32_t /*_TimeOut_U32*/, uint32_t &_rNb_U32, const uint8_t *pBuffer_U8)
{
	BOFERR Rts_E = BOF_ERR_WRITE;
	uint32_t Nb_U32;

	if ((mOpen_B) &&
	    (pBuffer_U8)
		)
	{
		Nb_U32 = _rNb_U32;

		if (mUartParam_X.SynchronousWrite_B)
		{
			if (WriteSynchronous(pBuffer_U8, Nb_U32))
			{
				Rts_E = BOF_ERR_NO_ERROR;
			}
		}
		else
		{
			if (mpTxData_O->GetNbFreeElement() >= Nb_U32)
			{
				if (mpTxData_O->PushBuffer(Nb_U32, pBuffer_U8) == BOF_ERR_NO_ERROR)
				{
#if defined (_WIN32)
					if (SignalThreadWakeUpEvent() )
					{
						Rts_E = BOF_ERR_NO_ERROR;
					}
#else
					if (SignalThreadWakeUpEvent() == BOF_ERR_NO_ERROR)
					{
						Rts_E = BOF_ERR_NO_ERROR;
					}
#endif
				}
			}
		}
	}
	return Rts_E;
}

BOFERR BofUart::V_WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S, uint32_t &_rNb_U32)
{
	_rNb_U32 = static_cast<uint32_t>(_rBuffer_S.size());
	return V_WriteData(_TimeoutInMs_U32, _rNb_U32, reinterpret_cast<const uint8_t *>(_rBuffer_S.c_str()));
}

BofComChannel *BofUart::V_Listen(uint32_t /*_TimeoutInMs_U32*/, const std::string &/*_rOption_S*/)
{
	return nullptr;
}


BOFERR BofUart::V_Connect(uint32_t /*_TimeoutInMs_U32*/, const std::string &/*_rTarget_S*/, const std::string &_rOption_S)
{
	uint8_t pBuffer_U8[8];
	BOFERR Rts_E = BOF_ERR_ENOTCONN;
	uint32_t Nb_U32, TimeoutInMs_U32, Val_U32, *pSign_U32;
	//TO=100;CON=HELLO WORLD
	TimeoutInMs_U32 = (Bof_GetUnsignedIntegerFromMultipleKeyValueString(_rOption_S, ";", "TO", '=', Val_U32) == BOF_ERR_NO_ERROR) ? Val_U32 : 100;

	Nb_U32 = sizeof(UART_LISTENCONNECT_SIGN);
	pSign_U32 = reinterpret_cast<uint32_t *>(pBuffer_U8);
	*pSign_U32 = UART_LISTENCONNECT_SIGN;

	if ((V_WriteData(TimeoutInMs_U32, Nb_U32, pBuffer_U8) == BOF_ERR_NO_ERROR) &&
	    (Nb_U32 == sizeof(UART_LISTENCONNECT_SIGN))
		)
	{
		Nb_U32 = sizeof(UART_LISTENCONNECT_SIGN);

		if ((V_ReadData(TimeoutInMs_U32, Nb_U32, pBuffer_U8) == BOF_ERR_NO_ERROR) &&
		    (Nb_U32 == sizeof(UART_LISTENCONNECT_SIGN))
			)
		{
			if ((*pSign_U32 == UART_LISTENCONNECT_SIGN) || (*pSign_U32 == UART_LISTENCONNECT_SIGN_REV))
			{
				Rts_E = BOF_ERR_NO_ERROR;
			}
		}
	}
	return Rts_E;
}

/*** OnProcessing ***************************************************/

/*!
 * Description
 * This function is called by the BofThread object which is
 * inherited by the BofUart object. This thread is used to Tx data
 * n com port in an sync way to woid that the caller waits until Tx ies finished.
 * Data is push by the caller in a circular buffer and send by this thread.
 * Datat is sent by interrupe by the wce kernel, so this thread will consume very
 * little cpu time.
 *
 * Parameters
 * _WaitCode_UL:  Specifies the result of the wait operation on the thread event.
 * If it is not used it is equal to WAIT_OBJECT_0
 *
 * Returns
 * ULONG: Thread exit code
 *
 * Remarks
 * ItTask entry point
 *
 * See Also
 * None
 */
BOFERR BofUart::V_OnProcessing()
{
	BOFERR Rts_E = BOF_ERR_WRITE;
	uint32_t NbMax_U32;
	uint8_t pData_U8[0x10000];
	bool Finish_B;


	if (mOpen_B)
	{
		do
		{
			Finish_B = true;
			NbMax_U32 = sizeof(pData_U8);

			if ((mpTxData_O) && (mpTxData_O->PopBuffer(&NbMax_U32, pData_U8) == BOF_ERR_NO_ERROR))
			{
				Rts_E = BOF_ERR_WRITE;

				if (NbMax_U32)
				{
					if (WriteSynchronous(pData_U8, NbMax_U32))
					{
						Rts_E = BOF_ERR_NO_ERROR;
						Finish_B = false;
					}
				}
			}
		} while (!Finish_B);
	}

	return Rts_E;
}


/*!
 * Description
 * This function performs a synchronous write on the UART
 *
 * Parameters
 * _pBuf_U8   - The pointer to the data to write
 * _Size_U32  - The size in bytes of the data
 *
 * Returns
 *  true  - The operation was successful
 *  false - The operation failed
 *
 * Remarks
 * Linux only
 *
 * See Also
 * None
 */
bool BofUart::WriteSynchronous(const uint8_t *_pBuf_U8, uint32_t _Size_U32)
{
	bool Ret_B = false;

	if (_pBuf_U8 != nullptr)
	{
		if (_Size_U32 > 0)
		{
#if defined (_WIN32)
			DWORD    NbBytesWritten_DW = 0;
			DWORD    Error_DW          = 0;
			uint32_t Idx_U32           = 0;
			bool     Ok_B              ;
			COMSTAT  Status_X;

			ClearCommError(mId_h, (DWORD *)&Error_DW, &Status_X);

			do
			{
				Ok_B = WriteFile(mId_h, &_pBuf_U8[Idx_U32], _Size_U32, &NbBytesWritten_DW, nullptr) ? true:false;

				if (Ok_B)
				{
					_Size_U32 -= NbBytesWritten_DW;
					Idx_U32   += NbBytesWritten_DW;
				}
			}
			while (Ok_B && (_Size_U32 > 0) );

			Ret_B = (_Size_U32 == 0);
#else
			int NbBytesWritten_i = 0;
			uint32_t Idx_U32 = 0;

			do
			{
				NbBytesWritten_i = static_cast<uint32_t>(write(mId_h, &_pBuf_U8[Idx_U32], _Size_U32));

				if (NbBytesWritten_i < 0)
				{
					if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
					{
						NbBytesWritten_i = 0;
					}
					else
					{
// printf("Error Idx %d Size %d (%d errno %d) writing device %.s : %.s\n", Idx_U32, _Size_U32, NbBytesWritten_i, errno, mpTTYDeviceName_c, strerror(errno) );
					}
				}

				if (NbBytesWritten_i >= 0)
				{
					_Size_U32 -= NbBytesWritten_i;
					Idx_U32 += NbBytesWritten_i;
				}
			} while ((NbBytesWritten_i >= 0) && (_Size_U32 > 0));

			Ret_B = (_Size_U32 == 0);
#endif
		}
			// Nothing to write
		else
		{
			Ret_B = true;
		}
	}

	return Ret_B;
}


#if defined (_WIN32)
#else

/*!
 * Description
 * This function set baud rate depending on the speed requested
 * If a predefined speed exists try use it, else try to configure custom speed
 *
 * Parameters
 * _ptty_X      :  pointer to a termios
 * _BaudRate_U32:  baudrate.
 *
 * Returns
 * bool: true if suceeded
 *
 * Remarks
 * Linux only
 *
 * See Also
 * None
 */
bool BofUart::SetBaudRateValue(struct termios *_ptty_X, uint32_t _BaudRate_U32)
{
	bool Rc_B = false;
	uint32_t BaudRate_U32 = GetBaudRateConstantValue(_BaudRate_U32);
	uint32_t ClosestBaudRate_U32 = 0;
	struct serial_struct serial_infos_X;

	if ((_BaudRate_U32 != 0) &&
	    (_ptty_X != nullptr) &&
	    (mId_h != (UART_HANDLE) (-1)))
	{
		if ((BaudRate_U32 != 0))         /* This is a constant baud rate known by the OS */
		{
			if ((cfsetispeed(_ptty_X, BaudRate_U32) < 0) ||
			    (cfsetospeed(_ptty_X, BaudRate_U32) < 0))
			{
// printf("Failed to apply serial port %s constant baud rate %d: %s\n", mpTTYDeviceName_c, BaudRate_U32, strerror(errno) );
			}
			else
			{
				Rc_B = true;
			}
		}
		else                               /* Try to set custom baud rates */
		{
			serial_infos_X.reserved_char[0] = 0;

			if (ioctl(mId_h, TIOCGSERIAL, &serial_infos_X) >= 0)
			{
				serial_infos_X.flags &= ~ASYNC_SPD_MASK;
				serial_infos_X.flags |= ASYNC_SPD_CUST;
				serial_infos_X.custom_divisor = (serial_infos_X.baud_base + (_BaudRate_U32 / 2)) / _BaudRate_U32;

				if (serial_infos_X.custom_divisor < 1)
				{
					serial_infos_X.custom_divisor = 1;
				}

				ClosestBaudRate_U32 = serial_infos_X.baud_base / serial_infos_X.custom_divisor;

				if ((ClosestBaudRate_U32 < _BaudRate_U32 * 98 / 100) ||
				    (ClosestBaudRate_U32 > _BaudRate_U32 * 102 / 100))
				{
// printf("Cannot set serial port speed to %d with 2%% error. Closest possible is %d with %d%% error\n", _BaudRate_U32,ClosestBaudRate_U32,ClosestBaudRate_U32 > _BaudRate_U32 ? (100 * (ClosestBaudRate_U32 - _BaudRate_U32) / ClosestBaudRate_U32): (100 * (_BaudRate_U32 - ClosestBaudRate_U32) / _BaudRate_U32) );
				}
				else
				{
					if ((ioctl(mId_h, TIOCSSERIAL, &serial_infos_X) < 0) ||
					    (ioctl(mId_h, TIOCGSERIAL, &serial_infos_X) < 0))
					{
// printf("Failed to configure serial port %s custom baud rate %d: %s\n", mpTTYDeviceName_c, ClosestBaudRate_U32, strerror(errno) );
					}
					else
					{
						/* Yes set it with B38400 value ;-) */
						if ((cfsetispeed(_ptty_X, B38400) < 0) ||
						    (cfsetospeed(_ptty_X, B38400) < 0))
						{
// printf("Failed to apply serial port %s custom baud rate %d: %s\n", mpTTYDeviceName_c, ClosestBaudRate_U32, strerror(errno) );
						}
						else
						{
							Rc_B = true;
						}
					}
				}
			}
		}
	}

	return Rc_B;
}

#endif

/*!
 * Description
 * This function return Device Number
 * Parameters
 * None
 *
 * Returns
 * uint32_t: Device Number
 *
 * Remarks
 * None
 *
 * See Also
 * None
 */
uint32_t BofUart::GetDeviceNumber()
{
#if defined (_WIN32)
	return mUartParam_X.Port_U32 - BOF_UART_PORT1 + 1;

#else
	return mUartParam_X.Port_U32 - BOF_UART_PORT1;
#endif
}


/*!
 * Description
 * This function return OS defined baud rate depending on the speed requested
 *
 * Parameters
 * _BaudRate_U32:  baudrate.
 *
 * Returns
 * uint32_t: Defined value (0 if undefined)
 *
 * Remarks
 * OS Dependant :  see termios.h for linux, winbase.h for windows
 *
 * See Also
 * None
 */
uint32_t GetBaudRateConstantValue(uint32_t _BaudRate_U32)
{
	uint32_t BaudValue_U32 = 0;

	switch (_BaudRate_U32)
	{
		case (110):
		{
#if defined (_WIN32)
			BaudValue_U32 = CBR_110;
#else
			BaudValue_U32 = B110;
#endif
			break;
		}

		case (300):
		{
#if defined (_WIN32)
			BaudValue_U32 = CBR_300;
#else
			BaudValue_U32 = B300;
#endif
			break;
		}

		case (600):
		{
#if defined (_WIN32)
			BaudValue_U32 = CBR_600;
#else
			BaudValue_U32 = B600;
#endif
			break;
		}

		case (1200):
		{
#if defined (_WIN32)
			BaudValue_U32 = CBR_1200;
#else
			BaudValue_U32 = B1200;
#endif
			break;
		}

		case (2400):
		{
#if defined (_WIN32)
			BaudValue_U32 = CBR_2400;
#else
			BaudValue_U32 = B2400;
#endif
			break;
		}

		case (4800):
		{
#if defined (_WIN32)
			BaudValue_U32 = CBR_4800;
#else
			BaudValue_U32 = B4800;
#endif
			break;
		}

		case (9600):
		{
#if defined (_WIN32)
			BaudValue_U32 = CBR_9600;
#else
			BaudValue_U32 = B9600;
#endif
			break;
		}

		case (19200):
		{
#if defined (_WIN32)
			BaudValue_U32 = CBR_19200;
#else
			BaudValue_U32 = B19200;
#endif
			break;
		}

		case (38400):
		{
#if defined (_WIN32)
			BaudValue_U32 = CBR_38400;
#else
			BaudValue_U32 = B38400;
#endif
			break;
		}

		case (57600):
		{
#if defined (_WIN32)
			BaudValue_U32 = CBR_57600;
#else
			BaudValue_U32 = B57600;
#endif
			break;
		}

		case (115200):
		{
#if defined (_WIN32)
			BaudValue_U32 = CBR_115200;
#else
			BaudValue_U32 = B115200;
#endif
			break;
		}

		default:
		{
		}
			break;
	}

	return BaudValue_U32;
}
END_BOF_NAMESPACE()