#include <bofstd/bofsocketchannelmanager.h>
#include <bofstd/bofsocketchannel.h>

BEGIN_BOF_NAMESPACE()


uint32_t BofSocketChannelManager::S_mAsyncCmdTicket_U32 = 0;

class BofSocketChannelManagerFactory : public BofSocketChannelManager
{
public:
		BofSocketChannelManagerFactory(const BOF_SOCKET_CHANNEL_MANAGER_PARAM &_rBofSocketChannelManagerParam_X) : BofSocketChannelManager(_rBofSocketChannelManagerParam_X)
		{}

		virtual ~BofSocketChannelManagerFactory()
		{}
};

std::shared_ptr<BofSocketChannelManager> BofSocketChannelManager::S_BofSocketChannelManagerFactory(const BOF_SOCKET_CHANNEL_MANAGER_PARAM &_rBofSocketChannelManagerParam_X)
{
	return std::make_shared<BofSocketChannelManagerFactory>(_rBofSocketChannelManagerParam_X);
}

BofSocketChannelManager::BofSocketChannelManager(const BOF_SOCKET_CHANNEL_MANAGER_PARAM &_rBofSocketChannelManagerParam_X)
	: BofThread()
{
//  DBG_OUT("============> create BofSocketChannelManager %s p %p\n", _rBofSocketChannelManagerParam_X.Name_S.c_str(),this);
	mBofSocketChannelManagerParam_X = _rBofSocketChannelManagerParam_X;
	mErrorCode_E = Bof_CreateMutex(_rBofSocketChannelManagerParam_X.Name_S + "_mtx", true, true, mBofSocketChannelManagerMutex);
	if (mErrorCode_E == BOFERR_NO_ERROR)
	{
		mErrorCode_E = BOFERR_NOT_ENOUGH_RESOURCE;
		mpsLoop = uvw::Loop::create();
		if (mpsLoop)
		{
			mpsLoop->data(std::make_shared<UVW_CONTEXT>(_rBofSocketChannelManagerParam_X.Name_S + "_mpsLoop", this));
			mErrorCode_E = BOFERR_NOT_ENOUGH_RESOURCE;
			mpsAsync = mpsLoop->resource<uvw::AsyncHandle>();
			if (mpsAsync)
			{
				mpsAsync->data(std::make_shared<UVW_CONTEXT>(_rBofSocketChannelManagerParam_X.Name_S + "_mpsAsync", this));
				mpsAsync->on<uvw::AsyncEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannelManager::OnAsyncEvent));
				mpsAsync->on<uvw::ErrorEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannelManager::OnAsyncError));

				mErrorCode_E = BOFERR_NOT_ENOUGH_RESOURCE;
				mpsManagerTimer = mpsLoop->resource<uvw::TimerHandle>();
				if (mpsManagerTimer)
				{
					mpsManagerTimer->data(std::make_shared<UVW_CONTEXT>(_rBofSocketChannelManagerParam_X.Name_S + "_mpsManagerTimer", this));
					mpsManagerTimer->on<uvw::TimerEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannelManager::OnTimerEvent));
					mpsManagerTimer->on<uvw::ErrorEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannelManager::OnTimerError));

					//        mpsTimer->again();  //To check error lambda
					mpsManagerTimer->start(uvw::TimerHandle::Time{0}, uvw::TimerHandle::Time{TIMER_GRANULARITY_IN_MS}); //use std::chrono::duration<uint64_t, std::milli>

					mErrorCode_E = BOFERR_NOT_ENOUGH_RESOURCE;
					mpAsyncCommandCollection.reset(new moodycamel::ReaderWriterQueue<BOF_SOCKET_ASYNC_CMD, 512>(128));
					if (mpAsyncCommandCollection)
					{
						mErrorCode_E = LaunchBofProcessingThread(mBofSocketChannelManagerParam_X.Name_S, false, 0, 0x10000, 0, mBofSocketChannelManagerParam_X.Policy_S32,
						                                         mBofSocketChannelManagerParam_X.Priority_S32, mBofSocketChannelManagerParam_X.StartStopTimeoutInMs_U32);
						//DBG_OUT("============> Thread BofSocketChannelManager %s STARTED %d\n", _rBofSocketChannelManagerParam_X.Name_S.c_str(), mErrorCode_E);

					}
				}
			}
		}
	}
}

