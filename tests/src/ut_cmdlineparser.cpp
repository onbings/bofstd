/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the BofCmdLineParse class
 *
 * Name:        ut_cmdlineparser.cpp
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
#include <bofstd/bofaudiostandard.h>
#include <bofstd/bofcommandlineparser.h>
#include <bofstd/bofdatetime.h>
#include <bofstd/bofenum.h>
#include <bofstd/bofpath.h>
#include <bofstd/bofsocket.h>
#include <bofstd/boftimecode.h>
#include <bofstd/bofuri.h>
#include <bofstd/bofvideostandard.h>

#include "gtestrunner.h"

#include <memory>

// To use a test fixture, derive from testing::Test class
class CmdLineParser_Test : public testing::Test
{
public:
  // Per-test-case set-up. Called before the first test in this test case.
  static void SetUpTestCase();

  // Per-test-case tear-down. Called after the last test in this test case.
  static void TearDownTestCase();

protected:
  // You can define per-test set-up and tear-down logic as usual.
  virtual void SetUp();
  virtual void TearDown();
};

USE_BOF_NAMESPACE()

enum class WORKING_MODE : uint32_t
{
  GENERATE = 0,
  MAKE
};
enum class BOF2D_AUDIO_FORMAT : int32_t
{
  BOF2D_AUDIO_FORMAT_PCM = 0,
  BOF2D_AUDIO_FORMAT_WAV,
  BOF2D_AUDIO_FORMAT_MAX
};
extern BOF::BofEnum<BOF2D_AUDIO_FORMAT> S_Bof2dAudioFormatEnumConverter;

struct ARRAYENTRY
{
  uint32_t c_U32;
  uint8_t a_U8;
  uint16_t b_U16;
  char pTxt_c[32];
  BOF_SOCKET_ADDRESS_COMPONENT IpV4_X;
  ARRAYENTRY()
  {
    Reset();
  }
  void Reset()
  {
    c_U32 = 0;
    a_U8 = 0;
    b_U16 = 0;
    pTxt_c[0] = 0;
    IpV4_X.Reset();
  }
};

struct CLI_APPPARAM
{
  bool AskHelp_B;
  WORKING_MODE WorkingMode_E;

  std::vector<std::string> VecStringRegExpCollection;
  std::vector<uint32_t> VecU32Collection;
  char pSlnPath[256];
  double Value_lf;
  uint32_t Value_U32;
  BOF_SOCKET_ADDRESS_COMPONENT IpV4_X;
  BOF_SOCKET_ADDRESS_COMPONENT IpV6_X;
  uint32_t pVal_U32[8];
  char pVal_c[7][256];
  ARRAYENTRY pArray_X[6];
  std::string Str_S;
  BofDateTime Date;
  BofDateTime Time;
  BofDateTime DateTime;
  BofPath Path;
  BofUri Uri1;
  BofUri Uri2;
  uint8_t Val_U8;
  uint16_t Val_U16;
  uint32_t Val_U32;
  uint64_t Val_U64;
  int8_t Val_S8;
  int16_t Val_S16;
  int32_t Val_S32;
  int64_t Val_S64;
  float Val_f;
  double Val_lf;
  BofVideoStandard Vs;
  BofAudioStandard As;
  BofTimecode Tc;
  BOF_SIZE<uint32_t> Size_X;

