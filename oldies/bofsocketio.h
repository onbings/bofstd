#pragma once

#include <bofstd/bofthread.h>
#include <bofstd/bofsocket.h>
#include <bofstd/bofcircularbuffer.h>

BEGIN_BOF_NAMESPACE()
enum class BOF_SOCKET_IO_NOTIFY_TYPE : uint32_t
{
		ASAP = 0,
		WHEN_FULL,
		WHEN_FULL_OR_DELIMITER_FOUND,
};
enum class BOF_SOCKET_IO_STATE : uint32_t
{
		IDLE = 0,
		CREATE,
		TRYTOCONNECT,
		CONNECTED,
		IO
};

struct BOF_SOCKET_IO_PARAM
{
		std::string Name_S;
		int32_t ThreadPolicy_i;
		int32_t ThreadPriority_i;
		uint32_t ThreadStartStopTimeoutInMs_U32;
		uint32_t ThreadStackSizeInByte_U32; //0 for default
		uint32_t IoTimeoutInMs_U32;
		uint32_t NotifyRcvBufferSize_U32;
//	BOF_SOCKET_IO_TYPE IoType_E;
		BOF_SOCKET_IO_NOTIFY_TYPE NotifyType_E;
		uint8_t Delimiter_U8;  //Used by BOF_SOCKET_DATA_READ_TYPE::WHEN_FULL_OR_DELIMITER_FOUND
		void *pData;   /*! Specifies a pointer to the internal buffer data zone (pre-allocated buffer). Set to nullptr if the memory must be allocated by the function (size id InternalRxBufferSizeInByte_U32)*/

		uint32_t NbMaxAsyncWritePendingRequest_U32;

		BOF_SOCKET_IO_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			Name_S = "";
			ThreadPolicy_i = BOF_THREAD_POLICY_OTHER;
			ThreadPriority_i = BOF_THREAD_DEFAULT_PRIORITY;
			ThreadStartStopTimeoutInMs_U32 = 5000;
			ThreadStackSizeInByte_U32 = 0;
			IoTimeoutInMs_U32 = 100;
			NotifyRcvBufferSize_U32 = 0x4000;
			//	IoType_E = BOF_SOCKET_IO_TYPE::READER;
			NotifyType_E = BOF_SOCKET_IO_NOTIFY_TYPE::ASAP;
			Delimiter_U8 = 0;
			pData = nullptr;

			NbMaxAsyncWritePendingRequest_U32 = 0;
		}
};

struct BOF_SOCKET_CONNECT_PARAM
{
		std::string BindIpAddress_S;
		std::string ConnectToIpAddress_S;
		uint32_t TryMax_U32;
		uint32_t Timeout_U32;
		uint32_t RcvBufferSize_U32;
		uint32_t SndBufferSize_U32;

		BOF_SOCKET_CONNECT_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			BindIpAddress_S = "";
			ConnectToIpAddress_S = "";
			TryMax_U32 = 1;
			Timeout_U32 = 100;
			RcvBufferSize_U32 = 0x10000;
			SndBufferSize_U32 = 0x10000;
		}
};

struct BOF_SOCKET_WRITE_PARAM
{
		uint32_t TimeoutInMs_U32;
		uint32_t Nb_U32;
		const uint8_t *pBuffer_U8;
		void *pUserArg;

		BOF_SOCKET_WRITE_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			TimeoutInMs_U32 = 0;
			Nb_U32 = 0;
			pBuffer_U8 = nullptr;
			pUserArg = nullptr;
		}
};

struct BOF_SOCKET_IO_THREAD_PARAM
{
		bool MustDisconnect_B;
		uint32_t NbTry_U32;
		BOF_SOCKET_CONNECT_PARAM BofSocketConnectParam_X;
		BOF_SOCKET_IO_STATE SocketIoState_E;
		BofSocket *pSocket;
		BOF_MUTEX MtxSocketIo_X;
		bool ClientSessionSocket_B;
		BOF_NAMESPACE::BofCircularBuffer<BOF_SOCKET_WRITE_PARAM> *pAsyncWriteRequestCollection_X;
		BOF_SOCKET_WRITE_PARAM SocketWriteParam_X;
		BOF_SOCKET_IO_PARAM BofSocketIoParam_X;

