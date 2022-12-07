/*
 * Copyright (c) 2013-2033, Onbings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the boffs interface. It wraps file system dependent system call
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
#include <bofstd/boffs.h>
#include <bofstd/bofstring.h>

#include <sys/stat.h>
#include <ios>
#include <fstream>

#if defined (_WIN32)
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

BEGIN_BOF_NAMESPACE()

BOFERR Bof_SetFsPermission(const BOF_FILE_PERMISSION _Permission_E, const BofPath &_rPath)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if ((_rPath.IsValid()) && (_rPath.IsExist()))
  {
    Rts_E = BOF_ERR_NO_ERROR;
#if defined (_WIN32)
    // under windows only _S_IREAD and _S_IREAD are supported
    int Permission_i = 0;
    if (Bof_IsBitFlagSet(_Permission_E, BOF_FILE_PERMISSION::BOF_PERM_S_IRUSR) || Bof_IsBitFlagSet(_Permission_E, BOF_FILE_PERMISSION::BOF_PERM_S_IRGRP) || Bof_IsBitFlagSet(_Permission_E, BOF_FILE_PERMISSION::BOF_PERM_S_IROTH))
    {
      Permission_i |= _S_IREAD;
    }
    if (Bof_IsBitFlagSet(_Permission_E, BOF_FILE_PERMISSION::BOF_PERM_S_IWUSR) || Bof_IsBitFlagSet(_Permission_E, BOF_FILE_PERMISSION::BOF_PERM_S_IWGRP) || Bof_IsBitFlagSet(_Permission_E, BOF_FILE_PERMISSION::BOF_PERM_S_IWOTH))
    {
      Permission_i |= _S_IWRITE;
    }
    if (Permission_i)
    {
      if (_chmod(_rPath.FullPathName(true).c_str(), Permission_i))
      {
        Rts_E = BOF_ERR_EACCES;
      }
    }
#else
    if (chmod(_rPath.FullPathName(false).c_str(), static_cast<int> (_Permission_E)))
    {
      Rts_E = BOF_ERR_EACCES;
    }
#endif
  }
  return Rts_E;
}


BOFERR Bof_GetFsPermission(const BofPath &_rPath, BOF_FILE_PERMISSION &_rPermission_E)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  int Mode_i, TheMode_i;

  if ((_rPath.IsValid()) && (_rPath.IsExist()))
  {
    Rts_E = BOF_ERR_INTERNAL;
    _rPermission_E = BOF_FILE_PERMISSION::BOF_PERM_S_NONE;
    Mode_i = -1;
    /*
    #if defined (_WIN32)
        DWORD FileAttribute_DW = GetFileAttributesA(_rPath.FullPathName(false).c_str());
        if (FileAttribute_DW != 0xffffffff)
        {
          Mode_i = 0;
          if (FileAttribute_DW & FILE_ATTRIBUTE_DIRECTORY)
            Mode_i |= S_IFDIR;
          else
            Mode_i |= S_IFREG;

          if (!(FileAttribute_DW & FILE_ATTRIBUTE_HIDDEN))
            Mode_i |= S_IRUSR;
          if (!(FileAttribute_DW & FILE_ATTRIBUTE_READONLY))
            Mode_i |= S_IWUSR;
        }
    #else
    */
    struct stat Stat_X;
    if (stat(_rPath.FullPathName(false).c_str(), &Stat_X) == 0)
      Mode_i = Stat_X.st_mode;
    //#endif

    //		Mode_i = zsys_file_mode(_rPath.FullPathName(false).c_str());
    if (Mode_i != -1)
    {
#if defined (_WIN32)
      TheMode_i = 0;
      if (Mode_i & _S_IREAD)
      {
        TheMode_i |= static_cast<int> (BOF_FILE_PERMISSION_READ_FOR_ALL);
      }
      if (Mode_i & _S_IWRITE)
      {
        TheMode_i |= static_cast<int> (BOF_FILE_PERMISSION_WRITE_FOR_ALL);
      }
      if (Mode_i & _S_IEXEC)
      {
        TheMode_i |= static_cast<int> (BOF_FILE_PERMISSION_EXEC_FOR_ALL);
      }
#else
      TheMode_i = Mode_i;
#endif

      _rPermission_E = static_cast<BOF_FILE_PERMISSION> (TheMode_i);
      Rts_E = BOF_ERR_NO_ERROR;
    }
  }
  return Rts_E;
}

