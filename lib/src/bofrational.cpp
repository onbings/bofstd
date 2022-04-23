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

BEGIN_BOF_NAMESPACE()

/*
uint64_t GreatestCommonDivisor(const uint64_t a, const uint64_t b)
{
  register uint64_t u = a;
  register uint64_t v = b;
  register uint64_t tmp;
  while (u > 0) {
    tmp = u;
    u = v % u;
    v = tmp;
  }
  return v;
}
*/
BofRational BofRational::FromDouble(const double val)
{
  return BofRational((int64_t)val * 1000000LL, 1000000LL,true); // FIXME : This is very inefficient and inaccurate
}

BofRational BofRational::FromFloat(const float val)
{
  return BofRational((int64_t)val * 1000000LL, 1000000LL,true); // FIXME : This is very inefficient and inaccurate
}


// Normalize fraction using Euklidian algorithm
// see http://www.linux-related.de/index.html?/coding/alg_euklid.htm
void BofRational::Normalize()
{
  register uint64_t u = (mNumerator_S64 >= 0) ? mNumerator_S64 : (-mNumerator_S64);
  register uint64_t v = mDenominator_U64;
  register uint64_t tmp;
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