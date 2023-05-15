/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the asyncmulticastdelegate class
 *
 * Name:        ut_asyncmulticastdelegate.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#include <asyncmulticastdelegate/DelegateLib.h>
#include <asyncmulticastdelegate/xallocator.h>
#include <bofstd/bofasync.h>

#include "gtestrunner.h"

#include <iostream>

using namespace DelegateLib;

static const int32_t TEST_INT = 12345678;

struct StructParam
{
  int32_t val;
};
int32_t FreeFuncIntWithReturn0()
{
  return TEST_INT;
}

void FreeFunc0()
{
}

void FreeFuncInt1(int32_t i)
{
  EXPECT_TRUE(i == TEST_INT);
}
int32_t FreeFuncIntWithReturn1(int32_t i)
{
  EXPECT_TRUE(i == TEST_INT);
  return i;
}
void FreeFuncPtrPtr1(StructParam **s)
{
  EXPECT_TRUE((*s)->val == TEST_INT);
}
void FreeFuncStruct1(StructParam s)
{
  EXPECT_TRUE(s.val == TEST_INT);
}
void FreeFuncStructPtr1(StructParam *s)
{
  EXPECT_TRUE(s->val == TEST_INT);
}
void FreeFuncStructConstPtr1(const StructParam *s)
{
  EXPECT_TRUE(s->val == TEST_INT);
}
void FreeFuncStructRef1(StructParam &s)
{
  EXPECT_TRUE(s.val == TEST_INT);
}
void FreeFuncStructConstRef1(const StructParam &s)
{
  EXPECT_TRUE(s.val == TEST_INT);
}

void FreeFuncInt2(int32_t i, int32_t i2)
{
  EXPECT_TRUE(i == TEST_INT);
  EXPECT_TRUE(i2 == TEST_INT);
}
int32_t FreeFuncIntWithReturn2(int32_t i, int32_t i2)
{
  EXPECT_TRUE(i == TEST_INT);
  return i;
}
void FreeFuncPtrPtr2(StructParam **s, int32_t i)
{
  EXPECT_TRUE((*s)->val == TEST_INT);
}
void FreeFuncStruct2(StructParam s, int32_t i)
{
  EXPECT_TRUE(s.val == TEST_INT);
}
void FreeFuncStructPtr2(StructParam *s, int32_t i)
{
  EXPECT_TRUE(s->val == TEST_INT);
}
void FreeFuncStructConstPtr2(const StructParam *s, int32_t i)
{
  EXPECT_TRUE(s->val == TEST_INT);
}
void FreeFuncStructRef2(StructParam &s, int32_t i)
{
  EXPECT_TRUE(s.val == TEST_INT);
}
void FreeFuncStructConstRef2(const StructParam &s, int32_t i)
{
  EXPECT_TRUE(s.val == TEST_INT);
}

void FreeFuncInt3(int32_t i, int32_t i2, int32_t i3)
{
  EXPECT_TRUE(i == TEST_INT);
  EXPECT_TRUE(i2 == TEST_INT);
  EXPECT_TRUE(i3 == TEST_INT);
}
int32_t FreeFuncIntWithReturn3(int32_t i, int32_t i2, int32_t i3)
{
  EXPECT_TRUE(i == TEST_INT);
  return i;
}
void FreeFuncPtrPtr3(StructParam **s, int32_t i, int32_t i2)
{
  EXPECT_TRUE((*s)->val == TEST_INT);
}
void FreeFuncStruct3(StructParam s, int32_t i, int32_t i2)
{
  EXPECT_TRUE(s.val == TEST_INT);
}
void FreeFuncStructPtr3(StructParam *s, int32_t i, int32_t i2)
{
  EXPECT_TRUE(s->val == TEST_INT);
}
void FreeFuncStructConstPtr3(const StructParam *s, int32_t i, int32_t i2)
{
  EXPECT_TRUE(s->val == TEST_INT);
}
void FreeFuncStructRef3(StructParam &s, int32_t i, int32_t i2)
{
  EXPECT_TRUE(s.val == TEST_INT);
}
void FreeFuncStructConstRef3(const StructParam &s, int32_t i, int32_t i2)
{
  EXPECT_TRUE(s.val == TEST_INT);
}

void FreeFuncInt4(int32_t i, int32_t i2, int32_t i3, int32_t i4)
{
  EXPECT_TRUE(i == TEST_INT);
  EXPECT_TRUE(i2 == TEST_INT);
  EXPECT_TRUE(i3 == TEST_INT);
}
int32_t FreeFuncIntWithReturn4(int32_t i, int32_t i2, int32_t i3, int32_t i4)
{
  EXPECT_TRUE(i == TEST_INT);
  return i;
}
void FreeFuncPtrPtr4(StructParam **s, int32_t i, int32_t i2, int32_t i3)
{
  EXPECT_TRUE((*s)->val == TEST_INT);
}
void FreeFuncStruct4(StructParam s, int32_t i, int32_t i2, int32_t i3)
{
  EXPECT_TRUE(s.val == TEST_INT);
}
void FreeFuncStructPtr4(StructParam *s, int32_t i, int32_t i2, int32_t i3)
{
  EXPECT_TRUE(s->val == TEST_INT);
}
void FreeFuncStructConstPtr4(const StructParam *s, int32_t i, int32_t i2, int32_t i3)
{
  EXPECT_TRUE(s->val == TEST_INT);
}
void FreeFuncStructRef4(StructParam &s, int32_t i, int32_t i2, int32_t i3)
{
  EXPECT_TRUE(s.val == TEST_INT);
}
void FreeFuncStructConstRef4(const StructParam &s, int32_t i, int32_t i2, int32_t i3)
{
  EXPECT_TRUE(s.val == TEST_INT);
}

void FreeFuncInt5(int32_t i, int32_t i2, int32_t i3, int32_t i4, int32_t i5)
{
  EXPECT_TRUE(i == TEST_INT);
  EXPECT_TRUE(i2 == TEST_INT);
  EXPECT_TRUE(i3 == TEST_INT);
}
int32_t FreeFuncIntWithReturn5(int32_t i, int32_t i2, int32_t i3, int32_t i4, int32_t i5)
{
  EXPECT_TRUE(i == TEST_INT);
  return i;
}

