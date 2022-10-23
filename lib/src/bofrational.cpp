/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements rational class
 *
 * Name:        bofrational.cpp
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
#include <bofstd/bofrational.h>

#include <iostream>
#include <valarray> 

BEGIN_BOF_NAMESPACE()

//https://stackoverflow.com/questions/26643695/converting-a-floating-point-decimal-value-to-a-fraction

BofRational::BofRational(double _Number_lf, uint32_t _NbCycle_U32, double _Precision_lf)
{
  int Sign_i;
  double NewNumber_lf, WholePart_lf;
  double DecimalPart_lf;
  uint32_t Counter_U32;

  Sign_i = (_Number_lf > 0) ? 1 : -1;
  _Number_lf = _Number_lf * Sign_i; //abs(number);
  Counter_U32 = 0;
  DecimalPart_lf = _Number_lf - (int)_Number_lf;
  std::valarray<double> Vec1{ double((int)_Number_lf), 1 }, Vec2{ 1,0 }, Temp;

  while ((DecimalPart_lf > _Precision_lf) & (Counter_U32 < _NbCycle_U32))
  {
    NewNumber_lf = 1.0 / DecimalPart_lf;
    WholePart_lf = (int)NewNumber_lf;

    Temp = Vec1;
    Vec1 = (WholePart_lf * Vec1) + Vec2;
    Vec2 = Temp;

    DecimalPart_lf = NewNumber_lf - WholePart_lf;
    Counter_U32++;
    //printf("bha: %d %f %f prec %f %f/%f\n", Counter_U32, WholePart_lf, DecimalPart_lf, _Precision_lf, Vec1[0], Vec1[1]);
  }
  double Val_lf = (double)(Sign_i * Vec1[0]) / (double)Vec1[1];
  Set((uint64_t)(Sign_i * Vec1[0]), (uint64_t)Vec1[1], false);
  //printf("%f Fract: %lld/%lld Val %f Delta %f Frac %lld/%lld\n", _Number_lf, (uint64_t)(Sign_i * Vec1[0]), (uint64_t)Vec1[1], Val_lf, Val_lf - _Number_lf, Num(), Den());
  //as_fraction(_Number_lf);
}


// Normalize fraction using Euclidian algorithm
// see http://www.linux-related.de/index.html?/coding/alg_euklid.htm
void BofRational::Normalize()
{
  uint64_t u = (mNumerator_S64 >= 0) ? mNumerator_S64 : (-mNumerator_S64);
  uint64_t v = mDenominator_U64;
  uint64_t tmp;
  while (u > 0) {
    tmp = u;
    u = v % u;
    v = tmp;
  }
  if (v > 1) {
    mNumerator_S64 /= v;
    mDenominator_U64 /= v;
  }
}



END_BOF_NAMESPACE()