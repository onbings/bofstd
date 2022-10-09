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
#include <bofstd/bofsystem.h>
#include <bofstd/bofparameter.h>
#include <bofstd/bofenum.h>
#include <bofstd/bofjsonwriter.h>

USE_BOF_NAMESPACE()

//Depends on line terminator and pathname
//#define CHECK_STR
void DisplayParamValue(BofMediaDetector &_rMediaInfoParser)
{
  std::string Result_S;
  uint32_t i_U32;
  char *p_c, *q_c, *pColon_c, *pBuffer_c, *pSpace_c;
  BOF::BofMediaDetector::MediaStreamType MediaStreamType_E = BOF::BofMediaDetector::MediaStreamType::General;
  std::string MediaStreamType_S, Type_S, Info_S;
  bool MoreThanOptionIAndO_B;
  //Oss = MediaInfoParser.Option(__T("Info_Version"), __T("0.7.13;MediaInfoDLL_Example_MSVC;0.7.13"));

  Result_S = _rMediaInfoParser.Option("Info_Parameters");
  pBuffer_c = new char[Result_S.size()];
  memcpy(pBuffer_c, Result_S.c_str(), Result_S.size());
  p_c = pBuffer_c;
  q_c = strchr(p_c, '\n');

  do
  {
    if (q_c)
    {
      *q_c = 0;
      pColon_c = strchr(p_c, ':');
      if (pColon_c)
      {
        *pColon_c = 0;
        pSpace_c = strchr(p_c, ' ');
        if (pSpace_c)
        {
          *pSpace_c = 0;
          if (strcmp(p_c, "Inform"))
          {
            Info_S = "";
            MoreThanOptionIAndO_B = false;
            EXPECT_EQ(_rMediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, p_c, BOF::BofMediaDetector::InfoType::Info, Result_S), BOF_ERR_NO_ERROR);
            if (Result_S != "")
            {
              Info_S += " I:'" + Result_S + "'";
            }

            //EXPECT_EQ(_rMediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, p_c, BOF::BofMediaDetector::InfoType::Name, Result_S), BOF_ERR_NO_ERROR);
            EXPECT_EQ(_rMediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, p_c, BOF::BofMediaDetector::InfoType::Text, Result_S), BOF_ERR_NO_ERROR);
            if (Result_S != "")
            {
              Info_S += " T:'" + Result_S + "'";
              MoreThanOptionIAndO_B = true;
            }
            EXPECT_EQ(_rMediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, p_c, BOF::BofMediaDetector::InfoType::Measure, Result_S), BOF_ERR_NO_ERROR);
            if (Result_S != "")
            {
              Info_S += " M:'" + Result_S + "'";
              MoreThanOptionIAndO_B = true;
            }
            EXPECT_EQ(_rMediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, p_c, BOF::BofMediaDetector::InfoType::Options, Result_S), BOF_ERR_NO_ERROR);
            if (Result_S != "")
            {
              Info_S += " O:'" + Result_S + "'";
            }
            //EXPECT_EQ(_rMediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, p_c, BOF::BofMediaDetector::InfoType::Name_Text, Result_S), BOF_ERR_NO_ERROR);
            EXPECT_EQ(_rMediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, p_c, BOF::BofMediaDetector::InfoType::Measure_Text, Result_S), BOF_ERR_NO_ERROR);
            if (Result_S != "")
            {
              Info_S += " m:'" + Result_S + "'";
              MoreThanOptionIAndO_B = true;
            }
            EXPECT_EQ(_rMediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, p_c, BOF::BofMediaDetector::InfoType::HowTo, Result_S), BOF_ERR_NO_ERROR);
            if (Result_S != "")
            {
              Info_S += " H:'" + Result_S + "'";
              MoreThanOptionIAndO_B = true;
            }
            EXPECT_EQ(_rMediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, p_c, BOF::BofMediaDetector::InfoType::Domain, Result_S), BOF_ERR_NO_ERROR);
            if (Result_S != "")
            {
              Info_S += " D:'" + Result_S + "'";
              MoreThanOptionIAndO_B = true;
            }
            if ((MoreThanOptionIAndO_B) && (Info_S != ""))
            {
              printf("%s Param '%s' = %s\n", MediaStreamType_S.c_str(), p_c, Info_S.c_str());
            }
          }
        }
      }
      else
      {
        Type_S = BOF::Bof_StringTrim(p_c);
        if (Type_S == "General")
        {
          MediaStreamType_E = BOF::BofMediaDetector::MediaStreamType::General;
          MediaStreamType_S = "General";
        }
        else if (Type_S == "Video")
        {
          MediaStreamType_E = BOF::BofMediaDetector::MediaStreamType::Video;
          MediaStreamType_S = "Video";
        }
        else if (Type_S == "Audio")
        {
          MediaStreamType_E = BOF::BofMediaDetector::MediaStreamType::Audio;
          MediaStreamType_S = "Audio";
        }
        else if (Type_S == "Text")
        {
          MediaStreamType_E = BOF::BofMediaDetector::MediaStreamType::Text;
          MediaStreamType_S = "Text";
        }
        else if (Type_S == "Other")
        {
          MediaStreamType_E = BOF::BofMediaDetector::MediaStreamType::Other;
          MediaStreamType_S = "Other";
        }
        else if (Type_S == "Image")
        {
          MediaStreamType_E = BOF::BofMediaDetector::MediaStreamType::Image;
          MediaStreamType_S = "Image";
        }
        else if (Type_S == "Menu")
        {
          MediaStreamType_E = BOF::BofMediaDetector::MediaStreamType::Menu;
          MediaStreamType_S = "Menu";
        }
      }
      p_c = q_c + 1;
      q_c = strchr(p_c, '\n');
    }
  } while (q_c);
  BOF_SAFE_DELETE_ARRAY(pBuffer_c);
}


