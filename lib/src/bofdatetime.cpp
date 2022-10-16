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

#include <iomanip> 

BEGIN_BOF_NAMESPACE()

BOFERR Bof_FileTimeToSystemTime(uint64_t _FileTime_U64, BOF_DATE_TIME &_rDateTime_X)
{
  BOFERR Rts_E = BOF_ERR_INTERNAL;

#if defined( _WIN32 )
  FILETIME FileTime_X;
  SYSTEMTIME SystemTime_X;

  FileTime_X.dwLowDateTime = (uint32_t)_FileTime_U64;
  FileTime_X.dwHighDateTime = (uint32_t)(_FileTime_U64 >> 32);

  if (FileTimeToSystemTime(&FileTime_X, &SystemTime_X))
  {
    _rDateTime_X = BOF_DATE_TIME((uint8_t)SystemTime_X.wDay, (uint8_t)SystemTime_X.wMonth, (uint16_t)SystemTime_X.wYear, (uint8_t)SystemTime_X.wHour, (uint8_t)SystemTime_X.wMinute, (uint8_t)SystemTime_X.wSecond, (uint32_t)SystemTime_X.wMilliseconds * 1000);
    Rts_E = _rDateTime_X.IsValid_B ? BOF_ERR_NO_ERROR: BOF_ERR_EINVAL;
  }
#else
  time_t t = (time_t)_FileTime_U64;
  struct tm *pNow_X = localtime(&t); //Or gmtime ?

  if (pNow_X)
  {
    _rDateTime_X = BOF_DATE_TIME((uint8_t)pNow_X->tm_mday, (uint8_t)(pNow_X->tm_mon + 1), (uint16_t)(pNow_X->tm_year + 1900), (uint8_t)pNow_X->tm_hour, (uint8_t)pNow_X->tm_min, (uint8_t)pNow_X->tm_sec, 0);
    Rts_E = _rDateTime_X.IsValid_B ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
  }
#endif

  return Rts_E;
}



BOFERR Bof_Now(BOF_DATE_TIME &_rDateTime_X)
{
  BOFERR Rts_E;
  Rts_E = Bof_TimeInSecSinceEpoch_To_BofDateTime(time(nullptr), _rDateTime_X);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    _rDateTime_X.MicroSecond_U32 = 0; // static_cast<uint16_t>(Bof_GetMsTickCount() % 1000);->no as we can have 2 call in the same day with milli2<milli1
  }
  return Rts_E;
}


BOFERR Bof_TimeInSecSinceEpoch_To_BofDateTime(time_t _TimeInSecSice1970, BOF_DATE_TIME &_rDateTime_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  struct tm *pTimeInfo_X;
  pTimeInfo_X = localtime(&_TimeInSecSice1970);
  _rDateTime_X.Reset();
  if (pTimeInfo_X)
  {
    _rDateTime_X = BOF_DATE_TIME((uint8_t)pTimeInfo_X->tm_mday, (uint8_t)(pTimeInfo_X->tm_mon + 1), (uint16_t)(pTimeInfo_X->tm_year + 1900), (uint8_t)pTimeInfo_X->tm_hour, (uint8_t)pTimeInfo_X->tm_min, (uint8_t)pTimeInfo_X->tm_sec, 0);
    Rts_E = _rDateTime_X.IsValid_B ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
  }
  return (Rts_E);
}

BOFERR Bof_DateInDaySinceEpoch_To_BofDateTime(time_t _DateInDaySince1970, BOF_DATE_TIME &_rDateTime_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  struct tm *pTimeInfo_X, t;
  time_t Time;

  memset(&t, 0, sizeof(t));
  t.tm_mday = 1;
  t.tm_mon = 0; /* Jan */
  t.tm_year = 70; /* since 1900 */
  t.tm_isdst = -1;

  t.tm_mday += static_cast<int>(_DateInDaySince1970);
  Time = mktime(&t);
  pTimeInfo_X = localtime(&Time);
  _rDateTime_X.Reset();
  if (pTimeInfo_X)
  {
    _rDateTime_X = BOF_DATE_TIME((uint8_t)pTimeInfo_X->tm_mday, (uint8_t)(pTimeInfo_X->tm_mon + 1), (uint16_t)(pTimeInfo_X->tm_year + 1900), (uint8_t)pTimeInfo_X->tm_hour, (uint8_t)pTimeInfo_X->tm_min, (uint8_t)pTimeInfo_X->tm_sec, 0);
    Rts_E = _rDateTime_X.IsValid_B ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
  }
  return (Rts_E);
}

