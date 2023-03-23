/*!
  Copyright (c) 2008-2020, Evs. All rights reserved.

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Name:        ut_binserializer.cpp
  Author:      b.harmel@gmail.com
  Revision:    1.0

  Rem:         Based on google test

  History:

  V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
*/
#include "gtestrunner.h"
#include <bofstd/bofbinserializer.h>

#define MAX_ELEMENTS (50)

// To use a test fixture, derive from testing::Test class
class SerializerTest : public testing::Test
{
protected:
  /*!
  Summary
    The test initialization method
  */
  virtual void SetUp()
  {
    mpBigEndianSerializer_O = new BOF::BofBinSerializer(BOF::BOF_STREAM_IS_BIG_ENDIAN);
    mpLittleEndianSerializer_OmpBigEndianSerializer_O = new BOF::BofBinSerializer(BOF::BOF_STREAM_IS_LITTLE_ENDIAN);
  }

  /*!
  Summary
    The test cleanup method
  */
  virtual void TearDown()
  {
    BOF_SAFE_DELETE(mpBigEndianSerializer_O);
    BOF_SAFE_DELETE(mpLittleEndianSerializer_OmpBigEndianSerializer_O);
  }

  BOF::BofBinSerializer *mpBigEndianSerializer_O;
  BOF::BofBinSerializer *mpLittleEndianSerializer_OmpBigEndianSerializer_O;
};

/*!
Summary
  This is the unit test of the CList constructor
*/
TEST_F(SerializerTest, Constructor)
{
  EXPECT_TRUE(mpBigEndianSerializer_O->GetStreamEndianness() == BOF::BOF_STREAM_IS_BIG_ENDIAN);
  EXPECT_TRUE(mpBigEndianSerializer_O->IsStreamBigEndian() == true);
  EXPECT_TRUE(mpBigEndianSerializer_O->IsStreamLittleEndian() == false);

  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->GetStreamEndianness() == BOF::BOF_STREAM_IS_LITTLE_ENDIAN);
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->IsStreamBigEndian() == false);
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->IsStreamLittleEndian() == true);
}

TEST_F(SerializerTest, Endianness)
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

