#pragma once

#include <list>
#include <bofstd/bofsocketos.h>
#include <bofstd/bofcircularbuffer.h>
#include <bofstd/bofpot.h>
#include <bofstd/bofthread.h>
#include <bofstd/bofenum.h>
#include <uv.h>
//template< typename ... Args >
//void DbgOut(const char *_pFmt_c, const Args &... _Args);
#define DBG_OUT printf
//#define DBG_OUT(format, ...)          DbgOut(format, ##__VA_ARGS__)

BEGIN_BOF_NAMESPACE()
class BofIoConnectionManager;

class IBofIoConnection;

class IBofIoConnectionFactory;

const uint32_t LOOP_TIMER_GRANULARITY = 100;  //ms
const uint32_t NB_MAX_WRITE_BUFFER_PER_OP = 32;
const uint32_t DEFAULT_ASYNC_PUSH_TIMEOUT = 2000;
enum class ASYNC_CMD : uint32_t
{
		ASYNC_CMD_NONE = 0,
		ASYNC_CMD_EXIT,
		ASYNC_CMD_WRITE,
		ASYNC_CMD_CONNECT,
		ASYNC_CMD_DISCONNECT,
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
#define DISPLAY_UV_CALLBACK_DATA(pUvObj, pStr)                  {if (pUvObj) {\
      BOF_UV_CALLBACK *pBofUvCallback=reinterpret_cast<BOF_UV_CALLBACK *>(reinterpret_cast<uv_handle_t *>(pUvObj)->data);\
      if (pBofUvCallback!=nullptr) {\
    /*	DBG_OUT("%s => '%s' %p MustDel %s LstHnd2Close %s data %p BofIo %p pCon %p pAsync %p pUvCon %p\n",pStr, pBofUvCallback->Name_S.c_str(),pUvObj, pBofUvCallback->MustDeleteUvObj_B ? "True ":"False", pBofUvCallback->LastHandleToClose_B ? "True ":"False",pBofUvCallback,pBofUvCallback->pBofIoConnectionManager, pBofUvCallback->pIBofIoConnection, pBofUvCallback->pBofIoAsyncRequest_X, pBofUvCallback->pUvConnection_X);*/ \
                  }}}

#define ALLOC_UV_CALLBACK_DATA(pUvObj, pBofUvCallback, Name, MustDeleteUvObj, pICon, pAsyncReq, pUvCon)    {pBofUvCallback=new BOF_UV_CALLBACK();\
    BOF_ASSERT(pBofUvCallback!=nullptr);\
    pBofUvCallback->Name_S=Name;\
    pBofUvCallback->MustDeleteUvObj_B=MustDeleteUvObj;\
    reinterpret_cast<uv_handle_t *>(pUvObj)->data= reinterpret_cast<void *>(pBofUvCallback);\
    pBofUvCallback->pBofIoConnectionManager=this;\
  /*	pBofUvCallback->pIBofIoConnection=pICon;*/ \
    pBofUvCallback->pBofIoAsyncRequest_X=pAsyncReq;\
    pBofUvCallback->pUvConnection_X=pUvCon;\
    if (pBofUvCallback->pUvConnection_X) pBofUvCallback->pUvConnection_X->pIBofIoConnection=pICon; \
    DISPLAY_UV_CALLBACK_DATA(pUvObj,"Alloc"); }

#define FREE_UV_CALLBACK_DATA(pUvObj)                          {if (pUvObj) {\
      BOF_UV_CALLBACK *pBofUvCallback=reinterpret_cast<BOF_UV_CALLBACK *>(reinterpret_cast<uv_handle_t *>(pUvObj)->data);\
      if (pBofUvCallback!=nullptr) {\
        DISPLAY_UV_CALLBACK_DATA(pUvObj,"Free");\
        reinterpret_cast<uv_handle_t *>(pUvObj)->data=nullptr;\
        if (pBofUvCallback->MustDeleteUvObj_B)  { DBG_OUT("!! Free nm %s Must %X pCb %p pUv %p !!\n",pBofUvCallback->Name_S.c_str(), pBofUvCallback->MustDeleteUvObj_B,pBofUvCallback, pUvObj);  BOF_SAFE_DELETE(pUvObj); pBofUvCallback->MustDeleteUvObj_B=false;}\
        BOF_SAFE_DELETE(pBofUvCallback);}}}

