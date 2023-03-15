/*
 * Copyright (c) 2000-2020, Onbings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the string component of the bofstd library
 *
 * Name:        ut_string.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:				 onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  November 15 2016  BHA : Initial release
 */
#include <bofstd/bofstring.h>

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

TEST(String_Test, MultiByteToWideChar)
{
	size_t  Len;
	wchar_t pWideCharStr_wc[0x100];

	Len = Bof_MultiByteToWideChar("babar", 0, pWideCharStr_wc, "C");
	EXPECT_EQ(Len, 0);

	Len = Bof_MultiByteToWideChar("babar", BOF_NB_ELEM_IN_ARRAY(pWideCharStr_wc), nullptr, "");
	EXPECT_EQ(Len, 5);

	Len = Bof_MultiByteToWideChar("babar", 0, nullptr, "C");
	EXPECT_EQ(Len, 5);

	Len = Bof_MultiByteToWideChar("babar", BOF_NB_ELEM_IN_ARRAY(pWideCharStr_wc), pWideCharStr_wc, nullptr);
	EXPECT_EQ(Len, 5);
	EXPECT_STREQ(pWideCharStr_wc, L"babar");

	Len = Bof_MultiByteToWideChar("babar", BOF_NB_ELEM_IN_ARRAY(pWideCharStr_wc), pWideCharStr_wc, "");
	EXPECT_EQ(Len, 5);
	EXPECT_STREQ(pWideCharStr_wc, L"babar");

	Len = Bof_MultiByteToWideChar("babar", BOF_NB_ELEM_IN_ARRAY(pWideCharStr_wc), pWideCharStr_wc, "");
	EXPECT_EQ(Len, 5);
	EXPECT_STREQ(pWideCharStr_wc, L"babar");

	// Not working in windows Len = Bof_MultiByteToWideChar(u8"z\u00df\u6c34\U0001f34c", BOF_NB_ELEM_IN_ARRAY(pWideCharStr_wc), pWideCharStr_wc);
}

TEST(String_Test, WideCharToMultiByte)
{
	size_t Len;
	char   pMultiByteStr_c[0x100];
	std::wstring Str_WS;
	std::string Str_S;

	Len = Bof_WideCharToMultiByte(L"babar", 0, pMultiByteStr_c, "");
	EXPECT_EQ(Len, 0);

	Len = Bof_WideCharToMultiByte(L"babar", BOF_NB_ELEM_IN_ARRAY(pMultiByteStr_c), nullptr, "");
	EXPECT_EQ(Len, 5);

	Len = Bof_WideCharToMultiByte(L"babar", 0, nullptr, "");
	EXPECT_EQ(Len, 5);

	Len = Bof_WideCharToMultiByte(L"babar", BOF_NB_ELEM_IN_ARRAY(pMultiByteStr_c), pMultiByteStr_c, nullptr);
	EXPECT_EQ(Len, 5);
	EXPECT_STREQ(pMultiByteStr_c, "babar");

	Len = Bof_WideCharToMultiByte(L"babar", BOF_NB_ELEM_IN_ARRAY(pMultiByteStr_c), pMultiByteStr_c, "");
	EXPECT_EQ(Len, 5);
	EXPECT_STREQ(pMultiByteStr_c, "babar");

	Len = Bof_WideCharToMultiByte(L"babar", BOF_NB_ELEM_IN_ARRAY(pMultiByteStr_c), pMultiByteStr_c, "");
	EXPECT_EQ(Len, 5);
	EXPECT_STREQ(pMultiByteStr_c, "babar");


// Not working in windows	Len = Bof_WideCharToMultiByte(L"z\u00df\u6c34\U0001d10b", BOF_NB_ELEM_IN_ARRAY(pMultiByteStr_c), pMultiByteStr_c, "");

	Str_S = "Hello World !\n";
	Str_WS = Bof_Utf8ToUtf16(Str_S);
	//EXPECT_STREQ(Str_S.c_str(), Str_WS.c_str());

	Str_WS = L"Hello World !\n";
	Str_S = Bof_Utf16ToUtf8(Str_WS);
	//EXPECT_STREQ(Str_S.c_str(), Str_WS.c_str());
//From https://en.wikipedia.org/wiki/Windows-1252
//	Str_WS = L"¼½¾¿";
//	Str_S = Bof_Utf16ToUtf8(Str_WS);
	Str_WS = L"\u0141";
	Str_S = Bof_Utf16ToUtf8(Str_WS);

}

