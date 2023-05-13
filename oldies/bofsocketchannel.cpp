/*
 * Copyright (c) 2013-2023, Evs Broadcast Equipment All rights reserved.
 *
 * Author:      Bernard HARMEL: b.harmel@evs.com
 * Web:				 www.evs.com
 * Revision:    1.0
 *
 * Rem:         Nothing
 *https://eli.thegreenplace.net/2017/concurrent-servers-part-4-libuv/
 * History:
 *
 * V 1.00  Dec 19 2017  BHA : Initial release
 */
#include <bofstd/bofsocketchannel.h>

BEGIN_BOF_NAMESPACE()

struct BOF_TCP_SOCKET_PARAM
{
  bool NoDelay_B;
  bool SimultaneousAccept_B;
  uint32_t KeepAliveTimeInMs_U32; // 0 means not used
  uint32_t RcvBufferSize_U32;
  uint32_t SndBufferSize_U32;
  ON_SOCKET_CONNECT_CALLBACK OnSocketConnectCallback;
  ON_SOCKET_ACCEPT_CALLBACK OnSocketAcceptCallback;
  ON_SOCKET_READ_CALLBACK OnSocketReadCallback;

  BOF_TCP_SOCKET_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    NoDelay_B = false;
    SimultaneousAccept_B = false;
    KeepAliveTimeInMs_U32 = 0;
    RcvBufferSize_U32 = 0;
    SndBufferSize_U32 = 0;
    OnSocketConnectCallback = nullptr;
    OnSocketAcceptCallback = nullptr;
    OnSocketReadCallback = nullptr;
  }
};

struct BOF_UDP_SOCKET_PARAM
{
  bool MulticastReceiver_B;
  bool MulticastLoop_B;
  bool Broadcast_B;
  uint32_t Ttl_U32;
  uint32_t RcvBufferSize_U32;
  uint32_t SndBufferSize_U32;
  ON_SOCKET_READ_CALLBACK OnSocketReadCallback;

  BOF_UDP_SOCKET_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    MulticastReceiver_B = false;
    MulticastLoop_B = false;
    Broadcast_B = false;
    Ttl_U32 = 0;
    RcvBufferSize_U32 = 0;
    SndBufferSize_U32 = 0;
    OnSocketReadCallback = nullptr;
  }
};

class BofSocketChannelFactory : public BofSocketChannel
{
public:
  BofSocketChannelFactory(const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager, const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X) : BofSocketChannel(_rpsBofSocketChannelManager, _rBofSocketChannelParam_X)
  {
  }

  BofSocketChannelFactory(const BOF_SOCKET_CHANNEL_STATE &_rInitialBofChannelState_X, const std::shared_ptr<uvw::TcpHandle> &_rpsAlreadyCreatedTcpHandle, const std::shared_ptr<uvw::UDPHandle> &_rpsAlreadyCreatedUdpHandle,
                          const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager, const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X)
      : BofSocketChannel(_rInitialBofChannelState_X, _rpsAlreadyCreatedTcpHandle, _rpsAlreadyCreatedUdpHandle, _rpsBofSocketChannelManager, _rBofSocketChannelParam_X)
  {
  }

  virtual ~BofSocketChannelFactory()
  {
  }
};

std::shared_ptr<BofSocketChannel> BofSocketChannel::S_BofSocketChannelFactory(const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager, const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X)
{
  return std::make_shared<BofSocketChannelFactory>(_rpsBofSocketChannelManager, _rBofSocketChannelParam_X);
}

// Internal use for listen/create client
std::shared_ptr<BofSocketChannel> BofSocketChannel::S_BofSocketChannelFactory(const BOF_SOCKET_CHANNEL_STATE &_rInitialBofChannelState_X, const std::shared_ptr<uvw::TcpHandle> &_rpsAlreadyCreatedTcpHandle,
                                                                              const std::shared_ptr<uvw::UDPHandle> &_rpsAlreadyCreatedUdpHandle, const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager,
                                                                              const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X)
{
  return std::make_shared<BofSocketChannelFactory>(_rInitialBofChannelState_X, _rpsAlreadyCreatedTcpHandle, _rpsAlreadyCreatedUdpHandle, _rpsBofSocketChannelManager, _rBofSocketChannelParam_X);
}

BofSocketChannel::BofSocketChannel(const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager, const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X)
{
  mErrorCode_E = SetupBofSocketChannel(mBofSocketChannelState_X, nullptr, nullptr, _rpsBofSocketChannelManager, _rBofSocketChannelParam_X);
}

// Internal use for listen/create client
BofSocketChannel::BofSocketChannel(const BOF_SOCKET_CHANNEL_STATE &_rInitialBofChannelState_X, const std::shared_ptr<uvw::TcpHandle> &_rpsAlreadyCreatedTcpHandle, const std::shared_ptr<uvw::UDPHandle> &_rpsAlreadyCreatedUdpHandle,
                                   const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager, const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X)
{
  mErrorCode_E = SetupBofSocketChannel(_rInitialBofChannelState_X, _rpsAlreadyCreatedTcpHandle, _rpsAlreadyCreatedUdpHandle, _rpsBofSocketChannelManager, _rBofSocketChannelParam_X);
}

BofSocketChannel::~BofSocketChannel()
{
  CHANNEL_RESOURCE_RELEASE_STATE ResourceReleaseState_E;

  DBG_OUT("----> ~BofSocketChannel Start %s ChannelResourceReleaseState %d\n", mBofSocketChannelParam_X.Name_S.c_str(), static_cast<uint32_t>(ChannelResourceReleaseState()));
  ResourceReleaseState_E = ChannelResourceReleaseState();
  if (ResourceReleaseState_E == CHANNEL_RESOURCE_RELEASE_STATE::IDLE)
  {
    if (mpsBofSocketChannelManager)
    {
      mpsBofSocketChannelManager->RemoveChannel("~BofSocketChannel", shared_from_this());
    }
  }
  WaitForEndOfClose(mBofSocketChannelParam_X.CloseTimeoutInMs_U32, mBofSocketChannelParam_X.ClosePollingTimeInMs_U32);

  if (!mDataPreAllocated_B)
  {
    BOF_SAFE_DELETE_ARRAY(mpDataBuffer_U8);
  }
  Bof_DestroyMutex(mBofSocketChannelMutex);

  //  DBG_OUT("----> ~BofSocketChannel End %s\n", mBofSocketChannelParam_X.Name_S.c_str());
}

// This must be calleb through the SendAsyncReleaseResourceCmd interface
BOFERR BofSocketChannel::CleanUpFromLibUvContext()
{
  BOFERR Rts_E = mErrorCode_E;

  //  DBG_OUT("-----------------------> %s CleanUpFromLibUvContext tim %p c %d udp %p c %d tcp %p c %d State %d\n", mBofSocketChannelParam_X.Name_S.c_str(), mpsTimer.get(), mpsTimer.use_count(), mpsUdpHandle.get(), mpsUdpHandle.use_count(),
  //  mpsTcpHandle.get(), mpsTcpHandle.use_count(), ChannelResourceReleaseState());
  // if (Rts_E == BOFERR_NO_ERROR)
  {
    if (mpsTimer)
    {
      mpsTimer->stop();
      mpsTimer->close();
    }
    if (mpsUdpHandle)
    {
      mpsUdpHandle->stop();
      mpsUdpHandle->close();
    }
    if (mpsTcpHandle)
    {
      mpsTcpHandle->stop();
      mpsTcpHandle->close();
    }
    Rts_E = BOFERR_NO_ERROR;
  }
  return Rts_E;
}

BOFERR BofSocketChannel::WaitForEndOfClose(uint32_t _TimeoutInMs_U32, uint32_t PollingTimeInMs_U32)
{
  CHANNEL_RESOURCE_RELEASE_STATE ResourceReleaseState_E;
  BOFERR Rts_E = BOFERR_NO_ERROR;
  uint32_t Start_U32, Delta_U32;

  ResourceReleaseState_E = ChannelResourceReleaseState();
  printf("!!!!!start!!!!!WaitForEndOfClose!!!!!!!!!!!!!!!!!!!!!! 2: state %d on %p\n", static_cast<uint32_t>(ResourceReleaseState_E), static_cast<void *>(this));
  if (ResourceReleaseState_E != CHANNEL_RESOURCE_RELEASE_STATE::CLOSED)
  {
    Start_U32 = Bof_GetMsTickCount();
    while (ResourceReleaseState_E != CHANNEL_RESOURCE_RELEASE_STATE::CLOSED)
    {
      Bof_Sleep(PollingTimeInMs_U32);
      Delta_U32 = Bof_ElapsedMsTime(Start_U32);
      if (Delta_U32 >= _TimeoutInMs_U32)
      {
        Rts_E = BOFERR_TIMEOUT;
        break;
      }
      ResourceReleaseState_E = ChannelResourceReleaseState();
    }
  }
  printf("!!!!!end!!!!!WaitForEndOfClose!!!!!!!!!!!!!!!!!!!!!! 2: state %d on %p\n", static_cast<uint32_t>(ResourceReleaseState_E), static_cast<void *>(this));

  return Rts_E;
}

BOFERR BofSocketChannel::InitErrorCode() const
{
  return mErrorCode_E;
}

BOFERR BofSocketChannel::LockBofSocketChannel(const std::string & /*_rLockerName_S*/)
{
  BOFERR Rts_E = Bof_LockMutex(mBofSocketChannelMutex);
  return Rts_E;
}

BOFERR BofSocketChannel::UnlockBofSocketChannel()
{
  return Bof_UnlockMutex(mBofSocketChannelMutex);
}

/*
const std::vector<std::shared_ptr<BofSocketChannel>> &BofSocketChannel::BofSocketAcceptedChannelCollection()
{
  return mBofSocketAcceptedChannelCollection;
}
*/
const BOF_SOCKET_CHANNEL_PARAM &BofSocketChannel::BofSocketChannelParam() const
{
  return mBofSocketChannelParam_X;
}

const BOF_SOCKET_CHANNEL_STATISTIC &BofSocketChannel::BofSocketChannelStatistic() const
{
  return mBofSocketChannelStatistic_X;
}

const BOF_SOCKET_CHANNEL_STATE &BofSocketChannel::BofSocketChannelState() const
{
  return mBofSocketChannelState_X;
}

bool BofSocketChannel::IsConnected() const
{
  return (mBofSocketChannelState_X.State_E >= BOF_SOCKET_STATE::CONNECTED);
}

bool BofSocketChannel::IsAnAcceptedClient() const
{
  return (mBofSocketChannelState_X.pAcceptedByThisListener != nullptr);
}

bool BofSocketChannel::IsAListener() const
{
  return (mBofSocketChannelParam_X.ListenBacklog_U32 != 0);
}

CHANNEL_RESOURCE_RELEASE_STATE BofSocketChannel::ChannelResourceReleaseState() const
{
  return mChannelResourceReleaseState_E;
}

void BofSocketChannel::ChannelResourceReleaseState(CHANNEL_RESOURCE_RELEASE_STATE _ReleaseState_E)
{
  printf("===========> %p goes from %d to %d\n", static_cast<void *>(this), static_cast<uint32_t>(mChannelResourceReleaseState_E), static_cast<uint32_t>(_ReleaseState_E));
  mChannelResourceReleaseState_E = _ReleaseState_E;
}

const BOF_SOCKET_ADDRESS &BofSocketChannel::LocalAddress() const
{
  return mInterfaceBofSocketAddress_X;
}

const BOF_SOCKET_ADDRESS &BofSocketChannel::PeerAddress() const
{
  return mBofSocketAddress_X;
}