#define CLOSE_UV_RESOURCE(pUvObj)                    {uv_handle_t *pHandle = reinterpret_cast<uv_handle_t *>(pUvObj);\
    BOF_ASSERT(pHandle!=nullptr);\
    BOF_UV_CALLBACK *pBofUvCallback=reinterpret_cast<BOF_UV_CALLBACK *>(reinterpret_cast<uv_handle_t *>(pUvObj)->data);\
    BOF_ASSERT(pBofUvCallback!=nullptr);\
    DISPLAY_UV_CALLBACK_DATA(pUvObj,"Close");\
    if (uv_is_closing(pHandle)==0) {uv_close(pHandle,S_OnUvCloseCallback); }\
      }

#define NOW(now) {BOF_NAMESPACE::BOF_DATE_TIME DateTime; BOF_NAMESPACE::Bof_Now(DateTime); now = BOF_NAMESPACE::Bof_FormatDateTime(DateTime, "%Y-%m-%d %H:%M:%S");  }

#define CONNECT_UV_FAILED(pUvObj, pIBofIoConnection, Sts) { uv_handle_t *pUv;pIBofIoConnection->ConnectError(Sts); \
                                                      BOF_UV_CALLBACK *pBofUvCallback=reinterpret_cast<BOF_UV_CALLBACK *>(reinterpret_cast<uv_handle_t *>(pUvObj)->data);\
                                                      BOF_ASSERT(pBofUvCallback!=nullptr);\
                                                      pUv=reinterpret_cast<uv_handle_t *>(pUvObj);pUvObj=nullptr;/* prevent dump when disconnect occurs (needed to freeup pUvConnect_X) pUvObj is _pUvConnection_X->pUv */\
                                                      CLOSE_UV_RESOURCE(pUv); /*, pIBofIoConnection);*/}

#define GET_CONNECTION(pUvConnection, Sts)                       {Sts = BOFERR_NO_MORE_CONNECTION;\
                                                                 pUvConnection = mpUvConnectionCollection->Get(0);\
                                                                 if (pUvConnection != nullptr) {\
                                                                 pUvConnection->Reset();\
                                                                 DBG_OUT("[GET] alloc %p\n",pUvConnection);\
                                                                 Sts = BOFERR_NO_ERROR;}}


#define RELEASE_CONNECTION(pUvConnection, Sts)                 {/* No as the object can be deleted -> pUvConnection->Name_S throw execept pUvConnection->pIBofIoConnection is not valid DBG_OUT("Release pUv %p pIBof %p %s %s\n",pUvConnection,pUvConnection->pIBofIoConnection,pUvConnection->Name_S.c_str(),pUvConnection->pIBofIoConnection->Name().c_str());*/ \
                                                              /* no pUvConnection->Reset();*/\
                                                              DBG_OUT("[REL] Release %p\n",pUvConnection);\
                                                              Sts = mpUvConnectionCollection->Release(pUvConnection);\
                                                              BOF_ASSERT(Sts == BOFERR_NO_ERROR);}

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
		IO,
		DISCONNECTED,
		TIMEOUT,
		LISTENING,
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

struct BOF_IO_CONNECTION_MANAGER_PARAM
{
		std::string Name_S;
		uint32_t EventTimeoutInMs_U32;
		uint32_t NbMaxConnection_U32;
		uint32_t NbMaxOpPending_U32;
		int32_t Policy_S32;
		int32_t Priority_S32;
		uint32_t StartStopTimeoutInMs_U32;

		BOF_IO_CONNECTION_MANAGER_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			Name_S = "";
			EventTimeoutInMs_U32 = 0;
			NbMaxConnection_U32 = 0;
			NbMaxOpPending_U32 = 0;
			Policy_S32 = BOF_THREAD_POLICY_OTHER;
			Priority_S32 = BOF_THREAD_DEFAULT_PRIORITY;
			StartStopTimeoutInMs_U32 = 5000;

		}
};