TEST(String_Test, StringTrim)
{
	// pgm_error_t *e;
	// pgm_init(&e);

// Left trim
	EXPECT_EQ(Bof_StringLeftTrim("  A", nullptr), "  A");
	EXPECT_EQ(Bof_StringLeftTrim(""), "");
	EXPECT_EQ(Bof_StringLeftTrim("      "), "");
	EXPECT_EQ(Bof_StringLeftTrim("  AB  "), "AB  ");
	EXPECT_EQ(Bof_StringLeftTrim(" \r\n\t AB  \n"), "AB  \n");
	EXPECT_EQ(Bof_StringLeftTrim("\r\n\t AB  \n"), "AB  \n");
	EXPECT_EQ(Bof_StringLeftTrim("ABCDEFGHIJ\n"), "ABCDEFGHIJ\n");
	EXPECT_EQ(Bof_StringLeftTrim("  AB  ", " \a\f\n\r\t\v"), "AB  ");
	EXPECT_EQ(Bof_StringLeftTrim(" \r\n\t AB  \n", "\n\r"), " \r\n\t AB  \n");
	EXPECT_EQ(Bof_StringLeftTrim("\r\n\t AB  \n", "\n\r"), "\t AB  \n");
	EXPECT_EQ(Bof_StringLeftTrim("ABCDEFGHIJ\n", "DCBA\n"), "EFGHIJ\n");

// Right trim
	EXPECT_EQ(Bof_StringRightTrim("  A  ", nullptr), "  A  ");
	EXPECT_EQ(Bof_StringRightTrim(""), "");
	EXPECT_EQ(Bof_StringRightTrim("      "), "");
	EXPECT_EQ(Bof_StringRightTrim("  AB  "), "  AB");
	EXPECT_EQ(Bof_StringRightTrim(" \r\n\t AB  \n"), " \r\n\t AB");
	EXPECT_EQ(Bof_StringRightTrim("\r\n\t AB  \t\n"), "\r\n\t AB");
	EXPECT_EQ(Bof_StringRightTrim("ABCDE\tFGHIJ\n"), "ABCDE\tFGHIJ");
	EXPECT_EQ(Bof_StringRightTrim("  AB  ", " \a\f\n\r\t\v"), "  AB");
	EXPECT_EQ(Bof_StringRightTrim(" \r\n\t AB  \n", " \n\r"), " \r\n\t AB");
	EXPECT_EQ(Bof_StringRightTrim("\r\n\t AB  \t\n", "\n\r"), "\r\n\t AB  \t");
	EXPECT_EQ(Bof_StringRightTrim("ABCDE\tFGHIJ\n", "JIH\n"), "ABCDE\tFG");

// Trim both
	EXPECT_EQ(Bof_StringTrim("  A  ", nullptr), "  A  ");
	EXPECT_EQ(Bof_StringTrim(""), "");
	EXPECT_EQ(Bof_StringTrim("      "), "");
	EXPECT_EQ(Bof_StringTrim("  AB  "), "AB");
	EXPECT_EQ(Bof_StringTrim(" \r\n\t AB  \n"), "AB");
	EXPECT_EQ(Bof_StringTrim("\r\n\t AB  \t\n"), "AB");
	EXPECT_EQ(Bof_StringTrim("ABCDE\tFGHIJ\n"), "ABCDE\tFGHIJ");
	EXPECT_EQ(Bof_StringTrim("  AB  ", " \a\f\n\r\t\v"), "AB");
	EXPECT_EQ(Bof_StringTrim(" \r\n\t AB  \n", " \n\r"), "\t AB");
	EXPECT_EQ(Bof_StringTrim("\r\n\t AB  \t\n", "\n\r"), "\t AB  \t");
	EXPECT_EQ(Bof_StringTrim("ABCDE\tFGHIJ\n", "BAIJ\n"), "CDE\tFGH");

}

