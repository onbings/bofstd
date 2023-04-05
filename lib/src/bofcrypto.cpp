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
 * Name:        BofCrypto.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Apr 14 2003  BHA : Initial release
 */
#include <bofstd/bofcrypto.h>
#include <bofstd/bofstream.h>

#include <cstring>
#include <fstream>
#include <iomanip>
// #include <openssl/sha.h>

BEGIN_BOF_NAMESPACE()

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S322 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

/*! F,G,H and I are basic MD5 functions.*/
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n)))) // ROTATE_LEFT rotates x left n bits.

/*! FF,GG,HH,and II transformations for rounds 1,2,3,and 4. Rotation is separate from addition to prevent recomputation.*/
#define FF(a, b, c, d, x, s, ac)                                                                                                                                                                                                                               \
  {                                                                                                                                                                                                                                                            \
    (a) += F((b), (c), (d)) + (x) + (uint32_t)(ac);                                                                                                                                                                                                            \
    (a) = ROTATE_LEFT((a), (s));                                                                                                                                                                                                                               \
    (a) += (b);                                                                                                                                                                                                                                                \
  }
#define GG(a, b, c, d, x, s, ac)                                                                                                                                                                                                                               \
  {                                                                                                                                                                                                                                                            \
    (a) += G((b), (c), (d)) + (x) + (uint32_t)(ac);                                                                                                                                                                                                            \
    (a) = ROTATE_LEFT((a), (s));                                                                                                                                                                                                                               \
    (a) += (b);                                                                                                                                                                                                                                                \
  }
#define HH(a, b, c, d, x, s, ac)                                                                                                                                                                                                                               \
  {                                                                                                                                                                                                                                                            \
    (a) += H((b), (c), (d)) + (x) + (uint32_t)(ac);                                                                                                                                                                                                            \
    (a) = ROTATE_LEFT((a), (s));                                                                                                                                                                                                                               \
    (a) += (b);                                                                                                                                                                                                                                                \
  }
#define II(a, b, c, d, x, s, ac)                                                                                                                                                                                                                               \
  {                                                                                                                                                                                                                                                            \
    (a) += I((b), (c), (d)) + (x) + (uint32_t)(ac);                                                                                                                                                                                                            \
    (a) = ROTATE_LEFT((a), (s));                                                                                                                                                                                                                               \
    (a) += (b);                                                                                                                                                                                                                                                \
  }

/*
 * void main()
 * {
 * BofCryptoMd5    *pMd5_O;
 * char   pString_c[256];
 * uint8_t  pDigest_U8[16];
 * uint32_t  i_U32,Len_U32;
 *
 * pMd5_O=new BofCryptoMd5();
 * strcpy(pString_c,"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
 * Len_U32=strlen(pString_c);
 * pMd5_O->Hash(Len_U32,(uint8_t *)pString_c,pDigest_U8);
 *
 * printf("MD5 (\"%s\")=", pString_c);
 * for (i_U32=0;i_U32<16;i_U32++) printf ("%02x",pDigest_U8[i_U32]);
 * printf ("\n");
 * getch();
 * }
 */