TEST(Bof2d_Test, MediaDetectorParam)
{
  BofMediaDetector MediaInfoParser;
  std::string Result_S;

  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar.jpg", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
  /*
    EXPECT_EQ(MediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, "FileExtension", BOF::BofMediaDetector::InfoType::Name, Result_S), BOF_ERR_NO_ERROR);
    printf("Name: %s\n", Result_S.c_str());
    EXPECT_EQ(MediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, "FileExtension", BOF::BofMediaDetector::InfoType::Text, Result_S), BOF_ERR_NO_ERROR);
    printf("Text: %s\n", Result_S.c_str());
    EXPECT_EQ(MediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, "FileExtension", BOF::BofMediaDetector::InfoType::Measure, Result_S), BOF_ERR_NO_ERROR);
    printf("Measure: %s\n", Result_S.c_str());
    EXPECT_EQ(MediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, "FileExtension", BOF::BofMediaDetector::InfoType::Options, Result_S), BOF_ERR_NO_ERROR);
    printf("Options: %s\n", Result_S.c_str());
    EXPECT_EQ(MediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, "FileExtension", BOF::BofMediaDetector::InfoType::Name_Text, Result_S), BOF_ERR_NO_ERROR);
    printf("Name_Text: %s\n", Result_S.c_str());
    EXPECT_EQ(MediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, "FileExtension", BOF::BofMediaDetector::InfoType::Measure_Text, Result_S), BOF_ERR_NO_ERROR);
    printf("Measure_Text: %s\n", Result_S.c_str());
    EXPECT_EQ(MediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, "FileExtension", BOF::BofMediaDetector::InfoType::Info, Result_S), BOF_ERR_NO_ERROR);
    printf("Info: %s\n", Result_S.c_str());
    EXPECT_EQ(MediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, "FileExtension", BOF::BofMediaDetector::InfoType::HowTo, Result_S), BOF_ERR_NO_ERROR);
    printf("HowTo: %s\n", Result_S.c_str());
    EXPECT_EQ(MediaInfoParser.Query(BOF::BofMediaDetector::MediaStreamType::General, "FileExtension", BOF::BofMediaDetector::InfoType::Domain, Result_S), BOF_ERR_NO_ERROR);
    printf("Domain: %s\n", Result_S.c_str());
  */

  DisplayParamValue(MediaInfoParser);
  EXPECT_EQ(MediaInfoParser.ParseFile("./data/sample-mp4-file.mp4", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
  DisplayParamValue(MediaInfoParser);

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

  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_jpg_with_bad_ext.png", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_jpg_with_bad_ext.png\r\nFormat                                   : JPEG\r\nFile size                                : 9.63 KiB\r\nFileExtension_Invalid                    : h3d jpeg jpg jpe jps mpo\r\n\r\nImage\r\nFormat                                   : JPEG\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : YUV\r\nChroma subsampling                       : 4:2:2\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossy\r\nStream size                              : 9.63 KiB (100%)\r\n\r\n");
#endif
  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_jpg_with_no_ext", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_jpg_with_no_ext\r\nFormat                                   : JPEG\r\nFile size                                : 9.69 KiB\r\nFileExtension_Invalid                    : h3d jpeg jpg jpe jps mpo\r\n\r\nImage\r\nFormat                                   : JPEG\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : YUV\r\nChroma subsampling                       : 4:2:2\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossy\r\nStream size                              : 9.69 KiB (100%)\r\n\r\n");
#endif

  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar.png", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar.png\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFile size                                : 1.45 KiB\r\n\r\nImage\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFormat_Compression                       : Deflate\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossless\r\nStream size                              : 1.45 KiB (100%)\r\n\r\n");