BofSocketChannelManager::~BofSocketChannelManager()
{
	//bool              Sts_B;
	BOFERR Sts_E;
	//CHANNEL_RESOURCE_RELEASE_STATE ResourceReleaseState_E;
	//DBG_OUT("============> DELETE BofSocketChannelManager %s p %p\n", mBofSocketChannelManagerParam_X.Name_S.c_str(), this);

//  DBG_OUT("----> ~BofSocketChannelManager Start %d\n", mBofSocketChannelCollection.size());
	while (mBofSocketChannelCollection.size())
	{
		RemoveChannel("~BofSocketChannelManager", mBofSocketChannelCollection[0]);
		//ResourceReleaseState_E = ChannelResourceReleaseState();
		mBofSocketChannelCollection[0]->WaitForEndOfClose(mBofSocketChannelManagerParam_X.CloseTimeoutInMs_U32, mBofSocketChannelManagerParam_X.ClosePollingTimeInMs_U32);
	}
//  WaitAsyncCommandQueueEmpty(20); //MANDATORY to avoid extra uv callback while deleting the remaining object

//A pose probleme car on peut �tre appelle dans OnAsyncEvent quand BofSocketAsyncCmd_X.psBofSocketChannel.use_count() atteint 1->on est appelle dans le contexte uv->wait marche pas
	//The SendAsyncReleaseResource with nullptr as _rpsBofSocketChannel is used to send a uv_walk/uv_close on all handle->the libuv loop will exit.

	BOF_NAMESPACE::BOF_EVENT NotifyEvent_X;
	BOF_NAMESPACE::Bof_CreateEvent("RelMgrResEvnt_" + mBofSocketChannelManagerParam_X.Name_S, false, 1, false, NotifyEvent_X);
	Sts_E = SendAsyncReleaseResourceCmd(mBofSocketChannelManagerParam_X.StartStopTimeoutInMs_U32, nullptr, nullptr, &NotifyEvent_X);
	BOF_ASSERT(Sts_E == BOFERR_NO_ERROR);
	if (Sts_E == BOFERR_NO_ERROR)
	{
		Sts_E = Bof_WaitForEvent(NotifyEvent_X, mBofSocketChannelManagerParam_X.StartStopTimeoutInMs_U32, 0);
		if (Sts_E != BOFERR_NO_ERROR)
		{
			Sts_E = Sts_E;
		}
		BOF_ASSERT(Sts_E == BOFERR_NO_ERROR);
	}

	//BOF_ASSERT(Sts_B);

	Bof_DestroyMutex(mBofSocketChannelManagerMutex);
	//DBG_OUT("----> ~BofSocketChannelManager End\n");
}


BOFERR BofSocketChannelManager::InitErrorCode() const
{
	return mErrorCode_E;
}

const BOF_SOCKET_CHANNEL_MANAGER_PARAM &BofSocketChannelManager::BofSocketChannelManagerParam() const
{
	return mBofSocketChannelManagerParam_X;
}

const BOF_SOCKET_CHANNEL_MANAGER_STATISTIC &BofSocketChannelManager::BofSocketChannelManagerStatistic()
{
	mBofSocketChannelManagerStatistic_X.NbAsyncCommandPending_U32 = static_cast<uint32_t>(mpAsyncCommandCollection->size_approx());
	return mBofSocketChannelManagerStatistic_X;
}

void BofSocketChannelManager::ResetStat()
{
	mBofSocketChannelManagerStatistic_X.Reset();
}

BOFERR BofSocketChannelManager::LockBofSocketChannelManager(const std::string &/*_rLockerName_S*/)
{
	BOFERR Rts_E = Bof_LockMutex(mBofSocketChannelManagerMutex);
	return Rts_E;
}

BOFERR BofSocketChannelManager::UnlockBofSocketChannelManager()
{
	return Bof_UnlockMutex(mBofSocketChannelManagerMutex);
}

const std::shared_ptr<uvw::Loop> &BofSocketChannelManager::TheLoop() const
{
	return mpsLoop;
}

BOFERR BofSocketChannelManager::OnTimerEvent(const uvw::TimerEvent &/*_rEvent*/, uvw::TimerHandle &_rHandle)
{
	BOFERR Rts_E = mErrorCode_E;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
		mBofSocketChannelManagerStatistic_X.NbTimerEvent_U32++;
		Rts_E = BOFERR_NO_ERROR;
	}
	return Rts_E;
}