/*!
Summary
  This is the unit test of the BOF::BofBinSerializer "ToByte" method
*/
TEST_F(SerializerTest, Serializer_ToByte)
{
  uint8_t Value_U8 = 0x12;
  int8_t Value_S8 = -13;
  uint16_t Value_U16 = 0xAAAA;
  int16_t Value_S16 = -546;
  uint32_t Value_U32 = 0xABCD1234;
  int32_t Value_S32 = 0xF12DA6E9;
  uint64_t Value_U64 = 0x0123456789ABCDEF;
  int64_t Value_S64 = 0xFEDCBA9876543210;
  char Value_c = 'c';
  float Value_f = -13.42f;
  bool true_B = true;
  bool false_B = false;
  uint32_t Idx_U32 = 0;
  uint32_t Size_U32 = 0;
  uint8_t pStream_U8[100];

  // Initialize the stream
  Size_U32 = sizeof(pStream_U8);
  Idx_U32 = 0;
  memset(pStream_U8, 0xBB, Size_U32);

  // Serialize the data in Big Endian format
  EXPECT_TRUE(mpBigEndianSerializer_O->ToByte(Value_U8, Idx_U32, pStream_U8, Size_U32) == sizeof(uint8_t));
  Idx_U32 += sizeof(uint8_t);
  EXPECT_TRUE(mpBigEndianSerializer_O->ToByte(Value_S8, Idx_U32, pStream_U8, Size_U32) == sizeof(int8_t));
  Idx_U32 += sizeof(int8_t);

  EXPECT_TRUE(mpBigEndianSerializer_O->ToByte(Value_U16, Idx_U32, pStream_U8, Size_U32) == sizeof(uint16_t));
  Idx_U32 += sizeof(uint16_t);
  EXPECT_TRUE(mpBigEndianSerializer_O->ToByte(Value_S16, Idx_U32, pStream_U8, Size_U32) == sizeof(int16_t));
  Idx_U32 += sizeof(int16_t);

  EXPECT_TRUE(mpBigEndianSerializer_O->ToByte(Value_U32, Idx_U32, pStream_U8, Size_U32) == sizeof(uint32_t));
  Idx_U32 += sizeof(uint32_t);
  EXPECT_TRUE(mpBigEndianSerializer_O->ToByte(Value_S32, Idx_U32, pStream_U8, Size_U32) == sizeof(int32_t));
  Idx_U32 += sizeof(int32_t);

  EXPECT_TRUE(mpBigEndianSerializer_O->ToByte(Value_U64, Idx_U32, pStream_U8, Size_U32) == sizeof(uint64_t));
  Idx_U32 += sizeof(uint64_t);
  EXPECT_TRUE(mpBigEndianSerializer_O->ToByte(Value_S64, Idx_U32, pStream_U8, Size_U32) == sizeof(int64_t));
  Idx_U32 += sizeof(int64_t);

  EXPECT_TRUE(mpBigEndianSerializer_O->ToByte(true_B, Idx_U32, pStream_U8, Size_U32) == sizeof(uint8_t));
  Idx_U32 += sizeof(uint8_t);
  EXPECT_TRUE(mpBigEndianSerializer_O->ToByte(false_B, Idx_U32, pStream_U8, Size_U32) == sizeof(uint8_t));
  Idx_U32 += sizeof(uint8_t);

  EXPECT_TRUE(mpBigEndianSerializer_O->ToByte(Value_c, Idx_U32, pStream_U8, Size_U32) == sizeof(int8_t));
  Idx_U32 += sizeof(int8_t);
  EXPECT_TRUE(mpBigEndianSerializer_O->ToByte(Value_f, Idx_U32, pStream_U8, Size_U32) == sizeof(uint32_t));
  Idx_U32 += sizeof(uint32_t);

  // Check the stream
  EXPECT_TRUE(*(uint8_t *)&pStream_U8[0] == Value_U8);
  EXPECT_TRUE(*(int8_t *)&pStream_U8[1] == Value_S8);

  EXPECT_TRUE((uint16_t)BOF_BE_TO_CPU_16(*(uint16_t *)&pStream_U8[2]) == Value_U16);
  EXPECT_TRUE((int16_t)BOF_BE_TO_CPU_16(*(int16_t *)&pStream_U8[4]) == Value_S16);

  EXPECT_TRUE((uint32_t)BOF_BE_TO_CPU_32(*(uint32_t *)&pStream_U8[6]) == Value_U32);
  EXPECT_TRUE((int32_t)BOF_BE_TO_CPU_32(*(int32_t *)&pStream_U8[10]) == Value_S32);

  EXPECT_TRUE((uint64_t)BOF_BE_TO_CPU_64(*(uint64_t *)&pStream_U8[14]) == Value_U64);
  EXPECT_TRUE((int64_t)BOF_BE_TO_CPU_64(*(int64_t *)&pStream_U8[22]) == Value_S64);

  EXPECT_TRUE((*(uint8_t *)&pStream_U8[30]) == 1);
  EXPECT_TRUE((*(uint8_t *)&pStream_U8[31]) == 0);

  EXPECT_TRUE((char)pStream_U8[32] == Value_c);
  EXPECT_TRUE(BOF_BE_TO_CPU_32(*(uint32_t *)&pStream_U8[33]) == 0xC156B852);

  // Initialize the stream
  Size_U32 = sizeof(pStream_U8);
  Idx_U32 = 0;
  memset(pStream_U8, 0xBB, Size_U32);

  // Serialize the data in Little Endian format
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->ToByte(Value_U8, Idx_U32, pStream_U8, Size_U32) == sizeof(uint8_t));
  Idx_U32 += sizeof(uint8_t);
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->ToByte(Value_S8, Idx_U32, pStream_U8, Size_U32) == sizeof(int8_t));
  Idx_U32 += sizeof(int8_t);

  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->ToByte(Value_U16, Idx_U32, pStream_U8, Size_U32) == sizeof(uint16_t));
  Idx_U32 += sizeof(uint16_t);
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->ToByte(Value_S16, Idx_U32, pStream_U8, Size_U32) == sizeof(int16_t));
  Idx_U32 += sizeof(int16_t);

  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->ToByte(Value_U32, Idx_U32, pStream_U8, Size_U32) == sizeof(uint32_t));
  Idx_U32 += sizeof(uint32_t);
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->ToByte(Value_S32, Idx_U32, pStream_U8, Size_U32) == sizeof(int32_t));
  Idx_U32 += sizeof(int32_t);

  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->ToByte(Value_U64, Idx_U32, pStream_U8, Size_U32) == sizeof(uint64_t));
  Idx_U32 += sizeof(uint64_t);
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->ToByte(Value_S64, Idx_U32, pStream_U8, Size_U32) == sizeof(int64_t));
  Idx_U32 += sizeof(int64_t);

  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->ToByte(true_B, Idx_U32, pStream_U8, Size_U32) == sizeof(uint8_t));
  Idx_U32 += sizeof(uint8_t);
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->ToByte(false_B, Idx_U32, pStream_U8, Size_U32) == sizeof(uint8_t));
  Idx_U32 += sizeof(uint8_t);

  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->ToByte(Value_c, Idx_U32, pStream_U8, Size_U32) == sizeof(int8_t));
  Idx_U32 += sizeof(int8_t);
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->ToByte(Value_f, Idx_U32, pStream_U8, Size_U32) == sizeof(uint32_t));
  Idx_U32 += sizeof(uint32_t);

  // Check the stream
  EXPECT_TRUE(*(uint8_t *)&pStream_U8[0] == Value_U8);
  EXPECT_TRUE(*(int8_t *)&pStream_U8[1] == Value_S8);

  EXPECT_TRUE((uint16_t)BOF_LE_TO_CPU_16(*(uint16_t *)&pStream_U8[2]) == Value_U16);
  EXPECT_TRUE((int16_t)BOF_LE_TO_CPU_16(*(int16_t *)&pStream_U8[4]) == Value_S16);

  EXPECT_TRUE((uint32_t)BOF_LE_TO_CPU_32(*(uint32_t *)&pStream_U8[6]) == Value_U32);
  EXPECT_TRUE((int32_t)BOF_LE_TO_CPU_32(*(int32_t *)&pStream_U8[10]) == Value_S32);

  EXPECT_TRUE((uint64_t)BOF_LE_TO_CPU_64(*(uint64_t *)&pStream_U8[14]) == Value_U64);
  EXPECT_TRUE((int64_t)BOF_LE_TO_CPU_64(*(int64_t *)&pStream_U8[22]) == Value_S64);

  EXPECT_TRUE((*(uint8_t *)&pStream_U8[30]) == 1);
  EXPECT_TRUE((*(uint8_t *)&pStream_U8[31]) == 0);

  EXPECT_TRUE((char)pStream_U8[32] == Value_c);
  EXPECT_TRUE(BOF_LE_TO_CPU_32(*(uint32_t *)&pStream_U8[33]) == 0xC156B852);
}