  CLI_APPPARAM()
  {
    int i;

    AskHelp_B = false;
    // Setup application default value
    WorkingMode_E = WORKING_MODE::GENERATE;

    VecU32Collection.clear();
    pSlnPath[0] = 0;
    Value_lf = 0;
    Value_U32 = 0;
    IpV4_X.Reset();
    IpV6_X.Reset();
    memset(&pVal_U32, 0, sizeof(pVal_U32));
    memset(&pVal_c, 0, sizeof(pVal_c));
    for (i = 0; i < BOF_NB_ELEM_IN_ARRAY(pArray_X); i++)
    {
      pArray_X[i].Reset();
    }
    Str_S = "empty";
    Date.Reset();
    Time.Reset();
    DateTime.Reset();

    Val_U8 = 0;
    Val_U16 = 0;
    Val_U32 = 0;
    Val_U64 = 0;
    Val_S8 = 0;
    Val_S16 = 0;
    Val_S32 = 0;
    Val_S64 = 0;
    Val_f = 0;
    Val_lf = 0;
    Size_X.Reset();
  }
};
BOF::BofEnum<BOF2D_AUDIO_FORMAT> S_Bof2dAudioFormatEnumConverter(
    {
        {BOF2D_AUDIO_FORMAT::BOF2D_AUDIO_FORMAT_PCM, "PCM"},
        {BOF2D_AUDIO_FORMAT::BOF2D_AUDIO_FORMAT_WAV, "WAV"},
        {BOF2D_AUDIO_FORMAT::BOF2D_AUDIO_FORMAT_MAX, "MAX"},
    },
    BOF2D_AUDIO_FORMAT::BOF2D_AUDIO_FORMAT_MAX);
struct BOF2D_AUDIO_OPTION
{
  BOF::BofPath BasePath;
  uint32_t NbChannel_U32;
  uint64_t ChannelLayout_U64;
  uint32_t SampleRateInHz_U32;
  BOF2D_AUDIO_FORMAT Format_E;

  BOF2D_AUDIO_OPTION()
  {
    Reset();
  }

  void Reset()
  {
    BasePath = "";
    NbChannel_U32 = 0;
    ChannelLayout_U64 = 0;
    SampleRateInHz_U32 = 0;
    Format_E = BOF2D_AUDIO_FORMAT::BOF2D_AUDIO_FORMAT_MAX;
  }
};
static BOF2D_AUDIO_OPTION S_AudioOption_X;
static CLI_APPPARAM S_AppParam_X;

BOFERR CmdLineParseResultUltimateCheck(int /*_Index_U32*/, const BOFPARAMETER & /*_rBofCommandlineOption_X*/, const bool _CheckOk_B, const char * /*_pOptNewVal_c*/)
{
  BOFERR Rts_E = _CheckOk_B ? BOF_ERR_NO_ERROR : BOF_ERR_NO;

  return Rts_E;
}

BOFERR CmdLineParseError(const char *_pError_c)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  return Rts_E;
}

