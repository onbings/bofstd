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
 * Name:        ut_enum.cpp
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
#include <bofstd/bofenum.h>

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

enum class MUSE_FILE_SYSTEM_MEDIA_TYPE :int32_t
{
  MUSE_FILE_SYSTEM_MEDIA_TYPE_UNKNOWN = 0,
  MUSE_FILE_SYSTEM_MEDIA_TYPE_STILL,
  MUSE_FILE_SYSTEM_MEDIA_TYPE_CLIP,
  MUSE_FILE_SYSTEM_MEDIA_TYPE_MAX
};
static BofEnum<MUSE_FILE_SYSTEM_MEDIA_TYPE> S_MuseFileSystemMediaTypeEnumConverter({
  { MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_UNKNOWN, "Unknown" },
  { MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_STILL, "Still" },
  { MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_CLIP, "Clip" },
  { MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_MAX, "Max" },
}, MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_UNKNOWN);

enum class MUSE_FILE_SYSTEM_MEDIA_TYPE_2 :int32_t
{
  MUSE_FILE_SYSTEM_MEDIA_TYPE_2_UNKNOWN = 0,
  MUSE_FILE_SYSTEM_MEDIA_TYPE_2_STILL,
  MUSE_FILE_SYSTEM_MEDIA_TYPE_2_CLIP,
  MUSE_FILE_SYSTEM_MEDIA_TYPE_2_MAX
};
static BofEnum<MUSE_FILE_SYSTEM_MEDIA_TYPE_2> S_MuseFileSystemMediaType_2_EnumConverter({
  { MUSE_FILE_SYSTEM_MEDIA_TYPE_2::MUSE_FILE_SYSTEM_MEDIA_TYPE_2_UNKNOWN, "Unknown2" },
  { MUSE_FILE_SYSTEM_MEDIA_TYPE_2::MUSE_FILE_SYSTEM_MEDIA_TYPE_2_STILL, "Still2" },
  { MUSE_FILE_SYSTEM_MEDIA_TYPE_2::MUSE_FILE_SYSTEM_MEDIA_TYPE_2_CLIP, "Clip2" },
  { MUSE_FILE_SYSTEM_MEDIA_TYPE_2::MUSE_FILE_SYSTEM_MEDIA_TYPE_2_MAX, "Max2" },
});


TEST(Enum_Test, WithDefaultValue)
{
  std::string Tp_S;
   
  Tp_S = S_MuseFileSystemMediaTypeEnumConverter.ToString(MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_UNKNOWN);
  EXPECT_STREQ(Tp_S.c_str(), "Unknown");
  EXPECT_EQ(S_MuseFileSystemMediaTypeEnumConverter.ToEnum(Tp_S), MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_UNKNOWN);
  Tp_S = S_MuseFileSystemMediaTypeEnumConverter.ToString(MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_STILL);
  EXPECT_STREQ(Tp_S.c_str(), "Still");
  EXPECT_EQ(S_MuseFileSystemMediaTypeEnumConverter.ToEnum(Tp_S), MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_STILL);
  Tp_S = S_MuseFileSystemMediaTypeEnumConverter.ToString(MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_CLIP);
  EXPECT_STREQ(Tp_S.c_str(), "Clip");
  EXPECT_EQ(S_MuseFileSystemMediaTypeEnumConverter.ToEnum(Tp_S), MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_CLIP);
  Tp_S = S_MuseFileSystemMediaTypeEnumConverter.ToString(MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_MAX);
  EXPECT_STREQ(Tp_S.c_str(), "Max");
  EXPECT_EQ(S_MuseFileSystemMediaTypeEnumConverter.ToEnum(Tp_S), MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_MAX);
  Tp_S = S_MuseFileSystemMediaTypeEnumConverter.ToString((MUSE_FILE_SYSTEM_MEDIA_TYPE)(999));
  EXPECT_STREQ(Tp_S.c_str(), "Unknown");
  EXPECT_EQ(S_MuseFileSystemMediaTypeEnumConverter.ToEnum(Tp_S), MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_UNKNOWN);
}

TEST(Enum_Test, WithoutDefaultValue)
{
  std::string Tp_S;
  bool ExceptionThrown_B = false;

  Tp_S = S_MuseFileSystemMediaType_2_EnumConverter.ToString(MUSE_FILE_SYSTEM_MEDIA_TYPE_2::MUSE_FILE_SYSTEM_MEDIA_TYPE_2_UNKNOWN);
  EXPECT_STREQ(Tp_S.c_str(), "Unknown2");
  EXPECT_EQ(S_MuseFileSystemMediaType_2_EnumConverter.ToEnum(Tp_S), MUSE_FILE_SYSTEM_MEDIA_TYPE_2::MUSE_FILE_SYSTEM_MEDIA_TYPE_2_UNKNOWN);
  Tp_S = S_MuseFileSystemMediaType_2_EnumConverter.ToString(MUSE_FILE_SYSTEM_MEDIA_TYPE_2::MUSE_FILE_SYSTEM_MEDIA_TYPE_2_STILL);
  EXPECT_STREQ(Tp_S.c_str(), "Still2");
  EXPECT_EQ(S_MuseFileSystemMediaType_2_EnumConverter.ToEnum(Tp_S), MUSE_FILE_SYSTEM_MEDIA_TYPE_2::MUSE_FILE_SYSTEM_MEDIA_TYPE_2_STILL);
  Tp_S = S_MuseFileSystemMediaType_2_EnumConverter.ToString(MUSE_FILE_SYSTEM_MEDIA_TYPE_2::MUSE_FILE_SYSTEM_MEDIA_TYPE_2_CLIP);
  EXPECT_STREQ(Tp_S.c_str(), "Clip2");
  EXPECT_EQ(S_MuseFileSystemMediaType_2_EnumConverter.ToEnum(Tp_S), MUSE_FILE_SYSTEM_MEDIA_TYPE_2::MUSE_FILE_SYSTEM_MEDIA_TYPE_2_CLIP);
  Tp_S = S_MuseFileSystemMediaType_2_EnumConverter.ToString(MUSE_FILE_SYSTEM_MEDIA_TYPE_2::MUSE_FILE_SYSTEM_MEDIA_TYPE_2_MAX);
  EXPECT_STREQ(Tp_S.c_str(), "Max2");
  EXPECT_EQ(S_MuseFileSystemMediaType_2_EnumConverter.ToEnum(Tp_S), MUSE_FILE_SYSTEM_MEDIA_TYPE_2::MUSE_FILE_SYSTEM_MEDIA_TYPE_2_MAX);
  try
  {
    Tp_S = S_MuseFileSystemMediaType_2_EnumConverter.ToString((MUSE_FILE_SYSTEM_MEDIA_TYPE_2)(999));
  }
  catch (const std::exception &rException)
  {
    ExceptionThrown_B = true;
    EXPECT_STREQ(rException.what(), "Unknown value");
  }
  EXPECT_TRUE(ExceptionThrown_B);
}