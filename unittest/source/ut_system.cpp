/*
 * This module implement the unit tests of the system part of the Bof library
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

#include <gtest/gtest.h>
#include <bofstd/bofsystem.h>
#include <bofstd/bofstringformatter.h>
#include <bofstd/boffs.h>
#include <cstdint>

#if defined (_WIN32)
#include <windows.h>
#else
#include <errno.h>
#include <arpa/inet.h>
#endif

USE_BOF_NAMESPACE()
const std::vector< uint8_t > S_RtcpPacket =
{
	0x80, 0xC8, 0x00, 0x06, 0xE3, 0x3D, 0xB8, 0xEF, 0xDB, 0xE1, 0x5F, 0x7B, 0x65, 0xE3, 0x53, 0xF7, 0x74, 0x4B, 0x34, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x80, 0xC8, 0x00, 0x06, 0xE3, 0x3D, 0xB8, 0xEF, 0xDB, 0xE1, 0x5F, 0x80, 0x66, 0x24, 0xDD, 0x2F, 0x74, 0x52, 0x12, 0xBE, 0x00, 0x00, 0xA1, 0xAF, 0x03, 0x1B, 0x2B, 0xE8,
	0x80, 0xC8, 0x00, 0x06, 0xE3, 0x3D, 0xB8, 0xEF, 0xDB, 0xE1, 0x5F, 0x90, 0xF7, 0xCE, 0xD9, 0x16, 0x74, 0x68, 0xD3, 0xC8, 0x00, 0x00, 0xA1, 0xB5, 0x03, 0x1B, 0x4A, 0xFF,
	/* Packet 14954 */
	0x80, 0xc8, 0x00, 0x06, 0xde, 0x10, 0xd6, 0x8a, 0xdb, 0xe4, 0x3b, 0x94, 0x26, 0x24, 0xdd, 0x2f, 0xbd, 0x5c, 0x31, 0x36, 0x00, 0x00, 0x11, 0x65, 0x00, 0x02, 0x3e, 0x05,
	0x81, 0xca, 0x00, 0x05, 0xde, 0x10, 0xd6, 0x8a, 0x01, 0x0a, 0x62, 0x68, 0x61, 0x40, 0x42, 0x48, 0x41, 0x2d, 0x50, 0x43, 0x00, 0x00, 0x00, 0x00,
	/* Packet 33904 */
	0x80, 0xc8, 0x00, 0x06, 0xde, 0x10, 0xd6, 0x8a, 0xdb, 0xe4, 0x3b, 0x97, 0xd0, 0x62, 0x4d, 0xd2, 0xbd, 0x5c, 0xa3, 0x96, 0x00, 0x00, 0x12, 0x1c, 0x00, 0x02, 0x55, 0x9c,
	0x81, 0xca, 0x00, 0x05, 0xde, 0x10, 0xd6, 0x8a, 0x01, 0x0a, 0x62, 0x68, 0x61, 0x40, 0x42, 0x48, 0x41, 0x2d, 0x50, 0x43, 0x00, 0x00, 0x00, 0x00,
	/* Packet 62896 */
	0x80, 0xc8, 0x00, 0x06, 0xde, 0x10, 0xd6, 0x8a, 0xdb, 0xe4, 0x3b, 0x9d, 0x50, 0xe5, 0x60, 0x41, 0xbd, 0x5d, 0x4f, 0x76, 0x00, 0x00, 0x13, 0x2f, 0x00, 0x02, 0x79, 0x0f,
	0x81, 0xca, 0x00, 0x05, 0xde, 0x10, 0xd6, 0x8a, 0x01, 0x0a, 0x62, 0x68, 0x61, 0x40, 0x42, 0x48, 0x41, 0x2d, 0x50, 0x43, 0x00, 0x00, 0x00, 0x00,
	/* Packet 79332 */
	0x80, 0xc8, 0x00, 0x06, 0xde, 0x10, 0xd6, 0x8a, 0xdb, 0xe4, 0x3b, 0xa0, 0xeb, 0x85, 0x1e, 0xb8, 0xbd, 0x5d, 0xc0, 0x7e, 0x00, 0x00, 0x13, 0xe3, 0x00, 0x02, 0x90, 0x43,
	0x81, 0xca, 0x00, 0x05, 0xde, 0x10, 0xd6, 0x8a, 0x01, 0x0a, 0x62, 0x68, 0x61, 0x40, 0x42, 0x48, 0x41, 0x2d, 0x50, 0x43, 0x00, 0x00, 0x00, 0x00,
	/* Packet 104105 */
	0x80, 0xc8, 0x00, 0x06, 0xde, 0x10, 0xd6, 0x8a, 0xdb, 0xe4, 0x3b, 0xa5, 0xc1, 0xca, 0xc0, 0x83, 0xbd, 0x5e, 0x57, 0x3e, 0x00, 0x00, 0x14, 0xd5, 0x00, 0x02, 0xaf, 0x75,
	0x81, 0xca, 0x00, 0x05, 0xde, 0x10, 0xd6, 0x8a, 0x01, 0x0a, 0x62, 0x68, 0x61, 0x40, 0x42, 0x48, 0x41, 0x2d, 0x50, 0x43, 0x00, 0x00, 0x00, 0x00,
	/* Packet 119160 */
	0x80, 0xc8, 0x00, 0x06, 0xde, 0x10, 0xd6, 0x8a, 0xdb, 0xe4, 0x3b, 0xa8, 0xc2, 0x4d, 0xd2, 0xf1, 0xbd, 0x5e, 0xb5, 0x76, 0x00, 0x00, 0x15, 0x6a, 0x00, 0x02, 0xc2, 0xaa,
	0x81, 0xca, 0x00, 0x05, 0xde, 0x10, 0xd6, 0x8a, 0x01, 0x0a, 0x62, 0x68, 0x61, 0x40, 0x42, 0x48, 0x41, 0x2d, 0x50, 0x43, 0x00, 0x00, 0x00, 0x00, 0x81, 0xcb, 0x00, 0x01,0xde,  0x10, 0xd6, 0x8a,
};