#endif
  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_png_with_bad_ext.jpg", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_png_with_bad_ext.jpg\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFile size                                : 1.45 KiB\r\nFileExtension_Invalid                    : png pns\r\n\r\nImage\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFormat_Compression                       : Deflate\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossless\r\nStream size                              : 1.45 KiB (100%)\r\n\r\n");
#endif
  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_png_with_no_ext", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_png_with_no_ext\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFile size                                : 976 Bytes\r\nFileExtension_Invalid                    : png pns\r\n\r\nImage\r\nFormat                                   : PNG\r\nFormat/Info                              : Portable Network Graphic\r\nFormat_Compression                       : Deflate\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 8 bits\r\nCompression mode                         : Lossless\r\nStream size                              : 976 Bytes (100%)\r\n\r\n");
#endif

  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_uncompress_32b.tga", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_uncompress_32b.tga\r\nFormat                                   : TGA\r\nFormat version                           : Version 2\r\nFile size                                : 197 KiB\r\n\r\nImage\r\nFormat                                   : Raw\r\nCodec ID                                 : 2\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 32 bits\r\n\r\n");
#endif
  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_rle_32b.tga", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_rle_32b.tga\r\nFormat                                   : TGA\r\nFormat version                           : Version 2\r\nFile size                                : 18.0 KiB\r\n\r\nImage\r\nFormat                                   : RLE\r\nFormat/Info                              : Run-length encoding\r\nCodec ID                                 : 10\r\nWidth                                    : 259 pixels\r\nHeight                                   : 194 pixels\r\nColor space                              : RGB\r\nBit depth                                : 32 bits\r\n\r\n");
#endif

  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_in_raw_32b_194x259.raw", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_in_raw_32b_194x259.raw\r\nFile size                                : 197 KiB\r\n\r\n");
#endif
  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar_1920x1080p59_8bit_captured_by_sdk.422", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);
#if defined(CHECK_STR)
  EXPECT_STREQ(Result_S.c_str(), "General\r\nComplete name                            : C:\\bld\\bofstd\\tests\\data\\colorbar_1920x1080p59_8bit_captured_by_sdk.422\r\nFile size                                : 1.98 MiB\r\n\r\n");
#endif

  EXPECT_EQ(MediaInfoParser.ParseFile("./data/sample-mp4-file.mp4", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);

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

/*
{
    "media": {
    "@ref": "C:\\pro\\evs-muse\\evs-muse-storage\\tests\\data\\pexels-alexander-grey-1149347.png",  => Replaced by Mfs name ex: /media/still/pexels-alexander-grey-1149347.png
        "track": [
        {
            "@type": "General",
            "ImageCount": 1,
            "Format": "PNG",
            "FileSize": 32183316,
            "StreamSize": 0,
            "File_Created_Date": "UTC 2022-09-21 12:16:43.685",
            "File_Modified_Date": "UTC 2022-09-21 12:16:43.701",
        },
        {
            "@type": "Image",
            "Format": "PNG",
            "Width": 5616,
            "Height": 3744,
            "ColorSpace": "RGB",
            "BitDepth": 8,
            "StreamSize": 32183316
            "Uri": " storage://10.129.4.172:11000/5/file/b09fdf1fdc7edc9f87c1c0f3efddf742e4f2f4f0"   =>Inserted by Mfs when ImportMedia is executed
        }
        ]
    }
}*/
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
  { MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_MAX, "Max" }
                                                                          });

