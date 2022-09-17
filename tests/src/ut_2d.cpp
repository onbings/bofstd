/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bof2d class
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

#include "gtestrunner.h"
#include <bofstd/bof2d.h>
#include <bofstd/boffs.h>

USE_BOF_NAMESPACE()

//Depends on line terminator and pathname
//#define CHECK_STR
TEST(Bof2d_Test, MediaDetectorParse)
{
  BofMediaDetector MediaDetector;
  std::string Result_S;
  
  EXPECT_NE(MediaDetector.ParseFile("./data/dontexist.jpg", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);

  EXPECT_EQ(MediaDetector.ParseFile("./data/colorbar.jpg", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar.jpg\r\nFormat                                   : JPEG\r\nFile size                                : 9.60 KiB\r\n\r\nImage\r\nFormat                                   : JPEG\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : YUV\r\nChroma subsampling                       : 4:2:2\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossy\r\nStream size                              : 9.60 KiB (100%)\r\n\r\n");
#endif
  EXPECT_EQ(MediaDetector.ParseFile("./data/colorbar.jpg", BofMediaDetector::ResultFormat::Html, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "<html>\r\n\r\n<head>\r\n<META http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /></head>\r\n<body>\r\n<table width=\"100%\" border=\"0\" cellpadding=\"1\" cellspacing=\"2\" style=\"border:1px solid Navy\">\r\n<tr>\r\n    <td width=\"150\"><h2>General</h2></td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Complete name :</i></td>\r\n    <td colspan=\"3\">C:\\bld\\bofstd\\tests\\data\\colorbar.jpg</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Format :</i></td>\r\n    <td colspan=\"3\">JPEG</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>File size :</i></td>\r\n    <td colspan=\"3\">9.60 KiB</td>\r\n  </tr>\r\n</table>\r\n<br />\r\n<table width=\"100%\" border=\"0\" cellpadding=\"1\" cellspacing=\"2\" style=\"border:1px solid Navy\">\r\n<tr>\r\n    <td width=\"150\"><h2>Image</h2></td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Format :</i></td>\r\n    <td colspan=\"3\">JPEG</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Width :</i></td>\r\n    <td colspan=\"3\">259 pixels</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Height :</i></td>\r\n    <td colspan=\"3\">194 pixels</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Color space :</i></td>\r\n    <td colspan=\"3\">YUV</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Chroma subsampling :</i></td>\r\n    <td colspan=\"3\">4:2:2</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Bit depth :</i></td>\r\n    <td colspan=\"3\">8 bits</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Compression mode :</i></td>\r\n    <td colspan=\"3\">Lossy</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Stream size :</i></td>\r\n    <td colspan=\"3\">9.60 KiB (100%)</td>\r\n  </tr>\r\n</table>\r\n<br />\r\n\r\n</body>\r\n</html>\r\n");
#endif
  EXPECT_EQ(MediaDetector.ParseFile("./data/colorbar.jpg", BofMediaDetector::ResultFormat::Json, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "{\r\n\"media\": {\r\n\"@ref\": \"C:\\\\bld\\\\bofstd\\\\tests\\\\data\\\\colorbar.jpg\",\r\n\"track\": [\r\n{\r\n\"@type\": \"General\",\r\n\"ImageCount\": \"1\",\r\n\"FileExtension\": \"jpg\",\r\n\"Format\": \"JPEG\",\r\n\"FileSize\": \"9830\",\r\n\"StreamSize\": \"0\",\r\n\"File_Created_Date\": \"UTC 2022-09-17 08:12:46.389\",\r\n\"File_Created_Date_Local\": \"2022-09-17 10:12:46.389\",\r\n\"File_Modified_Date\": \"UTC 2022-09-17 11:29:29.185\",\r\n\"File_Modified_Date_Local\": \"2022-09-17 13:29:29.185\"\r\n},\r\n{\r\n\"@type\": \"Image\",\r\n\"Format\": \"JPEG\",\r\n\"Width\": \"259\",\r\n\"Height\": \"194\",\r\n\"ColorSpace\": \"YUV\",\r\n\"ChromaSubsampling\": \"4:2:2\",\r\n\"BitDepth\": \"8\",\r\n\"Compression_Mode\": \"Lossy\",\r\n\"StreamSize\": \"9830\"\r\n}\r\n]\r\n}\r\n}\r\n");
#endif

  EXPECT_EQ(MediaDetector.ParseFile("./data/colorbar_jpg_with_bad_ext.png",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_jpg_with_bad_ext.png\r\nFormat                                   : JPEG\r\nFile size                                : 9.63 KiB\r\nFileExtension_Invalid                    : h3d jpeg jpg jpe jps mpo\r\n\r\nImage\r\nFormat                                   : JPEG\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : YUV\r\nChroma subsampling                       : 4:2:2\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossy\r\nStream size                              : 9.63 KiB (100%)\r\n\r\n");
#endif
  EXPECT_EQ(MediaDetector.ParseFile("./data/colorbar_jpg_with_no_ext",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_jpg_with_no_ext\r\nFormat                                   : JPEG\r\nFile size                                : 9.69 KiB\r\nFileExtension_Invalid                    : h3d jpeg jpg jpe jps mpo\r\n\r\nImage\r\nFormat                                   : JPEG\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : YUV\r\nChroma subsampling                       : 4:2:2\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossy\r\nStream size                              : 9.69 KiB (100%)\r\n\r\n");
#endif

  EXPECT_EQ(MediaDetector.ParseFile("./data/colorbar.png",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar.png\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFile size                                : 1.45 KiB\r\n\r\nImage\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFormat_Compression                       : Deflate\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossless\r\nStream size                              : 1.45 KiB (100%)\r\n\r\n");
#endif
  EXPECT_EQ(MediaDetector.ParseFile("./data/colorbar_png_with_bad_ext.jpg",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_png_with_bad_ext.jpg\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFile size                                : 1.45 KiB\r\nFileExtension_Invalid                    : png pns\r\n\r\nImage\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFormat_Compression                       : Deflate\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossless\r\nStream size                              : 1.45 KiB (100%)\r\n\r\n");
#endif
  EXPECT_EQ(MediaDetector.ParseFile("./data/colorbar_png_with_no_ext",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_png_with_no_ext\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFile size                                : 976 Bytes\r\nFileExtension_Invalid                    : png pns\r\n\r\nImage\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFormat_Compression                       : Deflate\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossless\r\nStream size                              : 976 Bytes (100%)\r\n\r\n");
#endif

  EXPECT_EQ(MediaDetector.ParseFile("./data/colorbar_uncompress_32b.tga",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_uncompress_32b.tga\r\nFormat                                   : TGA\r\nFormat version                           : Version 2\r\nFile size                                : 197 KiB\r\n\r\nImage\r\nFormat                                   : Raw\r\nCodec ID                                 : 2\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 32 bits\r\n\r\n");
#endif
  EXPECT_EQ(MediaDetector.ParseFile("./data/colorbar_rle_32b.tga",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_rle_32b.tga\r\nFormat                                   : TGA\r\nFormat version                           : Version 2\r\nFile size                                : 18.0 KiB\r\n\r\nImage\r\nFormat                                   : RLE\r\nFormat/Info                              : Run-length encoding\r\nCodec ID                                 : 10\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 32 bits\r\n\r\n");
#endif

  EXPECT_EQ(MediaDetector.ParseFile("./data/colorbar_in_raw_32b_194x259.raw",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_in_raw_32b_194x259.raw\r\nFile size                                : 197 KiB\r\n\r\n");
#endif
  EXPECT_EQ(MediaDetector.ParseFile("./data/colorbar_1920x1080p59_8bit_captured_by_sdk.422",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_1920x1080p59_8bit_captured_by_sdk.422\r\nFile size                                : 1.98 MiB\r\n\r\n");
#endif
}
BOFERR ParseFileBuffer(BofPath &_rPathname, std::string &_rResult_S)
{
  BofMediaDetector MediaDetector;
  std::string Result_S;
  BOFERR Rts_E;
  bool Finish_B;
  BOF::BOF_BUFFER BufferToDeleteAfterUsage_X, SubBuffer_X;
  uint64_t OffsetInBuffer_U64;
  uint32_t TotalByteNeeded_U32;

  EXPECT_EQ(Bof_ReadFile(_rPathname, BufferToDeleteAfterUsage_X), BOF_ERR_NO_ERROR);
  SubBuffer_X = BufferToDeleteAfterUsage_X;
  SubBuffer_X.MustBeDeleted_B = false;
  SubBuffer_X.Size_U64 = 64;

  OffsetInBuffer_U64 = 0;
  Finish_B = false;
  do
  {
    Rts_E = MediaDetector.ParseBuffer(SubBuffer_X, BofMediaDetector::ResultFormat::Text, _rResult_S, OffsetInBuffer_U64);
    if (Rts_E == BOF_ERR_SEEK)
    {
      SubBuffer_X.pData_U8 = BufferToDeleteAfterUsage_X.pData_U8 + OffsetInBuffer_U64;
    }
    else if (Rts_E == BOF_ERR_PENDING)
    {
      SubBuffer_X.pData_U8 += SubBuffer_X.Size_U64;
    }
    else Finish_B = true; //For BOF_ERR_NO_ERROR and the other one
  } while (!Finish_B);
  TotalByteNeeded_U32 = SubBuffer_X.pData_U8 - BufferToDeleteAfterUsage_X.pData_U8 + SubBuffer_X.Size_U64;
  printf("%s: %d byte parsed\n", _rPathname.FullPathName(false).c_str(), TotalByteNeeded_U32);
  return Rts_E;
}
TEST(Bof2d_Test, MediaDetectorParseBuffer)
{
  std::string Result_S;

  EXPECT_EQ(ParseFileBuffer(BOF::BofPath("./data/colorbar.jpg"), Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nFormat                                   : JPEG\r\nFile size                                : 9.60 KiB\r\n\r\nImage\r\nFormat                                   : JPEG\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : YUV\r\nChroma subsampling                       : 4:2:2\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossy\r\nStream size                              : 9.60 KiB (100%)\r\n\r\n");
#endif

  EXPECT_EQ(ParseFileBuffer(BOF::BofPath("./data/colorbar.png"), Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFile size                                : 1.45 KiB\r\n\r\nImage\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFormat_Compression                       : Deflate\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossless\r\nStream size                              : 1.45 KiB (100%)\r\n\r\n");
#endif

  EXPECT_EQ(ParseFileBuffer(BOF::BofPath("./data/colorbar_rle_32b.tga"), Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nFormat                                   : TGA\r\nFormat version                           : Version 1\r\nFile size                                : 18.0 KiB\r\n\r\nImage\r\nFormat                                   : RLE\r\nFormat/Info                              : Run-length encoding\r\nCodec ID                                 : 10\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 32 bits\r\n\r\n");
#endif
}

TEST(Bof2d_Test, MediaDetectorQuery)
{
  BofMediaDetector MediaDetector;
  std::string Result_S;

  EXPECT_EQ(MediaDetector.ParseFile("./data/colorbar.jpg", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_EQ(MediaDetector.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Name, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), "FileSize");
  EXPECT_EQ(MediaDetector.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Text, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), "9830");
  EXPECT_EQ(MediaDetector.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Measure, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), " byte");
  EXPECT_EQ(MediaDetector.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Options, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), "N YTY");
  EXPECT_EQ(MediaDetector.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Name_Text, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), "File size");
  EXPECT_EQ(MediaDetector.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Measure_Text, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), " byte");
  EXPECT_EQ(MediaDetector.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Info, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), "File size in bytes");
  EXPECT_EQ(MediaDetector.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::HowTo, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), "");
  EXPECT_EQ(MediaDetector.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Domain, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), "");

}


constexpr uint32_t SAMPLE_WIDTH = 1920;
constexpr uint32_t SAMPLE_HEIGHT = 540;
uint8_t            GL_pFileData_U8[SAMPLE_WIDTH * SAMPLE_HEIGHT * 4];
uint8_t            GL_ConvertedData_U8[SAMPLE_WIDTH * SAMPLE_HEIGHT * 4];

TEST(Bof2d_Test, UyvyToBgra)
{
  uint32_t Start_U32, DeltaInMs_U32, i_U32, FileSize_U32, ConvertedSize_U32;
  intptr_t Io;
  BOF::BOF_RECT Crop_X(SAMPLE_WIDTH / 2, SAMPLE_HEIGHT / 2, SAMPLE_WIDTH / 2, SAMPLE_HEIGHT / 2);

  FileSize_U32 = SAMPLE_WIDTH * SAMPLE_HEIGHT * 2;
  ConvertedSize_U32 = SAMPLE_WIDTH * SAMPLE_HEIGHT * 4;
  //  ConvertedSize_U32=(SAMPLE_WIDTH * SAMPLE_HEIGHT * 4)/4;//With crop and rotate
  EXPECT_EQ(Bof_OpenFile("./data/colorbar_1920x1080p59_8bit_captured_by_sdk.422", true, Io), BOF_ERR_NO_ERROR);

  EXPECT_EQ(BOF::Bof_ReadFile(Io, FileSize_U32, GL_pFileData_U8), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::Bof_CloseFile(Io), BOF_ERR_NO_ERROR);

  memset(GL_ConvertedData_U8, 0, ConvertedSize_U32);
  Start_U32 = BOF::Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < 10; i_U32++)
  {
    EXPECT_EQ(BOF::Bof_UyvyToBgra(SAMPLE_WIDTH, SAMPLE_HEIGHT, SAMPLE_WIDTH * 2, GL_pFileData_U8, SAMPLE_WIDTH * 4, nullptr, BOF_ROTATION_0, GL_ConvertedData_U8), BOF_ERR_NO_ERROR);
    //      Sts_E=BOF::Bof_UyvyToBgra(SAMPLE_WIDTH, SAMPLE_HEIGHT, SAMPLE_WIDTH * 2, GL_pFileData_U8, SAMPLE_WIDTH * 4,&Crop_X,BOF_ROTATION_90, GL_ConvertedData_U8);
  }
  DeltaInMs_U32 = BOF::Bof_ElapsedMsTime(Start_U32);
  if (!i_U32)
  {
    i_U32 = 1;
  }
  printf("%d conv in %d ms->%f / loop\n", i_U32, DeltaInMs_U32, (float)DeltaInMs_U32 / (float)i_U32);

  EXPECT_EQ(Bof_CreateFile(BOF::BOF_FILE_PERMISSION_READ_FOR_ALL | BOF::BOF_FILE_PERMISSION_WRITE_FOR_ALL, "./data/bgra.raw", false, Io), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::Bof_WriteFile(Io, ConvertedSize_U32, GL_ConvertedData_U8), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::Bof_CloseFile(Io), BOF_ERR_NO_ERROR);
}

TEST(Bof2d_Test, BgraToUyvy)
{
  uint32_t Start_U32, DeltaInMs_U32, i_U32, FileSize_U32, ConvertedSize_U32;
  intptr_t Io;
  BOF::BOF_RECT Crop_X(SAMPLE_WIDTH / 2, SAMPLE_HEIGHT / 2, SAMPLE_WIDTH / 2, SAMPLE_HEIGHT / 2);

  FileSize_U32 = SAMPLE_WIDTH * SAMPLE_HEIGHT * 4;
  ConvertedSize_U32 = SAMPLE_WIDTH * SAMPLE_HEIGHT * 2;
  // ConvertedSize_U32=(SAMPLE_WIDTH * SAMPLE_HEIGHT * 2)/4; //With crop and rotate
  EXPECT_EQ(Bof_OpenFile("./data/bgra.raw", true, Io), BOF_ERR_NO_ERROR);   //from previous test
  EXPECT_EQ(BOF::Bof_ReadFile(Io, FileSize_U32, GL_pFileData_U8), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::Bof_CloseFile(Io), BOF_ERR_NO_ERROR);

  memset(GL_ConvertedData_U8, 0, ConvertedSize_U32);
  Start_U32 = BOF::Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < 10; i_U32++)
  {
    EXPECT_EQ(BOF::Bof_BgraToUyvy(SAMPLE_WIDTH, SAMPLE_HEIGHT, SAMPLE_WIDTH * 4, GL_pFileData_U8, SAMPLE_WIDTH * 2, nullptr, BOF_ROTATION_0, GL_ConvertedData_U8), BOF_ERR_NO_ERROR);
    //      Sts_E=BOF::Bof_BgraToUyvy(SAMPLE_WIDTH, SAMPLE_HEIGHT, SAMPLE_WIDTH * 4, GL_pFileData_U8, SAMPLE_WIDTH * 2, &Crop_X,BOF_ROTATION_90, GL_ConvertedData_U8);
  }
  DeltaInMs_U32 = BOF::Bof_ElapsedMsTime(Start_U32);
  if (!i_U32)
  {
    i_U32 = 1;
  }
  printf("%d conv in %d ms->%f / loop\n", i_U32, DeltaInMs_U32, (float)DeltaInMs_U32 / (float)i_U32);

  EXPECT_EQ(Bof_CreateFile(BOF::BOF_FILE_PERMISSION_READ_FOR_ALL | BOF::BOF_FILE_PERMISSION_WRITE_FOR_ALL, "./uyvy.422", false, Io), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::Bof_WriteFile(Io, ConvertedSize_U32, GL_ConvertedData_U8), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::Bof_CloseFile(Io), BOF_ERR_NO_ERROR);
}