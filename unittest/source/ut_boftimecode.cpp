/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofenum class
 *
 * Name:        ut_boftimecode.cpp
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

/*** Include files ***********************************************************/

#include "gtestrunner.h"
#include <bofstd/boftimecode.h>

/*** Class *************************************************************************************************************************/

USE_BOF_NAMESPACE()

TEST(Timecode_Test, TcBin)
{
	uint64_t TcBin_U64;
	BOF_TIMECODE Tc1_X,Tc2_X;

	BOF_DATE_TIME DateTime1_X(26, 5, 1970, 8, 16, 32, 100);
	BofTimecode Tc1(false, DateTime1_X);

	Tc1_X=Tc1.ToByteStruct();
	Tc1_X.pUserBit_U8[0]=0x12;
	Tc1_X.pUserBit_U8[1]=0x34;
	Tc1_X.pUserBit_U8[2]=0x56;
	Tc1_X.pUserBit_U8[3]=0x78;
	EXPECT_EQ(BofTimecode::S_TimeCodeToBin(Tc1_X, TcBin_U64),BOF_ERR_NO_ERROR);
	EXPECT_EQ(BofTimecode::S_BinToTimeCode(TcBin_U64,Tc2_X),BOF_ERR_NO_ERROR);
	EXPECT_EQ(Tc1_X.Hour_U8,Tc2_X.Hour_U8);
	EXPECT_EQ(Tc1_X.Minute_U8,Tc2_X.Minute_U8);
	EXPECT_EQ(Tc1_X.Second_U8,Tc2_X.Second_U8);
	EXPECT_EQ(Tc1_X.Frame_U8,Tc2_X.Frame_U8);
	EXPECT_EQ(Tc1_X.TcFlag_U8,Tc2_X.TcFlag_U8);
	EXPECT_EQ(Tc1_X.pUserBit_U8[0],Tc2_X.pUserBit_U8[0]);
	EXPECT_EQ(Tc1_X.pUserBit_U8[1],Tc2_X.pUserBit_U8[1]);
	EXPECT_EQ(Tc1_X.pUserBit_U8[2],Tc2_X.pUserBit_U8[2]);
	EXPECT_EQ(Tc1_X.pUserBit_U8[3],Tc2_X.pUserBit_U8[3]);

	EXPECT_EQ(BofTimecode::S_IncBinTimeCode(TcBin_U64,53), BOF_ERR_NO_ERROR);
	EXPECT_EQ(BofTimecode::S_BinToTimeCode(TcBin_U64,Tc2_X),BOF_ERR_NO_ERROR);
	EXPECT_EQ(Tc2_X.Hour_U8,8);
	EXPECT_EQ(Tc2_X.Minute_U8,16);
	EXPECT_EQ(Tc2_X.Second_U8,33);
	EXPECT_EQ(Tc2_X.Frame_U8,4);
	EXPECT_EQ(Tc2_X.TcFlag_U8,0);
	EXPECT_EQ(Tc2_X.pUserBit_U8[0],Tc2_X.pUserBit_U8[0]);
	EXPECT_EQ(Tc2_X.pUserBit_U8[1],Tc2_X.pUserBit_U8[1]);
	EXPECT_EQ(Tc2_X.pUserBit_U8[2],Tc2_X.pUserBit_U8[2]);
	EXPECT_EQ(Tc2_X.pUserBit_U8[3],Tc2_X.pUserBit_U8[3]);
	EXPECT_EQ(BofTimecode::S_IncTimeCode(Tc2_X,55), BOF_ERR_NO_ERROR);
	EXPECT_EQ(Tc2_X.Hour_U8,8);
	EXPECT_EQ(Tc2_X.Minute_U8,16);
	EXPECT_EQ(Tc2_X.Second_U8,34);
	EXPECT_EQ(Tc2_X.Frame_U8,6);
	EXPECT_EQ(Tc2_X.TcFlag_U8,1);
	EXPECT_EQ(Tc2_X.pUserBit_U8[0],Tc2_X.pUserBit_U8[0]);
	EXPECT_EQ(Tc2_X.pUserBit_U8[1],Tc2_X.pUserBit_U8[1]);
	EXPECT_EQ(Tc2_X.pUserBit_U8[2],Tc2_X.pUserBit_U8[2]);
	EXPECT_EQ(Tc2_X.pUserBit_U8[3],Tc2_X.pUserBit_U8[3]);
}
TEST(Timecode_Test, Construct)
{
  uint64_t TcInMs_U64, TcValInMs_U64;
  int64_t DifMs_S64;
  BOF_DATE_TIME DateTime_X(26,5,2018,8,16,32,47);
  BofTimecode Tc1;

  EXPECT_FALSE(Tc1.IsNtsc());
  EXPECT_FALSE(Tc1.IsOddField());
  EXPECT_EQ(Tc1.FrameTime(), 1000.0f / 25.0f);
  EXPECT_EQ(Tc1.FieldTime(), 1000.0f / 50.0f);
  TcInMs_U64=Tc1.ToMs();
  EXPECT_EQ(TcInMs_U64, 0);
  Tc1.FromMs(Tc1.IsNtsc(), TcInMs_U64);
	EXPECT_STREQ(Tc1.ToString(true, "%Y-%b-%d",true,"%H:%M:%S", true).c_str(), "1970-Jan-01 00:00:00:00  @1000/25");
	EXPECT_STREQ(Tc1.ToString(true, "",true,"", true).c_str(), "1970-01-01 00:00:00:00  @1000/25");

  TcInMs_U64 = (static_cast<uint64_t>(367) * static_cast<uint64_t>(24 * 60 * 60 * 1000)) + static_cast<uint64_t>(1 * 60 * 60 * 1000) + static_cast<uint64_t>(2 * 60 * 1000) + static_cast<uint64_t>(3 * 1000) + static_cast<uint64_t>(16.6667f * 9);
  BofTimecode Tc2(true, TcInMs_U64);
  EXPECT_TRUE(Tc2.IsNtsc());
  EXPECT_TRUE(Tc2.IsOddField());
  EXPECT_EQ(Tc2.FrameTime(), 1000.0f / 30.0f);
  EXPECT_EQ(Tc2.FieldTime(), 1000.0f / 60.0f);
  TcValInMs_U64 = Tc2.ToMs();
  DifMs_S64 = TcInMs_U64 - TcValInMs_U64;
  if (DifMs_S64 < 0)
  {
    DifMs_S64 = -DifMs_S64;
  }
  EXPECT_LE(DifMs_S64, 2);
  Tc2.FromMs(Tc2.IsNtsc(), TcValInMs_U64);
	EXPECT_STREQ(Tc2.ToString(true, "%Y-%b-%d", true, "%H:%M:%S", true).c_str(), "1971-Jan-03 01:02:03:04. @1000/30");
	EXPECT_STREQ(Tc2.ToString(true, "", true, "", true).c_str(), "1971-01-03 01:02:03:04. @1000/30");

  BofTimecode Tc3(false, DateTime_X);
  EXPECT_FALSE(Tc3.IsNtsc());
  EXPECT_FALSE(Tc3.IsOddField());
  EXPECT_EQ(Tc3.FrameTime(), 1000.0f / 25.0f);
  EXPECT_EQ(Tc3.FieldTime(), 1000.0f / 50.0f);
  TcInMs_U64 = Tc3.ToMs();
  TcValInMs_U64 = (static_cast<uint64_t>(17677) * static_cast<uint64_t>(24 * 60 * 60 * 1000)) + static_cast<uint64_t>(8 * 60 * 60 * 1000) + static_cast<uint64_t>(16 * 60 * 1000) + static_cast<uint64_t>(32 * 1000) + static_cast<uint64_t>(40);
  EXPECT_EQ(TcInMs_U64, TcValInMs_U64);
  Tc3.FromMs(Tc3.IsNtsc(), TcValInMs_U64);
	EXPECT_STREQ(Tc3.ToString(true, "%Y-%b-%d", true, "%H:%M:%S", true).c_str(), "2018-May-26 08:16:32:01  @1000/25");
	EXPECT_STREQ(Tc3.ToString(true, "", true, "", true).c_str(), "2018-05-26 08:16:32:01  @1000/25");

	Tc1=BofTimecode("2018-05-26 08:16:32;01. @1000/30");
	EXPECT_TRUE(Tc1.IsNtsc());
	EXPECT_TRUE(Tc1.IsOddField());
	EXPECT_EQ(Tc1.FrameTime(), 1000.0f / 30.0f);

}
TEST(Timecode_Test, Operator)
{
  BOF_DATE_TIME DateTime1_X(26, 5, 1970, 8, 16, 32, 100);
  BofTimecode Tc1(false, DateTime1_X);
  BOF_DATE_TIME DateTime2_X(26, 5, 1970, 8, 16, 32, 900);
  BofTimecode Tc2(false, DateTime2_X);
  BOF_DATE_TIME DateTime3_X(2, 7, 2018, 0, 1, 2, 700);
  BofTimecode Tc3(false, DateTime3_X);
  BofTimecode Tc4(false, DateTime1_X);
  BOF_DATE_TIME DiffTime_X;
  uint32_t DiffDay_U32;
  uint64_t Diff_U64, DiffInField_U64;
  BOFERR Sts_E;
  BOF_TIMECODE Tc1_X;

  EXPECT_TRUE(Tc1 == Tc1);
  EXPECT_TRUE(Tc2 == Tc2);
  EXPECT_TRUE(Tc3 == Tc3);
  EXPECT_TRUE(Tc1 == Tc4);

  EXPECT_FALSE(Tc1 != Tc1);
  EXPECT_FALSE(Tc2 != Tc2);
  EXPECT_FALSE(Tc3 != Tc3);
  EXPECT_FALSE(Tc1 != Tc4);

  EXPECT_TRUE(Tc1 < Tc2);
  EXPECT_TRUE(Tc1 <= Tc2);
  EXPECT_TRUE(Tc2 < Tc3);
  EXPECT_TRUE(Tc2 <= Tc3);
  EXPECT_FALSE(Tc1 < Tc4);
  EXPECT_TRUE(Tc1 <= Tc4);

  EXPECT_TRUE(Tc2 > Tc1);
  EXPECT_TRUE(Tc2 >= Tc1);
  EXPECT_TRUE(Tc3 > Tc2);
  EXPECT_TRUE(Tc3 >= Tc2);
  EXPECT_FALSE(Tc4 > Tc1);
  EXPECT_TRUE(Tc4 >= Tc1);

  Diff_U64 = Tc2 - Tc1;
  EXPECT_EQ(Diff_U64, 40);
  Sts_E = Bof_DiffDateTime(DateTime3_X, DateTime2_X, DiffTime_X, DiffDay_U32);
  EXPECT_EQ(Sts_E, 0);
  DiffInField_U64 = static_cast<uint64_t>(((static_cast<uint64_t>(DiffDay_U32) * static_cast<uint64_t>(24 * 60 * 60 * 1000)) + static_cast<uint64_t>(DiffTime_X.Hour_U8 * 60 * 60 * 1000) + static_cast<uint64_t>(DiffTime_X.Minute_U8 * 60 * 1000) + static_cast<uint64_t>(DiffTime_X.Second_U8 * 1000) + static_cast<uint64_t>(DiffTime_X.Millisecond_U16)) / static_cast<uint64_t>(Tc2.FieldTime()));
  Diff_U64 = Tc3 - Tc2;
  EXPECT_EQ(Diff_U64, DiffInField_U64);

  Diff_U64 = Tc4 - Tc1;
  EXPECT_EQ(Diff_U64, 0);

  Tc1_X = Tc1.ToByteStruct();
  EXPECT_EQ(Tc1_X.NbDay_U16, 145);
  EXPECT_EQ(Tc1_X.Hour_U8, 8);
  EXPECT_EQ(Tc1_X.Minute_U8, 16);
  EXPECT_EQ(Tc1_X.Second_U8, 32);
  EXPECT_EQ(Tc1_X.Frame_U8, 2);
  EXPECT_EQ(Tc1_X.TcFlag_U8, 1);
  Tc1 = Tc1 + 1;
  Tc1_X = Tc1.ToByteStruct();
  EXPECT_EQ(Tc1_X.NbDay_U16, 145);
  EXPECT_EQ(Tc1_X.Hour_U8, 8);
  EXPECT_EQ(Tc1_X.Minute_U8, 16);
  EXPECT_EQ(Tc1_X.Second_U8, 32);
  EXPECT_EQ(Tc1_X.Frame_U8, 3);
  EXPECT_EQ(Tc1_X.TcFlag_U8, 0);
  Tc1 = Tc1 + 1;
  Tc1_X = Tc1.ToByteStruct();
  EXPECT_EQ(Tc1_X.NbDay_U16, 145);
  EXPECT_EQ(Tc1_X.Hour_U8, 8);
  EXPECT_EQ(Tc1_X.Minute_U8, 16);
  EXPECT_EQ(Tc1_X.Second_U8, 32);
  EXPECT_EQ(Tc1_X.Frame_U8, 3);
  EXPECT_EQ(Tc1_X.TcFlag_U8, 1);
  Tc1 = Tc1 + 43;
  Tc1_X = Tc1.ToByteStruct();
  EXPECT_EQ(Tc1_X.NbDay_U16, 145);
  EXPECT_EQ(Tc1_X.Hour_U8, 8);
  EXPECT_EQ(Tc1_X.Minute_U8, 16);
  EXPECT_EQ(Tc1_X.Second_U8, 33);
  EXPECT_EQ(Tc1_X.Frame_U8, 0);
  EXPECT_EQ(Tc1_X.TcFlag_U8, 0);
}