BOFERR BofSocketChannelManager::OnTimerError(const uvw::ErrorEvent &/*_rEvent*/, uvw::TimerHandle &_rHandle)
{
	BOFERR Rts_E = mErrorCode_E;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
		mBofSocketChannelManagerStatistic_X.NbTimerEventError_U32++;
//    DBG_OUT("TimerManager(%s) error %s %s code %d\n", psUvwContext_X->Name_S.c_str(), _rEvent.name(), _rEvent.what(), _rEvent.code());
		Rts_E = BOFERR_NO_ERROR;
	}
	return Rts_E;
}

BOFERR BofSocketChannelManager::OnAsyncEvent(const uvw::AsyncEvent &/*_rEvent*/, uvw::AsyncHandle &_rHandle)
{
	std::lock_guard<std::mutex> LockGuard(mLockGuardMutex);  //needed by WaitAsyncCommandQueueEmpty

	BOFERR Rts_E = mErrorCode_E;
	BOF_SOCKET_ASYNC_CMD BofSocketAsyncCmd_X;
	bool Finish_B;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
		Rts_E = BOFERR_NO_ERROR;
		mBofSocketChannelManagerStatistic_X.NbAsyncEvent_U32++;

		Finish_B = false;
		while (!Finish_B)
		{
			Rts_E = BOFERR_EMPTY;
			if (mpAsyncCommandCollection->try_dequeue(
				BofSocketAsyncCmd_X)) // Purge all pending cmd as uv_async_send states calling this function may not wakeup the event loop if it was already called previously within a short period of time
			{
				if (BofSocketAsyncCmd_X.Cmd_E != SOCKET_ASYNC_CMD::SOCKET_ASYNC_CMD_RELEASE_RESOURCE)
				{
					BOF_ASSERT(BofSocketAsyncCmd_X.psBofSocketChannel != nullptr);
				}
				BOF_ASSERT(BofSocketAsyncCmd_X.TimeOutInMs_U32 >= MIN_TIMEOUT_VALUE_IN_MS); //Needed for to works

				mBofSocketChannelManagerStatistic_X.NbAsyncCommand_U32++;

				DBG_OUT("----> OnAsyncEvent cmd %d AEvnt %d ACmd %d\n", static_cast<uint32_t>(BofSocketAsyncCmd_X.Cmd_E), mBofSocketChannelManagerStatistic_X.NbAsyncEvent_U32,
				        mBofSocketChannelManagerStatistic_X.NbAsyncCommand_U32);

				Rts_E = BOFERR_INVALID_PARAM;
				switch (BofSocketAsyncCmd_X.Cmd_E)
				{
					case SOCKET_ASYNC_CMD::SOCKET_ASYNC_CMD_NONE:
						break;

					case SOCKET_ASYNC_CMD::SOCKET_ASYNC_CMD_CONNECT:
						Rts_E = BofSocketAsyncCmd_X.psBofSocketChannel->OnAsyncConnect(BofSocketAsyncCmd_X);
						break;

					case SOCKET_ASYNC_CMD::SOCKET_ASYNC_CMD_LISTEN:
						Rts_E = BofSocketAsyncCmd_X.psBofSocketChannel->OnAsyncListen(BofSocketAsyncCmd_X);
						break;

					case SOCKET_ASYNC_CMD::SOCKET_ASYNC_CMD_DISCONNECT:
						Rts_E = BofSocketAsyncCmd_X.psBofSocketChannel->OnAsyncDisconnect(BofSocketAsyncCmd_X);
						break;

					case SOCKET_ASYNC_CMD::SOCKET_ASYNC_CMD_WRITE:
						Rts_E = BofSocketAsyncCmd_X.psBofSocketChannel->OnAsyncWrite(BofSocketAsyncCmd_X);
						break;

					case SOCKET_ASYNC_CMD::SOCKET_ASYNC_CMD_RELEASE_RESOURCE:
						Rts_E = OnAsyncReleaseResource(BofSocketAsyncCmd_X);
						break;

					default:
						Rts_E = BOFERR_INVALID_COMMAND;
						break;
				}
				BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);
			}
			else
			{
				Finish_B = true;
			}
		}
	}
	// int y = BofSocketAsyncCmd_X.psBofSocketChannel.use_count();
	return Rts_E;
}

BOFERR BofSocketChannelManager::OnAsyncError(const uvw::ErrorEvent &/*_rEvent*/, uvw::AsyncHandle &_rHandle)
{
	BOFERR Rts_E = mErrorCode_E;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
		mBofSocketChannelManagerStatistic_X.NbAsyncEventError_U32++;
		Rts_E = BOFERR_NO_ERROR;
	}
	return Rts_E;
}

