/*
 * Copyright (c) 2013-2033, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofpath class
 *
 * Name:        ut_path.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */

/*** Include files ***********************************************************/

#include "gtestrunner.h"
#include <bofstd/bofpath.h>
#include <bofstd/boffs.h>
#include <bofstd/bofstring.h>

/*** Class *************************************************************************************************************************/

USE_BOF_NAMESPACE()
#define FILENAME     "file"
#define EXTENSION    "bha"

TEST(Path_Test, PathConstructorDestructorWindows)
{
  std::string Pwd_S, PrevPwd_S;
	std::string::size_type SlashPrevDelimiterPos;

  EXPECT_EQ(Bof_GetCurrentDirectory(Pwd_S),BOF_ERR_NO_ERROR);
  Pwd_S = Bof_StringReplace(Pwd_S, "\\", '/');
	PrevPwd_S = "";
	if (Pwd_S.size() > 2)
	{
		SlashPrevDelimiterPos = Pwd_S.rfind('/', Pwd_S.size() - 2);
		if (SlashPrevDelimiterPos != std::string::npos)
		{
			PrevPwd_S = Pwd_S.substr(0, SlashPrevDelimiterPos + 1);
		}
	}

  BofPath Empty("");
  EXPECT_EQ(Empty.IsDirectory(), false);
  EXPECT_EQ(Empty.IsFile(), true);
  EXPECT_STREQ(Empty.FullPathName(false).c_str(), "");
  EXPECT_STREQ(Empty.DirectoryName(false).c_str(), "");
  EXPECT_STREQ(Empty.Extension().c_str(), "");
  EXPECT_STREQ(Empty.FileNameWithExtension().c_str(), "");
  EXPECT_STREQ(Empty.FileNameWithoutExtension().c_str(), "");

  BofPath a("./data/dir/");
  EXPECT_EQ(a.IsDirectory(), true);
  EXPECT_EQ(a.IsFile(), false);
  EXPECT_STREQ(a.FullPathName(false).c_str(), (Pwd_S+"data/dir/").c_str());
  EXPECT_STREQ(a.DirectoryName(false).c_str(), (Pwd_S + "data/dir/").c_str());
  EXPECT_STREQ(a.Extension().c_str(), "");
  EXPECT_STREQ(a.FileNameWithExtension().c_str(), "");
  EXPECT_STREQ(a.FileNameWithoutExtension().c_str(), "");

  BofPath b("./data/file");
  EXPECT_EQ(b.IsDirectory(), false);
  EXPECT_EQ(b.IsFile(), true);
  EXPECT_STREQ(b.FullPathName(false).c_str(), (Pwd_S + "data/file").c_str());
  EXPECT_STREQ(b.DirectoryName(false).c_str(), (Pwd_S + "data/").c_str());
  EXPECT_STREQ(b.Extension().c_str(), "");
  EXPECT_STREQ(b.FileNameWithExtension().c_str(), "file");
  EXPECT_STREQ(b.FileNameWithoutExtension().c_str(), "file");

	BofPath aa("data/dir/");
	EXPECT_EQ(aa.IsDirectory(), true);
	EXPECT_EQ(aa.IsFile(), false);
	EXPECT_STREQ(aa.FullPathName(false).c_str(), (Pwd_S + "data/dir/").c_str());
	EXPECT_STREQ(aa.DirectoryName(false).c_str(), (Pwd_S + "data/dir/").c_str());
	EXPECT_STREQ(aa.Extension().c_str(), "");
	EXPECT_STREQ(aa.FileNameWithExtension().c_str(), "");
	EXPECT_STREQ(aa.FileNameWithoutExtension().c_str(), "");

	BofPath bb("data/file");
	EXPECT_EQ(bb.IsDirectory(), false);
	EXPECT_EQ(bb.IsFile(), true);
	EXPECT_STREQ(bb.FullPathName(false).c_str(), (Pwd_S + "data/file").c_str());
	EXPECT_STREQ(bb.DirectoryName(false).c_str(), (Pwd_S + "data/").c_str());
	EXPECT_STREQ(bb.Extension().c_str(), "");
	EXPECT_STREQ(bb.FileNameWithExtension().c_str(), "file");
	EXPECT_STREQ(bb.FileNameWithoutExtension().c_str(), "file");


  BofPath d(".\\data\\dir\\");
  EXPECT_EQ(d.IsDirectory(), true);
  EXPECT_EQ(d.IsFile(), false);
  EXPECT_STREQ(d.FullPathName(false).c_str(), (Pwd_S + "data/dir/").c_str());
  EXPECT_STREQ(d.DirectoryName(false).c_str(), (Pwd_S + "data/dir/").c_str());
  EXPECT_STREQ(d.Extension().c_str(), "");
  EXPECT_STREQ(d.FileNameWithExtension().c_str(), "");
  EXPECT_STREQ(d.FileNameWithoutExtension().c_str(), "");

  BofPath e(".\\data\\file");
  EXPECT_EQ(e.IsDirectory(), false);
  EXPECT_EQ(e.IsFile(), true);
  EXPECT_STREQ(e.FullPathName(false).c_str(), (Pwd_S + "data/file").c_str());
  EXPECT_STREQ(e.DirectoryName(false).c_str(), (Pwd_S + "data/").c_str());
  EXPECT_STREQ(e.Extension().c_str(), "");
  EXPECT_STREQ(e.FileNameWithExtension().c_str(), "file");
  EXPECT_STREQ(e.FileNameWithoutExtension().c_str(), "file");

	BofPath f("../data/dir/");
	EXPECT_EQ(f.IsDirectory(), true);
	EXPECT_EQ(f.IsFile(), false);
	EXPECT_STREQ(f.FullPathName(false).c_str(), (PrevPwd_S + "data/dir/").c_str());
	EXPECT_STREQ(f.DirectoryName(false).c_str(), (PrevPwd_S + "data/dir/").c_str());
	EXPECT_STREQ(f.Extension().c_str(), "");
	EXPECT_STREQ(f.FileNameWithExtension().c_str(), "");
	EXPECT_STREQ(f.FileNameWithoutExtension().c_str(), "");

	if (PrevPwd_S.size() > 2)
	{
		SlashPrevDelimiterPos = PrevPwd_S.rfind('/', PrevPwd_S.size() - 2);
		if (SlashPrevDelimiterPos != std::string::npos)
		{
			PrevPwd_S = PrevPwd_S.substr(0, SlashPrevDelimiterPos + 1);
		}
		BofPath g("../../data/dir/");
		EXPECT_EQ(g.IsDirectory(), true);
		EXPECT_EQ(g.IsFile(), false);
		EXPECT_STREQ(g.FullPathName(false).c_str(), (PrevPwd_S + "data/dir/").c_str());
		EXPECT_STREQ(g.DirectoryName(false).c_str(), (PrevPwd_S + "data/dir/").c_str());
		EXPECT_STREQ(g.Extension().c_str(), "");
		EXPECT_STREQ(g.FileNameWithExtension().c_str(), "");
		EXPECT_STREQ(g.FileNameWithoutExtension().c_str(), "");
		if (PrevPwd_S.size() > 2)
		{
			SlashPrevDelimiterPos = PrevPwd_S.rfind('/', PrevPwd_S.size() - 2);
			if (SlashPrevDelimiterPos != std::string::npos)
			{
				PrevPwd_S = PrevPwd_S.substr(0, SlashPrevDelimiterPos + 1);
			}
			BofPath h("../../../data/dir/");
			EXPECT_EQ(h.IsDirectory(), true);
			EXPECT_EQ(h.IsFile(), false);
			EXPECT_STREQ(h.FullPathName(false).c_str(), (PrevPwd_S + "data/dir/").c_str());
			EXPECT_STREQ(h.DirectoryName(false).c_str(), (PrevPwd_S + "data/dir/").c_str());
			EXPECT_STREQ(h.Extension().c_str(), "");
			EXPECT_STREQ(h.FileNameWithExtension().c_str(), "");
			EXPECT_STREQ(h.FileNameWithoutExtension().c_str(), "");
		}
	}

	BofPath i(R"(Z:\a\b\c\d\e\f\g\file.abc)");
	EXPECT_EQ(i.IsDirectory(), false);
	EXPECT_EQ(i.IsFile(), true);
	EXPECT_STREQ(i.FullPathName(true).c_str(), R"(Z:\a\b\c\d\e\f\g\file.abc)");
	EXPECT_STREQ(i.DirectoryName(true).c_str(), R"(Z:\a\b\c\d\e\f\g\)");
	EXPECT_STREQ(i.Extension().c_str(), "abc");
	EXPECT_STREQ(i.FileNameWithExtension().c_str(), "file.abc");
	EXPECT_STREQ(i.FileNameWithoutExtension().c_str(), "file");

	i = BofPath (R"(Z:\a\b\c\d\e\f\g\..\file.abc)");
	EXPECT_STREQ(i.FullPathName(true).c_str(), R"(Z:\a\b\c\d\e\f\file.abc)");
	i = BofPath(R"(Z:\a\b\c\d\e\f\..\.\.\..\h\file.abc)");
	EXPECT_STREQ(i.FullPathName(true).c_str(), R"(Z:\a\b\c\d\h\file.abc)");
	i = BofPath(R"(Z:\a\b\c\d\e\f\..\F\..\G\h\file.abc)");
	EXPECT_STREQ(i.FullPathName(true).c_str(), R"(Z:\a\b\c\d\e\G\h\file.abc)");


	BofPath j(R"(Z:\a\b\c\d\e\f\g\dir\)");
	EXPECT_EQ(j.IsDirectory(), true);
	EXPECT_EQ(j.IsFile(), false);
	EXPECT_STREQ(j.FullPathName(true).c_str(), R"(Z:\a\b\c\d\e\f\g\dir\)");
	EXPECT_STREQ(j.DirectoryName(true).c_str(), R"(Z:\a\b\c\d\e\f\g\dir\)");
	EXPECT_STREQ(j.Extension().c_str(), "");
	EXPECT_STREQ(j.FileNameWithExtension().c_str(), "");
	EXPECT_STREQ(j.FileNameWithoutExtension().c_str(), "");
	/*
	BofPath i("data/dir/./../hello.");
	printf("i %s %s\n", Pwd_S.c_str(), i.FullPathName(false).c_str());
	BofPath j("data/dir/../../abc");
	printf("j %s %s\n", Pwd_S.c_str(), j.FullPathName(false).c_str());

	BofPath k("data/../dir/");
	printf("k %s %s\n", Pwd_S.c_str(), k.FullPathName(false).c_str());
	BofPath l("data/../../dir/");
	printf("l %s %s\n", Pwd_S.c_str(), l.FullPathName(false).c_str());
	*/

	BofPath Path0("C:\\");
	EXPECT_EQ(Path0.IsValid(), true);
#if defined (_WIN32)
	EXPECT_EQ(Path0.IsExist(), true);
#else
	EXPECT_EQ(Path0.IsExist(), false);
#endif
	EXPECT_EQ(Path0.IsDirectory(), true);
	EXPECT_EQ(Path0.IsFile(), false);
	EXPECT_STREQ(Path0.FullPathName(false).c_str(), "C:/");
	EXPECT_STREQ(Path0.FullPathName(true).c_str(), "C:\\");
	EXPECT_STREQ(Path0.DirectoryName(false).c_str(), "C:/");
	EXPECT_STREQ(Path0.DirectoryName(true).c_str(), "C:\\");
	EXPECT_STREQ(Path0.Extension().c_str(), "");
	EXPECT_STREQ(Path0.FileNameWithExtension().c_str(), "");
	EXPECT_STREQ(Path0.FileNameWithoutExtension().c_str(), "");

	BofPath Path1("C:\\", ".");
	EXPECT_EQ(Path1.IsValid(), true);
#if defined (_WIN32)
	EXPECT_EQ(Path0.IsExist(), true);
#else
	EXPECT_EQ(Path0.IsExist(), false);
#endif
	EXPECT_EQ(Path1.IsDirectory(), true);
	EXPECT_EQ(Path1.IsFile(), false);
	EXPECT_STREQ(Path1.FullPathName(false).c_str(), "C:/");
	EXPECT_STREQ(Path1.FullPathName(true).c_str(), "C:\\");
	EXPECT_STREQ(Path1.DirectoryName(false).c_str(), "C:/");
	EXPECT_STREQ(Path1.DirectoryName(true).c_str(), "C:\\");
	EXPECT_STREQ(Path1.Extension().c_str(), "");
	EXPECT_STREQ(Path1.FileNameWithExtension().c_str(), "");
	EXPECT_STREQ(Path1.FileNameWithoutExtension().c_str(), "");

	BofPath Path2(Path1);
	EXPECT_EQ(Path2.IsValid(), true);
#if defined (_WIN32)
	EXPECT_EQ(Path0.IsExist(), true);
#else
	EXPECT_EQ(Path0.IsExist(), false);
#endif
	EXPECT_EQ(Path2.IsDirectory(), true);
	EXPECT_EQ(Path2.IsFile(), false);
	EXPECT_STREQ(Path2.FullPathName(false).c_str(), "C:/");
	EXPECT_STREQ(Path2.FullPathName(true).c_str(), "C:\\");
	EXPECT_STREQ(Path2.DirectoryName(false).c_str(), "C:/");
	EXPECT_STREQ(Path2.DirectoryName(true).c_str(), "C:\\");
	EXPECT_STREQ(Path2.Extension().c_str(), "");
	EXPECT_STREQ(Path2.FileNameWithExtension().c_str(), "");
	EXPECT_STREQ(Path2.FileNameWithoutExtension().c_str(), "");

	BofPath Path3 = Path1;
	EXPECT_EQ(Path3.IsValid(), true);
#if defined (_WIN32)
	EXPECT_EQ(Path0.IsExist(), true);
#else
	EXPECT_EQ(Path0.IsExist(), false);
#endif
	EXPECT_EQ(Path3.IsDirectory(), true);
	EXPECT_EQ(Path3.IsFile(), false);
	EXPECT_STREQ(Path3.FullPathName(false).c_str(), "C:/");
	EXPECT_STREQ(Path3.FullPathName(true).c_str(), "C:\\");
	EXPECT_STREQ(Path3.DirectoryName(false).c_str(), "C:/");
	EXPECT_STREQ(Path3.DirectoryName(true).c_str(), "C:\\");
	EXPECT_STREQ(Path3.Extension().c_str(), "");
	EXPECT_STREQ(Path3.FileNameWithExtension().c_str(), "");
	EXPECT_STREQ(Path3.FileNameWithoutExtension().c_str(), "");

	BofPath Path4(std::move(Path3) );
	EXPECT_EQ(Path4.IsValid(), true);
#if defined (_WIN32)
	EXPECT_EQ(Path0.IsExist(), true);
#else
	EXPECT_EQ(Path0.IsExist(), false);
#endif
	EXPECT_EQ(Path4.IsDirectory(), true);
	EXPECT_EQ(Path4.IsFile(), false);
	EXPECT_STREQ(Path4.FullPathName(false).c_str(), "C:/");
	EXPECT_STREQ(Path4.FullPathName(true).c_str(), "C:\\");
	EXPECT_STREQ(Path4.DirectoryName(false).c_str(), "C:/");
	EXPECT_STREQ(Path4.DirectoryName(true).c_str(), "C:\\");
	EXPECT_STREQ(Path4.Extension().c_str(), "");
	EXPECT_STREQ(Path4.FileNameWithExtension().c_str(), "");
	EXPECT_STREQ(Path4.FileNameWithoutExtension().c_str(), "");

	BofPath Path5("");
	EXPECT_EQ(Path5.IsValid(), true);
#if defined (_WIN32)
	EXPECT_EQ(Path0.IsExist(), true);
#else
	EXPECT_EQ(Path0.IsExist(), false);
#endif
	EXPECT_EQ(Path5.IsDirectory(), false);
	EXPECT_EQ(Path5.IsFile(), true);
	EXPECT_STREQ(Path5.FullPathName(false).c_str(), "");
	EXPECT_STREQ(Path5.FullPathName(true).c_str(), "");
	EXPECT_STREQ(Path5.DirectoryName(false).c_str(), "");
	EXPECT_STREQ(Path5.DirectoryName(true).c_str(), "");
	EXPECT_STREQ(Path5.Extension().c_str(), "");
	EXPECT_STREQ(Path5.FileNameWithExtension().c_str(), "");
	EXPECT_STREQ(Path5.FileNameWithoutExtension().c_str(), "");
}

