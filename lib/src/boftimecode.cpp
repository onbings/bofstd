/*
 * Copyright (c) 2000-2020, Onbings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the boftimecode interface.
 *
 * Name:        boftimecode.h
 * Author:      Bernard HARMEL: b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 19 2017  BHA : Initial release
 */
#include <bofstd/boftimecode.h>
#include <bofstd/bofstring.h>

BEGIN_BOF_NAMESPACE()
BofTimecode::BofTimecode()
{
  mRate = BofRational(1000, 25, false);
  mTcValid_B = true;
}

BofTimecode::BofTimecode(bool _Ntsc_B, uint64_t _Ms_U64)
{
  FromMs(_Ntsc_B, _Ms_U64);
  mTcValid_B = true;
}

BofTimecode::BofTimecode(const BOF_TIMECODE &_rBofTimeCodeStruct_X)
{
  mTcValid_B = (FromByteStruct(_rBofTimeCodeStruct_X) == BOF_ERR_NO_ERROR);
}

BofTimecode::BofTimecode(bool _Ntsc_B, const BofDateTime &_rDateTime)
{
  time_t DateInDaySince1970;
  double Remain_lf;
  uint32_t NbDaySinceUnixEpoch_U32;

  BOFERR Sts_E = Bof_BofDateTime_To_NbDaySinceUnixEpoch(_rDateTime, NbDaySinceUnixEpoch_U32);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    mTcValid_B = true;
    mTc_X.NbDay_U16 = static_cast<uint16_t>(NbDaySinceUnixEpoch_U32);
    mTc_X.Hour_U8 = _rDateTime.Hour();
    mTc_X.Minute_U8 = _rDateTime.Minute();
    mTc_X.Second_U8 = _rDateTime.Second();
    mTc_X.Frame_U8 = static_cast<uint8_t>(static_cast<double>(_rDateTime.MicroSecond() / 1000) / FrameTime());
    mTc_X.TcFlag_U8 = _Ntsc_B ? BOF_TIMECODE_FLAG_NTSC : 0x00;
    mRate = (mTc_X.TcFlag_U8 & BOF_TIMECODE_FLAG_NTSC) ? BofRational(1000, 30, false) : BofRational(1000, 25, false);
    Remain_lf = static_cast<double>(_rDateTime.MicroSecond() / 1000) - (static_cast<uint32_t>(mTc_X.Frame_U8) * FrameTime());
    if (Remain_lf > (0.9f * FieldTime()))
    {
      mTc_X.TcFlag_U8 |= BOF_TIMECODE_FLAG_PARITY_ODD;
    }
  }
}
BofTimecode::BofTimecode(const char *_pTc_c)
{
  // 01234567890123456789012345678901
  //"2018-05-26 08:16:32;01. @1000/25"
  int NbScanField_i, Num_i, Den_i, Year_i, Month_i, Day_i, Hour_i, Minute_i, Second_i, Frame_i;
  BOF_TIMECODE BofTimeCodeStruct_X;
  BofDateTime DateTime;
  uint32_t NbDaySinceUnixEpoch_U32;
  char Drop_c, Parity_c;

  mTcValid_B = false;
  if (_pTc_c)
  {
    if (strlen(_pTc_c) >= 28)
    {
      NbScanField_i = sscanf(_pTc_c, "%04d-%02d-%02d %02d:%02d:%02d%c%02d%c @%d/%d", &Year_i, &Month_i, &Day_i, &Hour_i, &Minute_i, &Second_i, &Drop_c, &Frame_i, &Parity_c, &Num_i, &Den_i);
      if (NbScanField_i == 11)
      {
        DateTime = BofDateTime(static_cast<uint8_t>(Day_i), static_cast<uint8_t>(Month_i), static_cast<uint16_t>(Year_i), 0, 0, 0, 0);
        if (Bof_BofDateTime_To_NbDaySinceUnixEpoch(DateTime, NbDaySinceUnixEpoch_U32) == BOF_ERR_NO_ERROR)
        {
          BofTimeCodeStruct_X.Hour_U8 = static_cast<uint8_t>(Hour_i);
          BofTimeCodeStruct_X.Minute_U8 = static_cast<uint8_t>(Minute_i);
          BofTimeCodeStruct_X.Second_U8 = static_cast<uint8_t>(Second_i);
          BofTimeCodeStruct_X.Frame_U8 = static_cast<uint8_t>(Frame_i);

          BofTimeCodeStruct_X.TcFlag_U8 = 0;
          BofTimeCodeStruct_X.TcFlag_U8 |= static_cast<uint8_t>((Parity_c == '.') ? BOF_TIMECODE_FLAG_PARITY_ODD : 0);
          BofTimeCodeStruct_X.TcFlag_U8 |= static_cast<uint8_t>((Drop_c == ';') ? BOF_TIMECODE_FLAG_DROP : 0);
          BofTimeCodeStruct_X.TcFlag_U8 |= static_cast<uint8_t>(((Num_i == 1000) && (Den_i == 30)) ? BOF_TIMECODE_FLAG_NTSC : 0);
          BofTimeCodeStruct_X.NbDay_U16 = static_cast<uint16_t>(NbDaySinceUnixEpoch_U32);
          mTcValid_B = (FromByteStruct(BofTimeCodeStruct_X) == BOF_ERR_NO_ERROR);
          mRate = BofRational(Num_i, Den_i, false);	//Modified in FromByteStruct
        }
      }
    }
  }
}
/*
 DROP FRAME TIMECODE

Drop frame timecode is a format that allows the counter to more accurately indicate real time duration. It�s able to do this by dropping frames.
The specific pattern drops the first two frames of every minute except every tenth minute.

01:00:59;29
01:01:00;02
...
01:09:59;29
01:10:00;00
 */
