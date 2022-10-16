/*
 * This module implement the unit tests of the system part of the Bof library
 *
 * Name:        ut_system.cpp
 * Author:      Bernard HARMEL
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  November 15 2016  BHA : Initial release
 */
#include <bofstd/bofsystem.h>
#include <bofstd/bofstringformatter.h>
#include <bofstd/boffs.h>

#include "gtestrunner.h"

#include <cstdint>

#if defined (_WIN32)
#include <windows.h>
#else
#include <errno.h>
#include <arpa/inet.h>
#endif

USE_BOF_NAMESPACE()

TEST(System_Test, TickDelta)
{
  uint32_t Timer_U32, Delta_U32;

  Timer_U32 = Bof_GetMsTickCount();

  do
  {
    Delta_U32 = Bof_ElapsedMsTime(Timer_U32);
  } while (Delta_U32 < 1000);
  EXPECT_LT(Delta_U32, (uint32_t)1050);
  EXPECT_GE(Delta_U32, (uint32_t)1000);
}

TEST(System_Test, EnvVar)
{
  const char *pEnv_c;
  char       pVal_c[128];
  int        Sts_i;

  pEnv_c = Bof_GetEnvVar(nullptr);
  EXPECT_TRUE(pEnv_c == 0);

  Sts_i = Bof_SetEnvVar(nullptr, pVal_c, 0);
  EXPECT_NE(Sts_i, 0);
  Sts_i = Bof_SetEnvVar("nullptr", nullptr, 0);
  EXPECT_NE(Sts_i, 0);
  Sts_i = Bof_SetEnvVar(nullptr, nullptr, 0);
  EXPECT_NE(Sts_i, 0);

  Sts_i = Bof_SetEnvVar("BHA_TEST_VAR", "1:BHA", 1);
  EXPECT_EQ(Sts_i, 0);
  Sts_i = Bof_SetEnvVar("BHA_TEST_VAR", "2:BHA", 0);
  EXPECT_NE(Sts_i, 0);
  Sts_i = Bof_SetEnvVar("BHA_TEST_VAR", "3:BHA", 1);
  EXPECT_EQ(Sts_i, 0);

  pEnv_c = Bof_GetEnvVar("BHA_TEST");
  EXPECT_TRUE(pEnv_c == 0);

  pEnv_c = Bof_GetEnvVar("BHA_TEST_VAR");
  EXPECT_TRUE(pEnv_c != 0);

  EXPECT_STREQ(pEnv_c, "3:BHA");

}



