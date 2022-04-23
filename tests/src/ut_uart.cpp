/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the comchannel class
 *
 * Name:        ut_uart.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */

/*** Include files ***********************************************************/

#include "gtestrunner.h"
#include <bofstd/bofstd.h>
#include <bofstd/bofuart.h>

/*** Class *************************************************************************************************************************/

USE_BOF_NAMESPACE()

TEST(Uart_Test, Uart)
{
	BofUart        *pBofUart_O;
	BOF_UART_PARAM Params_X;
	uint32_t       i_U32, j_U32, Nb_U32, pBaud_U32[3];
	BOFERR         Sts_E;
	uint8_t        pBuffer_U8[128];

	pBaud_U32[0] = 9600;
	pBaud_U32[1] = 19200;
	pBaud_U32[2] = 38400;

	for (j_U32 = 0; j_U32 < 3; j_U32++)
	{
		// Initialize parameters
		Params_X.Reset();
		Params_X.Baud_U32 = pBaud_U32[j_U32];
		Params_X.Port_U32 = BOF_UART_PORT1;
		Params_X.Data_U8 = 8;
		Params_X.Parity_U8 = BOF_UART_NOPARITY;
		Params_X.Stop_U8 = BOF_UART_ONESTOPBIT;
		Params_X.RtsCts_B = false;
		Params_X.XonXoff_B = false;
		Params_X.DtrDsr_B = false;

		Params_X.BaseChannelParam_X.RcvBufferSize_U32 = 1024;
		Params_X.BaseChannelParam_X.SndBufferSize_U32 = 1024;
		Params_X.SynchronousWrite_B = true;
		pBofUart_O = new BofUart(Params_X);

		// The port has been opened
		EXPECT_TRUE(pBofUart_O != nullptr);
		if (pBofUart_O->IsPortOpened())	//Check if hardware is present
		{
			EXPECT_EQ(pBofUart_O->LastErrorCode(), 0);
			Nb_U32 = sizeof(pBuffer_U8);
			for (i_U32 = 0; i_U32 < Nb_U32; i_U32++)
			{
				pBuffer_U8[i_U32] = (uint8_t)i_U32;
			}
			Sts_E = pBofUart_O->V_WriteData(1000, Nb_U32, pBuffer_U8);
			EXPECT_EQ(Sts_E, 0);
			EXPECT_EQ(Nb_U32, sizeof(pBuffer_U8));

			Nb_U32 = sizeof(pBuffer_U8);
			Sts_E = pBofUart_O->V_ReadData(0, Nb_U32, pBuffer_U8);
			EXPECT_EQ(Sts_E, BOF_ERR_READ);
			EXPECT_EQ(Nb_U32, 0);

			Sts_E = pBofUart_O->V_FlushData(100);
			EXPECT_EQ(Sts_E, 0);
		}
		BOF_SAFE_DELETE(pBofUart_O);
		EXPECT_TRUE(pBofUart_O == nullptr);

	}
}