static std::vector<BOFPARAMETER> S_pCommandLineOption_X = {
    //? cannot be used                   {nullptr, std::string("?"),        std::string("Ask help."),                                                         std::string(""),                  std::string(""), BOFPARAMETER_ARG_FLAG::NONE,
    // BOF_PARAM_DEF_VARIABLE(S_AppParam_X.AskHelp_B, BOOL, true, 0)},

    {nullptr, std::string("h"), std::string("Ask help."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.AskHelp_B, BOOL, true, 0)},
    {nullptr, std::string("help"), std::string("Ask help."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.AskHelp_B, BOOL, true, 0)},

    {nullptr, std::string("vec_regexp"), std::string("Specifies a std::vector<std::string> regexp."), std::string("\\s*(0|91)?[6-9][0-9]{9}"), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG | BOFPARAMETER_ARG_FLAG::STR_FORMAT_ISREGEXP,
     BOF_PARAM_DEF_VECTOR(S_AppParam_X.VecStringRegExpCollection, STDSTRING, 0, 0)},
    {nullptr, std::string("vec_u32"), std::string("Specifies a std::vector<uint32_t>."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VECTOR(S_AppParam_X.VecU32Collection, UINT32, 0, 0)},
    {nullptr, std::string("u8"), std::string("Specifies a u8."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Val_U8, UINT8, 0, 0)},
    {nullptr, std::string("u16"), std::string("Specifies a u16."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Val_U16, UINT16, 0, 0)},
    {nullptr, std::string("u32"), std::string("Specifies a u32."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Val_U32, UINT32, 0, 0)},
    {nullptr, std::string("u64"), std::string("Specifies a u64."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Val_U64, UINT64, 0, 0)},
    {nullptr, std::string("s8"), std::string("Specifies a s8."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Val_S8, INT8, 0, 0)},
    {nullptr, std::string("s16"), std::string("Specifies a s16."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Val_S16, INT16, 0, 0)},
    {nullptr, std::string("s32"), std::string("Specifies a s32."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Val_S32, INT32, 0, 0)},
    {nullptr, std::string("s64"), std::string("Specifies a s64."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Val_S64, INT64, 0, 0)},
    {nullptr, std::string("ff"), std::string("Specifies a float."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Val_f, FLOAT, 0, 0)},
    {nullptr, std::string("dd"), std::string("Specifies a double."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Val_lf, DOUBLE, 0, 0)},

    {nullptr, std::string("g"), std::string("Selects 'GENERATE' working mode."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.WorkingMode_E, UINT32, static_cast<uint32_t>(WORKING_MODE::GENERATE), 0)},
    {nullptr, std::string("m"), std::string("Selects 'MAKE' working mode."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.WorkingMode_E, UINT32, static_cast<uint32_t>(WORKING_MODE::MAKE), 0)},

    {nullptr, std::string("sln"), std::string("Specifies the full path to a visual studio solution project file."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG,
     BOF_PARAM_DEF_VARIABLE(S_AppParam_X.pSlnPath, CHARSTRING, 1, sizeof(S_AppParam_X.pSlnPath) - 1)},
    {nullptr, std::string("double"), std::string("Specifies a double value."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Value_lf, DOUBLE, -1.3f, 7.0f)},
    {nullptr, std::string("uint32"), std::string("Specifies an u32 value."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Value_U32, UINT32, 5, 11111122)},
    {nullptr, std::string("i4"), std::string("Specifies IpV4 address."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.IpV4_X, IPV4, 0, 0)},
    {nullptr, std::string("i6"), std::string("Specifies IpV6 address."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.IpV6_X, IPV6, 0, 0)},
    {nullptr, std::string("uri1"), std::string("Specifies an uri with aut."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Uri1, URI, 0, 0)},
    {nullptr, std::string("uri2"), std::string("Specifies an uri without aut."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Uri2, URI, 0, 0)},

    {nullptr, std::string("a32"), std::string("Pure uint32_t array."), std::string("bha/0x%08X/"), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_ARRAY(S_AppParam_X.pVal_U32, UINT32, 0, 0)},
    {nullptr, std::string("ac256"), std::string("Pure array of char array."), std::string(">%-s<"), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG,
     BOF_PARAM_DEF_ARRAY(S_AppParam_X.pVal_c, CHARSTRING, 1, sizeof(S_AppParam_X.pVal_c[0]) - 1)},
    {nullptr, std::string("asf0"), std::string("Pure array of struct: field 0."), std::string("Val=0x%08X"), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG,
     BOF_PARAM_DEF_ARRAY_OF_STRUCT(ARRAYENTRY, S_AppParam_X.pArray_X, c_U32, UINT32, 0, 0)},
    {nullptr, std::string("asf1"), std::string("Pure array of struct: field 1."), std::string("V=0x%02X"), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG,
     BOF_PARAM_DEF_ARRAY_OF_STRUCT(ARRAYENTRY, S_AppParam_X.pArray_X, a_U8, UINT8, 0, 0)},
    {nullptr, std::string("asf2"), std::string("Pure array of struct: field 2."), std::string("Va=0x%04X"), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG,
     BOF_PARAM_DEF_ARRAY_OF_STRUCT(ARRAYENTRY, S_AppParam_X.pArray_X, b_U16, UINT16, 0, 0)},
    {nullptr, std::string("asf3"), std::string("Pure array of struct: field 3."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG,
     BOF_PARAM_DEF_ARRAY_OF_STRUCT(ARRAYENTRY, S_AppParam_X.pArray_X, pTxt_c, CHARSTRING, 1, sizeof(S_AppParam_X.pArray_X[0].pTxt_c) - 1)},
    {nullptr, std::string("asf4"), std::string("Pure array of struct: field 4."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG | BOFPARAMETER_ARG_FLAG::IP_FORMAT_SCHEME | BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT,
     BOF_PARAM_DEF_ARRAY_OF_STRUCT(ARRAYENTRY, S_AppParam_X.pArray_X, IpV4_X, IPV4, 0, 0)},

    {nullptr, std::string("str"), std::string("Specifies a std::string."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Str_S, STDSTRING, 0, 0)},

    {nullptr, std::string("date"), std::string("Specifies a date."), std::string("%d/%m/%Y"), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Date, DATE, 0, 0)},
    {nullptr, std::string("time"), std::string("Specifies a time."), std::string("%H:%M:%S"), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Time, TIME, 0, 0)},
    {nullptr, std::string("datetime"), std::string("Specifies a datetime."), std::string("%d/%m/%Y %H:%M:%S"), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.DateTime, DATETIME, 0, 0)},
    {nullptr, std::string("path"), std::string("Specifies a path."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Path, PATH, 0, 0)},

    {nullptr, std::string("vs"), std::string("Specifies a video standard."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Vs, VIDEOSTANDARD, 0, 0)},
    {nullptr, std::string("as"), std::string("Specifies an audio standard."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.As, AUDIOSTANDARD, 0, 0)},
    {nullptr, std::string("tc"), std::string("Specifies a timecode."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Tc, TC, 0, 0)},
    {nullptr, std::string("sz"), std::string("Specifies a size."), std::string(""), std::string(""), BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AppParam_X.Size_X, SIZE2D, 0, 0)},

    // Option
    {nullptr, "A_BASEFN", "if defined, audio buffer will be saved in this file", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AudioOption_X.BasePath, PATH, 0, 0)},
    {nullptr, "A_NBCHNL", "Specifies the number of audio channel to generate", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AudioOption_X.NbChannel_U32, UINT32, 0, 4096)},
    {nullptr, "A_LAYOUT", "Specifies the channel layout to generate", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AudioOption_X.ChannelLayout_U64, UINT64, 0, 0)},
    {nullptr, "A_RATE", "Specifies the audio sample rate to generate", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(S_AudioOption_X.SampleRateInHz_U32, UINT32, 0, 128000)},
    {nullptr, "A_FMT", "Specifies the audio format", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG,
     BOF_PARAM_DEF_ENUM(S_AudioOption_X.Format_E, BOF2D_AUDIO_FORMAT::BOF2D_AUDIO_FORMAT_PCM, BOF2D_AUDIO_FORMAT::BOF2D_AUDIO_FORMAT_MAX, S_Bof2dAudioFormatEnumConverter, BOF2D_AUDIO_FORMAT)},
};

