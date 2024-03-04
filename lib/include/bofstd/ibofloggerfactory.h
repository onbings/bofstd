/*
 * Copyright (c) 2024-2044, EVS Broadcast Equipment S.A. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module define the different logger interfaces
 *
 * Author:      Bernard HARMEL: b.harmel@evs.com
 *
 * History:
 * V 1.00  Feb 19 2024  BHA : Initial release
 */
#pragma once
#include <bofstd/bofstd.h>
#include <memory>

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
  virtual ~IBofLoggerFactory() = default;
  virtual std::shared_ptr<IBofLogger> V_Create(const uint32_t _ChannelIndex_U32, const uint32_t _MaxChannelIndex_U32, const std::string &_rChannelName_S) = 0;
  virtual bool V_SetLogSeverityLevel(const uint32_t _ChannelIndex_U32, IBofLogger::LogSeverity _SeverityLevel_E) = 0;
  virtual IBofLogger::LogSeverity V_GetLogSeverityLevel(const uint32_t _ChannelIndex_U32) const = 0;
};

struct BOF_LOGGER_FACTORY_STORAGE
{
  uint32_t NbMaxChannel_U32;
  std::vector<std::shared_ptr<BOF::IBofLogger>> LoggerCollection;
  BOF_LOGGER_FACTORY_STORAGE(uint32_t _NbMaxChannel_U32)
  {
    NbMaxChannel_U32 = _NbMaxChannel_U32;
    Reset();
  }
  void Reset()
  {
    uint32_t i_U32;
    LoggerCollection.clear();
    LoggerCollection.reserve(NbMaxChannel_U32);
    for (i_U32 = 0; i_U32 < NbMaxChannel_U32; i_U32++)
    {
      LoggerCollection.push_back(nullptr);
    }
  }
};
// #define MY_LOGGER TheBhaLogger
// BOF_LOGGER_DEFINE_STORAGE(MY_LOGGER, LOGGER_CHANNEL_MAX);
//==>define BOF::BOF_LOGGER_FACTORY_STORAGE GL_LoggerStorage_TheBhaLogger(LOGGER_CHANNEL_MAX)
#define BOF_LOGGER_STORAGE_NAME(name) GL_LoggerStorage_##name
#define BOF_LOGGER_DEFINE_STORAGE(name, nbmaxchannel) BOF::BOF_LOGGER_FACTORY_STORAGE BOF_LOGGER_STORAGE_NAME(name)(nbmaxchannel)

#define BOF_LOGGER_FACTORY_ADD(name, logger, channel, sts)            \
  if (channel < BOF_LOGGER_STORAGE_NAME(name).NbMaxChannel_U32)       \
  {                                                                   \
    BOF_LOGGER_STORAGE_NAME(name).LoggerCollection[channel] = logger; \
    sts = true;                                                       \
  }                                                                   \
  else                                                                \
  {                                                                   \
    sts = false;                                                      \
  }
#define BOF_LOGGER_FACTORY_GET(name, channel, logger)                 \
  if (channel < BOF_LOGGER_STORAGE_NAME(name).NbMaxChannel_U32)       \
  {                                                                   \
    logger = BOF_LOGGER_STORAGE_NAME(name).LoggerCollection[channel]; \
  }                                                                   \
  else                                                                \
  {                                                                   \
    logger = nullptr;                                                 \
  }