TEST(System_Test, DumpMemoryZone)
{
  std::string Dmp_S;
  uint8_t     pMemoryZone_U8[0x1000];
  std::string Out_S;
  uint32_t    i_U32;
  BOF_DUMP_MEMORY_ZONE_PARAM DumpMemoryZoneParam_X;

  for (i_U32 = 0; i_U32 < sizeof(pMemoryZone_U8); i_U32++)
  {
    pMemoryZone_U8[i_U32] = static_cast<uint8_t> (i_U32);
  }

  DumpMemoryZoneParam_X.NbItemToDump_U32 = 0;
  DumpMemoryZoneParam_X.pMemoryZone = pMemoryZone_U8;
  DumpMemoryZoneParam_X.NbItemPerLine_U32 = 16;
  DumpMemoryZoneParam_X.Separator_c = ' ';
  DumpMemoryZoneParam_X.ShowHexaPrefix_B = false;
  DumpMemoryZoneParam_X.GenerateVirtualOffset = true;
  DumpMemoryZoneParam_X.VirtualOffset_S64 = 0x12345678;
  DumpMemoryZoneParam_X.GenerateBinaryData_B = true;
  DumpMemoryZoneParam_X.GenerateAsciiData_B = true;
  DumpMemoryZoneParam_X.ReverseEndianness_B = false;
  DumpMemoryZoneParam_X.AccessSize_E = BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_8;
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  EXPECT_STREQ("", Dmp_S.c_str());

  DumpMemoryZoneParam_X.pMemoryZone = nullptr;
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  EXPECT_STREQ("", Dmp_S.c_str());

  DumpMemoryZoneParam_X.NbItemToDump_U32 = 1025;
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  EXPECT_STREQ("", Dmp_S.c_str());

  DumpMemoryZoneParam_X.Separator_c = 0;
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  EXPECT_STREQ("", Dmp_S.c_str());

  DumpMemoryZoneParam_X.NbItemToDump_U32 = 16;
  DumpMemoryZoneParam_X.pMemoryZone = pMemoryZone_U8;
  DumpMemoryZoneParam_X.Separator_c = ' ';
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  Out_S = Bof_Sprintf("12345678   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F ????????????????%s", Bof_Eol());
  EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str());

  DumpMemoryZoneParam_X.NbItemToDump_U32 = 64;
  DumpMemoryZoneParam_X.pMemoryZone = &pMemoryZone_U8[23];
  DumpMemoryZoneParam_X.VirtualOffset_S64 = 0xABCDEF01;
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  Out_S = Bof_Sprintf("ABCDEF01   17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 ????????? !\"#$%%&%sABCDEF11   27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 '()*+,-./0123456%sABCDEF21   37 38 39 3A 3B 3C 3D 3E 3F 40 41 42 43 44 45 46 789:;<=>?@ABCDEF%sABCDEF31   47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 GHIJKLMNOPQRSTUV%s", Bof_Eol(), Bof_Eol(), Bof_Eol(), Bof_Eol());
  EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str());

  DumpMemoryZoneParam_X.NbItemToDump_U32 = 18;
  DumpMemoryZoneParam_X.pMemoryZone = pMemoryZone_U8;
  DumpMemoryZoneParam_X.NbItemPerLine_U32 = 10;
  DumpMemoryZoneParam_X.VirtualOffset_S64 = 0x12345678;
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  Out_S = Bof_Sprintf("12345678   00 01 02 03 04 05 06 07 08 09 ??????????%s12345682   0A 0B 0C 0D 0E 0F 10 11       ????????  %s", Bof_Eol(), Bof_Eol());
  EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str());

  DumpMemoryZoneParam_X.Separator_c = ',';
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  Out_S = Bof_Sprintf("12345678,,,00,01,02,03,04,05,06,07,08,09,??????????%s12345682,,,0A,0B,0C,0D,0E,0F,10,11,      ????????  %s", Bof_Eol(), Bof_Eol());
  EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str());

  DumpMemoryZoneParam_X.ShowHexaPrefix_B = true;
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  Out_S = Bof_Sprintf("0x12345678,,,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,??????????%s0x12345682,,,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,          ????????  %s", Bof_Eol(), Bof_Eol());
  EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str());

  DumpMemoryZoneParam_X.ShowHexaPrefix_B = false;
  DumpMemoryZoneParam_X.GenerateVirtualOffset = false;
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  Out_S = Bof_Sprintf("00,01,02,03,04,05,06,07,08,09,??????????%s0A,0B,0C,0D,0E,0F,10,11,      ????????  %s", Bof_Eol(), Bof_Eol());
  EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str());

  DumpMemoryZoneParam_X.ShowHexaPrefix_B = true;
  DumpMemoryZoneParam_X.GenerateVirtualOffset = false;
  DumpMemoryZoneParam_X.GenerateAsciiData_B = false;
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  Out_S = Bof_Sprintf("0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,%s0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,          %s", Bof_Eol(), Bof_Eol());
  EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str());

  DumpMemoryZoneParam_X.pMemoryZone = &pMemoryZone_U8[64];
  DumpMemoryZoneParam_X.ShowHexaPrefix_B = false;
  DumpMemoryZoneParam_X.GenerateAsciiData_B = true;
  DumpMemoryZoneParam_X.GenerateBinaryData_B = false;
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  Out_S = Bof_Sprintf("@ABCDEFGHI%sJKLMNOPQ  %s", Bof_Eol(), Bof_Eol());
  EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str());

  DumpMemoryZoneParam_X.pMemoryZone = pMemoryZone_U8;
  DumpMemoryZoneParam_X.GenerateAsciiData_B = false;
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  EXPECT_STREQ("", Dmp_S.c_str());

  DumpMemoryZoneParam_X.NbItemToDump_U32 = 10;
  DumpMemoryZoneParam_X.pMemoryZone = pMemoryZone_U8;
  DumpMemoryZoneParam_X.NbItemPerLine_U32 = 8;
  DumpMemoryZoneParam_X.Separator_c = ' ';
  DumpMemoryZoneParam_X.ShowHexaPrefix_B = true;
  DumpMemoryZoneParam_X.GenerateVirtualOffset = true;
  DumpMemoryZoneParam_X.VirtualOffset_S64 = -1;
  DumpMemoryZoneParam_X.GenerateBinaryData_B = true;
  DumpMemoryZoneParam_X.GenerateAsciiData_B = true;
  DumpMemoryZoneParam_X.ReverseEndianness_B = true;
  DumpMemoryZoneParam_X.AccessSize_E = BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_16;
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  Out_S = Bof_Sprintf("0x%016llX   0x0001 0x0203 0x0405 0x0607 0x0809 0x0A0B 0x0C0D 0x0E0F ????????????????%s0x%016llX   0x1011 0x1213                                           ????            %s", reinterpret_cast<uint64_t>(pMemoryZone_U8), Bof_Eol(), reinterpret_cast<uint64_t>(pMemoryZone_U8 + 16), Bof_Eol());
  EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str());

  DumpMemoryZoneParam_X.NbItemToDump_U32 = 5;
  DumpMemoryZoneParam_X.NbItemPerLine_U32 = 4;
  DumpMemoryZoneParam_X.AccessSize_E = BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_32;
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  Out_S = Bof_Sprintf("0x%016llX   0x00010203 0x04050607 0x08090A0B 0x0C0D0E0F ????????????????%s0x%016llX   0x10111213                                  ????            %s", reinterpret_cast<uint64_t>(pMemoryZone_U8), Bof_Eol(), reinterpret_cast<uint64_t>(pMemoryZone_U8 + 16), Bof_Eol());
  EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str());

  DumpMemoryZoneParam_X.NbItemToDump_U32 = 2;
  DumpMemoryZoneParam_X.NbItemPerLine_U32 = 2;
  DumpMemoryZoneParam_X.AccessSize_E = BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_64;
  Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
  Out_S = Bof_Sprintf("0x%016llX   0x0001020304050607 0x08090A0B0C0D0E0F ????????????????%s", reinterpret_cast<uint64_t>(pMemoryZone_U8), Bof_Eol());
  EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str());
}