bool BofTimecode::IsNextTimecodeDrop() const
{
  bool Rts_B = false;
  uint8_t NextMin_U8;

  if ((mTc_X.TcFlag_U8 & BOF_TIMECODE_FLAG_NTSC) && (mTc_X.TcFlag_U8 & BOF_TIMECODE_FLAG_DROP))
  {
    if ((mTc_X.Second_U8 == 59) && (mTc_X.Frame_U8 == 29))
    {
      NextMin_U8 = static_cast<uint8_t>(mTc_X.Minute_U8 + 1);
      Rts_B = ((NextMin_U8 % 10) != 0);
    }
  }
  return Rts_B;
}

bool BofTimecode::IsTimecodeValid() const
{
  return mTcValid_B;
}
bool BofTimecode::IsNtsc() const
{
  return (mTc_X.TcFlag_U8 & BOF_TIMECODE_FLAG_NTSC) ? true : false;
}

bool BofTimecode::IsOddField() const
{
  return (mTc_X.TcFlag_U8 & BOF_TIMECODE_FLAG_PARITY_ODD) ? true : false;
}

double BofTimecode::FrameTime() const
{
  return IsNtsc() ? (1000.0f / 30.0f) : (1000.0f / 25.0f);
}

double BofTimecode::FieldTime() const
{
  return FrameTime() / 2;
}

uint64_t BofTimecode::ToMs() const
{
  uint64_t Rts_U64;
  Rts_U64 = (static_cast<uint64_t>(mTc_X.NbDay_U16) * static_cast<uint64_t>(24 * 60 * 60 * 1000));
  Rts_U64 += (static_cast<uint64_t>(mTc_X.Hour_U8) * static_cast<uint64_t>(60 * 60 * 1000));
  Rts_U64 += (static_cast<uint64_t>(mTc_X.Minute_U8) * static_cast<uint64_t>(60 * 1000));
  Rts_U64 += (static_cast<uint64_t>(mTc_X.Second_U8) * static_cast<uint64_t>(1000));
  Rts_U64 += (static_cast<uint64_t>(static_cast<double>(mTc_X.Frame_U8) * FrameTime()));
  Rts_U64 += (IsOddField() ? static_cast<uint64_t>(FieldTime()) : 0);
  return Rts_U64;
}