BOFERR BofSocketChannelManager::OnAsyncReleaseResource(const BOF_SOCKET_ASYNC_CMD &_rBofSocketAsyncCmd_X)
{
	bool NotifyEvent_B;
	uint32_t Count_U32 = 0;
	BOFERR Rts_E = mErrorCode_E;
	CHANNEL_RESOURCE_RELEASE_STATE ResourceReleaseState_E;

	if (_rBofSocketAsyncCmd_X.psBofSocketChannel == nullptr)
	{
		DBG_OUT("OnAsyncReleaseResource cleanup called by manager destructor\n");
	}
	else
	{
		DBG_OUT("OnAsyncReleaseResource cleanup called by channel %p destructor\n", static_cast<void *>(_rBofSocketAsyncCmd_X.psBofSocketChannel.get()));
	}
	//if (Rts_E == BOFERR_NO_ERROR)
	{
		NotifyEvent_B = true;
		if (_rBofSocketAsyncCmd_X.psBofSocketChannel == nullptr)
		{
//Final destructor of BofSocketChannelManager call this method with _rBofSocketAsyncCmd_X.psBofSocketChannel == nullptr
			mpsLoop->walk([&Count_U32](uvw::BaseHandle &_rBaseHandle) {
					++Count_U32;
					_rBaseHandle.close();
			});
//      DBG_OUT("kill %d alive manager resources\n", Count_U32);
			Rts_E = BOFERR_NO_ERROR;
		}
		else
		{
			ResourceReleaseState_E = _rBofSocketAsyncCmd_X.psBofSocketChannel->ChannelResourceReleaseState();
			if (ResourceReleaseState_E != CHANNEL_RESOURCE_RELEASE_STATE::PENDING)
			{
				printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 1: state %d on %p\n", static_cast<uint32_t>(ResourceReleaseState_E), static_cast<void *>(_rBofSocketAsyncCmd_X.psBofSocketChannel.get()));
			}
			BOF_ASSERT(ResourceReleaseState_E == CHANNEL_RESOURCE_RELEASE_STATE::PENDING);
			Rts_E = _rBofSocketAsyncCmd_X.psBofSocketChannel->CleanUpFromLibUvContext();
			_rBofSocketAsyncCmd_X.psBofSocketChannel->ChannelResourceReleaseState(CHANNEL_RESOURCE_RELEASE_STATE::CLOSING);
		}

		if (NotifyEvent_B)
		{
			if (_rBofSocketAsyncCmd_X.pNotifyEvent_X)
			{
				Bof_SignalEvent(*_rBofSocketAsyncCmd_X.pNotifyEvent_X, 0);
			}
		}
	}
	return Rts_E;
}

BOFERR BofSocketChannelManager::AddChannel(const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X, std::shared_ptr<BofSocketChannel> &_rpsBofSocketChannel)
{
	std::shared_ptr<BofSocketChannel> psBofSocketChannel;
	BOFERR Rts_E = mErrorCode_E;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = LockBofSocketChannelManager("AddChannel");
		if (Rts_E == BOFERR_NO_ERROR)
		{
			Rts_E = BOFERR_NO_MORE_HANDLE;
			if (mBofSocketChannelCollection.size() < mBofSocketChannelManagerParam_X.NbMaxChannel_U32)
			{
				psBofSocketChannel = BofSocketChannel::S_BofSocketChannelFactory(shared_from_this(), _rBofSocketChannelParam_X);
				if (psBofSocketChannel)
				{
					Rts_E = psBofSocketChannel->InitErrorCode();
					if (Rts_E == BOFERR_NO_ERROR)
					{
						_rpsBofSocketChannel = psBofSocketChannel;
						mBofSocketChannelCollection.push_back(_rpsBofSocketChannel);
//            DBG_OUT("----> AddChannel %s nb %d\n", _rBofSocketChannelParam_X.Name_S.c_str(), mBofSocketChannelCollection.size());
						Rts_E = BOFERR_NO_ERROR;
					}
				}
			}
			UnlockBofSocketChannelManager();
		}
	}
	return Rts_E;
}

