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
#if 0
static char *S_ppListOfGeneralParam[] = {"Count", "Status", "StreamCount", "StreamKind", "StreamKind/String", \
"StreamKindID", "StreamKindPos", "StreamOrder", "FirstPacketOrder", "Inform", "ID", "ID/String","OriginalSourceMedium_ID", \
"OriginalSourceMedium_ID / S","UniqueID","UniqueID / String","MenuID","MenuID / String","GeneralCount", \
"VideoCount","AudioCount","TetCount","OtherCount","ImageCount","MenuCount","Video_Format_List", \
"Video_Format_WithHint_Lis","Video_Codec_List","Video_Language_List","Audio_Format_List", \
"Audio_Format_WithHint_Lis", "Audio_Codec_List", "Audio_Language_List", "Text_Format_List", \
"Text_Format_WithHint_List", "Text_Codec_List", "Text_Language_Lis", "Other_Format_List", \
"Other_Format_WithHint_Lis", "Other_Codec_List", "Other_Language_List", "Image_Format_List", \
"Image_Format_WithHint_Lis", "Image_Codec_Lst", "Image_Language_List", "Menu_Format_List", \
"Menu_Format_WithHint_List", "Menu_Codec_List", "Menu_Language_List", "CompleteName", "FolderName", \
"FileNameExtesion", "FileName", "FileExtension", "CompleteName_Last", "FolderName_Last", \
"FileNameExtension_Last", "FileName_Last", "FileExtension_Last", "Format", "Format/Strng", \
"Format/Info", "Format/Url", "Format/Extensions", "Format_Commercial", "Format_Commercial_IfAny", \
"Format_Version", "Format_Profile", "Format_Level", "FormatCompression", "Format_Settings", \
"Format_AdditionalFeatures", "InternetMediaType", "CodecID", "CodecID/String", "CodecID/Info", \
"CodecID/Hint", "CodecID/Url", "CoecID_Description", "CodecID_Version", "CodecID_Compatible", "Interleaved", \
"Codec", "Codec/String", "Codec/Info", "Codec/Url", "Codec/Extensions", "Codec_Settings", \
"Codec_Settings_Automatic","FileSize","FileSize / String","FileSize / String1","FileSize / String2", \
"FileSize / String3","FileSize / String4","Duration","DurationString","Duration / String1", \
"Duration / String2","Duration / String3","Duration / String4","Duration / String5","Duration_Start", \
"Duration_End","OverallBitRate_Moe","OverallBitRate_Mode / Strin","OverallBitRate","OverallBitRate / String", \
"OverallBitRate_Minimum","OverallBitRate_Minimum / St","OverallBitRate_Nominal","OvrallBitRate_Nominal / St", \
"OverallBitRate_Maximum","OverallBitRate_Maximum / St","FrameRate","FrameRate / String","FrameRate_Num", \
"FrameRate_Den","FrameCount","Delay","Delay / String","Delay / String1","Delay / String2","Delay / String3", \
"Delay / String4","Delay / String5","Delay_Settings","Delay_DropFrame","Delay_Source","Delay_Source / String", \
"StreamSize","StreamSize / String","StreamSize / String1","StreamSize / String2","StreamSize / String3", \
"StreamSize / String4","StreamSize / Sting5","StreamSize_Proportion","StreamSize_Demuxed", \
"StreamSize_Demuxed / String","StreamSize_Demuxed / String1","StreamSize_Demuxed / String2", \
"StreamSize_Demued / String3","StreamSize_Demuxed / String4","StreamSize_Demuxed / String", \
"HeaderSize","DataSize","FooterSize","IsStreamable","Album_ReplayGain_Gain","Album_RplayGain_Gain / Stri","Album_ReplayGain_Peak","Encryption","Encryption_Format","Encryption_Length","Encryption_Method","Encryption_Mode","Encryption_Paddin","Encryption_InitializationV","UniversalAdID / String","UniversalAdID_Registry","UniversalAdID_Value","Title","Title_More","Title / Url","Domain","Collectio","Season","Season_Position","Season_Position_Total","Movie","Movie_More","Movie / Country","Movie / Url","Album","Album_More","Album / Sort","Album / Performer","Album / Performer / Sort","Album / Performer / Url","Comic","Comic_More","Comic / Position_Total","Part","Part / Position","Part / Position_Total","Track","Track_Mor","Track / Url"};
#endif