/*
const std::string &S_MuseFileSystemMediaTypeEnumToString(int _EnumValue_i)
{
  return S_MuseFileSystemMediaTypeEnumConverter.ToString(_EnumValue_i);
}
*/
int S_MuseFileSystemMediaTypeEnumFromString(const std::string &_rEnumValue_S)
{
  return static_cast<int>(S_MuseFileSystemMediaTypeEnumConverter.ToEnum(_rEnumValue_S));
}

#if 0
using MuseFileSystemMediaTypeEnumConverter = BofEnum<MUSE_FILE_SYSTEM_MEDIA_TYPE>;
static MuseFileSystemMediaTypeEnumConverter &S_MuseFileSystemMediaTypeConverter()
{
  static MuseFileSystemMediaTypeEnumConverter S_TheMuseFileSystemMediaTypeConverter
  {
    {
      {MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_UNKNOWN, "Unknown"},
      {MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_STILL, "Still"},
      {MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_CLIP, "Clip"},
      {MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_MAX, "Max"},
    },
    //			MUSE_FILE_SYSTEM_MEDIA_TYPE::UNDEF
  };
  return S_TheMuseFileSystemMediaTypeConverter;
}
//using BOF_PARAMETER_ENUM_TO_STRING = std::function<std::string(int _EnumValue_i)>;
//using BOF_PARAMETER_STRING_TO_ENUM = std::function<int(const std::string &_rEnumValue_S)>;
//std::string Tp_S = S_MuseFileSystemMediaTypeConverter().ToString(S_MuseFileSystemMediaStillJson_X.General_X.MediaType_E);
//MUSE_FILE_SYSTEM_MEDIA_TYPE Tp_E = S_MuseFileSystemMediaTypeConverter().ToEnum(Tp_S);

static std::string S_MuseFileSystemMediaTypeEnumToString(int _EnumValue_i)
{
  return S_MuseFileSystemMediaTypeConverter().ToString(static_cast<MUSE_FILE_SYSTEM_MEDIA_TYPE>(_EnumValue_i));
}
static int S_MuseFileSystemMediaTypeEnumFromString(const std::string &_rEnumValue_S)
{
  return static_cast<int>(S_MuseFileSystemMediaTypeConverter().ToEnum(_rEnumValue_S));
}
#endif
enum class MUSE_FILE_SYSTEM_MEDIA_FORMAT :int32_t
{
  MUSE_FILE_SYSTEM_MEDIA_FORMAT_UNKNOWN = 0,
  MUSE_FILE_SYSTEM_MEDIA_FORMAT_PNG,
  MUSE_FILE_SYSTEM_MEDIA_FORMAT_JPG,
  MUSE_FILE_SYSTEM_MEDIA_FORMAT_MAX
};
using MuseFileSystemMediaFormatEnumConverter = BofEnum<MUSE_FILE_SYSTEM_MEDIA_FORMAT>;
static MuseFileSystemMediaFormatEnumConverter &S_MuseFileSystemMediaFormatConverter()
{
  static MuseFileSystemMediaFormatEnumConverter S_TheMuseFileSystemMediaFormatConverter
  {
    {
      {MUSE_FILE_SYSTEM_MEDIA_FORMAT::MUSE_FILE_SYSTEM_MEDIA_FORMAT_UNKNOWN, "Unknown"},
      {MUSE_FILE_SYSTEM_MEDIA_FORMAT::MUSE_FILE_SYSTEM_MEDIA_FORMAT_PNG, "Png"},
      {MUSE_FILE_SYSTEM_MEDIA_FORMAT::MUSE_FILE_SYSTEM_MEDIA_FORMAT_JPG, "Jpeg"},
      {MUSE_FILE_SYSTEM_MEDIA_FORMAT::MUSE_FILE_SYSTEM_MEDIA_FORMAT_MAX, "Max"},
    },
    //			MUSE_FILE_SYSTEM_MEDIA_FORMAT::UNDEF
  };
  return S_TheMuseFileSystemMediaFormatConverter;
}