uint32_t BofSocketChannel::GetAndIncrementNextAvailablePort()
{
  uint32_t Rts_U32 = 0;

  BOF_ASSERT(IsAListener());
  if (IsAListener())
  {
    if (mNextAvailablePort_U32)
    {
      if (mNextAvailablePort_U32 < mMaxPort_U32)
      {
        Rts_U32 = ++mNextAvailablePort_U32;
      }
      else
      {
        Rts_U32 = mMinPort_U32;
        mNextAvailablePort_U32 = Rts_U32 + 1;
      }
    }
  }
  return Rts_U32;
}

void BofSocketChannel::ResetStat()
{
  mBofSocketChannelStatistic_X.Reset();
}

BOFERR BofSocketChannel::ChannelId(uint32_t _ChannelId_U32)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  mBofSocketChannelParam_X.ChannelId_U32 = _ChannelId_U32;
  return Rts_E;
}

BOFERR BofSocketChannel::Delimiter(uint8_t _Delimiter_U8)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  mBofSocketChannelParam_X.Delimiter_U8 = _Delimiter_U8;
  return Rts_E;
}

BOFERR BofSocketChannel::Name(const std::string &_rName_S)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  mBofSocketChannelParam_X.Name_S = _rName_S;
  return Rts_E;
}

BOFERR BofSocketChannel::NotifyType(BOF_SOCKET_NOTIFY_TYPE _NotifyType_E)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  mBofSocketChannelParam_X.NotifyType_E = _NotifyType_E;
  return Rts_E;
}

//!! do not change this without any exclusion pattern
BOFERR BofSocketChannel::NotifyRcvBufferSize(uint32_t _NotifyRcvBufferSize_U32, void *_pData)
{
  BOFERR Rts_E = BOFERR_NOT_ENOUGH_RESOURCE;

  if (!mDataPreAllocated_B)
  {
    BOF_SAFE_DELETE_ARRAY(mpDataBuffer_U8);
  }

  if (_pData)
  {
    BOF_ASSERT(_NotifyRcvBufferSize_U32);
    mDataPreAllocated_B = true;
    mpDataBuffer_U8 = reinterpret_cast<uint8_t *>(_pData);
    if (mpDataBuffer_U8)
    {
      Rts_E = BOFERR_NO_ERROR;
    }
  }
  else
  {
    mDataPreAllocated_B = false;
    if (_NotifyRcvBufferSize_U32) // For listener this value can be 0
    {
      mpDataBuffer_U8 = new uint8_t[_NotifyRcvBufferSize_U32];
      if (mpDataBuffer_U8)
      {
        Rts_E = BOFERR_NO_ERROR;
      }
    }
    else
    {
      Rts_E = BOFERR_NO_ERROR;
    }
  }
  if (Rts_E == BOFERR_NO_ERROR)
  {
    mWriteIndex_U32 = 0;
    mDelimiterStartIndex_U32 = 0;
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnSocketReadCallback(ON_SOCKET_READ_CALLBACK _OnSocketReadCallback)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  mBofSocketChannelParam_X.OnSocketReadCallback = _OnSocketReadCallback;
  return Rts_E;
}

BOFERR BofSocketChannel::RcvBufferSize(uint32_t _RcvBufferSize_U32)
{
  BOFERR Rts_E = mIsUdp_B ? SetSocketBufferSizeOption(mpsUdpHandle, _RcvBufferSize_U32, 0) : SetSocketBufferSizeOption(mpsTcpHandle, _RcvBufferSize_U32, 0);
  if (Rts_E == BOFERR_NO_ERROR)
  {
    mBofSocketChannelParam_X.RcvBufferSize_U32 = _RcvBufferSize_U32;
  }
  return Rts_E;
}

BOFERR BofSocketChannel::SndBufferSize(uint32_t _SndBufferSize_U32)
{
  BOFERR Rts_E = mIsUdp_B ? SetSocketBufferSizeOption(mpsUdpHandle, 0, _SndBufferSize_U32) : SetSocketBufferSizeOption(mpsTcpHandle, 0, _SndBufferSize_U32);
  if (Rts_E == BOFERR_NO_ERROR)
  {
    mBofSocketChannelParam_X.SndBufferSize_U32 = _SndBufferSize_U32;
  }
  return Rts_E;
}

BOFERR BofSocketChannel::GetBuffer(BOF_BUFFER &_rBuffer_X)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  _rBuffer_X.SizeInByte_U64 = mBofSocketChannelParam_X.NotifyRcvBufferSize_U32 - mWriteIndex_U32;
  _rBuffer_X.pData_U8 = &mpDataBuffer_U8[mWriteIndex_U32];
  _rBuffer_X.MustBeDeleted_B = false;
  return Rts_E;
}

