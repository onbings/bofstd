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

/*** Include ****************************************************************/

#include <bofstd/bofstd.h>
#include <bofstd/bofsystem.h>
#include <bofstd/bofpath.h>
#include <bofstd/bofflag.h>
#include <vector>

BEGIN_BOF_NAMESPACE()

enum class BOF_FILE_PERMISSION : uint32_t
{
		//Comes from linux->OCTAL values
		BOF_PERM_S_ISUID = 04000,  //set-user-ID (set process effective user ID onexecve(2))
		BOF_PERM_S_ISGID = 02000,  //set-group-ID (set process effective group ID on xecve(2); mandatory locking, as described in fcntl(2); take a new file's group from parent directory, as described in chown(2) and mkdir(2))
		BOF_PERM_S_ISVTX = 01000,    //sticky bit (restricted deletion flag, as described in unlink(2))
		BOF_PERM_S_IRUSR = 00400,  //read by owner
		BOF_PERM_S_IWUSR = 00200,    //write by owner
		BOF_PERM_S_IXUSR = 00100,    //execute/search by owner ("search" applies for directories, and means that entries within the directory can be accessed)
		BOF_PERM_S_IRGRP = 00040,    //read by group
		BOF_PERM_S_IWGRP = 00020,    //write by group
		BOF_PERM_S_IXGRP = 00010,    //execute/search by group
		BOF_PERM_S_IROTH = 00004,    //read by others
		BOF_PERM_S_IWOTH = 00002,    //write by others
		BOF_PERM_S_IXOTH = 00001,    //execute/search by others
		BOF_PERM_S_NONE = 00000,   //special none value
};
template<>
struct IsItAnEnumBitFLag<BOF_FILE_PERMISSION> : std::true_type
{
};

#if (1)
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_ALL_FOR_OWNER = (BOF_FILE_PERMISSION::BOF_PERM_S_IRUSR | BOF_FILE_PERMISSION::BOF_PERM_S_IWUSR | BOF_FILE_PERMISSION::BOF_PERM_S_IXUSR);
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_ALL_FOR_GROUP = (BOF_FILE_PERMISSION::BOF_PERM_S_IRGRP | BOF_FILE_PERMISSION::BOF_PERM_S_IWGRP | BOF_FILE_PERMISSION::BOF_PERM_S_IXGRP);
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_ALL_FOR_OTHER = (BOF_FILE_PERMISSION::BOF_PERM_S_IROTH | BOF_FILE_PERMISSION::BOF_PERM_S_IWOTH | BOF_FILE_PERMISSION::BOF_PERM_S_IXOTH);
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_ALL_FOR_ALL = (BOF_FILE_PERMISSION_ALL_FOR_OWNER | BOF_FILE_PERMISSION_ALL_FOR_GROUP | BOF_FILE_PERMISSION_ALL_FOR_OTHER);
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_READ_FOR_ALL = (BOF_FILE_PERMISSION::BOF_PERM_S_IRUSR | BOF_FILE_PERMISSION::BOF_PERM_S_IRGRP | BOF_FILE_PERMISSION::BOF_PERM_S_IROTH);
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_WRITE_FOR_ALL = (BOF_FILE_PERMISSION::BOF_PERM_S_IWUSR | BOF_FILE_PERMISSION::BOF_PERM_S_IWGRP | BOF_FILE_PERMISSION::BOF_PERM_S_IWOTH);
const BOF_FILE_PERMISSION BOF_FILE_PERMISSION_EXEC_FOR_ALL = (BOF_FILE_PERMISSION::BOF_PERM_S_IXUSR | BOF_FILE_PERMISSION::BOF_PERM_S_IXGRP | BOF_FILE_PERMISSION::BOF_PERM_S_IXOTH);
#else
#define BOF_FILE_PERMISSION_ALL_FOR_OWNER  (BOF_FILE_PERMISSION::BOF_PERM_S_IRUSR | BOF_FILE_PERMISSION::BOF_PERM_S_IWUSR | BOF_FILE_PERMISSION::BOF_PERM_S_IXUSR)
#define BOF_FILE_PERMISSION_ALL_FOR_GROUP  (BOF_FILE_PERMISSION::BOF_PERM_S_IRGRP | BOF_FILE_PERMISSION::BOF_PERM_S_IWGRP | BOF_FILE_PERMISSION::BOF_PERM_S_IXGRP)
#define BOF_FILE_PERMISSION_ALL_FOR_OTHER  (BOF_FILE_PERMISSION::BOF_PERM_S_IROTH | BOF_FILE_PERMISSION::BOF_PERM_S_IWOTH | BOF_FILE_PERMISSION::BOF_PERM_S_IXOTH)
#define BOF_FILE_PERMISSION_ALL_FOR_ALL  (BOF_FILE_PERMISSION_ALL_FOR_OWNER | BOF_FILE_PERMISSION_ALL_FOR_GROUP | BOF_FILE_PERMISSION_ALL_FOR_OTHER)
#define BOF_FILE_PERMISSION_READ_FOR_ALL  (BOF_FILE_PERMISSION::BOF_PERM_S_IRUSR | BOF_FILE_PERMISSION::BOF_PERM_S_IRGRP | BOF_FILE_PERMISSION::BOF_PERM_S_IROTH)
#define BOF_FILE_PERMISSION_WRITE_FOR_ALL  (BOF_FILE_PERMISSION::BOF_PERM_S_IWUSR | BOF_FILE_PERMISSION::BOF_PERM_S_IWGRP | BOF_FILE_PERMISSION::BOF_PERM_S_IWOTH)
#define BOF_FILE_PERMISSION_EXEC_FOR_ALL  (BOF_FILE_PERMISSION::BOF_PERM_S_IXUSR | BOF_FILE_PERMISSION::BOF_PERM_S_IXGRP | BOF_FILE_PERMISSION::BOF_PERM_S_IXOTH)
#endif


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
		BOF_FILE_ALL = 0xFFFFFFFF, //For Bof_FindFile
};
template<>
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
BOFSTD_EXPORT BOFERR Bof_ChangeCurrentDirectory(const BofPath &_rNewDirectory_S);

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
BOFSTD_EXPORT BOFERR Bof_CleanupDirectory(bool _Recursive_B, const BofPath &_rPath);