/*** Factory functions called at the beginning/end of each test case **********/

void CmdLineParser_Test::SetUpTestCase()
{
}

void CmdLineParser_Test::TearDownTestCase()
{
}

/*** Factory functions called at the beginning/end of each test *****************/

void CmdLineParser_Test::SetUp()
{
}

void CmdLineParser_Test::TearDown()
{
}

TEST_F(CmdLineParser_Test, Option)
{
  BofCommandLineParser BofCommandLineParser_O;

  for (int i = 0; i < 10; i++)
  {
    S_AudioOption_X.Reset();
    EXPECT_EQ(BofCommandLineParser_O.ToByte("--A_BASEFN=AudioOut;--A_NBCHNL=2;--A_LAYOUT=3;--A_RATE=48000;--A_FMT=WAV", S_pCommandLineOption_X, CmdLineParseResultUltimateCheck, CmdLineParseError), BOF_ERR_NO_ERROR);

    EXPECT_TRUE(S_AudioOption_X.BasePath.IsValid());
    EXPECT_EQ(S_AudioOption_X.ChannelLayout_U64, 3);
    EXPECT_EQ(S_AudioOption_X.Format_E, BOF2D_AUDIO_FORMAT::BOF2D_AUDIO_FORMAT_WAV);
    EXPECT_EQ(S_AudioOption_X.NbChannel_U32, 2);
    EXPECT_EQ(S_AudioOption_X.SampleRateInHz_U32, 48000);
  }
}

