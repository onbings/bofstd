/*
 * This module implement the unit tests of the datetime part of the Bof library
 *
 * Name:        ut_system.cpp
 * Author:      Bernard HARMEL
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  November 15 2016  BHA : Initial release
 */
#include <bofstd/bofdatetime.h>
#include <bofstd/bofsystem.h>

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

TEST(DateTime_Test, ValidateDateTime)
{
  BofDateTime DateTime;
  std::string DateTime_S;

  DateTime.Reset();
  EXPECT_TRUE(DateTime.IsValid());
  EXPECT_TRUE(DateTime.IsUnixEpoch());
  EXPECT_TRUE(DateTime.IsMidnight());

  DateTime = BofDateTime(1, 1, 1970, 0, 0, 0, 0);
  EXPECT_TRUE(DateTime.IsValid());
  EXPECT_TRUE(DateTime.IsUnixEpoch());
  EXPECT_TRUE(DateTime.IsMidnight());

  // DAY
  DateTime = BofDateTime(0, 1, 1970, 0, 0, 0, 0);
  EXPECT_FALSE(DateTime.IsValid());
  EXPECT_TRUE(DateTime.IsMidnight());

  DateTime = BofDateTime(32, 1, 1970, 0, 0, 0, 0);
  EXPECT_FALSE(DateTime.IsValid());
  EXPECT_FALSE(DateTime.IsUnixEpoch());
  EXPECT_TRUE(DateTime.IsMidnight());

  DateTime = BofDateTime(31, 1, 1970, 0, 0, 0, 0);
  EXPECT_TRUE(DateTime.IsValid());
  EXPECT_FALSE(DateTime.IsUnixEpoch());
  EXPECT_TRUE(DateTime.IsMidnight());

  // MONTH
  DateTime = BofDateTime(31, 0, 1970, 0, 0, 0, 0);
  EXPECT_FALSE(DateTime.IsValid());

  DateTime = BofDateTime(31, 13, 1970, 0, 0, 0, 0);
  EXPECT_FALSE(DateTime.IsValid());

  DateTime = BofDateTime(31, 12, 1970, 0, 0, 0, 0);
  EXPECT_TRUE(DateTime.IsValid());

  // YEAR
  DateTime = BofDateTime(31, 12, 1969, 0, 0, 0, 0);
  EXPECT_TRUE(DateTime.IsValid());

  DateTime = BofDateTime(31, 12, 3071, 0, 0, 0, 0);
  EXPECT_TRUE(DateTime.IsValid());

  DateTime = BofDateTime(31, 12, 3070, 0, 0, 0, 0);
  EXPECT_TRUE(DateTime.IsValid());

  // HOUR
  DateTime = BofDateTime(31, 12, 2070, 25, 0, 0, 0);
  EXPECT_TRUE(DateTime.IsValid());
  DateTime_S = DateTime.ToString("%Y-%m-%d %H:%M:%S");
  EXPECT_STREQ(DateTime_S.c_str(), "2071-01-01 01:00:00");

  DateTime = BofDateTime(31, 12, 2070, 24, 0, 0, 0);
  EXPECT_TRUE(DateTime.IsValid());
  DateTime_S = DateTime.ToString("%Y-%m-%d %H:%M:%S");
  EXPECT_STREQ(DateTime_S.c_str(), "2071-01-01 00:00:00");

  DateTime = BofDateTime(31, 12, 2070, 23, 0, 0, 0);
  EXPECT_TRUE(DateTime.IsValid());

  // MINUTE
  DateTime = BofDateTime(31, 12, 2070, 23, 255, 0, 0);
  EXPECT_TRUE(DateTime.IsValid());
  DateTime_S = DateTime.ToString("%Y-%m-%d %H:%M:%S");
  EXPECT_STREQ(DateTime_S.c_str(), "2071-01-01 03:15:00");

  DateTime = BofDateTime(31, 12, 2070, 23, 60, 0, 0);
  EXPECT_TRUE(DateTime.IsValid());
  DateTime_S = DateTime.ToString("%Y-%m-%d %H:%M:%S");
  EXPECT_STREQ(DateTime_S.c_str(), "2071-01-01 00:00:00");

  DateTime = BofDateTime(31, 12, 2070, 23, 59, 0, 0);
  EXPECT_TRUE(DateTime.IsValid());
  EXPECT_FALSE(DateTime.IsMidnight());

  // SECOND
  DateTime = BofDateTime(31, 12, 2070, 23, 59, 255, 0);
  EXPECT_TRUE(DateTime.IsValid());
  DateTime_S = DateTime.ToString("%Y-%m-%d %H:%M:%S");
  EXPECT_STREQ(DateTime_S.c_str(), "2071-01-01 00:03:15");

  DateTime = BofDateTime(31, 12, 2070, 23, 59, 60, 0);
  EXPECT_TRUE(DateTime.IsValid());

  DateTime = BofDateTime(31, 12, 2070, 23, 59, 59, 0);
  EXPECT_TRUE(DateTime.IsValid());

  // MICROSECOND
  DateTime = BofDateTime(31, 12, 2070, 23, 59, 59, 1000000);
  EXPECT_FALSE(DateTime.IsValid());

  DateTime = BofDateTime(31, 12, 2070, 23, 59, 59, 999999);
  EXPECT_TRUE(DateTime.IsValid());

  // LEAP
  DateTime = BofDateTime(28, 2, 2000, 0, 0, 0, 0);
  EXPECT_TRUE(DateTime.IsValid());

  DateTime = BofDateTime(29, 2, 2000, 0, 0, 0, 0);
  EXPECT_TRUE(DateTime.IsValid());

  DateTime = BofDateTime(30, 2, 2000, 0, 0, 0, 0);
  EXPECT_FALSE(DateTime.IsValid());
}

