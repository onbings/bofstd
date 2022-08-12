/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the logger channel interface
 *
 * Name:        iboflogchannel.h
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
#include <cstdint>
#include <vector>
#include <functional>
#include <bofstd/bofstd.h>
#include <bofstd/bofflag.h>
#include <bofstd/bofpath.h>
#include <bofstd/bofsystem.h>

BEGIN_BOF_NAMESPACE()

/*** Global variables ********************************************************/

/*** Definitions *************************************************************/

/*** Class *******************************************************************/

//Bit flag but not used (we could be able to send the same log on different sink but not used for now
enum class BOF_LOG_CHANNEL_SINK : uint32_t
{
		TO_NONE = 0,  //0x00000000,
		TO_FILE,// = 0x00000001,	//Single or rotating
		TO_DAILYFILE,// = 0x00000002,
		TO_RAM_CIRCULAR_BUFFER, // = 0x00000008,
		TO_STDERR,  // =        0x00000010,
		TO_STDERR_COLOR,  // =  0x00000020,
		TO_STDOUT,  // =        0x00000040,
		TO_STDOUT_COLOR,  // =  0x00000080,
		TO_LINUX_SYSLOG,  // =  0x00000100,
		TO_MSVC_DEBUGGER, // = 0x00000200,
};
//template< >
//struct IsItAnEnumBitFLag< BOF_LOG_CHANNEL_SINK > : std::true_type {};

enum class BOF_LOG_CHANNEL_FLAG : uint32_t
{
		NONE = 0x00000000,
		DELETE_PREVIOUS_LOGFILE = 0x00000001,
};
template<>
struct IsItAnEnumBitFLag<BOF_LOG_CHANNEL_FLAG> : std::true_type
{
};

enum class BOF_LOG_CHANNEL_LEVEL : uint32_t // Same as typedef enum{}level_enum;
{
		TRACE = 0,
		DBG,
		INFORMATION,
		WARNING,
		ERR,
		CRITICAL,
		OFF
};
/*
colors_[level::trace] = WHITE;
colors_[level::debug] = CYAN;
colors_[level::info] = GREEN;
colors_[level::warn] = YELLOW | BOLD;
colors_[level::err] = RED | BOLD;                         // red bold
colors_[level::critical] = BACKGROUND_RED | WHITE | BOLD; // white bold on red background
colors_[level::off] = 0;

colors_[level::trace] = white;
colors_[level::debug] = cyan;
colors_[level::info] = green;
colors_[level::warn] = yellow + bold;
colors_[level::err] = red + bold;
colors_[level::critical] = bold + on_red;
colors_[level::off] = reset;
*/
enum BOF_LOG_LEVEL_COLOR : uint32_t // Same as typedef enum{}level_enum;
{
  LOG_COLOR_NO_CHANGE = 0,
  LOG_COLOR_FORE_MASK = 0x0F,
  LOG_COLOR_BACK_MASK = 0xF0,
/// Formatting codes
  LOG_COLOR_BOLD=0x80000000,
//const std::string dark = "\033[2m";
//const std::string underline = "\033[4m";
//const std::string blink = "\033[5m";
//const std::string reverse = "\033[7m";
  LOG_COLOR_FORE_BLACK=1,
  LOG_COLOR_FORE_RED,
  LOG_COLOR_FORE_GREEN,
  LOG_COLOR_FORE_YELLOW,
  LOG_COLOR_FORE_BLUE,
  LOG_COLOR_FORE_MAGENTA,
  LOG_COLOR_FORE_CYAN,
  LOG_COLOR_FORE_WHITE,

  LOG_COLOR_BACK_BLACK=(LOG_COLOR_FORE_BLACK << 4),
  LOG_COLOR_BACK_RED = (LOG_COLOR_FORE_RED << 4),
  LOG_COLOR_BACK_GREEN = (LOG_COLOR_FORE_GREEN << 4),
  LOG_COLOR_BACK_YELLOW = (LOG_COLOR_FORE_YELLOW << 4),
  LOG_COLOR_BACK_BLUE = (LOG_COLOR_FORE_BLUE << 4),
  LOG_COLOR_BACK_MAGENTA = (LOG_COLOR_FORE_MAGENTA << 4),
  LOG_COLOR_BACK_CYAN = (LOG_COLOR_FORE_CYAN << 4),
  LOG_COLOR_BACK_WHITE = (LOG_COLOR_FORE_WHITE << 4),

};
//template< >
//struct IsItAnEnumBitFLag< BOF_LOG_CHANNEL_LEVEL > : std::true_type {};

enum class BOF_LOGGER_OVERFLOW_POLICY : uint32_t
{
		WAIT = 0, // Block / yield / sleep until message can be enqueued
		DISCARD, // Discard the message if enqueue fails
		OVERWRITE, // For circular structure, the oldest data will be erased to enqueue the new one
};

