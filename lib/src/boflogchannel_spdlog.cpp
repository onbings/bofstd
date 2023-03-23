/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the logger channel interface based on spdlog
 *
 * Name:        boflogchannel_spdlog.cpp
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
#include <bofstd/boffs.h>
#include <bofstd/boflogchannel_spdlog.h>
#include <bofstd/boflogger.h>
#include <bofstd/boflogsink_spdlog.h>

#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/stdout_sinks.h"
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/spdlog.h>

#if defined(_WIN32)
#else

#include "spdlog/sinks/syslog_sink.h"

#endif

#include "spdlog/sinks/msvc_sink.h"
#include <map>

BEGIN_BOF_NAMESPACE()
// ALWAYS is mapped to OFF spdlog internal level
static std::map<BOF_LOG_CHANNEL_LEVEL, spdlog::level::level_enum> S_ToSpdlogLevel = {
    {BOF_LOG_CHANNEL_LEVEL::TRACE, spdlog::level::level_enum::trace},  {BOF_LOG_CHANNEL_LEVEL::DBG, spdlog::level::level_enum::debug},  {BOF_LOG_CHANNEL_LEVEL::INFORMATION, spdlog::level::level_enum::info},
    {BOF_LOG_CHANNEL_LEVEL::WARNING, spdlog::level::level_enum::warn}, {BOF_LOG_CHANNEL_LEVEL::ERR, spdlog::level::level_enum::err},    {BOF_LOG_CHANNEL_LEVEL::CRITICAL, spdlog::level::level_enum::critical},
    {BOF_LOG_CHANNEL_LEVEL::OFF, spdlog::level::level_enum::off},      {BOF_LOG_CHANNEL_LEVEL::ALWAYS, spdlog::level::level_enum::off}, {BOF_LOG_CHANNEL_LEVEL::MAX, spdlog::level::level_enum::off}};

static spdlog::level::level_enum S_BofLoggerLevelToSpdlogLevel(BOF_LOG_CHANNEL_LEVEL _From_E)
{
  std::map<BOF_LOG_CHANNEL_LEVEL, spdlog::level::level_enum>::iterator It = S_ToSpdlogLevel.find(_From_E);
  BOF_ASSERT(It != S_ToSpdlogLevel.end()); // Must be found !
  return It->second;
}

// https://github.com/gabime/spdlog/issues/1501
/*
static std::shared_ptr<spdlog::sinks::basic_file_sink_mt>
create_file_sink(std::string out)
{
  bool truncate = false;
  return std::make_shared<spdlog::sinks::basic_file_sink_mt>(
    std::move(out), truncate);
}
*/
/*
class BofSpdLog :public spdlog::logger  //, public std::enable_shared_from_this<BofSpdLog>
{
public:
  void LogChannelBasePathName(const std::string &_rLogChannelBasePathName_S) { mLogChannelBasePathName_S = _rLogChannelBasePathName_S; }
  const std::string &LogChannelBasePathName() { return mLogChannelBasePathName_S; }

private:
  std::string  mLogChannelBasePathName_S;
};
*/
#if 0
void OnSpdlogOpenAsync()
{
  // printf("OnSpdlogOpenAsync called: set thread affinity for example%s",Bof_Eol());
}

void OnSpdlogCloseAsync()
{
  // printf("OnSpdlogCloseAsync called: %s",Bof_Eol());
}
#endif

// Opaque pointer design pattern: defined privately here all private data and functions: all of these can now change without recompiling callers ...
class BofLogChannelSpdLog::BofLogChannelSpdLogImplementation
{
private:
  struct BOF_LOG_CHANNEL_CHARACTERISTIC
  {
    std::string LogHeader_S;
    BOF_LOG_LEVEL_COLOR pLogLevelColor_E[(int)BOF_LOG_CHANNEL_LEVEL::MAX];
    BofPath LogPath;

    BOF_LOG_CHANNEL_CHARACTERISTIC()
    {
      Reset();
    }
    void Reset()
    {
      int i;

      LogHeader_S = "";
      for (i = 0; i < (int)BOF_LOG_CHANNEL_LEVEL::MAX; i++)
      {
        pLogLevelColor_E[i] = BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLACK;
      }
      LogPath = BofPath();
    }
  };
  bool mAddLineNumber_B = false;
  std::atomic<uint32_t> mLineNumber;
  std::shared_ptr<ramcircularbuffer_sink_mt> mpsCircularBufferSink = nullptr;
  std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> mpsStdOutColorSink = nullptr;
  std::shared_ptr<spdlog::sinks::stderr_color_sink_mt> mpsStdErrColorSink = nullptr;
  BOF_LOG_CHANNEL_PARAM mLogChannelParam_X;
  bool mLogOpened_B = false;
  uintptr_t mIoLog = -1;
  std::map<std::string, BOF_LOG_CHANNEL_CHARACTERISTIC> mLogChannelCharacteristicCollection;

public:
  BofLogChannelSpdLogImplementation()
  {
    mLineNumber.store(0);
    mLogChannelParam_X.Reset();
  }

  ~BofLogChannelSpdLogImplementation()
  {
    V_Remove();
  }

