/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines various cryptographic class.
 *    BofCryptoMd5: This one the RFC 1321 from the RSA Data Security, Inc. MD5 Message-Digest Algorithm
 *
 * Name:        BofCrypto.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Apr 14 2003  BHA : Initial release
 */

#pragma once

/*** Include ****************************************************************/

#include <cstdint>
#include <bofstd/bofstd.h>

BEGIN_BOF_NAMESPACE()
/*** Define *****************************************************************/

#define S11     7
#define S12     12
#define S13     17
#define S14     22
#define S21     5
#define S22     9
#define S23     14
#define S24     20
#define S31     4
#define S322    11
#define S33     16
#define S34     23
#define S41     6
#define S42     10
#define S43     15
#define S44     21

/*! F,G,H and I are basic MD5 functions.*/
#define F(x, y, z)                  ( ( (x) & (y) ) | ( (~x) & (z) ) )
#define G(x, y, z)                  ( ( (x) & (z) ) | ( (y) & (~z) ) )
#define H(x, y, z)                  ( (x) ^ (y) ^ (z) )
#define I(x, y, z)                  ( (y) ^ ( (x) | (~z) ) )

#define ROTATE_LEFT(x, n)           ( ( (x) << (n) ) | ( (x) >> (32 - (n) ) ) ) // ROTATE_LEFT rotates x left n bits.

/*! FF,GG,HH,and II transformations for rounds 1,2,3,and 4. Rotation is separate from addition to prevent recomputation.*/
#define FF(a, b, c, d, x, s, ac)    { (a) += F( (b), (c), (d) ) + (x) + (uint32_t)(ac); (a) = ROTATE_LEFT( (a), (s) ); (a) += (b); }
#define GG(a, b, c, d, x, s, ac)    { (a) += G( (b), (c), (d) ) + (x) + (uint32_t)(ac); (a) = ROTATE_LEFT( (a), (s) ); (a) += (b); }
#define HH(a, b, c, d, x, s, ac)    { (a) += H( (b), (c), (d) ) + (x) + (uint32_t)(ac); (a) = ROTATE_LEFT( (a), (s) ); (a) += (b); }
#define II(a, b, c, d, x, s, ac)    { (a) += I( (b), (c), (d) ) + (x) + (uint32_t)(ac); (a) = ROTATE_LEFT( (a), (s) ); (a) += (b); }

/*** Structure **************************************************************/

/*! MD5 context.
 *
 * These parameters are used to keep track of BofCryptoMd5 object state
 */
struct BOF_MD5_CONTEXT
{
		uint32_t pState_U32[4];                                                    /*! State (ABCD) */
		uint32_t pCount_U32[2];                                                    /*! Number of bits, modulo 2^64 (lsb first) */
		uint8_t pBuffer_U8[64];                                                   /*! Input buffer */
		BOF_MD5_CONTEXT()
		{
			Reset();
		}

		void Reset()
		{
			memset(pState_U32, 0, sizeof(pState_U32));
			memset(pCount_U32, 0, sizeof(pCount_U32));
			memset(pBuffer_U8, 0, sizeof(pBuffer_U8));
		}
};

/*** Enum *******************************************************************/

/*** Class definition *******************************************************/

/*** BofCryptoMd5 *******************************************************************/

/*!
 * Class BofCryptoMd5
 *
 * This class defines the interface for the BofCryptoMd5 object.
 */

class BofCryptoMd5
{
private:
		BOF_MD5_CONTEXT mMd5_X;
private:
		void Transform(uint32_t *pState_U32, uint8_t *pBlock_U8);

		void Encode(uint8_t *pOutput_U8, uint32_t *pInput_U32, uint32_t Len_U32);

		void Decode(uint32_t *pOutput_U32, uint8_t *pInput_U8, uint32_t Len_U32);

		void AddData(uint8_t *pInput_U8, uint32_t InputLen_U32);

		void Compute(uint8_t *pDigest_U8, uint32_t *_pNbOut_U32);

public:
		BofCryptoMd5();

		virtual ~BofCryptoMd5();

		bool Hash(uint32_t _NbIn_U32, uint8_t *_pDataIn_U8, uint32_t *_pNbOut_U32, uint8_t *_pDataOut_U8);
};
END_BOF_NAMESPACE()