void FreeFuncPtrPtr5(StructParam **s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
{
  EXPECT_TRUE((*s)->val == TEST_INT);
}
void FreeFuncStruct5(StructParam s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
{
  EXPECT_TRUE(s.val == TEST_INT);
}
void FreeFuncStructPtr5(StructParam *s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
{
  EXPECT_TRUE(s->val == TEST_INT);
}
void FreeFuncStructConstPtr5(const StructParam *s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
{
  EXPECT_TRUE(s->val == TEST_INT);
}
void FreeFuncStructRef5(StructParam &s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
{
  EXPECT_TRUE(s.val == TEST_INT);
}
void FreeFuncStructConstRef5(const StructParam &s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
{
  EXPECT_TRUE(s.val == TEST_INT);
}

class TestClass0
{
public:
  void MemberFunc0()
  {
  }
  void MemberFunc0Const() const
  {
  }
  int32_t MemberFuncWithReturn0()
  {
    return TEST_INT;
  }

  static void StaticFunc0()
  {
  }
};

class TestClass1
{
public:
  void MemberFuncInt1(int32_t i)
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  void MemberFuncInt1Const(int32_t i) const
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  int32_t MemberFuncIntWithReturn1(int32_t i)
  {
    EXPECT_TRUE(i == TEST_INT);
    return i;
  }
  void MemberFuncStruct1(StructParam s)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  void MemberFuncStructPtr1(StructParam *s)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  void MemberFuncStructPtrPtr1(StructParam **s)
  {
    EXPECT_TRUE((*s)->val == TEST_INT);
  }
  void MemberFuncStructConstPtr1(const StructParam *s)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  void MemberFuncStructRef1(StructParam &s)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  void MemberFuncStructConstRef1(const StructParam &s)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  void MemberFuncStructConstPtr1Bha(const StructParam *s)
  {
    //    if ((s->val % 1000) == 0)
    //    { printf("1: %d\n", s->val); }
  }
  void MemberFuncStructConstPtr2Bha(const StructParam *s)
  {
    //    if ((s->val % 1000) == 0)
    //    { printf("2: %d\n", s->val); }
  }

  static void StaticFuncInt1(int32_t i)
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  static void StaticFuncStruct1(StructParam s)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  static void StaticFuncStructPtr1(StructParam *s)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  static void StaticFuncStructConstPtr1(const StructParam *s)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  static void StaticFuncStructRef1(StructParam &s)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  static void StaticFuncStructConstRef1(const StructParam &s)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
};

class TestClass2
{
public:
  void MemberFuncInt2(int32_t i, int32_t i2)
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  void MemberFuncInt2Const(int32_t i, int32_t i2) const
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  int32_t MemberFuncIntWithReturn2(int32_t i, int32_t i2)
  {
    EXPECT_TRUE(i == TEST_INT);
    return i;
  }
  void MemberFuncStruct2(StructParam s, int32_t i)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  void MemberFuncStructPtr2(StructParam *s, int32_t i)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  void MemberFuncStructPtrPtr2(StructParam **s, int32_t i)
  {
    EXPECT_TRUE((*s)->val == TEST_INT);
  }
  void MemberFuncStructConstPtr2(const StructParam *s, int32_t i)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  void MemberFuncStructRef2(StructParam &s, int32_t i)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  void MemberFuncStructConstRef2(const StructParam &s, int32_t i)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }

  static void StaticFuncInt2(int32_t i, int32_t i2)
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  static void StaticFuncStruct2(StructParam s, int32_t i)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  static void StaticFuncStructPtr2(StructParam *s, int32_t i)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  static void StaticFuncStructConstPtr2(const StructParam *s, int32_t i)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  static void StaticFuncStructRef2(StructParam &s, int32_t i)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  static void StaticFuncStructConstRef2(const StructParam &s, int32_t i)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
};

class TestClass3
{
public:
  void MemberFuncInt3(int32_t i, int32_t i2, int32_t i3)
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  void MemberFuncInt3Const(int32_t i, int32_t i2, int32_t i3) const
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  int32_t MemberFuncIntWithReturn3(int32_t i, int32_t i2, int32_t i3)
  {
    EXPECT_TRUE(i == TEST_INT);
    return i;
  }
  void MemberFuncStruct3(StructParam s, int32_t i, int32_t i2)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  void MemberFuncStructPtr3(StructParam *s, int32_t i, int32_t i2)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  void MemberFuncStructPtrPtr3(StructParam **s, int32_t i, int32_t i2)
  {
    EXPECT_TRUE((*s)->val == TEST_INT);
  }
  void MemberFuncStructConstPtr3(const StructParam *s, int32_t i, int32_t i2)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  void MemberFuncStructRef3(StructParam &s, int32_t i, int32_t i2)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  void MemberFuncStructConstRef3(const StructParam &s, int32_t i, int32_t i2)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }

  static void StaticFuncInt3(int32_t i, int32_t i2, int32_t i3)
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  static void StaticFuncStruct3(StructParam s, int32_t i, int32_t i2)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  static void StaticFuncStructPtr3(StructParam *s, int32_t i, int32_t i2)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  static void StaticFuncStructConstPtr3(const StructParam *s, int32_t i, int32_t i2)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  static void StaticFuncStructRef3(StructParam &s, int32_t i, int32_t i2)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  static void StaticFuncStructConstRef3(const StructParam &s, int32_t i, int32_t i2)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
};

class TestClass4
{
public:
  void MemberFuncInt4(int32_t i, int32_t i2, int32_t i3, int32_t i4)
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  void MemberFuncInt4Const(int32_t i, int32_t i2, int32_t i3, int32_t i4) const
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  int32_t MemberFuncIntWithReturn4(int32_t i, int32_t i2, int32_t i3, int32_t i4)
  {
    EXPECT_TRUE(i == TEST_INT);
    return i;
  }
  void MemberFuncStruct4(StructParam s, int32_t i, int32_t i2, int32_t i3)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  void MemberFuncStructPtr4(StructParam *s, int32_t i, int32_t i2, int32_t i3)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  void MemberFuncStructPtrPtr4(StructParam **s, int32_t i, int32_t i2, int32_t i3)
  {
    EXPECT_TRUE((*s)->val == TEST_INT);
  }
  void MemberFuncStructConstPtr4(const StructParam *s, int32_t i, int32_t i2, int32_t i3)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  void MemberFuncStructRef4(StructParam &s, int32_t i, int32_t i2, int32_t i3)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  void MemberFuncStructConstRef4(const StructParam &s, int32_t i, int32_t i2, int32_t i3)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }

  static void StaticFuncInt4(int32_t i, int32_t i2, int32_t i3, int32_t i4)
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  static void StaticFuncStruct4(StructParam s, int32_t i, int32_t i2, int32_t i3)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  static void StaticFuncStructPtr4(StructParam *s, int32_t i, int32_t i2, int32_t i3)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  static void StaticFuncStructConstPtr4(const StructParam *s, int32_t i, int32_t i2, int32_t i3)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  static void StaticFuncStructRef4(StructParam &s, int32_t i, int32_t i2, int32_t i3)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  static void StaticFuncStructConstRef4(const StructParam &s, int32_t i, int32_t i2, int32_t i3)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
};

class TestClass5
{
public:
  void MemberFuncInt5(int32_t i, int32_t i2, int32_t i3, int32_t i4, int32_t i5)
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  void MemberFuncInt5Const(int32_t i, int32_t i2, int32_t i3, int32_t i4, int32_t i5) const
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  int32_t MemberFuncIntWithReturn5(int32_t i, int32_t i2, int32_t i3, int32_t i4, int32_t i5)
  {
    EXPECT_TRUE(i == TEST_INT);
    return i;
  }
  void MemberFuncStruct5(StructParam s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  void MemberFuncStructPtr5(StructParam *s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  void MemberFuncStructPtrPtr5(StructParam **s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
  {
    EXPECT_TRUE((*s)->val == TEST_INT);
  }
  void MemberFuncStructConstPtr5(const StructParam *s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  void MemberFuncStructRef5(StructParam &s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  void MemberFuncStructConstRef5(const StructParam &s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }

  static void StaticFuncInt5(int32_t i, int32_t i2, int32_t i3, int32_t i4, int32_t i5)
  {
    EXPECT_TRUE(i == TEST_INT);
  }
  static void StaticFuncStruct5(StructParam s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  static void StaticFuncStructPtr5(StructParam *s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  static void StaticFuncStructConstPtr5(const StructParam *s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
  {
    EXPECT_TRUE(s->val == TEST_INT);
  }
  static void StaticFuncStructRef5(StructParam &s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
  static void StaticFuncStructConstRef5(const StructParam &s, int32_t i, int32_t i2, int32_t i3, int32_t i4)
  {
    EXPECT_TRUE(s.val == TEST_INT);
  }
};

void SinglecastDelegateTests()
{
  StructParam structParam;
  structParam.val = TEST_INT;
  StructParam *pStructParam = &structParam;

  // N=0 Free Functions
  SinglecastDelegate0<> FreeFunc0SinglecastDelegate;
  EXPECT_TRUE(FreeFunc0SinglecastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFunc0SinglecastDelegate);
  FreeFunc0SinglecastDelegate = MakeDelegate(&FreeFunc0);
  EXPECT_TRUE(FreeFunc0SinglecastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFunc0SinglecastDelegate);
  FreeFunc0SinglecastDelegate();
  FreeFunc0SinglecastDelegate.Clear();
  EXPECT_TRUE(!FreeFunc0SinglecastDelegate);

  SinglecastDelegate0<int32_t> FreeFuncIntWithReturn0SinglecastDelegate;
  FreeFuncIntWithReturn0SinglecastDelegate = MakeDelegate(&FreeFuncIntWithReturn0);
  EXPECT_TRUE(FreeFuncIntWithReturn0SinglecastDelegate() == TEST_INT);

  // N=0 Member Functions
  TestClass0 testClass0;

  SinglecastDelegate0<> MemberFunc0SinglecastDelegate;
  EXPECT_TRUE(MemberFunc0SinglecastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFunc0SinglecastDelegate);
  MemberFunc0SinglecastDelegate = MakeDelegate(&testClass0, &TestClass0::MemberFunc0);
  EXPECT_TRUE(MemberFunc0SinglecastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFunc0SinglecastDelegate);
  MemberFunc0SinglecastDelegate();
  MemberFunc0SinglecastDelegate.Clear();
  EXPECT_TRUE(!MemberFunc0SinglecastDelegate);

  SinglecastDelegate0<int32_t> MemberFuncIntWithReturn0SinglecastDelegate;
  MemberFuncIntWithReturn0SinglecastDelegate = MakeDelegate(&testClass0, &TestClass0::MemberFuncWithReturn0);
  EXPECT_TRUE(MemberFuncIntWithReturn0SinglecastDelegate() == TEST_INT);

  // N=0 Static Functions
  SinglecastDelegate0<> StaticFunc0SinglecastDelegate;
  StaticFunc0SinglecastDelegate = MakeDelegate(&TestClass0::StaticFunc0);
  StaticFunc0SinglecastDelegate();

  // N=1 Free Functions
  SinglecastDelegate1<int32_t> FreeFuncInt1SinglecastDelegate;
  EXPECT_TRUE(FreeFuncInt1SinglecastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt1SinglecastDelegate);
  FreeFuncInt1SinglecastDelegate = MakeDelegate(&FreeFuncInt1);
  EXPECT_TRUE(FreeFuncInt1SinglecastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt1SinglecastDelegate);
  FreeFuncInt1SinglecastDelegate(TEST_INT);
  FreeFuncInt1SinglecastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt1SinglecastDelegate);

  SinglecastDelegate1<int32_t, int32_t> FreeFuncIntWithReturn1SinglecastDelegate;
  FreeFuncIntWithReturn1SinglecastDelegate = MakeDelegate(&FreeFuncIntWithReturn1);
  EXPECT_TRUE(FreeFuncIntWithReturn1SinglecastDelegate(TEST_INT) == TEST_INT);

  SinglecastDelegate1<StructParam **> FreeFuncPtrPtr1SinglecastDelegate;
  FreeFuncPtrPtr1SinglecastDelegate = MakeDelegate(&FreeFuncPtrPtr1);
  FreeFuncPtrPtr1SinglecastDelegate(&pStructParam);

  SinglecastDelegate1<StructParam> FreeFuncStruct1SinglecastDelegate;
  FreeFuncStruct1SinglecastDelegate = MakeDelegate(&FreeFuncStruct1);
  FreeFuncStruct1SinglecastDelegate(structParam);

  SinglecastDelegate1<StructParam *> FreeFuncStructPtr1SinglecastDelegate;
  FreeFuncStructPtr1SinglecastDelegate = MakeDelegate(&FreeFuncStructPtr1);
  FreeFuncStructPtr1SinglecastDelegate(&structParam);

  SinglecastDelegate1<const StructParam *> FreeFuncStructConstPtr1SinglecastDelegate;
  FreeFuncStructConstPtr1SinglecastDelegate = MakeDelegate(&FreeFuncStructConstPtr1);
  FreeFuncStructConstPtr1SinglecastDelegate(&structParam);

  SinglecastDelegate1<StructParam &> FreeFuncStructRef1SinglecastDelegate;
  FreeFuncStructRef1SinglecastDelegate = MakeDelegate(&FreeFuncStructRef1);
  FreeFuncStructRef1SinglecastDelegate(structParam);

  SinglecastDelegate1<const StructParam &> FreeFuncStructConstRef1SinglecastDelegate;
  FreeFuncStructConstRef1SinglecastDelegate = MakeDelegate(&FreeFuncStructConstRef1);
  FreeFuncStructConstRef1SinglecastDelegate(structParam);

  // N=1 Member Functions
  TestClass1 testClass1;

  SinglecastDelegate1<int32_t> MemberFuncInt1SinglecastDelegate;
  EXPECT_TRUE(MemberFuncInt1SinglecastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt1SinglecastDelegate);
  MemberFuncInt1SinglecastDelegate = MakeDelegate(&testClass1, &TestClass1::MemberFuncInt1);
  EXPECT_TRUE(MemberFuncInt1SinglecastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt1SinglecastDelegate);
  MemberFuncInt1SinglecastDelegate(TEST_INT);
  MemberFuncInt1SinglecastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt1SinglecastDelegate);

  SinglecastDelegate1<int32_t, int32_t> MemberFuncIntWithReturn1SinglecastDelegate;
  MemberFuncIntWithReturn1SinglecastDelegate = MakeDelegate(&testClass1, &TestClass1::MemberFuncIntWithReturn1);
  EXPECT_TRUE(MemberFuncIntWithReturn1SinglecastDelegate(TEST_INT) == TEST_INT);

  SinglecastDelegate1<StructParam> MemberFuncStruct1SinglecastDelegate;
  MemberFuncStruct1SinglecastDelegate = MakeDelegate(&testClass1, &TestClass1::MemberFuncStruct1);
  MemberFuncStruct1SinglecastDelegate(structParam);

  SinglecastDelegate1<StructParam *> MemberFuncStructPtr1SinglecastDelegate;
  MemberFuncStructPtr1SinglecastDelegate = MakeDelegate(&testClass1, &TestClass1::MemberFuncStructPtr1);
  MemberFuncStructPtr1SinglecastDelegate(&structParam);

  SinglecastDelegate1<const StructParam *> MemberFuncStructConstPtr1SinglecastDelegate;
  MemberFuncStructConstPtr1SinglecastDelegate = MakeDelegate(&testClass1, &TestClass1::MemberFuncStructConstPtr1);
  MemberFuncStructConstPtr1SinglecastDelegate(&structParam);

  SinglecastDelegate1<StructParam &> MemberFuncStructRef1SinglecastDelegate;
  MemberFuncStructRef1SinglecastDelegate = MakeDelegate(&testClass1, &TestClass1::MemberFuncStructRef1);
  MemberFuncStructRef1SinglecastDelegate(structParam);

  SinglecastDelegate1<const StructParam &> MemberFuncStructConstRef1SinglecastDelegate;
  MemberFuncStructConstRef1SinglecastDelegate = MakeDelegate(&testClass1, &TestClass1::MemberFuncStructConstRef1);
  MemberFuncStructConstRef1SinglecastDelegate(structParam);

  // N=1 Static Functions
  SinglecastDelegate1<int32_t> StaticFuncInt1SinglecastDelegate;
  StaticFuncInt1SinglecastDelegate = MakeDelegate(&TestClass1::StaticFuncInt1);
  StaticFuncInt1SinglecastDelegate(TEST_INT);

  SinglecastDelegate1<StructParam> StaticFuncStruct1SinglecastDelegate;
  StaticFuncStruct1SinglecastDelegate = MakeDelegate(&TestClass1::StaticFuncStruct1);
  StaticFuncStruct1SinglecastDelegate(structParam);

  SinglecastDelegate1<StructParam *> StaticFuncStructPtr1SinglecastDelegate;
  StaticFuncStructPtr1SinglecastDelegate = MakeDelegate(&TestClass1::StaticFuncStructPtr1);
  StaticFuncStructPtr1SinglecastDelegate(&structParam);

  SinglecastDelegate1<const StructParam *> StaticFuncStructConstPtr1SinglecastDelegate;
  StaticFuncStructConstPtr1SinglecastDelegate = MakeDelegate(&TestClass1::StaticFuncStructConstPtr1);
  StaticFuncStructConstPtr1SinglecastDelegate(&structParam);

  SinglecastDelegate1<StructParam &> StaticFuncStructRef1SinglecastDelegate;
  StaticFuncStructRef1SinglecastDelegate = MakeDelegate(&TestClass1::StaticFuncStructRef1);
  StaticFuncStructRef1SinglecastDelegate(structParam);

  SinglecastDelegate1<const StructParam &> StaticFuncStructConstRef1SinglecastDelegate;
  StaticFuncStructConstRef1SinglecastDelegate = MakeDelegate(&TestClass1::StaticFuncStructConstRef1);
  StaticFuncStructConstRef1SinglecastDelegate(structParam);

  // N=2 Free Functions
  SinglecastDelegate2<int32_t, int32_t> FreeFuncInt2SinglecastDelegate;
  EXPECT_TRUE(FreeFuncInt2SinglecastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt2SinglecastDelegate);
  FreeFuncInt2SinglecastDelegate = MakeDelegate(&FreeFuncInt2);
  EXPECT_TRUE(FreeFuncInt2SinglecastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt2SinglecastDelegate);
  FreeFuncInt2SinglecastDelegate(TEST_INT, TEST_INT);
  FreeFuncInt2SinglecastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt2SinglecastDelegate);

  SinglecastDelegate2<int32_t, int32_t, int32_t> FreeFuncIntWithReturn2SinglecastDelegate;
  FreeFuncIntWithReturn2SinglecastDelegate = MakeDelegate(&FreeFuncIntWithReturn2);
  EXPECT_TRUE(FreeFuncIntWithReturn2SinglecastDelegate(TEST_INT, TEST_INT) == TEST_INT);

  SinglecastDelegate2<StructParam **, int32_t> FreeFuncPtrPtr2SinglecastDelegate;
  FreeFuncPtrPtr2SinglecastDelegate = MakeDelegate(&FreeFuncPtrPtr2);
  FreeFuncPtrPtr2SinglecastDelegate(&pStructParam, TEST_INT);

  SinglecastDelegate2<StructParam, int32_t> FreeFuncStruct2SinglecastDelegate;
  FreeFuncStruct2SinglecastDelegate = MakeDelegate(&FreeFuncStruct2);
  FreeFuncStruct2SinglecastDelegate(structParam, TEST_INT);

  SinglecastDelegate2<StructParam *, int32_t> FreeFuncStructPtr2SinglecastDelegate;
  FreeFuncStructPtr2SinglecastDelegate = MakeDelegate(&FreeFuncStructPtr2);
  FreeFuncStructPtr2SinglecastDelegate(&structParam, TEST_INT);

  SinglecastDelegate2<const StructParam *, int32_t> FreeFuncStructConstPtr2SinglecastDelegate;
  FreeFuncStructConstPtr2SinglecastDelegate = MakeDelegate(&FreeFuncStructConstPtr2);
  FreeFuncStructConstPtr2SinglecastDelegate(&structParam, TEST_INT);

  SinglecastDelegate2<StructParam &, int32_t> FreeFuncStructRef2SinglecastDelegate;
  FreeFuncStructRef2SinglecastDelegate = MakeDelegate(&FreeFuncStructRef2);
  FreeFuncStructRef2SinglecastDelegate(structParam, TEST_INT);

  SinglecastDelegate2<const StructParam &, int32_t> FreeFuncStructConstRef2SinglecastDelegate;
  FreeFuncStructConstRef2SinglecastDelegate = MakeDelegate(&FreeFuncStructConstRef2);
  FreeFuncStructConstRef2SinglecastDelegate(structParam, TEST_INT);

  // N=2 Member Functions
  TestClass2 testClass2;

  SinglecastDelegate2<int32_t, int32_t> MemberFuncInt2SinglecastDelegate;
  EXPECT_TRUE(MemberFuncInt2SinglecastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt2SinglecastDelegate);
  MemberFuncInt2SinglecastDelegate = MakeDelegate(&testClass2, &TestClass2::MemberFuncInt2);
  EXPECT_TRUE(MemberFuncInt2SinglecastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt2SinglecastDelegate);
  MemberFuncInt2SinglecastDelegate(TEST_INT, TEST_INT);
  MemberFuncInt2SinglecastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt2SinglecastDelegate);

  SinglecastDelegate2<int32_t, int32_t, int32_t> MemberFuncIntWithReturn2SinglecastDelegate;
  MemberFuncIntWithReturn2SinglecastDelegate = MakeDelegate(&testClass2, &TestClass2::MemberFuncIntWithReturn2);
  EXPECT_TRUE(MemberFuncIntWithReturn2SinglecastDelegate(TEST_INT, TEST_INT) == TEST_INT);

  SinglecastDelegate2<StructParam, int32_t> MemberFuncStruct2SinglecastDelegate;
  MemberFuncStruct2SinglecastDelegate = MakeDelegate(&testClass2, &TestClass2::MemberFuncStruct2);
  MemberFuncStruct2SinglecastDelegate(structParam, TEST_INT);

  SinglecastDelegate2<StructParam *, int32_t> MemberFuncStructPtr2SinglecastDelegate;
  MemberFuncStructPtr2SinglecastDelegate = MakeDelegate(&testClass2, &TestClass2::MemberFuncStructPtr2);
  MemberFuncStructPtr2SinglecastDelegate(&structParam, TEST_INT);

  SinglecastDelegate2<const StructParam *, int32_t> MemberFuncStructConstPtr2SinglecastDelegate;
  MemberFuncStructConstPtr2SinglecastDelegate = MakeDelegate(&testClass2, &TestClass2::MemberFuncStructConstPtr2);
  MemberFuncStructConstPtr2SinglecastDelegate(&structParam, TEST_INT);

  SinglecastDelegate2<StructParam &, int32_t> MemberFuncStructRef2SinglecastDelegate;
  MemberFuncStructRef2SinglecastDelegate = MakeDelegate(&testClass2, &TestClass2::MemberFuncStructRef2);
  MemberFuncStructRef2SinglecastDelegate(structParam, TEST_INT);

  SinglecastDelegate2<const StructParam &, int32_t> MemberFuncStructConstRef2SinglecastDelegate;
  MemberFuncStructConstRef2SinglecastDelegate = MakeDelegate(&testClass2, &TestClass2::MemberFuncStructConstRef2);
  MemberFuncStructConstRef2SinglecastDelegate(structParam, TEST_INT);

  // N=2 Static Functions
  SinglecastDelegate2<int32_t, int32_t> StaticFuncInt2SinglecastDelegate;
  StaticFuncInt2SinglecastDelegate = MakeDelegate(&TestClass2::StaticFuncInt2);
  StaticFuncInt2SinglecastDelegate(TEST_INT, TEST_INT);

  SinglecastDelegate2<StructParam, int32_t> StaticFuncStruct2SinglecastDelegate;
  StaticFuncStruct2SinglecastDelegate = MakeDelegate(&TestClass2::StaticFuncStruct2);
  StaticFuncStruct2SinglecastDelegate(structParam, TEST_INT);

  SinglecastDelegate2<StructParam *, int32_t> StaticFuncStructPtr2SinglecastDelegate;
  StaticFuncStructPtr2SinglecastDelegate = MakeDelegate(&TestClass2::StaticFuncStructPtr2);
  StaticFuncStructPtr2SinglecastDelegate(&structParam, TEST_INT);

  SinglecastDelegate2<const StructParam *, int32_t> StaticFuncStructConstPtr2SinglecastDelegate;
  StaticFuncStructConstPtr2SinglecastDelegate = MakeDelegate(&TestClass2::StaticFuncStructConstPtr2);
  StaticFuncStructConstPtr2SinglecastDelegate(&structParam, TEST_INT);

  SinglecastDelegate2<StructParam &, int32_t> StaticFuncStructRef2SinglecastDelegate;
  StaticFuncStructRef2SinglecastDelegate = MakeDelegate(&TestClass2::StaticFuncStructRef2);
  StaticFuncStructRef2SinglecastDelegate(structParam, TEST_INT);

  SinglecastDelegate2<const StructParam &, int32_t> StaticFuncStructConstRef2SinglecastDelegate;
  StaticFuncStructConstRef2SinglecastDelegate = MakeDelegate(&TestClass2::StaticFuncStructConstRef2);
  StaticFuncStructConstRef2SinglecastDelegate(structParam, TEST_INT);

  // N=3 Free Functions
  SinglecastDelegate3<int32_t, int32_t, int32_t> FreeFuncInt3SinglecastDelegate;
  EXPECT_TRUE(FreeFuncInt3SinglecastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt3SinglecastDelegate);
  FreeFuncInt3SinglecastDelegate = MakeDelegate(&FreeFuncInt3);
  EXPECT_TRUE(FreeFuncInt3SinglecastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt3SinglecastDelegate);
  FreeFuncInt3SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT);
  FreeFuncInt3SinglecastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt3SinglecastDelegate);

  SinglecastDelegate3<int32_t, int32_t, int32_t, int32_t> FreeFuncIntWithReturn3SinglecastDelegate;
  FreeFuncIntWithReturn3SinglecastDelegate = MakeDelegate(&FreeFuncIntWithReturn3);
  EXPECT_TRUE(FreeFuncIntWithReturn3SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT) == TEST_INT);

  SinglecastDelegate3<StructParam, int32_t, int32_t> FreeFuncStruct3SinglecastDelegate;
  FreeFuncStruct3SinglecastDelegate = MakeDelegate(&FreeFuncStruct3);
  FreeFuncStruct3SinglecastDelegate(structParam, TEST_INT, TEST_INT);

  SinglecastDelegate3<StructParam **, int32_t, int32_t> FreeFuncPtrPtr3SinglecastDelegate;
  FreeFuncPtrPtr3SinglecastDelegate = MakeDelegate(&FreeFuncPtrPtr3);
  FreeFuncPtrPtr3SinglecastDelegate(&pStructParam, TEST_INT, TEST_INT);

  SinglecastDelegate3<StructParam *, int32_t, int32_t> FreeFuncStructPtr3SinglecastDelegate;
  FreeFuncStructPtr3SinglecastDelegate = MakeDelegate(&FreeFuncStructPtr3);
  FreeFuncStructPtr3SinglecastDelegate(&structParam, TEST_INT, TEST_INT);

  SinglecastDelegate3<const StructParam *, int32_t, int32_t> FreeFuncStructConstPtr3SinglecastDelegate;
  FreeFuncStructConstPtr3SinglecastDelegate = MakeDelegate(&FreeFuncStructConstPtr3);
  FreeFuncStructConstPtr3SinglecastDelegate(&structParam, TEST_INT, TEST_INT);

  SinglecastDelegate3<StructParam &, int32_t, int32_t> FreeFuncStructRef3SinglecastDelegate;
  FreeFuncStructRef3SinglecastDelegate = MakeDelegate(&FreeFuncStructRef3);
  FreeFuncStructRef3SinglecastDelegate(structParam, TEST_INT, TEST_INT);

  SinglecastDelegate3<const StructParam &, int32_t, int32_t> FreeFuncStructConstRef3SinglecastDelegate;
  FreeFuncStructConstRef3SinglecastDelegate = MakeDelegate(&FreeFuncStructConstRef3);
  FreeFuncStructConstRef3SinglecastDelegate(structParam, TEST_INT, TEST_INT);

  // N=3 Member Functions
  TestClass3 testClass3;

  SinglecastDelegate3<int32_t, int32_t, int32_t> MemberFuncInt3SinglecastDelegate;
  EXPECT_TRUE(MemberFuncInt3SinglecastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt3SinglecastDelegate);
  MemberFuncInt3SinglecastDelegate = MakeDelegate(&testClass3, &TestClass3::MemberFuncInt3);
  EXPECT_TRUE(MemberFuncInt3SinglecastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt3SinglecastDelegate);
  MemberFuncInt3SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt3SinglecastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt3SinglecastDelegate);

  SinglecastDelegate3<int32_t, int32_t, int32_t, int32_t> MemberFuncIntWithReturn3SinglecastDelegate;
  MemberFuncIntWithReturn3SinglecastDelegate = MakeDelegate(&testClass3, &TestClass3::MemberFuncIntWithReturn3);
  EXPECT_TRUE(MemberFuncIntWithReturn3SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT) == TEST_INT);

  SinglecastDelegate3<StructParam, int32_t, int32_t> MemberFuncStruct3SinglecastDelegate;
  MemberFuncStruct3SinglecastDelegate = MakeDelegate(&testClass3, &TestClass3::MemberFuncStruct3);
  MemberFuncStruct3SinglecastDelegate(structParam, TEST_INT, TEST_INT);

  SinglecastDelegate3<StructParam *, int32_t, int32_t> MemberFuncStructPtr3SinglecastDelegate;
  MemberFuncStructPtr3SinglecastDelegate = MakeDelegate(&testClass3, &TestClass3::MemberFuncStructPtr3);
  MemberFuncStructPtr3SinglecastDelegate(&structParam, TEST_INT, TEST_INT);

  SinglecastDelegate3<const StructParam *, int32_t, int32_t> MemberFuncStructConstPtr3SinglecastDelegate;
  MemberFuncStructConstPtr3SinglecastDelegate = MakeDelegate(&testClass3, &TestClass3::MemberFuncStructConstPtr3);
  MemberFuncStructConstPtr3SinglecastDelegate(&structParam, TEST_INT, TEST_INT);

  SinglecastDelegate3<StructParam &, int32_t, int32_t> MemberFuncStructRef3SinglecastDelegate;
  MemberFuncStructRef3SinglecastDelegate = MakeDelegate(&testClass3, &TestClass3::MemberFuncStructRef3);
  MemberFuncStructRef3SinglecastDelegate(structParam, TEST_INT, TEST_INT);

  SinglecastDelegate3<const StructParam &, int32_t, int32_t> MemberFuncStructConstRef3SinglecastDelegate;
  MemberFuncStructConstRef3SinglecastDelegate = MakeDelegate(&testClass3, &TestClass3::MemberFuncStructConstRef3);
  MemberFuncStructConstRef3SinglecastDelegate(structParam, TEST_INT, TEST_INT);

  // N=3 Static Functions
  SinglecastDelegate3<int32_t, int32_t, int32_t> StaticFuncInt3SinglecastDelegate;
  StaticFuncInt3SinglecastDelegate = MakeDelegate(&TestClass3::StaticFuncInt3);
  StaticFuncInt3SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate3<StructParam, int32_t, int32_t> StaticFuncStruct3SinglecastDelegate;
  StaticFuncStruct3SinglecastDelegate = MakeDelegate(&TestClass3::StaticFuncStruct3);
  StaticFuncStruct3SinglecastDelegate(structParam, TEST_INT, TEST_INT);

  SinglecastDelegate3<StructParam *, int32_t, int32_t> StaticFuncStructPtr3SinglecastDelegate;
  StaticFuncStructPtr3SinglecastDelegate = MakeDelegate(&TestClass3::StaticFuncStructPtr3);
  StaticFuncStructPtr3SinglecastDelegate(&structParam, TEST_INT, TEST_INT);

  SinglecastDelegate3<const StructParam *, int32_t, int32_t> StaticFuncStructConstPtr3SinglecastDelegate;
  StaticFuncStructConstPtr3SinglecastDelegate = MakeDelegate(&TestClass3::StaticFuncStructConstPtr3);
  StaticFuncStructConstPtr3SinglecastDelegate(&structParam, TEST_INT, TEST_INT);

  SinglecastDelegate3<StructParam &, int32_t, int32_t> StaticFuncStructRef3SinglecastDelegate;
  StaticFuncStructRef3SinglecastDelegate = MakeDelegate(&TestClass3::StaticFuncStructRef3);
  StaticFuncStructRef3SinglecastDelegate(structParam, TEST_INT, TEST_INT);

  SinglecastDelegate3<const StructParam &, int32_t, int32_t> StaticFuncStructConstRef3SinglecastDelegate;
  StaticFuncStructConstRef3SinglecastDelegate = MakeDelegate(&TestClass3::StaticFuncStructConstRef3);
  StaticFuncStructConstRef3SinglecastDelegate(structParam, TEST_INT, TEST_INT);

  // N=4 Free Functions
  SinglecastDelegate4<int32_t, int32_t, int32_t, int32_t> FreeFuncInt4SinglecastDelegate;
  EXPECT_TRUE(FreeFuncInt4SinglecastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt4SinglecastDelegate);
  FreeFuncInt4SinglecastDelegate = MakeDelegate(&FreeFuncInt4);
  EXPECT_TRUE(FreeFuncInt4SinglecastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt4SinglecastDelegate);
  FreeFuncInt4SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  FreeFuncInt4SinglecastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt4SinglecastDelegate);

  SinglecastDelegate4<int32_t, int32_t, int32_t, int32_t, int32_t> FreeFuncIntWithReturn4SinglecastDelegate;
  FreeFuncIntWithReturn4SinglecastDelegate = MakeDelegate(&FreeFuncIntWithReturn4);
  EXPECT_TRUE(FreeFuncIntWithReturn4SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT) == TEST_INT);

  SinglecastDelegate4<StructParam, int32_t, int32_t, int32_t> FreeFuncStruct4SinglecastDelegate;
  FreeFuncStruct4SinglecastDelegate = MakeDelegate(&FreeFuncStruct4);
  FreeFuncStruct4SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate4<StructParam **, int32_t, int32_t, int32_t> FreeFuncPtrPtr4SinglecastDelegate;
  FreeFuncPtrPtr4SinglecastDelegate = MakeDelegate(&FreeFuncPtrPtr4);
  FreeFuncPtrPtr4SinglecastDelegate(&pStructParam, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate4<StructParam *, int32_t, int32_t, int32_t> FreeFuncStructPtr4SinglecastDelegate;
  FreeFuncStructPtr4SinglecastDelegate = MakeDelegate(&FreeFuncStructPtr4);
  FreeFuncStructPtr4SinglecastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate4<const StructParam *, int32_t, int32_t, int32_t> FreeFuncStructConstPtr4SinglecastDelegate;
  FreeFuncStructConstPtr4SinglecastDelegate = MakeDelegate(&FreeFuncStructConstPtr4);
  FreeFuncStructConstPtr4SinglecastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate4<StructParam &, int32_t, int32_t, int32_t> FreeFuncStructRef4SinglecastDelegate;
  FreeFuncStructRef4SinglecastDelegate = MakeDelegate(&FreeFuncStructRef4);
  FreeFuncStructRef4SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate4<const StructParam &, int32_t, int32_t, int32_t> FreeFuncStructConstRef4SinglecastDelegate;
  FreeFuncStructConstRef4SinglecastDelegate = MakeDelegate(&FreeFuncStructConstRef4);
  FreeFuncStructConstRef4SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  // N=4 Member Functions
  TestClass4 testClass4;

  SinglecastDelegate4<int32_t, int32_t, int32_t, int32_t> MemberFuncInt4SinglecastDelegate;
  EXPECT_TRUE(MemberFuncInt4SinglecastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt4SinglecastDelegate);
  MemberFuncInt4SinglecastDelegate = MakeDelegate(&testClass4, &TestClass4::MemberFuncInt4);
  EXPECT_TRUE(MemberFuncInt4SinglecastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt4SinglecastDelegate);
  MemberFuncInt4SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt4SinglecastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt4SinglecastDelegate);

  SinglecastDelegate4<int32_t, int32_t, int32_t, int32_t, int32_t> MemberFuncIntWithReturn4SinglecastDelegate;
  MemberFuncIntWithReturn4SinglecastDelegate = MakeDelegate(&testClass4, &TestClass4::MemberFuncIntWithReturn4);
  EXPECT_TRUE(MemberFuncIntWithReturn4SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT) == TEST_INT);

  SinglecastDelegate4<StructParam, int32_t, int32_t, int32_t> MemberFuncStruct4SinglecastDelegate;
  MemberFuncStruct4SinglecastDelegate = MakeDelegate(&testClass4, &TestClass4::MemberFuncStruct4);
  MemberFuncStruct4SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate4<StructParam *, int32_t, int32_t, int32_t> MemberFuncStructPtr4SinglecastDelegate;
  MemberFuncStructPtr4SinglecastDelegate = MakeDelegate(&testClass4, &TestClass4::MemberFuncStructPtr4);
  MemberFuncStructPtr4SinglecastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate4<const StructParam *, int32_t, int32_t, int32_t> MemberFuncStructConstPtr4SinglecastDelegate;
  MemberFuncStructConstPtr4SinglecastDelegate = MakeDelegate(&testClass4, &TestClass4::MemberFuncStructConstPtr4);
  MemberFuncStructConstPtr4SinglecastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate4<StructParam &, int32_t, int32_t, int32_t> MemberFuncStructRef4SinglecastDelegate;
  MemberFuncStructRef4SinglecastDelegate = MakeDelegate(&testClass4, &TestClass4::MemberFuncStructRef4);
  MemberFuncStructRef4SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate4<const StructParam &, int32_t, int32_t, int32_t> MemberFuncStructConstRef4SinglecastDelegate;
  MemberFuncStructConstRef4SinglecastDelegate = MakeDelegate(&testClass4, &TestClass4::MemberFuncStructConstRef4);
  MemberFuncStructConstRef4SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  // N=4 Static Functions
  SinglecastDelegate4<int32_t, int32_t, int32_t, int32_t> StaticFuncInt4SinglecastDelegate;
  StaticFuncInt4SinglecastDelegate = MakeDelegate(&TestClass4::StaticFuncInt4);
  StaticFuncInt4SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate4<StructParam, int32_t, int32_t, int32_t> StaticFuncStruct4SinglecastDelegate;
  StaticFuncStruct4SinglecastDelegate = MakeDelegate(&TestClass4::StaticFuncStruct4);
  StaticFuncStruct4SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate4<StructParam *, int32_t, int32_t, int32_t> StaticFuncStructPtr4SinglecastDelegate;
  StaticFuncStructPtr4SinglecastDelegate = MakeDelegate(&TestClass4::StaticFuncStructPtr4);
  StaticFuncStructPtr4SinglecastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate4<const StructParam *, int32_t, int32_t, int32_t> StaticFuncStructConstPtr4SinglecastDelegate;
  StaticFuncStructConstPtr4SinglecastDelegate = MakeDelegate(&TestClass4::StaticFuncStructConstPtr4);
  StaticFuncStructConstPtr4SinglecastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate4<StructParam &, int32_t, int32_t, int32_t> StaticFuncStructRef4SinglecastDelegate;
  StaticFuncStructRef4SinglecastDelegate = MakeDelegate(&TestClass4::StaticFuncStructRef4);
  StaticFuncStructRef4SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate4<const StructParam &, int32_t, int32_t, int32_t> StaticFuncStructConstRef4SinglecastDelegate;
  StaticFuncStructConstRef4SinglecastDelegate = MakeDelegate(&TestClass4::StaticFuncStructConstRef4);
  StaticFuncStructConstRef4SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  // N=5 Free Functions
  SinglecastDelegate5<int32_t, int32_t, int32_t, int32_t, int32_t> FreeFuncInt5SinglecastDelegate;
  EXPECT_TRUE(FreeFuncInt5SinglecastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt5SinglecastDelegate);
  FreeFuncInt5SinglecastDelegate = MakeDelegate(&FreeFuncInt5);
  EXPECT_TRUE(FreeFuncInt5SinglecastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt5SinglecastDelegate);
  FreeFuncInt5SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  FreeFuncInt5SinglecastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt5SinglecastDelegate);

  SinglecastDelegate5<int32_t, int32_t, int32_t, int32_t, int32_t, int32_t> FreeFuncIntWithReturn5SinglecastDelegate;
  FreeFuncIntWithReturn5SinglecastDelegate = MakeDelegate(&FreeFuncIntWithReturn5);
  EXPECT_TRUE(FreeFuncIntWithReturn5SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT) == TEST_INT);

  SinglecastDelegate5<StructParam, int32_t, int32_t, int32_t, int32_t> FreeFuncStruct5SinglecastDelegate;
  FreeFuncStruct5SinglecastDelegate = MakeDelegate(&FreeFuncStruct5);
  FreeFuncStruct5SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate5<StructParam **, int32_t, int32_t, int32_t, int32_t> FreeFuncPtrPtr5SinglecastDelegate;
  FreeFuncPtrPtr5SinglecastDelegate = MakeDelegate(&FreeFuncPtrPtr5);
  FreeFuncPtrPtr5SinglecastDelegate(&pStructParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate5<StructParam *, int32_t, int32_t, int32_t, int32_t> FreeFuncStructPtr5SinglecastDelegate;
  FreeFuncStructPtr5SinglecastDelegate = MakeDelegate(&FreeFuncStructPtr5);
  FreeFuncStructPtr5SinglecastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate5<const StructParam *, int32_t, int32_t, int32_t, int32_t> FreeFuncStructConstPtr5SinglecastDelegate;
  FreeFuncStructConstPtr5SinglecastDelegate = MakeDelegate(&FreeFuncStructConstPtr5);
  FreeFuncStructConstPtr5SinglecastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate5<StructParam &, int32_t, int32_t, int32_t, int32_t> FreeFuncStructRef5SinglecastDelegate;
  FreeFuncStructRef5SinglecastDelegate = MakeDelegate(&FreeFuncStructRef5);
  FreeFuncStructRef5SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate5<const StructParam &, int32_t, int32_t, int32_t, int32_t> FreeFuncStructConstRef5SinglecastDelegate;
  FreeFuncStructConstRef5SinglecastDelegate = MakeDelegate(&FreeFuncStructConstRef5);
  FreeFuncStructConstRef5SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  // N=5 Member Functions
  TestClass5 testClass5;

  SinglecastDelegate5<int32_t, int32_t, int32_t, int32_t, int32_t> MemberFuncInt5SinglecastDelegate;
  EXPECT_TRUE(MemberFuncInt5SinglecastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt5SinglecastDelegate);
  MemberFuncInt5SinglecastDelegate = MakeDelegate(&testClass5, &TestClass5::MemberFuncInt5);
  EXPECT_TRUE(MemberFuncInt5SinglecastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt5SinglecastDelegate);
  MemberFuncInt5SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt5SinglecastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt5SinglecastDelegate);

  SinglecastDelegate5<int32_t, int32_t, int32_t, int32_t, int32_t, int32_t> MemberFuncIntWithReturn5SinglecastDelegate;
  MemberFuncIntWithReturn5SinglecastDelegate = MakeDelegate(&testClass5, &TestClass5::MemberFuncIntWithReturn5);
  EXPECT_TRUE(MemberFuncIntWithReturn5SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT) == TEST_INT);

  SinglecastDelegate5<StructParam, int32_t, int32_t, int32_t, int32_t> MemberFuncStruct5SinglecastDelegate;
  MemberFuncStruct5SinglecastDelegate = MakeDelegate(&testClass5, &TestClass5::MemberFuncStruct5);
  MemberFuncStruct5SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate5<StructParam *, int32_t, int32_t, int32_t, int32_t> MemberFuncStructPtr5SinglecastDelegate;
  MemberFuncStructPtr5SinglecastDelegate = MakeDelegate(&testClass5, &TestClass5::MemberFuncStructPtr5);
  MemberFuncStructPtr5SinglecastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate5<const StructParam *, int32_t, int32_t, int32_t, int32_t> MemberFuncStructConstPtr5SinglecastDelegate;
  MemberFuncStructConstPtr5SinglecastDelegate = MakeDelegate(&testClass5, &TestClass5::MemberFuncStructConstPtr5);
  MemberFuncStructConstPtr5SinglecastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate5<StructParam &, int32_t, int32_t, int32_t, int32_t> MemberFuncStructRef5SinglecastDelegate;
  MemberFuncStructRef5SinglecastDelegate = MakeDelegate(&testClass5, &TestClass5::MemberFuncStructRef5);
  MemberFuncStructRef5SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate5<const StructParam &, int32_t, int32_t, int32_t, int32_t> MemberFuncStructConstRef5SinglecastDelegate;
  MemberFuncStructConstRef5SinglecastDelegate = MakeDelegate(&testClass5, &TestClass5::MemberFuncStructConstRef5);
  MemberFuncStructConstRef5SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  // N=5 Static Functions
  SinglecastDelegate5<int32_t, int32_t, int32_t, int32_t, int32_t> StaticFuncInt5SinglecastDelegate;
  StaticFuncInt5SinglecastDelegate = MakeDelegate(&TestClass5::StaticFuncInt5);
  StaticFuncInt5SinglecastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate5<StructParam, int32_t, int32_t, int32_t, int32_t> StaticFuncStruct5SinglecastDelegate;
  StaticFuncStruct5SinglecastDelegate = MakeDelegate(&TestClass5::StaticFuncStruct5);
  StaticFuncStruct5SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate5<StructParam *, int32_t, int32_t, int32_t, int32_t> StaticFuncStructPtr5SinglecastDelegate;
  StaticFuncStructPtr5SinglecastDelegate = MakeDelegate(&TestClass5::StaticFuncStructPtr5);
  StaticFuncStructPtr5SinglecastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate5<const StructParam *, int32_t, int32_t, int32_t, int32_t> StaticFuncStructConstPtr5SinglecastDelegate;
  StaticFuncStructConstPtr5SinglecastDelegate = MakeDelegate(&TestClass5::StaticFuncStructConstPtr5);
  StaticFuncStructConstPtr5SinglecastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate5<StructParam &, int32_t, int32_t, int32_t, int32_t> StaticFuncStructRef5SinglecastDelegate;
  StaticFuncStructRef5SinglecastDelegate = MakeDelegate(&TestClass5::StaticFuncStructRef5);
  StaticFuncStructRef5SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  SinglecastDelegate5<const StructParam &, int32_t, int32_t, int32_t, int32_t> StaticFuncStructConstRef5SinglecastDelegate;
  StaticFuncStructConstRef5SinglecastDelegate = MakeDelegate(&TestClass5::StaticFuncStructConstRef5);
  StaticFuncStructConstRef5SinglecastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
}

