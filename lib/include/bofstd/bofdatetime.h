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

struct BOFSTD_EXPORT BOF_DATE_TIME;
//BOFSTD_EXPORT BOFERR Bof_ComputeDayOfWeek(const BOF_DATE_TIME &_rDateTime_X, uint8_t &_DayOfWeek_U8);  //0 is sunday
//BOFSTD_EXPORT const std::string Bof_DateTimeToString(const BOF_DATE_TIME &_rDateTime_X, const std::string &_rFormat_S = "%Y-%m-%d %H:%M:%S");
//#define BOF_SET_DATE_TIME(datetime, day, month, year, hour, minute, second, us) {datetime.Day_U8 = day;datetime.Month_U8 = month;datetime.Year_U16 = year;datetime.DayOfWeek_U8 = 0;datetime.Hour_U8 = hour;datetime.Minute_U8 = minute;datetime.Second_U8 = second;datetime.MicroSecond_U32=us;}

struct BOFSTD_EXPORT BOF_DATE_TIME
{
  uint16_t Year_U16;                     // 2014
  uint8_t Month_U8;                     // 1-12
  uint8_t Day_U8;                       // 1-31
  uint8_t Hour_U8;                      // 0-23
  uint8_t Minute_U8;                    // 0-59
  uint8_t Second_U8;                    // 0-59
  uint32_t MicroSecond_U32;              // 0-999999
  bool	  IsValid_B;

  BOF_DATE_TIME()
  {
    Reset();
  }

  void Reset()
  {
    Year_U16 = 1970;
    Month_U8 = 1;
    Day_U8 = 1;
    Hour_U8 = 0;
    Minute_U8 = 0;
    Second_U8 = 0;
    MicroSecond_U32 = 0;
    //			DayOfWeek_U8 = 4;  //Thursday
    InitDateTime();
  }

  BOF_DATE_TIME(uint8_t _Day_U8, uint8_t _Month_U8, uint16_t _Year_U16, uint8_t _Hour_U8, uint8_t _Minute_U8, uint8_t _Second_U8, uint32_t _MicroSecond_U32)
  {
    uint8_t DayOfWeek_U8;

    Year_U16 = _Year_U16;
    Month_U8 = _Month_U8;
    Day_U8 = _Day_U8;
    Hour_U8 = _Hour_U8;
    Minute_U8 = _Minute_U8;
    Second_U8 = _Second_U8;
    MicroSecond_U32 = _MicroSecond_U32;
    InitDateTime();
  }

  BOF_DATE_TIME(const std::tm &_rRm_X)
  {
    uint8_t DayOfWeek_U8;

    Year_U16 = static_cast<uint16_t>(_rRm_X.tm_year + 1900);
    Month_U8 = static_cast<uint8_t>(_rRm_X.tm_mon + 1);
    Day_U8 = static_cast<uint8_t>(_rRm_X.tm_mday);
    Hour_U8 = static_cast<uint8_t>(_rRm_X.tm_hour);
    Minute_U8 = static_cast<uint8_t>(_rRm_X.tm_min);
    Second_U8 = static_cast<uint8_t>(_rRm_X.tm_sec);
    MicroSecond_U32 = 0;
    InitDateTime();
  }

  std::string ToString(const std::string &_rFormat_S = "%Y-%m-%d %H:%M:%S")
  {
    std::string ToString_S, Format_S;
    char pToString_c[0x1000];
    std::size_t PosPercentQ;
    bool AddMicroSec_B;
    
    Format_S = _rFormat_S;
    AddMicroSec_B = false;
    PosPercentQ = Format_S.find("%q");
    if (PosPercentQ != std::string::npos)
    {
//Manage only one instance of %q and this one whould be the last one of the format string
      AddMicroSec_B = true;
//Need to remove %q because il lead to an assertion during strftime call as it is not supported
      Format_S = Format_S.erase(PosPercentQ, 2);
    }

    if (strftime(pToString_c, sizeof(pToString_c), Format_S.c_str(), &Tm_X) > 0)
    {
      ToString_S = pToString_c;
      if (AddMicroSec_B)
      {
        ToString_S += std::to_string(MicroSecond_U32);
      }
    }
    return ToString_S;
  }