enum class MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE :int32_t
{
  MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE_UNKNOWN = 0,
  MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE_RBG,
  MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE_RGBA,
  MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE_YUV,
  MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE_MAX
};
using MuseFileSystemMediaColorSpaceEnumConverter = BofEnum<MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE>;
static MuseFileSystemMediaColorSpaceEnumConverter &S_MuseFileSystemMediaColorSpaceConverter()
{
  static MuseFileSystemMediaColorSpaceEnumConverter S_TheMuseFileSystemMediaColorSpaceConverter
  {
    {
      {MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE::MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE_UNKNOWN, "Unknown"},
      {MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE::MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE_RBG, "Rgb"},
      {MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE::MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE_RGBA, "RgbA"},
      {MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE::MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE_YUV, "Yuv"},
      {MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE::MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE_MAX, "Max"},
    },
    //			MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE::UNDEF
  };
  return S_TheMuseFileSystemMediaColorSpaceConverter;
}

struct MUSE_FILE_SYSTEM_MEDIA_GENERAL
{
  uint64_t FileSize_U64;
  BOF_DATE_TIME Created_X;
  BOF_DATE_TIME Modified_X;

  MUSE_FILE_SYSTEM_MEDIA_TYPE MediaType_E;
  MUSE_FILE_SYSTEM_MEDIA_FORMAT MediaFormat_E;
  MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE ColorSpace_E;
  uint32_t Width_U32;
  uint32_t Height_U32;
  uint32_t BitDepth_U32;

  MUSE_FILE_SYSTEM_MEDIA_GENERAL()
  {
    Reset();
  }
  void Reset()
  {
    FileSize_U64 = 0;
    Created_X = BOF_DATE_TIME();
    Modified_X = BOF_DATE_TIME();

    MediaType_E = MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_UNKNOWN;
    MediaFormat_E = MUSE_FILE_SYSTEM_MEDIA_FORMAT::MUSE_FILE_SYSTEM_MEDIA_FORMAT_UNKNOWN;
    ColorSpace_E = MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE::MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE_UNKNOWN;
    Width_U32 = 0;
    Height_U32 = 0;
    BitDepth_U32 = 0;
  }
  std::string  ToString()
  {
    return "Sz " + std::to_string(FileSize_U64) + " B) Create " + Created_X.ToString() + " Modif " + Created_X.ToString() +
      S_MuseFileSystemMediaTypeEnumConverter.ToString(MediaType_E) + " " +
      S_MuseFileSystemMediaFormatConverter().ToString(MediaFormat_E) + " " +
      std::to_string(Width_U32) + "x" + std::to_string(Height_U32) + " " +
      S_MuseFileSystemMediaColorSpaceConverter().ToString(ColorSpace_E) + " Depth " +
      std::to_string(BitDepth_U32);
  }
};

struct MUSE_FILE_SYSTEM_MEDIA_STILL
{
  BofUri Uri;     //evs storage gui
  BofPath Ref;    //Original path
  BofPath Path;   //Muse filesystem path

  MUSE_FILE_SYSTEM_MEDIA_GENERAL      General_X;

  MUSE_FILE_SYSTEM_MEDIA_STILL()
  {
    Reset();
  }
  void Reset()
  {
    Uri = BofUri();
    Ref = BofPath();
    Path = BofPath();
    General_X.Reset();
  }
  std::string ToString()
  {
    return Uri.ToString() + " " + Ref.FullPathName(false) + " " + Path.FullPathName(false) + " " + General_X.ToString();
  }
};
MUSE_FILE_SYSTEM_MEDIA_STILL S_MuseFileSystemMediaStillJson_X;
std::vector< BOFPARAMETER > S_MuseFileSystemMediaStillJsonSchemaCollection =
{
// {nullptr, std::string("Type"), std::string(""), std::string(""), std::string("Media"), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_ENUM(S_MuseFileSystemMediaStillJson_X.General_X.MediaType_E, MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_UNKNOWN, MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_MAX, S_MuseFileSystemMediaTypeEnumToString, S_MuseFileSystemMediaTypeEnumFromString)},
 {nullptr, std::string("Type"), std::string(""), std::string(""), std::string("Media"), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_ENUM(S_MuseFileSystemMediaStillJson_X.General_X.MediaType_E, 
 MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_UNKNOWN, MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_MAX, 
 std::bind(&BofEnum<MUSE_FILE_SYSTEM_MEDIA_TYPE>::ToStringFromInt, &S_MuseFileSystemMediaTypeEnumConverter, std::placeholders::_1), S_MuseFileSystemMediaTypeEnumFromString)},
  utiliseer BOF_BIND_1_ARG_TO_METHOD remanier


  {nullptr, std::string("Uri"), std::string(""), std::string(""), std::string("Media"), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MuseFileSystemMediaStillJson_X.Uri, URI, 1, 512)},
  {nullptr, std::string("Ref"), std::string(""), std::string(""), std::string("Media"), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MuseFileSystemMediaStillJson_X.Ref, PATH, 1, 512)},
  {nullptr, std::string("Path"), std::string(""), std::string(""), std::string("Media"), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MuseFileSystemMediaStillJson_X.Path, PATH, 1, 512)},

  {nullptr, std::string("FileSize"), std::string(""), std::string(""), std::string("Media"), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MuseFileSystemMediaStillJson_X.General_X.FileSize_U64, UINT64, 0, 0)},
  {nullptr, std::string("Created"), std::string(""), std::string("UTC %Y-%m-%d %H:%M:%S"), std::string("Media"), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MuseFileSystemMediaStillJson_X.General_X.Created_X, DATETIME, 0, 0)},
  {nullptr, std::string("Modified"), std::string(""), std::string("UTC %Y-%m-%d %H:%M:%S"), std::string("Media"), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_MuseFileSystemMediaStillJson_X.General_X.Modified_X, DATETIME, 0, 0)},