void MulticastDelegateTests()
{
  StructParam structParam;
  structParam.val = TEST_INT;

  // N=0 Free Functions
  MulticastDelegate0 FreeFunc0MulticastDelegate;
  EXPECT_TRUE(FreeFunc0MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFunc0MulticastDelegate);
  FreeFunc0MulticastDelegate += MakeDelegate(&FreeFunc0);
  FreeFunc0MulticastDelegate += MakeDelegate(&FreeFunc0);
  EXPECT_TRUE(FreeFunc0MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFunc0MulticastDelegate);
  FreeFunc0MulticastDelegate();
  FreeFunc0MulticastDelegate -= MakeDelegate(&FreeFunc0);
  // BHA	EXPECT_TRUE(FreeFunc0MulticastDelegate);
  FreeFunc0MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFunc0MulticastDelegate);

  // N=0 Member Functions
  TestClass0 testClass0;

  MulticastDelegate0 MemberFunc0MulticastDelegate;
  EXPECT_TRUE(MemberFunc0MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFunc0MulticastDelegate);
  MemberFunc0MulticastDelegate += MakeDelegate(&testClass0, &TestClass0::MemberFunc0);
  EXPECT_TRUE(MemberFunc0MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFunc0MulticastDelegate);
  MemberFunc0MulticastDelegate();
  MemberFunc0MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFunc0MulticastDelegate);

  // N=0 Static Functions
  MulticastDelegate0 StaticFunc0MulticastDelegate;
  StaticFunc0MulticastDelegate += MakeDelegate(&TestClass0::StaticFunc0);
  StaticFunc0MulticastDelegate();

  // N=1 Free Functions
  MulticastDelegate1<int32_t> FreeFuncInt1MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt1MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt1MulticastDelegate);
  FreeFuncInt1MulticastDelegate += MakeDelegate(&FreeFuncInt1);
  FreeFuncInt1MulticastDelegate += MakeDelegate(&FreeFuncInt1);
  EXPECT_TRUE(FreeFuncInt1MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt1MulticastDelegate);
  FreeFuncInt1MulticastDelegate(TEST_INT);
  FreeFuncInt1MulticastDelegate -= MakeDelegate(&FreeFuncInt1);
  // BHA	EXPECT_TRUE(FreeFuncInt1MulticastDelegate);
  FreeFuncInt1MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt1MulticastDelegate);

  MulticastDelegate1<StructParam> FreeFuncStruct1MulticastDelegate;
  FreeFuncStruct1MulticastDelegate += MakeDelegate(&FreeFuncStruct1);
  FreeFuncStruct1MulticastDelegate(structParam);

  MulticastDelegate1<StructParam *> FreeFuncStructPtr1MulticastDelegate;
  FreeFuncStructPtr1MulticastDelegate += MakeDelegate(&FreeFuncStructPtr1);
  FreeFuncStructPtr1MulticastDelegate(&structParam);

  MulticastDelegate1<const StructParam *> FreeFuncStructConstPtr1MulticastDelegate;
  FreeFuncStructConstPtr1MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr1);
  FreeFuncStructConstPtr1MulticastDelegate(&structParam);

  MulticastDelegate1<StructParam &> FreeFuncStructRef1MulticastDelegate;
  FreeFuncStructRef1MulticastDelegate += MakeDelegate(&FreeFuncStructRef1);
  FreeFuncStructRef1MulticastDelegate(structParam);

  MulticastDelegate1<const StructParam &> FreeFuncStructConstRef1MulticastDelegate;
  FreeFuncStructConstRef1MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef1);
  FreeFuncStructConstRef1MulticastDelegate(structParam);

  // N=1 Member Functions
  TestClass1 testClass1;

  MulticastDelegate1<int32_t> MemberFuncInt1MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt1MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt1MulticastDelegate);
  MemberFuncInt1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncInt1);
  EXPECT_TRUE(MemberFuncInt1MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt1MulticastDelegate);
  MemberFuncInt1MulticastDelegate(TEST_INT);
  MemberFuncInt1MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt1MulticastDelegate);

  MulticastDelegate1<StructParam> MemberFuncStruct1MulticastDelegate;
  MemberFuncStruct1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStruct1);
  MemberFuncStruct1MulticastDelegate(structParam);

  MulticastDelegate1<StructParam *> MemberFuncStructPtr1MulticastDelegate;
  MemberFuncStructPtr1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStructPtr1);
  MemberFuncStructPtr1MulticastDelegate(&structParam);

  MulticastDelegate1<const StructParam *> MemberFuncStructConstPtr1MulticastDelegate;
  MemberFuncStructConstPtr1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStructConstPtr1);
  MemberFuncStructConstPtr1MulticastDelegate(&structParam);

  MulticastDelegate1<StructParam &> MemberFuncStructRef1MulticastDelegate;
  MemberFuncStructRef1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStructRef1);
  MemberFuncStructRef1MulticastDelegate(structParam);

  MulticastDelegate1<const StructParam &> MemberFuncStructConstRef1MulticastDelegate;
  MemberFuncStructConstRef1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStructConstRef1);
  MemberFuncStructConstRef1MulticastDelegate(structParam);

  // N=1 Static Functions
  MulticastDelegate1<int32_t> StaticFuncInt1MulticastDelegate;
  StaticFuncInt1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncInt1);
  StaticFuncInt1MulticastDelegate(TEST_INT);

  MulticastDelegate1<StructParam> StaticFuncStruct1MulticastDelegate;
  StaticFuncStruct1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStruct1);
  StaticFuncStruct1MulticastDelegate(structParam);

  MulticastDelegate1<StructParam *> StaticFuncStructPtr1MulticastDelegate;
  StaticFuncStructPtr1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStructPtr1);
  StaticFuncStructPtr1MulticastDelegate(&structParam);

  MulticastDelegate1<const StructParam *> StaticFuncStructConstPtr1MulticastDelegate;
  StaticFuncStructConstPtr1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStructConstPtr1);
  StaticFuncStructConstPtr1MulticastDelegate(&structParam);

  MulticastDelegate1<StructParam &> StaticFuncStructRef1MulticastDelegate;
  StaticFuncStructRef1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStructRef1);
  StaticFuncStructRef1MulticastDelegate(structParam);

  MulticastDelegate1<const StructParam &> StaticFuncStructConstRef1MulticastDelegate;
  StaticFuncStructConstRef1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStructConstRef1);
  StaticFuncStructConstRef1MulticastDelegate(structParam);

  // N=2 Free Functions
  MulticastDelegate2<int32_t, int32_t> FreeFuncInt2MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt2MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt2MulticastDelegate);
  FreeFuncInt2MulticastDelegate += MakeDelegate(&FreeFuncInt2);
  FreeFuncInt2MulticastDelegate += MakeDelegate(&FreeFuncInt2);
  EXPECT_TRUE(FreeFuncInt2MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt2MulticastDelegate);
  FreeFuncInt2MulticastDelegate(TEST_INT, TEST_INT);
  FreeFuncInt2MulticastDelegate -= MakeDelegate(&FreeFuncInt2);
  // BHA	EXPECT_TRUE(FreeFuncInt2MulticastDelegate);
  FreeFuncInt2MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt2MulticastDelegate);

  MulticastDelegate2<StructParam, int32_t> FreeFuncStruct2MulticastDelegate;
  FreeFuncStruct2MulticastDelegate += MakeDelegate(&FreeFuncStruct2);
  FreeFuncStruct2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegate2<StructParam *, int32_t> FreeFuncStructPtr2MulticastDelegate;
  FreeFuncStructPtr2MulticastDelegate += MakeDelegate(&FreeFuncStructPtr2);
  FreeFuncStructPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegate2<const StructParam *, int32_t> FreeFuncStructConstPtr2MulticastDelegate;
  FreeFuncStructConstPtr2MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr2);
  FreeFuncStructConstPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegate2<StructParam &, int32_t> FreeFuncStructRef2MulticastDelegate;
  FreeFuncStructRef2MulticastDelegate += MakeDelegate(&FreeFuncStructRef2);
  FreeFuncStructRef2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegate2<const StructParam &, int32_t> FreeFuncStructConstRef2MulticastDelegate;
  FreeFuncStructConstRef2MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef2);
  FreeFuncStructConstRef2MulticastDelegate(structParam, TEST_INT);

  // N=2 Member Functions
  TestClass2 testClass2;

  MulticastDelegate2<int32_t, int32_t> MemberFuncInt2MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt2MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt2MulticastDelegate);
  MemberFuncInt2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncInt2);
  EXPECT_TRUE(MemberFuncInt2MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt2MulticastDelegate);
  MemberFuncInt2MulticastDelegate(TEST_INT, TEST_INT);
  MemberFuncInt2MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt2MulticastDelegate);

  MulticastDelegate2<StructParam, int32_t> MemberFuncStruct2MulticastDelegate;
  MemberFuncStruct2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStruct2);
  MemberFuncStruct2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegate2<StructParam *, int32_t> MemberFuncStructPtr2MulticastDelegate;
  MemberFuncStructPtr2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStructPtr2);
  MemberFuncStructPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegate2<const StructParam *, int32_t> MemberFuncStructConstPtr2MulticastDelegate;
  MemberFuncStructConstPtr2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStructConstPtr2);
  MemberFuncStructConstPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegate2<StructParam &, int32_t> MemberFuncStructRef2MulticastDelegate;
  MemberFuncStructRef2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStructRef2);
  MemberFuncStructRef2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegate2<const StructParam &, int32_t> MemberFuncStructConstRef2MulticastDelegate;
  MemberFuncStructConstRef2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStructConstRef2);
  MemberFuncStructConstRef2MulticastDelegate(structParam, TEST_INT);

  // N=2 Static Functions
  MulticastDelegate2<int32_t, int32_t> StaticFuncInt2MulticastDelegate;
  StaticFuncInt2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncInt2);
  StaticFuncInt2MulticastDelegate(TEST_INT, TEST_INT);

  MulticastDelegate2<StructParam, int32_t> StaticFuncStruct2MulticastDelegate;
  StaticFuncStruct2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStruct2);
  StaticFuncStruct2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegate2<StructParam *, int32_t> StaticFuncStructPtr2MulticastDelegate;
  StaticFuncStructPtr2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStructPtr2);
  StaticFuncStructPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegate2<const StructParam *, int32_t> StaticFuncStructConstPtr2MulticastDelegate;
  StaticFuncStructConstPtr2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStructConstPtr2);
  StaticFuncStructConstPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegate2<StructParam &, int32_t> StaticFuncStructRef2MulticastDelegate;
  StaticFuncStructRef2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStructRef2);
  StaticFuncStructRef2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegate2<const StructParam &, int32_t> StaticFuncStructConstRef2MulticastDelegate;
  StaticFuncStructConstRef2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStructConstRef2);
  StaticFuncStructConstRef2MulticastDelegate(structParam, TEST_INT);

  // N=3 Free Functions
  MulticastDelegate3<int32_t, int32_t, int32_t> FreeFuncInt3MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt3MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt3MulticastDelegate);
  FreeFuncInt3MulticastDelegate += MakeDelegate(&FreeFuncInt3);
  FreeFuncInt3MulticastDelegate += MakeDelegate(&FreeFuncInt3);
  EXPECT_TRUE(FreeFuncInt3MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt3MulticastDelegate);
  FreeFuncInt3MulticastDelegate(TEST_INT, TEST_INT, TEST_INT);
  FreeFuncInt3MulticastDelegate -= MakeDelegate(&FreeFuncInt3);
  // BHA	EXPECT_TRUE(FreeFuncInt3MulticastDelegate);
  FreeFuncInt3MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt3MulticastDelegate);

  MulticastDelegate3<StructParam, int32_t, int32_t> FreeFuncStruct3MulticastDelegate;
  FreeFuncStruct3MulticastDelegate += MakeDelegate(&FreeFuncStruct3);
  FreeFuncStruct3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegate3<StructParam *, int32_t, int32_t> FreeFuncStructPtr3MulticastDelegate;
  FreeFuncStructPtr3MulticastDelegate += MakeDelegate(&FreeFuncStructPtr3);
  FreeFuncStructPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegate3<const StructParam *, int32_t, int32_t> FreeFuncStructConstPtr3MulticastDelegate;
  FreeFuncStructConstPtr3MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr3);
  FreeFuncStructConstPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegate3<StructParam &, int32_t, int32_t> FreeFuncStructRef3MulticastDelegate;
  FreeFuncStructRef3MulticastDelegate += MakeDelegate(&FreeFuncStructRef3);
  FreeFuncStructRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegate3<const StructParam &, int32_t, int32_t> FreeFuncStructConstRef3MulticastDelegate;
  FreeFuncStructConstRef3MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef3);
  FreeFuncStructConstRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  // N=3 Member Functions
  TestClass3 testClass3;

  MulticastDelegate3<int32_t, int32_t, int32_t> MemberFuncInt3MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt3MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt3MulticastDelegate);
  MemberFuncInt3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncInt3);
  EXPECT_TRUE(MemberFuncInt3MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt3MulticastDelegate);
  MemberFuncInt3MulticastDelegate(TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt3MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt3MulticastDelegate);

  MulticastDelegate3<StructParam, int32_t, int32_t> MemberFuncStruct3MulticastDelegate;
  MemberFuncStruct3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStruct3);
  MemberFuncStruct3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegate3<StructParam *, int32_t, int32_t> MemberFuncStructPtr3MulticastDelegate;
  MemberFuncStructPtr3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStructPtr3);
  MemberFuncStructPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegate3<const StructParam *, int32_t, int32_t> MemberFuncStructConstPtr3MulticastDelegate;
  MemberFuncStructConstPtr3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStructConstPtr3);
  MemberFuncStructConstPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegate3<StructParam &, int32_t, int32_t> MemberFuncStructRef3MulticastDelegate;
  MemberFuncStructRef3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStructRef3);
  MemberFuncStructRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegate3<const StructParam &, int32_t, int32_t> MemberFuncStructConstRef3MulticastDelegate;
  MemberFuncStructConstRef3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStructConstRef3);
  MemberFuncStructConstRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  // N=3 Static Functions
  MulticastDelegate3<int32_t, int32_t, int32_t> StaticFuncInt3MulticastDelegate;
  StaticFuncInt3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncInt3);
  StaticFuncInt3MulticastDelegate(TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate3<StructParam, int32_t, int32_t> StaticFuncStruct3MulticastDelegate;
  StaticFuncStruct3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStruct3);
  StaticFuncStruct3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegate3<StructParam *, int32_t, int32_t> StaticFuncStructPtr3MulticastDelegate;
  StaticFuncStructPtr3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStructPtr3);
  StaticFuncStructPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegate3<const StructParam *, int32_t, int32_t> StaticFuncStructConstPtr3MulticastDelegate;
  StaticFuncStructConstPtr3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStructConstPtr3);
  StaticFuncStructConstPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegate3<StructParam &, int32_t, int32_t> StaticFuncStructRef3MulticastDelegate;
  StaticFuncStructRef3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStructRef3);
  StaticFuncStructRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegate3<const StructParam &, int32_t, int32_t> StaticFuncStructConstRef3MulticastDelegate;
  StaticFuncStructConstRef3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStructConstRef3);
  StaticFuncStructConstRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  // N=4 Free Functions
  MulticastDelegate4<int32_t, int32_t, int32_t, int32_t> FreeFuncInt4MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt4MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt4MulticastDelegate);
  FreeFuncInt4MulticastDelegate += MakeDelegate(&FreeFuncInt4);
  FreeFuncInt4MulticastDelegate += MakeDelegate(&FreeFuncInt4);
  EXPECT_TRUE(FreeFuncInt4MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt4MulticastDelegate);
  FreeFuncInt4MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  FreeFuncInt4MulticastDelegate -= MakeDelegate(&FreeFuncInt4);
  // BHA EXPECT_TRUE(FreeFuncInt4MulticastDelegate);
  FreeFuncInt4MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt4MulticastDelegate);

  MulticastDelegate4<StructParam, int32_t, int32_t, int32_t> FreeFuncStruct4MulticastDelegate;
  FreeFuncStruct4MulticastDelegate += MakeDelegate(&FreeFuncStruct4);
  FreeFuncStruct4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate4<StructParam *, int32_t, int32_t, int32_t> FreeFuncStructPtr4MulticastDelegate;
  FreeFuncStructPtr4MulticastDelegate += MakeDelegate(&FreeFuncStructPtr4);
  FreeFuncStructPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate4<const StructParam *, int32_t, int32_t, int32_t> FreeFuncStructConstPtr4MulticastDelegate;
  FreeFuncStructConstPtr4MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr4);
  FreeFuncStructConstPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate4<StructParam &, int32_t, int32_t, int32_t> FreeFuncStructRef4MulticastDelegate;
  FreeFuncStructRef4MulticastDelegate += MakeDelegate(&FreeFuncStructRef4);
  FreeFuncStructRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate4<const StructParam &, int32_t, int32_t, int32_t> FreeFuncStructConstRef4MulticastDelegate;
  FreeFuncStructConstRef4MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef4);
  FreeFuncStructConstRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  // N=4 Member Functions
  TestClass4 testClass4;

  MulticastDelegate4<int32_t, int32_t, int32_t, int32_t> MemberFuncInt4MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt4MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt4MulticastDelegate);
  MemberFuncInt4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncInt4);
  EXPECT_TRUE(MemberFuncInt4MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt4MulticastDelegate);
  MemberFuncInt4MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt4MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt4MulticastDelegate);

  MulticastDelegate4<StructParam, int32_t, int32_t, int32_t> MemberFuncStruct4MulticastDelegate;
  MemberFuncStruct4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStruct4);
  MemberFuncStruct4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate4<StructParam *, int32_t, int32_t, int32_t> MemberFuncStructPtr4MulticastDelegate;
  MemberFuncStructPtr4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStructPtr4);
  MemberFuncStructPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate4<const StructParam *, int32_t, int32_t, int32_t> MemberFuncStructConstPtr4MulticastDelegate;
  MemberFuncStructConstPtr4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStructConstPtr4);
  MemberFuncStructConstPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate4<StructParam &, int32_t, int32_t, int32_t> MemberFuncStructRef4MulticastDelegate;
  MemberFuncStructRef4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStructRef4);
  MemberFuncStructRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate4<const StructParam &, int32_t, int32_t, int32_t> MemberFuncStructConstRef4MulticastDelegate;
  MemberFuncStructConstRef4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStructConstRef4);
  MemberFuncStructConstRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  // N=4 Static Functions
  MulticastDelegate4<int32_t, int32_t, int32_t, int32_t> StaticFuncInt4MulticastDelegate;
  StaticFuncInt4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncInt4);
  StaticFuncInt4MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate4<StructParam, int32_t, int32_t, int32_t> StaticFuncStruct4MulticastDelegate;
  StaticFuncStruct4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStruct4);
  StaticFuncStruct4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate4<StructParam *, int32_t, int32_t, int32_t> StaticFuncStructPtr4MulticastDelegate;
  StaticFuncStructPtr4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStructPtr4);
  StaticFuncStructPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate4<const StructParam *, int32_t, int32_t, int32_t> StaticFuncStructConstPtr4MulticastDelegate;
  StaticFuncStructConstPtr4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStructConstPtr4);
  StaticFuncStructConstPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate4<StructParam &, int32_t, int32_t, int32_t> StaticFuncStructRef4MulticastDelegate;
  StaticFuncStructRef4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStructRef4);
  StaticFuncStructRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate4<const StructParam &, int32_t, int32_t, int32_t> StaticFuncStructConstRef4MulticastDelegate;
  StaticFuncStructConstRef4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStructConstRef4);
  StaticFuncStructConstRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  // N=5 Free Functions
  MulticastDelegate5<int32_t, int32_t, int32_t, int32_t, int32_t> FreeFuncInt5MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt5MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt5MulticastDelegate);
  FreeFuncInt5MulticastDelegate += MakeDelegate(&FreeFuncInt5);
  FreeFuncInt5MulticastDelegate += MakeDelegate(&FreeFuncInt5);
  EXPECT_TRUE(FreeFuncInt5MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt5MulticastDelegate);
  FreeFuncInt5MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  FreeFuncInt5MulticastDelegate -= MakeDelegate(&FreeFuncInt5);
  // BHA	EXPECT_TRUE(FreeFuncInt5MulticastDelegate);
  FreeFuncInt5MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt5MulticastDelegate);

  MulticastDelegate5<StructParam, int32_t, int32_t, int32_t, int32_t> FreeFuncStruct5MulticastDelegate;
  FreeFuncStruct5MulticastDelegate += MakeDelegate(&FreeFuncStruct5);
  FreeFuncStruct5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate5<StructParam *, int32_t, int32_t, int32_t, int32_t> FreeFuncStructPtr5MulticastDelegate;
  FreeFuncStructPtr5MulticastDelegate += MakeDelegate(&FreeFuncStructPtr5);
  FreeFuncStructPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate5<const StructParam *, int32_t, int32_t, int32_t, int32_t> FreeFuncStructConstPtr5MulticastDelegate;
  FreeFuncStructConstPtr5MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr5);
  FreeFuncStructConstPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate5<StructParam &, int32_t, int32_t, int32_t, int32_t> FreeFuncStructRef5MulticastDelegate;
  FreeFuncStructRef5MulticastDelegate += MakeDelegate(&FreeFuncStructRef5);
  FreeFuncStructRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate5<const StructParam &, int32_t, int32_t, int32_t, int32_t> FreeFuncStructConstRef5MulticastDelegate;
  FreeFuncStructConstRef5MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef5);
  FreeFuncStructConstRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  // N=5 Member Functions
  TestClass5 testClass5;

  MulticastDelegate5<int32_t, int32_t, int32_t, int32_t, int32_t> MemberFuncInt5MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt5MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt5MulticastDelegate);
  MemberFuncInt5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncInt5);
  EXPECT_TRUE(MemberFuncInt5MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt5MulticastDelegate);
  MemberFuncInt5MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt5MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt5MulticastDelegate);

  MulticastDelegate5<StructParam, int32_t, int32_t, int32_t, int32_t> MemberFuncStruct5MulticastDelegate;
  MemberFuncStruct5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStruct5);
  MemberFuncStruct5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate5<StructParam *, int32_t, int32_t, int32_t, int32_t> MemberFuncStructPtr5MulticastDelegate;
  MemberFuncStructPtr5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStructPtr5);
  MemberFuncStructPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate5<const StructParam *, int32_t, int32_t, int32_t, int32_t> MemberFuncStructConstPtr5MulticastDelegate;
  MemberFuncStructConstPtr5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStructConstPtr5);
  MemberFuncStructConstPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate5<StructParam &, int32_t, int32_t, int32_t, int32_t> MemberFuncStructRef5MulticastDelegate;
  MemberFuncStructRef5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStructRef5);
  MemberFuncStructRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate5<const StructParam &, int32_t, int32_t, int32_t, int32_t> MemberFuncStructConstRef5MulticastDelegate;
  MemberFuncStructConstRef5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStructConstRef5);
  MemberFuncStructConstRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  // N=5 Static Functions
  MulticastDelegate5<int32_t, int32_t, int32_t, int32_t, int32_t> StaticFuncInt5MulticastDelegate;
  StaticFuncInt5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncInt5);
  StaticFuncInt5MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate5<StructParam, int32_t, int32_t, int32_t, int32_t> StaticFuncStruct5MulticastDelegate;
  StaticFuncStruct5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStruct5);
  StaticFuncStruct5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate5<StructParam *, int32_t, int32_t, int32_t, int32_t> StaticFuncStructPtr5MulticastDelegate;
  StaticFuncStructPtr5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStructPtr5);
  StaticFuncStructPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate5<const StructParam *, int32_t, int32_t, int32_t, int32_t> StaticFuncStructConstPtr5MulticastDelegate;
  StaticFuncStructConstPtr5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStructConstPtr5);
  StaticFuncStructConstPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate5<StructParam &, int32_t, int32_t, int32_t, int32_t> StaticFuncStructRef5MulticastDelegate;
  StaticFuncStructRef5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStructRef5);
  StaticFuncStructRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegate5<const StructParam &, int32_t, int32_t, int32_t, int32_t> StaticFuncStructConstRef5MulticastDelegate;
  StaticFuncStructConstRef5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStructConstRef5);
  StaticFuncStructConstRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
}

