/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines routines for creating and managing a socket communication
 * channel.
 *
 * Name:        bofsocket.h
 * Author:      Bernard HARMEL: b.harmel@belgacom.net
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Aug 21 2001  BHA : Initial release
 */
#pragma once
#include <bofstd/bofcomchannel.h>
#include <bofstd/bofsocketos.h>
#include <bofstd/bofsystem.h>
#include <bofstd/bofthread.h>

BEGIN_BOF_NAMESPACE()

// The Connect string contains after the ':' an optional user argument
#define BOFSOCKET_UDP_CONNECT "BOF_UDP_CONNECT_BHA_01" // At lease 22 bytes (Ethernet min size 64)

struct BOF_SOCKET_PARAM
{
  BOF_COM_CHANNEL_PARAM BaseChannelParam_X; // Base properties of each channel
  std::string BindIpAddress_S;              /*! A string identifying the ip address to which this socket is bound. For multicast client this one should be 0.0.0.0:multicast group port*/
  bool ReUseAddress_B;                      /*! Set SO_REUSEADDR  option on or off*/
  bool NoDelay_B;
  uint32_t Ttl_U32; /*! This value is used to setup the socket time to live*/

  uint16_t BroadcastPort_U16;                /*! Port to used for broadcast transmission (0 if not broadcast) */
  std::string MulticastInterfaceIpAddress_S; /*! A string identifying the ip address to which this socket is bound suich as udp://10.129.170.30;239.193.0.1. Multicast port is specified in BindIpAddress_S */
  bool MulticastSender_B;                    /*! if MulticastIpAddress_X!=0 this boolean is used to setup the socket as a multicast receiver or transmitter */
  bool FilterMulticastOnIpAddress_B;         /* Filter multicast receiver source Valid only for linux: https://www.reddit.com/r/networking/comments/7nketv/proper_use_of_bind_for_multicast_receive_on_linux/ */
  bool KeepAlive_B;                          /*! Set SO_KEEPALIVE option on or off*/

  /*
   * To do multicast on a single host, you only have to do two things :
   * - make sure that your receiving multicast sockets (MulticastReceiver_B) have SO_REUSEADDR set (ReUseAddress_B) so that multiple processes can bind the same multicast address
   * - make sure your sending multicast sockets (MulticastReceiver_B) have IP_MULTICAST_LOOP set (EnableLocalMulticast_B) so that packets will be "looped back" to receivers on the same system->If needed
   * If your application uses a single socket for both sending and receiving multicasts, you would set both socket options on it.
   */
  bool EnableLocalMulticast_B;                /*! set IP_MULTICAST_LOOP for enabling local multicast reception */
  uint32_t MaxNumberOfScatterGatherEntry_U32; /*! Provide allocation of Scatter Gather array if needed. 0 if not used. (WriteScatterGatherData)*/
  BOF_SOCKET_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    BaseChannelParam_X.Reset();
    BindIpAddress_S = "";
    ReUseAddress_B = false;
    NoDelay_B = false;
    Ttl_U32 = 0;
    BroadcastPort_U16 = 0;
    FilterMulticastOnIpAddress_B = false;
    MulticastInterfaceIpAddress_S = "";
    MulticastSender_B = false;
    KeepAlive_B = false;
    EnableLocalMulticast_B = false;
    MaxNumberOfScatterGatherEntry_U32 = 0;
  }
};

class BOFSTD_EXPORT BofSocket : public BofComChannel
{

private:
  // static BofPot< BOF_SOCKET_PARAM > *S_mpPotOfSocketState_X;     // Use for socket usage/statistic monitoring
  BOF_SOCKET_PARAM mBofSocketParam_X;
  BOFSOCKET mSocket;
  BOF_SOCKET_ADDRESS mMulticastIpInterfaceAddress_X; // Set if we want to create a multicast client or server
  BOF_SOCKET_ADDRESS mMulticastIpAddress_X;          // Set if we want to create a multicast client or server

  BOF_SOCKET_ADDRESS mSrcIpAddress_X;
  BOF_SOCKET_ADDRESS mDstIpAddress_X;
  uint32_t mMaxUdpLen_U32;
  bool mConnected_B;
  static std::atomic<int32_t> S_mBofSocketBalance;
  SCATTER_GATHER_BUFFER *mpScatterGatherBuffer_X;

public:
  /// @brief Creates a socket. It must be followed with a call to InitializeSocket to create a 'working' socket
  /// @return  a pointer to the newly created socket (nullptr if error)
  /// @remarks This is the object default constructor
  BofSocket();

  /// @brief Creates a socket that is bound to a specific service provider and associates a local address to it.
  /// @param _rBofSocketParam_X : Specifies the socket initialization parameters
  /// @return  a pointer to the newly created socket (nullptr if error)
  /// @remarks None
  BofSocket(const BOF_SOCKET_PARAM &_rBofSocketParam_X);

  /// @brief Used to instanciate a BofSocket object from an existing socket handle.
  /// @param _Socket : Specifies an existing socket handle returned for example from V_Listen call.
  /// @param _rBofSocketParam_X : Specifies the socket initialization parameters
  /// @return  a pointer to the newly created socket (nullptr if error)
  /// @remarks None
  BofSocket(BOFSOCKET _Socket, const BOF_SOCKET_PARAM &_rBofSocketParam_X);

