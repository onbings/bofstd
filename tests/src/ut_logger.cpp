/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the boflogger class
 *
 * Name:        ut_logger.cpp
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
#include "bofstd/boffs.h"
#include "bofstd/bofsocket.h"
#include <bofstd/bofdatetime.h>
#include <bofstd/boflogchannel_spdlog.h>
#include <bofstd/boflogger.h>

#include "gtestrunner.h"

// To use a test fixture, derive from testing::Test class
class Logger_Test : public testing::Test
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

/*** Factory functions called at the beginning/end of each test case **********/

void Logger_Test::SetUpTestCase()
{
}

void Logger_Test::TearDownTestCase()
{
}

/*** Factory functions called at the beginning/end of each test *****************/

void Logger_Test::SetUp()
{
}

void Logger_Test::TearDown()
{
}

USE_BOF_NAMESPACE()

const std::string LOGGER_NAME = "rBofLog";

#if defined(_WIN32)
const std::string FILELOGDIR = "C:/tmp/log/";
#else
const std::string FILELOGDIR = "/tmp/log/";
#endif
const std::string LOGHEADER = "%N%d/%m/%C %H:%M:%S:%e %L %n %^%v%$";
const uint32_t MAXLOGSIZEINBYTE = 0x1000;
const uint32_t MAXNUMBEROFLOGFILE = 3;
const uint32_t MAXNUMBEROFLOGGERQUEUEENTRIES = 0x100;

const uint32_t LOGLOOPMAX = MAXLOGSIZEINBYTE;

/*
   // Rotate files:
   // log.txt -> log.1.txt
   // log.1.txt -> log2.txt
   // log.2.txt -> log3.txt
   // log.3.txt -> delete
 */

void OnError(const std::string &_rErr_S)
{
  //printf("OnError %s%s", _rErr_S.c_str(), Bof_Eol());
}
const char *OnErrorCodeToString(uint32_t _ErrorCode_U32)
{
  return Bof_ErrorCode(static_cast<BOFERR>(_ErrorCode_U32));
}

static std::vector<BOF_LOG_CHANNEL_PARAM> S_LogChannelList = {
    {"LogChannel1", FILELOGDIR + "unlimited.log", LOGHEADER, BOF_LOG_CHANNEL_LEVEL::DBG, BOF_LOG_CHANNEL_SINK::TO_FILE, BOF_LOG_CHANNEL_FLAG::DELETE_PREVIOUS_LOGFILE, BOF_LOGGER_OVERFLOW_POLICY::DISCARD, 0, 0, 0},
    {"LogChannel2", FILELOGDIR + "limited.log", LOGHEADER, BOF_LOG_CHANNEL_LEVEL::WARNING, BOF_LOG_CHANNEL_SINK::TO_FILE, BOF_LOG_CHANNEL_FLAG::DELETE_PREVIOUS_LOGFILE, BOF_LOGGER_OVERFLOW_POLICY::DISCARD, MAXLOGSIZEINBYTE, 0, 0},
    {"LogChannel3", FILELOGDIR + "rotating.log", LOGHEADER, BOF_LOG_CHANNEL_LEVEL::CRITICAL, BOF_LOG_CHANNEL_SINK::TO_FILE, BOF_LOG_CHANNEL_FLAG::DELETE_PREVIOUS_LOGFILE, BOF_LOGGER_OVERFLOW_POLICY::DISCARD, MAXLOGSIZEINBYTE, MAXNUMBEROFLOGFILE, 0},
    {"LogChannel4", FILELOGDIR + "daily.log", LOGHEADER, BOF_LOG_CHANNEL_LEVEL::CRITICAL, BOF_LOG_CHANNEL_SINK::TO_DAILYFILE, BOF_LOG_CHANNEL_FLAG::DELETE_PREVIOUS_LOGFILE, BOF_LOGGER_OVERFLOW_POLICY::DISCARD, 0xFFFFFFFF, 0, 60},
    {"LogChannel5", "", LOGHEADER, BOF_LOG_CHANNEL_LEVEL::DBG, BOF_LOG_CHANNEL_SINK::TO_RAM_CIRCULAR_BUFFER, BOF_LOG_CHANNEL_FLAG::NONE, BOF_LOGGER_OVERFLOW_POLICY::OVERWRITE, MAXLOGSIZEINBYTE, 0, 0},
    {"LogChannel6", "", LOGHEADER, BOF_LOG_CHANNEL_LEVEL::DBG, BOF_LOG_CHANNEL_SINK::TO_STDERR, BOF_LOG_CHANNEL_FLAG::NONE, BOF_LOGGER_OVERFLOW_POLICY::DISCARD, 0, 0, 0},
    {"LogChannel7", "", LOGHEADER, BOF_LOG_CHANNEL_LEVEL::DBG, BOF_LOG_CHANNEL_SINK::TO_STDOUT_COLOR, BOF_LOG_CHANNEL_FLAG::NONE, BOF_LOGGER_OVERFLOW_POLICY::DISCARD, 0, 0, 0},
};

