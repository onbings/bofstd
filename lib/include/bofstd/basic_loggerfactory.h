/*
 * Copyright (c) 2024-2044, EVS Broadcast Equipment S.A. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module define a logger factory to manage logger instance
 *
 * Author:      Bernard HARMEL: b.harmel@evs.com
 *
 * History:
 * V 1.00  Feb 19 2024  BHA : Initial release
 */
#pragma once
#include <chrono>
#include <ctime>
#include <string.h>
#include <evs-hwfw-logger/factory/iloggerfactory.h> 

BEGIN_HWFWLOG_NAMESPACE()
class BasicLogger : public HWFWLOG::ILogger
{
public:
  BasicLogger(const uint32_t _ChannelIndex_U32, const std::string &_rChannelName_S)
      : HWFWLOG::ILogger()
  {
      mChannelIndex_U32 = _ChannelIndex_U32;
      mChannelName_S = _rChannelName_S;
      Configure(false, "");
  }
  virtual ~BasicLogger()
  {
    if (mpLogFile_X)
    {
      fclose(mpLogFile_X);
      mpLogFile_X = nullptr;
    }
  }
  void V_Log(LogSeverity /*_SeverityLevel_E*/, const char *_pLogMessage_c, ...) override
  {
    if ((mOutputOnScreen_B) || (mpLogFile_X))
    {
      char pHeader_c[0x100], pLog_c[0x1000];
      va_list VaList_X;
      const auto Now = std::chrono::high_resolution_clock::now();
      const std::chrono::duration<double> Delta = Now - mLogEpoch;
      const auto DeltaInuS = std::chrono::duration_cast<std::chrono::microseconds>(Delta).count();
      char pDateTime_c[128];
      std::time_t t;

      va_start(VaList_X, _pLogMessage_c);
      vsnprintf(pLog_c, sizeof(pLog_c), _pLogMessage_c, VaList_X);
      va_end(VaList_X);
      pLog_c[sizeof(pLog_c) - 1] = 0;

      t = std::time(nullptr);
      std::strftime(pDateTime_c, sizeof(pDateTime_c), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
      sprintf(pHeader_c, "%s: %zd Channel[%d][%s]->",  pDateTime_c, DeltaInuS, mChannelIndex_U32, mChannelName_S.c_str());
      if (mOutputOnScreen_B)
      {
        printf("%s%s", pHeader_c, pLog_c);
      }
      if (mpLogFile_X)
      {
        fwrite(pHeader_c, strlen(pHeader_c), 1, mpLogFile_X);
        fwrite(pLog_c, strlen(pLog_c), 1, mpLogFile_X);
      }
    }
  }
  void Configure(bool _OutputOnScreen_B, const std::string &_rLogFileSubDir_S)
  {
    char pLogFile_c[512];

    mOutputOnScreen_B = _OutputOnScreen_B;
    if (_rLogFileSubDir_S.empty())
    {
      mpLogFile_X = nullptr;
    }
    else
    {
      sprintf(pLogFile_c, "%s/%s_%03d.log", _rLogFileSubDir_S.c_str(), mChannelName_S.c_str(), mChannelIndex_U32);
      mpLogFile_X = fopen(pLogFile_c, "w+");
    }
  }

private:
  uint32_t mChannelIndex_U32 = 0;
  std::string mChannelName_S;
  const std::chrono::time_point<std::chrono::high_resolution_clock> mLogEpoch=std::chrono::high_resolution_clock::now();
  bool mOutputOnScreen_B = false;
  FILE *mpLogFile_X = nullptr;
};

class BasicLoggerFactory : public HWFWLOG::ILoggerFactory
{
public:
  BasicLoggerFactory(bool _OutputOnScreen_B, const std::string &_rLogFileSubDir_S)
      : mOutputOnScreen_B(_OutputOnScreen_B), mLogFileSubDir_S(_rLogFileSubDir_S)
  {
  }
  virtual ~BasicLoggerFactory() = default;

  std::shared_ptr<HWFWLOG::ILogger> V_Create(const uint32_t _ChannelIndex_U32, const uint32_t _MaxChannelIndex_U32, const std::string &_rChannelName_S) override
  {
    std::shared_ptr<BasicLogger> psRts = nullptr;
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
      psRts = std::make_shared<BasicLogger>(_ChannelIndex_U32, _rChannelName_S);
      if (psRts)
      {
        psRts->Configure(mOutputOnScreen_B, mLogFileSubDir_S);
        mLoggerCollection[_ChannelIndex_U32] = psRts;
      }
    }
    return psRts;
  }
  bool V_SetLogSeverityLevel(const uint32_t _ChannelIndex_U32, HWFWLOG::ILogger::LogSeverity _SeverityLevel_E) override
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
  HWFWLOG::ILogger::LogSeverity V_GetLogSeverityLevel(const uint32_t _ChannelIndex_U32) const override
  {
    HWFWLOG::ILogger::LogSeverity Rts_E = HWFWLOG::ILogger::LogSeverity::LOG_SEVERITY_MAX;

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
  std::vector<std::shared_ptr<BasicLogger>> mLoggerCollection;
  bool mOutputOnScreen_B;
  const std::string mLogFileSubDir_S;
};
END_HWFWLOG_NAMESPACE()