TEST(String_Test, StringRemove)
{
	EXPECT_EQ(Bof_StringRemove("  AB  ", " \a\f\n\r\t\v"), "AB");
	EXPECT_EQ(Bof_StringRemove("  A  ", "\a\f\n\r\t\v"), "  A  ");
	EXPECT_EQ(Bof_StringRemove("", " \a\f\n\r\t\v"), "");
	EXPECT_EQ(Bof_StringRemove("      ", " \a\f\n\r\t\v"), "");
	EXPECT_EQ(Bof_StringRemove("  AB  ", " \a\f\n\r\t\v"), "AB");
	EXPECT_EQ(Bof_StringRemove(" \r\n\t AB  \n", " \a\f\n\r\t\v"), "AB");
	EXPECT_EQ(Bof_StringRemove("\r\n\t AB  \t\n", " \a\f\n\r\t\v"), "AB");
	EXPECT_EQ(Bof_StringRemove("AB\nCDE\tFGH\f  \fIJ\n", " \a\f\n\r\t\v"), "ABCDEFGHIJ");
	EXPECT_EQ(Bof_StringRemove("  AB  ", " \a\f\n\r\t\v"), "AB");
	EXPECT_EQ(Bof_StringRemove(" \r\n\t A\n\t\rB  \n", " \n\r"), "\tA\tB");
	EXPECT_EQ(Bof_StringRemove("\r\n\t A\nB  \t\n", "\n\r"), "\t AB  \t");
	EXPECT_EQ(Bof_StringRemove("ABICDE\tFGHJJIJ\n", "BAIJ\n"), "CDE\tFGH");
}
TEST(String_Test, StringReplace)
{
	std::string Txt_S;

	EXPECT_EQ(Bof_StringReplace("  AB  ", " \a\f\n\r\t\v", ' '), "  AB  ");
	EXPECT_EQ(Bof_StringReplace("  A  ", " \a\f\n\r\t\v", ' '), "  A  ");
	EXPECT_EQ(Bof_StringReplace("", " \a\f\n\r\t\v", ' '), "");
	EXPECT_EQ(Bof_StringReplace("      ", " \a\f\n\r\t\v", '?'), "??????");
	EXPECT_EQ(Bof_StringReplace("  AB  ", " \a\f\n\r\t\v", '?'), "??AB??");
	EXPECT_EQ(Bof_StringReplace("  AB  ", " \a\f\n\r\t\v", ' '), "  AB  ");
	EXPECT_EQ(Bof_StringReplace("\r\nAB\n\r", " \a\f\n\r\t\v", '\r'), "\r\rAB\r\r");
	EXPECT_EQ(Bof_StringReplace(" \r\n\t AB  \n", " \a\f\n\r\t\v", '!'), "!!!!!AB!!!");
	EXPECT_EQ(Bof_StringReplace("\r\n\t AB  \t\n", " \a\f\n\r\t\v", 'Z'), "ZZZZABZZZZ");
	Txt_S = Bof_StringReplace("\r\n\t AB  \t\n", " \a\f\n\r\t\v", '\0');
	EXPECT_EQ(Txt_S.size(), 10);         // "\0\0\0\0AB\0\0\0\0");
	EXPECT_STREQ(Txt_S.c_str(), "");     // "\0\0\0\0AB\0\0\0\0");

	EXPECT_EQ(Bof_StringReplace("AB\nCDE\tFGH\f  \fIJ\n", " \a\f\n\r\t\v", '4'), "AB4CDE4FGH4444IJ4");
	EXPECT_EQ(Bof_StringReplace("  AB  ", " \a\f\n\r\t\v", '7'), "77AB77");
	EXPECT_EQ(Bof_StringReplace(" \r\n\t A\n\t\rB  \n", " \n\r", '.'), "...\t.A.\t.B...");
	EXPECT_EQ(Bof_StringReplace("\r\n\t A\nB  \t\n", "\n\r", '/'), "//\t A/B  \t/");
	EXPECT_EQ(Bof_StringReplace("ABICDE\tFGHJJIJ\n", "BAIJ\n", '^'), "^^^CDE\tFGH^^^^^");
}
TEST(String_Test, StringIsPresent)
{
	EXPECT_TRUE(Bof_StringIsPresent("/t<mp/babar.txt", "<>:\"\\|?*") );
	EXPECT_TRUE(Bof_StringIsPresent("/tmp/ba>bar.txt", "<>:\"\\|?*") );
	EXPECT_TRUE(Bof_StringIsPresent("/tmp/ba:bar.txt", "<>:\"\\|?*") );
	EXPECT_TRUE(Bof_StringIsPresent("/tmp/ba\"bar.txt", "<>:\"\\|?*") );
	EXPECT_TRUE(Bof_StringIsPresent("/tmp/babar.tx\\t", "<>:\"\\|?*") );
	EXPECT_TRUE(Bof_StringIsPresent("/tmp/babar.tx|t", "<>:\"\\|?*") );
	EXPECT_TRUE(Bof_StringIsPresent("/tmp/babar?txt", "<>:\"\\|?*") );
	EXPECT_TRUE(Bof_StringIsPresent("*tmp/babar.txt", "<>:\"\\|?*") );
	EXPECT_FALSE(Bof_StringIsPresent("/tmp/babar.txt", "<>:\"\\|?*") );
}

