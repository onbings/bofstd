/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the boflogger class
 *
 * Name:        boflogger.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:				onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#include <bofstd/bofdatetime.h>
#include <bofstd/boflogger.h>

#include <spdlog/async.h>
#include <spdlog/spdlog.h>

BEGIN_BOF_NAMESPACE()

bool BofLogger::S_mLoggerInitialized_B = false;
std::map<std::string, std::shared_ptr<IBofLogChannel>> BofLogger::S_mLogChannelList;
/*
static std::map<BOF_LOGGER_OVERFLOW_POLICY, spdlog::async_overflow_policy> S_ToSpdlogAsyncOverflowMode =
{
  { BOF_LOGGER_OVERFLOW_POLICY::WAIT,      spdlog::async_overflow_policy::block },
  { BOF_LOGGER_OVERFLOW_POLICY::OVERWRITE, spdlog::async_overflow_policy::overrun_oldest },
};

static spdlog::async_overflow_policy S_BofLoggerAsyncOverflowModeToSpdlogAsyncOverflowMode(BOF_LOGGER_OVERFLOW_POLICY _From_E)
{
  std::map<BOF_LOGGER_OVERFLOW_POLICY, spdlog::async_overflow_policy>::iterator It = S_ToSpdlogAsyncOverflowMode.find(_From_E);
  BOF_ASSERT(It != S_ToSpdlogAsyncOverflowMode.end());                               // Must be found !
  return It->second;
}
*/
std::shared_ptr<spdlog::logger> S_psKeepAnInstanceAlive = nullptr; // By putting this here, if user doesn't call  BofLogger::ShutdownLogger (object destruction from bof) it des not call delete 2 time and make dump when cleaning the app's heap

BofLogger::BofLogger()
{
  S_psKeepAnInstanceAlive = spdlog::get("BofLogger::BofLogger"); // Even if this pointer is empy it avoid app dump when the spdlog singleton instance need to be deleted
}

BofLogger::~BofLogger()
{
  ShutdownLogger();
}

BofLogger &BofLogger::S_Instance()
{
  // Since it's a static variable, if the class has already been created, It won't be created again so it **is** thread-safe in C++11.
  static BofLogger TheBofLoggerInstance;
  return TheBofLoggerInstance;
}

void OnSpdlogError(const std::string & /*_rErr_S*/)
{
  // printf("OnSpdlogError called: %s\n", _rErr_S.c_str());
}

BOFERR BofLogger::InitializeLogger(const BOF_LOGGER_PARAM &_rLoggerParam_X)
{
  BOFERR Rts_E = BOF_ERR_ALREADY_OPENED;
  // spdlog::async_overflow_policy OverflowPolicy_E;
  // OverflowPolicy_E = S_BofLoggerAsyncOverflowModeToSpdlogAsyncOverflowMode(mLoggerParam_X.OverflowPolicy_E);

  if (!S_mLoggerInitialized_B)
  {
    mLoggerParam_X = _rLoggerParam_X;

    spdlog::set_error_handler(mLoggerParam_X.OnError ? mLoggerParam_X.OnError : OnSpdlogError);
    if (mLoggerParam_X.MaxNumberOfAsyncLogQueueEntry_U32)
    {
      spdlog::init_thread_pool(mLoggerParam_X.MaxNumberOfAsyncLogQueueEntry_U32,
                               1); // queue with nk items and 1 backing thread.
                                   //					spdlog::set_async_mode(mLoggerParam_X.MaxNumberOfAsyncLogQueueEntry_U32, OverflowPolicy_E, mLoggerParam_X.OnAsyncStart ? mLoggerParam_X.OnAsyncStart : OnSpdlogOpenAsync,
                                   //std::chrono::milliseconds(mLoggerParam_X.AsyncAutoFushIntervalInMs_U32), mLoggerParam_X.OnAsyncStop ? mLoggerParam_X.OnAsyncStop : OnSpdlogCloseAsync);
    }
    Rts_E = BOF_ERR_NO_ERROR;

    S_mLoggerInitialized_B = (Rts_E == BOF_ERR_NO_ERROR);
  }
  return Rts_E;
}