// The first if could be replaced by assert...???...
#define BOF_LOGGER_LOG(name, channel, level, format, ...)                                             \
  if (channel < BOF_LOGGER_STORAGE_NAME(name).NbMaxChannel_U32)                                       \
  {                                                                                                   \
    if (BOF_LOGGER_STORAGE_NAME(name).LoggerCollection[channel])                                      \
    {                                                                                                 \
      if (level <= BOF_LOGGER_STORAGE_NAME(name).LoggerCollection[channel]->GetLogSeverityLevel())    \
      {                                                                                               \
        BOF_LOGGER_STORAGE_NAME(name).LoggerCollection[channel]->V_Log(level, format, ##__VA_ARGS__); \
        BOF_LOGGER_STORAGE_NAME(name).LoggerCollection[channel]->mNbLogOut_U32++;                     \
      }                                                                                               \
      else                                                                                            \
      {                                                                                               \
        BOF_LOGGER_STORAGE_NAME(name).LoggerCollection[channel]->mNbLogRejected_U32++;                \
      }                                                                                               \
    }                                                                                                 \
  }

#ifdef BOF_LOGGER_FACTORY_DISABLE
#define BOF_LOG_FORCE(name, channel, format, ...)
#define BOF_LOG_ERROR(name, channel, format, ...)
#define BOF_LOG_WARNING(name, channel, format, ...)
#define BOF_LOG_INFO(name, channel, format, ...)
#define BOF_LOG_VERBOSE(name, channel, format, ...)
#define BOF_LOG_DEBUG(name, channel, format, ...)
#else
#define BOF_LOG_FORCE(name, channel, format, ...) BOF_LOGGER_LOG(name, channel, BOF::IBofLogger::LOG_SEVERITY_FORCE, format, ##__VA_ARGS__)
#define BOF_LOG_ERROR(name, channel, format, ...) BOF_LOGGER_LOG(name, channel, BOF::IBofLogger::LOG_SEVERITY_ERROR, format, ##__VA_ARGS__)
#define BOF_LOG_WARNING(name, channel, format, ...) BOF_LOGGER_LOG(name, channel, BOF::IBofLogger::LOG_SEVERITY_WARNING, format, ##__VA_ARGS__)
#define BOF_LOG_INFO(name, channel, format, ...) BOF_LOGGER_LOG(name, channel, BOF::IBofLogger::LOG_SEVERITY_INFO, format, ##__VA_ARGS__)
#define BOF_LOG_VERBOSE(name, channel, format, ...) BOF_LOGGER_LOG(name, channel, BOF::IBofLogger::LOG_SEVERITY_VERBOSE, format, ##__VA_ARGS__)
#define BOF_LOG_DEBUG(name, channel, format, ...) BOF_LOGGER_LOG(name, channel, BOF::IBofLogger::LOG_SEVERITY_DEBUG, format, ##__VA_ARGS__)
#endif
END_BOF_NAMESPACE()

// Usage example:
#if 0
//*********** In the app ******************************************************************************************

#include <bofstd/ibofloggerfactory.h>
#include <glf/glf.h>
#include <stdarg.h>

BEGIN_UCO_NAMESPACE()

class Logger : public BOF::IBofLogger
{
public:
  Logger(const uint32_t _ChannelIndex_U32, const std::string &_rChannelName_S)
    : BOF::IBofLogger()
  {
    char pLogFile_c[256];

    mChannelIndex_U32 = _ChannelIndex_U32;
    mChannelName_S = _rChannelName_S;

    sprintf(pLogFile_c, "%s_%03d.log", _rChannelName_S.c_str(), mChannelIndex_U32);
    mpLogFile_X = fopen(pLogFile_c, "w+");
  }
  virtual ~Logger()
  {
    if (mpLogFile_X)
    {
      fclose(mpLogFile_X);
      mpLogFile_X = nullptr;
    }
  }
  void V_Log(LogSeverity _SeverityLevel_E, const char *_pLogMessage_c, ...) override
  {
    char pLog_c[0x1000];
    va_list VaList_X;

    va_start(VaList_X, _pLogMessage_c);
    vsnprintf(pLog_c, sizeof(pLog_c), _pLogMessage_c, VaList_X);
    va_end(VaList_X);

    pLog_c[sizeof(pLog_c) - 1] = 0;
    printf("Channel[%d][%s]->%s", mChannelIndex_U32, mChannelName_S.c_str(), pLog_c);
    fwrite(pLog_c, strlen(pLog_c), 1, mpLogFile_X);
  }

private:
  uint32_t mChannelIndex_U32 = 0;
  std::string mChannelName_S;
  FILE *mpLogFile_X = nullptr;
};

class LoggerFactory : public BOF::IBofLoggerFactory
{
public:
  LoggerFactory() = default;
  virtual ~LoggerFactory() = default;

  std::shared_ptr<BOF::IBofLogger> V_Create(const uint32_t _ChannelIndex_U32, const uint32_t _MaxChannelIndex_U32, const std::string &_rChannelName_S) override
  {
    std::shared_ptr<Logger> psRts = nullptr;
    uint32_t i_U32;

    if (_ChannelIndex_U32 < _MaxChannelIndex_U32)
    {
      if (mLoggerCollection.size() != _MaxChannelIndex_U32)
      {
        mLoggerCollection.clear();
        for (i_U32 = 0; i_U32 < _MaxChannelIndex_U32; i_U32++)
        {
          mLoggerCollection.push_back(nullptr);
        }
      }
      psRts = std::make_shared<Logger>(_ChannelIndex_U32, _rChannelName_S);
      mLoggerCollection[_ChannelIndex_U32] = psRts;
    }
    return psRts;
  }
  bool V_SetLogSeverityLevel(const uint32_t _ChannelIndex_U32, BOF::IBofLogger::LogSeverity _SeverityLevel_E) override
  {
    bool Rts_B = false;

    if (_ChannelIndex_U32 < mLoggerCollection.size())
    {
      if (mLoggerCollection[_ChannelIndex_U32])
      {
        Rts_B = mLoggerCollection[_ChannelIndex_U32]->SetLogSeverityLevel(_SeverityLevel_E);
      }
    }
    return Rts_B;
  }
  BOF::IBofLogger::LogSeverity V_GetLogSeverityLevel(const uint32_t _ChannelIndex_U32) const override
  {
    BOF::IBofLogger::LogSeverity Rts_E = BOF::IBofLogger::LogSeverity::LOG_SEVERITY_MAX;

    if (_ChannelIndex_U32 < mLoggerCollection.size())
    {
      if (mLoggerCollection[_ChannelIndex_U32])
      {
        Rts_E = mLoggerCollection[_ChannelIndex_U32]->GetLogSeverityLevel();
      }
    }
    return Rts_E;
  }

private:
  std::vector<std::shared_ptr<Logger>> mLoggerCollection;
};

END_UCO_NAMESPACE()

int main(int _Argc_i, char *_pArgv_c[])
{
  int Rts_i = 0;
  std::shared_ptr<UCO::LoggerFactory> psLoggerFactory = std::make_shared<UCO::LoggerFactory>();
  GLF::GlfInit(psLoggerFactory);
  psLoggerFactory->V_SetLogSeverityLevel(GLF::LOGGER_CHANNEL_INIT, BOF::IBofLogger::LOG_SEVERITY_DEBUG);
  psLoggerFactory->V_SetLogSeverityLevel(GLF::LOGGER_CHANNEL_CODEC, BOF::IBofLogger::LOG_SEVERITY_DEBUG);
  psLoggerFactory->V_SetLogSeverityLevel(GLF::LOGGER_CHANNEL_DMA, BOF::IBofLogger::LOG_SEVERITY_DEBUG);

  GLF::GlfFct();

  return Rts_i;
}
//*********** In the lib ******************************************************************************************
//*** .h:
BEGIN_GLF_NAMESPACE()
enum LOGGER_CHANNEL : uint32_t
{
  LOGGER_CHANNEL_INIT = 0,
  LOGGER_CHANNEL_CODEC,
  LOGGER_CHANNEL_DMA,
  LOGGER_CHANNEL_MAX
};
#define MY_LOGGER TheBhaLogger
#define WLOG(channel, ...) BOF_LOG_WARNING(MY_LOGGER, channel, ##__VA_ARGS__);

bool GlfInit(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory);
void GlfFct();

END_GLF_NAMESPACE()

//*** .cpp
BEGIN_GLF_NAMESPACE()

BOF_LOGGER_DEFINE_STORAGE(MY_LOGGER, LOGGER_CHANNEL_MAX);

bool GlfInit(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory)
{
  bool Rts_B = false;
  std::shared_ptr<BOF::IBofLogger> psLogger;

  if (_psLoggerFactory)
  {
    psLogger = _psLoggerFactory->V_Create(LOGGER_CHANNEL_INIT, LOGGER_CHANNEL_MAX, "INIT");
    BOF_LOGGER_FACTORY_ADD(MY_LOGGER, psLogger, LOGGER_CHANNEL_INIT, Rts_B);

    psLogger = _psLoggerFactory->V_Create(LOGGER_CHANNEL_CODEC, LOGGER_CHANNEL_MAX, "CODEC");
    BOF_LOGGER_FACTORY_ADD(MY_LOGGER, psLogger, LOGGER_CHANNEL_CODEC, Rts_B);

    psLogger = _psLoggerFactory->V_Create(LOGGER_CHANNEL_DMA, LOGGER_CHANNEL_MAX, "DMA");
    BOF_LOGGER_FACTORY_ADD(MY_LOGGER, psLogger, LOGGER_CHANNEL_DMA, Rts_B);
  }
  BOF_LOG_WARNING(MY_LOGGER, LOGGER_CHANNEL_INIT, "Log me Rts %d Ptr %p\n", Rts_B, psLogger.get());

  return Rts_B;
}

void GlfFct()
{
  int i = 1;
  WLOG(LOGGER_CHANNEL_INIT, "Warning from init, i is %d\n", i++);
  WLOG(LOGGER_CHANNEL_CODEC, "Warning from codec, i is %d\n", i++);
  WLOG(LOGGER_CHANNEL_DMA, "Warning from dma, i is %d\n", i++);
}
END_GLF_NAMESPACE()

#endif