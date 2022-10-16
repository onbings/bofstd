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
  BOF_DATE_TIME DateTime_X;

  DateTime_X = BOF_DATE_TIME(2, 1, 1970, 1, 2, 3, 123456);
  DateTime_X.ToString("%Y-%m-%d %H:%M:%S %q ms");    //%q is used by BOF_DATE_TIME to display MicroSecond_U32
  DateTime_X.ToString("%Y-%m-%d %H:%M:%S ms %q");    //%q is used by BOF_DATE_TIME to display MicroSecond_U32
  DateTime_X.ToString("%Y-%m-%d %H:%M:%S.%q");    //%q is used by BOF_DATE_TIME to display MicroSecond_U32
  DateTime_X.FromString("20011009.987654", "%Y%m%d.%q");
  DateTime_X.ToString("%Y-%m-%d %H:%M:%S.%q");    //%q is used by BOF_DATE_TIME to display MicroSecond_U32
  DateTime_X.FromString("20011009 ms=987650", "%Y%m%d ms=%q");
  DateTime_X.ToString("%Y-%m-%d %H:%M:%S.%q");    //%q is used by BOF_DATE_TIME to display MicroSecond_U32

  DateTime_X.Reset();
  EXPECT_TRUE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 1, 1, 1970, 0, 0, 0, 0);
  EXPECT_TRUE(DateTime_X.IsValid_B);

  // DAY
  DateTime_X = BOF_DATE_TIME( 0, 1, 1970, 0, 0, 0, 0);
  EXPECT_FALSE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 32, 1, 1970, 0, 0, 0, 0);
  EXPECT_FALSE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 31, 1, 1970, 0, 0, 0, 0);
  EXPECT_TRUE(DateTime_X.IsValid_B);

  // MONTH
  DateTime_X = BOF_DATE_TIME( 31, 0, 1970, 0, 0, 0, 0);
  EXPECT_FALSE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 31, 13, 1970, 0, 0, 0, 0);
  EXPECT_FALSE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 31, 12, 1970, 0, 0, 0, 0);
  EXPECT_TRUE(DateTime_X.IsValid_B);

  // YEAR
  DateTime_X = BOF_DATE_TIME( 31, 12, 1969, 0, 0, 0, 0);
  EXPECT_FALSE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 31, 12, 3071, 0, 0, 0, 0);
  EXPECT_FALSE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 31, 12, 3070, 0, 0, 0, 0);
  EXPECT_TRUE(DateTime_X.IsValid_B);

  // HOUR
  DateTime_X = BOF_DATE_TIME( 31, 12, 2070, 25, 0, 0, 0);
  EXPECT_FALSE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 31, 12, 2070, 24, 0, 0, 0);
  EXPECT_FALSE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 31, 12, 2070, 23, 0, 0, 0);
  EXPECT_TRUE(DateTime_X.IsValid_B);

  // MINUTE
  DateTime_X = BOF_DATE_TIME( 31, 12, 2070, 23, 255, 0, 0);
  EXPECT_FALSE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 31, 12, 2070, 23, 60, 0, 0);
  EXPECT_FALSE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 31, 12, 2070, 23, 59, 0, 0);
  EXPECT_TRUE(DateTime_X.IsValid_B);

  // SECOND
  DateTime_X = BOF_DATE_TIME( 31, 12, 2070, 23, 59, 255, 0);
  EXPECT_FALSE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 31, 12, 2070, 23, 59, 60, 0);
  EXPECT_TRUE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 31, 12, 2070, 23, 59, 59, 0);
  EXPECT_TRUE(DateTime_X.IsValid_B);

  // MICROSECOND
  DateTime_X = BOF_DATE_TIME( 31, 12, 2070, 23, 59, 59, 1000000);
  EXPECT_FALSE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 31, 12, 2070, 23, 59, 59, 999999);
  EXPECT_TRUE(DateTime_X.IsValid_B);

  // LEAP
  DateTime_X = BOF_DATE_TIME( 28, 2, 2000, 0, 0, 0, 0);
  EXPECT_TRUE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 29, 2, 2000, 0, 0, 0, 0);
  EXPECT_TRUE(DateTime_X.IsValid_B);

  DateTime_X = BOF_DATE_TIME( 30, 2, 2000, 0, 0, 0, 0);
  EXPECT_FALSE(DateTime_X.IsValid_B);
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
  BOFERR        Sts_E;
  BOF_DATE_TIME DateTime_X;

  DateTime_X.Reset();

  Sts_E = Bof_Now(DateTime_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  EXPECT_TRUE(DateTime_X.IsValid_B);
}