TEST(DateTime_Test, IsLeapYear)
{
  bool Sts_B;

  Sts_B = Bof_IsLeapYear(1900);
  EXPECT_FALSE(Sts_B);

  Sts_B = Bof_IsLeapYear(2000);
  EXPECT_TRUE(Sts_B);

  Sts_B = Bof_IsLeapYear(2008);
  EXPECT_TRUE(Sts_B);
}

TEST(DateTime_Test, GetDateTime)
{
  BOFERR Sts_E;
  BofDateTime DateTime;

  DateTime.Reset();

  Sts_E = Bof_Now(DateTime);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  EXPECT_TRUE(DateTime.IsValid());
}

TEST(DateTime_Test, SetDateTime)
{
  BOFERR Sts_E;
  BofDateTime DateTimeOrg; // , DateTime, DateTimeNew;

  Sts_E = Bof_Now(DateTimeOrg);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

#if 0 // Disable setdate time to check if it can explain teamcity hangout
  BofDateTime DateTime, DateTimeNew;

  DateTime = BofDateTime(31,12,2013,23,59,59,0);
  Sts_E = Bof_SetDateTime(DateTime);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Bof_MsSleep(2000);                         // We setup time to 23:59:59->wait to pass day
  Sts_E = Bof_Now(DateTimeNew);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DateTimeNew.Day(), 1); // We setup time to 23:59:59->wait to pass day
  EXPECT_EQ(DateTimeNew.Month(), 1);
  EXPECT_EQ(DateTimeNew.Year(), 2014);

  EXPECT_EQ(DateTimeNew.Hour(), 0);     // We setup time to 23:59:59->wait to pass day
  EXPECT_EQ(DateTimeNew.Minute(), 0);
  EXPECT_LT(DateTimeNew.Second(), 4);

  Sts_E = Bof_SetDateTime(DateTimeOrg);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
#endif
}