BOFERR Bof_BofDateTime_To_DateInDaySinceEpoch(const BOF_DATE_TIME &_rDateTime_X, time_t &_rDateInDaySince1970)
{
  BOF_DATE_TIME EpochDateTime_X, DiffTime_X;
  uint32_t DiffDay_U32;
  BOFERR Rts_E;

  EpochDateTime_X.Day_U8 = 1;
  EpochDateTime_X.Month_U8 = 1;
  EpochDateTime_X.Year_U16 = 1970;
  EpochDateTime_X.Hour_U8 = 0;
  EpochDateTime_X.Minute_U8 = 0;
  EpochDateTime_X.Second_U8 = 0;
  EpochDateTime_X.MicroSecond_U32 = 0;
  Rts_E = Bof_DiffDateTime(_rDateTime_X, DiffTime_X, EpochDateTime_X, DiffDay_U32);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    _rDateInDaySince1970 = DiffDay_U32;
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



BOFERR Bof_SetDateTime(const BOF_DATE_TIME &_rDateTime_X)
{
  BOFERR Rts_E = BOF_ERR_INTERNAL;

#if defined (_WIN32)
  SYSTEMTIME           SystemTime_X;

  SystemTime_X.wYear = _rDateTime_X.Year_U16;
  SystemTime_X.wMonth = _rDateTime_X.Month_U8;
  SystemTime_X.wDayOfWeek = _rDateTime_X.DayOfWeek();
  SystemTime_X.wDay = _rDateTime_X.Day_U8;
  SystemTime_X.wHour = _rDateTime_X.Hour_U8;
  SystemTime_X.wMinute = _rDateTime_X.Minute_U8;
  SystemTime_X.wSecond = _rDateTime_X.Second_U8;
  SystemTime_X.wMilliseconds = _rDateTime_X.MicroSecond_U32 / 1000;

  if (SetSystemTime(&SystemTime_X))
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
#else
  struct tm *pDate_X;
  time_t Time_X;

  time(&Time_X);
  pDate_X = localtime(&Time_X); // Or gmtime ?
  pDate_X->tm_year = _rDateTime_X.Year_U16 - 1900;
  pDate_X->tm_mon = _rDateTime_X.Month_U8 - 1;
  pDate_X->tm_wday = _rDateTime_X.DayOfWeek();
  pDate_X->tm_mday = _rDateTime_X.Day_U8;
  pDate_X->tm_hour = _rDateTime_X.Hour_U8;
  pDate_X->tm_min = _rDateTime_X.Minute_U8;
  pDate_X->tm_sec = _rDateTime_X.Second_U8;

  const struct timeval Tv_X = {
    mktime(pDate_X), 0
  };
  if (settimeofday(&Tv_X, nullptr) == 0)
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }

#endif
  return Rts_E;
}


/*
 * Depuis l'ajustement du calendrier gr?gorien, sont bissextiles les ann?es1 :
 * soit divisibles par 4 mais non divisibles par 100;
 * soit divisibles par 400.
 */
bool Bof_IsLeapYear(uint16_t _Year_U16)
{
  return ((_Year_U16 % 4 == 0 && _Year_U16 % 100 != 0) || (_Year_U16 % 400 == 0)) ? true : false;
}

// http://alcor.concordia.ca/~gpkatch/gdate-algorithm.html
BOFERR Bof_DateTimeToNumber(const BOF_DATE_TIME &_rDateTime_X, double &_rDayNumber_lf)
{
  BOFERR Rts_E;
  int Month_i, Year_i;
  double FracDay_lf;

  Month_i = (_rDateTime_X.Month_U8 + 9) % 12;
  Year_i = _rDateTime_X.Year_U16 - Month_i / 10;
  _rDayNumber_lf = static_cast<double>((365 * Year_i) + (Year_i / 4) - (Year_i / 100) + (Year_i / 400) + ((Month_i * 306 + 5) / 10) + (_rDateTime_X.Day_U8 - 1));
  FracDay_lf = ((static_cast<double>(_rDateTime_X.Hour_U8) * static_cast<double>(60.0f * 60.0f * 1000.0f * 1000.0f)) + (static_cast<double>(_rDateTime_X.Minute_U8) * static_cast<double>(60.0f * 1000.0f * 1000.0f)) +
                (static_cast<double>(_rDateTime_X.Second_U8) * static_cast<double>(1000.0f * 1000.0f)) + (static_cast<double>(_rDateTime_X.MicroSecond_U32)));
  _rDayNumber_lf = _rDayNumber_lf + FracDay_lf;
  Rts_E = BOF_ERR_NO_ERROR;

  return Rts_E;
}