uint32_t FillLog(uint32_t _Offset_U32, uint32_t _MaxLoop_U32, std::vector<BOF_LOG_CHANNEL_PARAM> _LogChannelList)
{
  BOFERR Sts_E;
  BofLogger &rBofLog = BofLogger::S_Instance();
  ;
  BOF_LOG_CHANNEL_PARAM BofLogParam_X;
  uint32_t Rts_U32, i_U32, j_U32, Nb_U32, Index_U32, Start_U32, Delta_U32, TotalLine_U32;
  std::vector<std::string> ListOfLogLine_S;
  std::shared_ptr<IBofLogChannel> psLogChannel;
  std::shared_ptr<IBofLogChannel> psExtractLogChannel;
  std::string ChannelName_S;

  std::string Val_S;
  char pVal_c[1024];
  float Val_f;
  double Val_lf;
  uint8_t Val_U8;
  uint16_t Val_U16;
  uint32_t Val_U32;
  uint64_t Val_U64;

  Rts_U32 = 0;
  for (i_U32 = 0; i_U32 < _LogChannelList.size(); i_U32++)
  {
    ChannelName_S = _LogChannelList[i_U32].ChannelName_S;
    psLogChannel = BofLogger::S_Instance().LogChannelInterface(ChannelName_S);

    Sts_E = rBofLog.Level(ChannelName_S, BOF_LOG_CHANNEL_LEVEL::TRACE);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

    Rts_U32++;
    BOF_LOGGER_CRITICAL(ChannelName_S, "AAA", 0);

    Sts_E = rBofLog.Flush(ChannelName_S);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

    for (j_U32 = 0; j_U32 < MAXNUMBEROFLOGGERQUEUEENTRIES; j_U32++)
    {
      Rts_U32++;
      Sts_E = psLogChannel->V_Log(BOF_LOG_CHANNEL_LEVEL::CRITICAL, Bof_Sprintf("Fast Line %d", j_U32));
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      Rts_U32++;
      Sts_E = rBofLog.Log("Log_Channel_0", BOF_LOG_CHANNEL_LEVEL::CRITICAL, "Slow Line %d", j_U32);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    }
    Sts_E = psLogChannel->V_Flush();
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

    psExtractLogChannel = BofLogger::S_Instance().LogChannelInterface(ChannelName_S);
    EXPECT_TRUE(psExtractLogChannel != nullptr);
    Sts_E = psExtractLogChannel->V_OpenLogStorage(0);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    Nb_U32 = 0;
    ListOfLogLine_S.clear();
    do
    {
      Index_U32 = static_cast<uint32_t>(ListOfLogLine_S.size());
      Sts_E = psExtractLogChannel->V_ReadLogStorage(10, ListOfLogLine_S);
      if (Sts_E == BOF_ERR_NO_ERROR)
      {
        Nb_U32 += 10;
      }
      else
      {
        Nb_U32 += static_cast<uint32_t>(ListOfLogLine_S.size()) - Index_U32;
      }
    } while (Sts_E == BOF_ERR_NO_ERROR);

    Index_U32 = 0;
    EXPECT_EQ(ListOfLogLine_S.size(), _Offset_U32 + 1 + (MAXNUMBEROFLOGGERQUEUEENTRIES * 2));
    EXPECT_EQ(Nb_U32, _Offset_U32 + 1 + (MAXNUMBEROFLOGGERQUEUEENTRIES * 2));
    Val_S = ListOfLogLine_S[Index_U32++].substr(21);
    EXPECT_STREQ(Bof_Sprintf(" C %s: %08X AAA%s", ChannelName_S.c_str(), Index_U32, Bof_Eol()).c_str(), Val_S.c_str());

    for (j_U32 = 0; j_U32 < MAXNUMBEROFLOGGERQUEUEENTRIES; j_U32++)
    {
      Val_S = ListOfLogLine_S[Index_U32++].substr(21);
      EXPECT_STREQ(Bof_Sprintf(" C %s: %08X Fast Line %d%s", ChannelName_S.c_str(), Index_U32, j_U32, Bof_Eol()).c_str(), Val_S.c_str());
      Val_S = ListOfLogLine_S[Index_U32++].substr(21);
      EXPECT_STREQ(Bof_Sprintf(" C %s: %08X Slow Line %d%s", ChannelName_S.c_str(), Index_U32, j_U32, Bof_Eol()).c_str(), Val_S.c_str());
    }
    Sts_E = psExtractLogChannel->V_CloseLogStorage();
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

    Start_U32 = Bof_GetMsTickCount();
    TotalLine_U32 = 0;
    for (j_U32 = 0; j_U32 < _MaxLoop_U32; j_U32++)
    {
      Val_S = Bof_Random(false, 64, 'A', 'z');
      strcpy(pVal_c, Val_S.c_str());
      Val_S = Bof_Random(false, 64, 'A', 'z');
      Val_f = static_cast<float>(Bof_Random(false, -1000000, 1000000)) / 0.12345678f;
      Val_lf = static_cast<double>(Bof_Random(false, -5000000, 5000000)) / 0.12345678f;
      Val_U8 = static_cast<uint8_t>(Bof_Random(false, 0x00, 0xFF));
      Val_U16 = static_cast<uint16_t>(Bof_Random(false, 0x0000, 0xFFFF));
      Val_U32 = Bof_Random(false, 0x00000000, 0xFFFFFFFF);
      Val_U64 = Bof_Random(false, 0x00000000, 0xFFFFFFFF);
      Sts_E = BofLogger::S_Instance().Log(ChannelName_S, BOF_LOG_CHANNEL_LEVEL::TRACE, "BofLogger1 %s %s %f %lf %d %d %d %zd", pVal_c, Val_S.c_str(), Val_f, Val_lf, Val_U8, Val_U16, Val_U32, Val_U64);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      TotalLine_U32++;
    }
    Rts_U32 += TotalLine_U32;
    Delta_U32 = Bof_ElapsedMsTime(Start_U32);
    //printf("IO %d line in %d ms->%d line/sec%s", j_U32, Delta_U32, Delta_U32 ? TotalLine_U32 * 1000 / Delta_U32 : 0, Bof_Eol());
    Delta_U32 = Bof_ElapsedMsTime(Start_U32);
    Sts_E = psLogChannel->V_Flush();
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    //printf("FLUSH %d line in %d ms->%d line/sec%s", j_U32, Delta_U32, Delta_U32 ? TotalLine_U32 * 1000 / Delta_U32 : 0, Bof_Eol());
  }
  return Rts_U32;
}