/*!
Summary
  This is the unit test of the BOF::BofBinSerializer "ToByte" method
*/
TEST_F(SerializerTest, Serializer_FromByte)
{
  uint8_t Value_U8 = 0;
  int8_t Value_S8 = 0;
  uint16_t Value_U16 = 0;
  int16_t Value_S16 = 0;
  uint32_t Value_U32 = 0;
  int32_t Value_S32 = 0;
  uint64_t Value_U64 = 0;
  int64_t Value_S64 = 0;
  char Value_c = 0;
  float Value_f = 0;
  bool true_B = true;
  bool false_B = false;
  uint32_t Idx_U32 = 0;
  uint32_t Size_U32 = 0;
  uint32_t I_U32 = 0;
  uint8_t pStream_U8[100];

  // Initialize the stream
  Size_U32 = sizeof(pStream_U8);
  Idx_U32 = 0;

  for (I_U32 = 0; I_U32 < Size_U32; I_U32++)
  {
    pStream_U8[I_U32] = (uint8_t)I_U32;
  }

  // Force bool value to false and float value
  *(uint8_t *)&pStream_U8[30] = 1;
  *(uint8_t *)&pStream_U8[31] = 0;
  *(uint32_t *)&pStream_U8[33] = BOF_CPU_TO_BE_32(0xC156B852); // = -13.42f

  // Serialize the data in Big Endian format
  EXPECT_TRUE(mpBigEndianSerializer_O->FromByte(&Value_U8, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(uint8_t));
  Idx_U32 += sizeof(uint8_t);
  EXPECT_TRUE(mpBigEndianSerializer_O->FromByte(&Value_S8, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(int8_t));
  Idx_U32 += sizeof(int8_t);

  EXPECT_TRUE(mpBigEndianSerializer_O->FromByte(&Value_U16, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(uint16_t));
  Idx_U32 += sizeof(uint16_t);
  EXPECT_TRUE(mpBigEndianSerializer_O->FromByte(&Value_S16, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(int16_t));
  Idx_U32 += sizeof(int16_t);

  EXPECT_TRUE(mpBigEndianSerializer_O->FromByte(&Value_U32, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(uint32_t));
  Idx_U32 += sizeof(uint32_t);
  EXPECT_TRUE(mpBigEndianSerializer_O->FromByte(&Value_S32, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(int32_t));
  Idx_U32 += sizeof(int32_t);

  EXPECT_TRUE(mpBigEndianSerializer_O->FromByte(&Value_U64, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(uint64_t));
  Idx_U32 += sizeof(uint64_t);
  EXPECT_TRUE(mpBigEndianSerializer_O->FromByte(&Value_S64, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(int64_t));
  Idx_U32 += sizeof(int64_t);

  EXPECT_TRUE(mpBigEndianSerializer_O->FromByte(&true_B, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(uint8_t));
  Idx_U32 += sizeof(uint8_t);
  EXPECT_TRUE(mpBigEndianSerializer_O->FromByte(&false_B, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(uint8_t));
  Idx_U32 += sizeof(uint8_t);

  EXPECT_TRUE(mpBigEndianSerializer_O->FromByte(&Value_c, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(int8_t));
  Idx_U32 += sizeof(int8_t);
  EXPECT_TRUE(mpBigEndianSerializer_O->FromByte(&Value_f, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(uint32_t));
  Idx_U32 += sizeof(uint32_t);

  // Check the stream
  EXPECT_TRUE(*(uint8_t *)&pStream_U8[0] == Value_U8);
  EXPECT_TRUE(*(int8_t *)&pStream_U8[1] == Value_S8);

  EXPECT_TRUE((uint16_t)BOF_BE_TO_CPU_16(*(uint16_t *)&pStream_U8[2]) == Value_U16);
  EXPECT_TRUE((int16_t)BOF_BE_TO_CPU_16(*(int16_t *)&pStream_U8[4]) == Value_S16);

  EXPECT_TRUE((uint32_t)BOF_BE_TO_CPU_32(*(uint32_t *)&pStream_U8[6]) == Value_U32);
  EXPECT_TRUE((int32_t)BOF_BE_TO_CPU_32(*(int32_t *)&pStream_U8[10]) == Value_S32);

  EXPECT_TRUE((uint64_t)BOF_BE_TO_CPU_64(*(uint64_t *)&pStream_U8[14]) == Value_U64);
  EXPECT_TRUE((int64_t)BOF_BE_TO_CPU_64(*(int64_t *)&pStream_U8[22]) == Value_S64);

  EXPECT_TRUE(true_B == true);
  EXPECT_TRUE(false_B == false);

  EXPECT_TRUE((char)pStream_U8[32] == Value_c);
  EXPECT_TRUE(-13.42f == Value_f);

  // Initialize the stream
  Size_U32 = sizeof(pStream_U8);
  Idx_U32 = 0;

  for (I_U32 = 0; I_U32 < Size_U32; I_U32++)
  {
    pStream_U8[I_U32] = (uint8_t)I_U32;
  }

  // Force bool value to false and float value
  *(uint8_t *)&pStream_U8[30] = 1;
  *(uint8_t *)&pStream_U8[31] = 0;
  *(uint32_t *)&pStream_U8[33] = BOF_CPU_TO_LE_32(0xC156B852); // = -13.42f

  // Serialize the data in Little Endian format
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->FromByte(&Value_U8, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(uint8_t));
  Idx_U32 += sizeof(uint8_t);
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->FromByte(&Value_S8, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(int8_t));
  Idx_U32 += sizeof(int8_t);

  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->FromByte(&Value_U16, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(uint16_t));
  Idx_U32 += sizeof(uint16_t);
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->FromByte(&Value_S16, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(int16_t));
  Idx_U32 += sizeof(int16_t);

  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->FromByte(&Value_U32, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(uint32_t));
  Idx_U32 += sizeof(uint32_t);
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->FromByte(&Value_S32, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(int32_t));
  Idx_U32 += sizeof(int32_t);

  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->FromByte(&Value_U64, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(uint64_t));
  Idx_U32 += sizeof(uint64_t);
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->FromByte(&Value_S64, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(int64_t));
  Idx_U32 += sizeof(int64_t);

  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->FromByte(&true_B, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(uint8_t));
  Idx_U32 += sizeof(uint8_t);
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->FromByte(&false_B, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(uint8_t));
  Idx_U32 += sizeof(uint8_t);

  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->FromByte(&Value_c, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(int8_t));
  Idx_U32 += sizeof(int8_t);
  EXPECT_TRUE(mpLittleEndianSerializer_OmpBigEndianSerializer_O->FromByte(&Value_f, Idx_U32, pStream_U8, Size_U32, 0) == sizeof(uint32_t));
  Idx_U32 += sizeof(uint32_t);

  // Check the stream
  EXPECT_TRUE(*(uint8_t *)&pStream_U8[0] == Value_U8);
  EXPECT_TRUE(*(int8_t *)&pStream_U8[1] == Value_S8);

  EXPECT_TRUE((uint16_t)BOF_LE_TO_CPU_16(*(uint16_t *)&pStream_U8[2]) == Value_U16);
  EXPECT_TRUE((int16_t)BOF_LE_TO_CPU_16(*(int16_t *)&pStream_U8[4]) == Value_S16);

  EXPECT_TRUE((uint32_t)BOF_LE_TO_CPU_32(*(uint32_t *)&pStream_U8[6]) == Value_U32);
  EXPECT_TRUE((int32_t)BOF_LE_TO_CPU_32(*(int32_t *)&pStream_U8[10]) == Value_S32);

  EXPECT_TRUE((uint64_t)BOF_LE_TO_CPU_64(*(uint64_t *)&pStream_U8[14]) == Value_U64);
  EXPECT_TRUE((int64_t)BOF_LE_TO_CPU_64(*(int64_t *)&pStream_U8[22]) == Value_S64);

  EXPECT_TRUE(true_B == true);
  EXPECT_TRUE(false_B == false);

  EXPECT_TRUE((char)pStream_U8[32] == Value_c);
  EXPECT_TRUE(-13.42f == Value_f);
}