TEST(System_Test, Util_Sprintf)
{
  std::string Text_S;
  char        pFormat_c[512], pText_c[1024], pStr_c[32];
  int         i = 1;
  float       j = 3.14f;

  strcpy(pStr_c, "String");

  strcpy(pFormat_c, "Hello World");
  sprintf(pText_c, pFormat_c);
  Text_S = Bof_Sprintf(pFormat_c);
  EXPECT_STREQ(pText_c, Text_S.c_str());

  strcpy(pFormat_c, "int %d %04d 0x%08X float %f %03.4f str %8s %-8s");
  sprintf(pText_c, pFormat_c, i, i, i, j, j, pStr_c, pStr_c);
  Text_S = Bof_Sprintf(pFormat_c, i, i, i, j, j, pStr_c, pStr_c);
  EXPECT_STREQ(pText_c, Text_S.c_str());

}

TEST(System_Test, Util_AlignedAlloc)
{
  BOF_BUFFER Buffer_X;
  void *pData;

  EXPECT_EQ(Bof_AlignedMemAlloc(BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM, 32, 128, false, false, Buffer_X), BOF_ERR_NO_ERROR);
  pData = Buffer_X.pData_U8;
  EXPECT_TRUE(pData != nullptr);
  EXPECT_EQ((reinterpret_cast<std::uintptr_t> (pData) & 0x1F), 0);
  EXPECT_EQ(Bof_AlignedMemFree(Buffer_X), BOF_ERR_NO_ERROR);
  EXPECT_NE(Bof_AlignedMemFree(Buffer_X), BOF_ERR_NO_ERROR);

  EXPECT_NE(Bof_AlignedMemAlloc(BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM, 25, 128, false, false, Buffer_X), BOF_ERR_NO_ERROR);
  pData = Buffer_X.pData_U8;
  EXPECT_TRUE(pData == nullptr);
  EXPECT_NE(Bof_AlignedMemFree(Buffer_X), BOF_ERR_NO_ERROR);

  EXPECT_EQ(Bof_AlignedMemAlloc(BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM, 256, 128, false, true, Buffer_X), BOF_ERR_NO_ERROR);
  pData = Buffer_X.pData_U8;
  EXPECT_TRUE(pData != nullptr);
  EXPECT_EQ((reinterpret_cast<std::uintptr_t> (pData) & 0xFF), 0);
  EXPECT_EQ(Bof_AlignedMemFree(Buffer_X), BOF_ERR_NO_ERROR);

#if defined(_WIN32)
  //lock not supported
  EXPECT_EQ(Bof_AlignedMemAlloc(BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM, 0x1000, 0x100000, false, false, Buffer_X), BOF_ERR_NO_ERROR);
#else
  EXPECT_EQ(Bof_AlignedMemAlloc(BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM, 0x1000, 0x100000, true, false, Buffer_X), BOF_ERR_NO_ERROR);
#endif
  pData = Buffer_X.pData_U8;
  EXPECT_TRUE(pData != nullptr);
  EXPECT_EQ((reinterpret_cast<std::uintptr_t> (pData) & 0xFF), 0);
  EXPECT_EQ(Bof_AlignedMemFree(Buffer_X), BOF_ERR_NO_ERROR);
}

