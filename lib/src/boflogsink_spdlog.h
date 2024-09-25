/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the spdlog extra sink object
 *
 * Name:        boflogsink_spdlog.h
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

#include <bofstd/bofstringcircularbuffer.h>
#include <spdlog/common.h>
#include <spdlog/details/file_helper.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/sink.h>

#include <fmt/format.h>
BEGIN_BOF_NAMESPACE()

bool CheckIfLimitedSizeIsReached(spdlog::details::file_helper &_rFileHelper, uint32_t _LogLineSiwze_U32, uint32_t &_rCrtFileSizeInByte_U32, uint32_t _MaxLogSizeInByte_U32);

template <typename Mutex> class ramcircularbuffer_sink : public spdlog::sinks::base_sink<Mutex>
{
private:
  BofStringCircularBuffer *mpBofStringCircularBuffer;

public:
  ramcircularbuffer_sink(BOF_LOGGER_OVERFLOW_POLICY _OverflowPolicy_E, uint32_t _BufferSizeInByte_U32);

  virtual ~ramcircularbuffer_sink();

  ramcircularbuffer_sink(const ramcircularbuffer_sink &) = delete; // You must do this to have a real singleton pattern http://stackoverflow.com/questions/27033368/why-in-singleton-pattern-we-do-make-copy-constructor-and-assignment-operator-a
  void operator=(const ramcircularbuffer_sink &) = delete;         // You must do this to have a real singleton pattern

  BOFERR PopSnapshot(std::string &_rLine_S);

  BOFERR ResyncSnapshot();

  BOFERR Reset();

protected:
  void sink_it_(const spdlog::details::log_msg &msg) override;

  void flush_() override;
};

typedef ramcircularbuffer_sink<std::mutex> ramcircularbuffer_sink_mt;
typedef ramcircularbuffer_sink<spdlog::details::null_mutex> ramcircularbuffer_sink_st;

template <typename Mutex> class simple_limitedfile_sink : public spdlog::sinks::base_sink<Mutex>
{
public:
  simple_limitedfile_sink(const spdlog::filename_t &filename, uint32_t _MaxLogSizeInByte, bool truncate = false);

  virtual ~simple_limitedfile_sink();

  void flush_() override;

protected:
  void sink_it_(const spdlog::details::log_msg &msg) override;

private:
  spdlog::details::file_helper _file_helper;
  uint32_t mMaxLogSizeInByte_U32;
  uint32_t mCrtFileSizeInByte_U32;
  std::string mLogFileName_S;
};

typedef simple_limitedfile_sink<std::mutex> simple_limitedfile_sink_mt;
typedef simple_limitedfile_sink<spdlog::details::null_mutex> simple_limitedfile_sink_st;

/*
 * Rotating file sink based on date. rotates at midnight
 */
template <typename Mutex, class FileNameCalc = spdlog::sinks::daily_filename_calculator> class limited_daily_file_sink : public spdlog::sinks::base_sink<std::mutex>
{
public:
  // create daily file sink which rotates on given time
  //  limited_daily_file_sink(const spdlog::filename_t &base_filename, const spdlog::filename_t &extension, int rotation_hour, int rotation_minute, uint32_t _MaxLogSizeInByte);
  limited_daily_file_sink(const spdlog::filename_t &base_filename, int rotation_hour, int rotation_minute, uint32_t _MaxLogSizeInByte);

  virtual ~limited_daily_file_sink();
  // spdlog::filename_t GetLogChannelPathName();

protected:
  void sink_it_(const spdlog::details::log_msg &msg) override;

  void flush_() override;

private:
  std::chrono::system_clock::time_point next_rotation_tp_();

  //		spdlog::filename_t _filename;
  spdlog::filename_t _base_filename;
  //		spdlog::filename_t _extension;
  int _rotation_h;
  int _rotation_m;
  std::chrono::system_clock::time_point _rotation_tp;
  spdlog::details::file_helper _file_helper;
  uint32_t mMaxLogSizeInByte_U32;
  uint32_t mCrtFileSizeInByte_U32;
  std::string mLogFileName_S;
};

typedef limited_daily_file_sink<std::mutex> limited_daily_file_sink_mt;
typedef limited_daily_file_sink<spdlog::details::null_mutex> limited_daily_file_sink_st;

