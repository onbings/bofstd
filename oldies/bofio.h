#pragma once

#include <bofstd/bofsocketos.h>
#include <bofstd/bofcircularbuffer.h>
#include <uv.h>

BEGIN_BOF_NAMESPACE()
class BofIo;

const uint32_t LOOP_TIMER_GRANULARITY = 100;  //ms
enum class ASYNC_CMD : uint32_t
{
		ASYNC_CMD_NONE = 0,
		ASYNC_CMD_EXIT,
		ASYNC_CMD_WRITE,
};

struct BOF_UV_CALLBACK
{
		std::string Name_S;
		BofIo *pBofIo;
		void *pUser;

		BOF_UV_CALLBACK()
		{
			Reset();
		}

		void Reset()
		{
			Name_S = "";
			pBofIo = nullptr;
			pUser = nullptr;
		}
};


/*
uv_handle_sis the abstract base class of all handles.
struct uv_handle_s {
	UV_HANDLE_FIELDS
};

uv_stream_t is a subclass of uv_handle_t.
uv_stream is an abstract class.
uv_stream_t is the parent class of uv_tcp_t, uv_pipe_t and uv_tty_t.
struct uv_stream_s {
UV_HANDLE_FIELDS
UV_STREAM_FIELDS
};

uv_tcp_t is a subclass of uv_stream_t.
Represents a TCP stream or TCP server.
struct uv_tcp_s {
UV_HANDLE_FIELDS
UV_STREAM_FIELDS
UV_TCP_PRIVATE_FIELDS
};
*/
#define HANDLE_AND_STREAM_FROM_TCP(pUvTcp, pUvTcpStream, pUvTcpHandle) {pUvTcpStream = reinterpret_cast<uv_stream_t *>(pUvTcp);BOF_ASSERT(pUvTcpStream!=nullptr);pUvTcpHandle = reinterpret_cast<uv_handle_t *>(pUvTcp);BOF_ASSERT(pUvTcpHandle!=nullptr);}

#define INIT_UV_CALLBACK_DATA(pUvObj, Name, pBofUvCallback, pUserArg)    {pBofUvCallback=new BOF_UV_CALLBACK();\
                                                              BOF_ASSERT(pBofUvCallback!=nullptr);\
                                                              pBofUvCallback->Name_S=Name;\
                                                              pBofUvCallback->pBofIo=this;\
                                                              pBofUvCallback->pUser=pUserArg;\
                                                              /*printf("'%-16s' (%p) alloc at %p\n",pBofUvCallback->Name_S.c_str(),pUvObj, pBofUvCallback);*/\
                                                              (pUvObj)->data= reinterpret_cast<void *>(pBofUvCallback);}
#define DELETE_UV_CALLBACK_DATA(pUvObj)                        {if (pUvObj) {\
                                                              BOF_UV_CALLBACK *pBofUvCallback=reinterpret_cast<BOF_UV_CALLBACK *>(pUvObj->data);\
                                                              if (pBofUvCallback!=nullptr) {\
                                                              /*printf("'%-16s' (%p) release  %p\n",pBofUvCallback->Name_S.c_str(),pUvObj,pBofUvCallback);*/\
                                                              BOF_SAFE_DELETE(pBofUvCallback);pUvObj->data=nullptr;}}}

#define RELEASE_UV_RESOURCE(pUvObj, CallClose)                  {uv_handle_t *pHandle_X = reinterpret_cast<uv_handle_t *>(pUvObj);\
                                                              /*printf("[%d] uv_close %p\n",BOF_NAMESPACE::Bof_GetMsTickCount(),pHandle_X);*/\
                                                              if (pHandle_X) {\
                                                              if ((CallClose) && (uv_is_closing(pHandle_X)==0)) {uv_close(pHandle_X, [](uv_handle_t *_pHandle_X) {/*printf("lambda uv_close %p\n", _pHandle_X); */ DELETE_UV_CALLBACK_DATA(_pHandle_X); }); }\
                                                                else {/*printf("Just del %p\n", pHandle_X); */ DELETE_UV_CALLBACK_DATA(pHandle_X);}\
                                                              }} //No capture lambda as the process is async !