  BOFERR V_Add(const BOF_LOG_CHANNEL_PARAM &_rLogParam_X)
  {
    BOFERR Rts_E = BOF_ERR_INIT;
    int32_t DailyRotationHour_S32, DailyRotationMinute_S32;
    std::shared_ptr<spdlog::logger> psLogger = nullptr;
    BofLogger &rBofLogger = BofLogger::S_Instance();

    mLogChannelParam_X = _rLogParam_X;
    switch (mLogChannelParam_X.LogSink_E)
    {
    case BOF_LOG_CHANNEL_SINK::TO_NONE: {
      Rts_E = BOF_ERR_INIT;
      if (rBofLogger.IsLoggerInAsyncMode())
      {
        if (rBofLogger.IsLoggerBlockingInAsyncMode())
        {
          psLogger = spdlog::create_async<spdlog::sinks::null_sink_mt>(mLogChannelParam_X.ChannelName_S);
        }
        else
        {
          psLogger = spdlog::create_async_nb<spdlog::sinks::null_sink_mt>(mLogChannelParam_X.ChannelName_S);
        }
      }
      else
      {
        psLogger = spdlog::create<spdlog::sinks::null_sink_mt>(mLogChannelParam_X.ChannelName_S);
      }
      if (psLogger)
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
    break;

    case BOF_LOG_CHANNEL_SINK::TO_FILE:
    case BOF_LOG_CHANNEL_SINK::TO_DAILYFILE: {
      Rts_E = BOF_ERR_EINVAL;
      if (mLogChannelParam_X.FileLogPath.FullPathName(false) != "")
      {
        //				std::string PathWithoutExt_S = mLogParam_X.FileLogPath.DirectoryName(false) + mLogParam_X.FileLogPath.FileNameWithoutExtension();
        BOF_FILE_PERMISSION Permission_E;

        Permission_E = BOF_FILE_PERMISSION_ALL_FOR_ALL;
        Rts_E = Bof_CreateDirectory(Permission_E, mLogChannelParam_X.FileLogPath.DirectoryName(true, false));
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          if (Bof_IsAnyBitFlagSet(mLogChannelParam_X.LogFlag_E, BOF_LOG_CHANNEL_FLAG::DELETE_PREVIOUS_LOGFILE))
          {
            DeleteLogStorage();
          }
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            if (mLogChannelParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_DAILYFILE)
            {
              // we can add daily limitted size and daily limet nb

              Rts_E = BOF_ERR_NOT_INIT;
              DailyRotationHour_S32 = static_cast<int32_t>(mLogChannelParam_X.DailyRotationTimeInMinuteAfterMidnight_U32 / 60);
              DailyRotationMinute_S32 = static_cast<int32_t>(mLogChannelParam_X.DailyRotationTimeInMinuteAfterMidnight_U32 - (DailyRotationHour_S32 * 60));
              //                    psLogger = spdlog::create<limited_daily_file_sink_mt>(mLogParam_X.ChannelName_S, PathWithoutExt_S, mLogParam_X.FileLogPath.Extension(), DailyRotationHour_S32, DailyRotationMinute_S32, mLogParam_X.MaxLogSizeInByte_U32);
              if (rBofLogger.IsLoggerInAsyncMode())
              {
                if (rBofLogger.IsLoggerBlockingInAsyncMode())
                {
                  psLogger = spdlog::create_async<limited_daily_file_sink_mt>(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.FileLogPath.FullPathName(false), DailyRotationHour_S32, DailyRotationMinute_S32, mLogChannelParam_X.MaxLogSizeInByte_U32);
                }
                else
                {
                  psLogger = spdlog::create_async_nb<limited_daily_file_sink_mt>(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.FileLogPath.FullPathName(false), DailyRotationHour_S32, DailyRotationMinute_S32, mLogChannelParam_X.MaxLogSizeInByte_U32);
                }
              }
              else
              {
                psLogger = spdlog::create<limited_daily_file_sink_mt>(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.FileLogPath.FullPathName(false), DailyRotationHour_S32, DailyRotationMinute_S32, mLogChannelParam_X.MaxLogSizeInByte_U32);
              }
              if (psLogger)
              {
                Rts_E = BOF_ERR_NO_ERROR;
              }
            }
            else
            {
              if (mLogChannelParam_X.MaxNumberOfLogFile_U32)
              {
                Rts_E = BOF_ERR_NOT_INIT;
                // psLogger = spdlog::create<spdlog::sinks::rotating_file_sink_mt>(mLogParam_X.ChannelName_S, PathWithoutExt_S, mLogParam_X.FileLogPath.Extension(), mLogParam_X.MaxLogSizeInByte_U32, mLogParam_X.MaxNumberOfLogFile_U32);
                if (rBofLogger.IsLoggerInAsyncMode())
                {
                  if (rBofLogger.IsLoggerBlockingInAsyncMode())
                  {
                    psLogger =
                        spdlog::create_async<spdlog::sinks::rotating_file_sink_mt>(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.FileLogPath.FullPathName(false), mLogChannelParam_X.MaxLogSizeInByte_U32, mLogChannelParam_X.MaxNumberOfLogFile_U32);
                  }
                  else
                  {
                    psLogger =
                        spdlog::create_async_nb<spdlog::sinks::rotating_file_sink_mt>(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.FileLogPath.FullPathName(false), mLogChannelParam_X.MaxLogSizeInByte_U32, mLogChannelParam_X.MaxNumberOfLogFile_U32);
                  }
                }
                else
                {
                  psLogger = spdlog::create<spdlog::sinks::rotating_file_sink_mt>(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.FileLogPath.FullPathName(false), mLogChannelParam_X.MaxLogSizeInByte_U32, mLogChannelParam_X.MaxNumberOfLogFile_U32);
                }
                if (psLogger)
                {
                  Rts_E = BOF_ERR_NO_ERROR;
                }
              }
              else
              {
                Rts_E = BOF_ERR_NOT_INIT;
                if (mLogChannelParam_X.MaxLogSizeInByte_U32)
                {
                  if (rBofLogger.IsLoggerInAsyncMode())
                  {
                    if (rBofLogger.IsLoggerBlockingInAsyncMode())
                    {
                      psLogger = spdlog::create_async<simple_limitedfile_sink_mt>(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.FileLogPath.FullPathName(false), mLogChannelParam_X.MaxLogSizeInByte_U32);
                    }
                    else
                    {
                      psLogger = spdlog::create_async_nb<simple_limitedfile_sink_mt>(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.FileLogPath.FullPathName(false), mLogChannelParam_X.MaxLogSizeInByte_U32);
                    }
                  }
                  else
                  {
                    psLogger = spdlog::create<simple_limitedfile_sink_mt>(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.FileLogPath.FullPathName(false), mLogChannelParam_X.MaxLogSizeInByte_U32);
                  }
                }
                else
                {
                  if (rBofLogger.IsLoggerInAsyncMode())
                  {
                    if (rBofLogger.IsLoggerBlockingInAsyncMode())
                    {
                      psLogger = spdlog::create_async<spdlog::sinks::basic_file_sink_mt>(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.FileLogPath.FullPathName(false));
                    }
                    else
                    {
                      psLogger = spdlog::create_async_nb<spdlog::sinks::basic_file_sink_mt>(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.FileLogPath.FullPathName(false));
                    }
                  }
                  else
                  {
                    psLogger = spdlog::create<spdlog::sinks::basic_file_sink_mt>(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.FileLogPath.FullPathName(false));
                  }
                }
                if (psLogger)
                {
                  Rts_E = BOF_ERR_NO_ERROR;
                }
              }
            }
          }
        }
      }
    }
    break;
      /*
      case BOF_LOG_CHANNEL_SINK::TO_STREAM:
      {
        Rts_E = BOF_ERR_INIT;
  //move a shared_ptr ....				psLogger = spdlog::create<spdlog::sinks::ostream_sink_mt>(mLogParam_X.ChannelName_S, std::move(*mLogParam_X.psOutputStream));
          std::ostringstream Oss(mLogParam_X.psOutputStream->str());
          psLogger = spdlog::create<spdlog::sinks::ostream_sink_mt>(mLogParam_X.ChannelName_S, std::move(Oss));	// std::move(*mLogParam_X.psOutputStream));
          if (psLogger)
          {
            Rts_E = BOF_ERR_NO_ERROR;
          }
        }
        break;
        */

    case BOF_LOG_CHANNEL_SINK::TO_RAM_CIRCULAR_BUFFER: {
      Rts_E = BOF_ERR_INIT;
      mpsCircularBufferSink = std::make_shared<ramcircularbuffer_sink_mt>(mLogChannelParam_X.BufferOverflowPolicy_E, mLogChannelParam_X.MaxLogSizeInByte_U32);
      if (rBofLogger.IsLoggerInAsyncMode())
      {
        if (rBofLogger.IsLoggerBlockingInAsyncMode())
        {
          psLogger = std::make_shared<spdlog::async_logger>(mLogChannelParam_X.ChannelName_S, mpsCircularBufferSink, spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        }
        else
        {
          psLogger = std::make_shared<spdlog::async_logger>(mLogChannelParam_X.ChannelName_S, mpsCircularBufferSink, spdlog::thread_pool(), spdlog::async_overflow_policy::overrun_oldest);
        }
      }
      else
      {
        psLogger = std::make_shared<spdlog::logger>(mLogChannelParam_X.ChannelName_S, mpsCircularBufferSink);
      }
      if (psLogger)
      {
        spdlog::details::registry::instance().initialize_logger(psLogger);
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
    break;

    case BOF_LOG_CHANNEL_SINK::TO_STDERR: {
      Rts_E = BOF_ERR_INIT;
      if (rBofLogger.IsLoggerInAsyncMode())
      {
        if (rBofLogger.IsLoggerBlockingInAsyncMode())
        {
          psLogger = spdlog::create_async<spdlog::sinks::stderr_sink_mt>(mLogChannelParam_X.ChannelName_S);
        }
        else
        {
          psLogger = spdlog::create_async_nb<spdlog::sinks::stderr_sink_mt>(mLogChannelParam_X.ChannelName_S);
        }
      }
      else
      {
        psLogger = spdlog::create<spdlog::sinks::stderr_sink_mt>(mLogChannelParam_X.ChannelName_S);
      }
      if (psLogger)
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
    break;

    case BOF_LOG_CHANNEL_SINK::TO_STDERR_COLOR: {
      Rts_E = BOF_ERR_INIT;
      mpsStdErrColorSink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
      if (rBofLogger.IsLoggerInAsyncMode())
      {
        if (rBofLogger.IsLoggerBlockingInAsyncMode())
        {
          psLogger = std::make_shared<spdlog::async_logger>(mLogChannelParam_X.ChannelName_S, mpsStdErrColorSink, spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        }
        else
        {
          psLogger = std::make_shared<spdlog::async_logger>(mLogChannelParam_X.ChannelName_S, mpsStdErrColorSink, spdlog::thread_pool(), spdlog::async_overflow_policy::overrun_oldest);
        }
      }
      else
      {
        psLogger = std::make_shared<spdlog::logger>(mLogChannelParam_X.ChannelName_S, mpsStdErrColorSink);
      }
      if (psLogger)
      {
        spdlog::details::registry::instance().initialize_logger(psLogger);
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
    break;

    case BOF_LOG_CHANNEL_SINK::TO_STDOUT: {
      Rts_E = BOF_ERR_INIT;
      if (rBofLogger.IsLoggerInAsyncMode())
      {
        if (rBofLogger.IsLoggerBlockingInAsyncMode())
        {
          psLogger = spdlog::create_async<spdlog::sinks::stdout_sink_mt>(mLogChannelParam_X.ChannelName_S);
        }
        else
        {
          psLogger = spdlog::create_async_nb<spdlog::sinks::stdout_sink_mt>(mLogChannelParam_X.ChannelName_S);
        }
      }
      else
      {
        psLogger = spdlog::create<spdlog::sinks::stdout_sink_mt>(mLogChannelParam_X.ChannelName_S);
      }
      if (psLogger)
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
    break;

    case BOF_LOG_CHANNEL_SINK::TO_STDOUT_COLOR: {
      Rts_E = BOF_ERR_INIT;
      mpsStdOutColorSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      if (rBofLogger.IsLoggerInAsyncMode())
      {
        if (rBofLogger.IsLoggerBlockingInAsyncMode())
        {
          psLogger = std::make_shared<spdlog::async_logger>(mLogChannelParam_X.ChannelName_S, mpsStdOutColorSink, spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        }
        else
        {
          psLogger = std::make_shared<spdlog::async_logger>(mLogChannelParam_X.ChannelName_S, mpsStdOutColorSink, spdlog::thread_pool(), spdlog::async_overflow_policy::overrun_oldest);
        }
      }
      else
      {
        psLogger = std::make_shared<spdlog::logger>(mLogChannelParam_X.ChannelName_S, mpsStdOutColorSink);
      }
      if (psLogger)
      {
        spdlog::details::registry::instance().initialize_logger(psLogger);
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
    break;

    case BOF_LOG_CHANNEL_SINK::TO_LINUX_SYSLOG: {
      Rts_E = BOF_ERR_NOT_SUPPORTED;
#if defined(_WIN32)
#else
      //                                                                     ::openlog(_ident.empty()? nullptr:_ident.c_str(), syslog_option,        syslog_facility);
      // psLogger = spdlog::create<spdlog::sinks::syslog_sink>(mLogParam_X.ChannelName_S, mLogParam_X.ChannelName_S, LOG_PID | LOG_NDELAY, LOG_USER);
      if (rBofLogger.IsLoggerInAsyncMode())
      {
        if (rBofLogger.IsLoggerBlockingInAsyncMode())
        {
          psLogger = spdlog::syslog_logger_mt<spdlog::async_factory>(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.ChannelName_S, LOG_PID | LOG_NDELAY, LOG_USER);
        }
        else
        {
          psLogger = spdlog::syslog_logger_mt<spdlog::async_factory>(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.ChannelName_S, LOG_PID | LOG_NDELAY, LOG_USER);
        }
      }
      else
      {
        psLogger = spdlog::syslog_logger_mt(mLogChannelParam_X.ChannelName_S, mLogChannelParam_X.ChannelName_S, LOG_PID | LOG_NDELAY, LOG_USER);
        ;
      }
      if (psLogger)
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
#endif
    }
    break;

    case BOF_LOG_CHANNEL_SINK::TO_MSVC_DEBUGGER: {
      Rts_E = BOF_ERR_NOT_SUPPORTED;
#if defined(_WIN32)
      if (rBofLogger.IsLoggerInAsyncMode())
      {
        if (rBofLogger.IsLoggerBlockingInAsyncMode())
        {
          psLogger = spdlog::create_async<spdlog::sinks::msvc_sink_mt>(mLogChannelParam_X.ChannelName_S);
        }
        else
        {
          psLogger = spdlog::create_async_nb<spdlog::sinks::msvc_sink_mt>(mLogChannelParam_X.ChannelName_S);
        }
      }
      else
      {
        psLogger = spdlog::create<spdlog::sinks::msvc_sink_mt>(mLogChannelParam_X.ChannelName_S);
      }
      if (psLogger)
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
#else
#endif
    }
    break;

    default:
      Rts_E = BOF_ERR_NOT_SUPPORTED;
      break;
    }
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      // #pragma message("Please fix me V_Add")
      mLogChannelCharacteristicCollection[mLogChannelParam_X.ChannelName_S].LogPath = mLogChannelParam_X.FileLogPath;

      V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL::TRACE, static_cast<BOF_LOG_LEVEL_COLOR>(BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_RED | BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_GREEN | BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLUE));
      V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL::DBG, static_cast<BOF_LOG_LEVEL_COLOR>(BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_GREEN | BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLUE));
      V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL::INFORMATION, static_cast<BOF_LOG_LEVEL_COLOR>(BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_GREEN));
      V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL::WARNING, static_cast<BOF_LOG_LEVEL_COLOR>(BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_RED | BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_GREEN | BOF_LOG_LEVEL_COLOR::LOG_COLOR_BOLD));
      V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL::ERR, static_cast<BOF_LOG_LEVEL_COLOR>(BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_RED | BOF_LOG_LEVEL_COLOR::LOG_COLOR_BOLD));
      V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL::CRITICAL, static_cast<BOF_LOG_LEVEL_COLOR>(BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_RED | BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_RED | BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_GREEN |
                                                                                        BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLUE | BOF_LOG_LEVEL_COLOR::LOG_COLOR_BOLD));
      // ALWAYS is mapped to OFF spdlog internal level
      V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL::ALWAYS, static_cast<BOF_LOG_LEVEL_COLOR>(BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLACK | BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_WHITE));
      V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL::OFF, static_cast<BOF_LOG_LEVEL_COLOR>(BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLACK | BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_WHITE));

      //      V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL::ALWAYS, static_cast<BOF_LOG_LEVEL_COLOR>(BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_RED));
      //      V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL::OFF, static_cast<BOF_LOG_LEVEL_COLOR>(BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLUE));

      //      V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL::OFF, static_cast<BOF_LOG_LEVEL_COLOR>(BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLACK | BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLACK ));
      /*
      colors_[level::warn] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; // intense yellow
      colors_[level::err] = FOREGROUND_RED | FOREGROUND_INTENSITY;                     // intense red
      colors_[level::critical] =
        BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; // intense white on red background
      colors_[level::off] = 0;
      */
      Rts_E = V_LogLevel(BOF_LOG_CHANNEL_LEVEL::TRACE);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Rts_E = V_LogHeader(mLogChannelParam_X.LogHeader_S);
      }
    }
    return Rts_E;
  }