struct BOF_IO_CHANNEL_PARAM
{
		std::string Address_S;    //Optional bind+connect target
		union
		{
				struct
				{
						bool NoDelay_B;
				} TCP;
				struct
				{
						bool MulticastReceiver_B;
						bool MulticastLoop_B;
						bool Broadcast_B;
						uint32_t Ttl_U32;
				} UDP;
		} IO_CHANNEL;
		uint32_t RcvBufferSize_U32;
		uint32_t SndBufferSize_U32;
		//  uint32_t NotifyRcvBufferSize_U32;
		uint32_t ConnectTimeoutInMs_U32;

		BOF_IO_CHANNEL_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			Address_S = "";
			IO_CHANNEL.UDP.MulticastReceiver_B = false;
			IO_CHANNEL.UDP.MulticastLoop_B = false;
			IO_CHANNEL.UDP.Broadcast_B = false;
			IO_CHANNEL.UDP.Ttl_U32 = 0;
			RcvBufferSize_U32 = 0;
			SndBufferSize_U32 = 0;
			//    NotifyRcvBufferSize_U32 = 0x1000;
			ConnectTimeoutInMs_U32 = 100;
		}
};

struct BOF_UV_CONNECTION
{
		bool Closing_B;
		std::string Name_S;
		IBofIoConnection *pIBofIoConnection;
		void *pUvIoConnect; //uv_connect_t ->if nullptr we are in passive mode (listen)
		void *pUvIoChannel; //uv_tcp_t/uv_udp_t
		BOF_IO_CHANNEL_PARAM IoChannelParam_X;
		// uint32_t TimeoutInMs_U32;
		uint32_t StartConnectTime_U32;
		bool CancelPendingOp_B;

		BOF_UV_CONNECTION()
		{
			Reset();
		}

		~BOF_UV_CONNECTION()  //Do not use compiler generated destructor (pointer to object inside)
		{
		}

		void Reset()
		{
			Closing_B = false;
			Name_S = "";
			pIBofIoConnection = nullptr;
			pUvIoConnect = nullptr;
			pUvIoChannel = nullptr;
			IoChannelParam_X.Reset();
			//   TimeoutInMs_U32 = 100;
			StartConnectTime_U32 = 0;
			CancelPendingOp_B = false;
		}
};

struct BOF_IO_ASYNC_REQ
{
		uint32_t Ticket_U32;
		ASYNC_CMD Cmd_E;
		uint32_t NbUvBufferValid_U32;
		bool pUvBufferMustBeDeleted_B[NB_MAX_WRITE_BUFFER_PER_OP];
		uv_buf_t pUvBuffer_X[NB_MAX_WRITE_BUFFER_PER_OP];
		//void                    *pUvIoChannel; //uv_tcp_t *pUvTcp_X
		BOF_UV_CONNECTION *pUvConnection_X;

		uv_write_t TcpWrtReq_X;
		uv_udp_send_t UdpWrtReq_X;
		uv_shutdown_t ShutdownReq_X;
		//IBofIoConnection        *pIBofIoConnection;
		bool SignalEndOfIo_B;
		void *pUserArg;
		uint32_t StartTimeInMs_U32;
		uint32_t TimeoutInMs_U32;

		BOF_IO_ASYNC_REQ()
		{
			Reset();
		}

		~BOF_IO_ASYNC_REQ()  //Do not use compiler generated destructor (pointer to object inside)
		{
		}

		void Reset()
		{
			Ticket_U32 = 0;
			Cmd_E = ASYNC_CMD::ASYNC_CMD_NONE;
			NbUvBufferValid_U32 = 0;
			//memset(pUvBufferMustBeDeleted_B, 0, sizeof(pUvBufferMustBeDeleted_B));
			//memset(pUvBuffer_X, 0, sizeof(pUvBuffer_X));
			pUvConnection_X = nullptr;
			// pIBofIoConnection = nullptr;
			SignalEndOfIo_B = false;
			//memset(&WrtReq_X, 0, sizeof(WrtReq_X));
			//memset(&ShutdownReq_X, 0, sizeof(ShutdownReq_X));
			pUserArg = nullptr;
			StartTimeInMs_U32 = 0;
			TimeoutInMs_U32 = 0;
		}
};