enum class BOF_IO_NOTIFY_TYPE : uint32_t
{
		ASAP = 0,
		WHEN_FULL,
		WHEN_FULL_OR_DELIMITER_FOUND,
};
enum class BOF_IO_STATE : uint32_t
{
		IDLE = 0,
		CONNECTING,
		CONNECTED,
//	IO,
			CLOSING,
		CLOSED
};

struct BOF_IO_PARAM
{
//	BOF_SOCK_TYPE IoSocketType_E;

		std::string Name_S;
		uint32_t IoTimeoutInMs_U32;
		uint32_t NotifyRcvBufferSize_U32;
		BOF_IO_NOTIFY_TYPE NotifyType_E;
		uint8_t Delimiter_U8;  //Used by BOF_IO_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND
		void *pData;   /*! Specifies a pointer to the internal buffer data zone (pre-allocated buffer). Set to nullptr if the memory must be allocated by the function (size id InternalRxBufferSizeInByte_U32)*/

		BOF_IO_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			//	IoSocketType_E=BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
			Name_S = "";
			IoTimeoutInMs_U32 = 100;
			NotifyRcvBufferSize_U32 = 0x4000;
			NotifyType_E = BOF_IO_NOTIFY_TYPE::ASAP;
			Delimiter_U8 = 0;
			pData = nullptr;
		}
};

struct BOF_IO_STATISTICS
{
		uint32_t NbOpTryConnect_U32;
		uint32_t NbOpTryConnectError_U32;
		uint32_t NbOpConnect_U32;
		uint32_t NbOpConnectError_U32;
		uint32_t NbOpDisconnect_U32;
		uint32_t NbOpDisconnectError_U32;
		uint32_t NbOpRemoteClientClosed_U32;
		uint32_t NbOpRead_U32;
		uint32_t NbOpReadError_U32;
		uint64_t NbDataByteRead_U64;
		uint32_t NbOpWrite_U32;
		uint32_t NbOpWriteError_U32;
		uint64_t NbDataByteWritten_U64;

		BOF_IO_STATISTICS()
		{
			Reset();
		}

		void Reset()
		{
			NbOpTryConnect_U32 = 0;
			NbOpTryConnectError_U32 = 0;
			NbOpConnect_U32 = 0;
			NbOpConnectError_U32 = 0;
			NbOpDisconnect_U32 = 0;
			NbOpDisconnectError_U32 = 0;
			NbOpRemoteClientClosed_U32 = 0;
			NbOpRead_U32 = 0;
			NbOpReadError_U32 = 0;
			NbDataByteRead_U64 = 0;
			NbOpWrite_U32 = 0;
			NbOpWriteError_U32 = 0;
			NbDataByteWritten_U64 = 0;
		}
};

struct BOF_IO_ASYNC_CMD
{
		ASYNC_CMD Cmd_E;
		BOF_BUFFER Buffer_X;

		BOF_IO_ASYNC_CMD()
		{
			Reset();
		}

		void Reset()
		{
			Cmd_E = ASYNC_CMD::ASYNC_CMD_NONE;
			Buffer_X.Reset();
		}
};

class BofIo
{
private:
		BOF_IO_STATISTICS mIoStatistic_X;

		bool mDataPreAllocated_B;
		uint8_t *mpDataBuffer_U8;
		uint32_t mWriteIndex_U32;
		uint32_t mDelimiterStartIndex_U32;
		BOF_IO_PARAM mBofIoParam_X;
		BOF_NAMESPACE::BofCircularBuffer<BOF_IO_ASYNC_CMD> *mpCmdCircularBuffer;
		std::string mUvWalkResult_S;
		std::vector<uv_handle_t *> mUvWalkHandleCollection;

protected:
		BOFERR mErrorCode_E;
		std::unique_ptr<uv_loop_t> mpuLoop_X;
		std::unique_ptr<uv_async_t> mpuAsync_X;
		std::unique_ptr<uv_timer_t> mpuTimer_X;

protected:
		BofIo(const BOF_IO_PARAM &_rBofIoParam_X);

		virtual ~BofIo();

		std::string UvWalk();

		const BOF_IO_STATISTICS &IoStatistic();

		void ResetIoStatistic();

		BOFERR LastErrorCode();

		uint32_t IoTimeoutInMs();

		BOFERR SendAsyncCmd(bool _Emergency_B, const BOF_IO_ASYNC_CMD &_rBofIoAsyncCmd_X);