const char *TEST_MAINLOGCHANNEL = "TestLog";
const char *TEST_LINUXSYSLOG = "TestSyslog";
enum DBG_FLAG_MASK : uint32_t
{
  DBG_INIT = 0x00000001,       /*! Init */
  DBG_INFO = 0x00000002,       /*! Info */
  BOF_DBG_ALWAYS = 0x80000000, /*! Always display */
  BOF_DBG_ERROR = 0x40000000   /*! Display error */
};

TEST_F(Logger_Test, LoggerInit)
{
  uint32_t i_U32, Mask_U32;
  std::vector<std::string> LogMaskNames_S{/*0*/ "INIT", "INFO", "", "", "", "", "", "", /*8*/ "", "", "", "", "", "", "", "", /*16*/ "", "", "", "", "", "", "", "", /*24*/ "", "", "", "", "", "", "ALWAYS", "ERROR"};
  BOFERR Sts_E;
  BOF_LOG_LEVEL_COLOR LogLevelColor_E;

  BofLogger &rBofLog = BofLogger::S_Instance();
  rBofLog.InitializeLogger({"SyncLogger", 0, 0, false, BOF_LOGGER_OVERFLOW_POLICY::WAIT, OnError, OnErrorCodeToString});
  rBofLog.AddLogChannel(std::make_shared<BofLogChannelSpdLog>(), {TEST_MAINLOGCHANNEL, "", LOGHEADER, BOF_LOG_CHANNEL_LEVEL::TRACE, BOF_LOG_CHANNEL_SINK::TO_STDOUT_COLOR, BOF_LOG_CHANNEL_FLAG::NONE, BOF_LOGGER_OVERFLOW_POLICY::DISCARD, 0, 0, 0});

  // rBofLog.LogMask(IPSWITCHER_MAINLOGCHANNEL, 0xFFFFFFFF ^ (DBG_FCT_ENTRY | DBG_FCT_EXIT | DBG_INFO));
  for (Mask_U32 = 1, i_U32 = 0; i_U32 < LogMaskNames_S.size(); i_U32++, Mask_U32 <<= 1)
  {
    rBofLog.LogMaskName(TEST_MAINLOGCHANNEL, Mask_U32, LogMaskNames_S[i_U32]);
  }

  BOF_LOG(INFORMATION, TEST_MAINLOGCHANNEL, "bha", 0);
  BOF_LOG(INFORMATION, TEST_MAINLOGCHANNEL, " is ", 0);
  BOF_LOG(INFORMATION, TEST_MAINLOGCHANNEL, "great\n", 0);
  BOF_LOG(INFORMATION, TEST_MAINLOGCHANNEL, "!\n", 0);

  BOF_LOG_EXTENDED(INFORMATION, TEST_MAINLOGCHANNEL, DBG_INIT, 0, "bha", 0);
  BOF_LOG_EXTENDED(INFORMATION, TEST_MAINLOGCHANNEL, DBG_INIT, 0, " is ", 0);
  BOF_LOG_EXTENDED(INFORMATION, TEST_MAINLOGCHANNEL, DBG_INIT, 0, "great\n", 0);
  BOF_LOG_EXTENDED(INFORMATION, TEST_MAINLOGCHANNEL, DBG_INIT, 0, "!\n", 0);

  LogLevelColor_E = static_cast<BOF_LOG_LEVEL_COLOR>(BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_GREEN | BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_RED | BOF_LOG_LEVEL_COLOR::LOG_COLOR_BOLD);
  Sts_E = rBofLog.LogLevelColor(TEST_MAINLOGCHANNEL, BOF_LOG_CHANNEL_LEVEL::TRACE, LogLevelColor_E);
  LogLevelColor_E = static_cast<BOF_LOG_LEVEL_COLOR>(BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_RED | BOF_LOG_LEVEL_COLOR::LOG_COLOR_BOLD);
  Sts_E = rBofLog.LogLevelColor(TEST_MAINLOGCHANNEL, BOF_LOG_CHANNEL_LEVEL::DBG, LogLevelColor_E);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  BOF_LOGGER_INFORMATION(TEST_MAINLOGCHANNEL, "Information from Hello World", 0);
  BOF_LOGGER_TRACE(TEST_MAINLOGCHANNEL, "Hello World", 0);
  BOF_LOGGER_EXTENDED_DBG(TEST_MAINLOGCHANNEL, 1, 3, "Mask=0x%08X", Mask_U32);
  rBofLog.Flush(TEST_MAINLOGCHANNEL);
  rBofLog.ShutdownLogger();

#if defined(__linux__)
  rBofLog.AddLogChannel(std::make_shared<BofLogChannelSpdLog>(), {TEST_LINUXSYSLOG, "", LOGHEADER, true, BOF_LOG_CHANNEL_LEVEL::TRACE, BOF_LOG_CHANNEL_SINK::TO_LINUX_SYSLOG, BOF_LOG_CHANNEL_FLAG::NONE, BOF_LOGGER_OVERFLOW_POLICY::DISCARD, 0, 0, 0});
  BOF_LOGGER_TRACE(TEST_LINUXSYSLOG, "Hello sysLog World", 0);
  rBofLog.Flush(TEST_LINUXSYSLOG);
#endif
}

