/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the IBofSocketIo interface.
 *
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			   onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Jan 05 2019  BHA : Initial release
 */
#pragma once

#include <bofstd/bofsocket.h>

BEGIN_BOF_NAMESPACE()

struct BOF_SOCKET_OPERATION_PARAM
{
  BofSocket *pSocket_O; /*! The pointer to the socket object */
  bool Read_B;          /*! Read data from the socket (write otherwise) */
  uint32_t Size_U32;    /*! The number of bytes to transfer */
  uint8_t *pBuffer_U8;  /*! The local data buffer */
  BOF_SOCKET_OPERATION_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    pSocket_O = nullptr;
    Read_B = false;
    Size_U32 = 0;
    pBuffer_U8 = nullptr;
  }
};

struct BOF_SOCKET_OPERATION_RESULT
{
  BOFERR Sts_E;       /*! The operation status */
  uint32_t Size_U32;  /*! The total size transferred in bytes */
  uint32_t Timer_U32; /*! The operation time in ms */
  BOF_SOCKET_OPERATION_RESULT()
  {
    Reset();
  }

  void Reset()
  {
    Sts_E = BOF_ERR_NO_ERROR;
    Size_U32 = 0;
    Timer_U32 = 0;
  }
};

class BOFSTD_EXPORT BofSocketThread : public BofThread
{
private:
  BOF_SOCKET_OPERATION_PARAM mSocketOperationParam_X;   /*! The operation params */
  BOF_SOCKET_OPERATION_RESULT mSocketOperationResult_X; /*! The operation results */
  bool mExit_B;                                         /*! The exit flag */
  bool mInUse_B;                                        /*! An operation is currently pending */
  bool mDone_B;                                         /*! The flag indicating the operation is done */
  bool mCancel_B;                                       /*! The operation was cancelled */

public:
  BofSocketThread(const std::string &_rName_S, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF_THREAD_PRIORITY _ThreadPriority_E);
  virtual ~BofSocketThread();

  BOFERR ProgramSocketOperation(BOF_SOCKET_OPERATION_PARAM &_rParam_X);
  bool IsOperationDone();
  bool IsOperationPending();
  BOFERR GetParams(BOF_SOCKET_OPERATION_PARAM &_rParam_X);
  BOFERR GetResult(BOF_SOCKET_OPERATION_RESULT &_rResult_X);

  BOFERR CancelOperation();
  BOFERR ClearOperation();

protected:
  BOFERR V_OnProcessing() override;
};

END_BOF_NAMESPACE()