void BofTimecode::FromMs(bool _Ntsc_B, uint64_t _Ms_U64)
{
  double Remain_lf;
  mTc_X.TcFlag_U8 = _Ntsc_B ? BOF_TIMECODE_FLAG_NTSC : 0x00;
  mRate = _Ntsc_B ? BofRational(1000, 30, false) : BofRational(1000, 25, false);

  mTc_X.NbDay_U16 = static_cast<uint16_t>(_Ms_U64 / static_cast<uint64_t>(24 * 60 * 60 * 1000));
  _Ms_U64 = _Ms_U64 - (static_cast<uint64_t>(mTc_X.NbDay_U16) * static_cast<uint64_t>(24 * 60 * 60 * 1000));

  mTc_X.Hour_U8 = static_cast<uint8_t>(_Ms_U64 / static_cast<uint64_t>(60 * 60 * 1000));
  _Ms_U64 = _Ms_U64 - (static_cast<uint64_t>(mTc_X.Hour_U8) * static_cast<uint64_t>(60 * 60 * 1000));

  mTc_X.Minute_U8 = static_cast<uint8_t>(_Ms_U64 / static_cast<uint64_t>(60 * 1000));
  _Ms_U64 = _Ms_U64 - (static_cast<uint64_t>(mTc_X.Minute_U8) * static_cast<uint64_t>(60 * 1000));

  mTc_X.Second_U8 = static_cast<uint8_t>(_Ms_U64 / static_cast<uint64_t>(1000));
  _Ms_U64 = _Ms_U64 - (static_cast<uint64_t>(mTc_X.Second_U8) * static_cast<uint64_t>(1000));

  mTc_X.Frame_U8 = static_cast<uint8_t>(static_cast<double>(_Ms_U64) / (FrameTime()));
  Remain_lf = static_cast<double>(_Ms_U64) - (static_cast<uint64_t>(mTc_X.Frame_U8) * FrameTime());
  if (Remain_lf > (0.9f * FieldTime()))
  {
    mTc_X.TcFlag_U8 |= BOF_TIMECODE_FLAG_PARITY_ODD;
  }
}

std::string BofTimecode::ToString(bool _ShowDate_B, const std::string &_rFormatDate_S, bool _ShowTime_B, const std::string &_rFormatTime_S, bool _ShowStandard_B)
{
  BofDateTime DateTime;
  std::string Rts_S, Frame_S;

  if (_ShowDate_B)
  {
    Bof_NbDaySinceUnixEpoch_To_BofDateTime(mTc_X.NbDay_U16, DateTime);
    Rts_S += DateTime.ToString((_rFormatDate_S == "") ? "%Y-%m-%d" : _rFormatDate_S);
  }
  if (_ShowTime_B)
  {
    if (Rts_S != "")
    {
      Rts_S += " ";
    }
    DateTime = BofDateTime(1, 1, 1970, mTc_X.Hour_U8, mTc_X.Minute_U8, mTc_X.Second_U8, 0);
    //"2018-05-26 08:16:32;01. @1000/25"
    if (_rFormatTime_S == "")
    {
      Rts_S += DateTime.ToString((mTc_X.TcFlag_U8 & BOF_TIMECODE_FLAG_DROP) ? "%H:%M:%S;" : "%H:%M:%S:");
      Frame_S = Bof_Sprintf("%02d%c", mTc_X.Frame_U8, (mTc_X.TcFlag_U8 & BOF_TIMECODE_FLAG_PARITY_ODD) ? '.' : ' ');
    }
    else
    {
      Rts_S += DateTime.ToString(_rFormatTime_S);
      Frame_S = Bof_Sprintf(":%02d%c", mTc_X.Frame_U8, (mTc_X.TcFlag_U8 & BOF_TIMECODE_FLAG_PARITY_ODD) ? '.' : ' ');
    }
    Rts_S += Frame_S;
  }
  if (_ShowStandard_B)
  {
    if (Rts_S != "")
    {
      Rts_S += " ";
    }
    //		Rts_S += IsNtsc() ? "@1000/30" : "@1000/25";
    Rts_S += "@" + std::to_string(mRate.Num()) + "/" + std::to_string(mRate.Den());
  }
  return Rts_S;
}

//ISO-8601: no
std::string BofTimecode::ToString(bool _ShowDate_B)
{
  std::string Rts_S;

  if (_ShowDate_B)
  {
    Rts_S = ToString(true, "", false, "", false);
  }
  Rts_S += " ";
  Rts_S += ToString(false, "", true, "", true);
  //	Rts_S += IsNtsc() ? "@60" : "@50";

  return Rts_S;

  /*
      std::string Format(const TimeCode &timeCode) const
      override{
        std::stringstream sstr;
        if (timeCode.Rate().Numerator() != 60 && timeCode.Rate().Numerator() != 50) {
          sstr << timeCode.Hour() << ":"
               << (timeCode.Minute() < 10 ? "0" : "") << timeCode.Minute() << ":"
               << (timeCode.Second() < 10 ? "0" : "") << timeCode.Second() << (timeCode.IsDrop() ? ";" : ":")
               << (timeCode.Sample() < 10 ? "0" : "") << timeCode.Sample();
        }
        else {
          sstr << timeCode.Hour() << ":"
               << (timeCode.Minute() < 10 ? "0" : "") << timeCode.Minute() << ":"
               << (timeCode.Second() < 10 ? "0" : "") << timeCode.Second() << (timeCode.IsDrop() ? ";" : ":")
               << (timeCode.Sample() < 10 ? "0" : "") << timeCode.Sample() / 2 << (timeCode.Sample() % 2 == 0 ? " " : ".");
        }
        return sstr.str();
      };
  */

}