  BOFERR V_Remove()
  {
    BOFERR Rts_E;

    V_CloseLogStorage();
    //		spdlog::drop(mLogParam_X.ChannelName_S);
    Rts_E = BOF_ERR_NO_ERROR;

    return Rts_E;
  }

  BOFERR V_OpenLogStorage(uint32_t /*_Offset_U32*/)
  {
    BOFERR Rts_E = BOF_ERR_ALREADY_OPENED;

    if (!mLogOpened_B)
    {
      if (mLogChannelParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_RAM_CIRCULAR_BUFFER)
      {
        if (mpsCircularBufferSink != nullptr)
        {
          Rts_E = mpsCircularBufferSink->ResyncSnapshot();
        }
      }
      if ((mLogChannelParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_FILE) || (mLogChannelParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_DAILYFILE))
      {
        Rts_E = Bof_OpenFile(mLogChannelParam_X.FileLogPath, true, false, mIoLog);
      }
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        mLogOpened_B = true;
      }
    }
    return Rts_E;
  }

  BOFERR V_ReadLogStorage(uint32_t _MaxLineToExtract_U32, std::vector<std::string> &_rExtractedLogLine_S) const
  {
    BOFERR Rts_E;
    uint32_t i_U32;
    std::string Line_S;

    Rts_E = BOF_ERR_EBUSY;
    if (mLogOpened_B) // Must call V_OpenLog first
    {
      Rts_E = BOF_ERR_EINVAL;
      if (_MaxLineToExtract_U32)
      {
        i_U32 = 0;
        do
        {
          if (mLogChannelParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_RAM_CIRCULAR_BUFFER)
          {
            Rts_E = mpsCircularBufferSink ? mpsCircularBufferSink->PopSnapshot(Line_S) : BOF_ERR_CLOSE;
          }
          if ((mLogChannelParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_FILE) || (mLogChannelParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_DAILYFILE))
          {
            Rts_E = (mIoLog != -1) ? Bof_ReadLine(mIoLog, Line_S) : BOF_ERR_CLOSE;
          }
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            _rExtractedLogLine_S.push_back(Line_S);
            i_U32++;
          }
        } while ((Rts_E == BOF_ERR_NO_ERROR) && (i_U32 < _MaxLineToExtract_U32));
      }
    }
    return Rts_E;
  }

  BOFERR V_CloseLogStorage()
  {
    BOFERR Rts_E = BOF_ERR_NOT_OPENED;

    if (mLogOpened_B)
    {
      if ((mLogChannelParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_FILE) || (mLogChannelParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_DAILYFILE))
      {
        Bof_CloseFile(mIoLog);
      }
      Rts_E = BOF_ERR_NO_ERROR;
      mLogOpened_B = false;
    }
    return Rts_E;
  }

  BOFERR V_Log(BOF_LOG_CHANNEL_LEVEL _Level_E, const std::string &_rLogMsg_S)
  {
    BOFERR Rts_E;
    spdlog::level::level_enum SpdLogLevel_E;
    char pLineNumber_c[16];
    std::shared_ptr<spdlog::logger> psLogger;
    // bool IsLogHeaderVisible_B;
    std::string Line_S;

    SpdLogLevel_E = S_BofLoggerLevelToSpdlogLevel(_Level_E);

    psLogger = spdlog::details::registry::instance().get(mLogChannelParam_X.ChannelName_S);
    BOF_ASSERT(psLogger != nullptr);
    Rts_E = BOF_ERR_INTERNAL;
    if (psLogger)
    {
      Rts_E = BOF_ERR_NO_ERROR;
      // IsLogHeaderVisible_B=psLogger->IsLogHeaderVisible();

      if (mAddLineNumber_B)
      {
        sprintf(pLineNumber_c, "%08X ", ++mLineNumber);
        Line_S = std::string(pLineNumber_c) + _rLogMsg_S.c_str();
      }
      else
      {
        Line_S = _rLogMsg_S.c_str();
      }
      if (mLogChannelParam_X.AutoAddEol_B)
      {
        Line_S += Bof_Eol();
      }
      psLogger->log(SpdLogLevel_E, Line_S.c_str());
    }

    return Rts_E;
  }

  BOFERR V_Flush()
  {
    BOFERR Rts_E;
    std::shared_ptr<spdlog::logger> psLogger;

    psLogger = spdlog::details::registry::instance().get(mLogChannelParam_X.ChannelName_S);
    BOF_ASSERT(psLogger != nullptr);
    Rts_E = BOF_ERR_INTERNAL;
    if (psLogger)
    {
      psLogger->flush();
      Rts_E = BOF_ERR_NO_ERROR;
    }
    return Rts_E;
  }

  BOFERR V_LogLevel(BOF_LOG_CHANNEL_LEVEL _LogLevel_E)
  {
    BOFERR Rts_E;
    std::shared_ptr<spdlog::logger> psLogger;
    spdlog::level::level_enum LogLevel_E;

    psLogger = spdlog::details::registry::instance().get(mLogChannelParam_X.ChannelName_S);
    BOF_ASSERT(psLogger != nullptr);
    Rts_E = BOF_ERR_INTERNAL;
    if (psLogger)
    {
      LogLevel_E = S_ToSpdlogLevel[_LogLevel_E];
      psLogger->set_level(LogLevel_E);
      Rts_E = BOF_ERR_NO_ERROR;
    }
    return Rts_E;
  }
  BOFERR V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL _LogLevel_E, BOF_LOG_LEVEL_COLOR _LogLevelColor_E)
  {
    BOFERR Rts_E;
    std::shared_ptr<spdlog::logger> psLogger;
    spdlog::level::level_enum LogLevel_E;
    BOF_LOG_LEVEL_COLOR Fore_E, Back_E;
    bool Bold_B;
#if defined(_WIN32)
    WORD Color = 0;
#else
    //    std::string Color = "";
    spdlog::string_view_t Color = "";
#endif

    psLogger = spdlog::details::registry::instance().get(mLogChannelParam_X.ChannelName_S);
    BOF_ASSERT(psLogger != nullptr);
    Rts_E = BOF_ERR_INTERNAL;
    if (psLogger)
    {
      mLogChannelCharacteristicCollection[mLogChannelParam_X.ChannelName_S].pLogLevelColor_E[(int)_LogLevel_E] = _LogLevelColor_E;

      LogLevel_E = S_ToSpdlogLevel[_LogLevel_E];
      Fore_E = static_cast<BOF_LOG_LEVEL_COLOR>(_LogLevelColor_E & LOG_COLOR_FORE_MASK);
      Back_E = static_cast<BOF_LOG_LEVEL_COLOR>(_LogLevelColor_E & LOG_COLOR_BACK_MASK);
      Bold_B = (_LogLevelColor_E & LOG_COLOR_BOLD) ? true : false;
      if (mLogChannelParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_STDOUT_COLOR)
      {
        BOF_ASSERT(mpsStdOutColorSink != nullptr);
        if (mpsStdOutColorSink != nullptr)
        {
#if defined(_WIN32)
          switch (Fore_E)
          {
          case LOG_COLOR_NO_CHANGE:
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLACK:
            Color = 0;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_RED:
            Color = FOREGROUND_RED;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_GREEN:
            Color = FOREGROUND_GREEN;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_YELLOW:
            Color = FOREGROUND_RED | FOREGROUND_GREEN;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLUE:
            Color = FOREGROUND_BLUE;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_MAGENTA:
            Color = FOREGROUND_RED | FOREGROUND_BLUE;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_CYAN:
            Color = FOREGROUND_GREEN | FOREGROUND_BLUE;
            break;

          default:
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_WHITE:
            Color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            break;
          }
          switch (Back_E)
          {
          case LOG_COLOR_NO_CHANGE:
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLACK:
            Color |= 0;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_RED:
            Color |= BACKGROUND_RED;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_GREEN:
            Color |= BACKGROUND_GREEN;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_YELLOW:
            Color |= BACKGROUND_RED | BACKGROUND_GREEN;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLUE:
            Color |= BACKGROUND_BLUE;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_MAGENTA:
            Color |= BACKGROUND_RED | BACKGROUND_BLUE;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_CYAN:
            Color |= BACKGROUND_GREEN | BACKGROUND_BLUE;
            //              mpsStdOutColorSink->set_color(LogLevel_E, Color);
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_WHITE:
            Color |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
            break;

          default:
            break;
          }
          if (Bold_B)
          {
            Color |= FOREGROUND_INTENSITY;
          }
          mpsStdOutColorSink->set_color(LogLevel_E, Color);
#else
          switch (Fore_E)
          {
          case LOG_COLOR_NO_CHANGE:
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLACK:
            Color = mpsStdOutColorSink->black;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_RED:
            Color = mpsStdOutColorSink->red;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_GREEN:
            Color = mpsStdOutColorSink->green;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_YELLOW:
            Color = mpsStdOutColorSink->yellow;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLUE:
            Color = mpsStdOutColorSink->blue;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_MAGENTA:
            Color = mpsStdOutColorSink->magenta;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_CYAN:
            Color = mpsStdOutColorSink->cyan;
            break;

          default:
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_WHITE:
            Color = mpsStdOutColorSink->white;
            break;
          }
          mpsStdOutColorSink->set_color(LogLevel_E, Color);
          switch (Back_E)
          {
          case LOG_COLOR_NO_CHANGE:
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLACK:
            Color = mpsStdOutColorSink->on_black;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_RED:
            Color = mpsStdOutColorSink->on_red;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_GREEN:
            Color = mpsStdOutColorSink->on_green;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_YELLOW:
            Color = mpsStdOutColorSink->on_yellow;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLUE:
            Color = mpsStdOutColorSink->on_blue;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_MAGENTA:
            Color = mpsStdOutColorSink->on_magenta;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_CYAN:
            Color = mpsStdOutColorSink->on_cyan;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_WHITE:
            Color = mpsStdOutColorSink->on_white;
            break;

          default:
            break;
          }

          mpsStdOutColorSink->set_color(LogLevel_E, Color);
          if (Bold_B)
          {
            Color = mpsStdOutColorSink->bold;
            mpsStdOutColorSink->set_color(LogLevel_E, Color);
          }
#endif
          Rts_E = BOF_ERR_NO_ERROR;
        }
      }
      else if (mLogChannelParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_STDERR_COLOR)
      {
        BOF_ASSERT(mpsStdErrColorSink != nullptr);
        if (mpsStdErrColorSink != nullptr)
        {
#if defined(_WIN32)
          if (Bold_B)
          {
            Color |= FOREGROUND_INTENSITY;
          }
          switch (Fore_E)
          {
          case LOG_COLOR_NO_CHANGE:
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLACK:
            Color = 0;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_RED:
            Color = FOREGROUND_RED;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_GREEN:
            Color = FOREGROUND_GREEN;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_YELLOW:
            Color = BACKGROUND_RED | BACKGROUND_GREEN;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLUE:
            Color = FOREGROUND_BLUE;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_MAGENTA:
            Color = FOREGROUND_RED | FOREGROUND_BLUE;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_CYAN:
            Color = FOREGROUND_GREEN | FOREGROUND_BLUE;
            break;

          default:
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_WHITE:
            Color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            break;
          }
          switch (Back_E)
          {
          case LOG_COLOR_NO_CHANGE:
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLACK:
            Color |= 0;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_RED:
            Color |= BACKGROUND_RED;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_GREEN:
            Color |= BACKGROUND_GREEN;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_YELLOW:
            Color |= BACKGROUND_RED | BACKGROUND_GREEN;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLUE:
            Color |= BACKGROUND_BLUE;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_MAGENTA:
            Color |= BACKGROUND_RED | BACKGROUND_BLUE;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_CYAN:
            Color |= BACKGROUND_GREEN | BACKGROUND_BLUE;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_WHITE:
            Color |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
            break;

          default:
            break;
          }

          mpsStdErrColorSink->set_color(LogLevel_E, Color);
#else
          if (Bold_B)
          {
            Color = mpsStdErrColorSink->bold;
            mpsStdErrColorSink->set_color(LogLevel_E, Color);
          }
          switch (Fore_E)
          {
          case LOG_COLOR_NO_CHANGE:
            break;

          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLACK:
            Color = mpsStdErrColorSink->black;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_RED:
            Color = mpsStdErrColorSink->red;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_GREEN:
            Color = mpsStdErrColorSink->green;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_YELLOW:
            Color = mpsStdErrColorSink->yellow;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_BLUE:
            Color = mpsStdErrColorSink->blue;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_MAGENTA:
            Color = mpsStdErrColorSink->magenta;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_CYAN:
            Color = mpsStdErrColorSink->cyan;
            break;

          default:
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_FORE_WHITE:
            Color = mpsStdErrColorSink->white;
            break;
          }
          mpsStdErrColorSink->set_color(LogLevel_E, Color);

          switch (Back_E)
          {
          case LOG_COLOR_NO_CHANGE:
            break;

          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLACK:
            Color = mpsStdErrColorSink->on_black;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_RED:
            Color = mpsStdErrColorSink->on_red;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_GREEN:
            Color = mpsStdErrColorSink->on_green;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_YELLOW:
            Color = mpsStdErrColorSink->on_yellow;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLUE:
            Color = mpsStdErrColorSink->on_blue;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_MAGENTA:
            Color = mpsStdErrColorSink->on_magenta;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_CYAN:
            Color = mpsStdErrColorSink->on_cyan;
            break;
          case BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_WHITE:
            Color = mpsStdErrColorSink->on_white;
            break;

          default:
            break;
          }
          mpsStdErrColorSink->set_color(LogLevel_E, Color);

#endif
          Rts_E = BOF_ERR_NO_ERROR;
        }
      }
      else
      {
        Rts_E = BOF_ERR_BAD_TYPE;
      }
    }
    return Rts_E;
  }

  BOF_LOG_LEVEL_COLOR V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL _LogLevel_E)
  {
    BOF_LOG_LEVEL_COLOR Rts_E = BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLACK;
    std::shared_ptr<spdlog::logger> psLogger;

    psLogger = spdlog::details::registry::instance().get(mLogChannelParam_X.ChannelName_S);
    BOF_ASSERT(psLogger != nullptr);
    if (psLogger)
    {
      Rts_E = mLogChannelCharacteristicCollection[mLogChannelParam_X.ChannelName_S].pLogLevelColor_E[(int)_LogLevel_E];
    }
    return Rts_E;
  }

  BOFERR V_LogHeader(const std::string &_rLogHeader_S)
  {
    BOFERR Rts_E;
    std::shared_ptr<spdlog::logger> psLogger;
    std::string LogHeader_S;

    psLogger = spdlog::details::registry::instance().get(mLogChannelParam_X.ChannelName_S);
    BOF_ASSERT(psLogger != nullptr);
    Rts_E = BOF_ERR_INTERNAL;
    if (psLogger)
    {
      mLogChannelCharacteristicCollection[mLogChannelParam_X.ChannelName_S].LogHeader_S = _rLogHeader_S;
      LogHeader_S = _rLogHeader_S;
      if (LogHeader_S == "")
      {
        LogHeader_S = "%v";
      }
      /*
      Pattern flags

      Pattern flags are in the form of %flag and resembles the strftime function:

      flag	meaning	example
      %v	The actual text to log	"some user text"
      %t	Thread id	"1232"
      %P	Process id	"3456"
      %n	Logger's name	"some logger name"
      %l	The log level of the message	"debug", "info", etc
      %L	Short log level of the message	"D", "I", etc
      %a	Abbreviated weekday name	"Thu"
      %A	Full weekday name	"Thursday"
      %b	Abbreviated month name	"Aug"
      %B	Full month name	"August"
      %c	Date and time representation	"Thu Aug 23 15:35:46 2014"
      %C	Year in 2 digits	"14"
      %Y	Year in 4 digits	"2014"
      %D or %x	Short MM/DD/YY date	"08/23/14"
      %m	Month 1-12	"11"
      %d	Day of month 1-31	"29"
      %H	Hours in 24 format 0-23	"23"
      %I	Hours in 12 format 1-12	"11"
      %M	Minutes 0-59	"59"
      %S	Seconds 0-59	"58"
      %e	Millisecond part of the current second 0-999	"678"
      %f	Microsecond part of the current second 0-999999	"056789"
      %F	Nanosecond part of the current second 0-999999999	"256789123"
      %p	AM/PM	"AM"
      %r	12 hour clock	"02:55:02 pm"
      %R	24-hour HH:MM time, equivalent to %H:%M	"23:55"
      %T or %X	ISO 8601 time format (HH:MM:SS), equivalent to %H:%M:%S	"23:55:59"
      %z	ISO 8601 offset from UTC in timezone ([+/-]HH:MM)	"+02:00"
      %E	Seconds since the epoch	"1528834770"
      %i	Message sequence number (disabled by default - edit 'tweakme.h' to enable)	"1154"
      %%	The % sign	"%"
      %+	spdlog's default format	"[2014-10-31 23:46:59.678] [mylogger] [info] Some message"
      %^	start color range	"[mylogger] [info(green)] Some message"
      %$	end color range (for example %^[+++]%$ %v)	[+++] Some message
      %@	Source file and line (use SPDLOG_TRACE(..),SPDLOG_INFO(...) etc.)	my_file.cpp:123
      %s	Source file (use SPDLOG_TRACE(..),SPDLOG_INFO(...) etc.)	my_file.cpp
      %#	Source line (use SPDLOG_TRACE(..),SPDLOG_INFO(...) etc.)	123
      %!	Source function (use SPDLOG_TRACE(..),SPDLOG_INFO(...) etc. see tweakme for pretty-print)	my_func

      // -----> Replaced by %i and SPDLOG_ENABLE_MESSAGE_COUNTER
      // We add a special %N marker which can only appear as the first pattern character if you want to use it.
      // it is used to add unique log line number ( as a %08X number) after the other pattern character and before the beginning of the log line.
      // With this 'ticket' value, you can detect log line buffer overflow as you will have "holes" in the line number sequence if it happens
      -> "%N "); // Can only appears as the first arg
      */
      /*
              mAddLineNumber_B = (LogHeader_S.substr(0, 3) == "%N "); // Can only appears as the first arg
              if (mAddLineNumber_B)
              {
                LogHeader_S = LogHeader_S.substr(3, LogHeader_S.size() - 3);
              }
      */
      psLogger->set_pattern(LogHeader_S.c_str(), spdlog::pattern_time_type::local);
      Rts_E = BOF_ERR_NO_ERROR;
    }
    return Rts_E;
  }
  std::string V_LogHeader()
  {
    std::string Rts_S;
    std::shared_ptr<spdlog::logger> psLogger;
    std::string LogHeader_S;

    psLogger = spdlog::details::registry::instance().get(mLogChannelParam_X.ChannelName_S);
    BOF_ASSERT(psLogger != nullptr);
    if (psLogger)
    {
      Rts_S = mLogChannelCharacteristicCollection[mLogChannelParam_X.ChannelName_S].LogHeader_S;
    }
    return Rts_S;
  }

  BOFERR V_LogChannelPathName(BofPath &_rLogPath)
  {
    BOFERR Rts_E;
    std::shared_ptr<spdlog::logger> psLogger;

    psLogger = spdlog::details::registry::instance().get(mLogChannelParam_X.ChannelName_S);
    BOF_ASSERT(psLogger != nullptr);
    Rts_E = BOF_ERR_INTERNAL;
    if (psLogger)
    {
      // #pragma message("Please fix me V_LogChannelPathName")
      _rLogPath = mLogChannelCharacteristicCollection[mLogChannelParam_X.ChannelName_S].LogPath;
      Rts_E = BOF_ERR_NO_ERROR;
    }
    return Rts_E;
  }

  BOFERR V_DeleteLogStorage()
  {
    BOFERR Rts_E;
    Rts_E = V_Remove(); // To be able to  delete active file...
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      /*
      std::shared_ptr< spdlog::logger > psLogger;
      int y = psLogger.use_count();
      psLogger=nullptr;
      y = psLogger.use_count();
      */
      spdlog::drop(mLogChannelParam_X.ChannelName_S);
      DeleteLogStorage();
      Rts_E = V_Add(mLogChannelParam_X);
    }
    return Rts_E;
  }