BOF_FILE_TYPE Bof_GetFileType(const BofPath &_rPath)
{
  BOF_FILE_TYPE Rts_E = BOF_FILE_TYPE::BOF_FILE_DONT_EXIST;

#if defined(_WIN32)
  struct _stati64 Stat_X;
  if (_stati64(_rPath.FullPathName(true).c_str(), &Stat_X) == 0)
  {
    if (_S_IFDIR & Stat_X.st_mode)
    {
      Rts_E = Rts_E | BOF_FILE_TYPE::BOF_FILE_DIR;
    }
    if (_S_IFCHR & Stat_X.st_mode)
    {
      Rts_E = Rts_E | BOF_FILE_TYPE::BOF_FILE_CHR;
    }
    if (_S_IFIFO & Stat_X.st_mode)
    {
      Rts_E = Rts_E | BOF_FILE_TYPE::BOF_FILE_FIFO;
    }
    if (_S_IFREG & Stat_X.st_mode)
    {
      Rts_E = Rts_E | BOF_FILE_TYPE::BOF_FILE_REG;
    }
  }

#else
  struct stat Stat_X;
  if (stat(_rPath.FullPathName(false).c_str(), &Stat_X) == 0)
  {
    if (S_ISDIR(Stat_X.st_mode))
    {
      Rts_E = Rts_E | BOF_FILE_TYPE::BOF_FILE_DIR;
    }
    if (S_ISCHR(Stat_X.st_mode))
    {
      Rts_E = Rts_E | BOF_FILE_TYPE::BOF_FILE_CHR;
    }
    if (S_ISFIFO(Stat_X.st_mode))
    {
      Rts_E = Rts_E | BOF_FILE_TYPE::BOF_FILE_FIFO;
    }
    if (S_ISREG(Stat_X.st_mode))
    {
      Rts_E = Rts_E | BOF_FILE_TYPE::BOF_FILE_REG;
    }
    if (S_ISBLK(Stat_X.st_mode))
    {
      Rts_E = Rts_E | BOF_FILE_TYPE::BOF_FILE_BLK;
    }
    if (S_ISLNK(Stat_X.st_mode))
    {
      Rts_E = Rts_E | BOF_FILE_TYPE::BOF_FILE_LNK;
    }
    if (S_ISSOCK(Stat_X.st_mode))
    {
      Rts_E = Rts_E | BOF_FILE_TYPE::BOF_FILE_SCK;
    }
  }
#endif

  return Rts_E;
}

BOFERR Bof_GetCurrentDirectory(std::string &_rPath_S)
{
  BOFERR Rts_E = BOF_ERR_INTERNAL;
  char pDir_c[0x1000];

#if defined (_WIN32)
  if (GetCurrentDirectoryA(sizeof(pDir_c), pDir_c))
  {
    _rPath_S = pDir_c;
    if (_rPath_S.back() != '\\')
    {
      _rPath_S += '\\';
    }
    Rts_E = BOF_ERR_NO_ERROR;
  }
#else
  if (getcwd(pDir_c, sizeof(pDir_c)) != nullptr)
  {
    _rPath_S = pDir_c;
    if (_rPath_S.back() != '/')
    {
      _rPath_S += '/';
    }
    Rts_E = BOF_ERR_NO_ERROR;
  }
#endif
  return (Rts_E);
}

BOFERR Bof_GetCurrentDirectory(BofPath &_rPath)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  std::string Dir_S;

  Rts_E = Bof_GetCurrentDirectory(Dir_S);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    _rPath = Dir_S;
  }
  return (Rts_E);
}

BOFERR Bof_ChangeCurrentDirectory(const BofPath &_rPath)
{
  BOFERR Rts_E = BOF_ERR_ENOTDIR;
  //int Sts_i;

#if defined (_WIN32)
  if (SetCurrentDirectoryA(_rPath.FullPathName(true).c_str()))
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
#else
  if (chdir(_rPath.FullPathName(false).c_str()) == 0)
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
#endif
  //	Sts_i = zsys_dir_change(_rPath.FullPathName(false).c_str());
  //	Rts_E = (Sts_i == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_DONT_EXIST;
  //printf("dir %s errno %d\n",_rPath.FullPathName(false).c_str(),errno);
  return Rts_E;
}

BOFERR Bof_CreateDirectory(const BOF_FILE_PERMISSION _Permission_E, const BofPath &_rPath)
{
  BOFERR Rts_E = BOF_ERR_EACCES;
  size_t Pos = 0;

  if (_rPath.IsExist())
  {
    Rts_E = (_rPath.IsDirectory()) ? BOF_ERR_NO_ERROR : BOF_ERR_BAD_TYPE;
  }
  else
  {
    Rts_E = BOF_ERR_INVALID_DST;
    if (_rPath.IsDirectory())
    {
      Rts_E = BOF_ERR_INTERNAL;
      do
      {
        Pos = _rPath.FullPathName(false).find_first_of('/', Pos + 1);
        if (Pos != std::string::npos)
        {
#if defined (_WIN32)
          if (CreateDirectoryA(_rPath.FullPathName(true).substr(0, Pos).c_str(), nullptr))
          {
            Rts_E = BOF_ERR_NO_ERROR;
          }
#else
          mkdir(_rPath.FullPathName(false).substr(0, Pos).c_str(), static_cast<mode_t>(BOF_FILE_PERMISSION_ALL_FOR_OWNER));
          //If intermediate dir exist mkdir return is -1				printf("mkdir at %d for %s->%d\n",Pos,_rPath.FullPathName(false).substr(0, Pos).c_str(), Sts_i);
          Rts_E = BOF_ERR_NO_ERROR;
#endif
        }
      } while (Pos != std::string::npos);
      //Rts_E = (zsys_dir_create(_rPath.FullPathName(false).c_str()) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_EACCES;

      // zsys_file_mode_default();
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Rts_E = Bof_SetFsPermission(_Permission_E, _rPath);
      }
    }
  }
  return (Rts_E);
}


