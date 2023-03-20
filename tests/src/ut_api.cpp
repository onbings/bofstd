/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the api functions
 *
 * Name:        ut_api.cpp
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
#include <bofstd/bofstd.h>
#include "bofstd/bofbinserializer.h"
#include "gtestrunner.h"

TEST(Api_Test, Version)
{
  printf("Bof version: '%s'\n", BOF::Bof_GetVersion().c_str());
}
TEST(Api_Test, ErrorCode)
{
  EXPECT_STREQ(BOF::Bof_ErrorCode(BOF_ERR_NO_ERROR), "No error");
  EXPECT_STREQ(BOF::Bof_ErrorCode(static_cast<BOFERR>(236996)), "Unknown error (236996/0x39DC4)");
}

TEST(Api_Test, Exception)
{
  bool ExceptionThrown_B = false;
  BOFERR Err_E = BOF_ERR_ALREADY_OPENED;

  //Check exception type
  EXPECT_THROW(THROW_BOF_EXCEPTION("[BofException]", "File '/tmp/log' is already opened", Err_E), BOF::BofException);
  //Check that exception is thrown and the exception content
  try
  {
    THROW_BOF_EXCEPTION("[BofException]", "File '/tmp/log' is already opened", Err_E);
  }
  catch (const BOF::BofException &rException)
  {
    std::string Msg_S;

    ExceptionThrown_B = true;
    Msg_S = rException.what();
    //   printf("throw: '%s'\n", Msg_S.c_str());
        //[BofException]: 10001: Already opened >File '/tmp/log' is already opened< @ C:\pro\github\bofstd\tests\src\ut_api.cpp:48 (Api_Test_Exception_Test::TestBody)
    EXPECT_STREQ(Msg_S.substr(0, 76).c_str(), "[BofException]: 10001: Already opened >File '/tmp/log' is already opened< @ "); //The rest of the txt is os and file dependant
  }
  EXPECT_TRUE(ExceptionThrown_B);
}