// millisecond are not used
BOFERR Bof_DiffDateTime(const BOF_DATE_TIME &_rFirstDateTime_X, const BOF_DATE_TIME &_rSecondDateTime_X, BOF_DATE_TIME &_rDiffTime_X, uint32_t &_rDiffDay_U32)
{
  BOFERR Rts_E;
  double DayNumber1_lf, DayNumber2_lf, Val_lf, FracVal_lf, ValInt_lf;

  _rDiffDay_U32 = 0;
  _rDiffTime_X.Reset();
  Rts_E = Bof_DateTimeToNumber(_rFirstDateTime_X, DayNumber1_lf);

  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Rts_E = Bof_DateTimeToNumber(_rSecondDateTime_X, DayNumber2_lf);

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = BOF_ERR_OPERATION_FAILED;
      if (DayNumber1_lf >= DayNumber2_lf)
      {
        Rts_E = BOF_ERR_NO_ERROR;
        Val_lf = DayNumber1_lf - DayNumber2_lf;
        FracVal_lf = modf(Val_lf, &ValInt_lf);
        _rDiffDay_U32 = static_cast<uint32_t>(ValInt_lf);
        Val_lf = 24.0f * FracVal_lf;
        FracVal_lf = modf(Val_lf, &ValInt_lf);
        _rDiffTime_X.Hour_U8 = static_cast<uint8_t>(ValInt_lf);
        Val_lf = 60.0f * FracVal_lf;
        FracVal_lf = modf(Val_lf, &ValInt_lf);
        _rDiffTime_X.Minute_U8 = static_cast<uint8_t>(ValInt_lf);
        Val_lf = 60.0f * FracVal_lf;
        FracVal_lf = modf(Val_lf, &ValInt_lf);
        _rDiffTime_X.Second_U8 = static_cast<uint8_t>(ValInt_lf);
        Val_lf = 1000.0f * 1000.0f * FracVal_lf;
        FracVal_lf = modf(Val_lf, &ValInt_lf);
        _rDiffTime_X.MicroSecond_U32 = static_cast<uint32_t>(ValInt_lf);
        if (_rDiffTime_X.MicroSecond_U32 > 999995)
        {
          _rDiffTime_X.MicroSecond_U32 = 0;
          _rDiffTime_X.Second_U8++;
          if (_rDiffTime_X.Second_U8 > 59)
          {
            _rDiffTime_X.Second_U8 = 0;
            _rDiffTime_X.Minute_U8++;
            if (_rDiffTime_X.Minute_U8 > 59)
            {
              _rDiffTime_X.Minute_U8 = 0;
              _rDiffTime_X.Hour_U8++;
              if (_rDiffTime_X.Hour_U8 > 23)
              {
                _rDiffTime_X.Hour_U8 = 0;
              }
            }
          }
        }
      }
    }
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

#if 0
//https://stackoverflow.com/questions/32188956/get-current-timestamp-in-microseconds-since-epoch
//https://www.web-dev-qa-db-fra.com/fr/c%2B%2B/c-comment-convertir-un-std-chrono-time-point-en-long-and-back/1054908078/
BOF_TIMEPOINT Bof_TimePoint(const BOF_DATE_TIME &_rDateTime_X)
{
  using Clock = std::chrono::high_resolution_clock;
  using TimePoint = std::chrono::time_point<Clock>;

  const Clock::duration duration_4_seconds = std::chrono::seconds(4);
  const TimePoint time_point_4_seconds(duration_4_seconds); // (2)
    // 4 seconds from start of epoch
  print_ms(time_point_4_seconds); // 4000 ms

  const TimePoint time_point_now = Clock::now(); // (3)
  print_ms(time_point_now); // 43098276 ms

  using namespace std::chrono;
  // Get current time with native precision
  auto now = system_clock::now();
  // Convert time_point to signed integral type
  auto integral_duration = now.time_since_epoch().count();
  // Convert signed integral type to time_point
  system_clock::time_point dt{ system_clock::duration{integral_duration} };
  // test
  if (dt != now)
    printf("Failure.\n");
  else
    printf("Success.\n");


  BOF_TIMEPOINT Rts;
  BOF_TIMEPOINT Rts = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now());
  BOF_TIMEPOINT Rts = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

  return Rts;
}

