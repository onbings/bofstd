/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofbit functions
 *
 * Name:        ut_bofbit.cpp
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
#include <bofstd/bofbit.h>

/*** Class *************************************************************************************************************************/

USE_BOF_NAMESPACE()

TEST(Bit_Test, BitSetPosition)
{
	uint32_t i_U32;
	uint64_t Mask_U64;
	EXPECT_EQ(Bof_MostSignificantBitSetPosition(0), 0xFF);

  EXPECT_EQ(Bof_MostSignificantBitSetPosition(0x100000000), 32);

  for (Mask_U64 = 1, i_U32 = 0; i_U32 < 64; i_U32++, Mask_U64 <<= 1)
	{
		EXPECT_EQ(Bof_MostSignificantBitSetPosition(Mask_U64), i_U32);
	}
	EXPECT_EQ(Bof_MostSignificantBitSetPosition(0x80000001), 31);
  EXPECT_EQ(Bof_MostSignificantBitSetPosition(0x00000003), 1);
  EXPECT_EQ(Bof_MostSignificantBitSetPosition(0x00000001), 0);
  EXPECT_EQ(Bof_MostSignificantBitSetPosition(0x100000000), 32);
  EXPECT_EQ(Bof_MostSignificantBitSetPosition(0x8000000000000000), 63);
  for (Mask_U64 = 1, i_U32 = 0; i_U32 < 64; i_U32++, Mask_U64 <<= 1)
  {
    EXPECT_EQ(Bof_LessSignificantBitSetPosition(Mask_U64), i_U32);
  }
}

TEST(Bit_Test, ErrorCode)
{
	EXPECT_STREQ(Bof_ErrorCode(BOF_ERR_NO_ERROR), "No error");
	EXPECT_STREQ(Bof_ErrorCode(static_cast<BOFERR>(236996)),"Unknown error (236996/0x39DC4)");
}