private:
  BOFERR DeleteLogStorage()
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;
    std::vector<BOF_FILE_FOUND> ListOfFile_X;
    uint32_t i_U32;
    std::string Pattern_S;
    //	std::shared_ptr< spdlog::logger > psLogger;
    //	BofPath DailyPath;

    if (mLogChannelParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_RAM_CIRCULAR_BUFFER)
    {
      Rts_E = mpsCircularBufferSink ? mpsCircularBufferSink->Reset() : BOF_ERR_CLOSE;
    }
    if ((mLogChannelParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_FILE) || (mLogChannelParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_DAILYFILE))
    {
      Rts_E = BOF_ERR_EINVAL;
      if (mLogChannelParam_X.FileLogPath.FullPathName(false) != "")
      {
        /*
        if (mLogParam_X.LogSink_E == BOF_LOG_CHANNEL_SINK::TO_DAILYFILE)
        {
        psLogger = spdlog::details::registry::instance().get(mLogParam_X.ChannelName_S);
        BOF_ASSERT(psLogger != nullptr);
        Rts_E = BOF_ERR_INTERNAL;
        if (psLogger)
        {
        DailyPath = psLogger->GetCurrentDailyFilename();
        Pattern_S = DailyPath.FileNameWithoutExtension() + "*." + mLogParam_X.FileLogPath.Extension();  *. to delete rotating file file.log->file.1.log file.2.log ...
        Rts_E = BOF_ERR_NO_ERROR;
        }
        }
        else
        */

        //			TO_DAILYFILE must delete..

        //			test limited TO_DAILYFILE
        {
          Pattern_S = mLogChannelParam_X.FileLogPath.FileNameWithoutExtension() + "*." + mLogChannelParam_X.FileLogPath.Extension(); //*. to delete rotating file file.log->file.1.log file.2.log ...
          Rts_E = BOF_ERR_NO_ERROR;
        }

        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          Rts_E = Bof_FindFile(mLogChannelParam_X.FileLogPath.DirectoryName(false, false), Pattern_S, BOF_FILE_TYPE::BOF_FILE_REG, false, 0xFFFFFFFF, ListOfFile_X);
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            for (i_U32 = 0; i_U32 < ListOfFile_X.size(); i_U32++)
            {
              Bof_DeleteFile(ListOfFile_X[i_U32].Path);
            }
          }
        }
      }
    }
    return Rts_E;
  }
};