TEST(Bof2d_Test, MediaDetectorParam)
{
  BofMediaDetector MediaInfoParser;
  std::string Result_S;
  uint32_t i_U32;
  char *p_c, *q_c, *pColon_c, *pBuffer_c, *pSpace_c;

  //Oss = MediaInfoParser.Option(__T("Info_Version"), __T("0.7.13;MediaInfoDLL_Example_MSVC;0.7.13"));

  Result_S = MediaInfoParser.Option("Info_Parameters");
  pBuffer_c = new char[Result_S.size()];
  memcpy(pBuffer_c, Result_S.c_str(), Result_S.size());
  p_c = pBuffer_c;
  q_c = strchr(p_c, '\n');
  while (q_c)
  {
    *q_c = 0;
    pColon_c = strchr(p_c, ':');
    if (pColon_c)
    {
      *pColon_c = 0;
      pSpace_c = strchr(p_c, ' ');
      if (pSpace_c)
      {
        *pSpace_c = 0;fzefze
      }
    }
    p_c = q_c + 1;
    q_c = strchr(p_c, '\n');
  }
  BOF_SAFE_DELETE_ARRAY(pBuffer_c);
  //std::wcout << Oss;

/*
  EXPECT_EQ(MediaInfoDetector.ParseFile("./data/colorbar.jpg", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);

  for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(S_ppListOfGeneralParam); i_U32++)
  {
    EXPECT_EQ(MediaInfoDetector.Query(BOF::BofMediaDetector::MediaStreamType::General, S_ppListOfGeneralParam[i_U32],
                                   BOF::BofMediaDetector::InfoType::Text, Result_S), BOF_ERR_NO_ERROR);
    printf("Param '%s'='%s'\n", S_ppListOfGeneralParam[i_U32], Result_S.c_str());
  }
  */
}

TEST(Bof2d_Test, MediaDetectorParse)
{
  BofMediaDetector MediaInfoParser;
  std::string Result_S;




  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar.jpg", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);

  BOFERR e = MediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, "Count",
                      BOF::BofMediaDetector::InfoType::Text, Result_S);

  e = MediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, "Status",
                                 BOF::BofMediaDetector::InfoType::Text, Result_S);

  e = MediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, "StreamCount",
                          BOF::BofMediaDetector::InfoType::Text, Result_S);

  return;



  EXPECT_NE(MediaInfoParser.ParseFile("./data/dontexist.jpg", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);

  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar.jpg", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar.jpg\r\nFormat                                   : JPEG\r\nFile size                                : 9.60 KiB\r\n\r\nImage\r\nFormat                                   : JPEG\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : YUV\r\nChroma subsampling                       : 4:2:2\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossy\r\nStream size                              : 9.60 KiB (100%)\r\n\r\n");