//Internal use for listen/create client
BOFERR BofSocketChannelManager::InternalAddChannel(const BOF_SOCKET_CHANNEL_STATE &_rInitialBofChannelState_X, const std::shared_ptr<uvw::TcpHandle> &_rpsAlreadyCreatedTcpHandle,
                                                   const std::shared_ptr<uvw::UDPHandle> &_rpsAlreadyCreatedUdpHandle, const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X,
                                                   std::shared_ptr<BofSocketChannel> &_rpsBofSocketChannel)
{
	std::shared_ptr<BofSocketChannel> psBofSocketChannel;
	BOFERR Rts_E = mErrorCode_E;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = LockBofSocketChannelManager("InternalAddChannel");
		if (Rts_E == BOFERR_NO_ERROR)
		{
			Rts_E = BOFERR_NO_MORE_HANDLE;
			if (mBofSocketChannelCollection.size() < mBofSocketChannelManagerParam_X.NbMaxChannel_U32)
			{
				psBofSocketChannel = BofSocketChannel::S_BofSocketChannelFactory(_rInitialBofChannelState_X, _rpsAlreadyCreatedTcpHandle, _rpsAlreadyCreatedUdpHandle, shared_from_this(),
				                                                                 _rBofSocketChannelParam_X);
				if (psBofSocketChannel)
				{
					Rts_E = psBofSocketChannel->InitErrorCode();
					if (Rts_E == BOFERR_NO_ERROR)
					{
						_rpsBofSocketChannel = psBofSocketChannel;
						mBofSocketChannelCollection.push_back(_rpsBofSocketChannel);
//            DBG_OUT("----> InternalAddChannel %s nb %d\n", _rBofSocketChannelParam_X.Name_S.c_str(), mBofSocketChannelCollection.size());
						Rts_E = BOFERR_NO_ERROR;
					}
				}
			}
			UnlockBofSocketChannelManager();
		}
	}
	return Rts_E;
}

BOFERR BofSocketChannelManager::RemoveChannel(const std::string &/*_rCaller_S*/, std::shared_ptr<BofSocketChannel> _psBofSocketChannel)
{
	std::shared_ptr<BofSocketChannel> psBofSocketChannel;
	BOFERR Rts_E = mErrorCode_E;
	CHANNEL_RESOURCE_RELEASE_STATE ResourceReleaseState_E;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = LockBofSocketChannelManager("RemoveChannel");
		if (Rts_E == BOFERR_NO_ERROR)
		{
			Rts_E = BOFERR_NOT_FOUND;
			auto It = std::find(mBofSocketChannelCollection.begin(), mBofSocketChannelCollection.end(), _psBofSocketChannel);
			if (It != mBofSocketChannelCollection.end())
			{
				mBofSocketChannelCollection.erase(It);  //So the SendAsyncReleaseResourceCmd can ponly be made 1 time
				ResourceReleaseState_E = _psBofSocketChannel->ChannelResourceReleaseState();
				DBG_OUT("----> RemoveChannel %s state %d\n", _psBofSocketChannel->BofSocketChannelParam().Name_S.c_str(), static_cast<uint32_t>(ResourceReleaseState_E));
				if (ResourceReleaseState_E == CHANNEL_RESOURCE_RELEASE_STATE::IDLE)
				{
					_psBofSocketChannel->ChannelResourceReleaseState(
						CHANNEL_RESOURCE_RELEASE_STATE::PENDING);  //Mark channel as waiting for uv to close it->to avoid weak_ptr expired-> destructor of BofSocketChannel
					Rts_E = SendAsyncReleaseResourceCmd(mBofSocketChannelManagerParam_X.StartStopTimeoutInMs_U32, nullptr, _psBofSocketChannel, nullptr);
					if (Rts_E == BOFERR_NO_ERROR)
					{
						//No we are in a uv callback Rts_E = Bof_WaitForEvent(NotifyEvent_X, mBofSocketChannelManagerParam_X.StartStopTimeoutInMs_U32);
					}
				}
				else
				{
					Rts_E = BOFERR_NO_ERROR;
				}
			}
			UnlockBofSocketChannelManager();
		}
	}
//  DBG_OUT("----> RemoveChannel %s by %s->%s\n", _psBofSocketChannel->BofSocketChannelParam().Name_S.c_str(), _rCaller_S.c_str(), BOF_NAMESPACE::Bof_ErrorCode(Rts_E));
	return Rts_E;
}

uint32_t BofSocketChannelManager::GetNumberOfChannel()
{
	return static_cast<uint32_t>(mBofSocketChannelCollection.size());
}

void BofSocketChannelManager::WaitAsyncCommandQueueEmpty(uint32_t _PollingTimeInMs_U32)
{
//Wait queue is empy
	while (mpAsyncCommandCollection->size_approx())
	{
		Bof_Sleep(_PollingTimeInMs_U32);
	}
//Wait end of queue processing
//  while (mInAsyncEventProcessing_B)
	{
//    Bof_Sleep(_PollTimeInMs_U32);
	}
	//std::lock_guard<std::mutex> LockGuard(mLockGuardMutex);  //used by OnAsyncEvent
}