// Synchronous test of MulticastDelegateSafe<>
void MulticastDelegateSafeTests()
{
  StructParam structParam;
  structParam.val = TEST_INT;

  // N=0 Member Functions
  TestClass0 testClass0;

  MulticastDelegateSafe0 MemberFunc0MulticastDelegate;
  EXPECT_TRUE(MemberFunc0MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFunc0MulticastDelegate);
  MemberFunc0MulticastDelegate += MakeDelegate(&testClass0, &TestClass0::MemberFunc0);
  EXPECT_TRUE(MemberFunc0MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFunc0MulticastDelegate);
  MemberFunc0MulticastDelegate();
  MemberFunc0MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFunc0MulticastDelegate);

  // N=1 Member Functions
  TestClass1 testClass1;

  MulticastDelegateSafe1<int32_t> MemberFuncInt1MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt1MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt1MulticastDelegate);
  MemberFuncInt1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncInt1);
  EXPECT_TRUE(MemberFuncInt1MulticastDelegate.Empty() == false);
  // BHA EXPECT_TRUE(MemberFuncInt1MulticastDelegate);
  MemberFuncInt1MulticastDelegate(TEST_INT);
  MemberFuncInt1MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt1MulticastDelegate);

  // N=2 Member Functions
  TestClass2 testClass2;

  MulticastDelegateSafe2<int32_t, int32_t> MemberFuncInt2MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt2MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt2MulticastDelegate);
  MemberFuncInt2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncInt2);
  EXPECT_TRUE(MemberFuncInt2MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt2MulticastDelegate);
  MemberFuncInt2MulticastDelegate(TEST_INT, TEST_INT);
  MemberFuncInt2MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt2MulticastDelegate);

  // N=3 Member Functions
  TestClass3 testClass3;

  MulticastDelegateSafe3<int32_t, int32_t, int32_t> MemberFuncInt3MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt3MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt3MulticastDelegate);
  MemberFuncInt3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncInt3);
  EXPECT_TRUE(MemberFuncInt3MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt3MulticastDelegate);
  MemberFuncInt3MulticastDelegate(TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt3MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt3MulticastDelegate);

  // N=4 Member Functions
  TestClass4 testClass4;

  MulticastDelegateSafe4<int32_t, int32_t, int32_t, int32_t> MemberFuncInt4MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt4MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt4MulticastDelegate);
  MemberFuncInt4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncInt4);
  EXPECT_TRUE(MemberFuncInt4MulticastDelegate.Empty() == false);
  // BHA EXPECT_TRUE(MemberFuncInt4MulticastDelegate);
  MemberFuncInt4MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt4MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt4MulticastDelegate);

  // N=5 Member Functions
  TestClass5 testClass5;

  MulticastDelegateSafe5<int32_t, int32_t, int32_t, int32_t, int32_t> MemberFuncInt5MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt5MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt5MulticastDelegate);
  MemberFuncInt5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncInt5);
  EXPECT_TRUE(MemberFuncInt5MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt5MulticastDelegate);
  MemberFuncInt5MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt5MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt5MulticastDelegate);
}

