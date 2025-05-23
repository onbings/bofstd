/*
 * Copyright (c) 2000-2020, Onbings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the boffs interface. It wraps file system dependent system call
 *
 * Name:        bofsystem.h
 * Author:      Bernard HARMEL: b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Jan 19 2017  BHA : Initial release
 */
#pragma once

#include <bofstd/bofdatetime.h>
#include <bofstd/bofenum.h>
#include <bofstd/bofflag.h>
#include <bofstd/bofpath.h>
#include <bofstd/bofstring.h>
#include <bofstd/bofsystem.h>
#include <bofstd/bofbuffer.h>
#include <vector>

BEGIN_BOF_NAMESPACE()
enum class BOF_SEEK_METHOD : uint32_t
{
  BOF_SEEK_BEGIN = 0, /*! The starting point is zero or the beginning of the file.*/
  BOF_SEEK_CURRENT,   /*! The starting point is the current value of the file pointer.*/
  BOF_SEEK_END        /*! The starting point is the current end-of-file position*/
};

enum class BOF_FILE_PERMISSION : uint32_t
{
  // Comes from linux->OCTAL values
  BOF_PERM_S_ISUID = 04000, // set-user-ID (set process effective user ID onexecve(2))
  BOF_PERM_S_ISGID = 02000, // set-group-ID (set process effective group ID on xecve(2); mandatory locking, as described in fcntl(2); take a new file's group from parent directory, as described in chown(2) and mkdir(2))
  BOF_PERM_S_ISVTX = 01000, // sticky bit (restricted deletion flag, as described in unlink(2))
  BOF_PERM_S_IRUSR = 00400, // read by owner
  BOF_PERM_S_IWUSR = 00200, // write by owner
  BOF_PERM_S_IXUSR = 00100, // execute/search by owner ("search" applies for directories, and means that entries within the directory can be accessed)
  BOF_PERM_S_IRGRP = 00040, // read by group
  BOF_PERM_S_IWGRP = 00020, // write by group
  BOF_PERM_S_IXGRP = 00010, // execute/search by group
  BOF_PERM_S_IROTH = 00004, // read by others
  BOF_PERM_S_IWOTH = 00002, // write by others
  BOF_PERM_S_IXOTH = 00001, // execute/search by others
  BOF_PERM_S_NONE = 00000,  // special none value
};
template <>
struct IsItAnEnumBitFLag<BOF_FILE_PERMISSION> : std::true_type
{
};
/*
If you don't make all these casting you will have probelm such as:
    printf("BOF_FILE_PERMISSION_READ_FOR_ALL %x\n", BOF_FILE_PERMISSION_READ_FOR_ALL);
    ==>print 0 !!!
    const BOF_FILE_PERMISSION a = (BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IRUSR | BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IRGRP | BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IROTH);
    printf("a %x\n", a);
    ==>print 124 (which is ok)
*/
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_ALL_FOR_OWNER = (BOF_FILE_PERMISSION)((uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IRUSR | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IWUSR | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IXUSR);
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_ALL_FOR_GROUP = (BOF_FILE_PERMISSION)((uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IRGRP | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IWGRP | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IXGRP);
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_ALL_FOR_OTHER = (BOF_FILE_PERMISSION)((uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IROTH | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IWOTH | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IXOTH);
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_ALL_FOR_ALL = (BOF_FILE_PERMISSION)((uint32_t)BOF::BOF_FILE_PERMISSION_ALL_FOR_OWNER | (uint32_t)BOF::BOF_FILE_PERMISSION_ALL_FOR_GROUP | (uint32_t)BOF::BOF_FILE_PERMISSION_ALL_FOR_OTHER);
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_READ_FOR_ALL = (BOF_FILE_PERMISSION)((uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IRUSR | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IRGRP | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IROTH);
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_WRITE_FOR_ALL = (BOF_FILE_PERMISSION)((uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IWUSR | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IWGRP | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IWOTH);
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_EXEC_FOR_ALL = (BOF_FILE_PERMISSION)((uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IXUSR | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IXGRP | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IXOTH);
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_DEFAULT_RW = (BOF_FILE_PERMISSION)((uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IRUSR | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IWUSR | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IRGRP | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IWGRP | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IROTH);
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_DEFAULT_R = (BOF_FILE_PERMISSION)((uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IRUSR | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IWUSR | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IRGRP | (uint32_t)BOF::BOF_FILE_PERMISSION::BOF_PERM_S_IROTH);

enum class BOF_FILE_TYPE : uint32_t
{
  BOF_FILE_DONT_EXIST = 0x00000000,

  BOF_FILE_REG = 0x00000001,
  BOF_FILE_DIR = 0x00000002,
  BOF_FILE_CHR = 0x00000004,
  BOF_FILE_BLK = 0x00000008,
  BOF_FILE_LNK = 0x00000010,
  BOF_FILE_SCK = 0x00000020,
  BOF_FILE_FIFO = 0x00000040,

  BOF_FILE_UNK = 0x80000000,
  BOF_FILE_ALL = 0xFFFFFFFF, // For Bof_FindFile
};
template <>
struct IsItAnEnumBitFLag<BOF_FILE_TYPE> : std::true_type
{
};

/// @brief Set permission on a file or directory
/// @param _Permission_E : Specifies the permission to set
/// @param _rPath : Specifies the pathname of the file or directory to set
/// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
/// @remarks None
BOFSTD_EXPORT BOFERR Bof_SetFsPermission(const BOF_FILE_PERMISSION _Permission_E, const BofPath &_rPath);

/// @brief Get permission of a file or directory
/// @param _rPath : Specifies the pathname of the file or directory to get
/// @param _rPermission_E : Return the file or directory permissions.
/// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
/// @remarks None
BOFSTD_EXPORT BOFERR Bof_GetFsPermission(const BofPath &_rPath, BOF_FILE_PERMISSION &_rPermission_E);

/// @brief Check if a file or directory is present on disk and returns its type
/// @param _rPath : Specifies the path of the file or directory to check
/// @return The typ of file
/// @remarks None
BOFSTD_EXPORT BOF_FILE_TYPE Bof_GetFileType(const BofPath &_rPath);

/// @brief Returns the current directory and its permission
/// @param _rPath : Returns the current directory
/// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
/// @remarks A directory delimiter is inserted at the end of the path
BOFSTD_EXPORT BOFERR Bof_GetCurrentDirectory(BofPath &_rPath);

BOFSTD_EXPORT BOFERR Bof_GetCurrentDirectory(std::string &_rPath_S);

/// @brief Change current directory and returns its permissions
/// @param _rNewDirectory_S : Specifies the new current directory
/// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
/// @remarks _rCurrentDirectory_S will be terminated by a / character
BOFSTD_EXPORT BOFERR Bof_SetCurrentDirectory(const BofPath &_rNewDirectory_S);

/// @brief Create a new directory and its sub directories if needed with a given permission
/// @param _Permission_E : Specifies the directory permissions.
/// @param _rPath : Specifies the path of the new directory to create
/// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
/// @remarks None
BOFSTD_EXPORT BOFERR Bof_CreateDirectory(const BOF_FILE_PERMISSION _Permission_E, const BofPath &_rPath);

/// @brief Remove a directory including files and sub directories
/// @param _rPath : Specifies the path of the directory to delete
/// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
/// @remarks None
BOFSTD_EXPORT BOFERR Bof_CleanupDirectory(bool _Recursive_B, const BofPath &_rPath, bool _RemoveRootDir_B);

BOFSTD_EXPORT BOFERR Bof_RemoveDirectory(const BofPath &_rPath);

struct BOF_FILE_FOUND
{
  BofPath Path;
  uint64_t Size_U64;
  BofDateTime Creation;
  BofDateTime LastAccess;
  BofDateTime LastWrite;
  BOF_FILE_TYPE FileType_E;

  BOF_FILE_FOUND()
  {
    Reset();
  }
  void Reset()
  {
    Path = "";
    Size_U64 = 0;
    Creation.Reset();
    LastAccess.Reset();
    LastWrite.Reset();
    FileType_E = BOF_FILE_TYPE::BOF_FILE_UNK;
  }
};

using BOF_DIRECTORY_PARSER_CALLBACK = std::function<bool(const BOF_FILE_FOUND &_rFileFound_X)>;
BOFSTD_EXPORT BOFERR Bof_DirectoryParser(const BofPath &_rPath, const std::string &_rPattern_S, const BOF_FILE_TYPE _FileTypeToFind_E, bool _Recursive_B, BOF_DIRECTORY_PARSER_CALLBACK &_rDirectoryParserCallback);
BOFSTD_EXPORT BOFERR Bof_FindFile(const BofPath &_rPath, const std::string &_rPattern_S, const BOF_FILE_TYPE _FileTypeToFind_E, bool _Recursive_B, uint32_t _MaxNumberOfResult_U32, std::vector<BOF_FILE_FOUND> &_rFileCollection);
BOFSTD_EXPORT bool Bof_IsFileHandleValid(uintptr_t _Io);
BOFSTD_EXPORT BOFERR Bof_CreateFile(const BOF_FILE_PERMISSION _Permission_E, const BofPath &_rPath, bool _Append_B, uintptr_t &_rIo);
BOFSTD_EXPORT BOFERR Bof_CreateTempFile(const BOF_FILE_PERMISSION _Permission_E, BofPath &_rPath, const std::string &_rExt_S, uintptr_t &_rIo);
BOFSTD_EXPORT BOFERR Bof_OpenFile(const BofPath &_rPath, bool _ReadOnly_B, bool _Append_B, uintptr_t &_rIo);

BOFSTD_EXPORT int64_t Bof_SetFileIoPosition(uintptr_t _Io, int64_t _Offset_S64, BOF_SEEK_METHOD _SeekMethod_E);
BOFSTD_EXPORT int64_t Bof_GetFileIoPosition(uintptr_t _Io);

BOFSTD_EXPORT BOFERR Bof_ReadLine(uintptr_t _Io, uint32_t &_rNb_U32, char *_pBuffer_c);
BOFSTD_EXPORT BOFERR Bof_ReadLine(const BOF::BofPath &_rPath, std::vector<std::string> &_rLineCollection);
BOFSTD_EXPORT BOFERR Bof_ReadLine(uintptr_t _Io, std::string &_rLine_S);
BOFSTD_EXPORT BOFERR Bof_WriteLine(uintptr_t _Io, const std::string &_rLine_S);

BOFSTD_EXPORT BOFERR Bof_ReadFile(uintptr_t _Io, uint32_t &_rNb_U32, uint8_t *_pBuffer_U8);
BOFSTD_EXPORT BOFERR Bof_WriteFile(uintptr_t _Io, uint32_t &_rNb_U32, const uint8_t *_pBuffer_U8);

BOFSTD_EXPORT BOFERR Bof_ReadLine(const BofPath &_rPath, std::string &_rBuffer_S);
BOFSTD_EXPORT BOFERR Bof_WriteLine(const BOF_FILE_PERMISSION _Permission_E, const BofPath &_rPath, bool _Append_B, const std::string &_rBuffer_S);

BOFSTD_EXPORT BOFERR Bof_FlushFile(uintptr_t _Io);
BOFSTD_EXPORT BOFERR Bof_CloseFile(uintptr_t &_rIo);

BOFSTD_EXPORT BOFERR Bof_ReadFile(const BofPath &_rPath, BOF_BUFFER &_rBufferToDeleteAfterUsage_X);
BOFSTD_EXPORT BOFERR Bof_ReadFile(const BofPath &_rPath, std::string &_rRawData_S);
BOFSTD_EXPORT BOFERR Bof_WriteFile(const BOF_FILE_PERMISSION _Permission_E, const BofPath &_rPath, const BOF_BUFFER &_rBuffer_X);
BOFSTD_EXPORT BOFERR Bof_WriteFile(const BOF_FILE_PERMISSION _Permission_E, const BofPath &_rPath, const std::string &_rRawData_S);

BOFSTD_EXPORT uint64_t Bof_GetFileSize(const BofPath &_rPath);
BOFSTD_EXPORT BOFERR Bof_DeleteFile(const BofPath &_rPath);
BOFSTD_EXPORT BOFERR Bof_RenameFile(const BofPath &_rOldPath, const BofPath &_rNewPath);
BOFSTD_EXPORT bool Bof_IsPathExist(const BofPath &_rPath, bool &_rItIsADirectory);
BOFSTD_EXPORT BOFERR Bof_CopyFile(bool _OverwriteIfExists_B, const BofPath &_rSrcPath, const BofPath &_rDstPath);
BOFSTD_EXPORT BOFERR Bof_ResetFileContent(const BofPath &_rPath, bool _ReOpenMode_B, int64_t _Offset_S64);

END_BOF_NAMESPACE()