BOFSTD_EXPORT BOFERR Bof_RemoveDirectory(const BofPath &_rPath);

struct BOFSTD_EXPORT BOF_FILE_FOUND
{
	BofPath Path;
	uint64_t Size_U64;
  BOF_DATE_TIME     Creation_X;
  BOF_DATE_TIME     LastAccess_X;
  BOF_DATE_TIME     LastWrite_X;
  BOF_FILE_TYPE     FileType_E;

  BOF_FILE_FOUND()
  {
    Reset();
  }
  void Reset()
  {
    Path="";
    Size_U64 = 0;
    Creation_X.Reset();
    LastAccess_X.Reset();
    LastWrite_X.Reset();
    FileType_E = BOF_FILE_TYPE::BOF_FILE_UNK;
  }
};
#if 0
#define FA_DIRECTORY                            0

#define _A_NORMAL                               0x00 /*! Normal file - No read/write restrictions */
#define _A_RDONLY                               0x01 /*! Read only file */
#define _A_HIDDEN                               0x02 /*! Hidden file */
#define _A_SYSTEM                               0x04 /*! System file */
#define _A_SUBDIR                               0x10 /*! Subdirectory */
#define _A_ARCH                                 0x20 /*! Archive file */

#define BOFFILEENUMINTERNALSTATE_MAGICNUMBER    0xFDBDC564
typedef struct
{
  U32   MagicNumber_U32;
  void *pHandle;        //DIR *
  bool  FindFileAndDirectory_B;
  char  pPath_c[256];
  char  pPattern_c[128];
}
BOFFILEENUMINTERNALSTATE;

typedef struct
{
  BOFDATETIME              CreationTime_X;
  BOFDATETIME              LastAccessTime_X;
  BOFDATETIME              LastWriteTime_X;
  U8                       Attrib_U8;
  U32                      Size_U32;
  char                     pName_c[256];

  BOFFILEENUMINTERNALSTATE Internal_X;
}
BOFFILEFOUND;
#endif