#endif
  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar.jpg", BofMediaDetector::ResultFormat::Html, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "<html>\r\n\r\n<head>\r\n<META http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /></head>\r\n<body>\r\n<table width=\"100%\" border=\"0\" cellpadding=\"1\" cellspacing=\"2\" style=\"border:1px solid Navy\">\r\n<tr>\r\n    <td width=\"150\"><h2>General</h2></td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Complete name :</i></td>\r\n    <td colspan=\"3\">C:\\bld\\bofstd\\tests\\data\\colorbar.jpg</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Format :</i></td>\r\n    <td colspan=\"3\">JPEG</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>File size :</i></td>\r\n    <td colspan=\"3\">9.60 KiB</td>\r\n  </tr>\r\n</table>\r\n<br />\r\n<table width=\"100%\" border=\"0\" cellpadding=\"1\" cellspacing=\"2\" style=\"border:1px solid Navy\">\r\n<tr>\r\n    <td width=\"150\"><h2>Image</h2></td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Format :</i></td>\r\n    <td colspan=\"3\">JPEG</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Width :</i></td>\r\n    <td colspan=\"3\">259 pixels</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Height :</i></td>\r\n    <td colspan=\"3\">194 pixels</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Color space :</i></td>\r\n    <td colspan=\"3\">YUV</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Chroma subsampling :</i></td>\r\n    <td colspan=\"3\">4:2:2</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Bit depth :</i></td>\r\n    <td colspan=\"3\">8 bits</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Compression mode :</i></td>\r\n    <td colspan=\"3\">Lossy</td>\r\n  </tr>\r\n  <tr>\r\n    <td><i>Stream size :</i></td>\r\n    <td colspan=\"3\">9.60 KiB (100%)</td>\r\n  </tr>\r\n</table>\r\n<br />\r\n\r\n</body>\r\n</html>\r\n");
#endif
  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar.jpg", BofMediaDetector::ResultFormat::Json, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "{\r\n\"media\": {\r\n\"@ref\": \"C:\\\\bld\\\\bofstd\\\\tests\\\\data\\\\colorbar.jpg\",\r\n\"track\": [\r\n{\r\n\"@type\": \"General\",\r\n\"ImageCount\": \"1\",\r\n\"FileExtension\": \"jpg\",\r\n\"Format\": \"JPEG\",\r\n\"FileSize\": \"9830\",\r\n\"StreamSize\": \"0\",\r\n\"File_Created_Date\": \"UTC 2022-09-17 08:12:46.389\",\r\n\"File_Created_Date_Local\": \"2022-09-17 10:12:46.389\",\r\n\"File_Modified_Date\": \"UTC 2022-09-17 11:29:29.185\",\r\n\"File_Modified_Date_Local\": \"2022-09-17 13:29:29.185\"\r\n},\r\n{\r\n\"@type\": \"Image\",\r\n\"Format\": \"JPEG\",\r\n\"Width\": \"259\",\r\n\"Height\": \"194\",\r\n\"ColorSpace\": \"YUV\",\r\n\"ChromaSubsampling\": \"4:2:2\",\r\n\"BitDepth\": \"8\",\r\n\"Compression_Mode\": \"Lossy\",\r\n\"StreamSize\": \"9830\"\r\n}\r\n]\r\n}\r\n}\r\n");
#endif

  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_jpg_with_bad_ext.png",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_jpg_with_bad_ext.png\r\nFormat                                   : JPEG\r\nFile size                                : 9.63 KiB\r\nFileExtension_Invalid                    : h3d jpeg jpg jpe jps mpo\r\n\r\nImage\r\nFormat                                   : JPEG\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : YUV\r\nChroma subsampling                       : 4:2:2\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossy\r\nStream size                              : 9.63 KiB (100%)\r\n\r\n");
#endif
  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_jpg_with_no_ext",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_jpg_with_no_ext\r\nFormat                                   : JPEG\r\nFile size                                : 9.69 KiB\r\nFileExtension_Invalid                    : h3d jpeg jpg jpe jps mpo\r\n\r\nImage\r\nFormat                                   : JPEG\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : YUV\r\nChroma subsampling                       : 4:2:2\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossy\r\nStream size                              : 9.69 KiB (100%)\r\n\r\n");
#endif

  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar.png",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar.png\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFile size                                : 1.45 KiB\r\n\r\nImage\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFormat_Compression                       : Deflate\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossless\r\nStream size                              : 1.45 KiB (100%)\r\n\r\n");
#endif
  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_png_with_bad_ext.jpg",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_png_with_bad_ext.jpg\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFile size                                : 1.45 KiB\r\nFileExtension_Invalid                    : png pns\r\n\r\nImage\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFormat_Compression                       : Deflate\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossless\r\nStream size                              : 1.45 KiB (100%)\r\n\r\n");