		bool DataPreAllocated_B;
		uint8_t *pDataBuffer_U8;
		uint32_t WriteIndex_U32;
		uint32_t DelimiterStartIndex_U32;

		BOF_SOCKET_IO_THREAD_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			MustDisconnect_B = false;
			NbTry_U32 = 0;
			BofSocketConnectParam_X.Reset();
			SocketIoState_E = BOF_SOCKET_IO_STATE::IDLE;
			pSocket = nullptr;
			MtxSocketIo_X.Reset();
			ClientSessionSocket_B = false;
			pAsyncWriteRequestCollection_X = nullptr;
			SocketWriteParam_X.Reset();
			BofSocketIoParam_X.Reset();

			DataPreAllocated_B = false;
			pDataBuffer_U8 = nullptr;
			WriteIndex_U32 = 0;
			DelimiterStartIndex_U32 = 0;
		}
};

class BofSocketIo : public BofThread
{
private:
		BOF_SOCKET_IO_THREAD_PARAM *mpBofSocketIoThreadParam_X;

protected:
		BOFERR mErrorCode_E;

public:
		BofSocketIo(const BOF_SOCKET_IO_PARAM &_rBofSocketIoParam_X);

		virtual ~BofSocketIo();

		bool IsConnected();

		BOFERR WriteData(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, const std::string &_rBuffer_S, void *_pUserArg);

		BOFERR WriteData(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, uint32_t &_rNb_U32, const uint8_t *_pBuffer_U8, void *_pUserArg);

		template<typename InputIterator>
		BOFERR WriteData(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, InputIterator _First, InputIterator _Last, void *_pUserArg);

		BofSocket *GetBofSocket();

		BOFERR LastErrorCode();

		BOFERR StartAsyncIo(BofSocket *_pSocket);  //Call this to have V_DataRead callback
		uint32_t IoTimeoutInMs();

protected:
		virtual BOFERR V_DataRead(uint32_t _Nb_U32, const uint8_t *_pBuffer_U8);

		virtual BOFERR V_DataWritten(BOFERR _Sts_E, uint32_t _NbRemain_U32, const uint8_t *_pRemainingBuffer_U8, void *_pUserArg);

		virtual BOFERR V_RemoteSocketClosed();

		virtual BOFERR V_TryToConnect(bool _Running_B, uint32_t _Try_U32, uint32_t _MaxTry_U32);

		virtual BOFERR V_Connected();

		virtual BOFERR V_Disconnected();

		BOFERR StartAsyncConnect(const BOF_SOCKET_CONNECT_PARAM &_rBofSocketConnectParam_X);

		BOFERR StartAsyncDisconnect();

private:
		static void S_ReleaseObject(BOF_SOCKET_IO_THREAD_PARAM *_pBofSocketIoThreadParam_X);

		BOFERR OnReadyRead();

		BOFERR V_OnProcessing() override;
};

template<typename InputIterator>
inline BOFERR BofSocketIo::WriteData(uint32_t _TimeoutInMs_U32, bool _AsyncMode_B, InputIterator _First, InputIterator _Last, void *_pUserArg)
{
	BOFERR Rts_E;
	uint32_t Nb_U32;

	std::vector<uint8_t> Array;;
	std::copy(_First, _Last, std::back_inserter(Array));
	Nb_U32 = Array.size();
	Rts_E = WriteData(_TimeoutInMs_U32, _AsyncMode_B, Nb_U32, Array.data(), _pUserArg);
	return Rts_E;
}
END_BOF_NAMESPACE()