TEST(Path_Test, PathConstructorDestructorLinux)
{
	BofPath Path0("/");
	EXPECT_EQ(Path0.IsValid(), true);
	EXPECT_EQ(Path0.IsExist(), true);
	EXPECT_EQ(Path0.IsDirectory(), true);
	EXPECT_EQ(Path0.IsFile(), false);
	EXPECT_STREQ(Path0.FullPathName(false).c_str(), "/");
	EXPECT_STREQ(Path0.FullPathName(true).c_str(), "\\");
	EXPECT_STREQ(Path0.DirectoryName(false).c_str(), "/");
	EXPECT_STREQ(Path0.DirectoryName(true).c_str(), "\\");
	EXPECT_STREQ(Path0.Extension().c_str(), "");
	EXPECT_STREQ(Path0.FileNameWithExtension().c_str(), "");
	EXPECT_STREQ(Path0.FileNameWithoutExtension().c_str(), "");

	BofPath Path1("/", ".");
	EXPECT_EQ(Path1.IsValid(), true);
	EXPECT_EQ(Path0.IsExist(), true);
	EXPECT_EQ(Path1.IsDirectory(), true);
	EXPECT_EQ(Path1.IsFile(), false);
	EXPECT_STREQ(Path1.FullPathName(false).c_str(), "/");
	EXPECT_STREQ(Path1.FullPathName(true).c_str(), "\\");
	EXPECT_STREQ(Path1.DirectoryName(false).c_str(), "/");
	EXPECT_STREQ(Path1.DirectoryName(true).c_str(), "\\");
	EXPECT_STREQ(Path1.Extension().c_str(), "");
	EXPECT_STREQ(Path1.FileNameWithExtension().c_str(), "");
	EXPECT_STREQ(Path1.FileNameWithoutExtension().c_str(), "");

	BofPath Path2(Path1);
	EXPECT_EQ(Path2.IsValid(), true);
	EXPECT_EQ(Path0.IsExist(), true);
	EXPECT_EQ(Path2.IsDirectory(), true);
	EXPECT_EQ(Path2.IsFile(), false);
	EXPECT_STREQ(Path2.FullPathName(false).c_str(), "/");
	EXPECT_STREQ(Path2.FullPathName(true).c_str(), "\\");
	EXPECT_STREQ(Path2.DirectoryName(false).c_str(), "/");
	EXPECT_STREQ(Path2.DirectoryName(true).c_str(), "\\");
	EXPECT_STREQ(Path2.Extension().c_str(), "");
	EXPECT_STREQ(Path2.FileNameWithExtension().c_str(), "");
	EXPECT_STREQ(Path2.FileNameWithoutExtension().c_str(), "");

	BofPath Path3 = Path1;
	EXPECT_EQ(Path3.IsValid(), true);
	EXPECT_EQ(Path0.IsExist(), true);
	EXPECT_EQ(Path3.IsDirectory(), true);
	EXPECT_EQ(Path3.IsFile(), false);
	EXPECT_STREQ(Path3.FullPathName(false).c_str(), "/");
	EXPECT_STREQ(Path3.FullPathName(true).c_str(), "\\");
	EXPECT_STREQ(Path3.DirectoryName(false).c_str(), "/");
	EXPECT_STREQ(Path3.DirectoryName(true).c_str(), "\\");
	EXPECT_STREQ(Path3.Extension().c_str(), "");
	EXPECT_STREQ(Path3.FileNameWithExtension().c_str(), "");
	EXPECT_STREQ(Path3.FileNameWithoutExtension().c_str(), "");

	BofPath Path4(std::move(Path3) );
	EXPECT_EQ(Path4.IsValid(), true);
	EXPECT_EQ(Path0.IsExist(), true);
	EXPECT_EQ(Path4.IsDirectory(), true);
	EXPECT_EQ(Path4.IsFile(), false);
	EXPECT_STREQ(Path4.FullPathName(false).c_str(), "/");
	EXPECT_STREQ(Path4.FullPathName(true).c_str(), "\\");
	EXPECT_STREQ(Path4.DirectoryName(false).c_str(), "/");
	EXPECT_STREQ(Path4.DirectoryName(true).c_str(), "\\");
	EXPECT_STREQ(Path4.Extension().c_str(), "");
	EXPECT_STREQ(Path4.FileNameWithExtension().c_str(), "");
	EXPECT_STREQ(Path4.FileNameWithoutExtension().c_str(), "");

}

