/*
 * Copyright (c) 2013-2033, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the boffs class
 *
 * Name:        ut_fs.cpp
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
#include "bofstd/bofstring.h"
#include <bofstd/boffs.h>

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

TEST(Fs_Test, DirectoryManagement)
{
  BOFERR Sts_E;
  std::string CrtDir_S, NewDir_S;
  BOF_FILE_PERMISSION Permission_E;
  BofPath CrtDir, NewPath, FileAsDirPath;
  uintptr_t Io, Io2;
  std::string NewFull_S;
  BOF_FILE_TYPE FileType_E;
  uint64_t Size_U64;

  Sts_E = Bof_GetCurrentDirectory(CrtDir);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STRNE(CrtDir.FullPathName(false).c_str(), "");
  EXPECT_TRUE(CrtDir.FullPathName(false).back() == '/');

  FileAsDirPath = CrtDir.FullPathName(false) + "babar1";
  Permission_E = BOF_FILE_PERMISSION_READ_FOR_ALL | BOF_FILE_PERMISSION_WRITE_FOR_ALL;
  Size_U64 = Bof_GetFileSize(FileAsDirPath);
  Sts_E = Bof_DeleteFile(FileAsDirPath);
  EXPECT_TRUE((Size_U64 == 0xFFFFFFFFFFFFFFFF) || (Size_U64 == 0x7FFFFFFFFFFFFFFF)); // Linux 0x7FFFFFFFFFFFFFFF Win 0xFFFFFFFFFFFFFFFF ???

  Sts_E = Bof_CleanupDirectory(true, BofPath(FileAsDirPath.FullPathName(false) + "/"), true);
  Sts_E = Bof_CleanupDirectory(true, BofPath(FileAsDirPath.FullPathName(false) + "/../babar2/"), true);

  FileType_E = Bof_GetFileType(CrtDir);
  EXPECT_EQ(FileType_E, BOF_FILE_TYPE::BOF_FILE_DIR);

  NewFull_S = CrtDir.FullPathName(false) + "babar1/";

  NewPath = CrtDir;
  Sts_E = NewPath.Combine("/babar1/"); // Remove double //
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  NewPath = CrtDir;
  Sts_E = NewPath.Combine("babar1/");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  EXPECT_EQ(NewPath.IsValid(), true);
  EXPECT_EQ(NewPath.IsExist(), false);
  EXPECT_EQ(NewPath.IsDirectory(), true);
  EXPECT_EQ(NewPath.IsFile(), false);
  EXPECT_STREQ(NewPath.FullPathName(false).c_str(), NewFull_S.c_str());
  EXPECT_STREQ(NewPath.DirectoryName(true, false).c_str(), NewFull_S.c_str());
  EXPECT_STREQ(NewPath.Extension().c_str(), "");
  EXPECT_STREQ(NewPath.FileNameWithExtension().c_str(), "");
  EXPECT_STREQ(NewPath.FileNameWithoutExtension().c_str(), "");

  Sts_E = Bof_CreateFile(Permission_E, FileAsDirPath, false, Io);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileAsDirPath.IsValid(), true);
  EXPECT_TRUE(FileAsDirPath.IsExist());
  EXPECT_TRUE(FileAsDirPath.IsFile());
  EXPECT_FALSE(FileAsDirPath.IsDirectory());

  Sts_E = Bof_CloseFile(Io);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Io, -1);

  FileType_E = Bof_GetFileType("FileAsDirPath123456789");
  EXPECT_EQ(FileType_E, BOF_FILE_TYPE::BOF_FILE_DONT_EXIST);
  FileType_E = Bof_GetFileType(FileAsDirPath);
  EXPECT_EQ(FileType_E, BOF_FILE_TYPE::BOF_FILE_REG);
  FileType_E = Bof_GetFileType(CrtDir);
  EXPECT_EQ(FileType_E, BOF_FILE_TYPE::BOF_FILE_DIR);

  Sts_E = Bof_CreateFile(Permission_E, FileAsDirPath, true, Io2);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileAsDirPath.IsValid(), true);
  EXPECT_TRUE(FileAsDirPath.IsExist());
  EXPECT_TRUE(FileAsDirPath.IsFile());
  EXPECT_FALSE(FileAsDirPath.IsDirectory());

  Sts_E = Bof_CloseFile(Io2);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Io2, -1);

  Sts_E = Bof_CreateFile(Permission_E, FileAsDirPath, true, Io);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileAsDirPath.IsValid(), true);
  EXPECT_TRUE(FileAsDirPath.IsExist());
  EXPECT_TRUE(FileAsDirPath.IsFile());
  EXPECT_FALSE(FileAsDirPath.IsDirectory());

  Sts_E = Bof_CreateFile(Permission_E, FileAsDirPath, true, Io2);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileAsDirPath.IsValid(), true);
  EXPECT_TRUE(FileAsDirPath.IsExist());
  EXPECT_TRUE(FileAsDirPath.IsFile());
  EXPECT_FALSE(FileAsDirPath.IsDirectory());

  Sts_E = Bof_CloseFile(Io);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Io, -1);

  Sts_E = Bof_CloseFile(Io2);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Io2, -1);

  Sts_E = Bof_CreateDirectory(Permission_E, NewPath);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_DeleteFile(FileAsDirPath); // To be sure
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CreateDirectory(Permission_E, NewPath);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CreateDirectory(Permission_E, NewPath);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_GetFsPermission(NewPath, Permission_E);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  EXPECT_TRUE((Permission_E & BOF_FILE_PERMISSION_READ_FOR_ALL) == BOF_FILE_PERMISSION_READ_FOR_ALL);
  EXPECT_TRUE((Permission_E & BOF_FILE_PERMISSION_WRITE_FOR_ALL) == BOF_FILE_PERMISSION_WRITE_FOR_ALL);
#if defined(_WIN32)
  EXPECT_TRUE((Permission_E & BOF_FILE_PERMISSION_EXEC_FOR_ALL) == BOF_FILE_PERMISSION_EXEC_FOR_ALL);
#else
  EXPECT_FALSE((Permission_E & BOF_FILE_PERMISSION_EXEC_FOR_ALL) == BOF_FILE_PERMISSION_EXEC_FOR_ALL);
#endif
  NewPath = CrtDir;
  Sts_E = NewPath.Combine("babar2/babar3/../babar4/../babar5/");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CreateDirectory(Permission_E, NewPath);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  NewPath = CrtDir;
  Sts_E = NewPath.Combine("babar1");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CleanupDirectory(true, NewPath, true);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  NewPath = CrtDir;
  Sts_E = NewPath.Combine("babar1/");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CleanupDirectory(true, NewPath, true);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  NewPath = CrtDir;
  Sts_E = NewPath.Combine("babar2/babar5/");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CleanupDirectory(true, NewPath, false);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CleanupDirectory(true, NewPath, true);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  NewPath = CrtDir;
  Sts_E = NewPath.Combine("babar2/");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CleanupDirectory(true, NewPath, true);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

TEST(Fs_Test, FileManagement)
{
  BOFERR Sts_E;
  std::string CrtDir_S, NewDir_S;
  BOF_FILE_PERMISSION Permission_E;
  BofPath CrtDir, NewPath, FileAsDirPath;
  uintptr_t Io, Io2;
  std::string NewFull_S;
  uint64_t Size_U64;
  BOF_FILE_TYPE FileType_E;

  Sts_E = Bof_GetCurrentDirectory(CrtDir);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STRNE(CrtDir.FullPathName(false).c_str(), "");
  EXPECT_TRUE(CrtDir.FullPathName(false).back() == '/');

  FileAsDirPath = CrtDir.FullPathName(false) + "babar1";
  Permission_E = BOF_FILE_PERMISSION_READ_FOR_ALL | BOF_FILE_PERMISSION_WRITE_FOR_ALL;
  // To be sure
  Sts_E = Bof_DeleteFile(FileAsDirPath);
  Sts_E = Bof_CleanupDirectory(true, BofPath(FileAsDirPath.FullPathName(false) + "/"), true);
  Sts_E = Bof_CleanupDirectory(true, BofPath(FileAsDirPath.FullPathName(false) + "/../babar2/"), true);

  FileType_E = Bof_GetFileType(CrtDir);
  EXPECT_EQ(FileType_E, BOF_FILE_TYPE::BOF_FILE_DIR);

  NewFull_S = CrtDir.FullPathName(false) + "babar1/";

  NewPath = CrtDir;
  Sts_E = NewPath.Combine("babar1/");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(NewPath.IsValid(), true);
  EXPECT_EQ(NewPath.IsExist(), false);
  EXPECT_EQ(NewPath.IsDirectory(), true);
  EXPECT_EQ(NewPath.IsFile(), false);
  EXPECT_STREQ(NewPath.FullPathName(false).c_str(), NewFull_S.c_str());
  EXPECT_STREQ(NewPath.DirectoryName(true, false).c_str(), NewFull_S.c_str());
  EXPECT_STREQ(NewPath.Extension().c_str(), "");
  EXPECT_STREQ(NewPath.FileNameWithExtension().c_str(), "");
  EXPECT_STREQ(NewPath.FileNameWithoutExtension().c_str(), "");

  Sts_E = Bof_CreateFile(Permission_E, FileAsDirPath, false, Io);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileAsDirPath.IsValid(), true);
  EXPECT_TRUE(FileAsDirPath.IsExist());
  EXPECT_TRUE(FileAsDirPath.IsFile());
  EXPECT_FALSE(FileAsDirPath.IsDirectory());
  Size_U64 = Bof_GetFileSize(FileAsDirPath);
  EXPECT_EQ(Size_U64, 0);

  Sts_E = Bof_CloseFile(Io);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Io, -1);

  Sts_E = Bof_CreateFile(Permission_E, FileAsDirPath, true, Io2);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileAsDirPath.IsValid(), true);
  EXPECT_TRUE(FileAsDirPath.IsExist());
  EXPECT_TRUE(FileAsDirPath.IsFile());
  EXPECT_FALSE(FileAsDirPath.IsDirectory());

  Sts_E = Bof_CloseFile(Io2);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Io2, -1);

  Sts_E = Bof_CreateFile(Permission_E, FileAsDirPath, true, Io);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileAsDirPath.IsValid(), true);
  EXPECT_TRUE(FileAsDirPath.IsExist());
  EXPECT_TRUE(FileAsDirPath.IsFile());
  EXPECT_FALSE(FileAsDirPath.IsDirectory());

  Sts_E = Bof_CreateFile(Permission_E, FileAsDirPath, true, Io2);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileAsDirPath.IsValid(), true);
  EXPECT_TRUE(FileAsDirPath.IsExist());
  EXPECT_TRUE(FileAsDirPath.IsFile());
  EXPECT_FALSE(FileAsDirPath.IsDirectory());

  Sts_E = Bof_CloseFile(Io);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Io, -1);

  Sts_E = Bof_CloseFile(Io2);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(Io2, -1);

  Sts_E = Bof_CreateDirectory(Permission_E, NewPath);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_DeleteFile(FileAsDirPath); // To be sure
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CreateDirectory(Permission_E, NewPath);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CreateDirectory(Permission_E, NewPath);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_GetFsPermission(NewPath, Permission_E);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  EXPECT_TRUE((Permission_E & BOF_FILE_PERMISSION_READ_FOR_ALL) == BOF_FILE_PERMISSION_READ_FOR_ALL);
  EXPECT_TRUE((Permission_E & BOF_FILE_PERMISSION_WRITE_FOR_ALL) == BOF_FILE_PERMISSION_WRITE_FOR_ALL);
#if defined(_WIN32)
  EXPECT_TRUE((Permission_E & BOF_FILE_PERMISSION_EXEC_FOR_ALL) == BOF_FILE_PERMISSION_EXEC_FOR_ALL);
#else
  EXPECT_FALSE((Permission_E & BOF_FILE_PERMISSION_EXEC_FOR_ALL) == BOF_FILE_PERMISSION_EXEC_FOR_ALL);
#endif

  NewPath = CrtDir;
  Sts_E = NewPath.Combine("babar2/babar3/../babar4/../babar5/");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Size_U64 = Bof_GetFileSize(NewPath);
  EXPECT_NE(Size_U64, 0);

  Sts_E = Bof_CreateDirectory(Permission_E, NewPath);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  NewPath = CrtDir;
  Sts_E = NewPath.Combine("babar1");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CleanupDirectory(true, NewPath, true);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  NewPath = CrtDir;
  Sts_E = NewPath.Combine("babar1/");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CleanupDirectory(true, NewPath, true);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  NewPath = CrtDir;
  Sts_E = NewPath.Combine("babar2/babar5");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CleanupDirectory(true, NewPath, true);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  NewPath = CrtDir;
  Sts_E = NewPath.Combine("babar2/");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CleanupDirectory(true, NewPath, true);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

TEST(Fs_Test, EntireFile)
{
  BOFERR Sts_E;
  BOF_FILE_PERMISSION Permission_E;
  std::string Line_S, LineRead_S;
  BofPath Dir, Path;

  Permission_E = BOF_FILE_PERMISSION_READ_FOR_ALL | BOF_FILE_PERMISSION_WRITE_FOR_ALL;
  Line_S = "Hello World";

  Sts_E = Bof_GetCurrentDirectory(Dir);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Path.CurrentDirectoryName(Dir.FullPathName(false));
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Path = "BhaFile.txt";

  EXPECT_EQ(Bof_WriteLine(Permission_E, Path, false, Line_S), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Bof_WriteLine(Permission_E, Path, true, Line_S), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Bof_ReadLine(Path, LineRead_S), BOF_ERR_NO_ERROR);
  Line_S = Line_S + Line_S;
  EXPECT_STREQ(LineRead_S.c_str(), Line_S.c_str());
}
TEST(Fs_Test, FileLayout)
{
  BOFERR Sts_E;
  BOF_FILE_PERMISSION Permission_E;
  // Permission_E  = BOF_FILE_PERMISSION_READ_FOR_ALL | BOF_FILE_PERMISSION_WRITE_FOR_ALL;
  BofPath CrtDir, DirLayoutRoot, Dir, File, NewFile;
  uint32_t i_U32, j_U32, k_U32, Nb_U32;
  uintptr_t Io;
  std::string Line_S, LineRead_S;
  uint64_t Pos_U64, GetPos_U64, NewPos_U64, Size_U64, Size2_U64;

  Sts_E = Bof_GetCurrentDirectory(CrtDir);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  DirLayoutRoot = CrtDir;
  Sts_E = DirLayoutRoot.Combine("TstRoot/");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_CleanupDirectory(true, DirLayoutRoot, true);

  Permission_E = BOF_FILE_PERMISSION_READ_FOR_ALL | BOF_FILE_PERMISSION_WRITE_FOR_ALL | BOF_FILE_PERMISSION_EXEC_FOR_ALL;
  Sts_E = Bof_CreateDirectory(Permission_E, DirLayoutRoot);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_ChangeCurrentDirectory(DirLayoutRoot);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_GetCurrentDirectory(Dir);
  EXPECT_TRUE(DirLayoutRoot == Dir);

  for (i_U32 = 0; i_U32 < 10; i_U32++)
  {
    Dir = DirLayoutRoot;
    Sts_E = Dir.Combine("SubDir_" + std::to_string(i_U32) + "/Level1/Level2/");
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

    Sts_E = Bof_CreateDirectory(Permission_E, Dir);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

    if (i_U32 < 8)
    {
      for (j_U32 = 0; j_U32 < 8; j_U32++)
      {
        File = Bof_Sprintf("%sFile_%06d.%d", Dir.FullPathName(false).c_str(), j_U32, (j_U32 % 2) ? 1 : 2);
        Sts_E = Bof_CreateFile(Permission_E, File, false, Io);
        EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

        Pos_U64 = 0;
        for (k_U32 = 0; k_U32 < 100; k_U32++)
        {
          Line_S = Bof_Sprintf("This is line %06d%s", k_U32, Bof_Eol());
          Nb_U32 = static_cast<uint32_t>(Line_S.size());
          Sts_E = Bof_WriteFile(Io, Nb_U32, reinterpret_cast<const uint8_t *>(Line_S.c_str()));
          EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
          EXPECT_EQ(Nb_U32, Line_S.size());
          Pos_U64 += Nb_U32;
          GetPos_U64 = Bof_GetFileIoPosition(Io);
          EXPECT_EQ(GetPos_U64, Pos_U64);
        }
        Sts_E = Bof_CloseFile(Io);
        EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
        EXPECT_EQ(Io, -1);
        Size_U64 = Bof_GetFileSize(File);
        EXPECT_NE(Size_U64, 0);

        NewFile = Bof_Sprintf("%sFile_%06d.%d.ren", Dir.FullPathName(false).c_str(), j_U32, (j_U32 % 2) ? 1 : 2);
        Sts_E = Bof_RenameFile(File, NewFile);
        EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

        Sts_E = Bof_CopyFile(true, NewFile, File);
        EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

        Size2_U64 = Bof_GetFileSize(NewFile);
        EXPECT_EQ(Size_U64, Size2_U64);

        Sts_E = Bof_DeleteFile(NewFile);
        EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

        Sts_E = Bof_OpenFile(File, true, false, Io);
        EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

        Pos_U64 = 0;
        for (k_U32 = 0; k_U32 < 100; k_U32 += 2)
        {
          NewPos_U64 = Bof_SetFileIoPosition(Io, Pos_U64, BOF_SEEK_METHOD::BOF_SEEK_BEGIN);
          EXPECT_EQ(NewPos_U64, Pos_U64);
          Line_S = Bof_Sprintf("This is line %06d%s", k_U32, Bof_Eol());
          Nb_U32 = static_cast<uint32_t>(Line_S.size());
          Sts_E = Bof_ReadLine(Io, LineRead_S);
          EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
          EXPECT_EQ(Nb_U32, Line_S.size());
          EXPECT_STREQ(Line_S.c_str(), LineRead_S.c_str());
          Pos_U64 += Nb_U32;
          Pos_U64 += Nb_U32;
        }
        Sts_E = Bof_CloseFile(Io);
      }
    }
  }
  Sts_E = Bof_ChangeCurrentDirectory(CrtDir);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

// Must be after FileLayout
TEST(Fs_Test, DirEnum)
{
  BOFERR Sts_E;
  BofPath CrtDir, DirLayoutRoot, Path;
  BOF_FILE_FOUND FileFound_X;
  std::vector<BOF_FILE_FOUND> FileCollection;
  uint32_t i_U32;
  BofDateTime DateTime;

  Sts_E = Bof_GetCurrentDirectory(CrtDir);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  DirLayoutRoot = CrtDir;
  Sts_E = DirLayoutRoot.Combine("TstRoot/");
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  //  FileCollection.clear();
  //  Sts_E = Bof_FindFile("C:\\bld\\evs-muse\\resources\\", "*", BOF::BOF_FILE_TYPE::BOF_FILE_DIR, false, FileCollection);

  FileCollection.clear();
  Sts_E = Bof_FindFile(DirLayoutRoot, "*.*", BOF_FILE_TYPE::BOF_FILE_ALL, true, 0xFFFFFFFF, FileCollection);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileCollection.size(), 94); // 64 file and 30 dir
  for (i_U32 = 0; i_U32 < FileCollection.size(); i_U32++)
  {
    //		Sts_E = Bof_TimeInSecSince1970_To_BofDateTime(FileCollection[i_U32].LastAccess_X, DateTime_X);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    //		printf("Size %06lld Time %s Dir %s Path %s%s", FileCollection[i_U32].Size_U64, Bof_FormatDateTime(DateTime_X).c_str(), FileCollection[i_U32].Path.IsDirectory() ? "true " : "false", FileCollection[i_U32].Path.FullPathName(false).c_str(),
    //Bof_Eol());
  }
  FileCollection.clear();
  Sts_E = Bof_FindFile(DirLayoutRoot, "*.*", BOF_FILE_TYPE::BOF_FILE_ALL, false, 0xFFFFFFFF, FileCollection);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileCollection.size(), 10);
  for (i_U32 = 0; i_U32 < FileCollection.size(); i_U32++)
  {
    //  Sts_E = Bof_TimeInSecSince1970_To_BofDateTime(FileCollection[i_U32].LastAccess_X, DateTime_X);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    //		printf("Size %06lld Time %s Dir %s Path %s%s", FileCollection[i_U32].Size_U64, Bof_FormatDateTime(DateTime_X).c_str(), FileCollection[i_U32].Path.IsDirectory() ? "true " : "false", FileCollection[i_U32].Path.FullPathName(false).c_str(),
    //Bof_Eol());
  }
  FileCollection.clear();
  Sts_E = Bof_FindFile(DirLayoutRoot, "*.1", BOF_FILE_TYPE::BOF_FILE_DIR, true, 0xFFFFFFFF, FileCollection);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileCollection.size(), 30);
  FileCollection.clear();
  Sts_E = Bof_FindFile(DirLayoutRoot, "*.1", BOF_FILE_TYPE::BOF_FILE_ALL, true, 0xFFFFFFFF, FileCollection);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileCollection.size(), 62); // 32 file and 30 dir
  FileCollection.clear();
  Sts_E = Bof_FindFile(DirLayoutRoot, "*.2", BOF_FILE_TYPE::BOF_FILE_REG, true, 0xFFFFFFFF, FileCollection);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileCollection.size(), 32);

  FileCollection.clear();
  Sts_E = Bof_FindFile(DirLayoutRoot, "*.*", BOF_FILE_TYPE::BOF_FILE_DIR, false, 0xFFFFFFFF, FileCollection);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileCollection.size(), 10);

  FileCollection.clear();
  Sts_E = Bof_FindFile(DirLayoutRoot, "*.*", BOF_FILE_TYPE::BOF_FILE_REG, true, 0xFFFFFFFF, FileCollection);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileCollection.size(), 64); // not 80 as the two last dir are empty

  FileCollection.clear();
  Sts_E = Bof_FindFile(DirLayoutRoot, "*.*", BOF_FILE_TYPE::BOF_FILE_ALL, false, 0xFFFFFFFF, FileCollection);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(FileCollection.size(), 10);

  FileCollection.clear();
  Sts_E = Bof_FindFile(DirLayoutRoot, "*.*", BOF_FILE_TYPE::BOF_FILE_ALL, false, 7, FileCollection);
  EXPECT_EQ(Sts_E, BOF_ERR_CANCEL);
  EXPECT_EQ(FileCollection.size(), 7);
}

TEST(Fs_Test, CreateTempFile)
{
  uintptr_t Io;
  BOFERR Sts_E;
  BofPath DirPath, Path;

  Sts_E = Bof_GetCurrentDirectory(DirPath);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Path = DirPath;
  Sts_E = Bof_CreateTempFile(BOF_FILE_PERMISSION_ALL_FOR_ALL, Path, "tmp", Io);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Path.DirectoryName(false, false).c_str(), DirPath.DirectoryName(false, false).c_str());
  EXPECT_STREQ(Path.Extension().c_str(), "tmp");
  Sts_E = Bof_CloseFile(Io);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
}

TEST(Fs_Test, ResetFileContentReOpenMode)
{
  BOFERR Sts_E;
  BOF_FILE_PERMISSION Permission_E;
  std::string Line_S, NewLine_S, LineRead_S, LineWrt_S;
  BofPath Dir, Path;
  uintptr_t Io;
  uint32_t Nb_U32;
  /*
    std::string RawData_S;
    uint32_t Start_U32, Delta_U32, i_U32;
    //https://www.learningcontainer.com/sample-docx-file-for-testing/
    for (i_U32 = 0; i_U32 < 10; i_U32++)
    {
      Start_U32 = BOF::Bof_GetMsTickCount();
      Sts_E = Bof_ReadFile("C:/Users/User/Downloads/32-MB-docx-file-download.docx", RawData_S);
      Delta_U32 = BOF::Bof_ElapsedMsTime(Start_U32);
      //printf("[%d] Delta %d ms Sz %zd B %zd KB %zd MB Err %s\n", i_U32, Delta_U32, RawData_S.size(), RawData_S.size() / 1024, RawData_S.size() / 1024 / 1024,BOF::Bof_ErrorCode(Sts_E));
    }
  */
  Permission_E = BOF_FILE_PERMISSION_READ_FOR_ALL | BOF_FILE_PERMISSION_WRITE_FOR_ALL;
  Line_S = "Hello World";

  Sts_E = Bof_GetCurrentDirectory(Dir);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Path.CurrentDirectoryName(Dir.FullPathName(false));
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Path = "BhaFile.txt";

  Sts_E = Bof_CreateFile(Permission_E, Path, false, Io);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Nb_U32 = static_cast<uint32_t>(Line_S.size());
  EXPECT_EQ(Bof_WriteFile(Io, Nb_U32, reinterpret_cast<const uint8_t *>(Line_S.c_str())), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Nb_U32, Line_S.size());

  Nb_U32 = static_cast<uint32_t>(Line_S.size());
  EXPECT_EQ(Bof_WriteFile(Io, Nb_U32, reinterpret_cast<const uint8_t *>(Line_S.c_str())), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Nb_U32, Line_S.size());

  EXPECT_EQ(Bof_ReadFile(Path, LineRead_S), BOF_ERR_NO_ERROR);
  NewLine_S = Line_S + Line_S;
  EXPECT_STREQ(LineRead_S.c_str(), NewLine_S.c_str());

  EXPECT_EQ(Bof_ResetFileContent(Path, true, 0), BOF_ERR_NO_ERROR);

  EXPECT_EQ(Bof_ReadFile(Path, LineRead_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(LineRead_S.c_str(), "");

  LineWrt_S = "Bye cruel World";
  Nb_U32 = static_cast<uint32_t>(LineWrt_S.size());
  EXPECT_EQ(Bof_WriteFile(Io, Nb_U32, reinterpret_cast<const uint8_t *>(LineWrt_S.c_str())), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Nb_U32, LineWrt_S.size());

  EXPECT_EQ(Bof_CloseFile(Io), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Bof_ReadFile(Path, LineRead_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(LineRead_S.c_str(), ""); // Extra zero are inserted at the beginning as Io has an offset of Line_S.size() but the file has been shrinked to O byte->write add extra 0 upp to crt file pos LineWrt_S.c_str());
}

TEST(Fs_Test, ResetFileContent)
{
  BOFERR Sts_E;
  BOF_FILE_PERMISSION Permission_E;
  std::string Line_S, NewLine_S, LineRead_S, LineWrt_S;
  BofPath Dir, Path;
  uintptr_t Io;
  uint32_t Nb_U32;

  Permission_E = BOF_FILE_PERMISSION_READ_FOR_ALL | BOF_FILE_PERMISSION_WRITE_FOR_ALL;
  Line_S = "Hello World";

  Sts_E = Bof_GetCurrentDirectory(Dir);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Path.CurrentDirectoryName(Dir.FullPathName(false));
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Path = "BhaFile.txt";

  Sts_E = Bof_CreateFile(Permission_E, Path, false, Io);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Nb_U32 = static_cast<uint32_t>(Line_S.size());
  EXPECT_EQ(Bof_WriteFile(Io, Nb_U32, reinterpret_cast<const uint8_t *>(Line_S.c_str())), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Nb_U32, Line_S.size());

  Nb_U32 = static_cast<uint32_t>(Line_S.size());
  EXPECT_EQ(Bof_WriteFile(Io, Nb_U32, reinterpret_cast<const uint8_t *>(Line_S.c_str())), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Nb_U32, Line_S.size());

  EXPECT_EQ(Bof_ReadFile(Path, LineRead_S), BOF_ERR_NO_ERROR);
  NewLine_S = Line_S + Line_S;
  EXPECT_STREQ(LineRead_S.c_str(), NewLine_S.c_str());

#if defined(_WIN32)
#else
  EXPECT_EQ(Bof_ResetFileContent(Path, false, Line_S.size()), BOF_ERR_NO_ERROR);

  EXPECT_EQ(Bof_ReadFile(Path, LineRead_S), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(LineRead_S.c_str(), Line_S.c_str());

  LineWrt_S = "Bye cruel World";
  Nb_U32 = static_cast<uint32_t>(LineWrt_S.size());
  EXPECT_EQ(Bof_WriteFile(Io, Nb_U32, reinterpret_cast<const uint8_t *>(LineWrt_S.c_str())), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Nb_U32, LineWrt_S.size());

  EXPECT_EQ(Bof_CloseFile(Io), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Bof_ReadFile(Path, LineRead_S), BOF_ERR_NO_ERROR);
  //	EXPECT_STREQ(LineRead_S.c_str(), (Line_S + LineWrt_S).c_str());
  EXPECT_STREQ(LineRead_S.c_str(), Line_S.c_str()); // Extra zero are inserted at the beginning as Io has an offset of Line_S.size() but the file has been shrinked to O byte->write add extra 0 upp to crt file pos LineWrt_S.c_str());
#endif
}