TEST(DateTime_Test, DeltaMsToHms)
{
  uint32_t DeltaInMs_U32, Day_U32, Hour_U32, Minute_U32, Second_U32, Ms_U32, OutDay_U32, OutHour_U32, OutMinute_U32, OutSecond_U32, OutMs_U32;

  EXPECT_EQ(Bof_DeltaMsToHms(0, OutDay_U32, OutHour_U32, OutMinute_U32, OutSecond_U32, OutMs_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(0, OutDay_U32);
  EXPECT_EQ(0, OutHour_U32);
  EXPECT_EQ(0, OutMinute_U32);
  EXPECT_EQ(0, OutSecond_U32);
  EXPECT_EQ(0, OutMs_U32);

  Day_U32 = 1;
  Hour_U32 = 2;
  Minute_U32 = 3;
  Second_U32 = 4;
  Ms_U32 = 5;
  DeltaInMs_U32 = (Day_U32 * 24 * 60 * 60 * 1000) + (Hour_U32 * 60 * 60 * 1000) + (Minute_U32 * 60 * 1000) + (Second_U32 * 1000) + Ms_U32;
  EXPECT_EQ(Bof_DeltaMsToHms(DeltaInMs_U32, OutDay_U32, OutHour_U32, OutMinute_U32, OutSecond_U32, OutMs_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Day_U32, OutDay_U32);
  EXPECT_EQ(Hour_U32, OutHour_U32);
  EXPECT_EQ(Minute_U32, OutMinute_U32);
  EXPECT_EQ(Second_U32, OutSecond_U32);
  EXPECT_EQ(Ms_U32, OutMs_U32);

  EXPECT_EQ(Bof_DeltaMsToHms(0xFFFFFFFF, OutDay_U32, OutHour_U32, OutMinute_U32, OutSecond_U32, OutMs_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(49, OutDay_U32);
  EXPECT_EQ(17, OutHour_U32);
  EXPECT_EQ(2, OutMinute_U32);
  EXPECT_EQ(47, OutSecond_U32);
  EXPECT_EQ(295, OutMs_U32);
}

TEST(DateTime_Test, DiffDateTime)
{
  BOFERR Sts_E;
  int32_t DiffDay_S32;
  BofDateTime FirstDateTime, SecondDateTime, DiffTime;

  FirstDateTime = BofDateTime(26, 5, 2000, 0, 0, 0, 0);
  SecondDateTime = BofDateTime(27, 5, 2000, 1, 2, 3, 4);

  Sts_E = Bof_DiffDateTime(SecondDateTime, FirstDateTime, DiffTime, DiffDay_S32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(DiffTime.IsValid());
  EXPECT_EQ(DiffDay_S32, 1);
  EXPECT_EQ(DiffTime.Hour(), 1);
  EXPECT_EQ(DiffTime.Minute(), 2);
  EXPECT_EQ(DiffTime.Second(), 3);
  EXPECT_EQ(DiffTime.MicroSecond(), 4);
  EXPECT_EQ(DiffTime.Day(), 0);
  EXPECT_EQ(DiffTime.Month(), 0);
  EXPECT_EQ(DiffTime.Year(), 0);

  FirstDateTime = BofDateTime(26, 5, 2000, 1, 2, 3, 4);
  SecondDateTime = BofDateTime(27, 5, 2000, 0, 0, 0, 0);

  Sts_E = Bof_DiffDateTime(SecondDateTime, FirstDateTime, DiffTime, DiffDay_S32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(DiffTime.IsValid());
  EXPECT_EQ(DiffDay_S32, 0);
  EXPECT_EQ(DiffTime.Hour(), 22);
  EXPECT_EQ(DiffTime.Minute(), 57);
  EXPECT_EQ(DiffTime.Second(), 56);
  EXPECT_EQ(DiffTime.MicroSecond(), 999996);
  EXPECT_EQ(DiffTime.Day(), 0);
  EXPECT_EQ(DiffTime.Month(), 0);
  EXPECT_EQ(DiffTime.Year(), 0);

  FirstDateTime = BofDateTime(18, 1, 2000, 1, 30, 20, 0);
  SecondDateTime = FirstDateTime;
  Sts_E = Bof_DiffDateTime(SecondDateTime, FirstDateTime, DiffTime, DiffDay_S32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(DiffTime.IsValid());
  EXPECT_EQ(DiffDay_S32, 0);
  EXPECT_EQ(DiffTime.Hour(), 0);
  EXPECT_EQ(DiffTime.Minute(), 0);
  EXPECT_EQ(DiffTime.Second(), 0);
  EXPECT_EQ(DiffTime.MicroSecond(), 0);
  EXPECT_EQ(DiffTime.Day(), 0);
  EXPECT_EQ(DiffTime.Month(), 0);
  EXPECT_EQ(DiffTime.Year(), 0);

  SecondDateTime = BofDateTime(18, 1, 2001, 1, 30, 20, 0);
  Sts_E = Bof_DiffDateTime(SecondDateTime, FirstDateTime, DiffTime, DiffDay_S32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(DiffTime.IsValid());
  EXPECT_EQ(DiffDay_S32, 366);
  EXPECT_EQ(DiffTime.Hour(), 0);
  EXPECT_EQ(DiffTime.Minute(), 0);
  EXPECT_EQ(DiffTime.Second(), 0);
  EXPECT_EQ(DiffTime.MicroSecond(), 0);
  EXPECT_EQ(DiffTime.Day(), 0);
  EXPECT_EQ(DiffTime.Month(), 0);
  EXPECT_EQ(DiffTime.Year(), 0);

  SecondDateTime = BofDateTime(18, 2, 2001, 1, 30, 20, 0);
  Sts_E = Bof_DiffDateTime(SecondDateTime, FirstDateTime, DiffTime, DiffDay_S32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(DiffTime.IsValid());
  EXPECT_EQ(DiffDay_S32, 366 + 31);
  EXPECT_EQ(DiffTime.Hour(), 0);
  EXPECT_EQ(DiffTime.Minute(), 0);
  EXPECT_EQ(DiffTime.Second(), 0);
  EXPECT_EQ(DiffTime.MicroSecond(), 0);
  EXPECT_EQ(DiffTime.Day(), 0);
  EXPECT_EQ(DiffTime.Month(), 0);
  EXPECT_EQ(DiffTime.Year(), 0);

  SecondDateTime = BofDateTime(25, 2, 2001, 1, 30, 20, 0);
  Sts_E = Bof_DiffDateTime(SecondDateTime, FirstDateTime, DiffTime, DiffDay_S32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(DiffTime.IsValid());
  EXPECT_EQ(DiffDay_S32, 366 + 31 + 7);
  EXPECT_EQ(DiffTime.Hour(), 0);
  EXPECT_EQ(DiffTime.Minute(), 0);
  EXPECT_EQ(DiffTime.Second(), 0);
  EXPECT_EQ(DiffTime.MicroSecond(), 0);
  EXPECT_EQ(DiffTime.Day(), 0);
  EXPECT_EQ(DiffTime.Month(), 0);
  EXPECT_EQ(DiffTime.Year(), 0);

  FirstDateTime = BofDateTime(18, 1, 2000, 0, 0, 0, 0);
  SecondDateTime = BofDateTime(18, 1, 2000, 0, 0, 1, 999999);
  Sts_E = Bof_DiffDateTime(SecondDateTime, FirstDateTime, DiffTime, DiffDay_S32);
  EXPECT_TRUE(DiffTime.IsValid());
  EXPECT_EQ(DiffDay_S32, 0);
  EXPECT_EQ(DiffTime.Hour(), 0);
  EXPECT_EQ(DiffTime.Minute(), 0);
  EXPECT_EQ(DiffTime.Second(), 1);
  EXPECT_EQ(DiffTime.MicroSecond(), 999999);
  EXPECT_EQ(DiffTime.Day(), 0);
  EXPECT_EQ(DiffTime.Month(), 0);
  EXPECT_EQ(DiffTime.Year(), 0);

  SecondDateTime = BofDateTime(18, 1, 2000, 0, 2, 1, 1);
  Sts_E = Bof_DiffDateTime(SecondDateTime, FirstDateTime, DiffTime, DiffDay_S32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(DiffTime.IsValid());
  EXPECT_EQ(DiffDay_S32, 0);
  EXPECT_EQ(DiffTime.Hour(), 0);
  EXPECT_EQ(DiffTime.Minute(), 2);
  EXPECT_EQ(DiffTime.Second(), 1);
  EXPECT_EQ(DiffTime.MicroSecond(), 1);
  EXPECT_EQ(DiffTime.Day(), 0);
  EXPECT_EQ(DiffTime.Month(), 0);
  EXPECT_EQ(DiffTime.Year(), 0);

  SecondDateTime = BofDateTime(18, 1, 2000, 3, 2, 1, 1024);
  Sts_E = Bof_DiffDateTime(SecondDateTime, FirstDateTime, DiffTime, DiffDay_S32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(DiffTime.IsValid());
  EXPECT_EQ(DiffDay_S32, 0);
  EXPECT_EQ(DiffTime.Hour(), 3);
  EXPECT_EQ(DiffTime.Minute(), 2);
  EXPECT_EQ(DiffTime.Second(), 1);
  EXPECT_EQ(DiffTime.MicroSecond(), 1024);
  EXPECT_EQ(DiffTime.Day(), 0);
  EXPECT_EQ(DiffTime.Month(), 0);
  EXPECT_EQ(DiffTime.Year(), 0);
}

TEST(DateTime_Test, NbDaySinceUnixEpoch)
{
  BofDateTime DateTime;
  std::string DateTime_S;
  uint32_t NbDaySinceUnixEpoch_U32;

  DateTime = BofDateTime(2, 1, 1970, 12, 13, 14, 15);
  EXPECT_EQ(Bof_BofDateTime_To_NbDaySinceUnixEpoch(DateTime, NbDaySinceUnixEpoch_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(NbDaySinceUnixEpoch_U32, 1);

  DateTime = BofDateTime(2, 3, 1970, 0, 0, 0, 0);
  EXPECT_EQ(Bof_BofDateTime_To_NbDaySinceUnixEpoch(DateTime, NbDaySinceUnixEpoch_U32), BOF_ERR_NO_ERROR);
  EXPECT_EQ(NbDaySinceUnixEpoch_U32, 60);

  DateTime.Reset();
  EXPECT_EQ(Bof_NbDaySinceUnixEpoch_To_BofDateTime(NbDaySinceUnixEpoch_U32, DateTime), BOF_ERR_NO_ERROR);
  DateTime_S = DateTime.ToString("%Y-%m-%d %H:%M:%S.%q"); //%q is used by BofDateTime to display MicroSecond_U32
  EXPECT_STREQ(DateTime_S.c_str(), "1970-03-02 00:00:00.0");

  EXPECT_EQ(Bof_NbDaySinceUnixEpoch_To_BofDateTime(365 + 31 + 7, DateTime), BOF_ERR_NO_ERROR);
  DateTime_S = DateTime.ToString("%Y-%m-%d %H:%M:%S.%q us=%q"); //%q is used by BofDateTime to display MicroSecond_U32
  EXPECT_STREQ(DateTime_S.c_str(), "1971-02-08 00:00:00.0 us=0");
}

TEST(DateTime_Test, StringDateTime)
{
  BofDateTime DateTime, DateTimeFromString;
  std::string DateTime_S;

  EXPECT_EQ(Bof_NbDaySinceUnixEpoch_To_BofDateTime(365 + 31 + 7, DateTime), BOF_ERR_NO_ERROR);
  DateTime_S = DateTime.ToString("%Y-%m-%d %H:%M:%S.%q us=%q"); //%q is used by BofDateTime to display MicroSecond_U32
  EXPECT_STREQ(DateTime_S.c_str(), "1971-02-08 00:00:00.0 us=0");

  DateTime = BofDateTime(12, 34, 56, 0);
  ;
  DateTime_S = DateTime.ToString();
  DateTimeFromString = DateTimeFromString.FromString(DateTime_S);
  EXPECT_TRUE(DateTime == DateTimeFromString);
  EXPECT_STREQ(DateTime_S.c_str(), "1970-01-01 12:34:56");

  DateTime_S = DateTime.ToString("%b %d %H:%M:%S");
  DateTimeFromString.Reset();
  DateTimeFromString = DateTimeFromString.FromString(DateTime_S, "%b %d %H:%M:%S");
  // DateTimeFromString.Year() = DateTime.Year();  //Not in format string
  // DateTimeFromString.Second() = 56;//Not in format string
  // DateTimeFromString.DayOfWeek_U8 = 1;//Bad as Year_U16/Second_U8 are not in format string
  EXPECT_TRUE(DateTime == DateTimeFromString);
  EXPECT_STREQ(DateTime_S.c_str(), "Jan 01 12:34:56");

  DateTime = BofDateTime(2, 1, 1970, 1, 2, 3, 123456);
  DateTime_S = DateTime.ToString("%Y-%m-%d %H:%M:%S.%q us=%q"); //%q is used by BofDateTime to display MicroSecond_U32
  EXPECT_STREQ(DateTime_S.c_str(), "1970-01-02 01:02:03.123456 us=123456");
  DateTime_S = DateTime.ToString("%Y-%m-%d %H:%M:%S us %q"); //%q is used by BofDateTime to display MicroSecond_U32
  EXPECT_STREQ(DateTime_S.c_str(), "1970-01-02 01:02:03 us 123456");
  DateTime_S = DateTime.ToString("%Y-%m-%d %H:%M:%S.%q"); //%q is used by BofDateTime to display MicroSecond_U32
  EXPECT_STREQ(DateTime_S.c_str(), "1970-01-02 01:02:03.123456");

  DateTime.FromString("20011009.987654", "%Y%m%d.%q");
  DateTime_S = DateTime.ToString("%Y-%m-%d %H:%M:%S.%q us=%q"); //%q is used by BofDateTime to display MicroSecond_U32
  EXPECT_STREQ(DateTime_S.c_str(), "2001-10-09 00:00:00.987654 us=987654");

  DateTime.FromString("20011009 ms=987650", "%Y%m%d ms=%q");
  DateTime_S = DateTime.ToString("%Y-%m-%d %H:%M:%S.%q"); //%q is used by BofDateTime to display MicroSecond_U32
  EXPECT_STREQ(DateTime_S.c_str(), "2001-10-09 00:00:00.987650");
}

TEST(DateTime_Test, TickSleep)
{
  BOFERR Sts_E;
  BofDateTime FirstDateTime, SecondDateTime, DiffTime;
  uint32_t Start_U32, Delta_U32, i_U32;
  int32_t DiffDay_S32;
  const uint32_t NBLOOP = 20;

  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NBLOOP; i_U32++)
  {
    Bof_MsSleep(1);
  }
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, i_U32 * 1);
  EXPECT_LT(Delta_U32, i_U32 * 1 * 3); // Win 1562 Lin 106 New Win with timeBeginPeriod:124
  //printf("1: %d\r\n", Delta_U32);

  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NBLOOP; i_U32++)
  {
    Bof_MsSleep(2);
  }
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, i_U32 * 2);
  EXPECT_LT(Delta_U32, i_U32 * 2 * 2); // Win 1562 Lin 206 New Win with timeBeginPeriod:216

  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NBLOOP; i_U32++)
  {
    Bof_MsSleep(5);
  }
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, i_U32 * 5);
  EXPECT_LT(Delta_U32, static_cast<uint32_t>(static_cast<float>(i_U32) * 5.0f * 1.5f)); // Win 1562 Lin 506 New Win with timeBeginPeriod:517
  //printf("5: %d\r\n", Delta_U32);

  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NBLOOP; i_U32++)
  {
    Bof_MsSleep(10);
  }
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, i_U32 * 10);
  EXPECT_LT(Delta_U32, static_cast<uint32_t>(static_cast<float>(i_U32 * 10) * 1.5f)); // Win 1562 Lin 1005 New Win with timeBeginPeriod:1021
  //printf("10: %d\r\n", Delta_U32);

  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NBLOOP; i_U32++)
  {
    Bof_MsSleep(20);
  }
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, i_U32 * 20);
  EXPECT_LT(Delta_U32, static_cast<uint32_t>(static_cast<float>(i_U32 * 20) * 1.5f)); // Win 3125 Lin 2005 New Win with timeBeginPeriod:2041
  //printf("20: %d\r\n", Delta_U32);

  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NBLOOP; i_U32++)
  {
    Bof_MsSleep(40);
  }
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, i_U32 * 40);
  EXPECT_LT(Delta_U32, static_cast<uint32_t>(static_cast<float>(i_U32 * 40) * 1.1f)); // Win 4688 Lin 4006 New Win with timeBeginPeriod:4074
  //printf("40: %d\r\n", Delta_U32);

  Start_U32 = Bof_GetMsTickCount();
  Sts_E = Bof_Now(FirstDateTime);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Bof_MsSleep(3000);

  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  Sts_E = Bof_Now(SecondDateTime);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_DiffDateTime(SecondDateTime, FirstDateTime, DiffTime, DiffDay_S32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DiffDay_S32, 0);
  EXPECT_EQ(DiffTime.Hour(), 0);
  EXPECT_EQ(DiffTime.Minute(), 0);
  EXPECT_GE(DiffTime.Second(), 3);
  EXPECT_LE(DiffTime.Second(), 4);
  // EXPECT_EQ(DiffTime.MicroSecond(), 0);
  EXPECT_GT(Delta_U32, (uint32_t)2900);
  EXPECT_LT(Delta_U32, (uint32_t)3100);

  Start_U32 = Bof_GetMsTickCount();
  Sts_E = Bof_Now(FirstDateTime);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Bof_MsSleep(100);
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  Sts_E = Bof_Now(SecondDateTime);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_DiffDateTime(SecondDateTime, FirstDateTime, DiffTime, DiffDay_S32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DiffDay_S32, 0);
  EXPECT_EQ(DiffTime.Hour(), 0);
  EXPECT_EQ(DiffTime.Minute(), 0);
  // can be one
  EXPECT_LE(DiffTime.Second(), 1);
  // EXPECT_EQ(DiffTime.MicroSecond(), 0);
  EXPECT_GE(Delta_U32, (uint32_t)90);
  EXPECT_LE(Delta_U32, (uint32_t)110);

  Start_U32 = Bof_GetMsTickCount();
  Sts_E = Bof_Now(FirstDateTime);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Bof_MsSleep(30); // Sleep can't be lower than the timer res (on some system it is 15 ms)
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  Sts_E = Bof_Now(SecondDateTime);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_DiffDateTime(SecondDateTime, FirstDateTime, DiffTime, DiffDay_S32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DiffDay_S32, 0);
  EXPECT_EQ(DiffTime.Hour(), 0);
  EXPECT_EQ(DiffTime.Minute(), 0);
  // can be one
  EXPECT_LE(DiffTime.Second(), 1);
  // EXPECT_EQ(DiffTime.MicroSecond(), 0);
  EXPECT_GE(Delta_U32, (uint32_t)15);
  EXPECT_LE(Delta_U32, (uint32_t)45);
}