BOFERR Bof_DirectoryParser(const BofPath &_rPath, const std::string &_rPattern_S, const BOF_FILE_TYPE _FileTypeToFind_E, bool _Recursive_B, BOF_DIRECTORY_PARSER_CALLBACK &_rDirectoryParserCallback)
{
  BOFERR Rts_E = BOF_ERR_ENOENT;
  bool EntryOk_B, Finish_B, ItIsADirectory_B, DotDotDir_B, DotDir_B;  // , EmptyDir_B;
  BOF_FILE_FOUND FileFound_X; // , DotFileFound_X;
  uint64_t Time_U64;

#if defined( _WIN32 )
  HANDLE FindFirstFile_h;
  WIN32_FIND_DATAA FindData_X;
#else
  char pPath_c[0x1000];
  DIR *pDir_X;
  struct dirent *pDirEntry_X;
  struct stat FileStat_X;
#endif

  //printf("==>Enter %s\n", _rPath.FullPathName(true).c_str());

  if (_rPath.IsDirectory())
  {
#if defined( _WIN32 )
    std::string PatternFilename_S;
    PatternFilename_S = _rPath.FullPathName(true) + "*";
    FindFirstFile_h = FindFirstFileA(PatternFilename_S.c_str(), &FindData_X);
    if (FindFirstFile_h != INVALID_HANDLE_VALUE)
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
#else
    pDir_X = opendir(_rPath.FullPathName(false).c_str());
    if (pDir_X)
    {
      pDirEntry_X = readdir(pDir_X);
      if (pDirEntry_X != nullptr)
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
      else
      {
        closedir(pDir_X);
      }
    }
#endif
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      //   EmptyDir_B = true;
      do
      {
        Finish_B = true;
        ItIsADirectory_B = false;
#if defined( _WIN32 )
        DotDir_B = ((FindData_X.cFileName[0] == '.') && (FindData_X.cFileName[1] == 0)); //.
        DotDotDir_B = ((FindData_X.cFileName[0] == '.') && (FindData_X.cFileName[1] == '.') && (FindData_X.cFileName[2] == 0)); //..
        //if ((DotDir_B) || (DotDotDir_B))
        if (DotDotDir_B)
        {
          EntryOk_B = false;
        }
        else
        {
          //  if (EmptyDir_B)
          {
            //    EmptyDir_B = DotDir_B;
          }
          EntryOk_B = true;
          ItIsADirectory_B = (FindData_X.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
          FileFound_X.Path = _rPath;
          FileFound_X.Path.Combine(FindData_X.cFileName);
        }
#else
        //printf("dir entry %s\n",pDirEntry_X->d_name);
        DotDir_B = ((pDirEntry_X->d_name[0] == '.') && (pDirEntry_X->d_name[1] == 0)); //.
        DotDotDir_B = ((pDirEntry_X->d_name[0] == '.') && (pDirEntry_X->d_name[1] == '.') && (pDirEntry_X->d_name[2] == 0)); //..
        if (DotDotDir_B)
        {
          EntryOk_B = false;
        }
        else
        {
          snprintf(pPath_c, sizeof(pPath_c), "%s/%s", _rPath.FullPathName(false).c_str(), pDirEntry_X->d_name);
          //printf("0:EntryOk_B %d ItIsADirectory_B %d pPath_c %s\n",EntryOk_B,ItIsADirectory_B,pPath_c);
          if (lstat(pPath_c, &FileStat_X))
          {
            EntryOk_B = false;
            //printf("1:EntryOk_B %d ItIsADirectory_B %d errno %d\n",EntryOk_B,ItIsADirectory_B,errno);

          }
          else
          {
            EntryOk_B = true;
            ItIsADirectory_B = (S_ISDIR(FileStat_X.st_mode)) ? true : false;
            FileFound_X.Path = _rPath;
            FileFound_X.Path.Combine(pDirEntry_X->d_name);
            //printf("2:EntryOk_B %d ItIsADirectory_B %d\n",EntryOk_B,ItIsADirectory_B);

          }
          //printf("3:EntryOk_B %d ItIsADirectory_B %d\n",EntryOk_B,ItIsADirectory_B);
        }
#endif
        //printf("EntryOk_B %d ItIsADirectory_B %d\n",EntryOk_B,ItIsADirectory_B);
        if (EntryOk_B)
        {
          if (ItIsADirectory_B)
          {
            FileFound_X.Path.Combine("\\");
            if (!DotDir_B)
            {
              if (_Recursive_B)
              {
                //printf("==>RECCU %s\n", FileFound_X.Path.FullPathName(true).c_str());
                Rts_E = Bof_DirectoryParser(FileFound_X.Path, _rPattern_S, _FileTypeToFind_E, _Recursive_B, _rDirectoryParserCallback);
                //printf("==>ENDR reset %s to \n", FileFound_X.Path.FullPathName(true).c_str());  // , _rPath.FullPathName(true).c_str());
                if (Rts_E == BOF_ERR_NO_ERROR)
                {
                  //    FileFound_X.Path = _rPath;
                }
              }
            }
          }
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            FileFound_X.FileType_E = BOF_FILE_TYPE::BOF_FILE_DIR;
            EntryOk_B = ((Bof_IsBitFlagSet(_FileTypeToFind_E, FileFound_X.FileType_E) && (ItIsADirectory_B)));
            if (!EntryOk_B)
            {
              FileFound_X.FileType_E = BOF_FILE_TYPE::BOF_FILE_REG;
              EntryOk_B = ((Bof_IsBitFlagSet(_FileTypeToFind_E, FileFound_X.FileType_E) && (!ItIsADirectory_B)));
            }
            if ((EntryOk_B) && (!ItIsADirectory_B))
            {
              EntryOk_B = (Bof_PatternCompare(FileFound_X.Path.FileNameWithExtension().c_str(), _rPattern_S.c_str()));
            }
            if (EntryOk_B)
            {
#if defined( _WIN32 )
              FileFound_X.Size_U64 = (static_cast<uint64_t>(FindData_X.nFileSizeHigh) << 32) | static_cast<uint64_t>(FindData_X.nFileSizeLow);
              // Set file times and dates
              Time_U64 = (static_cast<uint64_t>(FindData_X.ftCreationTime.dwHighDateTime) << 32) | static_cast<uint64_t>(FindData_X.ftCreationTime.dwHighDateTime);
              Bof_FileTimeToSystemTime(Time_U64, FileFound_X.Creation);
              Time_U64 = (static_cast<uint64_t>(FindData_X.ftLastAccessTime.dwHighDateTime) << 32) | static_cast<uint64_t>(FindData_X.ftLastAccessTime.dwHighDateTime);
              Bof_FileTimeToSystemTime(Time_U64, FileFound_X.LastAccess);
              Time_U64 = (static_cast<uint64_t>(FindData_X.ftLastWriteTime.dwHighDateTime) << 32) | static_cast<uint64_t>(FindData_X.ftLastWriteTime.dwHighDateTime);
              Bof_FileTimeToSystemTime(Time_U64, FileFound_X.LastWrite);
#else
              FileFound_X.Size_U64 = FileStat_X.st_size;
              Time_U64 = static_cast<uint64_t>(FileStat_X.st_ctime);
              Bof_FileTimeToSystemTime(Time_U64, FileFound_X.Creation);
              Time_U64 = static_cast<uint64_t>(FileStat_X.st_atime);
              Bof_FileTimeToSystemTime(Time_U64, FileFound_X.LastAccess);
              Time_U64 = static_cast<uint64_t>(FileStat_X.st_mtime);
              Bof_FileTimeToSystemTime(Time_U64, FileFound_X.LastWrite);
#endif
              if (EntryOk_B)
              {
                //                printf("Add %s\n", FileFound_X.Path.FullPathName(true).c_str());
                if (DotDir_B)
                {
                  //             DotFileFound_X = FileFound_X;
                }
                else
                {
                  //printf("--> %s\n", FileFound_X.Path.FullPathName(true).c_str());

                  if (!_rDirectoryParserCallback(FileFound_X))
                  {
                    Finish_B = true;
                    Rts_E = BOF_ERR_CANCEL;
                  }
                }
              }
            }
          }
        }
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
#if defined( _WIN32 )
          if (FindNextFileA(FindFirstFile_h, &FindData_X))
          {
            Finish_B = false;
          }
          else
          {
            FindClose(FindFirstFile_h);
            /*
                        if (EmptyDir_B)
                        {
                          //printf("Empy %s\n", DotFileFound_X.Path.FullPathName(true).c_str());
                          if (!_rDirectoryParserCallback(DotFileFound_X))
                          {
                            Finish_B = true;
                            Rts_E = BOF_ERR_CANCEL;
                          }
                        }
            */
          }
#else
          pDirEntry_X = readdir(pDir_X);
          if (pDirEntry_X != nullptr)
          {
            Finish_B = false;
          }
          else
          {
            closedir(pDir_X);
          }
#endif
        }
      } while (!Finish_B);
    }
  }
  //printf("Leave %s\n", _rPath.FullPathName(true).c_str());
  return Rts_E;
}
BOFERR Bof_FindFile(const BofPath &_rPath, const std::string &_rPattern_S, const BOF_FILE_TYPE _FileTypeToFind_E, bool _Recursive_B, std::vector<BOF_FILE_FOUND> &_rFileCollection)
{
  _rFileCollection.clear();
  BOF_DIRECTORY_PARSER_CALLBACK Cb = [&](const BOF_FILE_FOUND &_rFileFound_X) -> bool { _rFileCollection.push_back(_rFileFound_X); return true; };
  BOFERR Rts_E = Bof_DirectoryParser(_rPath, _rPattern_S, _FileTypeToFind_E, _Recursive_B, Cb);
  return Rts_E;
}
BOFERR Bof_CleanupDirectory(bool _Recursive_B, const BofPath &_rPath, bool _RemoveRootDir_B)
{
  BOF_DIRECTORY_PARSER_CALLBACK Cb = [&](const BOF_FILE_FOUND &_rFileFound_X) -> bool {bool Rts_B = false; if (_rFileFound_X.FileType_E == BOF_FILE_TYPE::BOF_FILE_DIR) { if (Bof_RemoveDirectory(_rFileFound_X.Path) == BOF_ERR_NO_ERROR) Rts_B = true; } else { if (Bof_DeleteFile(_rFileFound_X.Path) == BOF_ERR_NO_ERROR) Rts_B = true; } return Rts_B; };
  //BOF_DIRECTORY_PARSER_CALLBACK Cb = [&](const BOF_FILE_FOUND &_rFileFound_X) -> bool {bool Rts_B = true; if (_rFileFound_X.FileType_E == BOF_FILE_TYPE::BOF_FILE_DIR) { Bof_RemoveDirectory(_rFileFound_X.Path); } else { if (Bof_DeleteFile(_rFileFound_X.Path) != BOF_ERR_NO_ERROR) Rts_B = false; } return Rts_B; };
  //  BOF_DIRECTORY_PARSER_CALLBACK Cb = [&](const BOF_FILE_FOUND &_rFileFound_X) -> bool {bool Rts_B = true; if (_rFileFound_X.FileType_E == BOF_FILE_TYPE::BOF_FILE_REG) { if (Bof_DeleteFile(_rFileFound_X.Path) != BOF_ERR_NO_ERROR) Rts_B = false; } return Rts_B; };
  BOFERR Rts_E = Bof_DirectoryParser(_rPath, "*", BOF_FILE_TYPE::BOF_FILE_ALL, _Recursive_B, Cb);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    if (_RemoveRootDir_B)
    {
      Rts_E = Bof_RemoveDirectory(_rPath);
    }
  }
  return Rts_E;
}

