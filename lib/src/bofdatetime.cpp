/*
 * Copyright (c) 2000-2020, Onbings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bofdatetime interface. 
 *
 * Name:        bofdatetime.h
 * Author:      Bernard HARMEL: b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Jan 19 2017  BHA : Initial release
 */
#include <bofstd/bofdatetime.h>

#if defined (_WIN32)
#include <Winsock2.h>
#include <conio.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <iostream> 
#include <iomanip> 

BEGIN_BOF_NAMESPACE()

BofDateTime::BofDateTime()
{
  Reset();
}

void BofDateTime::Reset()
{
  mYear_U16 = 1970;
  mMonth_U8 = 1;
  mDay_U8 = 1;
  mHour_U8 = 0;
  mMinute_U8 = 0;
  mSecond_U8 = 0;
  mMicroSecond_U32 = 0;
  //			DayOfWeek_U8 = 4;  //Thursday
  InitDateTime();
}

BofDateTime::BofDateTime(uint8_t _Day_U8, uint8_t _Month_U8, uint16_t _Year_U16, uint8_t _Hour_U8, uint8_t _Minute_U8, uint8_t _Second_U8, uint32_t _MicroSecond_U32)
{
  mYear_U16 = _Year_U16;
  mMonth_U8 = _Month_U8;
  mDay_U8 = _Day_U8;
  mHour_U8 = _Hour_U8;
  mMinute_U8 = _Minute_U8;
  mSecond_U8 = _Second_U8;
  mMicroSecond_U32 = _MicroSecond_U32;
  InitDateTime();
}
BofDateTime::BofDateTime(uint8_t _Day_U8, uint8_t _Month_U8, uint16_t _Year_U16)
{
  mYear_U16 = _Year_U16;
  mMonth_U8 = _Month_U8;
  mDay_U8 = _Day_U8;
  mHour_U8 = 0;
  mMinute_U8 = 0;
  mSecond_U8 = 0;
  mMicroSecond_U32 = 0;
  InitDateTime();
}
BofDateTime::BofDateTime(uint8_t _Hour_U8, uint8_t _Minute_U8, uint8_t _Second_U8, uint32_t _MicroSecond_U32)
{
  mYear_U16 = 1970;
  mMonth_U8 = 1;
  mDay_U8 = 1;
  mHour_U8 = _Hour_U8;
  mMinute_U8 = _Minute_U8;
  mSecond_U8 = _Second_U8;
  mMicroSecond_U32 = _MicroSecond_U32;
  InitDateTime();
}

BofDateTime::BofDateTime(const std::tm &_rTm_X, uint32_t _MicroSecond_U32)
{
  mYear_U16 = static_cast<uint16_t>(_rTm_X.tm_year + 1900);
  mMonth_U8 = static_cast<uint8_t>(_rTm_X.tm_mon + 1);
  mDay_U8 = static_cast<uint8_t>(_rTm_X.tm_mday);
  mHour_U8 = static_cast<uint8_t>(_rTm_X.tm_hour);
  mMinute_U8 = static_cast<uint8_t>(_rTm_X.tm_min);
  mSecond_U8 = static_cast<uint8_t>(_rTm_X.tm_sec);
  mMicroSecond_U32 = _MicroSecond_U32;
  InitDateTime();
}
void BofDateTime::ClearDate()
{
  mYear_U16 = 0;
  mMonth_U8 = 0;
  mDay_U8 = 0;
}

bool BofDateTime::IsTimeInADay() const
{
  bool Rts_B = false;
  if ((mHour_U8 < 24) && (mMinute_U8 < 60) && (mSecond_U8 < 60) && (mMicroSecond_U32 < 1000000))
  {
    Rts_B = true;
  }
  return Rts_B;
}
/*
* https://stackoverflow.com/questions/62795859/using-cs-date-library-to-read-times
 std::istringstream ss { argv[1] };
    std::chrono::system_clock::time_point dt
        { std::chrono::system_clock::now() };

    if ( date::from_stream(ss, "%F T %T %z", dt) ) {
        std::cout << "Cond 1 entered\n";
    }
    else if ( ss.clear(), ss.seekg(0); date::from_stream(ss, "%d-%m %R", dt)) {
        std::cout << "Cond 2 entered\n";
    }
*/