static uint8_t S_pMd5Padding_U8[] = {0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/*!
 * Description
 * This constructor begins an MD5 operation,writing a new context.
 *
 * Parameters
 * None
 *
 * Returns
 * None
 *
 * Remarks
 * None
 *
 * See Also
 * None
 */

BofCryptoMd5::BofCryptoMd5()
{
  mMd5_X.Reset();
  mMd5_X.pCount_U32[0] = mMd5_X.pCount_U32[1] = 0;

  /* Load magic initialization constants.*/
  mMd5_X.pState_U32[0] = 0x67452301;
  mMd5_X.pState_U32[1] = 0xefcdab89;
  mMd5_X.pState_U32[2] = 0x98badcfe;
  mMd5_X.pState_U32[3] = 0x10325476;
}

/*!
 * Description
 * This function releases all the resources allocated by the object
 *
 * Parameters
 * None
 *
 * Returns
 * None
 *
 * Remarks
 * None
 *
 * See Also
 * None
 */
BofCryptoMd5::~BofCryptoMd5()
{
}

/*!
 * Description
 * This function compute the BofCryptoMd5 hash value of a given data buffer
 *
 * Parameters
 * _NbIn_U32 :     Specifies the buffer length
 * _pDataIn_U8 :   Specifies a pointer to the input data
 * _pOut_U32:	   Specifies the maximum buffer capacity of _pDataOut_U8
 * _pDataOut_U32 :  Returns the commputed MD5 hash value (128 bits) of the input buffer
 *
 * Returns
 * None
 *
 * Remarks
 * pHashCode_U8 must be a 16 bytes long buffer (128 bits)
 *
 * Hash ("") = d41d8cd98f00b204e9800998ecf8427e
 * Hash ("a") = 0cc175b9c0f1b6a831c399e269772661
 * Hash ("abc") = 900150983cd24fb0d6963f7d28e17f72
 * Hash ("message digest") = f96b697d7cb7938d525a2f31aaf161d0
 * Hash ("abcdefghijklmnopqrstuvwxyz") = c3fcd3d76192e4007dfb496cca67e13b
 * Hash ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") = d174ab98d277d9f5a5611c2c9f419d9f
 * Hash ("12345678901234567890123456789012345678901234567890123456789012345678901234567890") = 57edf4a22be3c955ac49da2e2107b67a
 *
 * See Also
 * None
 */
bool BofCryptoMd5::Hash(uint32_t _NbIn_U32, uint8_t *_pDataIn_U8, uint32_t *_pNbOut_U32, uint8_t *_pDataOut_U8)
{
  bool Rts_B = false;

  if ((_pDataIn_U8) && (_pNbOut_U32) && (_pDataOut_U8))
  {
    if (*_pNbOut_U32 >= 16)
    {
      AddData(_pDataIn_U8, _NbIn_U32);
      Compute(_pDataOut_U8, _pNbOut_U32);
      Rts_B = true;
    }
  }
  return Rts_B;
}

/*!
 * Description
 * This function continues an MD5 message-digest operation,processing another message block,
 * and updating the context.
 *
 * Parameters
 * pInput_U8 :       Specifies a pointer to the message to digest
 * InputLen_U32 :      Specifies the message length
 *
 * Returns
 * None
 *
 * Remarks
 * None
 *
 * See Also
 * None
 */
void BofCryptoMd5::AddData(uint8_t *pInput_U8, uint32_t InputLen_U32)
{
  uint32_t i_U32, Index_U32, PartLen_U32;

  /* Compute number of bytes mod 64 */
  Index_U32 = (uint32_t)((mMd5_X.pCount_U32[0] >> 3) & 0x3F);

  /* AddData number of bits */
  if ((mMd5_X.pCount_U32[0] += ((uint32_t)InputLen_U32 << 3)) < ((uint32_t)InputLen_U32 << 3))
  {
    mMd5_X.pCount_U32[1]++;
  }
  mMd5_X.pCount_U32[1] += ((uint32_t)InputLen_U32 >> 29);

  PartLen_U32 = 64 - Index_U32;

  /* Transform as many times as possible.*/
  if (InputLen_U32 >= PartLen_U32)
  {
    memcpy(&mMd5_X.pBuffer_U8[Index_U32], pInput_U8, PartLen_U32);
    Transform(mMd5_X.pState_U32, mMd5_X.pBuffer_U8);

    for (i_U32 = PartLen_U32; i_U32 + 63 < InputLen_U32; i_U32 += 64)
    {
      Transform(mMd5_X.pState_U32, &pInput_U8[i_U32]);
    }

    Index_U32 = 0;
  }
  else
  {
    i_U32 = 0;
  }

  /* Buffer remaining input */
  memcpy(&mMd5_X.pBuffer_U8[Index_U32], &pInput_U8[i_U32], InputLen_U32 - i_U32);
}

/*!
 * Description
 * This function ends an MD5 message-digest operation,writing the message digest and zeroizing the
 * context.
 *
 * Parameters
 * pDigest_U8 :  Returns the result of the MD5 message computation.
 *
 * Returns
 * None
 *
 * Remarks
 * pDigest_U8 must be a 16 byte long buffer (128 bits)
 *
 * See Also
 * None
 */
void BofCryptoMd5::Compute(uint8_t *pDigest_U8, uint32_t *_pNbOut_U32)
{
  uint8_t pBits_U8[8];
  uint32_t Index_U32, PadLen_U32;

  /* Save number of bits */
  Encode(pBits_U8, mMd5_X.pCount_U32, 8);

  /* Pad out to 56 mod 64.*/
  Index_U32 = (uint32_t)((mMd5_X.pCount_U32[0] >> 3) & 0x3f);
  PadLen_U32 = (Index_U32 < 56) ? (56 - Index_U32) : (120 - Index_U32);

  /*
   * The message is "padded" (extended) so that its length (in bits) is congruent to 448, modulo 512.
   * That is, the message is extended so that it is just 64 bits shy of being a multiple of 512 bits
   * long. Padding is always performed, even if the length of the message is already congruent to
   * 448, modulo 512.
   * Padding is performed as follows: a single "1" bit is appended to the message, and then "0" bits
   * are appended so that the length in bits of the padded message becomes congruent to 448,
   * modulo 512. In all, at least one bit and at most 512 bits are appended.
   */

  AddData(S_pMd5Padding_U8, PadLen_U32);

  /*
   * A 64-bit representation of b (the length of the message before the padding bits were added) is
   * appended to the result of the previous step. In the unlikely event that b is greater than 2^64,
   * then only the low-order 64 bits of b are used. (These bits are appended as two 32-bit words and
   * appended low-order word first in accordance with the previous conventions.)
   *
   * At this point the resulting message (after padding with bits and with b) has a length that is
   * an exact multiple of 512 bits. Equivalently, this message has a length that is an exact
   * multiple of 16 (32-bit) words. Let M[0 ... N-1] denote the words of the resulting message,
   * where N is a multiple of 16.
   */
  AddData(pBits_U8, 8);

  /* Store state in pDigest_U8 */
  Encode(pDigest_U8, mMd5_X.pState_U32, 16);

  if (_pNbOut_U32)
  {
    *_pNbOut_U32 = 16;
  }
  /* Zeroize sensitive information.*/

  memset(&mMd5_X, 0, sizeof(mMd5_X));
}

/*!
 * Description
 * This function is basic MD5 transformation, it transforms state based on block.
 *
 * Parameters
 * pState_U32 :  Specifies MD5 state.
 * pBlock_U8 :  Specifies a pointer to data block
 *
 * Returns
 * None
 *
 * Remarks
 * None
 *
 * See Also
 * None
 */
void BofCryptoMd5::Transform(uint32_t *pState_U32, uint8_t *pBlock_U8)
{
  uint32_t a_U32 = pState_U32[0], b_U32 = pState_U32[1], c_U32 = pState_U32[2], d_U32 = pState_U32[3], px_U32[16];

  Decode(px_U32, pBlock_U8, 64);

  /* Round 1 */
  FF(a_U32, b_U32, c_U32, d_U32, px_U32[0], S11, 0xd76aa478);  /* 1 */
  FF(d_U32, a_U32, b_U32, c_U32, px_U32[1], S12, 0xe8c7b756);  /* 2 */
  FF(c_U32, d_U32, a_U32, b_U32, px_U32[2], S13, 0x242070db);  /* 3 */
  FF(b_U32, c_U32, d_U32, a_U32, px_U32[3], S14, 0xc1bdceee);  /* 4 */
  FF(a_U32, b_U32, c_U32, d_U32, px_U32[4], S11, 0xf57c0faf);  /* 5 */
  FF(d_U32, a_U32, b_U32, c_U32, px_U32[5], S12, 0x4787c62a);  /* 6 */
  FF(c_U32, d_U32, a_U32, b_U32, px_U32[6], S13, 0xa8304613);  /* 7 */
  FF(b_U32, c_U32, d_U32, a_U32, px_U32[7], S14, 0xfd469501);  /* 8 */
  FF(a_U32, b_U32, c_U32, d_U32, px_U32[8], S11, 0x698098d8);  /* 9 */
  FF(d_U32, a_U32, b_U32, c_U32, px_U32[9], S12, 0x8b44f7af);  /* 10 */
  FF(c_U32, d_U32, a_U32, b_U32, px_U32[10], S13, 0xffff5bb1); /* 11 */
  FF(b_U32, c_U32, d_U32, a_U32, px_U32[11], S14, 0x895cd7be); /* 12 */
  FF(a_U32, b_U32, c_U32, d_U32, px_U32[12], S11, 0x6b901122); /* 13 */
  FF(d_U32, a_U32, b_U32, c_U32, px_U32[13], S12, 0xfd987193); /* 14 */
  FF(c_U32, d_U32, a_U32, b_U32, px_U32[14], S13, 0xa679438e); /* 15 */
  FF(b_U32, c_U32, d_U32, a_U32, px_U32[15], S14, 0x49b40821); /* 16 */

  /* Round 2 */
  GG(a_U32, b_U32, c_U32, d_U32, px_U32[1], S21, 0xf61e2562);  /* 17 */
  GG(d_U32, a_U32, b_U32, c_U32, px_U32[6], S22, 0xc040b340);  /* 18 */
  GG(c_U32, d_U32, a_U32, b_U32, px_U32[11], S23, 0x265e5a51); /* 19 */
  GG(b_U32, c_U32, d_U32, a_U32, px_U32[0], S24, 0xe9b6c7aa);  /* 20 */
  GG(a_U32, b_U32, c_U32, d_U32, px_U32[5], S21, 0xd62f105d);  /* 21 */
  GG(d_U32, a_U32, b_U32, c_U32, px_U32[10], S22, 0x2441453);  /* 22 */
  GG(c_U32, d_U32, a_U32, b_U32, px_U32[15], S23, 0xd8a1e681); /* 23 */
  GG(b_U32, c_U32, d_U32, a_U32, px_U32[4], S24, 0xe7d3fbc8);  /* 24 */
  GG(a_U32, b_U32, c_U32, d_U32, px_U32[9], S21, 0x21e1cde6);  /* 25 */
  GG(d_U32, a_U32, b_U32, c_U32, px_U32[14], S22, 0xc33707d6); /* 26 */
  GG(c_U32, d_U32, a_U32, b_U32, px_U32[3], S23, 0xf4d50d87);  /* 27 */
  GG(b_U32, c_U32, d_U32, a_U32, px_U32[8], S24, 0x455a14ed);  /* 28 */
  GG(a_U32, b_U32, c_U32, d_U32, px_U32[13], S21, 0xa9e3e905); /* 29 */
  GG(d_U32, a_U32, b_U32, c_U32, px_U32[2], S22, 0xfcefa3f8);  /* 30 */
  GG(c_U32, d_U32, a_U32, b_U32, px_U32[7], S23, 0x676f02d9);  /* 31 */
  GG(b_U32, c_U32, d_U32, a_U32, px_U32[12], S24, 0x8d2a4c8a); /* 32 */

  /* Round 3 */
  HH(a_U32, b_U32, c_U32, d_U32, px_U32[5], S31, 0xfffa3942);   /* 33 */
  HH(d_U32, a_U32, b_U32, c_U32, px_U32[8], S322, 0x8771f681);  /* 34 */
  HH(c_U32, d_U32, a_U32, b_U32, px_U32[11], S33, 0x6d9d6122);  /* 35 */
  HH(b_U32, c_U32, d_U32, a_U32, px_U32[14], S34, 0xfde5380c);  /* 36 */
  HH(a_U32, b_U32, c_U32, d_U32, px_U32[1], S31, 0xa4beea44);   /* 37 */
  HH(d_U32, a_U32, b_U32, c_U32, px_U32[4], S322, 0x4bdecfa9);  /* 38 */
  HH(c_U32, d_U32, a_U32, b_U32, px_U32[7], S33, 0xf6bb4b60);   /* 39 */
  HH(b_U32, c_U32, d_U32, a_U32, px_U32[10], S34, 0xbebfbc70);  /* 40 */
  HH(a_U32, b_U32, c_U32, d_U32, px_U32[13], S31, 0x289b7ec6);  /* 41 */
  HH(d_U32, a_U32, b_U32, c_U32, px_U32[0], S322, 0xeaa127fa);  /* 42 */
  HH(c_U32, d_U32, a_U32, b_U32, px_U32[3], S33, 0xd4ef3085);   /* 43 */
  HH(b_U32, c_U32, d_U32, a_U32, px_U32[6], S34, 0x4881d05);    /* 44 */
  HH(a_U32, b_U32, c_U32, d_U32, px_U32[9], S31, 0xd9d4d039);   /* 45 */
  HH(d_U32, a_U32, b_U32, c_U32, px_U32[12], S322, 0xe6db99e5); /* 46 */
  HH(c_U32, d_U32, a_U32, b_U32, px_U32[15], S33, 0x1fa27cf8);  /* 47 */
  HH(b_U32, c_U32, d_U32, a_U32, px_U32[2], S34, 0xc4ac5665);   /* 48 */

  /* Round 4 */
  II(a_U32, b_U32, c_U32, d_U32, px_U32[0], S41, 0xf4292244);  /* 49 */
  II(d_U32, a_U32, b_U32, c_U32, px_U32[7], S42, 0x432aff97);  /* 50 */
  II(c_U32, d_U32, a_U32, b_U32, px_U32[14], S43, 0xab9423a7); /* 51 */
  II(b_U32, c_U32, d_U32, a_U32, px_U32[5], S44, 0xfc93a039);  /* 52 */
  II(a_U32, b_U32, c_U32, d_U32, px_U32[12], S41, 0x655b59c3); /* 53 */
  II(d_U32, a_U32, b_U32, c_U32, px_U32[3], S42, 0x8f0ccc92);  /* 54 */
  II(c_U32, d_U32, a_U32, b_U32, px_U32[10], S43, 0xffeff47d); /* 55 */
  II(b_U32, c_U32, d_U32, a_U32, px_U32[1], S44, 0x85845dd1);  /* 56 */
  II(a_U32, b_U32, c_U32, d_U32, px_U32[8], S41, 0x6fa87e4f);  /* 57 */
  II(d_U32, a_U32, b_U32, c_U32, px_U32[15], S42, 0xfe2ce6e0); /* 58 */
  II(c_U32, d_U32, a_U32, b_U32, px_U32[6], S43, 0xa3014314);  /* 59 */
  II(b_U32, c_U32, d_U32, a_U32, px_U32[13], S44, 0x4e0811a1); /* 60 */
  II(a_U32, b_U32, c_U32, d_U32, px_U32[4], S41, 0xf7537e82);  /* 61 */
  II(d_U32, a_U32, b_U32, c_U32, px_U32[11], S42, 0xbd3af235); /* 62 */
  II(c_U32, d_U32, a_U32, b_U32, px_U32[2], S43, 0x2ad7d2bb);  /* 63 */
  II(b_U32, c_U32, d_U32, a_U32, px_U32[9], S44, 0xeb86d391);  /* 64 */

  pState_U32[0] += a_U32;
  pState_U32[1] += b_U32;
  pState_U32[2] += c_U32;
  pState_U32[3] += d_U32;

  /* Zeroize sensitive information.*/
  memset((uint8_t *)px_U32, 0, sizeof(px_U32));
}

/*!
 * Description
 * This function encodes input(uint32_t) into output(uint8_t). Assumes Len_U32 is a multiple of 4.
 *
 * Parameters
 * pOutput_U8 :  Specifies pointer to result buffer
 * pInput_U32 :  Specifies pointer to input buffer
 * Len_U32 :      Specifies input buffer length
 *
 * Returns
 * None
 *
 * Remarks
 * None
 *
 * See Also
 * None
 */
void BofCryptoMd5::Encode(uint8_t *pOutput_U8, uint32_t *pInput_U32, uint32_t Len_U32)
{
  uint32_t i_U32, j_U32;

  for (i_U32 = 0, j_U32 = 0; j_U32 < Len_U32; i_U32++, j_U32 += 4)
  {
    pOutput_U8[j_U32] = (uint8_t)(pInput_U32[i_U32] & 0xff);
    pOutput_U8[j_U32 + 1] = (uint8_t)((pInput_U32[i_U32] >> 8) & 0xff);
    pOutput_U8[j_U32 + 2] = (uint8_t)((pInput_U32[i_U32] >> 16) & 0xff);
    pOutput_U8[j_U32 + 3] = (uint8_t)((pInput_U32[i_U32] >> 24) & 0xff);
  }
}

/*!
 * Description
 * This function decodes input(uint8_t) into output(uint32_t). Assumes Len_U32 is a multiple of 4
 *
 * Parameters
 * pOutput_U8 :  Specifies pointer to result buffer
 * pInput_U32 :  Specifies pointer to input buffer
 * Len_U32 :      Specifies input buffer length
 *
 * Returns
 * None
 *
 * Remarks
 * None
 *
 * See Also
 * None
 */
void BofCryptoMd5::Decode(uint32_t *pOutput_U32, uint8_t *pInput_U8, uint32_t Len_U32)
{
  uint32_t i_U32, j_U32;

  for (i_U32 = 0, j_U32 = 0; j_U32 < Len_U32; i_U32++, j_U32 += 4)
  {
    pOutput_U32[i_U32] = ((uint32_t)pInput_U8[j_U32]) | (((uint32_t)pInput_U8[j_U32 + 1]) << 8) | (((uint32_t)pInput_U8[j_U32 + 2]) << 16) | (((uint32_t)pInput_U8[j_U32 + 3]) << 24);
  }
}
/* from openssl:
std::string Bof_ComputeSha1(uint32_t _Nb_U32, const void *_pBuffer)
{
  uint8_t pSha1_U8[32]; // 20 is enought;
  uint32_t i_U32;
  std::ostringstream Output;

  SHA1((unsigned char *)(_pBuffer), _Nb_U32, pSha1_U8);
  Output << std::hex;
  for (i_U32 = 0; i_U32 < 20; i_U32++)
  {
    Output << std::setfill('0') << std::setw(2) << static_cast<int>(pSha1_U8[i_U32]);
  }
  //printf("Sha1='%s' len %d\n", Output.str().c_str(), Output.str().size());
  return(Output.str());
}
std::string Bof_ComputeSha1(const std::string &_Input_S)
{
  return Bof_ComputeSha1(_Input_S.size(), _Input_S.c_str());
}
*/

static const size_t BLOCK_INTS = 16; /* number of 32bit integers per SHA1 block */
static const size_t BLOCK_BYTES = BLOCK_INTS * 4;

inline static void S_Reset(uint32_t digest[], std::string &buffer, uint64_t &transforms)
{
  /* SHA1 initialization constants */
  digest[0] = 0x67452301;
  digest[1] = 0xefcdab89;
  digest[2] = 0x98badcfe;
  digest[3] = 0x10325476;
  digest[4] = 0xc3d2e1f0;

  /* Reset counters */
  buffer = "";
  transforms = 0;
}

inline static uint32_t S_Rol(const uint32_t value, const size_t bits)
{
  return (value << bits) | (value >> (32 - bits));
}

inline static uint32_t S_Blk(const uint32_t block[BLOCK_INTS], const size_t i)
{
  return S_Rol(block[(i + 13) & 15] ^ block[(i + 8) & 15] ^ block[(i + 2) & 15] ^ block[i], 1);
}

/*
 * (S_R0+S_R1), S_R2, S_R3, S_R4 are the different operations used in SHA1
 */

inline static void S_R0(const uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
  z += ((w & (x ^ y)) ^ y) + block[i] + 0x5a827999 + S_Rol(v, 5);
  w = S_Rol(w, 30);
}

inline static void S_R1(uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
  block[i] = S_Blk(block, i);
  z += ((w & (x ^ y)) ^ y) + block[i] + 0x5a827999 + S_Rol(v, 5);
  w = S_Rol(w, 30);
}

inline static void S_R2(uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
  block[i] = S_Blk(block, i);
  z += (w ^ x ^ y) + block[i] + 0x6ed9eba1 + S_Rol(v, 5);
  w = S_Rol(w, 30);
}

inline static void S_R3(uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
  block[i] = S_Blk(block, i);
  z += (((w | x) & y) | (w & x)) + block[i] + 0x8f1bbcdc + S_Rol(v, 5);
  w = S_Rol(w, 30);
}

inline static void S_R4(uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
  block[i] = S_Blk(block, i);
  z += (w ^ x ^ y) + block[i] + 0xca62c1d6 + S_Rol(v, 5);
  w = S_Rol(w, 30);
}

/*
 * Hash a single 512-bit block. This is the core of the algorithm.
 */

inline static void S_Transform(uint32_t digest[], uint32_t block[BLOCK_INTS], uint64_t &transforms)
{
  /* Copy digest[] to working vars */
  uint32_t a = digest[0];
  uint32_t b = digest[1];
  uint32_t c = digest[2];
  uint32_t d = digest[3];
  uint32_t e = digest[4];

  /* 4 rounds of 20 operations each. Loop unrolled. */
  S_R0(block, a, b, c, d, e, 0);
  S_R0(block, e, a, b, c, d, 1);
  S_R0(block, d, e, a, b, c, 2);
  S_R0(block, c, d, e, a, b, 3);
  S_R0(block, b, c, d, e, a, 4);
  S_R0(block, a, b, c, d, e, 5);
  S_R0(block, e, a, b, c, d, 6);
  S_R0(block, d, e, a, b, c, 7);
  S_R0(block, c, d, e, a, b, 8);
  S_R0(block, b, c, d, e, a, 9);
  S_R0(block, a, b, c, d, e, 10);
  S_R0(block, e, a, b, c, d, 11);
  S_R0(block, d, e, a, b, c, 12);
  S_R0(block, c, d, e, a, b, 13);
  S_R0(block, b, c, d, e, a, 14);
  S_R0(block, a, b, c, d, e, 15);
  S_R1(block, e, a, b, c, d, 0);
  S_R1(block, d, e, a, b, c, 1);
  S_R1(block, c, d, e, a, b, 2);
  S_R1(block, b, c, d, e, a, 3);
  S_R2(block, a, b, c, d, e, 4);
  S_R2(block, e, a, b, c, d, 5);
  S_R2(block, d, e, a, b, c, 6);
  S_R2(block, c, d, e, a, b, 7);
  S_R2(block, b, c, d, e, a, 8);
  S_R2(block, a, b, c, d, e, 9);
  S_R2(block, e, a, b, c, d, 10);
  S_R2(block, d, e, a, b, c, 11);
  S_R2(block, c, d, e, a, b, 12);
  S_R2(block, b, c, d, e, a, 13);
  S_R2(block, a, b, c, d, e, 14);
  S_R2(block, e, a, b, c, d, 15);
  S_R2(block, d, e, a, b, c, 0);
  S_R2(block, c, d, e, a, b, 1);
  S_R2(block, b, c, d, e, a, 2);
  S_R2(block, a, b, c, d, e, 3);
  S_R2(block, e, a, b, c, d, 4);
  S_R2(block, d, e, a, b, c, 5);
  S_R2(block, c, d, e, a, b, 6);
  S_R2(block, b, c, d, e, a, 7);
  S_R3(block, a, b, c, d, e, 8);
  S_R3(block, e, a, b, c, d, 9);
  S_R3(block, d, e, a, b, c, 10);
  S_R3(block, c, d, e, a, b, 11);
  S_R3(block, b, c, d, e, a, 12);
  S_R3(block, a, b, c, d, e, 13);
  S_R3(block, e, a, b, c, d, 14);
  S_R3(block, d, e, a, b, c, 15);
  S_R3(block, c, d, e, a, b, 0);
  S_R3(block, b, c, d, e, a, 1);
  S_R3(block, a, b, c, d, e, 2);
  S_R3(block, e, a, b, c, d, 3);
  S_R3(block, d, e, a, b, c, 4);
  S_R3(block, c, d, e, a, b, 5);
  S_R3(block, b, c, d, e, a, 6);
  S_R3(block, a, b, c, d, e, 7);
  S_R3(block, e, a, b, c, d, 8);
  S_R3(block, d, e, a, b, c, 9);
  S_R3(block, c, d, e, a, b, 10);
  S_R3(block, b, c, d, e, a, 11);
  S_R4(block, a, b, c, d, e, 12);
  S_R4(block, e, a, b, c, d, 13);
  S_R4(block, d, e, a, b, c, 14);
  S_R4(block, c, d, e, a, b, 15);
  S_R4(block, b, c, d, e, a, 0);
  S_R4(block, a, b, c, d, e, 1);
  S_R4(block, e, a, b, c, d, 2);
  S_R4(block, d, e, a, b, c, 3);
  S_R4(block, c, d, e, a, b, 4);
  S_R4(block, b, c, d, e, a, 5);
  S_R4(block, a, b, c, d, e, 6);
  S_R4(block, e, a, b, c, d, 7);
  S_R4(block, d, e, a, b, c, 8);
  S_R4(block, c, d, e, a, b, 9);
  S_R4(block, b, c, d, e, a, 10);
  S_R4(block, a, b, c, d, e, 11);
  S_R4(block, e, a, b, c, d, 12);
  S_R4(block, d, e, a, b, c, 13);
  S_R4(block, c, d, e, a, b, 14);
  S_R4(block, b, c, d, e, a, 15);

  /* Add the working vars back into digest[] */
  digest[0] += a;
  digest[1] += b;
  digest[2] += c;
  digest[3] += d;
  digest[4] += e;

  /* Count the number of transformations */
  transforms++;
}

inline static void S_BufferToBlock(const std::string &buffer, uint32_t block[BLOCK_INTS])
{
  /* Convert the std::string (byte buffer) to a uint32_t array (MSB) */
  for (size_t i = 0; i < BLOCK_INTS; i++)
  {
    block[i] = (buffer[4 * i + 3] & 0xff) | (buffer[4 * i + 2] & 0xff) << 8 | (buffer[4 * i + 1] & 0xff) << 16 | (buffer[4 * i + 0] & 0xff) << 24;
  }
}

inline BofSha1::BofSha1()
{
  S_Reset(mpDigest_U32, mBuffer_S, mTransform_U64);
}

void BofSha1::Update(const std::string &s)
{
  std::istringstream is(s);
  Update(is);
}

void BofSha1::Update(uint32_t _Nb_U32, const void *_pBuffer)
{
  BofIMemoryStream MemoryStream(_Nb_U32, (const char *)_pBuffer);
  Update(MemoryStream);
}

void BofSha1::Update(std::istream &is)
{
  while (true)
  {
    char sbuf[BLOCK_BYTES];
    is.read(sbuf, BLOCK_BYTES - mBuffer_S.size());
    mBuffer_S.append(sbuf, (std::size_t)is.gcount());
    if (mBuffer_S.size() != BLOCK_BYTES)
    {
      return;
    }
    uint32_t block[BLOCK_INTS];
    S_BufferToBlock(mBuffer_S, block);
    S_Transform(mpDigest_U32, block, mTransform_U64);
    mBuffer_S.clear();
  }
}

/*
 * Add padding and return the message digest.
 */

std::string BofSha1::Final()
{
  /* Total number of hashed bits */
  uint64_t total_bits = (mTransform_U64 * BLOCK_BYTES + mBuffer_S.size()) * 8;

  /* Padding */
  mBuffer_S += (char)0x80;
  size_t orig_size = mBuffer_S.size();
  while (mBuffer_S.size() < BLOCK_BYTES)
  {
    mBuffer_S += (char)0x00;
  }

  uint32_t block[BLOCK_INTS];
  S_BufferToBlock(mBuffer_S, block);

  if (orig_size > BLOCK_BYTES - 8)
  {
    S_Transform(mpDigest_U32, block, mTransform_U64);
    for (size_t i = 0; i < BLOCK_INTS - 2; i++)
    {
      block[i] = 0;
    }
  }

  /* Append total_bits, split this uint64_t into two uint32_t */
  block[BLOCK_INTS - 1] = (uint32_t)total_bits;
  block[BLOCK_INTS - 2] = (uint32_t)(total_bits >> 32);
  S_Transform(mpDigest_U32, block, mTransform_U64);

  /* Hex std::string */
  std::ostringstream result;
  for (size_t i = 0; i < sizeof(mpDigest_U32) / sizeof(mpDigest_U32[0]); i++)
  {
    result << std::hex << std::setfill('0') << std::setw(8);
    result << mpDigest_U32[i];
  }

  /* Reset for next run */
  S_Reset(mpDigest_U32, mBuffer_S, mTransform_U64);

  return result.str();
}

std::string BofSha1::S_FromFile(const std::string &_rFilename_S)
{
  std::ifstream Stream(_rFilename_S.c_str(), std::ios::binary);
  BofSha1 Checksum;
  Checksum.Update(Stream);
  return Checksum.Final();
}

std::string BofSha1::S_FromBuffer(uint32_t _Nb_U32, const void *_pBuffer)
{
  BofSha1 Checksum;
  Checksum.Update(_Nb_U32, _pBuffer);
  return Checksum.Final();
}

std::string BofSha1::S_FromString(const std::string &_Input_S)
{
  return S_FromBuffer(_Input_S.size(), _Input_S.c_str());
}

END_BOF_NAMESPACE()