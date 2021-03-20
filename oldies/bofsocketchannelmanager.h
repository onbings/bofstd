#pragma once

#include <map>
#include <vector>
#include <functional>
#include <uvw.hpp>
#include <bofstd/bofthread.h>
#include <atomicops.h>
#include <readerwriterqueue.h>
#include <memory.h>

BEGIN_BOF_NAMESPACE()

#define DBG_OUT printf

class BofSocketChannel;

struct BOF_SOCKET_CHANNEL_PARAM;
struct BOF_SOCKET_ASYNC_CMD;
struct BOF_SOCKET_CHANNEL_STATE;

const uint32_t TIMER_GRANULARITY_IN_MS = 100;
const uint32_t MIN_TIMEOUT_VALUE_IN_MS = (2 * TIMER_GRANULARITY_IN_MS);   //Must be >= TIMER_GRANULARITY_IN_MS (see OnAsyncConnect)
struct BOF_SOCKET_PEER_INFO
{
		std::string IpAddress_S;
		uint16_t Port_U16;

		BOF_SOCKET_PEER_INFO()
		{
			Reset();
		}

		void Reset()
		{
			IpAddress_S = "";
			Port_U16 = 0;
		}
};

typedef std::function<BOFERR(uint32_t _Id_U32, std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> _psBofSocketChannel, const BOF_SOCKET_PEER_INFO &_rPeerInfo_X)> ON_SOCKET_CONNECT_CALLBACK;
typedef std::function<BOFERR(uint32_t _Id_U32, std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> _psBofSocketChannel, const BOF_SOCKET_PEER_INFO &_rPeerInfo_X,
                             BOF_NAMESPACE::BOF_SOCKET_CHANNEL_PARAM *_pBofSocketChannelParam_X)> ON_SOCKET_ACCEPT_CALLBACK;
typedef std::function<BOFERR(uint32_t _Id_U32, std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> _psBofSocketChannel, const BOF_BUFFER &_rBuffer_X,
                             const BOF_SOCKET_PEER_INFO &_rPeerInfo_X)> ON_SOCKET_READ_CALLBACK;

enum class BOF_SOCKET_NOTIFY_TYPE : uint32_t
{
		NEVER = 0,
		ASAP,
		WHEN_FULL,
		WHEN_FULL_OR_DELIMITER_FOUND,
};
enum class BOF_SOCKET_STATE : uint32_t
{
		DISCONNECTED = 0,
		LISTENING,
		LISTENING_TIMEOUT,
		CONNECTING,
		CONNECT_TIMEOUT,
		CONNECT_ERROR,
		DISCONNECTING,
		DISCONNECT_TIMEOUT,
		DISCONNECT_ERROR,
		IO_ERROR,
		END,
		SHUTDOWN,

		CONNECTED,  //All the state above this level return true to IsConnected method
		IO,
};
enum class SOCKET_ASYNC_CMD : uint32_t
{
		SOCKET_ASYNC_CMD_NONE = 0,
		SOCKET_ASYNC_CMD_CONNECT,
		SOCKET_ASYNC_CMD_LISTEN,
		SOCKET_ASYNC_CMD_DISCONNECT,
		SOCKET_ASYNC_CMD_WRITE,
		SOCKET_ASYNC_CMD_RELEASE_RESOURCE,
};


struct BOF_SOCKET_ASYNC_CMD
{
		SOCKET_ASYNC_CMD Cmd_E;
		std::shared_ptr<void> psUserArg;
		BOF_NAMESPACE::BOF_EVENT *pNotifyEvent_X; //If null caller must poll on BofSocketChannelState to get op status otherwise the event will be nsignaled and caller can call BofSocketChannelState to get final status
		std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> psBofSocketChannel;
		uint32_t Ticket_U32;
		uint32_t StartTimeInMs_U32;
		uint32_t TimeOutInMs_U32;
		uint32_t pCmdArg_U32[4];
		bool ReUse_B;
		std::vector<BOF_BUFFER> BufferCollection;

		BOF_SOCKET_ASYNC_CMD()
		{
			Reset();
		}

		void Reset()
		{
			Cmd_E = SOCKET_ASYNC_CMD::SOCKET_ASYNC_CMD_NONE;
			psUserArg = nullptr;
			pNotifyEvent_X = nullptr;
			psBofSocketChannel = nullptr;
			Ticket_U32 = 0;
			StartTimeInMs_U32 = 0;
			TimeOutInMs_U32 = 0;
			BufferCollection.clear();
			ReUse_B = false;
			memset(pCmdArg_U32, 0, sizeof(pCmdArg_U32));
		}
};