BOFERR BofLogger::ShutdownLogger()
{
  BOFERR Rts_E = BOF_ERR_NOT_OPENED;

  if (S_mLoggerInitialized_B)
  {
    for (auto Item : S_mLogChannelList)
    {
      Item.second->V_Remove();
    }
    S_mLogChannelList.clear();
    Rts_E = BOF_ERR_NO_ERROR;
    spdlog::drop_all();
    spdlog::shutdown();
    S_mLoggerInitialized_B = false;
  }
  return Rts_E;
}

bool BofLogger::IsLoggerInitialized() const
{
  return S_mLoggerInitialized_B;
}

bool BofLogger::IsLoggerInAsyncMode() const
{
  return mLoggerParam_X.MaxNumberOfAsyncLogQueueEntry_U32 ? true : false;
}
bool BofLogger::IsLoggerBlockingInAsyncMode() const
{
  return (mLoggerParam_X.OverflowPolicy_E == BOF_LOGGER_OVERFLOW_POLICY::WAIT) ? true : false;
}
BOFERR BofLogger::AddLogChannel(std::shared_ptr<IBofLogChannel> _psLogInterface, const BOF_LOG_CHANNEL_PARAM &_rLogParam_X)
{
  BOFERR Rts_E = BOF_ERR_INIT;

  if (S_mLoggerInitialized_B)
  {
    Rts_E = BOF_ERR_EINVAL;
    if (_psLogInterface != nullptr)
    {
      //      _psLogInterface->V_Initialize(mLoggerParam_X);  //Could be != from BOF_ERR_NO_ERROR if there is already an added channel but we don t care

      Rts_E = _psLogInterface->V_Add(_rLogParam_X);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Rts_E = _psLogInterface->LogLevel(_rLogParam_X.LogLevel_E);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          Rts_E = BOF_ERR_EEXIST;
          if (S_mLogChannelList.insert(std::make_pair(_rLogParam_X.ChannelName_S, _psLogInterface)).second)
          {
            Rts_E = BOF_ERR_NO_ERROR;
          }
        }
      }
    }
  }
  return Rts_E;
}

BOFERR BofLogger::RemoveLogChannel(const std::string &_rChannelName_S)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    It->second->V_Remove();
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

std::shared_ptr<IBofLogChannel> BofLogger::LogChannelInterface(const std::string &_rChannelName_S)
{
  std::shared_ptr<IBofLogChannel> psRts;

  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    psRts = It->second;
  }
  return psRts;
}

BOFERR BofLogger::Level(const std::string &_rChannelName_S, BOF_LOG_CHANNEL_LEVEL _LogLevel_E)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    It->second->LogLevel(_LogLevel_E);
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

BOF_LOG_CHANNEL_LEVEL BofLogger::Level(const std::string &_rChannelName_S) const
{
  BOF_LOG_CHANNEL_LEVEL Rts_E = BOF_LOG_CHANNEL_LEVEL::OFF;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    Rts_E = It->second->LogLevel();
  }
  return Rts_E;
}

BOFERR BofLogger::LogMask(const std::string &_rChannelName_S, uint32_t _LogMask_U32)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    It->second->LogMask(_LogMask_U32);
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

uint32_t BofLogger::LogMask(const std::string &_rChannelName_S) const
{
  uint32_t Rts_U32 = 0;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    Rts_U32 = It->second->LogMask();
  }
  return Rts_U32;
}

BOFERR BofLogger::LogMaskName(const std::string &_rChannelName_S, uint32_t _LogMask_U32, const std::string &_rLogMaskName_S)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    It->second->LogMaskName(_LogMask_U32, _rLogMaskName_S);
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

const std::string BofLogger::LogMaskName(const std::string &_rChannelName_S, uint32_t _LogMask_U32) const
{
  std::string Rts;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    Rts = It->second->LogMaskName(_LogMask_U32);
  }
  return Rts;
}

BOFERR BofLogger::LogHeader(const std::string &_rChannelName_S, const std::string &_rLogHeader_S)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    It->second->V_LogHeader(_rLogHeader_S);
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

std::string BofLogger::LogHeader(const std::string &_rChannelName_S) const
{
  std::string Rts_S;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    Rts_S = It->second->V_LogHeader();
  }
  return Rts_S;
}