BOFERR Bof_RemoveDirectory(const BofPath &_rPath)
{
  BOFERR Rts_E = BOF_ERR_ENOENT;

  if ((_rPath.IsExist()) && (_rPath.IsDirectory()))
  {
    Rts_E = BOF_ERR_EACCES;
#if defined( _WIN32 )
    if (_rmdir(_rPath.FullPathName(true).c_str()) == 0)
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
#else
    if (rmdir(_rPath.FullPathName(false).c_str()) == 0)
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
#endif
  }
  // printf("del %s errno %d->%x\n", _rPath.FullPathName(false).c_str(),errno, Rts_E);
  return Rts_E;
}

bool Bof_IsFileHandleValid(intptr_t _Io)
{
  bool Rts_B = false;

  if (_Io != BOF_FS_INVALID_HANDLE)
  {
    Rts_B = true;
  }
  return Rts_B;
}

BOFERR Bof_CreateFile(const BOF_FILE_PERMISSION _Permission_E, const BofPath &_rPath, bool _Append_B, intptr_t &_rIo)
{
  BOFERR Rts_E = BOF_ERR_CREATE;
  int Io_i, Flag_i;
#if defined (_WIN32)
  Flag_i = (_Append_B) ? (_O_RDWR | _O_CREAT | _O_APPEND | _O_BINARY) : (_O_RDWR | _O_CREAT | _O_BINARY | _O_TRUNC);
//Io_i = _open(_rPath.FullPathName(false).c_str(), Flag_i, _S_IREAD | _S_IWRITE | _S_IEXEC);
  Io_i = _open(_rPath.FullPathName(false).c_str(), Flag_i, _S_IREAD);
#else
  Flag_i = (_Append_B) ? (O_RDWR | O_CREAT | O_APPEND) : (O_RDWR | O_CREAT | O_TRUNC);
  Io_i = open(_rPath.FullPathName(false).c_str(), Flag_i, _Permission_E);
#endif
  if (Io_i != BOF_FS_INVALID_HANDLE)
  {
    _rIo = static_cast<intptr_t> (Io_i);
    //Rts_E = BOF_ERR_NO_ERROR;
    Rts_E = Bof_SetFsPermission(_Permission_E, _rPath);
  }
  return (Rts_E);
}

