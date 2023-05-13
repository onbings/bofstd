#include <bofstd/bofioconnectionmanager.h>
#include <bofstd/bofsocketos.h>
#include <bofstd/bofstringformatter.h>
#include <bofstd/ibofioconnection.h>
#include <bofstd/ibofioconnectionfactory.h>

static uint32_t S_LineNumber_U32 = 0;
static uint32_t S_LastTime_U32 = 0;

template <typename... Args> void DbgOut(const char *_pFmt_c, const Args &..._Args)
{
  uint32_t Now_U32, Delta_U32;
  char pText_c[0x1000];

  Now_U32 = BOF_NAMESPACE::Bof_GetMsTickCount();
  Delta_U32 = Now_U32 - S_LastTime_U32;
  sprintf(pText_c, _pFmt_c, _Args...);
  printf("%08d %06d %06d %s", S_LineNumber_U32++, Now_U32, Delta_U32, pText_c);
  S_LastTime_U32 = Now_U32;
}

BEGIN_BOF_NAMESPACE()

uint32_t BofIoConnectionManager::S_mCmdTicket_U32 = 0;
uint32_t BofIoConnectionManager::S_mReqTicket_U32 = 0;
uint32_t BofIoConnectionManager::S_mListenId_U32 = 0;
uint32_t BofIoConnectionManager::S_mSessionId_U32 = 0;
uint32_t BofIoConnectionManager::S_mClientId_U32 = 0;

BofIoConnectionManager::BofIoConnectionManager(const BOF_IO_CONNECTION_MANAGER_PARAM &_rBofIoConnectionManagerParam_X)
    : BofThread(_rBofIoConnectionManagerParam_X.Name_S), mpBofIoAsyncCmdCollection(nullptr), mBofIoConnectionManagerParam_X(_rBofIoConnectionManagerParam_X), mUvWalkResult_S(""), mpuLoop_X(std::make_unique<uv_loop_t>()),
      mpuAsync_X(std::make_unique<uv_async_t>()), mpuTimer_X(std::make_unique<uv_timer_t>()), mpIoAsyncRequestCollection(nullptr), mpUvConnectionCollection(nullptr), mLastLockerName_S("")
{
  BOF_CIRCULAR_BUFFER_PARAM BofCircularBufferParam_X;
  BOF_POT_PARAM BofPotParam_X;
  BOF_UV_CALLBACK *pBofUvCallback_X;
  int Sts_i;

  mErrorCode_E = Bof_CreateMutex(_rBofIoConnectionManagerParam_X.Name_S + "_mtx", false, false, mConnectionMtx);
  if (mErrorCode_E == BOFERR_NO_ERROR)
  {
    mErrorCode_E = BOFERR_NOT_ENOUGH_RESOURCE;
    BofCircularBufferParam_X.MultiThreadAware_B = true;
    BofCircularBufferParam_X.NbMaxElement_U32 = _rBofIoConnectionManagerParam_X.NbMaxOpPending_U32;
    BofCircularBufferParam_X.Overwrite_B = false;
    BofCircularBufferParam_X.Blocking_B = true; // false;
    mpBofIoAsyncCmdCollection = new BofCircularBuffer<BOF_IO_ASYNC_CMD>(BofCircularBufferParam_X);
    if (mpBofIoAsyncCmdCollection)
    {
      mErrorCode_E = mpBofIoAsyncCmdCollection->LastErrorCode();
    }
  }

  if (mErrorCode_E == BOFERR_NO_ERROR)
  {
    ALLOC_UV_CALLBACK_DATA(mpuLoop_X.get(), pBofUvCallback_X, std::string("Loop"), false, nullptr, nullptr, nullptr);
    Sts_i = uv_loop_init(mpuLoop_X.get());
    mErrorCode_E = (Sts_i == 0) ? BOFERR_NO_ERROR : BOFERR_NOT_ENOUGH_RESOURCE;
    if (mErrorCode_E == BOFERR_NO_ERROR)
    {
      mErrorCode_E = BOFERR_NOT_ENOUGH_RESOURCE;
      ALLOC_UV_CALLBACK_DATA(mpuTimer_X.get(), pBofUvCallback_X, std::string("Timer"), false, nullptr, nullptr, nullptr);
      Sts_i = uv_timer_init(mpuLoop_X.get(), mpuTimer_X.get());
      BOF_ASSERT(Sts_i == 0);
      if (Sts_i == 0)
      {
        ALLOC_UV_CALLBACK_DATA(mpuAsync_X.get(), pBofUvCallback_X, std::string("Async"), false, nullptr, nullptr, nullptr);
        Sts_i = uv_async_init(mpuLoop_X.get(), mpuAsync_X.get(), S_OnUvAsyncCallback);
        BOF_ASSERT(Sts_i == 0);
        if (Sts_i == 0)
        {
          mErrorCode_E = BOFERR_NO_ERROR;
        }
      }
    }
    if (mErrorCode_E == BOFERR_NO_ERROR)
    {
      mErrorCode_E = BOFERR_NOT_ENOUGH_RESOURCE;

      BofPotParam_X.GetDoNotErasePotElement_B = true;
      BofPotParam_X.MagicNumber_U32 = 0;
      BofPotParam_X.MultiThreadAware_B = true;
      BofPotParam_X.Blocking_B = false;
      // mpPotOfIoAsyncRequest->Get():
      // NbMaxOpPending_U32 for each pending op in mpBofIoAsyncCmdCollection OnUvAsyncWrite
      // +NbMaxConnection_U32 for OnUvAsyncDisconnect
      // *2 security
      BofPotParam_X.PotCapacity_U32 = _rBofIoConnectionManagerParam_X.NbMaxOpPending_U32 + (mBofIoConnectionManagerParam_X.NbMaxConnection_U32 * 2);
      mpIoAsyncRequestCollection = new BOF_NAMESPACE::BofPot<BOF_IO_ASYNC_REQ>(BofPotParam_X);
      if (mpIoAsyncRequestCollection)
      {
        mErrorCode_E = mpIoAsyncRequestCollection->LastErrorCode();
      }
      if (mErrorCode_E == BOFERR_NO_ERROR)
      {
        BofPotParam_X.GetDoNotErasePotElement_B = true;
        BofPotParam_X.MagicNumber_U32 = 0;
        BofPotParam_X.MultiThreadAware_B = true;
        BofPotParam_X.Blocking_B = false;
        BofPotParam_X.PotCapacity_U32 = _rBofIoConnectionManagerParam_X.NbMaxConnection_U32;
        mpUvConnectionCollection = new BOF_NAMESPACE::BofPot<BOF_UV_CONNECTION>(BofPotParam_X);
        if (mpUvConnectionCollection)
        {
          mErrorCode_E = mpUvConnectionCollection->LastErrorCode();
        }
        if (mErrorCode_E == BOFERR_NO_ERROR)
        {
        }
      }
    }
    if (mErrorCode_E == BOFERR_NO_ERROR)
    {
      mErrorCode_E = LaunchThread(false, 0, 0x40000, 0, mBofIoConnectionManagerParam_X.Policy_S32, mBofIoConnectionManagerParam_X.Priority_S32, mBofIoConnectionManagerParam_X.StartStopTimeoutInMs_U32);
    }
  }
}

BofIoConnectionManager::~BofIoConnectionManager()
{
  BOF_UV_CONNECTION *pUvConnection_X;
  BOFERR Sts_E;
  bool Sts_B;

  std::string Dbg_S;
  Dbg_S = BuildConnectionInfo();
  DBG_OUT("Before while\n\n%s", Dbg_S.c_str());

  while (mUvDisconnectingConnectionCollection.size())
  {
    DBG_OUT("Wed1 %d\n", mUvDisconnectingConnectionCollection.size());
    BOF_NAMESPACE::Bof_Sleep(20);
  }

  DBG_OUT("StopUvLoop start\n");
  Sts_E = StopUvLoop();
  DBG_OUT("StopUvLoop stop\n");

  BOF_ASSERT(Sts_E == BOFERR_NO_ERROR);
  Sts_B = HasThreadExited(mBofIoConnectionManagerParam_X.StartStopTimeoutInMs_U32);
  BOF_ASSERT(Sts_B);

  Dbg_S = BuildConnectionInfo();
  DBG_OUT("Before Push\n\n%s", Dbg_S.c_str());

  LockBofIoConnectionManager("~BofIoConnectionManager1"); // We have time, we are shutdowning
  pUvConnection_X = mpUvConnectionCollection->GetFirstUsed(0);
  while (pUvConnection_X)
  {
    Sts_E = PushDisconnect("~BofIoConnectionManager::while", pUvConnection_X, true); // Need the loop for event signaling
    BOF_SAFE_DELETE(pUvConnection_X->pIBofIoConnection);
    pUvConnection_X = mpUvConnectionCollection->GetNextUsed(pUvConnection_X);
  }

  Dbg_S = BuildConnectionInfo();
  DBG_OUT("Before UnlockBofIoConnectionManager\n\n%s", Dbg_S.c_str());

  UnlockBofIoConnectionManager();
  //  goto k;
  while (mUvDisconnectingConnectionCollection.size())
  {
    DBG_OUT("Wed2 %d\n", mUvDisconnectingConnectionCollection.size());
    BOF_NAMESPACE::Bof_Sleep(20);
  }
  goto k;
  DBG_OUT("StopUvLoop start\n");
  Sts_E = StopUvLoop();
  DBG_OUT("StopUvLoop stop\n");

  BOF_ASSERT(Sts_E == BOFERR_NO_ERROR);
  Sts_B = HasThreadExited(mBofIoConnectionManagerParam_X.StartStopTimeoutInMs_U32);
  BOF_ASSERT(Sts_B);
k:
  BOF_SAFE_DELETE(mpBofIoAsyncCmdCollection);
  BOF_SAFE_DELETE(mpIoAsyncRequestCollection);
  BOF_SAFE_DELETE(mpUvConnectionCollection);
  Bof_DestroyMutex(mConnectionMtx);
}

BOFERR BofIoConnectionManager::PushDisconnect(const std::string &_rDbgName_S, BOF_UV_CONNECTION *_pUvConnection_X, bool _CancelPendingOp_B)
{
  BOFERR Rts_E = mpUvConnectionCollection->IsPotElementInUse(_pUvConnection_X);

  // DBG_OUT("[DISC] Disconnect pUv %p pCon %p %s of %s IoState %s Closing %s NbPend %d CancelPendingOp %d InUse %X\n", _pUvConnection_X, _pUvConnection_X ? _pUvConnection_X->pIBofIoConnection : nullptr, _rDbgName_S.c_str(), _pUvConnection_X ?
  // _pUvConnection_X->Name_S.c_str() : "???????",	_pUvConnection_X ? (_pUvConnection_X->pIBofIoConnection ? _pUvConnection_X->pIBofIoConnection->IoStateString().c_str() : "???????") : "???????", _pUvConnection_X ? _pUvConnection_X->Closing_B ? "True" :
  // "False" : "???", mUvDisconnectingConnectionCollection.size(), _CancelPendingOp_B, Rts_E ? 0 : 1);

  if (Rts_E == BOFERR_NO_ERROR)
  {
    // No as it can come from ~IBofIoConnection() which set pUvConnection_X->pIBofIoConnection to nullptr to signal Object is already deleted		BOF_ASSERT(_pUvConnection_X->pIBofIoConnection != nullptr);
    LockBofIoConnectionManager("PushDisconnect_" + _rDbgName_S); // Diconnect can come from V_OnUvTcpRead or IoCmdChannel::Disconnect for example
    if (_pUvConnection_X->Closing_B)
    {
      UnlockBofIoConnectionManager();
    }
    else
    {
      _pUvConnection_X->Closing_B = true;
      _pUvConnection_X->CancelPendingOp_B = _CancelPendingOp_B;
      mUvDisconnectingConnectionCollection.push_back(_pUvConnection_X);
      Rts_E = DisconnectUvStream("PushDisconnect", _pUvConnection_X);
      DBG_OUT("[DISC] pUv %p pushed for disc sts %d->NbPend %d\n", _pUvConnection_X, Rts_E, mUvDisconnectingConnectionCollection.size());
      BOF_ASSERT(Rts_E == BOFERR_NO_ERROR); // Need the loop for event signaling
      UnlockBofIoConnectionManager();
      Rts_E = BOFERR_NO_ERROR;
    }
  }
  else
  {
    // BOFERR_NOT_FOUND->It is already disconnected
    Rts_E = BOFERR_NOT_FOUND; // Signal to caller not to WaitForDisconnect
  }

  return Rts_E;
}

