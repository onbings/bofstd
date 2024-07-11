/*
 * Copyright (c) 2024-2044, EVS Broadcast Equipment S.A. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module define the a logger factory to manage logger instance
 *
 * Author:      Bernard HARMEL: b.harmel@evs.com
 *
 * History:
 * V 1.00  Feb 19 2024  BHA : Initial release
 */
#pragma once
#include <memory>
#include <string>
#include <vector>
#include <cstdarg>

#include "bofstd/bofstd.h"

BEGIN_BOF_NAMESPACE()
class IBofLogger
{
public:
  enum LogSeverity : uint32_t
  {
    LOG_SEVERITY_FORCE = 0,
    LOG_SEVERITY_ERROR,
    LOG_SEVERITY_WARNING,
    LOG_SEVERITY_INFO,
    LOG_SEVERITY_VERBOSE,
    LOG_SEVERITY_DEBUG,
    LOG_SEVERITY_MAX
  };

  IBofLogger() = default;
  virtual ~IBofLogger() = default;
  virtual void V_Log(LogSeverity _SeverityLevel_E, const char *_pLogMessage_c, ...) = 0;
  inline bool SetLogSeverityLevel(LogSeverity _SeverityLevel_E)
  {
    bool Rts_B = false;
    if (_SeverityLevel_E < LOG_SEVERITY_MAX)
    {
      mLogSeverityLevel_E = _SeverityLevel_E;
      Rts_B = true;
    }
    return Rts_B;
  }
  inline LogSeverity GetLogSeverityLevel() const
  {
    return mLogSeverityLevel_E;
  }
  // For ut
  uint32_t mNbLogOut_U32 = 0;
  uint32_t mNbLogRejected_U32 = 0;

private:
  LogSeverity mLogSeverityLevel_E = LogSeverity::LOG_SEVERITY_MAX;
};

class IBofLoggerFactory
{
public:
  IBofLoggerFactory() = default;
  virtual ~IBofLoggerFactory() = default;
  virtual std::shared_ptr<BOF::IBofLogger> V_Create(const std::string &_rLibNamePrefix_S, const std::string &_rLoggerChannelName_S) = 0;
  virtual std::shared_ptr<BOF::IBofLogger> V_GetLogger(const std::string &_rLibNamePrefix_S, const std::string &_rLoggerChannelName_S) = 0;
  virtual bool V_Destroy(const std::string &_rLibNamePrefix_S, const std::string &_rLoggerChannelName_S) = 0;
};

#define LOGGER_LOG(psLogger, level, format, ...)                                                                                                               \
  if (psLogger)                                                                                                                                                \
  {                                                                                                                                                            \
    if (level <= psLogger->GetLogSeverityLevel())                                                                                                              \
    {                                                                                                                                                          \
      psLogger->V_Log(level, format, ##__VA_ARGS__);                                                                                                           \
      psLogger->mNbLogOut_U32++;                                                                                                                               \
    }                                                                                                                                                          \
    else                                                                                                                                                       \
    {                                                                                                                                                          \
      psLogger->mNbLogRejected_U32++;                                                                                                                          \
    }                                                                                                                                                          \
  }

#ifdef LOGGER_FACTORY_DISABLE_LOGGING
#define LOG_FORCE(psLogger, format, ...)
#define LOG_ERROR(psLogger, format, ...)
#define LOG_WARNING(psLogger, format, ...)
#define LOG_INFO(psLogger, format, ...)
#define LOG_VERBOSE(psLogger, format, ...)
#define LOG_DEBUG(psLogger, format, ...)
#else
#define LOG_FORCE(psLogger, format, ...) LOGGER_LOG(psLogger, BOF::IBofLogger::LOG_SEVERITY_FORCE, format, ##__VA_ARGS__)
#define LOG_ERROR(psLogger, format, ...) LOGGER_LOG(psLogger, BOF::IBofLogger::LOG_SEVERITY_ERROR, format, ##__VA_ARGS__)
#define LOG_WARNING(psLogger, format, ...) LOGGER_LOG(psLogger, BOF::IBofLogger::LOG_SEVERITY_WARNING, format, ##__VA_ARGS__)
#define LOG_INFO(psLogger, format, ...) LOGGER_LOG(psLogger, BOF::IBofLogger::LOG_SEVERITY_INFO, format, ##__VA_ARGS__)
#define LOG_VERBOSE(psLogger, format, ...) LOGGER_LOG(psLogger, BOF::IBofLogger::LOG_SEVERITY_VERBOSE, format, ##__VA_ARGS__)
#define LOG_DEBUG(psLogger, format, ...) LOGGER_LOG(psLogger, BOF::IBofLogger::LOG_SEVERITY_DEBUG, format, ##__VA_ARGS__)
#endif
END_BOF_NAMESPACE()
