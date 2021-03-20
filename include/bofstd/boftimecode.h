/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * Author:      Bernard HARMEL: b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 19 2017  BHA : Initial release
 */
#pragma once

#include <bofstd/bofstd.h>
#include <bofstd/bofsystem.h>

BEGIN_BOF_NAMESPACE()

#define BOF_TIMECODE_FLAG_PARITY_ODD  0x01 //Bit 0 : Timecode parity   0 : Even    1 : Odd
#define BOF_TIMECODE_FLAG_COLOR_FRAME  0x02  ///<Timecode flag: color frame bitmask
#define BOF_TIMECODE_FLAG_DROP  0x40        //Bit 6 : Drop type 	      0 : NoDrop  1 : Drop
#define BOF_TIMECODE_FLAG_NTSC  0x80    //Bit 7 : Video standard    0 : Pal 1 : Ntsc
#pragma pack(1)

//8+4 bytes
struct BOF_TIMECODE
{
		/**
		* Returns a date information as the number of day elapsed since 1/1/1970.
		*/
		uint16_t NbDay_U16;
		/**
		* Returns the timecode hour value.
		*/
		uint8_t Hour_U8;
		/**
		* Returns the timecode minute value.
		*/
		uint8_t Minute_U8;
		/**
		* Returns the timecode second value.
		*/
		uint8_t Second_U8;
		/**
		* Returns the timecode frame value.
		*/
		uint8_t Frame_U8;
		/**
		* Returns the timecode flag value.
		Bit 0: Timecode parity 0:Even 1:Odd
		Bit 6: Drop type 	0: NoDrop 1:Drop
		Bit 7: Video standard 0:Pal 1:Ntsc
		*/
		uint8_t TcFlag_U8;
		/**
		* Returns the timecode spare value.
		*/
		uint8_t Spare_U8;
		uint8_t pUserBit_U8[4];    ///< The timecode user bits

		BOF_TIMECODE()
		{
			Reset();
		}

		void Reset()
		{
			NbDay_U16 = 0;
			Hour_U8 = 0;
			Minute_U8 = 0;
			Second_U8 = 0;
			Frame_U8 = 0;
			TcFlag_U8 = 0;
			Spare_U8 = 0;
			pUserBit_U8[0]=0;
			pUserBit_U8[1]=0;
			pUserBit_U8[2]=0;
			pUserBit_U8[3]=0;
		}
};

#pragma pack()

class BofTimecode
{
private:
		BOF_TIMECODE mTc_X;
	  bool mTcValid_B=false;

public:
		BofTimecode() = default;    //Default constructor
		BofTimecode(const BofTimecode &) = default; //Copy constructor
		BofTimecode(BofTimecode &&) = default; //Move constructor
		BofTimecode &operator=(const BofTimecode &) = default;  //Copy assignment operator
		BofTimecode &operator=(BofTimecode &&) = default; //Move assignment operator
		virtual ~BofTimecode() = default; //Default destructor

		BofTimecode(bool _Ntsc_B, uint64_t _Ms_U64);
		BofTimecode(const BOF_TIMECODE &_rBofTimeCodeStruct_X);
		BofTimecode(bool _Ntsc_B, const BOF_DATE_TIME &_rDateTime_X);
		bool IsNextTimecodeDrop() const;
  	bool IsTimecodeValid() const;
	  bool IsNtsc() const;
		bool IsOddField() const;
		double FrameTime() const;
		double FieldTime() const;
		uint64_t ToMs() const;
		void FromMs(bool _Ntsc_B, uint64_t _Ms_U64);
		std::string ToString(bool _ShowDate_B, const std::string &_rFormatDate_S, bool _ShowTime_B, const std::string &_rFormatTime_S, bool _ShowStandard_B);
		std::string ToString(bool _ShowDate_B);  //ISO-8601
		bool operator==(const BofTimecode &_Rhs) const;
		bool operator!=(const BofTimecode &_Rhs) const;
		bool operator<(const BofTimecode &_Rhs) const;
		bool operator>(const BofTimecode &_Rhs) const;
		bool operator<=(const BofTimecode &_Rhs) const;
		bool operator>=(const BofTimecode &_Rhs) const;
		int64_t operator-(const BofTimecode &_Rhs) const;
		BofTimecode operator+(int32_t _NbField_S32);
		BOF_TIMECODE ToByteStruct() const;
		BOFERR FromByteStruct(const BOF_TIMECODE &_rBofTimeCodeStruct_X);

	static BOFERR S_ValidateTimecode(const BOF_TIMECODE &_rBofTimeCodeStruct_X);
	static BOFERR S_BinToTimeCode(uint64_t _Tc_U64, BOF_TIMECODE &_rTc_X);
	static BOFERR S_TimeCodeToBin(const BOF_TIMECODE &_rTc_X, uint64_t &_rTc_U64);
	static BOFERR S_IncBinTimeCode(uint64_t &_rTc_U64, int32_t _NbField_S32);
	static BOFERR S_IncTimeCode(BOF_TIMECODE &_rTc_X, int32_t _NbField_S32);
};

END_BOF_NAMESPACE()