  BOF_DATE_TIME FromString(const std::string &_rDateTime_S, const std::string &_rFormat_S = "%Y-%m-%d %H:%M:%S")
  {
    char *p_c;
    std::tm TmInfo_X;
    uint32_t uS_U32;
    std::string Format_S;
    std::size_t PosPercentQ;
    bool AddMicroSec_B;

    Format_S = _rFormat_S;
    AddMicroSec_B = false;
    PosPercentQ = Format_S.find("%q");
    if (PosPercentQ != std::string::npos)
    {
      //Manage only one instance of %q and this one whould be the last one of the format string
      AddMicroSec_B = true;
      //Need to remove %q because il lead to an assertion during strftime call as it is not supported
      Format_S = Format_S.erase(PosPercentQ, 2);
    }

//In case the input string contains more characters than required by the format string, the return value points right after the last consumed
//input character.In case the whole input string is consumed, the return value points to the null byte at the end of the string.
//If strptime() fails to match all of the format string and therefore an error occurred, the function returns NULL.

    memset(&TmInfo_X, 0, sizeof(TmInfo_X));
    TmInfo_X.tm_mday = 1;
    TmInfo_X.tm_year = 70;
    p_c = strptime(_rDateTime_S.c_str(), Format_S.c_str(), &TmInfo_X);
    if (p_c)
    {
      uS_U32 = 0;
      if (*p_c)
      {
        uS_U32 = std::atoi(p_c);
      }

      *this = BOF_DATE_TIME(TmInfo_X.tm_mday, TmInfo_X.tm_mon, TmInfo_X.tm_year, TmInfo_X.tm_hour, TmInfo_X.tm_min, TmInfo_X.tm_sec, uS_U32);
    }

    return *this;
  }

  bool operator==(const BOF_DATE_TIME &_Other) const
  {
    return ((Year_U16 == _Other.Year_U16) && (Month_U8 == _Other.Month_U8) && (Day_U8 == _Other.Day_U8) && (Hour_U8 == _Other.Hour_U8) &&
            (Minute_U8 == _Other.Minute_U8) && (Second_U8 == _Other.Second_U8) && (MicroSecond_U32 == _Other.MicroSecond_U32));
  }

  bool operator!=(const BOF_DATE_TIME &_Other) const
  {
    return !(*this == _Other);
  }

  uint8_t DayOfWeek() const
  {
    return static_cast<uint8_t>(Wd.c_encoding());
  }

  date::year_month_day YearMountDay() const
  {
    return Ymd;
  }

  std::tm Tm() const 
  {
    return Tm_X;
  }
  std::time_t UtcTimeT() const
  {
    return TimeT;
  }
  std::chrono::system_clock::time_point TimePoint() const
  {
    return Tp;
  }
private:
  //  uint8_t DayOfWeek_U8;                 // 0-6 0:sunday
  std::tm Tm_X;
  std::time_t TimeT;
  date::year_month_day Ymd;
  date::weekday Wd;
  std::chrono::system_clock::time_point Tp;

  void InitDateTime()
  {
    Ymd = date::year_month_day(date::year(Year_U16), date::month(Month_U8), date::day(Day_U8));
    IsValid_B = Ymd.ok();
    if (IsValid_B)
    {
      if (MicroSecond_U32 < 1000000)
      {
        Wd = date::weekday(Ymd);
        Tm_X.tm_year = Year_U16 - 1900;
        Tm_X.tm_mon = Month_U8 - 1;
        Tm_X.tm_mday = Day_U8;
        Tm_X.tm_hour = Hour_U8;
        Tm_X.tm_min = Minute_U8;
        Tm_X.tm_sec = Second_U8;
        Tm_X.tm_wday = DayOfWeek();
        // tm_yday ?
        // tm_isdst ?

#ifdef _WIN32
        TimeT = _mkgmtime(&Tm_X);
#else
        TimeT = timegm(&TimeInfo_X);
#endif

        Tp = std::chrono::system_clock::from_time_t(TimeT);
        std::chrono::microseconds SubSec(MicroSecond_U32);
        Tp += SubSec;
      }
      else
      {
        IsValid_B = false;
      }
    }
  }
};