struct BOF_IO_ASYNC_CMD
{
		uint32_t Ticket_U32;
		ASYNC_CMD Cmd_E;
		bool SignalEndOfIo_B;
//  bool              CancelPendingOp_B;
		uint32_t NbMaxConnection_U32;
		uint32_t NbBufferValid_U32;
		BOF_BUFFER pBuffer_X[NB_MAX_WRITE_BUFFER_PER_OP];
//  void              *pUvIoConnect; //uv_connect_t ->if nullptr we are in passive mode (listen)
//  void              *pUvIoChannel; //uv_tcp_t *pUvTcp_X
		void *pUserArg;
//  IBofIoConnection *pIBofIoConnection;
		BOF_IO_ASYNC_REQ *pBofIoAsyncRequest_X;
		BOF_UV_CONNECTION *pUvConnection_X;

		std::string Caller_S;

		uint32_t StartTimeInMs_U32;
		uint32_t TimeoutInMs_U32;

		BOF_IO_ASYNC_CMD()
		{
			Reset();
		}

		void Reset()
		{
			Ticket_U32 = 0;
			Cmd_E = ASYNC_CMD::ASYNC_CMD_NONE;
			SignalEndOfIo_B = false;
//    CancelPendingOp_B = false;
			NbMaxConnection_U32 = 0;
			NbBufferValid_U32 = 0;
			//memset(pBuffer_X, 0, sizeof(pBuffer_X));

			//  pUvIoConnect = nullptr;
			//  pUvIoChannel = nullptr;
			pUserArg = nullptr;

			//  pIBofIoConnection = nullptr;
			pBofIoAsyncRequest_X = nullptr;
			pUvConnection_X = nullptr;
			Caller_S = "";
			StartTimeInMs_U32 = 0;
			TimeoutInMs_U32 = 0;
		}
};

struct BOF_UV_CALLBACK
{
		std::string Name_S;
		BofIoConnectionManager *pBofIoConnectionManager;
		BOF_IO_ASYNC_REQ *pBofIoAsyncRequest_X;
		BOF_UV_CONNECTION *pUvConnection_X;
		bool MustDeleteUvObj_B;

		BOF_UV_CALLBACK()
		{
			Reset();
		}

		~BOF_UV_CALLBACK()  //Do not use compiler generated destructor (pointer to object inside)
		{
		}

		void Reset()
		{
			Name_S = "";
			pBofIoConnectionManager = nullptr;
			pBofIoAsyncRequest_X = nullptr;
			pUvConnection_X = nullptr;
			MustDeleteUvObj_B = false;
		}
};

struct IBOF_IO_CONNECTION_PARAM
{
		std::string Name_S;
		uint32_t NotifyRcvBufferSize_U32;
		BOF_IO_NOTIFY_TYPE NotifyType_E;
		uint8_t Delimiter_U8;  //Used by BOF_IO_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND
		void *pData;   /*! Specifies a pointer to the internal buffer data zone (pre-allocated buffer). Set to nullptr if the memory must be allocated by the function (size id InternalRxBufferSizeInByte_U32)*/

		IBOF_IO_CONNECTION_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			Name_S = "";
			NotifyRcvBufferSize_U32 = 0x4000;
			NotifyType_E = BOF_IO_NOTIFY_TYPE::ASAP;
			Delimiter_U8 = 0;
			pData = nullptr;
		}
};

class BofIoConnectionManager : public BofThread
{
private:
		BOF_IO_CONNECTION_MANAGER_PARAM mBofIoConnectionManagerParam_X;
		BOF_IO_STATISTICS mIoStatistic_X;

