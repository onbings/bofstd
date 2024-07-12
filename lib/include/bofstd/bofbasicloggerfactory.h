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
#include <map>
#include <mutex>
#include <string.h>
#include <bofstd/ibofloggerfactory.h>

BEGIN_BOF_NAMESPACE()
class BasicLogger : public BOF::IBofLogger
{
public:
  BasicLogger(const std::string &_rLibNamePrefix_S, const std::string &_rLoggerChannelName_S) : BOF::IBofLogger()
  {
    mChannelName_S = _rLibNamePrefix_S + _rLoggerChannelName_S;
    Open(false, false, false, "");
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
      sprintf(pHeader_c, "%s: %zd [%s]->", pDateTime_c, DeltaInuS, mChannelName_S.c_str());
      if (mOutputOnScreen_B)
      {
        printf("%s%s", pHeader_c, pLog_c);
      }
      if (mpLogFile_X)
      {
        fwrite(pHeader_c, strlen(pHeader_c), 1, mpLogFile_X);
        fwrite(pLog_c, strlen(pLog_c), 1, mpLogFile_X);
        if (mAutoFlush_B)
        {
          Flush();
        }
      }
    }
  }
  bool Open(bool _OutputOnScreen_B, bool _Append_B, bool _AutoFlush_B, const std::string &_rLogFileSubDir_S)
  {
    bool Rts_B = true;
    char pLogFile_c[512];

    mOutputOnScreen_B = _OutputOnScreen_B;
    mAutoFlush_B = _AutoFlush_B;
    if (_rLogFileSubDir_S.empty())
    {
      mpLogFile_X = nullptr;
    }
    else
    {
      sprintf(pLogFile_c, "%s/%s.log", _rLogFileSubDir_S.c_str(), mChannelName_S.c_str());
      mpLogFile_X = _Append_B ? fopen(pLogFile_c, "w+"): fopen(pLogFile_c, "a+");
      if (mpLogFile_X == nullptr)
      {
        V_Log(LOG_SEVERITY_FORCE, "New log session started...\n");
        Rts_B = false;
      }
    }
    return Rts_B;
  }
  bool Close()
  {
    bool Rts_B = true;

    if (mpLogFile_X)
    {
      V_Log(LOG_SEVERITY_FORCE, "Log session finished !\n");
      fclose(mpLogFile_X);
      mpLogFile_X = nullptr;
    }
    mOutputOnScreen_B = false;

    return Rts_B;
  }

  uint64_t Size()
  {
    uint64_t Rts_U64=0; //CurrentPosition_U64

    if (mpLogFile_X)
    {
      //CurrentPosition_U64 = ftell(mpLogFile_X);
      //fseek(mpLogFile_X, 0, SEEK_END);
      Rts_U64 = ftell(mpLogFile_X);
      //fseek(mpLogFile_X, CurrentPosition_U64, SEEK_SET);
    }
    return Rts_U64;
  }
  bool Flush()
  {
    bool Rts_B = true;

    if (mpLogFile_X)
    {
      fflush(mpLogFile_X);
    }
    return Rts_B;
  }
private:
  std::string mChannelName_S;
  const std::chrono::time_point<std::chrono::high_resolution_clock> mLogEpoch = std::chrono::high_resolution_clock::now();
  bool mOutputOnScreen_B = false;
  bool mAutoFlush_B = false;
  FILE *mpLogFile_X = nullptr;
};

class BofBasicLoggerFactory : public BOF::IBofLoggerFactory
{
public:
  BofBasicLoggerFactory(bool _OutputOnScreen_B, bool _Append_B, bool _AutoFlush_B, const std::string &_rLogFileSubDir_S) : 
    mOutputOnScreen_B(_OutputOnScreen_B),  mAppend_B(_Append_B),  mAutoFlush_B(_AutoFlush_B),  mLogFileSubDir_S(_rLogFileSubDir_S)
  {
  }
  virtual ~BofBasicLoggerFactory() = default;

  std::string BuildChannelName(const std::string &_rLibNamePrefix_S, const std::string &_rLoggerChannelName_S)
  {
    return _rLibNamePrefix_S + _rLoggerChannelName_S;
  }
  std::shared_ptr<BOF::IBofLogger> V_Create(const std::string &_rLibNamePrefix_S, const std::string &_rLoggerChannelName_S) override
  {
    std::shared_ptr<BasicLogger> psRts = nullptr;
    std::string ChannelName_S;

    std::lock_guard<std::mutex> Lock(mLoggerCollectionMtx);
    if (V_GetLogger(_rLibNamePrefix_S, _rLoggerChannelName_S) == nullptr)
    {
      psRts = std::make_shared<BasicLogger>(_rLibNamePrefix_S, _rLoggerChannelName_S);
      if (psRts)
      {
        if (psRts->Open(mOutputOnScreen_B, mAppend_B, mAutoFlush_B, mLogFileSubDir_S))
        {
          ChannelName_S = BuildChannelName(_rLibNamePrefix_S, _rLoggerChannelName_S);
          mLoggerCollection[ChannelName_S] = psRts;
        }
        else
        {
          psRts = nullptr;
        }
      }
    }
    return psRts;
  }
  std::shared_ptr<BOF::IBofLogger> V_GetLogger(const std::string &_rLibNamePrefix_S, const std::string &_rLoggerChannelName_S) override
  {
    std::shared_ptr<BasicLogger> psRts = nullptr;
    std::string ChannelName_S;
    // no mutex as it is used by V_Create and V_Destroy
    ChannelName_S = BuildChannelName(_rLibNamePrefix_S, _rLoggerChannelName_S);
    auto It = mLoggerCollection.find(ChannelName_S);
    if (It != mLoggerCollection.end())
    {
      psRts = It->second;
    }
    return psRts;
  }
  bool V_Destroy(const std::string &_rLibNamePrefix_S, const std::string &_rLoggerChannelName_S) override
  {
    bool Rts_B = false;
    std::string ChannelName_S;

    std::lock_guard<std::mutex> Lock(mLoggerCollectionMtx);
    if (V_GetLogger(_rLibNamePrefix_S, _rLoggerChannelName_S) != nullptr)
    {
      ChannelName_S = BuildChannelName(_rLibNamePrefix_S, _rLoggerChannelName_S);
      mLoggerCollection.erase(ChannelName_S);
      Rts_B = true;
    }
    return Rts_B;
  }

private:
  std::mutex mLoggerCollectionMtx;
  std::map<std::string, std::shared_ptr<BasicLogger>> mLoggerCollection;
  bool mOutputOnScreen_B;
  bool mAppend_B;
  bool mAutoFlush_B;
  const std::string mLogFileSubDir_S;
};
END_BOF_NAMESPACE()
