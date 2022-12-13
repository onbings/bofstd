/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofguid functions
 *
 * Name:        ut_guid.cpp
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
#include <bofstd/bofguid.h>

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

TEST(Guid_Test, BofGuid)
{
  BofGuid Guid1;
  BofGuid Guid2;
  EXPECT_FALSE(Guid1 == Guid2);
  EXPECT_TRUE(Guid1 != Guid2);
  EXPECT_FALSE(Guid1.IsNull());
  EXPECT_FALSE(Guid2.IsNull());
  Guid2 = Guid1;
  EXPECT_TRUE(Guid1 == Guid2);
  EXPECT_FALSE(Guid1 != Guid2);

  Guid2 = std::move(Guid1);
  EXPECT_FALSE(Guid1 == Guid2);
  EXPECT_TRUE(Guid1 != Guid2);

  const uint8_t pData_U8[] = { 0xA1,0xB2,0xC3,0xD4,0xE5,0xF6,0xA7,0xB8 ,0xC9,0xD0,0xE1,0xF2 ,0xA3,0xB4,0xC5,0xD6 };
  BofGuid Guid3(pData_U8);

  std::string GuidVal_S = Guid3.ToString(false);
  std::string GuidCannonicalVal_S = Guid3.ToString(true);

  EXPECT_STREQ(GuidVal_S.c_str(), "A1B2C3D4E5F6A7B8C9D0E1F2A3B4C5D6");
  EXPECT_STREQ(GuidCannonicalVal_S.c_str(), "a1b2c3d4-e5f6-a7b8-c9d0-e1f2a3b4c5d6");
  std::string GuidDecoratedCannonicalVal_S = "{" + GuidCannonicalVal_S + "}";

  BofGuid Guid4(GuidVal_S);
  BofGuid Guid5(GuidCannonicalVal_S);
  BofGuid Guid6(GuidDecoratedCannonicalVal_S);
  EXPECT_TRUE(Guid4 == Guid5);
  EXPECT_TRUE(Guid4 == Guid6);
  Guid4 = BofGuid(GuidVal_S);
  EXPECT_TRUE(Guid4 == Guid5);

  GuidVal_S = Guid4.ToString(false);
  GuidVal_S = Guid5.ToString(false);
  GuidVal_S = Guid6.ToString(false);

  const uint8_t pData2_U8[] = { 0xA1,0xB2,0xC3,0xD4,0xE5,0xF6,0xA7,0xB8 ,0xC9,0xD0,0xE1,0xF2 ,0xA3,0xB4,0xC5,0xD6 };
  BofGuid Guid10(pData2_U8);
  const uint8_t pData3_U8[] = { 0xA1,0xB2,0xC3,0xD4,0xE5,0xF6,0xA7,0xB8 ,0xC9,0xD0,0xE1,0xF2 ,0xA3,0xB4,0xC5,0xD7 };
  BofGuid Guid11(pData3_U8);
  const uint8_t pData4_U8[] = { 0xA0,0xB2,0xC3,0xD4,0xE5,0xF6,0xA7,0xB8 ,0xC9,0xD0,0xE1,0xF2 ,0xA3,0xB4,0xC5,0xD6 };
  BofGuid Guid12(pData4_U8);

  EXPECT_FALSE(Guid3 < Guid10);
  EXPECT_TRUE(Guid3 == Guid10);

  EXPECT_FALSE(Guid3 > Guid10);
  EXPECT_FALSE(Guid3 != Guid10);

  EXPECT_TRUE(Guid3 < Guid11);
  EXPECT_FALSE(Guid3 == Guid11);

  EXPECT_FALSE(Guid3 > Guid11);
  EXPECT_TRUE(Guid3 != Guid11);

  EXPECT_FALSE(Guid3 < Guid12);
  EXPECT_FALSE(Guid3 == Guid12);

  EXPECT_TRUE(Guid3 > Guid12);
  EXPECT_TRUE(Guid3 != Guid12);

  Guid3.Clear();
  EXPECT_TRUE(Guid3.IsNull());


}