using BOF_DIRECTORY_PARSER_CALLBACK = std::function<bool(const BOF_FILE_FOUND &_rFileFound_X)> ;
BOFSTD_EXPORT BOFERR Bof_DirectoryParser(const BofPath &_rPath, const std::string &_rPattern_S, const BOF_FILE_TYPE _FileTypeToFind_E, bool _Recursive_B, BOF_DIRECTORY_PARSER_CALLBACK &_rDirectoryParserCallback);
BOFSTD_EXPORT BOFERR Bof_FindFile(const BofPath &_rPath, const std::string &_rPattern_S, const BOF_FILE_TYPE _FileTypeToFind_E, bool _Recursive_B, std::vector<BOF_FILE_FOUND> &_rFileCollection);
BOFSTD_EXPORT bool Bof_IsFileHandleValid(intptr_t _Io);
BOFSTD_EXPORT BOFERR Bof_CreateFile(const BOF_FILE_PERMISSION _Permission_E, const BofPath &_rPath, bool _Append_B, intptr_t &_rIo);
BOFSTD_EXPORT BOFERR Bof_CreateTempFile(const BOF_FILE_PERMISSION _Permission_E, BofPath &_rPath, const std::string &_rExt_S, intptr_t &_rIo);
BOFSTD_EXPORT BOFERR Bof_OpenFile(const BofPath &_rPath, bool _ReadOnly_B, intptr_t &_rIo);

BOFSTD_EXPORT int64_t Bof_SetFileIoPosition(intptr_t _Io, int64_t _Offset_S64, BOF_SEEK_METHOD _SeekMethod_E);
BOFSTD_EXPORT int64_t Bof_GetFileIoPosition(intptr_t _Io);

BOFSTD_EXPORT BOFERR Bof_ReadLine(intptr_t _Io, std::string &_rLine_S);
BOFSTD_EXPORT BOFERR Bof_WriteLine(intptr_t _Io, const std::string &_rLine_S);

BOFSTD_EXPORT BOFERR Bof_ReadFile(intptr_t _Io, uint32_t &_rNb_U32, uint8_t *_pBuffer_U8);
BOFSTD_EXPORT BOFERR Bof_WriteFile(intptr_t _Io, uint32_t &_rNb_U32, const uint8_t *_pBuffer_U8);

BOFSTD_EXPORT BOFERR Bof_ReadLine(const BofPath& _rPath, std::string& _rBuffer_S);
BOFSTD_EXPORT BOFERR Bof_WriteLine(const BOF_FILE_PERMISSION _Permission_E, const BofPath &_rPath, bool _Append_B, const std::string &_rBuffer_S);

BOFSTD_EXPORT BOFERR Bof_FlushFile(intptr_t _Io);
BOFSTD_EXPORT BOFERR Bof_CloseFile(intptr_t &_rIo);

BOFSTD_EXPORT BOFERR Bof_ReadFile(const BofPath &_rPath, BOF_BUFFER &_rBufferToDeleteAfterUsage_X);
BOFSTD_EXPORT BOFERR Bof_ReadFile(const BofPath &_rPath, std::string &_rRawData_S);

BOFSTD_EXPORT uint64_t Bof_GetFileSize(const BofPath &_rPath);
BOFSTD_EXPORT BOFERR Bof_DeleteFile(const BofPath &_rPath);
BOFSTD_EXPORT BOFERR Bof_RenameFile(const BofPath &_rOldPath, const BofPath &_rNewPath);
BOFSTD_EXPORT bool Bof_IsFileExist(const BofPath &_rPath);
BOFSTD_EXPORT BOFERR Bof_CopyFile(bool _OverwriteIfExists_B, const BofPath &_rSrcPath, const BofPath &_rDstPath);
BOFSTD_EXPORT BOFERR Bof_ResetFileContent(const BofPath &_rPath, bool _ReOpenMode_B, int64_t _Offset_S64);

END_BOF_NAMESPACE()