TEST(Api_Test, Endianness)
{
  uint16_t Val_U16 = 0x1234, ValSwap_U16;
  uint32_t Val_U32 = 0x56789ABC, ValSwap_U32;
  uint64_t Val_U64 = 0xFEDCBA9876543210, ValSwap_U64;

  if (BOF::Bof_IsCpuLittleEndian())
  {
    ValSwap_U64 = BOF_CPU_TO_LE_64(Val_U64);
    EXPECT_EQ(Val_U64, 0xFEDCBA9876543210);
    EXPECT_EQ(ValSwap_U64, 0xFEDCBA9876543210);

    ValSwap_U32 = BOF_CPU_TO_LE_32(Val_U32);
    EXPECT_EQ(Val_U32, 0x56789ABC);
    EXPECT_EQ(ValSwap_U32, 0x56789ABC);

    ValSwap_U16 = BOF_CPU_TO_LE_16(Val_U16);
    EXPECT_EQ(Val_U16, 0x1234);
    EXPECT_EQ(ValSwap_U16, 0x1234);

    ValSwap_U64 = BOF_CPU_TO_BE_64(Val_U64);
    EXPECT_EQ(Val_U64, 0xFEDCBA9876543210);
    EXPECT_EQ(ValSwap_U64, 0x1032547698BADCFE);

    ValSwap_U32 = BOF_CPU_TO_BE_32(Val_U32);
    EXPECT_EQ(Val_U32, 0x56789ABC);
    EXPECT_EQ(ValSwap_U32, 0xBC9A7856);

    ValSwap_U16 = BOF_CPU_TO_BE_16(Val_U16);
    EXPECT_EQ(Val_U16, 0x1234);
    EXPECT_EQ(ValSwap_U16, 0x3412);

    ValSwap_U64 = BOF_LE_TO_CPU_64(Val_U64);
    EXPECT_EQ(Val_U64, 0xFEDCBA9876543210);
    EXPECT_EQ(ValSwap_U64, 0xFEDCBA9876543210);

    ValSwap_U32 = BOF_LE_TO_CPU_32(Val_U32);
    EXPECT_EQ(Val_U32, 0x56789ABC);
    EXPECT_EQ(ValSwap_U32, 0x56789ABC);

    ValSwap_U16 = BOF_LE_TO_CPU_16(Val_U16);
    EXPECT_EQ(Val_U16, 0x1234);
    EXPECT_EQ(ValSwap_U16, 0x1234);

    ValSwap_U64 = BOF_BE_TO_CPU_64(Val_U64);
    EXPECT_EQ(Val_U64, 0xFEDCBA9876543210);
    EXPECT_EQ(ValSwap_U64, 0x1032547698BADCFE);

    ValSwap_U32 = BOF_BE_TO_CPU_32(Val_U32);
    EXPECT_EQ(Val_U32, 0x56789ABC);
    EXPECT_EQ(ValSwap_U32, 0xBC9A7856);

    ValSwap_U16 = BOF_BE_TO_CPU_16(Val_U16);
    EXPECT_EQ(Val_U16, 0x1234);
    EXPECT_EQ(ValSwap_U16, 0x3412);
  }
  else
  {
    ValSwap_U64 = BOF_CPU_TO_BE_64(Val_U64);
    EXPECT_EQ(Val_U64, 0xFEDCBA9876543210);
    EXPECT_EQ(ValSwap_U64, 0xFEDCBA9876543210);

    ValSwap_U32 = BOF_CPU_TO_BE_32(Val_U32);
    EXPECT_EQ(Val_U32, 0x56789ABC);
    EXPECT_EQ(ValSwap_U32, 0x56789ABC);

    ValSwap_U16 = BOF_CPU_TO_BE_16(Val_U16);
    EXPECT_EQ(Val_U16, 0x1234);
    EXPECT_EQ(ValSwap_U16, 0x1234);

    ValSwap_U64 = BOF_CPU_TO_LE_64(Val_U64);
    EXPECT_EQ(Val_U64, 0xFEDCBA9876543210);
    EXPECT_EQ(ValSwap_U64, 0x1032547698BADCFE);

    ValSwap_U32 = BOF_CPU_TO_LE_32(Val_U32);
    EXPECT_EQ(Val_U32, 0x56789ABC);
    EXPECT_EQ(ValSwap_U32, 0xBC9A7856);

    ValSwap_U16 = BOF_CPU_TO_LE_16(Val_U16);
    EXPECT_EQ(Val_U16, 0x1234);
    EXPECT_EQ(ValSwap_U16, 0x3412);

    ValSwap_U64 = BOF_BE_TO_CPU_64(Val_U64);
    EXPECT_EQ(Val_U64, 0xFEDCBA9876543210);
    EXPECT_EQ(ValSwap_U64, 0xFEDCBA9876543210);

    ValSwap_U32 = BOF_BE_TO_CPU_32(Val_U32);
    EXPECT_EQ(Val_U32, 0x56789ABC);
    EXPECT_EQ(ValSwap_U32, 0x56789ABC);

    ValSwap_U16 = BOF_BE_TO_CPU_16(Val_U16);
    EXPECT_EQ(Val_U16, 0x1234);
    EXPECT_EQ(ValSwap_U16, 0x1234);

    ValSwap_U64 = BOF_LE_TO_CPU_64(Val_U64);
    EXPECT_EQ(Val_U64, 0xFEDCBA9876543210);
    EXPECT_EQ(ValSwap_U64, 0x1032547698BADCFE);

    ValSwap_U32 = BOF_LE_TO_CPU_32(Val_U32);
    EXPECT_EQ(Val_U32, 0x56789ABC);
    EXPECT_EQ(ValSwap_U32, 0xBC9A7856);

    ValSwap_U16 = BOF_LE_TO_CPU_16(Val_U16);
    EXPECT_EQ(Val_U16, 0x1234);
    EXPECT_EQ(ValSwap_U16, 0x3412);
  }
}