#endif
  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_png_with_no_ext",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_png_with_no_ext\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFile size                                : 976 Bytes\r\nFileExtension_Invalid                    : png pns\r\n\r\nImage\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFormat_Compression                       : Deflate\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossless\r\nStream size                              : 976 Bytes (100%)\r\n\r\n");
#endif

  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_uncompress_32b.tga",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_uncompress_32b.tga\r\nFormat                                   : TGA\r\nFormat version                           : Version 2\r\nFile size                                : 197 KiB\r\n\r\nImage\r\nFormat                                   : Raw\r\nCodec ID                                 : 2\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 32 bits\r\n\r\n");
#endif
  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_rle_32b.tga",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_rle_32b.tga\r\nFormat                                   : TGA\r\nFormat version                           : Version 2\r\nFile size                                : 18.0 KiB\r\n\r\nImage\r\nFormat                                   : RLE\r\nFormat/Info                              : Run-length encoding\r\nCodec ID                                 : 10\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 32 bits\r\n\r\n");
#endif

  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_in_raw_32b_194x259.raw",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_in_raw_32b_194x259.raw\r\nFile size                                : 197 KiB\r\n\r\n");
#endif
  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_1920x1080p59_8bit_captured_by_sdk.422",BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_1920x1080p59_8bit_captured_by_sdk.422\r\nFile size                                : 1.98 MiB\r\n\r\n");
#endif
}
BOFERR ParseFileBuffer(BofPath &_rPathname, std::string &_rResult_S)
{
  BofMediaDetector MediaInfoParser;
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
    Rts_E = MediaInfoParser.ParseBuffer(SubBuffer_X, BofMediaDetector::ResultFormat::Text, _rResult_S, OffsetInBuffer_U64);
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
  BOF::BofPath Path("./data/colorbar.jpg");

  EXPECT_EQ(ParseFileBuffer(Path, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nFormat                                   : JPEG\r\nFile size                                : 9.60 KiB\r\n\r\nImage\r\nFormat                                   : JPEG\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : YUV\r\nChroma subsampling                       : 4:2:2\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossy\r\nStream size                              : 9.60 KiB (100%)\r\n\r\n");
#endif

  Path = BOF::BofPath("./data/colorbar.png");
  EXPECT_EQ(ParseFileBuffer(Path, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFile size                                : 1.45 KiB\r\n\r\nImage\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFormat_Compression                       : Deflate\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossless\r\nStream size                              : 1.45 KiB (100%)\r\n\r\n");
#endif

  Path = BOF::BofPath("./data/colorbar_rle_32b.tga");
  EXPECT_EQ(ParseFileBuffer(Path, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nFormat                                   : TGA\r\nFormat version                           : Version 1\r\nFile size                                : 18.0 KiB\r\n\r\nImage\r\nFormat                                   : RLE\r\nFormat/Info                              : Run-length encoding\r\nCodec ID                                 : 10\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 32 bits\r\n\r\n");
#endif
}

TEST(Bof2d_Test, MediaDetectorQuery)
{
  BofMediaDetector MediaInfoParser;
  std::string Result_S;

  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar.jpg", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_EQ(MediaInfoParser.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Name, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), "FileSize");
  EXPECT_EQ(MediaInfoParser.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Text, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), "9830");
  EXPECT_EQ(MediaInfoParser.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Measure, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), " byte");
  EXPECT_EQ(MediaInfoParser.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Options, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), "N YTY");
  EXPECT_EQ(MediaInfoParser.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Name_Text, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), "File size");
  EXPECT_EQ(MediaInfoParser.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Measure_Text, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), " byte");
  EXPECT_EQ(MediaInfoParser.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Info, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), "File size in bytes");
  EXPECT_EQ(MediaInfoParser.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::HowTo, Result_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Result_S.c_str(), "");
  //EXPECT_EQ(MediaInfoParser.Query(BofMediaDetector::MediaStreamType::General, "FileSize", BofMediaDetector::InfoType::Domain, Result_S), BOF_ERR_NO_ERROR);
  //EXPECT_STREQ(Result_S.c_str(), "");

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