std::string BofDateTime::ToString(const std::string &_rFormat_S)
{
  std::string ToString_S, Format_S, MicroSecond_S;
  char pToString_c[0x1000];
  std::size_t PosPercentQ;

  PosPercentQ = _rFormat_S.find("%q");
  if (PosPercentQ != std::string::npos)
  {
    Format_S = _rFormat_S;
    MicroSecond_S = std::to_string(mMicroSecond_U32);
    while (PosPercentQ != std::string::npos)
    {
      //Need to remove %q because il lead to an assertion during strftime call as it is not supported
      Format_S = Format_S.erase(PosPercentQ, 2);
      Format_S.insert(PosPercentQ, MicroSecond_S);
      PosPercentQ = Format_S.find("%q", PosPercentQ);
    }
    if (strftime(pToString_c, sizeof(pToString_c), Format_S.c_str(), &mTm_X) > 0)
    {
      ToString_S = pToString_c;
    }
  }
  else
  {
    if (strftime(pToString_c, sizeof(pToString_c), _rFormat_S.c_str(), &mTm_X) > 0)
    {
      ToString_S = pToString_c;
    }
  }
  return ToString_S;
}


BofDateTime BofDateTime::FromString(const std::string &_rDateTime_S, const std::string &_rFormat_S)
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
    if (AddMicroSec_B)
    {
      if (*p_c)
      {
        uS_U32 = std::atoi(p_c);
      }
    }
    *this = BofDateTime(TmInfo_X.tm_mday, TmInfo_X.tm_mon + 1, TmInfo_X.tm_year + 1900, TmInfo_X.tm_hour, TmInfo_X.tm_min, TmInfo_X.tm_sec, uS_U32);
  }

  return *this;
}

bool BofDateTime::operator==(const BofDateTime &_Other) const
{
  return ((mYear_U16 == _Other.Year()) && (mMonth_U8 == _Other.Month()) && (mDay_U8 == _Other.Day()) && (mHour_U8 == _Other.Hour()) &&
          (mMinute_U8 == _Other.Minute()) && (mSecond_U8 == _Other.Second()) && (mMicroSecond_U32 == _Other.MicroSecond()));
}

bool BofDateTime::operator!=(const BofDateTime &_Other) const
{
  return !(*this == _Other);
}

bool BofDateTime::IsValid() const
{
  return mIsValid_B;
}

uint8_t BofDateTime::DayOfWeek() const
{
  return static_cast<uint8_t>(mWd.c_encoding());
}
uint16_t BofDateTime::Year() const
{
  return mYear_U16;
}
uint8_t BofDateTime::Month() const
{
  return mMonth_U8;
}
uint8_t BofDateTime::Day() const
{
  return mDay_U8;
}
uint8_t BofDateTime::Hour() const
{
  return mHour_U8;
}
uint8_t BofDateTime::Minute() const
{
  return mMinute_U8;
}
uint8_t BofDateTime::Second() const
{
  return mSecond_U8;
}
uint32_t BofDateTime::MicroSecond() const
{
  return mMicroSecond_U32;
}
date::year_month_day BofDateTime::YearMountDay() const
{
  return mYmd;
}

std::tm BofDateTime::Tm() const
{
  return mTm_X;
}
std::time_t BofDateTime::UtcTimeT() const
{
  return mTimeT;
}
std::chrono::system_clock::time_point BofDateTime::TimePoint() const
{
  return mTp;
}
//double BofDateTime::DateTimeNumber() const
//{
//  return mDateTimeNum_lf;
//}


