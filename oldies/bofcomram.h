/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines routines for creating and managing a shared memory communication
 * channel.
 *
 * Name:        BofComram.h
 * Author:      Bernard HARMEL: b.harmel@belgacom.net
 * Revision:    1.0
 *
 * Rem:
 * None
 *
 * History:
 *
 * V 1.00  Oct 21 2002  BHA : Initial release
 */

#pragma once

/*** Include ****************************************************************/
#include <bofstd/bofstd.h>
#include <bofstd/bofsystem.h>
#include <bofstd/ibofcomramsignal.h>
#include <bofstd/bofcomchannel.h>
// #include <bofstd/bofbinserializer.h>
#include <atomic>
BEGIN_BOF_NAMESPACE();

/*** Defines ****************************************************************/

// const uint32_t BOF_COMRAM_MAGIC = 0x54658E48;


/*** Structures *************************************************************/

/*** Enum *******************************************************************/

struct BOF_COMRAM_PARAM
{
	bool										 Master_B;							//True if this instance of comram wait for cmd from slave False if this instance of comram is the initiator of a data exchange in a master slave protocol
	bool                     MultiThreadAware_B;   /*! true if the object is used in a multi threaded application (use mMtx_X)*/
	BOF_COM_CHANNEL_PARAM    BaseChannelParam_X;   // Base properties of each channel
	void                     *pAddress;            // If specified (!=nullptr) the Comram has been allocated by caller->no delete from here !!! if it is allocated by the caller its size must take into accont the fact that each memory zone is preceeded by a BOFCOMRAMHEADER
	BOF_COMRAM_PARAM()
	{
		Reset();
	}
	void                     Reset()
	{
		Master_B = false;
		BaseChannelParam_X.Reset();
		pAddress       = nullptr;
	}
};

#pragma pack(1)
struct BOF_COMRAM_HEADER
{
	uint32_t    AccessControl_U32;  /*! Access control to the rest of the BOF_COMRAM_HEADER structure */
	uint32_t    TicketIn_U32;      /*! Identify each request */
	uint32_t    TicketOut_U32;      /*! Identify each request */
	uint32_t    MaxDataSizeInByte_U32; // Max Data payload size following this header
	uint32_t    PayloadSize_U32;      /*! Number of valid byte following this header*/
	BOF_COMRAM_HEADER()
	{
		Reset();
	}
	void                       Reset()
	{
		AccessControl_U32 = 0;
		TicketIn_U32 = 0;
		TicketOut_U32 = 0;
		MaxDataSizeInByte_U32 = 0;
		PayloadSize_U32=0;
	}
};
#pragma pack()

/*** Class definition *******************************************************/

/*** Comram *********************************************************************/

class BofComram:public BofComChannel
{
private:
	static std::atomic<uint32_t> S_mTicket_U32;
	IBofComramSignal	*mpIBofComramSignal;
	BOF_COMRAM_PARAM                 mComramParam_X;
	volatile BOF_COMRAM_HEADER       *mpComramHeader_X;
	volatile uint8_t                 *mpComramData_U8;
	uint32_t                         mWritePos_U32;
	uint32_t                         mReadPos_U32;
	BOF_MUTEX                        mMtx_X;
	BOF_SHARED_MEMORY                mComram_X;
	bool                             mSharedComramAllocator_B;
	uint32_t												 mSizeInByte_U32;       // The basic value is equal to BaseChannelParam_X.RcvBufferSize_U32+BaseChannelParam_X.SndBufferSize_U32. If pAddress is nullptr, this is the basic size of the zone. This one will be increased by sizeof(BOFCOMRAMHEADER) as each memory zone is preceded by a BOFCOMRAMHEADER
	uint32_t												 mCommandPending_U32;

public:
	BofComram(IBofComramSignal	*_pIBofComramSignal, const BOF_COMRAM_PARAM & _rComramParam_X);
	~BofComram();

	BofComram & operator             = (const BofComram &) = delete; // Disallow copying
	BofComram(const BofComram &)                           = delete;

	BOFERR													 SendCommand(uint32_t _TimeoutInMs_U32, uint32_t  _Nb_U32, const void *_pBuffer, BofSignalFunction _SignalFunction, void *_pContext);
	BOFERR													 WaitForCommand(uint32_t _TimeoutInMs_U32, BofSignalFunction _SignalFunction, void *_pContext);
	bool														 IsCommandPending();

private:
	BOFERR                           LockComramForIo(uint32_t _TimeoutInMs_U32);
	BOFERR                           UnlockComramForIo(uint32_t _TimeoutInMs_U32);
	BOFERR                           SetComramIoPosition(bool _ReadOffset_B, int32_t _Offset_S32, BOF_SEEK_METHOD _SeekMethod_E);
	uint32_t                         GetComramIoPosition(bool _ReadOffset_B);

	BOFERR                           V_Connect(uint32_t _TimeoutInMs_U32, const std::string & _rTarget_S, const std::string & _rOption_S) override;
	BofComChannel										*V_Listen(uint32_t _TimeoutInMs_U32, const std::string & _rOption_S) override;
	BOFERR                           V_ReadData(uint32_t _TimeoutInMs_U32, uint32_t & _rNb_U32, uint8_t *_pBuffer_U8) override;
	BOFERR                           V_WriteData(uint32_t _TimeoutInMs_U32, uint32_t & _rNb_U32, const uint8_t *_pBuffer_U8) override;
	BOFERR													 V_WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S, uint32_t & _rNb_U32) override;
	BOFERR                           V_GetStatus(BOF_COM_CHANNEL_STATUS & _rStatus_X) override;
	BOFERR                           V_FlushData(uint32_t _TimeoutInMs_U32) override;
	BOFERR													 V_WaitForDataToRead(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPendingByte_U32) override;

#if 0
	// Based on Read/Write scalar in bofstd/bofbinserialer.h
	// Return the size in byte of the data read/written (0 in case of error).
	template< typename T >
	uint32_t                         ReadScalar(T & _rValue_T);
	template< typename T >
	uint32_t                         ReadNativeBlob(T & _rValue_T);
	template< typename T >
	uint32_t                         WriteScalar(T _Value_T);
	template< typename T >
	uint32_t                         WriteNativeBlob(T _NewValue_T);
#endif
};
END_BOF_NAMESPACE();