//  {nullptr, std::string("Type"), std::string(""), std::string(""), std::string("Media"), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_ENUM(S_MuseFileSystemMediaStillJson_X.General_X.MediaType_E, MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_UNKNOWN, MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_MAX, S_MuseFileSystemMediaTypeEnumToString, S_MuseFileSystemMediaTypeEnumFromString)},
//  {nullptr, std::string("Format"), std::string(""), std::string(""), std::string("Media"), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_ENUM(S_MuseFileSystemMediaStillJson_X.General_X.MediaFormat_E, MUSE_FILE_SYSTEM_MEDIA_FORMAT::MUSE_FILE_SYSTEM_MEDIA_FORMAT_UNKNOWN, MUSE_FILE_SYSTEM_MEDIA_FORMAT::MUSE_FILE_SYSTEM_MEDIA_FORMAT_MAX)},
//  {nullptr, std::string("ColorSpace"), std::string(""), std::string(""), std::string("Media"), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_ENUM(S_MuseFileSystemMediaStillJson_X.General_X.ColorSpace_E, MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE::MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE_UNKNOWN, MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE::MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE_MAX)},
};

using BHA = std::function<int (int)>;

TEST(Bof2d_Test, MediaDetectorToJson)
{
  BofMediaDetector MediaInfoParser;
  std::string Result_S, JsonOut_S;
  BofJsonWriter BofJsonWriter;
  //https://stackoverflow.com/questions/24874478/use-stdbind-with-overloaded-functions
//  BOF_PARAMETER_ENUM_TO_STRING f = std::bind(static_cast<const std::string & (int) const>(&BofEnum<MUSE_FILE_SYSTEM_MEDIA_TYPE>::ToString), &S_MuseFileSystemMediaTypeEnumConverter, 22); std::placeholders::_1);
  //BOF_PARAMETER_ENUM_TO_STRING f = std::bind(static_cast<const std::string & (&) (int) const>(&BofEnum<MUSE_FILE_SYSTEM_MEDIA_TYPE>::ToString), &S_MuseFileSystemMediaTypeEnumConverter, std::placeholders::_1);
  //  BOF_PARAMETER_ENUM_TO_STRING f = std::bind(static_cast<const std::string & (*) (int) const>(&BofEnum<MUSE_FILE_SYSTEM_MEDIA_TYPE>::ToString), &S_MuseFileSystemMediaTypeEnumConverter, std::placeholders::_1);
  //BOF_PARAMETER_ENUM_TO_STRING f = std::bind<const std::string &(int) const>(&BofEnum<MUSE_FILE_SYSTEM_MEDIA_TYPE>::ToString, &S_MuseFileSystemMediaTypeEnumConverter, std::placeholders::_1);
  //BOF_PARAMETER_ENUM_TO_STRING f = std::bind<BOF_PARAMETER_ENUM_TO_STRING>(&BofEnum<MUSE_FILE_SYSTEM_MEDIA_TYPE>::ToString, &S_MuseFileSystemMediaTypeEnumConverter, std::placeholders::_1));
  //BOF_PARAMETER_ENUM_TO_STRING g = std::bind(&BofEnum<MUSE_FILE_SYSTEM_MEDIA_TYPE>::ToStringbha, &S_MuseFileSystemMediaTypeEnumConverter, std::placeholders::_1);
  //BHA h =                        std::bind(&BofEnum<MUSE_FILE_SYSTEM_MEDIA_TYPE>::Bha, &S_MuseFileSystemMediaTypeEnumConverter, std::placeholders::_1);

  EXPECT_EQ(MediaInfoParser.ParseFile("./data/colorbar.jpg", BofMediaDetector::ResultFormat::Text, Result_S), BOF_ERR_NO_ERROR);

  S_MuseFileSystemMediaStillJson_X.Uri = BofUri("storage://10.129.4.172:11000/5/file/b09fdf1fdc7edc9f87c1c0f3efddf742e4f2f4f0");
  S_MuseFileSystemMediaStillJson_X.Ref = BofPath("C:\\pro\\evs-muse\\evs-muse-storage\\tests\\data\\pexels-alexander-grey-1149347.png");
  S_MuseFileSystemMediaStillJson_X.Path = BofPath("/media/still/pexels-alexander-grey-1149347.png");

  S_MuseFileSystemMediaStillJson_X.General_X.FileSize_U64 = 32183316;
  S_MuseFileSystemMediaStillJson_X.General_X.Created_X = BOF_DATE_TIME(21,9,2022,12,16,43,685);
  S_MuseFileSystemMediaStillJson_X.General_X.Modified_X = BOF_DATE_TIME(21, 9, 2022, 12, 16, 43, 701);
  S_MuseFileSystemMediaStillJson_X.General_X.MediaType_E = MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_STILL;
  S_MuseFileSystemMediaStillJson_X.General_X.MediaFormat_E = MUSE_FILE_SYSTEM_MEDIA_FORMAT::MUSE_FILE_SYSTEM_MEDIA_FORMAT_PNG;
  S_MuseFileSystemMediaStillJson_X.General_X.ColorSpace_E = MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE::MUSE_FILE_SYSTEM_MEDIA_COLOR_SPACE_RBG;
  S_MuseFileSystemMediaStillJson_X.General_X.Width_U32 = 5616;
  S_MuseFileSystemMediaStillJson_X.General_X.Height_U32 = 3744;
  S_MuseFileSystemMediaStillJson_X.General_X.BitDepth_U32 = 8;

  std::string Tp_S = S_MuseFileSystemMediaTypeEnumConverter.ToString(S_MuseFileSystemMediaStillJson_X.General_X.MediaType_E);
  MUSE_FILE_SYSTEM_MEDIA_TYPE Tp_E = S_MuseFileSystemMediaTypeEnumConverter.ToEnum(Tp_S);

  /*
    {
        "media": {
        "@ref": "C:\\pro\\evs-muse\\evs-muse-storage\\tests\\data\\pexels-alexander-grey-1149347.png",  => Replaced by Mfs name ex: /media/still/pexels-alexander-grey-1149347.png
            "track": [
            {
                "@type": "General",
                "ImageCount": 1,
                "Format": "PNG",
                "FileSize": 32183316,
                "StreamSize": 0,
                "File_Created_Date": "UTC 2022-09-21 12:16:43.685",
                "File_Modified_Date": "UTC 2022-09-21 12:16:43.701",
            },
            {
                "@type": "Image",
                "Format": "PNG",
                "Width": 5616,
                "Height": 3744,
                "ColorSpace": "RGB",
                "BitDepth": 8,
                "StreamSize": 32183316
                "Uri": " storage://10.129.4.172:11000/5/file/b09fdf1fdc7edc9f87c1c0f3efddf742e4f2f4f0"   =>Inserted by Mfs when ImportMedia is executed
            }
            ]
        }
    }*/
  EXPECT_EQ(BofJsonWriter.FromByte(false, false, S_MuseFileSystemMediaStillJsonSchemaCollection, JsonOut_S), BOF_ERR_NO_ERROR);
  printf("%s", JsonOut_S.c_str());

}

TEST(Bof2d_Test, MediaDetectorFromJson)
{
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