BOFERR Bof_CreateTempFile(const BOF_FILE_PERMISSION _Permission_E, BofPath &_rPath, const std::string &_rExt_S, intptr_t &_rIo)
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;
  uint32_t i_U32;
  BofPath TmpPath;
  std::string File_S;
  intptr_t Io;

  _rIo = BOF_FS_INVALID_HANDLE;
  if (_rPath.IsDirectory())
  {
    for (i_U32 = 0; i_U32 < 8; i_U32++)
    {
      File_S = Bof_Random(true, 16, 'A', 'z');
      TmpPath = _rPath.FullPathName(false) + File_S + "." + _rExt_S;
      Rts_E = Bof_CreateFile(_Permission_E, TmpPath, false, Io);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        _rIo = Io;
        _rPath = TmpPath;
        break;
      }
    }
  }
  return Rts_E;
}

BOFERR Bof_OpenFile(const BofPath &_rPath, bool _ReadOnly_B, intptr_t &_rIo)
{
  BOFERR Rts_E = BOF_ERR_NOT_OPENED;
  int Io_i, Flag_i;

#if defined (_WIN32)
  Flag_i = _ReadOnly_B ? (_O_RDONLY | _O_BINARY) : (_O_RDWR | _O_BINARY);
#else
  Flag_i = _ReadOnly_B ? (O_RDONLY) : (O_RDWR);
#endif
  _rIo = static_cast<intptr_t> (-1);
  Io_i = open(_rPath.FullPathName(false).c_str(), Flag_i);
  if (Io_i != BOF_FS_INVALID_HANDLE)
  {
    _rIo = static_cast<intptr_t> (Io_i);
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return (Rts_E);
}

int64_t Bof_GetFileIoPosition(intptr_t _Io)
{
  return Bof_SetFileIoPosition(_Io, 0, BOF_SEEK_METHOD::BOF_SEEK_CURRENT);
}

int64_t Bof_SetFileIoPosition(intptr_t _Io, int64_t _Offset_S64, BOF_SEEK_METHOD _SeekMethod_E)
{
  int64_t Rts_S64 = -1;
  int Io_i = static_cast<int> (_Io);

  if (Io_i != BOF_FS_INVALID_HANDLE)
  {
#if defined (_WIN32)
    Rts_S64 = _lseeki64(Io_i, _Offset_S64, static_cast<int> (_SeekMethod_E));
#else
    Rts_S64 = lseek64(Io_i, _Offset_S64, static_cast<int> (_SeekMethod_E));
#endif
  }
  return Rts_S64;
}

int ReadLine(intptr_t _Io, uint32_t _Max_U32, char *_pBuffer_c)
{
  int i, Rts_i = -1, Len_i;
  char *pData_c;
  uint32_t Remain_U32;
  char c, pDataRead_c[0x200];
  int64_t Pos_S64;
  bool Break_B;

  if ((_Max_U32) && (_pBuffer_c))
  {
    Pos_S64 = Bof_SetFileIoPosition(_Io, 0, BOF_SEEK_METHOD::BOF_SEEK_CURRENT);
    Rts_i = 0;
    c = 0;
    pData_c = _pBuffer_c;
    Remain_U32 = _Max_U32;
    do
    {
      Len_i = static_cast<int>(read(static_cast<int>(_Io), pDataRead_c, sizeof(pDataRead_c)));
      if (Len_i > 0)
      {
        Break_B = false;
        for (i = 0; i < Len_i; i++)
        {
          c = pDataRead_c[i];
          *pData_c++ = c;
          if ((c == 0) || (c == '\n'))
          {
            i++;
            Break_B = true;
            break;
          }
        }
        Remain_U32 -= i;
        Rts_i += i;
        if (Break_B)
        {
          break;
        }
      }
      else
      {
        break;
      }
    } while (Remain_U32);
    if (Rts_i > 0)
    {
      Bof_SetFileIoPosition(_Io, Pos_S64 + Rts_i, BOF_SEEK_METHOD::BOF_SEEK_BEGIN);
      if (c != 0)
      {
        if (Remain_U32)
        {
          _pBuffer_c[Rts_i] = 0;
        }
        else
        {
          _pBuffer_c[_Max_U32 - 1] = 0;
        }
      }
    }
  }
  return (Rts_i);
}

BOFERR Bof_ReadLine(intptr_t _Io, std::string &_rLine_S)
{
  BOFERR Rts_E = BOF_ERR_READ;
  char pBuffer_c[0x10000];

  int Len_i = ReadLine(_Io, sizeof(pBuffer_c), pBuffer_c);
  if (Len_i > 0)
  {
    _rLine_S = pBuffer_c;
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

BOFERR Bof_ReadLine(const BofPath &_rPath, std::string &_rBuffer_S)
{
  BOFERR Rts_E = BOF_ERR_ENOENT;

  if (Bof_GetFileType(_rPath) == BOF_FILE_TYPE::BOF_FILE_REG)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    std::fstream In(_rPath.FullPathName(false), std::fstream::in);
    std::getline(In, _rBuffer_S, '\0');  //There should be no \0 in text files.
  }
#if 0
  BOFERR Rts_E = BOF_ERR_EOF;
  intptr_t Io;
  uint32_t Nb_U32;
  uint64_t Size_U64;

  Size_U64 = Bof_GetFileSize(_rPath);
  if (Size_U64 != static_cast<uint64_t> (-1))
  {
    Rts_E = Bof_OpenFile(_rPath, true, Io);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (_rBuffer_S.capacity() <= Size_U64)
      {
        _rBuffer_S.reserve(static_cast<uint32_t>(Size_U64));
      }
      //C++11 and later do require contiguous storage for std::string
      Nb_U32 = static_cast<uint32_t>(Size_U64);
      //			Rts_E = Bof_ReadFile(Io, Nb_U32, reinterpret_cast<void *>(_rBuffer_S.c_str()));
      Rts_E = Bof_ReadFile(Io, Nb_U32, &_rBuffer_S[0]);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        _rBuffer_S.size = Nb_U32;
      }
      Bof_CloseFile(Io);
    }
  }
#endif
  return (Rts_E);
}
BOFERR Bof_WriteLine(intptr_t _Io, const std::string &_rLine_S)
{
  uint32_t Nb_U32 = static_cast<uint32_t>(_rLine_S.size());

  return Bof_WriteFile(_Io, Nb_U32, reinterpret_cast<const uint8_t *>(_rLine_S.c_str()));
}
BOFERR Bof_WriteLine(const BOF_FILE_PERMISSION _Permission_E, const BofPath &_rPath, bool _Append_B, const std::string &_rBuffer_S)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  uint32_t Nb_U32;
  intptr_t Io;

  Rts_E = Bof_CreateFile(_Permission_E, _rPath, _Append_B, Io);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Nb_U32 = static_cast<uint32_t>(_rBuffer_S.size());
    Rts_E = Bof_WriteFile(Io, Nb_U32, reinterpret_cast<const uint8_t *>(_rBuffer_S.c_str()));
    Bof_CloseFile(Io);
  }
  return (Rts_E);
}