/*** Test case ******************************************************************/

USE_BOF_NAMESPACE()

TEST(System_Test, TickDelta)
{
	uint32_t Timer_U32, Delta_U32;

	Timer_U32 = Bof_GetMsTickCount();

	do
	{
		Delta_U32 = Bof_ElapsedMsTime(Timer_U32);
	}
	while (Delta_U32 < 1000);
	EXPECT_LT(Delta_U32, (uint32_t)1050);
	EXPECT_GE(Delta_U32, (uint32_t)1000);
}

TEST(System_Test, ValidateDateTime)
{
	BOFERR        Sts_E;
	BOF_DATE_TIME DateTime_X;

	DateTime_X.Reset();

	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 1, 1, 1970, 0, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	// DAY
	BOF_SET_DATE_TIME(DateTime_X, 0, 1, 1970, 0, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 32, 1, 1970, 0, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 1, 1970, 0, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	// MONTH
	BOF_SET_DATE_TIME(DateTime_X, 31, 0, 1970, 0, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 13, 1970, 0, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 1970, 0, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	// YEAR
	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 1969, 0, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 3071, 0, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 3070, 0, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	// DAYOFWEEK
	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 0, 0, 0, 0);
	DateTime_X.DayOfWeek_U8    = 0xFF;
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 0, 0, 0, 0);
	DateTime_X.DayOfWeek_U8 = 7;
	Sts_E = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 0, 0, 0, 0);
	DateTime_X.DayOfWeek_U8 = 0;
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	// HOUR
	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 25, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 24, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 23, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	// MINUTE
	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 23, 255, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 23, 60, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 23, 59, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	// SECOND
	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 23, 59, 255, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 23, 59, 60, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);	//Leap second
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 23, 59, 59, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	// MILLISECOND
	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 23, 59, 59, 0xFFFF);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 23, 59, 59, 1000);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 31, 12, 2070, 23, 59, 59, 999);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	// LEAP
	BOF_SET_DATE_TIME(DateTime_X, 28, 2, 2000, 0, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 29, 2, 2000, 0, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	BOF_SET_DATE_TIME(DateTime_X, 30, 2, 2000, 0, 0, 0, 0);
	Sts_E                      = Bof_ValidateDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
}

TEST(System_Test, IsLeapYear)
{
	bool Sts_B;

	Sts_B = Bof_IsLeapYear(1900);
	EXPECT_FALSE(Sts_B);

	Sts_B = Bof_IsLeapYear(2000);
	EXPECT_TRUE(Sts_B);

	Sts_B = Bof_IsLeapYear(2008);
	EXPECT_TRUE(Sts_B);
}