template <typename Mutex> ramcircularbuffer_sink<Mutex>::ramcircularbuffer_sink(BOF_LOGGER_OVERFLOW_POLICY _OverflowPolicy_E, uint32_t _BufferSizeInByte_U32)
{
  BOF_STRING_CIRCULAR_BUFFER_PARAM Param_X;

  Param_X.MultiThreadAware_B = false; // We do not want to stop app logging as we 'snapshot' the buffer which can be long
  Param_X.PriorityInversionAware_B = false;
  Param_X.SnapshotMode_B = true;
  Param_X.BufferSizeInByte_U32 = _BufferSizeInByte_U32;
  Param_X.pData_c = nullptr;
  Param_X.Overwrite_B = (_OverflowPolicy_E == BOF_LOGGER_OVERFLOW_POLICY::OVERWRITE) ? true : false;
  Param_X.Blocking_B = false;
  mpBofStringCircularBuffer = new BofStringCircularBuffer(Param_X);
  if ((mpBofStringCircularBuffer) && (mpBofStringCircularBuffer->LastErrorCode() == BOF_ERR_NO_ERROR))
  {
  }
}

template <typename Mutex> ramcircularbuffer_sink<Mutex>::~ramcircularbuffer_sink()
{
  BOF_SAFE_DELETE(mpBofStringCircularBuffer);
}

template <typename Mutex> BOFERR ramcircularbuffer_sink<Mutex>::PopSnapshot(std::string &_rLine_S)
{
  BOFERR Rts_E = BOF_ERR_INIT;
  uint32_t NbMax_U32;
  char pData_c[0x10000];

  if (mpBofStringCircularBuffer)
  {
    NbMax_U32 = sizeof(pData_c);
    Rts_E = mpBofStringCircularBuffer->PopString(&NbMax_U32, pData_c, 0);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      _rLine_S = pData_c;
    }
  }
  return Rts_E;
}

