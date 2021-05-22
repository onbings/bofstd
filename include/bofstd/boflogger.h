/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the boflogger class
 *
 * Name:        boflogger.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         fmt lib: http://fmtlib.net/Text%20Formatting.html
 * https://github.com/gabime/spdlog/issues/298 : Change  #define SPDLOG_EOL ";-)\n" in spdlog Tweakme.h to prevent \n at the end of each log #define SPDLOG_EOL ""
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */

#pragma once

/*** Include files ***********************************************************/

#include <bofstd/bofstd.h>
#include <bofstd/iboflogchannel.h>
#include <bofstd/boflogchannel_spdlog.h>
#include <map>
#include <memory>
#include "bofstringformatter.h"

BEGIN_BOF_NAMESPACE()

/*** Defines *****************************************************************/
//#define BOF_LOGGER_DEBUG_ON            // To avoid extra cost when we want no log and no fct call at all
//The ##__VA_ARGS__ syntax is non-standard. It is a "swallow comma if the __VA_ARGS__ is empty" extension implemented by GCC, and seems to have been adopted by other compilers.
#ifdef BOF_LOGGER_DEBUG_OFF
#define BOF_LOGGER_TRACE(LOG_DESTINATION,LOG_FORMAT, ...)
#define BOF_LOGGER_DBG(LOG_DESTINATION,LOG_FORMAT, ...)
#define BOF_LOGGER_INFORMATION(LOG_DESTINATION,LOG_FORMAT, ...)
#define BOF_LOGGER_WARNING(LOG_DESTINATION,LOG_FORMAT, ...)
#define BOF_LOGGER_ERROR(LOG_DESTINATION,LOG_FORMAT, ...)
#define BOF_LOGGER_CRITICAL(LOG_DESTINATION,LOG_FORMAT, ...)
#define BOF_LOGGER_RAW_OUTPUT(LOG_DESTINATION,MSG)
#else
#define BOF_LOGGER_TRACE(LOG_DESTINATION, LOG_FORMAT, ...)          BOF_NAMESPACE::BofLogger::S_Instance().Log(LOG_DESTINATION,BOF_NAMESPACE::BOF_LOG_CHANNEL_LEVEL::TRACE,LOG_FORMAT, ##__VA_ARGS__)
#define BOF_LOGGER_DBG(LOG_DESTINATION, LOG_FORMAT, ...)          BOF_NAMESPACE::BofLogger::S_Instance().Log(LOG_DESTINATION,BOF_NAMESPACE::BOF_LOG_CHANNEL_LEVEL::DBG,LOG_FORMAT, ##__VA_ARGS__)
#define BOF_LOGGER_INFORMATION(LOG_DESTINATION, LOG_FORMAT, ...)    BOF_NAMESPACE::BofLogger::S_Instance().Log(LOG_DESTINATION,BOF_NAMESPACE::BOF_LOG_CHANNEL_LEVEL::INFORMATION,LOG_FORMAT, ##__VA_ARGS__)
#define BOF_LOGGER_WARNING(LOG_DESTINATION, LOG_FORMAT, ...)        BOF_NAMESPACE::BofLogger::S_Instance().Log(LOG_DESTINATION,BOF_NAMESPACE::BOF_LOG_CHANNEL_LEVEL::WARNING,LOG_FORMAT, ##__VA_ARGS__)
#define BOF_LOGGER_ERROR(LOG_DESTINATION, LOG_FORMAT, ...)          BOF_NAMESPACE::BofLogger::S_Instance().Log(LOG_DESTINATION,BOF_NAMESPACE::BOF_LOG_CHANNEL_LEVEL::ERR, LOG_FORMAT, ##__VA_ARGS__)
#define BOF_LOGGER_CRITICAL(LOG_DESTINATION, LOG_FORMAT, ...)       BOF_NAMESPACE::BofLogger::S_Instance().Log(LOG_DESTINATION,BOF_NAMESPACE::BOF_LOG_CHANNEL_LEVEL::CRITICAL, LOG_FORMAT, ##__VA_ARGS__)
#define BOF_LOGGER_RAW_OUTPUT(LOG_DESTINATION, MSG) {BOF_LOGGER_DBG(LOG_DESTINATION, MSG, 0);}
#endif