TEST(Path_Test, PathParsing)
{
	BOFERR Sts_E;
	BofPath Path("C:\\file");
	EXPECT_EQ(Path.IsValid(), true);
	EXPECT_EQ(Path.IsExist(), false);
	EXPECT_EQ(Path.IsDirectory(), false);
	EXPECT_EQ(Path.IsFile(), true);
	EXPECT_STREQ(Path.FullPathName(false).c_str(), "C:/file");
	EXPECT_STREQ(Path.FullPathName(true).c_str(), "C:\\file");
	EXPECT_STREQ(Path.DirectoryName(false).c_str(), "C:/");
	EXPECT_STREQ(Path.DirectoryName(true).c_str(), "C:\\");
	EXPECT_STREQ(Path.Extension().c_str(), "");
	EXPECT_STREQ(Path.FileNameWithExtension().c_str(), "file");
	EXPECT_STREQ(Path.FileNameWithoutExtension().c_str(), "file");

	Path  = "C:\\file.";
	EXPECT_EQ(Path.IsValid(), true);
	EXPECT_EQ(Path.IsExist(), false);
	EXPECT_EQ(Path.IsDirectory(), false);
	EXPECT_EQ(Path.IsFile(), true);
	EXPECT_STREQ(Path.FullPathName(false).c_str(), "C:/file");
	EXPECT_STREQ(Path.FullPathName(true).c_str(), "C:\\file");
	EXPECT_STREQ(Path.DirectoryName(false).c_str(), "C:/");
	EXPECT_STREQ(Path.DirectoryName(true).c_str(), "C:\\");
	EXPECT_STREQ(Path.Extension().c_str(), "");
	EXPECT_STREQ(Path.FileNameWithExtension().c_str(), "file");
	EXPECT_STREQ(Path.FileNameWithoutExtension().c_str(), "file");

	Path  = "C:\\file.bha";
	EXPECT_EQ(Path.IsValid(), true);
	EXPECT_EQ(Path.IsExist(), false);
	EXPECT_EQ(Path.IsDirectory(), false);
	EXPECT_EQ(Path.IsFile(), true);
	EXPECT_STREQ(Path.FullPathName(false).c_str(), "C:/file.bha");
	EXPECT_STREQ(Path.FullPathName(true).c_str(), "C:\\file.bha");
	EXPECT_STREQ(Path.DirectoryName(false).c_str(), "C:/");
	EXPECT_STREQ(Path.DirectoryName(true).c_str(), "C:\\");
	EXPECT_STREQ(Path.Extension().c_str(), "bha");
	EXPECT_STREQ(Path.FileNameWithExtension().c_str(), "file.bha");
	EXPECT_STREQ(Path.FileNameWithoutExtension().c_str(), "file");

	Path  = "C:\\file.bha.txt";
	EXPECT_EQ(Path.IsValid(), true);
	EXPECT_EQ(Path.IsExist(), false);
	EXPECT_EQ(Path.IsDirectory(), false);
	EXPECT_EQ(Path.IsFile(), true);
	EXPECT_STREQ(Path.FullPathName(false).c_str(), "C:/file.bha.txt");
	EXPECT_STREQ(Path.FullPathName(true).c_str(), "C:\\file.bha.txt");
	EXPECT_STREQ(Path.DirectoryName(false).c_str(), "C:/");
	EXPECT_STREQ(Path.DirectoryName(true).c_str(), "C:\\");
	EXPECT_STREQ(Path.Extension().c_str(), "txt");
	EXPECT_STREQ(Path.FileNameWithExtension().c_str(), "file.bha.txt");
	EXPECT_STREQ(Path.FileNameWithoutExtension().c_str(), "file.bha");

	Path  = "C:\\file.bha/hello.world..txt";
	EXPECT_EQ(Path.IsValid(), true);
	EXPECT_EQ(Path.IsExist(), false);
	EXPECT_EQ(Path.IsDirectory(), false);
	EXPECT_EQ(Path.IsFile(), true);
	EXPECT_STREQ(Path.FullPathName(false).c_str(), "C:/file.bha/hello.world..txt");
	EXPECT_STREQ(Path.FullPathName(true).c_str(), "C:\\file.bha\\hello.world..txt");
	EXPECT_STREQ(Path.DirectoryName(false).c_str(), "C:/file.bha/");
	EXPECT_STREQ(Path.DirectoryName(true).c_str(), "C:\\file.bha\\");
	EXPECT_STREQ(Path.Extension().c_str(), "txt");
	EXPECT_STREQ(Path.FileNameWithExtension().c_str(), "hello.world..txt");
	EXPECT_STREQ(Path.FileNameWithoutExtension().c_str(), "hello.world.");

	Path  = "C:/tmp.azer/./A/././B.bha";
	EXPECT_EQ(Path.IsValid(), true);    // Cf windows

	Path  = "C:/tmp/./A/././B/C/";
	EXPECT_EQ(Path.IsValid(), true);

	Path  = "C:/tmp/././.";
	EXPECT_EQ(Path.IsValid(), true);

	Path  = "C:/tmp/././././../..";
	EXPECT_EQ(Path.IsValid(), false);   

	Path  = "C:/\\";
	EXPECT_EQ(Path.IsValid(), false);

	Path  = "C:/\\/";
	EXPECT_EQ(Path.IsValid(), false);

	Path  = "C:/\\//";
	EXPECT_EQ(Path.IsValid(), false);

	Path  = "C:/tmp//.";
	EXPECT_EQ(Path.IsValid(), false);

	Path  = "C:/tmp/////.";
	EXPECT_EQ(Path.IsValid(), false);

	Path  = "C:/tmp//A/B/C//D";
	EXPECT_EQ(Path.IsValid(), false);

	Path  = "C:/tmp//A/B/C//D//";
	EXPECT_EQ(Path.IsValid(), false);

	BofPath Path5("C:\\", "..");
	EXPECT_EQ(Path5.IsValid(), false);

	BofPath Path6(nullptr);
	EXPECT_EQ(Path6.IsValid(), false);

	Path  = nullptr;
	EXPECT_EQ(Path.IsValid(), false);

	Path  = "C:/tmp/../../../..";
	EXPECT_EQ(Path.IsValid(), false);

	Path  = "C:/tmp/.../...././...........";
	EXPECT_EQ(Path.IsValid(), false);

	Path  = "C:/tmp/.../a";
	EXPECT_EQ(Path.IsValid(), false);

	Sts_E=Path.CurrentDirectoryName("C:\\tmp\\bha\\1/2/3");
	EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

	Path = "./babar";
	EXPECT_EQ(Path.IsValid(), true);

	Sts_E=Path.CurrentDirectoryName("C:\\tmp\\bha\\1/2/3/");
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	Path = "./babar";
	EXPECT_EQ(Path.IsValid(), true);

	Sts_E = Path.CurrentDirectoryName("C:\\tmp\\bha\\1/2/3/../../");
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	Path  = "..";
	EXPECT_EQ(Path.IsValid(), true);

	Sts_E = Path.CurrentDirectoryName("C:\\tmp\\bha\\1/2/3/4/5/");
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	Path  = "../babar";
	EXPECT_EQ(Path.IsValid(), true);

	Path  = "";
	EXPECT_EQ(Path.IsValid(), true);

	Path  = "\t";
	EXPECT_EQ(Path.IsValid(), false);

	Path  = "C:/tmp/../..";
	EXPECT_EQ(Path.IsValid(), false);

	Path  = "C:/tmp/../../file.txt";
	EXPECT_EQ(Path.IsValid(), false);

	Sts_E = Path.CurrentDirectoryName("C:\\tmp\\bha\\1/2/3/././");
	EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

	Path  = "CD/tmp/";
	EXPECT_EQ(Path.IsValid(), true);

	Path  = "C:Ktmp/";
	EXPECT_EQ(Path.IsValid(), false);

	Path  = "@:/tmp/";
	EXPECT_EQ(Path.IsValid(), false);

	BofPath Path7("/A\\B\\c/D");
	BofPath Path8 = Path7;

	EXPECT_TRUE(Path7 == Path8);
	EXPECT_FALSE(Path7 != Path8);

	Path7 = "/A/B/c/D/";
	Path8 = "/A/B/c/D\\";

	EXPECT_TRUE(Path7 == Path8);
	EXPECT_FALSE(Path7 != Path8);

	Path7 = "/A/B/C/D/";
	Path8 = "A/B/c/D\\";

	EXPECT_TRUE(Path7 != Path8);
	EXPECT_FALSE(Path7 == Path8);
}

