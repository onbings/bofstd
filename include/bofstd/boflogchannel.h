/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the log module interface
 *
 * Name:        boflog.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */

#pragma once

/*** Include files ***********************************************************/
#include <bofstd/bofstd.h>
#include <bofstd/bofflag.h>
#include <spdlog/spdlog.h>

BEGIN_BOF_NAMESPACE()

/*** Global variables ********************************************************/

/*** Definitions *************************************************************/

/*** Class *******************************************************************/

//ex: BOF_LOGGER_DBGOUT(rBofLog_O,1000, Trace,"%s",pMsg_c)
//    BOF_LOGGER_DBGOUT(rBofLog_O,1000, Info,"%s",pMsg_c)
#define BOF_LOGGER_DBGOUT(logger, dbgsz, fct, fmt, ...)    { char dbgbuf[dbgsz]; snprintf(dbgbuf, sizeof( dbgbuf ), fmt, __VA_ARGS__); dbgbuf[sizeof( dbgbuf ) - 1] = 0; logger->fct(dbgbuf); }

enum class BOF_LOGGER_FLAG : uint32_t
{
		TO_NONE = 0x00000000,
		TO_STDERR = 0x00000001,
		TO_STDERR_COLOR = 0x00000002,
		TO_STDOUT = 0x00000004,
		TO_STDOUT_COLOR = 0x00000008,
		TO_SYSLOG = 0x00000010,
		DELETE_PREVIOUS_LOGFILE = 0x80000000,
};
template<>
struct IsItAnEnumBitFLag<BOF_LOGGER_FLAG> : std::true_type
{
};

enum class BOF_LOGGER_LEVEL : uint32_t      //Same as typedef enum{}level_enum;
{
		TRACE = 0,
		DBG,
		INFO,
		WARN,
		ERR,
		CRITICAL,
		OFF
};

struct BOFLOGPARAM
{
		std::string LoggerName_S;
		std::string LogPath_S;
		std::string LogPattern_S;
		BOF_LOGGER_LEVEL LogLevel_E;
		BOF_LOGGER_FLAG LogFlag_E;
		size_t MaxLogSizeInByte;
		size_t MaxNumberOfLogFile;
		size_t MaxNumberOfLoggerQueueEntries;

		BOFLOGPARAM()
		{
			LoggerName_S = "";
			LogPath_S = "";
			LogPattern_S = "%N %d/%m/%C %H:%M:%S:%e %L %n: %v";    //https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
			LogLevel_E = BOF_LOGGER_LEVEL::OFF;
			LogFlag_E = BOF_LOGGER_FLAG::TO_NONE;
			MaxLogSizeInByte = 0x100000;
			MaxNumberOfLogFile = 3;
			MaxNumberOfLoggerQueueEntries = 0x100000;             //queue size must be power of 2
		}
};

class BofLogger
{
private:
		BofLogger();

		static std::shared_ptr<spdlog::logger> S_mpsTheLogger_O;

public:
		static BofLogger *S_GetInstance();

		virtual ~BofLogger();

		int ConfigureLogger(const BOFLOGPARAM &_rBofLogParam_X);

		int ShutdownLogger();

		// logger.Info(cppformat_string, arg1, arg2, arg3, ...) call style
		template<typename ... Args>
		inline int Trace(const char *_pFmt_c, const Args &... _Args)
		{
			int Rts_i = -1;

			if (S_mpsTheLogger_O)
			{
				S_mpsTheLogger_O->trace(_pFmt_c, _Args ...);
				Rts_i = 0;
			}
			return Rts_i;
		}

		template<typename ... Args>
		inline int Debug(const char *_pFmt_c, const Args &... _Args)
		{
			int Rts_i = -1;

			if (S_mpsTheLogger_O)
			{
				S_mpsTheLogger_O->debug(_pFmt_c, _Args ...);
				Rts_i = 0;
			}
			return Rts_i;
		}

		template<typename ... Args>
		inline int Info(const char *_pFmt_c, const Args &... _Args)
		{
			int Rts_i = -1;

			if (S_mpsTheLogger_O)
			{
				S_mpsTheLogger_O->info(_pFmt_c, _Args ...);
				Rts_i = 0;
			}
			return Rts_i;
		}

		template<typename ... Args>
		inline int Warn(const char *_pFmt_c, const Args &... _Args)
		{
			int Rts_i = -1;

			if (S_mpsTheLogger_O)
			{
				S_mpsTheLogger_O->warn(_pFmt_c, _Args ...);
				Rts_i = 0;
			}
			return Rts_i;
		}

		template<typename ... Args>
		inline int Error(const char *_pFmt_c, const Args &... _Args)
		{
			int Rts_i = -1;

			if (S_mpsTheLogger_O)
			{
				S_mpsTheLogger_O->error(_pFmt_c, _Args ...);
				Rts_i = 0;
			}
			return Rts_i;
		}

		template<typename ... Args>
		inline int Critical(const char *_pFmt_c, const Args &... _Args)
		{
			int Rts_i = -1;

			if (S_mpsTheLogger_O)
			{
				S_mpsTheLogger_O->critical(_pFmt_c, _Args ...);
				Rts_i = 0;
			}
			return Rts_i;
		}

		// name and level
		inline const std::string Name() const
		{
			std::string Rts_S = "";

			if (S_mpsTheLogger_O)
			{
				Rts_S = S_mpsTheLogger_O->name();
			}
			return Rts_S;
		}

		inline int Level(BOF_LOGGER_LEVEL _LogLevel_E)
		{
			int Rts_i = -1;

			if (S_mpsTheLogger_O)
			{
				S_mpsTheLogger_O->set_level(static_cast<spdlog::level::level_enum>( _LogLevel_E ));
				Rts_i = 0;
			}
			return Rts_i;
		}

		inline BOF_LOGGER_LEVEL Level() const
		{
			BOF_LOGGER_LEVEL Rts_E = BOF_LOGGER_LEVEL::OFF;

			if (S_mpsTheLogger_O)
			{
				Rts_E = static_cast<BOF_LOGGER_LEVEL>( S_mpsTheLogger_O->level());
			}
			return Rts_E;
		}

		inline int Flush()
		{
			int Rts_i = -1;

			if (S_mpsTheLogger_O)
			{
				S_mpsTheLogger_O->flush();
				Rts_i = 0;
			}
			return Rts_i;
		}
};

END_BOF_NAMESPACE();