BOFERR BofSocketChannelManager::SendAsyncConnectCmd(uint32_t _TimeoutInMs_U32, const std::shared_ptr<void> &_rpsUserArg, const std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> &_rpsBofSocketChannel,
                                                    BOF_NAMESPACE::BOF_EVENT *_pNotifyEvent_X)
{
	BOFERR Rts_E = mErrorCode_E;
	BOF_SOCKET_ASYNC_CMD BofSocketAsyncCmd_X;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = BOFERR_INVALID_PARAM;
		if ((_TimeoutInMs_U32 >= MIN_TIMEOUT_VALUE_IN_MS) && (_rpsBofSocketChannel))//Needed for to works
		{
			Rts_E = BOFERR_ALREADY_OPENED;
			if (!_rpsBofSocketChannel->IsConnected())
			{
				BofSocketAsyncCmd_X.Cmd_E = SOCKET_ASYNC_CMD::SOCKET_ASYNC_CMD_CONNECT;
				BofSocketAsyncCmd_X.psUserArg = _rpsUserArg;
				BofSocketAsyncCmd_X.pNotifyEvent_X = _pNotifyEvent_X;
				BofSocketAsyncCmd_X.psBofSocketChannel = _rpsBofSocketChannel;
				BofSocketAsyncCmd_X.Ticket_U32 = ++S_mAsyncCmdTicket_U32;
				BofSocketAsyncCmd_X.StartTimeInMs_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
				BofSocketAsyncCmd_X.TimeOutInMs_U32 = _TimeoutInMs_U32;
				Rts_E = BOFERR_FULL;
				if (mpAsyncCommandCollection->enqueue(BofSocketAsyncCmd_X))
				{
					Rts_E = BOFERR_NO_ERROR;
					mpsAsync->send();
				}
			}
		}
	}
	return Rts_E;
}

BOFERR
BofSocketChannelManager::SendAsyncDisconnectCmd(uint32_t _TimeoutInMs_U32, const std::shared_ptr<void> &_rpsUserArg, const std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> &_rpsBofSocketChannel,
                                                bool _ReUse_B, BOF_NAMESPACE::BOF_EVENT *_pNotifyEvent_X)
{
	BOFERR Rts_E = mErrorCode_E;
	BOF_SOCKET_ASYNC_CMD BofSocketAsyncCmd_X;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = BOFERR_INVALID_PARAM;
		if ((_TimeoutInMs_U32 >= MIN_TIMEOUT_VALUE_IN_MS) && (_rpsBofSocketChannel))//Needed for to works
		{
			Rts_E = BOFERR_NOT_OPENED;
			if (_rpsBofSocketChannel->IsConnected())
			{
				BofSocketAsyncCmd_X.Cmd_E = SOCKET_ASYNC_CMD::SOCKET_ASYNC_CMD_DISCONNECT;
				BofSocketAsyncCmd_X.psUserArg = _rpsUserArg;
				BofSocketAsyncCmd_X.pNotifyEvent_X = _pNotifyEvent_X;
				BofSocketAsyncCmd_X.psBofSocketChannel = _rpsBofSocketChannel;
				BofSocketAsyncCmd_X.Ticket_U32 = ++S_mAsyncCmdTicket_U32;
				BofSocketAsyncCmd_X.StartTimeInMs_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
				BofSocketAsyncCmd_X.TimeOutInMs_U32 = _TimeoutInMs_U32;
				BofSocketAsyncCmd_X.pCmdArg_U32[0] = _ReUse_B ? 0x00000001 : 0x00000000;
				Rts_E = BOFERR_FULL;
				if (mpAsyncCommandCollection->enqueue(BofSocketAsyncCmd_X))
				{
					Rts_E = BOFERR_NO_ERROR;
					mpsAsync->send();
				}
			}
		}
	}
	return Rts_E;
}