template <typename Mutex> BOFERR ramcircularbuffer_sink<Mutex>::ResyncSnapshot()
{
  BOFERR Rts_E = BOF_ERR_INIT;

  if (mpBofStringCircularBuffer)
  {
    mpBofStringCircularBuffer->ResyncSnapshot(0);
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

template <typename Mutex> BOFERR ramcircularbuffer_sink<Mutex>::Reset()
{
  BOFERR Rts_E = BOF_ERR_INIT;

  if (mpBofStringCircularBuffer)
  {
    mpBofStringCircularBuffer->Reset();
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

template <typename Mutex> void ramcircularbuffer_sink<Mutex>::sink_it_(const spdlog::details::log_msg &msg)
{
  if (mpBofStringCircularBuffer)
  {
#if 0 // spdlog 1.3.1
    fmt::memory_buffer formatted;
    spdlog::sinks::sink::formatter_->format(msg, formatted);
    mpBofStringCircularBuffer->PushBinary(static_cast<uint32_t>(formatted.size()), formatted.data(), 0);
#else
    mpBofStringCircularBuffer->PushBinary(static_cast<uint32_t>(msg.payload.size()), msg.payload.data(), 0);
#endif
  }
}

template <typename Mutex> void ramcircularbuffer_sink<Mutex>::flush_()
{
}

template <typename Mutex> simple_limitedfile_sink<Mutex>::simple_limitedfile_sink(const spdlog::filename_t &filename, uint32_t _MaxLogSizeInByte, bool truncate)
{
  mMaxLogSizeInByte_U32 = _MaxLogSizeInByte;
  _file_helper.open(filename, truncate);
  // #pragma message("Please fix me simple_limitedfile_sink")
  mLogFileName_S = filename;
  flush_(); // To be sure that _file_helper.size() is ok
  mCrtFileSizeInByte_U32 = static_cast<uint32_t>(_file_helper.size());
}

template <typename Mutex> simple_limitedfile_sink<Mutex>::~simple_limitedfile_sink()
{
  _file_helper.close();
}

template <typename Mutex> void simple_limitedfile_sink<Mutex>::flush_()
{
  _file_helper.flush();
}

template <typename Mutex> void simple_limitedfile_sink<Mutex>::sink_it_(const spdlog::details::log_msg &msg)
{
#if 0 // spdlog 1.3.1
  fmt::memory_buffer formatted;
  spdlog::sinks::sink::formatter_->format(msg, formatted);
#else
  spdlog::memory_buf_t formatted;
  spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
#endif
  if (CheckIfLimitedSizeIsReached(_file_helper, formatted.size(), mCrtFileSizeInByte_U32, mMaxLogSizeInByte_U32))
  {
    // #pragma message("Please fix me simple_limitedfile_sink")
    _file_helper.open(mLogFileName_S, true);
    _file_helper.write(formatted);
    flush_(); // To be sure that _file_helper.size() is ok
    mCrtFileSizeInByte_U32 = static_cast<uint32_t>(_file_helper.size());
    ;
  }
  else
  {
    _file_helper.write(formatted);
  }
}

template <typename Mutex, class FileNameCalc>
limited_daily_file_sink<Mutex, FileNameCalc>::limited_daily_file_sink(const spdlog::filename_t &base_filename, int rotation_hour, int rotation_minute, uint32_t _MaxLogSizeInByte)
    : _base_filename(base_filename), _rotation_h(rotation_hour), _rotation_m(rotation_minute)
//  : _base_filename(base_filename), _extension(extension), _rotation_h(rotation_hour), _rotation_m(rotation_minute)
{
  spdlog::filename_t filename;

  mMaxLogSizeInByte_U32 = _MaxLogSizeInByte;
  if (rotation_hour < 0 || rotation_hour > 23 || rotation_minute < 0 || rotation_minute > 59)
  {
    throw spdlog::spdlog_ex("daily_file_sink: Invalid rotation time in ctor");
  }
  _rotation_tp = next_rotation_tp_();
  filename = spdlog::sinks::daily_filename_calculator::calc_filename(_base_filename, spdlog::details::os::localtime());
  _file_helper.open(filename);
  // #pragma message("Please fix me limited_daily_file_sink")
  flush_(); // To be sure that _file_helper.size() is ok
  mLogFileName_S = filename;
  mCrtFileSizeInByte_U32 = static_cast<uint32_t>(_file_helper.size());
}

template <typename Mutex, class FileNameCalc> limited_daily_file_sink<Mutex, FileNameCalc>::~limited_daily_file_sink()
{
  _file_helper.close();
}

template <typename Mutex, class FileNameCalc> void limited_daily_file_sink<Mutex, FileNameCalc>::flush_()
{
  _file_helper.flush();
}

template <typename Mutex, class FileNameCalc> void limited_daily_file_sink<Mutex, FileNameCalc>::sink_it_(const spdlog::details::log_msg &msg)
{
  spdlog::filename_t filename;

#if 0 // spdlog 1.3.1
  fmt::memory_buffer formatted;
  sink::formatter_->format(msg, formatted);
#else
  spdlog::memory_buf_t formatted;
  spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
#endif

  if ((CheckIfLimitedSizeIsReached(_file_helper, formatted.size(), mCrtFileSizeInByte_U32, mMaxLogSizeInByte_U32)) || (std::chrono::system_clock::now() >= _rotation_tp))
  {
    filename = FileNameCalc::calc_filename(_base_filename, spdlog::details::os::localtime());
    // #pragma message("Please fix me limited_daily_file_sink")
    mLogFileName_S = filename;
    //    LogChannelPathName(filename);
    _file_helper.open(filename);
    _file_helper.write(formatted);
    flush_(); // To be sure that _file_helper.size() is ok
    mCrtFileSizeInByte_U32 = static_cast<uint32_t>(_file_helper.size());
    _rotation_tp = next_rotation_tp_();
  }
  else
  {
    _file_helper.write(formatted);
  }
}

template <typename Mutex, class FileNameCalc> std::chrono::system_clock::time_point limited_daily_file_sink<Mutex, FileNameCalc>::next_rotation_tp_()
{
  auto now = std::chrono::system_clock::now();
  time_t tnow = std::chrono::system_clock::to_time_t(now);
  tm date = spdlog::details::os::localtime(tnow);
  date.tm_hour = _rotation_h;
  date.tm_min = _rotation_m;
  date.tm_sec = 0;
  auto rotation_time = std::chrono::system_clock::from_time_t(std::mktime(&date));
  if (rotation_time > now)
  {
    return rotation_time;
  }
  else
  {
    return std::chrono::system_clock::time_point(rotation_time + std::chrono::hours(24));
  }
}

END_BOF_NAMESPACE()