void BofDateTime::InitDateTime()
{
  mYmd = date::year_month_day(date::year(mYear_U16), date::month(mMonth_U8), date::day(mDay_U8));
  mIsValid_B = mYmd.ok();
  if (mIsValid_B)
  {
    if (mMicroSecond_U32 < 1000000)
    {
      mWd = date::weekday(mYmd);
      mTm_X.tm_year = mYear_U16 - 1900;
      mTm_X.tm_mon = mMonth_U8 - 1;
      mTm_X.tm_mday = mDay_U8;
      mTm_X.tm_hour = mHour_U8;
      mTm_X.tm_min = mMinute_U8;
      mTm_X.tm_sec = mSecond_U8;
      mTm_X.tm_wday = DayOfWeek();
      // tm_yday ?
      // tm_isdst ?

#ifdef _WIN32
      mTimeT = _mkgmtime(&mTm_X);
#else
      mTimeT = timegm(&mTm_X);
#endif

      mTp = std::chrono::system_clock::from_time_t(mTimeT);
      std::chrono::microseconds SubSec(mMicroSecond_U32);
      mTp += SubSec;

      auto DurationInDay = floor<date::days>(mTp);
      mTime = date::make_time(mTp - DurationInDay);
//      std::cout.fill('0');
//      std::cout << mYmd.day() << '-' << std::setw(2) << static_cast<unsigned>(mYmd.month()) << '-' << mYmd.year() << ' ' << mTime << '\n';
//      std::cout << mTp.time_since_epoch().count() << std::endl;

      //mDateTimeNum_lf = static_cast<double>(mTp.time_since_epoch().count()) + (static_cast<double>(mMicroSecond_U32) / 1000000.0);

    }
    else
    {
      mIsValid_B = false;
    }
  }
}

/*
 * Depuis l'ajustement du calendrier gr?gorien, sont bissextiles les ann?es1 :
 * soit divisibles par 4 mais non divisibles par 100;
 * soit divisibles par 400.
 */
bool Bof_IsLeapYear(uint16_t _Year_U16)
{
//  return ((_Year_U16 % 4 == 0 && _Year_U16 % 100 != 0) || (_Year_U16 % 400 == 0)) ? true : false;
  return (date::year(_Year_U16).is_leap());
}

BOFERR Bof_FileTimeToSystemTime(uint64_t _FileTime_U64, BofDateTime &_rDateTime)
{
  BOFERR Rts_E = BOF_ERR_INTERNAL;

#if defined( _WIN32 )
  FILETIME FileTime_X;
  SYSTEMTIME SystemTime_X;

  FileTime_X.dwLowDateTime = (uint32_t)_FileTime_U64;
  FileTime_X.dwHighDateTime = (uint32_t)(_FileTime_U64 >> 32);

  if (FileTimeToSystemTime(&FileTime_X, &SystemTime_X))
  {
    _rDateTime = BofDateTime((uint8_t)SystemTime_X.wDay, (uint8_t)SystemTime_X.wMonth, (uint16_t)SystemTime_X.wYear, (uint8_t)SystemTime_X.wHour, (uint8_t)SystemTime_X.wMinute, (uint8_t)SystemTime_X.wSecond, (uint32_t)SystemTime_X.wMilliseconds * 1000);
    Rts_E = _rDateTime.IsValid() ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
  }
#else
  time_t t = (time_t)_FileTime_U64;
  struct tm *pNow_X = localtime(&t); //Or gmtime ?

  if (pNow_X)
  {
    _rDateTime = BofDateTime((uint8_t)pNow_X->tm_mday, (uint8_t)(pNow_X->tm_mon + 1), (uint16_t)(pNow_X->tm_year + 1900), (uint8_t)pNow_X->tm_hour, (uint8_t)pNow_X->tm_min, (uint8_t)pNow_X->tm_sec, 0);
    Rts_E = _rDateTime.IsValid() ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
  }
#endif

  return Rts_E;
}