BOFERR BofIoConnectionManager::LockBofIoConnectionManager(const std::string &_rLockerName_S)
{
  BOFERR Rts_E = Bof_LockMutex(mConnectionMtx);
  if (Rts_E == BOFERR_NO_ERROR)
  {
    mLastLockerName_S = _rLockerName_S;
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::UnlockBofIoConnectionManager()
{
  return Bof_UnlockMutex(mConnectionMtx);
}

uint32_t BofIoConnectionManager::GetNbConnnection() const
{
  return mpUvConnectionCollection->GetNbElementOutOfThePot();
}

std::string BofIoConnectionManager::BuildConnectionInfo()
{
  std::string Rts_S;
  BOF_UV_CONNECTION *pUvConnection_X;
  uint32_t Index_U32, Delta_U32;
  bool IsUdp_B;

  LockBofIoConnectionManager("BuildConnectionInfo"); // We have time, we are debugging

  Rts_S += "Connection list\n";

  Index_U32 = 0;
  Rts_S += Bof_Sprintf("Ind Name                             pIBofCon pUvIoCon pUvIoChn State        Start    Delta  To        Udp   Rcv Buf  Snd Buf  Address\n");
  pUvConnection_X = mpUvConnectionCollection->GetFirstUsed(0);
  while (pUvConnection_X)
  {
    IsUdp_B = pUvConnection_X->pIBofIoConnection ? (pUvConnection_X->pIBofIoConnection->ProtocolType() == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP) : false;

    Delta_U32 = Bof_ElapsedMsTime(pUvConnection_X->StartConnectTime_U32);
    Rts_S += Bof_Sprintf("%03d %-32s %08p %08p %08p %-12s %08X %06d %06d %s %08X %08X %-42s ", Index_U32++, pUvConnection_X->Name_S.c_str(), pUvConnection_X->pIBofIoConnection, pUvConnection_X->pUvIoConnect, pUvConnection_X->pUvIoChannel,
                         pUvConnection_X->pIBofIoConnection ? pUvConnection_X->pIBofIoConnection->IoStateString().c_str() : "Nullptr", pUvConnection_X->StartConnectTime_U32, Delta_U32, pUvConnection_X->IoChannelParam_X.ConnectTimeoutInMs_U32,
                         IsUdp_B ? "true " : "false", pUvConnection_X->IoChannelParam_X.RcvBufferSize_U32, pUvConnection_X->IoChannelParam_X.SndBufferSize_U32, pUvConnection_X->IoChannelParam_X.Address_S.c_str());
    if (IsUdp_B)
    {
      Rts_S += Bof_Sprintf("%s %s %s %d\n", pUvConnection_X->IoChannelParam_X.IO_CHANNEL.UDP.Broadcast_B ? "true " : "false", pUvConnection_X->IoChannelParam_X.IO_CHANNEL.UDP.MulticastLoop_B ? "true " : "false",
                           pUvConnection_X->IoChannelParam_X.IO_CHANNEL.UDP.MulticastReceiver_B ? "true " : "false", pUvConnection_X->IoChannelParam_X.IO_CHANNEL.UDP.Ttl_U32);
    }
    else
    {
      Rts_S += Bof_Sprintf("%s\n", pUvConnection_X->IoChannelParam_X.IO_CHANNEL.TCP.NoDelay_B ? "true " : "false");
    }
    pUvConnection_X = mpUvConnectionCollection->GetNextUsed(pUvConnection_X);
  }

  Index_U32 = 0;
  Rts_S += Bof_Sprintf("\nInd Con   Sess  Prt Local                          Bind                           Dest                           Fact     Mgr      Error\n");
  pUvConnection_X = mpUvConnectionCollection->GetFirstUsed(0);
  while (pUvConnection_X)
  {
    if (pUvConnection_X->pIBofIoConnection)
    {
      if (pUvConnection_X->pIBofIoConnection->LastErrorCode() == 0xDDDDDDDD)
      {
        pUvConnection_X = pUvConnection_X;
      }
      Rts_S += Bof_Sprintf("%03d %s %s %s %-30s %-30s %-30s %08p %08p %s\n", Index_U32++, pUvConnection_X->pIBofIoConnection->IsConnected() ? "true " : "false", pUvConnection_X->pIBofIoConnection->ServerSession() ? "true " : "false",
                           Bof_ProtocolTypeToString(pUvConnection_X->pIBofIoConnection->ProtocolType()).c_str(), Bof_IpAddressToString(pUvConnection_X->pIBofIoConnection->LocalAddress(), true, true).c_str(),
                           Bof_IpAddressToString(pUvConnection_X->pIBofIoConnection->BindAddress(), true, true).c_str(), Bof_IpAddressToString(pUvConnection_X->pIBofIoConnection->DestinationAddress(), true, true).c_str(),
                           pUvConnection_X->pIBofIoConnection->IConnectionFactory(), pUvConnection_X->pIBofIoConnection->IoConnectionManager(), Bof_ErrorCode(pUvConnection_X->pIBofIoConnection->LastErrorCode()));
    }
    else
    {
      Rts_S += Bof_Sprintf("%03d NULL pointer\n", Index_U32++);
    }
    pUvConnection_X = mpUvConnectionCollection->GetNextUsed(pUvConnection_X);
  }

  Rts_S += Bof_Sprintf("\nConnecting list (%d entries)\n", mUvConnectingConnectionCollection.size());
  if (mUvConnectingConnectionCollection.size())
  {
    Index_U32 = 0;
    Rts_S += Bof_Sprintf("\nInd pIBofCon Name\n");
    for (auto It : mUvConnectingConnectionCollection)
    {
      Rts_S += Bof_Sprintf("%03d %08p %s\n", Index_U32++, It->pIBofIoConnection, It->Name_S.c_str());
    }
  }

  Rts_S += Bof_Sprintf("\nDisconnecting list (%d entries)\n", mUvDisconnectingConnectionCollection.size());
  if (mUvDisconnectingConnectionCollection.size())
  {
    Index_U32 = 0;
    Rts_S += Bof_Sprintf("\nInd pUv      Clsng pCon     IoState   Name\n");
    for (auto It : mUvDisconnectingConnectionCollection)
    {
      Rts_S += Bof_Sprintf("%03d %08p %s %08p %s %s\n", Index_U32++, It, It->Closing_B ? "True " : "False", It->pIBofIoConnection, It->pIBofIoConnection ? It->pIBofIoConnection->IoStateString().c_str() : "???????", It->Name_S.c_str());
    }
  }

  UnlockBofIoConnectionManager();

  return Rts_S;
}

/*
   BOF_UV_CONNECTION *BofIoConnectionManager::UvConnection(IBofIoConnection *_pIBofIoConnection)
   {
   BOF_UV_CONNECTION *pUvConnection_X, *pRts_X = nullptr;
   uint32_t Nb_U32;

   LockBofIoConnectionManager("UvConnection"); //We have no time.... a optimizer avec une map intermediaire ?
   pUvConnection_X = mpUvConnectionCollection->GetFirstUsed(0);
   Nb_U32 = 0;
   while (pUvConnection_X)
   {
    Nb_U32++;
    if (pUvConnection_X->pIBofIoConnection != nullptr)
    {
      BOF_ASSERT(pUvConnection_X->pIBofIoConnection != nullptr);
      if (pUvConnection_X->pIBofIoConnection == _pIBofIoConnection)
      {
        pRts_X = pUvConnection_X;
        break;
      }
    }
    pUvConnection_X = mpUvConnectionCollection->GetNextUsed(pUvConnection_X);
   }
   UnlockBofIoConnectionManager();
   return pRts_X;
   }
 */
bool BofIoConnectionManager::IsIBofIoConnectionAlreadyInTable(IBofIoConnection *_pIBofIoConnection)
{
  BOF_UV_CONNECTION *pUvConnection_X;
  bool Rts_B = false;

  LockBofIoConnectionManager("IsIBofIoConnectionAlreadyInTable"); // We have time as it is a BOF_ASSERT check function
  pUvConnection_X = mpUvConnectionCollection->GetFirstUsed(0);
  while (pUvConnection_X)
  {
    // Is null at this stage BOF_ASSERT(pUvConnection_X->pIBofIoConnection != nullptr);
    if (pUvConnection_X->pIBofIoConnection == _pIBofIoConnection)
    {
      Rts_B = true;
      break;
    }
    pUvConnection_X = mpUvConnectionCollection->GetNextUsed(pUvConnection_X);
  }
  UnlockBofIoConnectionManager();
  return Rts_B;
}

BOFERR BofIoConnectionManager::Connect(IBofIoConnection *_pIBofIoConnection, const BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X)
{
  BOFERR Rts_E = BOFERR_INVALID_PARAM, Sts_E;
  BOF_UV_CONNECTION *pUvConnection_X;

  if (_pIBofIoConnection)
  {
    LockBofIoConnectionManager("Connect1");
    GET_CONNECTION(pUvConnection_X, Rts_E);
    UnlockBofIoConnectionManager();
    if (Rts_E == BOFERR_NO_ERROR)
    {
      BOF_ASSERT(IsIBofIoConnectionAlreadyInTable(_pIBofIoConnection) == false);
      pUvConnection_X->pIBofIoConnection = _pIBofIoConnection;
      pUvConnection_X->pIBofIoConnection->UvConnection(pUvConnection_X);
      Rts_E = OpenUvConnection(pUvConnection_X->pIBofIoConnection, pUvConnection_X->pIBofIoConnection->Name() + "_con", 0, _rBofIoChannelParam_X, pUvConnection_X);
      if (Rts_E == BOFERR_NO_ERROR)
      {
        pUvConnection_X->pIBofIoConnection->Name(pUvConnection_X->Name_S);
      }
      else
      {
        LockBofIoConnectionManager("Connect2");
        RELEASE_CONNECTION(pUvConnection_X, Sts_E);
        UnlockBofIoConnectionManager();
      }
    }
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::Listen(IBofIoConnectionFactory *_pIBofIoConnectionFactory, const BOF_NAMESPACE::IBOF_IO_CONNECTION_PARAM &_rIBofIoConnectionParam_X, const BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X, uint32_t _NbMaxConnection_U32,
                                      IBofIoConnection **_ppIBofIoConnection)
{
  BOFERR Rts_E = BOFERR_INVALID_PARAM, Sts_E;
  BOF_UV_CONNECTION *pUvConnection_X;
  IBofIoConnection *pIBofIoConnection;

  LockBofIoConnectionManager("Listen1");
  GET_CONNECTION(pUvConnection_X, Rts_E);
  UnlockBofIoConnectionManager();
  if (Rts_E == BOFERR_NO_ERROR)
  {
    Rts_E = BOFERR_NOT_ENOUGH_RESOURCE;
    pIBofIoConnection = new BOF_NAMESPACE::IBofIoConnection(_rIBofIoConnectionParam_X, this);
    BOF_ASSERT(pIBofIoConnection != nullptr);
    BOF_ASSERT(IsIBofIoConnectionAlreadyInTable(pIBofIoConnection) == false);
    pUvConnection_X->pIBofIoConnection = pIBofIoConnection;
    pUvConnection_X->pIBofIoConnection->UvConnection(pUvConnection_X);
    pUvConnection_X->pIBofIoConnection->IConnectionFactory(_pIBofIoConnectionFactory);
    Rts_E = OpenUvConnection(pUvConnection_X->pIBofIoConnection, pUvConnection_X->pIBofIoConnection->Name() + "_lis", _NbMaxConnection_U32, _rBofIoChannelParam_X, pUvConnection_X);
    if (Rts_E == BOFERR_NO_ERROR)
    {
      pUvConnection_X->pIBofIoConnection->Name(pUvConnection_X->Name_S);
      if (_ppIBofIoConnection)
      {
        *_ppIBofIoConnection = pIBofIoConnection;
      }
    }
    if (Rts_E != BOFERR_NO_ERROR)
    {
      RELEASE_CONNECTION(pUvConnection_X, Sts_E);
    }
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::SimulateConnectionRequest(IBofIoConnectionFactory *_pIBofIoConnectionFactory, const std::string &_rName_S, IBofIoConnection **_ppIBofIoConnection)
{
  BOFERR Rts_E = BOFERR_WRONG_MODE, Sts_E;
  BOF_UV_CONNECTION *pUvConnection_X;
  BOF_IO_CHANNEL_INFO BofIoChannelInfo_X;
  IBofIoConnection *pIBofIoConnection;

  if (_pIBofIoConnectionFactory)
  {
    Rts_E = BOFERR_INVALID_PARAM;
    if (_ppIBofIoConnection)
    {
      LockBofIoConnectionManager("SimulateConnectionRequest1");
      GET_CONNECTION(pUvConnection_X, Rts_E);
      UnlockBofIoConnectionManager();
      if (Rts_E == BOFERR_NO_ERROR)
      {
        BofIoChannelInfo_X.Name_S = "Simulator";
        BofIoChannelInfo_X.ProtocolType_E = BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP;
        pIBofIoConnection = _pIBofIoConnectionFactory->V_CreateSession(BofIoChannelInfo_X, this, pUvConnection_X->IoChannelParam_X);
        BOF_ASSERT(IsIBofIoConnectionAlreadyInTable(pIBofIoConnection) == false);
        pUvConnection_X->pIBofIoConnection = pIBofIoConnection;
        pUvConnection_X->pIBofIoConnection->UvConnection(pUvConnection_X);
        pUvConnection_X->Name_S = "sim_" + _rName_S;

        Rts_E = BOFERR_CREATE;
        if (pUvConnection_X->pIBofIoConnection)
        {
          pUvConnection_X->pIBofIoConnection->Name(pUvConnection_X->Name_S);
          pUvConnection_X->pIBofIoConnection->ServerSession(true);
          pUvConnection_X->pIBofIoConnection->IoState(BOF_IO_STATE::CONNECTED);
          Rts_E = BOFERR_NO_ERROR;
        }
        if (Rts_E != BOFERR_NO_ERROR)
        {
          LockBofIoConnectionManager("SimulateConnectionRequest2");
          RELEASE_CONNECTION(pUvConnection_X, Sts_E);
          UnlockBofIoConnectionManager();
        }
      }
    }
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::SimulateDisconnectionRequest(IBofIoConnectionFactory *_pIBofIoConnectionFactory, IBofIoConnection *_pIBofIoConnection)
{
  BOFERR Rts_E = BOFERR_WRONG_MODE, Sts_E;
  BOF_UV_CONNECTION *pUvConnection_X;

  if ((_pIBofIoConnectionFactory) && (_pIBofIoConnection))
  {
    Rts_E = BOFERR_NOT_FOUND;
    pUvConnection_X = _pIBofIoConnection->UvConnection();
    if (pUvConnection_X)
    {
      BOF_ASSERT(pUvConnection_X->pIBofIoConnection != nullptr);
      BOF_ASSERT(pUvConnection_X->pIBofIoConnection == _pIBofIoConnection);
      pUvConnection_X->pIBofIoConnection->IoState(BOF_IO_STATE::DISCONNECTED);
      Rts_E = _pIBofIoConnectionFactory->V_SessionDisconnected(pUvConnection_X->pIBofIoConnection);

      RELEASE_CONNECTION(pUvConnection_X, Sts_E);
    }
  }
  return Rts_E;
}

std::string BofIoConnectionManager::UvWalk()
{
  mUvWalkResult_S = "";
  mUvWalkHandleCollection.clear();
  uv_walk(mpuLoop_X.get(), S_OnUvWalkCallback, this);

  // BofIo *pBofIo = dynamic_cast<BofIo *>(this);
  // pBofIo->V_OnUvWalk(nullptr, nullptr);

  return mUvWalkResult_S;
}

const BOF_IO_STATISTICS &BofIoConnectionManager::IoStatistic()
{
  return mIoStatistic_X;
}

void BofIoConnectionManager::ResetIoStatistic()
{
  mIoStatistic_X.Reset();
}

BOFERR BofIoConnectionManager::LastErrorCode()
{
  return mErrorCode_E;
}

BOFERR BofIoConnectionManager::SendAsyncCmd(bool _Emergency_B, BOF_IO_ASYNC_CMD &_rBofIoAsyncCmd_X)
{
  BOFERR Rts_E;

  _rBofIoAsyncCmd_X.Ticket_U32 = ++S_mCmdTicket_U32;
  if (_Emergency_B)
  {
    Rts_E = mpBofIoAsyncCmdCollection->InsertAsFirst(&_rBofIoAsyncCmd_X, true, 0);
  }
  else
  {
    Rts_E = mpBofIoAsyncCmdCollection->Push(&_rBofIoAsyncCmd_X, DEFAULT_ASYNC_PUSH_TIMEOUT);
  }
  if (Rts_E == BOFERR_NO_ERROR)
  {

    Rts_E = (uv_async_send(mpuAsync_X.get()) == 0) ? BOFERR_NO_ERROR : BOFERR_WRITE;
    BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);
  }
  DBG_OUT("[DISC1] SendAsyncCmd emer %d cmd %d caller %s pUv %p pCon %p %s IoState %s Closing %s NbPend %d Rts %X\n", _Emergency_B, _rBofIoAsyncCmd_X.Cmd_E, _rBofIoAsyncCmd_X.Caller_S.c_str(), _rBofIoAsyncCmd_X.pUvConnection_X,
          _rBofIoAsyncCmd_X.pUvConnection_X ? _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection : nullptr, _rBofIoAsyncCmd_X.pUvConnection_X ? _rBofIoAsyncCmd_X.pUvConnection_X->Name_S.c_str() : "???????",
          _rBofIoAsyncCmd_X.pUvConnection_X ? (_rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection ? _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection->IoStateString().c_str() : "???????") : "???????",
          _rBofIoAsyncCmd_X.pUvConnection_X ? _rBofIoAsyncCmd_X.pUvConnection_X->Closing_B ? "True" : "False" : "???", mUvDisconnectingConnectionCollection.size(), Rts_E);

  return Rts_E;
}

// Passive mode if _NbMaxConnection_U32 is different from 0
BOFERR BofIoConnectionManager::OpenUvConnection(IBofIoConnection *_pIBofIoConnection, const std::string &_rName_S, uint32_t _NbMaxConnection_U32, const BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X, BOF_UV_CONNECTION *_pUvConnection_X)
{
  BOFERR Rts_E = BOFERR_INVALID_PARAM;
  BOF_SOCKET_ADDRESS_COMPONENT InterfaceIpAddressComponent_X, IpAddressComponent_X;
  int Sts_i;
  uint32_t Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32;
  struct sockaddr_in IpAddress_X;
  BOF_SOCKET_ADDRESS SocketAddress_X;
  BOF_UV_CALLBACK *pBofUvCallback_X;
  BOF_PROTOCOL_TYPE BindProtocolType_E;
  bool IsUdp_B = false;

  BOF_ASSERT(_pIBofIoConnection != nullptr);
  if (_pUvConnection_X)
  {
    BindProtocolType_E = BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNDEF;
    _pUvConnection_X->pIBofIoConnection = _pIBofIoConnection;
    _pUvConnection_X->pIBofIoConnection->UvConnection(_pUvConnection_X);
    _pUvConnection_X->pIBofIoConnection->IoState(BOF_IO_STATE::IDLE);
    _pUvConnection_X->IoChannelParam_X = _rBofIoChannelParam_X;
    // _pUvConnection_X->TimeoutInMs_U32 = (_rBofIoConnectParam_X.Timeout_U32 >= LOOP_TIMER_GRANULARITY) ? _rBofIoConnectParam_X.Timeout_U32 : LOOP_TIMER_GRANULARITY;
    _pUvConnection_X->StartConnectTime_U32 = Bof_GetMsTickCount();
    BOF_ASSERT(_pUvConnection_X->pIBofIoConnection != nullptr);
    Rts_E = Bof_SplitUrlAddress(_rBofIoChannelParam_X.Address_S, InterfaceIpAddressComponent_X, IpAddressComponent_X);
    if (Rts_E == BOFERR_NO_ERROR)
    {
      Rts_E = BOFERR_BAD_PROTOCOL;
      BindProtocolType_E = Bof_ProtocolType(InterfaceIpAddressComponent_X.Protocol_S);
      if ((BindProtocolType_E != BOF_PROTOCOL_TYPE::BOF_PROTOCOL_SHARED_RAM) && (BindProtocolType_E != BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNDEF) && (BindProtocolType_E != BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN) &&
          (BindProtocolType_E != BOF_PROTOCOL_TYPE::BOF_PROTOCOL_ZMQ_PGM) && (BindProtocolType_E != BOF_PROTOCOL_TYPE::BOF_PROTOCOL_MAX))
      {
        Rts_E = BOFERR_BAD_ADDRESS;
        Sts_i = uv_ip4_addr(InterfaceIpAddressComponent_X.IpAddress_S.c_str(), InterfaceIpAddressComponent_X.Port_U16, &IpAddress_X);
        if (Sts_i == 0)
        {
          IsUdp_B = (BindProtocolType_E == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP);
          BOF_U32IPADDR_TO_U8IPADDR(IpAddress_X.sin_addr.s_addr, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32);
          SocketAddress_X.Set(false, IsUdp_B ? BOF_SOCK_TYPE::BOF_SOCK_UDP : BOF_SOCK_TYPE::BOF_SOCK_TCP, IsUdp_B ? BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP : BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, ntohs(IpAddress_X.sin_port));
          _pUvConnection_X->pIBofIoConnection->BindAddress(SocketAddress_X);
          Rts_E = BOFERR_NO_ERROR;
        }
      }
    }
    if (Rts_E == BOFERR_NO_ERROR)
    {
      Rts_E = BOFERR_BAD_PROTOCOL;
      _pUvConnection_X->pIBofIoConnection->ProtocolType(Bof_ProtocolType(IpAddressComponent_X.Protocol_S));
      if (_pUvConnection_X->pIBofIoConnection->ProtocolType() == BindProtocolType_E)
      {
        Rts_E = BOFERR_BAD_ADDRESS;
        Sts_i = uv_ip4_addr(IpAddressComponent_X.IpAddress_S.c_str(), IpAddressComponent_X.Port_U16, &IpAddress_X);
        if (Sts_i == 0)
        {
          IsUdp_B = (_pUvConnection_X->pIBofIoConnection->ProtocolType() == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP);
          BOF_U32IPADDR_TO_U8IPADDR(IpAddress_X.sin_addr.s_addr, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32);
          SocketAddress_X.Set(false, IsUdp_B ? BOF_SOCK_TYPE::BOF_SOCK_UDP : BOF_SOCK_TYPE::BOF_SOCK_TCP, IsUdp_B ? BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP : BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, ntohs(IpAddress_X.sin_port));
          _pUvConnection_X->pIBofIoConnection->DestinationAddress(SocketAddress_X);
          Rts_E = BOFERR_NO_ERROR;
        }
      }
    }
    if (Rts_E == BOFERR_NO_ERROR)
    {
      if (_NbMaxConnection_U32) // _PassiveMode_B)
      {
        _pUvConnection_X->Name_S = _rName_S + "_" + std::to_string(++S_mListenId_U32);

        _pUvConnection_X->pUvIoConnect = nullptr;
        _pUvConnection_X->pUvIoChannel = IsUdp_B ? reinterpret_cast<void *>(new uv_udp_t) : reinterpret_cast<void *>(new uv_tcp_t);
        BOF_ASSERT(_pUvConnection_X->pUvIoChannel != nullptr);
        ALLOC_UV_CALLBACK_DATA(_pUvConnection_X->pUvIoChannel, pBofUvCallback_X, _pUvConnection_X->Name_S, true, _pUvConnection_X->pIBofIoConnection, nullptr, _pUvConnection_X);

        // _pUvConnection_X->TimeoutInMs_U32 = (_rBofIoConnectParam_X.Timeout_U32 >= LOOP_TIMER_GRANULARITY) ? _rBofIoConnectParam_X.Timeout_U32 : LOOP_TIMER_GRANULARITY;
        // LockBofIoConnectionManager("OpenUvConnection1");
        _pUvConnection_X->pIBofIoConnection->IoChannelParam(_rBofIoChannelParam_X);
        _pUvConnection_X->pIBofIoConnection->IoState(BOF_IO_STATE::LISTENING); // In this case the connection is deleted by the lib
                                                                               // UnlockBofIoConnectionManager();
      }
      else
      {
        _pUvConnection_X->Name_S = _rName_S + "_" + std::to_string(++S_mClientId_U32);
        if (IsUdp_B)
        {
          _pUvConnection_X->pUvIoConnect = nullptr;
        }
        else
        {
          _pUvConnection_X->pUvIoConnect = new uv_connect_t;
          BOF_ASSERT(_pUvConnection_X->pUvIoConnect != nullptr);
          ALLOC_UV_CALLBACK_DATA(_pUvConnection_X->pUvIoConnect, pBofUvCallback_X, _pUvConnection_X->Name_S, true, _pUvConnection_X->pIBofIoConnection, nullptr, _pUvConnection_X);
        }

        _pUvConnection_X->pUvIoChannel = IsUdp_B ? reinterpret_cast<void *>(new uv_udp_t) : reinterpret_cast<void *>(new uv_tcp_t);
        BOF_ASSERT(_pUvConnection_X->pUvIoChannel != nullptr);
        ALLOC_UV_CALLBACK_DATA(_pUvConnection_X->pUvIoChannel, pBofUvCallback_X, _pUvConnection_X->Name_S, true, _pUvConnection_X->pIBofIoConnection, nullptr, _pUvConnection_X);

        _pUvConnection_X->pIBofIoConnection->IoChannelParam(_rBofIoChannelParam_X);
        _pUvConnection_X->pIBofIoConnection->IoState(BOF_IO_STATE::CONNECTING);

        LockBofIoConnectionManager("OpenUvConnection2");
        mUvConnectingConnectionCollection.push_back(_pUvConnection_X);
        UnlockBofIoConnectionManager();

        Rts_E = BOFERR_NO_ERROR;
      }
      if (Rts_E == BOFERR_NO_ERROR)
      {
        Rts_E = ConnectUvStream(_pUvConnection_X, _NbMaxConnection_U32);
        if ((Rts_E != BOFERR_NO_ERROR) && (_NbMaxConnection_U32 == 0))
        {
          LockBofIoConnectionManager("OpenUvConnection3");
          auto It = std::find(mUvConnectingConnectionCollection.begin(), mUvConnectingConnectionCollection.end(), _pUvConnection_X);
          BOF_ASSERT(It != mUvConnectingConnectionCollection.end());
          if (It != mUvConnectingConnectionCollection.end())
          {
            mUvConnectingConnectionCollection.erase(It);
          }
          UnlockBofIoConnectionManager();
        }
      }
    }
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::StopUvLoop()
{
  BOFERR Rts_E;
  BOF_IO_ASYNC_CMD _rBofIoAsyncCmd_X;

  _rBofIoAsyncCmd_X.Cmd_E = ASYNC_CMD::ASYNC_CMD_EXIT;
  Rts_E = SendAsyncCmd(true, _rBofIoAsyncCmd_X);
  return Rts_E;
}

BOFERR BofIoConnectionManager::ConnectUvStream(BOF_UV_CONNECTION *_pUvConnection_X, uint32_t _NbMaxConnection_U32)
{
  BOFERR Rts_E = BOFERR_INVALID_PARAM;
  BOF_IO_ASYNC_CMD _rBofIoAsyncCmd_X;

  if (_pUvConnection_X)
  {
    _rBofIoAsyncCmd_X.Cmd_E = ASYNC_CMD::ASYNC_CMD_CONNECT;
    _rBofIoAsyncCmd_X.NbMaxConnection_U32 = _NbMaxConnection_U32; // PassiveMode_B if != 0
    _rBofIoAsyncCmd_X.pUvConnection_X = _pUvConnection_X;
    Rts_E = SendAsyncCmd(false, _rBofIoAsyncCmd_X);
  }
  return Rts_E;
}

// Will trigger BofIoConnectionManager::V_OnUvClose by calling CLOSE_UV_RESOURCE in the ASYNC_CMD::ASYNC_CMD_DISCONNECT cmd
BOFERR BofIoConnectionManager::DisconnectUvStream(const std::string &_rCaller_S, BOF_UV_CONNECTION *_pUvConnection_X)
{
  BOFERR Rts_E;
  BOF_IO_ASYNC_CMD BofIoAsyncCmd_X;

  BofIoAsyncCmd_X.Caller_S = _rCaller_S;
  BofIoAsyncCmd_X.Cmd_E = ASYNC_CMD::ASYNC_CMD_DISCONNECT;
  BofIoAsyncCmd_X.pUvConnection_X = _pUvConnection_X;
  Rts_E = SendAsyncCmd(false, BofIoAsyncCmd_X);
  BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);
  return Rts_E;
}

BOFERR BofIoConnectionManager::ReleaseUvLoopResource()
{
  BOFERR Rts_E;
  int Sts_i;
  uint32_t i_U32;

  Sts_i = uv_timer_stop(mpuTimer_X.get());
  BOF_ASSERT(Sts_i == 0);

  UvWalk();
  Rts_E = BOFERR_NO_ERROR;

  for (i_U32 = 0; i_U32 < mUvWalkHandleCollection.size(); i_U32++)
  {
    CLOSE_UV_RESOURCE(mUvWalkHandleCollection[i_U32]); // , nullptr);		//This will cancel uv_connect_t / pending uv_tcp_connect
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::OnUvAsyncWrite(const BOF_IO_ASYNC_CMD &_rBofIoAsyncCmd_X)
{
  BOFERR Rts_E = BOFERR_INTERNAL;
  BOF_IO_ASYNC_REQ *pBofIoAsyncRequest_X;
  BOF_UV_CALLBACK *pUvCallback_X, *pBofUvCallback_X;
  int Sts_i;
  uint32_t i_U32;
  uv_udp_t *pUvUdp_X;
  uv_stream_t *pUvStream_X;
  uv_write_t *pTcpWrtReq_X;
  uv_udp_send_t *pUdpWrtReq_X;
  const struct sockaddr *pIpAddress_X;
  BOF_SOCKET_ADDRESS SocketAddress_X;

  BOF_ASSERT(_rBofIoAsyncCmd_X.pUvConnection_X != nullptr);
  if (_rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection == nullptr)
  {
    i_U32 = 0;
  }

  BOF_ASSERT(_rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection != nullptr);
  bool IsUdp_B = (_rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection->ProtocolType() == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP);

  Rts_E = BOFERR_TOO_LARGE;

  char p[256];
  int i;
  if (_rBofIoAsyncCmd_X.pBuffer_X[0].pData_U8)
  {
    i = (_rBofIoAsyncCmd_X.pBuffer_X[0].SizeInByte_U32 < sizeof(p)) ? _rBofIoAsyncCmd_X.pBuffer_X[0].SizeInByte_U32 : 16;
    strncpy(p, (char *)_rBofIoAsyncCmd_X.pBuffer_X[0].pData_U8, i);
    p[i] = 0;
  }
  else
  {
    strcpy(p, "null");
  }

  DBG_OUT("[WRT1] pUv %p pCon %p %s IoState %s Closing %s NbPend %d Data %d:%s Nbbufval %d\n", _rBofIoAsyncCmd_X.pUvConnection_X, _rBofIoAsyncCmd_X.pUvConnection_X ? _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection : nullptr,
          _rBofIoAsyncCmd_X.pUvConnection_X ? _rBofIoAsyncCmd_X.pUvConnection_X->Name_S.c_str() : "???????",
          _rBofIoAsyncCmd_X.pUvConnection_X ? (_rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection ? _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection->IoStateString().c_str() : "???????") : "???????",
          _rBofIoAsyncCmd_X.pUvConnection_X ? _rBofIoAsyncCmd_X.pUvConnection_X->Closing_B ? "True" : "False" : "???", mUvDisconnectingConnectionCollection.size(), _rBofIoAsyncCmd_X.pBuffer_X[0].SizeInByte_U32, p, _rBofIoAsyncCmd_X.NbBufferValid_U32);

  if (_rBofIoAsyncCmd_X.NbBufferValid_U32 <= NB_MAX_WRITE_BUFFER_PER_OP)
  {
    Rts_E = BOFERR_NOT_ENOUGH_RESOURCE;
    pBofIoAsyncRequest_X = mpIoAsyncRequestCollection->Get(0);
    DBG_OUT("[WRT2] pUv %p pBofIoAsyncRequest_X %p capa %d/%d\n", _rBofIoAsyncCmd_X.pUvConnection_X, pBofIoAsyncRequest_X, mpIoAsyncRequestCollection->GetNbElementOutOfThePot(), mpIoAsyncRequestCollection->GetCapacity());

    if (pBofIoAsyncRequest_X)
    {
      pBofIoAsyncRequest_X->NbUvBufferValid_U32 = _rBofIoAsyncCmd_X.NbBufferValid_U32;
      for (i_U32 = 0; i_U32 < pBofIoAsyncRequest_X->NbUvBufferValid_U32; i_U32++)
      {
        pBofIoAsyncRequest_X->pUvBufferMustBeDeleted_B[i_U32] = _rBofIoAsyncCmd_X.pBuffer_X[i_U32].MustBeDeleted_B;
        pBofIoAsyncRequest_X->pUvBuffer_X[i_U32].base = reinterpret_cast<char *>(_rBofIoAsyncCmd_X.pBuffer_X[i_U32].pData_U8);
        pBofIoAsyncRequest_X->pUvBuffer_X[i_U32].len = _rBofIoAsyncCmd_X.pBuffer_X[i_U32].SizeInByte_U32;
      }
      pBofIoAsyncRequest_X->Ticket_U32 = ++S_mReqTicket_U32;
      pBofIoAsyncRequest_X->Cmd_E = _rBofIoAsyncCmd_X.Cmd_E;
      pBofIoAsyncRequest_X->pUvConnection_X = _rBofIoAsyncCmd_X.pUvConnection_X;
      pBofIoAsyncRequest_X->SignalEndOfIo_B = _rBofIoAsyncCmd_X.SignalEndOfIo_B;
      pBofIoAsyncRequest_X->pUserArg = _rBofIoAsyncCmd_X.pUserArg;
      pBofIoAsyncRequest_X->StartTimeInMs_U32 = Bof_GetMsTickCount();
      pBofIoAsyncRequest_X->TimeoutInMs_U32 = _rBofIoAsyncCmd_X.TimeoutInMs_U32;
      DBG_OUT("[WRT3] pUv %p IsUdp_B %d\n", _rBofIoAsyncCmd_X.pUvConnection_X, IsUdp_B);

      if (IsUdp_B)
      {
        pUvUdp_X = reinterpret_cast<uv_udp_t *>(_rBofIoAsyncCmd_X.pUvConnection_X->pUvIoChannel);
        pUvCallback_X = reinterpret_cast<BOF_UV_CALLBACK *>(pUvUdp_X->data);
        BOF_ASSERT(pUvCallback_X != nullptr);
        ALLOC_UV_CALLBACK_DATA(&pBofIoAsyncRequest_X->UdpWrtReq_X, pBofUvCallback_X, (std::string("UdpWrtReq_") + pUvCallback_X->Name_S), false, _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection, pBofIoAsyncRequest_X, _rBofIoAsyncCmd_X.pUvConnection_X);
        SocketAddress_X = _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection->DestinationAddress();
        pIpAddress_X = reinterpret_cast<const struct sockaddr *>(&SocketAddress_X.IpV4Address_X);
        Sts_i = uv_udp_send(&pBofIoAsyncRequest_X->UdpWrtReq_X, pUvUdp_X, pBofIoAsyncRequest_X->pUvBuffer_X, pBofIoAsyncRequest_X->NbUvBufferValid_U32, pIpAddress_X, S_OnUvWriteUdpCallback);
      }
      else
      {
        pUvStream_X = reinterpret_cast<uv_stream_t *>(_rBofIoAsyncCmd_X.pUvConnection_X->pUvIoChannel);
        pUvCallback_X = reinterpret_cast<BOF_UV_CALLBACK *>(pUvStream_X->data);
        BOF_ASSERT(pUvCallback_X != nullptr);
        ALLOC_UV_CALLBACK_DATA(&pBofIoAsyncRequest_X->TcpWrtReq_X, pBofUvCallback_X, (std::string("TcpWrtReq_") + pUvCallback_X->Name_S), false, _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection, pBofIoAsyncRequest_X, _rBofIoAsyncCmd_X.pUvConnection_X);
        Sts_i = uv_write(&pBofIoAsyncRequest_X->TcpWrtReq_X, pUvStream_X, pBofIoAsyncRequest_X->pUvBuffer_X, pBofIoAsyncRequest_X->NbUvBufferValid_U32, S_OnUvWriteTcpCallback);
        DBG_OUT("[WRT4] pUv %p uv_write %d\n", _rBofIoAsyncCmd_X.pUvConnection_X, Sts_i);
      }

      if (Sts_i == 0)
      {
        Rts_E = BOFERR_NO_ERROR;
      }
      else
      {
        Rts_E = BOFERR_WRITE;
        if (IsUdp_B)
        {
          pUdpWrtReq_X = &pBofIoAsyncRequest_X->UdpWrtReq_X;
          FREE_UV_CALLBACK_DATA(pUdpWrtReq_X);
        }
        else
        {
          pTcpWrtReq_X = &pBofIoAsyncRequest_X->TcpWrtReq_X;
          FREE_UV_CALLBACK_DATA(pTcpWrtReq_X);
        }
        mpIoAsyncRequestCollection->Release(pBofIoAsyncRequest_X); // Return to pot
      }
    } // if (pBofIoAsyncRequest_X)
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::OnUvAsyncConnect(BOF_IO_ASYNC_CMD &_rBofIoAsyncCmd_X)
{
  BOFERR Rts_E;
  bool IsUdp_B;
  BOF_SOCKET_ADDRESS SocketAddress_X;
  BOF_UV_CALLBACK BofUvCallback_X, pBofUvCallback_X;
  const struct sockaddr *pIpAddress_X;
  int Sts_i;

  BOF_ASSERT(_rBofIoAsyncCmd_X.pUvConnection_X != nullptr);
  BOF_ASSERT(_rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection != nullptr);
  IsUdp_B = (_rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection->ProtocolType() == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP);

  if (IsUdp_B)
  {
    uv_udp_t *pUvUdp_X = reinterpret_cast<uv_udp_t *>(_rBofIoAsyncCmd_X.pUvConnection_X->pUvIoChannel);

    Rts_E = (uv_udp_init(mpuLoop_X.get(), pUvUdp_X) == 0) ? BOFERR_NO_ERROR : BOFERR_INIT;
    BOF_ASSERT(Rts_E == 0);

    if (Rts_E == BOFERR_NO_ERROR)
    {
      SocketAddress_X = _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection->BindAddress();
      pIpAddress_X = reinterpret_cast<const struct sockaddr *>(&SocketAddress_X.IpV4Address_X);
      Rts_E = (uv_udp_bind(pUvUdp_X, pIpAddress_X, UV_UDP_REUSEADDR) == 0) ? BOFERR_NO_ERROR : BOFERR_BIND;
      BOF_ASSERT(Rts_E == 0);

      BofUvCallback_X.Name_S = "OnUvAsyncConnect_Udp";
      BofUvCallback_X.pBofIoConnectionManager = this;
      BofUvCallback_X.pBofIoAsyncRequest_X = _rBofIoAsyncCmd_X.pBofIoAsyncRequest_X;
      BofUvCallback_X.pUvConnection_X = _rBofIoAsyncCmd_X.pUvConnection_X;
      if (BofUvCallback_X.pUvConnection_X)
      {
        BofUvCallback_X.pUvConnection_X->pIBofIoConnection = _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection;
      }
      BofUvCallback_X.MustDeleteUvObj_B = true;
      if (_rBofIoAsyncCmd_X.NbMaxConnection_U32 == 0)
      {
        Rts_E = V_OnUvConnect(&BofUvCallback_X, nullptr, 0);
      }
      else
      {
        Rts_E = V_OnUvConnection(&BofUvCallback_X, nullptr, 0, _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection, pUvUdp_X);
      }
    }
  }
  else
  {
    uv_connect_t *pUvConnect_X = reinterpret_cast<uv_connect_t *>(_rBofIoAsyncCmd_X.pUvConnection_X->pUvIoConnect);
    uv_tcp_t *pUvTcp_X = reinterpret_cast<uv_tcp_t *>(_rBofIoAsyncCmd_X.pUvConnection_X->pUvIoChannel);
    uv_stream_t *pUvStream_X = reinterpret_cast<uv_stream_t *>(_rBofIoAsyncCmd_X.pUvConnection_X->pUvIoChannel);

    Rts_E = (uv_tcp_init(mpuLoop_X.get(), pUvTcp_X) == 0) ? BOFERR_NO_ERROR : BOFERR_INIT;
    BOF_ASSERT(Rts_E == 0);

    if (Rts_E == BOFERR_NO_ERROR)
    {
      SocketAddress_X = _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection->BindAddress();
      pIpAddress_X = reinterpret_cast<const struct sockaddr *>(&SocketAddress_X.IpV4Address_X);
      Sts_i = uv_tcp_bind(pUvTcp_X, pIpAddress_X, 0);
      Rts_E = (Sts_i == 0) ? BOFERR_NO_ERROR : BOFERR_BIND;
      BOF_ASSERT(Rts_E == 0);
      if (Rts_E == BOFERR_NO_ERROR)
      {
        if ((pUvConnect_X) && (_rBofIoAsyncCmd_X.NbMaxConnection_U32 == 0))
        {
          SocketAddress_X = _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection->DestinationAddress();
          pIpAddress_X = reinterpret_cast<const struct sockaddr *>(&SocketAddress_X.IpV4Address_X);
          Rts_E = (uv_tcp_connect(pUvConnect_X, pUvTcp_X, pIpAddress_X, BofIoConnectionManager::S_OnUvConnectCallback) == 0) ? BOFERR_NO_ERROR : BOFERR_CONNECT;
        }
        else
        {
          uint32_t Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32;
          struct sockaddr SockName_X;
          const struct sockaddr_in *pIpAddress_X;
          int NameLen_i = sizeof(SockName_X);
          int Sts_i = uv_tcp_getsockname(pUvTcp_X, &SockName_X, &NameLen_i);
          BOF_ASSERT(Sts_i == 0);

          pIpAddress_X = reinterpret_cast<struct sockaddr_in *>(&SockName_X);
          BOF_U32IPADDR_TO_U8IPADDR(pIpAddress_X->sin_addr.s_addr, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32);
          SocketAddress_X.Set(false, BOF_SOCK_TYPE::BOF_SOCK_TCP, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, ntohs(pIpAddress_X->sin_port));
          _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection->LocalAddress(SocketAddress_X);

          Rts_E = (uv_listen(pUvStream_X, _rBofIoAsyncCmd_X.NbMaxConnection_U32, S_OnUvConnectionCallback) == 0) ? BOFERR_NO_ERROR : BOFERR_DISCONNECTED;
        }
      }
    }
  }
  if (Rts_E != BOFERR_NO_ERROR)
  {
    // CONNECT_UV_FAILED will fire V_ConnectFailed in V_OnUvConnect uv callback    _rBofIoAsyncCmd_X.pIBofIoConnection->V_ConnectFailed(Rts_E);
    CONNECT_UV_FAILED(_rBofIoAsyncCmd_X.pUvConnection_X->pUvIoChannel, _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection, Rts_E);
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::OnUvAsyncDisconnect(const BOF_IO_ASYNC_CMD &_rBofIoAsyncCmd_X)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  uv_shutdown_t *pShutdown_X;
  int Sts_i;
  BOF_IO_ASYNC_REQ *pBofIoAsyncRequest_X;
  BOF_UV_CALLBACK *pUvCallback_X, *pBofUvCallback_X;
  uv_tcp_t *pUvTcp_X;
  uv_udp_t *pUvUdp_X;
  uv_stream_t *pUvStream_X;
  uv_connect_t *pUvConnect_X;
  BOF_SOCKET_ADDRESS_COMPONENT InterfaceIpAddressComponent_X, IpAddressComponent_X;
  bool IsUdp_B;

  BOF_ASSERT(_rBofIoAsyncCmd_X.pUvConnection_X);
  if (_rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection) ////DBG_OUT("ProcessDisconnect->Object %s is already deleted\n", pUvConnection_X->Name_S.c_str());
  {
    IsUdp_B = (_rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection->ProtocolType() == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP);
  }
  else
  {
    IsUdp_B = false;
  }
  if (IsUdp_B)
  {
    pUvUdp_X = reinterpret_cast<uv_udp_t *>(_rBofIoAsyncCmd_X.pUvConnection_X->pUvIoChannel);
    pUvConnect_X = nullptr;
    pUvTcp_X = nullptr;
    pUvStream_X = nullptr;
  }
  else
  {
    pUvUdp_X = nullptr;
    pUvConnect_X = reinterpret_cast<uv_connect_t *>(_rBofIoAsyncCmd_X.pUvConnection_X->pUvIoConnect);
    pUvTcp_X = reinterpret_cast<uv_tcp_t *>(_rBofIoAsyncCmd_X.pUvConnection_X->pUvIoChannel);
    pUvStream_X = reinterpret_cast<uv_stream_t *>(_rBofIoAsyncCmd_X.pUvConnection_X->pUvIoChannel);
  }
  DBG_OUT("[DISC3] OnUvAsyncDisconnect cmd %d caller %s pUv %p pCon %p %s IoState %s Closing %s NbPend %d\n", _rBofIoAsyncCmd_X.Cmd_E, _rBofIoAsyncCmd_X.Caller_S.c_str(), _rBofIoAsyncCmd_X.pUvConnection_X,
          _rBofIoAsyncCmd_X.pUvConnection_X ? _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection : nullptr, _rBofIoAsyncCmd_X.pUvConnection_X ? _rBofIoAsyncCmd_X.pUvConnection_X->Name_S.c_str() : "???????",
          _rBofIoAsyncCmd_X.pUvConnection_X ? (_rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection ? _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection->IoStateString().c_str() : "???????") : "???????",
          _rBofIoAsyncCmd_X.pUvConnection_X ? _rBofIoAsyncCmd_X.pUvConnection_X->Closing_B ? "True" : "False" : "???", mUvDisconnectingConnectionCollection.size());

  DBG_OUT("[DISC4] OnUvAsyncDisconnect strm %p pconnect %p puvtcp %p cancel %d iswrt %d qs %d\n", pUvStream_X, pUvConnect_X, pUvTcp_X, _rBofIoAsyncCmd_X.pUvConnection_X->CancelPendingOp_B, uv_is_writable(pUvStream_X), pUvStream_X->write_queue_size);

  DISPLAY_UV_CALLBACK_DATA(pUvStream_X, "pUvStream_X");
  if (pUvStream_X)
  {
    Sts_i = uv_read_stop(pUvStream_X);
    BOF_ASSERT(Sts_i == 0);
  }
  DISPLAY_UV_CALLBACK_DATA(pUvConnect_X, "pUvConnect_X");
  DBG_OUT("[DISC5] OnUvAsyncDisconnect strm %p pconnect %p puvtcp %p cancel %d iswrt %d qs %d\n", pUvStream_X, pUvConnect_X, pUvTcp_X, _rBofIoAsyncCmd_X.pUvConnection_X->CancelPendingOp_B, uv_is_writable(pUvStream_X), pUvStream_X->write_queue_size);
  if (pUvConnect_X)
  {
    FREE_UV_CALLBACK_DATA(pUvConnect_X);
  }
  // DBG_OUT("### OnUvAsyncDisconnect %p canc %d iswrt %d sz %d\n", pUvTcp_X, _rBofIoAsyncCmd_X.CancelPendingOp_B, uv_is_writable(pUvStream_X), pUvTcpStream_X->write_queue_size);
  if (pUvTcp_X)
  {
    DBG_OUT("[DISC6] OnUvAsyncDisconnect strm %p pconnect %p puvtcp %p cancel %d iswrt %d qs %d\n", pUvStream_X, pUvConnect_X, pUvTcp_X, _rBofIoAsyncCmd_X.pUvConnection_X->CancelPendingOp_B, uv_is_writable(pUvStream_X), pUvStream_X->write_queue_size);
    if ((!_rBofIoAsyncCmd_X.pUvConnection_X->CancelPendingOp_B) && (uv_is_writable(pUvStream_X)) && (pUvStream_X->write_queue_size > 0))
    {
      Rts_E = BOFERR_NOT_ENOUGH_RESOURCE;
      pBofIoAsyncRequest_X = mpIoAsyncRequestCollection->Get(0);
      BOF_ASSERT(pBofIoAsyncRequest_X != nullptr);
      DBG_OUT("[DISC7] OnUvAsyncDisconnect strm %p pconnect %p puvtcp %p cancel %d iswrt %d qs %d\n", pUvStream_X, pUvConnect_X, pUvTcp_X, _rBofIoAsyncCmd_X.pUvConnection_X->CancelPendingOp_B, uv_is_writable(pUvStream_X), pUvStream_X->write_queue_size);

      if (pBofIoAsyncRequest_X)
      {
        pBofIoAsyncRequest_X->NbUvBufferValid_U32 = 0;
        pBofIoAsyncRequest_X->Ticket_U32 = ++S_mReqTicket_U32;
        pBofIoAsyncRequest_X->Cmd_E = ASYNC_CMD::ASYNC_CMD_DISCONNECT;
        pBofIoAsyncRequest_X->pUvConnection_X->pUvIoChannel = pUvTcp_X;
        pBofIoAsyncRequest_X->pUserArg = _rBofIoAsyncCmd_X.pUserArg;
        pBofIoAsyncRequest_X->StartTimeInMs_U32 = Bof_GetMsTickCount();
        pBofIoAsyncRequest_X->TimeoutInMs_U32 = _rBofIoAsyncCmd_X.TimeoutInMs_U32;
        pUvCallback_X = reinterpret_cast<BOF_UV_CALLBACK *>(pUvTcp_X->data);
        BOF_ASSERT(pUvCallback_X != nullptr);

        ALLOC_UV_CALLBACK_DATA(&pBofIoAsyncRequest_X->ShutdownReq_X, pBofUvCallback_X, (std::string("DisReq_") + pUvCallback_X->Name_S), false, _rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection, pBofIoAsyncRequest_X, _rBofIoAsyncCmd_X.pUvConnection_X);
        // DBG_OUT("### shutdown\n");
        Sts_i = uv_shutdown(&pBofIoAsyncRequest_X->ShutdownReq_X, pUvStream_X, S_OnUvShutdownCallback);
        if (Sts_i == 0)
        {
          Rts_E = BOFERR_NO_ERROR;
        }
        else
        {
          Rts_E = BOFERR_CLOSE;
          pShutdown_X = &pBofIoAsyncRequest_X->ShutdownReq_X;
          FREE_UV_CALLBACK_DATA(pShutdown_X);
          mpIoAsyncRequestCollection->Release(pBofIoAsyncRequest_X); // Return to pot
        }
      } // if (pBofIoAsyncRequest_X)
    }
    else
    {
      Rts_E = BOFERR_NO_ERROR;
      pUvCallback_X = reinterpret_cast<BOF_UV_CALLBACK *>(pUvTcp_X->data);
      // DBG_OUT("### CLOSE_UV_RESOURCE %s\n", pUvCallback_X->Name_S.c_str());

      uv_handle_t *pHandle = reinterpret_cast<uv_handle_t *>(pUvTcp_X);
      DBG_OUT("[DISC8] OnUvAsyncDisconnect strm %p pconnect %p puvtcp %p cancel %d iswrt %d qs %d->Is closing %X\n", pUvStream_X, pUvConnect_X, pUvTcp_X, _rBofIoAsyncCmd_X.pUvConnection_X->CancelPendingOp_B, uv_is_writable(pUvStream_X),
              pUvStream_X->write_queue_size, uv_is_closing(pHandle));

      CLOSE_UV_RESOURCE(pUvTcp_X); // , _rBofIoAsyncCmd_X.pIBofIoConnection);
    }
  }
  if (pUvUdp_X)
  {
    Sts_i = uv_udp_recv_stop(pUvUdp_X);
    BOF_ASSERT(Sts_i == 0);

    if (Bof_IsMulticastUrlAddress(_rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection->IoChannelParam().Address_S, InterfaceIpAddressComponent_X, IpAddressComponent_X))
    {
      if (_rBofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection->IoChannelParam().IO_CHANNEL.UDP.MulticastReceiver_B)
      {
        // ou est le port ?
        Sts_i = uv_udp_set_membership(pUvUdp_X, IpAddressComponent_X.IpAddress_S.c_str(), InterfaceIpAddressComponent_X.IpAddress_S.c_str(), UV_LEAVE_GROUP);
        BOF_ASSERT(Sts_i == 0);
      }
    }
    pUvCallback_X = reinterpret_cast<BOF_UV_CALLBACK *>(pUvUdp_X->data);
    // DBG_OUT("### CLOSE_UV_RESOURCE %s\n", pUvCallback_X->Name_S.c_str());
    CLOSE_UV_RESOURCE(pUvUdp_X); // , _rBofIoAsyncCmd_X.pIBofIoConnection);
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::OnUvAlloc(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_buf_t *_pBuffer_X)
{
  BOFERR Rts_E = BOFERR_DISCONNECTED;
  BOF_BUFFER Buffer_X;

  BOF_ASSERT(_pBuffer_X != nullptr);
  _pBuffer_X->len = 0;
  _pBuffer_X->base = nullptr;
  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X != nullptr);
  if (_pBofUvCallback_X->pUvConnection_X->pIBofIoConnection != nullptr)
  {
    Rts_E = _pBofUvCallback_X->pUvConnection_X->pIBofIoConnection->GetBuffer(Buffer_X);
    BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);
    BOF_ASSERT(Buffer_X.pData_U8 != nullptr);
    BOF_ASSERT(Buffer_X.SizeInByte_U32 != 0);

    if (Rts_E == BOFERR_NO_ERROR)
    {
      _pBuffer_X->len = Buffer_X.SizeInByte_U32;
      _pBuffer_X->base = reinterpret_cast<char *>(Buffer_X.pData_U8);
    }
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::OnUvRead(BOF_UV_CALLBACK *_pBofUvCallback_X, ssize_t _NbRead, const uv_buf_t *_pBuffer_X)
{
  BOFERR Rts_E;

  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X->pIBofIoConnection != nullptr);
  if (_NbRead < 0)
  {
    if (_NbRead == UV_EOF)
    {
      Rts_E = BOFERR_EOF;
    }
    else
    {
      DBG_OUT("[%08X] OnUvRead sz %d buf %d:%p->INTERNAL\n", BOF_NAMESPACE::Bof_GetMsTickCount(), _NbRead, _pBuffer_X->len, _pBuffer_X->base);
      Rts_E = BOFERR_INTERNAL;
    }
  }
  else
  {
    Rts_E = _pBofUvCallback_X->pUvConnection_X->pIBofIoConnection->OnReadyRead(_NbRead);
    // DBG_OUT("[%08X] OnUvRead Rts %x strm %p sz %d buf %d:%p\n", BOF_NAMESPACE::Bof_GetMsTickCount(), Rts_E, _pUvStream_X, _NbRead, _pBuffer_X->len, _pBuffer_X->base);
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::OnUvWrite(BOF_UV_CALLBACK *_pBofUvCallback_X, int _Status_i)
{
  BOFERR Rts_E;
  uint32_t i_U32;
  BOF_IO_ASYNC_REQ *pBofIoAsyncRequest_X;

  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X->pIBofIoConnection != nullptr);
  // pBofIoAsyncRequest_X = reinterpret_cast<BOF_IO_ASYNC_REQ *>(_pBofUvCallback_X->pUser);
  pBofIoAsyncRequest_X = _pBofUvCallback_X->pBofIoAsyncRequest_X;
  // Rts_E = pBofIoAsyncRequest_X->pIBofIoConnection->V_DataWritten(static_cast<BOFERR>(_Status_i), pBofIoAsyncRequest_X->pUserArg);
  Rts_E = _pBofUvCallback_X->pUvConnection_X->pIBofIoConnection->V_DataWritten(static_cast<BOFERR>(_Status_i), pBofIoAsyncRequest_X->pUserArg);
  /*
     if (pBofIoAsyncRequest_X->pUvBuffer_X[0].len < 100)
     {
     char p[128];
     memcpy(p, pBofIoAsyncRequest_X->pUvBuffer_X[0].base, pBofIoAsyncRequest_X->pUvBuffer_X[0].len);
     p[pBofIoAsyncRequest_X->pUvBuffer_X[0].len] = 0;
     DBG_OUT("[%08X] OnUvWrite Rts %x %d buf [0]=%d:%p MustbeFreed %d con %s->%s\n", BOF_NAMESPACE::Bof_GetMsTickCount(), Rts_E, pBofIoAsyncRequest_X->NbUvBufferValid_U32, pBofIoAsyncRequest_X->pUvBuffer_X[0].len, pBofIoAsyncRequest_X->pUvBuffer_X[0].base,
     pBofIoAsyncRequest_X->pUvBufferMustbeFreed_B[0], pBofIoAsyncRequest_X->pIBofIoConnection->Name().c_str(),p);
     }
   */
  for (i_U32 = 0; i_U32 < pBofIoAsyncRequest_X->NbUvBufferValid_U32; i_U32++)
  {
    if (pBofIoAsyncRequest_X->pUvBufferMustBeDeleted_B[i_U32])
    {
      // DBG_OUT("[[[free %p\n", _pBofIoAsyncRequest_X->pUvBuffer_X[0].base);
      BOF_SAFE_DELETE_ARRAY(pBofIoAsyncRequest_X->pUvBuffer_X[0].base);
    }
  }
  return Rts_E;
}

// Client size
BOFERR BofIoConnectionManager::V_OnUvConnect(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_connect_t *_pReq_X, int _Status_i)
{
  BOFERR Rts_E = BOFERR_INTERNAL;
  int Sts_i, NameLen_i;
  IBofIoConnection *pIBofIoConnection;
  uv_udp_t *pUvUdp_X;
  uv_tcp_t *pUvTcp_X;
  uv_stream_t *pUvStream_X;
  uv_handle_t *pUvHandle_X;
  BOF_UV_CONNECTION *pUvConnection_X;
  struct sockaddr SockName_X;
  BOF_SOCKET_ADDRESS SocketAddress_X;
  const struct sockaddr_in *pIpAddress_X;
  uint32_t Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32;
  bool IsUdp_B = false;

  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  if (_pBofUvCallback_X->pUvConnection_X == nullptr) // IsUdp_B = (_pReq_X == nullptr);
  {
    pIBofIoConnection = nullptr;
    IsUdp_B = true;
  }
  else
  {
    BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X != nullptr);
    BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X->pIBofIoConnection != nullptr);
    // pIBofIoConnection = reinterpret_cast<IBofIoConnection *>(_pBofUvCallback_X->pUser);
    pIBofIoConnection = _pBofUvCallback_X->pUvConnection_X->pIBofIoConnection;
    BOF_ASSERT(pIBofIoConnection != nullptr);
    // pUvConnection_X = UvConnection(pIBofIoConnection);
  }
  pUvConnection_X = _pBofUvCallback_X->pUvConnection_X;
  BOF_ASSERT(pUvConnection_X != nullptr);

  // NO IsUdp_B = (pIBofIoConnection->ProtocolType() == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP);

  pUvHandle_X = reinterpret_cast<uv_handle_t *>(pUvConnection_X->pUvIoChannel);
  pUvStream_X = reinterpret_cast<uv_stream_t *>(pUvConnection_X->pUvIoChannel);
  if (IsUdp_B)
  {
    pUvTcp_X = nullptr;
    pUvUdp_X = reinterpret_cast<uv_udp_t *>(pUvConnection_X->pUvIoChannel);
  }
  else
  {
    pUvTcp_X = reinterpret_cast<uv_tcp_t *>(pUvConnection_X->pUvIoChannel);
    pUvUdp_X = nullptr;
  }
  if ((_Status_i == 0) || (_Status_i == UV_ECANCELED) || (_Status_i == UV_ENETUNREACH) || (_Status_i == UV_ETIMEDOUT) || (UV_ECONNREFUSED))
  {
    if (_Status_i == 0)
    {
      Rts_E = BofIoConnectionManager::S_SetIoChannelOption(IsUdp_B, pUvHandle_X, pUvConnection_X->IoChannelParam_X);
      if (Rts_E == BOFERR_NO_ERROR)
      {
        pIBofIoConnection->IoState(BOF_IO_STATE::CONNECTED);
        Rts_E = pIBofIoConnection->V_Connected();
        Rts_E = pIBofIoConnection->SignalConnect();
        BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);
        if (IsUdp_B)
        {
          NameLen_i = sizeof(SockName_X);
          Sts_i = uv_udp_getsockname(pUvUdp_X, &SockName_X, &NameLen_i);
          BOF_ASSERT(Sts_i == 0);

          Sts_i = uv_udp_recv_start(pUvUdp_X, S_OnUvAllocCallback, S_OnUvReadUdpCallback); // Called in the uv_loop context
        }
        else
        {
          NameLen_i = sizeof(SockName_X);
          Sts_i = uv_tcp_getsockname(pUvTcp_X, &SockName_X, &NameLen_i);
          BOF_ASSERT(Sts_i == 0);

          Sts_i = uv_read_start(pUvStream_X, S_OnUvAllocCallback, S_OnUvReadTcpCallback); // Called in the uv_loop context
        }
        pIpAddress_X = reinterpret_cast<struct sockaddr_in *>(&SockName_X);
        BOF_U32IPADDR_TO_U8IPADDR(pIpAddress_X->sin_addr.s_addr, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32);
        SocketAddress_X.Set(false, BOF_SOCK_TYPE::BOF_SOCK_TCP, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32, ntohs(pIpAddress_X->sin_port));
        pUvConnection_X->pIBofIoConnection->LocalAddress(SocketAddress_X);
      }
    }
    else
    {
      if (_Status_i != UV_ECANCELED) // Connect op cancelled by timeout/retry->uv_close sent in timer cb
      {
      }
      Rts_E = (pIBofIoConnection->ConnectError() == BOFERR_NO_ERROR) ? BOFERR_CONNECT : pIBofIoConnection->ConnectError();

      pIBofIoConnection->IoState(BOF_IO_STATE::DISCONNECTED);
      pIBofIoConnection->V_ConnectFailed(pIBofIoConnection->ConnectError());

      PushDisconnect("V_OnUvConnect", pIBofIoConnection->UvConnection(), true); // No WaitForDisconnect as we are in callback
    }
  }
  else
  {
    // const char *p = uv_strerror(_Status_i);
    BOF_ASSERT(_Status_i == 0);
  }

  LockBofIoConnectionManager("V_OnUvConnect");
  auto It = std::find(mUvConnectingConnectionCollection.begin(), mUvConnectingConnectionCollection.end(), pUvConnection_X);

  BOF_ASSERT(It != mUvConnectingConnectionCollection.end());
  if (It != mUvConnectingConnectionCollection.end())
  {
    mUvConnectingConnectionCollection.erase(It);
  }
  UnlockBofIoConnectionManager();

  return Rts_E;
}

// Server side
BOFERR BofIoConnectionManager::V_OnUvConnection(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_stream_t *_pUvStream_X, int _Status_i, IBofIoConnection *_pIBofIoListeningUdpConnection, void *_pUvIoChannel)
{
  BOFERR Rts_E = BOFERR_INTERNAL;
  int Sts_i, NameLen_i;
  struct sockaddr SockName_X;
  BOF_UV_CALLBACK *pBofUvCallback_X;
  uv_stream_t *pUvStream_X;
  uv_handle_t *pUvHandle_X;
  BOF_UV_CONNECTION *pUvConnection_X;
  IBofIoConnection *pIBofIoConnection, *pIBofIoConnectionCreatedByFactory;
  IBofIoConnectionFactory *pIBofIoConnectionFactory;
  uv_udp_t *pUvUdp_X = nullptr;
  struct sockaddr_in *pIpAddress_X;
  uint32_t Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32;
  BOF_SOCKET_ADDRESS SocketAddress_X;
  bool IsUdp_B;
  BOF_IO_CHANNEL_INFO BofIoChannelInfo_X;

  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X->pIBofIoConnection != nullptr);
  BOF_ASSERT(_Status_i == 0);
  // BOF_ASSERT(_pUvStream_X !=nullptr);

  // pIBofIoConnection = reinterpret_cast<IBofIoConnection *>(_pBofUvCallback_X->pUser);
  pIBofIoConnection = _pBofUvCallback_X->pUvConnection_X->pIBofIoConnection;
  BOF_ASSERT(pIBofIoConnection != nullptr);
  pIBofIoConnectionFactory = pIBofIoConnection->IConnectionFactory();

  if (_Status_i == 0)
  {
    Rts_E = BOFERR_NOT_AVAILABLE;
    if (pIBofIoConnectionFactory)
    {
      LockBofIoConnectionManager("V_OnUvConnection");
      GET_CONNECTION(pUvConnection_X, Rts_E);
      UnlockBofIoConnectionManager();
      if (Rts_E == BOFERR_NO_ERROR)
      {
        IsUdp_B = (_pUvStream_X == nullptr);
        Rts_E = BOFERR_CREATE;
        BofIoChannelInfo_X.ProtocolType_E = IsUdp_B ? BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP : BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP;
        BofIoChannelInfo_X.Name_S = _pBofUvCallback_X->Name_S;
        pIBofIoConnectionCreatedByFactory = pIBofIoConnectionFactory->V_CreateSession(BofIoChannelInfo_X, this, pUvConnection_X->IoChannelParam_X);
        BOF_ASSERT(IsIBofIoConnectionAlreadyInTable(pIBofIoConnectionCreatedByFactory) == false);
        pUvConnection_X->pIBofIoConnection = pIBofIoConnectionCreatedByFactory;
        pUvConnection_X->pIBofIoConnection->UvConnection(pUvConnection_X);

        if (pUvConnection_X->pIBofIoConnection)
        {
          pUvConnection_X->pIBofIoConnection->IConnectionFactory(pIBofIoConnectionFactory);
          pUvConnection_X->pIBofIoConnection->ServerSession(true);
          pUvConnection_X->pIBofIoConnection->IoState(BOF_IO_STATE::CONNECTED);
          pUvConnection_X->Name_S = _pBofUvCallback_X->Name_S + "_ses_" + std::to_string(++S_mSessionId_U32); // std::to_string(mpPotOfUvConnection->GetIndexOfEntry(pUvConnection_X));
          pUvConnection_X->StartConnectTime_U32 = Bof_GetMsTickCount();

          if (IsUdp_B)
          {
            BOF_ASSERT(_pIBofIoListeningUdpConnection != nullptr);
            BOF_ASSERT(_pUvIoChannel != nullptr);
            pUvConnection_X->pUvIoChannel = _pUvIoChannel;
            pUvUdp_X = reinterpret_cast<uv_udp_t *>(pUvConnection_X->pUvIoChannel);
            pUvStream_X = reinterpret_cast<uv_stream_t *>(pUvConnection_X->pUvIoChannel);

            NameLen_i = sizeof(SockName_X);
            Sts_i = uv_udp_getsockname(pUvUdp_X, &SockName_X, &NameLen_i);
            BOF_ASSERT(Sts_i == 0);

            pUvConnection_X->pIBofIoConnection->ProtocolType(BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP);
            pUvConnection_X->pIBofIoConnection->BindAddress(_pIBofIoListeningUdpConnection->BindAddress());
            pUvConnection_X->pIBofIoConnection->DestinationAddress(_pIBofIoListeningUdpConnection->DestinationAddress());
          }
          else
          {
            uv_tcp_t *pUvTcp_X = new uv_tcp_t;
            pUvConnection_X->pUvIoChannel = pUvTcp_X;
            BOF_ASSERT(pUvTcp_X != nullptr);

            Sts_i = uv_tcp_init(mpuLoop_X.get(), pUvTcp_X);
            BOF_ASSERT(Sts_i == 0);

            pUvStream_X = reinterpret_cast<uv_stream_t *>(pUvConnection_X->pUvIoChannel);
            Sts_i = uv_accept(_pUvStream_X, pUvStream_X); // Same than Session_X.pUvTcp_X
            BOF_ASSERT(Sts_i == 0);

            NameLen_i = sizeof(SockName_X);
            Sts_i = uv_tcp_getpeername(pUvTcp_X, &SockName_X, &NameLen_i);
            BOF_ASSERT(Sts_i == 0);

            pIpAddress_X = reinterpret_cast<struct sockaddr_in *>(&SockName_X);
            BOF_U32IPADDR_TO_U8IPADDR(pIpAddress_X->sin_addr.s_addr, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32);
            SocketAddress_X.Set(false, IsUdp_B ? BOF_SOCK_TYPE::BOF_SOCK_UDP : BOF_SOCK_TYPE::BOF_SOCK_TCP, IsUdp_B ? BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP : BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32,
                                ntohs(pIpAddress_X->sin_port));
            pUvConnection_X->pIBofIoConnection->DestinationAddress(SocketAddress_X);

            NameLen_i = sizeof(SockName_X);
            Sts_i = uv_tcp_getsockname(pUvTcp_X, &SockName_X, &NameLen_i);
            BOF_ASSERT(Sts_i == 0);

            pUvConnection_X->pIBofIoConnection->ProtocolType(BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP);
            // not valid in tcp           pUvConnection_X->pIBofIoConnection->BindAddress(_pIBofIoListeningUdpConnection->BindAddress());
            // not valid in tcp           pUvConnection_X->pIBofIoConnection->DestinationAddress(_pIBofIoListeningUdpConnection->DestinationAddress());
          }

          pIpAddress_X = reinterpret_cast<struct sockaddr_in *>(&SockName_X);
          BOF_U32IPADDR_TO_U8IPADDR(pIpAddress_X->sin_addr.s_addr, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32);
          SocketAddress_X.Set(false, IsUdp_B ? BOF_SOCK_TYPE::BOF_SOCK_UDP : BOF_SOCK_TYPE::BOF_SOCK_TCP, IsUdp_B ? BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP : BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP, Ip1_U32, Ip2_U32, Ip3_U32, Ip4_U32,
                              ntohs(pIpAddress_X->sin_port));
          pUvConnection_X->pIBofIoConnection->LocalAddress(SocketAddress_X);
          if (IsUdp_B)
          {
          }
          else
          {
            // pUvConnection_X->pIBofIoConnection->BindAddress(SocketAddress_X);
            SocketAddress_X.Set(false, BOF_SOCK_TYPE::BOF_SOCK_TCP, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP, 0, 0, 0, 0, 0); // ntohs(pIpAddress_X->sin_port));
            pUvConnection_X->pIBofIoConnection->BindAddress(SocketAddress_X);
            pUvConnection_X->IoChannelParam_X.Address_S = Bof_IpAddressToString(pUvConnection_X->pIBofIoConnection->BindAddress(), true, true) + ";" + Bof_IpAddressToString(pUvConnection_X->pIBofIoConnection->DestinationAddress(), false, true);
          }
          pUvHandle_X = reinterpret_cast<uv_handle_t *>(pUvConnection_X->pUvIoChannel);
          Rts_E = BofIoConnectionManager::S_SetIoChannelOption(IsUdp_B, pUvHandle_X, pUvConnection_X->IoChannelParam_X);
          if (Rts_E == BOFERR_NO_ERROR)
          {
            pUvConnection_X->pIBofIoConnection->Name(pUvConnection_X->Name_S);

            ALLOC_UV_CALLBACK_DATA(pUvConnection_X->pUvIoChannel, pBofUvCallback_X, pUvConnection_X->Name_S, true, pUvConnection_X->pIBofIoConnection, nullptr, pUvConnection_X);
            Rts_E = pUvConnection_X->pIBofIoConnection->V_Connected();   // Signal on created session connection
            Rts_E = pUvConnection_X->pIBofIoConnection->SignalConnect(); // Signal on created session connection
            BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);

            if (IsUdp_B)
            {
              Sts_i = uv_udp_recv_start(pUvUdp_X, S_OnUvAllocCallback, S_OnUvReadUdpCallback); // Called in the uv_loop context
            }
            else
            {
              Sts_i = uv_read_start(pUvStream_X, S_OnUvAllocCallback, S_OnUvReadTcpCallback);
            }

            Rts_E = BOFERR_NO_ERROR;
          }

          if (Rts_E != BOFERR_NO_ERROR)
          {
            CONNECT_UV_FAILED(pUvConnection_X->pUvIoChannel, pUvConnection_X->pIBofIoConnection, Rts_E); // pUvConnection_X->pUvTcp_X same as pUvStream_X
          }
          else
          {
            DBG_OUT("CONNECTED pUv=%p add %s name %s\n", pUvConnection_X, pUvConnection_X->IoChannelParam_X.Address_S.c_str(), pUvConnection_X->Name_S.c_str());
          }
        }
      }
    }
  }
  else
  {
    // const char *p = uv_strerror(_Status_i);
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::V_OnUvAlloc(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_handle_t *_pHandle, size_t _SuggestedSize, uv_buf_t *_pBuffer_X)
{
  BOFERR Rts_E = BOFERR_DISCONNECTED;
  BOF_BUFFER Buffer_X;

  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X != nullptr);
  BOF_ASSERT(_pHandle != nullptr);
  BOF_ASSERT(_pBuffer_X != nullptr);

  if (_pBofUvCallback_X->pUvConnection_X->pIBofIoConnection != nullptr)
  {
    Rts_E = OnUvAlloc(_pBofUvCallback_X, _pBuffer_X);
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::V_OnUvReadTcp(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_stream_t *_pStream_X, ssize_t _NbRead, const uv_buf_t *_pBuffer_X)
{
  BOFERR Rts_E = BOFERR_NOT_FOUND;

  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X != nullptr);
  BOF_ASSERT(_pStream_X != nullptr);
  BOF_ASSERT(_pBuffer_X != nullptr);

  Rts_E = (_pBofUvCallback_X->pUvConnection_X->pIBofIoConnection != nullptr) ? OnUvRead(_pBofUvCallback_X, _NbRead, _pBuffer_X) : BOFERR_CLOSE;
  if (Rts_E != BOFERR_NO_ERROR)
  {
    PushDisconnect("V_OnUvTcpRead", _pBofUvCallback_X->pUvConnection_X, true); // No WaitForDisconnect as we are in callback
  }
  return Rts_E;
}

BOFERR
BofIoConnectionManager::V_OnUvReadUdp(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_udp_t *_pUvUdp_X, ssize_t _NbRead, const uv_buf_t *_pBuffer_X, const struct sockaddr *_pAddress_X, uint32_t _Flag_U32)
{
  BOFERR Rts_E = BOFERR_NOT_FOUND;

  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X->pIBofIoConnection != nullptr);
  BOF_ASSERT(_pUvUdp_X != nullptr);
  BOF_ASSERT(_pBuffer_X != nullptr);
  // can be null when destination unreachable BOF_ASSERT(_pAddress_X != nullptr);
  // std::string Ip_S = (_pAddress_X != nullptr) ? Bof_SockAddrInToString(*_pAddress_X, true) : "NULL";

  Rts_E = OnUvRead(_pBofUvCallback_X, _NbRead, _pBuffer_X);
  if (Rts_E != BOFERR_NO_ERROR)
  {
    DBG_OUT("### ReadErr %s IoState %s->Post disconnect\n", _pBofUvCallback_X->pUvConnection_X->pIBofIoConnection->Name().c_str(), _pBofUvCallback_X->pUvConnection_X->pIBofIoConnection->IoStateString().c_str());
    // Disconnect("V_OnUvUdpRead", pUvConnection_X->pIBofIoConnection, true);	//No WaitForDisconnect as we are in callback
    LockBofIoConnectionManager("V_OnUvReadUdp Error");
    mUvDisconnectingConnectionCollection.push_back(_pBofUvCallback_X->pUvConnection_X);
    UnlockBofIoConnectionManager();
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::V_OnUvWriteTcp(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_write_t *_pReq_X, int _Status_i)
{
  BOFERR Rts_E;
  BOF_IO_ASYNC_REQ *pBofIoAsyncRequest_X;
  uv_write_t *pTcpWrtReq_X;
  IBofIoConnection *pIBofIoConnection;

  // DBG_OUT("[%08X] BofIoServer::V_OnUvWrite pReq %p\n", BOF_NAMESPACE::Bof_GetMsTickCount(), _pReq_X);
  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X->pIBofIoConnection != nullptr);
  BOF_ASSERT(_pReq_X != nullptr);
  // pBofIoAsyncRequest_X = reinterpret_cast<BOF_IO_ASYNC_REQ *>(_pBofUvCallback_X->pUser);
  pIBofIoConnection = _pBofUvCallback_X->pUvConnection_X->pIBofIoConnection;
  pBofIoAsyncRequest_X = _pBofUvCallback_X->pBofIoAsyncRequest_X;
  BOF_ASSERT(pBofIoAsyncRequest_X != nullptr);
  BOF_ASSERT(pIBofIoConnection != nullptr);
  BOF_ASSERT(pBofIoAsyncRequest_X->NbUvBufferValid_U32 >= 1);
  Rts_E = OnUvWrite(_pBofUvCallback_X, _Status_i);
  if (pBofIoAsyncRequest_X->SignalEndOfIo_B)
  {
    pIBofIoConnection->SignalEndOfIo();
  }
  pTcpWrtReq_X = &pBofIoAsyncRequest_X->TcpWrtReq_X;
  if (_Status_i) // TODO ???
  {
  }
  FREE_UV_CALLBACK_DATA(pTcpWrtReq_X);
  mpIoAsyncRequestCollection->Release(pBofIoAsyncRequest_X); // Return to pot

  return Rts_E;
}

BOFERR BofIoConnectionManager::V_OnUvWriteUdp(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_udp_send_t *_pReq_X, int _Status_i)
{
  BOFERR Rts_E;
  BOF_IO_ASYNC_REQ *pBofIoAsyncRequest_X;
  uv_udp_send_t *pUdpWrtReq_X;
  IBofIoConnection *pIBofIoConnection;

  // DBG_OUT("[%08X] BofIoServer::V_OnUvWrite pReq %p\n", BOF_NAMESPACE::Bof_GetMsTickCount(), _pReq_X);
  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X->pIBofIoConnection != nullptr);
  BOF_ASSERT(_pReq_X != nullptr);
  // pBofIoAsyncRequest_X = reinterpret_cast<BOF_IO_ASYNC_REQ *>(_pBofUvCallback_X->pUser);
  pIBofIoConnection = _pBofUvCallback_X->pUvConnection_X->pIBofIoConnection;
  pBofIoAsyncRequest_X = _pBofUvCallback_X->pBofIoAsyncRequest_X;
  BOF_ASSERT(pBofIoAsyncRequest_X != nullptr);
  // BOF_ASSERT(pBofIoAsyncRequest_X->pIBofIoConnection != nullptr);
  BOF_ASSERT(pIBofIoConnection != nullptr);
  BOF_ASSERT(pBofIoAsyncRequest_X->NbUvBufferValid_U32 >= 1);
  Rts_E = OnUvWrite(_pBofUvCallback_X, _Status_i);
  if (pBofIoAsyncRequest_X->SignalEndOfIo_B)
  {
    // pBofIoAsyncRequest_X->pIBofIoConnection->SignalEndOfIo();
    pIBofIoConnection->SignalEndOfIo();
  }
  pUdpWrtReq_X = &pBofIoAsyncRequest_X->UdpWrtReq_X;
  FREE_UV_CALLBACK_DATA(pUdpWrtReq_X);
  mpIoAsyncRequestCollection->Release(pBofIoAsyncRequest_X); // Return to pot

  return Rts_E;
}

BOFERR BofIoConnectionManager::V_OnUvAsync(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_async_t *_pHandle)
{
  BOFERR Rts_E = BOFERR_INTERNAL;
  BOF_IO_ASYNC_CMD BofIoAsyncCmd_X;
  bool Finish_B;

  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  BOF_ASSERT(_pHandle != nullptr);
  BOF_ASSERT(_pHandle == mpuAsync_X.get());

  Finish_B = false;
  while (!Finish_B)
  {
    Rts_E = mpBofIoAsyncCmdCollection->Pop(&BofIoAsyncCmd_X,
                                           0); // Purge all pending cmd as uv_async_send states calling this function may not wakeup the event loop if it was already called previously within a short period of time
    if (Rts_E == BOFERR_NO_ERROR)
    {
      char p[256];
      int i;
      if (BofIoAsyncCmd_X.pBuffer_X[0].pData_U8)
      {
        i = (BofIoAsyncCmd_X.pBuffer_X[0].SizeInByte_U32 < sizeof(p)) ? BofIoAsyncCmd_X.pBuffer_X[0].SizeInByte_U32 : 16;
        strncpy(p, (char *)BofIoAsyncCmd_X.pBuffer_X[0].pData_U8, i);
        p[i] = 0;
      }
      else
      {
        strcpy(p, "null");
      }
      DBG_OUT("[DISC2] V_OnUvAsync cmd %d caller %s pUv %p pCon %p %s IoState %s Closing %s NbPend %d Data %d:%s\n", BofIoAsyncCmd_X.Cmd_E, BofIoAsyncCmd_X.Caller_S.c_str(), BofIoAsyncCmd_X.pUvConnection_X,
              BofIoAsyncCmd_X.pUvConnection_X ? BofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection : nullptr, BofIoAsyncCmd_X.pUvConnection_X ? BofIoAsyncCmd_X.pUvConnection_X->Name_S.c_str() : "???????",
              BofIoAsyncCmd_X.pUvConnection_X ? (BofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection ? BofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection->IoStateString().c_str() : "???????") : "???????",
              BofIoAsyncCmd_X.pUvConnection_X ? BofIoAsyncCmd_X.pUvConnection_X->Closing_B ? "True" : "False" : "???", mUvDisconnectingConnectionCollection.size(), BofIoAsyncCmd_X.pBuffer_X[0].SizeInByte_U32, p);

      switch (BofIoAsyncCmd_X.Cmd_E)
      {
        case ASYNC_CMD::ASYNC_CMD_EXIT:
          Rts_E = ReleaseUvLoopResource();
          Finish_B = true;
          break;

        case ASYNC_CMD::ASYNC_CMD_WRITE:
          if (BofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection)
          {
            Rts_E = OnUvAsyncWrite(BofIoAsyncCmd_X);
          }
          break;

        case ASYNC_CMD::ASYNC_CMD_CONNECT:
          if (BofIoAsyncCmd_X.pUvConnection_X->pIBofIoConnection)
          {
            Rts_E = OnUvAsyncConnect(BofIoAsyncCmd_X);
          }
          break;

        case ASYNC_CMD::ASYNC_CMD_DISCONNECT:
          // printf(")))))Receive Disc2 %p rts %X\n", BofIoAsyncCmd_X.pIBofIoConnection, Rts_E);
          Rts_E = OnUvAsyncDisconnect(BofIoAsyncCmd_X);
          // printf(")))))End of Disc3 %p rts %X\n", BofIoAsyncCmd_X.pIBofIoConnection, Rts_E);
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
  return Rts_E;
}

BOFERR BofIoConnectionManager::V_OnUvTimer(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_timer_t *_pHandle)
{
  BOFERR Rts_E = BOFERR_INTERNAL;
  uint32_t Delta_U32;
  bool IncIt_B;
  uv_tcp_t *pUvTcp_X;

  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  BOF_ASSERT(_pHandle == mpuTimer_X.get());
  BOF_ASSERT(_pHandle != nullptr);

  // DBG  UvWalk();  std::string g = mUvWalkResult_S;

  LockBofIoConnectionManager("V_OnUvTimer"); // We have no time....
  auto It = mUvConnectingConnectionCollection.begin();
  while (It != mUvConnectingConnectionCollection.end())
  {
    BOF_ASSERT(*It != nullptr);
    IncIt_B = true;
    if ((*It)->pIBofIoConnection) // Can be under construction
    {
      pUvTcp_X = reinterpret_cast<uv_tcp_t *>((*It)->pUvIoChannel);
      BOF_ASSERT(pUvTcp_X != nullptr);
      switch ((*It)->pIBofIoConnection->IoState())
      {
        case BOF_IO_STATE::CONNECTING:
          Delta_U32 = Bof_ElapsedMsTime((*It)->StartConnectTime_U32);
          if (Delta_U32 >= (*It)->IoChannelParam_X.ConnectTimeoutInMs_U32)
          {
            (*It)->pIBofIoConnection->IoState(BOF_IO_STATE::TIMEOUT);
            CONNECT_UV_FAILED(pUvTcp_X, (*It)->pIBofIoConnection, BOFERR_TIMEOUT);
            It = mUvConnectingConnectionCollection.erase(It);
            IncIt_B = false;
          }
          break;

        default:
          break;
      }
    }
    if (IncIt_B)
    {
      ++It;
    }
  }
  UnlockBofIoConnectionManager();
  return Rts_E;
}

BOFERR BofIoConnectionManager::EndOfFactory(IBofIoConnectionFactory *_pIBofIoConnectionFactory)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  BOF_UV_CONNECTION *pUvConnection_X;

  if (_pIBofIoConnectionFactory)
  {
    LockBofIoConnectionManager("EndOfFactory"); // We have no time....

    pUvConnection_X = mpUvConnectionCollection->GetFirstUsed(0);
    while (pUvConnection_X)
    {
      if (pUvConnection_X->pIBofIoConnection)
      {
        if (pUvConnection_X->pIBofIoConnection->IConnectionFactory() == _pIBofIoConnectionFactory)
        {
          pUvConnection_X->pIBofIoConnection->IConnectionFactory(nullptr);
        }
      }
      pUvConnection_X = mpUvConnectionCollection->GetNextUsed(pUvConnection_X);
    }
    UnlockBofIoConnectionManager();
  }
  return Rts_E;
}

BOFERR BofIoConnectionManager::EndOfConnection(BOF_UV_CONNECTION *_pUvConnection_X)
{
  BOFERR Rts_E = BOFERR_NO_ERROR, Sts_E;
  bool ServerSession_B;
  IBofIoConnectionFactory *pIBofIoConnectionFactory;

  if ((_pUvConnection_X) && (_pUvConnection_X->pIBofIoConnection))
  {
    DBG_OUT("[EOC] pUv %p pCon %p Connected %d IoState %s\n", _pUvConnection_X, _pUvConnection_X->pIBofIoConnection, _pUvConnection_X->pIBofIoConnection->IsConnected(), _pUvConnection_X->pIBofIoConnection->IoStateString().c_str());
    if (_pUvConnection_X->pIBofIoConnection->IsConnected())
    {
      ServerSession_B = _pUvConnection_X->pIBofIoConnection->ServerSession();               // Save value before signaling disconnect->delete
      pIBofIoConnectionFactory = _pUvConnection_X->pIBofIoConnection->IConnectionFactory(); // Save value before signaling disconnect->delete

      _pUvConnection_X->pIBofIoConnection->IoState(BOF_IO_STATE::DISCONNECTED);
      DBG_OUT("[EOC] virtual call for pUv %p pCon %p Object %s IoState %s ServerSession %s pIBofIoConnectionFactory %p->Virtual call\n", _pUvConnection_X, _pUvConnection_X->pIBofIoConnection, _pUvConnection_X->pIBofIoConnection->Name().c_str(),
              _pUvConnection_X->pIBofIoConnection->IoStateString().c_str(), ServerSession_B ? "True" : "False", pIBofIoConnectionFactory);

      Sts_E = _pUvConnection_X->pIBofIoConnection->V_Disconnected();
      Rts_E = _pUvConnection_X->pIBofIoConnection->SignalDisconnect();
      BOF_ASSERT(Rts_E == BOFERR_NO_ERROR);

      if (ServerSession_B)
      {
        // DBG_OUT("### call V_SessionDisconnected with pIBofIoConnectionFactory %p\n", mpIBofIoConnectionFactory);
        if (pIBofIoConnectionFactory) // If nullptr, already deleted and signaled by EndOfFactory
        {
          Sts_E = pIBofIoConnectionFactory->V_SessionDisconnected(_pUvConnection_X->pIBofIoConnection); // Delete the obj
        }
        DBG_OUT("[EOC] of ServerSession pUv %p pCon %p fac %p\n", _pUvConnection_X, _pUvConnection_X->pIBofIoConnection, pIBofIoConnectionFactory);
      }
    }
    else
    {
      DBG_OUT("[EOC]->Object %s pCon %p is not active\n", _pUvConnection_X->pIBofIoConnection->Name().c_str(), _pUvConnection_X->pIBofIoConnection);
    }
  }
  else
  {
    DBG_OUT("EndOfConnection->pUv %p or pCon %p is NULL\n", _pUvConnection_X, _pUvConnection_X ? _pUvConnection_X->pIBofIoConnection : nullptr);
  }
  return Rts_E;
}

// Triggered by CLOSE_UV_RESOURCE
BOFERR BofIoConnectionManager::V_OnUvClose(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_handle_t *_pHandle)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  // IBofIoConnection *pIBofIoConnection;

  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  LockBofIoConnectionManager("V_OnUvClose");
  // BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X != nullptr);
  // BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X->pIBofIoConnection != nullptr);
  if ((_pBofUvCallback_X->pUvConnection_X == nullptr) || (_pBofUvCallback_X->pUvConnection_X->pIBofIoConnection == nullptr))
  {
    DBG_OUT("[ONCLS] %s has a pUv %p or pCon %p null\n", _pBofUvCallback_X->Name_S.c_str(), _pBofUvCallback_X->pUvConnection_X, _pBofUvCallback_X->pUvConnection_X ? _pBofUvCallback_X->pUvConnection_X->pIBofIoConnection : nullptr);
    // pIBofIoConnection = nullptr;
  }
  else
  {
    // pIBofIoConnection = _pBofUvCallback_X->pUvConnection_X->pIBofIoConnection;
    BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X->pUvIoChannel == _pHandle);
    EndOfConnection(_pBofUvCallback_X->pUvConnection_X);
  }
  DBG_OUT("[ONCLS] %s has a cb %p pUv %p -> pCon %p pIo %p/%p pConnect %p Remain %d \n", _pBofUvCallback_X->Name_S.c_str(), _pBofUvCallback_X, _pBofUvCallback_X->pUvConnection_X,
          _pBofUvCallback_X->pUvConnection_X ? _pBofUvCallback_X->pUvConnection_X->pIBofIoConnection : nullptr, _pBofUvCallback_X->pUvConnection_X ? _pBofUvCallback_X->pUvConnection_X->pUvIoChannel : nullptr, _pHandle,
          _pBofUvCallback_X->pUvConnection_X ? _pBofUvCallback_X->pUvConnection_X->pUvIoConnect : nullptr, mUvDisconnectingConnectionCollection.size());
  if (_pBofUvCallback_X->pUvConnection_X)
  {
    auto It = std::find(mUvDisconnectingConnectionCollection.begin(), mUvDisconnectingConnectionCollection.end(), _pBofUvCallback_X->pUvConnection_X);
    // No		BOF_ASSERT(It != mUvDisconnectingConnectionCollection.end() );
    if (It != mUvDisconnectingConnectionCollection.end())
    {
      mUvDisconnectingConnectionCollection.erase(It);
      DBG_OUT("[ONCLS] %s pUv %p pCon %p has been erased->%d remains\n", _pBofUvCallback_X->Name_S.c_str(), _pBofUvCallback_X->pUvConnection_X, _pBofUvCallback_X->pUvConnection_X ? _pBofUvCallback_X->pUvConnection_X->pIBofIoConnection : nullptr,
              mUvDisconnectingConnectionCollection.size());
    }
  }

  // NO only one dele point DBG_OUT("V_OnUvClose %s delete %p\n", _pBofUvCallback_X->Name_S.c_str(), pIBofIoConnection);
  // BOF_SAFE_DELETE(pIBofIoConnection)
  if (_pBofUvCallback_X->pUvConnection_X)
  {
    RELEASE_CONNECTION(_pBofUvCallback_X->pUvConnection_X, Rts_E);
  }
  FREE_UV_CALLBACK_DATA(_pHandle);
  UnlockBofIoConnectionManager();

  return Rts_E;
}

BOFERR BofIoConnectionManager::V_OnUvShutdown(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_shutdown_t *_pReq_X, int _Status_i)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  // BOF_IO_ASYNC_REQ *pBofIoAsyncRequest_X;
  uv_shutdown_t *pShutdown_X;
  IBofIoConnection *pIBofIoConnection;

  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pUvConnection_X->pIBofIoConnection != nullptr);
  pIBofIoConnection = _pBofUvCallback_X->pUvConnection_X->pIBofIoConnection;
  BOF_ASSERT(_pReq_X != nullptr);
  // pBofIoAsyncRequest_X = reinterpret_cast<BOF_IO_ASYNC_REQ *>(_pBofUvCallback_X->pUser);
  BOF_ASSERT(_pBofUvCallback_X->pBofIoAsyncRequest_X != nullptr);
  BOF_ASSERT(pIBofIoConnection != nullptr);
  BOF_ASSERT(_pBofUvCallback_X->pBofIoAsyncRequest_X->NbUvBufferValid_U32 == 0);

  pShutdown_X = &_pBofUvCallback_X->pBofIoAsyncRequest_X->ShutdownReq_X;
  FREE_UV_CALLBACK_DATA(pShutdown_X);
  mpIoAsyncRequestCollection->Release(_pBofUvCallback_X->pBofIoAsyncRequest_X); // Return to pot

  CLOSE_UV_RESOURCE(_pBofUvCallback_X->pBofIoAsyncRequest_X->pUvConnection_X->pUvIoChannel); // , pBofIoAsyncRequest_X->pIBofIoConnection);

  return Rts_E;
}
// From uv-common.h (private)
#define UV__HANDLE_INTERNAL 0x80
#define UV__HANDLE_ACTIVE 0x40
#define UV__HANDLE_REF 0x20
#define UV__HANDLE_CLOSING 0x01

/*
   uv_print_all_handles(uv_default_loop(), stderr);

   [--I] signal   0x1a25ea8
   [-AI] async    0x1a25cf0
   [R--] idle     0x1a7a8c8

   The format is[flags] handle - type handle - address.For flags :

   R is printed for a handle that is referenced
   A is printed for a handle that is active
   I is printed for a handle that is internal
 */
BOFERR BofIoConnectionManager::V_OnUvWalk(BOF_UV_CALLBACK *_pBofUvCallback_X, uv_handle_t *_pHandle)
{
  BOFERR Rts_E = BOFERR_NO_ERROR;
  const char *pType_c;

  BOF_ASSERT(_pBofUvCallback_X != nullptr);
  BOF_ASSERT(_pHandle != nullptr);

  switch (_pHandle->type)
  {
#define X(uc, lc)                                                                                                                                                                                                                                              \
  case UV_##uc:                                                                                                                                                                                                                                                \
    pType_c = #lc;                                                                                                                                                                                                                                             \
    break;
    UV_HANDLE_TYPE_MAP(X)
#undef X
    default:
      pType_c = "<unknown>";
  }

  mUvWalkResult_S += BOF_NAMESPACE::Bof_Sprintf("%p %s[%c%c%c] %-8s %p '%s'\n", _pBofUvCallback_X, uv_is_active(_pHandle) ? "ON :" : "OFF:", "R-"[!(_pHandle->flags & UV__HANDLE_REF)], "A-"[!(_pHandle->flags & UV__HANDLE_ACTIVE)],
                                                "I-"[!(_pHandle->flags & UV__HANDLE_INTERNAL)], pType_c, (void *)_pHandle, _pBofUvCallback_X -> Name_S.c_str());
  // NO	if (uv_is_active(_pHandle))
  {
    mUvWalkHandleCollection.push_back(_pHandle);
  }
  // DBG_OUT("Walk n %d\n%s\n", mUvWalkHandleCollection.size(), mUvWalkResult_S.c_str());
  return Rts_E;
}

BOFERR BofIoConnectionManager::V_OnProcessing()
{
  BOFERR Rts_E = BOFERR_INIT;
  int Sts_i;
  uv_loop_t *pLoop_X;
  // uv_stream_t	 *pTcpServerStream_X = reinterpret_cast<uv_stream_t *>(mpuTcpServer_X.get());

  // DBG_OUT("-------------->BofIoServer enter\n");
  Sts_i = uv_timer_start(mpuTimer_X.get(), S_OnUvTimerCallback, LOOP_TIMER_GRANULARITY,
                         LOOP_TIMER_GRANULARITY); // mBofIoServerParam_X.BofIoParam_X.IoTimeoutInMs_U32, mBofIoServerParam_X.IoTimeoutInMs_U32);
  BOF_ASSERT(Sts_i == 0);

  try
  {
    if (Sts_i == 0)
    {
      Sts_i = uv_run(mpuLoop_X.get(), UV_RUN_DEFAULT);
      BOF_ASSERT((Sts_i == 0) || (Sts_i == 1));
      Rts_E = (Sts_i == 0) ? BOFERR_NO_ERROR : BOFERR_STOPPED;
      pLoop_X = mpuLoop_X.get();
      Sts_i = uv_loop_close(pLoop_X);
      BOF_ASSERT(Sts_i == 0);
      FREE_UV_CALLBACK_DATA(pLoop_X); // pLoop_X is nullptr after this
      // DBG_OUT("-------------->BofIoServer uv_loop_close %d\n", Sts_i);
      Rts_E = BOFERR_CANCEL; // For bofthread exit
    }
  }
  catch (std::exception &e)
  {
    DBG_OUT("BofIoConnectionManager1 exception %s\n", e.what());
  }
  // DBG_OUT("-------------->V_OnProcessing uv_loop thread exit %d\n", Rts_E);

  return Rts_E;
}

BOFERR BofIoConnectionManager::S_SetIoChannelOption(bool _IsUdp_B, uv_handle_t *_pUvHandle_X, const BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X)
{
  BOFERR Rts_E = BOFERR_INVALID_PARAM;
  int Sts_i, Val_i, ReadVal_i, Option_i;
  BOF_SOCKET_ADDRESS_COMPONENT InterfaceIpAddressComponent_X, IpAddressComponent_X;
  uv_os_sock_t Socket;

  if (_pUvHandle_X) // && (_pUvTcp_X->type == UV_TCP))
  {
    if (_IsUdp_B)
    {
      uv_udp_t *pUvUdp_X = reinterpret_cast<uv_udp_t *>(_pUvHandle_X);
#if defined(_WIN32)
      Socket = pUvUdp_X->socket;
#else
      Socket = pUvUdp_X->io_watcher.fd;
#endif
      if (Bof_IsMulticastUrlAddress(_rBofIoChannelParam_X.Address_S, InterfaceIpAddressComponent_X, IpAddressComponent_X))
      {
        // DBG_OUT("### Sock multicast receiver %s Loop %s ttl %d interface %s broad %s\n", _rBofIoChannelParam_X.IO_CHANNEL.UDP.MulticastReceiver_B ? "true" : "false", _rBofIoChannelParam_X.IO_CHANNEL.UDP.MulticastLoop_B ? "true" : "false",
        // _rBofIoChannelParam_X.IO_CHANNEL.UDP.Ttl_U32, InterfaceIpAddressComponent_X.IpAddress_S.c_str(), _rBofIoChannelParam_X.IO_CHANNEL.UDP.Broadcast_B ? "true" : "false");

        if (_rBofIoChannelParam_X.IO_CHANNEL.UDP.MulticastReceiver_B)
        {
          Sts_i = uv_udp_set_membership(pUvUdp_X, IpAddressComponent_X.IpAddress_S.c_str(), InterfaceIpAddressComponent_X.IpAddress_S.c_str(), UV_JOIN_GROUP);
          BOF_ASSERT(Sts_i == 0);
        }
        else
        {
          Sts_i = uv_udp_set_multicast_loop(pUvUdp_X, _rBofIoChannelParam_X.IO_CHANNEL.UDP.MulticastLoop_B);
          BOF_ASSERT(Sts_i == 0);
          Sts_i = uv_udp_set_multicast_ttl(pUvUdp_X, _rBofIoChannelParam_X.IO_CHANNEL.UDP.Ttl_U32);
          BOF_ASSERT(Sts_i == 0);
          Sts_i = uv_udp_set_multicast_interface(pUvUdp_X, InterfaceIpAddressComponent_X.IpAddress_S.c_str());
          BOF_ASSERT(Sts_i == 0);
        }
      }
      else if (_rBofIoChannelParam_X.IO_CHANNEL.UDP.Broadcast_B)
      {
        Sts_i = uv_udp_set_broadcast(pUvUdp_X, true);
        BOF_ASSERT(Sts_i == 0);
      }
    }
    else
    {
      uv_tcp_t *pUvTcp_X = reinterpret_cast<uv_tcp_t *>(_pUvHandle_X);

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

      Socket = uv__stream_fd((uv_stream_t *)_pUvHandle_X);
      ;
#endif
      Sts_i = uv_tcp_nodelay(pUvTcp_X, _rBofIoChannelParam_X.IO_CHANNEL.TCP.NoDelay_B);
      BOF_ASSERT(Sts_i == 0);
      // DBG_OUT("### Sock tcp nodelay %s\n", _rBofIoChannelParam_X.IO_CHANNEL.TCP.NoDelay_B ? "true":"false");
    }
    // DBG_OUT("### Sock rcv 0x%X snd 0x%X\n", _rBofIoChannelParam_X.RcvBufferSize_U32, _rBofIoChannelParam_X.RcvBufferSize_U32);

    /* Allow reuse of the port. */
    Option_i = 1;
    Sts_i = setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&Option_i), sizeof(Option_i));
    BOF_ASSERT(Sts_i == 0);

#if defined(_WIN32)
    Val_i = _rBofIoChannelParam_X.RcvBufferSize_U32;
#else
    Val_i = _rBofIoChannelParam_X.RcvBufferSize_U32 / 2; /* linux sets double the value */
#endif
    if (Val_i)
    {
      Sts_i = uv_recv_buffer_size(_pUvHandle_X, &Val_i);
      BOF_ASSERT(Sts_i == 0);

      ReadVal_i = 0;
      Sts_i = uv_recv_buffer_size(_pUvHandle_X, &ReadVal_i);
      BOF_ASSERT(Sts_i == 0);
      BOF_ASSERT(static_cast<uint32_t>(ReadVal_i) == _rBofIoChannelParam_X.RcvBufferSize_U32);
    }

#if defined(_WIN32)
    Val_i = _rBofIoChannelParam_X.SndBufferSize_U32;
#else
    Val_i = _rBofIoChannelParam_X.SndBufferSize_U32 / 2; /* linux sets double the value */
#endif
    if (Val_i)
    {
      Sts_i = uv_send_buffer_size(_pUvHandle_X, &Val_i);
      BOF_ASSERT(Sts_i == 0);

      ReadVal_i = 0;
      Sts_i = uv_send_buffer_size(_pUvHandle_X, &ReadVal_i);
      BOF_ASSERT(Sts_i == 0);
      BOF_ASSERT(static_cast<uint32_t>(ReadVal_i) == _rBofIoChannelParam_X.SndBufferSize_U32);

      Sts_i = uv_recv_buffer_size(_pUvHandle_X, &Val_i);
      BOF_ASSERT(Sts_i == 0);
    }

    Rts_E = BOFERR_NO_ERROR;
  }
  return Rts_E;
}

// Client side
void BofIoConnectionManager::S_OnUvConnectCallback(uv_connect_t *_pReq_X, int _Status_i)
{
  // DBG_OUT("S_OnUvConnectCallback pReq %p Sts %X\n", _pReq_X, _Status_i);
  BOF_UV_CALLBACK *pBofIoCallback_X = _pReq_X ? reinterpret_cast<BOF_UV_CALLBACK *>(_pReq_X->data) : nullptr;
  if ((pBofIoCallback_X) && (pBofIoCallback_X->pBofIoConnectionManager))
  {
    pBofIoCallback_X->pBofIoConnectionManager->V_OnUvConnect(pBofIoCallback_X, _pReq_X, _Status_i);
  }
}

// Server side
void BofIoConnectionManager::S_OnUvConnectionCallback(uv_stream_t *_pStream, int _Status_i)
{
  // DBG_OUT("S_OnUvConnectionCallback pStrm %p Sts %X\n", _pStream, _Status_i);
  BOF_UV_CALLBACK *pBofIoCallback_X = _pStream ? reinterpret_cast<BOF_UV_CALLBACK *>(_pStream->data) : nullptr;
  if ((pBofIoCallback_X) && (pBofIoCallback_X->pBofIoConnectionManager))
  {
    pBofIoCallback_X->pBofIoConnectionManager->V_OnUvConnection(pBofIoCallback_X, _pStream, _Status_i, nullptr, nullptr);
  }
}

void BofIoConnectionManager::S_OnUvAllocCallback(uv_handle_t *_pHandle, size_t _SuggestedSize, uv_buf_t *_pBuffer_X)
{
  // DBG_OUT("S_OnUvAllocCallback pHndl %p Sz %X pBuf %p\n", _pHandle, _SuggestedSize, _pBuffer_X);
  BOF_UV_CALLBACK *pBofIoCallback_X = _pHandle ? reinterpret_cast<BOF_UV_CALLBACK *>(_pHandle->data) : nullptr;
  if ((pBofIoCallback_X) && (pBofIoCallback_X->pBofIoConnectionManager))
  {
    pBofIoCallback_X->pBofIoConnectionManager->V_OnUvAlloc(pBofIoCallback_X, _pHandle, _SuggestedSize, _pBuffer_X);
  }
}

void BofIoConnectionManager::S_OnUvReadTcpCallback(uv_stream_t *_pStream_X, ssize_t _NbRead, const uv_buf_t *_pBuffer_X)
{
  // DBG_OUT("S_OnUvReadTcpCallback pStrm %p Nb %X pBuf %p\n", _pStream_X, _NbRead, _pBuffer_X);
  if (_pBuffer_X)
  {
    if (_pBuffer_X->base) //&& (_pBuffer_X->len))
    {
      BOF_UV_CALLBACK *pBofIoCallback_X = _pStream_X ? reinterpret_cast<BOF_UV_CALLBACK *>(_pStream_X->data) : nullptr;
      if ((pBofIoCallback_X) && (pBofIoCallback_X->pBofIoConnectionManager))
      {
        pBofIoCallback_X->pBofIoConnectionManager->V_OnUvReadTcp(pBofIoCallback_X, _pStream_X, _NbRead, _pBuffer_X);
      }
    }
  }
}

void BofIoConnectionManager::S_OnUvReadUdpCallback(uv_udp_t *_pUvUdp_X, ssize_t _NbRead, const uv_buf_t *_pBuffer_X, const struct sockaddr *_pAddress_X, uint32_t _Flag_U32)
{
  // DBG_OUT("S_OnUvReadUdpCallback uv_udp_t %p Nb %X pBuf %p pAdd %p flg %X\n", _pUvUdp_X, _NbRead, _pBuffer_X, _pAddress_X, _Flag_U32);
  if (_pBuffer_X)
  {
    if (_pBuffer_X->base) //&& (_pBuffer_X->len))
    {
      BOF_UV_CALLBACK *pBofIoCallback_X = _pUvUdp_X ? reinterpret_cast<BOF_UV_CALLBACK *>(_pUvUdp_X->data) : nullptr;
      if ((pBofIoCallback_X) && (pBofIoCallback_X->pBofIoConnectionManager))
      {
        pBofIoCallback_X->pBofIoConnectionManager->V_OnUvReadUdp(pBofIoCallback_X, _pUvUdp_X, _NbRead, _pBuffer_X, _pAddress_X, _Flag_U32);
      }
    }
  }
}

void BofIoConnectionManager::S_OnUvWriteTcpCallback(uv_write_t *_pStream_X, int _Status_i)
{
  // DBG_OUT("S_OnUvWriteTcpCallback pStrm %p Sts %X\n", _pStream_X, _Status_i);
  BOF_UV_CALLBACK *pBofIoCallback_X = _pStream_X ? reinterpret_cast<BOF_UV_CALLBACK *>(_pStream_X->data) : nullptr;
  if ((pBofIoCallback_X) && (pBofIoCallback_X->pBofIoConnectionManager))
  {
    pBofIoCallback_X->pBofIoConnectionManager->V_OnUvWriteTcp(pBofIoCallback_X, _pStream_X, _Status_i);
  }
}

void BofIoConnectionManager::S_OnUvWriteUdpCallback(uv_udp_send_t *_pReq_X, int _Status_i)
{
  // DBG_OUT("S_OnUvWriteUdpCallback _pReq_X %p Sts %X\n", _pReq_X, _Status_i);
  BOF_UV_CALLBACK *pBofIoCallback_X = _pReq_X ? reinterpret_cast<BOF_UV_CALLBACK *>(_pReq_X->data) : nullptr;
  if ((pBofIoCallback_X) && (pBofIoCallback_X->pBofIoConnectionManager))
  {
    pBofIoCallback_X->pBofIoConnectionManager->V_OnUvWriteUdp(pBofIoCallback_X, _pReq_X, _Status_i);
  }
}

void BofIoConnectionManager::S_OnUvAsyncCallback(uv_async_t *_pHandle)
{
  // DBG_OUT("S_OnUvAsyncCallback pHndl %p\n", _pHandle);
  BOF_UV_CALLBACK *pBofIoCallback_X = _pHandle ? reinterpret_cast<BOF_UV_CALLBACK *>(_pHandle->data) : nullptr;
  if ((pBofIoCallback_X) && (pBofIoCallback_X->pBofIoConnectionManager))
  {
    pBofIoCallback_X->pBofIoConnectionManager->V_OnUvAsync(pBofIoCallback_X, _pHandle);
  }
}

void BofIoConnectionManager::S_OnUvTimerCallback(uv_timer_t *_pHandle)
{
  // DBG_OUT("S_OnUvTimerCallback pHndl %p\n", _pHandle);
  BOF_UV_CALLBACK *pBofIoCallback_X = _pHandle ? reinterpret_cast<BOF_UV_CALLBACK *>(_pHandle->data) : nullptr;
  if ((pBofIoCallback_X) && (pBofIoCallback_X->pBofIoConnectionManager))
  {
    pBofIoCallback_X->pBofIoConnectionManager->V_OnUvTimer(pBofIoCallback_X, _pHandle);
  }
}

void BofIoConnectionManager::S_OnUvCloseCallback(uv_handle_t *_pHandle)
{
  // DBG_OUT("S_OnUvCloseCallback pHndl %p\n", _pHandle);
  BOF_UV_CALLBACK *pBofIoCallback_X = _pHandle ? reinterpret_cast<BOF_UV_CALLBACK *>(_pHandle->data) : nullptr;
  // DBG_OUT("S_OnUvCloseCallback pHndl %p %s\n", _pHandle, pBofIoCallback_X->Name_S.c_str());
  if ((pBofIoCallback_X) && (pBofIoCallback_X->pBofIoConnectionManager))
  {
    pBofIoCallback_X->pBofIoConnectionManager->V_OnUvClose(pBofIoCallback_X, _pHandle);
  }
}

void BofIoConnectionManager::S_OnUvShutdownCallback(uv_shutdown_t *_pReq_X, int _Status_i)
{
  DBG_OUT("S_OnUvShutdownCallback pReq %p Sts %X\n", _pReq_X, _Status_i);
  BOF_UV_CALLBACK *pBofIoCallback_X = _pReq_X ? reinterpret_cast<BOF_UV_CALLBACK *>(_pReq_X->data) : nullptr;
  if ((pBofIoCallback_X) && (pBofIoCallback_X->pBofIoConnectionManager))
  {
    pBofIoCallback_X->pBofIoConnectionManager->V_OnUvShutdown(pBofIoCallback_X, _pReq_X, _Status_i);
  }
}

void BofIoConnectionManager::S_OnUvWalkCallback(uv_handle_t *_pHandle, void *_pUserArg)
{
  // DBG_OUT("S_OnUvWalkCallback pHndl %p pUser %p\n", _pHandle, _pUserArg);
  BofIoConnectionManager *pBofIoConnectionManager = _pUserArg ? reinterpret_cast<BofIoConnectionManager *>(_pUserArg) : nullptr;
  BOF_UV_CALLBACK *pBofIoCallback_X = _pHandle ? reinterpret_cast<BOF_UV_CALLBACK *>(_pHandle->data) : nullptr;
  if ((pBofIoConnectionManager) && (pBofIoCallback_X) && (pBofIoCallback_X->pBofIoConnectionManager))
  {
    pBofIoConnectionManager->V_OnUvWalk(pBofIoCallback_X, _pHandle);
  }
}

END_BOF_NAMESPACE()