BOFERR BofLogger::LogLevelColor(const std::string &_rChannelName_S, BOF_LOG_CHANNEL_LEVEL _LogLevel_E, BOF_LOG_LEVEL_COLOR _LogLevelColor_E)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    Rts_E = It->second->V_LogLevelColor(_LogLevel_E, _LogLevelColor_E);
  }
  return Rts_E;
}

BOF_LOG_LEVEL_COLOR BofLogger::LogLevelColor(const std::string &_rChannelName_S, BOF_LOG_CHANNEL_LEVEL _LogLevel_E)
{
  BOF_LOG_LEVEL_COLOR Rts_E = BOF_LOG_LEVEL_COLOR::LOG_COLOR_BACK_BLACK;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    Rts_E = It->second->V_LogLevelColor(_LogLevel_E);
  }
  return Rts_E;
}
BOFERR BofLogger::Flush(const std::string &_rChannelName_S)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    Rts_E = It->second->V_Flush();
  }
  return Rts_E;
}

BOFERR BofLogger::LogChannelPathName(const std::string &_rChannelName_S, BofPath &_rLogPath)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    It->second->V_LogChannelPathName(_rLogPath);
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

BOFERR BofLogger::OpenLogStorage(const std::string &_rChannelName_S, uint32_t _Offset_U32)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    Rts_E = It->second->V_OpenLogStorage(_Offset_U32);
  }
  return Rts_E;
}

BOFERR BofLogger::ReadLogStorage(const std::string &_rChannelName_S, uint32_t _MaxLineToExtract_U32, std::vector<std::string> &_rExtractedLogLine_S)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    Rts_E = It->second->V_ReadLogStorage(_MaxLineToExtract_U32, _rExtractedLogLine_S);
  }
  return Rts_E;
}

BOFERR BofLogger::CloseLogStorage(const std::string &_rChannelName_S)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    Rts_E = It->second->V_CloseLogStorage();
  }
  return Rts_E;
}

BOFERR BofLogger::DeleteLogStorage(const std::string &_rChannelName_S)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
  if (It != S_mLogChannelList.end())
  {
    Rts_E = It->second->V_DeleteLogStorage();
  }
  return Rts_E;
}

BOF_LOG_ERROR_CODE_TOSTRING_CALLBACK BofLogger::GetOnErrorCodeToStringCallback() const
{
  return mLoggerParam_X.OnErrorCodeToString;
}