BOFERR Bof_ValidateDateTime(const BOF_DATE_TIME &_rDateTime_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;


  ///std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> 
  auto tt = _rDateTime_X.TimePoint();
  date::year_month_day a(date::year(_rDateTime_X.Year_U16), date::month(_rDateTime_X.Month_U8), date::day(_rDateTime_X.Day_U8));
  date::weekday b = date::weekday(a);
  bool ok = a.ok();
  a = date::year_month_day(date::year(2020), date::month(2), date::day(29));
  ok = a.ok();
  a = date::year_month_day(date::year(2022), date::month(2), date::day(29));
  ok = a.ok();
  a = date::year_month_day(date::year(2022), date::month(4), date::day(30));
  ok = a.ok();
  a = date::year_month_day(date::year(2022), date::month(4), date::day(31));
  ok = a.ok();

  //auto a = date::sys_days{ 10_d / 11 / 2012 };  // +12h + 38min + 40s + 123456us;

  std::cout << tt.time_since_epoch().count() << std::endl;
  //auto t = date::sys_days{ 10_d / 11 / 2012 };  // +12h + 38min + 40s + 123456us;
  date::year y(_rDateTime_X.Year_U16);
  date::month m(_rDateTime_X.Month_U8);
  date::day d(_rDateTime_X.Day_U8);
  date::year_month_day dmy(y, m, d);
  //dmy = dmy + 12h + 38min + 40s + 123456us;
  std::cout << dmy << std::endl;
  //auto tt = date::sys_days(dmy);// +38min + 40s + 123456us;


//  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> tt;

//  std::chrono::milliseconds add(_rDateTime_X.MicroSecond_U32 / 1000 );
//  tt += add;

  //tt = tt + 12h;
//  tt += 38min;
//  tt += 40sec;
//  tt += 123456us;
  std::cout << tt.time_since_epoch().count() << std::endl;

  //https://stackoverflow.com/questions/12835577/how-to-convert-stdchronotime-point-to-calendar-datetime-string-with-fraction
  //https://mariusbancila.ro/blog/2016/10/31/a-better-date-and-time-c-library/
//  auto t = date::sys_days{ 10_d / 11 / 2012 } + 12h + 38min + 40s + 123456us;

  //	std::chrono::system_clock::from_time_t(std::mktime(&tm));
  auto dp = floor<date::days>(tt);
  auto time = date::make_time(tt - dp);
  auto ymd = date::year_month_day{ dp };
  std::cout.fill('0');
  std::cout << ymd.day() << '-' << std::setw(2) << static_cast<unsigned>(ymd.month()) << '-' << ymd.year() << ' ' << time << '\n';
  std::cout << ymd.day() << '-' << std::setw(2) << static_cast<unsigned>(ymd.month()) << '-' << ymd.year() << ' ' << time << '\n';


  return Rts_E;
}

BOFERR Bof_ComputeDayOfWeek(const BOF_DATE_TIME &_rDateTime_X, uint8_t &_DayOfWeek_U8)  //0 is sunday
{
  //https://stackoverflow.com/questions/6054016/c-program-to-find-day-of-week-given-date
  BOFERR Rts_E = BOF_ERR_EINVAL;
  int d, m, y;
  Bof_ValidateDateTime(_rDateTime_X);

  if ((_rDateTime_X.Day_U8 >= 1) &&
      (_rDateTime_X.Day_U8 <= 31) &&
      (_rDateTime_X.Month_U8 >= 1) &&
      (_rDateTime_X.Month_U8 <= 12) &&
      (_rDateTime_X.Year_U16 >= 1970) &&
      (_rDateTime_X.Year_U16 <= 3070)            // This should be enought for me (-;
      )
  {
    Rts_E = BOF_ERR_NO_ERROR;

    d = _rDateTime_X.Day_U8; //Day     1-31
    m = _rDateTime_X.Month_U8; //Month   1-12`
    y = _rDateTime_X.Year_U16; //Year    2013` 

    _DayOfWeek_U8 = static_cast<uint8_t>((d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) % 7);
  }
  return Rts_E;
}
#endif

