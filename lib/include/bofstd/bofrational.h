/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines rational class manipulation
 *
 * Name:        bofvideostandard.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  May 26 2020  BHA : Initial release
 */

#pragma once

#include <bofstd/bofstd.h>

BEGIN_BOF_NAMESPACE()

//uint64_t GreatestCommonDivisor(const uint64_t a, const uint64_t b);

class BOFSTD_EXPORT BofRational
{
public:
  BofRational() : mNumerator_S64(0), mDenominator_U64(1)
  {
  }
  BofRational(const BofRational &r) : mNumerator_S64(r.mNumerator_S64), mDenominator_U64(r.mDenominator_U64)
  {
  }
  BofRational(const int64_t num) : mNumerator_S64(num), mDenominator_U64(1)
  {
  }
  BofRational(const int64_t num, const uint64_t den, bool _Normalize_B)
  {
    Set(num, den, _Normalize_B);
  }
  inline BofRational& operator=(const BofRational &r)
  {
    mNumerator_S64 = r.mNumerator_S64;
    mDenominator_U64 = r.mDenominator_U64;
    return *this;
  }
  inline BofRational& operator=(const int64_t &i)
  {
    mNumerator_S64 = i;
    mDenominator_U64 = 1;
    return *this;
  }
  inline bool operator==(const BofRational &r) const
  {
    return ((mNumerator_S64 == r.mNumerator_S64) && (mDenominator_U64 == r.mDenominator_U64));
  }
  inline bool operator==(const int64_t val) const
  {
    return ((mNumerator_S64 == val) && (mDenominator_U64 == 1));
  }
  inline bool operator!=(const BofRational &r) const
  {
    return ((mNumerator_S64 != r.mNumerator_S64) || (mDenominator_U64 != r.mDenominator_U64));
  }
  inline bool operator!=(const int64_t val) const
  {
    return ((mNumerator_S64 != val) || (mDenominator_U64 != 1));
  }
  inline bool operator<(const BofRational &r) const
  {
    return ((int64_t)(mNumerator_S64 * r.mDenominator_U64) < (int64_t)(mDenominator_U64 * r.mNumerator_S64));
  }
  inline bool operator<(const int64_t val) const
  {
    return (mNumerator_S64 < (int64_t)(mDenominator_U64 * val));
  }
  inline bool operator<=(const BofRational &r) const
  {
    return ((int64_t)(mNumerator_S64 * r.mDenominator_U64) <= (int64_t)(mDenominator_U64 * r.mNumerator_S64));
  }
  inline bool operator<=(const int64_t val) const
  {
    return (mNumerator_S64 <= (int64_t)(mDenominator_U64 * val));
  }
  inline bool operator>(const BofRational &r) const
  {
    return ((int64_t)(mNumerator_S64 * r.mDenominator_U64) > (int64_t)(mDenominator_U64 * r.mNumerator_S64));
  }
  inline bool operator>(const int64_t val) const
  {
    return (mNumerator_S64 > (int64_t)(mDenominator_U64 * val));
  }
  inline bool operator>=(const BofRational &r) const
  {
    return ((int64_t)(mNumerator_S64 * r.mDenominator_U64) >= (int64_t)(mDenominator_U64 * r.mNumerator_S64));
  }
  inline bool operator>=(const int64_t val) const
  {
    return (mNumerator_S64 >= (int64_t)(mDenominator_U64 * val));
  }
  // prefix operator ++
  BofRational& operator++()
  {
    mNumerator_S64 += mDenominator_U64;
    return *this;
  }
  // postfix operator ++
  BofRational operator++(int)
  {
    BofRational r = (*this);
    mNumerator_S64 += mDenominator_U64;
    return r;
  }
  // prefix operator --
  BofRational& operator--()
  {
    mNumerator_S64 -= mDenominator_U64;
    return *this;
  }
  // postfix operator --
  BofRational operator--(int)
  {
    BofRational r = (*this);
    mNumerator_S64 -= mDenominator_U64;
    return r;
  }
  inline BofRational operator+(const BofRational &r) const
  {
    return BofRational(mNumerator_S64 * r.mDenominator_U64 + r.mNumerator_S64 * mDenominator_U64, mDenominator_U64 * r.mDenominator_U64,true);
  }
  inline BofRational operator+(const int64_t val) const
  {
    return BofRational(mNumerator_S64 + val * mDenominator_U64, mDenominator_U64,true);
  }
  inline BofRational& operator+=(const BofRational &r)
  {
    Set(mNumerator_S64 * r.mDenominator_U64 + r.mNumerator_S64 * mDenominator_U64, mDenominator_U64 * r.mDenominator_U64,true);
    return *this;
  }
  inline BofRational& operator+=(const int64_t val)
  {
    Set(mNumerator_S64 + val * mDenominator_U64, mDenominator_U64,true);
    return *this;
  }
  inline BofRational operator-(const BofRational &r) const
  {
    return BofRational(mNumerator_S64 * r.mDenominator_U64 - r.mNumerator_S64 * mDenominator_U64, mDenominator_U64 * r.mDenominator_U64,true);
  }
  inline BofRational operator-(const int64_t val) const
  {
    return BofRational(mNumerator_S64 - val * mDenominator_U64, mDenominator_U64, true);
  }
  inline BofRational& operator-=(const BofRational &r)
  {
    Set(mNumerator_S64 * r.mDenominator_U64 - r.mNumerator_S64 * mDenominator_U64, mDenominator_U64 * r.mDenominator_U64,true);
    return *this;
  }
  inline BofRational& operator-=(const int64_t val)
  {
    Set(mNumerator_S64 - val * mDenominator_U64, mDenominator_U64,true);
    return *this;
  }
  inline BofRational operator*(const BofRational &r) const
  {
    return BofRational(mNumerator_S64 * r.mNumerator_S64, mDenominator_U64 * r.mDenominator_U64,true);
  }
  inline BofRational operator*(const int64_t val) const
  {
    return BofRational(mNumerator_S64 * val, mDenominator_U64,true);
  }
  inline BofRational& operator*=(const BofRational &r)
  {
    Set(mNumerator_S64 * r.mNumerator_S64, mDenominator_U64 * r.mDenominator_U64,true);
    return *this;
  }
  inline BofRational& operator*=(const int64_t val)
  {
    Set(mNumerator_S64 * val, mDenominator_U64, true);
    return *this;
  }
  inline BofRational operator/(const BofRational &r) const
  {
    return BofRational(mNumerator_S64 * r.mDenominator_U64, mDenominator_U64 * r.mNumerator_S64,true);
  }
  inline BofRational operator/(const int64_t val) const
  {
    return BofRational(mNumerator_S64, mDenominator_U64 * val,true);
  }
  inline BofRational& operator/=(const BofRational &r)
  {
    Set(mNumerator_S64 * r.mDenominator_U64, mDenominator_U64 * r.mNumerator_S64,true);
    return *this;
  }
  inline BofRational& operator/=(const int64_t val)
  {
    Set(mNumerator_S64, mDenominator_U64 * val,true);
    return *this;
  }

