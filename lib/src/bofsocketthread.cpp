/*! 
  Copyright (c) 2008-2020, Evs. All rights reserved.

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  The class that manage socket operations on a thread

  Name:        SocketThread.cpp
  Author:      Nicolas Marique : n.marique@evs.tv
  Revision:    1.0

  History:

	V 1.00  February 18 2014  NMA : Initial release
*/
#include <bofstd/bofsocketthread.h>

BEGIN_BOF_NAMESPACE()
BofSocketThread::BofSocketThread(const std::string &_rName_S, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF_THREAD_PRIORITY _ThreadPriority_E) : BofThread()
{
  //	BOFERR Sts_E;

  mSocketOperationParam_X.Reset();
  mSocketOperationResult_X.Reset();
  mExit_B = false;
  mInUse_B = false;
  mDone_B = false;
  mCancel_B = false;
  LaunchBofProcessingThread(_rName_S, false, 0, _ThreadSchedulerPolicy_E, _ThreadPriority_E, 0, 2000, 0);
}

BofSocketThread::~BofSocketThread()
{
  //	StopThread();
}

BOFERR BofSocketThread::ProgramSocketOperation(BOF_SOCKET_OPERATION_PARAM &_rParam_X)
{
  BOFERR Ret_E = BOF_ERR_EINVAL;

  // No operation is currently pending
  if (!mInUse_B)
  {
    // Get the parameters
    mSocketOperationParam_X = _rParam_X;

    mInUse_B = true;
    mDone_B = false;
    mCancel_B = false;
    mExit_B = false;
    // Launch the thread
    Ret_E = SignalThreadWakeUpEvent();
  }
  return Ret_E;
}


bool BofSocketThread::IsOperationDone()
{
  return mDone_B;
}


bool BofSocketThread::IsOperationPending()
{
  return mInUse_B;
}


BOFERR BofSocketThread::CancelOperation()
{
  BOFERR Rts_E = BOF_ERR_NOT_RUNNING;

  if (IsOperationPending())
  {
    mCancel_B = true;
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}


BOFERR BofSocketThread::GetParams(BOF_SOCKET_OPERATION_PARAM &_rParam_X)
{
  BOFERR Rts_E;

  _rParam_X = mSocketOperationParam_X;
  Rts_E = BOF_ERR_NO_ERROR;
  return Rts_E;
}

BOFERR BofSocketThread::GetResult(BOF_SOCKET_OPERATION_RESULT &_rResult_X)
{
  BOFERR Rts_E;

  _rResult_X = mSocketOperationResult_X;
  Rts_E = BOF_ERR_NO_ERROR;
  return Rts_E;
}

BOFERR BofSocketThread::ClearOperation()
{
  BOFERR Rts_E;

  mSocketOperationParam_X.Reset();
  mSocketOperationResult_X.Reset();
  mInUse_B = false;
  mDone_B = false;
  mCancel_B = false;
  Rts_E = BOF_ERR_NO_ERROR;
  return Rts_E;
}

BOFERR BofSocketThread::V_OnProcessing()
{
  uint32_t Size_U32;
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  uint32_t Total_U32;
  uint8_t *pBuf_U8;
  bool IsBlockingMode_B;

  while (!IsThreadLoopMustExit())
  {
    Rts_E = WaitForThreadWakeUpEvent(0xFFFFFFFF);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (!IsThreadLoopMustExit())
      {
        // Grab parameters
        Size_U32 = mSocketOperationParam_X.Size_U32;
        pBuf_U8 = mSocketOperationParam_X.pBuffer_U8;
        Total_U32 = 0;

        // Set socket to blocking mode
        IsBlockingMode_B = mSocketOperationParam_X.pSocket_O->IsBlocking();
        if (!IsBlockingMode_B)
        {
          mSocketOperationParam_X.pSocket_O->SetNonBlockingMode(false);
        }
        do
        {
          if (mSocketOperationParam_X.Read_B)
          {
            Rts_E = mSocketOperationParam_X.pSocket_O->V_ReadData(100, Size_U32, pBuf_U8);
          }
          else
          {
            Rts_E = mSocketOperationParam_X.pSocket_O->V_WriteData(100, Size_U32, pBuf_U8);
          }
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            Total_U32 += Size_U32;
            pBuf_U8 += Size_U32;
            Size_U32 = (mSocketOperationParam_X.Size_U32 - Total_U32);
          }
          if (Rts_E == BOF_ERR_ETIMEDOUT)
          {
            Rts_E = BOF_ERR_NO_ERROR;
          }

        } while (!mExit_B && !mCancel_B && (Total_U32 < mSocketOperationParam_X.Size_U32) && (Rts_E == BOF_ERR_NO_ERROR));

        if (!IsBlockingMode_B)
        {
          mSocketOperationParam_X.pSocket_O->SetNonBlockingMode(true);
        }

        // Act that operation was cancelled
        if (mCancel_B)
        {
          Rts_E = BOF_ERR_STOPPED;
        }

        // Set results
        mSocketOperationResult_X.Sts_E = Rts_E;
        mSocketOperationResult_X.Size_U32 = Total_U32;

        // Flag it as done
        mDone_B = true;
      }
    }
  }

  return Rts_E;
}
END_BOF_NAMESPACE()