  /// @brief Delete a previously created BofSocket object. All dynamically allocated resources are released here.
  /// @return  None
  /// @remarks None
  virtual ~BofSocket();

  BofSocket &operator=(const BofSocket &) = delete; // Disallow copying
  BofSocket(const BofSocket &) = delete;

  BOFERR LastErrorCode()
  {
    return mErrorCode_E;
  }

  /// @brief Used to initialize a BofSocket object. Should only be called after creating s BofSocket using the default constructor
  /// @param _rBofSocketParam_X : Specifies the socket initialization parameters
  /// @return  A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
  /// @remarks If this fails, you MUST destroy the object and retry with other parameters
  BOFERR InitializeSocket(const BOF_SOCKET_PARAM &_rBofSocketParam_X);

  // BOFERR                            SetRemoteEndpoint(uint8_t *_pIp_U8, uint16_t _Port_U16);
  // BOFERR                            SetLinkedBofSocketServer(BofSocketServer *_pLinkedSocketServer_O);
  // BOF_SOCKET_ADDRESS										GetRemoteEndpoint();
  // BOFERR														SetRemoteEndpoint(BOF_SOCKET_ADDRESS _Address_X);
  BOFSOCKET GetSocketHandle() const;

  uint32_t GetMaxUdpLen() const;

  bool IsTcp() const;

  bool IsUdp() const;

  BOFERR V_Connect(uint32_t _TimeoutInMs_U32, const std::string &_rTarget_S, const std::string &_rOption_S) override;

  BofComChannel *V_Listen(uint32_t _TimeoutInMs_U32, const std::string &_rOption_S) override;

  BOFERR V_ReadData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, uint8_t *_pBuffer_U8) override;

  BOFERR V_WriteData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, const uint8_t *_pBuffer_U8) override;

  BOFERR V_WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S, uint32_t &_rNb_U32) override;

  BOFERR V_GetStatus(BOF_COM_CHANNEL_STATUS &_rStatus_X) override;

  BOFERR V_FlushData(uint32_t _TimeoutInMs_U32) override;

  BOFERR V_WaitForDataToRead(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPendingByte_U32) override;

  BOFERR WaitForDataToReadOrSpaceToWrite(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPendingByte_U32, bool &_rCanWrite_B);

  BOFERR WriteScatterGatherData(uint32_t _TimeoutInMs_U32, const std::vector<BOF_BUFFER> &_rBufferCollection, uint32_t &_rNbByteWritten_U32);

  BOFERR ReadScatterGatherData(uint32_t _TimeoutInMs_U32, const std::vector<BOF_BUFFER> &_rBufferCollection, uint32_t &_rNbByteRead_U32, bool &_rPartialRead_B);

  BOFERR ReadString(uint32_t _TimeoutInMs_U32, std::string &_rStr_S, char _EolDelimiter_c);

  static BOFERR S_InitializeStack();

  static BOFERR S_ShutdownStack();

  static int S_BofSocketBalance();

  BOF_SOCKET_ADDRESS GetSrcIpAddress();

  BOF_SOCKET_ADDRESS GetDstIpAddress();

  BOFERR SetNonBlockingMode(bool _NonBlocking_B);

  bool IsBlocking();
  static bool S_IsPortFree(uint16_t _Port_U18);
  static uint16_t S_FindFreePort(bool _JustOne_B, uint32_t _PortMin_U32, uint32_t _PortMax_U32);
  BOFERR SetTtl(uint32_t _Ttl_U32);

  BOFERR DisableNagle();

  BOFERR SetSocketBufferSize(uint32_t _RcvBufferSize_U32, uint32_t _SndBufferSize_U32);

  BOFERR SetDstIpAddress(BOF_SOCKET_ADDRESS &_rDstIpAddress_X);

  BOFERR SetWriteTimeout(uint32_t _TimeoutInMs_U32);

  BOFERR SetReadTimeout(uint32_t _TimeoutInMs_U32);

  BOFERR SetReadOrWriteTimeout(uint32_t _TimeoutInMs_U32, bool &_ReadDataPending_B, bool &_SpaceAvailableForWrite_B);

  BOFERR ShutdownSocket();

private:
  BOFERR ComputeScatterGatherList(const std::vector<BOF_BUFFER> &_rBufferCollection, uint32_t _MaxNumberOfScatterGatherEntry_U32, SCATTER_GATHER_BUFFER *_pScatterGatherBuffer_X, uint32_t &_rNbBuffer_U32, uint64_t &_rTotal_U64);

  BOFERR SetupSocket(bool _IpV6_B, bool _Bind_B);

  BOFERR SetTimeoutOption(int _Option_i, uint32_t _TimeoutInMs_U32);

  void SetConnectedState(bool _ConnectedState_B);

  BOFERR SetSendInterfaceForMulticast(const BOF_SOCKET_ADDRESS &_rInterfaceIpAddress_X);
};

END_BOF_NAMESPACE()