// Asynchronous test of MulticastDelegateSafe<>
void MulticastDelegateSafeAsyncTests(BofMsgThread *_pTestThread)
{
  StructParam structParam;
  structParam.val = TEST_INT;

  // N=0 Free Functions
  MulticastDelegateSafe0 FreeFunc0MulticastDelegate;
  EXPECT_TRUE(FreeFunc0MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFunc0MulticastDelegate);
  FreeFunc0MulticastDelegate += MakeDelegate(&FreeFunc0, _pTestThread);
  FreeFunc0MulticastDelegate += MakeDelegate(&FreeFunc0, _pTestThread);
  EXPECT_TRUE(FreeFunc0MulticastDelegate.Empty() == false);
  // BHA EXPECT_TRUE(FreeFunc0MulticastDelegate);
  FreeFunc0MulticastDelegate();
  FreeFunc0MulticastDelegate -= MakeDelegate(&FreeFunc0, _pTestThread);
  // BHA	EXPECT_TRUE(FreeFunc0MulticastDelegate);
  FreeFunc0MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFunc0MulticastDelegate);

  // N=0 Member Functions
  TestClass0 testClass0;

  MulticastDelegateSafe0 MemberFunc0MulticastDelegate;
  EXPECT_TRUE(MemberFunc0MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFunc0MulticastDelegate);
  MemberFunc0MulticastDelegate += MakeDelegate(&testClass0, &TestClass0::MemberFunc0, _pTestThread);
  MemberFunc0MulticastDelegate += MakeDelegate(&testClass0, &TestClass0::MemberFunc0Const, _pTestThread);
  EXPECT_TRUE(MemberFunc0MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFunc0MulticastDelegate);
  if (MemberFunc0MulticastDelegate)
  {
    MemberFunc0MulticastDelegate();
  }
  MemberFunc0MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFunc0MulticastDelegate);

  // N=0 Static Functions
  MulticastDelegateSafe0 StaticFunc0MulticastDelegate;
  StaticFunc0MulticastDelegate += MakeDelegate(&TestClass0::StaticFunc0, _pTestThread);
  StaticFunc0MulticastDelegate();

  // N=1 Free Functions
  MulticastDelegateSafe1<int32_t> FreeFuncInt1MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt1MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt1MulticastDelegate);
  FreeFuncInt1MulticastDelegate += MakeDelegate(&FreeFuncInt1, _pTestThread);
  FreeFuncInt1MulticastDelegate += MakeDelegate(&FreeFuncInt1, _pTestThread);
  EXPECT_TRUE(FreeFuncInt1MulticastDelegate.Empty() == false);
  // BHAASSERT_TRUE(FreeFuncInt1MulticastDelegate);
  FreeFuncInt1MulticastDelegate(TEST_INT);
  FreeFuncInt1MulticastDelegate -= MakeDelegate(&FreeFuncInt1, _pTestThread);
  // BHA	EXPECT_TRUE(FreeFuncInt1MulticastDelegate);
  FreeFuncInt1MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt1MulticastDelegate);

  MulticastDelegateSafe1<StructParam> FreeFuncStruct1MulticastDelegate;
  FreeFuncStruct1MulticastDelegate += MakeDelegate(&FreeFuncStruct1);
  FreeFuncStruct1MulticastDelegate(structParam);

  MulticastDelegateSafe1<StructParam *> FreeFuncStructPtr1MulticastDelegate;
  FreeFuncStructPtr1MulticastDelegate += MakeDelegate(&FreeFuncStructPtr1, _pTestThread);
  FreeFuncStructPtr1MulticastDelegate(&structParam);

  MulticastDelegateSafe1<const StructParam *> FreeFuncStructConstPtr1MulticastDelegate;
  FreeFuncStructConstPtr1MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr1, _pTestThread);
  FreeFuncStructConstPtr1MulticastDelegate(&structParam);

  MulticastDelegateSafe1<StructParam &> FreeFuncStructRef1MulticastDelegate;
  FreeFuncStructRef1MulticastDelegate += MakeDelegate(&FreeFuncStructRef1, _pTestThread);
  FreeFuncStructRef1MulticastDelegate(structParam);

  MulticastDelegateSafe1<const StructParam &> FreeFuncStructConstRef1MulticastDelegate;
  FreeFuncStructConstRef1MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef1, _pTestThread);
  FreeFuncStructConstRef1MulticastDelegate(structParam);

  // N=1 Member Functions
  TestClass1 testClass1;

  MulticastDelegateSafe1<int32_t> MemberFuncInt1MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt1MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt1MulticastDelegate);
  MemberFuncInt1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncInt1, _pTestThread);
  MemberFuncInt1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncInt1Const, _pTestThread);
  EXPECT_TRUE(MemberFuncInt1MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt1MulticastDelegate);
  MemberFuncInt1MulticastDelegate(TEST_INT);
  MemberFuncInt1MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt1MulticastDelegate);

  MulticastDelegateSafe1<StructParam> MemberFuncStruct1MulticastDelegate;
  MemberFuncStruct1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStruct1, _pTestThread);
  if (MemberFuncStruct1MulticastDelegate)
  {
    MemberFuncStruct1MulticastDelegate(structParam);
  }

  MulticastDelegateSafe1<StructParam *> MemberFuncStructPtr1MulticastDelegate;
  MemberFuncStructPtr1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStructPtr1, _pTestThread);
  MemberFuncStructPtr1MulticastDelegate(&structParam);

  MulticastDelegateSafe1<const StructParam *> MemberFuncStructConstPtr1MulticastDelegate;
  MemberFuncStructConstPtr1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStructConstPtr1, _pTestThread);
  MemberFuncStructConstPtr1MulticastDelegate(&structParam);

  MulticastDelegateSafe1<StructParam &> MemberFuncStructRef1MulticastDelegate;
  MemberFuncStructRef1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStructRef1, _pTestThread);
  MemberFuncStructRef1MulticastDelegate(structParam);

  MulticastDelegateSafe1<const StructParam &> MemberFuncStructConstRef1MulticastDelegate;
  MemberFuncStructConstRef1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStructConstRef1, _pTestThread);
  MemberFuncStructConstRef1MulticastDelegate(structParam);

  // N=1 Static Functions
  MulticastDelegateSafe1<int32_t> StaticFuncInt1MulticastDelegate;
  StaticFuncInt1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncInt1, _pTestThread);
  StaticFuncInt1MulticastDelegate(TEST_INT);

  MulticastDelegateSafe1<StructParam> StaticFuncStruct1MulticastDelegate;
  StaticFuncStruct1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStruct1, _pTestThread);
  StaticFuncStruct1MulticastDelegate(structParam);

  MulticastDelegateSafe1<StructParam *> StaticFuncStructPtr1MulticastDelegate;
  StaticFuncStructPtr1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStructPtr1, _pTestThread);
  StaticFuncStructPtr1MulticastDelegate(&structParam);

  MulticastDelegateSafe1<const StructParam *> StaticFuncStructConstPtr1MulticastDelegate;
  StaticFuncStructConstPtr1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStructConstPtr1, _pTestThread);
  StaticFuncStructConstPtr1MulticastDelegate(&structParam);

  MulticastDelegateSafe1<StructParam &> StaticFuncStructRef1MulticastDelegate;
  StaticFuncStructRef1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStructRef1, _pTestThread);
  StaticFuncStructRef1MulticastDelegate(structParam);

  MulticastDelegateSafe1<const StructParam &> StaticFuncStructConstRef1MulticastDelegate;
  StaticFuncStructConstRef1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStructConstRef1, _pTestThread);
  StaticFuncStructConstRef1MulticastDelegate(structParam);

  // N=2 Free Functions
  MulticastDelegateSafe2<int32_t, int32_t> FreeFuncInt2MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt2MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt2MulticastDelegate);
  FreeFuncInt2MulticastDelegate += MakeDelegate(&FreeFuncInt2, _pTestThread);
  FreeFuncInt2MulticastDelegate += MakeDelegate(&FreeFuncInt2, _pTestThread);
  EXPECT_TRUE(FreeFuncInt2MulticastDelegate.Empty() == false);
  // BHAASSERT_TRUE(FreeFuncInt2MulticastDelegate);
  FreeFuncInt2MulticastDelegate(TEST_INT, TEST_INT);
  FreeFuncInt2MulticastDelegate -= MakeDelegate(&FreeFuncInt2, _pTestThread);
  // BHA	EXPECT_TRUE(FreeFuncInt2MulticastDelegate);
  FreeFuncInt2MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt2MulticastDelegate);

  MulticastDelegateSafe2<StructParam, int32_t> FreeFuncStruct2MulticastDelegate;
  FreeFuncStruct2MulticastDelegate += MakeDelegate(&FreeFuncStruct2, _pTestThread);
  FreeFuncStruct2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegateSafe2<StructParam *, int32_t> FreeFuncStructPtr2MulticastDelegate;
  FreeFuncStructPtr2MulticastDelegate += MakeDelegate(&FreeFuncStructPtr2, _pTestThread);
  FreeFuncStructPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegateSafe2<const StructParam *, int32_t> FreeFuncStructConstPtr2MulticastDelegate;
  FreeFuncStructConstPtr2MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr2, _pTestThread);
  FreeFuncStructConstPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegateSafe2<StructParam &, int32_t> FreeFuncStructRef2MulticastDelegate;
  FreeFuncStructRef2MulticastDelegate += MakeDelegate(&FreeFuncStructRef2, _pTestThread);
  FreeFuncStructRef2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegateSafe2<const StructParam &, int32_t> FreeFuncStructConstRef2MulticastDelegate;
  FreeFuncStructConstRef2MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef2, _pTestThread);
  FreeFuncStructConstRef2MulticastDelegate(structParam, TEST_INT);

  // N=2 Member Functions
  TestClass2 testClass2;

  MulticastDelegateSafe2<int32_t, int32_t> MemberFuncInt2MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt2MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt2MulticastDelegate);
  MemberFuncInt2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncInt2, _pTestThread);
  MemberFuncInt2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncInt2Const, _pTestThread);
  EXPECT_TRUE(MemberFuncInt2MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt2MulticastDelegate);
  MemberFuncInt2MulticastDelegate(TEST_INT, TEST_INT);
  MemberFuncInt2MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt2MulticastDelegate);

  MulticastDelegateSafe2<StructParam, int32_t> MemberFuncStruct2MulticastDelegate;
  MemberFuncStruct2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStruct2, _pTestThread);
  if (MemberFuncStruct2MulticastDelegate)
  {
    MemberFuncStruct2MulticastDelegate(structParam, TEST_INT);
  }

  MulticastDelegateSafe2<StructParam *, int32_t> MemberFuncStructPtr2MulticastDelegate;
  MemberFuncStructPtr2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStructPtr2, _pTestThread);
  MemberFuncStructPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegateSafe2<const StructParam *, int32_t> MemberFuncStructConstPtr2MulticastDelegate;
  MemberFuncStructConstPtr2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStructConstPtr2, _pTestThread);
  MemberFuncStructConstPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegateSafe2<StructParam &, int32_t> MemberFuncStructRef2MulticastDelegate;
  MemberFuncStructRef2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStructRef2, _pTestThread);
  MemberFuncStructRef2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegateSafe2<const StructParam &, int32_t> MemberFuncStructConstRef2MulticastDelegate;
  MemberFuncStructConstRef2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStructConstRef2, _pTestThread);
  MemberFuncStructConstRef2MulticastDelegate(structParam, TEST_INT);

  // N=2 Static Functions
  MulticastDelegateSafe2<int32_t, int32_t> StaticFuncInt2MulticastDelegate;
  StaticFuncInt2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncInt2, _pTestThread);
  StaticFuncInt2MulticastDelegate(TEST_INT, TEST_INT);

  MulticastDelegateSafe2<StructParam, int32_t> StaticFuncStruct2MulticastDelegate;
  StaticFuncStruct2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStruct2, _pTestThread);
  StaticFuncStruct2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegateSafe2<StructParam *, int32_t> StaticFuncStructPtr2MulticastDelegate;
  StaticFuncStructPtr2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStructPtr2, _pTestThread);
  StaticFuncStructPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegateSafe2<const StructParam *, int32_t> StaticFuncStructConstPtr2MulticastDelegate;
  StaticFuncStructConstPtr2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStructConstPtr2, _pTestThread);
  StaticFuncStructConstPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegateSafe2<StructParam &, int32_t> StaticFuncStructRef2MulticastDelegate;
  StaticFuncStructRef2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStructRef2, _pTestThread);
  StaticFuncStructRef2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegateSafe2<const StructParam &, int32_t> StaticFuncStructConstRef2MulticastDelegate;
  StaticFuncStructConstRef2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStructConstRef2, _pTestThread);
  StaticFuncStructConstRef2MulticastDelegate(structParam, TEST_INT);

  // N=3 Free Functions
  MulticastDelegateSafe3<int32_t, int32_t, int32_t> FreeFuncInt3MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt3MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt3MulticastDelegate);
  FreeFuncInt3MulticastDelegate += MakeDelegate(&FreeFuncInt3, _pTestThread);
  FreeFuncInt3MulticastDelegate += MakeDelegate(&FreeFuncInt3, _pTestThread);
  EXPECT_TRUE(FreeFuncInt3MulticastDelegate.Empty() == false);
  // BHAASSERT_TRUE(FreeFuncInt3MulticastDelegate);
  FreeFuncInt3MulticastDelegate(TEST_INT, TEST_INT, TEST_INT);
  FreeFuncInt3MulticastDelegate -= MakeDelegate(&FreeFuncInt3, _pTestThread);
  // BHA	EXPECT_TRUE(FreeFuncInt3MulticastDelegate);
  FreeFuncInt3MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt3MulticastDelegate);

  MulticastDelegateSafe3<StructParam, int32_t, int32_t> FreeFuncStruct3MulticastDelegate;
  FreeFuncStruct3MulticastDelegate += MakeDelegate(&FreeFuncStruct3, _pTestThread);
  FreeFuncStruct3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<StructParam *, int32_t, int32_t> FreeFuncStructPtr3MulticastDelegate;
  FreeFuncStructPtr3MulticastDelegate += MakeDelegate(&FreeFuncStructPtr3, _pTestThread);
  FreeFuncStructPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<const StructParam *, int32_t, int32_t> FreeFuncStructConstPtr3MulticastDelegate;
  FreeFuncStructConstPtr3MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr3, _pTestThread);
  FreeFuncStructConstPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<StructParam &, int32_t, int32_t> FreeFuncStructRef3MulticastDelegate;
  FreeFuncStructRef3MulticastDelegate += MakeDelegate(&FreeFuncStructRef3, _pTestThread);
  FreeFuncStructRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<const StructParam &, int32_t, int32_t> FreeFuncStructConstRef3MulticastDelegate;
  FreeFuncStructConstRef3MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef3, _pTestThread);
  FreeFuncStructConstRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  // N=3 Member Functions
  TestClass3 testClass3;

  MulticastDelegateSafe3<int32_t, int32_t, int32_t> MemberFuncInt3MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt3MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt3MulticastDelegate);
  MemberFuncInt3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncInt3, _pTestThread);
  MemberFuncInt3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncInt3Const, _pTestThread);
  EXPECT_TRUE(MemberFuncInt3MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt3MulticastDelegate);
  MemberFuncInt3MulticastDelegate(TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt3MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt3MulticastDelegate);

  MulticastDelegateSafe3<StructParam, int32_t, int32_t> MemberFuncStruct3MulticastDelegate;
  MemberFuncStruct3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStruct3, _pTestThread);
  if (MemberFuncStruct3MulticastDelegate)
  {
    MemberFuncStruct3MulticastDelegate(structParam, TEST_INT, TEST_INT);
  }

  MulticastDelegateSafe3<StructParam *, int32_t, int32_t> MemberFuncStructPtr3MulticastDelegate;
  MemberFuncStructPtr3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStructPtr3, _pTestThread);
  MemberFuncStructPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<const StructParam *, int32_t, int32_t> MemberFuncStructConstPtr3MulticastDelegate;
  MemberFuncStructConstPtr3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStructConstPtr3, _pTestThread);
  MemberFuncStructConstPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<StructParam &, int32_t, int32_t> MemberFuncStructRef3MulticastDelegate;
  MemberFuncStructRef3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStructRef3, _pTestThread);
  MemberFuncStructRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<const StructParam &, int32_t, int32_t> MemberFuncStructConstRef3MulticastDelegate;
  MemberFuncStructConstRef3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStructConstRef3, _pTestThread);
  MemberFuncStructConstRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  // N=3 Static Functions
  MulticastDelegateSafe3<int32_t, int32_t, int32_t> StaticFuncInt3MulticastDelegate;
  StaticFuncInt3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncInt3, _pTestThread);
  StaticFuncInt3MulticastDelegate(TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<StructParam, int32_t, int32_t> StaticFuncStruct3MulticastDelegate;
  StaticFuncStruct3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStruct3, _pTestThread);
  StaticFuncStruct3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<StructParam *, int32_t, int32_t> StaticFuncStructPtr3MulticastDelegate;
  StaticFuncStructPtr3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStructPtr3, _pTestThread);
  StaticFuncStructPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<const StructParam *, int32_t, int32_t> StaticFuncStructConstPtr3MulticastDelegate;
  StaticFuncStructConstPtr3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStructConstPtr3, _pTestThread);
  StaticFuncStructConstPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<StructParam &, int32_t, int32_t> StaticFuncStructRef3MulticastDelegate;
  StaticFuncStructRef3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStructRef3, _pTestThread);
  StaticFuncStructRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<const StructParam &, int32_t, int32_t> StaticFuncStructConstRef3MulticastDelegate;
  StaticFuncStructConstRef3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStructConstRef3, _pTestThread);
  StaticFuncStructConstRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  // N=4 Free Functions
  MulticastDelegateSafe4<int32_t, int32_t, int32_t, int32_t> FreeFuncInt4MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt4MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt4MulticastDelegate);
  FreeFuncInt4MulticastDelegate += MakeDelegate(&FreeFuncInt4, _pTestThread);
  FreeFuncInt4MulticastDelegate += MakeDelegate(&FreeFuncInt4, _pTestThread);
  EXPECT_TRUE(FreeFuncInt4MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt4MulticastDelegate);
  FreeFuncInt4MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  FreeFuncInt4MulticastDelegate -= MakeDelegate(&FreeFuncInt4, _pTestThread);
  // BHA	EXPECT_TRUE(FreeFuncInt4MulticastDelegate);
  FreeFuncInt4MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt4MulticastDelegate);

  MulticastDelegateSafe4<StructParam, int32_t, int32_t, int32_t> FreeFuncStruct4MulticastDelegate;
  FreeFuncStruct4MulticastDelegate += MakeDelegate(&FreeFuncStruct4, _pTestThread);
  FreeFuncStruct4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<StructParam *, int32_t, int32_t, int32_t> FreeFuncStructPtr4MulticastDelegate;
  FreeFuncStructPtr4MulticastDelegate += MakeDelegate(&FreeFuncStructPtr4, _pTestThread);
  FreeFuncStructPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<const StructParam *, int32_t, int32_t, int32_t> FreeFuncStructConstPtr4MulticastDelegate;
  FreeFuncStructConstPtr4MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr4, _pTestThread);
  FreeFuncStructConstPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<StructParam &, int32_t, int32_t, int32_t> FreeFuncStructRef4MulticastDelegate;
  FreeFuncStructRef4MulticastDelegate += MakeDelegate(&FreeFuncStructRef4, _pTestThread);
  FreeFuncStructRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<const StructParam &, int32_t, int32_t, int32_t> FreeFuncStructConstRef4MulticastDelegate;
  FreeFuncStructConstRef4MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef4, _pTestThread);
  FreeFuncStructConstRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  // N=4 Member Functions
  TestClass4 testClass4;

  MulticastDelegateSafe4<int32_t, int32_t, int32_t, int32_t> MemberFuncInt4MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt4MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt4MulticastDelegate);
  MemberFuncInt4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncInt4, _pTestThread);
  MemberFuncInt4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncInt4Const, _pTestThread);
  EXPECT_TRUE(MemberFuncInt4MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt4MulticastDelegate);
  MemberFuncInt4MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt4MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt4MulticastDelegate);

  MulticastDelegateSafe4<StructParam, int32_t, int32_t, int32_t> MemberFuncStruct4MulticastDelegate;
  MemberFuncStruct4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStruct4, _pTestThread);
  if (MemberFuncStruct4MulticastDelegate)
  {
    MemberFuncStruct4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);
  }

  MulticastDelegateSafe4<StructParam *, int32_t, int32_t, int32_t> MemberFuncStructPtr4MulticastDelegate;
  MemberFuncStructPtr4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStructPtr4, _pTestThread);
  MemberFuncStructPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<const StructParam *, int32_t, int32_t, int32_t> MemberFuncStructConstPtr4MulticastDelegate;
  MemberFuncStructConstPtr4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStructConstPtr4, _pTestThread);
  MemberFuncStructConstPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<StructParam &, int32_t, int32_t, int32_t> MemberFuncStructRef4MulticastDelegate;
  MemberFuncStructRef4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStructRef4, _pTestThread);
  MemberFuncStructRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<const StructParam &, int32_t, int32_t, int32_t> MemberFuncStructConstRef4MulticastDelegate;
  MemberFuncStructConstRef4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStructConstRef4, _pTestThread);
  MemberFuncStructConstRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  // N=4 Static Functions
  MulticastDelegateSafe4<int32_t, int32_t, int32_t, int32_t> StaticFuncInt4MulticastDelegate;
  StaticFuncInt4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncInt4, _pTestThread);
  StaticFuncInt4MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<StructParam, int32_t, int32_t, int32_t> StaticFuncStruct4MulticastDelegate;
  StaticFuncStruct4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStruct4, _pTestThread);
  StaticFuncStruct4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<StructParam *, int32_t, int32_t, int32_t> StaticFuncStructPtr4MulticastDelegate;
  StaticFuncStructPtr4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStructPtr4, _pTestThread);
  StaticFuncStructPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<const StructParam *, int32_t, int32_t, int32_t> StaticFuncStructConstPtr4MulticastDelegate;
  StaticFuncStructConstPtr4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStructConstPtr4, _pTestThread);
  StaticFuncStructConstPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<StructParam &, int32_t, int32_t, int32_t> StaticFuncStructRef4MulticastDelegate;
  StaticFuncStructRef4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStructRef4, _pTestThread);
  StaticFuncStructRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<const StructParam &, int32_t, int32_t, int32_t> StaticFuncStructConstRef4MulticastDelegate;
  StaticFuncStructConstRef4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStructConstRef4, _pTestThread);
  StaticFuncStructConstRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  // N=5 Free Functions
  MulticastDelegateSafe5<int32_t, int32_t, int32_t, int32_t, int32_t> FreeFuncInt5MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt5MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt5MulticastDelegate);
  FreeFuncInt5MulticastDelegate += MakeDelegate(&FreeFuncInt5, _pTestThread);
  FreeFuncInt5MulticastDelegate += MakeDelegate(&FreeFuncInt5, _pTestThread);
  EXPECT_TRUE(FreeFuncInt5MulticastDelegate.Empty() == false);
  // BHAASSERT_TRUE(FreeFuncInt5MulticastDelegate);
  FreeFuncInt5MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  FreeFuncInt5MulticastDelegate -= MakeDelegate(&FreeFuncInt5, _pTestThread);
  // BHA	EXPECT_TRUE(FreeFuncInt5MulticastDelegate);
  FreeFuncInt5MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt5MulticastDelegate);

  MulticastDelegateSafe5<StructParam, int32_t, int32_t, int32_t, int32_t> FreeFuncStruct5MulticastDelegate;
  FreeFuncStruct5MulticastDelegate += MakeDelegate(&FreeFuncStruct5, _pTestThread);
  FreeFuncStruct5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<StructParam *, int32_t, int32_t, int32_t, int32_t> FreeFuncStructPtr5MulticastDelegate;
  FreeFuncStructPtr5MulticastDelegate += MakeDelegate(&FreeFuncStructPtr5, _pTestThread);
  FreeFuncStructPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<const StructParam *, int32_t, int32_t, int32_t, int32_t> FreeFuncStructConstPtr5MulticastDelegate;
  FreeFuncStructConstPtr5MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr5, _pTestThread);
  FreeFuncStructConstPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<StructParam &, int32_t, int32_t, int32_t, int32_t> FreeFuncStructRef5MulticastDelegate;
  FreeFuncStructRef5MulticastDelegate += MakeDelegate(&FreeFuncStructRef5, _pTestThread);
  FreeFuncStructRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<const StructParam &, int32_t, int32_t, int32_t, int32_t> FreeFuncStructConstRef5MulticastDelegate;
  FreeFuncStructConstRef5MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef5, _pTestThread);
  FreeFuncStructConstRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  // N=5 Member Functions
  TestClass5 testClass5;

  MulticastDelegateSafe5<int32_t, int32_t, int32_t, int32_t, int32_t> MemberFuncInt5MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt5MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt5MulticastDelegate);
  MemberFuncInt5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncInt5, _pTestThread);
  MemberFuncInt5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncInt5Const, _pTestThread);
  EXPECT_TRUE(MemberFuncInt5MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt5MulticastDelegate);
  MemberFuncInt5MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt5MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt5MulticastDelegate);

  MulticastDelegateSafe5<StructParam, int32_t, int32_t, int32_t, int32_t> MemberFuncStruct5MulticastDelegate;
  MemberFuncStruct5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStruct5, _pTestThread);
  if (MemberFuncStruct5MulticastDelegate)
  {
    MemberFuncStruct5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  }

  MulticastDelegateSafe5<StructParam *, int32_t, int32_t, int32_t, int32_t> MemberFuncStructPtr5MulticastDelegate;
  MemberFuncStructPtr5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStructPtr5, _pTestThread);
  MemberFuncStructPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<const StructParam *, int32_t, int32_t, int32_t, int32_t> MemberFuncStructConstPtr5MulticastDelegate;
  MemberFuncStructConstPtr5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStructConstPtr5, _pTestThread);
  MemberFuncStructConstPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<StructParam &, int32_t, int32_t, int32_t, int32_t> MemberFuncStructRef5MulticastDelegate;
  MemberFuncStructRef5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStructRef5, _pTestThread);
  MemberFuncStructRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<const StructParam &, int32_t, int32_t, int32_t, int32_t> MemberFuncStructConstRef5MulticastDelegate;
  MemberFuncStructConstRef5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStructConstRef5, _pTestThread);
  MemberFuncStructConstRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  // N=5 Static Functions
  MulticastDelegateSafe5<int32_t, int32_t, int32_t, int32_t, int32_t> StaticFuncInt5MulticastDelegate;
  StaticFuncInt5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncInt5, _pTestThread);
  StaticFuncInt5MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<StructParam, int32_t, int32_t, int32_t, int32_t> StaticFuncStruct5MulticastDelegate;
  StaticFuncStruct5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStruct5, _pTestThread);
  StaticFuncStruct5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<StructParam *, int32_t, int32_t, int32_t, int32_t> StaticFuncStructPtr5MulticastDelegate;
  StaticFuncStructPtr5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStructPtr5, _pTestThread);
  StaticFuncStructPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<const StructParam *, int32_t, int32_t, int32_t, int32_t> StaticFuncStructConstPtr5MulticastDelegate;
  StaticFuncStructConstPtr5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStructConstPtr5, _pTestThread);
  StaticFuncStructConstPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<StructParam &, int32_t, int32_t, int32_t, int32_t> StaticFuncStructRef5MulticastDelegate;
  StaticFuncStructRef5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStructRef5, _pTestThread);
  StaticFuncStructRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<const StructParam &, int32_t, int32_t, int32_t, int32_t> StaticFuncStructConstRef5MulticastDelegate;
  StaticFuncStructConstRef5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStructConstRef5, _pTestThread);
  StaticFuncStructConstRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
}