struct BOF_SOCKET_CHANNEL_MANAGER_PARAM
{
		std::string Name_S;
		uint32_t NbMaxChannel_U32;
		int32_t Policy_S32;
		int32_t Priority_S32;
		uint32_t StartStopTimeoutInMs_U32;
		uint32_t CloseTimeoutInMs_U32;
		uint32_t ClosePollingTimeInMs_U32;

		BOF_SOCKET_CHANNEL_MANAGER_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			Name_S = "";
			NbMaxChannel_U32 = 0;
			Policy_S32 = BOF_THREAD_POLICY_OTHER;
			Priority_S32 = BOF_THREAD_DEFAULT_PRIORITY;
			StartStopTimeoutInMs_U32 = 5000;
			CloseTimeoutInMs_U32 = 1000;
			ClosePollingTimeInMs_U32 = 20;

		}

		bool operator==(const BOF_SOCKET_CHANNEL_MANAGER_PARAM &_Other) const
		{
			return ((Name_S == _Other.Name_S) &&
			        (NbMaxChannel_U32 == _Other.NbMaxChannel_U32) &&
			        (Policy_S32 == _Other.Policy_S32) &&
			        (Priority_S32 == _Other.Priority_S32) &&
			        (StartStopTimeoutInMs_U32 == _Other.StartStopTimeoutInMs_U32) &&
			        (CloseTimeoutInMs_U32 == _Other.CloseTimeoutInMs_U32) &&
			        (ClosePollingTimeInMs_U32 == _Other.ClosePollingTimeInMs_U32));
		}
};

struct UVW_CONTEXT
{
		std::string Name_S;
		void *pUserArg;

		UVW_CONTEXT()
		{
			Reset();
		}

		void Reset()
		{
			Name_S = "";
			pUserArg = nullptr;
		}

		UVW_CONTEXT(const std::string &_rName_S, void *_pUserArg)
		{
			Name_S = _rName_S;
			pUserArg = _pUserArg;
		}
};

struct BOF_SOCKET_CHANNEL_MANAGER_STATISTIC
{
		uint32_t NbAsyncEvent_U32;
		uint32_t NbAsyncCommand_U32;
		uint32_t NbAsyncEventError_U32;
		uint32_t NbTimerEvent_U32;
		uint32_t NbTimerEventError_U32;
		uint32_t NbAsyncCommandPending_U32;

		BOF_SOCKET_CHANNEL_MANAGER_STATISTIC()
		{
			Reset();
		}

		void Reset()
		{
			NbAsyncEvent_U32 = 0;
			NbAsyncCommand_U32 = 0;
			NbAsyncEventError_U32 = 0;
			NbTimerEvent_U32 = 0;
			NbTimerEventError_U32 = 0;
			NbAsyncCommandPending_U32 = 0;
		}
};

class BofSocketChannelManager : public BofThread, public std::enable_shared_from_this<BofSocketChannelManager>
{
private:
		BOF_SOCKET_CHANNEL_MANAGER_PARAM mBofSocketChannelManagerParam_X;
		BOF_SOCKET_CHANNEL_MANAGER_STATISTIC mBofSocketChannelManagerStatistic_X;
		BOF_MUTEX mBofSocketChannelManagerMutex;
		BOFERR mErrorCode_E = BOFERR_NO_ERROR;

		std::shared_ptr<uvw::Loop> mpsLoop = nullptr;
		std::shared_ptr<uvw::AsyncHandle> mpsAsync = nullptr;
		std::shared_ptr<uvw::TimerHandle> mpsManagerTimer = nullptr;

		//  std::map<std::shared_ptr<BofSocketChannel>, BOF_SOCKET_CHANNEL_PARAM>        mBofSocketChannelCollection;
		std::vector<std::shared_ptr<BofSocketChannel>> mBofSocketChannelCollection;
		std::unique_ptr<moodycamel::ReaderWriterQueue<BOF_SOCKET_ASYNC_CMD, 512>> mpAsyncCommandCollection = nullptr;       // Reserve space for at least 100 elements up front

		static uint32_t S_mAsyncCmdTicket_U32;
protected:
		BofSocketChannelManager(const BOF_SOCKET_CHANNEL_MANAGER_PARAM &_rBofSocketChannelManagerParam_X);

		virtual ~BofSocketChannelManager();

public:
		static std::shared_ptr<BofSocketChannelManager> S_BofSocketChannelManagerFactory(const BOF_SOCKET_CHANNEL_MANAGER_PARAM &_rBofSocketChannelManagerParam_X);

		//https://cpppatterns.com/patterns/rule-of-five.html
		BofSocketChannelManager(const BofSocketChannelManager &_rOther) = delete;

		BofSocketChannelManager(BofSocketChannelManager &&_rrOther) = delete;

		BofSocketChannelManager &operator=(const BofSocketChannelManager &_rOther) = delete;

