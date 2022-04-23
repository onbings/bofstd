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
#include <bofstd/bofsocket.h>

#include <functional>
#include <memory>

BEGIN_BOF_NAMESPACE()
class BofSocketIo;
enum class BOF_SOCKET_SESSION_TYPE :uint32_t;

//Callback is an alternative to virtual function when the object which receive the virtual is not the same than the one which manage it (pimpl Opaque pointer design pattern)
enum class BOF_SOCKETIO_CALLBACK_TYPE :uint32_t
{
	BOF_SOCKETIO_CALLBACK_SIGNAL_UNKNOWN = 0,
	BOF_SOCKETIO_CALLBACK_SIGNAL_CONNECTED,
	BOF_SOCKETIO_CALLBACK_SIGNAL_OPENED,
	BOF_SOCKETIO_CALLBACK_SIGNAL_DATAREAD,
	BOF_SOCKETIO_CALLBACK_SIGNAL_DATAWRITTEN,
	BOF_SOCKETIO_CALLBACK_SIGNAL_ERROR,
	BOF_SOCKETIO_CALLBACK_SIGNAL_DISCONNECTED,
	BOF_SOCKETIO_CALLBACK_SIGNAL_CLOSED,
	BOF_SOCKETIO_CALLBACK_SIGNAL_POLL,
};

struct BOF_SOCKETIO_CALLBACK_ARG
{
//	void *pUserArg;
	BOF_SOCKETIO_CALLBACK_TYPE CbType_E;
	union
	{
		struct 
		{
			BOF_SOCKET_SESSION_TYPE SessionType_E;
		} SignalConnected;
		struct 
		{
			uint32_t SessionId_U32;
			BofSocketIo *pSocketSession;
		} SignalOpened;
		struct 
		{
			uint32_t Nb_U32;
			const uint8_t *pBuffer_U8;
		} SignalDataRead;
		struct 
		{
			BOFERR Sts_E;
			uint32_t NbSent_U32;
			const uint8_t *pBufferSent_U8;
			uint32_t NbRemain_U32;
			const uint8_t *pRemainingBuffer_U8;
			void *pWriteContext;
		} SignalDataWritten;
		struct 
		{
			BOFERR Error_E;
			const char *pInfo_c;
			bool SessionWillBeClosed_B;
		} SignalError;
		struct 
		{
			uint32_t NotUsed_U32;
		} SignalDisconnected;
		struct 
		{
			uint32_t NotUsed_U32;
		} SignalClosed;
		struct 
		{
			uint16_t	REvent_U16;
			BofSocketIo	*pSocketSession;
		} SignalPoll;
	} Arg;
	BOF_SOCKETIO_CALLBACK_ARG()
	{
		Reset();
	}
	void Reset()
	{
//		 pUserArg = nullptr;
		 CbType_E= BOF_SOCKETIO_CALLBACK_TYPE::BOF_SOCKETIO_CALLBACK_SIGNAL_UNKNOWN;
		 Arg.SignalDataWritten.Sts_E=BOF_ERR_NO_ERROR;		//The longest one
		 Arg.SignalDataWritten.NbSent_U32=0;
		 Arg.SignalDataWritten.pBufferSent_U8 = nullptr;;
		 Arg.SignalDataWritten.NbRemain_U32=0;
		 Arg.SignalDataWritten.pRemainingBuffer_U8=nullptr;
		 Arg.SignalDataWritten.pWriteContext=nullptr;
	}
};

using BOF_SOCKETIO_CALLBACK = std::function< BOFERR(const BOF_SOCKETIO_CALLBACK_ARG &_rCbArg_X) >;

class IBofSocketIo
{
public:
    virtual ~IBofSocketIo()   {}
		virtual BOFERR V_SignalConnected(BOF_SOCKET_SESSION_TYPE /*_SessionType_E*/) { return BOF_ERR_NO_ERROR; }
    virtual BOFERR V_SignalOpened(uint32_t /*_SessionId_U32*/, std::shared_ptr<BofSocketIo>	/*_psSocketSession*/ ) { return BOF_ERR_NO_ERROR; }
    virtual BOFERR V_SignalDataRead(uint32_t /*_Nb_U32*/, const uint8_t * /*_pBuffer_U8*/) { return BOF_ERR_NO_ERROR; }
    virtual BOFERR V_SignalDataWritten(BOFERR /*_Sts_E*/, uint32_t /*_NbSent_U32*/, const uint8_t * /*_pBufferSent_U8*/, uint32_t /*_NbRemain_U32*/, const uint8_t * /*_pRemainingBuffer_U8*/, void * /*_pWriteContext*/) { return BOF_ERR_NO_ERROR; }
    virtual BOFERR V_SignalError(BOFERR /*_Error_E*/, const char * /*_pInfo_c*/, bool /*_SessionWillBeClosed_B*/) { return BOF_ERR_NO_ERROR; }
    virtual BOFERR V_SignalDisconnected() { return BOF_ERR_NO_ERROR; }
    virtual BOFERR V_SignalClosed() { return BOF_ERR_NO_ERROR; }
		virtual BOFERR V_SignalPoll(uint16_t	/*_REvent_U16*/, std::shared_ptr<BofSocketIo>	/*_psSocketSession*/) { return BOF_ERR_NO_ERROR; }

};

END_BOF_NAMESPACE()