BOFERR Bof_ReadFile(intptr_t _Io, uint32_t &_rNb_U32, uint8_t *_pBuffer_U8)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  int Io_i = static_cast<int> (_Io);
  int NbRead_i;

  if ((Io_i != BOF_FS_INVALID_HANDLE) && (_pBuffer_U8))
  {
    NbRead_i = static_cast<int>(read(Io_i, _pBuffer_U8, static_cast<size_t>(_rNb_U32)));
    _rNb_U32 = static_cast<uint32_t>(NbRead_i);
    if (NbRead_i < 0)
    {
      Rts_E = BOF_ERR_READ;
      _rNb_U32 = 0;
    }
    else
    {
      Rts_E = (NbRead_i == 0) ? BOF_ERR_EOF : BOF_ERR_NO_ERROR;
    }
    //Rts_E = (NbToRead_U32 == _rNb_U32) ? BOF_ERR_NO_ERROR : BOF_ERR_READ;
  }
  return (Rts_E);
}

BOFERR Bof_WriteFile(intptr_t _Io, uint32_t &_rNb_U32, const uint8_t *_pBuffer_U8)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  int Io_i = static_cast<int> (_Io);
  uint32_t NbToWrite_U32;

  if ((Io_i != BOF_FS_INVALID_HANDLE) && (_pBuffer_U8))
  {
    NbToWrite_U32 = _rNb_U32;
    _rNb_U32 = static_cast<uint32_t>(write(Io_i, _pBuffer_U8, _rNb_U32));
    Rts_E = (NbToWrite_U32 == _rNb_U32) ? BOF_ERR_NO_ERROR : BOF_ERR_READ;
  }
  return (Rts_E);
}
BOFERR Bof_ReadFile(const BofPath &_rPath, BOF_BUFFER &_rBufferToDeleteAfterUsage_X)
{
  BOFERR Rts_E = BOF_ERR_DONT_EXIST;
  intptr_t Io;
  uint64_t FileSize_U64;
  uint32_t Nb_U32;

  FileSize_U64 = BOF::Bof_GetFileSize(_rPath);
  if (FileSize_U64 != static_cast<uint64_t>(-1))
  {
    Rts_E = BOF_ERR_ENOMEM;
    _rBufferToDeleteAfterUsage_X.Reset();
    _rBufferToDeleteAfterUsage_X.MustBeDeleted_B = true;
    _rBufferToDeleteAfterUsage_X.Size_U64 = FileSize_U64;
    _rBufferToDeleteAfterUsage_X.Capacity_U64 = FileSize_U64;
    _rBufferToDeleteAfterUsage_X.pData_U8 = new uint8_t[FileSize_U64];
    if (_rBufferToDeleteAfterUsage_X.pData_U8)
    {
      Rts_E = Bof_OpenFile(_rPath, true, Io);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Nb_U32 = static_cast<uint32_t>(_rBufferToDeleteAfterUsage_X.Capacity_U64);
        Rts_E = Bof_ReadFile(Io, Nb_U32, _rBufferToDeleteAfterUsage_X.pData_U8);
        Bof_CloseFile(Io);
      }
      if (Rts_E != BOF_ERR_NO_ERROR)
      {
        BOF_SAFE_DELETE(_rBufferToDeleteAfterUsage_X.pData_U8);
        _rBufferToDeleteAfterUsage_X.Reset();
      }
    }
  }
  return Rts_E;
}