TEST(String_Test, StringIsAllTheSameChar)
{
	EXPECT_FALSE(Bof_StringIsAllTheSameChar("......A", '.') );
	EXPECT_FALSE(Bof_StringIsAllTheSameChar("..A....", '.') );
	EXPECT_FALSE(Bof_StringIsAllTheSameChar("A......A", '.') );
	EXPECT_FALSE(Bof_StringIsAllTheSameChar("", '.') );
	EXPECT_TRUE(Bof_StringIsAllTheSameChar(".", '.') );
	EXPECT_TRUE(Bof_StringIsAllTheSameChar(".....................", '.') );
}

TEST(String_Test, StringSplit)
{
	std::vector< std::string > SplittedStr_S;

	SplittedStr_S = Bof_StringSplit("  AB  ", " \a\f\n\r\t\v");
	EXPECT_EQ(SplittedStr_S.size(), 5);
	EXPECT_EQ(SplittedStr_S[0], "");
	EXPECT_EQ(SplittedStr_S[1], "");
	EXPECT_EQ(SplittedStr_S[2], "AB");
	EXPECT_EQ(SplittedStr_S[3], "");
	EXPECT_EQ(SplittedStr_S[4], "");
	EXPECT_EQ(Bof_StringJoin(SplittedStr_S, "@"), "@@AB@@");

	SplittedStr_S = Bof_StringSplit("  AB  ", "\a\f\n\r\t\v");
	EXPECT_EQ(SplittedStr_S.size(), 1);
	EXPECT_EQ(SplittedStr_S[0], "  AB  ");
	EXPECT_EQ(Bof_StringJoin(SplittedStr_S, "@"), "  AB  ");

	SplittedStr_S = Bof_StringSplit("", " \a\f\n\r\t\v");
	EXPECT_EQ(SplittedStr_S.size(), 1);
	EXPECT_EQ(SplittedStr_S[0], "");
	EXPECT_EQ(Bof_StringJoin(SplittedStr_S, "@"), "");

	SplittedStr_S = Bof_StringSplit("   ", " \a\f\n\r\t\v");
	EXPECT_EQ(SplittedStr_S.size(), 4);
	EXPECT_EQ(SplittedStr_S[0], "");
	EXPECT_EQ(SplittedStr_S[1], "");
	EXPECT_EQ(SplittedStr_S[2], "");
	EXPECT_EQ(SplittedStr_S[3], "");
	EXPECT_EQ(Bof_StringJoin(SplittedStr_S, "@"), "@@@");

	SplittedStr_S = Bof_StringSplit("/", "/");
	EXPECT_EQ(SplittedStr_S.size(), 2);
	EXPECT_EQ(SplittedStr_S[0], "");
	EXPECT_EQ(SplittedStr_S[1], "");
	EXPECT_EQ(Bof_StringJoin(SplittedStr_S, "/"), "/");

	SplittedStr_S = Bof_StringSplit("  /A/B  ", "/");
	EXPECT_EQ(SplittedStr_S.size(), 3);
	EXPECT_EQ(SplittedStr_S[0], "  ");
	EXPECT_EQ(SplittedStr_S[1], "A");
	EXPECT_EQ(SplittedStr_S[2], "B  ");
	EXPECT_EQ(Bof_StringJoin(SplittedStr_S, "/"), "  /A/B  ");

	SplittedStr_S = Bof_StringSplit("/  A/B  ", "/");
	EXPECT_EQ(SplittedStr_S.size(), 3);
	EXPECT_EQ(SplittedStr_S[0], "");
	EXPECT_EQ(SplittedStr_S[1], "  A");
	EXPECT_EQ(SplittedStr_S[2], "B  ");
	EXPECT_EQ(Bof_StringJoin(SplittedStr_S, "/"), "/  A/B  ");

	SplittedStr_S = Bof_StringSplit("/  A/B  ", " ");
	EXPECT_EQ(SplittedStr_S.size(), 5);
	EXPECT_EQ(SplittedStr_S[0], "/");
	EXPECT_EQ(SplittedStr_S[1], "");
	EXPECT_EQ(SplittedStr_S[2], "A/B");
	EXPECT_EQ(SplittedStr_S[3], "");
	EXPECT_EQ(SplittedStr_S[4], "");
	EXPECT_EQ(Bof_StringJoin(SplittedStr_S, "/"), "///A/B//");

	SplittedStr_S = Bof_StringSplit("\r\nAB\n\r", "\r");
	EXPECT_EQ(SplittedStr_S.size(), 3);
	EXPECT_EQ(SplittedStr_S[0], "");
	EXPECT_EQ(SplittedStr_S[1], "\nAB\n");
	EXPECT_EQ(SplittedStr_S[2], "");
	EXPECT_EQ(Bof_StringJoin(SplittedStr_S, "/"), "/\nAB\n/");

	SplittedStr_S = Bof_StringSplit(" \r\n\t AB  \n", "\t");
	EXPECT_EQ(SplittedStr_S.size(), 2);
	EXPECT_EQ(SplittedStr_S[0], " \r\n");
	EXPECT_EQ(SplittedStr_S[1], " AB  \n");
	EXPECT_EQ(Bof_StringJoin(SplittedStr_S, "/"), " \r\n/ AB  \n");

	SplittedStr_S = Bof_StringSplit("\r\n\t AB  \t\n", "\n\r");
	EXPECT_EQ(SplittedStr_S.size(), 4);
	EXPECT_EQ(SplittedStr_S[0], "");
	EXPECT_EQ(SplittedStr_S[1], "");
	EXPECT_EQ(SplittedStr_S[2], "\t AB  \t");
	EXPECT_EQ(SplittedStr_S[3], "");
	EXPECT_EQ(Bof_StringJoin(SplittedStr_S, "/"), "//\t AB  \t/");

	SplittedStr_S = Bof_StringSplit("\r\n\t AB  \t\n", "\0");
	EXPECT_EQ(SplittedStr_S.size(), 1);
	EXPECT_EQ(SplittedStr_S[0], "\r\n\t AB  \t\n");
	EXPECT_EQ(Bof_StringJoin(SplittedStr_S, "/"), "\r\n\t AB  \t\n");

	SplittedStr_S = Bof_StringSplit("/A/B/C/D/", "/");
	EXPECT_EQ(SplittedStr_S.size(), 6);
	EXPECT_EQ(SplittedStr_S[0], "");
	EXPECT_EQ(SplittedStr_S[1], "A");
	EXPECT_EQ(SplittedStr_S[2], "B");
	EXPECT_EQ(SplittedStr_S[3], "C");
	EXPECT_EQ(SplittedStr_S[4], "D");
	EXPECT_EQ(SplittedStr_S[5], "");
	EXPECT_EQ(Bof_StringJoin(SplittedStr_S, "/"), "/A/B/C/D/");
}

