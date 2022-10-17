/*
 * Copyright (c) 2000-2020, Onbings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bofsatetime interface. 
 *
 * Name:        bofsystem.h
 * Author:      Bernard HARMEL: b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Jan 19 2017  BHA : Initial release
 */
#pragma once
//In this order
#include <chrono>
#include <date/date.h>
using namespace date::literals;
using namespace std::chrono_literals;

#include <bofstd/bofstd.h>

#if defined (_WIN32)
char *strptime(const char *buf, const char *fmt, struct tm *tm);
#else
#endif

BEGIN_BOF_NAMESPACE()

class BOFSTD_EXPORT BofDateTime
{
public:
  BofDateTime();
  BofDateTime(uint8_t _Day_U8, uint8_t _Month_U8, uint16_t _Year_U16, uint8_t _Hour_U8, uint8_t _Minute_U8, uint8_t _Second_U8, uint32_t _MicroSecond_U32);
  BofDateTime(uint8_t _Day_U8, uint8_t _Month_U8, uint16_t _Year_U16);
  BofDateTime(uint8_t _Hour_U8, uint8_t _Minute_U8, uint8_t _Second_U8, uint32_t _MicroSecond_U32);
  BofDateTime(const std::tm &_rTm_X, uint32_t _MicroSecond_U32);

  void ClearDate();
  bool IsTimeInADay() const;
  bool IsValid() const;
  void Reset();
  std::string ToString(const std::string &_rFormat_S = "%Y-%m-%d %H:%M:%S");
  BofDateTime FromString(const std::string &_rDateTime_S, const std::string &_rFormat_S = "%Y-%m-%d %H:%M:%S");
  bool operator==(const BofDateTime &_Other) const;
  bool operator!=(const BofDateTime &_Other) const;
  uint8_t DayOfWeek() const;
  uint16_t Year() const;
  uint8_t Month() const;
  uint8_t Day() const;
  uint8_t Hour() const;
  uint8_t Minute() const;
  uint8_t Second() const;
  uint32_t MicroSecond() const;

  date::year_month_day YearMountDay() const;
  std::tm Tm() const;
  std::time_t UtcTimeT() const;
  std::chrono::system_clock::time_point TimePoint() const;
  //double DateTimeNumber() const;

private:
  void InitDateTime();

  uint16_t mYear_U16;                     // 2014
  uint8_t mMonth_U8;                     // 1-12
  uint8_t mDay_U8;                       // 1-31
  uint8_t mHour_U8;                      // 0-23
  uint8_t mMinute_U8;                    // 0-59
  uint8_t mSecond_U8;                    // 0-59
  uint32_t mMicroSecond_U32;              // 0-999999
  bool	  mIsValid_B;

  std::tm mTm_X;
  std::time_t mTimeT;
  date::year_month_day mYmd;
  date::weekday mWd;
  date::hh_mm_ss<std::chrono::nanoseconds> mTime;

  std::chrono::system_clock::time_point mTp;
//  double mDateTimeNum_lf;
};

BOFSTD_EXPORT BOFERR Bof_ComputeDayOfWeek(const BofDateTime &_rDateTime, uint8_t &_DayOfWeek_U8);  //0 is sunday
BOFSTD_EXPORT BOFERR Bof_Now(BofDateTime &_rDateTime);
BOFSTD_EXPORT BOFERR Bof_FileTimeToSystemTime(uint64_t _FileTime_U64, BofDateTime &_rDateTime);
BOFSTD_EXPORT BOFERR Bof_NbDaySinceUnixEpoch_To_BofDateTime(uint32_t _NbDaySinceUnixEpoch_U32, BofDateTime &_rDateTime);
BOFSTD_EXPORT BOFERR Bof_BofDateTime_To_NbDaySinceUnixEpoch(const BofDateTime &_rDateTime, uint32_t &_rNbDaySinceUnixEpoch_U32);
BOFSTD_EXPORT BOFERR Bof_SetDateTime(const BofDateTime &_rDateTime);
BOFSTD_EXPORT bool   Bof_IsLeapYear(uint16_t _Year_U16);
BOFSTD_EXPORT BOFERR Bof_DiffDateTime(const BofDateTime &_rFirstDateTime, const BofDateTime &_rSecondDateTime, BofDateTime &_rDiffTime, int32_t &_rDiffDay_S32);
BOFSTD_EXPORT BOFERR Bof_DeltaMsToHms(uint32_t _DeltaInMs_U32, uint32_t &_rDay_U32, uint32_t &_rHour_U32, uint32_t &_rMinute_U32, uint32_t &_rSecond_U32, uint32_t &_rMs_U32);

END_BOF_NAMESPACE()