/*
 * Copyright (c) 2020-2040, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines routines for creating and managing a multiple producer-multiple consumer queue
 * It is based on ythe well known cameron314/concurrentqueue https://github.com/cameron314/concurrentqueue
 *
 * History:
 *
 * V 1.00  May 26 2020  BHA : Initial release
 */
#pragma once

#include <bofstd/bofstd.h>
//#include <concurrentqueue/concurrentqueue.h>
#include <concurrentqueue/blockingconcurrentqueue.h>

BEGIN_BOF_NAMESPACE()
struct BOF_MPMC_QUEUE_PARAM
{
  uint32_t NbMaxElement_U32; /*! Specifies the maximum number of element inside the queue*/

  BOF_MPMC_QUEUE_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    NbMaxElement_U32 = 0;
  }
};

template <typename DataType>
class BofMpMcQueue
{
private:
  BOF_MPMC_QUEUE_PARAM mMpMcQueueParam_X;
  std::unique_ptr<moodycamel::BlockingConcurrentQueue<DataType>> mpuMpMcQueue = nullptr;

public:
  BofMpMcQueue(const BOF_MPMC_QUEUE_PARAM &_rMpMcQueueParam_X)
  {
    mMpMcQueueParam_X = _rMpMcQueueParam_X;
    mpuMpMcQueue = std::make_unique<moodycamel::BlockingConcurrentQueue<DataType>>(mMpMcQueueParam_X.NbMaxElement_U32);
  }
  virtual ~BofMpMcQueue()
  {

  }
  BOFERR Push(const DataType &_rData, uint32_t _TimeoutInMs_U32)
  {
    BOFERR Rts_E = BOF_ERR_FULL;
    if (mpuMpMcQueue->try_enqueue(std::move(_rData))) //, _TimeoutInMs_U32 * 1000))
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
    return Rts_E;
  }
  BOFERR Pop(DataType &_rData, uint32_t _TimeoutInMs_U32)
  {
    BOFERR Rts_E = BOF_ERR_EMPTY;
    if (mpuMpMcQueue->wait_dequeue_timed(_rData, _TimeoutInMs_U32 * 1000))
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
    return Rts_E;
  }
};
END_BOF_NAMESPACE()

/*
# Allocates more memory if necessary
enqueue(item) : bool
enqueue(prod_token, item) : bool
enqueue_bulk(item_first, count) : bool
enqueue_bulk(prod_token, item_first, count) : bool

# Fails if not enough memory to enqueue
try_enqueue(item) : bool
try_enqueue(prod_token, item) : bool
try_enqueue_bulk(item_first, count) : bool
try_enqueue_bulk(prod_token, item_first, count) : bool

# Attempts to dequeue from the queue (never allocates)
try_dequeue(item&) : bool
try_dequeue(cons_token, item&) : bool
try_dequeue_bulk(item_first, max) : size_t
try_dequeue_bulk(cons_token, item_first, max) : size_t

# If you happen to know which producer you want to dequeue from
try_dequeue_from_producer(prod_token, item&) : bool
try_dequeue_bulk_from_producer(prod_token, item_first, max) : size_t

# A not-necessarily-accurate count of the total number of elements
size_approx() : size_t
*/