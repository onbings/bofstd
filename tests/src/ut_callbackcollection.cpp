/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the BofCallback class
 *
 * Name:        ut_callbackcollection.cpp
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
#include <bofstd/bofcallbackcollection.h>

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

struct MY_CALLBACK_ARG
{
  int a;
  float b;
  std::string c;
};
using MY_CALLBACK = std::function<BOFERR(const MY_CALLBACK_ARG &)>;
using MY_VOID_CALLBACK = std::function<void(const MY_CALLBACK_ARG &)>;
using MY_VOID2_CALLBACK = std::function<void(int)>;
using MY_INT_CALLBACK = std::function<int(int)>;

static int S_CallbackCounter_i = 0;
BOFERR MyCallback1(const MY_CALLBACK_ARG &_rArg_X)
{
  S_CallbackCounter_i++;
  //printf("MyCallback1 %d: a %d b %f c %s\n", S_CallbackCounter_i, _rArg_X.a, _rArg_X.b, _rArg_X.c.c_str());
  return (S_CallbackCounter_i > 10) ? BOF_ERR_CANCEL : BOF_ERR_NO_ERROR;
}
BOFERR MyCallback2(const MY_CALLBACK_ARG &_rArg_X)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  S_CallbackCounter_i++;
  //printf("MyCallback2 %d: a %d b %f c %s\n", S_CallbackCounter_i, _rArg_X.a, _rArg_X.b, _rArg_X.c.c_str());
  return Rts_E;
}
void MyCallback3(const MY_CALLBACK_ARG &_rArg_X)
{
  //printf("MyCallback3 %d: a %d b %f c %s\n", ++CallbackCounter_i, _rArg_X.a, _rArg_X.b, _rArg_X.c.c_str());
}
int MyCallback4(int _a_i)
{
  //printf("MyCallback4 %d: other a=%d\n", ++CallbackCounter_i, _a_i);
  return _a_i * 2;
}
void MyCallback5(int _a_i)
{
  //printf("MyCallback5 %d: other a=%d\n", ++CallbackCounter_i, _a_i);
}

TEST(CallbackCollection_Test, Void)
{
  uint32_t Id_U32;
  MY_CALLBACK_ARG Arg_X;

  // EXPECT_EQ(BofCallbackCollection<int>::S_Instance().Register(MyCallback5, Id_U32), BOF_ERR_NO_ERROR);

  //	BofCallbackCollection<MY_INT_CALLBACK>::S_Instance().CallIt(MyCallback4,3);

  EXPECT_EQ(BofCallbackCollection<MY_VOID2_CALLBACK>::S_Instance().Register(MyCallback5, Id_U32), BOF_ERR_NO_ERROR);
  // BofCallbackCollection<MY_VOID2_CALLBACK>::S_Instance().UnregisterIfFail(false);
  EXPECT_EQ(BofCallbackCollection<MY_VOID2_CALLBACK>::S_Instance().Call(Id_U32, 11), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofCallbackCollection<MY_VOID2_CALLBACK>::S_Instance().Unregister(Id_U32), BOF_ERR_NO_ERROR);

  EXPECT_EQ(BofCallbackCollection<MY_INT_CALLBACK>::S_Instance().Register(MyCallback4, Id_U32), BOF_ERR_NO_ERROR);
  BofCallbackCollection<MY_INT_CALLBACK>::S_Instance().UnregisterIfFail(true);
  EXPECT_NE(BofCallbackCollection<MY_INT_CALLBACK>::S_Instance().Call(Id_U32, 22), BOF_ERR_NO_ERROR); //->return 44-> unregistered
  EXPECT_NE(BofCallbackCollection<MY_INT_CALLBACK>::S_Instance().Unregister(Id_U32), BOF_ERR_NO_ERROR);

  EXPECT_EQ(BofCallbackCollection<MY_VOID_CALLBACK>::S_Instance().Register(MyCallback3, Id_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofCallbackCollection<MY_VOID_CALLBACK>::S_Instance().Call(Id_U32, Arg_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofCallbackCollection<MY_VOID_CALLBACK>::S_Instance().Unregister(Id_U32), BOF_ERR_NO_ERROR);

  EXPECT_EQ(BofCallbackCollection<MY_CALLBACK>::S_Instance().Register(MyCallback2, Id_U32), BOF_ERR_NO_ERROR);
  BofCallbackCollection<MY_CALLBACK>::S_Instance().UnregisterIfFail(true);
  EXPECT_EQ(BofCallbackCollection<MY_CALLBACK>::S_Instance().Call(Id_U32, Arg_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BofCallbackCollection<MY_CALLBACK>::S_Instance().Unregister(Id_U32), BOF_ERR_NO_ERROR);
}

TEST(CallbackCollection_Test, Constructor)
{
  uint32_t i_U32, pId_U32[16];
  BOFERR Sts_E;
  MY_CALLBACK_ARG Arg_X;

  BofCallbackCollection<MY_CALLBACK>::S_Instance().UnregisterIfFail(true);
  for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pId_U32); i_U32++)
  {
    if (i_U32 % 3)
    {
      EXPECT_EQ(BofCallbackCollection<MY_CALLBACK>::S_Instance().Register(MyCallback1, pId_U32[i_U32]), BOF_ERR_NO_ERROR);
    }
    else
    {
      EXPECT_EQ(BofCallbackCollection<MY_CALLBACK>::S_Instance().Register(MyCallback2, pId_U32[i_U32]), BOF_ERR_NO_ERROR);
    }
  }
  for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pId_U32); i_U32++)
  {
    Sts_E = BofCallbackCollection<MY_CALLBACK>::S_Instance().Call(pId_U32[i_U32], Arg_X);
    //printf("i=%d mod3=%d sts %d\n", i_U32, (i_U32 % 3), Sts_E);
    if (i_U32 % 3)
    {
      if (i_U32 >= 10)
      {
        EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
      }
      else
      {
        EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      }
    }
    else
    {
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    }
  }
  for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pId_U32); i_U32++)
  {
    BofCallbackCollection<MY_CALLBACK>::S_Instance().Unregister(pId_U32[i_U32]);
  }
  for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pId_U32); i_U32++)
  {
    if (i_U32 % 3)
    {
      EXPECT_EQ(BofCallbackCollection<MY_CALLBACK>::S_Instance().Register(MyCallback1, pId_U32[i_U32]), BOF_ERR_NO_ERROR);
    }
    else
    {
      EXPECT_EQ(BofCallbackCollection<MY_CALLBACK>::S_Instance().Register(MyCallback2, pId_U32[i_U32]), BOF_ERR_NO_ERROR);
    }
  }
  BofCallbackCollection<MY_CALLBACK>::S_Instance().UnregisterIfFail(false);
  for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pId_U32); i_U32++)
  {
    Arg_X.a++;
    Arg_X.b += 2.0f;
    Arg_X.c = "Hello_" + std::to_string(Arg_X.a);
    EXPECT_EQ(BofCallbackCollection<MY_CALLBACK>::S_Instance().Call(Arg_X), BOF_ERR_NO_ERROR);
  }
  for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pId_U32); i_U32++)
  {
    EXPECT_EQ(BofCallbackCollection<MY_CALLBACK>::S_Instance().Unregister(pId_U32[i_U32]), BOF_ERR_NO_ERROR);
  }
}