struct BOF_LOG_CHANNEL_PARAM
{
		std::string ChannelName_S;
		BofPath FileLogPath;
    std::string LogHeader_S;
    bool AutoAddEol_B;
    BOF_LOG_CHANNEL_LEVEL LogLevel_E;
		BOF_LOG_CHANNEL_SINK LogSink_E;
		BOF_LOG_CHANNEL_FLAG LogFlag_E;
    BOF_LOGGER_OVERFLOW_POLICY BufferOverflowPolicy_E;
		uint32_t MaxLogSizeInByte_U32;    //TO_FILE/TO_RAM_CIRCULAR_BUFFER If TO_FILE and MaxNumberOfLogFile_U32==0 and MaxLogSizeInByte_U32 != 0 ->log is sent to a fixed size log file if MaxLogSizeInByte_U32==0 the single log file size is unlimitted
		uint32_t MaxNumberOfLogFile_U32;  //TO_FILE 0: Single file n>0: n rotating files
		uint32_t DailyRotationTimeInMinuteAfterMidnight_U32;    //TO_DAILYFILE: Should contains the number of minutes after midnight to allapsed before rotationg and create a new daily file
		//Can't be a unique_ptr as it can come from a std::vector<BOF_LOG_CHANNEL_PARAM> global static var which is read only or return a const iterator->move semantic does not work
//	std::shared_ptr<std::ostringstream> psOutputStream;	//TO_STREAM 
		BOF_LOG_CHANNEL_PARAM()
		{
			Reset();
		}

		BOF_LOG_CHANNEL_PARAM(std::string _ChannelName_S, BofPath _FileLogPath, std::string _LogHeader_S, bool _AutoAddEol_B,BOF_LOG_CHANNEL_LEVEL _LogLevel_E, BOF_LOG_CHANNEL_SINK _LogSink_E, BOF_LOG_CHANNEL_FLAG _LogFlag_E, BOF_LOGGER_OVERFLOW_POLICY _BufferOverflowPolicy_E,
		                      uint32_t _MaxLogSizeInByte_U32, uint32_t _MaxNumberOfLogFile_U32, uint32_t _DailyRotationTimeInMinuteAfterMidnight_U32)
		{
			ChannelName_S = _ChannelName_S;
			FileLogPath = _FileLogPath;
      // https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
      // -----> Replaced by %i and SPDLOG_ENABLE_MESSAGE_COUNTER
      // We add a special %N market which can only appear as the first pattern character if you want to use it.
      // it is used to add unique log line number ( as a %08X number) after the other pattern character and before the beginning of the log line.
      // With this 'ticket' value, you can detect log line buffer overflow as you will have "holes" in the line number sequence if it happens
      LogHeader_S = _LogHeader_S;
      AutoAddEol_B=_AutoAddEol_B;
			LogLevel_E = _LogLevel_E;
			LogSink_E = _LogSink_E;
			LogFlag_E = _LogFlag_E;
      BufferOverflowPolicy_E = _BufferOverflowPolicy_E;
			MaxLogSizeInByte_U32 = _MaxLogSizeInByte_U32;
			MaxNumberOfLogFile_U32 = _MaxNumberOfLogFile_U32;
			DailyRotationTimeInMinuteAfterMidnight_U32 = _DailyRotationTimeInMinuteAfterMidnight_U32;
		}

		void Reset()
		{
			ChannelName_S = "";
			FileLogPath = "";
      LogHeader_S = "%i %d/%m/%C %H:%M:%S.%e %L %n: %^%v%$";
      AutoAddEol_B=true;
      LogLevel_E = BOF_LOG_CHANNEL_LEVEL::OFF;
			LogSink_E = BOF_LOG_CHANNEL_SINK::TO_NONE;
			LogFlag_E = BOF_LOG_CHANNEL_FLAG::NONE;
      BufferOverflowPolicy_E = BOF_LOGGER_OVERFLOW_POLICY::DISCARD;
			MaxLogSizeInByte_U32 = 0;
			MaxNumberOfLogFile_U32 = 0;
			DailyRotationTimeInMinuteAfterMidnight_U32 = 0;
		}
};
#if 0 //spdlog 1.3.1
typedef std::function<void(const std::string &_rErr_S)> BOF_LOG_ASYNC_ERROR_CALLBACK;
#else
typedef void (*BOF_LOG_ASYNC_ERROR_CALLBACK)(const std::string &_rErr_S);
#endif
//typedef std::function<void(void)> BOF_LOG_ASYNC_START_STOP_CALLBACK;
typedef std::function<const char *(uint32_t _ErrorCode_U32)> BOF_LOG_ERROR_CODE_TOSTRING_CALLBACK;

/*
For spdlog impl: Each logger pre allocates on the heap a buffer whose size is MaxNumberOfAsyncLogQueueEntry_U32 x sizeof(slot).
Each slot (in 64 bits) is 104 bytes, so for example set_async_mode(8192) would allocate on the heap 8192*104=832KB for each async logger.
*/
struct BOF_LOGGER_PARAM
{
		std::string Name_S;
		uint32_t MaxNumberOfAsyncLogQueueEntry_U32;
		uint32_t AsyncAutoFushIntervalInMs_U32;
		bool FastFormat_B;

