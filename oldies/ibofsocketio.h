#pragma once

#include <bofstd/bofstd.h>

BEGIN_BOF_NAMESPACE()
class IBofSocketIo
{
public:
		virtual ~IBofSocketIo()
		{}

		virtual BOFERR V_SignalDataRead(uint32_t _Nb_U32, const uint8_t *_pBuffer_U8)
		{ return BOFERR_NO_ERROR; }

		virtual BOFERR V_SignalDataWritten(BOFERR _Sts_E, uint32_t _NbRemain_U32, const uint8_t *_pRemainingBuffer_U8, void *_pUserArg)
		{ return BOFERR_NO_ERROR; }

		virtual BOFERR V_SignalRemoteSocketClosed()
		{ return BOFERR_NO_ERROR; }

		virtual BOFERR V_SignalTryToConnect(bool _Running_B, uint32_t _Try_U32, uint32_t _MaxTry_U32)
		{ return BOFERR_NO_ERROR; }

		virtual BOFERR V_SignalConnected()
		{ return BOFERR_NO_ERROR; }

		virtual BOFERR V_SignalDisconnected()
		{ return BOFERR_NO_ERROR; }
};

END_BOF_NAMESPACE()