  inline int64_t Ceil() const
  {
    return ((mNumerator_S64 + mDenominator_U64 - 1) / mDenominator_U64);
  }
  inline BofRational Inv() const
  {
    return BofRational(mDenominator_U64, mNumerator_S64,true);
  }
  inline int64_t Num() const
  {
    return mNumerator_S64;
  }
  inline uint64_t Den() const
  {
    return mDenominator_U64;
  }
  inline int64_t Floor() const
  {
    return (mNumerator_S64 / mDenominator_U64);
  }
  inline int64_t Round() const
  {
    return (mDenominator_U64 > 1) ? ((mNumerator_S64 + ((mDenominator_U64 + 1) >> 1)) / mDenominator_U64) : mNumerator_S64;
  }

  inline bool IsInteger() const
  {
    return (mDenominator_U64 == 1);
  }

  static BofRational FromDouble(const double val);
  inline double ToDouble() const
  {
    return (double)mNumerator_S64 / (double)mDenominator_U64;
  }

  static BofRational FromFloat(const float val);
  inline float ToFloat() const
  {
    return (float)((double)mNumerator_S64 / (double)mDenominator_U64);
  }



protected:
  int64_t  mNumerator_S64; // signed value
  uint64_t mDenominator_U64; // unsigned value

  void Normalize();
  void Set(const int64_t num, const uint64_t den, bool _Normalize_B)
  {
    mNumerator_S64 = num;
    mDenominator_U64 = (den ? den : 1);
    if (_Normalize_B)
		{
			Normalize();
		}
  }

};

inline bool operator==(const int64_t i, const BofRational &r)
{
  return (BofRational(i) == r);
}

inline bool operator!=(const int64_t i, const BofRational &r)
{
  return (BofRational(i) != r);
}

inline bool operator<(const int64_t i, const BofRational &r)
{
  return (BofRational(i) < r);
}

inline bool operator<=(const int64_t i, const BofRational &r)
{
  return (BofRational(i) <= r);
}

inline bool operator>(const int64_t i, const BofRational &r)
{
  return (BofRational(i) > r);
}

inline bool operator>=(const int64_t i, const BofRational &r)
{
  return (BofRational(i) >= r);
}

inline BofRational operator+(const int64_t i, const BofRational &r)
{
  return (BofRational(i) + r);
}

inline BofRational operator-(const int64_t i, const BofRational &r)
{
  return (BofRational(i) - r);
}

inline BofRational operator*(const int64_t i, const BofRational &r)
{
  return (BofRational(i) * r);
}

inline BofRational operator/(const int64_t i, const BofRational &r)
{
  return (BofRational(i) / r);
}


END_BOF_NAMESPACE()