BOFERR BofSocketChannelManager::SendAsyncListenCmd(uint32_t _TimeoutInMs_U32, const std::shared_ptr<void> &_rpsUserArg, const std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> &_rpsBofSocketChannel,
                                                   uint32_t _MinPort_U32, uint32_t _MaxPort_U32, BOF_NAMESPACE::BOF_EVENT *_pNotifyEvent_X)
{
	BOFERR Rts_E = mErrorCode_E;
	BOF_SOCKET_ASYNC_CMD BofSocketAsyncCmd_X;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = BOFERR_INVALID_PARAM;
		if ((_TimeoutInMs_U32 >= MIN_TIMEOUT_VALUE_IN_MS) && (_rpsBofSocketChannel))//Needed for to works
		{
			Rts_E = BOFERR_ALREADY_OPENED;
			if (!_rpsBofSocketChannel->IsConnected())
			{
				BofSocketAsyncCmd_X.Cmd_E = SOCKET_ASYNC_CMD::SOCKET_ASYNC_CMD_LISTEN;
				BofSocketAsyncCmd_X.psUserArg = _rpsUserArg;
				BofSocketAsyncCmd_X.pNotifyEvent_X = _pNotifyEvent_X;
				BofSocketAsyncCmd_X.psBofSocketChannel = _rpsBofSocketChannel;
				BofSocketAsyncCmd_X.Ticket_U32 = ++S_mAsyncCmdTicket_U32;
				BofSocketAsyncCmd_X.StartTimeInMs_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
				BofSocketAsyncCmd_X.TimeOutInMs_U32 = _TimeoutInMs_U32;
				BofSocketAsyncCmd_X.pCmdArg_U32[0] = _MinPort_U32;
				BofSocketAsyncCmd_X.pCmdArg_U32[1] = _MaxPort_U32;
				Rts_E = BOFERR_FULL;
				if (mpAsyncCommandCollection->enqueue(BofSocketAsyncCmd_X))
				{
					Rts_E = BOFERR_NO_ERROR;
					mpsAsync->send();
				}
			}
		}
	}
	return Rts_E;
}

BOFERR BofSocketChannelManager::SendAsyncWriteCmd(uint32_t _TimeoutInMs_U32, const std::shared_ptr<void> &_rpsUserArg, const std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> &_rpsBofSocketChannel,
                                                  const std::vector<BOF_BUFFER> &_rBufferCollection, BOF_NAMESPACE::BOF_EVENT *_pNotifyEvent_X)
{
	BOF_SOCKET_ASYNC_CMD BofSocketAsyncCmd_X;
	BOFERR Rts_E = mErrorCode_E;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = BOFERR_INVALID_PARAM;
		if ((_TimeoutInMs_U32 >= MIN_TIMEOUT_VALUE_IN_MS) && (_rpsBofSocketChannel))//Needed for to works
		{
			Rts_E = BOFERR_NOT_OPENED;
			if (_rpsBofSocketChannel->IsConnected())
			{
				BofSocketAsyncCmd_X.Cmd_E = SOCKET_ASYNC_CMD::SOCKET_ASYNC_CMD_WRITE;
				BofSocketAsyncCmd_X.psUserArg = _rpsUserArg;
				BofSocketAsyncCmd_X.pNotifyEvent_X = _pNotifyEvent_X;
				BofSocketAsyncCmd_X.psBofSocketChannel = _rpsBofSocketChannel;
				BofSocketAsyncCmd_X.Ticket_U32 = ++S_mAsyncCmdTicket_U32;
				BofSocketAsyncCmd_X.StartTimeInMs_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
				BofSocketAsyncCmd_X.TimeOutInMs_U32 = _TimeoutInMs_U32;
				BofSocketAsyncCmd_X.BufferCollection = _rBufferCollection;
				Rts_E = BOFERR_FULL;
				if (mpAsyncCommandCollection->enqueue(BofSocketAsyncCmd_X))
				{
					Rts_E = BOFERR_NO_ERROR;
					mpsAsync->send();
				}
			}
		}
	}
	return Rts_E;
}