void DelegateMemberSpTests()
{
  std::shared_ptr<TestClass0> testClass0(new TestClass0());
  auto DelegateMemberSp0 = MakeDelegate(testClass0, &TestClass0::MemberFunc0);
  DelegateMemberSp0();

  std::shared_ptr<TestClass1> testClass1(new TestClass1());
  auto DelegateMemberSp1 = MakeDelegate(testClass1, &TestClass1::MemberFuncInt1);
  DelegateMemberSp1(TEST_INT);

  std::shared_ptr<TestClass2> testClass2(new TestClass2());
  auto DelegateMemberSp2 = MakeDelegate(testClass2, &TestClass2::MemberFuncInt2);
  DelegateMemberSp2(TEST_INT, TEST_INT);

  std::shared_ptr<TestClass3> testClass3(new TestClass3());
  auto DelegateMemberSp3 = MakeDelegate(testClass3, &TestClass3::MemberFuncInt3);
  DelegateMemberSp3(TEST_INT, TEST_INT, TEST_INT);

  std::shared_ptr<TestClass4> testClass4(new TestClass4());
  auto DelegateMemberSp4 = MakeDelegate(testClass4, &TestClass4::MemberFuncInt4);
  DelegateMemberSp4(TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  std::shared_ptr<TestClass5> testClass5(new TestClass5());
  auto DelegateMemberSp5 = MakeDelegate(testClass5, &TestClass5::MemberFuncInt5);
  DelegateMemberSp5(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
}

void DelegateMemberAsyncSpTests(BofMsgThread *_pTestThread)
{
  std::shared_ptr<TestClass0> testClass0(new TestClass0());
  auto DelegateMemberAsyncSp0 = MakeDelegate(testClass0, &TestClass0::MemberFunc0, _pTestThread);
  DelegateMemberAsyncSp0();

  std::shared_ptr<TestClass1> testClass1(new TestClass1());
  auto DelegateMemberAsyncSp1 = MakeDelegate(testClass1, &TestClass1::MemberFuncInt1, _pTestThread);
  DelegateMemberAsyncSp1(TEST_INT);

  std::shared_ptr<TestClass2> testClass2(new TestClass2());
  auto DelegateMemberAsyncSp2 = MakeDelegate(testClass2, &TestClass2::MemberFuncInt2, _pTestThread);
  DelegateMemberAsyncSp2(TEST_INT, TEST_INT);

  std::shared_ptr<TestClass3> testClass3(new TestClass3());
  auto DelegateMemberAsyncSp3 = MakeDelegate(testClass3, &TestClass3::MemberFuncInt3, _pTestThread);
  DelegateMemberAsyncSp3(TEST_INT, TEST_INT, TEST_INT);

  std::shared_ptr<TestClass4> testClass4(new TestClass4());
  auto DelegateMemberAsyncSp4 = MakeDelegate(testClass4, &TestClass4::MemberFuncInt4, _pTestThread);
  DelegateMemberAsyncSp4(TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  std::shared_ptr<TestClass5> testClass5(new TestClass5());
  auto DelegateMemberAsyncSp5 = MakeDelegate(testClass5, &TestClass5::MemberFuncInt5, _pTestThread);
  DelegateMemberAsyncSp5(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
}

void DelegateMemberAsyncWaitTests(BofMsgThread *_pTestThread)
{
  const int LOOP_CNT = 100;
  StructParam structParam;
  structParam.val = TEST_INT;
  StructParam *pStructParam = &structParam;

  // N=0 Free Functions
  MulticastDelegateSafe0 FreeFunc0MulticastDelegate;
  EXPECT_TRUE(FreeFunc0MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFunc0MulticastDelegate);
  FreeFunc0MulticastDelegate += MakeDelegate(&FreeFunc0, _pTestThread, WAIT_INFINITE);
  FreeFunc0MulticastDelegate += MakeDelegate(&FreeFunc0, _pTestThread, WAIT_INFINITE);
  EXPECT_TRUE(FreeFunc0MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFunc0MulticastDelegate);
  FreeFunc0MulticastDelegate();
  FreeFunc0MulticastDelegate -= MakeDelegate(&FreeFunc0, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(FreeFunc0MulticastDelegate);
  FreeFunc0MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFunc0MulticastDelegate);

  // N=0 Member Functions
  TestClass0 testClass0;

  MulticastDelegateSafe0 MemberFunc0MulticastDelegate;
  EXPECT_TRUE(MemberFunc0MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFunc0MulticastDelegate);
  // auto d1 = MakeDelegate(&testClass0, &TestClass0::MemberFunc0, _pTestThread, WAIT_INFINITE);
  // MemberFunc0MulticastDelegate += d1;
  MemberFunc0MulticastDelegate += MakeDelegate(&testClass0, &TestClass0::MemberFunc0, _pTestThread, WAIT_INFINITE);
  MemberFunc0MulticastDelegate += MakeDelegate(&testClass0, &TestClass0::MemberFunc0Const, _pTestThread, WAIT_INFINITE);
  EXPECT_TRUE(MemberFunc0MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFunc0MulticastDelegate);
  if (MemberFunc0MulticastDelegate)
  {
    MemberFunc0MulticastDelegate();
  }
  MemberFunc0MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFunc0MulticastDelegate);

  // N=0 Static Functions
  MulticastDelegateSafe0 StaticFunc0MulticastDelegate;
  StaticFunc0MulticastDelegate += MakeDelegate(&TestClass0::StaticFunc0, _pTestThread, WAIT_INFINITE);
  StaticFunc0MulticastDelegate();

  // N=0 Free/Member Functions with Return
  auto FreeFuncIntWithReturn0Delegate = MakeDelegate(&FreeFuncIntWithReturn0, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(FreeFuncIntWithReturn0Delegate);
  if (FreeFuncIntWithReturn0Delegate)
  {
    EXPECT_TRUE(FreeFuncIntWithReturn0Delegate() == TEST_INT);
    EXPECT_TRUE(FreeFuncIntWithReturn0Delegate.IsSuccess() == true);
    FreeFuncIntWithReturn0Delegate.Clear();
  }

  auto MemberFuncIntWithReturn0Delegate = MakeDelegate(&testClass0, &TestClass0::MemberFuncWithReturn0, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(MemberFuncIntWithReturn0Delegate);
  if (MemberFuncIntWithReturn0Delegate)
  {
    EXPECT_TRUE(MemberFuncIntWithReturn0Delegate() == TEST_INT);
    EXPECT_TRUE(MemberFuncIntWithReturn0Delegate.IsSuccess() == true);
    MemberFuncIntWithReturn0Delegate.Clear();
  }

  FreeFuncIntWithReturn0Delegate = MakeDelegate(&FreeFuncIntWithReturn0, _pTestThread, 1);
  for (int i = 0; i < LOOP_CNT; i++)
  {
    int ret = FreeFuncIntWithReturn0Delegate();
  }

  MemberFuncIntWithReturn0Delegate = MakeDelegate(&testClass0, &TestClass0::MemberFuncWithReturn0, _pTestThread, 1);
  for (int i = 0; i < LOOP_CNT; i++)
  {
    int ret = MemberFuncIntWithReturn0Delegate();
  }

  // N=1 Free Functions
  MulticastDelegateSafe1<int32_t> FreeFuncInt1MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt1MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt1MulticastDelegate);
  FreeFuncInt1MulticastDelegate += MakeDelegate(&FreeFuncInt1, _pTestThread, WAIT_INFINITE);
  FreeFuncInt1MulticastDelegate += MakeDelegate(&FreeFuncInt1, _pTestThread, WAIT_INFINITE);
  EXPECT_TRUE(FreeFuncInt1MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt1MulticastDelegate);
  FreeFuncInt1MulticastDelegate(TEST_INT);
  FreeFuncInt1MulticastDelegate -= MakeDelegate(&FreeFuncInt1, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(FreeFuncInt1MulticastDelegate);
  FreeFuncInt1MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt1MulticastDelegate);

  MulticastDelegateSafe1<StructParam> FreeFuncStruct1MulticastDelegate;
  FreeFuncStruct1MulticastDelegate += MakeDelegate(&FreeFuncStruct1);
  FreeFuncStruct1MulticastDelegate(structParam);

  MulticastDelegateSafe1<StructParam *> FreeFuncStructPtr1MulticastDelegate;
  FreeFuncStructPtr1MulticastDelegate += MakeDelegate(&FreeFuncStructPtr1, _pTestThread, WAIT_INFINITE);
  FreeFuncStructPtr1MulticastDelegate(&structParam);

  MulticastDelegateSafe1<const StructParam *> FreeFuncStructConstPtr1MulticastDelegate;
  FreeFuncStructConstPtr1MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr1, _pTestThread, WAIT_INFINITE);
  FreeFuncStructConstPtr1MulticastDelegate(&structParam);

  MulticastDelegateSafe1<StructParam &> FreeFuncStructRef1MulticastDelegate;
  FreeFuncStructRef1MulticastDelegate += MakeDelegate(&FreeFuncStructRef1, _pTestThread, WAIT_INFINITE);
  FreeFuncStructRef1MulticastDelegate(structParam);

  MulticastDelegateSafe1<const StructParam &> FreeFuncStructConstRef1MulticastDelegate;
  FreeFuncStructConstRef1MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef1, _pTestThread, WAIT_INFINITE);
  FreeFuncStructConstRef1MulticastDelegate(structParam);

  // N=1 Member Functions
  TestClass1 testClass1;

  MulticastDelegateSafe1<int32_t> MemberFuncInt1MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt1MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt1MulticastDelegate);
  MemberFuncInt1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncInt1, _pTestThread, WAIT_INFINITE);
  MemberFuncInt1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncInt1Const, _pTestThread, WAIT_INFINITE);
  EXPECT_TRUE(MemberFuncInt1MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt1MulticastDelegate);
  MemberFuncInt1MulticastDelegate(TEST_INT);
  MemberFuncInt1MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt1MulticastDelegate);

  MulticastDelegateSafe1<StructParam> MemberFuncStruct1MulticastDelegate;
  MemberFuncStruct1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStruct1, _pTestThread, WAIT_INFINITE);
  if (MemberFuncStruct1MulticastDelegate)
  {
    MemberFuncStruct1MulticastDelegate(structParam);
  }

  MulticastDelegateSafe1<StructParam *> MemberFuncStructPtr1MulticastDelegate;
  MemberFuncStructPtr1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStructPtr1, _pTestThread, WAIT_INFINITE);
  MemberFuncStructPtr1MulticastDelegate(&structParam);

  MulticastDelegateSafe1<StructParam **> MemberFuncStructPtrPtr1MulticastDelegate;
  MemberFuncStructPtrPtr1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStructPtrPtr1, _pTestThread, WAIT_INFINITE);
  MemberFuncStructPtrPtr1MulticastDelegate(&pStructParam);

  MulticastDelegateSafe1<const StructParam *> MemberFuncStructConstPtr1MulticastDelegate;
  MemberFuncStructConstPtr1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStructConstPtr1, _pTestThread, WAIT_INFINITE);
  MemberFuncStructConstPtr1MulticastDelegate(&structParam);

  MulticastDelegateSafe1<StructParam &> MemberFuncStructRef1MulticastDelegate;
  MemberFuncStructRef1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStructRef1, _pTestThread, WAIT_INFINITE);
  MemberFuncStructRef1MulticastDelegate(structParam);

  MulticastDelegateSafe1<const StructParam &> MemberFuncStructConstRef1MulticastDelegate;
  MemberFuncStructConstRef1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStructConstRef1, _pTestThread, WAIT_INFINITE);
  MemberFuncStructConstRef1MulticastDelegate(structParam);

  // N=1 Static Functions
  MulticastDelegateSafe1<int32_t> StaticFuncInt1MulticastDelegate;
  StaticFuncInt1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncInt1, _pTestThread, WAIT_INFINITE);
  StaticFuncInt1MulticastDelegate(TEST_INT);

  MulticastDelegateSafe1<StructParam> StaticFuncStruct1MulticastDelegate;
  StaticFuncStruct1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStruct1, _pTestThread, WAIT_INFINITE);
  StaticFuncStruct1MulticastDelegate(structParam);

  MulticastDelegateSafe1<StructParam *> StaticFuncStructPtr1MulticastDelegate;
  StaticFuncStructPtr1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStructPtr1, _pTestThread, WAIT_INFINITE);
  StaticFuncStructPtr1MulticastDelegate(&structParam);

  MulticastDelegateSafe1<const StructParam *> StaticFuncStructConstPtr1MulticastDelegate;
  StaticFuncStructConstPtr1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStructConstPtr1, _pTestThread, WAIT_INFINITE);
  StaticFuncStructConstPtr1MulticastDelegate(&structParam);

  MulticastDelegateSafe1<StructParam &> StaticFuncStructRef1MulticastDelegate;
  StaticFuncStructRef1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStructRef1, _pTestThread, WAIT_INFINITE);
  StaticFuncStructRef1MulticastDelegate(structParam);

  MulticastDelegateSafe1<const StructParam &> StaticFuncStructConstRef1MulticastDelegate;
  StaticFuncStructConstRef1MulticastDelegate += MakeDelegate(&TestClass1::StaticFuncStructConstRef1, _pTestThread, WAIT_INFINITE);
  StaticFuncStructConstRef1MulticastDelegate(structParam);

  // N=1 Free/Member Functions with Return
  auto FreeFuncIntWithReturn1Delegate = MakeDelegate(&FreeFuncIntWithReturn1, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(FreeFuncIntWithReturn1Delegate);
  if (FreeFuncIntWithReturn1Delegate)
  {
    EXPECT_TRUE(FreeFuncIntWithReturn1Delegate(TEST_INT) == TEST_INT);
    EXPECT_TRUE(FreeFuncIntWithReturn1Delegate.IsSuccess() == true);
    FreeFuncIntWithReturn1Delegate.Clear();
  }

  auto MemberFuncIntWithReturn1Delegate = MakeDelegate(&testClass1, &TestClass1::MemberFuncIntWithReturn1, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(MemberFuncIntWithReturn1Delegate);
  if (MemberFuncIntWithReturn1Delegate)
  {
    EXPECT_TRUE(MemberFuncIntWithReturn1Delegate(TEST_INT) == TEST_INT);
    EXPECT_TRUE(MemberFuncIntWithReturn1Delegate.IsSuccess() == true);
    MemberFuncIntWithReturn1Delegate.Clear();
  }

  FreeFuncIntWithReturn1Delegate = MakeDelegate(&FreeFuncIntWithReturn1, _pTestThread, 1);
  for (int i = 0; i < LOOP_CNT; i++)
  {
    int ret = FreeFuncIntWithReturn1Delegate(TEST_INT);
  }

  MemberFuncIntWithReturn1Delegate = MakeDelegate(&testClass1, &TestClass1::MemberFuncIntWithReturn1, _pTestThread, 1);
  for (int i = 0; i < LOOP_CNT; i++)
  {
    int ret = MemberFuncIntWithReturn1Delegate(TEST_INT);
  }

  // N=2 Free Functions
  MulticastDelegateSafe2<int32_t, int32_t> FreeFuncInt2MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt2MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt2MulticastDelegate);
  FreeFuncInt2MulticastDelegate += MakeDelegate(&FreeFuncInt2, _pTestThread, WAIT_INFINITE);
  FreeFuncInt2MulticastDelegate += MakeDelegate(&FreeFuncInt2, _pTestThread, WAIT_INFINITE);
  EXPECT_TRUE(FreeFuncInt2MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt2MulticastDelegate);
  FreeFuncInt2MulticastDelegate(TEST_INT, TEST_INT);
  FreeFuncInt2MulticastDelegate -= MakeDelegate(&FreeFuncInt2, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(FreeFuncInt2MulticastDelegate);
  FreeFuncInt2MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt2MulticastDelegate);

  MulticastDelegateSafe2<StructParam, int32_t> FreeFuncStruct2MulticastDelegate;
  FreeFuncStruct2MulticastDelegate += MakeDelegate(&FreeFuncStruct2, _pTestThread, WAIT_INFINITE);
  FreeFuncStruct2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegateSafe2<StructParam *, int32_t> FreeFuncStructPtr2MulticastDelegate;
  FreeFuncStructPtr2MulticastDelegate += MakeDelegate(&FreeFuncStructPtr2, _pTestThread, WAIT_INFINITE);
  FreeFuncStructPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegateSafe2<const StructParam *, int32_t> FreeFuncStructConstPtr2MulticastDelegate;
  FreeFuncStructConstPtr2MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr2, _pTestThread, WAIT_INFINITE);
  FreeFuncStructConstPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegateSafe2<StructParam &, int32_t> FreeFuncStructRef2MulticastDelegate;
  FreeFuncStructRef2MulticastDelegate += MakeDelegate(&FreeFuncStructRef2, _pTestThread, WAIT_INFINITE);
  FreeFuncStructRef2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegateSafe2<const StructParam &, int32_t> FreeFuncStructConstRef2MulticastDelegate;
  FreeFuncStructConstRef2MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef2, _pTestThread, WAIT_INFINITE);
  FreeFuncStructConstRef2MulticastDelegate(structParam, TEST_INT);

  // N=2 Member Functions
  TestClass2 testClass2;

  MulticastDelegateSafe2<int32_t, int32_t> MemberFuncInt2MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt2MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt2MulticastDelegate);
  MemberFuncInt2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncInt2, _pTestThread, WAIT_INFINITE);
  MemberFuncInt2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncInt2Const, _pTestThread, WAIT_INFINITE);
  EXPECT_TRUE(MemberFuncInt2MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt2MulticastDelegate);
  MemberFuncInt2MulticastDelegate(TEST_INT, TEST_INT);
  MemberFuncInt2MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt2MulticastDelegate);

  MulticastDelegateSafe2<StructParam, int32_t> MemberFuncStruct2MulticastDelegate;
  MemberFuncStruct2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStruct2, _pTestThread, WAIT_INFINITE);
  if (MemberFuncStruct2MulticastDelegate)
  {
    MemberFuncStruct2MulticastDelegate(structParam, TEST_INT);
  }

  MulticastDelegateSafe2<StructParam *, int32_t> MemberFuncStructPtr2MulticastDelegate;
  MemberFuncStructPtr2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStructPtr2, _pTestThread, WAIT_INFINITE);
  MemberFuncStructPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegateSafe2<const StructParam *, int32_t> MemberFuncStructConstPtr2MulticastDelegate;
  MemberFuncStructConstPtr2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStructConstPtr2, _pTestThread, WAIT_INFINITE);
  MemberFuncStructConstPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegateSafe2<StructParam &, int32_t> MemberFuncStructRef2MulticastDelegate;
  MemberFuncStructRef2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStructRef2, _pTestThread, WAIT_INFINITE);
  MemberFuncStructRef2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegateSafe2<const StructParam &, int32_t> MemberFuncStructConstRef2MulticastDelegate;
  MemberFuncStructConstRef2MulticastDelegate += MakeDelegate(&testClass2, &TestClass2::MemberFuncStructConstRef2, _pTestThread, WAIT_INFINITE);
  MemberFuncStructConstRef2MulticastDelegate(structParam, TEST_INT);

  // N=2 Static Functions
  MulticastDelegateSafe2<int32_t, int32_t> StaticFuncInt2MulticastDelegate;
  StaticFuncInt2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncInt2, _pTestThread, WAIT_INFINITE);
  StaticFuncInt2MulticastDelegate(TEST_INT, TEST_INT);

  MulticastDelegateSafe2<StructParam, int32_t> StaticFuncStruct2MulticastDelegate;
  StaticFuncStruct2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStruct2, _pTestThread, WAIT_INFINITE);
  StaticFuncStruct2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegateSafe2<StructParam *, int32_t> StaticFuncStructPtr2MulticastDelegate;
  StaticFuncStructPtr2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStructPtr2, _pTestThread, WAIT_INFINITE);
  StaticFuncStructPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegateSafe2<const StructParam *, int32_t> StaticFuncStructConstPtr2MulticastDelegate;
  StaticFuncStructConstPtr2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStructConstPtr2, _pTestThread, WAIT_INFINITE);
  StaticFuncStructConstPtr2MulticastDelegate(&structParam, TEST_INT);

  MulticastDelegateSafe2<StructParam &, int32_t> StaticFuncStructRef2MulticastDelegate;
  StaticFuncStructRef2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStructRef2, _pTestThread, WAIT_INFINITE);
  StaticFuncStructRef2MulticastDelegate(structParam, TEST_INT);

  MulticastDelegateSafe2<const StructParam &, int32_t> StaticFuncStructConstRef2MulticastDelegate;
  StaticFuncStructConstRef2MulticastDelegate += MakeDelegate(&TestClass2::StaticFuncStructConstRef2, _pTestThread, WAIT_INFINITE);
  StaticFuncStructConstRef2MulticastDelegate(structParam, TEST_INT);

  // N=2 Free/Member Functions with Return
  auto FreeFuncIntWithReturn2Delegate = MakeDelegate(&FreeFuncIntWithReturn2, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(FreeFuncIntWithReturn2Delegate);
  if (FreeFuncIntWithReturn2Delegate)
  {
    EXPECT_TRUE(FreeFuncIntWithReturn2Delegate(TEST_INT, TEST_INT) == TEST_INT);
    EXPECT_TRUE(FreeFuncIntWithReturn2Delegate.IsSuccess() == true);
    FreeFuncIntWithReturn2Delegate.Clear();
  }

  auto MemberFuncIntWithReturn2Delegate = MakeDelegate(&testClass2, &TestClass2::MemberFuncIntWithReturn2, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(MemberFuncIntWithReturn2Delegate);
  if (MemberFuncIntWithReturn2Delegate)
  {
    EXPECT_TRUE(MemberFuncIntWithReturn2Delegate(TEST_INT, TEST_INT) == TEST_INT);
    EXPECT_TRUE(MemberFuncIntWithReturn2Delegate.IsSuccess() == true);
    MemberFuncIntWithReturn2Delegate.Clear();
  }

  FreeFuncIntWithReturn2Delegate = MakeDelegate(&FreeFuncIntWithReturn2, _pTestThread, 1);
  for (int i = 0; i < LOOP_CNT; i++)
  {
    int ret = FreeFuncIntWithReturn2Delegate(TEST_INT, TEST_INT);
  }

  MemberFuncIntWithReturn2Delegate = MakeDelegate(&testClass2, &TestClass2::MemberFuncIntWithReturn2, _pTestThread, 1);
  for (int i = 0; i < LOOP_CNT; i++)
  {
    int ret = MemberFuncIntWithReturn2Delegate(TEST_INT, TEST_INT);
  }

  // N=3 Free Functions
  MulticastDelegateSafe3<int32_t, int32_t, int32_t> FreeFuncInt3MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt3MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt3MulticastDelegate);
  FreeFuncInt3MulticastDelegate += MakeDelegate(&FreeFuncInt3, _pTestThread, WAIT_INFINITE);
  FreeFuncInt3MulticastDelegate += MakeDelegate(&FreeFuncInt3, _pTestThread, WAIT_INFINITE);
  EXPECT_TRUE(FreeFuncInt3MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt3MulticastDelegate);
  FreeFuncInt3MulticastDelegate(TEST_INT, TEST_INT, TEST_INT);
  FreeFuncInt3MulticastDelegate -= MakeDelegate(&FreeFuncInt3, _pTestThread, WAIT_INFINITE);
  // BHA EXPECT_TRUE(FreeFuncInt3MulticastDelegate);
  FreeFuncInt3MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt3MulticastDelegate);

  MulticastDelegateSafe3<StructParam, int32_t, int32_t> FreeFuncStruct3MulticastDelegate;
  FreeFuncStruct3MulticastDelegate += MakeDelegate(&FreeFuncStruct3, _pTestThread, WAIT_INFINITE);
  FreeFuncStruct3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<StructParam *, int32_t, int32_t> FreeFuncStructPtr3MulticastDelegate;
  FreeFuncStructPtr3MulticastDelegate += MakeDelegate(&FreeFuncStructPtr3, _pTestThread, WAIT_INFINITE);
  FreeFuncStructPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<const StructParam *, int32_t, int32_t> FreeFuncStructConstPtr3MulticastDelegate;
  FreeFuncStructConstPtr3MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr3, _pTestThread, WAIT_INFINITE);
  FreeFuncStructConstPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<StructParam &, int32_t, int32_t> FreeFuncStructRef3MulticastDelegate;
  FreeFuncStructRef3MulticastDelegate += MakeDelegate(&FreeFuncStructRef3, _pTestThread, WAIT_INFINITE);
  FreeFuncStructRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<const StructParam &, int32_t, int32_t> FreeFuncStructConstRef3MulticastDelegate;
  FreeFuncStructConstRef3MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef3, _pTestThread, WAIT_INFINITE);
  FreeFuncStructConstRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  // N=3 Member Functions
  TestClass3 testClass3;

  MulticastDelegateSafe3<int32_t, int32_t, int32_t> MemberFuncInt3MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt3MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt3MulticastDelegate);
  MemberFuncInt3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncInt3, _pTestThread, WAIT_INFINITE);
  MemberFuncInt3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncInt3Const, _pTestThread, WAIT_INFINITE);
  EXPECT_TRUE(MemberFuncInt3MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt3MulticastDelegate);
  MemberFuncInt3MulticastDelegate(TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt3MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt3MulticastDelegate);

  MulticastDelegateSafe3<StructParam, int32_t, int32_t> MemberFuncStruct3MulticastDelegate;
  MemberFuncStruct3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStruct3, _pTestThread, WAIT_INFINITE);
  if (MemberFuncStruct3MulticastDelegate)
  {
    MemberFuncStruct3MulticastDelegate(structParam, TEST_INT, TEST_INT);
  }

  MulticastDelegateSafe3<StructParam *, int32_t, int32_t> MemberFuncStructPtr3MulticastDelegate;
  MemberFuncStructPtr3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStructPtr3, _pTestThread, WAIT_INFINITE);
  MemberFuncStructPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<const StructParam *, int32_t, int32_t> MemberFuncStructConstPtr3MulticastDelegate;
  MemberFuncStructConstPtr3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStructConstPtr3, _pTestThread, WAIT_INFINITE);
  MemberFuncStructConstPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<StructParam &, int32_t, int32_t> MemberFuncStructRef3MulticastDelegate;
  MemberFuncStructRef3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStructRef3, _pTestThread, WAIT_INFINITE);
  MemberFuncStructRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<const StructParam &, int32_t, int32_t> MemberFuncStructConstRef3MulticastDelegate;
  MemberFuncStructConstRef3MulticastDelegate += MakeDelegate(&testClass3, &TestClass3::MemberFuncStructConstRef3, _pTestThread, WAIT_INFINITE);
  MemberFuncStructConstRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  // N=3 Static Functions
  MulticastDelegateSafe3<int32_t, int32_t, int32_t> StaticFuncInt3MulticastDelegate;
  StaticFuncInt3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncInt3, _pTestThread, WAIT_INFINITE);
  StaticFuncInt3MulticastDelegate(TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<StructParam, int32_t, int32_t> StaticFuncStruct3MulticastDelegate;
  StaticFuncStruct3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStruct3, _pTestThread, WAIT_INFINITE);
  StaticFuncStruct3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<StructParam *, int32_t, int32_t> StaticFuncStructPtr3MulticastDelegate;
  StaticFuncStructPtr3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStructPtr3, _pTestThread, WAIT_INFINITE);
  StaticFuncStructPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<const StructParam *, int32_t, int32_t> StaticFuncStructConstPtr3MulticastDelegate;
  StaticFuncStructConstPtr3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStructConstPtr3, _pTestThread, WAIT_INFINITE);
  StaticFuncStructConstPtr3MulticastDelegate(&structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<StructParam &, int32_t, int32_t> StaticFuncStructRef3MulticastDelegate;
  StaticFuncStructRef3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStructRef3, _pTestThread, WAIT_INFINITE);
  StaticFuncStructRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  MulticastDelegateSafe3<const StructParam &, int32_t, int32_t> StaticFuncStructConstRef3MulticastDelegate;
  StaticFuncStructConstRef3MulticastDelegate += MakeDelegate(&TestClass3::StaticFuncStructConstRef3, _pTestThread, WAIT_INFINITE);
  StaticFuncStructConstRef3MulticastDelegate(structParam, TEST_INT, TEST_INT);

  // N=3 Free/Member Functions with Return
  auto FreeFuncIntWithReturn3Delegate = MakeDelegate(&FreeFuncIntWithReturn3, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(FreeFuncIntWithReturn3Delegate);
  if (FreeFuncIntWithReturn3Delegate)
  {
    EXPECT_TRUE(FreeFuncIntWithReturn3Delegate(TEST_INT, TEST_INT, TEST_INT) == TEST_INT);
    EXPECT_TRUE(FreeFuncIntWithReturn3Delegate.IsSuccess() == true);
    FreeFuncIntWithReturn3Delegate.Clear();
  }

  auto MemberFuncIntWithReturn3Delegate = MakeDelegate(&testClass3, &TestClass3::MemberFuncIntWithReturn3, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(MemberFuncIntWithReturn3Delegate);
  if (MemberFuncIntWithReturn3Delegate)
  {
    EXPECT_TRUE(MemberFuncIntWithReturn3Delegate(TEST_INT, TEST_INT, TEST_INT) == TEST_INT);
    EXPECT_TRUE(MemberFuncIntWithReturn3Delegate.IsSuccess() == true);
    MemberFuncIntWithReturn3Delegate.Clear();
  }

  FreeFuncIntWithReturn3Delegate = MakeDelegate(&FreeFuncIntWithReturn3, _pTestThread, 1);
  for (int i = 0; i < LOOP_CNT; i++)
  {
    int ret = FreeFuncIntWithReturn3Delegate(TEST_INT, TEST_INT, TEST_INT);
  }

  MemberFuncIntWithReturn3Delegate = MakeDelegate(&testClass3, &TestClass3::MemberFuncIntWithReturn3, _pTestThread, 1);
  for (int i = 0; i < LOOP_CNT; i++)
  {
    int ret = MemberFuncIntWithReturn3Delegate(TEST_INT, TEST_INT, TEST_INT);
  }

  // N=4 Free Functions
  MulticastDelegateSafe4<int32_t, int32_t, int32_t, int32_t> FreeFuncInt4MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt4MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt4MulticastDelegate);
  FreeFuncInt4MulticastDelegate += MakeDelegate(&FreeFuncInt4, _pTestThread, WAIT_INFINITE);
  FreeFuncInt4MulticastDelegate += MakeDelegate(&FreeFuncInt4, _pTestThread, WAIT_INFINITE);
  EXPECT_TRUE(FreeFuncInt4MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt4MulticastDelegate);
  FreeFuncInt4MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  FreeFuncInt4MulticastDelegate -= MakeDelegate(&FreeFuncInt4, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(FreeFuncInt4MulticastDelegate);
  FreeFuncInt4MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt4MulticastDelegate);

  MulticastDelegateSafe4<StructParam, int32_t, int32_t, int32_t> FreeFuncStruct4MulticastDelegate;
  FreeFuncStruct4MulticastDelegate += MakeDelegate(&FreeFuncStruct4, _pTestThread, WAIT_INFINITE);
  FreeFuncStruct4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<StructParam *, int32_t, int32_t, int32_t> FreeFuncStructPtr4MulticastDelegate;
  FreeFuncStructPtr4MulticastDelegate += MakeDelegate(&FreeFuncStructPtr4, _pTestThread, WAIT_INFINITE);
  FreeFuncStructPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<const StructParam *, int32_t, int32_t, int32_t> FreeFuncStructConstPtr4MulticastDelegate;
  FreeFuncStructConstPtr4MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr4, _pTestThread, WAIT_INFINITE);
  FreeFuncStructConstPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<StructParam &, int32_t, int32_t, int32_t> FreeFuncStructRef4MulticastDelegate;
  FreeFuncStructRef4MulticastDelegate += MakeDelegate(&FreeFuncStructRef4, _pTestThread, WAIT_INFINITE);
  FreeFuncStructRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<const StructParam &, int32_t, int32_t, int32_t> FreeFuncStructConstRef4MulticastDelegate;
  FreeFuncStructConstRef4MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef4, _pTestThread, WAIT_INFINITE);
  FreeFuncStructConstRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  // N=4 Member Functions
  TestClass4 testClass4;

  MulticastDelegateSafe4<int32_t, int32_t, int32_t, int32_t> MemberFuncInt4MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt4MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt4MulticastDelegate);
  MemberFuncInt4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncInt4, _pTestThread, WAIT_INFINITE);
  MemberFuncInt4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncInt4Const, _pTestThread, WAIT_INFINITE);
  EXPECT_TRUE(MemberFuncInt4MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt4MulticastDelegate);
  MemberFuncInt4MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt4MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt4MulticastDelegate);

  MulticastDelegateSafe4<StructParam, int32_t, int32_t, int32_t> MemberFuncStruct4MulticastDelegate;
  MemberFuncStruct4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStruct4, _pTestThread, WAIT_INFINITE);
  if (MemberFuncStruct4MulticastDelegate)
  {
    MemberFuncStruct4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);
  }

  MulticastDelegateSafe4<StructParam *, int32_t, int32_t, int32_t> MemberFuncStructPtr4MulticastDelegate;
  MemberFuncStructPtr4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStructPtr4, _pTestThread, WAIT_INFINITE);
  MemberFuncStructPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<const StructParam *, int32_t, int32_t, int32_t> MemberFuncStructConstPtr4MulticastDelegate;
  MemberFuncStructConstPtr4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStructConstPtr4, _pTestThread, WAIT_INFINITE);
  MemberFuncStructConstPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<StructParam &, int32_t, int32_t, int32_t> MemberFuncStructRef4MulticastDelegate;
  MemberFuncStructRef4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStructRef4, _pTestThread, WAIT_INFINITE);
  MemberFuncStructRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<const StructParam &, int32_t, int32_t, int32_t> MemberFuncStructConstRef4MulticastDelegate;
  MemberFuncStructConstRef4MulticastDelegate += MakeDelegate(&testClass4, &TestClass4::MemberFuncStructConstRef4, _pTestThread, WAIT_INFINITE);
  MemberFuncStructConstRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  // N=4 Static Functions
  MulticastDelegateSafe4<int32_t, int32_t, int32_t, int32_t> StaticFuncInt4MulticastDelegate;
  StaticFuncInt4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncInt4, _pTestThread, WAIT_INFINITE);
  StaticFuncInt4MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<StructParam, int32_t, int32_t, int32_t> StaticFuncStruct4MulticastDelegate;
  StaticFuncStruct4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStruct4, _pTestThread, WAIT_INFINITE);
  StaticFuncStruct4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<StructParam *, int32_t, int32_t, int32_t> StaticFuncStructPtr4MulticastDelegate;
  StaticFuncStructPtr4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStructPtr4, _pTestThread, WAIT_INFINITE);
  StaticFuncStructPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<const StructParam *, int32_t, int32_t, int32_t> StaticFuncStructConstPtr4MulticastDelegate;
  StaticFuncStructConstPtr4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStructConstPtr4, _pTestThread, WAIT_INFINITE);
  StaticFuncStructConstPtr4MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<StructParam &, int32_t, int32_t, int32_t> StaticFuncStructRef4MulticastDelegate;
  StaticFuncStructRef4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStructRef4, _pTestThread, WAIT_INFINITE);
  StaticFuncStructRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe4<const StructParam &, int32_t, int32_t, int32_t> StaticFuncStructConstRef4MulticastDelegate;
  StaticFuncStructConstRef4MulticastDelegate += MakeDelegate(&TestClass4::StaticFuncStructConstRef4, _pTestThread, WAIT_INFINITE);
  StaticFuncStructConstRef4MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT);

  // N=4 Free/Member Functions with Return
  auto FreeFuncIntWithReturn4Delegate = MakeDelegate(&FreeFuncIntWithReturn4, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(FreeFuncIntWithReturn4Delegate);
  if (FreeFuncIntWithReturn4Delegate)
  {
    EXPECT_TRUE(FreeFuncIntWithReturn4Delegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT) == TEST_INT);
    EXPECT_TRUE(FreeFuncIntWithReturn4Delegate.IsSuccess() == true);
    FreeFuncIntWithReturn4Delegate.Clear();
  }

  auto MemberFuncIntWithReturn4Delegate = MakeDelegate(&testClass4, &TestClass4::MemberFuncIntWithReturn4, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(MemberFuncIntWithReturn4Delegate);
  if (MemberFuncIntWithReturn4Delegate)
  {
    EXPECT_TRUE(MemberFuncIntWithReturn4Delegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT) == TEST_INT);
    EXPECT_TRUE(MemberFuncIntWithReturn4Delegate.IsSuccess() == true);
    MemberFuncIntWithReturn4Delegate.Clear();
  }

  FreeFuncIntWithReturn4Delegate = MakeDelegate(&FreeFuncIntWithReturn4, _pTestThread, 1);
  for (int i = 0; i < LOOP_CNT; i++)
  {
    int ret = FreeFuncIntWithReturn4Delegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  }

  MemberFuncIntWithReturn4Delegate = MakeDelegate(&testClass4, &TestClass4::MemberFuncIntWithReturn4, _pTestThread, 1);
  for (int i = 0; i < LOOP_CNT; i++)
  {
    int ret = MemberFuncIntWithReturn4Delegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  }

  // N=5 Free Functions
  MulticastDelegateSafe5<int32_t, int32_t, int32_t, int32_t, int32_t> FreeFuncInt5MulticastDelegate;
  EXPECT_TRUE(FreeFuncInt5MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!FreeFuncInt5MulticastDelegate);
  FreeFuncInt5MulticastDelegate += MakeDelegate(&FreeFuncInt5, _pTestThread, WAIT_INFINITE);
  FreeFuncInt5MulticastDelegate += MakeDelegate(&FreeFuncInt5, _pTestThread, WAIT_INFINITE);
  EXPECT_TRUE(FreeFuncInt5MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(FreeFuncInt5MulticastDelegate);
  FreeFuncInt5MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  FreeFuncInt5MulticastDelegate -= MakeDelegate(&FreeFuncInt5, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(FreeFuncInt5MulticastDelegate);
  FreeFuncInt5MulticastDelegate.Clear();
  EXPECT_TRUE(!FreeFuncInt5MulticastDelegate);

  MulticastDelegateSafe5<StructParam, int32_t, int32_t, int32_t, int32_t> FreeFuncStruct5MulticastDelegate;
  FreeFuncStruct5MulticastDelegate += MakeDelegate(&FreeFuncStruct5, _pTestThread, WAIT_INFINITE);
  FreeFuncStruct5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<StructParam *, int32_t, int32_t, int32_t, int32_t> FreeFuncStructPtr5MulticastDelegate;
  FreeFuncStructPtr5MulticastDelegate += MakeDelegate(&FreeFuncStructPtr5, _pTestThread, WAIT_INFINITE);
  FreeFuncStructPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<const StructParam *, int32_t, int32_t, int32_t, int32_t> FreeFuncStructConstPtr5MulticastDelegate;
  FreeFuncStructConstPtr5MulticastDelegate += MakeDelegate(&FreeFuncStructConstPtr5, _pTestThread, WAIT_INFINITE);
  FreeFuncStructConstPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<StructParam &, int32_t, int32_t, int32_t, int32_t> FreeFuncStructRef5MulticastDelegate;
  FreeFuncStructRef5MulticastDelegate += MakeDelegate(&FreeFuncStructRef5, _pTestThread, WAIT_INFINITE);
  FreeFuncStructRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<const StructParam &, int32_t, int32_t, int32_t, int32_t> FreeFuncStructConstRef5MulticastDelegate;
  FreeFuncStructConstRef5MulticastDelegate += MakeDelegate(&FreeFuncStructConstRef5, _pTestThread, WAIT_INFINITE);
  FreeFuncStructConstRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  // N=5 Member Functions
  TestClass5 testClass5;

  MulticastDelegateSafe5<int32_t, int32_t, int32_t, int32_t, int32_t> MemberFuncInt5MulticastDelegate;
  EXPECT_TRUE(MemberFuncInt5MulticastDelegate.Empty() == true);
  EXPECT_TRUE(!MemberFuncInt5MulticastDelegate);
  MemberFuncInt5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncInt5, _pTestThread, WAIT_INFINITE);
  MemberFuncInt5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncInt5Const, _pTestThread, WAIT_INFINITE);
  EXPECT_TRUE(MemberFuncInt5MulticastDelegate.Empty() == false);
  // BHA	EXPECT_TRUE(MemberFuncInt5MulticastDelegate);
  MemberFuncInt5MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  MemberFuncInt5MulticastDelegate.Clear();
  EXPECT_TRUE(!MemberFuncInt5MulticastDelegate);

  MulticastDelegateSafe5<StructParam, int32_t, int32_t, int32_t, int32_t> MemberFuncStruct5MulticastDelegate;
  MemberFuncStruct5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStruct5, _pTestThread, WAIT_INFINITE);
  if (MemberFuncStruct5MulticastDelegate)
  {
    MemberFuncStruct5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  }

  MulticastDelegateSafe5<StructParam *, int32_t, int32_t, int32_t, int32_t> MemberFuncStructPtr5MulticastDelegate;
  MemberFuncStructPtr5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStructPtr5, _pTestThread, WAIT_INFINITE);
  MemberFuncStructPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<const StructParam *, int32_t, int32_t, int32_t, int32_t> MemberFuncStructConstPtr5MulticastDelegate;
  MemberFuncStructConstPtr5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStructConstPtr5, _pTestThread, WAIT_INFINITE);
  MemberFuncStructConstPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<StructParam &, int32_t, int32_t, int32_t, int32_t> MemberFuncStructRef5MulticastDelegate;
  MemberFuncStructRef5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStructRef5, _pTestThread, WAIT_INFINITE);
  MemberFuncStructRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<const StructParam &, int32_t, int32_t, int32_t, int32_t> MemberFuncStructConstRef5MulticastDelegate;
  MemberFuncStructConstRef5MulticastDelegate += MakeDelegate(&testClass5, &TestClass5::MemberFuncStructConstRef5, _pTestThread, WAIT_INFINITE);
  MemberFuncStructConstRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  // N=5 Static Functions
  MulticastDelegateSafe5<int32_t, int32_t, int32_t, int32_t, int32_t> StaticFuncInt5MulticastDelegate;
  StaticFuncInt5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncInt5, _pTestThread, WAIT_INFINITE);
  StaticFuncInt5MulticastDelegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<StructParam, int32_t, int32_t, int32_t, int32_t> StaticFuncStruct5MulticastDelegate;
  StaticFuncStruct5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStruct5, _pTestThread, WAIT_INFINITE);
  StaticFuncStruct5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<StructParam *, int32_t, int32_t, int32_t, int32_t> StaticFuncStructPtr5MulticastDelegate;
  StaticFuncStructPtr5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStructPtr5, _pTestThread, WAIT_INFINITE);
  StaticFuncStructPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<const StructParam *, int32_t, int32_t, int32_t, int32_t> StaticFuncStructConstPtr5MulticastDelegate;
  StaticFuncStructConstPtr5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStructConstPtr5, _pTestThread, WAIT_INFINITE);
  StaticFuncStructConstPtr5MulticastDelegate(&structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<StructParam &, int32_t, int32_t, int32_t, int32_t> StaticFuncStructRef5MulticastDelegate;
  StaticFuncStructRef5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStructRef5, _pTestThread, WAIT_INFINITE);
  StaticFuncStructRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  MulticastDelegateSafe5<const StructParam &, int32_t, int32_t, int32_t, int32_t> StaticFuncStructConstRef5MulticastDelegate;
  StaticFuncStructConstRef5MulticastDelegate += MakeDelegate(&TestClass5::StaticFuncStructConstRef5, _pTestThread, WAIT_INFINITE);
  StaticFuncStructConstRef5MulticastDelegate(structParam, TEST_INT, TEST_INT, TEST_INT, TEST_INT);

  // N=5 Free/Member Functions with Return
  auto FreeFuncIntWithReturn5Delegate = MakeDelegate(&FreeFuncIntWithReturn5, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(FreeFuncIntWithReturn5Delegate);
  if (FreeFuncIntWithReturn5Delegate)
  {
    EXPECT_TRUE(FreeFuncIntWithReturn5Delegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT) == TEST_INT);
    EXPECT_TRUE(FreeFuncIntWithReturn5Delegate.IsSuccess() == true);
    FreeFuncIntWithReturn5Delegate.Clear();
  }

  auto MemberFuncIntWithReturn5Delegate = MakeDelegate(&testClass5, &TestClass5::MemberFuncIntWithReturn5, _pTestThread, WAIT_INFINITE);
  // BHA	EXPECT_TRUE(MemberFuncIntWithReturn5Delegate);
  if (MemberFuncIntWithReturn5Delegate)
  {
    EXPECT_TRUE(MemberFuncIntWithReturn5Delegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT) == TEST_INT);
    EXPECT_TRUE(MemberFuncIntWithReturn5Delegate.IsSuccess() == true);
    MemberFuncIntWithReturn5Delegate.Clear();
  }

  FreeFuncIntWithReturn5Delegate = MakeDelegate(&FreeFuncIntWithReturn5, _pTestThread, 1);
  for (int i = 0; i < LOOP_CNT; i++)
  {
    int ret = FreeFuncIntWithReturn5Delegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  }

  MemberFuncIntWithReturn5Delegate = MakeDelegate(&testClass5, &TestClass5::MemberFuncIntWithReturn5, _pTestThread, 1);
  for (int i = 0; i < LOOP_CNT; i++)
  {
    int ret = MemberFuncIntWithReturn5Delegate(TEST_INT, TEST_INT, TEST_INT, TEST_INT, TEST_INT);
  }
}