BOFERR Bof_Now(BofDateTime &_rDateTime)
{
  BOFERR Rts_E = BOF_ERR_INTERNAL;

  std::chrono::system_clock::time_point TimePoint{ std::chrono::system_clock::now() };
  auto DurationInDay = floor<date::days>(TimePoint);
  date::year_month_day Ymd(DurationInDay);
  auto Time = date::make_time(TimePoint - DurationInDay);

  //std::cout.fill('0');
  //std::cout << Ymd.day() << '-' << std::setw(2) << static_cast<unsigned>(Ymd.month()) << '-' << Ymd.year() << ' ' << Time << '\n';
  //std::cout << TimePoint.time_since_epoch().count() << std::endl;

  _rDateTime = BofDateTime((uint32_t)Ymd.day(), (uint32_t)Ymd.month(), (int)Ymd.year(), Time.hours().count(), Time.minutes().count(), Time.seconds().count(), std::chrono::duration_cast<std::chrono::microseconds>(Time.subseconds()).count());
  Rts_E = _rDateTime.IsValid() ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;

  return Rts_E;
}

BOFERR Bof_NbDaySinceUnixEpoch_To_BofDateTime(uint32_t _NbDaySinceUnixEpoch_U32, BofDateTime &_rDateTime)
{
  BOFERR Rts_E;

  date::year_month_day Epoch = date::year(1970) / date::jan / date::day(1); // Assuming you're referring to the traditional Unix epoch (some systems such as Cocoa on OS X use the first day of the millenium, Jan 1, 2001 as their epoch)
  //std::cout.fill('0');
  //std::cout << Epoch.day() << '-' << std::setw(2) << static_cast<unsigned>(Epoch.month()) << '-' << Epoch.year() << '\n';

  date::year_month_day Ymd = date::sys_days( Epoch ) + date::days(_NbDaySinceUnixEpoch_U32);
  //std::cout << Ymd.day() << '-' << std::setw(2) << static_cast<unsigned>(Ymd.month()) << '-' << Ymd.year() << '\n';

  //  _rDateTime = BofDateTime((uint32_t)Ymd.day(), (uint32_t)Ymd.month(), (int)Ymd.year(), Time.hours().count(), Time.minutes().count(), Time.seconds().count(), std::chrono::duration_cast<std::chrono::microseconds>(Time.subseconds()).count());
  _rDateTime = BofDateTime((uint32_t)Ymd.day(), (uint32_t)Ymd.month(), (int)Ymd.year(), 0, 0, 0, 0);
  Rts_E = _rDateTime.IsValid() ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;

  return (Rts_E);
}

BOFERR Bof_BofDateTime_To_NbDaySinceUnixEpoch(const BofDateTime &_rDateTime, uint32_t &_rNbDaySinceUnixEpoch_U32)
{
  BOFERR Rts_E = BOF_ERR_INVALID_ANSWER;

  if (_rDateTime.IsValid())
  {
    date::year_month_day Ymd = date::year_month_day(date::year(_rDateTime.Year()), date::month(_rDateTime.Month()), date::day(_rDateTime.Day()));
//    _rNbDaySinceUnixEpoch_U32 = date::days(Ymd).count() - date::days(Epoch).count();
    _rNbDaySinceUnixEpoch_U32 = date::sys_days(Ymd).time_since_epoch().count();
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return (Rts_E);
}

BOFERR Bof_DeltaMsToHms(uint32_t _DeltaInMs_U32, uint32_t &_rDay_U32, uint32_t &_rHour_U32, uint32_t &_rMinute_U32, uint32_t &_rSecond_U32, uint32_t &_rMs_U32)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;

  _rMs_U32 = _DeltaInMs_U32 % 1000;
  _DeltaInMs_U32 /= 1000;
  _rSecond_U32 = _DeltaInMs_U32 % 60;
  _DeltaInMs_U32 /= 60;
  _rMinute_U32 = _DeltaInMs_U32 % 60;
  _DeltaInMs_U32 /= 60;
  _rHour_U32 = _DeltaInMs_U32 % 24;
  _DeltaInMs_U32 /= 24;
  _rDay_U32 = _DeltaInMs_U32;
  return Rts_E;
}