const uint32_t BOF_LOGGER_CHANNEL_FLAG_MASK_DBG_ALWAYS = 0x80000000;  /*! Always display */
const uint32_t BOF_LOGGER_CHANNEL_FLAG_MASK_DBG_ERROR = 0x40000000;  /*! Display error */
const uint32_t BOF_LOGGER_MAX_FAST_FORMAT_BUFFER_SIZE = 0x2000;

#define BOF_LOGGER_EXTENDED_TRACE(LOG_DESTINATION, LOG_CHANNEL_ID, LOG_ERROR_CODE, LOG_FORMAT, ...) {char pLog_c[BOF_NAMESPACE::BOF_LOGGER_MAX_FAST_FORMAT_BUFFER_SIZE];             if (!BOF_NAMESPACE::BofLogger::S_FormatMaskLogMsg(LOG_DESTINATION,LOG_CHANNEL_ID,                   sizeof(pLog_c),pLog_c,__FILE__,__LINE__,__FUNCTION__,LOG_ERROR_CODE, LOG_FORMAT, ##__VA_ARGS__)) {BOF_LOGGER_TRACE(LOG_DESTINATION, pLog_c, 0);}}
#define BOF_LOGGER_EXTENDED_DBG(LOG_DESTINATION, LOG_CHANNEL_ID, LOG_ERROR_CODE, LOG_FORMAT, ...) {char pLog_c[BOF_NAMESPACE::BOF_LOGGER_MAX_FAST_FORMAT_BUFFER_SIZE];             if (!BOF_NAMESPACE::BofLogger::S_FormatMaskLogMsg(LOG_DESTINATION,LOG_CHANNEL_ID,                   sizeof(pLog_c),pLog_c,__FILE__,__LINE__,__FUNCTION__,LOG_ERROR_CODE, LOG_FORMAT, ##__VA_ARGS__)) {BOF_LOGGER_DBG(LOG_DESTINATION, pLog_c,0);}}
#define BOF_LOGGER_EXTENDED_INFORMATION(LOG_DESTINATION, LOG_CHANNEL_ID, LOG_ERROR_CODE, LOG_FORMAT, ...) {char pLog_c[BOF_NAMESPACE::BOF_LOGGER_MAX_FAST_FORMAT_BUFFER_SIZE];       if (!BOF_NAMESPACE::BofLogger::S_FormatMaskLogMsg(LOG_DESTINATION,LOG_CHANNEL_ID,                   sizeof(pLog_c),pLog_c,__FILE__,__LINE__,__FUNCTION__,LOG_ERROR_CODE, LOG_FORMAT, ##__VA_ARGS__)) {BOF_LOGGER_INFORMATION(LOG_DESTINATION, pLog_c,0);}}
#define BOF_LOGGER_EXTENDED_WARNING(LOG_DESTINATION, LOG_CHANNEL_ID, LOG_ERROR_CODE, LOG_FORMAT, ...) {char pLog_c[BOF_NAMESPACE::BOF_LOGGER_MAX_FAST_FORMAT_BUFFER_SIZE];           if (!BOF_NAMESPACE::BofLogger::S_FormatMaskLogMsg(LOG_DESTINATION,LOG_CHANNEL_ID,                   sizeof(pLog_c),pLog_c,__FILE__,__LINE__,__FUNCTION__,LOG_ERROR_CODE, LOG_FORMAT, ##__VA_ARGS__)) {BOF_LOGGER_WARNING(LOG_DESTINATION, pLog_c, 0);}}
#define BOF_LOGGER_EXTENDED_ERROR(LOG_DESTINATION, LOG_CHANNEL_ID, LOG_ERROR_CODE, LOG_FORMAT, ...) {char pLog_c[BOF_NAMESPACE::BOF_LOGGER_MAX_FAST_FORMAT_BUFFER_SIZE];             if (!BOF_NAMESPACE::BofLogger::S_FormatMaskLogMsg(LOG_DESTINATION,LOG_CHANNEL_ID,                   sizeof(pLog_c),pLog_c,__FILE__,__LINE__,__FUNCTION__,LOG_ERROR_CODE, LOG_FORMAT, ##__VA_ARGS__)) {BOF_LOGGER_ERROR(LOG_DESTINATION, pLog_c, 0);}}
#define BOF_LOGGER_EXTENDED_CRITICAL(LOG_DESTINATION, LOG_CHANNEL_ID, LOG_ERROR_CODE, LOG_FORMAT, ...) {char pLog_c[BOF_NAMESPACE::BOF_LOGGER_MAX_FAST_FORMAT_BUFFER_SIZE];          if (!BOF_NAMESPACE::BofLogger::S_FormatMaskLogMsg(LOG_DESTINATION,LOG_CHANNEL_ID,                   sizeof(pLog_c),pLog_c,__FILE__,__LINE__,__FUNCTION__,LOG_ERROR_CODE, LOG_FORMAT, ##__VA_ARGS__)) {BOF_LOGGER_CRITICAL(LOG_DESTINATION, pLog_c, 0);}}
#define BOF_LOGGER_EXTENDED_ALWAYS_OUTPUT(LOG_DESTINATION, LOG_CHANNEL_ID, LOG_ERROR_CODE, LOG_FORMAT, ...) { char pLog_c[BOF_NAMESPACE::BOF_LOGGER_MAX_FAST_FORMAT_BUFFER_SIZE]; if (!BOF_NAMESPACE::BofLogger::S_FormatMaskLogMsg(LOG_DESTINATION, (LOG_CHANNEL_ID | BOF_DBG_ALWAYS), sizeof(pLog_c), pLog_c, __FILE__, __LINE__, __FUNCTION__, LOG_ERROR_CODE, LOG_FORMAT, ##__VA_ARGS__)) { BOF_LOGGER_DBG(LOG_DESTINATION, pLog_c, 0); } }