TEST(String_Test, StringJoin)
{
// std::string Bof_StringJoin(const std::vector<std::string> &_rStr_S, const char *_pCharDelimiter_c);
	std::vector< std::string > StrToJoin_S;

	StrToJoin_S.clear();
	EXPECT_EQ(Bof_StringJoin(StrToJoin_S, "/"), "");

	StrToJoin_S.clear();
	StrToJoin_S.push_back("");
	EXPECT_EQ(Bof_StringJoin(StrToJoin_S, "/"), "");

	StrToJoin_S.clear();
	StrToJoin_S.push_back("A");
	EXPECT_EQ(Bof_StringJoin(StrToJoin_S, "/"), "A");

	StrToJoin_S.clear();
	StrToJoin_S.push_back("A");
	StrToJoin_S.push_back("B");
	StrToJoin_S.push_back("C");
	EXPECT_EQ(Bof_StringJoin(StrToJoin_S, "/"), "A/B/C");

	StrToJoin_S.clear();
	StrToJoin_S.push_back("A");
	StrToJoin_S.push_back("B");
	StrToJoin_S.push_back("C");
	EXPECT_EQ(Bof_StringJoin(StrToJoin_S, "/\\r\n"), "A/\\r\nB/\\r\nC");


	StrToJoin_S.clear();
	StrToJoin_S.push_back("/");
	EXPECT_EQ(Bof_StringJoin(StrToJoin_S, "/"), "/");

	StrToJoin_S.clear();
	StrToJoin_S.push_back("/");
	StrToJoin_S.push_back("/");
	EXPECT_EQ(Bof_StringJoin(StrToJoin_S, "/"), "///");

	StrToJoin_S.clear();
	StrToJoin_S.push_back("  ");
	StrToJoin_S.push_back("A");
	StrToJoin_S.push_back("B  ");
	EXPECT_EQ(Bof_StringJoin(StrToJoin_S, "/"), "  /A/B  ");

	StrToJoin_S.clear();
	StrToJoin_S.push_back("  A");
	StrToJoin_S.push_back("B  ");
	EXPECT_EQ(Bof_StringJoin(StrToJoin_S, "/"), "  A/B  ");

	StrToJoin_S.clear();
	StrToJoin_S.push_back("/");
	StrToJoin_S.push_back("A/B");
	EXPECT_EQ(Bof_StringJoin(StrToJoin_S, "/"), "//A/B");

	StrToJoin_S.clear();
	StrToJoin_S.push_back("");
	StrToJoin_S.push_back("A");
	StrToJoin_S.push_back("B");
	StrToJoin_S.push_back("C");
	StrToJoin_S.push_back("D");
	StrToJoin_S.push_back("");
	EXPECT_EQ(Bof_StringJoin(StrToJoin_S, "/"), "/A/B/C/D/");

}


