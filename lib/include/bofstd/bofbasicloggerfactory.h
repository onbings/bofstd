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
#include <bofstd/ibofloggerfactory.h>
#include <bofstd/boffs.h>
#include <chrono>
#include <ctime>
#include <map>
#include <mutex>
#include <string.h>

BEGIN_BOF_NAMESPACE()
class BasicLogger : public BOF::IBofLogger
{
public:
  BasicLogger(const std::string &_rLibNamePrefix_S, const std::string &_rLoggerChannelName_S)
      : BOF::IBofLogger()
  {
    mChannelName_S = _rLibNamePrefix_S + _rLoggerChannelName_S;
    Open(false, false, false, 0, "");
  }
  virtual ~BasicLogger()
  {
    if (mpLogFile_X)
    {
      fclose(mpLogFile_X);
      mpLogFile_X = nullptr;
    }
  }
  void V_Log(LogSeverity /*_SeverityLevel_E*/, const std::string &_rFile_S, uint32_t _Line_U32, const std::string &_rFunc_S, const char *_pLogMessage_c, ...) override
  {
    if ((mOutputOnScreen_B) || (mpLogFile_X))
    {
      char pHeader_c[0x100], pLog_c[0x1000];
      const char *pFile_c;
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
      BOF_GET_FILE_FROM_PATH(_rFile_S.c_str(), pFile_c)
      sprintf(pHeader_c, "%s: %zd [%s] %s:%d (%s)->", pDateTime_c, DeltaInuS, mChannelName_S.c_str(), pFile_c, _Line_U32, _rFunc_S.c_str());
      mNbLogLine_U32++;
      if (mOutputOnScreen_B)
      {
        printf("%s%s", pHeader_c, pLog_c);
      }
      if (mpLogFile_X)
      {
        if ((mNbLogLine_U32 & 0x0000000F)==0)
        {
          if (Size() > mMaxSizeInByte_U32)
          {
            SwapLogFile();
          }
        }
        if (mpLogFile_X)
        {
          fwrite(pHeader_c, strlen(pHeader_c), 1, mpLogFile_X);
          fwrite(pLog_c, strlen(pLog_c), 1, mpLogFile_X);
          if ((mNbLogLine_U32 & 0x0000000F) == 0)
          {
            if (mAutoFlush_B)
            {
              Flush();
            }
          }
        }
      }
    }
  }
  bool Open(bool _OutputOnScreen_B, bool _Append_B, bool _AutoFlush_B, uint32_t _MaxSizeInByte_U32, const std::string &_rLogFileSubDir_S)
  {
    bool Rts_B = true;

    mOutputOnScreen_B = _OutputOnScreen_B;
    mAutoFlush_B = _AutoFlush_B;
    mMaxSizeInByte_U32 = _MaxSizeInByte_U32;
    mLogFileSubDir_S = _rLogFileSubDir_S;
    mNbLogLine_U32 = 0; //Not really true if append mode but not serious as it is only used to lower call frequency to SwapLogFile check and Flush
    if (_rLogFileSubDir_S.empty())
    {
      mpLogFile_X = nullptr;
    }
    else
    {
      sprintf(mpLogFilePath_c, "%s/%s.log", _rLogFileSubDir_S.c_str(), mChannelName_S.c_str());
      mpLogFile_X = _Append_B ? fopen(mpLogFilePath_c, "w+") : fopen(mpLogFilePath_c, "a+");
      if (mpLogFile_X)
      {
        V_Log(LOG_SEVERITY_FORCE, __FILE__, __LINE__, __func__, "New log session started...\n");
      }
      else
      {
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
      V_Log(LOG_SEVERITY_FORCE, __FILE__, __LINE__, __func__, "Log session finished !\n");
      fclose(mpLogFile_X);
      mpLogFile_X = nullptr;
    }
    mOutputOnScreen_B = false;

    return Rts_B;
  }
  bool SwapLogFile()
  {
    bool Rts_B = false;
    char pBackLogFilePath_c[1024];

    //if (Close()) //No because never ending loop (Close call V_Log) and modify var mOutputOnScreen_B
    if (mpLogFile_X)
    {
      fclose(mpLogFile_X);
      sprintf(pBackLogFilePath_c, "%s.back", mpLogFilePath_c);
      Bof_DeleteFile(pBackLogFilePath_c);
      if (Bof_RenameFile(mpLogFilePath_c, pBackLogFilePath_c) == BOF_ERR_NO_ERROR)
      {
        Rts_B = Open(mOutputOnScreen_B, false, mAutoFlush_B, mMaxSizeInByte_U32, mLogFileSubDir_S);
      }
    }
    return Rts_B;
  }
  uint64_t Size()
  {
    uint64_t Rts_U64 = 0, CurrentPosition_U64;

    if (mpLogFile_X)
    {
      CurrentPosition_U64 = ftell(mpLogFile_X);
      fseek(mpLogFile_X, 0, SEEK_END);
      Rts_U64 = ftell(mpLogFile_X);
      fseek(mpLogFile_X, CurrentPosition_U64, SEEK_SET);
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
  std::string mLogFileSubDir_S;
  char mpLogFilePath_c[1024];
  uint32_t mMaxSizeInByte_U32 = 0;
  uint32_t mNbLogLine_U32 = 0;
};

class BofBasicLoggerFactory : public BOF::IBofLoggerFactory
{
public:
  BofBasicLoggerFactory(bool _OutputOnScreen_B, bool _Append_B, bool _AutoFlush_B, const std::string &_rLogFileSubDir_S)
      : mOutputOnScreen_B(_OutputOnScreen_B), mAppend_B(_Append_B), mAutoFlush_B(_AutoFlush_B), mLogFileSubDir_S(_rLogFileSubDir_S)
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
        if (psRts->Open(mOutputOnScreen_B, mAppend_B, mAutoFlush_B, (1024*1024), mLogFileSubDir_S))
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