#define BOF_LOG_EXTENDED(LOG_TYPE, LOG_DESTINATION, LOG_CHANNEL_ID, LOG_ERROR_CODE, LOG_FORMAT, ...) BOF_LOGGER_EXTENDED_##LOG_TYPE(LOG_DESTINATION,LOG_CHANNEL_ID,LOG_ERROR_CODE,LOG_FORMAT, ##__VA_ARGS__)
#define BOF_LOG(LOG_TYPE, LOG_DESTINATION, LOG_FORMAT, ...) BOF_LOGGER_##LOG_TYPE(LOG_DESTINATION, LOG_FORMAT,  ##__VA_ARGS__)

/*** Global variables ********************************************************/

typedef std::map<std::string, std::shared_ptr<IBofLogChannel> >::iterator LogFctLookupIterator;

/*** Class *************************************************************************************************************************/
// Singleton http://stackoverflow.com/questions/11711920/how-to-implement-multithread-safe-singleton-in-c11-without-using-mutex
class BofLogger final
{
private:
		BOF_LOGGER_PARAM mLoggerParam_X;
		static bool S_mLoggerInitialized_B;

protected:
		BofLogger();

		virtual ~BofLogger();

public:
		static std::map<std::string, std::shared_ptr<IBofLogChannel> > S_mLogChannelList;

		// delete copy and move constructors and assign operators
		BofLogger(BofLogger const &) = delete; // Copy construct
		BofLogger(BofLogger &&) = delete; // Move construct
		BofLogger &operator=(BofLogger const &) = delete; // Copy assign
		BofLogger &operator=(BofLogger &&) = delete; // Move assign

		static BofLogger &S_Instance();

		BOFERR InitializeLogger(const BOF_LOGGER_PARAM &_rLoggerParam_X);

		BOFERR ShutdownLogger();

		bool IsLoggerInitialized() const;

    bool IsLoggerInAsyncMode() const;

    bool IsLoggerBlockingInAsyncMode() const;

		BOFERR AddLogChannel(std::shared_ptr<IBofLogChannel> _psLogInterface, const BOF_LOG_CHANNEL_PARAM &_rBofLogParam_X);

		BOFERR RemoveLogChannel(const std::string &_rChannelName_S);

		BOFERR Level(const std::string &_rChannelName_S, BOF_LOG_CHANNEL_LEVEL _LogLevel_E);

		BOF_LOG_CHANNEL_LEVEL Level(const std::string &_rChannelName_S) const;

		BOFERR LogMask(const std::string &_rChannelName_S, uint32_t _LogMask_U32);

		uint32_t LogMask(const std::string &_rChannelName_S) const;

		BOFERR LogMaskName(const std::string &_rChannelName_S, uint32_t _LogMask_U32, const std::string &_rLogMaskName_S);

		const std::string LogMaskName(const std::string &_rChannelName_S, uint32_t _LogMask_U32) const;