		BOF_NAMESPACE::BofCircularBuffer<BOF_IO_ASYNC_CMD> *mpBofIoAsyncCmdCollection;
		std::string mUvWalkResult_S;
		std::vector<uv_handle_t *> mUvWalkHandleCollection;
		BOF_MUTEX mConnectionMtx;
		BOFERR mErrorCode_E;
		std::unique_ptr<uv_loop_t> mpuLoop_X;
		std::unique_ptr<uv_async_t> mpuAsync_X;
		std::unique_ptr<uv_timer_t> mpuTimer_X;
		BOF_NAMESPACE::BofPot<BOF_IO_ASYNC_REQ> *mpIoAsyncRequestCollection;
		BOF_NAMESPACE::BofPot<BOF_UV_CONNECTION> *mpUvConnectionCollection;
		std::list<BOF_UV_CONNECTION *> mUvConnectingConnectionCollection;  //TODO change in std::map
		std::list<BOF_UV_CONNECTION *> mUvDisconnectingConnectionCollection;
		std::string mLastLockerName_S;
		//uint32_t                                              mNbSessionConnnection_U32; mpUvConnectionCollection->GetNbElementOutOfThePot()
		static uint32_t S_mCmdTicket_U32;
		static uint32_t S_mReqTicket_U32;
		static uint32_t S_mListenId_U32;
		static uint32_t S_mSessionId_U32;
		static uint32_t S_mClientId_U32;

public:
		BofIoConnectionManager(const BOF_IO_CONNECTION_MANAGER_PARAM &_rBofIoConnectionManagerParam_X);

		virtual ~BofIoConnectionManager();

		//https://cpppatterns.com/patterns/rule-of-five.html
		BofIoConnectionManager(const BofIoConnectionManager &_rOther) = delete;

		BofIoConnectionManager(BofIoConnectionManager &&_rrOther) = delete;

		BofIoConnectionManager &operator=(const BofIoConnectionManager &_rOther) = delete;

		BofIoConnectionManager &operator=(BofIoConnectionManager &&_rrOther) = delete;

		std::string BuildConnectionInfo();

		//BOF_UV_CONNECTION *UvConnection(IBofIoConnection *_pIBofIoConnection);
		bool IsIBofIoConnectionAlreadyInTable(IBofIoConnection *_pIBofIoConnection);

		BOFERR LockBofIoConnectionManager(const std::string &_rLockerName_S);

		BOFERR UnlockBofIoConnectionManager();

		uint32_t GetNbConnnection() const;

		BOFERR Connect(IBofIoConnection *_pIBofIoConnection, const BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X);

		BOFERR Listen(IBofIoConnectionFactory *_pIBofIoConnectionFactory, const BOF_NAMESPACE::IBOF_IO_CONNECTION_PARAM &_rIBofIoConnectionParam_X, const BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X,
		              uint32_t _NbMaxConnection_U32, IBofIoConnection **_ppIBofIoConnection);

		BOFERR PushDisconnect(const std::string &_rDbgName_S, BOF_UV_CONNECTION *_pUvConnection_X, bool _CancelPendingOp_B);

		BOFERR SimulateConnectionRequest(IBofIoConnectionFactory *_pIBofIoConnectionFactory, const std::string &_rName_S, IBofIoConnection **_ppIBofIoConnection);

		BOFERR SimulateDisconnectionRequest(IBofIoConnectionFactory *_pIBofIoConnectionFactory, IBofIoConnection *_pIBofIoConnection);

		BOFERR SendAsyncCmd(bool _Emergency_B, BOF_IO_ASYNC_CMD &_rBofIoAsyncCmd_X);

		BOFERR EndOfFactory(IBofIoConnectionFactory *_pIBofIoConnectionFactory);

		BOFERR EndOfConnection(BOF_UV_CONNECTION *_pUvConnection_X);

private:
		BOFERR OpenUvConnection(IBofIoConnection *_pIBofIoConnection, const std::string &_rName_S, uint32_t _NbMaxConnection_U32, const BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X,
		                        BOF_UV_CONNECTION *_pUvConnection_X);

		std::string UvWalk();

		const BOF_IO_STATISTICS &IoStatistic();

		void ResetIoStatistic();

		BOFERR LastErrorCode();

		BOFERR StopUvLoop();

		BOFERR ReleaseUvLoopResource();

		BOFERR ConnectUvStream(BOF_UV_CONNECTION *_pUvConnection_X, uint32_t _NbMaxConnection_U32);