//BOFSTD_EXPORT std::chrono::system_clock::time_point Bof_CreateTimePoint(const BOF_DATE_TIME &_rDateTime_X);
//BOFSTD_EXPORT BOFERR Bof_ValidateDateTime(const BOF_DATE_TIME &_rDateTime_X);
BOFSTD_EXPORT BOFERR Bof_ComputeDayOfWeek(const BOF_DATE_TIME &_rDateTime_X, uint8_t &_DayOfWeek_U8);  //0 is sunday
BOFSTD_EXPORT BOFERR Bof_Now(BOF_DATE_TIME &_rDateTime_X);
BOFSTD_EXPORT BOFERR Bof_FileTimeToSystemTime(uint64_t _FileTime_U64, BOF_DATE_TIME &_rDateTime_X);
BOFSTD_EXPORT BOFERR Bof_DiffDateTime(const BOF_DATE_TIME &_rFirstDateTime_X, const BOF_DATE_TIME &_rSecondDateTime_X, BOF_DATE_TIME &_rDiffTime_X, uint32_t &_rDiffDay_U32);
//Above BOFSTD_EXPORT BOFERR Bof_ComputeDayOfWeek(const BOF_DATE_TIME &_rDateTime_X, uint8_t &_DayOfWeek_U8);  //0 is sunday
//Above BOFSTD_EXPORT const std::string Bof_DateTimeToString(const BOF_DATE_TIME &_rDateTime_X, const std::string &_rFormat_S = "%Y-%m-%d %H:%M:%S");

BOFSTD_EXPORT BOFERR Bof_TimeInSecSinceEpoch_To_BofDateTime(time_t _TimeInSecSice1970, BOF_DATE_TIME &_rDateTime_X);
BOFSTD_EXPORT BOFERR Bof_DateInDaySinceEpoch_To_BofDateTime(time_t _DateInDaySince1970, BOF_DATE_TIME &_rDateTime_X);
BOFSTD_EXPORT BOFERR Bof_BofDateTime_To_DateInDaySinceEpoch(const BOF_DATE_TIME &_rDateTime_X, time_t &_rDateInDaySince1970);
BOFSTD_EXPORT BOFERR Bof_SetDateTime(const BOF_DATE_TIME &_rDateTime_X);
BOFSTD_EXPORT bool Bof_IsLeapYear(uint16_t _Year_U16);
BOFSTD_EXPORT BOFERR Bof_DateTimeToNumber(const BOF_DATE_TIME &_pDateTime_X, double &_rDayNumber_lf);
BOFSTD_EXPORT BOFERR Bof_DiffDateTime(const BOF_DATE_TIME &_rFirstDateTime_X, const BOF_DATE_TIME &_rSecondDateTime_X, BOF_DATE_TIME &_rDiffTime_X, uint32_t &_rDiffDay_U32);
//BOFSTD_EXPORT BOF_DATE_TIME Bof_DateTimeFromString(const std::string &_rDateTime_S, const std::string &_rFormat_S = "%Y-%m-%d %H:%M:%S");
BOFSTD_EXPORT BOFERR Bof_DeltaMsToHms(uint32_t _DeltaInMs_U32, uint32_t &_rDay_U32, uint32_t &_rHour_U32, uint32_t &_rMinute_U32, uint32_t &_rSecond_U32, uint32_t &_rMs_U32);


END_BOF_NAMESPACE()