TEST(Async_Test, AsyncMulticastDelegateLib)
{
  BofMsgThread testThread;

  testThread.LaunchThread("DelegateUnitTestsThread", BOF::BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF::BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000, 0);

  // Run unit tests repeatedly to expose problems (e.g. deadlocks, memory leaks) a
  // with async delegates.
  for (int i = 0; i < 100; i++)
  {
    SinglecastDelegateTests();
    MulticastDelegateTests();
    MulticastDelegateSafeTests();
    MulticastDelegateSafeAsyncTests(&testThread);
    DelegateMemberAsyncWaitTests(&testThread);

    DelegateMemberSpTests();
    DelegateMemberAsyncSpTests(&testThread);
  }

  //  testThread.ExitThread();
  // xalloc_stats();
}

TEST(Async_Test, MulticastDelegatePerf)
{
  BofMsgThread testThread;
  TestClass1 testClass1;
  struct StructParam structParam;
  uint32_t StartInMs_U32, i_U32;

  testThread.LaunchThread("DelegateUnitTestsThread", BOF::BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF::BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000, 0);

  MulticastDelegateSafe1<const StructParam *> MemberFuncStructConstPtr1MulticastDelegate;
  MemberFuncStructConstPtr1MulticastDelegate += MakeDelegate(&testClass1, &TestClass1::MemberFuncStructConstPtr1Bha, &testThread);

  StartInMs_U32 = BOF::Bof_GetMsTickCount();
  // MulticastDelegateSafeAsyncTests();
  for (i_U32 = 0; i_U32 < 100000; i_U32++)
  {
    structParam.val = i_U32;
    MemberFuncStructConstPtr1MulticastDelegate(&structParam);
  }
  // printf("%d notif in %d ms\n", i_U32, BOF::Bof_ElapsedMsTime(StartInMs_U32));
  MemberFuncStructConstPtr1MulticastDelegate.Clear();

  std::shared_ptr<TestClass1> psTestClass1(new TestClass1());
  MemberFuncStructConstPtr1MulticastDelegate += MakeDelegate(psTestClass1, &TestClass1::MemberFuncStructConstPtr2Bha, &testThread);

  // auto DelegateMemberAsyncSp1 = MakeDelegate(testClass1, &TestClass1::MemberFuncStructConstPtr1Bha, &testThread);
  StartInMs_U32 = BOF::Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < 100000; i_U32++)
  {
    structParam.val = i_U32;
    MemberFuncStructConstPtr1MulticastDelegate(&structParam);
  }
  // printf("%d notif in %d ms\n", i_U32, BOF::Bof_ElapsedMsTime(StartInMs_U32));
}