#if 0
TEST_F(Logger_Test, LoggerFile)
{
  BOFERR                Sts_E;
  BofLogger &rBofLog = BofLogger::S_Instance();;
  std::vector < BOF_LOG_CHANNEL_PARAM > LogChannelList;
  BOF_LOG_CHANNEL_PARAM BofLogParam_X;
  uint32_t              i_U32, Nb_U32;
  std::vector<std::string> ListOfLogLine_S;
  std::shared_ptr<IBofLogChannel> psLogChannel;
  std::shared_ptr<IBofLogChannel> psExtractLogChannel;


  BOF_LOG_CHANNEL_PARAM LogChannelParam_X;

  i_U32 = 0;
  LogChannelParam_X.ChannelName_S = "Log_Channel_" + std::to_string(i_U32);
  LogChannelParam_X.FileLogPath = FILELOGDIR + LogChannelParam_X.ChannelName_S + ".log";
  LogChannelParam_X.LogLevel_E = BOF_LOG_CHANNEL_LEVEL::INFORMATION;
  LogChannelParam_X.LogSink_E = BOF_LOG_CHANNEL_SINK::TO_FILE;
  LogChannelParam_X.LogFlag_E = BOF_LOG_CHANNEL_FLAG::DELETE_PREVIOUS_LOGFILE;
  LogChannelParam_X.MaxLogSizeInByte_U32 = MAXLOGSIZEINBYTE;
  LogChannelParam_X.MaxNumberOfLogFile_U32 = MAXNUMBEROFLOGFILE;
  LogChannelList.push_back(LogChannelParam_X);

  std::shared_ptr<BofLogChannelSpdLog> psSpdLogChannel = std::make_shared<BofLogChannelSpdLog>();

  Sts_E = rBofLog.AddLogChannel(psSpdLogChannel, LogChannelList[i_U32]);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Nb_U32 = FillLog(0, 10, LogChannelList);
  EXPECT_EQ(Nb_U32, 10 + 1 + (MAXNUMBEROFLOGGERQUEUEENTRIES * 2));

  Nb_U32 = FillLog(Nb_U32, 1000, LogChannelList);
  EXPECT_EQ(Nb_U32, 1000 + 1 + (MAXNUMBEROFLOGGERQUEUEENTRIES * 2));

  psSpdLogChannel->V_DeleteLogStorage();
  Nb_U32 = FillLog(0, 10, LogChannelList);
  EXPECT_EQ(Nb_U32, 10 + 1 + (MAXNUMBEROFLOGGERQUEUEENTRIES * 2));
}

