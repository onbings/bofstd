/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the  bofiolistener class.
 *
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Jan 05 2019  BHA : Initial release
 */

#pragma once

#include <atomic>

#include <bofstd/bofcomchannel.h>
#include <bofstd/bofsystem.h>

BEGIN_BOF_NAMESPACE()
///
/// The BofIoListener class is an object which can wait on a BofComChannel object and call a virtual function when a connection request is detected.
/// @see BofComChannel
///
class BofIoListener
{
private:
  std::atomic<bool> ListenRunning;
  BofCvSetter<bool> mListenRunningCvSetter = [&](bool _ListenRunning_B) { ListenRunning.store(_ListenRunning_B); };
  BofCvPredicateAndReset<> mListenRunningCvPredicateAndReset = [&]() -> bool { return ListenRunning.load(); };
  BOF_CONDITIONAL_VARIABLE mListenRunningCv_X;
  uint32_t mListenTimeoutInMs_U32 = 0;

public:
  ///
  /// @brief BofIoListener object constructor.
  /// @details This method initializes a default BofIoListener object.
  /// @remark None.
  /// @see ~BofIoListener.
  ///
  BofIoListener();
  ///
  /// @brief BofIoListener object destructor.
  /// @details This method rleases all the resources used by the BofIoListener object.
  /// @remark None.
  /// @see BofIoListener.
  ///
  virtual ~BofIoListener();

  /// @private
  BofIoListener(BofIoListener const &) = delete; // Copy construct
  /// @private
  BofIoListener(BofIoListener &&) = delete; // Move construct
  /// @private
  BofIoListener &operator=(BofIoListener const &) = delete; // Copy assign
  /// @private
  BofIoListener &operator=(BofIoListener &&) = delete; // Move assign

  ///
  /// @brief Listen for incoming connection request.
  /// @details This method blocks for a given amount a time until a connection request is received.
  /// @param[in] _TimeoutInMs_U32 Specifies the maximum time in millisecond allowed to perform the operation. When a connection request is received a V_ConnectionRequest function call will be issued.
  /// If no connection request is received during this period a V_ConnectionRequestTimeout function call will be issued.
  /// @param[in] _rIoChannel Specifies a BofComChannel object which is used to wait for. This oject must have been created by the caller with a listenner attribute (BOF_SOCKET_PARAM.BaseChannelParam_X.ListenBackLog_U32 must be different from 0).
  /// @returns An BOFERR error code describing the status of the call (e.g. BOFERR_NO_ERROR if the operation is successful).
  /// @see CancelListen, V_ConnectionRequest, V_ConnectionRequestTimeout.
  ///
  BOFERR Listen(uint32_t _TimeoutInMs_U32, BofComChannel &_rIoChannel);

  ///
  /// @brief Cancel a running Listen process.
  /// @details This method stops a pending Listen method call. It is mainly used by the destuctor of the oject which could have been called while another thread has just called Listen.
  /// @returns An BOFERR error code describing the status of the call (e.g. BOFERR_NO_ERROR if the operation is successful).
  /// @see Listen.
  ///
  BOFERR CancelListen();
  ///
  /// @brief Connection request virtual method.
  /// @details This virtual method is called by Listen when a connection request is received.
  /// @returns An BOFERR error code describing the status of the call (e.g. BOFERR_NO_ERROR if the operation is successful).
  /// @see Listen, V_ConnectionRequestTimeout.
  ///
  virtual BOFERR V_ConnectionRequest() = 0;
  ///
  /// @brief Connection request timeout virtual method.
  /// @details This virtual method is called by Listen when a connection request timeout occurs.
  /// @returns An BOFERR error code describing the status of the call (e.g. BOFERR_NO_ERROR if the operation is successful).
  /// @see Listen, V_ConnectionRequest.
  ///
  virtual BOFERR V_ConnectionRequestTimeout() = 0;
#if 0
private:
  BOFERR V_Connect(uint32_t _TimeoutInMs_U32, const std::string &_rTarget_S, const std::string &_rOption_S) override;
  BofComChannel *V_Listen(uint32_t _TimeoutInMs_U32, const std::string &_rOption_S) override;
  BOFERR V_ReadData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, uint8_t *_pBuffer_U8) override;
  BOFERR V_WriteData(uint32_t _TimeoutInMs_U32, uint32_t &_rNb_U32, const uint8_t *_pBuffer_U8) override;
  BOFERR V_WriteData(uint32_t _TimeoutInMs_U32, const std::string &_rBuffer_S, uint32_t &_rNb_U32) override;
  BOFERR V_GetStatus(BOF_COM_CHANNEL_STATUS &_rStatus_X) override;
  BOFERR V_FlushData(uint32_t _TimeoutInMs_U32) override;
  BOFERR V_WaitForDataToRead(uint32_t _TimeoutInMs_U32, uint32_t &_rNbPendingByte_U32) override;
#endif

};
END_BOF_NAMESPACE()