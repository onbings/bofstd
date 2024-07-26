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
#include <cstdarg>
#include <memory>
#include <string>
#include <vector>

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
  virtual void V_Log(LogSeverity _SeverityLevel_E, const std::string &_rFile_S, uint32_t _Line_U32, const std::string &_rFunc_S, const char *_pLogMessage_c, ...) = 0;
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

#define LOGGER_LOG(psLogger, Level, File, Line, Func, Format, ...)     \
  if (psLogger)                                                        \
  {                                                                    \
    if (Level <= psLogger->GetLogSeverityLevel())                      \
    {                                                                  \
      psLogger->V_Log(Level, File, Line, Func, Format, ##__VA_ARGS__); \
      psLogger->mNbLogOut_U32++;                                       \
    }                                                                  \
    else                                                               \
    {                                                                  \
      psLogger->mNbLogRejected_U32++;                                  \
    }                                                                  \
  }

#ifdef LOGGER_FACTORY_DISABLE_LOGGING
#define LOG_FORCE(psLogger, Format, ...)
#define LOG_ERROR(psLogger, Format, ...)
#define LOG_WARNING(psLogger, Format, ...)
#define LOG_INFO(psLogger, Format, ...)
#define LOG_VERBOSE(psLogger, Format, ...)
#define LOG_DEBUG(psLogger, Format, ...)
#else
#define LOG_FORCE(psLogger, Format, ...) LOGGER_LOG(psLogger, BOF::IBofLogger::LOG_SEVERITY_FORCE, __FILE__, __LINE__, __func__, Format, ##__VA_ARGS__)
#define LOG_ERROR(psLogger, Format, ...) LOGGER_LOG(psLogger, BOF::IBofLogger::LOG_SEVERITY_ERROR, __FILE__, __LINE__, __func__, Format, ##__VA_ARGS__)
#define LOG_WARNING(psLogger, Format, ...) LOGGER_LOG(psLogger, BOF::IBofLogger::LOG_SEVERITY_WARNING, __FILE__, __LINE__, __func__, Format, ##__VA_ARGS__)
#define LOG_INFO(psLogger, Format, ...) LOGGER_LOG(psLogger, BOF::IBofLogger::LOG_SEVERITY_INFO, __FILE__, __LINE__, __func__, Format, ##__VA_ARGS__)
#define LOG_VERBOSE(psLogger, Format, ...) LOGGER_LOG(psLogger, BOF::IBofLogger::LOG_SEVERITY_VERBOSE, __FILE__, __LINE__, __func__, Format, ##__VA_ARGS__)
#define LOG_DEBUG(psLogger, Format, ...) LOGGER_LOG(psLogger, BOF::IBofLogger::LOG_SEVERITY_DEBUG, __FILE__, __LINE__, __func__, Format, ##__VA_ARGS__)
#endif
END_BOF_NAMESPACE()
