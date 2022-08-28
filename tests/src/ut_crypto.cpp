/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the crypto class
 *
 * Name:        ut_crypto.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */

/*** Include files ***********************************************************/
#include <bofstd/bofstd.h>
/*** Include ****************************************************************/
#include <cstdint>
#include <gtest/gtest.h>
#include <bofstd/bofcrypto.h>

/*** Define *****************************************************************/

/*** Enum *******************************************************************/

/*** Struct *******************************************************************/

/* http://www.ietf.org/rfc/rfc1321.txt
 * MD5 test suite :
 * MD5("") = d41d8cd98f00b204e9800998ecf8427e
 * MD5("a") = 0cc175b9c0f1b6a831c399e269772661
 * MD5("abc") = 900150983cd24fb0d6963f7d28e17f72
 * MD5("message digest") = f96b697d7cb7938d525a2f31aaf161d0
 * MD5("abcdefghijklmnopqrstuvwxyz") = c3fcd3d76192e4007dfb496cca67e13b
 * MD5("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") =
 * d174ab98d277d9f5a5611c2c9f419d9f
 * MD5("123456789012345678901234567890123456789012345678901234567890123456
 * 78901234567890") = 57edf4a22be3c955ac49da2e2107b67a
 */
struct MD5_TEST_PATTERN_ENTRY
{
	const char    *pDataIn_c;
	uint8_t       pDataOut_U8[16];
};

/*** Const *******************************************************************/

static const MD5_TEST_PATTERN_ENTRY S_pMd5TestPatternTable_X[] =
{
	{ "",                                                                                 { 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e } },
	{ "a",                                                                                { 0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8, 0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61 } },
	{ "abc",                                                                              { 0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0, 0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72 } },
	{ "message digest",                                                                   { 0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d, 0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0 } },
	{ "abcdefghijklmnopqrstuvwxyz",                                                       { 0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00, 0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b } },
	{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",                   { 0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5, 0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f } },
	{ "12345678901234567890123456789012345678901234567890123456789012345678901234567890", { 0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55, 0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a } },
};

/*** Class definition *******************************************************/

class BofCrypto_Test: public::testing::TestWithParam< MD5_TEST_PATTERN_ENTRY >
{
public:
	BofCrypto_Test(): mpBofCryptoMd5_O(nullptr)
	{
	}

	// Per-test-case set-up. Called before the first test in this test case.
	static void SetUpTestCase() {}

	// Per-test-case tear-down. Called after the last test in this test case.
	static void TearDownTestCase() {}
protected:

	// You can define per-test set-up and tear-down logic as usual.
	virtual void                SetUp();
	virtual void                TearDown();

	BOF::BofCryptoMd5 *mpBofCryptoMd5_O;
};

USE_BOF_NAMESPACE()

/*** Global variables ********************************************************/

void BofCrypto_Test::SetUp()
{
	mpBofCryptoMd5_O = new BofCryptoMd5();
	EXPECT_TRUE(mpBofCryptoMd5_O != 0);
}


void BofCrypto_Test::TearDown()
{
	BOF_SAFE_DELETE(mpBofCryptoMd5_O);
	EXPECT_TRUE(mpBofCryptoMd5_O == 0);
}


INSTANTIATE_TEST_CASE_P(Md5TestPattern, BofCrypto_Test, ::testing::ValuesIn(S_pMd5TestPatternTable_X));
//INSTANTIATE_TEST_SUITE_P(Md5TestPattern, BofCrypto_Test, ::testing::ValuesIn(S_pMd5TestPatternTable_X));

TEST_P(BofCrypto_Test, HashOk)
{
	char                         pIn_c[256];
	uint8_t                      pOut_U8[16];
	uint32_t                     NbIn_U32, NbOut_U32;
	bool                         Sts_B, Equal_B;
	const MD5_TEST_PATTERN_ENTRY & TestEntry_X = GetParam();

 	strcpy(pIn_c, TestEntry_X.pDataIn_c);
	NbIn_U32  = static_cast<uint32_t>(strlen(pIn_c));
	NbOut_U32 = sizeof(pOut_U8);
	Sts_B     = mpBofCryptoMd5_O->Hash(NbIn_U32, (uint8_t *)pIn_c, &NbOut_U32, pOut_U8);
	EXPECT_TRUE(Sts_B);
	EXPECT_EQ(NbOut_U32, 16);
	Equal_B   = (memcmp(TestEntry_X.pDataOut_U8, pOut_U8, NbOut_U32) == 0);
	EXPECT_TRUE(Equal_B);
}

TEST_P(BofCrypto_Test, HashBad)
{
	char     pIn_c[256];
	uint8_t  pOut_U8[16];
	uint32_t NbIn_U32, NbOut_U32;
	bool     Sts_B;

	// const MD5TESTPATTERNENTRY &TestEntry_X = GetParam();

	strcpy(pIn_c, "");
	NbIn_U32  = sizeof(pIn_c);
	NbOut_U32 = sizeof(pOut_U8);
	Sts_B     = mpBofCryptoMd5_O->Hash(NbIn_U32, nullptr, &NbOut_U32, pOut_U8);
	EXPECT_FALSE(Sts_B);

	Sts_B     = mpBofCryptoMd5_O->Hash(NbIn_U32, (uint8_t *)pIn_c, nullptr, pOut_U8);
	EXPECT_FALSE(Sts_B);

	Sts_B     = mpBofCryptoMd5_O->Hash(NbIn_U32, (uint8_t *)pIn_c, &NbOut_U32, nullptr);
	EXPECT_FALSE(Sts_B);
}

//https://www.di-mgt.com.au/sha_testvectors.html
TEST(BofCrypto_Test, Sha1)
{
	EXPECT_STREQ(BofSha1::S_FromString("").c_str(), "da39a3ee5e6b4b0d3255bfef95601890afd80709");
	EXPECT_STREQ(BofSha1::S_FromString("abc").c_str(), "a9993e364706816aba3e25717850c26c9cd0d89d");
	EXPECT_STREQ(BofSha1::S_FromString("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq").c_str(), "84983e441c3bd26ebaae4aa1f95129e5e54670f1");
	EXPECT_STREQ(BofSha1::S_FromString("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu").c_str(), "a49b2446a02c645bf419f995b67091253a04a259");
	EXPECT_STREQ(BofSha1::S_FromString(std::string(1000000, 'a')).c_str(), "34aa973cd4c4daa4f61eeb2bdbad27316534016f");

	EXPECT_STREQ(BofSha1::S_FromBuffer(56, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq").c_str(), "84983e441c3bd26ebaae4aa1f95129e5e54670f1");

	EXPECT_STREQ(BofSha1::S_FromBuffer(0,   "").c_str(), "da39a3ee5e6b4b0d3255bfef95601890afd80709");
	EXPECT_STREQ(BofSha1::S_FromBuffer(3,   "abc").c_str(), "a9993e364706816aba3e25717850c26c9cd0d89d");
	EXPECT_STREQ(BofSha1::S_FromBuffer(56,  "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq").c_str(), "84983e441c3bd26ebaae4aa1f95129e5e54670f1");
	EXPECT_STREQ(BofSha1::S_FromBuffer(112, "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu").c_str(), "a49b2446a02c645bf419f995b67091253a04a259");

}