BOFERR BofLogger::S_FormatMaskLogMsg(const char *_pChannelName_c, uint32_t _Mask_U32, uint32_t _MaxSizeInFormattedLog_Line_U32, char *_pFormattedLogLine_c, const char *_pFile_c, uint32_t _Line_U32, const char *_pFunction_c, uint32_t _Sts_U32,
                                     const char *_pFormat_c, ...)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  LogFctLookupIterator It;
  uint32_t Mask_U32;
  int TextLen_i, Remain_i;
  char pFile_c[128], pError_c[128], *p_c;
  const char *pErrorCodeString_c;
  va_list Va_X;
  BofDateTime DateTime;
  std::string DateTime_S;

  if ((_pChannelName_c) && (_pFormattedLogLine_c) && (_MaxSizeInFormattedLog_Line_U32))
  {
    Rts_E = BOF_ERR_NOT_FOUND;
    _pFormattedLogLine_c[0] = 0;
    It = BofLogger::S_mLogChannelList.find(_pChannelName_c);
    if (It != BofLogger::S_mLogChannelList.end())
    {
      Rts_E = BOF_ERR_NO;
      //	Mask_U32 = (_Mask_U32 & (0xFFFFFFFF ^ BOF_DBG_ERROR));     //remove BOF_DBG_ERROR from mask for || part of the if
      Mask_U32 = _Mask_U32;
      // printf(">0>>>Logmask %X %X %X\n",It->second->LogMask(),_Mask_U32, Mask_U32);
      if ((It->second->ShouldLogMask(Mask_U32)) || ((_Sts_U32) && (It->second->LogMask() & BOF_LOGGER_CHANNEL_FLAG_MASK_DBG_ERROR)) || (Mask_U32 & BOF_LOGGER_CHANNEL_FLAG_MASK_DBG_ALWAYS))
      {
        Rts_E = BOF_ERR_NO_ERROR;
        va_start(Va_X, _pFormat_c);
        //	Mask_U32 = (Mask_U32 & (0xFFFFFFFF ^ BOF_DBG_ALWAYS));

        Remain_i = _MaxSizeInFormattedLog_Line_U32 - 1;
        if (!_Line_U32)
        {
          TextLen_i = vsnprintf(_pFormattedLogLine_c, Remain_i, _pFormat_c, Va_X);
          (TextLen_i > 0) ? Remain_i -= TextLen_i : _pFormattedLogLine_c[0] = 0;
        }
        else
        {
          if (_pFile_c)
          {
            p_c = strrchr(const_cast<char *>(_pFile_c), '/');
            if (!p_c)
            {
              p_c = strrchr(const_cast<char *>(_pFile_c), '\\');
            }
            p_c ? strcpy(pFile_c, p_c + 1) : strcpy(pFile_c, const_cast<char *>(_pFile_c));
          }
          else
          {
            strcpy(pFile_c, "UNKNOWN");
          }
          if (BOF::BofLogger::S_Instance().GetOnErrorCodeToStringCallback())
          {
            pErrorCodeString_c = (BOF::BofLogger::S_Instance().GetOnErrorCodeToStringCallback())(_Sts_U32);
          }
          else
          {
            snprintf(pError_c, sizeof(pError_c), "Err %d/0x%X Bof %s", _Sts_U32, _Sts_U32, Bof_ErrorCode(static_cast<BOFERR>(_Sts_U32)));
            pErrorCodeString_c = pError_c;
          }
#if defined(DAEMONSERVICEMODE)
          //					TextLen_i = snprintf(_pFormattedLogLine_c, Remain_i, "[%8.8s] t=%d (%06d) %s %d %s (%s)=>", It->second->LogMaskName(Mask_U32).c_str(), Bof_GetMsTickCount(),
          Bof_Now(DateTime);
          DateTime_S = DateTime.ToString("%H:%M:%S");

          pErrorCodeString_c = TextLen_i = snprintf(_pFormattedLogLine_c, Remain_i, "[%8.8s] (%06lu) %s %d ms %s %d %s (%s)=>", It->second->LogMaskName(Mask_U32).c_str(), It->second->GetEllapsedTimeInMsSinceLast(), DateTime_S.c_str(), Bof_GetMsTickCount(),
                                                    pFile_c, _Line_U32, _pFunction_c ? _pFunction_c : "UNKNOWN", pErrorCodeString_c);
          if (TextLen_i > 0)
          {
            Remain_i -= TextLen_i;
            TextLen_i = vsnprintf(&_pFormattedLogLine_c[TextLen_i], Remain_i, _pFormat_c, Va_X);
            (TextLen_i > 0) ? Remain_i -= TextLen_i : _pFormattedLogLine_c[0] = 0;
          }
          else
          {
            _pFormattedLogLine_c[0] = 0;
          }
#else
          Bof_Now(DateTime);
          DateTime_S = DateTime.ToString("%d/%m/%y %H:%M:%S.") + std::to_string(Bof_GetMsTickCount() % 1000);
          TextLen_i = snprintf(_pFormattedLogLine_c, Remain_i, "[%u] (%06u) %s  %s %d %s (%s)\n[%8.8s] ", Bof_GetMsTickCount(), It->second->EllapsedTimeInMsSinceLast(), DateTime_S.c_str(), pFile_c, _Line_U32, _pFunction_c ? _pFunction_c : "UNKNOWN",
                               pErrorCodeString_c, It->second->LogMaskName(Mask_U32).c_str());
          //   printf(">1>>>%s\n",_pFormattedLogLine_c);
          //   printf(">2>>>%s Mask %08X\n",It->second->LogMaskName(Mask_U32).c_str(),Mask_U32);
          if (TextLen_i > 0)
          {
            Remain_i -= TextLen_i;
            TextLen_i = vsnprintf(&_pFormattedLogLine_c[TextLen_i], Remain_i, _pFormat_c, Va_X);
            (TextLen_i > 0) ? Remain_i -= TextLen_i : _pFormattedLogLine_c[0] = 0;
          }
          else
          {
            _pFormattedLogLine_c[0] = 0;
          }
#endif
        }
        va_end(Va_X);
        _pFormattedLogLine_c[_MaxSizeInFormattedLog_Line_U32 - 1] = 0; // Security
      }
    }
  }

  return Rts_E;
}
END_BOF_NAMESPACE()