BOFERR BofSocketChannel::OnReadyRead(uint32_t _NbNewByte_U32, bool _Partial_B, const BOF_SOCKET_PEER_INFO &_rPeerInfo_X)
{
  BOFERR Rts_E = BOFERR_NO_ERROR, Sts_E;
  uint32_t Nb_U32, NbRemainingByte_U32, StartIndex_U32, LastIndex_U32, i_U32, NbByteInDelimitedPacket_U32, NbByteToMove_U32;

  if (_NbNewByte_U32)
  {
    NbRemainingByte_U32 = mBofSocketChannelParam_X.NotifyRcvBufferSize_U32 - mWriteIndex_U32;
    Nb_U32 = (_NbNewByte_U32 <= NbRemainingByte_U32) ? _NbNewByte_U32 : NbRemainingByte_U32;

    if (Nb_U32)
    {
      //       memcpy(&mpDataBuffer_U8[mWriteIndex_U32], _rBuffer_X.pData_U8, Nb_U32);
      StartIndex_U32 = mWriteIndex_U32;
      LastIndex_U32 = StartIndex_U32 + Nb_U32;
      mWriteIndex_U32 += Nb_U32;

      if (mBofSocketChannelParam_X.NotifyType_E == BOF_SOCKET_NOTIFY_TYPE::NEVER)
      {
        mWriteIndex_U32 = 0;
        mDelimiterStartIndex_U32 = 0;
      }
      else if (mBofSocketChannelParam_X.NotifyType_E == BOF_SOCKET_NOTIFY_TYPE::ASAP)
      {
        Sts_E = OnDataRead(Nb_U32, mpDataBuffer_U8, _Partial_B, _rPeerInfo_X);
        if (Sts_E == BOFERR_NO_ERROR)
        {
          mBofSocketChannelStatistic_X.NbOpRead_U32++;
          mBofSocketChannelStatistic_X.NbDataByteRead_U64 += Nb_U32;
        }
        else
        {
          mBofSocketChannelStatistic_X.NbOpReadError_U32++;
        }
        mWriteIndex_U32 = 0;
        mDelimiterStartIndex_U32 = 0;
      }
      else if (mBofSocketChannelParam_X.NotifyType_E == BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL)
      {
        if (mWriteIndex_U32 == mBofSocketChannelParam_X.NotifyRcvBufferSize_U32)
        {
          Sts_E = OnDataRead(mBofSocketChannelParam_X.NotifyRcvBufferSize_U32, mpDataBuffer_U8, _Partial_B, _rPeerInfo_X);
          if (Sts_E == BOFERR_NO_ERROR)
          {
            mBofSocketChannelStatistic_X.NbOpRead_U32++;
            mBofSocketChannelStatistic_X.NbDataByteRead_U64 += mBofSocketChannelParam_X.NotifyRcvBufferSize_U32;
          }
          else
          {
            mBofSocketChannelStatistic_X.NbOpReadError_U32++;
          }
          mWriteIndex_U32 = 0;
          mDelimiterStartIndex_U32 = 0;
        }
      }
      else if (mBofSocketChannelParam_X.NotifyType_E == BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND)
      {
        // We look for Delimiter_U8 in mpDataBuffer_U8 data chunk per data chunk
        BOF_ASSERT(mWriteIndex_U32 <= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);
        BOF_ASSERT(mDelimiterStartIndex_U32 < mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);
        BOF_ASSERT(LastIndex_U32 <= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);

        do
        {
          for (i_U32 = StartIndex_U32; i_U32 < LastIndex_U32; i_U32++)
          {
            if (mpDataBuffer_U8[i_U32] == mBofSocketChannelParam_X.Delimiter_U8)
            {
              NbByteInDelimitedPacket_U32 = i_U32 - mDelimiterStartIndex_U32 + 1;
              BOF_ASSERT(NbByteInDelimitedPacket_U32 <= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);

              Sts_E = OnDataRead(NbByteInDelimitedPacket_U32, &mpDataBuffer_U8[mDelimiterStartIndex_U32], _Partial_B, _rPeerInfo_X);
              if (Sts_E == BOFERR_NO_ERROR)
              {
                mBofSocketChannelStatistic_X.NbOpRead_U32++;
                mBofSocketChannelStatistic_X.NbDataByteRead_U64 += NbByteInDelimitedPacket_U32;
              }
              else
              {
                mBofSocketChannelStatistic_X.NbOpReadError_U32++;
              }
              StartIndex_U32 = i_U32 + 1;
              BOF_ASSERT(StartIndex_U32 <= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);

              mDelimiterStartIndex_U32 = i_U32 + 1;
              BOF_ASSERT(mDelimiterStartIndex_U32 <= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);
              break;
            }
          }
        } while (i_U32 < LastIndex_U32);

        if (mWriteIndex_U32 >= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32) // can happens if last byte of buffer is equal to Delimiter_U8 or no Delimiter_U8 byte found before the end
        {
          if (mDelimiterStartIndex_U32 == 0)
          {
            Sts_E = OnDataRead(mBofSocketChannelParam_X.NotifyRcvBufferSize_U32, mpDataBuffer_U8, _Partial_B, _rPeerInfo_X);
            if (Sts_E == BOFERR_NO_ERROR)
            {
              mBofSocketChannelStatistic_X.NbOpRead_U32++;
              mBofSocketChannelStatistic_X.NbDataByteRead_U64 += mBofSocketChannelParam_X.NotifyRcvBufferSize_U32;
            }
            else
            {
              mBofSocketChannelStatistic_X.NbOpReadError_U32++;
            }
            mWriteIndex_U32 = 0;
            mDelimiterStartIndex_U32 = 0;
          }
          else
          {
            NbByteToMove_U32 = mBofSocketChannelParam_X.NotifyRcvBufferSize_U32 - mDelimiterStartIndex_U32;

            BOF_ASSERT(NbByteToMove_U32 < mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);
            if (NbByteToMove_U32)
            {
              memcpy(mpDataBuffer_U8, &mpDataBuffer_U8[mDelimiterStartIndex_U32], NbByteToMove_U32);
            }
            mWriteIndex_U32 = NbByteToMove_U32;
            mDelimiterStartIndex_U32 = 0;
          }
        }
        BOF_ASSERT(mWriteIndex_U32 <= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);
        BOF_ASSERT(mDelimiterStartIndex_U32 <= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::SetupBofSocketChannel(const BOF_SOCKET_CHANNEL_STATE &_rInitialBofChannelState_X, const std::shared_ptr<uvw::TcpHandle> &_rpsAlreadyCreatedTcpHandle, const std::shared_ptr<uvw::UDPHandle> &_rpsAlreadyCreatedUdpHandle,
                                               const std::shared_ptr<BofSocketChannelManager> &_rpsBofSocketChannelManager, const BOF_SOCKET_CHANNEL_PARAM &_rBofSocketChannelParam_X)
{
  BOFERR Rts_E;
  BOF_TCP_SOCKET_PARAM BofTcpSocketParam_X;
  BOF_UDP_SOCKET_PARAM BofUdpSocketParam_X;
  std::string IpAddress_S;

  // int y = _rpsBofSocketChannelManager.use_count();
  // BofSocketChannelManager *p = _rpsBofSocketChannelManager.get();

  Rts_E = BOFERR_INVALID_PARAM;
  mpsBofSocketChannelManager = _rpsBofSocketChannelManager;
  mBofSocketChannelParam_X = _rBofSocketChannelParam_X;
  mBofSocketChannelState_X = _rInitialBofChannelState_X;
  Rts_E = Bof_CreateMutex(mBofSocketChannelParam_X.Name_S + "_mtx", false, false, mBofSocketChannelMutex);
  if (Rts_E == BOFERR_NO_ERROR)
  {
    Rts_E = NotifyRcvBufferSize(mBofSocketChannelParam_X.NotifyRcvBufferSize_U32, mBofSocketChannelParam_X.pData);
    if (Rts_E == BOFERR_NO_ERROR)
    {
      Rts_E = Bof_SplitUrlAddress(mBofSocketChannelParam_X.IpAddress_S, mInterfaceBofSocketAddress_X, mBofSocketAddress_X);
      if (Rts_E == BOFERR_NO_ERROR)
      {
        Rts_E = BOFERR_BAD_PROTOCOL;
        if ((mInterfaceBofSocketAddress_X.ProtocolType_E == mBofSocketAddress_X.ProtocolType_E) && (mInterfaceBofSocketAddress_X.SocketType_E == mBofSocketAddress_X.SocketType_E) && (mInterfaceBofSocketAddress_X.IpV6_B == mBofSocketAddress_X.IpV6_B))
        {
          if ((mInterfaceBofSocketAddress_X.ProtocolType_E != BOF_PROTOCOL_TYPE::BOF_PROTOCOL_SHARED_RAM) && (mInterfaceBofSocketAddress_X.ProtocolType_E != BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNDEF) &&
              (mInterfaceBofSocketAddress_X.ProtocolType_E != BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN) && (mInterfaceBofSocketAddress_X.ProtocolType_E != BOF_PROTOCOL_TYPE::BOF_PROTOCOL_ZMQ_PGM) &&
              (mInterfaceBofSocketAddress_X.ProtocolType_E != BOF_PROTOCOL_TYPE::BOF_PROTOCOL_MAX))
          {
            mIsUdp_B = (mInterfaceBofSocketAddress_X.SocketType_E == BOF_SOCK_TYPE::BOF_SOCK_UDP);
            Rts_E = BOFERR_NO_ERROR;
            if (mpsBofSocketChannelManager)
            {
              Rts_E = BOFERR_NOT_ENOUGH_RESOURCE;
              mpsTimer = mpsBofSocketChannelManager->TheLoop()->resource<uvw::TimerHandle>();
              if (mpsTimer)
              {
                mpsTimer->data(std::make_shared<UVW_CONTEXT>(mBofSocketChannelParam_X.Name_S + "_mpsTimer", this));
                mpsTimer->on<uvw::TimerEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnTimerEvent));
                mpsTimer->on<uvw::ErrorEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnTimerError));
                //        mpsTimer->again();  //To check error lambda
                mpsTimer->start(uvw::TimerHandle::Time{0}, uvw::TimerHandle::Time{TIMER_GRANULARITY_IN_MS}); // use std::chrono::duration<uint64_t, std::milli>
                Rts_E = BOFERR_NO_ERROR;
              }
            }
            if (Rts_E == BOFERR_NO_ERROR)
            {
              if (mIsUdp_B)
              {
                if (mpsBofSocketChannelManager)
                {
                  Rts_E = BOFERR_NOT_AVAILABLE;
                  if (!IsAListener())
                  {
                    Rts_E = BOFERR_NOT_ENOUGH_RESOURCE;
                    mpsUdpHandle = (_rpsAlreadyCreatedUdpHandle != nullptr) ? _rpsAlreadyCreatedUdpHandle : mpsBofSocketChannelManager->TheLoop()->resource<uvw::UDPHandle>();
                    if (mpsUdpHandle)
                    {
                      if (_rpsAlreadyCreatedUdpHandle == nullptr)
                      {
                        IpAddress_S = Bof_IpAddressToString(mInterfaceBofSocketAddress_X, false, false);
                        mpsUdpHandle->bind(IpAddress_S, mInterfaceBofSocketAddress_X.Port());
                      }
                      BofUdpSocketParam_X.MulticastReceiver_B = false;
                      BofUdpSocketParam_X.MulticastLoop_B = false;
                      BofUdpSocketParam_X.Broadcast_B = false;
                      BofUdpSocketParam_X.Ttl_U32 = 32;
                      BofUdpSocketParam_X.RcvBufferSize_U32 = mBofSocketChannelParam_X.RcvBufferSize_U32;
                      BofUdpSocketParam_X.SndBufferSize_U32 = mBofSocketChannelParam_X.SndBufferSize_U32;
                      BofUdpSocketParam_X.OnSocketReadCallback = mBofSocketChannelParam_X.OnSocketReadCallback;
                      Rts_E = BOFERR_NO_ERROR; // Must be clear for SetSocketChannelOption call
                      Rts_E = SetSocketChannelOption(mpsUdpHandle, BofUdpSocketParam_X);

                      if (Rts_E == BOFERR_NO_ERROR)
                      {
                        mpsUdpHandle->data(std::make_shared<UVW_CONTEXT>(mBofSocketChannelParam_X.Name_S + "_mpsUdpHandle", this));
                        mpsUdpHandle->on<uvw::UDPDataEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnUdpDataEvent));
                        mpsUdpHandle->on<uvw::UDPDataPreAllocatedEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnUdpDataPreAllocatedEvent)); // BHA
                        mpsUdpHandle->on<uvw::SendEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnUdpSendEvent));
                        mpsUdpHandle->on<uvw::ErrorEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnUdpError));
                        mpsUdpHandle->on<uvw::CloseEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnUdpCloseEvent));
                        if (_rpsAlreadyCreatedUdpHandle != nullptr)
                        {
                          mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::CONNECTED;
                        }
                      }
                    }
                  }
                }
              }
              else
              {
                if (mpsBofSocketChannelManager)
                {
                  Rts_E = BOFERR_NOT_ENOUGH_RESOURCE;
                  mpsTcpHandle = (_rpsAlreadyCreatedTcpHandle != nullptr) ? _rpsAlreadyCreatedTcpHandle : mpsBofSocketChannelManager->TheLoop()->resource<uvw::TcpHandle>();
                  if (mpsTcpHandle)
                  {
                    if (_rpsAlreadyCreatedTcpHandle == nullptr)
                    {
                      IpAddress_S = Bof_IpAddressToString(mInterfaceBofSocketAddress_X, false, false);
                      mpsTcpHandle->bind(IpAddress_S, mInterfaceBofSocketAddress_X.Port());
                    }
                    BofTcpSocketParam_X.NoDelay_B = true;
                    BofTcpSocketParam_X.SimultaneousAccept_B = IsAListener();
                    BofTcpSocketParam_X.KeepAliveTimeInMs_U32 = 0; // 0 means not used
                    BofTcpSocketParam_X.RcvBufferSize_U32 = mBofSocketChannelParam_X.RcvBufferSize_U32;
                    BofTcpSocketParam_X.SndBufferSize_U32 = mBofSocketChannelParam_X.SndBufferSize_U32;
                    BofTcpSocketParam_X.OnSocketConnectCallback = IsAListener() ? nullptr : mBofSocketChannelParam_X.OnSocketConnectCallback;
                    BofTcpSocketParam_X.OnSocketAcceptCallback = IsAListener() ? nullptr : mBofSocketChannelParam_X.OnSocketAcceptCallback;
                    BofTcpSocketParam_X.OnSocketReadCallback = IsAListener() ? nullptr : mBofSocketChannelParam_X.OnSocketReadCallback;
                    Rts_E = BOFERR_NO_ERROR; // Must be clear for SetSocketChannelOption call
                    Rts_E = SetSocketChannelOption(mpsTcpHandle, BofTcpSocketParam_X);
                    if (Rts_E == BOFERR_NO_ERROR)
                    {
                      mpsTcpHandle->on<uvw::CloseEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnTcpCloseEvent));
                      mpsTcpHandle->on<uvw::ErrorEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnTcpError));
                      if (!IsAListener())
                      {
                        mpsTcpHandle->data(std::make_shared<UVW_CONTEXT>(mBofSocketChannelParam_X.Name_S + "_mpsTcpHandle", this));
                        mpsTcpHandle->on<uvw::DataEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnTcpDataEvent));
                        mpsTcpHandle->on<uvw::DataPreAllocatedEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnTcpDataPreallocatedEvent)); // BHA
                        mpsTcpHandle->on<uvw::WriteEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnTcpWriteEvent));
                        mpsTcpHandle->on<uvw::EndEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnTcpEndEvent));
                        mpsTcpHandle->on<uvw::ShutdownEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnTcpShutdownEvent));
                        mpsTcpHandle->on<uvw::ConnectEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnTcpConnect));
                        if (_rpsAlreadyCreatedTcpHandle != nullptr)
                        {
                          mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::CONNECTED;
                        }
                      }
                      else
                      {
                        mpsTcpHandle->on<uvw::ListenEvent>(BOF_BIND_2_ARG_TO_METHOD(BofSocketChannel::OnTcpListenEvent));
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::AddAcceptedChannel(const std::shared_ptr<BofSocketChannel> &_rpsBofSocketChannel)
{
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    Rts_E = LockBofSocketChannel("AddAcceptedChannel");
    if (Rts_E == BOFERR_NO_ERROR)
    {
      Rts_E = BOFERR_NO_MORE_HANDLE;
      if (mBofSocketAcceptedChannelCollection.size() < mBofSocketChannelParam_X.ListenBacklog_U32)
      {
        mBofSocketAcceptedChannelCollection.push_back(_rpsBofSocketChannel);
        Rts_E = BOFERR_NO_ERROR;
      }
      UnlockBofSocketChannel();
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::RemoveAcceptedChannel(const std::string & /*_rCaller_S*/, const std::shared_ptr<BofSocketChannel> &_rpsBofSocketChannel)
{
  std::shared_ptr<BofSocketChannel> psBofSocketChannel;
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    Rts_E = LockBofSocketChannel("RemoveAcceptedChannel");
    if (Rts_E == BOFERR_NO_ERROR)
    {
      Rts_E = BOFERR_NOT_FOUND;
      auto It = std::find(mBofSocketAcceptedChannelCollection.begin(), mBofSocketAcceptedChannelCollection.end(), _rpsBofSocketChannel);
      if (It != mBofSocketAcceptedChannelCollection.end())
      {
        Rts_E = mpsBofSocketChannelManager->RemoveChannel("RemoveAcceptedChannel", *It);
        //        DBG_OUT("----> RemoveAcceptedChannel %s by %s -> %s\n",(*It)->BofSocketChannelParam().Name_S.c_str(), _rCaller_S.c_str(), BOF_NAMESPACE::Bof_ErrorCode(Rts_E));
        mBofSocketAcceptedChannelCollection.erase(It);
      }
      UnlockBofSocketChannel();
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::CloseListenerChannelAndItsAcceptedChannel()
{
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    BOF_ASSERT(IsAListener());
    Rts_E = BOFERR_NOT_AVAILABLE;
    if (IsAListener())
    {
      Rts_E = LockBofSocketChannel("CloseAllAcceptedClientOfThisChannel");
      if (Rts_E == BOFERR_NO_ERROR)
      {
        // Remove all accepted channel
        while (mBofSocketAcceptedChannelCollection.size())
        {
          RemoveAcceptedChannel("CloseAllAcceptedClientOfThisChannel", mBofSocketAcceptedChannelCollection[0]);
        }
        // Remove Listener
        mpsBofSocketChannelManager->RemoveChannel("CloseListenerChannelAndItsAcceptedChannel", shared_from_this());
        UnlockBofSocketChannel();
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnDataRead(uint32_t _Nb_U32, uint8_t *_pBuffer_U8, bool _Partial_B, const BOF_SOCKET_PEER_INFO &_rPeerInfo_X)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;

  if (mBofSocketChannelParam_X.OnSocketReadCallback)
  {
    BOF_BUFFER Buffer_X(nullptr, false, _Partial_B, _Nb_U32, _pBuffer_U8);
    Rts_E = mBofSocketChannelParam_X.OnSocketReadCallback(mBofSocketChannelParam_X.ChannelId_U32, shared_from_this(), Buffer_X, _rPeerInfo_X);
  }
  return Rts_E;
}

BOFERR BofSocketChannel::WriteData(uint32_t _TimeoutInMs_U32, bool _MakeACopy_B, const std::string &_rData_S)
{
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    Rts_E = WriteData(_TimeoutInMs_U32, _MakeACopy_B, static_cast<uint32_t>(_rData_S.size()), reinterpret_cast<const uint8_t *>(_rData_S.c_str()));
  }
  return Rts_E;
}

BOFERR BofSocketChannel::WriteData(uint32_t _TimeoutInMs_U32, bool _MakeACopy_B, uint32_t _Size_U32, const uint8_t *_pData_U8)
{
  BOFERR Rts_E = mErrorCode_E;
  std::vector<BOF_BUFFER> BufferCollection;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    Rts_E = mpsBofSocketChannelManager->Write(_TimeoutInMs_U32, nullptr, shared_from_this(), _MakeACopy_B, _Size_U32, _pData_U8, nullptr);
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnUdpDataEvent(const uvw::UDPDataEvent &_rEvent, uvw::UDPHandle &_rHandle)
{
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    mBofSocketChannelStatistic_X.NbDataEvent_U32++;
    if (mBofSocketChannelParam_X.OnSocketReadCallback)
    {
      BOF_BUFFER Buffer_X(nullptr, false, _rEvent.partial, _rEvent.length, const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(_rEvent.data.get())));
      BOF_SOCKET_PEER_INFO PeerInfo_X;
      PeerInfo_X.IpAddress_S = _rEvent.sender.ip;
      PeerInfo_X.Port_U16 = static_cast<uint16_t>(_rEvent.sender.port);
      mBofSocketChannelParam_X.OnSocketReadCallback(mBofSocketChannelParam_X.ChannelId_U32, shared_from_this(), Buffer_X, PeerInfo_X);
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnUdpDataPreAllocatedEvent(const uvw::UDPDataPreAllocatedEvent &_rEvent, uvw::UDPHandle &_rHandle)
{
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    mBofSocketChannelStatistic_X.NbDataEvent_U32++;
    BOF_ASSERT(reinterpret_cast<const uint8_t *>(_rEvent.data) >= mpDataBuffer_U8);
    BOF_ASSERT(reinterpret_cast<const uint8_t *>(_rEvent.data) < mpDataBuffer_U8 + mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);
    BOF_SOCKET_PEER_INFO PeerInfo_X;
    PeerInfo_X.IpAddress_S = _rEvent.sender.ip;
    PeerInfo_X.Port_U16 = static_cast<uint16_t>(_rEvent.sender.port);
    OnReadyRead(static_cast<uint32_t>(_rEvent.length), _rEvent.partial, PeerInfo_X);
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnUdpSendEvent(const uvw::SendEvent & /*_rEvent*/, uvw::UDPHandle &_rHandle)
{
  bool NotifyEvent_B;
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    NotifyEvent_B = true;
    mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::IO;

    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    mBofSocketChannelStatistic_X.NbWriteEvent_U32++;

    if (NotifyEvent_B)
    {
      if (mBofSocketChannelState_X.pNotifyEvent_X)
      {
        Bof_SignalEvent(*mBofSocketChannelState_X.pNotifyEvent_X, 0);
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnUdpError(const uvw::ErrorEvent & /*_rEvent*/, uvw::UDPHandle &_rHandle)
{
  bool NotifyEvent_B;
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    NotifyEvent_B = true;

    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    mBofSocketChannelStatistic_X.NbError_U32++;

    mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::IO_ERROR;
    //    DBG_OUT("----> UdpError %d State %d\n", mBofSocketChannelStatistic_X.NbError_U32, mBofSocketChannelState_X.State_E);
    if (NotifyEvent_B)
    {
      if (mBofSocketChannelState_X.pNotifyEvent_X)
      {
        Bof_SignalEvent(*mBofSocketChannelState_X.pNotifyEvent_X, 0);
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnUdpCloseEvent(const uvw::CloseEvent & /*_rEvent*/, uvw::UDPHandle &_rHandle)
{
  bool NotifyEvent_B;
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    NotifyEvent_B = false;
    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    mBofSocketChannelStatistic_X.NbCloseEvent_U32++;
    switch (mBofSocketChannelState_X.State_E)
    {
      case BOF_SOCKET_STATE::DISCONNECTING:
        mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::DISCONNECTED;
        NotifyEvent_B = true;
        break;

      default:
        BOF_ASSERT(1);
        break;
    }
    if (NotifyEvent_B)
    {
      if (mBofSocketChannelState_X.pNotifyEvent_X)
      {
        Bof_SignalEvent(*mBofSocketChannelState_X.pNotifyEvent_X, 0);
      }
    }
  }
  ChannelResourceReleaseState(CHANNEL_RESOURCE_RELEASE_STATE::CLOSED);
  return Rts_E;
}

BOFERR BofSocketChannel::OnTcpDataEvent(const uvw::DataEvent &_rEvent, uvw::TcpHandle &_rHandle)
{
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    mBofSocketChannelStatistic_X.NbDataEvent_U32++;
    if (mBofSocketChannelParam_X.OnSocketReadCallback)
    {
      BOF_BUFFER Buffer_X(nullptr, false, false, _rEvent.length, reinterpret_cast<uint8_t *>(_rEvent.data.get()));
      BOF_SOCKET_PEER_INFO PeerInfo_X;
      mBofSocketChannelParam_X.OnSocketReadCallback(mBofSocketChannelParam_X.ChannelId_U32, shared_from_this(), Buffer_X, PeerInfo_X);
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnTcpDataPreallocatedEvent(const uvw::DataPreAllocatedEvent &_rEvent, uvw::TcpHandle &_rHandle)
{
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    mBofSocketChannelStatistic_X.NbDataEvent_U32++;
    BOF_ASSERT(reinterpret_cast<uint8_t *>(_rEvent.data) >= mpDataBuffer_U8);
    BOF_ASSERT(reinterpret_cast<uint8_t *>(_rEvent.data) < mpDataBuffer_U8 + mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);
    BOF_SOCKET_PEER_INFO PeerInfo_X;
    OnReadyRead(static_cast<uint32_t>(_rEvent.length), false, PeerInfo_X);
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnTcpWriteEvent(const uvw::WriteEvent & /*_rEvent*/, uvw::TcpHandle &_rHandle)
{
  bool NotifyEvent_B;
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    NotifyEvent_B = true;
    mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::IO;

    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    mBofSocketChannelStatistic_X.NbWriteEvent_U32++;

    if (NotifyEvent_B)
    {
      if (mBofSocketChannelState_X.pNotifyEvent_X)
      {
        Bof_SignalEvent(*mBofSocketChannelState_X.pNotifyEvent_X, 0);
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnTcpCloseEvent(const uvw::CloseEvent & /*_rEvent*/, uvw::TcpHandle &_rHandle)
{
  bool NotifyEvent_B;
  BOF_SOCKET_PEER_INFO PeerInfo_X;
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    NotifyEvent_B = false;
    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    //    DBG_OUT("Close %s\n", psUvwContext_X->Name_S.c_str());
    mBofSocketChannelStatistic_X.NbCloseEvent_U32++;
    switch (mBofSocketChannelState_X.State_E)
    {
      case BOF_SOCKET_STATE::DISCONNECTING:
        mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::DISCONNECTED;
        NotifyEvent_B = true;
        break;

      default:
        BOF_ASSERT(1);
        break;
    }

    DBG_OUT("---------OnTcpCloseEvent--------------> %s OnTcpCloseEvent ResState %d->CHANNEL_RESOURCE_RELEASE_STATE::CLOSED this %p\n", mBofSocketChannelParam_X.Name_S.c_str(), static_cast<uint32_t>(ChannelResourceReleaseState()),
            static_cast<void *>(this));
    ChannelResourceReleaseState(CHANNEL_RESOURCE_RELEASE_STATE::CLOSED); // Before notify !

    if (NotifyEvent_B)
    {
      NotifyEvent(nullptr, nullptr, nullptr);
    }
    if (IsAnAcceptedClient())
    {
      mBofSocketChannelState_X.pAcceptedByThisListener->RemoveAcceptedChannel("OnTcpCloseEvent", shared_from_this());
    }
  }

  return Rts_E;
}

BOFERR BofSocketChannel::OnTcpError(const uvw::ErrorEvent &_rEvent, uvw::TcpHandle &_rHandle)
{
  bool NotifyEvent_B;
  BOF_SOCKET_PEER_INFO PeerInfo_X;
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    NotifyEvent_B = false;
    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    mBofSocketChannelStatistic_X.NbError_U32++;
    DBG_OUT("----> T=%d TcpError %d State %d ResState %d chnl %p code %d -> name %s what %s\n", Bof_GetMsTickCount(), mBofSocketChannelStatistic_X.NbError_U32, static_cast<uint32_t>(mBofSocketChannelState_X.State_E),
            static_cast<uint32_t>(mChannelResourceReleaseState_E), static_cast<void *>(this), _rEvent.code(), _rEvent.name(), _rEvent.what());
    switch (mBofSocketChannelState_X.State_E)
    {
      case BOF_SOCKET_STATE::CONNECTING:
        mBofSocketChannelStatistic_X.NbOpConnectError_U32++;
        mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::CONNECT_ERROR;
        NotifyEvent_B = true;
        break;

      case BOF_SOCKET_STATE::DISCONNECTING:
        mBofSocketChannelStatistic_X.NbOpDisconnectError_U32++;
        mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::DISCONNECT_ERROR;
        NotifyEvent_B = true;
        break;

      case BOF_SOCKET_STATE::IO:
        mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::IO_ERROR;
        NotifyEvent_B = true;
        break;

      default:
        break;
    }
    if (NotifyEvent_B)
    {
      NotifyEvent(nullptr, nullptr, nullptr);
    }
    if (IsAnAcceptedClient())
    {
      mBofSocketChannelState_X.pAcceptedByThisListener->RemoveAcceptedChannel("OnTcpError", shared_from_this());
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnTcpEndEvent(const uvw::EndEvent & /*_rEvent*/, uvw::TcpHandle &_rHandle)
{
  bool NotifyEvent_B;
  BOF_SOCKET_PEER_INFO PeerInfo_X;
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    mBofSocketChannelStatistic_X.NbTcpEndEvent_U32++;
    mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::END;
    NotifyEvent_B = true;

    if (NotifyEvent_B)
    {
      NotifyEvent(nullptr, nullptr, nullptr);
    }

    if (IsAnAcceptedClient())
    {
      mBofSocketChannelState_X.pAcceptedByThisListener->RemoveAcceptedChannel("OnTcpEndEvent", shared_from_this());
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnTcpShutdownEvent(const uvw::ShutdownEvent & /*_rEvent*/, uvw::TcpHandle &_rHandle)
{
  bool NotifyEvent_B;
  BOF_SOCKET_PEER_INFO PeerInfo_X;
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    mBofSocketChannelStatistic_X.NbTcpShutdownEvent_U32++;
    mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::SHUTDOWN;
    NotifyEvent_B = true;

    if (NotifyEvent_B)
    {
      NotifyEvent(nullptr, nullptr, nullptr);
    }
    if (IsAnAcceptedClient())
    {
      mBofSocketChannelState_X.pAcceptedByThisListener->RemoveAcceptedChannel("OnTcpShutdownEvent", shared_from_this());
    }
  }
  return Rts_E;
}

template <typename T> void AllocCallback(uv_handle_t *_pPrivateUvHandle, std::size_t /*_SuggestedSize*/, uv_buf_t *_pUvBuffer)
{
  BOF_BUFFER Buffer_X;
  BOFERR Sts_E;

  BOF_ASSERT(_pPrivateUvHandle != nullptr);
  BOF_ASSERT(_pUvBuffer != nullptr);
  if ((_pPrivateUvHandle) && (_pUvBuffer))
  {
    T *pStream = (static_cast<T *>(_pPrivateUvHandle->data));
    if (pStream)
    {
      std::shared_ptr<UVW_CONTEXT> psUvwContext_X = pStream->template data<UVW_CONTEXT>();
      if (psUvwContext_X)
      {
        BofSocketChannel *pBofSocketChannel = reinterpret_cast<BofSocketChannel *>(psUvwContext_X->pUserArg);
        if (pBofSocketChannel)
        {
          Sts_E = pBofSocketChannel->GetBuffer(Buffer_X);
          if (Sts_E == BOFERR_NO_ERROR)
          {
            _pUvBuffer->base = reinterpret_cast<char *>(Buffer_X.pData_U8);
            _pUvBuffer->len = static_cast<uint32_t>(Buffer_X.SizeInByte_U64);
          }
        }
      }
    }
  }
}

BOFERR BofSocketChannel::OnTcpListenEvent(const uvw::ListenEvent & /*_rEvent*/, uvw::TcpHandle &_rHandle)
{
  BOFERR Rts_E = mErrorCode_E; //,Sts_E;
  BOF_SOCKET_CHANNEL_PARAM BofSocketChannelParam_X, BofSocketChannelParamFromAcceptedClient_X;
  BOF_SOCKET_CHANNEL_STATE BofChannelState_X;
  uvw::Addr LocalIp;
  uvw::Addr RemoteIp;
  std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> psBofSocketChannel;
  BOF_SOCKET_PEER_INFO PeerInfo_X;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    BOF_ASSERT(IsAListener());
    Rts_E = BOFERR_NOT_AVAILABLE;
    if (IsAListener())
    {
      Rts_E = BOFERR_NOT_RUNNING;
      if (mBofSocketChannelState_X.State_E == BOF_SOCKET_STATE::LISTENING) // Can be LISTENING_TIMEOUT for example
      {
        std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
        mBofSocketChannelStatistic_X.NbTcpListenEvent_U32++;
        Rts_E = LockBofSocketChannel("OnTcpListenEvent");
        if (Rts_E == BOFERR_NO_ERROR)
        {
          Rts_E = BOFERR_NO_MORE_CONNECTION;
          if (mBofSocketAcceptedChannelCollection.size() < mBofSocketChannelParam_X.ListenBacklog_U32)
          {
            // TODO check nbclient max->Keep
            std::shared_ptr<uvw::TcpHandle> ClientChannel = mpsBofSocketChannelManager->TheLoop()->resource<uvw::TcpHandle>();
            _rHandle.accept(*ClientChannel);
            LocalIp = _rHandle.sock();                                                                                 // std::cout << "local: " << local.ip << " " << local.port << std::endl;
            RemoteIp = ClientChannel->peer();                                                                          // std::cout << "remote: " << remote.ip << " " << remote.port << std::endl;
            BofSocketChannelParam_X.ChannelId_U32 = static_cast<uint32_t>(mBofSocketAcceptedChannelCollection.size()); // TODO a changer
            BofSocketChannelParam_X.Delimiter_U8 = mBofSocketChannelParam_X.Delimiter_U8;
            BofSocketChannelParam_X.IpAddress_S = "tcp://" + LocalIp.ip + ":" + std::to_string(LocalIp.port) + ";" + RemoteIp.ip + ":" + std::to_string(RemoteIp.port);
            BofSocketChannelParam_X.ListenBacklog_U32 = 0;
            BofSocketChannelParam_X.Name_S = mBofSocketChannelParam_X.Name_S + "_Client_" + std::to_string(BofSocketChannelParam_X.ChannelId_U32);
            BofSocketChannelParam_X.NotifyRcvBufferSize_U32 = mBofSocketChannelParam_X.NotifyRcvBufferSize_U32;
            BofSocketChannelParam_X.NotifyType_E = mBofSocketChannelParam_X.NotifyType_E;
            BofSocketChannelParam_X.pData = mBofSocketChannelParam_X.pData;
            BofSocketChannelParam_X.RcvBufferSize_U32 = mBofSocketChannelParam_X.RcvBufferSize_U32;
            BofSocketChannelParam_X.SndBufferSize_U32 = mBofSocketChannelParam_X.SndBufferSize_U32;
            BofSocketChannelParam_X.CloseTimeoutInMs_U32 = mBofSocketChannelParam_X.CloseTimeoutInMs_U32;
            BofSocketChannelParam_X.ClosePollingTimeInMs_U32 = mBofSocketChannelParam_X.ClosePollingTimeInMs_U32;

            BofSocketChannelParam_X.OnSocketConnectCallback = mBofSocketChannelParam_X.OnSocketConnectCallback;
            BofSocketChannelParam_X.OnSocketAcceptCallback = mBofSocketChannelParam_X.OnSocketAcceptCallback;
            BofSocketChannelParam_X.OnSocketReadCallback = mBofSocketChannelParam_X.OnSocketReadCallback;

            BofChannelState_X.pAcceptedByThisListener = this;
            BofChannelState_X.pNotifyEvent_X = mBofSocketChannelState_X.pNotifyEvent_X;
            BofChannelState_X.psUserArg = mBofSocketChannelState_X.psUserArg;
            BofChannelState_X.StartTimeInMs_U32 = Bof_GetMsTickCount();
            BofChannelState_X.State_E = BOF_SOCKET_STATE::CONNECTED;
            BofChannelState_X.TimeOutInMs_U32 = mBofSocketChannelState_X.TimeOutInMs_U32;

            Rts_E = mpsBofSocketChannelManager->InternalAddChannel(BofChannelState_X, ClientChannel, nullptr, BofSocketChannelParam_X, psBofSocketChannel);
            if (Rts_E == BOFERR_NO_ERROR)
            {
              if (mBofSocketChannelParam_X.OnSocketAcceptCallback)
              {
                PeerInfo_X.IpAddress_S = RemoteIp.ip;
                PeerInfo_X.Port_U16 = static_cast<uint16_t>(RemoteIp.port);
                BofSocketChannelParamFromAcceptedClient_X = BofSocketChannelParam_X;
                Rts_E = NotifyEvent(psBofSocketChannel, &PeerInfo_X, &BofSocketChannelParamFromAcceptedClient_X);
                // Rts_E = mBofSocketChannelParam_X.OnSocketAcceptCallback(BofSocketChannelParam_X.ChannelId_U32, psBofSocketChannel, PeerInfo_X, &BofSocketChannelParamFromAcceptedClient_X);
                if (BofSocketChannelParam_X.ChannelId_U32 != BofSocketChannelParamFromAcceptedClient_X.ChannelId_U32)
                {
                  psBofSocketChannel->ChannelId(BofSocketChannelParamFromAcceptedClient_X.ChannelId_U32);
                }
                if (BofSocketChannelParam_X.Delimiter_U8 != BofSocketChannelParamFromAcceptedClient_X.Delimiter_U8)
                {
                  psBofSocketChannel->Delimiter(BofSocketChannelParamFromAcceptedClient_X.Delimiter_U8);
                }
                if (BofSocketChannelParam_X.Name_S != BofSocketChannelParamFromAcceptedClient_X.Name_S)
                {
                  psBofSocketChannel->Name(BofSocketChannelParamFromAcceptedClient_X.Name_S);
                }
                if ((BofSocketChannelParam_X.NotifyRcvBufferSize_U32 != BofSocketChannelParamFromAcceptedClient_X.NotifyRcvBufferSize_U32) || (BofSocketChannelParam_X.pData != BofSocketChannelParamFromAcceptedClient_X.pData))
                {
                  //!! do not change this without any exclusion pattern
                  psBofSocketChannel->NotifyRcvBufferSize(BofSocketChannelParamFromAcceptedClient_X.NotifyRcvBufferSize_U32, BofSocketChannelParamFromAcceptedClient_X.pData);
                }
                if (BofSocketChannelParam_X.NotifyType_E != BofSocketChannelParamFromAcceptedClient_X.NotifyType_E)
                {
                  psBofSocketChannel->NotifyType(BofSocketChannelParamFromAcceptedClient_X.NotifyType_E);
                }
                if (BofSocketChannelParam_X.OnSocketReadCallback.target_type() == BofSocketChannelParamFromAcceptedClient_X.OnSocketReadCallback.target_type())
                {
                  //            if (&BofSocketChannelParam_X.OnSocketReadCallback != BofSocketChannelParamFromAcceptedClient_X.OnSocketReadCallback.target<ON_SOCKET_READ_CALLBACK>())
                  {
                    psBofSocketChannel->OnSocketReadCallback(BofSocketChannelParamFromAcceptedClient_X.OnSocketReadCallback);
                  }
                }
                if (BofSocketChannelParam_X.RcvBufferSize_U32 != BofSocketChannelParamFromAcceptedClient_X.RcvBufferSize_U32)
                {
                  psBofSocketChannel->RcvBufferSize(BofSocketChannelParamFromAcceptedClient_X.RcvBufferSize_U32);
                }
                if (BofSocketChannelParam_X.SndBufferSize_U32 != BofSocketChannelParamFromAcceptedClient_X.SndBufferSize_U32)
                {
                  psBofSocketChannel->SndBufferSize(BofSocketChannelParamFromAcceptedClient_X.SndBufferSize_U32);
                }
              }
              Rts_E = AddAcceptedChannel(psBofSocketChannel);
              BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);
              if (Rts_E == BOFERR_NO_ERROR)
              {
                ClientChannel->readPreAllocated(AllocCallback<uvw::TcpHandle>);
              }
            }
          }
          UnlockBofSocketChannel();
        }
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnTcpConnect(const uvw::ConnectEvent & /*_rEvent*/, uvw::TcpHandle &_rHandle)
{
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    mBofSocketChannelStatistic_X.NbConnectEvent_U32++;
    mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::CONNECTED;
    if (mBofSocketChannelState_X.pNotifyEvent_X)
    {
      Bof_SignalEvent(*mBofSocketChannelState_X.pNotifyEvent_X, 0);
    }
    mpsTcpHandle->readPreAllocated(AllocCallback<uvw::TcpHandle>);
    // mpsTcpHandle->read();
  }
  return Rts_E;
}

#if 0

BOFERR BofSocketChannel::OnTcpEvent(const uvw::CloseEvent &_rEvent, uvw::TcpHandle &_rHandle)
{
	BOFERR Rts_E = mErrorCode_E;

	if (Rts_E == BOFERR_NO_ERROR)
	{
		std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
		mBofSocketChannelStatistic_X.NbTcpEvent_U32++;
		if (_rHandle.category() == uvw::HandleCategory::Type::UV_TCP) {}
		if (_rHandle.type() == uvw::HandleType::TCP) {}

		tcp->on<uvw::ListenEvent>([](const uvw::ListenEvent &, uvw::TcpHandle &srv) {
			std::cout << "listen" << std::endl;

			std::shared_ptr<uvw::TcpHandle> client = srv.loop().resource<uvw::TcpHandle>();

			client->on<uvw::ErrorEvent>([](const uvw::ErrorEvent &, uvw::TcpHandle &) {
				std::cout << "error listen client" << std::endl;
			});

			client->on<uvw::CloseEvent>([ptr = srv.shared_from_this()](const uvw::CloseEvent &, uvw::TcpHandle &) {
				std::cout << "close listen client" << std::endl;
				ptr->close();
			});

			srv.accept(*client);

			uvw::Addr local = srv.sock();
			std::cout << "local: " << local.ip << " " << local.port << std::endl;

			uvw::Addr remote = client->peer();
			std::cout << "remote: " << remote.ip << " " << remote.port << std::endl;

			client->on<uvw::DataEvent>([](const uvw::DataEvent &event, uvw::TcpHandle &) {
				std::cout.write(event.data.get(), event.length) << std::endl;
				std::cout << "data length: " << event.length << std::endl;
			});

			client->on<uvw::EndEvent>([](const uvw::EndEvent &, uvw::TcpHandle &handle) {
				std::cout << "end" << std::endl;
				int count = 0;
				//      handle.loop().walk([&count](uvw::BaseHandle &) { ++count; });
				handle.loop().walk([&count](uvw::BaseHandle &_r)
				{
					++count;
					//       _r.close();
				});
				std::cout << "still alive: " << count << " handles" << std::endl;
				//      handle.close();
			});
			client->once<uvw::ShutdownEvent>([](const uvw::ShutdownEvent &, uvw::TcpHandle &handle) {
		client->on<uvw::CloseEvent>([ptr = srv.shared_from_this()](const uvw::CloseEvent &, uvw::TcpHandle &) {
			std::cout << "close listen client" << std::endl;
			ptr->close();
		});
		Rts_E = BOFERR_NO_ERROR;

	}
	return Rts_E;
}
#endif

BOFERR BofSocketChannel::OnTimerEvent(const uvw::TimerEvent & /*_rEvent*/, uvw::TimerHandle &_rHandle)
{
  BOFERR Rts_E = mErrorCode_E;
  bool NotifyEvent_B;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    NotifyEvent_B = false;
    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    mBofSocketChannelStatistic_X.NbTimerEvent_U32++;
    //    DBG_OUT("----> OnTimerEvent %s st %d t %d/%d\n", psUvwContext_X->Name_S.c_str(),mBofSocketChannelState_X.State_E, mBofSocketChannelState_X.StartTimeInMs_U32, mBofSocketChannelState_X.TimeOutInMs_U32);
    switch (mBofSocketChannelState_X.State_E)
    {
      case BOF_SOCKET_STATE::CONNECTING:
        if (Bof_ElapsedMsTime(mBofSocketChannelState_X.StartTimeInMs_U32) >= mBofSocketChannelState_X.TimeOutInMs_U32)
        {
          mBofSocketChannelStatistic_X.NbOpConnectError_U32++;
          mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::CONNECT_TIMEOUT;
          NotifyEvent_B = true;
        }
        break;

      case BOF_SOCKET_STATE::DISCONNECTING:
        if (Bof_ElapsedMsTime(mBofSocketChannelState_X.StartTimeInMs_U32) >= mBofSocketChannelState_X.TimeOutInMs_U32)
        {
          mBofSocketChannelStatistic_X.NbOpDisconnectError_U32++;
          mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::DISCONNECT_TIMEOUT;
          NotifyEvent_B = true;
        }
        break;

      case BOF_SOCKET_STATE::LISTENING:
        if (Bof_ElapsedMsTime(mBofSocketChannelState_X.StartTimeInMs_U32) >= mBofSocketChannelState_X.TimeOutInMs_U32)
        {
          mBofSocketChannelStatistic_X.NbOpListenError_U32++;
          mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::LISTENING_TIMEOUT;
          CloseListenerChannelAndItsAcceptedChannel();
          NotifyEvent_B = true;
        }
        break;
      default:
        break;
    }
    if (NotifyEvent_B)
    {
      if (mBofSocketChannelState_X.pNotifyEvent_X)
      {
        Bof_SignalEvent(*mBofSocketChannelState_X.pNotifyEvent_X, 0);
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnTimerError(const uvw::ErrorEvent & /*_rEvent*/, uvw::TimerHandle &_rHandle)
{
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    std::shared_ptr<UVW_CONTEXT> psUvwContext_X = _rHandle.data<UVW_CONTEXT>();
    mBofSocketChannelStatistic_X.NbTimerEventError_U32++;
    Rts_E = BOFERR_NO_ERROR;
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnAsyncConnect(const BOF_SOCKET_ASYNC_CMD &_rBofSocketAsyncCmd_X)
{
  BOFERR Rts_E = mErrorCode_E;
  std::string IpAddress_S;
  bool NotifyEvent_B;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    Rts_E = BOFERR_NOT_OPENED;
    if (mpsBofSocketChannelManager)
    {
      Rts_E = BOFERR_ALREADY_OPENED;
      if (!IsConnected())
      {
        Rts_E = BOFERR_NO_ERROR;
        NotifyEvent_B = false;

        mBofSocketChannelState_X.StartTimeInMs_U32 = _rBofSocketAsyncCmd_X.StartTimeInMs_U32;
        // Lower _rBofSocketAsyncCmd_X to in engine as caller can call Bof_WaitForEvent(pNotifyEvent_X[i_U32], ASYNC_CONNECT_TIMEOUT-> in case of timeout we want to be the first to signal CONNECT_TIMEOUT
        mBofSocketChannelState_X.TimeOutInMs_U32 = _rBofSocketAsyncCmd_X.TimeOutInMs_U32 - (1 * TIMER_GRANULARITY_IN_MS);
        mBofSocketChannelState_X.pNotifyEvent_X = _rBofSocketAsyncCmd_X.pNotifyEvent_X;
        // No lock as timer cb is called also by uv as this one
        if (Bof_ElapsedMsTime(_rBofSocketAsyncCmd_X.StartTimeInMs_U32) >= _rBofSocketAsyncCmd_X.TimeOutInMs_U32)
        {
          mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::CONNECT_TIMEOUT;
          NotifyEvent_B = true;
        }
        else
        {
          if (mIsUdp_B)
          {
            mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::CONNECTED;
            mBofSocketChannelStatistic_X.NbOpConnect_U32++;
            mBofSocketChannelStatistic_X.NbConnectEvent_U32++;
            NotifyEvent_B = true;
            mpsUdpHandle->recvPreAllocated(AllocCallback<uvw::UDPHandle>);
            // mpsUdpHandle->recv();
          }
          else
          {
            IpAddress_S = Bof_IpAddressToString(mBofSocketAddress_X, false, false);
            mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::CONNECTING;
            // DBG_OUT("----> Connect %s port %d\n", IpAddress_S.c_str(), mBofSocketAddress_X.Port());
            mBofSocketChannelStatistic_X.NbOpConnect_U32++;
            mpsTcpHandle->connect(IpAddress_S, mBofSocketAddress_X.Port());
          }
        }
        if (NotifyEvent_B)
        {
          if (mBofSocketChannelState_X.pNotifyEvent_X)
          {
            Rts_E = Bof_SignalEvent(*mBofSocketChannelState_X.pNotifyEvent_X, 0);
          }
        }
      }
    }
  }
  return Rts_E;
}

#if 0
std::shared_ptr<uvw::TcpHandle> psHandle = mpsloloop.resource<uvw::TcpHandle>();

	tcp->on<uvw::ErrorEvent>([](const uvw::ErrorEvent &, uvw::TcpHandle &) {
		std::cout << "error " << std::endl;
	});

	tcp->once<uvw::WriteEvent>([](const uvw::WriteEvent &, uvw::TcpHandle &handle) {
		std::cout << "write" << std::endl;
		handle.close();
	});

	tcp->once<uvw::ConnectEvent>([](const uvw::ConnectEvent &, uvw::TcpHandle &handle) {
		std::cout << "connect" << std::endl;

		auto dataTryWrite = std::unique_ptr<char[]>(new char[1]{ 'a' });
		int bw = handle.tryWrite(std::move(dataTryWrite), 1);
		std::cout << "written: " << ((int)bw) << std::endl;

		auto dataWrite = std::unique_ptr<char[]>(new char[2]{ 'b', 'c' });
		handle.write(std::move(dataWrite), 2);
	});

	tcp->once<uvw::CloseEvent>([](const uvw::CloseEvent &, uvw::TcpHandle &) {
		std::cout << "close" << std::endl;
	});

	tcp->connect("127.0.0.1", 4242);
#endif

BOFERR BofSocketChannel::OnAsyncDisconnect(const BOF_SOCKET_ASYNC_CMD &_rBofSocketAsyncCmd_X)
{
  BOFERR Rts_E = mErrorCode_E;
  std::string IpAddress_S;
  bool NotifyEvent_B;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    Rts_E = BOFERR_NOT_OPENED;
    if (mpsBofSocketChannelManager)
    {
      Rts_E = BOFERR_NOT_OPENED;
      printf("_______ disconnect %p iscon %d ResState %d\n", static_cast<void *>(this), IsConnected(), static_cast<uint32_t>(mChannelResourceReleaseState_E));

      if (IsConnected())
      {
        Rts_E = BOFERR_NO_ERROR;
        NotifyEvent_B = false;

        mBofSocketChannelState_X.StartTimeInMs_U32 = _rBofSocketAsyncCmd_X.StartTimeInMs_U32;
        // Lower _rBofSocketAsyncCmd_X to in engine as caller can call Bof_WaitForEvent(pNotifyEvent_X[i_U32], ASYNC_CONNECT_TIMEOUT-> in case of timeout we want to be the first to signal CONNECT_TIMEOUT
        mBofSocketChannelState_X.TimeOutInMs_U32 = _rBofSocketAsyncCmd_X.TimeOutInMs_U32 - (1 * TIMER_GRANULARITY_IN_MS);
        mBofSocketChannelState_X.pNotifyEvent_X = _rBofSocketAsyncCmd_X.pNotifyEvent_X;
        // No lock as timer cb is called also by uv as this one
        if (Bof_ElapsedMsTime(_rBofSocketAsyncCmd_X.StartTimeInMs_U32) >= _rBofSocketAsyncCmd_X.TimeOutInMs_U32)
        {
          printf("_______ disconnect %p TO\n", static_cast<void *>(this));

          mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::DISCONNECT_TIMEOUT;
          NotifyEvent_B = true;
        }
        else
        {
          printf("_______ disconnect %p mIsUdp_B %d\n", static_cast<void *>(this), mIsUdp_B);
          if (mIsUdp_B)
          {
            if (mpsUdpHandle->closing())
            {
              mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::DISCONNECTED;
              NotifyEvent_B = true;
            }
            else
            {
              mBofSocketChannelStatistic_X.NbOpDisconnect_U32++;
              mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::DISCONNECTING;
              mpsUdpHandle->stop();
              if (_rBofSocketAsyncCmd_X.pCmdArg_U32[0] == 0)
              {
                // Close should not be made here as we have no sync with manager or channel...
                ChannelResourceReleaseState(CHANNEL_RESOURCE_RELEASE_STATE::PENDING);
                mpsUdpHandle->close();
              }
            }
          }
          else
          {
            printf("_______ disconnect %p mpsTcpHandle->closing() %d\n", static_cast<void *>(this), mpsTcpHandle->closing());

            if (mpsTcpHandle->closing())
            {
              mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::DISCONNECTED;
              NotifyEvent_B = true;
            }
            else
            {
              mBofSocketChannelStatistic_X.NbOpDisconnect_U32++;
              mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::DISCONNECTING;
              mpsTcpHandle->stop();
              if (_rBofSocketAsyncCmd_X.pCmdArg_U32[0] == 0)
              {
                printf("_______ disconnect %p stop/close ResState %d->Mark as pending as we call close\n", static_cast<void *>(this), static_cast<uint32_t>(mChannelResourceReleaseState_E));
                ChannelResourceReleaseState(CHANNEL_RESOURCE_RELEASE_STATE::PENDING);
                mpsTcpHandle->close();
              }
              else
              {
                printf("_______ disconnect %p stop/close ResState %d\n", static_cast<void *>(this), static_cast<uint32_t>(mChannelResourceReleaseState_E));
              }
            }
          }
        }
        if (NotifyEvent_B)
        {
          if (mBofSocketChannelState_X.pNotifyEvent_X)
          {
            Rts_E = Bof_SignalEvent(*mBofSocketChannelState_X.pNotifyEvent_X, 0);
          }
        }
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnAsyncListen(const BOF_SOCKET_ASYNC_CMD &_rBofSocketAsyncCmd_X)
{
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    Rts_E = BOFERR_NOT_AVAILABLE;
    BOF_ASSERT(IsAListener());
    if (IsAListener())
    {
      Rts_E = BOFERR_NOT_OPENED;
      if (mpsBofSocketChannelManager)
      {
        Rts_E = BOFERR_ALREADY_OPENED;
        if (!IsConnected())
        {
          Rts_E = BOFERR_NO_ERROR;
          mBofSocketChannelState_X.StartTimeInMs_U32 = _rBofSocketAsyncCmd_X.StartTimeInMs_U32;
          // Lower _rBofSocketAsyncCmd_X to in engine as caller can call Bof_WaitForEvent(pNotifyEvent_X[i_U32], ASYNC_CONNECT_TIMEOUT-> in case of timeout we want to be the first to signal CONNECT_TIMEOUT
          mBofSocketChannelState_X.TimeOutInMs_U32 = (_rBofSocketAsyncCmd_X.TimeOutInMs_U32 > 100) ? _rBofSocketAsyncCmd_X.TimeOutInMs_U32 - 100 : static_cast<uint32_t>(static_cast<float>(_rBofSocketAsyncCmd_X.TimeOutInMs_U32) * 0.9f);
          mBofSocketChannelState_X.pNotifyEvent_X = _rBofSocketAsyncCmd_X.pNotifyEvent_X;
          mMinPort_U32 = (_rBofSocketAsyncCmd_X.pCmdArg_U32[0] <= _rBofSocketAsyncCmd_X.pCmdArg_U32[1]) ? _rBofSocketAsyncCmd_X.pCmdArg_U32[0] : _rBofSocketAsyncCmd_X.pCmdArg_U32[1];
          mMaxPort_U32 = (_rBofSocketAsyncCmd_X.pCmdArg_U32[0] <= _rBofSocketAsyncCmd_X.pCmdArg_U32[1]) ? _rBofSocketAsyncCmd_X.pCmdArg_U32[1] : _rBofSocketAsyncCmd_X.pCmdArg_U32[0];

          mMinPort_U32 = (mMinPort_U32 == 0) ? 49152 : mMinPort_U32; // https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers
          mMaxPort_U32 = (mMaxPort_U32 == 0) ? 65535 : mMaxPort_U32; // https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers;

          mNextAvailablePort_U32 = mMinPort_U32;
          mBofSocketChannelState_X.State_E = BOF_SOCKET_STATE::LISTENING;
          mpsTcpHandle->listen(mBofSocketChannelParam_X.ListenBacklog_U32);
        }
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::OnAsyncWrite(const BOF_SOCKET_ASYNC_CMD &_rBofSocketAsyncCmd_X)
{
  BOFERR Rts_E = mErrorCode_E;
  uint32_t i_U32;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    for (i_U32 = 0; i_U32 < _rBofSocketAsyncCmd_X.BufferCollection.size(); i_U32++)
    {
      if (mIsUdp_B)
      {
        std::string IpAddress_S = Bof_IpAddressToString(mBofSocketAddress_X, false, false);
        if (_rBofSocketAsyncCmd_X.BufferCollection[i_U32].MustBeDeleted_B)
        {
          std::unique_ptr<char[]> puData_U8(reinterpret_cast<char *>(_rBofSocketAsyncCmd_X.BufferCollection[i_U32].pData_U8));
          // char *p = puData_U8.get();
          mpsUdpHandle->send(uvw::Addr{IpAddress_S, mBofSocketAddress_X.Port()}, std::move(puData_U8), static_cast<uint32_t>(_rBofSocketAsyncCmd_X.BufferCollection[i_U32].SizeInByte_U64));
        }
        else
        {
          mpsUdpHandle->send(uvw::Addr{IpAddress_S, mBofSocketAddress_X.Port()}, reinterpret_cast<char *>(_rBofSocketAsyncCmd_X.BufferCollection[i_U32].pData_U8), static_cast<uint32_t>(_rBofSocketAsyncCmd_X.BufferCollection[i_U32].SizeInByte_U64));
        }
      }
      else
      {
        if (_rBofSocketAsyncCmd_X.BufferCollection[i_U32].MustBeDeleted_B)
        {
          std::unique_ptr<char[]> puData_U8(reinterpret_cast<char *>(_rBofSocketAsyncCmd_X.BufferCollection[i_U32].pData_U8));
          // char *p = puData_U8.get();
          mpsTcpHandle->write(std::move(puData_U8), static_cast<uint32_t>(_rBofSocketAsyncCmd_X.BufferCollection[i_U32].SizeInByte_U64));
        }
        else
        {
          mpsTcpHandle->write(reinterpret_cast<char *>(_rBofSocketAsyncCmd_X.BufferCollection[i_U32].pData_U8), static_cast<uint32_t>(_rBofSocketAsyncCmd_X.BufferCollection[i_U32].SizeInByte_U64));
        }
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::SetSocketBufferSizeOption(const std::shared_ptr<uvw::TcpHandle> &_rpsHandle, uint32_t _RcvBufferSize_U32, uint32_t _SndBufferSize_U32)
{
  BOFERR Rts_E = mErrorCode_E;
  int ReadVal_i, Val_i;

  if (Rts_E == BOFERR_NO_ERROR)
  {
#if defined(_WIN32)
    Val_i = _RcvBufferSize_U32;
#else
    Val_i = _RcvBufferSize_U32 / 2; /* linux sets double the value */
#endif
    if (Val_i)
    {
      Rts_E = BOFERR_WRONG_SIZE;
      if (_rpsHandle->recvBufferSize(Val_i))
      {
        ReadVal_i = _rpsHandle->recvBufferSize();
        BOF_ASSERT(static_cast<uint32_t>(ReadVal_i) == _RcvBufferSize_U32);
        Rts_E = BOFERR_NO_ERROR;
      }
    }
  }

  if (Rts_E == BOFERR_NO_ERROR)
  {
#if defined(_WIN32)
    Val_i = _SndBufferSize_U32;
#else
    Val_i = _SndBufferSize_U32 / 2; /* linux sets double the value */
#endif
    if (Val_i)
    {
      Rts_E = BOFERR_WRONG_SIZE;
      if (_rpsHandle->sendBufferSize(Val_i))
      {
        ReadVal_i = _rpsHandle->sendBufferSize();
        BOF_ASSERT(static_cast<uint32_t>(ReadVal_i) == _SndBufferSize_U32);
        Rts_E = BOFERR_NO_ERROR;
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::SetSocketChannelOption(const std::shared_ptr<uvw::TcpHandle> &_rpsHandle, const BOF_TCP_SOCKET_PARAM &_rBofTcpSocketParam_X)
{
  int Option_i, Sts_i;
  uv_os_sock_t Socket;
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    if (_rpsHandle->noDelay(_rBofTcpSocketParam_X.NoDelay_B))
    {
      if (_rpsHandle->simultaneousAccepts(_rBofTcpSocketParam_X.SimultaneousAccept_B))
      {
        if (_rpsHandle->keepAlive((_rBofTcpSocketParam_X.KeepAliveTimeInMs_U32 != 0), uvw::TcpHandle::Time{_rBofTcpSocketParam_X.KeepAliveTimeInMs_U32}))
        {
          Rts_E = SetSocketBufferSizeOption(_rpsHandle, _rBofTcpSocketParam_X.RcvBufferSize_U32, _rBofTcpSocketParam_X.SndBufferSize_U32);
        }
      }
    }
    if (Rts_E == BOFERR_NO_ERROR)
    {
      uv_tcp_t *pUvTcp_X = _rpsHandle->raw();
#if defined(_WIN32)
      Socket = pUvTcp_X->socket;
#else
      // From unix\internal.h
#if defined(__APPLE__)
      int uv___stream_fd(const uv_stream_t *handle);
#define uv__stream_fd(handle) (uv___stream_fd((const uv_stream_t *)(handle)))
#else
#define uv__stream_fd(handle) ((handle)->io_watcher.fd)
#endif /* defined(__APPLE__) */

      Socket = uv__stream_fd((uv_stream_t *)pUvTcp_X);
      ;
#endif
      /* Allow reuse of the port. */
      Option_i = 1;
      Sts_i = setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&Option_i), sizeof(Option_i));
      BOF_ASSERT(Sts_i == 0);
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::SetSocketBufferSizeOption(const std::shared_ptr<uvw::UDPHandle> &_rpsHandle, uint32_t _RcvBufferSize_U32, uint32_t _SndBufferSize_U32)
{
  BOFERR Rts_E = mErrorCode_E;
  int ReadVal_i, Val_i;

  if (Rts_E == BOFERR_NO_ERROR)
  {
#if defined(_WIN32)
    Val_i = _RcvBufferSize_U32;
#else
    Val_i = _RcvBufferSize_U32 / 2; /* linux sets double the value */
#endif
    if (Val_i)
    {
      Rts_E = BOFERR_WRONG_SIZE;
      if (_rpsHandle->recvBufferSize(Val_i))
      {
        ReadVal_i = _rpsHandle->recvBufferSize();
        BOF_ASSERT(static_cast<uint32_t>(ReadVal_i) == _RcvBufferSize_U32);
        Rts_E = BOFERR_NO_ERROR;
      }
    }
  }

  if (Rts_E == BOFERR_NO_ERROR)
  {
#if defined(_WIN32)
    Val_i = _SndBufferSize_U32;
#else
    Val_i = _SndBufferSize_U32 / 2; /* linux sets double the value */
#endif
    if (Val_i)
    {
      Rts_E = BOFERR_WRONG_SIZE;
      if (_rpsHandle->sendBufferSize(Val_i))
      {
        ReadVal_i = _rpsHandle->sendBufferSize();
        BOF_ASSERT(static_cast<uint32_t>(ReadVal_i) == _SndBufferSize_U32);
        Rts_E = BOFERR_NO_ERROR;
      }
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::SetSocketChannelOption(const std::shared_ptr<uvw::UDPHandle> &_rpsHandle, const BOF_UDP_SOCKET_PARAM &_rBofUdpSocketParam_X)
{
  int Sts_i, Option_i;
  BOF_SOCKET_ADDRESS_COMPONENT InterfaceIpAddressComponent_X, IpAddressComponent_X;
  uv_os_sock_t Socket;
  BOFERR Rts_E = mErrorCode_E;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    if (_rpsHandle->broadcast(_rBofUdpSocketParam_X.Broadcast_B))
    {
      if (Bof_IsMulticastUrlAddress(mBofSocketChannelParam_X.IpAddress_S, InterfaceIpAddressComponent_X, IpAddressComponent_X))
      {
        if (_rBofUdpSocketParam_X.MulticastReceiver_B)
        {
          if (_rpsHandle->multicastMembership(IpAddressComponent_X.IpAddress_S.c_str(), InterfaceIpAddressComponent_X.IpAddress_S.c_str(), uvw::UDPHandle::Membership::JOIN_GROUP))
          {
            Rts_E = BOFERR_NO_ERROR;
          }
        }
        else
        {
          if (_rpsHandle->multicastLoop(_rBofUdpSocketParam_X.MulticastLoop_B))
          {
            if (_rpsHandle->multicastTtl(_rBofUdpSocketParam_X.Ttl_U32))
            {
              if (_rpsHandle->multicastInterface(InterfaceIpAddressComponent_X.IpAddress_S.c_str()))
              {
                Rts_E = SetSocketBufferSizeOption(_rpsHandle, _rBofUdpSocketParam_X.RcvBufferSize_U32, _rBofUdpSocketParam_X.SndBufferSize_U32);
              }
            }
          }
        }
      }
    }
    if (Rts_E == BOFERR_NO_ERROR)
    {
      uv_udp_t *pUvUdp_X = _rpsHandle->raw();
#if defined(_WIN32)
      Socket = pUvUdp_X->socket;
#else
      Socket = pUvUdp_X->io_watcher.fd;
#endif

      /* Allow reuse of the port. */
      Option_i = 1;
      Sts_i = setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&Option_i), sizeof(Option_i));
      BOF_ASSERT(Sts_i == 0);
    }
  }
  return Rts_E;
}

BOFERR BofSocketChannel::NotifyEvent(std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> _psBofSocketChannel, BOF_SOCKET_PEER_INFO *_pPeerInfo_X, BOF_SOCKET_CHANNEL_PARAM *_pBofSocketChannelParamFromAcceptedClient_X)
{
  BOF_SOCKET_PEER_INFO PeerInfo_X;
  BOFERR Rts_E = mErrorCode_E;
  std::shared_ptr<BOF_NAMESPACE::BofSocketChannel> psBofSocketChannel = (_psBofSocketChannel == nullptr) ? shared_from_this() : _psBofSocketChannel;

  if (Rts_E == BOFERR_NO_ERROR)
  {
    if (_pPeerInfo_X)
    {
      PeerInfo_X = *_pPeerInfo_X;
    }
    else
    {
      PeerInfo_X.IpAddress_S = Bof_IpAddressToString(mBofSocketAddress_X, false, false);
      PeerInfo_X.Port_U16 = mBofSocketAddress_X.Port();
    }
    if (mBofSocketChannelParam_X.OnSocketConnectCallback)
    {
      Rts_E = mBofSocketChannelParam_X.OnSocketConnectCallback(mBofSocketChannelParam_X.ChannelId_U32, psBofSocketChannel, PeerInfo_X);
    }
    if (mBofSocketChannelParam_X.OnSocketAcceptCallback)
    {
      Rts_E = mBofSocketChannelParam_X.OnSocketAcceptCallback(mBofSocketChannelParam_X.ChannelId_U32, psBofSocketChannel, PeerInfo_X, _pBofSocketChannelParamFromAcceptedClient_X);
    }
    if (mBofSocketChannelState_X.pNotifyEvent_X)
    {
      Bof_SignalEvent(*mBofSocketChannelState_X.pNotifyEvent_X, 0);
    }
  }
  return Rts_E;
}

END_BOF_NAMESPACE()

#if 0

void timer(uvw::Loop &loop)
{
	auto handleRepeat = loop.resource<uvw::TimerHandle>();

	//  int checkTimerRepeatEvent = 10;

	handleRepeat->on<uvw::ErrorEvent>([](const auto &, auto &) {  std::cout << "error timer " << std::endl; });

	handleRepeat->on<uvw::TimerEvent>([](const auto &, auto &handle) {
		static int checkTimerRepeatEvent = 10;
		if (checkTimerRepeatEvent == 0) {
			handle.stop();
			handle.close();
			std::cout << "stop timer " << std::endl;
			int count = 0;
			handle.loop().walk([&count](uvw::BaseHandle &_r)
			{
				++count;
				_r.close();
			});
			std::cout << "kill still alive: " << count << " handles" << std::endl;
		}
		else {
			checkTimerRepeatEvent--;
			std::cout << "timer run " << checkTimerRepeatEvent << " " << BOF_NAMESPACE::Bof_GetMsTickCount() << std::endl;
		}
	});

	handleRepeat->start(uvw::TimerHandle::Time{ 0 }, uvw::TimerHandle::Time{ 1000 }); //use std::chrono::duration<uint64_t, std::milli>

}

void listen(uvw::Loop &loop) {
	std::shared_ptr<uvw::TcpHandle> tcp = loop.resource<uvw::TcpHandle>();

	tcp->noDelay(true);
	tcp->keepAlive(true, uvw::TcpHandle::Time{ 128 });
	tcp->simultaneousAccepts(true);

	tcp->on<uvw::ErrorEvent>([](const uvw::ErrorEvent &, uvw::TcpHandle &) {
		std::cout << "error listen " << std::endl;
	});

	tcp->on<uvw::ListenEvent>([](const uvw::ListenEvent &, uvw::TcpHandle &srv) {
		std::cout << "listen" << std::endl;

		std::shared_ptr<uvw::TcpHandle> client = srv.loop().resource<uvw::TcpHandle>();

		client->on<uvw::ErrorEvent>([](const uvw::ErrorEvent &, uvw::TcpHandle &) {
			std::cout << "error listen client" << std::endl;
		});

		client->on<uvw::CloseEvent>([ptr = srv.shared_from_this()](const uvw::CloseEvent &, uvw::TcpHandle &) {
			std::cout << "close listen client" << std::endl;
			ptr->close();
		});

		srv.accept(*client);

		uvw::Addr local = srv.sock();
		std::cout << "local: " << local.ip << " " << local.port << std::endl;

		uvw::Addr remote = client->peer();
		std::cout << "remote: " << remote.ip << " " << remote.port << std::endl;

		client->on<uvw::DataEvent>([](const uvw::DataEvent &event, uvw::TcpHandle &) {
			std::cout.write(event.data.get(), event.length) << std::endl;
			std::cout << "data length: " << event.length << std::endl;
		});

		client->on<uvw::EndEvent>([](const uvw::EndEvent &, uvw::TcpHandle &handle) {
			std::cout << "end" << std::endl;
			int count = 0;
			//      handle.loop().walk([&count](uvw::BaseHandle &) { ++count; });
			handle.loop().walk([&count](uvw::BaseHandle &_r)
			{
				++count;
				//       _r.close();
			});
			std::cout << "still alive: " << count << " handles" << std::endl;
			//      handle.close();
		});
		client->once<uvw::ShutdownEvent>([](const uvw::ShutdownEvent &, uvw::TcpHandle &handle) {
			handle.close();
		});
		client->read();
	});

	tcp->once<uvw::CloseEvent>([](const uvw::CloseEvent &, uvw::TcpHandle &) {
		std::cout << "close" << std::endl;
	});

	tcp->bind("127.0.0.1", 4242);
	tcp->listen();
}


void conn(uvw::Loop &loop) {
	auto tcp = loop.resource<uvw::TcpHandle>();

	tcp->on<uvw::ErrorEvent>([](const uvw::ErrorEvent &, uvw::TcpHandle &) {
		std::cout << "error " << std::endl;
	});

	tcp->once<uvw::WriteEvent>([](const uvw::WriteEvent &, uvw::TcpHandle &handle) {
		std::cout << "write" << std::endl;
		handle.close();
	});

	tcp->once<uvw::ConnectEvent>([](const uvw::ConnectEvent &, uvw::TcpHandle &handle) {
		std::cout << "connect" << std::endl;

		auto dataTryWrite = std::unique_ptr<char[]>(new char[1]{ 'a' });
		int bw = handle.tryWrite(std::move(dataTryWrite), 1);
		std::cout << "written: " << ((int)bw) << std::endl;

		auto dataWrite = std::unique_ptr<char[]>(new char[2]{ 'b', 'c' });
		handle.write(std::move(dataWrite), 2);
	});

	tcp->once<uvw::CloseEvent>([](const uvw::CloseEvent &, uvw::TcpHandle &) {
		std::cout << "close" << std::endl;
	});

	tcp->connect("127.0.0.1", 4242);
}

void udp(uvw::Loop &loop) {
	auto handle = loop.resource<uvw::UDPHandle>();

	handle->multicastMembership("224.0.0.1", "127.0.0.1", uvw::UDPHandle::Membership::JOIN_GROUP);
	//handle->multicastMembership("224.0.0.1", "127.0.0.1", uvw::UDPHandle::Membership::LEAVE_GROUP);
	handle->multicastLoop(true);
	handle->multicastTtl(42);
	handle->multicastInterface("127.0.0.1");
	handle->broadcast(true);
	handle->ttl(42);
	auto server = loop.resource<uvw::UDPHandle>();
	auto client = loop.resource<uvw::UDPHandle>();

	struct bh
	{
		int x, y;
		bh(int a, int b)
		{
			x = a;
			y = b * 2;
		}
	};

	server->data(std::make_shared<bh>(42, 55));
	std::shared_ptr<bh> p = server->data<bh>();
	auto resource = loop.resource<uvw::Condition>();

	server->on<uvw::ErrorEvent>([](const auto &, auto &)
	{
		std::cout << "udp server err" << std::endl;
	});
	client->on<uvw::ErrorEvent>([](const auto &a, auto &b)
	{
		std::cout << "udp client err" << std::endl;
	});

	server->on<uvw::UDPDataEvent>([](const uvw::UDPDataEvent &e, uvw::UDPHandle &handle) {
		//   handle.close();
		printf("udp rcv %d:%p part %d sen %s:%d\n", e.length, e.data.get(), e.partial, e.sender.ip.c_str(), e.sender.port);
	});

	client->on<uvw::SendEvent>([](const uvw::SendEvent &e, uvw::UDPHandle &handle) {
		//handle.close();
		printf("Udp sent \n");
	});
	const std::string address = std::string{ "0.0.0.0" }; // "127.0.0.1"};
	const unsigned int port = 4242;

	server->bind(address, port);
	server->recv();
	auto dataTrySend = std::unique_ptr<char[]>(new char[1]{ 'a' });

	client->trySend(uvw::Addr{ address, port }, dataTrySend.get(), 1);
	auto dataSend = std::unique_ptr<char[]>(new char[2]{ 'b', 'c' });

	client->send(uvw::Addr{ address , port }, dataSend.get(), 2);
	client->send(address, port, nullptr, 0);

	client->send(uvw::Addr{ address , port }, std::move(dataSend), 2);
	client->send(address, port, std::unique_ptr<char[]>{}, 0);
}


void g() {
	auto loop = uvw::Loop::getDefault();
	timer(*loop);
	listen(*loop);
	conn(*loop);
	conn(*loop);
	udp(*loop);
	loop->run();
	loop = nullptr;
}

Work:
handle->on<uvw::CheckEvent>([&checkWorkEvent](const auto &, auto &hndl) {


void on_after_work(uv_work_t* req, int status) {
free(req);
}

void on_work(uv_work_t* req) {
// "Work"

if (random() % 5 == 0) {
printf("Sleeping...\n");
sleep(3);
}

}
uv_work_t* work_req = (uv_work_t*)malloc(sizeof(*work_req));
uv_queue_work(uv_default_loop(), work_req, on_work, on_after_work);


							}

#endif