TEST_F(Logger_Test, LoggerRam)
{
  BOFERR                Sts_E;
  BofLogger &rBofLog = BofLogger::S_Instance();;
  std::vector < BOF_LOG_CHANNEL_PARAM > LogChannelList;
  BOF_LOG_CHANNEL_PARAM BofLogParam_X;
  uint32_t              i_U32, Nb_U32;
  std::vector<std::string> ListOfLogLine_S;
  std::shared_ptr<IBofLogChannel> psLogChannel;
  std::shared_ptr<IBofLogChannel> psExtractLogChannel;


  BOF_LOG_CHANNEL_PARAM LogChannelParam_X;

  i_U32 = 0;
  LogChannelParam_X.ChannelName_S = "Log_Channel_" + std::to_string(i_U32);
  LogChannelParam_X.FileLogPath = "";
  LogChannelParam_X.LogLevel_E = BOF_LOG_CHANNEL_LEVEL::INFORMATION;
  LogChannelParam_X.LogSink_E = BOF_LOG_CHANNEL_SINK::TO_RAM_CIRCULAR_BUFFER;
  LogChannelParam_X.LogFlag_E = BOF_LOG_CHANNEL_FLAG::NONE;
  LogChannelParam_X.MaxLogSizeInByte_U32 = MAXLOGSIZEINBYTE;
  LogChannelParam_X.MaxNumberOfLogFile_U32 = 0;
  LogChannelList.push_back(LogChannelParam_X);

  std::shared_ptr<BofLogChannelSpdLog> psSpdLogChannel = std::make_shared<BofLogChannelSpdLog>();

  Sts_E = rBofLog.AddLogChannel(psSpdLogChannel, LogChannelList[i_U32]);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Nb_U32 = FillLog(0, 10, LogChannelList);
  EXPECT_EQ(Nb_U32, 10 + 1 + (MAXNUMBEROFLOGGERQUEUEENTRIES * 2));

  Nb_U32 = FillLog(Nb_U32, 1000, LogChannelList);
  EXPECT_EQ(Nb_U32, 1000 + 1 + (MAXNUMBEROFLOGGERQUEUEENTRIES * 2));
}
#endif