BOFERR
BofSocketChannelManager::SendAsyncReleaseResourceCmd(uint32_t _TimeoutInMs_U32, const std::shared_ptr<void> &_rpsUserArg, const std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> &_rpsBofSocketChannel,
                                                     BOF_NAMESPACE::BOF_EVENT *_pNotifyEvent_X)
{
	BOF_SOCKET_ASYNC_CMD BofSocketAsyncCmd_X;
	BOFERR Rts_E = mErrorCode_E;

	//int y = _rpsBofSocketChannel.use_count();
	//BofSocketChannel *p = _rpsBofSocketChannel.get();

	if (Rts_E == BOFERR_NO_ERROR)
	{
		Rts_E = BOFERR_INVALID_PARAM;
		if (_TimeoutInMs_U32 >= MIN_TIMEOUT_VALUE_IN_MS)
		{
			if (_rpsBofSocketChannel)
			{
				printf("!!!!!!!!!!!!!!!!!SendAsyncReleaseResourceCmd!!!!!!!!!!!!!!! 4: state %d on %p\n", static_cast<uint32_t>(_rpsBofSocketChannel->ChannelResourceReleaseState()),
				       static_cast<void *>(_rpsBofSocketChannel.get()));
			}
			BofSocketAsyncCmd_X.Cmd_E = SOCKET_ASYNC_CMD::SOCKET_ASYNC_CMD_RELEASE_RESOURCE;  //if _rpsBofSocketChannel is nullptr->Call all res of libuv (Manager delete)
			BofSocketAsyncCmd_X.psUserArg = _rpsUserArg;
			BofSocketAsyncCmd_X.pNotifyEvent_X = _pNotifyEvent_X;
			BofSocketAsyncCmd_X.psBofSocketChannel = _rpsBofSocketChannel;
			BofSocketAsyncCmd_X.Ticket_U32 = ++S_mAsyncCmdTicket_U32;
			BofSocketAsyncCmd_X.StartTimeInMs_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
			BofSocketAsyncCmd_X.TimeOutInMs_U32 = _TimeoutInMs_U32;

			Rts_E = BOFERR_FULL;
			if (mpAsyncCommandCollection->enqueue(BofSocketAsyncCmd_X))
			{
				Rts_E = BOFERR_NO_ERROR;
				mpsAsync->send();
			}
		}
	}
	return Rts_E;
}

BOFERR
BofSocketChannelManager::Write(uint32_t _TimeoutInMs_U32, const std::shared_ptr<void> &_rpsUserArg, const std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> &_rpsBofSocketChannel, bool _MakeACopy_B,
                               uint32_t _Size_U32, const uint8_t *_pBuffer_U8, BOF_NAMESPACE::BOF_EVENT *_pNotifyEvent_X)
{
	BOFERR Rts_E;
	std::vector<BOF_BUFFER> BufferCollection;
	uint8_t *pBuffer_U8;

	Rts_E = BOFERR_NOT_OPENED;
	if (_rpsBofSocketChannel->IsConnected())
	{
		Rts_E = BOFERR_NO_ERROR;
		if (_MakeACopy_B)
		{
			pBuffer_U8 = new uint8_t[_Size_U32];
			if (pBuffer_U8)
			{
				memcpy(pBuffer_U8, _pBuffer_U8, _Size_U32);
				BufferCollection.push_back(BOF_BUFFER(nullptr, _MakeACopy_B, false, _Size_U32, const_cast<uint8_t *>(pBuffer_U8)));
			}
			else
			{
				Rts_E = BOFERR_NOT_ENOUGH_RESOURCE;
			}
		}
		else
		{
			BufferCollection.push_back(BOF_BUFFER(nullptr, _MakeACopy_B, false, _Size_U32, const_cast<uint8_t *>(_pBuffer_U8)));
		}
		if (Rts_E == BOFERR_NO_ERROR)
		{
			Rts_E = SendAsyncWriteCmd(_TimeoutInMs_U32, _rpsUserArg, _rpsBofSocketChannel, BufferCollection, _pNotifyEvent_X);
		}
	}
	return Rts_E;

}

BOFERR
BofSocketChannelManager::Write(uint32_t _TimeoutInMs_U32, const std::shared_ptr<void> &_rpsUserArg, const std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> &_rpsBofSocketChannel, bool _MakeACopy_B,
                               const std::string _rBuffer_S, BOF_NAMESPACE::BOF_EVENT *_pNotifyEvent_X)
{
	return Write(_TimeoutInMs_U32, _rpsUserArg, _rpsBofSocketChannel, _MakeACopy_B, static_cast<uint32_t>(_rBuffer_S.size()), reinterpret_cast<const uint8_t *>(_rBuffer_S.c_str()), _pNotifyEvent_X);
}

BOFERR BofSocketChannelManager::V_OnProcessing()
{
	BOFERR Rts_E = BOFERR_INIT;

	//DBG_OUT("BofSocketChannelManager thread START...\n");
	if (mpsLoop)
	{
		mpsLoop->run(); //<uvw::details::UVRunMode::DEFAULT>
		Rts_E = BOFERR_CANCEL;                                                                                   // For bofthread exit
	}
	//DBG_OUT("BofSocketChannelManager thread exit\n");
	return Rts_E;
}


END_BOF_NAMESPACE()