TEST(System_Test, NanoTickCount)
{
  uint64_t Start_U64, Delta_U64;

  Start_U64 = Bof_GetNsTickCount();
  Bof_MsSleep(BOF_S_TO_MS(1));
  Delta_U64 = Bof_ElapsedNsTime(Start_U64);
  EXPECT_GE(Delta_U64, BOF_S_TO_NANO(1));
  EXPECT_LT(Delta_U64, BOF_S_TO_NANO(1) * 1.1);
}

TEST(System_Test, MsTickCount)
{
  uint32_t Start_U32, Delta_U32;

  Start_U32 = Bof_GetMsTickCount();
  Bof_MsSleep(BOF_S_TO_MS(1));
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, BOF_S_TO_MS(1));
  EXPECT_LT(Delta_U32, BOF_S_TO_MS(1) * 1.1);
}

TEST(System_Test, Eol)
{
  const char *pEol_c = Bof_Eol();

#if defined (_WIN32)
  EXPECT_STREQ(pEol_c, "\r\n");
#else
  EXPECT_STREQ(pEol_c, "\n");
#endif
}

const uint32_t NBRAMDOMLOOP = 14000;
TEST(System_Test, Random)
{
  uint32_t    i_U32, pTime_U32[100], MeanLow_U32, MeanHigh_U32;
  int32_t     Val_S32, MinValue_S32, MaxValue_S32;

  std::string Str_S, Prv_S;

  memset(pTime_U32, 0, sizeof(pTime_U32));
  MinValue_S32 = 3;
  MaxValue_S32 = 12;
  Bof_Random(true, MinValue_S32, MaxValue_S32);
  for (i_U32 = 0; i_U32 < NBRAMDOMLOOP; i_U32++)
  {
    Val_S32 = Bof_Random(false, MinValue_S32, MaxValue_S32);
    EXPECT_GE(Val_S32, MinValue_S32);
    EXPECT_LE(Val_S32, MaxValue_S32);
    pTime_U32[Val_S32]++;
    // printf("%d: %d\r\n", i_U32, Val_U32);
  }
  MeanLow_U32 = static_cast<uint32_t> (static_cast<float>(NBRAMDOMLOOP / (MaxValue_S32 - MinValue_S32 + 1)) * 0.80f);
  MeanHigh_U32 = static_cast<uint32_t> (static_cast<float>(NBRAMDOMLOOP / (MaxValue_S32 - MinValue_S32 + 1)) * 1.20f);
  for (i_U32 = 0; i_U32 < static_cast<uint32_t> (MinValue_S32); i_U32++)
  {
    EXPECT_EQ(pTime_U32[i_U32], static_cast<uint32_t> (0));
  }

  for (i_U32 = MinValue_S32; i_U32 <= static_cast<uint32_t> (MaxValue_S32); i_U32++)
  {
    EXPECT_NE(pTime_U32[i_U32], static_cast<uint32_t> (0));
    EXPECT_GE(pTime_U32[i_U32], MeanLow_U32);
    EXPECT_LE(pTime_U32[i_U32], MeanHigh_U32);

    // printf("%d is present %d times\r\n", i_U32, pTime_U32[i_U32]);
  }
  for (i_U32 = MaxValue_S32 + 1; i_U32 < BOF_NB_ELEM_IN_ARRAY(pTime_U32); i_U32++)
  {
    EXPECT_EQ(pTime_U32[i_U32], static_cast<uint32_t> (0));
  }

  memset(pTime_U32, 0, sizeof(pTime_U32));
  MinValue_S32 = -89;
  MaxValue_S32 = -20;
  Bof_Random(true, MinValue_S32, MaxValue_S32);
  for (i_U32 = 0; i_U32 < NBRAMDOMLOOP; i_U32++)
  {
    Val_S32 = Bof_Random(false, MinValue_S32, MaxValue_S32);
    EXPECT_GE(Val_S32, MinValue_S32);
    EXPECT_LE(Val_S32, MaxValue_S32);
    pTime_U32[-Val_S32]++;
    // printf("%d: %d\r\n", i_U32, Val_U32);
  }
  MeanLow_U32 = static_cast<uint32_t> (static_cast<float>(NBRAMDOMLOOP / (MaxValue_S32 - MinValue_S32 + 1)) * 0.75f);
  MeanHigh_U32 = static_cast<uint32_t> (static_cast<float>(NBRAMDOMLOOP / (MaxValue_S32 - MinValue_S32 + 1)) * 1.25f);
  for (i_U32 = 0; i_U32 < static_cast<uint32_t> (-MaxValue_S32); i_U32++)
  {
    EXPECT_EQ(pTime_U32[i_U32], static_cast<uint32_t> (0));
  }

  for (i_U32 = -MaxValue_S32; i_U32 <= static_cast<uint32_t> (-MinValue_S32); i_U32++)
  {
    EXPECT_NE(pTime_U32[i_U32], static_cast<uint32_t> (0));
    EXPECT_GE(pTime_U32[i_U32], MeanLow_U32);
    EXPECT_LE(pTime_U32[i_U32], MeanHigh_U32);

    // printf("%d is present %d times\r\n", i_U32, pTime_U32[i_U32]);
  }
  for (i_U32 = -MinValue_S32 + 1; i_U32 < BOF_NB_ELEM_IN_ARRAY(pTime_U32); i_U32++)
  {
    EXPECT_EQ(pTime_U32[i_U32], static_cast<uint32_t> (0));
  }



  memset(pTime_U32, 0, sizeof(pTime_U32));
  MinValue_S32 = -19;
  MaxValue_S32 = 8;
  Bof_Random(true, MinValue_S32, MaxValue_S32);
  for (i_U32 = 0; i_U32 < NBRAMDOMLOOP; i_U32++)
  {
    Val_S32 = Bof_Random(false, MinValue_S32, MaxValue_S32);
    EXPECT_GE(Val_S32, MinValue_S32);
    EXPECT_LE(Val_S32, MaxValue_S32);
    pTime_U32[50 + Val_S32]++;
    // printf("%d: %d\r\n", i_U32, Val_U32);
  }
  MeanLow_U32 = static_cast<uint32_t> (static_cast<float>(NBRAMDOMLOOP / (MaxValue_S32 - MinValue_S32 + 1)) * 0.80f);
  MeanHigh_U32 = static_cast<uint32_t> (static_cast<float>(NBRAMDOMLOOP / (MaxValue_S32 - MinValue_S32 + 1)) * 1.20f);
  for (i_U32 = 0; i_U32 < static_cast<uint32_t> (50 + MinValue_S32); i_U32++)
  {
    EXPECT_EQ(pTime_U32[i_U32], static_cast<uint32_t> (0));
  }

  for (i_U32 = 50 + MinValue_S32; i_U32 <= static_cast<uint32_t> (50 + MaxValue_S32); i_U32++)
  {
    EXPECT_NE(pTime_U32[i_U32], static_cast<uint32_t> (0));
    EXPECT_GE(pTime_U32[i_U32], MeanLow_U32);
    EXPECT_LE(pTime_U32[i_U32], MeanHigh_U32);

    // printf("%d is present %d times\r\n", i_U32, pTime_U32[i_U32]);
  }
  for (i_U32 = 50 + MaxValue_S32 + 1; i_U32 < BOF_NB_ELEM_IN_ARRAY(pTime_U32); i_U32++)
  {
    EXPECT_EQ(pTime_U32[i_U32], static_cast<uint32_t> (0));
  }

  Prv_S = "";
  for (i_U32 = 0; i_U32 < 1000; i_U32++)
  {
    Str_S = Bof_Random(true, 40, 'A', 'Z');
    EXPECT_STRNE(Prv_S.c_str(), Str_S.c_str());
    Prv_S = Str_S;
    // printf("%d: %s\r\n", i_U32, Str_S.c_str());
  }
}