#if 0
TEST_F(Logger_Test, LoggerMultiSink)
{
  BOFERR                Sts_E;
  BofLogger &rBofLog = BofLogger::S_Instance();;
  std::vector < BOF_LOG_CHANNEL_PARAM > LogChannelList;
  BOF_LOG_CHANNEL_PARAM BofLogParam_X;
  uint32_t              i_U32, Nb_U32;
  std::vector<std::string> ListOfLogLine_S;
  std::shared_ptr<IBofLogChannel> psLogChannel;
  std::shared_ptr<IBofLogChannel> psExtractLogChannel;

  BOF_LOG_CHANNEL_PARAM LogChannelParam_X;

  i_U32 = 0;
  LogChannelParam_X.ChannelName_S = "Log_Channel_" + std::to_string(i_U32);
  LogChannelParam_X.FileLogPath = FILELOGDIR + LogChannelParam_X.ChannelName_S + ".log";
  LogChannelParam_X.LogLevel_E = BOF_LOG_CHANNEL_LEVEL::INFORMATION;
  LogChannelParam_X.LogFlag_E = BOF_LOG_CHANNEL_FLAG::DELETE_PREVIOUS_LOGFILE;
  LogChannelParam_X.MaxLogSizeInByte_U32 = MAXLOGSIZEINBYTE;
  LogChannelParam_X.MaxNumberOfLogFile_U32 = MAXNUMBEROFLOGFILE;
  LogChannelList.push_back(LogChannelParam_X);

  std::shared_ptr<BofLogChannelSpdLog> psSpdLogChannel = std::make_shared<BofLogChannelSpdLog>();

  Sts_E = rBofLog.AddLogChannel(psSpdLogChannel, LogChannelList[i_U32]);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Nb_U32 = FillLog(0, 10, LogChannelList);
  EXPECT_EQ(Nb_U32, 10 + 1 + (MAXNUMBEROFLOGGERQUEUEENTRIES * 2));

  Nb_U32 = FillLog(Nb_U32, 1000, LogChannelList);
  EXPECT_EQ(Nb_U32, 1000 + 1 + (MAXNUMBEROFLOGGERQUEUEENTRIES * 2));
}
#endif