		BOFERR DisconnectUvStream(const std::string &_rCaller_S, BOF_UV_CONNECTION *_pUvConnection_X);

		BOFERR S_SetIoChannelOption(bool _IsUdp_B, uv_handle_t *_pUvHandle_X, const BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X);

		static void S_OnUvCloseCallback(uv_handle_t *_pHandle);

		static void S_OnUvConnectCallback(uv_connect_t *_pReq_X, int _Status_i);

		static void S_OnUvConnectionCallback(uv_stream_t *_pServer, int _Status_i);

		static void S_OnUvAllocCallback(uv_handle_t *_pHandle, size_t _SuggestedSize, uv_buf_t *_pBuffer_X);

		static void S_OnUvReadTcpCallback(uv_stream_t *_pStream_X, ssize_t _NbRead, const uv_buf_t *_pBuffer_X);

		static void S_OnUvReadUdpCallback(uv_udp_t *_pUvUdp_X, ssize_t _NbRead, const uv_buf_t *_pBuffer_X, const struct sockaddr *_pAddress_X, uint32_t _Flag_U32);

		static void S_OnUvWriteTcpCallback(uv_write_t *_pStream_X, int _Status_i);

		static void S_OnUvWriteUdpCallback(uv_udp_send_t *_pReq_X, int _Status_i);

		static void S_OnUvAsyncCallback(uv_async_t *_pHandle);

		static void S_OnUvTimerCallback(uv_timer_t *_pHandle);

		static void S_OnUvShutdownCallback(uv_shutdown_t *_pReq_X, int _Status_i);

		static void S_OnUvWalkCallback(uv_handle_t *_pHandle, void *_pUserArg);

		virtual BOFERR V_OnProcessing() override;

		virtual BOFERR V_OnUvConnect(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_connect_t *_pReq_X, int _Status_i); // , void *_pUvIoChannel);
		virtual BOFERR V_OnUvConnection(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_stream_t *_pUvStream_X, int _Status_i, IBofIoConnection *_pIBofIoListeningUdpConnection, void *_pUvIoChannel);

		virtual BOFERR V_OnUvAsync(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_async_t *_pHandle);

		virtual BOFERR V_OnUvTimer(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_timer_t *_pHandle);

		virtual BOFERR V_OnUvAlloc(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_handle_t *_pHandle, size_t _SuggestedSize, uv_buf_t *_pBuffer_X);

		virtual BOFERR V_OnUvClose(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_handle_t *_pHandle);

		virtual BOFERR V_OnUvReadTcp(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_stream_t *_pStream_X, ssize_t _NbRead, const uv_buf_t *_pBuffer_X);

		virtual BOFERR V_OnUvReadUdp(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_udp_t *_pUvUdp_X, ssize_t _NbRead, const uv_buf_t *_pBuffer_X, const struct sockaddr *_pAddress_X, uint32_t _Flag_U32);

		virtual BOFERR V_OnUvWriteTcp(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_write_t *_pReq_X, int _Status_i);

		virtual BOFERR V_OnUvWriteUdp(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_udp_send_t *_pReq_X, int _Status_i);

		virtual BOFERR V_OnUvShutdown(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_shutdown_t *_pReq_X, int _Status_i);

		virtual BOFERR V_OnUvWalk(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_handle_t *_pHandle);

		BOFERR OnUvAsyncConnect(BOF_IO_ASYNC_CMD &_rBofIoAsyncCmd_X);

		BOFERR OnUvAsyncDisconnect(const BOF_IO_ASYNC_CMD &_rBofIoAsyncCmd_X);

		BOFERR OnUvAlloc(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_buf_t *_pBuffer_X);

		BOFERR OnUvRead(BOF_UV_CALLBACK *_pBofUvCallback_X, ssize_t _NbRead, const uv_buf_t *_pBuffer_X);

		BOFERR OnUvWrite(BOF_UV_CALLBACK *_pBofUvCallback_X, int _Status_i);

public:
		BOFERR OnUvAsyncWrite(const BOF_IO_ASYNC_CMD &_rBofIoAsyncCmd_X);
};


END_BOF_NAMESPACE()