struct NOTIFY_ARG
{
  int *pUserContext;
  uint32_t Cpt_U32;
  uint32_t NotifyEventMask_U32;
};
void MulticastNotifyCallback(const NOTIFY_ARG *_pNotifyArg_X)
{
  if (_pNotifyArg_X)
  {
    // if ((_pNotifyArg_X->Cpt_U32 % 1000)==0)
    {
      //      printf("MulticastNotifyCallback: %p val %x %d %x\n", _pNotifyArg_X->pUserContext,*_pNotifyArg_X->pUserContext, _pNotifyArg_X->Cpt_U32, _pNotifyArg_X->NotifyEventMask_U32);
    }
  }
}
static int a = 0x12345678;
static int b = 0x90ABCDEF;
static int c = 0x87654321;

// #include <type_traits>
TEST(Async_Test, AsyncNotifier)
{
  BOF::BOF_MULTICAST_ASYNC_NOTIFIER_PARAM MulticastAsyncNotifierParam_X;
  uint32_t StartInMs_U32, i_U32;
  NOTIFY_ARG MulticastNotifyArg_X;
  MulticastAsyncNotifierParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER;
  MulticastAsyncNotifierParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000;
  MulticastAsyncNotifierParam_X.ThreadCpuCoreAffinityMask_U64 = 0;
  //  MulticastAsyncNotifierParam_X.MaxPendingRequest_U32     = 100;

  //  std::cout << std::is_pointer<StructParam>::value << '\n';
  /*
    BofMsgThread       testThread;
    testThread.LaunchThread("DelegateUnitTestsThread", 0, BOF::BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER, BOF::BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000);
    StructParam structParam;
    MulticastDelegateSafe1<StructParam &> FreeFuncStructRef1MulticastDelegate;
    FreeFuncStructRef1MulticastDelegate += MakeDelegate(&FreeFuncStructRef1, &testThread);
    FreeFuncStructRef1MulticastDelegate(structParam);
  */
  BOF::BofMulticastAsyncNotifier<NOTIFY_ARG> MulticastAsyncNotifier(MulticastAsyncNotifierParam_X);

  ASSERT_EQ(MulticastAsyncNotifier.Register(&MulticastNotifyCallback, &a), BOF_ERR_NO_ERROR);
  ASSERT_EQ(MulticastAsyncNotifier.Register(&MulticastNotifyCallback, &b), BOF_ERR_NO_ERROR);
  ASSERT_EQ(MulticastAsyncNotifier.Register(&MulticastNotifyCallback, &b), BOF_ERR_NO_ERROR);

  StartInMs_U32 = BOF::Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < 100000; i_U32++)
  {
    MulticastNotifyArg_X.Cpt_U32 = i_U32;
    MulticastNotifyArg_X.NotifyEventMask_U32 = i_U32 * i_U32;
    ASSERT_EQ(MulticastAsyncNotifier.Notify(&MulticastNotifyArg_X), BOF_ERR_NO_ERROR);
  }

  ASSERT_EQ(MulticastAsyncNotifier.Unregister(&MulticastNotifyCallback), BOF_ERR_NO_ERROR);

  // printf("%d notif in %d ms %d still pending, wait for the end\n", i_U32, BOF::Bof_ElapsedMsTime(StartInMs_U32), MulticastAsyncNotifier.NbPendingNotification());
  ASSERT_EQ(MulticastAsyncNotifier.WaitForNoMoreNotificationPending(10, 5000), BOF_ERR_NO_ERROR);
  // printf("Terminated in %d ms %d still pending\n", BOF::Bof_ElapsedMsTime(StartInMs_U32), MulticastAsyncNotifier.NbPendingNotification());
}

TEST(Async_Test, SyncNotifier)
{
  BOF::BOF_MULTICAST_SYNC_NOTIFIER_PARAM MulticastSyncNotifierParam_X;
  uint32_t StartInMs_U32, i_U32;
  NOTIFY_ARG MulticastNotifyArg_X;
  BOF::BofMulticastSyncNotifier<NOTIFY_ARG> MulticastSyncNotifier(MulticastSyncNotifierParam_X);

  ASSERT_EQ(MulticastSyncNotifier.Register(&MulticastNotifyCallback, &a), BOF_ERR_NO_ERROR);
  ASSERT_EQ(MulticastSyncNotifier.Register(&MulticastNotifyCallback, &b), BOF_ERR_NO_ERROR);
  ASSERT_EQ(MulticastSyncNotifier.Register(&MulticastNotifyCallback, &b), BOF_ERR_NO_ERROR);

  StartInMs_U32 = BOF::Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < 100000; i_U32++)
  {
    MulticastNotifyArg_X.Cpt_U32 = i_U32;
    MulticastNotifyArg_X.NotifyEventMask_U32 = i_U32 * i_U32;
    ASSERT_EQ(MulticastSyncNotifier.Notify(&MulticastNotifyArg_X), BOF_ERR_NO_ERROR);
  }

  ASSERT_EQ(MulticastSyncNotifier.Unregister(&MulticastNotifyCallback), BOF_ERR_NO_ERROR);
  ASSERT_EQ(MulticastSyncNotifier.Notify(&MulticastNotifyArg_X), BOF_ERR_NO_ERROR);
  ASSERT_EQ(MulticastSyncNotifier.Unregister(&MulticastNotifyCallback), BOF_ERR_NO_ERROR);
  ASSERT_EQ(MulticastSyncNotifier.Notify(&MulticastNotifyArg_X), BOF_ERR_NO_ERROR);
  ASSERT_EQ(MulticastSyncNotifier.Unregister(&MulticastNotifyCallback), BOF_ERR_NO_ERROR);
  ASSERT_NE(MulticastSyncNotifier.Notify(&MulticastNotifyArg_X), BOF_ERR_NO_ERROR);
  ASSERT_NE(MulticastSyncNotifier.Notify(&MulticastNotifyArg_X), BOF_ERR_NO_ERROR);
}