    BOF_LOGGER_OVERFLOW_POLICY OverflowPolicy_E;
		BOF_LOG_ASYNC_ERROR_CALLBACK OnError;
		BOF_LOG_ERROR_CODE_TOSTRING_CALLBACK OnErrorCodeToString;

		BOF_LOGGER_PARAM()
		{
			Reset();
		}

		BOF_LOGGER_PARAM(std::string _Name_S, uint32_t _MaxNumberOfAsyncLogQueueEntry_U32, uint32_t _AsyncAutoFushIntervalInMs_U32, bool _FastFormat_B,
		                 BOF_LOGGER_OVERFLOW_POLICY _OverflowPolicy_E, /*BOF_LOG_ASYNC_START_STOP_CALLBACK _OnAsyncAdd, BOF_LOG_ASYNC_START_STOP_CALLBACK _OnAsyncRemove,*/
		                 BOF_LOG_ASYNC_ERROR_CALLBACK _OnError,
		                 BOF_LOG_ERROR_CODE_TOSTRING_CALLBACK _OnErrorCodeToString)
		{
			Name_S = _Name_S;
			MaxNumberOfAsyncLogQueueEntry_U32 = _MaxNumberOfAsyncLogQueueEntry_U32;
			AsyncAutoFushIntervalInMs_U32 = _AsyncAutoFushIntervalInMs_U32;
			FastFormat_B = _FastFormat_B;
      OverflowPolicy_E = _OverflowPolicy_E;
			OnError = _OnError;
			OnErrorCodeToString = _OnErrorCodeToString;
		}

		void Reset()
		{
			Name_S = "";

			MaxNumberOfAsyncLogQueueEntry_U32 = 0;                                   // queue size must be power of 2: O sync mode
			AsyncAutoFushIntervalInMs_U32 = 0;
			FastFormat_B = false;
			OverflowPolicy_E = BOF_LOGGER_OVERFLOW_POLICY::DISCARD;
			OnError = nullptr;
			OnErrorCodeToString = nullptr;
		}
};

class IBofLogChannel
{
protected:
		BOF_LOG_CHANNEL_LEVEL mLogLevel_E;
		uint32_t mLogMask_U32;
		uint32_t mStartLogTimeInMs_U32;
		uint32_t mLastLogTimeInMs_U32;
		std::vector<std::string> mpLogMaskName_S;

public:
		IBofLogChannel();

		// Empty virtual destructor for proper cleanup
		virtual ~IBofLogChannel()
		{}

		virtual BOFERR V_Add(const BOF_LOG_CHANNEL_PARAM &_rLogParam_X) = 0;

		virtual BOFERR V_Remove() = 0;

		virtual BOFERR V_Log(BOF_LOG_CHANNEL_LEVEL _Level_E, const std::string &_rLogMsg_S) = 0;

		virtual BOFERR V_Flush() = 0;

		virtual BOFERR V_LogChannelPathName(BofPath &_rLogPath) = 0;

		virtual BOFERR V_OpenLogStorage(uint32_t _Offset_U32) = 0;

		virtual BOFERR V_ReadLogStorage(uint32_t _MaxLineToExtract_U32, std::vector<std::string> &_rExtractedLogLine_S) const = 0;

		virtual BOFERR V_CloseLogStorage() = 0;

		virtual BOFERR V_DeleteLogStorage() = 0;

    virtual BOFERR V_LogLevel(BOF_LOG_CHANNEL_LEVEL _LogLevel_E) const = 0;

    virtual BOFERR V_LogLevelColor(BOF_LOG_CHANNEL_LEVEL _LogLevel_E, BOF_LOG_LEVEL_COLOR _LogLevelColor_E) const = 0;

    virtual BOFERR V_LogHeader(const std::string &_rPattern_S) const = 0;

    BOFERR LogLevel(BOF_LOG_CHANNEL_LEVEL _LogLevel_E);

	  BOF_LOG_CHANNEL_LEVEL LogLevel() const;

	  bool ShouldLog(BOF_LOG_CHANNEL_LEVEL _LogLevel_E) const;

	  BOFERR LogMask(uint32_t _LogMask_U32);

	  uint32_t LogMask() const;

	  BOFERR LogMaskName(uint32_t _LogMask_U32, const std::string &_rLogMaskName_S);

	  const std::string LogMaskName(uint32_t _LogMask_U32) const;

	  bool ShouldLogMask(uint32_t _LogMask_U32) const;

	  uint32_t EllapsedTimeInMsSinceStart();

	  uint32_t EllapsedTimeInMsSinceLast();

protected:
		void ResetLastLogTimeInMs();

private:
};

END_BOF_NAMESPACE()