TEST(System_Test, GetDateTime)
{
	BOFERR        Sts_E;
	BOF_DATE_TIME DateTime_X;

	DateTime_X.Reset();

	Sts_E = Bof_Now(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	Sts_E = Bof_ValidateDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

TEST(System_Test, SetDateTime)
{
	BOFERR        Sts_E;
	BOF_DATE_TIME DateTimeOrg_X; // , DateTime_X, DateTimeNew_X;

	Sts_E                      = Bof_Now(DateTimeOrg_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

#if 0                                  // Disable setdate time to check if it can explain teamcity hangout
	DateTime_X = DateTimeOrg_X;
	DateTime_X.Day_U8          = 26;
	Sts_E                      = Bof_SetDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	DateTime_X.Month_U8        = 13;
	Sts_E                      = Bof_SetDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	DateTime_X.Month_U8        = 12;
	Sts_E                      = Bof_SetDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	DateTime_X.Year_U16        = 12000;
	Sts_E                      = Bof_SetDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	DateTime_X.Year_U16        = 2014;
	Sts_E                      = Bof_SetDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	DateTime_X.Hour_U8         = 24;
	Sts_E                      = Bof_SetDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	DateTime_X.Hour_U8         = 23;
	Sts_E                      = Bof_SetDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	DateTime_X.Minute_U8       = 60;
	Sts_E                      = Bof_SetDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	DateTime_X.Minute_U8       = 59;
	Sts_E                      = Bof_SetDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	DateTime_X.Second_U8       = 60;
	Sts_E                      = Bof_SetDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	DateTime_X.Second_U8       = 59;
	Sts_E                      = Bof_SetDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	DateTime_X.Millisecond_U16 = 1000;
	Sts_E                      = Bof_SetDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	DateTime_X.Millisecond_U16 = 999;
	Sts_E                      = Bof_SetDateTime(DateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	DateTime_X.Day_U8          = 32;
	Sts_E                      = Bof_SetDateTime(DateTime_X);
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);


	Bof_MsSleep(2000);                         // We setup time to 23:59:59->wait to pass day
	Sts_E                      = Bof_Now(DateTimeNew_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(DateTimeNew_X.Day_U8, 26 + 1); // We setup time to 23:59:59->wait to pass day

	EXPECT_EQ(DateTimeNew_X.Month_U8, 12);
	EXPECT_EQ(DateTimeNew_X.Year_U16, 2014);

	// EXPECT_EQ(DateTimeNew_X.DayOfWeek_U8, DateTime_X.DayOfWeek_U8);
	EXPECT_EQ(DateTimeNew_X.Hour_U8, 0);     // We setup time to 23:59:59->wait to pass day
	EXPECT_EQ(DateTimeNew_X.Minute_U8, 0);
	EXPECT_LT(DateTimeNew_X.Second_U8, 4);

	// EXPECT_EQ(DateTimeNew_X.Millisecond_U16, DateTime_X.Millisecond_U16);

	Sts_E                      = Bof_SetDateTime(DateTimeOrg_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
#endif
}
TEST(System_Test, DeltaMsToHms)
{
  uint32_t DeltaInMs_U32,Day_U32, Hour_U32, Minute_U32, Second_U32, Ms_U32, OutDay_U32, OutHour_U32, OutMinute_U32, OutSecond_U32, OutMs_U32;

  EXPECT_EQ(Bof_DeltaMsToHms(0, OutDay_U32, OutHour_U32, OutMinute_U32, OutSecond_U32, OutMs_U32),BOF_ERR_NO_ERROR);
  EXPECT_EQ(0, OutDay_U32);
  EXPECT_EQ(0, OutHour_U32);
  EXPECT_EQ(0, OutMinute_U32);
  EXPECT_EQ(0, OutSecond_U32);
  EXPECT_EQ(0, OutMs_U32);

  Day_U32=1;Hour_U32= 2;Minute_U32=3;Second_U32=4;Ms_U32=5;DeltaInMs_U32=(Day_U32*24*60*60*1000)+(Hour_U32*60*60*1000)+(Minute_U32*60*1000)+(Second_U32*1000)+Ms_U32;
  EXPECT_EQ(Bof_DeltaMsToHms(DeltaInMs_U32, OutDay_U32, OutHour_U32, OutMinute_U32, OutSecond_U32, OutMs_U32),BOF_ERR_NO_ERROR);
  EXPECT_EQ(Day_U32, OutDay_U32);
  EXPECT_EQ(Hour_U32, OutHour_U32);
  EXPECT_EQ(Minute_U32, OutMinute_U32);
  EXPECT_EQ(Second_U32, OutSecond_U32);
  EXPECT_EQ(Ms_U32, OutMs_U32);

  EXPECT_EQ(Bof_DeltaMsToHms(0xFFFFFFFF, OutDay_U32, OutHour_U32, OutMinute_U32, OutSecond_U32, OutMs_U32),BOF_ERR_NO_ERROR);
  EXPECT_EQ(49, OutDay_U32);
  EXPECT_EQ(17, OutHour_U32);
  EXPECT_EQ(2, OutMinute_U32);
  EXPECT_EQ(47, OutSecond_U32);
  EXPECT_EQ(295, OutMs_U32);
}
TEST(System_Test, DiffDateTime)
{
	BOFERR        Sts_E;
  uint32_t      DiffDay_U32;
  double DayNumber_lf;
	BOF_DATE_TIME FirstDateTime_X, SecondDateTime_X, DiffTime_X;

	FirstDateTime_X.Reset();
	Sts_E                           = Bof_DateTimeToNumber(FirstDateTime_X, DayNumber_lf);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	FirstDateTime_X.Day_U8          = 26;
	FirstDateTime_X.Month_U8        = 5;
	FirstDateTime_X.Year_U16        = 2000;
	Sts_E                           = Bof_DateTimeToNumber(FirstDateTime_X, DayNumber_lf);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(DayNumber_lf, 730571.0f);

	FirstDateTime_X.Reset();
	SecondDateTime_X.Reset();
	DiffTime_X.Reset();

	Sts_E                           = Bof_DiffDateTime(SecondDateTime_X, FirstDateTime_X,  DiffTime_X, DiffDay_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(DiffDay_U32, 0);
	EXPECT_EQ(DiffTime_X.Hour_U8, 0);
	EXPECT_EQ(DiffTime_X.Minute_U8, 0);
	EXPECT_EQ(DiffTime_X.Second_U8, 0);
	EXPECT_EQ(DiffTime_X.Millisecond_U16, 0);
	EXPECT_EQ(DiffTime_X.Day_U8, 1);
	EXPECT_EQ(DiffTime_X.Month_U8, 1);
	EXPECT_EQ(DiffTime_X.Year_U16, 1970);
	EXPECT_EQ(DiffTime_X.DayOfWeek_U8, 4);

	FirstDateTime_X.Day_U8          = 18;
	FirstDateTime_X.Month_U8        = 1;
	FirstDateTime_X.Year_U16        = 2000;
	FirstDateTime_X.Hour_U8         = 1;
	FirstDateTime_X.Minute_U8       = 30;
	FirstDateTime_X.Second_U8       = 20;
	FirstDateTime_X.Millisecond_U16 = 0;

	SecondDateTime_X                = FirstDateTime_X;
	Sts_E                           = Bof_DiffDateTime(SecondDateTime_X, FirstDateTime_X, DiffTime_X, DiffDay_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(DiffDay_U32, 0);
	EXPECT_EQ(DiffTime_X.Hour_U8, 0);
	EXPECT_EQ(DiffTime_X.Minute_U8, 0);
	EXPECT_EQ(DiffTime_X.Second_U8, 0);

	SecondDateTime_X.Year_U16       = static_cast<uint16_t>(FirstDateTime_X.Year_U16 + 1);
	Sts_E                           = Bof_DiffDateTime(SecondDateTime_X, FirstDateTime_X, DiffTime_X, DiffDay_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(DiffDay_U32, 366);
	EXPECT_EQ(DiffTime_X.Hour_U8, 0);
	EXPECT_EQ(DiffTime_X.Minute_U8, 0);
	EXPECT_EQ(DiffTime_X.Second_U8, 0);

	SecondDateTime_X.Month_U8       = static_cast<uint8_t>(FirstDateTime_X.Month_U8 + 1);
	Sts_E                           = Bof_DiffDateTime( SecondDateTime_X,FirstDateTime_X, DiffTime_X, DiffDay_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(DiffDay_U32, 366 + 31);
	EXPECT_EQ(DiffTime_X.Hour_U8, 0);
	EXPECT_EQ(DiffTime_X.Minute_U8, 0);
	EXPECT_EQ(DiffTime_X.Second_U8, 0);

	SecondDateTime_X.Day_U8         = static_cast<uint8_t>(FirstDateTime_X.Day_U8 + 7);
	Sts_E                           = Bof_DiffDateTime(SecondDateTime_X,FirstDateTime_X,  DiffTime_X, DiffDay_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(DiffDay_U32, 366 + 31 + 7);
	EXPECT_EQ(DiffTime_X.Hour_U8, 0);
	EXPECT_EQ(DiffTime_X.Minute_U8, 0);
	EXPECT_EQ(DiffTime_X.Second_U8, 0);

	FirstDateTime_X.Hour_U8         = 0;
	FirstDateTime_X.Minute_U8       = 0;
	FirstDateTime_X.Second_U8       = 0;
	FirstDateTime_X.Millisecond_U16 = 0;
	SecondDateTime_X                = FirstDateTime_X;
  SecondDateTime_X.Second_U8      = 1;
	Sts_E                           = Bof_DiffDateTime( SecondDateTime_X, FirstDateTime_X, DiffTime_X, DiffDay_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(DiffDay_U32, 0);
	EXPECT_EQ(DiffTime_X.Hour_U8, 0);
	EXPECT_EQ(DiffTime_X.Minute_U8, 0);
	EXPECT_EQ(DiffTime_X.Second_U8, 1);
  SecondDateTime_X.Minute_U8      = 2;
	Sts_E                           = Bof_DiffDateTime(SecondDateTime_X,FirstDateTime_X,  DiffTime_X, DiffDay_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(DiffDay_U32, 0);
	EXPECT_EQ(DiffTime_X.Hour_U8, 0);
	EXPECT_EQ(DiffTime_X.Minute_U8, 2);
	EXPECT_EQ(DiffTime_X.Second_U8, 1);

  SecondDateTime_X.Hour_U8        = 3;
	Sts_E                           = Bof_DiffDateTime(SecondDateTime_X,FirstDateTime_X,  DiffTime_X, DiffDay_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(DiffDay_U32, 0);
	EXPECT_EQ(DiffTime_X.Hour_U8, 3);
	EXPECT_EQ(DiffTime_X.Minute_U8, 2);
	EXPECT_EQ(DiffTime_X.Second_U8, 1);
}
TEST(System_Test, StringDateTime)
{
  BOF_DATE_TIME DateTime_X, DateTimeFromString_X;
  std::string DateTime_S;

  EXPECT_EQ(Bof_DateInDaySince1970_To_BofDateTime(365+31+7, DateTime_X),BOF_ERR_NO_ERROR);
  DateTime_X.Hour_U8 = 12;
  DateTime_X.Minute_U8 = 34;
  DateTime_X.Second_U8 = 56;
  DateTime_X.Millisecond_U16 = 0;

  DateTime_S= Bof_DateTimeToString(DateTime_X);
  DateTimeFromString_X.Reset();
  DateTimeFromString_X = Bof_DateTimeFromString(DateTime_S);
  EXPECT_TRUE(DateTime_X == DateTimeFromString_X);
  EXPECT_STREQ(DateTime_S.c_str(), "1971-02-08 12:34:56");

  DateTime_S = Bof_DateTimeToString(DateTime_X, "%b %d %H:%M");
  DateTimeFromString_X.Reset();
  DateTimeFromString_X = Bof_DateTimeFromString(DateTime_S, "%b %d %H:%M");
  DateTimeFromString_X.Year_U16 = DateTime_X.Year_U16;  //Not in format string
  DateTimeFromString_X.Second_U8 = 56;//Not in format string
  DateTimeFromString_X.DayOfWeek_U8 = 1;//Bad as Year_U16/Second_U8 are not in format string
  EXPECT_TRUE(DateTime_X == DateTimeFromString_X);
  EXPECT_STREQ(DateTime_S.c_str(), "Feb 08 12:34");

}

TEST(System_Test, TickSleep)
{
	BOFERR        Sts_E;
	BOF_DATE_TIME FirstDateTime_X, SecondDateTime_X, DiffTime_X;
	uint32_t      DiffDay_U32, Start_U32, Delta_U32,i_U32;
	const uint32_t NBLOOP = 20;

	Start_U32 = Bof_GetMsTickCount();
	for (i_U32 = 0; i_U32 < NBLOOP;i_U32++)
	{
    Bof_MsSleep(1);
	}
	Delta_U32 = Bof_ElapsedMsTime(Start_U32);
	EXPECT_GE(Delta_U32, i_U32*1);
	EXPECT_LT(Delta_U32, i_U32*1*3);		//Win 1562 Lin 106 New Win with timeBeginPeriod:124
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
	Sts_E     = Bof_Now(FirstDateTime_X);
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
	Sts_E     = Bof_Now(SecondDateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	Sts_E     = Bof_DiffDateTime( SecondDateTime_X,FirstDateTime_X, DiffTime_X, DiffDay_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(DiffDay_U32, 0);
	EXPECT_EQ(DiffTime_X.Hour_U8, 0);
	EXPECT_EQ(DiffTime_X.Minute_U8, 0);
	EXPECT_GE(DiffTime_X.Second_U8, 3);
	EXPECT_LE(DiffTime_X.Second_U8, 4);
	EXPECT_EQ(DiffTime_X.Millisecond_U16, 0);
	EXPECT_GT(Delta_U32, (uint32_t)2900);
	EXPECT_LT(Delta_U32, (uint32_t)3100);

	Start_U32 = Bof_GetMsTickCount();
	Sts_E     = Bof_Now(FirstDateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Bof_MsSleep(100);
	Delta_U32 = Bof_ElapsedMsTime(Start_U32);
	Sts_E     = Bof_Now(SecondDateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	Sts_E     = Bof_DiffDateTime( SecondDateTime_X,FirstDateTime_X, DiffTime_X, DiffDay_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(DiffDay_U32, 0);
	EXPECT_EQ(DiffTime_X.Hour_U8, 0);
	EXPECT_EQ(DiffTime_X.Minute_U8, 0);
	// can be one
	EXPECT_LE(DiffTime_X.Second_U8, 1);
	EXPECT_EQ(DiffTime_X.Millisecond_U16, 0);
	EXPECT_GE(Delta_U32, (uint32_t)90);
	EXPECT_LE(Delta_U32, (uint32_t)110);

	Start_U32 = Bof_GetMsTickCount();
	Sts_E     = Bof_Now(FirstDateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Bof_MsSleep(30);                       // Sleep can't be lower than the timer res (on some system it is 15 ms)
	Delta_U32 = Bof_ElapsedMsTime(Start_U32);
	Sts_E     = Bof_Now(SecondDateTime_X);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	Sts_E     = Bof_DiffDateTime( SecondDateTime_X, FirstDateTime_X,DiffTime_X, DiffDay_U32);
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(DiffDay_U32, 0);
	EXPECT_EQ(DiffTime_X.Hour_U8, 0);
	EXPECT_EQ(DiffTime_X.Minute_U8, 0);
	// can be one
	EXPECT_LE(DiffTime_X.Second_U8, 1);
	EXPECT_EQ(DiffTime_X.Millisecond_U16, 0);
	EXPECT_GE(Delta_U32, (uint32_t)15);
	EXPECT_LE(Delta_U32, (uint32_t)45);

}

TEST(System_Test, EnvVar)
{
	const char *pEnv_c;
	char       pVal_c[128];
	int        Sts_i;

	pEnv_c = Bof_GetEnvVar(nullptr);
	EXPECT_TRUE(pEnv_c == 0);

	Sts_i  = Bof_SetEnvVar(nullptr, pVal_c, 0);
	EXPECT_NE(Sts_i, 0);
	Sts_i  = Bof_SetEnvVar("nullptr", nullptr, 0);
	EXPECT_NE(Sts_i, 0);
	Sts_i  = Bof_SetEnvVar(nullptr, nullptr, 0);
	EXPECT_NE(Sts_i, 0);

	Sts_i  = Bof_SetEnvVar("BHA_TEST_VAR", "1:BHA", 1);
	EXPECT_EQ(Sts_i, 0);
	Sts_i  = Bof_SetEnvVar("BHA_TEST_VAR", "2:BHA", 0);
	EXPECT_NE(Sts_i, 0);
	Sts_i  = Bof_SetEnvVar("BHA_TEST_VAR", "3:BHA", 1);
	EXPECT_EQ(Sts_i, 0);

	pEnv_c = Bof_GetEnvVar("BHA_TEST");
	EXPECT_TRUE(pEnv_c == 0);

	pEnv_c = Bof_GetEnvVar("BHA_TEST_VAR");
	EXPECT_TRUE(pEnv_c != 0);

	EXPECT_STREQ(pEnv_c, "3:BHA");

}



TEST(System_Test, DumpMemoryZone)
{
	std::string Dmp_S;
	uint8_t     pMemoryZone_U8[0x1000];
	std::string Out_S;
	uint32_t    i_U32;
	BOF_DUMP_MEMORY_ZONE_PARAM DumpMemoryZoneParam_X;

	for (i_U32 = 0 ; i_U32 < sizeof(pMemoryZone_U8) ; i_U32++)
	{
		pMemoryZone_U8[i_U32] = static_cast< uint8_t > (i_U32);
	}

	DumpMemoryZoneParam_X.NbItemToDump_U32 = 0;
	DumpMemoryZoneParam_X.pMemoryZone = pMemoryZone_U8;
	DumpMemoryZoneParam_X.NbItemPerLine_U32 = 16;
	DumpMemoryZoneParam_X.Separator_c = ' ';
	DumpMemoryZoneParam_X.ShowHexaPrefix_B = false;
	DumpMemoryZoneParam_X.GenerateVirtualOffset = true;
	DumpMemoryZoneParam_X.VirtualOffset_S64 = 0x12345678;
	DumpMemoryZoneParam_X.GenerateBinaryData_B = true;
	DumpMemoryZoneParam_X.GenerateAsciiData_B = true;
	DumpMemoryZoneParam_X.ReverseEndianness_B = false;
	DumpMemoryZoneParam_X.AccessSize_E = BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_8;
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	EXPECT_STREQ("", Dmp_S.c_str() );

	DumpMemoryZoneParam_X.pMemoryZone = nullptr;
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	EXPECT_STREQ("", Dmp_S.c_str() );

	DumpMemoryZoneParam_X.NbItemToDump_U32 = 1025;
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	EXPECT_STREQ("", Dmp_S.c_str() );

	DumpMemoryZoneParam_X.Separator_c = 0;
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	EXPECT_STREQ("", Dmp_S.c_str() );

	DumpMemoryZoneParam_X.NbItemToDump_U32 = 16;
	DumpMemoryZoneParam_X.pMemoryZone = pMemoryZone_U8;
	DumpMemoryZoneParam_X.Separator_c = ' ';
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	Out_S = Bof_Sprintf("12345678   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F ????????????????%s", Bof_Eol() );
	EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str() );

	DumpMemoryZoneParam_X.NbItemToDump_U32 = 64;
	DumpMemoryZoneParam_X.pMemoryZone = &pMemoryZone_U8[23];
	DumpMemoryZoneParam_X.VirtualOffset_S64 = 0xABCDEF01;
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	Out_S = Bof_Sprintf("ABCDEF01   17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 ????????? !\"#$%%&%sABCDEF11   27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 '()*+,-./0123456%sABCDEF21   37 38 39 3A 3B 3C 3D 3E 3F 40 41 42 43 44 45 46 789:;<=>?@ABCDEF%sABCDEF31   47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 GHIJKLMNOPQRSTUV%s", Bof_Eol(), Bof_Eol(), Bof_Eol(), Bof_Eol() );
	EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str() );

	DumpMemoryZoneParam_X.NbItemToDump_U32 = 18;
	DumpMemoryZoneParam_X.pMemoryZone = pMemoryZone_U8;
	DumpMemoryZoneParam_X.NbItemPerLine_U32 = 10;
	DumpMemoryZoneParam_X.VirtualOffset_S64 = 0x12345678;
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	Out_S = Bof_Sprintf("12345678   00 01 02 03 04 05 06 07 08 09 ??????????%s12345682   0A 0B 0C 0D 0E 0F 10 11       ????????  %s", Bof_Eol(), Bof_Eol() );
	EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str() );

	DumpMemoryZoneParam_X.Separator_c = ',';
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	Out_S = Bof_Sprintf("12345678,,,00,01,02,03,04,05,06,07,08,09,??????????%s12345682,,,0A,0B,0C,0D,0E,0F,10,11,      ????????  %s", Bof_Eol(), Bof_Eol() );
	EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str() );

	DumpMemoryZoneParam_X.ShowHexaPrefix_B = true;
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	Out_S = Bof_Sprintf("0x12345678,,,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,??????????%s0x12345682,,,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,          ????????  %s", Bof_Eol(), Bof_Eol() );
	EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str() );

	DumpMemoryZoneParam_X.ShowHexaPrefix_B = false;
	DumpMemoryZoneParam_X.GenerateVirtualOffset = false;
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	Out_S = Bof_Sprintf("00,01,02,03,04,05,06,07,08,09,??????????%s0A,0B,0C,0D,0E,0F,10,11,      ????????  %s", Bof_Eol(), Bof_Eol() );
	EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str() );

	DumpMemoryZoneParam_X.ShowHexaPrefix_B = true;
	DumpMemoryZoneParam_X.GenerateVirtualOffset = false;
	DumpMemoryZoneParam_X.GenerateAsciiData_B = false;
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	Out_S = Bof_Sprintf("0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,%s0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,          %s", Bof_Eol(), Bof_Eol() );
	EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str() );

	DumpMemoryZoneParam_X.pMemoryZone = &pMemoryZone_U8[64];
	DumpMemoryZoneParam_X.ShowHexaPrefix_B = false;
	DumpMemoryZoneParam_X.GenerateAsciiData_B = true;
	DumpMemoryZoneParam_X.GenerateBinaryData_B = false;
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	Out_S = Bof_Sprintf("@ABCDEFGHI%sJKLMNOPQ  %s", Bof_Eol(), Bof_Eol() );
	EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str() );

	DumpMemoryZoneParam_X.pMemoryZone = pMemoryZone_U8;
	DumpMemoryZoneParam_X.GenerateAsciiData_B = false;
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	EXPECT_STREQ("", Dmp_S.c_str() );

	DumpMemoryZoneParam_X.NbItemToDump_U32 = 10;
	DumpMemoryZoneParam_X.pMemoryZone = pMemoryZone_U8;
	DumpMemoryZoneParam_X.NbItemPerLine_U32 = 8;
	DumpMemoryZoneParam_X.Separator_c = ' ';
	DumpMemoryZoneParam_X.ShowHexaPrefix_B = true;
	DumpMemoryZoneParam_X.GenerateVirtualOffset = true;
	DumpMemoryZoneParam_X.VirtualOffset_S64 = -1;
	DumpMemoryZoneParam_X.GenerateBinaryData_B = true;
	DumpMemoryZoneParam_X.GenerateAsciiData_B = true;
	DumpMemoryZoneParam_X.ReverseEndianness_B = true;
	DumpMemoryZoneParam_X.AccessSize_E = BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_16;
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	Out_S = Bof_Sprintf("0x%016llX   0x0001 0x0203 0x0405 0x0607 0x0809 0x0A0B 0x0C0D 0x0E0F ????????????????%s0x%016llX   0x1011 0x1213                                           ????            %s",reinterpret_cast<uint64_t>(pMemoryZone_U8), Bof_Eol(),reinterpret_cast<uint64_t>(pMemoryZone_U8+16), Bof_Eol());
	EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str());

	DumpMemoryZoneParam_X.NbItemToDump_U32 = 5;
	DumpMemoryZoneParam_X.NbItemPerLine_U32 = 4;
	DumpMemoryZoneParam_X.AccessSize_E = BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_32;
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	Out_S = Bof_Sprintf("0x%016llX   0x00010203 0x04050607 0x08090A0B 0x0C0D0E0F ????????????????%s0x%016llX   0x10111213                                  ????            %s", reinterpret_cast<uint64_t>(pMemoryZone_U8), Bof_Eol(),reinterpret_cast<uint64_t>(pMemoryZone_U8+16), Bof_Eol());
	EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str());

	DumpMemoryZoneParam_X.NbItemToDump_U32 = 2;
	DumpMemoryZoneParam_X.NbItemPerLine_U32 = 2;
	DumpMemoryZoneParam_X.AccessSize_E = BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_64;
	Dmp_S = Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
	Out_S = Bof_Sprintf("0x%016llX   0x0001020304050607 0x08090A0B0C0D0E0F ????????????????%s", reinterpret_cast<uint64_t>(pMemoryZone_U8), Bof_Eol());
	EXPECT_STREQ(Out_S.c_str(), Dmp_S.c_str());
}

TEST(System_Test, Util_Sprintf)
{
	std::string Text_S;
	char        pFormat_c[512], pText_c[1024], pStr_c[32];
	int         i = 1;
	float       j = 3.14f;

	strcpy(pStr_c, "String");

	strcpy(pFormat_c, "Hello World");
	sprintf(pText_c, pFormat_c);
	Text_S = Bof_Sprintf(pFormat_c);
	EXPECT_STREQ(pText_c, Text_S.c_str() );

	strcpy(pFormat_c, "int %d %04d 0x%08X float %f %03.4f str %8s %-8s");
	sprintf(pText_c, pFormat_c, i, i, i, j, j, pStr_c, pStr_c);
	Text_S = Bof_Sprintf(pFormat_c, i, i, i, j, j, pStr_c, pStr_c);
	EXPECT_STREQ(pText_c, Text_S.c_str() );

}

TEST(System_Test, Util_AlignedAlloc)
{
	BOF_BUFFER Buffer_X;
	void *pData;

	EXPECT_EQ(Bof_AlignedMemAlloc(BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM, 32, 128, false, false, Buffer_X), BOF_ERR_NO_ERROR);
	pData = Buffer_X.pData_U8;
	EXPECT_TRUE(pData != nullptr);
	EXPECT_EQ( (reinterpret_cast< std::uintptr_t > (pData) & 0x1F), 0);
	EXPECT_EQ(Bof_AlignedMemFree(Buffer_X), BOF_ERR_NO_ERROR);
	EXPECT_NE(Bof_AlignedMemFree(Buffer_X), BOF_ERR_NO_ERROR);

	EXPECT_NE(Bof_AlignedMemAlloc(BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM, 25, 128, false, false, Buffer_X), BOF_ERR_NO_ERROR);
	pData = Buffer_X.pData_U8;
	EXPECT_TRUE(pData == nullptr);
	EXPECT_NE(Bof_AlignedMemFree(Buffer_X), BOF_ERR_NO_ERROR);

	EXPECT_EQ(Bof_AlignedMemAlloc(BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM, 256, 128, false, true, Buffer_X), BOF_ERR_NO_ERROR);
	pData = Buffer_X.pData_U8;
	EXPECT_TRUE(pData != nullptr);
	EXPECT_EQ( (reinterpret_cast< std::uintptr_t > (pData) & 0xFF), 0);
	EXPECT_EQ(Bof_AlignedMemFree(Buffer_X), BOF_ERR_NO_ERROR);

#if defined(_WIN32)
	//lock not supported
	EXPECT_EQ(Bof_AlignedMemAlloc(BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM, 0x1000, 0x100000, false, false, Buffer_X), BOF_ERR_NO_ERROR);
#else
	EXPECT_EQ(Bof_AlignedMemAlloc(BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM, 0x1000, 0x100000, true, false, Buffer_X), BOF_ERR_NO_ERROR);
#endif
  pData = Buffer_X.pData_U8;
  EXPECT_TRUE(pData != nullptr);
  EXPECT_EQ( (reinterpret_cast< std::uintptr_t > (pData) & 0xFF), 0);
  EXPECT_EQ(Bof_AlignedMemFree(Buffer_X), BOF_ERR_NO_ERROR);
}

TEST(System_Test, NanoTickCount)
{
	uint64_t Start_U64, Delta_U64;

	Start_U64 = Bof_GetNsTickCount();
  Bof_MsSleep(BOF_S_TO_MS(1));
	Delta_U64 = Bof_ElapsedNsTime(Start_U64);
	EXPECT_GE(Delta_U64, BOF_S_TO_NANO(1) );
	EXPECT_LT(Delta_U64, BOF_S_TO_NANO(1) * 1.1);
}

TEST(System_Test, MsTickCount)
{
	uint32_t Start_U32, Delta_U32;

	Start_U32 = Bof_GetMsTickCount();
  Bof_MsSleep(BOF_S_TO_MS(1));
	Delta_U32 = Bof_ElapsedMsTime(Start_U32);
	EXPECT_GE(Delta_U32, BOF_S_TO_MS(1) );
	EXPECT_LT(Delta_U32, BOF_S_TO_MS(1) * 1.1);
}

TEST(System_Test, Eol)
{
	const char *pEol_c        = Bof_Eol();

#if defined (_WIN32)
	EXPECT_STREQ(pEol_c, "\r\n");
#else
	EXPECT_STREQ(pEol_c, "\n");
#endif
}

const uint32_t NBRAMDOMLOOP = 14000;
TEST(System_Test, Random)
{
	uint32_t    i_U32, pTime_U32[100], MeanLow_U32, MeanHigh_U32;
	int32_t     Val_S32, MinValue_S32, MaxValue_S32;

	std::string Str_S, Prv_S;

	memset(pTime_U32, 0, sizeof(pTime_U32) );
	MinValue_S32 = 3;
	MaxValue_S32 = 12;
	Bof_Random(true, MinValue_S32, MaxValue_S32);
	for (i_U32 = 0 ; i_U32 < NBRAMDOMLOOP ; i_U32++)
	{
		Val_S32 = Bof_Random(false, MinValue_S32, MaxValue_S32);
		EXPECT_GE(Val_S32, MinValue_S32);
		EXPECT_LE(Val_S32, MaxValue_S32);
		pTime_U32[Val_S32]++;
// printf("%d: %d\r\n", i_U32, Val_U32);
	}
	MeanLow_U32  = static_cast< uint32_t > ( static_cast<float>(NBRAMDOMLOOP / (MaxValue_S32 - MinValue_S32 + 1) ) * 0.80f);
	MeanHigh_U32 = static_cast< uint32_t > ( static_cast<float>(NBRAMDOMLOOP / (MaxValue_S32 - MinValue_S32 + 1) ) * 1.20f);
	for (i_U32 = 0 ; i_U32 < static_cast< uint32_t > (MinValue_S32) ; i_U32++)
	{
		EXPECT_EQ(pTime_U32[i_U32], static_cast< uint32_t > (0) );
	}

	for (i_U32 = MinValue_S32 ; i_U32 <= static_cast< uint32_t > (MaxValue_S32) ; i_U32++)
	{
		EXPECT_NE(pTime_U32[i_U32], static_cast< uint32_t > (0) );
		EXPECT_GE(pTime_U32[i_U32], MeanLow_U32);
		EXPECT_LE(pTime_U32[i_U32], MeanHigh_U32);

		// printf("%d is present %d times\r\n", i_U32, pTime_U32[i_U32]);
	}
	for (i_U32 = MaxValue_S32 + 1 ; i_U32 < BOF_NB_ELEM_IN_ARRAY(pTime_U32) ; i_U32++)
	{
		EXPECT_EQ(pTime_U32[i_U32], static_cast< uint32_t > (0) );
	}

	memset(pTime_U32, 0, sizeof(pTime_U32) );
	MinValue_S32 = -89;
	MaxValue_S32 = -20;
	Bof_Random(true, MinValue_S32, MaxValue_S32);
	for (i_U32 = 0 ; i_U32 < NBRAMDOMLOOP ; i_U32++)
	{
		Val_S32 = Bof_Random(false, MinValue_S32, MaxValue_S32);
		EXPECT_GE(Val_S32, MinValue_S32);
		EXPECT_LE(Val_S32, MaxValue_S32);
		pTime_U32[-Val_S32]++;
		// printf("%d: %d\r\n", i_U32, Val_U32);
	}
	MeanLow_U32  = static_cast< uint32_t > ( static_cast<float>(NBRAMDOMLOOP / (MaxValue_S32 - MinValue_S32 + 1) ) * 0.75f);
	MeanHigh_U32 = static_cast< uint32_t > ( static_cast<float>(NBRAMDOMLOOP / (MaxValue_S32 - MinValue_S32 + 1) ) * 1.25f);
	for (i_U32 = 0 ; i_U32 < static_cast< uint32_t > (-MaxValue_S32) ; i_U32++)
	{
		EXPECT_EQ(pTime_U32[i_U32], static_cast< uint32_t > (0) );
	}

	for (i_U32 = -MaxValue_S32 ; i_U32 <= static_cast< uint32_t > (-MinValue_S32) ; i_U32++)
	{
		EXPECT_NE(pTime_U32[i_U32], static_cast< uint32_t > (0) );
		EXPECT_GE(pTime_U32[i_U32], MeanLow_U32);
		EXPECT_LE(pTime_U32[i_U32], MeanHigh_U32);

		// printf("%d is present %d times\r\n", i_U32, pTime_U32[i_U32]);
	}
	for (i_U32 = -MinValue_S32 + 1 ; i_U32 < BOF_NB_ELEM_IN_ARRAY(pTime_U32) ; i_U32++)
	{
		EXPECT_EQ(pTime_U32[i_U32], static_cast< uint32_t > (0) );
	}



	memset(pTime_U32, 0, sizeof(pTime_U32) );
	MinValue_S32 = -19;
	MaxValue_S32 = 8;
	Bof_Random(true, MinValue_S32, MaxValue_S32);
	for (i_U32 = 0 ; i_U32 < NBRAMDOMLOOP ; i_U32++)
	{
		Val_S32 = Bof_Random(false, MinValue_S32, MaxValue_S32);
		EXPECT_GE(Val_S32, MinValue_S32);
		EXPECT_LE(Val_S32, MaxValue_S32);
		pTime_U32[50 + Val_S32]++;
		// printf("%d: %d\r\n", i_U32, Val_U32);
	}
	MeanLow_U32  = static_cast< uint32_t > ( static_cast<float>(NBRAMDOMLOOP / (MaxValue_S32 - MinValue_S32 + 1) ) * 0.80f);
	MeanHigh_U32 = static_cast< uint32_t > ( static_cast<float>(NBRAMDOMLOOP / (MaxValue_S32 - MinValue_S32 + 1) ) * 1.20f);
	for (i_U32 = 0 ; i_U32 < static_cast< uint32_t > (50 + MinValue_S32) ; i_U32++)
	{
		EXPECT_EQ(pTime_U32[i_U32], static_cast< uint32_t > (0) );
	}

	for (i_U32 = 50 + MinValue_S32 ; i_U32 <= static_cast< uint32_t > (50 + MaxValue_S32) ; i_U32++)
	{
		EXPECT_NE(pTime_U32[i_U32], static_cast< uint32_t > (0) );
		EXPECT_GE(pTime_U32[i_U32], MeanLow_U32);
		EXPECT_LE(pTime_U32[i_U32], MeanHigh_U32);

		// printf("%d is present %d times\r\n", i_U32, pTime_U32[i_U32]);
	}
	for (i_U32 = 50 + MaxValue_S32 + 1 ; i_U32 < BOF_NB_ELEM_IN_ARRAY(pTime_U32) ; i_U32++)
	{
		EXPECT_EQ(pTime_U32[i_U32], static_cast< uint32_t > (0) );
	}

	Prv_S        = "";
	for (i_U32 = 0 ; i_U32 < 1000 ; i_U32++)
	{
		Str_S = Bof_Random(true, 40, 'A', 'Z');
		EXPECT_STRNE(Prv_S.c_str(), Str_S.c_str() );
		Prv_S = Str_S;
// printf("%d: %s\r\n", i_U32, Str_S.c_str());
	}
}

TEST(System_Test, RandomString)
{
	uint32_t    i_U32, j_U32, Size_U32;
	char        MinValue_c, MaxValue_c;
	std::string Val_S;

	MinValue_c = 'A';
	MaxValue_c = 'z';
	Size_U32   = 256;
	Bof_Random(true, MinValue_c, MaxValue_c);
	for (i_U32 = 0 ; i_U32 < 1000 ; i_U32++)
	{
		Val_S = Bof_Random(false, Size_U32, MinValue_c, MaxValue_c);
		EXPECT_EQ(Size_U32, Val_S.size() );
		for (j_U32 = 0 ; j_U32 < Size_U32 ; j_U32++)
		{
			EXPECT_GE(Val_S[j_U32], MinValue_c);
			EXPECT_LE(Val_S[j_U32], MaxValue_c);
		}
	}
}

TEST(System_Test, Exec)
{
	BOFERR      Sts_E;
	std::string Cmd_S, Output_S;
	int32_t     ExitCode_S32;

#if defined (_WIN32)
	Cmd_S    = Bof_Sprintf("dir");
#else
	Cmd_S    = Bof_Sprintf("ls");
#endif
	Sts_E    = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);
//	std::cerr << "[          ] Exit code " << ExitCode_S32 << " Output:" << std::endl << Output_S << std::endl;
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(ExitCode_S32, 0);
	EXPECT_TRUE(Output_S != "");

	Output_S = "";
	Sts_E    = Bof_Exec(Cmd_S, nullptr, ExitCode_S32);
//	std::cerr << "[          ] Exit code " << ExitCode_S32 << " Output:" << std::endl << Output_S << std::endl;
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
	EXPECT_EQ(ExitCode_S32, 0);
	EXPECT_TRUE(Output_S == "");
}