// Opaque pointer design pattern: ... set Implementation values ...
BofLogChannelSpdLog::BofLogChannelSpdLog() : mpuBofLogChannelSpdLogImplementation(new BofLogChannelSpdLogImplementation())
{
}

BofLogChannelSpdLog::~BofLogChannelSpdLog()
{
}

BOFERR BofLogChannelSpdLog::V_Add(const BOF_LOG_CHANNEL_PARAM &_rLogParam_X)
{
  return mpuBofLogChannelSpdLogImplementation->V_Add(_rLogParam_X);
}

BOFERR BofLogChannelSpdLog::V_Remove()
{
  return mpuBofLogChannelSpdLogImplementation->V_Remove();
}

BOFERR BofLogChannelSpdLog::V_Log(BOF_LOG_CHANNEL_LEVEL _Level_E, const std::string &_rLogMsg_S)
{
  BOFERR Rts_E = BOF_ERR_NO;

  if (ShouldLog(_Level_E))
  {
    Rts_E = mpuBofLogChannelSpdLogImplementation->V_Log(_Level_E, _rLogMsg_S);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      ResetLastLogTimeInMs();
    }
  }
  return Rts_E;
}

BOFERR BofLogChannelSpdLog::V_Flush()
{
  return mpuBofLogChannelSpdLogImplementation->V_Flush();
}