BOFERR Bof_SetDateTime(const BofDateTime &_rDateTime)
{
  BOFERR Rts_E = BOF_ERR_INVALID_ANSWER;

  if (_rDateTime.IsValid())
  {
#if defined (_WIN32)
    SYSTEMTIME           SystemTime_X;

    SystemTime_X.wYear = _rDateTime.Year();
    SystemTime_X.wMonth = _rDateTime.Month();
    SystemTime_X.wDayOfWeek = _rDateTime.DayOfWeek();
    SystemTime_X.wDay = _rDateTime.Day();
    SystemTime_X.wHour = _rDateTime.Hour();
    SystemTime_X.wMinute = _rDateTime.Minute();
    SystemTime_X.wSecond = _rDateTime.Second();
    SystemTime_X.wMilliseconds = _rDateTime.MicroSecond() / 1000;

    if (SetSystemTime(&SystemTime_X))
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
#else
    struct tm *pDate_X;
    time_t Time_X;

    time(&Time_X);
    pDate_X = localtime(&Time_X); // Or gmtime ?
    pDate_X->tm_year = _rDateTime.Year() - 1900;
    pDate_X->tm_mon = _rDateTime.Month() - 1;
    pDate_X->tm_wday = _rDateTime.DayOfWeek();
    pDate_X->tm_mday = _rDateTime.Day();
    pDate_X->tm_hour = _rDateTime.Hour();
    pDate_X->tm_min = _rDateTime.Minute();
    pDate_X->tm_sec = _rDateTime.Second();

    const struct timeval Tv_X =
    {
      mktime(pDate_X), 0
    };
    if (settimeofday(&Tv_X, nullptr) == 0)
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }

#endif
  }
  return Rts_E;
}

// millisecond are not used
BOFERR Bof_DiffDateTime(const BofDateTime &_rFirstDateTime, const BofDateTime &_rSecondDateTime, BofDateTime &_rDiffTime, int32_t &_rDiffDay_S32)
{
  BOFERR Rts_E = BOF_ERR_INVALID_ANSWER;

  _rDiffTime.Reset();
  _rDiffDay_S32 = 0;

  if ((_rFirstDateTime.IsValid()) && (_rSecondDateTime.IsValid()))
  {
    auto Duration = _rFirstDateTime.TimePoint() - _rSecondDateTime.TimePoint();
    auto NbDay = floor<date::days>(Duration);
    _rDiffDay_S32 = (int32_t)NbDay.count();
    auto OffsetInHmsus = Duration - NbDay;
    auto Time = date::make_time(OffsetInHmsus);

    //std::cout << _rDiffDay_S32 << std::endl;
    //std::cout << Time << std::endl;

    _rDiffTime = BofDateTime(Time.hours().count(), Time.minutes().count(), Time.seconds().count(), std::chrono::duration_cast<std::chrono::microseconds>(Time.subseconds()).count());
    _rDiffTime.ClearDate();
    Rts_E = _rDiffTime.IsValid() ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
  }
  return Rts_E;
}

END_BOF_NAMESPACE()


#if defined (_WIN32)
//https://stackoverflow.com/questions/321849/strptime-equivalent-on-windows
char *strptime(const char *s, const char *f, struct tm *tm)
{
  // Isn't the C++ standard lib nice? std::get_time is defined such that its
  // format parameters are the exact same as strptime. Of course, we have to
  // create a string stream first, and imbue it with the current C locale, and
  // we also have to make sure we return the right things if it fails, or
  // if it succeeds, but this is still far simpler an implementation than any
  // of the versions in any of the C standard libraries.
  std::istringstream input(s);
  input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
  input >> std::get_time(tm, f);
  if (input.fail())
  {
    return nullptr;
  }
  return (char *)(s + input.tellg());
}
#else
#endif