TEST(DateTime_Test, SetDateTime)
{
  BOFERR        Sts_E;
  BOF_DATE_TIME DateTimeOrg_X; // , DateTime_X, DateTimeNew_X;

  Sts_E = Bof_Now(DateTimeOrg_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

#if 0                                  // Disable setdate time to check if it can explain teamcity hangout
  DateTime_X = DateTimeOrg_X;
  DateTime_X.Day_U8 = 26;
  Sts_E = Bof_SetDateTime(DateTime_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  DateTime_X.Month_U8 = 13;
  Sts_E = Bof_SetDateTime(DateTime_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  DateTime_X.Month_U8 = 12;
  Sts_E = Bof_SetDateTime(DateTime_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  DateTime_X.Year_U16 = 12000;
  Sts_E = Bof_SetDateTime(DateTime_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  DateTime_X.Year_U16 = 2014;
  Sts_E = Bof_SetDateTime(DateTime_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  DateTime_X.Hour_U8 = 24;
  Sts_E = Bof_SetDateTime(DateTime_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  DateTime_X.Hour_U8 = 23;
  Sts_E = Bof_SetDateTime(DateTime_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  DateTime_X.Minute_U8 = 60;
  Sts_E = Bof_SetDateTime(DateTime_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  DateTime_X.Minute_U8 = 59;
  Sts_E = Bof_SetDateTime(DateTime_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  DateTime_X.Second_U8 = 60;
  Sts_E = Bof_SetDateTime(DateTime_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  DateTime_X.Second_U8 = 59;
  Sts_E = Bof_SetDateTime(DateTime_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  DateTime_X.MicroSecond_U32 = 1000 * 1000;
  Sts_E = Bof_SetDateTime(DateTime_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  DateTime_X.MicroSecond_U32 = 999 * 1000;
  Sts_E = Bof_SetDateTime(DateTime_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  DateTime_X.Day_U8 = 32;
  Sts_E = Bof_SetDateTime(DateTime_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);


  Bof_MsSleep(2000);                         // We setup time to 23:59:59->wait to pass day
  Sts_E = Bof_Now(DateTimeNew_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DateTimeNew_X.Day_U8, 26 + 1); // We setup time to 23:59:59->wait to pass day

  EXPECT_EQ(DateTimeNew_X.Month_U8, 12);
  EXPECT_EQ(DateTimeNew_X.Year_U16, 2014);

  // EXPECT_EQ(DateTimeNew_X.DayOfWeek_U8, DateTime_X.DayOfWeek_U8);
  EXPECT_EQ(DateTimeNew_X.Hour_U8, 0);     // We setup time to 23:59:59->wait to pass day
  EXPECT_EQ(DateTimeNew_X.Minute_U8, 0);
  EXPECT_LT(DateTimeNew_X.Second_U8, 4);

  // EXPECT_EQ(DateTimeNew_X.MicroSecond_U32, DateTime_X.MicroSecond_U32);

  Sts_E = Bof_SetDateTime(DateTimeOrg_X);
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

  Day_U32 = 1; Hour_U32 = 2; Minute_U32 = 3; Second_U32 = 4; Ms_U32 = 5; DeltaInMs_U32 = (Day_U32 * 24 * 60 * 60 * 1000) + (Hour_U32 * 60 * 60 * 1000) + (Minute_U32 * 60 * 1000) + (Second_U32 * 1000) + Ms_U32;
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
  BOFERR        Sts_E;
  uint32_t      DiffDay_U32;
  double DayNumber_lf;
  BOF_DATE_TIME FirstDateTime_X, SecondDateTime_X, DiffTime_X;

  FirstDateTime_X.Reset();
  Sts_E = Bof_DateTimeToNumber(FirstDateTime_X, DayNumber_lf);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  FirstDateTime_X.Day_U8 = 26;
  FirstDateTime_X.Month_U8 = 5;
  FirstDateTime_X.Year_U16 = 2000;
  Sts_E = Bof_DateTimeToNumber(FirstDateTime_X, DayNumber_lf);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DayNumber_lf, 730571.0f);

  FirstDateTime_X.Reset();
  SecondDateTime_X.Reset();
  DiffTime_X.Reset();

  Sts_E = Bof_DiffDateTime(SecondDateTime_X, FirstDateTime_X, DiffTime_X, DiffDay_U32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DiffDay_U32, 0);
  EXPECT_EQ(DiffTime_X.Hour_U8, 0);
  EXPECT_EQ(DiffTime_X.Minute_U8, 0);
  EXPECT_EQ(DiffTime_X.Second_U8, 0);
  EXPECT_EQ(DiffTime_X.MicroSecond_U32, 0);
  EXPECT_EQ(DiffTime_X.Day_U8, 1);
  EXPECT_EQ(DiffTime_X.Month_U8, 1);
  EXPECT_EQ(DiffTime_X.Year_U16, 1970);
  EXPECT_EQ(DiffTime_X.DayOfWeek(), 4);

  FirstDateTime_X.Day_U8 = 18;
  FirstDateTime_X.Month_U8 = 1;
  FirstDateTime_X.Year_U16 = 2000;
  FirstDateTime_X.Hour_U8 = 1;
  FirstDateTime_X.Minute_U8 = 30;
  FirstDateTime_X.Second_U8 = 20;
  FirstDateTime_X.MicroSecond_U32 = 0;

  SecondDateTime_X = FirstDateTime_X;
  Sts_E = Bof_DiffDateTime(SecondDateTime_X, FirstDateTime_X, DiffTime_X, DiffDay_U32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DiffDay_U32, 0);
  EXPECT_EQ(DiffTime_X.Hour_U8, 0);
  EXPECT_EQ(DiffTime_X.Minute_U8, 0);
  EXPECT_EQ(DiffTime_X.Second_U8, 0);

  SecondDateTime_X.Year_U16 = static_cast<uint16_t>(FirstDateTime_X.Year_U16 + 1);
  Sts_E = Bof_DiffDateTime(SecondDateTime_X, FirstDateTime_X, DiffTime_X, DiffDay_U32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DiffDay_U32, 366);
  EXPECT_EQ(DiffTime_X.Hour_U8, 0);
  EXPECT_EQ(DiffTime_X.Minute_U8, 0);
  EXPECT_EQ(DiffTime_X.Second_U8, 0);

  SecondDateTime_X.Month_U8 = static_cast<uint8_t>(FirstDateTime_X.Month_U8 + 1);
  Sts_E = Bof_DiffDateTime(SecondDateTime_X, FirstDateTime_X, DiffTime_X, DiffDay_U32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DiffDay_U32, 366 + 31);
  EXPECT_EQ(DiffTime_X.Hour_U8, 0);
  EXPECT_EQ(DiffTime_X.Minute_U8, 0);
  EXPECT_EQ(DiffTime_X.Second_U8, 0);

  SecondDateTime_X.Day_U8 = static_cast<uint8_t>(FirstDateTime_X.Day_U8 + 7);
  Sts_E = Bof_DiffDateTime(SecondDateTime_X, FirstDateTime_X, DiffTime_X, DiffDay_U32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DiffDay_U32, 366 + 31 + 7);
  EXPECT_EQ(DiffTime_X.Hour_U8, 0);
  EXPECT_EQ(DiffTime_X.Minute_U8, 0);
  EXPECT_EQ(DiffTime_X.Second_U8, 0);

  FirstDateTime_X.Hour_U8 = 0;
  FirstDateTime_X.Minute_U8 = 0;
  FirstDateTime_X.Second_U8 = 0;
  FirstDateTime_X.MicroSecond_U32 = 0;
  SecondDateTime_X = FirstDateTime_X;
  SecondDateTime_X.Second_U8 = 1;
  Sts_E = Bof_DiffDateTime(SecondDateTime_X, FirstDateTime_X, DiffTime_X, DiffDay_U32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DiffDay_U32, 0);
  EXPECT_EQ(DiffTime_X.Hour_U8, 0);
  EXPECT_EQ(DiffTime_X.Minute_U8, 0);
  EXPECT_EQ(DiffTime_X.Second_U8, 1);
  SecondDateTime_X.Minute_U8 = 2;
  Sts_E = Bof_DiffDateTime(SecondDateTime_X, FirstDateTime_X, DiffTime_X, DiffDay_U32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DiffDay_U32, 0);
  EXPECT_EQ(DiffTime_X.Hour_U8, 0);
  EXPECT_EQ(DiffTime_X.Minute_U8, 2);
  EXPECT_EQ(DiffTime_X.Second_U8, 1);

  SecondDateTime_X.Hour_U8 = 3;
  Sts_E = Bof_DiffDateTime(SecondDateTime_X, FirstDateTime_X, DiffTime_X, DiffDay_U32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DiffDay_U32, 0);
  EXPECT_EQ(DiffTime_X.Hour_U8, 3);
  EXPECT_EQ(DiffTime_X.Minute_U8, 2);
  EXPECT_EQ(DiffTime_X.Second_U8, 1);
}

TEST(DateTime_Test, StringDateTime)
{
  BOF_DATE_TIME DateTime_X, DateTimeFromString_X;
  std::string DateTime_S;

  EXPECT_EQ(Bof_DateInDaySinceEpoch_To_BofDateTime(365 + 31 + 7, DateTime_X), BOF_ERR_NO_ERROR);
  DateTime_X.Hour_U8 = 12;
  DateTime_X.Minute_U8 = 34;
  DateTime_X.Second_U8 = 56;
  DateTime_X.MicroSecond_U32 = 0;

  DateTime_S = DateTime_X.ToString();
  DateTimeFromString_X.Reset();
  DateTimeFromString_X = DateTimeFromString_X.FromString(DateTime_S);
  EXPECT_TRUE(DateTime_X == DateTimeFromString_X);
  EXPECT_STREQ(DateTime_S.c_str(), "1971-02-08 12:34:56");

  DateTime_S = DateTime_X.ToString("%b %d %H:%M");
  DateTimeFromString_X.Reset();
  DateTimeFromString_X = DateTimeFromString_X.FromString(DateTime_S, "%b %d %H:%M");
 // DateTimeFromString_X.Year_U16 = DateTime_X.Year_U16;  //Not in format string
 // DateTimeFromString_X.Second_U8 = 56;//Not in format string
 // DateTimeFromString_X.DayOfWeek_U8 = 1;//Bad as Year_U16/Second_U8 are not in format string
  EXPECT_TRUE(DateTime_X == DateTimeFromString_X);
  EXPECT_STREQ(DateTime_S.c_str(), "Feb 08 12:34");
}

TEST(DateTime_Test, TickSleep)
{
  BOFERR        Sts_E;
  BOF_DATE_TIME FirstDateTime_X, SecondDateTime_X, DiffTime_X;
  uint32_t      DiffDay_U32, Start_U32, Delta_U32, i_U32;
  const uint32_t NBLOOP = 20;

  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NBLOOP; i_U32++)
  {
    Bof_MsSleep(1);
  }
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, i_U32 * 1);
  EXPECT_LT(Delta_U32, i_U32 * 1 * 3);		//Win 1562 Lin 106 New Win with timeBeginPeriod:124
  printf("1: %d\r\n", Delta_U32);

  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NBLOOP; i_U32++)
  {
    Bof_MsSleep(2);
  }
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, i_U32 * 2);
  EXPECT_LT(Delta_U32, i_U32 * 2 * 2);		//Win 1562 Lin 206 New Win with timeBeginPeriod:216

  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NBLOOP; i_U32++)
  {
    Bof_MsSleep(5);
  }
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, i_U32 * 5);
  EXPECT_LT(Delta_U32, static_cast<uint32_t>(static_cast<float>(i_U32) * 5.0f * 1.5f));		//Win 1562 Lin 506 New Win with timeBeginPeriod:517
  printf("5: %d\r\n", Delta_U32);

  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NBLOOP; i_U32++)
  {
    Bof_MsSleep(10);
  }
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, i_U32 * 10);
  EXPECT_LT(Delta_U32, static_cast<uint32_t>(static_cast<float>(i_U32 * 10) * 1.5f));		//Win 1562 Lin 1005 New Win with timeBeginPeriod:1021
  printf("10: %d\r\n", Delta_U32);

  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NBLOOP; i_U32++)
  {
    Bof_MsSleep(20);
  }
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, i_U32 * 20);
  EXPECT_LT(Delta_U32, static_cast<uint32_t>(static_cast<float>(i_U32 * 20) * 1.5f));		//Win 3125 Lin 2005 New Win with timeBeginPeriod:2041
  printf("20: %d\r\n", Delta_U32);

  Start_U32 = Bof_GetMsTickCount();
  for (i_U32 = 0; i_U32 < NBLOOP; i_U32++)
  {
    Bof_MsSleep(40);
  }
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  EXPECT_GE(Delta_U32, i_U32 * 40);
  EXPECT_LT(Delta_U32, static_cast<uint32_t>(static_cast<float>(i_U32 * 40) * 1.1f));		//Win 4688 Lin 4006 New Win with timeBeginPeriod:4074
  printf("40: %d\r\n", Delta_U32);

  Start_U32 = Bof_GetMsTickCount();
  Sts_E = Bof_Now(FirstDateTime_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Bof_MsSleep(3000);
  /*
     uint32_t End_U32 = Bof_GetMsTickCount();
     uint32_t k = End_U32 - Start_U32;

     Start_U32 = 0xf7a0b107;
     End_U32 = 0x6d4;


     uint32_t now = 0x6d4;
     uint32_t start = 0xf7a0b107;

     uint32_t delta = 0xFFFFFFFF - start;
     delta = (now < start) ? 0xFFFFFFFF - start - now : now - start;
   */
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  Sts_E = Bof_Now(SecondDateTime_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_DiffDateTime(SecondDateTime_X, FirstDateTime_X, DiffTime_X, DiffDay_U32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DiffDay_U32, 0);
  EXPECT_EQ(DiffTime_X.Hour_U8, 0);
  EXPECT_EQ(DiffTime_X.Minute_U8, 0);
  EXPECT_GE(DiffTime_X.Second_U8, 3);
  EXPECT_LE(DiffTime_X.Second_U8, 4);
  EXPECT_EQ(DiffTime_X.MicroSecond_U32, 0);
  EXPECT_GT(Delta_U32, (uint32_t)2900);
  EXPECT_LT(Delta_U32, (uint32_t)3100);

  Start_U32 = Bof_GetMsTickCount();
  Sts_E = Bof_Now(FirstDateTime_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Bof_MsSleep(100);
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  Sts_E = Bof_Now(SecondDateTime_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_DiffDateTime(SecondDateTime_X, FirstDateTime_X, DiffTime_X, DiffDay_U32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DiffDay_U32, 0);
  EXPECT_EQ(DiffTime_X.Hour_U8, 0);
  EXPECT_EQ(DiffTime_X.Minute_U8, 0);
  // can be one
  EXPECT_LE(DiffTime_X.Second_U8, 1);
  EXPECT_EQ(DiffTime_X.MicroSecond_U32, 0);
  EXPECT_GE(Delta_U32, (uint32_t)90);
  EXPECT_LE(Delta_U32, (uint32_t)110);

  Start_U32 = Bof_GetMsTickCount();
  Sts_E = Bof_Now(FirstDateTime_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Bof_MsSleep(30);                       // Sleep can't be lower than the timer res (on some system it is 15 ms)
  Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  Sts_E = Bof_Now(SecondDateTime_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_DiffDateTime(SecondDateTime_X, FirstDateTime_X, DiffTime_X, DiffDay_U32);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(DiffDay_U32, 0);
  EXPECT_EQ(DiffTime_X.Hour_U8, 0);
  EXPECT_EQ(DiffTime_X.Minute_U8, 0);
  // can be one
  EXPECT_LE(DiffTime_X.Second_U8, 1);
  EXPECT_EQ(DiffTime_X.MicroSecond_U32, 0);
  EXPECT_GE(Delta_U32, (uint32_t)15);
  EXPECT_LE(Delta_U32, (uint32_t)45);

}