TEST_F(Logger_Test, LogAlways)
{
  constexpr const char *MFS_LOGGER_MAINCHANNEL = "Mfs";
  uint32_t Mask_U32, i_U32;
  BOF_LOG_LEVEL_COLOR CritLogLevelColor_E, LogLevelColor_E;
  std::string CritLogHeader_S;
  BOF_LOGGER_PARAM LoggerParam_X;
  BofLogger &rBofLog = BOF::BofLogger::S_Instance();
  std::vector<std::string> LogMaskNamesCollection{/*0*/ "INIT", "INFO", "CONNECT", "", "", "", "", "", /*8*/ "",  "", "", "", "", "", "",       "",
                                                  /*16*/ "",    "",     "",        "", "", "", "", "", /*24*/ "", "", "", "", "", "", "ALWAYS", "ERROR"}; // see enum LOG_CHANNEL_FLAG_MASK
  LoggerParam_X.Name_S = MFS_LOGGER_MAINCHANNEL;
  LoggerParam_X.MaxNumberOfAsyncLogQueueEntry_U32 = 0x800;
  LoggerParam_X.AsyncAutoFushIntervalInMs_U32 = 0;
  LoggerParam_X.FastFormat_B = true;
  LoggerParam_X.OverflowPolicy_E = BOF::BOF_LOGGER_OVERFLOW_POLICY::DISCARD;
  LoggerParam_X.OnError = nullptr;
  LoggerParam_X.OnErrorCodeToString = nullptr;
  rBofLog.InitializeLogger(LoggerParam_X);
  rBofLog.AddLogChannel(std::make_shared<BOF::BofLogChannelSpdLog>(),
                        {MFS_LOGGER_MAINCHANNEL, "", "%N%L %^%v%$", BOF::BOF_LOG_CHANNEL_LEVEL::TRACE, BOF::BOF_LOG_CHANNEL_SINK::TO_STDOUT_COLOR, BOF::BOF_LOG_CHANNEL_FLAG::NONE, BOF::BOF_LOGGER_OVERFLOW_POLICY::OVERWRITE, 0, 0, 0});

  rBofLog.LogMask(MFS_LOGGER_MAINCHANNEL, 0xFFFFFFFF ^ (0));
  for (Mask_U32 = 1, i_U32 = 0; i_U32 < LogMaskNamesCollection.size(); i_U32++, Mask_U32 <<= 1)
  {
    rBofLog.LogMaskName(MFS_LOGGER_MAINCHANNEL, Mask_U32, LogMaskNamesCollection[i_U32]);
  }
  // ALWAYS is mapped to OFF spdlog internal level
  CritLogHeader_S = rBofLog.LogHeader(MFS_LOGGER_MAINCHANNEL);
  CritLogLevelColor_E = rBofLog.LogLevelColor(MFS_LOGGER_MAINCHANNEL, BOF::BOF_LOG_CHANNEL_LEVEL::ALWAYS);
  LogLevelColor_E = static_cast<BOF::BOF_LOG_LEVEL_COLOR>(BOF::BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLACK | BOF::BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_WHITE);
  // rBofLog.LogLevelColor(MFS_LOGGER_MAINCHANNEL, BOF::BOF_LOG_CHANNEL_LEVEL::ALWAYS, LogLevelColor_E);
  rBofLog.LogHeader(MFS_LOGGER_MAINCHANNEL, "");

  BOF_LOGGER_RAW_OUTPUT(MFS_LOGGER_MAINCHANNEL, "Storage Info:\n  Version '%s'", "1.2.3.4");
  BOF_LOGGER_RAW_OUTPUT(MFS_LOGGER_MAINCHANNEL, "  Api '%s'", "5.6.7.8");
  rBofLog.Flush(MFS_LOGGER_MAINCHANNEL);
  Bof_MsSleep(100);

  // rBofLog.LogLevelColor(MFS_LOGGER_MAINCHANNEL, BOF::BOF_LOG_CHANNEL_LEVEL::ALWAYS, CritLogLevelColor_E);
  rBofLog.LogHeader(MFS_LOGGER_MAINCHANNEL, CritLogHeader_S);

  BOF_LOGGER_CRITICAL(MFS_LOGGER_MAINCHANNEL, "Storage Info:\n  Version '%s'", "1.2.3.4");
  BOF_LOGGER_CRITICAL(MFS_LOGGER_MAINCHANNEL, "  Api '%s'", "5.6.7.8");

  Bof_MsSleep(100);
}
TEST_F(Logger_Test, LoggerMultiChannel)
{
  BOFERR Sts_E;
  BofLogger &rBofLog = BofLogger::S_Instance();
  ;
  uint32_t i_U32, j_U32, Start_U32, Delta_U32, LogLevelColor_U32;
  std::shared_ptr<IBofLogChannel> psLogChannel;
  BofPath LogPath;
  BOF_LOG_LEVEL_COLOR LogLevelColor_E;

  rBofLog.InitializeLogger({"AsyncLogger", MAXNUMBEROFLOGGERQUEUEENTRIES, 0, false, BOF_LOGGER_OVERFLOW_POLICY::WAIT, OnError, OnErrorCodeToString});
  for (i_U32 = 0; i_U32 < S_LogChannelList.size(); i_U32++)
  {
    std::shared_ptr<BofLogChannelSpdLog> psSpdLogChannel = std::make_shared<BofLogChannelSpdLog>();
    Sts_E = rBofLog.AddLogChannel(psSpdLogChannel, S_LogChannelList[i_U32]);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    Sts_E = psSpdLogChannel->V_LogChannelPathName(LogPath);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    Sts_E = psSpdLogChannel->LogLevel(S_LogChannelList[i_U32].LogLevel_E);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    // No as daily file is different and stdcolor/circularbuffer have non name    EXPECT_STREQ(LogPath.FullPathName(false).c_str(), S_LogChannelList[i_U32].FileLogPath.FullPathName(false).c_str());
  }

  BofPath DailyPath;
  Sts_E = rBofLog.LogChannelPathName(S_LogChannelList[3].ChannelName_S, DailyPath); // This one is dynamic: daily log file name daily_2017-07-23_06-28.log

  j_U32 = 0;
  LogLevelColor_E = BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLACK;
  for (i_U32 = 0; i_U32 < S_LogChannelList.size(); i_U32++)
  {
    Start_U32 = Bof_GetMsTickCount();
    for (j_U32 = 0; j_U32 < (MAXNUMBEROFLOGGERQUEUEENTRIES * 3) + 7; j_U32++)
    {
      if (i_U32 == 6)
      {
        Sts_E = rBofLog.LogLevelColor(S_LogChannelList[i_U32].ChannelName_S, BOF_LOG_CHANNEL_LEVEL::CRITICAL, LogLevelColor_E);
        EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
        LogLevelColor_U32 = static_cast<uint32_t>(LogLevelColor_E);
        LogLevelColor_U32++;
        LogLevelColor_E = static_cast<BOF_LOG_LEVEL_COLOR>(LogLevelColor_U32);
        if (LogLevelColor_E > BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_WHITE)
        {
          LogLevelColor_E = BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLACK;
        }
      }
      Sts_E = rBofLog.Log(S_LogChannelList[i_U32].ChannelName_S, BOF_LOG_CHANNEL_LEVEL::CRITICAL, "Log %08X", j_U32);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    }
    Delta_U32 = Bof_ElapsedMsTime(Start_U32);
    //printf("Io on channel %s in %s: %d log in %d ms->%d log/sec%s", S_LogChannelList[i_U32].ChannelName_S.c_str(), S_LogChannelList[i_U32].FileLogPath.FullPathName(false).c_str(), j_U32, Delta_U32, Delta_U32 ? (j_U32 * 1000) / Delta_U32 : 0, Bof_Eol());
    Sts_E = rBofLog.Flush(S_LogChannelList[i_U32].ChannelName_S);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    Delta_U32 = Bof_ElapsedMsTime(Start_U32);
    //printf("Flush on channel %s in %s: %d log in %d ms->%d log/sec%s", S_LogChannelList[i_U32].ChannelName_S.c_str(), S_LogChannelList[i_U32].FileLogPath.FullPathName(false).c_str(), j_U32, Delta_U32, Delta_U32 ? (j_U32 * 1000) / Delta_U32 : 0, Bof_Eol());
  }
  Bof_MsSleep(500);
  //24/03/23 10:59:15:664 C LogChannel7 00000774 Log 00000305
  uint32_t OneLineSize_U32 = 8 + 1 + 12 + 1 + 1 + 1 + static_cast<uint32_t>(strlen(S_LogChannelList[0].ChannelName_S.c_str())) + 1 + 8 + 1 + 3 + 1 + 8 + static_cast<uint32_t>(strlen(Bof_Eol()));
  uint32_t FileSize_U32 = (j_U32 * OneLineSize_U32);
  EXPECT_EQ(Bof_GetFileSize(S_LogChannelList[0].FileLogPath), FileSize_U32);
  EXPECT_LE(Bof_GetFileSize(S_LogChannelList[1].FileLogPath), MAXLOGSIZEINBYTE);

  uint32_t LinePerFile_U32 = MAXLOGSIZEINBYTE / OneLineSize_U32;
  uint32_t NbLineInCurrentFile_U32 = j_U32 % LinePerFile_U32;
  //24/03/23 11:02:33:065 C LogChannel3 00000628 Log 00000273
  uint32_t RotatingSize_U32 = NbLineInCurrentFile_U32 * (8 + 1 + 12 + 1 + 1 + 1 + static_cast<uint32_t>(strlen(S_LogChannelList[2].ChannelName_S.c_str())) + 1 + 8 + 1 + 3 + 1 + 8 + static_cast<uint32_t>(strlen(Bof_Eol())));
  // printf("%d %d %d\n", Bof_GetFileSize(S_LogChannelList[2].FileLogPath), RotatingSize_U32, RotatingSize_U32 + 1);
  EXPECT_LE(RotatingSize_U32, Bof_GetFileSize(S_LogChannelList[2].FileLogPath));
  EXPECT_LE(Bof_GetFileSize(S_LogChannelList[2].FileLogPath), RotatingSize_U32 + 2);
  // BHATODO FIX  EXPECT_EQ(Bof_GetFileSize(DailyPath), j_U32 * ((6 + 1 + 12 + 1 + 1 + 1 + strlen(S_LogChannelList[3].ChannelName_S.c_str()) + 2 + 8 + 1) + 4 + 8 + strlen(Bof_Eol())));

  for (i_U32 = 0; i_U32 < S_LogChannelList.size(); i_U32++)
  {
    Start_U32 = Bof_GetMsTickCount();
    for (j_U32 = 0; j_U32 < (MAXNUMBEROFLOGGERQUEUEENTRIES * 3) + 7; j_U32++)
    {
      Sts_E = rBofLog.Log(S_LogChannelList[i_U32].ChannelName_S, BOF_LOG_CHANNEL_LEVEL::CRITICAL, "Log %08X", i_U32);
      EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
      if (j_U32 == MAXNUMBEROFLOGGERQUEUEENTRIES)
      {
        Sts_E = rBofLog.DeleteLogStorage(S_LogChannelList[i_U32].ChannelName_S);
      }
    }
    Delta_U32 = Bof_ElapsedMsTime(Start_U32);
    //printf("Io on channel %s in %s: %d log in %d ms->%d log/sec%s", S_LogChannelList[i_U32].ChannelName_S.c_str(), S_LogChannelList[i_U32].FileLogPath.FullPathName(false).c_str(), j_U32, Delta_U32, Delta_U32 ? (j_U32 * 1000) / Delta_U32 : 0, Bof_Eol());
    Sts_E = rBofLog.Flush(S_LogChannelList[i_U32].ChannelName_S);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    Delta_U32 = Bof_ElapsedMsTime(Start_U32);
    //printf("Flush on channel %s in %s: %d log in %d ms->%d log/sec%s", S_LogChannelList[i_U32].ChannelName_S.c_str(), S_LogChannelList[i_U32].FileLogPath.FullPathName(false).c_str(), j_U32, Delta_U32, Delta_U32 ? (j_U32 * 1000) / Delta_U32 : 0, Bof_Eol());
  }
  rBofLog.ShutdownLogger();
}