		BOFERR GetAsyncCmd(BOF_IO_ASYNC_CMD &_rBofIoAsyncCmd_X);

		BOFERR StopUvLoop();

		BOFERR ReleaseUvLoopResource();

		BOFERR WriteData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, const uint8_t *_pBuffer_U8);

		BOFERR WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S);

		template<typename InputIterator>
		BOFERR WriteData(uint32_t _TimeoutInMs_U32, InputIterator _First, InputIterator _Last);

public:
		static BOFERR S_GetInfo();

protected:
		BOFERR S_SetupTcpOption(uv_tcp_t *_pUvTcp_X, bool _NoDelay_B, uint32_t _RcvBufferSize_U32, uint32_t _SndBufferSize_U32);

		static void S_OnUvConnectCallback(uv_connect_t *_pReq_X, int _Status_i);

		static void S_OnUvConnectionCallback(uv_stream_t *_pServer, int _Status_i);

		static void S_OnUvAllocCallback(uv_handle_t *_pHandle, size_t _SuggestedSize, uv_buf_t *_pBuffer_X);

		static void S_OnUvReadCallback(uv_stream_t *_pStream_X, ssize_t _NbRead, const uv_buf_t *_pBuffer_X);

		static void S_OnUvWriteCallback(uv_write_t *_pStream_X, int _Status_i);

		static void S_OnUvAsyncCallback(uv_async_t *_pHandle);

		static void S_OnUvTimerCallback(uv_timer_t *_pHandle);

		static void S_OnUvCloseCallback(uv_handle_t *_pHandle);

		static void S_OnUvShutdownCallback(uv_shutdown_t *_pReq_X, int _Status_i);

		static void S_OnUvWalkCallback(uv_handle_t *_pHandle, void *_pUserArg);

		virtual BOFERR V_OnUvConnect(BOF_UV_CALLBACK *_pBofUvCallback, uv_connect_t *_pReq_X, int _Status_i);

		virtual BOFERR V_OnUvConnection(BOF_UV_CALLBACK *_pBofUvCallback, uv_stream_t *_pServer, int _Status_i);

		virtual BOFERR V_OnUvAlloc(BOF_UV_CALLBACK *_pBofUvCallback, uv_handle_t *_pHandle, size_t _SuggestedSize, uv_buf_t *_pBuffer_X);

		virtual BOFERR V_OnUvRead(BOF_UV_CALLBACK *_pBofUvCallback, uv_stream_t *_pStream_X, ssize_t _NbRead, const uv_buf_t *_pBuffer_X);

		virtual BOFERR V_OnUvWrite(BOF_UV_CALLBACK *_pBofUvCallback, uv_write_t *_pReq_X, int _Status_i);

		virtual BOFERR V_OnUvAsync(BOF_UV_CALLBACK *_pBofUvCallback, uv_async_t *_pHandle);

		virtual BOFERR V_OnUvTimer(BOF_UV_CALLBACK *_pBofUvCallback, uv_timer_t *_pHandle);

		virtual BOFERR V_OnUvClose(BOF_UV_CALLBACK *_pBofUvCallback, uv_handle_t *_pHandle);

		virtual BOFERR V_OnUvShutdown(BOF_UV_CALLBACK *_pBofUvCallback, uv_shutdown_t *_pReq_X, int _Status_i);

		virtual BOFERR V_OnUvWalk(BOF_UV_CALLBACK *_pBofUvCallback, uv_handle_t *_pHandle);

		virtual BOFERR V_DataRead(uint32_t _Nb_U32, const uint8_t *_pBuffer_U8);

private:
		BOFERR OnReadyRead(uv_buf_t *_pBuffer_X);
};

template<typename InputIterator>
inline BOFERR BofIo::WriteData(uint32_t _TimeoutInMs_U32, InputIterator _First, InputIterator _Last)
{
	BOFERR Rts_E;
	uint32_t Nb_U32;

	std::vector<uint8_t> Array;;
	std::copy(_First, _Last, std::back_inserter(Array));
	Nb_U32 = Array.size();
	Rts_E = WriteData(_TimeoutInMs_U32, Nb_U32, Array.data());
	return Rts_E;
}

END_BOF_NAMESPACE()