TEST_F(CmdLineParser_Test, CmdLine)
{
  int Argc_i, Sts_i;
  char ppArgument_c[128][128], *pArgv_c[128];
  std::string HelpString_S, Ip_S, Uri_S;
  BofCommandLineParser *pBofCommandLineParser_O;

  Argc_i = 0;

  strcpy(ppArgument_c[Argc_i], "MyTestApp.exe");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  // strcpy(ppArgument_c[Argc_i], "--i4=udp://john.doe:password@192.168.1.2:23");
  // strcpy(ppArgument_c[Argc_i], "--i4=192.168.1.2:23");
  // strcpy(ppArgument_c[Argc_i], "--u64=22");
  // pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  // Argc_i++;
  // goto l;

  strcpy(ppArgument_c[Argc_i], "-h");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--help");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  // const regex pattern("\\s?(0|91)?[6-9][0-9]{9}"); is regular expression used to validate mobile number.
  //  0) Trim white space char 1) Begins with 0 or 91    2) Then contains 6,7 or 8 or 9.   3) Then contains 9 digits
  // The following input contains good and bad mobile number
  strcpy(ppArgument_c[Argc_i], "--vec_regexp=  06123456789, 26123456789, 917123456789, 91712345678, 918123456789");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--vec_u32=  1, 2,3 , 100 , 101");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--u8=1");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--u16=2");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--u32=3");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--u64=4");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--s8=-1");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--s16=-2");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--s32=-3");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--s64=-4");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--ff=-3.14159");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--dd=-1.2345678");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "-g");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "-m");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--sln=\"sln file\"");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--double=1.23456789");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--uint32=1234");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--i6=tcp://[0004:db8:0::1]:80");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--i4=udp://john.doe:password@192.168.1.2:23");
  // strcpy(ppArgument_c[Argc_i], "--i4=192.168.1.2:23");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--uri1=myprotocol:/forum/questions/file.txt?justkey&order=newest&tag=networking#top");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--uri2=myprotocol://john.doe:password@1.2.3.4:123/forum/questions/file.txt?justkey&order=newest&tag=networking#top");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--a32=0xABCDEF,1,2,0xDEADBEEF,44,55,,");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--ac256=a z e r t y i o p");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--asf0=0x1234ABCD");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--asf1=128");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--asf2=40000");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--asf3=hello world,hello world at 1,at 2,at 3,");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;
  strcpy(ppArgument_c[Argc_i], "--asf4=1.2.3.4,5.6.7.8:9,udp://10.20.30.40:50");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--str=i'm a std string");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--date=1/2/2003");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--time=12:34:56");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--datetime=12/11/2010 13:14:15");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--path=C:/tmp/babar/file.txt");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--vs=1920x1080@59.94i"); // Or 1920x1080_59i");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--as=16xS24L32@48000");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--tc=1977-07-16 01:02:03:04  @1001/60");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  strcpy(ppArgument_c[Argc_i], "--sz=123x456");
  pArgv_c[Argc_i] = ppArgument_c[Argc_i];
  Argc_i++;

  // l:
  pBofCommandLineParser_O = new BofCommandLineParser();

  Sts_i = pBofCommandLineParser_O->ToByte(Argc_i, pArgv_c, S_pCommandLineOption_X, CmdLineParseResultUltimateCheck, CmdLineParseError); // Call Parse()
  EXPECT_EQ(Sts_i, 0);

  if (S_AppParam_X.AskHelp_B)
  {
    pBofCommandLineParser_O->BuildHelpString(S_pCommandLineOption_X, std::string(ppArgument_c[0]) + "\n", HelpString_S);
    // printf("%s", HelpString_S.c_str());
  }
  else
  {
    // printf("No help required.\n");
  }

  // goto m;
  // const regex pattern("(0|91)?[6-9][0-9]{9}"); is regular expression used to validate mobile number.
  // 1) Begins with 0 or 91    2) Then contains 6,7 or 8 or 9.   3) Then contains 9 digits
  // The following input contains good and bad mobile number
  // strcpy(ppArgument_c[Argc_i], "--vec_regexp=  06123456789, 26123456789, 917123456789, 91712345678, 918123456789");

  EXPECT_EQ(S_AppParam_X.VecStringRegExpCollection.size(), 3);
  EXPECT_STREQ(S_AppParam_X.VecStringRegExpCollection[0].c_str(), "  06123456789");
  EXPECT_STREQ(S_AppParam_X.VecStringRegExpCollection[1].c_str(), " 917123456789");
  EXPECT_STREQ(S_AppParam_X.VecStringRegExpCollection[2].c_str(), " 918123456789");

  EXPECT_EQ(S_AppParam_X.VecU32Collection[0], 1);
  EXPECT_EQ(S_AppParam_X.VecU32Collection[1], 2);
  EXPECT_EQ(S_AppParam_X.VecU32Collection[2], 3);
  EXPECT_EQ(S_AppParam_X.VecU32Collection[3], 100);
  EXPECT_EQ(S_AppParam_X.VecU32Collection[4], 101);

  EXPECT_EQ(S_AppParam_X.Val_U8, 1);
  EXPECT_EQ(S_AppParam_X.Val_U16, 2);
  EXPECT_EQ(S_AppParam_X.Val_U32, 3);
  EXPECT_EQ(S_AppParam_X.Val_U64, 4);

  EXPECT_EQ(S_AppParam_X.Val_S8, -1);
  EXPECT_EQ(S_AppParam_X.Val_S16, -2);
  EXPECT_EQ(S_AppParam_X.Val_S32, -3);
  EXPECT_EQ(S_AppParam_X.Val_S64, -4);

  EXPECT_NEAR(S_AppParam_X.Val_f, -3.14159, 0.0001);
  EXPECT_NEAR(S_AppParam_X.Val_lf, -1.2345678, 0.0001);

  EXPECT_EQ(S_AppParam_X.pVal_U32[0], 0xABCDEF); // 0xABCDEF);
  EXPECT_EQ(S_AppParam_X.pVal_U32[1], 1);
  EXPECT_EQ(S_AppParam_X.pVal_U32[2], 2);
  EXPECT_EQ(S_AppParam_X.pVal_U32[3], 0xDEADBEEF);
  EXPECT_EQ(S_AppParam_X.pVal_U32[4], 44);
  EXPECT_EQ(S_AppParam_X.pVal_U32[5], 55);
  EXPECT_EQ(S_AppParam_X.pVal_U32[6], 0);
  EXPECT_EQ(S_AppParam_X.pVal_U32[7], 0);

  EXPECT_EQ(S_AppParam_X.DateTime.Day(), 12);
  EXPECT_EQ(S_AppParam_X.DateTime.Month(), 11);
  EXPECT_EQ(S_AppParam_X.DateTime.Year(), 2010);
  EXPECT_EQ(S_AppParam_X.DateTime.Hour(), 13);
  EXPECT_EQ(S_AppParam_X.DateTime.Minute(), 14);
  EXPECT_EQ(S_AppParam_X.DateTime.Second(), 15);
  EXPECT_EQ(S_AppParam_X.DateTime.MicroSecond(), 0);

  EXPECT_EQ(S_AppParam_X.Date.Day(), 1);
  EXPECT_EQ(S_AppParam_X.Date.Month(), 2);
  EXPECT_EQ(S_AppParam_X.Date.Year(), 2003);

  EXPECT_EQ(S_AppParam_X.Time.Hour(), 12);
  EXPECT_EQ(S_AppParam_X.Time.Minute(), 34);
  EXPECT_EQ(S_AppParam_X.Time.Second(), 56);
  EXPECT_EQ(S_AppParam_X.Time.MicroSecond(), 0);

  Ip_S = Bof_SocketAddressToString(S_AppParam_X.IpV4_X.Ip_X, true, true);
  EXPECT_STREQ(Ip_S.c_str(), "udp://192.168.1.2:23");

  Ip_S = S_AppParam_X.IpV4_X.ToString(true, true, true, true);
  EXPECT_STREQ(Ip_S.c_str(), "udp://john.doe:password@192.168.1.2:23");

  Ip_S = Bof_SocketAddressToString(S_AppParam_X.IpV6_X.Ip_X, true, true);
  EXPECT_STREQ(Ip_S.c_str(), "tcp://[4:db8::1]:80");

  EXPECT_STREQ(S_AppParam_X.pSlnPath, "\"sln file\"");
  EXPECT_STREQ(S_AppParam_X.Str_S.c_str(), "i'm a std string");
  EXPECT_EQ(S_AppParam_X.WorkingMode_E, WORKING_MODE::MAKE);
  EXPECT_EQ(S_AppParam_X.Value_lf, 1.23456789);
  EXPECT_EQ(S_AppParam_X.Value_U32, 1234);
  EXPECT_EQ(S_AppParam_X.pVal_U32[0], 0xABCDEF);
  EXPECT_EQ(S_AppParam_X.pArray_X[0].a_U8, 128);
  EXPECT_EQ(S_AppParam_X.pArray_X[0].b_U16, 40000);
  EXPECT_EQ(S_AppParam_X.pArray_X[0].c_U32, 0x1234ABCD);
  EXPECT_STREQ(S_AppParam_X.pArray_X[0].pTxt_c, "hello world");
  EXPECT_STREQ(S_AppParam_X.pVal_c[0], "a z e r t y i o p");
  EXPECT_STREQ(S_AppParam_X.Path.FullPathName(false).c_str(), "C:/tmp/babar/file.txt");

  EXPECT_EQ(S_AppParam_X.pVal_U32[3], 0xDEADBEEF);
  EXPECT_STREQ(S_AppParam_X.pArray_X[0].pTxt_c, "hello world");
  EXPECT_STREQ(S_AppParam_X.pArray_X[1].pTxt_c, "hello world at 1");
  EXPECT_STREQ(S_AppParam_X.pArray_X[2].pTxt_c, "at 2");
  EXPECT_STREQ(S_AppParam_X.pArray_X[3].pTxt_c, "at 3");
  EXPECT_STREQ(S_AppParam_X.pArray_X[4].pTxt_c, "");
  // m:
  Ip_S = Bof_SocketAddressToString(S_AppParam_X.pArray_X[0].IpV4_X.Ip_X, true, true);
  EXPECT_STREQ(Ip_S.c_str(), "???://1.2.3.4:0");
  Ip_S = Bof_SocketAddressToString(S_AppParam_X.pArray_X[1].IpV4_X.Ip_X, true, true);
  EXPECT_STREQ(Ip_S.c_str(), "???://5.6.7.8:9");
  Ip_S = Bof_SocketAddressToString(S_AppParam_X.pArray_X[2].IpV4_X.Ip_X, true, true);
  EXPECT_STREQ(Ip_S.c_str(), "udp://10.20.30.40:50");

  Uri_S = S_AppParam_X.Uri1.ToString();
  EXPECT_STREQ(Uri_S.c_str(), "myprotocol:/forum/questions/file.txt?justkey&order=newest&tag=networking#top");
  Uri_S = S_AppParam_X.Uri2.ToString();
  EXPECT_STREQ(Uri_S.c_str(), "myprotocol://john.doe:password@1.2.3.4:123/forum/questions/file.txt?justkey&order=newest&tag=networking#top");

  EXPECT_STREQ(S_AppParam_X.Vs.ToString().c_str(), "1920x1080@59.94i");
  EXPECT_STREQ(S_AppParam_X.As.ToString().c_str(), "16xS24L32@48000");
  EXPECT_STREQ(S_AppParam_X.Tc.ToString(true).c_str(), "1977-07-16 01:02:03:04  @1001/60");
  EXPECT_EQ(S_AppParam_X.Size_X.Width, 123);
  EXPECT_EQ(S_AppParam_X.Size_X.Height, 456);

  /*

  char                pVal_c[7][256];
  */
  pBofCommandLineParser_O->BuildHelpString(S_pCommandLineOption_X, ppArgument_c[0], HelpString_S);
  // printf("%s", HelpString_S.c_str());

  BOF_SAFE_DELETE(pBofCommandLineParser_O);
}