		BofSocketChannelManager &operator=(BofSocketChannelManager &&_rrOther) = delete;

		BOFERR InitErrorCode() const;

		const BOF_SOCKET_CHANNEL_MANAGER_PARAM &BofSocketChannelManagerParam() const;

		const BOF_SOCKET_CHANNEL_MANAGER_STATISTIC &BofSocketChannelManagerStatistic();

		void ResetStat();

		BOFERR LockBofSocketChannelManager(const std::string &_rLockerName_S);

		BOFERR UnlockBofSocketChannelManager();

		const std::shared_ptr<uvw::Loop> &TheLoop() const;

		BOFERR AddChannel(const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X, std::shared_ptr<BofSocketChannel> &_rpsBofSocketChannel);

		//Internal use for listen/create client
		BOFERR InternalAddChannel(const BOF_SOCKET_CHANNEL_STATE &_rInitialBofChannelState_X, const std::shared_ptr<uvw::TcpHandle> &_rpsAlreadyCreatedTcpHandle,
		                          const std::shared_ptr<uvw::UDPHandle> &_rpsAlreadyCreatedUdpHandle, const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X,
		                          std::shared_ptr<BofSocketChannel> &_rpsBofSocketChannel);

		BOFERR RemoveChannel(const std::string &_rCaller_S, std::shared_ptr<BofSocketChannel> _psBofSocketChannel);  //No reference as it is called with shared_from_this
		uint32_t GetNumberOfChannel();

		BOFERR SendAsyncConnectCmd(uint32_t _TimeoutInMs_U32, const std::shared_ptr<void> &_rpsUserArg, const std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> &_rpsBofSocketChannel,
		                           BOF_NAMESPACE::BOF_EVENT *_pNotifyEvent_X);

		BOFERR SendAsyncDisconnectCmd(uint32_t _TimeoutInMs_U32, const std::shared_ptr<void> &_rpsUserArg, const std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> &_rpsBofSocketChannel, bool _ReUse_B,
		                              BOF_NAMESPACE::BOF_EVENT *_pNotifyEvent_X);

		BOFERR SendAsyncWriteCmd(uint32_t _TimeoutInMs_U32, const std::shared_ptr<void> &_rpsUserArg, const std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> &_rpsBofSocketChannel,
		                         const std::vector<BOF_BUFFER> &_rBufferCollection, BOF_NAMESPACE::BOF_EVENT *_pNotifyEvent_X);

		BOFERR SendAsyncListenCmd(uint32_t _TimeoutInMs_U32, const std::shared_ptr<void> &_rpsUserArg, const std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> &_rpsBofSocketChannel, uint32_t _MinPort_U32,
		                          uint32_t _MaxPort_U32, BOF_NAMESPACE::BOF_EVENT *_pNotifyEvent_X);

		BOFERR
		Write(uint32_t _TimeoutInMs_U32, const std::shared_ptr<void> &_rpsUserArg, const std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> &_rpsBofSocketChannel, bool _MakeACopy_B, uint32_t _Size_U32,
		      const uint8_t *_pBuffer_U8, BOF_NAMESPACE::BOF_EVENT *_pNotifyEvent_X);

		BOFERR Write(uint32_t _TimeoutInMs_U32, const std::shared_ptr<void> &_rpsUserArg, const std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> &_rpsBofSocketChannel, bool _MakeACopy_B,
		             const std::string _rBuffer_S, BOF_NAMESPACE::BOF_EVENT *_pNotifyEvent_X);

		BOFERR SendAsyncReleaseResourceCmd(uint32_t _TimeoutInMs_U32, const std::shared_ptr<void> &_rpsUserArg, const std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> &_rpsBofSocketChannel,
		                                   BOF_NAMESPACE::BOF_EVENT *_pNotifyEvent_X);

		void WaitAsyncCommandQueueEmpty(uint32_t _PollingTimeInMs_U32);

private:
		BOFERR OnTimerEvent(const uvw::TimerEvent &_rEvent, uvw::TimerHandle &_rHandle);

		BOFERR OnTimerError(const uvw::ErrorEvent &_rEvent, uvw::TimerHandle &_rHandle);

		BOFERR OnAsyncEvent(const uvw::AsyncEvent &_rEvent, uvw::AsyncHandle &_rHandle);

		BOFERR OnAsyncError(const uvw::ErrorEvent &_rEvent, uvw::AsyncHandle &_rHandle);

		BOFERR OnAsyncReleaseResource(const BOF_SOCKET_ASYNC_CMD &_rBofSocketAsyncCmd_X);

		std::mutex mLockGuardMutex;

		virtual BOFERR V_OnProcessing() override;

};


END_BOF_NAMESPACE()