    BOFERR LogLevelColor(const std::string &_rChannelName_S, BOF_LOG_CHANNEL_LEVEL _LogLevel_E, BOF_LOG_LEVEL_COLOR _LogLevelColor_E);

		BOFERR Flush(const std::string &_rChannelName_S);

		//BOFERR								AsyncShutdown(const std::string & _rChannelName_S);
		BOFERR LogChannelPathName(const std::string &_rChannelName_S, BofPath &_rLogPath);  //Mainly for BOF_LOG_CHANNEL_SINK::TO_DAILYFILE which is dynamic
		BOFERR OpenLogStorage(const std::string &_rChannelName_S, uint32_t _Offset_U32);

		BOFERR ReadLogStorage(const std::string &_rChannelName_S, uint32_t _MaxLineToExtract_U32, std::vector<std::string> &_rExtractedLogLine_S);

		BOFERR CloseLogStorage(const std::string &_rChannelName_S);

		BOFERR DeleteLogStorage(const std::string &_rChannelName_S);

		BOF_LOG_ERROR_CODE_TOSTRING_CALLBACK GetOnErrorCodeToStringCallback() const;

		static BOFERR S_FormatMaskLogMsg(const char *_pChannelName_c, uint32_t _Mask_U32, uint32_t _MaxSizeInFormattedLog_Line_U32, char *_pFormattedLogLine_c, const char *_pFile_c, uint32_t _Line_U32,
		                                 const char *_pFunction_c, uint32_t _Sts_U32, const char *_pFormat_c, ...);

		std::shared_ptr<IBofLogChannel> LogChannelInterface(const std::string &_rChannelName_S);

		template<typename ... Args>
		BOFERR Log(const std::string &_rChannelName_S, BOF_LOG_CHANNEL_LEVEL _Level_E, const char *_pFmt_c, const Args &... _Args)
		{
			BOFERR Rts_E = BOF_ERR_NOT_FOUND;
			std::shared_ptr<IBofLogChannel> psBofLogChannel;
			std::string FormatttedOutput_S;
			char pBuffer_c[BOF_NAMESPACE::BOF_LOGGER_MAX_FAST_FORMAT_BUFFER_SIZE], *p_c;

			LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
			if (It != S_mLogChannelList.end())
			{
				Rts_E = BOF_ERR_NO;
				psBofLogChannel = It->second;
				if (psBofLogChannel->ShouldLog(_Level_E) && (_pFmt_c))
				{
					Rts_E = BOF_ERR_NO_ERROR;
					if (mLoggerParam_X.FastFormat_B)
					{
						//FormatttedOutput_S = Bof_Sprintf(_pFmt_c, _Args ...);
						//FormatttedOutput_S = Bof_SafeSprintf(_pFmt_c, _Args ...);
						p_c = Bof_FastSnprintf(pBuffer_c, sizeof(pBuffer_c), _pFmt_c, _Args ...);  //Std::string maust be passed as Str_S.c_str() as Bof_FastSnprintf use C function
						if (p_c)
						{
							FormatttedOutput_S = p_c;
						}
					}
					else
					{
						FormatttedOutput_S = Bof_SafeFmtSprintf(_pFmt_c, _Args ...);
					}
					if (Rts_E == BOF_ERR_NO_ERROR)
					{

						Rts_E = psBofLogChannel->V_Log(_Level_E, FormatttedOutput_S);
					}
				}
			}
			return Rts_E;
		}

		BOFERR Log(const std::string &_rChannelName_S, BOF_LOG_CHANNEL_LEVEL _Level_E, const char *_pLog_c)
		{
			BOFERR Rts_E = BOF_ERR_NOT_FOUND;
			std::shared_ptr<IBofLogChannel> psBofLogChannel;

			LogFctLookupIterator It = S_mLogChannelList.find(_rChannelName_S);
			if (It != S_mLogChannelList.end())
			{
				Rts_E = BOF_ERR_NO;
				psBofLogChannel = It->second;
				if (psBofLogChannel->ShouldLog(_Level_E) && (_pLog_c))
				{
					Rts_E = psBofLogChannel->V_Log(_Level_E, _pLog_c);
				}
			}
			return Rts_E;
		}

		BOFERR Log(const std::string &_rChannelName_S, BOF_LOG_CHANNEL_LEVEL _Level_E, const std::string &_rLog_S)
		{
			return Log(_rChannelName_S, _Level_E, _rLog_S.c_str());
		}
};

END_BOF_NAMESPACE()