bool BofTimecode::operator==(const BofTimecode &_Rhs) const
{
  BOF_TIMECODE RhsTc_X = _Rhs.ToByteStruct();
  return ((mTc_X.NbDay_U16 == RhsTc_X.NbDay_U16)
          && (mTc_X.Hour_U8 == RhsTc_X.Hour_U8)
          && (mTc_X.Minute_U8 == RhsTc_X.Minute_U8)
          && (mTc_X.Second_U8 == RhsTc_X.Second_U8)
          && (mTc_X.Frame_U8 == RhsTc_X.Frame_U8)
          && (mTc_X.TcFlag_U8 == RhsTc_X.TcFlag_U8));
}

bool BofTimecode::operator!=(const BofTimecode &_Rhs) const
{
  return !(*this == _Rhs);
}

bool BofTimecode::operator<(const BofTimecode &_Rhs) const
{
  uint64_t LhsInMs_U64, RhsInMs_U64;
  BOF_ASSERT(IsNtsc() == _Rhs.IsNtsc());
  LhsInMs_U64 = ToMs();
  RhsInMs_U64 = _Rhs.ToMs();
  return (LhsInMs_U64 < RhsInMs_U64);
}

bool BofTimecode::operator>(const BofTimecode &_Rhs) const
{
  return (_Rhs < *this);
}

bool BofTimecode::operator<=(const BofTimecode &_Rhs) const
{
  return !(*this > _Rhs);
}

bool BofTimecode::operator>=(const BofTimecode &_Rhs) const
{
  return !(*this < _Rhs);
}

int64_t BofTimecode::operator-(const BofTimecode &_Rhs) const
{
  uint64_t LhsInMs_U64, RhsInMs_U64;
  BOF_ASSERT(IsNtsc() == _Rhs.IsNtsc());
  LhsInMs_U64 = ToMs();
  RhsInMs_U64 = _Rhs.ToMs();
  return static_cast<int64_t>((static_cast<double>(LhsInMs_U64 - RhsInMs_U64) / FieldTime()) + 0.1f);
}

BofTimecode BofTimecode::operator+(int32_t _NbField_S32)
{
  uint64_t ThisInMs_U64 = ToMs();
  ThisInMs_U64 += static_cast<uint64_t>(FieldTime() * static_cast<double>(_NbField_S32));
  FromMs(IsNtsc(), ThisInMs_U64);
  return *this;
}

BOF_TIMECODE BofTimecode::ToByteStruct() const
{
  return mTc_X;
}

BOFERR BofTimecode::FromByteStruct(const BOF_TIMECODE &_rBofTimeCodeStruct_X)
{
  BOFERR Rts_E;

  Rts_E = S_ValidateTimecode(_rBofTimeCodeStruct_X);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    mTc_X = _rBofTimeCodeStruct_X;
    mRate = (mTc_X.TcFlag_U8 & BOF_TIMECODE_FLAG_NTSC) ? BofRational(1000, 30, false) : BofRational(1000, 25, false);
  }
  return Rts_E;
}

