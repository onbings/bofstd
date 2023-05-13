/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the pot class
 *
 * Name:        ut_queue.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */
#include <bofstd/bofqueue.h>
#include <bofstd/bofsystem.h>

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

// TestType tracks correct usage of constructors and destructors

struct TestType
{
  static std::set<const TestType *> constructed;
  TestType() noexcept
  {
    assert(constructed.count(this) == 0);
    constructed.insert(this);
  };
  TestType(const TestType &other) noexcept
  {
    assert(constructed.count(this) == 0);
    assert(constructed.count(&other) == 1);
    constructed.insert(this);
  };
  TestType(TestType &&other) noexcept
  {
    assert(constructed.count(this) == 0);
    assert(constructed.count(&other) == 1);
    constructed.insert(this);
  };
  TestType &operator=(const TestType &other) noexcept
  {
    assert(constructed.count(this) == 1);
    assert(constructed.count(&other) == 1);
    return *this;
  };
  TestType &operator=(TestType &&other) noexcept
  {
    assert(constructed.count(this) == 1);
    assert(constructed.count(&other) == 1);
    return *this;
  }
  ~TestType() noexcept
  {
    assert(constructed.count(this) == 1);
    constructed.erase(this);
  };
};

std::set<const TestType *> TestType::constructed;
/*** Factory functions called at the beginning/end of each test case **********/

TEST(Queue_Test, Queue)
{
  BOFERR Sts_E;
  BofQueue<uint32_t> Queue(8, true);
  uint32_t i_U32, Size_U32, Val_U32;
  bool Sts_B;

  Sts_B = Queue.IsEmpty();
  EXPECT_EQ(Sts_B, true);
  for (i_U32 = 0; i_U32 < Queue.Capacity(); i_U32++)
  {
    Sts_E = Queue.Push(100, i_U32);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    Size_U32 = Queue.Size();
    EXPECT_EQ(Size_U32, i_U32 + 1);
    Sts_B = Queue.IsFull();
    EXPECT_EQ(Sts_B, (i_U32 == Queue.Capacity() - 1));
    Sts_B = Queue.IsEmpty();
    EXPECT_EQ(Sts_B, false);
  }
  Val_U32 = Queue.Capacity();
  Sts_E = Queue.Push(100, Val_U32);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Queue.Pop(100, Val_U32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Val_U32, 0);
  Size_U32 = Queue.Size();
  EXPECT_EQ(Size_U32, Queue.Capacity() - 1);

  Val_U32 = Queue.Capacity();
  Sts_E = Queue.Push(100, Val_U32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_B = Queue.IsFull();
  EXPECT_EQ(Sts_B, true);
  for (i_U32 = 0; i_U32 < Queue.Capacity(); i_U32++)
  {
    Sts_E = Queue.Pop(100, Val_U32);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(Val_U32, i_U32 + 1);
    Size_U32 = Queue.Size();
    EXPECT_EQ(Size_U32, Queue.Capacity() - i_U32 - 1);
    Sts_B = Queue.IsFull();
    EXPECT_EQ(Sts_B, false);
    Sts_B = Queue.IsEmpty();
    EXPECT_EQ(Sts_B, (i_U32 == Queue.Capacity() - 1));
  }
}