TEST(System_Test, RandomString)
{
  uint32_t    i_U32, j_U32, Size_U32;
  char        MinValue_c, MaxValue_c;
  std::string Val_S;

  MinValue_c = 'A';
  MaxValue_c = 'z';
  Size_U32 = 256;
  Bof_Random(true, MinValue_c, MaxValue_c);
  for (i_U32 = 0; i_U32 < 1000; i_U32++)
  {
    Val_S = Bof_Random(false, Size_U32, MinValue_c, MaxValue_c);
    EXPECT_EQ(Size_U32, Val_S.size());
    for (j_U32 = 0; j_U32 < Size_U32; j_U32++)
    {
      EXPECT_GE(Val_S[j_U32], MinValue_c);
      EXPECT_LE(Val_S[j_U32], MaxValue_c);
    }
  }
}

TEST(System_Test, Exec)
{
  BOFERR      Sts_E;
  std::string Cmd_S, Output_S;
  int32_t     ExitCode_S32;

#if defined (_WIN32)
  Cmd_S = Bof_Sprintf("dir");
#else
  Cmd_S = Bof_Sprintf("ls");
#endif
  Sts_E = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);
  //	std::cerr << "[          ] Exit code " << ExitCode_S32 << " Output:" << std::endl << Output_S << std::endl;
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(ExitCode_S32, 0);
  EXPECT_TRUE(Output_S != "");

  Output_S = "";
  Sts_E = Bof_Exec(Cmd_S, nullptr, ExitCode_S32);
  //	std::cerr << "[          ] Exit code " << ExitCode_S32 << " Output:" << std::endl << Output_S << std::endl;
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(ExitCode_S32, 0);
  EXPECT_TRUE(Output_S == "");
}