BOFERR BofTimecode::S_ValidateTimecode(const BOF_TIMECODE &_rBofTimeCodeStruct_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BofDateTime DateTime(1, 1, 1970, _rBofTimeCodeStruct_X.Hour_U8, _rBofTimeCodeStruct_X.Minute_U8, _rBofTimeCodeStruct_X.Second_U8, 0);

  if (DateTime.IsValid())
  {
    Rts_E = BOF_ERR_TOO_BIG;
    if (_rBofTimeCodeStruct_X.TcFlag_U8 & BOF_TIMECODE_FLAG_NTSC)
    {
      if (_rBofTimeCodeStruct_X.Frame_U8 < 30)
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
    else
    {
      if ((_rBofTimeCodeStruct_X.Frame_U8 < 25) && !(_rBofTimeCodeStruct_X.TcFlag_U8 & BOF_TIMECODE_FLAG_DROP))
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
  }
  return Rts_E;
}
//https://en.wikipedia.org/wiki/Linear_timecode
BOFERR BofTimecode::S_BinToTimeCode(uint64_t _Tc_U64, BOF_TIMECODE &_rTc_X)
{
  BOFERR Rts_E;
  uint32_t LowValue_U32, HighValue_U32;
  uint8_t ValLow_U8, ValHigh_U8;

  HighValue_U32 = static_cast<uint32_t>(_Tc_U64 >> 32);
  LowValue_U32 = static_cast<uint32_t>(_Tc_U64);
  ValLow_U8 = static_cast<uint8_t>(LowValue_U32 & 0x0000000F);
  ValHigh_U8 = static_cast<uint8_t>((LowValue_U32 & 0x00000300) >> 8);
  _rTc_X.Frame_U8 = static_cast<uint8_t>((10 * ValHigh_U8) + ValLow_U8);

  ValLow_U8 = static_cast<uint8_t>((LowValue_U32 & 0x000F0000) >> 16);
  ValHigh_U8 = static_cast<uint8_t>((LowValue_U32 & 0x07000000) >> 24);
  _rTc_X.Second_U8 = static_cast<uint8_t>((10 * ValHigh_U8) + ValLow_U8);

  ValLow_U8 = static_cast<uint8_t>(HighValue_U32 & 0x0000000F);
  ValHigh_U8 = static_cast<uint8_t>((HighValue_U32 & 0x00000700) >> 8);
  _rTc_X.Minute_U8 = static_cast<uint8_t>((10 * ValHigh_U8) + ValLow_U8);

  ValLow_U8 = static_cast<uint8_t>((HighValue_U32 & 0x000F0000) >> 16);
  ValHigh_U8 = static_cast<uint8_t>((HighValue_U32 & 0x03000000) >> 24);
  _rTc_X.Hour_U8 = static_cast<uint8_t>((10 * ValHigh_U8) + ValLow_U8);

  ValLow_U8 = static_cast<uint8_t>((LowValue_U32 & 0x000000F0) >> 4);
  ValHigh_U8 = static_cast<uint8_t>((LowValue_U32 & 0x0000F000) >> 8);
  _rTc_X.pUserBit_U8[0] = ValHigh_U8 | ValLow_U8;

  ValLow_U8 = static_cast<uint8_t>((LowValue_U32 & 0x00F00000) >> 20);
  ValHigh_U8 = static_cast<uint8_t>((LowValue_U32 & 0xF0000000) >> 24);
  _rTc_X.pUserBit_U8[1] = ValHigh_U8 | ValLow_U8;

  ValLow_U8 = static_cast<uint8_t>((HighValue_U32 & 0x000000F0) >> 4);
  ValHigh_U8 = static_cast<uint8_t>((HighValue_U32 & 0x0000F000) >> 8);
  _rTc_X.pUserBit_U8[2] = ValHigh_U8 | ValLow_U8;

  ValLow_U8 = static_cast<uint8_t>((HighValue_U32 & 0x00F00000) >> 20);
  ValHigh_U8 = static_cast<uint8_t>((HighValue_U32 & 0xF0000000) >> 24);
  _rTc_X.pUserBit_U8[3] = ValHigh_U8 | ValLow_U8;

  _rTc_X.TcFlag_U8 = 0;
  if (LowValue_U32 & 0x00000400)
  {
    _rTc_X.TcFlag_U8 |= BOF_TIMECODE_FLAG_DROP;
  }
  if (LowValue_U32 & 0x00000800)
  {
    _rTc_X.TcFlag_U8 |= BOF_TIMECODE_FLAG_COLOR_FRAME;
  }
  if (LowValue_U32 & 0x08000000)
  {
    _rTc_X.TcFlag_U8 |= BOF_TIMECODE_FLAG_PARITY_ODD;
  }
  Rts_E = S_ValidateTimecode(_rTc_X);
  return Rts_E;
}
//https://en.wikipedia.org/wiki/Linear_timecode
BOFERR BofTimecode::S_TimeCodeToBin(const BOF_TIMECODE &_rTc_X, uint64_t &_rTc_U64)
{
  BOFERR Rts_E;
  uint8_t ValLow_U8, ValHigh_U8;

  Rts_E = S_ValidateTimecode(_rTc_X);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    ValHigh_U8 = _rTc_X.Frame_U8 / 10;
    ValLow_U8 = static_cast<uint8_t>(_rTc_X.Frame_U8 - (ValHigh_U8 * 10));
    _rTc_U64 = static_cast<uint32_t>(ValLow_U8);
    _rTc_U64 |= static_cast<uint32_t>(ValHigh_U8) << 8;

    ValHigh_U8 = _rTc_X.Second_U8 / 10;
    ValLow_U8 = static_cast<uint8_t>(_rTc_X.Second_U8 - (ValHigh_U8 * 10));
    _rTc_U64 |= static_cast<uint32_t>(ValLow_U8) << 16;
    _rTc_U64 |= static_cast<uint32_t>(ValHigh_U8) << 24;

    ValHigh_U8 = _rTc_X.Minute_U8 / 10;
    ValLow_U8 = static_cast<uint8_t>(_rTc_X.Minute_U8 - (ValHigh_U8 * 10));
    _rTc_U64 |= static_cast<uint64_t>(ValLow_U8) << 32;
    _rTc_U64 |= static_cast<uint64_t>(ValHigh_U8) << 40;

    ValHigh_U8 = _rTc_X.Hour_U8 / 10;
    ValLow_U8 = static_cast<uint8_t>(_rTc_X.Hour_U8 - (ValHigh_U8 * 10));
    _rTc_U64 |= static_cast<uint64_t>(ValLow_U8) << 48;
    _rTc_U64 |= static_cast<uint64_t>(ValHigh_U8) << 56;

    ValHigh_U8 = (_rTc_X.pUserBit_U8[0] & 0xF0);
    ValLow_U8 = (_rTc_X.pUserBit_U8[0] & 0x0F);
    _rTc_U64 |= static_cast<uint32_t>(ValLow_U8) << 4;
    _rTc_U64 |= static_cast<uint32_t>(ValHigh_U8) << 8;

    ValHigh_U8 = (_rTc_X.pUserBit_U8[1] & 0xF0);
    ValLow_U8 = (_rTc_X.pUserBit_U8[1] & 0x0F);
    _rTc_U64 |= static_cast<uint32_t>(ValLow_U8) << 20;
    _rTc_U64 |= static_cast<uint32_t>(ValHigh_U8) << 24;

    ValHigh_U8 = (_rTc_X.pUserBit_U8[2] & 0xF0);
    ValLow_U8 = (_rTc_X.pUserBit_U8[2] & 0x0F);
    _rTc_U64 |= static_cast<uint64_t>(ValLow_U8) << 36;
    _rTc_U64 |= static_cast<uint64_t>(ValHigh_U8) << 40;

    ValHigh_U8 = (_rTc_X.pUserBit_U8[3] & 0xF0);
    ValLow_U8 = (_rTc_X.pUserBit_U8[3] & 0x0F);
    _rTc_U64 |= static_cast<uint64_t>(ValLow_U8) << 52;
    _rTc_U64 |= static_cast<uint64_t>(ValHigh_U8) << 56;

    if (_rTc_X.TcFlag_U8 & BOF_TIMECODE_FLAG_DROP)
    {
      _rTc_U64 |= 0x00000400;
    }

    if (_rTc_X.TcFlag_U8 & BOF_TIMECODE_FLAG_COLOR_FRAME)
    {
      _rTc_U64 |= 0x00000800;
    }
    if (_rTc_X.TcFlag_U8 & BOF_TIMECODE_FLAG_PARITY_ODD)
    {
      _rTc_U64 |= 0x08000000;
    }
  }
  return Rts_E;
}

BOFERR BofTimecode::S_IncBinTimeCode(uint64_t &_rTc_U64, int32_t _NbField_S32)
{
  BOFERR Rts_E;
  BOF_TIMECODE Tc_X;

  Rts_E = S_BinToTimeCode(_rTc_U64, Tc_X);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Rts_E = BOF_ERR_EINVAL;
    BofTimecode Tc(Tc_X);
    if (Tc.IsTimecodeValid())
    {
      Tc = Tc + _NbField_S32;
      Tc_X = Tc.ToByteStruct();
      Rts_E = S_TimeCodeToBin(Tc_X, _rTc_U64);
    }
  }
  return Rts_E;
}
BOFERR BofTimecode::S_IncTimeCode(BOF_TIMECODE &_rTc_X, int32_t _NbField_S32)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BofTimecode Tc(_rTc_X);

  if (Tc.IsTimecodeValid())
  {
    Rts_E = BOF_ERR_NO_ERROR;
    Tc = Tc + _NbField_S32;
    _rTc_X = Tc.ToByteStruct();
  }
  return Rts_E;
}
END_BOF_NAMESPACE()