TEST(String_Test, Bof_RemoveDuplicateSuccessiveCharacter)
{
	std::string Str_S;
	Str_S = "AABCABBBBBBDEFFF";
	Bof_RemoveDuplicateSuccessiveCharacter(Str_S, 0);
	EXPECT_STREQ("ABCABDEF", Str_S.c_str());
	Str_S = "AABCABBBBBBDEFFF";
	Bof_RemoveDuplicateSuccessiveCharacter(Str_S, 'A');
	EXPECT_STREQ("ABCABBBBBBDEFFF", Str_S.c_str());

}

TEST(String_Test, MultipleKeyValueString)
{
	uint32_t Val_U32;
	int32_t Val_S32;
	std::string Val_S;

	EXPECT_EQ(Bof_GetUnsignedIntegerFromMultipleKeyValueString("AA=;\r\nZB=-2;\nYC=;", ";\r\n", "ZB", '=', Val_U32), 0);
	EXPECT_EQ(Val_U32, 0xFFFFFFFE);

	EXPECT_EQ(Bof_GetIntegerFromMultipleKeyValueString("AA=;\r\nZB=-2;\nYC=;", ";\r\n", "ZB", '=', Val_S32), 0);
	EXPECT_EQ(Val_S32, -2);

	EXPECT_EQ(Bof_GetStringFromMultipleKeyValueString("AA=;\r\nZB=+-2;\nYC=;", ";\r\n", "ZB", '=', Val_S), 0);
	EXPECT_STREQ(Val_S.c_str(), "+-2");

	EXPECT_EQ(Bof_GetUnsignedIntegerFromMultipleKeyValueString("AA=1;\r\nBB=2;\nCC=3;\n\rDD=", ";\r\n", "AA", '=', Val_U32), 0);
	EXPECT_EQ(Val_U32, 1);

	EXPECT_EQ(Bof_GetUnsignedIntegerFromMultipleKeyValueString("AA=1;\r\nBB=2;\nCC=3;\n\rDD=", ";\r\n", "BB", '=', Val_U32), 0);
	EXPECT_EQ(Val_U32, 2);

	EXPECT_EQ(Bof_GetUnsignedIntegerFromMultipleKeyValueString("AA=1;\r\nBB=2;\nCC=3;\n\rDD=", ";\r\n", "CC", '=', Val_U32), 0);
	EXPECT_EQ(Val_U32, 3);

	EXPECT_EQ(Bof_GetUnsignedIntegerFromMultipleKeyValueString("AA=1;\r\nBB=2;\nCC=3;\n\rDD=", ";\r\n", "DD", '=', Val_U32), 0);
	EXPECT_EQ(Val_U32, 0);

	EXPECT_EQ(Bof_GetUnsignedIntegerFromMultipleKeyValueString("TO=100;CON=HELLO WORLD", ";", "TO", '=', Val_U32), 0);
	EXPECT_EQ(Val_U32, 100);

	EXPECT_EQ(Bof_GetStringFromMultipleKeyValueString("TO=100;CON=HELLO WORLD", ";", "CON", '=', Val_S), 0);
	EXPECT_STREQ(Val_S.c_str(), "HELLO WORLD");

}
TEST(String_Test, Bof_StrNCpy)
{
	char pSrc_c[128], pDst_c[128];
	int Len_i;

	strcpy(pDst_c, "Abcd");
	strcpy(pSrc_c, "efgH");
	EXPECT_TRUE(Bof_StrNCpy(nullptr, pSrc_c, 1)==nullptr);
	EXPECT_EQ(pDst_c[0], 'A');
	EXPECT_TRUE(Bof_StrNCpy(pDst_c, nullptr, 1) == pDst_c);
	EXPECT_EQ(pDst_c[0], 'A');
	EXPECT_TRUE(Bof_StrNCpy(pDst_c, pSrc_c, 0) == pDst_c);
	EXPECT_EQ(pDst_c[0], 'A');

	Len_i = static_cast<int>(strlen(pSrc_c));
	EXPECT_TRUE(Bof_StrNCpy(pDst_c, pSrc_c, Len_i) == pDst_c);
	EXPECT_EQ(pDst_c[0], 'e');
	EXPECT_EQ(pDst_c[Len_i - 2], 'g');
	EXPECT_EQ(pDst_c[Len_i -1], 0);

	strcpy(pDst_c, "Abcd");
	strcpy(pSrc_c, "efgH");
	EXPECT_TRUE(Bof_StrNCpy(pDst_c, pSrc_c, 2) == pDst_c);
	EXPECT_EQ(pDst_c[0], 'e');
	EXPECT_EQ(pDst_c[1], 0);

	strcpy(pDst_c, "Abcd");
	strcpy(pSrc_c, "efgH");
	EXPECT_TRUE(Bof_StrNCpy(pDst_c, pSrc_c, 1) == pDst_c);
	EXPECT_EQ(pDst_c[0], 0);


	strcpy(pDst_c, "Abcdefgh");
	strcpy(pSrc_c, "1234");
	Len_i = static_cast<int>(strlen(pSrc_c)+1);
	EXPECT_TRUE(Bof_StrNCpy(pDst_c, pSrc_c, Len_i) == pDst_c);
	EXPECT_TRUE(memcmp(pDst_c,pSrc_c, Len_i)==0);

	strcpy(pDst_c, "Abcdefgh");
	strcpy(pSrc_c, "1234");	
	Len_i = static_cast<int>(strlen(pDst_c)+1);
	EXPECT_TRUE(Bof_StrNCpy(pDst_c, pSrc_c, Len_i) == pDst_c);
	Len_i = static_cast<int>(strlen(pSrc_c));
	EXPECT_TRUE(memcmp(pDst_c, pSrc_c, Len_i + 1) == 0);

	strcpy(pDst_c, "1234");
	strcpy(pSrc_c, "Abcdefgh");
	Len_i = static_cast<int>(strlen(pSrc_c) + 1);
	EXPECT_TRUE(Bof_StrNCpy(pDst_c, pSrc_c, Len_i) == pDst_c);
	EXPECT_TRUE(memcmp(pDst_c, pSrc_c, Len_i) == 0);

	strcpy(pDst_c, "1234");
	strcpy(pSrc_c, "Abcdefgh");
	Len_i = static_cast<int>(strlen(pDst_c) + 1);
	EXPECT_TRUE(Bof_StrNCpy(pDst_c, pSrc_c, Len_i) == pDst_c);
	EXPECT_TRUE(memcmp(pDst_c, pSrc_c, Len_i-1) == 0);
	EXPECT_EQ(pDst_c[Len_i-1], 0);
}

TEST(String_Test, UpperLower)
{
	EXPECT_STREQ(Bof_StringToLower("AbCdEf").c_str(), "abcdef");
	EXPECT_STREQ(Bof_StringToUpper("AbCdEf").c_str(), "ABCDEF");
}