BOFERR Bof_ReadFile(const BofPath &_rPath, std::string &_rRawData_S)
{
  BOFERR Rts_E = BOF_ERR_OPERATION_FAILED;
#pragma message("TODO: optimize this BOFERR Bof_ReadFile(const BofPath &_rPath, std::string &_rRawData_S)")
  // Open the file: Note that we have to use binary mode as we want to return a string
  // representing matching the bytes of the file on the file system.
#if defined(_WIN32)
  std::ifstream Io(_rPath.FullPathName(true), std::ios::in | std::ios::binary);
#else
  std::ifstream Io(_rPath.FullPathName(false), std::ios::in | std::ios::binary);
#endif
  if (Io.is_open())
  {
    //		std::string content{ std::istreambuf_iterator<char>(Io), std::istreambuf_iterator<char>() };
    _rRawData_S = std::string{ std::istreambuf_iterator<char>(Io), std::istreambuf_iterator<char>() };
    Io.close();
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}
BOFERR Bof_WriteFile(const BOF_FILE_PERMISSION _Permission_E, const BofPath &_rPath, const BOF_BUFFER &_rBuffer_X)
{
  BOFERR Rts_E;
  intptr_t Io;
  uint32_t Nb_U32;

  Rts_E = Bof_CreateFile(_Permission_E, _rPath, false, Io);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Nb_U32 = static_cast<uint32_t>(_rBuffer_X.Capacity_U64);
    Rts_E = Bof_WriteFile(Io, Nb_U32, _rBuffer_X.pData_U8);
    Bof_CloseFile(Io);
  }
  return Rts_E;
}

BOFERR Bof_WriteFile(const BOF_FILE_PERMISSION _Permission_E, const BofPath &_rPath, const std::string &_rRawData_S)
{
  BOFERR Rts_E;
  BOF_BUFFER Buffer_X(_rRawData_S.size(), _rRawData_S.size(), (uint8_t *)(_rRawData_S.c_str()), false);

  Rts_E = Bof_WriteFile(_Permission_E, _rPath, Buffer_X);
  return Rts_E;
}


BOFERR Bof_FlushFile(intptr_t _Io)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  int Io_i = static_cast<int> (_Io);

  if (Io_i != BOF_FS_INVALID_HANDLE)
  {
    //		Rts_E = BOF_ERR_FLUSH;
        // if (flush(Io_i)==0)
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
  }
  return (Rts_E);
}

