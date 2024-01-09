/*
 * Copyright (c) 2013-2033, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bofpath module of the bofstd library
 *
 * Name:        bofpath.cpp
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
#include "bofstd/boffs.h"
#include "bofstd/bofstring.h"
#include <bofstd/bofpath.h>
#include <bofstd/bofsystem.h>

#include <cctype>

#if defined(_WIN32)
#include <windows.h>
#endif
BEGIN_BOF_NAMESPACE()

BofPath::BofPath(bool _PureFilename_B)
{
  InitPathField(nullptr, _PureFilename_B, "");
}

BofPath::BofPath(const char *_pPath_c, const char *_pForbiddenChar_c, bool _PureFilename_B)
{
  std::string Path_S;

  if (_pPath_c)
  {
    Path_S = _pPath_c;
    InitPathField(_pForbiddenChar_c, _PureFilename_B,  Path_S);
  }
  else
  {
    mValid_B = false;
    // InitPathField("");
  }
}

BofPath::BofPath(const std::string &_rPath_S, bool _PureFilename_B)
{
  InitPathField(nullptr, _PureFilename_B, _rPath_S);
}

BofPath::BofPath(const std::string &_rDirectory_S, const std::string &_rFile_S)
{
  InitPathField(nullptr, false, _rDirectory_S + _rFile_S);
}

BofPath::BofPath(const BofPath &_rOther_O)
{
  InitPathField(_rOther_O.ForbiddenChar().c_str(), (_rOther_O.DirectoryName(false, false) == ""), _rOther_O.ToString());
}

BofPath::BofPath(BofPath &&_rrOther_O)
{
  InitPathField(_rrOther_O.ForbiddenChar().c_str(), (_rrOther_O.DirectoryName(false, false) == ""), _rrOther_O.ToString());
  _rrOther_O.InitPathField(nullptr, false, "");
}

BofPath &BofPath::operator=(const BofPath &_rOther_O)
{
  InitPathField(_rOther_O.ForbiddenChar().c_str(), (_rOther_O.DirectoryName(false, false) == ""), _rOther_O.ToString());
  return *this;
}

BofPath &BofPath::operator=(BofPath &&_rrOther_O)
{
  if (this != &_rrOther_O)
  {
    // Release the current objects resources
    InitPathField(_rrOther_O.ForbiddenChar().c_str(), (_rrOther_O.DirectoryName(false, false) == ""), _rrOther_O.ToString());
    // Reset other
    _rrOther_O.InitPathField(nullptr, false, "");
  }
  return *this;
}

BofPath &BofPath::operator=(const char *_pNewPath_c)
{
  std::string Path_S;
  std::string::size_type DelimiterPos;

  if (_pNewPath_c)
  {
    Path_S = _pNewPath_c;
    InitPathField(nullptr, !IsDirectoryDelimiterPresent(Path_S), Path_S);
  }
  else
  {
    mValid_B = false;
  }
  return *this;
}

BofPath &BofPath::operator=(const std::string &_rNewPath_S)
{
  InitPathField(nullptr, !IsDirectoryDelimiterPresent(_rNewPath_S), _rNewPath_S);
  return *this;
}

BofPath::~BofPath()
{
}

bool BofPath::operator==(const BofPath &_rOther_O) const
{
  return (mDiskName_S == _rOther_O.mDiskName_S) && (mDirectoryName_S == _rOther_O.mDirectoryName_S) && (mFileNameWithExtension_S == _rOther_O.mFileNameWithExtension_S);
}

bool BofPath::operator!=(const BofPath &_rOther_O) const
{
  return !(*this == _rOther_O);
}

std::string BofPath::DirectoryName(bool _WithDiskName_B, bool _Windows_B) const
{
  std::string Rts_S;

  Rts_S = _WithDiskName_B ? mDiskName_S + mDirectoryName_S : mDirectoryName_S;
  if (_Windows_B)
  {
    Rts_S = Bof_StringReplace(Rts_S, "/", '\\');
  }
  return Rts_S;
}
uint32_t BofPath::NumberOfSubDirectory() const
{
  return static_cast<uint32_t>(mSubdirCollection.size());
}

std::string BofPath::SubDirectory(uint32_t _Level_U32, bool _Windows_B) const
{
  std::string Rts_S;
  if (_Level_U32 < mSubdirCollection.size())
  {
    Rts_S = mSubdirCollection[_Level_U32];
    if (_Windows_B)
    {
      Rts_S = Bof_StringReplace(Rts_S, "/", '\\');
    }
  }
  return Rts_S;
}

const std::string &BofPath::FileNameWithoutExtension() const
{
  return mFileNameWithoutExtension_S;
}

BOFERR BofPath::FileNameWithoutExtension(const std::string &_rFileNameWithoutExtension_S)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  std::string::size_type ExtensionPos;

  ExtensionPos = _rFileNameWithoutExtension_S.find('.');
  if (ExtensionPos == std::string::npos)
  {
    if (!IsDirectoryDelimiterPresent(_rFileNameWithoutExtension_S))
    {
      Rts_E = BOF_ERR_NO_ERROR;
      mFileNameWithoutExtension_S = _rFileNameWithoutExtension_S;
    }
  }
  return Rts_E;
}

BOFERR BofPath::FileNameWithExtension(const std::string &_rFileNameWithExtension_S)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  std::string::size_type ExtensionPos;

  if (!IsDirectoryDelimiterPresent(_rFileNameWithExtension_S))
  {
    ExtensionPos = _rFileNameWithExtension_S.rfind('.');
    mFileNameWithExtension_S = _rFileNameWithExtension_S;
    if (ExtensionPos == std::string::npos)
    {
      mExtension_S = "";
      mFileNameWithoutExtension_S = mFileNameWithExtension_S;
    }
    else
    {
      mExtension_S = _rFileNameWithExtension_S.substr(ExtensionPos + 1);
      mFileNameWithoutExtension_S = mFileNameWithExtension_S.substr(0, ExtensionPos);
    }
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

const std::string &BofPath::Extension() const
{
  return mExtension_S;
}

BOFERR BofPath::Extension(const std::string &_rExtension_S)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  std::string::size_type ExtensionPos;

  ExtensionPos = _rExtension_S.find('.');
  if (ExtensionPos == std::string::npos)
  {
    if (!IsDirectoryDelimiterPresent(_rExtension_S))
    {
      Rts_E = BOF_ERR_NO_ERROR;
      mFileNameWithoutExtension_S = _rExtension_S;
    }
  }
  return Rts_E;
}

std::string BofPath::FullPathName(bool _Windows_B) const
{
  std::string Rts_S;

  Rts_S = DirectoryName(true, _Windows_B);
  if (IsFile())
  {
    Rts_S += mFileNameWithoutExtension_S;
    if (!mExtension_S.empty())
    {
      Rts_S += ('.' + mExtension_S);
    }
  }
  return Rts_S;
}
std::string BofPath::FullPathNameWithoutExtension(bool _Windows_B) const
{
  std::string Rts_S;

  Rts_S = DirectoryName(true, _Windows_B);
  if (IsFile())
  {
    Rts_S += mFileNameWithoutExtension_S;
  }
  return Rts_S;
}
std::string BofPath::ToString(bool _Windows_B) const
{
  return FullPathName(_Windows_B);
}
std::wstring BofPath::FullWidePathName(bool _Windows_B) const
{
  std::wstring Rts_WS;
  std::string Fn_S = FullPathName(_Windows_B);
  Rts_WS = Bof_Utf8ToUtf16(Fn_S);

  return Rts_WS;
}
BofPath BofPath::operator+(const std::string &_rRelativePath_S) const
{
  BofPath Rts_O = *this; // Make a copy of myself.  Same as BofPath Rts_O(*this);
  Rts_O.Combine(_rRelativePath_S);
  return Rts_O;
}

BOFERR BofPath::Combine(const std::string &_rRelativePath_S)
{
  BOFERR Rts_E; // = BOF_ERR_BAD_TYPE;

  // no as we want to be able to Combine file with '\' to get a directory if (IsDirectory())
  {
    Rts_E = InitPathField(nullptr, false, ToString() + _rRelativePath_S);
  }
  return Rts_E;
}

const std::string &BofPath::FileNameWithExtension() const
{
  return mFileNameWithExtension_S;
}

bool BofPath::IsDirectory() const
{
  return ((!mDirectoryName_S.empty()) && (mFileNameWithExtension_S.empty())); // "" is a file !
}

bool BofPath::IsFile() const
{
  return (!IsDirectory());
}

bool BofPath::IsExist() const
{
  bool Rts_B = false;
  BOF_FILE_TYPE FileType_E;

  FileType_E = Bof_GetFileType(*this);
  Rts_B = IsDirectory() ? (FileType_E == BOF_FILE_TYPE::BOF_FILE_DIR) : ((FileType_E != BOF_FILE_TYPE::BOF_FILE_DONT_EXIST) && (FileType_E != BOF_FILE_TYPE::BOF_FILE_DIR));

  return (Rts_B);
}

bool BofPath::IsValid() const
{
  return (mValid_B);
}

bool BofPath::IsEmpty() const
{
  return (ToString() == "");
}

std::string BofPath::CurrentDirectoryName() const
{
  return mCurrentDirectoryName_S;
}
std::string BofPath::ForbiddenChar() const
{
  return mForbiddenCharacter_S;
}

BOFERR BofPath::CurrentDirectoryName(const std::string &_rNewCurrentDirectoryName_S)
{
  BOFERR Rts_E;
  std::string ThePath_S, DiskName_S;
  char LastChar_c;

  Rts_E = Normalize(false, _rNewCurrentDirectoryName_S, ThePath_S, DiskName_S);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Rts_E = BOF_ERR_FORMAT;
    if (!ThePath_S.empty())
    {
      LastChar_c = *ThePath_S.rbegin();
      // Path is not empty, check if it is a pure directory
      if (LastChar_c == '/')
      {
        mCurrentDirectoryName_S = ThePath_S;
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
  }
  return Rts_E;
}

BOFERR BofPath::InitPathField(const char *_pForbiddenChar_c, bool _PureFilename_B,const std::string &_rPath_S)
{
  BOFERR Rts_E;
  char LastChar_c;
  std::string ThePath_S;
  std::string::size_type ExtensionPos, FilenamePos;

  mForbiddenCharacter_S = _pForbiddenChar_c ? _pForbiddenChar_c:"<>:\"\\|?*\a\f\n\r\t\v";
  mFileNameWithExtension_S = "";
  mFileNameWithoutExtension_S = "";
  mExtension_S = "";
  mCurrentDirectoryName_S = "";
  mDirectoryName_S = "";
  mDiskName_S = "";
  mSubdirCollection.clear();
  Rts_E = Normalize(_PureFilename_B, _rPath_S, ThePath_S, mDiskName_S);
  if (Rts_E == BOF_ERR_EMPTY) // for "" filename
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    mValid_B = true;

    if (!ThePath_S.empty())
    {
      LastChar_c = *ThePath_S.rbegin();
      // Path is not empty, check if it is a pure directory
      if (LastChar_c == '/')
      {
        mDirectoryName_S = ThePath_S;
      }
      else
      {
        // Path is not a pure directory, extract file and extension value
        FilenamePos = ThePath_S.rfind('/');
        if (FilenamePos == std::string::npos)
        {
          if (!_PureFilename_B)
          {
            Rts_E = BOF_ERR_INTERNAL;
            mFileNameWithExtension_S = ThePath_S;
            // pure filename: not possible the normalize step should have returned an error
            BOF_ASSERT(0);
          }
          else
          {
            FilenamePos = 0;
            mDirectoryName_S = "";
            mFileNameWithExtension_S = ThePath_S;
          }
        }
        else
        {
          // Directory+filename
          mDirectoryName_S = ThePath_S.substr(0, FilenamePos + 1);
          mFileNameWithExtension_S = ThePath_S.substr(FilenamePos + 1);
          if (*mFileNameWithExtension_S.rbegin() == '.') // File.
          {
            mFileNameWithExtension_S.pop_back();
          }
        }
        // Process extension
        ExtensionPos = ThePath_S.rfind('.');
        if ((ExtensionPos == std::string::npos) || (ExtensionPos <= FilenamePos)) // . in subdir
        {
          mFileNameWithoutExtension_S = mFileNameWithExtension_S;
        }
        else
        {
          mExtension_S = ThePath_S.substr(ExtensionPos + 1);
          if (!_PureFilename_B)
          {
            mFileNameWithoutExtension_S = ThePath_S.substr(FilenamePos + 1, ExtensionPos - FilenamePos - 1);
          }
          else
          {
            mFileNameWithoutExtension_S = ThePath_S.substr(0, ExtensionPos);
          }
        }
      }

      mSubdirCollection = Bof_StringSplit(ThePath_S, "/");
      if (mSubdirCollection.size())
      {
        mSubdirCollection.erase(mSubdirCollection.begin()); // split first entry is ""
        if (mSubdirCollection.size())
        {
          mSubdirCollection.erase(mSubdirCollection.end() - 1); // Remove dir (/) or filename
        }
      }
      for (auto &rIt : mSubdirCollection)
      {
        rIt = '/' + rIt + '/';
      }
    }
  }
  else
  {
    mValid_B = false;
  }
  return (Rts_E);
}

bool BofPath::IsWindowsDiskPath(const std::string &_rPath_S)
{
  return ((std::isalpha(_rPath_S[0])) && (_rPath_S[1] == ':') && ((_rPath_S[2] == '/') || (_rPath_S[2] == '\\')));
}

bool BofPath::IsForbiddenChar(const std::string &_rPath_S)
{
  bool Rts_B;

#if defined(__EMSCRIPTEN__)
//During file packaging under emscripten windows we can have path like /C:/pro/...
  if ((_rPath_S[0] == '/') && (_rPath_S[2] == ':') && (_rPath_S[3] == '/'))
  {
    Rts_B = Bof_StringIsPresent(_rPath_S.substr(3), mForbiddenCharacter_S);
  }
  else
  {
    Rts_B = Bof_StringIsPresent(_rPath_S, mForbiddenCharacter_S);
  }
#else
  Rts_B=Bof_StringIsPresent(_rPath_S, mForbiddenCharacter_S);
#endif
  return Rts_B;
}

BOFERR BofPath::Normalize(bool _PureFilename_B, const std::string &_rRawPath_S, std::string &_rNormalizedPath_S, std::string &_rDiskName_S)
{
  BOFERR Rts_E;
  std::string Pwd_S;
  std::string::size_type SlashDelimiterPos, SlashPrevDelimiterPos;
  std::vector<std::string> ListOfDir_S;
  char Prev_c;

  _rDiskName_S = "";
  // Remove bad char on the left and rigth side
  // Should be error
  if (Bof_StringIsPresent(_rRawPath_S, "\a\f\n\r\t\v"))
  {
    Rts_E = BOF_ERR_EINVAL;
  }
  else
  {
    _rNormalizedPath_S = Bof_StringTrim(_rRawPath_S);
    if (_rNormalizedPath_S.empty())
    {
      _rNormalizedPath_S = "";
      Rts_E = BOF_ERR_EMPTY;
    }
    else
    {
      Rts_E = BOF_ERR_NO_ERROR;

      // Replace \ by /
      _rNormalizedPath_S = Bof_StringReplace(_rNormalizedPath_S, "\\", '/');
      SlashDelimiterPos = _rNormalizedPath_S.find('/');
      Rts_E = Bof_GetCurrentDirectory(Pwd_S);
      Pwd_S = Bof_StringReplace(Pwd_S, "\\", '/');
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        if (SlashDelimiterPos == std::string::npos)
        {
          if (!_PureFilename_B)
          {
            _rNormalizedPath_S = Pwd_S + _rNormalizedPath_S;
            //_rNormalizedPath_S = Bof_StringReplace(_rNormalizedPath_S, "\\", '/');
          }
        }
        else
        {
          if ((!_rNormalizedPath_S.empty()) && (_rNormalizedPath_S[0] == '.'))
          {
            while ((Rts_E == BOF_ERR_NO_ERROR) && (!_rNormalizedPath_S.empty()) && (_rNormalizedPath_S[0] == '.'))
            {
              if (_rNormalizedPath_S[1] == '/')
              {
                _rNormalizedPath_S = _rNormalizedPath_S.substr(2);
              }
              else if (_rNormalizedPath_S[1] == '.')
              {
                Rts_E = BOF_ERR_TOO_SMALL;
                if (Pwd_S.size() > 2)
                {
                  SlashPrevDelimiterPos = Pwd_S.rfind('/', Pwd_S.size() - 2);
                  if (SlashPrevDelimiterPos == std::string::npos)
                  {
                    Rts_E = BOF_ERR_TOO_BIG;
                  }
                  else
                  {
                    Pwd_S = Pwd_S.substr(0, SlashPrevDelimiterPos + 1);
                    _rNormalizedPath_S = _rNormalizedPath_S.substr(3);
                    Rts_E = BOF_ERR_NO_ERROR;
                  }
                }
              }
            }
            if (Rts_E == BOF_ERR_NO_ERROR)
            {
              _rNormalizedPath_S = Pwd_S + _rNormalizedPath_S;
            }
          }
        }
      }
    }
  }
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    // Extract disk name for windows path
    if (IsWindowsDiskPath(_rNormalizedPath_S))
    {
      _rDiskName_S = _rNormalizedPath_S.substr(0, 2);
      _rNormalizedPath_S = _rNormalizedPath_S.substr(2);
    }
    if (_rNormalizedPath_S[0] != '/') // data/dir/file without ./ or ../
    {
      if (!_PureFilename_B)
      {
        _rNormalizedPath_S = Pwd_S + _rNormalizedPath_S;
        if (IsWindowsDiskPath(_rNormalizedPath_S))
        {
          _rDiskName_S = _rNormalizedPath_S.substr(0, 2);
          _rNormalizedPath_S = _rNormalizedPath_S.substr(2);
        }
      }
    }
    // Reject relative path
    if (!_PureFilename_B)
    {
      if (_rNormalizedPath_S[0] != '/')
      {
        if (mCurrentDirectoryName_S != "")
        {
          _rNormalizedPath_S = mCurrentDirectoryName_S + _rNormalizedPath_S;
          if (IsWindowsDiskPath(_rNormalizedPath_S))
          {
            _rDiskName_S = _rNormalizedPath_S.substr(0, 2);
            _rNormalizedPath_S = _rNormalizedPath_S.substr(2);
          }
        }
        else
        {
          Rts_E = BOF_ERR_FORMAT;
        }
      }
    }
    //		else
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      // Check for forbidden char in path
      if (IsForbiddenChar(_rNormalizedPath_S))
      {
        Rts_E = BOF_ERR_EINVAL;
      }
      else
      {
        if (!_PureFilename_B)
        {
          // Reduce path (. and .. processing)
          if (_rNormalizedPath_S == "/")
          {
          }
          else
          {
            if (Rts_E == BOF_ERR_NO_ERROR)
            {
              Prev_c = ~_rNormalizedPath_S[0];
              for (auto It = _rNormalizedPath_S.begin(); It != _rNormalizedPath_S.end(); ++It)
              {
                // if (Prev_c == *It)
                if ((Prev_c == '/') && (*It == '/'))
                {
                  _rNormalizedPath_S.erase(It);
                  It--;
                }
                else
                {
                  Prev_c = *It;
                }
              }
            }
            if (Rts_E == BOF_ERR_NO_ERROR)
            {
              ListOfDir_S = Bof_StringSplit(_rNormalizedPath_S, "/");
              if (ListOfDir_S.size())
              {
                // Remove .
                ListOfDir_S.erase(std::remove_if(ListOfDir_S.begin(), ListOfDir_S.end(), [](const std::string &_rStr_S) { return _rStr_S == "."; }), ListOfDir_S.end());

                if (ListOfDir_S.size() > 1)
                {
                  // Process ..
                  std::vector<std::string>::iterator itdot = ListOfDir_S.begin();
                  do
                  {
                    itdot = std::adjacent_find(itdot, ListOfDir_S.end(), [](const std::string &_rStr1_S, const std::string &_rStr2_S) { return ((_rStr1_S != "..") && (_rStr2_S == "..")); });
                    if (itdot != ListOfDir_S.end())
                    {
                      itdot = ListOfDir_S.erase(itdot, itdot + 2);
                      if (itdot == ListOfDir_S.begin())
                      {
                        /// tmp/../..
                        Rts_E = BOF_ERR_UNDERRUN;
                        break;
                      }
                      else
                      {
                        --itdot;
                      }
                    }
                  } while (itdot != ListOfDir_S.end());
                }
              }
            }
            if (Rts_E == BOF_ERR_NO_ERROR)
            {
              // Special case such as C:/.. or V:/tmp/......
              for (std::string &rStr_S : ListOfDir_S)
              {
                if (Bof_StringIsAllTheSameChar(rStr_S, '.'))
                {
                  Rts_E = BOF_ERR_INVALID_STATE;
                  break;
                }
              }
            }
            if (Rts_E == BOF_ERR_NO_ERROR)
            {
              _rNormalizedPath_S = "/" + Bof_StringJoin(ListOfDir_S, "/");
              BOF_ASSERT(_rNormalizedPath_S[0] == '/');
              if (_rNormalizedPath_S[1] == '/')
              {
                _rNormalizedPath_S = _rNormalizedPath_S.erase(0, 1);
              }
              std::string::const_iterator itslash = std::adjacent_find(_rNormalizedPath_S.begin(), _rNormalizedPath_S.end(), [](const char _Char1_c, const char _Char2_c) { return ((_Char1_c == '/') && (_Char2_c == '/')); });
              if (itslash != _rNormalizedPath_S.end())
              {
                Rts_E = BOF_ERR_INVALID_STATE;
              }
            }
          }
        }
      }
    }
  }
  return Rts_E;
}

bool BofPath::IsDirectoryDelimiterPresent(const std::string &_rPath_S) const
{
  bool Rts_B = false;
  std::string::size_type DelimiterPos;

  DelimiterPos = _rPath_S.find("/");
  if (DelimiterPos != std::string::npos)
  {
    Rts_B = true;
  }
  else
  {
    DelimiterPos = _rPath_S.find("\\");
    if (DelimiterPos != std::string::npos)
    {
      Rts_B = true;
    }
  }
  return Rts_B;
}
END_BOF_NAMESPACE()