BOFERR BofLogChannelSpdLog::V_LogLevel(BOF_LOG_CHANNEL_LEVEL _LogLevel_E) const
{
  return mpuBofLogChannelSpdLogImplementation->V_LogLevel(_LogLevel_E);
}

BOFERR BofLogChannelSpdLog::V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL _LogLevel_E, BOF_LOG_LEVEL_COLOR _LogLevelColor_E) const
{
  return mpuBofLogChannelSpdLogImplementation->V_LogLevelColor(_LogLevel_E, _LogLevelColor_E);
}
BOF_LOG_LEVEL_COLOR BofLogChannelSpdLog::V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL _LogLevel_E) const
{
  return mpuBofLogChannelSpdLogImplementation->V_LogLevelColor(_LogLevel_E);
}
BOFERR BofLogChannelSpdLog::V_LogHeader(const std::string &_rPattern_S) const
{
  return mpuBofLogChannelSpdLogImplementation->V_LogHeader(_rPattern_S);
}
std::string BofLogChannelSpdLog::V_LogHeader() const
{
  return mpuBofLogChannelSpdLogImplementation->V_LogHeader();
}
BOFERR BofLogChannelSpdLog::V_LogChannelPathName(BofPath &_rLogPath)
{
  return mpuBofLogChannelSpdLogImplementation->V_LogChannelPathName(_rLogPath);
}

BOFERR BofLogChannelSpdLog::V_OpenLogStorage(uint32_t _Offset_U32)
{
  return mpuBofLogChannelSpdLogImplementation->V_OpenLogStorage(_Offset_U32);
}

BOFERR BofLogChannelSpdLog::V_ReadLogStorage(uint32_t _MaxLineToExtract_U32, std::vector<std::string> &_rExtractedLogLine_S) const
{
  return mpuBofLogChannelSpdLogImplementation->V_ReadLogStorage(_MaxLineToExtract_U32, _rExtractedLogLine_S);
}

BOFERR BofLogChannelSpdLog::V_CloseLogStorage()
{
  return mpuBofLogChannelSpdLogImplementation->V_CloseLogStorage();
}

BOFERR BofLogChannelSpdLog::V_DeleteLogStorage()
{
  return mpuBofLogChannelSpdLogImplementation->V_DeleteLogStorage();
}
END_BOF_NAMESPACE()