BOFERR Bof_CloseFile(intptr_t &_rIo)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  int Io_i = static_cast<int> (_rIo);

  if (Io_i != BOF_FS_INVALID_HANDLE)
  {
    Rts_E = BOF_ERR_CLOSE;
    if (close(Io_i) == 0)
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
    _rIo = BOF_FS_INVALID_HANDLE;
  }
  return (Rts_E);
}

uint64_t Bof_GetFileSize(const BofPath &_rPath)
{
  uint64_t Rts_U64 = static_cast<uint64_t> (-1);

  std::ifstream Ifs(_rPath.FullPathName(false).c_str(), std::ifstream::ate | std::ifstream::binary);
  if (Ifs)
  {
    Rts_U64 = Ifs.tellg();
  }
  return Rts_U64;
}

BOFERR Bof_DeleteFile(const BofPath &_rPath)
{
  BOFERR Rts_E;

#if defined (_WIN32)
  if (_unlink(_rPath.FullPathName(false).c_str()) == 0)
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  else
  {
    Rts_E = (errno == EACCES) ? BOF_ERR_EACCES : BOF_ERR_NOT_FOUND; // 1: EPERM
  }
#else
  if (unlink(_rPath.FullPathName(false).c_str()) == 0)
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  else
  {
    Rts_E = (errno == EPERM) ? BOF_ERR_EACCES : BOF_ERR_NOT_FOUND;  // 1: EPERM
  }
#endif

  return Rts_E;
}


BOFERR Bof_RenameFile(const BofPath &_rOldPath, const BofPath &_rNewPath)
{
  BOFERR Rts_E;

#if defined (_WIN32)
  //char  pOldName_c[512], pNewName_c[512];
  //Bof_MultiByteToWideChar(_rOldPath.FullPathName(false).c_str(), sizeof(pOldName_wc) / sizeof(pOldName_wc[0]), pOldName_wc);
  //Bof_MultiByteToWideChar(_rNewPath.FullPathName(false).c_str(), sizeof(pNewName_wc) / sizeof(pNewName_wc[0]), pNewName_wc);
  //Rts_E = MoveFile(pOldName_c, pNewName_c) ? BOF_ERR_NO_ERROR : BOF_ERR_EACCES;
  Rts_E = MoveFileA(_rOldPath.FullPathName(false).c_str(), _rNewPath.FullPathName(false).c_str()) ? BOF_ERR_NO_ERROR : BOF_ERR_EACCES;
#else
  Rts_E = (rename(_rOldPath.FullPathName(false).c_str(), _rNewPath.FullPathName(false).c_str()) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_EACCES;
#endif

  return Rts_E;
}

bool Bof_IsFileExist(const BofPath &_rPath)
{
  return _rPath.IsExist();
  //ifstream infile(fileName);
  //return infile.good();
}

BOFERR Bof_CopyFile(bool _OverwriteIfExists_B, const BofPath &_rSrcPath, const BofPath &_rDstPath)
{
  BOFERR Rts_E = BOF_ERR_ENOENT;
  bool CreateOut_B;

  std::ifstream Ifs(_rSrcPath.FullPathName(false).c_str());
  // ios::trunc means that the output file will be overwritten if exists
  if (Ifs)
  {
    Rts_E = BOF_ERR_EXIST;
    CreateOut_B = _OverwriteIfExists_B ? true : Bof_IsFileExist(_rSrcPath);
    if (CreateOut_B)
    {
      Rts_E = BOF_ERR_CREATE;
      std::ofstream Ofs(_rDstPath.FullPathName(false).c_str(), std::ios::trunc);
      if (Ofs)
      {
        Ofs << Ifs.rdbuf();
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
  }
  return Rts_E;
}

/*
 * _ReOpenMode_B: if true, the file is closed and re-opened with trunc attribute (_Offset_S64 has no effect)
 * If false the handle on the file is conserved so external app continue to write/read in the file. In this case _Offset_S64
 * specifies how many bytes to leave in the file after trucation
 */
BOFERR Bof_ResetFileContent(const BofPath &_rPath, bool _ReOpenMode_B, int64_t _Offset_S64)
{
  BOFERR Rts_E = BOF_ERR_ENOENT;

  if (_ReOpenMode_B)
  {
    std::ofstream Ofs;
    Ofs.open(_rPath.FullPathName(false).c_str(), std::ofstream::out | std::ofstream::trunc);
    if (Ofs)
    {
      Rts_E = BOF_ERR_NO_ERROR;
      Ofs.close();
    }
  }
  else
  {
#if defined (_WIN32)
    Rts_E = BOF_ERR_NOT_SUPPORTED;
#if 0
    HANDLE Io_h = CreateFileA(_rPath.FullPathName(false).c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    bad handle if file is already opened
      //DWORD f = GetLastError(); 
      if (Io_h != BOF_INVALID_HANDLE_VALUE)
      {
        if (SetFilePointer(Io_h, static_cast<LONG>(_Offset_S64), NULL, FILE_BEGIN) == _Offset_S64)
        {
          if (SetEndOfFile(Io_h))
          {
            Rts_E = BOF_ERR_NO_ERROR;
          }
        }
        CloseHandle(Io_h);
      }
#endif
#else
    if (!truncate64(_rPath.FullPathName(false).c_str(), _Offset_S64))
    {
      Rts_E = BOF_ERR_NO_ERROR;
    }
#endif
  }
  return Rts_E;
}

END_BOF_NAMESPACE()